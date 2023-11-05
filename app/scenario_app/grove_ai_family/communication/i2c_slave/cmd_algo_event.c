/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_algo_event.c
* @brief     处理算法相关事务事务
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-24
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdint.h>
#include <stdlib.h>

#include "hardware_config.h"
#include "hx_drv_timer.h"

#include "isp.h"

#include "grove_ai_config.h"
#include "logger.h"

#include "internal_flash.h"

#include "sensor_core.h"

#include "tflitemicro_algo_common.h"
#include "tflitemicro_algo.h"

#include "cmd_def.h"
#include "cmd_handler.h"
#include "cmd_sys_event.h"
#include "cmd_algo_event.h"

volatile cmd_algo_event_t g_algo_handler;

static char preview_str[4096] = {0};

void cmd_algo_config_show()
{
    LOGGER_INFO("cmd_algo_event_t:\n{");
    LOGGER_INFO("\tstart_magic1: %08X\n", g_algo_handler.start_magic1);
    LOGGER_INFO("\tstart_magic2: %08X\n", g_algo_handler.start_magic2);
    LOGGER_INFO("\talgo: %d\n", g_algo_handler.algo);
    LOGGER_INFO("\tmodel: %d\n", g_algo_handler.model);
    LOGGER_INFO("\tperiod: %d\n", g_algo_handler.period);
    LOGGER_INFO("\tconfidence: %d\n", g_algo_handler.confidence);
    LOGGER_INFO("\tiou: %d\n", g_algo_handler.iou);
    LOGGER_INFO("\tend_magic1: %08X\n", g_algo_handler.end_magic1);
    LOGGER_INFO("\tend_magic2: %08X\n", g_algo_handler.end_magic2);

    LOGGER_INFO("}\n");
}

ERROR_T hardware_init()
{
    ALGO_TYPE_T algo_type = tflitemicro_algo_type();
    ERROR_T ret = ERROR_NONE;
    switch (algo_type)
    {

    case ALGO_VISION:
    {
        const algo_hardware_config_t *hardware_config = tflitemicro_algo_harware_config();
        LOGGER_INFO("Sensor_Cfg_t width: %d, height: %d\n", hardware_config->vision_config.width, hardware_config->vision_config.height);
        Sensor_Cfg_t sensor_cfg_t = {
            .sensor_type = SENSOR_CAMERA,
            .data.camera_cfg.width = hardware_config->vision_config.width,
            .data.camera_cfg.height = hardware_config->vision_config.height,
        };

        ret = datapath_init(sensor_cfg_t.data.camera_cfg.width,
                            sensor_cfg_t.data.camera_cfg.height);
        if (ret != ERROR_NONE)
        {
            return ret;
        }
        ret = sensor_init(&sensor_cfg_t);
        if (ret != ERROR_NONE)
        {
            return ret;
        }
    }

    default:
        break;
    }

    return ERROR_NONE;
}

ERROR_T preview()
{
    ALGO_TYPE_T algo_type = tflitemicro_algo_type();
    switch (algo_type)
    {
    case ALGO_VISION:
    {
        volatile uint32_t jpeg_addr;
        volatile uint32_t jpeg_size;
        datapath_get_jpeg_img(&jpeg_addr, &jpeg_size);
        hx_drv_webusb_write_vision(jpeg_addr, jpeg_size);
    }
    break;
    default:
        break;
    }

    if (ERROR_NONE == tflitemicro_algo_get_preview(preview_str, sizeof(preview_str)))
    {
        hx_drv_webusb_write_text((uint8_t *)preview_str, strlen(preview_str));
        LOGGER_INFO("%s\n", preview_str);
    }

    return ERROR_NONE;
}

