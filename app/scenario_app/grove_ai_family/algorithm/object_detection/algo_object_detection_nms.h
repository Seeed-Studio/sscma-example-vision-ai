/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_object_detection_nms.h
* @brief     目标检测算法nms
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-22
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#ifndef OBJECT_DETECTION_NMS_H
#define OBJECT_DETECTION_NMS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <forward_list>
#include <math.h>
#include "algo_object_detection.h"

std::forward_list<object_detection_t> nms_get_obeject_detection_topn(int8_t *dataset, uint16_t top_n, uint8_t threshold, uint8_t nms, uint16_t width, uint16_t height, int num_record, int8_t num_class, float scale, int zero_point);

#endif
