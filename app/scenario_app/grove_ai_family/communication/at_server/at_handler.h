/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      at_handler.h
* @brief     处理AT命令入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-06-25
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef __AT_HANDLER_H
#define __AT_HANDLER_H

#include <stdbool.h>
#include <stdint.h>
#include "at_server.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AT_RESPONSE_OK "OK"
#define AT_RESPONSE_ERROR "ERROR"

#define AT_CMD(cmd, func)        \
    {                            \
        .cmd = cmd, .func = func \
    }

#define AT_CMD "AT"
#define AT_CMD_LEN (sizeof(AT_CMD) - 1)

#define AT_NAME_CMD "AT+NAME"
#define AT_NAME_CMD_LEN (sizeof(AT_NAME_CMD) - 1)

#define AT_RESET_CMD "AT+RST"
#define AT_RESET_CMD_LEN (sizeof(AT_RESET_CMD) - 1)

#define AT_VERSION_CMD "AT+VER"
#define AT_VERSION_CMD_LEN (sizeof(AT_VERSION_CMD) - 1)

#define AT_ID_CMD "AT+ID"
#define AT_ID_CMD_LEN (sizeof(AT_ID_CMD) - 1)

#define AT_STATE_CMD "AT+STATE"
#define AT_STATE_CMD_LEN (sizeof(AT_STATE_CMD) - 1)

#define AT_ERROR_CMD "AT+ERR"
#define AT_ERROR_CMD_LEN (sizeof(AT_ERROR_CMD) - 1)

#define AT_ALGO_CMD "AT+ALGO"
#define AT_ALGO_CMD_LEN (sizeof(AT_ALGO_CMD) - 1)

#define AT_MODEL_CMD "AT+MODEL"
#define AT_MODEL_CMD_LEN (sizeof(AT_MODEL_CMD) - 1)

#define AT_VMODEL_CMD "AT+VMODEL"
#define AT_VMODEL_CMD_LEN (sizeof(AT_VMODEL_CMD) - 1)

#define AT_VALGO_CMD "AT+VALGO"
#define AT_VALGO_CMD_LEN (sizeof(AT_VALGO_CMD) - 1)

#define AT_CONFIDENCE_CMD "AT+CONF"
#define AT_CONFIDENCE_CMD_LEN (sizeof(AT_CONFIDENCE_CMD) - 1)

#define AT_CONFIG_CMD "AT+CFG"
#define AT_CONFIG_CMD_LEN (sizeof(AT_CONFIG_CMD) - 1)

#define AT_IOU_CMD "AT+IOU"
#define AT_IOU_CMD_LEN (sizeof(AT_IOU_CMD) - 1)

#define AT_INVOKE_CMD "AT+INVOKE"
#define AT_INVOKE_CMD_LEN (sizeof(AT_INVOKE_CMD) - 1)

#define AT_RLEN_CMD "AT+RLEN"
#define AT_RLEN_CMD_LEN (sizeof(AT_RLEN_CMD) - 1)

#define AT_RDATA_CMD "AT+RDATA"
#define AT_RDATA_CMD_LEN (sizeof(AT_RDATA_CMD) - 1)

#define AT_SAVE_CMD "AT+SAVE"
#define AT_SAVE_CMD_LEN (sizeof(AT_SAVE_CMD) - 1)

#define AT_CLEAR_CMD "AT+CLEAR"
#define AT_CLEAR_CMD_LEN (sizeof(AT_CLEAR_CMD) - 1)

#define AT_LOG_CMD "AT+LOG"
#define AT_LOG_CMD_LEN (sizeof(AT_LOG_CMD) - 1)

#define AT_POINT_CMD "AT+POINT"
#define AT_POINT_CMD_LEN (sizeof(AT_POINT_CMD) - 1)

#define AT_CAMERA_CMD "AT+CAMERA"
#define AT_CAMERA_CMD_LEN (sizeof(AT_CAMERA_CMD) - 1)

#define AT_SAMPLE_CMD "AT+SAMPLE"
#define AT_SAMPLE_CMD_LEN (sizeof(AT_SAMPLE_CMD) - 1)

    extern volatile at_cmd_t at_cmd_table[];

#ifdef __cplusplus
}
#endif

#endif