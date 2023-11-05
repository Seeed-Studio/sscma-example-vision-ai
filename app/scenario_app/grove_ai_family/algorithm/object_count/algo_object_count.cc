/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_object_count.h
* @brief     目标检测算法模型
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-05-10
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
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

#include "tflitemicro_algo_common.h"
#include "tflitemicro_algo_private.h"

#include "algo_object_count.h"
#include "algo_object_count_nms.h"

#define IMG_PREVIEW_MAX_SIZE 10
#define IMAGE_PREIVEW_ELEMENT_NUM 2
#define IMAGE_PREIVEW_ELEMENT_SIZE 4
#define IMAGE_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d,\"count\":%d, \"object\":{\"target\": [%s],\"count\": [%s]}}"

#define OBJECT_DETECTION_INPUT_TYPE kTfLiteInt8
#define OBJECT_DETECTION_INPUT_DIMS_SIZE 4

#define OBJECT_DETECTION_OUTPUT_TYPE kTfLiteInt8
#define OBJECT_DETECTION_OUTPUT_DIMS_SIZE 3

volatile static algo_filter_config_t _object_count_config;
static std::forward_list<object_detection_t> _object_dection_list;
static std::forward_list<object_count_t> _object_count_list;

ALGO_ERROR_T algo_objection_count_init(tflitemicro_t *algo, const void *config)
{
    if (algo == nullptr || config == nullptr)
    {
        return ALGO_ERROR;
    }
    // check input format
    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    if (tensor_input->dims->size != OBJECT_DETECTION_INPUT_DIMS_SIZE && tensor_input->type != OBJECT_DETECTION_INPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    if (tensor_input->dims->data[0] != 1)
    {
        return ALGO_ERROR;
    }

    // check output format
    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    if (tensor_output->dims->size != OBJECT_DETECTION_OUTPUT_DIMS_SIZE || tensor_output->type != OBJECT_DETECTION_OUTPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    if (tensor_output->dims->data[0] != 1)
    {
        return ALGO_ERROR;
    }

    algo_filter_config_t *_config = (algo_filter_config_t *)config;

    _object_count_config.confidence = _config->confidence;
    _object_count_config.iou = _config->iou;

    _object_dection_list.clear();
    _object_count_list.clear();

    uint16_t w = tensor_input->dims->data[1];
    uint16_t h = tensor_input->dims->data[2];
    algo->algo_type = ALGO_VISION;
    algo->hardware_config.vision_config.width = w;
    algo->hardware_config.vision_config.height = h;

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_objection_count_data_pre(const tflitemicro_t *algo, const uint8_t *data)
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
    for (int i = 0; i < tensor_input->bytes; i++)
    {
        tensor_input->data.int8[i] -= 128;
    }

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_objection_count_data_post(const tflitemicro_t *algo, uint16_t *length)
{

    _object_dection_list.clear();
    _object_count_list.clear();

    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    uint16_t w = tensor_input->dims->data[1];
    uint16_t h = tensor_input->dims->data[2];

    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    uint32_t records = tensor_output->dims->data[1];
    uint32_t num_class = tensor_output->dims->data[2] - 5;

    float scale = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->scale->data[0];
    int zero_point = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->zero_point->data[0];

#if 0
    for (int i = 0; i < records; i++)
    {
        float confidence = float(tensor_output->data.int8[i * num_class + 9] - zero_point) * scale;
        if (confidence > .1)
        {
            int8_t max = -128;
            int target = 0;
            for (int j = 0; j < 20; j++)
            {
                if (max < tensor_output->data.int8[i * 25 + 5 + j])
                {
                    max = tensor_output->data.int8[i * 25 + 5 + j];
                    target = j;
                }
            }
            int x = int(float(float(tensor_output->data.int8[i * 25 + 0] - zero_point) * scale) * 192);
            int y = int(float(float(tensor_output->data.int8[i * 25 + 1] - zero_point) * scale) * 192);
            int w = int(float(float(tensor_output->data.int8[i * 25 + 2] - zero_point) * scale) * 192);
            int h = int(float(float(tensor_output->data.int8[i * 25 + 3] - zero_point) * scale) * 192);

            LOGGER_INFO("index: %d target: %d max: %d confidence: %d box{x: %d, y: %d, w: %d, h: %d}\n", i, target, max, int((float)confidence * 100), x, y, w, h);
        }
    }
#endif

    _object_dection_list = nms_get_obeject_count_topn(tensor_output->data.int8, records, _object_count_config.confidence, _object_count_config.iou, w, h, records, num_class, scale, zero_point);
    for (std::forward_list<object_detection_t>::iterator detection_it = _object_dection_list.begin(); detection_it != _object_dection_list.end(); ++detection_it)
    {
        uint8_t hit = 0;
        for (std::forward_list<object_count_t>::iterator count_it = _object_count_list.begin(); count_it != _object_count_list.end(); ++count_it)
        {
            if (count_it->target == detection_it->target)
            {
                hit = 1;
                count_it->count++;
            }
        }
        if (hit == 0)
        {
            object_count_t obj;
            obj.target = detection_it->target;
            obj.count = 1;
            _object_count_list.emplace_front(obj);
        }
    }
    *length = std::distance(_object_count_list.begin(), _object_count_list.end());

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_objection_count_get_data(const tflitemicro_t *algo, uint16_t index, uint8_t *data, uint16_t *length)
{
    if (data == NULL || length == NULL)
    {
        return ALGO_ERROR;
    }
    uint32_t size = std::distance(_object_count_list.begin(), _object_count_list.end());

    if (index >= size || _object_count_list.empty())
    {
        return ALGO_ERROR;
    }

    auto front = _object_count_list.begin();
    std::advance(front, index);

    *length = sizeof(object_count_t);
    object_count_t obj;
    obj.count = front->count;
    obj.target = front->target;
    memcpy(data, &obj, *length);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_object_count_get_preview(const tflitemicro_t *algo, char *preview, uint16_t max_length)
{
    if (algo == nullptr || preview == nullptr)
    {
        return ALGO_ERROR;
    }
    if (_object_count_list.empty())
    {
        return ALGO_ERROR;
    }
    uint16_t index = 0;
    // 获取目前结果集长度
    uint16_t size = std::distance(_object_count_list.begin(), _object_count_list.end());

    // 输入preview最多能有多少element
    uint16_t available_size = (max_length - sizeof(IMAGE_PREVIEW_FORMATE)) / (IMAGE_PREIVEW_ELEMENT_SIZE * IMAGE_PREIVEW_ELEMENT_NUM);

    if (available_size < 1)
    {
        return ALGO_ERROR;
    }

    // element数组
    char element[IMAGE_PREIVEW_ELEMENT_NUM][IMG_PREVIEW_MAX_SIZE * IMAGE_PREIVEW_ELEMENT_SIZE] = {0};

    // 生成element
    for (auto it = _object_count_list.begin(); it != _object_count_list.end(); ++it)
    {
        if (index == 0)
        {
            snprintf(element[0], sizeof(element[0]), "%d", it->target);
            snprintf(element[1], sizeof(element[1]), "%d", it->count);
        }
        else
        {
            snprintf(element[0], sizeof(element[0]), "%s,%d", element[0], it->target);
            snprintf(element[1], sizeof(element[1]), "%s,%d", element[1], it->count);
        }
        index++;
        // 如果超过最大的可预览长度 则退出
        if (index > IMG_PREVIEW_MAX_SIZE || index > available_size)
        {
            break;
        }
    }

    // 规格化preview
    snprintf(preview, max_length, IMAGE_PREVIEW_FORMATE, algo->alog_index, algo->model_index, size, element[0], element[1]);
    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_objection_count_deinit(const tflitemicro_t *algo)
{
    _object_dection_list.clear();
    _object_count_list.clear();
    return ALGO_ERROR_NONE;
}

tflitemicro_algo_t algo_object_count = {
    .algo_init = algo_objection_count_init,
    .algo_data_pre = algo_objection_count_data_pre,
    .algo_data_post = algo_objection_count_data_post,
    .algo_get_result = algo_objection_count_get_data,
    .algo_get_preview = algo_object_count_get_preview,
    .algo_deinit = algo_objection_count_deinit,
};