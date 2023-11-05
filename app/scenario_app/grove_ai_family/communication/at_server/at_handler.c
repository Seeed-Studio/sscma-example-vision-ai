/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      at_handler.c
* @brief     处理AT命令入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-06-25
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "grove_ai_config.h"
#include "hx_drv_iomux.h"
#include "hx_drv_wdt.h"
#include "internal_flash.h"
#include "console_io.h"
#include "powermode.h"
#include "i2c_comm.h"
#include "grove_ai_config.h"
#include "logger.h"
#include "at_handler.h"

#include "cmd_sys_event.h"
#include "cmd_algo_event.h"
#include "tflitemicro_algo_common.h"

#include "sensor_core.h"

static int at_cmd_at(const char *arg, size_t len)
{
    return 1;
}

static int at_cmd_reset(const char *arg, size_t len)
{
#ifdef EXTERNAL_LDO
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_EXTERNAL_LDO);
#else
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_INTERNAL_LDO);
#endif
    return 1;
}

static int at_cmd_version(const char *arg, size_t len)
{
    at_server_reply("%02X.%02X", g_sys_handler.version[0], g_sys_handler.version[1]);
    return 0;
}

static int at_cmd_name(const char *arg, size_t len)
{
    at_server_reply("%s", BOARD_NAME);
    return 0;
}

static int at_cmd_id(const char *arg, size_t len)
{
    at_server_reply("%02X-%02X", g_sys_handler.id[0], g_sys_handler.id[1]);
    return 0;
}

static int at_cmd_state(const char *arg, size_t len)
{
    at_server_reply("%d", g_sys_handler.state);
    return 0;
}

static int at_cmd_error(const char *arg, size_t len)
{
    at_server_reply("%d", g_error);
    return 0;
}

static int at_cmd_algo(const char *arg, size_t len)
{
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d", g_algo_handler.algo);
    }
    else
    {
        int algo = atoi(arg);
        if (algo < 0 || algo >= ALGO_MAX)
        {
            return -1;
        }
        else
        {
            g_algo_handler.algo = algo;
            return 1;
        }
    }
    return 0;
}

static int at_cmd_model(const char *arg, size_t len)
{
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d", g_algo_handler.model);
    }
    else
    {
        int model = atoi(arg);
        if (model < 0 || model >= MODEL_MAX)
        {
            return -1;
        }
        else
        {
            ERROR_T ret = tflitemicro_algo_check_model(model);
            if (ERROR_NONE != ret)
            {
                LOGGER_ERROR(ret, "Check Model 0x%02X Error", model);
                return -1;
            }
            else
            {
                g_algo_handler.model = model;
                return 1;
            }
        }
    }
    return 0;
}

static int at_cmd_cfg(const char *arg, size_t len)
{
    at_server_reply("%d", g_algo_handler.rotate);
    return 0;
}

static int at_cmd_confidence(const char *arg, size_t len)
{
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d", g_algo_handler.confidence);
    }
    else
    {
        int confidence = atoi(arg);
        if (confidence < 0 || confidence > 100)
        {
            return -1;
        }
        else
        {
            g_algo_handler.confidence = confidence;
            return 1;
        }
    }
    return 0;
}

static int at_cmd_iou(const char *arg, size_t len)
{
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d", g_algo_handler.iou);
    }
    else
    {
        int iou = atoi(arg);
        if (iou < 0 || iou > 100)
        {
            return -1;
        }
        else
        {
            g_algo_handler.iou = iou;
            return 1;
        }
    }
    return 0;
}

static int at_cmd_vmodel(const char *arg, size_t len)
{
    char smodel[128] = {0};
    uint32_t model = tflitemicro_algo_get_model();
    for (int i = 0; i < MODEL_MAX; i++)
    {
        if (model & (1 << i))
        {
            snprintf(smodel + strlen(smodel), sizeof(smodel) - strlen(smodel), "%d,", i);
        }
    }
    at_server_reply("%s", smodel);
}

static int at_cmd_valgo(const char *arg, size_t len)
{
    char valgo[128] = {0};
    for (int i = 0; i < ALGO_MAX; i++)
    {
        snprintf(valgo + strlen(valgo), sizeof(valgo) - strlen(valgo), "%d,", i);
    }
    at_server_reply("%s", valgo);
}

static int at_cmd_invoke(const char *arg, size_t len)
{
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d", g_algo_handler.invoke);
    }
    else
    {
        if(g_algo_handler.state != ERROR_NONE)
        {
            return -1;
        }
        int32_t invoke = atoi(arg);
        g_algo_handler.invoke = invoke;
        return 1;
    }
    return 0;
}

static int at_cmd_save(const char *arg, size_t len)
{
    g_algo_handler.start_magic1 = ALGO_CONFIG_MAGIC1;
    g_algo_handler.start_magic2 = ALGO_CONFIG_MAGIC2;
    g_algo_handler.end_magic1 = ALGO_CONFIG_MAGIC1;
    g_algo_handler.end_magic2 = ALGO_CONFIG_MAGIC2;
    if (internal_flash_write(ALGO_CONFIG_ADDR, &g_algo_handler, sizeof(g_algo_handler)) != 0)
    {
        return -1;
    }

    return 1;
}

