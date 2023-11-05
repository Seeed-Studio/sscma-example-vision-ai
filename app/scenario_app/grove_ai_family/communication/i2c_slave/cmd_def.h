/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_def.c
* @brief     I2C命令相关数据结构定义
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef __CMD_DEF_H
#define __CMD_DEF_H

#include <stdbool.h>
#include <stdint.h>

/* Exported macros ---------------------------------------------------------*/
#define CMD_HEADER_LENGTH 0x02

#define FEATURE_INVAILD 0xFF
#define CMD_INVAILD 0xFF

/* Exported typedef --------------------------------------------------------*/

/**
 * @brief
 *
 *
 *
 */
typedef enum cmd_op
{
    CMD_READ = 0x00,
    CMD_WRITE = 0x01,
    CMD_NONE = 0x02
} CMD_OP_T;

/**
 * @brief cmd state enum
 *
 * This enumeration type is used to record CMD interaction status
 *
 */
typedef enum cmd_state
{
    CMD_STATE_IDLE = 0x0,
    CMD_STATE_RUNNING = 0x01,
    CMD_STATE_ERROR = 0x02
} CMD_STATE_T;

typedef CMD_STATE_T (*cmd_callback_t)(uint8_t *read_buf, uint8_t *write_buf);

/**
 * @brief
 *
 *
 *
 */
typedef struct
{
    uint8_t cmd;
    CMD_OP_T op;
    bool check_busy;
    uint8_t length;
    cmd_callback_t cb;
} cmd_event_t;

/**
 * @brief
 *
 *
 *
 */
typedef struct
{
    uint8_t feature;
    cmd_event_t *list;
} feature_event_t;

#endif