ERROR_T invoke()
{
    ALGO_TYPE_T algo_type = tflitemicro_algo_type();
    const uint8_t *data = NULL;
    uint32_t tick = 0;
    switch (algo_type)
    {
    case ALGO_VISION:
    {

        datapath_start_work();
        // temp
        while (!datapath_get_img_state())
        {
            tick++;
            if (tick > BOARD_PLL_CLK_400M)
            {
                return ERROR_ALGO_INVOKE;
            }
        }
        data = datapath_get_yuv_img_addr();
    }
    default:
        break;
    }
    if (data != NULL)
    {
        return tflitemicro_algo_invoke(data, &g_algo_handler.ret_len);
    }
    else
    {
        return ERROR_ALGO_INVOKE;
    }
}

ERROR_T cmd_algo_event_init()
{
    memset(&g_algo_handler, 0, sizeof(g_algo_handler));
    if (internal_flash_read(ALGO_CONFIG_ADDR, &g_algo_handler, sizeof(g_algo_handler)) == 0)
    {

        g_algo_handler.period = 0;
        g_algo_handler.ret_len = 0;
        g_algo_handler.state = ERROR_NONE;
        g_algo_handler.invoke = 0;

        if (g_algo_handler.start_magic1 != ALGO_CONFIG_MAGIC1 || g_algo_handler.start_magic2 != ALGO_CONFIG_MAGIC2 ||
            g_algo_handler.end_magic1 != ALGO_CONFIG_MAGIC1 || g_algo_handler.end_magic2 != ALGO_CONFIG_MAGIC2)
        {
            g_algo_handler.start_magic1 = ALGO_CONFIG_MAGIC1;
            g_algo_handler.start_magic2 = ALGO_CONFIG_MAGIC2;
            g_algo_handler.end_magic1 = ALGO_CONFIG_MAGIC1;
            g_algo_handler.end_magic2 = ALGO_CONFIG_MAGIC2;
            g_algo_handler.algo = ALGO_IMAGE_CLASSIFICATION;
            g_algo_handler.model = MODEL_PRE_INDEX_1;
            g_algo_handler.confidence = 50;
            g_algo_handler.iou = 45;
            g_algo_handler.config.pointer.start_x = 20;
            g_algo_handler.config.pointer.start_y = 200;
            g_algo_handler.config.pointer.end_x = 200;
            g_algo_handler.config.pointer.end_y = 200;
            g_algo_handler.config.pointer.center_x = 120;
            g_algo_handler.config.pointer.center_y = 120;
            g_algo_handler.config.pointer.from = 0;
            g_algo_handler.config.pointer.to = 1000;
            internal_flash_write(ALGO_CONFIG_ADDR, &g_algo_handler, sizeof(g_algo_handler));
        }
    }
    else
    {

        g_algo_handler.algo = ALGO_IMAGE_CLASSIFICATION;
        g_algo_handler.model = MODEL_PRE_INDEX_1;
        g_algo_handler.period = 0;
        g_algo_handler.state = ERROR_NONE;
        g_algo_handler.invoke = 0;
        g_algo_handler.ret_len = 0;
        g_algo_handler.confidence = 50;
        g_algo_handler.iou = 45;
    }
    g_algo_handler.ret_len = 0;
    g_algo_handler.rotate = VISION_ROTATION;

#ifdef DEBUG_LOCAL
    g_algo_handler.algo = ALGO_OBJECT_DETECTION;
    g_algo_handler.model = 0x11;
    g_algo_handler.confidence = 50;
    g_algo_handler.iou = 45;
#endif

    cmd_algo_config_show();

    ERROR_T ret = ERROR_NONE;
    // setup algo filter config
    algo_filter_config_t fliter_config;
    fliter_config.confidence = g_algo_handler.confidence;
    fliter_config.iou = g_algo_handler.iou;

    // setup
    ret = tflitemicro_algo_setup(g_algo_handler.algo, g_algo_handler.model, &fliter_config);

    if (ret == ERROR_NONE)
    {
        tflitemicro_algo_show();
        ret = hardware_init();
    }
    else
    {
        g_sys_handler.state = CMD_STATE_ERROR;
    }
    g_algo_handler.state = ret;
    return ret;
}
void cmd_algo_event_task(void *args)
{
    ERROR_T ret;
    static uint64_t tick = 0;
    if (g_algo_handler.state == ERROR_NONE)
    {
#ifdef DEBUG_LOCAL
        g_algo_handler.invoke = CMD_ALGO_INVOKE_START;
#endif
        if (g_algo_handler.invoke >= 1 || g_algo_handler.invoke < 0)
        {
            if (g_algo_handler.invoke >= 1)
            {
                g_algo_handler.invoke--;
            }
            ret = invoke();
            CMD_STATE_T state = CMD_STATE_RUNNING;
            if (ERROR_NONE != ret)
            {
                state = CMD_STATE_ERROR;
            }
            else
            {
                state = CMD_STATE_IDLE;
            }
            preview();
            g_sys_handler.state = state;
        }
    }
    else
    {
        tick++;
        if (tick > BOARD_PLL_CLK_400M / 100)
        {
            tick = 0;
            get_error(preview_str, g_algo_handler.state, sizeof(preview_str));
            hx_drv_webusb_write_text((uint8_t *)preview_str, strlen(preview_str));
            LOGGER_INFO("%s\n", preview_str);
        }
    }
}

