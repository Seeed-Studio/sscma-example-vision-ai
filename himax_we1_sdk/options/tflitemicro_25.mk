# directory declaration
#LIB_CV_DIR = $(LIBRARIES_ROOT)/cv/tflitemicro_25
LIB_CV_DIR = $(EMBARC_ROOT)

LIB_CV_ASMSRCDIR	= $(LIB_CV_DIR)
LIB_CV_INCDIR	= $(LIB_CV_DIR)

#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party/gemmlowp
#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party
#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party/flatbuffers/include
#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party/arc_mli_package/include
#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party/arc_mli_package/include/api
#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party/kissfft
#LIB_CV_INCDIR += $(LIB_CV_DIR)/third_party/ruy

LIB_CV_INCDIR += $(EMBARC_ROOT)/third_party/gemmlowp
LIB_CV_INCDIR += $(EMBARC_ROOT)/third_party/flatbuffers/include
LIB_CV_INCDIR += $(EMBARC_ROOT)/third_party/arc_mli_package/include
LIB_CV_INCDIR += $(EMBARC_ROOT)/third_party/arc_mli_package/include/api
LIB_CV_INCDIR += $(EMBARC_ROOT)/third_party/kissfft
LIB_CV_INCDIR += $(EMBARC_ROOT)/third_party/ruy

