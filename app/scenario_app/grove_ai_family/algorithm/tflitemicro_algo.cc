/**
*****************************************************************************************
*     Copyright(c) 2022, Seeed Technology Corporation. All rights reserved.
*****************************************************************************************
* @file      tflitemicro_algo.c
* @brief     统一的算法模型入口
* @author    Hongtai Liu (lht856@foxmail.com)
* @date      2022-04-20
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2022 Seeed Technology Corporation</center></h2>
**************************************************************************************
*/

#include "grove_ai_config.h"
#include "logger.h"
#include "tflitemicro_algo_common.h"
#include "tflitemicro_algo_private.h"
#include "tflitemicro_algo.h"

#include "person_panda.h"
#define MODEL_MAX_SIZE 0x100000 // 每个model的最大size

#define RETRY_TIMES 10

/* 区分旧版以及新版模型文件， 旧版模型直接通过tflite转
 * 新版本模型通过head+tflie, 其中head=MODEL_MAGIC_NUM | INDEX
 * INDEX定义为1-4为自定义模型，其它为seeed保留模型
 * 目前 0x11 保留位 人体检测模型
 */
#define MODEL_MAGIC_NUM 0x4C485400

#define ADDR_MODEL_PRE_INDEX_1 _acperson_panda
#define ADDR_MODEL_EXT_INDEX_1 0x30000000
#define ADDR_MODEL_EXT_INDEX_2 0x30100000
#define ADDR_MODEL_EXT_INDEX_3 0x30200000
#define ADDR_MODEL_EXT_INDEX_4 0x30300000

uint8_t tensor_arena[TENSOR_ARENA_SIZE] __attribute__((aligned(16)));
static tflite::MicroMutableOpResolver<25> micro_op_resolver;

extern tflitemicro_algo_t algo_object_detection;
extern tflitemicro_algo_t algo_object_count;
extern tflitemicro_algo_t algo_image_classification;
extern tflitemicro_algo_t algo_pointer_meter;
extern tflitemicro_algo_t algo_digital_meter;

static tflitemicro_algo_t *_alog_list[] = {
    &algo_object_detection,
    &algo_object_count,
    &algo_image_classification,
    &algo_pointer_meter,
    &algo_digital_meter,
};

static tflitemicro_t _tflite_algo = {
    .alog_index = ALGO_MAX,
    .model_index = MODEL_MAX,
    .model = nullptr,
    .interpreter = nullptr,
};

extern "C" void DebugLog(const char *s) { LOGGER_INFO("%s", s); }

ERROR_T tflitemicro_algo_show()
{
    if (_tflite_algo.interpreter == nullptr)
        return ERROR_NONE;

    LOGGER_INFO("Algo info: \n");
    LOGGER_INFO("Algo type: %d\n{", _tflite_algo.algo_type);
    switch (_tflite_algo.algo_type)
    {
    case ALGO_VISION:
        LOGGER_INFO("\twidth: %d\n", _tflite_algo.hardware_config.vision_config.width);
        LOGGER_INFO("\theight: %d\n", _tflite_algo.hardware_config.vision_config.height);
        break;

    default:
        break;
    }
    LOGGER_INFO("}\n");
    LOGGER_INFO("input: {\n");
    for (int i = 0; i < _tflite_algo.interpreter->inputs_size(); i++)
    {
        TfLiteTensor *tensor = _tflite_algo.interpreter->input(i);
        LOGGER_INFO("\tinput[%d]:{\n", i);
        LOGGER_INFO("\t\ttype: %d\n", tensor->type);
        LOGGER_INFO("\t\tbytes: %d\n", tensor->bytes);
        LOGGER_INFO("\t\tdims: [ ");
        for (int j = 0; j < tensor->dims->size; j++)
        {
            LOGGER_INFO("%d, ", tensor->dims->data[j]);
        }
        LOGGER_INFO("]\n");
        TfLiteAffineQuantization *quantization = (TfLiteAffineQuantization *)(tensor->quantization.params);
        LOGGER_INFO("\t\tquantization: {\n");
        float scale = quantization->scale->data[i];
        LOGGER_INFO("\t\t\tscale * 1000000: %d\n", int((float)scale * 1000000));
        int zero_point = quantization->zero_point->data[i];
        LOGGER_INFO("\t\t\tzero_point: %d\n", zero_point);
        LOGGER_INFO("\t\t}\n");
        LOGGER_INFO("\t}\n");
    }
    LOGGER_INFO("}\n");

    LOGGER_INFO("output: {\n");
    for (int i = 0; i < _tflite_algo.interpreter->outputs_size(); i++)
    {
        TfLiteTensor *tensor = _tflite_algo.interpreter->output(i);
        LOGGER_INFO("\toutput[%d]:{\n", i);
        LOGGER_INFO("\t\ttype: %d\n", tensor->type);
        LOGGER_INFO("\t\tbytes: %d\n", tensor->bytes);
        LOGGER_INFO("\t\tdims: [ ");
        for (int j = 0; j < tensor->dims->size; j++)
        {
            LOGGER_INFO("%d, ", tensor->dims->data[j]);
        }
        LOGGER_INFO("]\n");
        TfLiteAffineQuantization *quantization = (TfLiteAffineQuantization *)(tensor->quantization.params);
        LOGGER_INFO("\t\tquantization: {\n");
        float scale = quantization->scale->data[i];
        LOGGER_INFO("\t\t\tscale * 1000000: %d\n", int((float)scale * 1000000));
        int zero_point = quantization->zero_point->data[i];
        LOGGER_INFO("\t\t\tzero_point: %d\n", zero_point);
        LOGGER_INFO("\t\t}\n");
        LOGGER_INFO("\t}\n");
    }
    LOGGER_INFO("}\n");

    return ERROR_NONE;
}

