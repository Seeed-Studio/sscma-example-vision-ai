#ifndef CAMERA_CORE_H
#define CAMERA_CORE_H

#include <stdio.h>

#include "datapath.h"
#include "el_common.h"
#include "hx_drv_CIS_common.h"
#include "hx_drv_iic_m.h"
#include "hx_drv_iomux.h"

typedef struct {
    int8_t                  camera_sensor_addr;
    HX_CIS_SensorSetting_t* sensor_cfg;
    uint16_t                sensor_cfg_len;

    CIS_XHSHUTDOWN_INDEX_E   xshutdown_pin;
    SENSORCTRL_XSLEEP_CTRL_E xsleep_ctl;

    void   (*power_init)(void);
    void   (*power_off)(void);
    int8_t (*get_sensor_id)(uint32_t* sensor_id);
    int8_t (*set_output_size)(uint16_t width, uint16_t height);
} Camera_Hal_Struct;

#ifdef __cplusplus
extern "C" {
#endif
el_err_code_t camera_init(uint32_t width, uint32_t height);
el_err_code_t camera_deinit();
#ifdef __cplusplus
}
#endif
#endif
