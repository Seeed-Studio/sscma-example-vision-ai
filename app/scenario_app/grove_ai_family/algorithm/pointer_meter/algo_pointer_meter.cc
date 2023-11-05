/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_poniter_meter.h
* @brief     指针表计算法模型
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-05-11
* @version   v1.1
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <math.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/kernels/micro_ops.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_error_reporter.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_interpreter.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/schema/schema_generated.h>

#include "grove_ai_config.h"
#include "logger.h"

#include "isp.h"

#include "tflitemicro_algo_common.h"
#include "tflitemicro_algo_private.h"

#include "algo_pointer_meter.h"
#include "cmd_algo_event.h"

#include "hx_drv_webusb.h"

#include <forward_list>

#define PI acos(-1)

#define IMG_PREVIEW_MAX_SIZE 16
#define IMAGE_PREIVEW_ELEMENT_NUM 2
#define IMAGE_PREIVEW_ELEMENT_SIZE 4
#define IMAGE_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d,\"count\":%d, \"object\":{\"x\": [%s],\"y\": [%s]}, \"value\":%d}"

#define METER_RESULT_MAX_SIZE 4

#define METER_INPUT_TYPE kTfLiteInt8
#define METER_INPUT_DIMS_SIZE 4

#define METER_OUTPUT_TYPE kTfLiteInt8
#define METER_OUTPUT_DIMS_SIZE 2

volatile static algo_filter_config_t _meter_config;
static point_t points[METER_RESULT_MAX_SIZE];
static int32_t value;

float get_value(float x, float y, float start_x, float start_y,
                float end_x, float end_y, float center_x, float center_y, float start_value, float end_value)
{
    /*
    pfld model post-precessiong, paras:
        (x, y): coordinate of pointer.
        (start_x, start_y): coordinate of starting point.
        (end_x, end_y): coordinate of ending point.
        (center_x, center_y): coordinate of center of circle.
        range: range from start point to end point.
    */
    float center_to_start, center_to_end, start_to_end; // The three sides of a triangle from center to start, then to end.
    float theta, theta1;
    float A, B, C, D;
    float start_to_pointer, center_to_pointer; // The two sides of start to pointer and center to pointer.
    float out;

    center_to_start = sqrt(pow(fabs(center_x - start_x), 2) + pow(fabs(center_y - start_y), 2));
    center_to_end = sqrt(pow(fabs(center_x - end_x), 2) + pow(fabs(center_y - end_y), 2));
    start_to_end = sqrt(pow(fabs(end_x - start_x), 2) + pow(fabs(end_y - start_y), 2));
    theta = acos((pow(center_to_start, 2) + pow(center_to_end, 2) - pow(start_to_end, 2)) / (2 * center_to_start * center_to_end)); // cosθ=(a^2 + b^2 - c^2) / 2ab
    theta = 2 * PI - theta;

    // determine center in which side of line from start to pointer.
    A = center_y - start_y;
    B = start_x - center_x;
    C = (center_x * start_y) - (start_x * center_y);
    D = A * x + B * y + C; // linear function: Ax+By+C=0.

    start_to_pointer = sqrt(pow(fabs(x - start_x), 2) + pow(fabs(y - start_y), 2));
    center_to_pointer = sqrt(pow(fabs(x - center_x), 2) + pow(fabs(y - center_y), 2));
    theta1 = acos((pow(center_to_start, 2) + pow(center_to_pointer, 2) - pow(start_to_pointer, 2)) / (2 * center_to_start * center_to_pointer));

    if (D < 0)
    {
        theta1 = 2 * PI - theta1;
    }

    if (theta1 > theta)
    {
        return -1;
    }
    else
    {
        out = (end_value - start_value) * (theta1 / theta) + start_value;
        if (out < start_value)
            return start_value;
        else if (out > end_value)
            return end_value;
        else
            return out;
    }
}

