# tdd_ir_driver

该组件 local.mk 已经过修改，不会再查找组件下所有 .c 文件，只使用与芯片平台对应的驱动文件。
对应变动：
```makefile
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/tdd_ir_driver_$(TARGET_PLATFORM).c
```

所以再对驱动源文件进行命名时需要遵守 tdd_ir_driver_芯片平台.c 的命名方式，这里的芯片平台 (TARGET_PLATFORM) 是在 `build_app.sh` 中进行声明的。
