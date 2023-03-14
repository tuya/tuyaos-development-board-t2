############################################################
# 执行单元测试
############################################################
PRJ_LIB := $(ROOT_DIR)/libs/libtuyaos.a $(ROOT_DIR)/libs/libtuyaos_adapter.a
PRJ_INC := $(addprefix $(ROOT_DIR)/,$(TUYA_SDK_INC_ALL_SUBDIRS))
OUTPUT_UT_DIR := $(OUTPUT_DIR)/ut
ut:
	@make -C scripts/ut all PRJ_LIB="$(PRJ_LIB)" PRJ_INC="$(PRJ_INC)" OUTPUT_DIR="$(OUTPUT_UT_DIR)"

clean_ut:
	@rm -rf $(OUTPUT_UT_DIR)

clean: clean_ut

.PHONY: ut clean_ut
ut: os
