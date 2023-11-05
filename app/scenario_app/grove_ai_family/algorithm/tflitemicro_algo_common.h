/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      tflitemicro_algo_common.h
* @brief     算法模型部分公开类型定义
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-20
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef TFLITEMICRO_ALGO_COMMON__H
#define TFLITEMICRO_ALGO_COMMON__H

#define FORWARD_LIST_INSERT_RETRY_MAX_CNT 10

typedef enum
{
    ALGO_VISION = 0,
    ALGO_AUDIO = 1,
    ALGO_GYRO = 2,
    ALGO_TYPE_MAX
} ALGO_TYPE_T;

typedef enum
{
    ALGO_OBJECT_DETECTION = 0,
    ALGO_OBJECT_COUNT = 1,
    ALGO_IMAGE_CLASSIFICATION = 2,
    ALGO_POINTER_METER = 3,
    ALGO_DIGITAL_METER = 4,
    ALGO_MAX,
} ALGO_INDEX_T;

typedef enum
{
    MODEL_PRE_INDEX_1 = 0x00,
    MODEL_EXT_INDEX_1 = 0x01,
    MODEL_EXT_INDEX_2 = 0x02,
    MODEL_EXT_INDEX_3 = 0x03,
    MODEL_EXT_INDEX_4 = 0x04,
    MODEL_MAX = 0x20,
} MODEL_INDEX_T;

typedef struct
{
    uint8_t confidence;
    uint8_t iou;
} algo_filter_config_t;

typedef struct
{
    uint16_t width;
    uint16_t height;
} algo_vision_config_t;

typedef union
{
    algo_vision_config_t vision_config;
} algo_hardware_config_t;

typedef struct
{
    uint16_t start_x;
    uint16_t start_y;
    uint16_t end_x;
    uint16_t end_y;
    uint16_t center_x;
    uint16_t center_y;
    uint32_t from; // keep 3 point after decimal point   1.234 multiply 1000
    uint32_t to;   // keep 3 point after decimal point   1.234 multiply 1000
} algo_pointer_t;

typedef struct
{
    algo_pointer_t pointer;
} algo_software_config_t;

#endif