ERROR_T tflitemicro_algo_init()
{
    ERROR_T ret = ERROR_NONE;
    _tflite_algo.algo_type = ALGO_TYPE_MAX;
    _tflite_algo.alog_index = ALGO_MAX;
    _tflite_algo.model_index = MODEL_MAX;
    _tflite_algo.model = nullptr;
    _tflite_algo.interpreter = nullptr;

    TfLiteStatus status = kTfLiteOk;

    status = micro_op_resolver.AddConv2D();
    status = micro_op_resolver.AddReshape();
    status = micro_op_resolver.AddShape();
    status = micro_op_resolver.AddPack();
    status = micro_op_resolver.AddPad();
    status = micro_op_resolver.AddPadV2();
    status = micro_op_resolver.AddAdd();
    status = micro_op_resolver.AddSub();
    status = micro_op_resolver.AddRelu();
    status = micro_op_resolver.AddMaxPool2D();
    status = micro_op_resolver.AddSplit();
    status = micro_op_resolver.AddConcatenation();
    status = micro_op_resolver.AddResizeNearestNeighbor();
    status = micro_op_resolver.AddQuantize();
    status = micro_op_resolver.AddTranspose();
    status = micro_op_resolver.AddLogistic();
    status = micro_op_resolver.AddMul();
    status = micro_op_resolver.AddSplitV();
    status = micro_op_resolver.AddSplit();
    status = micro_op_resolver.AddStridedSlice();
    status = micro_op_resolver.AddMean();
    status = micro_op_resolver.AddFullyConnected();
    status = micro_op_resolver.AddSoftmax();
    status = micro_op_resolver.AddDepthwiseConv2D();
    status = micro_op_resolver.AddLeakyRelu();

    if (status != kTfLiteOk)
    {
        return ERROR_ALGO_INIT;
    }
    return ret;
}

ERROR_T tflitemicro_algo_setup(ALGO_INDEX_T algo_index, MODEL_INDEX_T model_index, const algo_filter_config_t *config)
{
    _ENTRY;

    ERROR_T ret = ERROR_NONE;
    uint8_t retry_times = RETRY_TIMES;
    uint32_t model_addr = 0;
    const tflite::Model *_model = nullptr;

    if (_tflite_algo.interpreter != nullptr)
    {
        ret = ERROR_ALGO_PARM_INVALID;
        return ret;
    }

    if (model_index >= MODEL_MAX || algo_index >= ALGO_MAX || config == NULL)
    {
        ret = ERROR_ALGO_PARM_INVALID;
        return ret;
    }

    if (_tflite_algo.model_index != model_index)
    {
        ret = ERROR_MODEL_INVALID;
        while (retry_times--)
        {
            if (model_index == MODEL_PRE_INDEX_1)
            {
                _model = ::tflite::GetModel((void *)ADDR_MODEL_PRE_INDEX_1);
            }
            else
            {
                if (model_index <= 4)
                { // custom model
                    model_addr = ADDR_MODEL_EXT_INDEX_1 + (model_index - 1) * MODEL_MAX_SIZE;
                }
                else
                { // seeed preset model
                    model_addr = ADDR_MODEL_EXT_INDEX_1 + (3 - (model_index - 1) % 4) * MODEL_MAX_SIZE;
                }

                // The new version of the model skips the header information
                if ((*(uint32_t *)model_addr & 0xFFFFFF00) == MODEL_MAGIC_NUM)
                {
                    model_addr += 4;
                }
                _model = ::tflite::GetModel((void *)model_addr);
            }
            if (_model->version() == TFLITE_SCHEMA_VERSION)
            {
                ret = ERROR_NONE;
                break;
            }
            board_delay_ms(10);
        }

        if (ERROR_NONE != ret)
        {
            return ret;
        }

        _tflite_algo.model_index = model_index;
        _tflite_algo.model = _model;
    }

    if (_tflite_algo.alog_index != algo_index)
    {
        _tflite_algo.alog_index = algo_index;
    }

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(_tflite_algo.model, micro_op_resolver,
                                                       tensor_arena, sizeof(tensor_arena),
                                                       &_tflite_algo.error_reporter);

    _tflite_algo.interpreter = &static_interpreter;
    if (_tflite_algo.interpreter->AllocateTensors() != kTfLiteOk)
    {
        ret = ERROR_MODEL_PARSE;
        _tflite_algo.interpreter = nullptr;
        return ret;
    }

    if (ALGO_ERROR_NONE != _alog_list[_tflite_algo.alog_index]->algo_init(&_tflite_algo, config))
    {
        ret = ERROR_ALGO_MISMATCH;
        return ret;
    }
    _EXIT;
    return ret;
}

