/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_pointer_meter.h
* @brief     指针仪表算法
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-05-11
* @version   v1.1
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef ALGO_POINTER_METER__H
#define ALGO_POINTER_METER__H

#include <stdint.h>
#include "tflitemicro_algo_common.h"

typedef struct
{
    uint16_t x;
    uint16_t y;
} point_t;

#endif
