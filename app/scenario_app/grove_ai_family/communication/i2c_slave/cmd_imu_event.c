/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_imu_event.c
* @brief     处理IMU相关事务
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-05-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "hardware_config.h"

#include "grove_ai_config.h"
#include "logger.h"

#include "imu_core.h"

#include "cmd_imu_event.h"

static uint8_t preview_str[256] = {0};

typedef struct
{
    bool sample;
    ERROR_T state;
    uint8_t acc_available;
    uint8_t gyro_available;
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} cmd_imu_event_t;

volatile cmd_imu_event_t _imu_handler;

ERROR_T cmd_imu_event_init()
{
    ERROR_T ret = ERROR_NONE;
    _imu_handler.sample = false;
    _imu_handler.acc_available = 0;
    _imu_handler.gyro_available = 0;
    _imu_handler.acc_x = 0;
    _imu_handler.acc_y = 0;
    _imu_handler.acc_z = 0;
    _imu_handler.gyro_x = 0;
    _imu_handler.gyro_x = 0;
    _imu_handler.gyro_x = 0;
    ret = imu_init();
    _imu_handler.state = ret;
    return ret;
}

ERROR_T cmd_imu_event_deinit()
{
    _imu_handler.sample = false;
    _imu_handler.acc_available = 0;
    _imu_handler.gyro_available = 0;
    _imu_handler.acc_x = 0;
    _imu_handler.acc_y = 0;
    _imu_handler.acc_z = 0;
    _imu_handler.gyro_x = 0;
    _imu_handler.gyro_x = 0;
    _imu_handler.gyro_x = 0;

    return ERROR_NONE;
}

void cmd_imu_event_task(void *args)
{
    static uint64_t tick = 0;
    if (_imu_handler.state == ERROR_NONE)
    {
        if (_imu_handler.sample)
        {
            if (imu_acc_available())
            {
                _imu_handler.acc_available = CMD_IMU_ACC_AVAILABLE;
                _imu_handler.acc_x = imu_get_acc_x();
                _imu_handler.acc_y = imu_get_acc_y();
                _imu_handler.acc_z = imu_get_acc_z();
            }
            else
            {
                _imu_handler.acc_available = 0x00;
            }
            if (imu_gyro_available())
            {
                _imu_handler.gyro_available = CMD_IMU_GYRO_AVAILABLE;
                _imu_handler.gyro_x = imu_get_gyro_x();
                _imu_handler.gyro_y = imu_get_gyro_y();
                _imu_handler.gyro_z = imu_get_gyro_z();
            }
            else
            {
                _imu_handler.gyro_available = 0x00;
            }
        }
    }
    else
    {
        tick++;
        if (tick > BOARD_PLL_CLK_400M / 100)
        {
            tick = 0;
            get_error(preview_str, _imu_handler.state, sizeof(preview_str));
            hx_drv_webusb_write_text((uint8_t *)preview_str, strlen(preview_str));
            LOGGER_INFO("%s\n", preview_str);
        }
    }
    return;
}

static CMD_STATE_T cmd_imu_read_sample_state(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    if (_imu_handler.sample)
    {
        write_buf[0] = CMD_IMU_SAMPLE_AVAILABLE;
    }
    else
    {
        write_buf[0] = 0;
    }

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_write_sample_state(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    if (read_buf[2] != 0)
    {
        _imu_handler.sample = true;
    }
    else
    {
        _imu_handler.sample = false;
    }

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_acc_available(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    write_buf[0] = _imu_handler.acc_available;

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_gyro_available(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    write_buf[0] = _imu_handler.gyro_available;

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_acc_x(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    memcpy(write_buf, (void *)&_imu_handler.acc_x, 4);

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_acc_y(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    memcpy(write_buf, (void *)&_imu_handler.acc_y, 4);

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_acc_z(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    memcpy(write_buf, (void *)&_imu_handler.acc_z, 4);

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_gyro_x(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    memcpy(write_buf, (void *)&_imu_handler.gyro_x, 4);

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_gyro_y(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    memcpy(write_buf, (void *)&_imu_handler.gyro_y, 4);

    _EXIT;

    return CMD_STATE_IDLE;
}

static CMD_STATE_T cmd_imu_read_gyro_z(uint8_t *read_buf, uint8_t *write_buf)
{
    _ENTRY;

    memcpy(write_buf, (void *)&_imu_handler.gyro_z, 4);

    _EXIT;

    return CMD_STATE_IDLE;
}

const cmd_event_t imu_event_list[] = {
    {
        .cmd = CMD_IMU_READ_SAMPLE_STATE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_SAMPLE_LENGTH,
        .cb = cmd_imu_read_sample_state,
    },
    {
        .cmd = CMD_IMU_WRITE_SAMPLE_STATE,
        .op = CMD_WRITE,
        .check_busy = false,
        .length = CMD_IMU_WRITE_SAMPLE_LENGTH,
        .cb = cmd_imu_write_sample_state,
    },
    {
        .cmd = CMD_IMU_READ_ACC_AVAIABLE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_ACC_AVAIABLE_LENGTH,
        .cb = cmd_imu_read_acc_available,
    },
    {
        .cmd = CMD_IMU_READ_ACC_X,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_ACC_X_LENGTH,
        .cb = cmd_imu_read_acc_x,
    },
    {
        .cmd = CMD_IMU_READ_ACC_Y,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_ACC_Y_LENGTH,
        .cb = cmd_imu_read_acc_y,
    },
    {
        .cmd = CMD_IMU_READ_ACC_Z,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_ACC_Z_LENGTH,
        .cb = cmd_imu_read_acc_z,
    },
    {
        .cmd = CMD_IMU_READ_GYRO_AVAIABLE,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_GYRO_AVAIABLE_LENGTH,
        .cb = cmd_imu_read_gyro_available,
    },
    {
        .cmd = CMD_IMU_READ_GYRO_X,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_GYRO_X_LENGTH,
        .cb = cmd_imu_read_gyro_x,
    },
    {
        .cmd = CMD_IMU_READ_GYRO_Y,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_GYRO_Y_LENGTH,
        .cb = cmd_imu_read_gyro_y,
    },
    {
        .cmd = CMD_IMU_READ_GYRO_Z,
        .op = CMD_READ,
        .check_busy = false,
        .length = CMD_IMU_READ_GYRO_Z_LENGTH,
        .cb = cmd_imu_read_gyro_z,
    },
    {
        .cmd = 0xFF,
    },
};