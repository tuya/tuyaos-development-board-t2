# 当前文件所在目录
LOCAL_PATH := $(call my-dir)

#---------------------------------------

# 清除 LOCAL_xxx 变量
include $(CLEAR_VARS)

# 当前模块名
LOCAL_MODULE := $(notdir $(LOCAL_PATH))

# 模块源代码
LOCAL_SRC_FILES := $(shell find $(LOCAL_PATH)/src -name "*.c" -o -name "*.cpp" -o -name "*.cc")

# 模块的 CFLAGS
LOCAL_CFLAGS := -I$(LOCAL_PATH)/include
LOCAL_CFLAGS += -DUSER_SW_VER=\"$(APP_VER)\" -DAPP_BIN_NAME=\"$(APP_NAME)\"

# 生成静态库
include $(BUILD_STATIC_LIBRARY)

# 生成动态库
include $(BUILD_SHARED_LIBRARY)

#---------------------------------------

