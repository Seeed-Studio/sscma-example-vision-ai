/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      meter.c
* @brief
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-12-09
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include "hx_drv_timer.h"
#include "grove_ai_config.h"
#include "hx_drv_webusb.h"
#include "logger.h"
#include "debugger.h"
#include "datapath.h"
#include "sensor_core.h"
#include "external_flash.h"
#include "communication_core.h"
#include "hx_drv_lsm6ds3.h"
#include "tflitemicro_algo.h"

int main(void)
{
    int ret = 0;

    hx_drv_timer_init();
    debugger_init();

    external_flash_xip_enable();

    communication_init();

    DEV_LSM6DS3_PTR lsm6ds3 = hx_drv_lsm6ds3_init((USE_SS_IIC_E)SS_IIC_0_ID);
    hx_drv_lsm6ds3_begin(lsm6ds3);
    ret = tflitemicro_algo_init();
    float data[186] = {0};
    while (1)
    {
        for (int i = 0; i < 186; i += 3)
        {
            data[i] = (float)hx_drv_lsm6ds3_read_acc_x(lsm6ds3) * 9.8;
            data[i + 1] = (float)hx_drv_lsm6ds3_read_acc_y(lsm6ds3) * 9.8;
            data[i + 2] = (float)hx_drv_lsm6ds3_read_acc_z(lsm6ds3) * 9.8;
            board_delay_ms(16);
        }
        tflitemicro_algo_run(data);
    }

    return 0;
}
