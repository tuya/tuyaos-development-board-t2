
# lib解压
ORI_LIBS_PATH=$(ROOT_DIR)/tmp/TuyaOS/libs
NPMS_PATH=$(ROOT_DIR)/tmp/npms_build
TAG_OBJS_PATH=$(NPMS_PATH)/objs
npms_unzip_libs:
	@if [ ! -d $(ORI_LIBS_PATH) ]; then \
		echo "no dir: $(ORI_LIBS_PATH)"; \
		exit 1; fi
	@echo "unzip none permissions libs ..."
	@rm -rf $(TAG_OBJS_PATH); mkdir -p $(TAG_OBJS_PATH)
	@export libs="`ls -1 $(ORI_LIBS_PATH)/*.a`"; \
		for l in $$libs; do \
			export _lib_name=`basename $${l}`; \
			echo "$${_lib_name}"; \
			export _lib_path="$${_lib_name%%.a}"; \
			mkdir -p $(TAG_OBJS_PATH)/$${_lib_path}; \
			cp $${l} -t $(TAG_OBJS_PATH)/$${_lib_path}; \
			cd $(TAG_OBJS_PATH)/$${_lib_path}; \
			$(AR) -x $${_lib_name}; \
			rm $${_lib_name}; \
			done


# objs复制
STATIC_OBJS_DIR ?= $(call static-objects-dir)
npms_copy_objs:
	@if [ ! -d $(STATIC_OBJS_DIR) ]; then \
		echo "no dir: $(STATIC_OBJS_DIR)"; \
		exit 1; fi
	@echo "copy none permissions objs ..."
	@export ori_objs="`find $(STATIC_OBJS_DIR) -name "*\.c\.o"`"; \
		for o in $$ori_objs; do \
			export _o_base="`basename $$o`"; \
			export _o_tag="`find $(TAG_OBJS_PATH) -name $$_o_base`"; \
			if [ $$_o_tag ]; then \
				cp $$o $$_o_tag; \
				fi; \
			done


# 重新打包lib
TAG_LIBS_PATH=$(NPMS_PATH)/lib
OUTPUT_LIB_PATH=$(OUTPUT_DIR)/lib
npms_os_static:
	@echo "build none permissions libs ..."
	@mkdir -p $(TAG_LIBS_PATH)
	@export objs_dir="`ls -F $(TAG_OBJS_PATH) | grep -oP '.*(?=/$$)' `"; \
		for d in $$objs_dir; do \
			echo $$d; \
			$(call build-static-library-by-dirs,$(TAG_LIBS_PATH)/$$d.a,$(TAG_OBJS_PATH)/$$d); \
			done
	@cp $(TAG_LIBS_PATH)/* -t $(OUTPUT_LIB_PATH)
	@echo "build none permissions libs done" 


# clean
npms_clean:
	@rm -rf $(NPMS_PATH)

npms_copy_objs: npms_unzip_libs
npms_os_static: npms_copy_objs
npms_os: npms_os_static

.PHONY: npms_os npms_os_static npms_copy_objs npms_unzip_libs npms_clean

