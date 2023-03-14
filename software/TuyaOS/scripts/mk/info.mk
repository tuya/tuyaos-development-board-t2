############################################################
# 生成编译详情文件
############################################################
INFO_OUTPUT_DIR := $(OUTPUT_DIR)/info
INFO_OUTPUT_JSON := $(INFO_OUTPUT_DIR)/compile_information.json
pre_info:
	@mkdir -p $(INFO_OUTPUT_DIR)
	@rm -f $(INFO_OUTPUT_JSON)
	@echo "{" > $(INFO_OUTPUT_JSON)
	@echo "\"TOOLCHAIN_PATH\": \"$(TOOLCHAIN_PATH)\"," >> $(INFO_OUTPUT_JSON)
	@echo "\"CC\": \"$(CC)\"," >> $(INFO_OUTPUT_JSON)
	@echo "\"CFLAGS\": \"$(CFLAGS)\"" >> $(INFO_OUTPUT_JSON)
	@echo "}" >> $(INFO_OUTPUT_JSON)

info:
	@if [ "$(HAS_PYTHON3)" = "" ]; then echo has no python3; exit 1; fi
	@python3 -u ./scripts/build_compile_info.py $(INFO_OUTPUT_JSON) $(_XMAKE_INFO_DIR) 
	@echo "->: $(INFO_OUTPUT_JSON)"
	@echo "------ build info end ---------"

clean_info:
	@rm -rf $(_XMAKE_INFO_DIR)
	@rm -rf $(INFO_OUTPUT_DIR)

info: xmake_out pre_info

.PHONY: info pre_info clean_info
