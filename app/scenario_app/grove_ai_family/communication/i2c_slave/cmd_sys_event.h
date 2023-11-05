/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_sys_event.c
* @brief     处理system事务
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/
#ifndef __CMD_SYSTEM_EVENT_H
#define __CMD_SYSTEM_EVENT_H

#include "grove_ai_config.h"
#include "cmd_def.h"

/* Exported macros ---------------------------------------------------------*/
#define FEATURE_SYSTEM 0x80

#define CMD_SYS_READ_VERSION 0x01
#define CMD_SYS_VERSION_LENGTH 2
#define CMD_SYS_READ_ID 0x02
#define CMD_SYS_ID_LENGTH 2
#define CMD_SYS_READ_STATE 0x03
#define CMD_SYS_STATE_LENGTH 1
#define CMD_SYS_READ_ERROR_CODE 0x04
#define CMD_SYS_ERROR_CODE_LENGTH 1
#define CMD_SYS_RESET 0x20

/**
 * @brief sys event
 *
 *
 *
 */
typedef struct
{
    uint8_t version[2];
    uint8_t id[2];
    CMD_STATE_T state;
} cmd_sys_event_t;

extern volatile cmd_sys_event_t g_sys_handler;
extern const cmd_event_t sys_event_list[];

ERROR_T cmd_sys_event_init(void);

#endif