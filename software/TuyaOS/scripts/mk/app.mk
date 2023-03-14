############################################################
# 基本变量
############################################################
APP_PROJ_PATH := $(ROOT_DIR)/apps/$(APP_NAME)
CATEGORY_PROJ_NAME := $(notdir $(ROOT_DIR))


############################################################
# 应用生成单个静态库
############################################################
# 生成目标文件列表
ifneq ($(APP_NAME),)
APP_STATIC_OBJS_DIRS := $(STATIC_OBJS_DIR)/apps/$(APP_NAME)
APP_LIB := $(OUTPUT_DIR)/lib/lib${APP_NAME}
endif
app_static:
	@mkdir -p $(APP_STATIC_OBJS_DIRS)
	@$(call build-static-library-by-dirs,$(APP_LIB).a,$(APP_STATIC_OBJS_DIRS))
	@cp $(APP_LIB).a $(APP_LIB).a.stripped
	@$(STRIP) --strip-debug $(APP_LIB).a.stripped
	@mkdir -p $(LIB_DIR_OUT)
	@cp $(APP_LIB).a $(LIB_DIR_OUT)/
	@cp $(APP_LIB).a.stripped $(LIB_DIR_OUT)/

APP_COMP_STATIC_OBJS_DIRS := $(STATIC_OBJS_DIR)/application_components
APP_COMP_EXTERN_LIB := $(APP_PROJ_PATH)/libs/libtuyaapp_components.a
app_comp_static:
	@mkdir -p $(APP_COMP_STATIC_OBJS_DIRS)
	@$(call build-static-library-by-dirs,$(OUTPUT_DIR)/lib/libtuyaapp_components.a,$(APP_COMP_STATIC_OBJS_DIRS))
	@cp $(OUTPUT_DIR)/lib/libtuyaapp_components.a $(OUTPUT_DIR)/lib/libtuyaapp_components.a.stripped
	@$(STRIP) --strip-debug $(OUTPUT_DIR)/lib/libtuyaapp_components.a.stripped
	@mkdir -p $(LIB_DIR_OUT)
	@cp $(OUTPUT_DIR)/lib/libtuyaapp_components.a $(LIB_DIR_OUT)/
	@cp $(OUTPUT_DIR)/lib/libtuyaapp_components.a.stripped $(LIB_DIR_OUT)/
	@if [ -f $(APP_COMP_EXTERN_LIB) ]; then \
		cp $(APP_COMP_EXTERN_LIB) $(LIB_DIR_OUT)/; \
		fi
	@if [ -f $(APP_COMP_EXTERN_LIB).stripped ]; then \
		cp $(APP_COMP_EXTERN_LIB).stripped $(LIB_DIR_OUT)/; \
		fi

APP_DRIV_STATIC_OBJS_DIRS := $(STATIC_OBJS_DIR)/application_drivers
APP_DRIV_EXTERN_LIB := $(APP_PROJ_PATH)/libs/libtuyaapp_drivers.a
app_driv_static:
	@mkdir -p $(APP_DRIV_STATIC_OBJS_DIRS)
	@$(call build-static-library-by-dirs,$(OUTPUT_DIR)/lib/libtuyaapp_drivers.a,$(APP_DRIV_STATIC_OBJS_DIRS))
	@cp $(OUTPUT_DIR)/lib/libtuyaapp_drivers.a $(OUTPUT_DIR)/lib/libtuyaapp_drivers.a.stripped
	@$(STRIP) --strip-debug $(OUTPUT_DIR)/lib/libtuyaapp_drivers.a.stripped
	@mkdir -p $(LIB_DIR_OUT)
	@cp $(OUTPUT_DIR)/lib/libtuyaapp_drivers.a $(LIB_DIR_OUT)/
	@cp $(OUTPUT_DIR)/lib/libtuyaapp_drivers.a.stripped $(LIB_DIR_OUT)/
	@if [ -f $(APP_DRIV_EXTERN_LIB) ]; then \
		cp $(APP_DRIV_EXTERN_LIB) $(LIB_DIR_OUT)/; \
		fi
	@if [ -f $(APP_DRIV_EXTERN_LIB).stripped ]; then \
		cp $(APP_DRIV_EXTERN_LIB).stripped $(LIB_DIR_OUT)/; \
		fi

app_adapter_static:
	@if [ ! -f $(OUTPUT_DIR)/lib/libtuyaos_adapter.a ]; then \
		$(AR) -rcs $(OUTPUT_DIR)/lib/libtuyaos_adapter.a; fi;
	@cp $(OUTPUT_DIR)/lib/libtuyaos_adapter.a $(LIB_DIR_OUT)

############################################################
# 应用产物
############################################################
APP_COMP_ROOT := application_components
APP_DRIV_ROOT := application_drivers
APP_COMP_NAMES := $(shell ls $(APP_COMP_ROOT) 2>/dev/null)
APP_DRIV_NAMES := $(shell ls $(APP_DRIV_ROOT) 2>/dev/null)

