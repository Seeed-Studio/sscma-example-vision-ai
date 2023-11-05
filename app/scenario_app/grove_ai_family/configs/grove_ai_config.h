/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      grove_ai_config.h
* @brief     grove ai project configuration file
* @author    jian xiong (953308023@qq.com)
* @date      2022-04-24
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/
#ifndef GROVE_AI_CONFIG_H
#define GROVE_AI_CONFIG_H

#define GROVE_AI_FAMILY_MAIN_VER 0x02
#define GROVE_AI_FAMILY_SUB_VER 0x00

#define TENSOR_ARENA_SIZE 600 * 1024

#define SENSOR_WIDTH_DEFAULT 192
#define SENSOR_HEIGHT_DEFAULT 192

#define ALGO_CONFIG_ADDR 0x1FD000
#define ERROR_FILE_ADDR 0x1FF000

#define USE_WEBUSB
#define USE_I2C_SLAVE

#define I2C_SLAVE_ADDR 0x62

#define AT_SERVER_MAX_BUF_SIZE 512

#define LOGGER_LEVEL 3

#ifdef SENSECAP_AI
#define BOARD_NAME "SenseCAP A1101"
#include "./boards/lorawan_vision_ai_config.h"
#elif defined GROVE_VISION_AI
#define BOARD_NAME "Grove Vision AI"
#include "./boards/grove_vision_ai_config.h"
#else
error "you must specify the board."
#endif

#define IGNORE_FIRST_X_PICS 0

#endif
