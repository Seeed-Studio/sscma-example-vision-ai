/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      at_server.h
* @brief     处理AT命令入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-06-25
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef __AT_SERVER_H
#define __AT_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        const char *cmd;
        uint8_t cmd_len;
        int (*handler)(const char *param, size_t param_len);
    } at_cmd_t;

    void at_server_init(void);
    void at_server_start(void);
    void at_server_task(void);
    void at_server_reply(const char *fmt, ...);
    void at_server_lock(void);
    void at_server_unlock(void);

#ifdef __cplusplus
}
#endif

#endif