LIB_CV_INCDIR += $(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli


LIB_CV_CSRCDIR = $(LIB_CV_DIR)/tensorflow/lite/c
LIB_CV_CCSRCSDIR =  \
$(LIB_CV_DIR)/tensorflow/lite/micro \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels \
$(LIB_CV_DIR)/tensorflow/lite/micro/memory_planner \
$(LIB_CV_DIR)/tensorflow/lite/core/api \
$(LIB_CV_DIR)/tensorflow/lite/kernels/internal \
$(LIB_CV_DIR)/tensorflow/lite/kernels \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc

# find all the source files in the target directories
LIB_CV_CSRCS = $(call get_csrcs, $(LIB_CV_CSRCDIR))
LIB_CV_CXXSRCS = $(call get_cxxsrcs, $(LIB_CV_CXXSRCSDIR))
LIB_CV_ASMSRCS = $(call get_asmsrcs, $(LIB_CV_ASMSRCDIR))

#tensorflow/lite/micro/kernels/conv.cc \
#tensorflow/lite/micro/kernels/fully_connected.cc \
#tensorflow/lite/micro/kernels/depthwise_conv.cc\
#tensorflow/lite/micro/kernels/pooling.cc \

LIB_CV_CCSRCS := \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/scratch_buffers.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/scratch_buf_mgr.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/conv.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/pooling.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/fully_connected.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/depthwise_conv.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/mli_slicers.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/simple_memory_allocator.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/mock_micro_graph.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_allocator.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/all_ops_resolver.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/memory_helpers.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_interpreter.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_error_reporter.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_time.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/recording_simple_memory_allocator.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_string.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_profiler.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_utils.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/test_helpers.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/micro_graph.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/system_setup.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/recording_micro_allocator.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/memory_planner/linear_memory_planner.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc \
$(LIB_CV_DIR)/tensorflow/lite/c/common.c \
$(LIB_CV_DIR)/tensorflow/lite/core/api/error_reporter.cc \
$(LIB_CV_DIR)/tensorflow/lite/core/api/flatbuffer_conversions.cc \
$(LIB_CV_DIR)/tensorflow/lite/core/api/op_resolver.cc \
$(LIB_CV_DIR)/tensorflow/lite/core/api/tensor_utils.cc \
$(LIB_CV_DIR)/tensorflow/lite/kernels/internal/quantization_util.cc \
$(LIB_CV_DIR)/tensorflow/lite/kernels/kernel_util.cc \
$(LIB_CV_DIR)/tensorflow/lite/schema/schema_utils.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/activations.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/add.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/add_n.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arg_min_max.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/batch_to_space_nd.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/cast.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/ceil.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/circular_buffer.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/comparisons.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/concatenation.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/conv.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/conv_common.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/cumsum.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/depth_to_space.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/depthwise_conv.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/depthwise_conv_common.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/dequantize.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/detection_postprocess.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/elementwise.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/elu.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/ethosu.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/exp.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/expand_dims.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/fill.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/floor.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/floor_div.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/floor_mod.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/fully_connected.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/fully_connected_common.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/gather.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/gather_nd.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/hard_swish.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/if.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/kernel_runner.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/kernel_util.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/l2norm.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/l2_pool_2d.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/leaky_relu.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/logical.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/logistic.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/log_softmax.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/maximum_minimum.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/mul.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/neg.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/pack.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/pad.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/arc_mli/pooling.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/prelu.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/quantize.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/quantize_common.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/reduce.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/reshape.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/resize_bilinear.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/round.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/shape.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/softmax.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/softmax_common.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/space_to_batch_nd.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/split.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/split_v.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/squeeze.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/strided_slice.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/sub.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/svdf.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/svdf_common.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/tanh.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/transpose.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/transpose_conv.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/unpack.cc \
$(LIB_CV_DIR)/tensorflow/lite/micro/kernels/zeros_like.cc 


ifeq ($(ALGO_TYPE), TFLITE_MICRO_YOLO_FASTEST)
	ifeq (YOLO_FASTEST_HUMAN_DETECTION, $(findstring YOLO_FASTEST_HUMAN_DETECTION, $(APPL_DEFINES))) 
		LIB_CV_CCSRCS += $(LIB_CV_DIR)/examples/yolo_coco/yolo_coco.cc
	endif
endif

ifeq ($(ALGO_TYPE), TFLITE_MICRO_GOOGLE_PERSON)
LIB_CV_CCSRCS += $(LIB_CV_DIR)/examples/person_detection_experimental/person_detect_model_data.cc
APPL_DEFINES += -DTFLITE_MICRO_GOOGLE_PERSON
endif
ifeq ($(ALGO_TYPE), TFLITE_MICRO_HIMAX_PERSON)
	ifeq (YOLO, $(findstring YOLO, $(APPL_DEFINES)))
		LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/yolo_PTQ_int8.cc
	endif
	ifeq (HX_VIP_YOMO, $(findstring HX_VIP_YOMO, $(APPL_DEFINES)))
		#BD
		ifeq (HX_VIP_YOMO_20210115_E_PANA, $(findstring HX_VIP_YOMO_20210115_E_PANA, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/YOMO_E_PANA_20210115.cc
		else ifeq (HX_VIP_YOMO_20210121_PBR_PANA, $(findstring HX_VIP_YOMO_20210121_PBR_PANA, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/YOMO_PBR_AoS96_PANA_20210121.cc
		else ifeq (HX_VIP_YOMO_DOOR_DETECT, $(findstring HX_VIP_YOMO_DOOR_DETECT, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/YOMO_DOOR_DETECT.cc
		else ifeq (HX_VIP_YOMO_AMR_ROI, $(findstring HX_VIP_YOMO_AMR_ROI, $(APPL_DEFINES)))
			ifeq (AMR_DEMO_DATASET, $(findstring AMR_DEMO_DATASET, $(APPL_DEFINES)))
				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/AMR_ROI_DEMO_DATASET_10CM.cc
				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/AMR_CR_DEMO_DATASET_10CM.cc
			else ifeq (AMR_ORIG_DATASET, $(findstring AMR_ORIG_DATASET, $(APPL_DEFINES)))
				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/AMR_ROI_ORIG_DATASET.cc
				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/AMR_CR_ORIG_DATASET.cc
			else ifeq (AMR_CR_DEMO_FIX_LAYOUT_5CM_DATASET, $(findstring AMR_CR_DEMO_FIX_LAYOUT_5CM_DATASET, $(APPL_DEFINES)))
				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/AMR_CR_DEMO_FIX_LAYOUT_5CM.cc#AMR_CR_DEMO_FIX_LAYOUT_5CM.cc
			endif
			
		endif	
		# static hand gesture
		ifeq (HX_VIP_STATIC_GESTURE_320x240, $(findstring HX_VIP_STATIC_GESTURE_320x240, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/HGR_tiny_hands_3_mix_44_L.cc
		else ifeq (HX_VIP_STATIC_GESTURE_04_V2_320x200, $(findstring HX_VIP_STATIC_GESTURE_04_V2_320x200, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/tiny_hands_3_mix_04_MobileNetV2.cc
		else ifeq (HX_VIP_STATIC_GESTURE_06_V2_320x200, $(findstring HX_VIP_STATIC_GESTURE_06_V2_320x200, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/tiny_hands_3_mix_06_MobileNetV2.cc
		else ifeq (HX_VIP_STATIC_GESTURE_07_V3_320x200, $(findstring HX_VIP_STATIC_GESTURE_07_V3_320x200, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/tiny_hands_3_mix_07_MobileNetV3_S.cc
		else ifeq (HX_VIP_STATIC_GESTURE_91_320x240, $(findstring HX_VIP_STATIC_GESTURE_91_320x240, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/vip_gesture_mix_91_MobileNetV2.cc
		else ifeq (HX_VIP_STATIC_GESTURE_27_320x240, $(findstring HX_VIP_STATIC_GESTURE_27_320x240, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/vip_gesture_sc_bg_ft_27_MobileNetV2.cc
		else ifeq (HX_VIP_HAND_CROP_96_GESTURE_M1_1_25_64x64, $(findstring HX_VIP_HAND_CROP_96_GESTURE_M1_1_25_64x64, $(APPL_DEFINES)))		
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/vip_hand_crop_96_gesture_M1_1.25_64x64.cc
		endif
		
		#FD
		ifeq (BIG_MODEL, $(findstring BIG_MODEL, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/_224_6anchors_YOMOv4_S_CIOU_gamma0.2to4_more.cc
		else ifeq (SMALL_MODEL, $(findstring SMALL_MODEL, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/FD_2b_c9_pana03_20211210.cc
		endif	
	
		ifeq (TEST_TIME, $(findstring TEST_TIME, $(APPL_DEFINES)))
			LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/FD_2b_c9.cc
		endif		
		
		
		
		
		
		ifeq (AIOT_YUNTAI_VIP, $(findstring AIOT_YUNTAI_VIP, $(APPL_DEFINES)))
			ifeq (YUNTAI_FD_HGC_KCF, $(findstring YUNTAI_FD_HGC_KCF, $(APPL_DEFINES)))
			    ifeq (BIG_MODEL, $(findstring BIG_MODEL, $(APPL_DEFINES)))
			        LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/_224_6anchors_YOMOv4_S_CIOU_gamma0.2to4_more.cc
			    else ifeq (SMALL_MODEL, $(findstring SMALL_MODEL, $(APPL_DEFINES)))
			        LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/FD_2b_c9_pana03_20211210.cc
			    endif
#				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/vip_hand_crop_96_gesture_M1_1.25_64x64.cc
				LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/vip_hand_crop_92_gesture_M1_d16_b2_2c_48x48_palm.cc
			endif
		endif
		
	else
		LIB_CV_CCSRCS += $(LIBRARIES_ROOT)/cv/himax/model/yolo_PTQ_int8.cc
	endif

APPL_DEFINES += -DTFLITE_MICRO_HIMAX_PERSON
endif

# get object files
LIB_CV_COBJS = $(call get_relobjs, $(LIB_CV_CSRCS))
LIB_CV_CXXOBJS = $(call get_relobjs, $(LIB_CV_CXXSRCS))
LIB_CV_CCOBJS = $(call get_relobjs, $(LIB_CV_CCSRCS))
LIB_CV_ASMOBJS = $(call get_relobjs, $(LIB_CV_ASMSRCS))
LIB_CV_OBJS = $(LIB_CV_COBJS) $(LIB_CV_ASMOBJS) $(LIB_CV_CXXOBJS) $(LIB_CV_CCOBJS)

#CV_CXXFLAGS += -O3 -DNDEBUG -g -DTF_LITE_STATIC_MEMORY -DEMBARC_TCF_GENERATED -fno-rtti -Hnocopyr -O3 -Hpurge -Hcl -fslp-vectorize-aggressive -ffunction-sections -fdata-sections
#CV_CCFLAGS +=  -DNDEBUG -g -DTF_LITE_STATIC_MEMORY -DEMBARC_TCF_GENERATED -Hnocopyr -O3 -Hpurge -Hcl -fslp-vectorize-aggressive -ffunction-sections -fdata-sections
#CV_CXXFLAGS += -DSCRATCH_MEM_Z_SIZE=0x10000

APPL_DEFINES += -O3 -DNDEBUG -g -DTF_LITE_STATIC_MEMORY -DEMBARC_TCF_GENERATED -Os -ffunction-sections -fdata-sections
APPL_DEFINES +=  -DNDEBUG -g -DTF_LITE_STATIC_MEMORY -DEMBARC_TCF_GENERATED -Os -ffunction-sections -fdata-sections
ifeq ($(TOOLCHAIN), mw)
APPL_DEFINES += -fno-rtti -Hnocopyr -Hpurge -Hcl -fslp-vectorize-aggressive
APPL_DEFINES += -Hnocopyr -Hpurge -Hcl -fslp-vectorize-aggressive
endif

APPL_DEFINES += -DSCRATCH_MEM_Z_SIZE=0x10000

# extra macros to be defined
LIB_CV_DEFINES = -DLIB_TFLITEMICRO

# genearte library
CV_LIB = $(OUT_DIR)/libtflitemicro_25.a

LIB_INCDIR += $(LIB_CV_INCDIR)


ifdef USER_LINKER_SCRIPT_FILE
	override LINKER_SCRIPT_FILE := $(USER_LINKER_SCRIPT_FILE)
endif

ifeq "$(HOST_OS)" "Windows"
MLI_LIB = $(BOARD_OUT_DIR)\$(BUILD_INFO)\libmli.a
PREBUILT_MLI_LIB = $(EMBARC_ROOT)\third_party\arc_mli_package\bin\himax_arcem9d_r16\release\libmli.a
else
MLI_LIB = $(BOARD_OUT_DIR)/$(BUILD_INFO)/libmli.a
PREBUILT_MLI_LIB = $(EMBARC_ROOT)/third_party/arc_mli_package/bin/himax_arcem9d_r16/release/libmli.a
endif

$(MLI_LIB) :
	$(CP) $(PREBUILT_MLI_LIB) $(MLI_LIB)

# library generation rule
ifeq ($(LIB_CV_PREBUILT), 1)
ifeq "$(HOST_OS)" "Windows"
CV_LIB = $(BOARD_OUT_DIR)\$(BUILD_INFO)\libtflitemicro_25.a
else
CV_LIB = $(BOARD_OUT_DIR)/$(BUILD_INFO)/libtflitemicro_25.a
endif
#
ifeq "$(HOST_OS)" "Windows"
ifeq ($(ALGO_TYPE), TFLITE_MICRO_GOOGLE_PERSON)
CV_PREBUILT_LIB = .\library\cv\tflitemicro_25\prebuilt_lib\libtflitemicro_google_person.a
endif
ifeq ($(ALGO_TYPE), TFLITE_MICRO_HIMAX_PERSON)
CV_PREBUILT_LIB = .\library\cv\himax\prebuilt_lib\libtflitemicro_25_himax_person.a
endif
ifeq ($(ALGO_TYPE), TFLITE_MICRO_YOLO_FASTEST)
CV_PREBUILT_LIB = .\library\cv\tflitemicro_25\prebuilt_lib\libtflitemicro_yolo_fastest.a
endif
else
ifeq ($(ALGO_TYPE), TFLITE_MICRO_GOOGLE_PERSON)
	ifeq ($(TOOLCHAIN), mw)
		CV_PREBUILT_LIB = ./library/cv/tflitemicro_25/prebuilt_lib/libtflitemicro_google_person.a
	else
		CV_PREBUILT_LIB = ./library/cv/tflitemicro_25/prebuilt_lib/libtflitemicro_google_person_gnu.a
	endif
endif
ifeq ($(ALGO_TYPE), TFLITE_MICRO_HIMAX_PERSON)
	ifeq ($(TOOLCHAIN), mw)
		CV_PREBUILT_LIB = ./library/cv/himax/prebuilt_lib/libtflitemicro_25_himax_person.a
	else
		CV_PREBUILT_LIB = ./library/cv/himax/prebuilt_lib/libtflitemicro_25_himax_person_gnu.a
	endif
endif
ifeq ($(ALGO_TYPE), TFLITE_MICRO_YOLO_FASTEST)
	ifeq ($(TOOLCHAIN), mw)
		CV_PREBUILT_LIB = ./library/cv/tflitemicro_25/prebuilt_lib/libtflitemicro_yolo_fastest.a
	else
		CV_PREBUILT_LIB = ./library/cv/tflitemicro_25/prebuilt_lib/libtflitemicro_yolo_fastest_gnu.a
	endif
endif
endif
$(warning $(CV_LIB)) 
$(CV_LIB) :
	$(CP) $(CV_PREBUILT_LIB) $(CV_LIB)
else
# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
CV_LIB_CSRCDIR += $(LIB_CV_CSRCDIR)
CV_LIB_CXXSRCDIR += $(LIB_CV_CXXSRCDIR)
CV_LIB_CCSRCDIR += $(LIB_CV_CCSRCDIR)
CV_LIB_ASMSRCDIR += $(LIB_CV_ASMSRCDIR)

CV_LIB_CSRCS += $(LIB_CV_CSRCS)
CV_LIB_CXXSRCS += $(LIB_CV_CXXSRCS)
CV_LIB_CCSRCS += $(LIB_CV_CCSRCS)
CV_LIB_ASMSRCS += $(LIB_CV_ASMSRCS)
CV_LIB_ALLSRCS += $(LIB_CV_CSRCS) $(LIB_CV_ASMSRCS) $(CV_LIB_CXXSRCS) $(CV_LIB_CCSRCS)

CV_LIB_COBJS += $(LIB_CV_COBJS)
CV_LIB_CXXOBJS += $(LIB_CV_CXXOBJS)
CV_LIB_CCOBJS += $(LIB_CV_CCOBJS)
CV_LIB_ASMOBJS += $(LIB_CV_ASMOBJS)
CV_LIB_ALLOBJS += $(LIB_CV_OBJS)

CV_LIB_DEFINES += $(LIB_CV_DEFINES)
CV_LIB_DEPS += $(LIB_CV_DEPS)
CV_LIB_LIBS += $(LIB_LIB_CV)

#$(warning no prebuilt tflitemicro_25)

$(CV_LIB): $(LIB_CV_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_CV_OBJS)
endif

# define BSS_LIB
override BSS_LIB = $(OUT_DIR)/libbss.a
