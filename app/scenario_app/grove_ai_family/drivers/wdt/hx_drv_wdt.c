/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      hx_drv_wdt.c
* @brief     内部看门狗驱动
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-06-25
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include "grove_ai_config.h"
#include "board_config.h"
#include "hx_drv_timer.h"
#include "hx_drv_pmu.h"
#include "hx_drv_iomux.h"
#include "powermode.h"
#include "logger.h"
#include "debugger.h"

void pmu_wdt_timeout_cb(void *param)
{
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, 1 - I2C_SYNC_STATE);
    LOGGER_ERROR(ERROR_WDT_TIMEOUT, "Watchdog timeout.");
#ifdef EXTERNAL_LDO
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_EXTERNAL_LDO);
#else
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_INTERNAL_LDO);
#endif
}

void hx_drv_wdt_start(uint32_t timeout_ms, void (*timeout_cb)(void *param))
{
    hx_drv_pmu_wdt_start(PMU_TIMEOUT_WDT, timeout_ms, 1, pmu_wdt_timeout_cb); // TODO timeout_cb
}

void hx_drv_wdt_feed(void)
{

    hx_drv_pmu_set_ctrl(PMU_WDT_RTC_LOAD_EN1T, 1); // timer reload
}
