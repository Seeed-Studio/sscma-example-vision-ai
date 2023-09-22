#include "core/edgelab.h"

using namespace edgelab;
using namespace edgelab::types;

int main()
{
    uint32_t frame = 0;

    Device* device  = Device::get_device();
    Serial* serial  = device->get_serial();

    //TODO: need to fix
    //add this delay for console output ready
    el_sleep(500);

    el_err_code_t ret = EL_OK;
    ret = serial->init();
    el_printf("serial init=%d\r\n", ret);

    const size_t buffer_size = 128;
    char*        buffer      = new char[buffer_size]{};

    for (;;)
    {
        serial->get_line(buffer, buffer_size);
        serial->send_bytes(buffer, strlen(buffer));
        el_sleep(100);
    }

    delete[] buffer;

    return 0;
}
