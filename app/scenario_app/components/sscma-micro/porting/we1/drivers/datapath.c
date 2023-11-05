#include "datapath.h"

#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "arc_timer.h"
#include "board_config.h"
#include "hx_drv_edm.h"
#include "hx_drv_inp1bitparser.h"
#include "powermode.h"
#include "sensor_dp_lib.h"

static DataPath_state datapath_state = {
  0,
};

static DataPath_struct datapath_struct = {
  .wdma1_startaddr             = NULL,
  .wdma2_startaddr             = NULL,
  .wdma3_startaddr             = NULL,
  .jpegsize_autofill_startaddr = NULL,

  .cyclic_buffer_cnt = 1,
  .hw5x5_cfg =
    {
                .hw5x5_path         = HW5x5_PATH_THROUGH_DEMOSAIC,
                .demos_pattern_mode = DEMOS_PATTENMODE_BGGR,
                .demos_color_mode   = DEMOS_COLORMODE_YUV422,
                .demos_bndmode      = DEMOS_BNDODE_EXTEND0,
                .demoslpf_roundmode = DEMOSLPF_ROUNDMODE_FLOOR,
                .hw55_crop_stx      = 0,
                .hw55_crop_sty      = 0,
                .hw55_in_width      = SENSOR_WIDTH_DEFAULT,
                .hw55_in_height     = SENSOR_HEIGHT_DEFAULT,
                .fir_lbp_th         = 3,
                .fir_procmode       = FIR_PROCMODE_LBP1,
                .firlpf_bndmode     = FIRLPF_BNDODE_REPEAT,
                },
  .jpeg_cfg =
    {
                .jpeg_path      = JPEG_PATH_ENCODER_EN,
                .jpeg_enctype   = JPEG_ENC_TYPE_YUV422,
                .jpeg_encqtable = JPEG_ENC_QTABLE_4X,
                .enc_width      = SENSOR_WIDTH_DEFAULT,
                .enc_height     = SENSOR_HEIGHT_DEFAULT,
                .dec_width      = SENSOR_WIDTH_DEFAULT,
                .dec_height     = SENSOR_HEIGHT_DEFAULT,
                .dec_roi_stx    = 0,
                .dec_roi_sty    = 0,
                },
};

static void datapath_hw5x5jpeg_recapture() { sensordplib_retrigger_capture(); }

el_err_code_t datapath_stop_work(void) {
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();
    return EL_OK;
}

void datapath_cb(SENSORDPLIB_STATUS_E event) {
    EL_LOGD("event=%d\n", event);

    switch (event) {
    case SENSORDPLIB_STATUS_XDMA_FRAME_READY:
        datapath_state.g_frame_ready = true;
        datapath_state.g_frame_count++;
        break;
    default:
        EL_LOGD("Unkonw event\n");
        break;
    }
}

el_err_code_t datapath_init(uint16_t width, uint16_t height) {
    sensordplib_stop_capture();
    sensordplib_start_swreset();
    sensordplib_stop_swreset_WoSensorCtrl();

    datapath_state.g_frame_ready = false;
    datapath_state.g_frame_count = 0;

    datapath_struct.wdma1_startaddr             = (uint32_t)el_malloc(width * height / 4);  // jpeg
    datapath_struct.wdma2_startaddr             = datapath_struct.wdma1_startaddr;          // jpeg
    datapath_struct.wdma3_startaddr             = (uint32_t)el_malloc(width * height * 3);  // hw5x5
    datapath_struct.jpegsize_autofill_startaddr = (uint32_t)el_malloc(100);                 // jpeg size autofill
    EL_LOGD("wdma1_startaddr=0x%x,wdma2_startaddr=0x%x,wdma3_startaddr=0x%x,jpegsize_autofill_startaddr=0x%x\n",
            datapath_struct.wdma1_startaddr,
            datapath_struct.wdma2_startaddr,
            datapath_struct.wdma3_startaddr,
            datapath_struct.jpegsize_autofill_startaddr);
    sensordplib_set_xDMA_baseaddrbyapp(
      datapath_struct.wdma1_startaddr, datapath_struct.wdma2_startaddr, datapath_struct.wdma3_startaddr);
    sensordplib_set_jpegfilesize_addrbyapp(datapath_struct.jpegsize_autofill_startaddr);

#ifdef EXTERNAL_LDO
    hx_drv_pmu_set_ctrl(PMU_PWR_PLAN, PMU_WE1_POWERPLAN_EXTERNAL_LDO);
    hx_lib_pm_cldo_en(0);
#else
    hx_drv_pmu_set_ctrl(PMU_PWR_PLAN, PMU_WE1_POWERPLAN_INTERNAL_LDO);
#endif

    datapath_struct.hw5x5_cfg.hw55_in_width  = width;
    datapath_struct.hw5x5_cfg.hw55_in_height = height;
    datapath_struct.jpeg_cfg.enc_width       = width;
    datapath_struct.jpeg_cfg.enc_height      = height;
    datapath_struct.jpeg_cfg.dec_width       = width;
    datapath_struct.jpeg_cfg.dec_height      = height;

    return EL_OK;
}

