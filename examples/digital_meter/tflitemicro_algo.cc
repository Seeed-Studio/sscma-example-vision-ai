/*
 * tflitemicro_algo.cc
 *
 *  Created on: 20220922
 *      Author: 902453
 */
#include <math.h>
#include <stdint.h>
#include <forward_list>
#include "grove_ai_config.h"
#include "isp.h"
#include "hx_drv_webusb.h"
#include "embARC_debug.h"

#include "tflitemicro_algo.h"

#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "algo_digital_meter_nms.h"

/* 区分旧版以及新版模型文件， 旧版模型直接通过tflite转
 * 新版本模型通过head+tflie, 其中head=MODEL_MAGIC_NUM | INDEX
 * INDEX定义为1-4为自定义模型，其它为seeed保留模型
 * 目前 0x11 保留位 人体检测模型
 */
#define MODEL_MAGIC_NUM 0x4C485400

// Globals, used for compatibility with Arduino-style sketches.
namespace
{
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    static std::forward_list<object_t> _digital_meter_list;
    static uint32_t value = 0;

    // In order to use optimized tensorflow lite kernels, a signed int8_t quantized
    // model is preferred over the legacy unsigned model format. This means that
    // throughout this project, input images must be converted from unisgned to
    // signed format. The easiest and quickest way to convert from unsigned to
    // signed 8-bit integers is to subtract 128 from the unsigned value to get a
    // signed value.

    // An area of memory to use for input, output, and intermediate arrays.
    constexpr int kTensorArenaSize = 600 * 1024;
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__CCAC__)
    static uint8_t tensor_arena[kTensorArenaSize] __attribute__((aligned(16)));
#else
#pragma Bss(".tensor_arena")
    static uint8_t tensor_arena[kTensorArenaSize];
#pragma Bss()
#endif // if defined (_GNUC_) && !defined (_CCAC_)
} // namespace

// TFu debug log, make it hardware platform print
extern "C" void DebugLog(const char *s) { xprintf("%s", s); } //{ fprintf(stderr, "%s", s); }

#define MODEL_INDEX 1
#define ALGORITHM_INDEX 4
#define IOU 20
#define CONFIDENCE 50
#define IMG_PREVIEW_MAX_SIZE 20
#define IMAGE_PREIVEW_ELEMENT_NUM 6
#define IMAGE_PREIVEW_ELEMENT_SIZE 4
#define IMAGE_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d, \"count\":%d, \"object\":{\"x\": [%s],\"y\": [%s],\"w\": [%s],\"h\": [%s],\"target\": [%s],\"confidence\": [%s]}, \"value\":%d}"

extern "C" int tflitemicro_algo_init()
{
    int ercode = 0;

    // static tflite::ErrorReporter micro_error_reporter;
    // error_reporter = &micro_error_reporter;
    uint32_t model_addr = 0x30200000;
    // The new version of the model skips the header information
    if ((*(uint32_t *)model_addr & 0xFFFFFF00) == MODEL_MAGIC_NUM)
    {
        model_addr += 4;
    }
    model = ::tflite::GetModel((void *)model_addr);

    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        // TF_LITE_REPORT_ERROR(error_reporter,
        //                      "Model provided is schema version %d not equal "
        //                      "to supported version %d.",
        //                      model->version(), TFLITE_SCHEMA_VERSION);
        return -1;
    }

    static tflite::MicroMutableOpResolver<17> micro_op_resolver;
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddPad();
    micro_op_resolver.AddPadV2();
    micro_op_resolver.AddAdd();
    micro_op_resolver.AddSub();
    micro_op_resolver.AddRelu();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddConcatenation();
    micro_op_resolver.AddQuantize();
    micro_op_resolver.AddTranspose();
    micro_op_resolver.AddLogistic();
    micro_op_resolver.AddSplitV();
    micro_op_resolver.AddMul();
    micro_op_resolver.AddStridedSlice();
    micro_op_resolver.AddSlice();
    micro_op_resolver.AddResizeNearestNeighbor();

    // Build an interpreter to run the model with.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;
    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        // TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return -1;
    }
    // Get information about the memory area to use for the model's input.
    input = interpreter->input(0);
    output = interpreter->output(0);

    return ercode;
}

