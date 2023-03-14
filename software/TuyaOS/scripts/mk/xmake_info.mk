
# LOCAL_MODULE          := hello
# LOCAL_PATH            := $(call my-dir)
# LOCAL_SRC_FILES       := src/hello.c
# LOCAL_CFLAGS          := -I$(LOCAL_PATH)/inc
# LOCAL_TUYA_SDK_INC    := $(LOCAL_PATH)/include
# LOCAL_TUYA_SDK_CFLAGS := -DDebug=1


# out compile information
_LOCAL_OUT_TARGET := $(_XMAKE_INFO_DIR)/$(LOCAL_MODULE).json
$(_LOCAL_OUT_TARGET): LOCAL_MODULE := $(LOCAL_MODULE)
$(_LOCAL_OUT_TARGET): LOCAL_PATH := $(LOCAL_PATH)
$(_LOCAL_OUT_TARGET): LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
$(_LOCAL_OUT_TARGET): LOCAL_CFLAGS := $(LOCAL_CFLAGS)
$(_LOCAL_OUT_TARGET): LOCAL_TUYA_SDK_INC := $(LOCAL_TUYA_SDK_INC)
$(_LOCAL_OUT_TARGET): LOCAL_TUYA_SDK_CFLAGS := $(LOCAL_TUYA_SDK_CFLAGS)


xmake_out: $(_LOCAL_OUT_TARGET)
$(_LOCAL_OUT_TARGET):
	@mkdir -p `dirname $@`
	@echo "{" > $@
	@$(call out-json,LOCAL_MODULE,$(COMMA),$@)
	@$(call out-json,LOCAL_PATH,$(COMMA),$@)
	@$(call out-json,LOCAL_SRC_FILES,$(COMMA),$@)
	@$(call out-json,LOCAL_CFLAGS,$(COMMA),$@)
	@$(call out-json,LOCAL_TUYA_SDK_INC,$(COMMA),$@)
	@$(call out-json,LOCAL_TUYA_SDK_CFLAGS,$(EMPTY),$@)
	@echo "}" >> $@

