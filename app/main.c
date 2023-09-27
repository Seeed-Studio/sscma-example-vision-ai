#if defined(FREERTOS_DEMO)
#include <freertos_demo/freertos_demo.h>

int main(void)
{
	freertos_demo();
}
#else
#error "Unknown APP_TYPE"
#endif