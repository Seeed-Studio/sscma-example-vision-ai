#include "edgelab.h"

int main()
{
    auto* data_delegate = DataDelegate::get_delegate();
    auto* storage       = data_delegate->get_storage_handler();
    int32_t boot_count  = 0;

    //TODO: need to fix
    //add this delay for console output ready
    el_sleep(500);

    el_err_code_t ret = EL_OK;
    ret = storage->init();
    el_printf("storage init=%d\r\n", ret);

    if(!storage->contains("boot_count")) {
        el_printf("not contain boot_count\n");
        *storage << el_make_storage_kv("boot_count", boot_count);
    }
    *storage >> el_make_storage_kv("boot_count", boot_count);
    el_printf("system boot count:%d\n", boot_count);

    *storage << el_make_storage_kv("boot_count", ++boot_count);

    for(;;);

    return 0;
}