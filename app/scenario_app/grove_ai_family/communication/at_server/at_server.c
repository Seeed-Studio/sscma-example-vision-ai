/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      at_server.c
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
#include "hx_drv_iomux.h"
#include "hx_drv_wdt.h"
#include "hx_drv_webusb.h"
#include "console_io.h"
#include "powermode.h"
#include "i2c_comm.h"
#include "grove_ai_config.h"
#include "logger.h"
#include "at_server.h"
#include "at_handler.h"

static DEV_UART *console_uart = NULL;
static bool _at_server_start = false;
static char _at_server_buf[AT_SERVER_MAX_BUF_SIZE] = {0};
static char _at_response_buf[AT_SERVER_MAX_BUF_SIZE] = {0};
static uint32_t _at_server_buf_len = 0;

#define AT_RESPONSE_FORMATE "\r\n{\"type\":\"AT\", \"data\":\"%s\"}\r\n"

size_t find_first_of(const char *str, const char *charset)
{
    size_t i = 0;
    while (str[i] != '\0')
    {
        size_t j = 0;
        while (charset[j] != '\0')
        {
            if (str[i] == charset[j])
            {
                return i;
            }
            j++;
        }
        i++;
    }
    return -1;
}

void at_server_init(void)
{
    at_server_lock();
    console_uart = hx_drv_uart_get_dev((USE_SS_UART_E)CONSOLE_UART_ID);
    memset(_at_server_buf, 0, AT_SERVER_MAX_BUF_SIZE);
    _at_server_buf_len = 0;
    at_server_unlock();
}
void at_server_start(void)
{
    at_server_lock();
    _at_server_start = true;
    at_server_reply(AT_RESPONSE_OK);
    at_server_unlock();
}

int at_server_cmd_handler(const char *cmd, size_t cmd_len, const char *param, size_t param_len)
{
    int ret = -1;
    if (cmd == NULL || cmd_len == 0)
    {
        ret = -1;
        goto exit;
    }
    for (size_t i = 0; at_cmd_table[i].cmd != NULL; i++)
    {
        if (strncmp(at_cmd_table[i].cmd, cmd, cmd_len) == 0)
        {
            if (at_cmd_table[i].cmd_len > cmd_len)
            {
                continue;
            }
            ret = at_cmd_table[i].handler(param, param_len);
            break;
        }
    }

exit:
    if (ret == 1)
    {
        at_server_reply(AT_RESPONSE_OK);
    }
    else if (ret == -1)
    {
        at_server_reply(AT_RESPONSE_ERROR);
    }

    return -1;
}

void at_server_task(void)
{
    char c;
    size_t pos = 0;
    at_server_lock();
    if (_at_server_start)
    {
        while (console_uart->uart_read_nonblock((void *)&c, 1) != 0)
        {
            if (c == '\r' || c == '\n')
            {
                if (_at_server_buf_len > 0)
                {

                    pos = find_first_of(_at_server_buf, "?=");

                    if (pos != -1)
                    {

                        if (_at_server_buf[pos] == '?')
                        {
                            // AT+xxx?
                            _at_server_buf[pos] = '\0';
                            at_server_unlock();
                            at_server_cmd_handler(_at_server_buf, _at_server_buf_len, _at_server_buf + pos + 1, 0);
                            at_server_lock();
                        }
                        else if (_at_server_buf[pos] == '=')
                        {
                            // AT+xxx=xxx
                            _at_server_buf[pos] = '\0';
                            at_server_unlock();
                            at_server_cmd_handler(_at_server_buf, _at_server_buf_len, _at_server_buf + pos + 1, 1);
                            at_server_lock();
                        }
                    }
                    else
                    {
                        // AT+xxx
                        at_server_unlock();
                        at_server_cmd_handler(_at_server_buf, _at_server_buf_len, NULL, 0);
                        at_server_lock();
                    }

                    memset(_at_server_buf, 0, AT_SERVER_MAX_BUF_SIZE);
                    _at_server_buf_len = 0;
                }
            }
            else
            {
                if (_at_server_buf_len < AT_SERVER_MAX_BUF_SIZE)
                {
                    _at_server_buf[_at_server_buf_len++] = c;
                }
                else
                {
                    LOGGER_INFO("AT command too long\r\n");
                    memset(_at_server_buf, 0, AT_SERVER_MAX_BUF_SIZE);
                    _at_server_buf_len = 0;
                }
            }
        }
    }
    at_server_unlock();
}
void at_server_lock(void)
{
    hx_drv_wdt_feed();
}
void at_server_unlock(void)
{
    hx_drv_wdt_feed();
}

void at_server_reply(const char *fmt, ...)
{
    char print_buf[256] = {0};

    if (console_uart == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    int r = vsnprintf(print_buf, sizeof(print_buf), fmt, args);
    va_end(args);

    if (r > 0)
    {
        snprintf(_at_response_buf, sizeof(_at_response_buf), AT_RESPONSE_FORMATE, print_buf);
        console_uart->uart_write(_at_response_buf, strlen(_at_response_buf));
        hx_drv_webusb_write_text(_at_response_buf, strlen(_at_response_buf));
    }
}
