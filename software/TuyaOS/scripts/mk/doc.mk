############################################################
# 生成文档
############################################################
TMP_DOC_DIR := $(OUTPUT_DIR)/.doc
OUTPUT_DOC_DIR := $(OUTPUT_DIR)/tuya_iot_doc
HEADER_DIR :=  include
doc:
	@if [ "$(HAS_PYTHON3)" = "" ]; then echo has no python3; exit 1; fi
	@echo "====== build sdk doc begin ======="
	# 复制头文件
	@cp -a --parents $(TUYA_SDK_INC) -t ./include
	@python3 ./scripts/build_sdk_doc.py $(TMP_DOC_DIR) $(OUTPUT_DOC_DIR) $(HEADER_DIR)
	@echo "------ build sdk doc end ---------"

clean_doc:
	@rm -rf $(TMP_DOC_DIR)
	@rm -rf $(OUTPUT_DOC_DIR)

DOXYGEN_FILE := ./scripts/Doxyfile
DOXYGEN_OUTDIR := output/docs  # 保持和文件DOXYGEN_FILE中的定义一致
tydoc:
	@echo "====== build tydoc begin ======="
	@if [ "$(HAS_PYTHON3)" = "" ]; then echo has no python3; exit 1; fi
	@if [ ! -f ./scripts/error_code/release/tuya_error_code.md ]; then \
		cp ./scripts/error_code/release/tuya_error_code.md ./docs/; fi
	@python3 ./scripts/build_resources_doc.py ./docs
	@cp -a $(TUYA_SDK_DOCS) ./docs 2>/dev/null||true
	@doxygen $(DOXYGEN_FILE)>/dev/null 2>&1
	@if [ -d $(OS_FILES_DIR) ]; then \
		cp -fr  $(DOXYGEN_OUTDIR) $(OS_FILES_DIR)/; fi
	@echo "------ build tydoc end ---------"

clean_tydoc:
	@rm -rf $(DOXYGEN_OUTDIR)

clean: clean_tydoc

.PHONY: doc tydoc