ALGO_ERROR_T algo_pointer_meter_init(tflitemicro_t *algo, const void *config)
{
    if (algo == nullptr || config == nullptr)
    {
        return ALGO_ERROR;
    }
    // check input format
    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    if (tensor_input->dims->size != METER_INPUT_DIMS_SIZE && tensor_input->type != METER_INPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    if (tensor_input->dims->data[0] != 1)
    {
        return ALGO_ERROR;
    }

    // check output format
    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    if (tensor_output->dims->size != METER_OUTPUT_DIMS_SIZE || tensor_output->type != METER_OUTPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    algo_filter_config_t *_config = (algo_filter_config_t *)config;

    uint16_t h = tensor_input->dims->data[1];
    uint16_t w = tensor_input->dims->data[2];
    algo->algo_type = ALGO_VISION;
    algo->hardware_config.vision_config.width = 240;
    algo->hardware_config.vision_config.height = 240;

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_pointer_meter_data_pre(const tflitemicro_t *algo, const uint8_t *data)
{
    if (algo == nullptr || data == nullptr)
    {
        return ALGO_ERROR;
    }
    TfLiteTensor *tensor_input = algo->interpreter->input(0);

    uint16_t h = tensor_input->dims->data[1];
    uint16_t w = tensor_input->dims->data[2];
    uint16_t c = tensor_input->dims->data[3];

    yuv422p2rgb(tensor_input->data.uint8, data, algo->hardware_config.vision_config.height, algo->hardware_config.vision_config.width, c, h, w, VISION_ROTATION);

    for (int i = 0; i < tensor_input->bytes; i++)
    {
        tensor_input->data.int8[i] -= 128;
    }

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_pointer_meter_data_post(const tflitemicro_t *algo, uint16_t *length)
{

    if (algo == nullptr || length == nullptr)
    {
        return ALGO_ERROR;
    }

    algo_pointer_t *config = (algo_pointer_t *)&g_algo_handler.config.pointer;
    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    TfLiteTensor *tensor_output = algo->interpreter->output(0);

    uint16_t h = tensor_input->dims->data[1];
    uint16_t w = tensor_input->dims->data[2];

    float scale = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->scale->data[0];
    int zero_point = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->zero_point->data[0];
    uint32_t records = tensor_output->bytes / 2;

    for (int i = 0; i < records; i++)
    {
        points[i].x = uint16_t(float(float(tensor_output->data.int8[i * 2] - zero_point) * scale) * algo->hardware_config.vision_config.width);
        points[i].y = uint16_t(float(float(tensor_output->data.int8[i * 2 + 1] - zero_point) * scale) * algo->hardware_config.vision_config.height);
    }
    if (records == 4)
    {
        config->start_x = points[2].x;
        config->start_y = points[2].y;
        config->end_x = points[3].x;
        config->end_y = points[3].y;
        config->center_x = points[0].x;
        config->center_y = points[0].y;

        value = (uint32_t)get_value(points[1].x, points[1].y, config->start_x, config->start_y, config->end_x, config->end_y, config->center_x, config->center_y, config->from, config->to);
    }
    else
    {

        value = (uint32_t)get_value(points[0].x, points[0].y, config->start_x, config->start_y, config->end_x, config->end_y, config->center_x, config->center_y, config->from, config->to);
    }
    *length = 1;

    LOGGER_INFO("x: %d, y: %d, value: %d\n", points[0].x, points[0].y, value);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_pointer_meter_get_data(const tflitemicro_t *algo, uint16_t index, uint8_t *data, uint16_t *length)
{

    if (algo == nullptr || data == nullptr || length == nullptr)
    {
        return ALGO_ERROR;
    }

    *length = sizeof(uint32_t);

    memcpy(data, &value, *length);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_pointer_meter_get_preview(const tflitemicro_t *algo, char *preview, uint16_t max_length)
{
    if (algo == nullptr || preview == nullptr)
    {
        return ALGO_ERROR;
    }

    uint16_t index = 0;
    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    // 获取目前结果集长度

    // 输入preview最多能有多少element
    uint16_t available_size = (max_length - sizeof(IMAGE_PREVIEW_FORMATE)) / (IMAGE_PREIVEW_ELEMENT_SIZE * IMAGE_PREIVEW_ELEMENT_NUM);

    if (available_size < 1)
    {
        return ALGO_ERROR;
    }

    // element数组
    char element[IMAGE_PREIVEW_ELEMENT_NUM][IMG_PREVIEW_MAX_SIZE * IMAGE_PREIVEW_ELEMENT_SIZE] = {0};

    // 生成element
    for (uint8_t i = 0; i < tensor_output->bytes / 2; i++)
    {
        if (index == 0)
        {
            snprintf(element[0], sizeof(element[0]), "%d", points[i].x);
            snprintf(element[1], sizeof(element[1]), "%d", points[i].y);
        }
        else
        {
            snprintf(element[0], sizeof(element[0]), "%s,%d", element[0], points[i].x);
            snprintf(element[1], sizeof(element[1]), "%s,%d", element[1], points[i].y);
        }
        index++;
        // 如果超过最大的可预览长度 则退出
        if (index > IMG_PREVIEW_MAX_SIZE || index > available_size)
        {
            break;
        }
    }

    // 规格化preview
    snprintf(preview, max_length, IMAGE_PREVIEW_FORMATE, algo->alog_index, algo->model_index, index, element[0], element[1], value);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_pointer_meter_deinit(const tflitemicro_t *algo)
{
    value = 0;
    return ALGO_ERROR_NONE;
}

tflitemicro_algo_t algo_pointer_meter = {
    .algo_init = algo_pointer_meter_init,
    .algo_data_pre = algo_pointer_meter_data_pre,
    .algo_data_post = algo_pointer_meter_data_post,
    .algo_get_result = algo_pointer_meter_get_data,
    .algo_get_preview = algo_pointer_meter_get_preview,
    .algo_deinit = algo_pointer_meter_deinit,
};