CMD_STATE_T cmd_algo_read_algo(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    write_buf[0] = g_algo_handler.algo;

    _EXIT;

    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_write_algo(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint8_t algo = read_buf[2];

    if (algo < ALGO_MAX)
    {
        g_algo_handler.algo = algo;
    }

    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_model(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_algo_handler.model;

    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_write_model(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint8_t model = read_buf[2];

    if (model < MODEL_MAX)
    {
        ERROR_T ret = tflitemicro_algo_check_model(model);
        if (ERROR_NONE != ret)
        {
            LOGGER_ERROR(ret, "Check Model 0x%02X Error\n\r", model);
            return CMD_STATE_ERROR;
        }
        else
        {
            g_algo_handler.model = model;
        }
    }
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_vaild_model(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    uint32_t model = tflitemicro_algo_get_model();

    write_buf[3] = (model >> 24) & 0XFF;
    write_buf[2] = (model >> 16) & 0xFF;
    write_buf[1] = (model >> 8) & 0xFF;
    write_buf[0] = model & 0xFF;

    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_period(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = (g_algo_handler.period >> 24) & 0XFF;
    write_buf[1] = (g_algo_handler.period >> 16) & 0xFF;
    write_buf[2] = (g_algo_handler.period >> 8) & 0xFF;
    write_buf[3] = g_algo_handler.period & 0xFF;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_write_period(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    g_algo_handler.period = 0;

    g_algo_handler.period += (uint32_t)read_buf[2] << 24;
    g_algo_handler.period += (uint32_t)read_buf[3] << 16;
    g_algo_handler.period += (uint32_t)read_buf[4] << 8;
    g_algo_handler.period += (uint32_t)read_buf[5];

    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_confidence(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_algo_handler.confidence;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_write_confidence(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint8_t confidence = read_buf[2];

    if (confidence > 100)
    {
        confidence = 100;
    }
    g_algo_handler.confidence = confidence;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_point(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_algo_handler.config.pointer.start_x >> 8 & 0xFF;
    write_buf[1] = g_algo_handler.config.pointer.start_x & 0xFF;
    write_buf[2] = g_algo_handler.config.pointer.start_y >> 8 & 0xFF;
    write_buf[3] = g_algo_handler.config.pointer.start_y & 0xFF;
    write_buf[4] = g_algo_handler.config.pointer.end_x >> 8 & 0xFF;
    write_buf[5] = g_algo_handler.config.pointer.end_x & 0xFF;
    write_buf[6] = g_algo_handler.config.pointer.end_y >> 8 & 0xFF;
    write_buf[7] = g_algo_handler.config.pointer.end_y & 0xFF;
    write_buf[8] = g_algo_handler.config.pointer.center_x >> 8 & 0xFF;
    write_buf[9] = g_algo_handler.config.pointer.center_x & 0xFF;
    write_buf[10] = g_algo_handler.config.pointer.center_y >> 8 & 0xFF;
    write_buf[11] = g_algo_handler.config.pointer.center_y & 0xFF;
    write_buf[12] = g_algo_handler.config.pointer.from >> 24 & 0xFF;
    write_buf[13] = g_algo_handler.config.pointer.from >> 16 & 0xFF;
    write_buf[14] = g_algo_handler.config.pointer.from >> 8 & 0xFF;
    write_buf[15] = g_algo_handler.config.pointer.from & 0xFF;
    write_buf[16] = g_algo_handler.config.pointer.to >> 24 & 0xFF;
    write_buf[17] = g_algo_handler.config.pointer.to >> 16 & 0xFF;
    write_buf[18] = g_algo_handler.config.pointer.to >> 8 & 0xFF;
    write_buf[19] = g_algo_handler.config.pointer.to & 0xFF;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_write_point(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    g_algo_handler.config.pointer.start_x = (uint16_t)read_buf[2] << 8 | (uint16_t)read_buf[3];
    g_algo_handler.config.pointer.start_y = (uint16_t)read_buf[4] << 8 | (uint16_t)read_buf[5];
    g_algo_handler.config.pointer.end_x = (uint16_t)read_buf[6] << 8 | (uint16_t)read_buf[7];
    g_algo_handler.config.pointer.end_y = (uint16_t)read_buf[8] << 8 | (uint16_t)read_buf[9];
    g_algo_handler.config.pointer.center_x = (uint16_t)read_buf[10] << 8 | (uint16_t)read_buf[11];
    g_algo_handler.config.pointer.center_y = (uint16_t)read_buf[12] << 8 | (uint16_t)read_buf[13];
    g_algo_handler.config.pointer.from = (uint32_t)read_buf[14] << 24 | (uint32_t)read_buf[15] << 16 | (uint32_t)read_buf[16] << 8 | (uint32_t)read_buf[17];
    g_algo_handler.config.pointer.to = (uint32_t)read_buf[18] << 24 | (uint32_t)read_buf[19] << 16 | (uint32_t)read_buf[20] << 8 | (uint32_t)read_buf[21];
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_iou(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_algo_handler.iou;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_write_iou(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint8_t iou = read_buf[2];

    if (iou > 100)
    {
        iou = 100;
    }
    g_algo_handler.iou = iou;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_ret_len(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = (g_algo_handler.ret_len >> 8) & 0xFF;
    write_buf[1] = g_algo_handler.ret_len & 0xFF;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_read_ret(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint16_t len = 0;
    uint16_t index = ((uint16_t)read_buf[2] << 8) + read_buf[3];
    uint8_t data[128] = {0};
    if (ERROR_NONE == tflitemicro_algo_get_result(index, data, &len))
    {
        memcpy(write_buf, data, len);
    }
    else
    {
        return CMD_STATE_ERROR;
    }
    /* we need to change the detect result length due to different algorithm */
    for (uint8_t i = 0; i < 256; ++i)
    {
        if (algo_event_list[i].cmd == CMD_ALGO_READ_RET)
        {
            algo_event_list[i].length = len;
            break;
        }
    }
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_config_save(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    g_algo_handler.start_magic1 = ALGO_CONFIG_MAGIC1;
    g_algo_handler.start_magic2 = ALGO_CONFIG_MAGIC2;
    g_algo_handler.end_magic1 = ALGO_CONFIG_MAGIC1;
    g_algo_handler.end_magic2 = ALGO_CONFIG_MAGIC2;

    if (internal_flash_write(ALGO_CONFIG_ADDR, &g_algo_handler, sizeof(g_algo_handler)) != 0)
    {
        return CMD_STATE_ERROR;
    }
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_config_clear(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    if (internal_flash_clear(ALGO_CONFIG_ADDR, sizeof(g_algo_handler)) != 0)
    {
        return CMD_STATE_ERROR;
    }
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_algo_invoke(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    if (g_algo_handler.state != ERROR_NONE)
    {
        return CMD_STATE_ERROR;
    }
    g_algo_handler.invoke = CMD_ALGO_INVOKE_START;
    _EXIT;
    return CMD_STATE_RUNNING;
}

cmd_event_t algo_event_list[] = {
    {
        .cmd = CMD_ALGO_READ_ALGO,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_ALGO_ALGO_LENGTH,
        .cb = cmd_algo_read_algo,
    },
    {
        .cmd = CMD_ALGO_WRITE_ALGO,
        .op = CMD_WRITE,
        .check_busy = false,
        .length = CMD_ALGO_ALGO_LENGTH,
        .cb = cmd_algo_write_algo,
    },
    {
        .cmd = CMD_ALGO_READ_MODEL,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_ALGO_MODEL_LENGTH,
        .cb = cmd_algo_read_model,
    },
    {
        .cmd = CMD_ALGO_WRITE_MODEL,
        .op = CMD_WRITE,
        .check_busy = true,
        .length = CMD_ALGO_MODEL_LENGTH,
        .cb = cmd_algo_write_model,
    },
    {
        .cmd = CMD_ALGO_READ_VALID_MODEL,
        .op = CMD_READ,
        .check_busy = true,
        .length = CMD_ALGO_VALID_MODEL_LENGTH,
        .cb = cmd_algo_read_vaild_model,
    },
    {
        .cmd = CMD_ALGO_READ_PERIOD,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_ALGO_PERIOD_LENGTH,
        .cb = cmd_algo_read_period,
    },
    {
        .cmd = CMD_ALGO_WRITE_PERIOD,
        .op = CMD_WRITE,
        .check_busy = true,
        .length = CMD_ALGO_PERIOD_LENGTH,
        .cb = cmd_algo_write_period,
    },
    {
        .cmd = CMD_ALGO_READ_CONFIDENCE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_ALGO_CONFIDENCE_LENGTH,
        .cb = cmd_algo_read_confidence,
    },
    {
        .cmd = CMD_ALGO_WRITE_CONFIDENCE,
        .op = CMD_WRITE,
        .check_busy = true,
        .length = CMD_ALGO_CONFIDENCE_LENGTH,
        .cb = cmd_algo_write_confidence,
    },
    {
        .cmd = CMD_ALGO_READ_IOU,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_ALGO_IOU_LENGTH,
        .cb = cmd_algo_read_iou,
    },
    {
        .cmd = CMD_ALGO_WRITE_IOU,
        .op = CMD_WRITE,
        .check_busy = true,
        .length = CMD_ALGO_IOU_LENGTH,
        .cb = cmd_algo_write_iou,
    },
    {
        .cmd = CMD_ALGO_READ_RET_LEN,
        .op = CMD_READ,
        .check_busy = true,
        .length = CMD_ALGO_READ_RET_LEN_LENGTH,
        .cb = cmd_algo_read_ret_len,
    },
    {
        .cmd = CMD_ALGO_READ_POINTER,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_ALGO_POINTER_LENGTH,
    },
    {
        .cmd = CMD_ALGO_WRITE_POINTER,
        .op = CMD_WRITE,
        .check_busy = true,
        .length = CMD_ALGO_POINTER_LENGTH,
    },
    {
        .cmd = CMD_ALGO_READ_RET,
        .op = CMD_READ,
        .check_busy = true,
        .length = 0,
        .cb = cmd_algo_read_ret,
    },
    {
        .cmd = CMD_ALGO_CONFIG_SAVE,
        .op = CMD_NONE,
        .check_busy = true,
        .length = 0,
        .cb = cmd_algo_config_save,
    },
    {
        .cmd = CMD_ALGO_CONFIG_CLEAR,
        .op = CMD_NONE,
        .check_busy = true,
        .length = 0,
        .cb = cmd_algo_config_clear,
    },
    {
        .cmd = CMD_ALGO_INOVKE,
        .op = CMD_NONE,
        .check_busy = true,
        .length = 0,
        .cb = cmd_algo_invoke,
    },
    {
        .cmd = 0xFF,
    },
};