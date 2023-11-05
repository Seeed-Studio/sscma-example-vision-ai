/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_image_classification.h
* @brief     图像分类算法
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-29
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef ALGO_IMAGE_CLASSIFICATION__H
#define ALGO_IMAGE_CLASSIFICATION__H

#include <stdint.h>
#include "tflitemicro_algo_common.h"

#define OBJ_MAX_SISE

typedef struct
{
    uint8_t confidence;
} algo_image_classification_config_t;

typedef struct
{
    uint8_t target;
    uint8_t confidence;
} image_classification_t;

#endif
