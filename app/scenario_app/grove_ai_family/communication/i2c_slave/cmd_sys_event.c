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

#include <stdint.h>
#include <stdlib.h>

#include "grove_ai_config.h"
#include "logger.h"

#include "powermode.h"

#include "cmd_def.h"
#include "cmd_sys_event.h"

volatile cmd_sys_event_t g_sys_handler;

ERROR_T cmd_sys_event_init()
{
    g_sys_handler.state = CMD_STATE_IDLE;
    g_sys_handler.version[0] = GROVE_AI_FAMILY_MAIN_VER;
    g_sys_handler.version[1] = GROVE_AI_FAMILY_SUB_VER;
    g_sys_handler.id[0] = GROVE_AI_FAMILY_MAIN_ID;
    g_sys_handler.id[1] = GROVE_AI_FAMILY_SUB_ID;

    return ERROR_NONE;
}

CMD_STATE_T cmd_sys_read_version(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_sys_handler.version[0];
    write_buf[1] = g_sys_handler.version[1];
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_sys_read_id(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_sys_handler.id[0];
    write_buf[1] = g_sys_handler.id[1];
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_sys_read_state(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_sys_handler.state;
    _EXIT;
    return write_buf[0];
}

CMD_STATE_T cmd_sys_read_error_code(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
    write_buf[0] = g_error;
    _EXIT;
    return CMD_STATE_IDLE;
}

CMD_STATE_T cmd_sys_reset(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;
#ifdef EXTERNAL_LDO
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_EXTERNAL_LDO);
#else
    hx_lib_pm_chip_rst(PMU_WE1_POWERPLAN_INTERNAL_LDO);
#endif
    _EXIT;
    return CMD_STATE_IDLE;
}

const cmd_event_t sys_event_list[] = {
    {
        .cmd = CMD_SYS_READ_VERSION,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_SYS_VERSION_LENGTH,
        .cb = cmd_sys_read_version,
    },
    {
        .cmd = CMD_SYS_READ_ID,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_SYS_ID_LENGTH,
        .cb = cmd_sys_read_id,
    },
    {
        .cmd = CMD_SYS_READ_STATE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_SYS_STATE_LENGTH,
        .cb = cmd_sys_read_state,
    },
    {
        .cmd = CMD_SYS_READ_ERROR_CODE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_SYS_ERROR_CODE_LENGTH,
        .cb = cmd_sys_read_error_code,
    },
    {
        .cmd = CMD_SYS_RESET,
        .op = CMD_NONE,
        .check_busy = true,
        .cb = cmd_sys_reset,
    },
    {
        .cmd = 0xFF,
    },
};
