SCENARIO_APP_SUPPORT_LIST += $(APP_TYPE)

APPL_DEFINES += -DSSCMA_YOLO

#boards
APPL_DEFINES += -DGROVE_VISION_AI

#if you don't use the OV camera, pls comment this code
APPL_DEFINES += -DCIS_OV_SENSOR -DCIS_OV2640_BAYER

LIB_CV_PREBUILT = 0

LIB_SEL += arc_mli
LIB_SEL += sensordp pwrmgmt
LIB_SEL += i2c_comm
LIB_SEL += audio
LIB_SEL += sscma_micro
LIB_SEL += tflitemicro_25

OS_SEL = freertos
OS_TYPE = RTOS

# 32 kB
STACKSZ = 65536
# 64 kB
HEAPSZ = 65536

USER_LINKER_SCRIPT_FILE = $(SCENARIO_APP_ROOT)/sscma_yolo/sscma_yolo.ld

COMPONENT_ROOT = components
COMPONENT_SUPPORT_LIST +=	$(COMPONENT_ROOT)/configs \
								$(COMPONENT_ROOT)/debugger \
								$(COMPONENT_ROOT)/communication \
								$(COMPONENT_ROOT)/communication/webusb \
								$(COMPONENT_ROOT)/datapath \
								$(COMPONENT_ROOT)/utils \
								$(COMPONENT_ROOT)/drivers \
								$(COMPONENT_ROOT)/drivers/flash \
								$(COMPONENT_ROOT)/drivers/sensor \
								$(COMPONENT_ROOT)/drivers/sensor/camera \
								$(COMPONENT_ROOT)/drivers/sensor/imu \
								$(COMPONENT_ROOT)/sscma-micro \
								$(COMPONENT_ROOT)/sscma-micro/core \
								$(COMPONENT_ROOT)/sscma-micro/core/algorithm \
								$(COMPONENT_ROOT)/sscma-micro/core/data \
								$(COMPONENT_ROOT)/sscma-micro/core/engine \
								$(COMPONENT_ROOT)/sscma-micro/core/synchronize \
								$(COMPONENT_ROOT)/sscma-micro/core/utils \
								$(COMPONENT_ROOT)/sscma-micro/porting \
								$(COMPONENT_ROOT)/sscma-micro/porting/we1 \
								$(COMPONENT_ROOT)/sscma-micro/porting/we1/boards \
								$(COMPONENT_ROOT)/sscma-micro/porting/we1/drivers \
								$(COMPONENT_ROOT)/sscma-micro/sscma \
								$(COMPONENT_ROOT)/sscma-micro/sscma/callback \
								$(COMPONENT_ROOT)/sscma-micro/sscma/callback/internal \
								$(COMPONENT_ROOT)/sscma-micro/sscma/repl \
								$(COMPONENT_ROOT)/sscma-micro/sscma/interpreter \
								$(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB 

SCENARIO_APP_SUPPORT_LIST += sscma_yolo 

