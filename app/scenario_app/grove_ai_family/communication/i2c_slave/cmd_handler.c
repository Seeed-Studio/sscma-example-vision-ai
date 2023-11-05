/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_handler.c
* @brief     处理i2c命令入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hx_drv_iomux.h"
#include "i2c_comm.h"
#include "grove_ai_config.h"
#include "logger.h"
#include "cmd_def.h"
#include "cmd_handler.h"
#include "cmd_sys_event.h"
#include "cmd_algo_event.h"
#include "cmd_gpio_event.h"
#include "cmd_imu_event.h"

/* Private variable -------------------------------------------------------*/
static uint8_t _i2c_read_buf[I2CCOMM_MAX_WBUF_SIZE] = {0};
static uint8_t _i2c_write_buf[I2CCOMM_MAX_RBUF_SIZE] = {0};
static CMD_HANDLER_STATE_T _i2c_cmd_state;

static feature_event_t feature_event_list[] = {
    {
        .feature = FEATURE_SYSTEM,
        .list = sys_event_list,
    },

    {
        .feature = FEATURE_ALGO,
        .list = algo_event_list,
    },
#ifdef CMD_IMU
    {
        .feature = FEATURE_IMU,
        .list = imu_event_list,
    },
#endif
#ifdef CMD_GPIO
    {
        .feature = FEATURE_GPIO,
        .list = gpio_event_list,
    },
#endif
    {
        .feature = FEATURE_INVAILD,
    }};

/* Global variable --------------------------------------------------------*/

/* Private function */
static void cmd_event_rx_cb(void *);
static void cmd_event_tx_cb(void *);
static void cmd_event_err_cb(void *);
static uint8_t cmd_handler_crc8(uint8_t *p, char counter);

/**
 * @brief  i2c event irq handle
 * @param  none
 * @retval none
 */
static void cmd_event_rx_cb(void *param)
{
    (void)param;

    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, 1 - I2C_SYNC_STATE);
    hx_drv_wdt_reset();
    uint8_t crc8 = 0;
    _ENTRY;

    CMD_STATE_T state;
    uint8_t feature = _i2c_read_buf[I2CFMT_FEATURE_OFFSET];
    uint8_t cmd = _i2c_read_buf[I2CFMT_COMMAND_OFFSET];

    for (int i = 0; feature_event_list[i].feature != FEATURE_INVAILD; i++)
    {
        if (feature == feature_event_list[i].feature)
        {
            cmd_event_t *event = feature_event_list[i].list;
            for (int j = 0; event->cmd != CMD_INVAILD; j++)
            {
                if (cmd == event->cmd)
                {
                    // 有一些指令需要检查是否忙， 如果忙则丢弃本次I2C 命令事件
                    if (event->check_busy && g_sys_handler.state == CMD_STATE_RUNNING)
                    {
                        LOGGER_WARNING("The system is busy. This command event is discarded");
                        goto end;
                    }

                    if (event->cb != NULL)
                    {
                        // LOGGER_INFO("Find OP: Feature: %02X Cmd: %02X\n", feature, cmd);
                        if (event->check_busy)
                        {
                            g_sys_handler.state = CMD_STATE_RUNNING;
                        }
                        if (event->op == CMD_READ)
                        {
                            memset(_i2c_write_buf, 0x00, sizeof(_i2c_write_buf));
                            state = event->cb(_i2c_read_buf, _i2c_write_buf);
// Generate crc8 to place at the end
#ifdef CMD_READ_CRC
                            crc8 = cmd_handler_crc8(_i2c_write_buf, event->length);
                            _i2c_write_buf[event->length] = crc8;
#endif
                            hx_lib_i2ccomm_enable_write(_i2c_write_buf);
                        }
                        else
                        {
                            if (event->op == CMD_WRITE)
                            {
// if crc error, discard this event
#ifdef CMD_WRITE_CRC
                                crc8 = cmd_handler_crc8(_i2c_read_buf, CMD_HEADER_LENGTH + event->length);
                                if (_i2c_read_buf[CMD_HEADER_LENGTH + event->length] != crc8)
                                {
                                    break;
                                }
#endif
                            }
                            state = event->cb(_i2c_read_buf, _i2c_write_buf);
                        }
                        if (event->check_busy)
                        {
                            g_sys_handler.state = state;
                        }
                    }
                    break;
                }
                event++;
            }
            break;
        }
    }

