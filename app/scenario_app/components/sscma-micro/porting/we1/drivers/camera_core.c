#include "camera_core.h"

#include "hx_drv_dp.h"
#include "hx_drv_inp.h"
#include "hx_drv_sensorctrl.h"
#include "sensor_dp_lib.h"

extern Camera_Hal_Struct ov2640_driver;
extern Camera_Hal_Struct hm0360_driver;

#if defined(OV2640_CAMERA)
Camera_Hal_Struct* camera_hal = &ov2640_driver;
#elif defined(HM0360_CAMERA)
Camera_Hal_Struct* camera_hal = &hm0360_driver;
#else
Camera_Hal_Struct* camera_hal = NULL;
#endif

el_err_code_t camera_init(uint32_t width, uint32_t height) {
    uint32_t             sensor_id   = 0x00;
    uint32_t             result      = DP_NO_ERROR;
    SENSORDPLIB_STREAM_E stream_type = SENSORDPLIB_STREAM_NONEAOS;

    // parameter check
    if (camera_hal == NULL) {
        return EL_EINVAL;
    }
    if (camera_hal->power_init == NULL) {
        return EL_EIO;
    }
    if (camera_hal->get_sensor_id == NULL) {
        return EL_EIO;
    }
    if (camera_hal->sensor_cfg == NULL) {
        return EL_EIO;
    }
    if (camera_hal->set_output_size == NULL) {
        return EL_EIO;
    }

    // init the MCLK for camera
    result = hx_drv_dp_set_dp_clk_src(DP_CLK_SRC_XTAL_24M_POST);
    if (result) {
        return EL_EIO;
    }
    result = hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
    if (result) {
        return EL_EIO;
    }

    result = hx_drv_cis_init(camera_hal->xshutdown_pin, SENSORCTRL_MCLK_DIV1);
    if (result) {
        return EL_EIO;
    }

    // power init
    camera_hal->power_init();

    // get camera I2C ID
    if (0 != camera_hal->get_sensor_id(&sensor_id)) {
        return EL_EIO;
    }

    EL_LOGD("Camera ID: 0x%x", sensor_id);

    // config the camera regs
    result = hx_drv_cis_setRegTable(camera_hal->sensor_cfg, camera_hal->sensor_cfg_len);
    if (result) {
        return EL_EIO;
    }

    // config the output size of camera
    result = camera_hal->set_output_size(width, height);
    if (result) {
        return EL_EINVAL;
    }

    // INP and sensor control config
    if (sensor_id == 0x360)
        stream_type = SENSORDPLIB_STREAM_HM0360_CONT_MCLK;
    else
        stream_type = SENSORDPLIB_STREAM_NONEAOS;
    result = sensordplib_set_sensorctrl_inp(
      SENSORDPLIB_SENSOR_HM0360_MODE1, stream_type, width, height, INP_SUBSAMPLE_DISABLE);
    if (result) {
        return EL_EIO;
    }

    result = hx_drv_sensorctrl_set_MCLKCtrl(SENSORCTRL_MCLKCTRL_NONAOS);
    if (result) {
        return EL_EIO;
    }

    // config the xsleep pin
    if (camera_hal->xsleep_ctl == SENSORCTRL_XSLEEP_BY_SC) {
        result = hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_SC);
        if (result) {
            return EL_EIO;
        }
    } else {
        result = hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
        if (result) {
            return EL_EIO;
        }
        result = hx_drv_sensorctrl_set_xSleep(1);
        if (result) {
            return EL_EIO;
        }
    }

    sensordplib_set_mclkctrl_xsleepctrl_bySCMode();

    EL_LOGD("camera init success!");

    return EL_OK;
}

el_err_code_t camera_deinit() {
    // parameter check
    if (camera_hal == NULL) {
        return EL_ENOTSUP;
    }
    if (camera_hal->power_off == NULL) {
        return EL_ENOTSUP;
    }

    camera_hal->power_off();

    return EL_OK;
}
