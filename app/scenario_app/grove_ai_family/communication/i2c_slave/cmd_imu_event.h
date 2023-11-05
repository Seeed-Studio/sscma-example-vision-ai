/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      cmd_gpio_event.h
* @brief     处理GPIO相关事务
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-05-19
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef __CMD_IMU_EVENT_H
#define __CMD_IMU_EVENT_H

#include <stdint.h>

#include "hx_drv_iomux.h"

#include "cmd_def.h"

/* Exported macros ---------------------------------------------------------*/
#define FEATURE_IMU 0x93

#define CMD_IMU_READ_SAMPLE_STATE 0x00
#define CMD_IMU_READ_SAMPLE_LENGTH 0x01
#define CMD_IMU_WRITE_SAMPLE_STATE 0x02
#define CMD_IMU_WRITE_SAMPLE_LENGTH 0x01
#define CMD_IMU_READ_ACC_AVAIABLE 0xA0
#define CMD_IMU_READ_ACC_AVAIABLE_LENGTH 0x01
#define CMD_IMU_READ_ACC_X 0xA1
#define CMD_IMU_READ_ACC_X_LENGTH 0x04
#define CMD_IMU_READ_ACC_Y 0xA2
#define CMD_IMU_READ_ACC_Y_LENGTH 0x04
#define CMD_IMU_READ_ACC_Z 0xA3
#define CMD_IMU_READ_ACC_Z_LENGTH 0x04
#define CMD_IMU_READ_GYRO_AVAIABLE 0xB0
#define CMD_IMU_READ_GYRO_AVAIABLE_LENGTH 0x01
#define CMD_IMU_READ_GYRO_X 0xB1
#define CMD_IMU_READ_GYRO_X_LENGTH 0x04
#define CMD_IMU_READ_GYRO_Y 0xB2
#define CMD_IMU_READ_GYRO_Y_LENGTH 0x04
#define CMD_IMU_READ_GYRO_Z 0xB3
#define CMD_IMU_READ_GYRO_Z_LENGTH 0x04

#define CMD_IMU_SAMPLE_AVAILABLE 0x01
#define CMD_IMU_ACC_AVAILABLE 0x01
#define CMD_IMU_GYRO_AVAILABLE 0x01

extern const cmd_event_t imu_event_list[];

ERROR_T cmd_imu_event_init(void);
void cmd_imu_event_task(void *args);

#endif