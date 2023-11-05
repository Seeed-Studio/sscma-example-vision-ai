/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_gpio_event.h
* @brief     处理GPIO相关事务
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-05-18
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdint.h>

#include "hx_drv_iomux.h"

#include "cmd_def.h"

/* Exported macros ---------------------------------------------------------*/
#define FEATURE_GPIO 0x90

#define CMD_GPIO_READ_STATE 0x00
#define CMD_GPIO_READ_STATE_LENGTH 0x01
#define CMD_GPIO_WRITE_STATE 0x01
#define CMD_GPIO_WRITE_STATE_LENGTH 0x02

typedef enum
{
    CMD_GPIO_MODE_INPUT = 0x00,
    CMD_GPIO_MODE_OUTPUT = 0x01,
    CMD_GPIO_MODE_NONE = 0x02
} CMD_GPIO_MODE_T;

typedef enum
{
    CMD_GPIO_STATE_LOW = 0x00,
    CMD_GPIO_STATE_HIGH = 0x01,
} CMD_GPIO_STATE_T;

extern const cmd_event_t gpio_event_list[];

ERROR_T cmd_gpio_event_init(void);