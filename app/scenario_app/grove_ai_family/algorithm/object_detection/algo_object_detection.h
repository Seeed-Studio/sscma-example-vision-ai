/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_object_detection.h
* @brief     目标检测算法模型
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-20
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef ALGO_OBJECT_DETECTION__H
#define ALGO_OBJECT_DETECTION__H

#include <stdint.h>

enum
{
    OBJECT_DETECTION_X_INDEX = 0,
    OBJECT_DETECTION_Y_INDEX = 1,
    OBJECT_DETECTION_W_INDEX = 2,
    OBJECT_DETECTION_H_INDEX = 3,
    OBJECT_DETECTION_C_INDEX = 4,
    OBJECT_DETECTION_T_INDEX = 5
};

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint8_t confidence;
    uint8_t target;
} object_detection_t;

#endif
