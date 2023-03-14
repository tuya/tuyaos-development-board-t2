
# LOCAL_PATH      := $(call my-dir)
# LOCAL_MODULE    := hello
# LOCAL_SRC_FILES := src/hello.c
# LOCAL_CFLAGS    := -I$(LOCAL_PATH)/include

# source list
_LOCAL_EXECUTABLE_SRC_LIST := $(call source-add-prefix,$(LOCAL_PATH),$(LOCAL_SRC_FILES))
-include $(call source-dependcies,$(_LOCAL_EXECUTABLE_SRC_LIST))

# executable compile options
_LOCAL_EXCUTABLE_OBJ_LIST := $(call source-executable-objects,$(_LOCAL_EXECUTABLE_SRC_LIST))
$(_LOCAL_EXCUTABLE_OBJ_LIST): PRIVATE_CFLAGS := $(LOCAL_CFLAGS)

# build executable target
_LOCAL_EXECUTABLE_TARGET := $(XMAKE_OUTDIR)/bin/$(LOCAL_MODULE)
$(_LOCAL_EXECUTABLE_TARGET): PRIVATE_LDFLAGS := $(LOCAL_LDFLAGS)
xmake_executable: $(_LOCAL_EXECUTABLE_TARGET)
$(_LOCAL_EXECUTABLE_TARGET): $(_LOCAL_EXCUTABLE_OBJ_LIST)
	@$(call build-executable,$@,$^)