app:
	@_app_name=$(APP_NAME); \
	_app_version=$(APP_VER); \
	if [ -z "$${_app_name}" ]; then \
		_app_name=`sh ./scripts/get_sub_dir.sh apps`;        \
		if [ -z "$${_app_version}" ]; then \
			cd ./apps/$${_app_name}; \
			_app_version=`sh ../../scripts/get_ver_tag.sh`;        \
			cd -; \
		fi; \
	fi;\
	make app_by_name APP_NAME=$${_app_name} APP_VER=$${_app_version}

############################################################
# APP 打包
############################################################
# 生成APP包文件目录，主要用于开源输出，本地使用直接使用源码，不用包含本地的开发环境信息
APP_FILES_PATH_NAME=$(CATEGORY_PROJ_NAME)-$(APP_NAME)
APP_FILES_DIR=$(dir $(OUTPUT_DIR))/$(APP_FILES_PATH_NAME)
app_files:
	@echo "====== build app $(APP_NAME) files begin ======="
	
	# 清空目录
	@rm -fr $(APP_FILES_DIR)
	@mkdir -p $(APP_FILES_DIR)
	@mkdir -p $(APP_FILES_DIR)/libs
	#@mkdir -p $(APP_FILES_DIR)/build/config
	#@mkdir -p $(APP_FILES_DIR)/application_components
	@mkdir -p $(APP_FILES_DIR)/application_drivers
	
	# 复制开源组件
	@echo $(TUYA_APP_OPENSOURCE)
	@if [ -n  "$(TUYA_APP_OPENSOURCE)" ]; then\
		cp -a --parents $(TUYA_APP_OPENSOURCE) -t $(APP_FILES_DIR);\
	 fi

	# 复制驱动组件
	@if [ -d  "./application_drivers" ]; then\
		cp -a ./application_drivers -t $(APP_FILES_DIR);\
	 fi

	# 复制应用组件，包括源码，文档，local.mk等
	@cp ./apps/$(APP_NAME)/* $(APP_FILES_DIR)/ -rf
	
	# 复制闭源头文件
	@mkdir -p $(APP_FILES_DIR)/include 
	@if [ -n  "$(TUYA_SDK_INC)" ]; then\
		cp -a --parents $(TUYA_SDK_INC) $(APP_FILES_DIR)/include/ 2>/dev/null;\
	 fi

	# 复制闭源库文件
	# @cp ./libs/libtuyaapp_drivers.a $(APP_FILES_DIR)/libs 2>/dev/null
	# @cp ./libs/libtuyaapp_drivers.a.stripped $(APP_FILES_DIR)/libs	2>/dev/null
	@cp ./libs/libtuyaapp_components.a $(APP_FILES_DIR)/libs 2>/dev/null
	@cp ./libs/libtuyaapp_components.a.stripped $(APP_FILES_DIR)/libs 2>/dev/null
	
	# 复制配置文件tuya_app.config
	#@if [ -f "./build/tuya_app.config" ]; then\
	#	cp ./build/tuya_app.config $(APP_FILES_DIR)/build/ 2>/dev/null;\
	#fi

	# 清理一些文件
	@find $(APP_FILES_DIR) -name '.git' -type d | xargs rm -rf
	@find $(APP_FILES_DIR) -name '.gitignore' | xargs rm -rf
	@find $(APP_FILES_DIR) -name '.gitkeep' | xargs rm -rf
	@find $(APP_FILES_DIR) -name '*.yaml' | xargs rm -rf
	@rm $(APP_FILES_DIR)/include/apps -rf
	@rm $(APP_FILES_DIR)/output -rf
	@rm $(APP_FILES_DIR)/include/application_drivers -rf
	
	@echo "------ build app $(APP_NAME) files end ---------"

	
# 产物包全路径名称
ifneq ($(CI_PACKAGE_FULLNAME),)
PACKAGE_FULLNAME := $(CI_PACKAGE_FULLNAME)
else
PACKAGE_FULLNAME := $(dir $(OUTPUT_DIR))/dist/$(PROJECT_NAME)_$(SDK_FULL_VER).tar.gz
endif	

app_pack:
	@ _app_name=$(APP_NAME); \
	if [ -z "$${_app_name}" ]; then \
		_app_name=`sh ./scripts/get_sub_dir.sh apps`;	\
	fi; \
	make app_pack_by_name APP_NAME=$${_app_name} APP_PACK_FLAG=1

app_pack_by_name:
	@if [ -z "$(APP_NAME)" ]; then \
		echo "error: no app name !"; \
		exit 99; \
		fi
		
	@echo "====== pack app($(APP_NAME)) begin ======="
	@rm -fr $(APP_FILES_DIR)/../tuyaos_product.tar.gz
	@cd $(APP_FILES_DIR)/../ && tar zcvf tuyaos_product.tar.gz $(APP_FILES_PATH_NAME)
	@rm -fr $(PACKAGE_FULLNAME)
	@mkdir -p `dirname $(PACKAGE_FULLNAME)`
	@mv $(APP_FILES_DIR)/../tuyaos_product.tar.gz $(PACKAGE_FULLNAME)
	@echo "====== pack app($(APP_NAME)) end ======="
	
# 在当前环境中编译
ifeq ($(CONFIG_TUYAOS_BSP_ATTACH_SDK), y)
USER_CMD = $(CONFIG_TUYAOS_BSP_ATTACH_SDK_NAME)
else
USER_CMD = all
endif

USER_SW_VER=1.0.0
ifneq ($(APP_VER),)
USER_SW_VER=$(APP_VER)
endif

app_by_name:
	@if [ -z "$(APP_NAME)" ]; then \
		echo "error: no app name !"; \
		exit 99; \
		fi
	@echo "====== build app($(APP_NAME)) begin ======="
	
	# 开始编译app
	@mkdir -p apps/$(APP_NAME)/output 2>/dev/null
	@if [ -e $(TUYAOS_BUILD_PATH)/build_path ]; then \
		. $(TUYAOS_BUILD_PATH)/build_path; \
	fi; \
	_demos=$(APP_NAME);	\
	_app_path=apps/$${_demos};	\
		echo $${_app_path};	\
		cd ./apps/$${_demos}/; \
		if [ -f build.sh ]; then	\
			sh ./build.sh $${_demos} $(USER_SW_VER) $(TARGET_PLATFORM) $${_app_path} $(USER_CMD);	\
		elif [ -f ${TUYAOS_BUILD_PATH}/$${TUYA_APPS_BUILD_PATH}/$${TUYA_APPS_BUILD_CMD} ]; then	\
		    cd ${TUYAOS_BUILD_PATH}/$${TUYA_APPS_BUILD_PATH};	\
		    sh $${TUYA_APPS_BUILD_CMD} $${_demos} $(USER_SW_VER) $(TARGET_PLATFORM) $${_app_path} $(USER_CMD);	\
		elif [ -f Makefile -o -f makefile ]; then	\
			make APP_BIN_NAME=$${_demos} USER_SW_VER=$(USER_SW_VER) APP_PATH=$${_app_path} USER_CMD=$(USER_CMD);	\
		else	\
		    echo "No Build Command!";\
		fi
	@echo "------ build app($(APP_NAME)) end ---------"

app_clean_by_name:
	@if [ -z "$(APP_NAME)" ]; then \
		echo "error: no app name !"; \
		exit 99; \
		fi
	@echo "====== clean app($(APP_NAME)) begin ======="

	# 开始清理app
	@rm -fr $(OUTPUT_DIR)
	@if [ -e $(TUYAOS_BUILD_PATH)/build_path ]; then \
		. $(TUYAOS_BUILD_PATH)/build_path; \
	fi; \
	_demos=$(APP_NAME);	\
	_app_path=apps/$${_demos};	\
		echo $${_app_path};	\
		cd ./apps/$${_demos}/; \
		if [ -f build.sh ]; then	\
			sh ./build.sh $${_demos} $(USER_SW_VER) $(TARGET_PLATFORM) $${_app_path} $(USER_CMD);	\
		elif [ -f ${TUYAOS_BUILD_PATH}/$${TUYA_APPS_BUILD_PATH}/$${TUYA_APPS_BUILD_CMD} ]; then	\
		    cd ${TUYAOS_BUILD_PATH}/$${TUYA_APPS_BUILD_PATH};	\
		    sh $${TUYA_APPS_BUILD_CMD} $${_demos} $(USER_SW_VER) $(TARGET_PLATFORM) $${_app_path} $(USER_CMD);	\
		elif [ -f Makefile -o -f makefile ]; then	\
			make APP_BIN_NAME=$${_demos} USER_SW_VER=$(USER_SW_VER) APP_PATH=$${_app_path} USER_CMD=$(USER_CMD);	\
		else	\
		    echo "No Clean Command!";\
		fi
	@echo "------ clean app($(APP_NAME)) end ---------"


app_static_clean:
	@echo "clean application static ..."
	@rm -rf $(STATIC_OBJS_DIR)/apps
	@rm -rf $(APP_COMP_STATIC_OBJS_DIRS)
	@rm -rf $(APP_DRIV_STATIC_OBJS_DIRS)


############################################################

app_by_name: app_comp_static app_driv_static app_adapter_static app_static
app_pack_by_name: app_files
app_files: app_comp_static app_driv_static
app_driv_static:xmake_static
app_comp_static:xmake_static
ifeq ($(SDK_ENV),1)
app: os_files
endif

.PHONY: app_files app_by_name app_pack_by_name
