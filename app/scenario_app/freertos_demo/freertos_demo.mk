SCENARIO_APP_SUPPORT_LIST += $(APP_TYPE)

APPL_DEFINES += -DFREERTOS_DEMO

#boards
APPL_DEFINES += -DGROVE_VISION_AI

#if you don't use the OV camera, pls comment this code
APPL_DEFINES += -DCIS_OV_SENSOR -DCIS_OV2640_BAYER

LIB_SEL += arc_mli
LIB_SEL += sensordp pwrmgmt
LIB_SEL += i2c_comm
LIB_SEL += audio

OS_SEL = freertos
OS_TYPE = RTOS

# 32 kB
STACKSZ = 65536
# 64 kB
HEAPSZ = 65536

USER_LINKER_SCRIPT_FILE = $(SCENARIO_APP_ROOT)/freertos_demo/freertos_demo.ld

SCENARIO_APP_SUPPORT_LIST += freertos_demo 

