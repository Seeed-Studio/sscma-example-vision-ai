/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      algo_image_classification.h
* @brief     图像分类算法模型
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-20
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

#include "algo_image_classification.h"

#include "hx_drv_webusb.h"

#include <forward_list>

#define IMG_PREVIEW_MAX_SIZE 10
#define IMAGE_PREIVEW_ELEMENT_NUM 2
#define IMAGE_PREIVEW_ELEMENT_SIZE 4
#define IMAGE_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d,\"count\":%d, \"object\":{\"target\": [%s],\"confidence\": [%s]}}"

#define IMAGE_CLASSIFICATION_RESULT_MAX_SIZE 1

#define IMAGE_CLASSIFICATION_INPUT_TYPE kTfLiteInt8
#define IMAGE_CLASSIFICATION_INPUT_DIMS_SIZE 4

#define IMAGE_CLASSIFICATION_OUTPUT_TYPE kTfLiteInt8
#define IMAGE_CLASSIFICATION_OUTPUT_DIMS_SIZE 1

volatile static algo_filter_config_t _image_classification_config;
static std::forward_list<image_classification_t> _image_classification_list;

static bool _image_classification_comparator(image_classification_t &oa, image_classification_t &ob)
{
    return oa.confidence > ob.confidence;
}

ALGO_ERROR_T algo_image_classification_init(tflitemicro_t *algo, const void *config)
{
    if (algo == nullptr || config == nullptr)
    {
        return ALGO_ERROR;
    }
    // check input format
    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    if (tensor_input->dims->size != IMAGE_CLASSIFICATION_INPUT_DIMS_SIZE && tensor_input->type != IMAGE_CLASSIFICATION_INPUT_TYPE)
    {
        return ALGO_ERROR;
    }

    if (tensor_input->dims->data[0] != 1)
    {
        return ALGO_ERROR;
    }

    // check output format
    TfLiteTensor *tensor_output = algo->interpreter->output(0);
    if (tensor_output->dims->size != IMAGE_CLASSIFICATION_OUTPUT_DIMS_SIZE || tensor_output->type != IMAGE_CLASSIFICATION_OUTPUT_TYPE)
    {
        return ALGO_ERROR;
    }
    algo_filter_config_t *_config = (algo_filter_config_t *)config;

    _image_classification_list.clear();
    _image_classification_config.confidence = _config->confidence;

    uint16_t h = tensor_input->dims->data[1];
    uint16_t w = tensor_input->dims->data[2];
    algo->algo_type = ALGO_VISION;
    algo->hardware_config.vision_config.width = w;
    algo->hardware_config.vision_config.height = h;

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_image_classification_data_pre(const tflitemicro_t *algo, const uint8_t *data)
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

ALGO_ERROR_T algo_image_classification_data_post(const tflitemicro_t *algo, uint16_t *length)
{

    if (algo == nullptr || length == nullptr)
    {
        return ALGO_ERROR;
    }

    _image_classification_list.clear();

    TfLiteTensor *tensor_input = algo->interpreter->input(0);
    TfLiteTensor *tensor_output = algo->interpreter->output(0);

    float scale = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->scale->data[0];
    int zero_point = ((TfLiteAffineQuantization *)(tensor_output->quantization.params))->zero_point->data[0];
    uint32_t records = tensor_output->dims->data[0];

    uint8_t target = 0;
    float max_confidence = float(tensor_output->data.int8[target] - zero_point) / float(127 - zero_point);
    for (uint32_t i = 0; i < records; i++)
    {
        float confidence = float(tensor_output->data.int8[i] - zero_point) / float(127 - zero_point);
        // LOGGER_WARNING("{target: %d value: %d confidence: %d}\n", i, tensor_output->data.int8[i], int(float(confidence) * 100));
        if (confidence < max_confidence)
        {
            continue;
        }
        max_confidence = confidence;
        target = i;
    }

    if (int(float(max_confidence) * 100) >= _image_classification_config.confidence)
    {
        image_classification_t obj;
        obj.target = target;
        obj.confidence = int(float(max_confidence) * 100);
        /*
        Add the retry here to avoid the insert fail of forward list
        */
        for (int i = 0; i < FORWARD_LIST_INSERT_RETRY_MAX_CNT; ++i)
        {
            _image_classification_list.emplace_front(obj);
            *length = std::distance(_image_classification_list.begin(), _image_classification_list.end());
            if (*length == 1)
            {
                break;
            }
        }
    }

    *length = std::distance(_image_classification_list.begin(), _image_classification_list.end());

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_image_classification_get_data(const tflitemicro_t *algo, uint16_t index, uint8_t *data, uint16_t *length)
{

    if (algo == nullptr || data == nullptr || length == nullptr)
    {
        return ALGO_ERROR;
    }

    uint32_t size = std::distance(_image_classification_list.begin(), _image_classification_list.end());

    if (index >= size || _image_classification_list.empty())
    {
        return ALGO_ERROR;
    }

    auto front = _image_classification_list.begin();
    std::advance(front, index);

    *length = sizeof(image_classification_t);
    image_classification_t obj;
    obj.confidence = front->confidence;
    obj.target = front->target;
    memcpy(data, &obj, *length);

    return ALGO_ERROR_NONE;
}

ALGO_ERROR_T algo_image_classification_get_preview(const tflitemicro_t *algo, char *preview, uint16_t max_length)
{
    if (algo == nullptr || preview == nullptr)
    {
        return ALGO_ERROR;
    }
    if (_image_classification_list.empty())
    {
        return ALGO_ERROR;
    }
    uint16_t index = 0;
    // 获取目前结果集长度
    uint16_t size = std::distance(_image_classification_list.begin(), _image_classification_list.end());

    // 输入preview最多能有多少element
    uint16_t available_size = (max_length - sizeof(IMAGE_PREVIEW_FORMATE)) / (IMAGE_PREIVEW_ELEMENT_SIZE * IMAGE_PREIVEW_ELEMENT_NUM);

    if (available_size < 1)
    {
        return ALGO_ERROR;
    }

    // element数组
    char element[IMAGE_PREIVEW_ELEMENT_NUM][IMG_PREVIEW_MAX_SIZE * IMAGE_PREIVEW_ELEMENT_SIZE] = {0};

    // 生成element
    for (auto it = _image_classification_list.begin(); it != _image_classification_list.end(); ++it)
    {
        if (index == 0)
        {
            snprintf(element[0], sizeof(element[0]), "%d", it->target);
            snprintf(element[1], sizeof(element[1]), "%d", it->confidence);
        }
        else
        {
            snprintf(element[0], sizeof(element[0]), "%s,%d", element[0], it->target);
            snprintf(element[1], sizeof(element[1]), "%s,%d", element[1], it->confidence);
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

ALGO_ERROR_T algo_image_classification_deinit(const tflitemicro_t *algo)
{
    _image_classification_list.clear();
    return ALGO_ERROR_NONE;
}

tflitemicro_algo_t algo_image_classification = {
    .algo_init = algo_image_classification_init,
    .algo_data_pre = algo_image_classification_data_pre,
    .algo_data_post = algo_image_classification_data_post,
    .algo_get_result = algo_image_classification_get_data,
    .algo_get_preview = algo_image_classification_get_preview,
    .algo_deinit = algo_image_classification_deinit,
};