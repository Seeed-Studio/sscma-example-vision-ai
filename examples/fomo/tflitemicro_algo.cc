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

#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace
{
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    std::forward_list<fomo_t> _fomo_list;

    // In order to use optimized tensorflow lite kernels, a signed int8_t quantized
    // model is preferred over the legacy unsigned model format. This means that
    // throughout this project, input images must be converted from unisgned to
    // signed format. The easiest and quickest way to convert from unsigned to
    // signed 8-bit integers is to subtract 128 from the unsigned value to get a
    // signed value.

    // An area of memory to use for input, output, and intermediate arrays.
    constexpr int kTensorArenaSize = 450 * 1024;
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
#define ALGORITHM_INDEX 0
#define CONFIDENCE_THRESHOLD 50
#define IMG_PREVIEW_MAX_SIZE 10
#define IMAGE_PREIVEW_ELEMENT_NUM 6
#define IMAGE_PREIVEW_ELEMENT_SIZE 4
#define IMAGE_PREVIEW_FORMATE "{\"type\":\"preview\", \"algorithm\":%d, \"model\":%d, \"count\":%d, \"object\":{\"x\": [%s],\"y\": [%s],\"w\": [%s],\"h\": [%s],\"target\": [%s],\"confidence\": [%s]}}"

extern "C" int tflitemicro_algo_init()
{
    int ercode = 0;
    uint32_t *xip_flash_addr;

    xip_flash_addr = (uint32_t *)0x30000000;

    // get model (.tflite) from flash
    model = ::tflite::GetModel(xip_flash_addr);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        MicroPrintf(
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return -1;
    }

    static tflite::MicroMutableOpResolver<16> micro_op_resolver;
    micro_op_resolver.AddPad();
    micro_op_resolver.AddPadV2();
    micro_op_resolver.AddAdd();
    micro_op_resolver.AddShape();
    micro_op_resolver.AddSplit();
    micro_op_resolver.AddSlice();
    micro_op_resolver.AddSplitV();
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddLogistic();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddTranspose();
    micro_op_resolver.AddLeakyRelu();
    micro_op_resolver.AddConcatenation();
    micro_op_resolver.AddResizeBilinear();
    micro_op_resolver.AddDepthwiseConv2D();

    // Build an interpreter to run the model with.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;
    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        MicroPrintf("AllocateTensors() failed");
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

    uint32_t start_time = board_get_cur_us();
    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        MicroPrintf("Invoke failed.");
        return -1;
    }

    LOGGER_INFO("Inference time: %d ms", (board_get_cur_us() - start_time) / 1000);

    return 0;
}

int tflitemicro_algo_get_preview(char *preview, uint16_t max_length)
{
    return -1;
}

void tflitemicro_algo_exit()
{
    hx_lib_pm_cplus_deinit();
}