ERROR_T tflitemicro_algo_invoke(const uint8_t *data, uint16_t *length)
{
    *length = 0;
    if (data == NULL || length == NULL)
    {
        return ERROR_ALGO_PARM_INVALID;
    }

    if (_alog_list[_tflite_algo.alog_index]->algo_data_pre(&_tflite_algo, data) != ALGO_ERROR_NONE)
    {
        return ERROR_DATA_PRE;
    }

    if (_tflite_algo.interpreter->Invoke() != kTfLiteOk)
    {
        return ERROR_ALGO_INVOKE;
    }

    if (_alog_list[_tflite_algo.alog_index]->algo_data_post(&_tflite_algo, length) != ALGO_ERROR_NONE)
    {
        return ERROR_DATA_POST;
    }

    return ERROR_NONE;
}

ERROR_T tflitemicro_algo_get_result(uint16_t index, uint8_t *data, uint16_t *length)
{
    if (data == NULL || length == NULL)
    {
        return ERROR_NONE;
    }

    if (_alog_list[_tflite_algo.alog_index]->algo_get_result(&_tflite_algo, index, data, length) != ALGO_ERROR_NONE)
    {
        return ERROR_ALGO_GET_RESULT;
    }

    return ERROR_NONE;
}

ERROR_T tflitemicro_algo_get_preview(char *preview, uint16_t max_length)
{
    if (preview == nullptr)
    {
        return ERROR_NONE;
    }
    memset(preview, 0, max_length);
    if (_alog_list[_tflite_algo.alog_index]->algo_get_preview(&_tflite_algo, preview, max_length) != ALGO_ERROR_NONE)
    {
        return ERROR_ALGO_GET_RESULT;
    }

    return ERROR_NONE;
}

ERROR_T tflitemicro_algo_deinit()
{
    return ERROR_NONE;
}

ERROR_T tflitemicro_algo_check_model(MODEL_INDEX_T model_index)
{
    ERROR_T ret = ERROR_NONE;
    uint32_t model_addr = 0;
    const tflite::Model *_model = nullptr;
    uint8_t retry_times = RETRY_TIMES;

    ret = ERROR_MODEL_INVALID;
    while (retry_times--)
    {
        if (model_index == MODEL_PRE_INDEX_1)
        {
            _model = ::tflite::GetModel((void *)ADDR_MODEL_PRE_INDEX_1);
        }
        else
        {
            if (model_index <= 4)
            { // custom model
                model_addr = ADDR_MODEL_EXT_INDEX_1 + (model_index - 1) * MODEL_MAX_SIZE;
            }
            else
            { // seeed preset model
                model_addr = ADDR_MODEL_EXT_INDEX_1 + (3 - (model_index - 1) % 4) * MODEL_MAX_SIZE;
            }
            // The new version of the model skips the header information
            if ((*(uint32_t *)model_addr & 0xFFFFFF00) == MODEL_MAGIC_NUM)
            {
                model_addr += 4;
            }
            _model = ::tflite::GetModel((void *)model_addr);
        }

        if (_model->version() == TFLITE_SCHEMA_VERSION)
        {
            ret = ERROR_NONE;
            break;
        }
        board_delay_ms(10);
    }

    return ret;
}

uint32_t tflitemicro_algo_get_model()
{
    uint32_t model = 1; // pre 1
    uint8_t index = 0;
    uint32_t model_addr = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        model_addr = ADDR_MODEL_EXT_INDEX_1 + i * MODEL_MAX_SIZE;
        index = i + 1;

        if ((*(uint32_t *)model_addr & 0xFFFFFF00) == MODEL_MAGIC_NUM)
        {
            index = (*(uint32_t *)model_addr) & 0xFF; // get index form model header
            model_addr += 4;
        }

        if (::tflite::GetModel((void *)model_addr)->version() == TFLITE_SCHEMA_VERSION)
        {
            model |= 1 << (index); // if model vaild, then set bit
        }
    }
    return model;
}

ALGO_TYPE_T tflitemicro_algo_type()
{
    return _tflite_algo.algo_type;
}

algo_hardware_config_t *tflitemicro_algo_harware_config()
{
    return &_tflite_algo.hardware_config;
}

ALGO_INDEX_T tflitemicro_algo_algo_index()
{
    return _tflite_algo.alog_index;
}
MODEL_INDEX_T tflitemicro_algo_model_index()
{
    return _tflite_algo.model_index;
}