/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      grove_ai_family.cc
* @brief     程序主函数入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include "board_config.h"
#include "hx_drv_timer.h"
#include "console_io.h"
#include "grove_ai_family.h"
#include "grove_ai_config.h"
#include "logger.h"

#include "debugger.h"

#include "cmd_handler.h"
#include "cmd_sys_event.h"
#include "cmd_algo_event.h"

#include "at_server.h"

#include "hx_drv_wdt.h"

#include "tflitemicro_algo_common.h"
#include "tflitemicro_algo.h"
#include "algo_object_detection.h"
#include "algo_object_count.h"
#include "algo_image_classification.h"

#include "datapath.h"
#include "sensor_core.h"
#include "communication_core.h"

#include "external_flash.h"

#include "bird.h"
#include "person.h"
#include "ak47.h"
#include "bathtub.h"
#include "ak47_96x96.h"
#include "bathtub_96x96.h"

extern "C" void app_main(void)
{

    char c;
    hx_drv_timer_init();
    debugger_init();
    hx_drv_wdt_start(10000, NULL);

    LOGGER_INFO("\r\n");
    LOGGER_INFO("  _____                     _______ __            ___             \r\n");
    LOGGER_INFO(" / ___/ ___  ___  ___  ____/ / ___// /___  ______/ (_)___         \r\n");
    LOGGER_INFO(" \\__ \\ / _ \\/ _ \\/ _ \\/ __  /\\__ \\/ __/ / / / __  / / __ \\\r\n");
    LOGGER_INFO(" ___/ /  __/  __/  __/ /_/ /___/ / /_/ /_/ / /_/ / / /_/ /       \r\n");
    LOGGER_INFO("/____/\\___/\\___/\\___/\\____//____/\\__/\\____/\\____/_/\\____/\r\n");
    LOGGER_INFO("\r\n");
    LOGGER_INFO("Name: %s\r\n", BOARD_NAME);
    LOGGER_INFO("Version: %02X-%02X\r\n", GROVE_AI_FAMILY_MAIN_VER, GROVE_AI_FAMILY_SUB_VER);
    LOGGER_INFO("ID: %02X-%02X\r\n", GROVE_AI_FAMILY_MAIN_ID, GROVE_AI_FAMILY_SUB_ID);
    LOGGER_INFO("Build:%s,%s\r\n", __TIME__, __DATE__);
    LOGGER_INFO("Copyright (c) 2022 Seeed Studio\r\n");

    external_flash_xip_enable();

    error_file_load();

    communication_init();

    tflitemicro_algo_init();

    cmd_handler_init();
    at_server_init();

    cmd_handler_start();
    at_server_start();
    for (;;)
    {
        hx_drv_wdt_feed();
        cmd_handler_task();
        at_server_task();
    }

    return;
}
