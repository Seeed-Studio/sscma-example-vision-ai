/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      hx_drv_wdt.h
* @brief     内部看门狗驱动
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-06-25
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef HX_DRV_WDT__H
#define HX_DRV_WDT__H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void hx_drv_wdt_start(uint32_t timeout_ms, void (*timeout_cb)(void *param));
    void hx_drv_wdt_feed(void);

#ifdef __cplusplus
}
#endif

#endif