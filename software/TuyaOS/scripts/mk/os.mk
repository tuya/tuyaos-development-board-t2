############################################################
# 生成单个静态库
############################################################
# 生成目标文件列表
STATIC_OBJS_DIR := $(call static-objects-dir)
STATIC_OBJS_DIR_TMP := $(STATIC_OBJS_DIR)_tmp
OS_STATIC_OBJS_DIRS := $(STATIC_OBJS_DIR)/components $(STATIC_OBJS_DIR)/adapter
LIB_DIR_OUT := $(ROOT_DIR)/libs

# 生成命令
os_static:
	@mkdir -p $(OS_STATIC_OBJS_DIRS)
	@rm -rf $(STATIC_OBJS_DIR_TMP)
	@cp -ar $(STATIC_OBJS_DIR) $(STATIC_OBJS_DIR_TMP)
	# 去除分包编译内容
	@for s in $(OS_SUB_LIB_INFO); do \
		_need=$${s#*:}; \
		_need_full=`echo $$_need | sed -r "s, |^, $(STATIC_OBJS_DIR)\/,g"`; \
		rm -rf $$_need_full; \
		done
	@$(call build-static-library-sections-by-dirs-files,$(OS_STATIC_OBJS_DIRS),$(MULTI_SECION_FILES),$(CONFIG_MULTI_SECION_NAME))
	@$(call build-static-library-by-dirs,$(OUTPUT_DIR)/lib/libtuyaos.a,$(OS_STATIC_OBJS_DIRS))
	@cp $(OUTPUT_DIR)/lib/libtuyaos.a $(OUTPUT_DIR)/lib/libtuyaos.a.stripped
	@$(STRIP) --strip-debug $(OUTPUT_DIR)/lib/libtuyaos.a.stripped
	@mkdir -p $(LIB_DIR_OUT)
	@cp $(OUTPUT_DIR)/lib/libtuyaos.a $(LIB_DIR_OUT)/
	@cp $(OUTPUT_DIR)/lib/libtuyaos.a.stripped $(LIB_DIR_OUT)/
	@if [ ! -f $(OUTPUT_DIR)/lib/libtuyaos_adapter.a ]; then \
		$(AR) -rcs $(OUTPUT_DIR)/lib/libtuyaos_adapter.a; fi;
	@cp $(OUTPUT_DIR)/lib/libtuyaos_adapter.a  $(LIB_DIR_OUT)
	@rm -rf $(STATIC_OBJS_DIR)
	@mv $(STATIC_OBJS_DIR_TMP) $(STATIC_OBJS_DIR)

os_sub_static:
	@mkdir -p $(LIB_DIR_OUT)
	@for s in $(OS_SUB_LIB_INFO); do \
		_tag=$${s%%:*}; \
		_tag_full=$(XMAKE_OUTDIR)/lib/lib$$_tag.a; \
		_need=$${s#*:}; \
		_need_full=`echo $$_need | sed -r "s, |^, $(STATIC_OBJS_DIR)\/,g"`; \
		$(call build-static-library-by-dirs,$$_tag_full,$$_need_full); \
		cp $$_tag_full $(LIB_DIR_OUT)/; \
		done

############################################################
# 生成单个动态库
############################################################
# 生成目标文件列表
SHARED_OBJS_DIR := $(call shared-objects-dir)
OS_SHARED_OBJS_DIRS := $(SHARED_OBJS_DIR)
SO_DIR_OUT := $(ROOT_DIR)/libs

# 排除目标文件列表，某些平台特殊需求，需要把某些组件的.o排除在涂鸦IoT SDK之外
OS_SHARED_OBJS_EXCLUDE := $(subst ",, $(CONFIG_SDK_SHARED_OBJS_EXCLUDE)) #将"替换
OS_SHARED_OBJS_EXCLUDE_DIR := $(addprefix $(OS_SHARED_OBJS_DIRS)/components/,$(OS_SHARED_OBJS_EXCLUDE))

# 生成命令
os_shared:
	@rm -rf $(OS_SHARED_OBJS_EXCLUDE_DIR)
	@mkdir -p $(OS_SHARED_OBJS_DIRS)
	@$(call build-shared-library-by-dirs,$(OUTPUT_DIR)/lib/libtuyaos.so,$(OS_SHARED_OBJS_DIRS))
	@cp $(OUTPUT_DIR)/lib/libtuyaos.so $(OUTPUT_DIR)/lib/libtuyaos.so.stripped
	@$(STRIP) --strip-debug $(OUTPUT_DIR)/lib/libtuyaos.so.stripped
	@mkdir -p $(SO_DIR_OUT)
	@cp $(OUTPUT_DIR)/lib/libtuyaos.so $(SO_DIR_OUT)/
	@cp $(OUTPUT_DIR)/lib/libtuyaos.so.stripped $(SO_DIR_OUT)/
	@if [ ! -f $(OUTPUT_DIR)/lib/libtuyaos_adapter.so ]; then \
		touch $(OUTPUT_DIR)/lib/libtuyaos_adapter.so; fi ;
	@cp $(OUTPUT_DIR)/lib/libtuyaos_adapter.so  $(SO_DIR_OUT);


############################################################
# OS 打包
############################################################
# 生成OS包文件目录，主要用于开源输出，本地使用直接使用源码，不用包含本地的开发环境信息
OS_FILES_PATH_NAME=TuyaOS
OS_FILES_DIR=$(dir $(OUTPUT_DIR))/$(OS_FILES_PATH_NAME)
os_files:
	@echo "====== build OS files begin ======="
	# 清空目录
	@rm -fr $(OS_FILES_DIR)
	@mkdir -p $(OS_FILES_DIR)

	# 复制头文件
	@cp -a --parents $(TUYA_SDK_INC) -t ./include
	
	# copy需要输出的目录到目标
	@mkdir -p $(OS_FILES_DIR)/build
	@mkdir -p $(OS_FILES_DIR)/libs
	@cp -fr ./build/tuya_iot.config $(OS_FILES_DIR)/build
	@if [ -f $(ABLITY_JSON_PATH) ]; then cp $(ABLITY_JSON_PATH) $(OS_FILES_DIR)/build; fi
	@cp -fr ./build/build_param.template $(OS_FILES_DIR)/build
	@rm -fr ./include/vendor/ -rf
	@cp -fr ./include $(OS_FILES_DIR)/
	@cp -fr ./libs/libtuyaos.*	$(OS_FILES_DIR)/libs/
	@if [ -f ./libs/libCHIP.a ]; then cp -fr ./libs/libCHIP.a $(OS_FILES_DIR)/libs/; fi
	@cp -fr ./scripts $(OS_FILES_DIR)

	# copy分包编译产物
	@if [ -f ./build/sub_lib.mk ]; then cp ./build/sub_lib.mk $(OS_FILES_DIR)/build; fi
	@for s in $(OS_SUB_LIB_INFO); do \
		_tag=$${s%%:*}; \
		_tag_full=./libs/lib$$_tag.a; \
		cp $$_tag_full  $(OS_FILES_DIR)/libs/; done

	# 检查源码是否输出
	@src=`find $(OS_FILES_DIR)/include  -name "*.c" -o -name "*.cpp" -o -name "*.cc"`; \
	if [ ! x"$$src" = x"" ]; then \
		echo ">>>>>>>>>> Delete C files in include <<<<<<<<<<"; \
		echo "$${src}"; \
		find $(OS_FILES_DIR)/include  -name "*.c" -o -name "*.cpp" -o -name "*.cc" | xargs rm; \
	fi

	# 用户开发环境预备，保留空文件夹
	@mkdir -p $(OS_FILES_DIR)/apps

	# 工具链不需要输出
	# 文档信息
	@cp CHANGELOG.md $(OS_FILES_DIR)/
	@cp README.md $(OS_FILES_DIR)/
	@cp LICENSE $(OS_FILES_DIR)/
	
	# 生成构建脚本
	@cp ./scripts/build_app.user.sh $(OS_FILES_DIR)/build_app.sh
	@cp ./scripts/prepare_app.user.sh $(OS_FILES_DIR)/prepare_app.sh
	@cp ./Makefile $(OS_FILES_DIR)/Makefile	
	@sed -i 's,####TARGET_PLATFORM####,$(TARGET_PLATFORM),g' $(OS_FILES_DIR)/build_app.sh
	@sed -i 's,####TARGET_PLATFORM_REPO####,$(TARGET_PLATFORM_REPO),g' $(OS_FILES_DIR)/build_app.sh
	@sed -i 's,####TARGET_PLATFORM_VERSION####,$(TARGET_PLATFORM_VERSION),g' $(OS_FILES_DIR)/build_app.sh
	@chmod a+x $(OS_FILES_DIR)/build_app.sh	

	# 清理一些文件
	@find $(OS_FILES_DIR) -name '.git' -type d | xargs rm -rf
	@find $(OS_FILES_DIR) -name '.gitignore' | xargs rm -rf
	@find $(OS_FILES_DIR) -name '.gitkeep' | xargs rm -rf
	@find $(OS_FILES_DIR) -name '*.yaml' | xargs rm -rf
	@find $(OS_FILES_DIR) -name 'local.mk' | xargs rm -rf
	
	# copy必要的adapter目录（系统驱动和系统接口）
	#@cp -fr ./adapter $(OS_FILES_DIR)/
	@rm -fr $(OS_FILES_DIR)/include/adapter/cellular_catx
	@rm -fr $(OS_FILES_DIR)/include/adapter/nb
	@rm -fr $(OS_FILES_DIR)/include/adapter/zigbee
	@rm -fr $(OS_FILES_DIR)/include/adapter/subg
	@if [ -z "$(CONFIG_ENABLE_DISPLAY)" ]; then rm -fr $(OS_FILES_DIR)/include/adapter/display; fi
	@if [ -z "$(CONFIG_ENABLE_MEDIA)" ]; then rm -fr $(OS_FILES_DIR)/include/adapter/media; fi
	@if [ -z "$(CONFIG_ENABLE_STORAGE)" ]; then rm -fr $(OS_FILES_DIR)/include/adapter/storage; fi

	
	@echo "------ build OS files end ---------"


# kernel 打包
# 产物包全路径名称
ifneq ($(CI_PACKAGE_FULLNAME),)
PACKAGE_FULLNAME := $(CI_PACKAGE_FULLNAME)
else
PACKAGE_FULLNAME := $(dir $(OUTPUT_DIR))/dist/$(PROJECT_NAME)_$(SDK_FULL_VER).tar.gz
endif

os_pack:
	@echo "====== pack begin ======="
	@rm -fr $(OS_FILES_DIR)/../tuyaos.tar.gz
	@cd $(OS_FILES_DIR)/../ && tar zcvf tuyaos.tar.gz $(OS_FILES_PATH_NAME)
	@rm -fr $(PACKAGE_FULLNAME)
	@mkdir -p `dirname $(PACKAGE_FULLNAME)`
	@mv $(OS_FILES_DIR)/../tuyaos.tar.gz $(PACKAGE_FULLNAME)
	@echo "------ pack end ---------"


pack: os_pack
os_pack: os_files tydoc
os_files: os
ifeq ($(BUILD_STATIC), 1)
os: os_static
endif
ifeq ($(BUILD_SHARED), 1)
os: os_shared
endif
os_static: xmake_static os_sub_static
os_shared: xmake_shared

.PHONY: kernel kernel_static kernel_shared kernel_files kernel_pack pack os
