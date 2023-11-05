SCENARIO_APP_SUPPORT_LIST += $(APP_TYPE)

APPL_DEFINES += -DGROVE_AI_FAMILY

#APPL_DEFINES += -DDEBUG_LOCAL


#boards
APPL_DEFINES += -DGROVE_VISION_AI
#APPL_DEFINES += -DSENSECAP_AI

#if you don't use the OV camera, pls comment this code
APPL_DEFINES += -DCIS_OV_SENSOR -DCIS_OV2640_BAYER

LIB_SEL += tflitemicro_25

LIB_SEL += sensordp pwrmgmt
LIB_SEL += i2c_comm
LIB_SEL += audio

SCENARIO_APP_SUPPORT_LIST +=	grove_ai_family/configs \
								grove_ai_family/debugger \
								grove_ai_family/img \
								grove_ai_family/model \
								grove_ai_family/utlis \
								grove_ai_family/algorithm \
								grove_ai_family/algorithm/object_detection \
								grove_ai_family/algorithm/object_count \
								grove_ai_family/algorithm/image_classification \
								grove_ai_family/algorithm/pointer_meter \
								grove_ai_family/algorithm/digital_meter \
								grove_ai_family/communication \
								grove_ai_family/communication/webusb \
								grove_ai_family/communication/i2c_slave \
								grove_ai_family/communication/at_server \
								grove_ai_family/datapath \
								grove_ai_family/drivers \
								grove_ai_family/drivers/flash \
								grove_ai_family/drivers/sensor \
								grove_ai_family/drivers/wdt \
								grove_ai_family/drivers/sensor/camera \
								grove_ai_family/drivers/sensor/imu \
								grove_ai_family/drivers/sensor/mic 
					
