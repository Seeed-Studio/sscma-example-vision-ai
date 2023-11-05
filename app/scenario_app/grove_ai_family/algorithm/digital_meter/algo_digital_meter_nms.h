/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_object_count_nms.h
* @brief     目标计数算法nms
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-05-10
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef OBJECT_COUNT_NMS_H
#define OBJECT_COUNT_NMS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <forward_list>
#include <math.h>
#include "algo_object_count.h"

enum
{
    OBJECT_X_INDEX = 0,
    OBJECT_Y_INDEX = 1,
    OBJECT_W_INDEX = 2,
    OBJECT_H_INDEX = 3,
    OBJECT_C_INDEX = 4,
    OBJECT_T_INDEX = 5
};

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint8_t confidence;
    uint8_t target;
} object_t;

std::forward_list<object_t> nms_get_obeject_topn(int8_t *dataset, uint16_t top_n, uint8_t threshold, uint8_t nms, uint16_t width, uint16_t height, int num_record, int8_t num_class, float scale, int zero_point);

#endif
