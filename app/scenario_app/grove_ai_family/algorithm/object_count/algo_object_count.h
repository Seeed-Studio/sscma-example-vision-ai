/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_object_count.h
* @brief     目标技术算法模型
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-05-10
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef ALGO_OBJECT_COUNT__H
#define ALGO_OBJECT_COUNT__H

#include <stdint.h>

typedef struct
{
    uint8_t target;
    uint8_t count;
} object_count_t;

#endif
