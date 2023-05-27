##
# \defgroup	MK_BOARD	Board Makefile Configurations
# \brief	makefile related to board configurations
##

# boards root declaration
BOARDS_ROOT = $(EMBARC_ROOT)/himax_we1_sdk/board
TCF = $(BOARDS_ROOT)/arcem9d_wei_r16.tcf

## Compiler Options
BOARD_CORE_DIR = $(BOARDS_ROOT)
BOARD_INCDIR   = $(BOARDS_ROOT)

## GNU Toolchain Options For Core ##
COMMON_CORE_OPT_GNU = @$(EMBARC_GENERATED_DIR)/$(GENE_GCC_ARG)
CCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
CXXCORE_OPT_GNU += $(COMMON_CORE_OPT_GNU)
ACORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
LCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
