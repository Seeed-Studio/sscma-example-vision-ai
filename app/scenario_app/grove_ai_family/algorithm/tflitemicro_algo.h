/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      tflitemicro_algo.h
* @brief     统一的算法模型入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-20
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef TFLITEMICRO_ALGO__H
#define TFLITEMICRO_ALGO__H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tflitemicro_algo_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    ERROR_T tflitemicro_algo_init();
    ERROR_T tflitemicro_algo_show();
    ERROR_T tflitemicro_algo_setup(ALGO_INDEX_T algo_index, MODEL_INDEX_T model_index, const algo_filter_config_t *config);
    ERROR_T tflitemicro_algo_invoke(const uint8_t *data, uint16_t *length);
    ERROR_T tflitemicro_algo_get_result(uint16_t index, uint8_t *data, uint16_t *length);
    ERROR_T tflitemicro_algo_get_preview(char *preview, uint16_t max_length);
    ERROR_T tflitemicro_algo_check_model(MODEL_INDEX_T model_index);
    uint32_t tflitemicro_algo_get_model();
    ERROR_T tflitemicro_algo_deinit();
    ALGO_TYPE_T tflitemicro_algo_type();
    ALGO_INDEX_T tflitemicro_algo_algo_index();
    MODEL_INDEX_T tflitemicro_algo_model_index();
    algo_hardware_config_t *tflitemicro_algo_harware_config();

#ifdef __cplusplus
}
#endif

#endif