el_err_code_t datapath_start_work(void) {
    memset(&datapath_state, 0x00, sizeof(datapath_state));
    datapath_state.g_frame_ready = false;
    datapath_state.g_frame_count = 0;
    sensordplib_set_int_hw5x5_jpeg_wdma23(
      datapath_struct.hw5x5_cfg, datapath_struct.jpeg_cfg, datapath_struct.cyclic_buffer_cnt, NULL);

    hx_dplib_register_cb(datapath_cb, SENSORDPLIB_CB_FUNTYPE_DP);

    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
    sensordplib_set_sensorctrl_start();

    return EL_OK;
}

el_err_code_t datapath_capture(uint32_t timeout) {
    uint32_t time                = el_get_time_ms();
    datapath_state.g_frame_ready = false;
    datapath_hw5x5jpeg_recapture();
    while (!datapath_state.g_frame_ready) {
        if (el_get_time_ms() - time > timeout) {
            return EL_ETIMOUT;
        }
        el_sleep(5);
    }
    return EL_OK;
}

int datapath_restart() {
    int ret = 0;
    /*
    we set the img_ready to false to enable a new capture
    */
    datapath_state.g_frame_ready = false;
    datapath_state.g_frame_count = 0;

    sensordplib_set_int_hw5x5_jpeg_wdma23(
      datapath_struct.hw5x5_cfg, datapath_struct.jpeg_cfg, datapath_struct.cyclic_buffer_cnt, NULL);
    ret = sensordplib_set_sensorctrl_start();

    return ret;
}

el_img_t datapath_get_frame() {
    el_img_t img;
    img.data   = (uint8_t*)datapath_struct.wdma3_startaddr;
    img.width  = datapath_struct.hw5x5_cfg.hw55_in_width;
    img.height = datapath_struct.hw5x5_cfg.hw55_in_height;
    img.format = EL_PIXEL_FORMAT_YUV422;
    img.size   = img.width * img.height * 3 / 2;
    img.rotate = EL_PIXEL_ROTATE_0;

    el_printf("frame data addr: 0x%x, size: %d\n", img.data, img.size);

    return img;
}
el_img_t datapath_get_jpeg() {
    el_img_t jpeg;
    uint8_t  frame_no, buffer_no = 0;
    uint32_t reg_val = 0, mem_val = 0;
    hx_drv_xdma_get_WDMA2_bufferNo(&buffer_no);
    hx_drv_xdma_get_WDMA2NextFrameIdx(&frame_no);
    if (frame_no == 0) {
        frame_no = buffer_no - 1;
    } else {
        frame_no = frame_no - 1;
    }
    jpeg.width  = datapath_struct.jpeg_cfg.enc_width;
    jpeg.height = datapath_struct.jpeg_cfg.enc_height;
    jpeg.format = EL_PIXEL_FORMAT_JPEG;
    jpeg.rotate = EL_PIXEL_ROTATE_0;
    hx_drv_jpeg_get_EncOutRealMEMSize(&reg_val);
    hx_drv_jpeg_get_FillFileSizeToMem(frame_no, (uint32_t)datapath_struct.jpegsize_autofill_startaddr, &mem_val);
    hx_drv_jpeg_get_MemAddrByFrameNo(frame_no, datapath_struct.wdma2_startaddr, &jpeg.data);
    jpeg.size = mem_val == reg_val ? mem_val : reg_val;

    el_printf("jpeg data addr: 0x%x, size: %d\n", jpeg.data, jpeg.size);

    return jpeg;
}

el_err_code_t datapath_set_roi_start_position(uint16_t start_x, uint16_t start_y) {
    datapath_struct.hw5x5_cfg.hw55_crop_stx = start_x;
    datapath_struct.hw5x5_cfg.hw55_crop_sty = start_y;

    return EL_OK;
}
