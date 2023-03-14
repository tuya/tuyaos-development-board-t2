
# LOCAL_PATH      := $(call my-dir)
# LOCAL_MODULE    := hello
# LOCAL_SRC_FILES := src/hello.c
# LOCAL_CFLAGS    := -I$(LOCAL_PATH)/include

# source list
_LOCAL_SHARED_SRC_LIST := $(call source-add-prefix,$(LOCAL_PATH),$(LOCAL_SRC_FILES))
-include $(call source-dependcies,$(_LOCAL_SHARED_SRC_LIST))

# shared compile options
_LOCAL_SHARED_OBJ_LIST := $(call source-shared-objects,$(_LOCAL_SHARED_SRC_LIST))
$(_LOCAL_SHARED_OBJ_LIST): PRIVATE_CFLAGS := $(LOCAL_CFLAGS)
$(_LOCAL_SHARED_OBJ_LIST): PRIVATE_LDFLAGS := $(LOCAL_LDFLAGS)

# build shared target
_LOCAL_SHARED_TARGET := $(XMAKE_OUTDIR)/lib/lib$(LOCAL_MODULE).so
xmake_shared: $(_LOCAL_SHARED_TARGET)
$(_LOCAL_SHARED_TARGET): $(_LOCAL_SHARED_OBJ_LIST)
	@$(call build-shared-library,$@,$^)

