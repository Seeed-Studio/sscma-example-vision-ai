#include "hx_drv_timer.h"
#include "debugger.h"
#include "logger.h"

#include "hx_drv_webusb.h"
#include "datapath.h"
#include "sensor_core.h"
#include "edgelab.h"

#include "yolo_model_data.h"

#define kTensorArenaSize (800 * 1024)

uint8_t malloc_buf[kTensorArenaSize] __attribute__((aligned(16)));

uint16_t color[] = {
  0x0000,
  0x03E0,
  0x001F,
  0x7FE0,
  0xFFFF,
};

int main()
{
    uint32_t frame = 0;
    el_err_code_t ret = EL_OK;

    Device*  device  = Device::get_device();
    Camera*  camera  = device->get_camera();

    hx_drv_webusb_init();

    camera->init(240, 240);

    debugger_init();
    hx_drv_timer_init();

    auto* engine       = new InferenceEngine();
    auto* tensor_arena = malloc_buf;
    ret = engine->init(tensor_arena, kTensorArenaSize);
    ret = engine->load_model(g_yolo_model_data, g_yolo_model_data_len);
    auto* algorithm = new AlgorithmYOLO(engine);

    for (;;)
    {
        el_printf("Frame: %d\r", frame++);
        el_img_t web_img, algo_img;
        camera->start_stream();
        camera->get_jpeg(&web_img);
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
            //el_draw_rect(&web_img, x, y, box.w, box.h, color[++i % 5], 4);
        }
        el_printf("preprocess: %d, run: %d, postprocess: %d\n", preprocess_time, run_time, postprocess_time);

        hx_drv_webusb_write_vision(web_img.data, web_img.size);
    }
    return 0;
}