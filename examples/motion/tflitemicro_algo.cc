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

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
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

extern "C" int tflitemicro_algo_init()
{
    int ercode = 0;
    uint32_t *xip_flash_addr;

    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;
    xip_flash_addr = (uint32_t *)0x31000000;

    // get model (.tflite) from flash
    model = ::tflite::GetModel(xip_flash_addr);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return -1;
    }

    static tflite::MicroMutableOpResolver<3> micro_op_resolver;
    micro_op_resolver.AddRelu();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddFullyConnected();

    // Build an interpreter to run the model with.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroInterpreter static_interpreter(
        model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;
    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return -1;
    }
    // Get information about the memory area to use for the model's input.
    input = interpreter->input(0);
    output = interpreter->output(0);

    return ercode;
}

extern "C" int tflitemicro_algo_run(float *data)
{

    // Copy data into input tensor

    for (size_t i = 0; i < input->bytes; i++)
    {
        /* code */
        input->data.int8[i] = (int8_t)((float)(data[i]) * input->params.scale + input->params.zero_point) - 128;
    }

    uint32_t start_time = board_get_cur_us();

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk)
    {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
        return -1;
    }

    uint32_t end_time = board_get_cur_us();

    // get result
    float *output_data = output->data.f;

    LOGGER_INFO("tflitemicro_algo_run time: %d us\n", end_time - start_time);
    for (int i = 0; i < output->bytes / sizeof(float); i++)
    {
        /* code */
        LOGGER_INFO("output_data[%d] = %d\n", i, (int32_t)((float)(output_data[i]) * 1000));
    }

    return 0;
}

void tflitemicro_algo_exit()
{
    hx_lib_pm_cplus_deinit();
}
