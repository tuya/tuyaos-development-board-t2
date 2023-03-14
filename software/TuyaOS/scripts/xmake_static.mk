
# LOCAL_PATH      := $(call my-dir)
# LOCAL_MODULE    := hello
# LOCAL_SRC_FILES := src/hello.c
# LOCAL_CFLAGS    := -I$(LOCAL_PATH)/include

# source list
_LOCAL_STATIC_SRC_LIST := $(call source-add-prefix,$(LOCAL_PATH),$(LOCAL_SRC_FILES))
-include $(call source-dependcies,$(_LOCAL_STATIC_SRC_LIST))

# static compile options
_LOCAL_STATIC_OBJ_LIST := $(call source-static-objects,$(_LOCAL_STATIC_SRC_LIST))
$(_LOCAL_STATIC_OBJ_LIST): PRIVATE_CFLAGS := $(LOCAL_CFLAGS)

# build static target
_LOCAL_STATIC_TARGET := $(XMAKE_OUTDIR)/lib/lib$(LOCAL_MODULE).a
xmake_static: $(_LOCAL_STATIC_TARGET)
$(_LOCAL_STATIC_TARGET): $(_LOCAL_STATIC_OBJ_LIST)
	@$(call build-static-library,$@,$^)

