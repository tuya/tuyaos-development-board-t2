############################################################
# 内部使用的环境定义
############################################################
HAS_PYTHON3 := $(shell which python3)
SDK_ENV := $(shell [ -f make.yaml ] && echo "1")
PROD_ENV := $(shell [ -f cluster.yaml ] && echo "1")

TUYA_SDK_INC :=				 # 加载至CFLAGS中提供给其他组件使用；打包进SDK产物中；
TUYA_APP_OPENSOURCE_INC :=	 # 加载至CFLAGS中提供给其他组件使用；不打包进SDK产物中；
TUYA_SDK_CFLAGS :=			 # 模块对外CFLAGS：其他组件编译时可感知到
TUYA_SDK_DOCS :=			 # 模块对外DOCS：组件对外输出文档
TUYA_APP_OPENSOURCE :=		 # 模块对外开源
OS_SUB_LIB_INFO :=		 	 # 分包编译支持
MULTI_SECION_FILES :=		 # 分段差分文件
