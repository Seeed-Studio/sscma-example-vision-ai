/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_gpio_event.c
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
#include <stdlib.h>

#include "grove_ai_config.h"
#include "logger.h"

#include "hx_drv_iomux.h"

#include "cmd_gpio_event.h"

ERROR_T cmd_gpio_event_init()
{
    return ERROR_NONE;
}

CMD_STATE_T cmd_gpio_read_state(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint8_t index = read_buf[2];
    uint8_t value = 0;

    hx_drv_iomux_set_pmux(index, 2);
    hx_drv_iomux_get_invalue(index, &value);

    write_buf[0] = value;
    _EXIT;

    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_gpio_write_state(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    uint8_t index = read_buf[2];
    uint8_t value = read_buf[3];

    hx_drv_iomux_set_pmux(index, 3);
    hx_drv_iomux_set_outvalue(index, value);

    _EXIT;
    return CMD_STATE_IDLE;
}

const cmd_event_t gpio_event_list[] = {
    {
        .cmd = CMD_GPIO_READ_STATE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_GPIO_READ_STATE_LENGTH,
        .cb = cmd_gpio_read_state,
    },
    {
        .cmd = CMD_GPIO_WRITE_STATE,
        .op = CMD_WRITE,
        .check_busy = false,
        .length = CMD_GPIO_WRITE_STATE_LENGTH,
        .cb = cmd_gpio_write_state,
    },
    {
        .cmd = 0xFF,
    },
};