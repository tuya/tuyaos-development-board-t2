############################################################
# 默认编译目标
############################################################
all: app

############################################################
# 使用 xmake、配置文件、环境标识、参数
############################################################
include build/build_param
XMAKE_OUTDIR := $(OUTPUT_DIR)
include scripts/mk/xmake.mk
include $(_XMAKE_DIR)/definitions.mk
-include $(_XMAKE_DIR)/env.mk
-include build/tuya_iot.config
-include vendor/$(TARGET_PLATFORM)/tuyaos/tuyaos_kernel.config

############################################################
# 包含开发环境定义
############################################################
-include $(TUYAOS_BUILD_PATH)/platform.mk
-include build/sub_lib.mk
CFLAGS += $(TUYA_PLATFORM_CFLAGS)

############################################################
# 包含所有Adapter、components组件的local.mk 并配置头文件路径
############################################################
-include $(shell find adapter components  -name local.mk 2>/dev/null)
-include vendor/$(TARGET_PLATFORM)/tuyaos/tuyaos_adapter/local.mk
ifneq ($(APP_NAME),)
-include apps/$(APP_NAME)/local.mk
-include apps/$(APP_NAME)/build/tuya_app.config
-include $(shell find application_components application_drivers -name local.mk 2>/dev/null)
endif

TUYA_SDK_INC_ALL_SUBDIRS := $(foreach dir,$(TUYA_SDK_INC),$(shell find $(dir) -type d))
TUYA_SDK_INC_ALL_SUBDIRS += $(shell find include -type d)
TUYA_APP_OPENSOURCE_ALL_SUBDIRS := $(foreach dir,$(TUYA_APP_OPENSOURCE_INC),$(shell find $(dir) -type d))
TUYA_APP_OPENSOURCE_ALL_SUBDIRS += $(shell find include -type d)
CFLAGS += $(addprefix -I,$(TUYA_SDK_INC_ALL_SUBDIRS))
CFLAGS += $(addprefix -I,$(TUYA_APP_OPENSOURCE_ALL_SUBDIRS))
CFLAGS += $(TUYA_SDK_CFLAGS)


-include $(_XMAKE_DIR)/info.mk
-include $(_XMAKE_DIR)/config.mk
-include $(_XMAKE_DIR)/os.mk
-include $(_XMAKE_DIR)/app.mk
-include $(_XMAKE_DIR)/none_permissions.mk
-include $(_XMAKE_DIR)/doc.mk
-include $(_XMAKE_DIR)/ut.mk

-include $(_XMAKE_DIR)/help.mk


############################################################
# 其他目标和依赖关系
############################################################
.PHONY: all clean os_clean

# 清空中间文件
clean:
	rm -fr $(OUTPUT_DIR)

os_clean: clean
	rm -fr $(LIB_DIR_OUT)
	rm -fr $(ROOT_DIR)/include/*