end:
    memset(_i2c_read_buf, 0xFF, CMD_HEADER_LENGTH);
    hx_lib_i2ccomm_enable_read(_i2c_read_buf, I2CCOMM_MAX_RBUF_SIZE);
    _EXIT;
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, I2C_SYNC_STATE);
    return;
}
static void cmd_event_tx_cb(void *param)
{
    (void)param;
    // LOGGER_INFO("\n%s\n", __FUNCTION__);
}
static void cmd_event_err_cb(void *param)
{
    (void)param;
    // OGGER_INFO("\n%s\n", __FUNCTION__);
    memset(_i2c_read_buf, 0xFF, CMD_HEADER_LENGTH);
    hx_lib_i2ccomm_enable_read(_i2c_read_buf, I2CCOMM_MAX_RBUF_SIZE);
    // hx_lib_i2ccomm_enable_write(_i2c_write_buf);
}

void cmd_handler_init()
{
    ERROR_T ret;
    ret = cmd_sys_event_init();
    if (ret != ERROR_NONE)
    {
        LOGGER_ERROR(ret, "Command initialization failed.\n");
    }

    ret = cmd_algo_event_init();
    if (ret != ERROR_NONE)
    {
        LOGGER_ERROR(ret, "Command initialization failed.\n");
    }
#ifdef CMD_GPIO
    ret = cmd_gpio_event_init();
    if (ret != ERROR_NONE)
    {
        LOGGER_ERROR(ret, "Command initialization failed.\n");
    }
#endif
#ifdef CMD_IMU
    ret = cmd_imu_event_init();
    if (ret != ERROR_NONE)
    {
        LOGGER_ERROR(ret, "Command initialization failed.\n");
    }
#endif

    memset(_i2c_read_buf, 0x00, sizeof(_i2c_read_buf));
    memset(_i2c_write_buf, 0x00, sizeof(_i2c_write_buf));

    I2CCOMM_CFG_T i2c_cfg;
    i2c_cfg.slv_addr = I2C_SLAVE_ADDR;
    i2c_cfg.read_cb = cmd_event_rx_cb;
    i2c_cfg.write_cb = NULL;           // cmd_event_tx_cb;
    i2c_cfg.err_cb = cmd_event_err_cb; // cmd_event_err_cb;
    hx_lib_i2ccomm_init(i2c_cfg);
    hx_drv_iomux_set_pmux(I2C_SYNC_PIN, 3);
}

void cmd_handler_start()
{
    hx_lib_i2ccomm_enable_write(_i2c_write_buf);
    hx_lib_i2ccomm_start(_i2c_read_buf, I2CCOMM_MAX_RBUF_SIZE);
    /*
    In order to make the E5 cat detect the finish of himax startup.
    we set the GPIO of i2c sync to unavailable first and then
    available.
    */
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, 1 - I2C_SYNC_STATE);
    board_delay_ms(5);
    hx_drv_iomux_set_outvalue(I2C_SYNC_PIN, I2C_SYNC_STATE);
}

void cmd_handler_task()
{
#ifdef CMD_IMU
    cmd_imu_event_task(NULL);
#endif
    cmd_algo_event_task(NULL);
}

void cmd_handler_lock()
{
    g_sys_handler.state = CMD_STATE_RUNNING;
}

void cmd_handler_unlock()
{
    g_sys_handler.state = CMD_STATE_IDLE;
}

// CRC8 x^8+x^5+x^4+x^0
const uint8_t CRC8Table[] = {
    0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
    157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
    35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
    190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
    70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
    219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
    101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
    248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
    140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
    17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
    175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
    50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
    202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
    87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
    233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
    116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

uint8_t cmd_handler_crc8(uint8_t *p, char counter)
{
    uint8_t crc8 = 0;

    for (; counter > 0; counter--)
    {
        crc8 = CRC8Table[crc8 ^ *p];
        p++;
    }
    return (crc8);
}