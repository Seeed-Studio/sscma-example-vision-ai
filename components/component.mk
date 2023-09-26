SCENARIO_APP_SUPPORT_LIST +=	$(COMPONENT_ROOT)/configs \
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


# -------------- SSCMA-Micro --------------
sscma_get_csrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.c $(subdir)/*.C))
sscma_get_cxxsrcs = $(foreach subdir, $(1), $(wildcard $(subdir)/*.cpp $(subdir)/*.CPP))

SSCMA_INCDIRS = $(COMPONENT_ROOT)/sscma-micro \
                $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB \
				$(COMPONENT_ROOT)/sscma-micro/third_party/JPEGENC

# SSCMA_CSRCS += $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB/fal_flash.c \
# 			   $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB/fal_partition.c \
# 			   $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB/fal_rtt.c \
# 			   $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB/fdb_kvdb.c \
# 			   $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB/fdb_utils.c \
# 			   $(COMPONENT_ROOT)/sscma-micro/third_party/FlashDB/fdb.c
# SSCMA_CXXSRCS += $(COMPONENT_ROOT)/sscma-micro/third_party/JPEGENC/JPEGENC.cpp

SSCMA_SRCDIRS = $(COMPONENT_ROOT)/sscma-micro/core \
				$(COMPONENT_ROOT)/sscma-micro/core/algorithm \
				$(COMPONENT_ROOT)/sscma-micro/core/data \
				$(COMPONENT_ROOT)/sscma-micro/core/engine \
				$(COMPONENT_ROOT)/sscma-micro/core/synchronize \
				$(COMPONENT_ROOT)/sscma-micro/core/utils \
				$(COMPONENT_ROOT)/sscma-micro/porting \
				$(COMPONENT_ROOT)/sscma-micro/porting/himax \
				$(COMPONENT_ROOT)/sscma-micro/sscma \
				$(COMPONENT_ROOT)/sscma-micro/sscma/callback \
				$(COMPONENT_ROOT)/sscma-micro/sscma/callback/internal \
				$(COMPONENT_ROOT)/sscma-micro/sscma/repl \
				$(COMPONENT_ROOT)/sscma-micro/sscma/interpreter

SSCMA_CXXSRCS += $(call sscma_get_cxxsrcs, $(SSCMA_SRCDIRS))
