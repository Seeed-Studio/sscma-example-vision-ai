
#include "edgelab.h"
#include "powermode.h"
#include "sscma/main_task.hpp"
using namespace edgelab;

static void app(void* arg) {
    el_err_code_t ret    = EL_OK;
    Device*       device = edgelab::Device::get_device();
    Camera*       camera = device->get_camera();

    el_printf("device: 0x%lx\n", &device);
    el_printf("camera: 0x%lx\n", &camera);

    camera->init(240, 240);

    Models* models = new Models();

    el_printf("Init models...\n");
    models->init();

    el_printf("Get all model info and print ->\n");
    std::forward_list<el_model_info_t> all_model_info = models->get_all_model_info();
    for (const auto& i : all_model_info)
        el_printf("\tmodel: {id: %d, type: %d, flash_address: 0x%lX, size: %ld, memory_address: %lx}\n",
                  i.id,
                  i.type,
                  i.addr_flash,
                  i.size,
                  i.addr_memory);

    if (std::distance(all_model_info.begin(), all_model_info.end()) == 0) {
        el_printf("No model found, exit\n");
        while (1) {
            el_sleep(1000);
            el_printf("*");
        }
    }

    auto* tensor_arena = (void*)el_malloc(600 * 1024);
    if (tensor_arena == NULL) {
        el_printf("malloc failed\n");
    } else {
        el_printf("malloc success: %d\n", tensor_arena);
    }

    auto* engine = new EngineTFLite();
    engine->init(tensor_arena, 600 * 1024);

    ret = engine->load_model((void*)all_model_info.begin()->addr_memory, all_model_info.begin()->size);

    if (ret != EL_OK) {
        el_printf("load model failed\n");
    } else {
        el_printf("load model success\n");
    }

    auto* algorithm = new AlgorithmYOLO(engine);

    while (1) {
        // // xprintf("%s\n", "hello world");
        // // el_sleep(1000);
        if (camera->start_stream() != EL_OK) {
            el_printf("timeout\n");
            continue;
        }
        el_img_t algo_img;
        camera->get_frame(&algo_img);
        algorithm->run(&algo_img);
        uint32_t preprocess_time  = algorithm->get_preprocess_time();
        uint32_t run_time         = algorithm->get_run_time();
        uint32_t postprocess_time = algorithm->get_postprocess_time();
        uint8_t  i                = 0u;
        for (const auto& box : algorithm->get_results()) {
            el_printf("\tbox -> cx_cy_w_h: [%d, %d, %d, %d] t: [%d] s: [%d]\n",
                      box.x,
                      box.y,
                      box.w,
                      box.h,
                      box.target,
                      box.score);

            int16_t y = box.y - box.h / 2;
            int16_t x = box.x - box.w / 2;
            // el_draw_rect(&web_img, x, y, box.w, box.h, color[++i % 5], 4);
        }
        el_printf("preprocess: %d, run: %d, postprocess: %d\n", preprocess_time, run_time, postprocess_time);
        el_img_t jpeg_img;
        auto     ret = camera->get_jpeg(&jpeg_img);
        el_printf("jpeg size: 0x%08x, add: 0x%08x\n", jpeg_img.size, jpeg_img.data);
        if (ret == EL_OK) [[likely]] {
            auto* buffer = new char[((jpeg_img.size + 2) / 3) * 4 + 1]{};
            if (buffer != NULL) {
                memset(buffer, 0, ((jpeg_img.size + 2) / 3) * 4 + 1);
                el_base64_encode(jpeg_img.data, jpeg_img.size, buffer);
                el_printf("data:image/jpeg;base64,%s\n", buffer);
                delete[] buffer;
            } else {
                el_printf("buffer is NULL\n");
            }
        }
        el_printf("jpeg encode time: %d\n", 0);
        camera->stop_stream();
    }
}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
extern "C" int app_main(void) {
    if (xTaskCreate(app, "app", 32*1024, NULL, 1, NULL) != pdPASS) {
        printf("app creation failed!.\r\n");
        while (1)
            ;
    }
    vTaskStartScheduler();

    for (;;)
        ;
}
