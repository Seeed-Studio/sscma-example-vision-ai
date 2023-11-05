/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      tflitemicro_algo_private.h
* @brief     算法模型私有相关结构定义
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-20
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef TFLITEMICRO_ALGO_PRIVATE__H
#define TFLITEMICRO_ALGO_PRIVATE__H

#include "tflitemicro_algo_common.h"

#include <library/cv/tflitemicro_25/tensorflow/lite/micro/kernels/micro_ops.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_error_reporter.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_interpreter.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/all_ops_resolver.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/schema/schema_generated.h>

typedef enum
{
    ALGO_ERROR_NONE = 0x00,
    ALGO_ERROR = 0x01
} ALGO_ERROR_T;

typedef struct
{
    ALGO_TYPE_T algo_type;
    ALGO_INDEX_T alog_index;
    MODEL_INDEX_T model_index;
    algo_hardware_config_t hardware_config;
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    tflite::MicroErrorReporter error_reporter;
    uint16_t result_length;
} tflitemicro_t;

typedef ALGO_ERROR_T (*tflitemicro_algo_init_cb_t)(tflitemicro_t *algo, const void *config);
typedef ALGO_ERROR_T (*tflitemicro_algo_pre_cb_t)(const tflitemicro_t *algo, const uint8_t *data);
typedef ALGO_ERROR_T (*tflitemicro_algo_post_cb_t)(const tflitemicro_t *algo, uint16_t *length);
typedef ALGO_ERROR_T (*tflitemicro_algo_get_result_cb_t)(const tflitemicro_t *algo, uint16_t index, uint8_t *data, uint16_t *length);
typedef ALGO_ERROR_T (*tflitemicro_algo_get_preview_cb_t)(const tflitemicro_t *algo, char *preview, uint16_t max_length);
typedef ALGO_ERROR_T (*tflitemicro_algo_deinit_cb_t)(const tflitemicro_t *algo);

typedef struct
{
    tflitemicro_algo_init_cb_t algo_init;
    tflitemicro_algo_pre_cb_t algo_data_pre;
    tflitemicro_algo_post_cb_t algo_data_post;
    tflitemicro_algo_get_result_cb_t algo_get_result;
    tflitemicro_algo_get_preview_cb_t algo_get_preview;
    tflitemicro_algo_deinit_cb_t algo_deinit;
} tflitemicro_algo_t;

#endif
