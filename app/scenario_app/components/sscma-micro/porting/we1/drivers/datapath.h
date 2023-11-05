#ifndef DATAPATH_H
#define DATAPATH_H

#include "el_common.h"
#include "hx_drv_hw5x5.h"
#include "hx_drv_jpeg.h"
#include "iot_custom_config.h"

/*Error Retry Count*/
#define MAX_HW5x5JPEG_ERR_RETRY_CNT 10

typedef enum {
    APP_STATE_INIT,
    APP_STATE_FACE_LIVE_HW5X5JPEG,
    APP_STATE_STOP,
} APP_STATE_E;

typedef struct {
    uint32_t    wdma1_startaddr;
    uint32_t    wdma2_startaddr;
    uint32_t    wdma3_startaddr;
    uint32_t    jpegsize_autofill_startaddr;
    uint8_t     cyclic_buffer_cnt;
    HW5x5_CFG_T hw5x5_cfg;
    JPEG_CFG_T  jpeg_cfg;
} DataPath_struct;

typedef struct {
    volatile uint8_t  g_frame_ready;
    volatile uint32_t g_frame_count;

} DataPath_state;

#ifdef __cplusplus
extern "C" {
#endif
el_err_code_t datapath_init(uint16_t width, uint16_t height);
el_err_code_t datapath_start_work(void);
el_err_code_t datapath_stop_work(void);
el_err_code_t datapath_capture(uint32_t timeout);
el_err_code_t datapath_set_roi_start_position(uint16_t start_x, uint16_t start_y);
el_img_t      datapath_get_frame();
el_img_t      datapath_get_jpeg();
#ifdef __cplusplus
}
#endif

#endif
