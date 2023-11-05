/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_handler.h
* @brief     处理i2c命令入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef __CMD_HANDLER_H
#define __CMD_HANDLER_H

#include "cmd_def.h"

typedef enum
{
    CMD_HANDLER_STATE_CMD,
    CMD_HANDERL_STATE_OP,
} CMD_HANDLER_STATE_T;

#ifdef __cplusplus
extern "C" {
#endif

void cmd_handler_init(void);
void cmd_handler_start(void);
void cmd_handler_task(void);
void cmd_handler_lock(void);
void cmd_handler_unlock(void);

#ifdef __cplusplus
}
#endif

#endif
