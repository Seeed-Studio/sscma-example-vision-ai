/**
*****************************************************************************************
*     Copyright(c) 2023, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_digital_meter.h
* @brief     数字表计算法模型
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2023-05-11
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2023 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include <library/cv/tflitemicro_25/tensorflow/lite/micro/kernels/micro_ops.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_error_reporter.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_interpreter.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <library/cv/tflitemicro_25/tensorflow/lite/schema/schema_generated.h>

#include "grove_ai_config.h"
#include "logger.h"

#include "isp.h"
#include "hx_drv_webusb.h"

#include "tflitemicro_algo_common.h"
#include "tflitemicro_algo_private.h"

#include "algo_digital_meter.h"
#include "algo_digital_meter_nms.h"

#define IMG_PREVIEW_MAX_SIZE 20
#define IMAGE_PREIVEW_ELEMENT_NUM 6
#define IMAGE_PREIVEW_ELEMENT_SIZE 4
#define IMAGE_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d, \"count\":%d, \"object\":{\"x\": [%s],\"y\": [%s],\"w\": [%s],\"h\": [%s],\"target\": [%s],\"confidence\": [%s]}, \"value\":%d}"

#define OBJECT_INPUT_TYPE kTfLiteInt8
#define OBJECT_INPUT_DIMS_SIZE 4

#define OBJECT_OUTPUT_TYPE kTfLiteInt8
#define OBJECT_OUTPUT_DIMS_SIZE 3

volatile static algo_filter_config_t _digital_meter_config;
static std::forward_list<object_t> _digital_meter_list;
static int32_t value = 0;

ALGO_ERROR_T algo_digital_meter_init(tflitemicro_t *algo, const void *config)
{
    if (algo == nullptr || config == nullptr)
    {
        return ALGO_ERROR;
    }
    // check input format
    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    if (tensor_input->dims->size != OBJECT_INPUT_DIMS_SIZE && tensor_input->type != OBJECT_INPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    if (tensor_input->dims->data[0] != 1)
    {
        return ALGO_ERROR;
    }

    // check output format
    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    if (tensor_output->dims->size != OBJECT_OUTPUT_DIMS_SIZE || tensor_output->type != OBJECT_OUTPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    if (tensor_output->dims->data[0] != 1)
    {
        return ALGO_ERROR;
    }

    algo_filter_config_t *_config = (algo_filter_config_t *)config;

    _digital_meter_config.confidence = _config->confidence;
    _digital_meter_config.iou = _config->iou;

    _digital_meter_list.clear();

    uint16_t w = tensor_input->dims->data[1];
    uint16_t h = tensor_input->dims->data[2];
    algo->algo_type = ALGO_VISION;
    algo->hardware_config.vision_config.width = w;
    algo->hardware_config.vision_config.height = h;

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_digital_meter_data_pre(const tflitemicro_t *algo, const uint8_t *data)
{
    if (algo == nullptr || data == nullptr)
    {
        return ALGO_ERROR;
    }
    TfLiteTensor *tensor_input = algo->interpreter->input(0);

    uint16_t w = tensor_input->dims->data[1];
    uint16_t h = tensor_input->dims->data[2];
    uint16_t c = tensor_input->dims->data[3];

    yuv422p2rgb(tensor_input->data.uint8, data, algo->hardware_config.vision_config.height, algo->hardware_config.vision_config.width, c, h, w, VISION_ROTATION);
    // 将rgb 图像 发送到 webusb
    // hx_drv_webusb_write_vision(tensor_input->data.uint8, h * w * c);
    for (int i = 0; i < tensor_input->bytes; i++)
    {
        tensor_input->data.int8[i] -= 128;
    }

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_digital_meter_data_post(const tflitemicro_t *algo, uint16_t *length)
{
    _digital_meter_list.clear();

    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    uint16_t weight = tensor_input->dims->data[1];
    uint16_t height = tensor_input->dims->data[2];

    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    uint32_t records = tensor_output->dims->data[1];
    uint32_t num_class = tensor_output->dims->data[2] - OBJECT_T_INDEX;
    int16_t num_element = num_class + OBJECT_T_INDEX;

    float scale = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->scale->data[0];
    int zero_point = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->zero_point->data[0];
    uint32_t tmp_value = 0;
    uint32_t tmp_place = 0;

#if 0
    for (int i = 0; i < records; i++)
    {
        float confidence = float(tensor_output->data.int8[i * num_element + OBJECT_C_INDEX] - zero_point) * scale;
        if (confidence > .50)
        {
            int8_t max = -128;
            int target = 0;
            for (int j = 0; j < num_class; j++)
            {
                if (max < tensor_output->data.int8[i * num_element + OBJECT_T_INDEX + j])
                {
                    max = tensor_output->data.int8[i * num_element + OBJECT_T_INDEX + j];
                    target = j;
                }
            }
            int x = int(float(float(tensor_output->data.int8[i * num_element + OBJECT_X_INDEX] - zero_point) * scale) * weight);
            int y = int(float(float(tensor_output->data.int8[i * num_element + OBJECT_Y_INDEX] - zero_point) * scale) * height);
            int w = int(float(float(tensor_output->data.int8[i * num_element + OBJECT_W_INDEX] - zero_point) * scale) * weight);
            int h = int(float(float(tensor_output->data.int8[i * num_element + OBJECT_H_INDEX] - zero_point) * scale) * height);

            LOGGER_INFO("index: %d target: %d max: %d confidence: %d box{x: %d, y: %d, w: %d, h: %d}\n", i, target, max, int((float)confidence * 100), x, y, w, h);
        }
    }
#endif
    _digital_meter_list = nms_get_obeject_topn(tensor_output->data.int8, records, _digital_meter_config.confidence, 20, weight, height, records, num_class, scale, zero_point);

    tmp_value = 0;
    tmp_place = 1;
    for (auto &obj : _digital_meter_list)
    {

        obj.x = int(float(float(obj.x) / weight) * algo->hardware_config.vision_config.width);
        obj.y = int(float(float(obj.y) / height) * algo->hardware_config.vision_config.height);
        obj.w = int(float(float(obj.w) / weight) * algo->hardware_config.vision_config.width);
        obj.h = int(float(float(obj.h) / height) * algo->hardware_config.vision_config.height);

        if (obj.target > 9) // use old value
        {
            tmp_value = tmp_value * 10 + ((value / tmp_place) % 10);
            continue;
        }
        tmp_value = tmp_value * 10 + obj.target;
        tmp_place *= 10;
    }

    value = tmp_value * 1000;

    *length = 1; // sizeof(uint32_t);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_digital_meter_get_data(const tflitemicro_t *algo, uint16_t index, uint8_t *data, uint16_t *length)
{
    if (data == NULL || length == NULL)
    {
        return ALGO_ERROR;
    }

    if (index > 1)
    {
        return ALGO_ERROR;
    }

    *length = sizeof(value);
    memcpy(data, &value, sizeof(value));

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_digital_meter_get_preview(const tflitemicro_t *algo, char *preview, uint16_t max_length)
{
    if (algo == nullptr || preview == nullptr)
    {
        return ALGO_ERROR;
    }
    if (_digital_meter_list.empty())
    {
        return ALGO_ERROR;
    }
    uint16_t index = 0;
    // 获取目前结果集长度
    uint16_t size = std::distance(_digital_meter_list.begin(), _digital_meter_list.end());

    // 输入preview最多能有多少element
    uint16_t available_size = (max_length - sizeof(IMAGE_PREVIEW_FORMATE)) / (IMAGE_PREIVEW_ELEMENT_SIZE * IMAGE_PREIVEW_ELEMENT_NUM);

    if (available_size < 1)
    {
        return ALGO_ERROR;
    }

    // element数组
    char element[IMAGE_PREIVEW_ELEMENT_NUM][IMG_PREVIEW_MAX_SIZE * IMAGE_PREIVEW_ELEMENT_SIZE] = {0};

    // 生成element
    for (auto it = _digital_meter_list.begin(); it != _digital_meter_list.end(); ++it)
    {
        if (index == 0)
        {
            snprintf(element[0], sizeof(element[0]), "%d", it->x);
            snprintf(element[1], sizeof(element[1]), "%d", it->y);
            snprintf(element[2], sizeof(element[2]), "%d", it->w);
            snprintf(element[3], sizeof(element[3]), "%d", it->h);
            snprintf(element[4], sizeof(element[4]), "%d", it->target);
            snprintf(element[5], sizeof(element[5]), "%d", it->confidence);
        }
        else
        {
            snprintf(element[0], sizeof(element[0]), "%s,%d", element[0], it->x);
            snprintf(element[1], sizeof(element[1]), "%s,%d", element[1], it->y);
            snprintf(element[2], sizeof(element[2]), "%s,%d", element[2], it->w);
            snprintf(element[3], sizeof(element[3]), "%s,%d", element[3], it->h);
            snprintf(element[4], sizeof(element[4]), "%s,%d", element[4], it->target);
            snprintf(element[5], sizeof(element[5]), "%s,%d", element[5], it->confidence);
        }
        index++;
        // 如果超过最大的可预览长度 则退出
        if (index > IMG_PREVIEW_MAX_SIZE || index > available_size)
        {
            break;
        }
    }

    // 规格化preview
    snprintf(preview, max_length, IMAGE_PREVIEW_FORMATE, algo->alog_index, algo->model_index, size, element[0], element[1], element[2], element[3], element[4], element[5], value);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_digital_meter_deinit(const tflitemicro_t *algo)
{
    _digital_meter_list.clear();
    return ALGO_ERROR_NONE;
}

tflitemicro_algo_t algo_digital_meter = {
    .algo_init = algo_digital_meter_init,
    .algo_data_pre = algo_digital_meter_data_pre,
    .algo_data_post = algo_digital_meter_data_post,
    .algo_get_result = algo_digital_meter_get_data,
    .algo_get_preview = algo_digital_meter_get_preview,
    .algo_deinit = algo_digital_meter_deinit,
};