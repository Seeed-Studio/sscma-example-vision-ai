/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_motion.h
* @brief
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-08-22
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef ALGO_METER__H
#define ALGO_METER__H

#ifdef __cplusplus
extern "C"
{
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "embARC_debug.h"
#include "hx_drv_pmu.h"
#include "powermode.h"
#endif

    typedef struct
    {
        uint8_t x;
        uint8_t y;
        uint8_t confidence;
        uint8_t target;
    } fomo_t;

    int tflitemicro_algo_init();
    int tflitemicro_algo_run(float *data);
    void tflitemicro_algo_exit();

#ifdef __cplusplus
}
#endif

#endif