static int at_cmd_config_clear(const char *arg, size_t len)
{
    if (internal_flash_clear(ALGO_CONFIG_ADDR, sizeof(g_algo_handler)) != 0)
    {
        return -1;
    }
    return 1;
}

static int at_cmd_log_show(const char *arg, size_t len)
{
    error_file_show();
    return 1;
}

static int at_cmd_log_clear(const char *arg, size_t len)
{
    error_file_clear();
    return 1;
}

static int at_cmd_point(const char *arg, size_t len)
{
    algo_pointer_t *config = &g_algo_handler.config.pointer;
    char *token;
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d,%d,%d,%d,%d,%d,%d,%d", config->start_x, config->start_y, config->end_x, config->end_y, config->center_x, config->center_y, config->from, config->to);
    }
    else
    {
        token = strtok((char *)arg, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->start_x = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->start_y = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->end_x = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->end_y = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->center_x = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->center_y = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->from = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->to = atoi(token);
        return 1;
    }

    return 0;
}

static int at_cmd_camera(const char *arg, size_t len)
{
    algo_hardware_config_t *config = tflitemicro_algo_harware_config();
    ERROR_T ret;
    if (arg == NULL || len == 0)
    {
        at_server_reply("%d,%d", config->vision_config.width, config->vision_config.height);
    }
    else
    {
        char *token;
        token = strtok((char *)arg, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->vision_config.width = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            return -1;
        }
        config->vision_config.height = atoi(token);

        Sensor_Cfg_t sensor_cfg_t = {
            .sensor_type = SENSOR_CAMERA,
            .data.camera_cfg.width = config->vision_config.width,
            .data.camera_cfg.height = config->vision_config.height,
        };

        ret = datapath_init(sensor_cfg_t.data.camera_cfg.width,
                            sensor_cfg_t.data.camera_cfg.height);
        if (ret != ERROR_NONE)
        {
            return -1;
        }
        ret = sensor_init(&sensor_cfg_t);
        if (ret != ERROR_NONE)
        {
            return -1;
        }
    }
    return 0;
}

static int at_cmd_sample(const char *arg, size_t len)
{
    volatile uint32_t jpeg_addr;
    volatile uint32_t jpeg_size;
    uint32_t tick = 0;
    datapath_start_work();
    // temp
    while (!datapath_get_img_state())
    {
        tick++;
        if (tick > BOARD_PLL_CLK_400M)
        {
            return -1;
        }
    }
    datapath_get_jpeg_img(&jpeg_addr, &jpeg_size);
    hx_drv_webusb_write_vision(jpeg_addr, jpeg_size);
    return 1;
}

volatile at_cmd_t at_cmd_table[] = {
    {AT_CMD, AT_CMD_LEN, at_cmd_at},
    {AT_RESET_CMD, AT_RESET_CMD_LEN, at_cmd_reset},
    {AT_VERSION_CMD, AT_VERSION_CMD_LEN, at_cmd_version},
    {AT_NAME_CMD, AT_VERSION_CMD_LEN, at_cmd_name},
    {AT_ID_CMD, AT_ID_CMD_LEN, at_cmd_id},
    {AT_STATE_CMD, AT_STATE_CMD_LEN, at_cmd_state},
    {AT_ERROR_CMD, AT_ERROR_CMD_LEN, at_cmd_error},
    {AT_ALGO_CMD, AT_ALGO_CMD_LEN, at_cmd_algo},
    {AT_MODEL_CMD, AT_MODEL_CMD_LEN, at_cmd_model},
    {AT_CONFIG_CMD, AT_CONFIG_CMD_LEN, at_cmd_cfg},
    {AT_CONFIDENCE_CMD, AT_CONFIDENCE_CMD_LEN, at_cmd_confidence},
    {AT_IOU_CMD, AT_IOU_CMD_LEN, at_cmd_iou},
    {AT_VMODEL_CMD, AT_VMODEL_CMD_LEN, at_cmd_vmodel},
    {AT_VALGO_CMD, AT_VALGO_CMD_LEN, at_cmd_valgo},
    {AT_INVOKE_CMD, AT_INVOKE_CMD_LEN, at_cmd_invoke},
    {AT_SAVE_CMD, AT_SAVE_CMD_LEN, at_cmd_save},
    {AT_CLEAR_CMD, AT_CLEAR_CMD_LEN, at_cmd_config_clear},
    {AT_LOG_CMD, AT_LOG_CMD_LEN, at_cmd_log_show},
    {AT_POINT_CMD, AT_POINT_CMD_LEN, at_cmd_point},
    {AT_CAMERA_CMD, AT_CAMERA_CMD_LEN, at_cmd_camera},
    {AT_SAMPLE_CMD, AT_SAMPLE_CMD_LEN, at_cmd_sample},
    {NULL, NULL},
};