#include "hx_drv_timer.h"
#include "debugger.h"
#include "logger.h"

#include "hx_drv_webusb.h"
#include "datapath.h"
#include "sensor_core.h"
#include "core/edgelab.h"
#include "porting/himax/el_camera_himax.h"

using namespace edgelab;
using namespace edgelab::types;

int main()
{
    uint32_t frame = 0;

    Device*  device  = Device::get_device();
    Camera*  camera  = device->get_camera();

    hx_drv_webusb_init();

    camera->init(240, 240);

    debugger_init();
    hx_drv_timer_init();

    for (;;)
    {
        LOGGER_INFO("Frame: %d\r", frame++);
        el_img_t img;
        camera->start_stream();
        reinterpret_cast<CameraHimax*>(camera)->get_jpeg(&img);

        hx_drv_webusb_write_vision(img.data, img.size);
    }
    return 0;
}