extern "C" int tflitemicro_algo_run(uint32_t img, uint32_t ow, uint32_t oh)
{

    uint16_t h = input->dims->data[1];
    uint16_t w = input->dims->data[2];
    uint16_t c = input->dims->data[3];

    yuv422p2rgb(input->data.uint8, (const uint8_t *)img, oh, ow, c, h, w, VISION_ROTATION);

    for (int i = 0; i < input->bytes; i++)
    {
        input->data.int8[i] -= 128;
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        MicroPrintf("Invoke failed.");
        return -1;
    }

    // Get the results of the inference attempt
    float scale = ((TfLiteAffineQuantization *)(output->quantization.params))->scale->data[0];
    int zero_point = ((TfLiteAffineQuantization *)(output->quantization.params))->zero_point->data[0];

    uint32_t records = output->dims->data[1];
    uint32_t num_class = output->dims->data[2] - OBJECT_T_INDEX;
    int16_t num_element = num_class + OBJECT_T_INDEX;

#if 1
    logger("scale: %d, zero_point: %d, records: %d, num_class: %d, num_element: %d\n", int(scale * 10000), zero_point, records, num_class, num_element);
    for (int i = 0; i < records; i++)
    {
        float confidence = float(output->data.int8[i * num_element + OBJECT_C_INDEX] - zero_point) * scale;
        if (confidence > 50)
        {
            logger("confidence: %d->%d\n", output->data.int8[i * num_element + OBJECT_C_INDEX], int(confidence));
            int8_t max = -128;
            int target = 0;
            for (int j = 0; j < num_class; j++)
            {
                if (max < output->data.int8[i * num_element + OBJECT_T_INDEX + j])
                {
                    max = output->data.int8[i * num_element + OBJECT_T_INDEX + j];
                    target = j;
                }
            }
            int x = int(float(float(output->data.int8[i * num_element + OBJECT_X_INDEX] - zero_point) * scale));
            int y = int(float(float(output->data.int8[i * num_element + OBJECT_Y_INDEX] - zero_point) * scale));
            int w = int(float(float(output->data.int8[i * num_element + OBJECT_W_INDEX] - zero_point) * scale));
            int h = int(float(float(output->data.int8[i * num_element + OBJECT_H_INDEX] - zero_point) * scale));

            // logger("index: %d target: %d max: %d confidence: %d box{x: %d, y: %d, w: %d, h: %d}\n", i, target, max, int((float)confidence * 100), x, y, w, h);
        }
    }
#endif

    // _digital_meter_list.clear();

    // _digital_meter_list = nms_get_obeject_topn(output->data.int8, records, CONFIDENCE, IOU, w, h, records, num_class, scale, zero_point);

    // uint32_t tmp_value = 0;
    // uint32_t tmp_place = 1;
    // for (auto &obj : _digital_meter_list)
    // {
    //     if (obj.target > 9) // use old value
    //     {
    //         tmp_value = tmp_value * 10 + ((value / tmp_place) % 10);
    //         continue;
    //     }
    //     tmp_value = tmp_value * 10 + obj.target;
    //     tmp_place *= 10;
    // }

    value = 0;

    return 0;
}

int tflitemicro_algo_get_preview(char *preview, uint16_t max_length)
{
    if (_digital_meter_list.empty())
    {
        return -1;
    }
    uint16_t index = 0;
    uint16_t size = std::distance(_digital_meter_list.begin(), _digital_meter_list.end());

    uint16_t available_size = (max_length - sizeof(IMAGE_PREVIEW_FORMATE)) / (IMAGE_PREIVEW_ELEMENT_SIZE * IMAGE_PREIVEW_ELEMENT_NUM);

    if (available_size < 1)
    {
        return -1;
    }

    char element[IMAGE_PREIVEW_ELEMENT_NUM][IMG_PREVIEW_MAX_SIZE * IMAGE_PREIVEW_ELEMENT_SIZE] = {0};

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

        if (index > IMG_PREVIEW_MAX_SIZE || index > available_size)
        {
            break;
        }
    }

    snprintf(preview, max_length, IMAGE_PREVIEW_FORMATE, ALGORITHM_INDEX, MODEL_INDEX, size, element[0], element[1], element[2], element[3], element[4], element[5], value);

    return 0;
}

void tflitemicro_algo_exit()
{
    hx_lib_pm_cplus_deinit();
}
