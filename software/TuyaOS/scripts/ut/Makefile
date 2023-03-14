############################################################
# 全局环境变量配置
############################################################
TOPDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../)
-include $(TOPDIR)/build/tuya_iot.config
OUTPUT_DIR ?= $(TOPDIR)/output/ut

CFLAGS   := -g -Werror-implicit-function-declaration
CXXFLAGS := -g --std=c++11

COV ?= 1
LOG ?= 1

# gcov覆盖率检测
ifeq ($(COV), 1)
CFLAGS   += --coverage
CXXFLAGS += --coverage
XMAKE_EXECUTABLE_LDFLAGS += --coverage
endif

############################################################
# 默认编译目标
############################################################
all: ut_run
ifeq ($(COV), 1)
all: coverage
endif


############################################################
# 使用 xmake
############################################################
XMAKE_OUTDIR := $(OUTPUT_DIR)
_XMAKE_TOPDIR := $(TOPDIR)
-include $(TOPDIR)/scripts/mk/xmake.mk
TUYA_UT := 1
CFLAGS += -DTUYA_UT=$(TUYA_UT)
CXXFLAGS += -DTUYA_UT=$(TUYA_UT)


############################################################
# 包含所有SDK组件的local.mk 并 配置头文件路径
############################################################
TUYA_SDK_INC :=				 # 加载至CFLAGS中提供给其他组件使用；打包进SDK产物中；
TUYA_SDK_CFLAGS :=			 # 模块对外CFLAGS：其他组件编译时可感知到
-include $(shell find $(TOPDIR)/adapter $(TOPDIR)/components -name local.mk 2>/dev/null)
TUYA_SDK_INC_ALL_SUBDIRS := $(foreach dir,$(TUYA_SDK_INC),$(shell find $(dir) -type d))
TUYA_SDK_INC_ALL_SUBDIRS += $(TOPDIR)/include/base/include
CFLAGS += $(addprefix -I,$(TUYA_SDK_INC_ALL_SUBDIRS))
CFLAGS += $(TUYA_SDK_CFLAGS)
CXXFLAGS += $(addprefix -I,$(TUYA_SDK_INC_ALL_SUBDIRS))


############################################################
# 包含所有SDK组件的单元测试 ut.mk
############################################################
UT_CLEAR_VARS   := ut_clear.mk
UT_BUILD_TARGET := ut_target.mk
UT_TARGETS      :=
TUYA_UT_INC :=
-include $(shell find $(TOPDIR)/components -name ut.mk)
UT_INC_ALL_SUBDIRS := $(foreach dir,$(TUYA_UT_INC),$(shell find $(dir) -type d))
CXXFLAGS += $(addprefix -I,$(UT_INC_ALL_SUBDIRS))


############################################################
# ut 目标定义
############################################################
# 判断是否使用 gdb
UT_USE_GDB :=
ifeq ($(GDB),1)
ifneq ($(TEST),)
UT_USE_GDB := 1
endif
endif

# 参数判断
UT_ARG_FILTER :=
UT_ARG_REPEAT :=
UT_ARG_RUN_DISABLED :=
UT_ARG_PRINT_TIME :=
UT_ARG_OUTPUT :=
ifneq ($(FILTER),)
UT_ARG_FILTER := "--gtest_filter=$(FILTER)"
endif
ifneq ($(REPEAT),)
UT_ARG_FILTER := "--gtest_repeat=$(REPEAT)"
endif
ifeq ($(RUN_DISABLED),1)
UT_ARG_RUN_DISABLED := "--gtest_also_run_disabled_tests"
endif
ifeq ($(PRINT_TIME),0)
UT_ARG_PRINT_TIME := "--gtest_print_time=0"
endif
ifeq ($(LOG),1)
UT_ARG_OUTPUT := "--gtest_output=json:$(XMAKE_OUTDIR)/log/"
endif

# 单元测试运行规则定义
ut_run: prepare
ut_run/%: $(XMAKE_OUTDIR)/bin/ut_%
	@echo "-------- run `basename $^` unit test ------------------------"
ifeq ($(UT_USE_GDB),1)
	cd $(dir $^); gdb $(notdir $^)
else
	cd $(dir $^); ./$(notdir $^) $(UT_ARG_FILTER) $(UT_ARG_REPEAT) $(UT_ARG_RUN_DISABLED) $(UT_ARG_PRINT_TIME) $(UT_ARG_OUTPUT)
endif

ifeq ($(TEST),)
ut: $(UT_TARGETS)
ut_run: $(patsubst $(XMAKE_OUTDIR)/bin/ut_%,ut_run/%,$(UT_TARGETS))
else
ut: $(XMAKE_OUTDIR)/bin/ut_$(TEST)
ut_run: ut_run/$(TEST)
endif

############################################################
# 获取覆盖率
############################################################
coverage:
	make gcov
	make lcov

gcov:
	@_gcda=`find $(OUTPUT_DIR) -name "*.gcda"`; \
	_c=`echo $$_gcda | sed 's,\.gcda,\.c,g'`; echo $$_c; \
	_src=`echo $$_c | grep -oP "(?<=/)\w*\.\w*(?=\.c)"`; echo $$_src; \
	_gcov=`echo $$_src.gcov | sed 's, ,\.gcov ,g'`; echo $$_gcov; \
	gcov $$_c; \
	mkdir $(OUTPUT_DIR)/gcov; \
	cp -f $$_gcov -t $(OUTPUT_DIR)/gcov; \
	rm *.gcov

lcov:
	lcov -c -o locv.info -d $(OUTPUT_DIR)
	genhtml locv.info -o $(OUTPUT_DIR)/lcov
	mv locv.info $(OUTPUT_DIR)/lcov


############################################################
# ut 打包
############################################################
ifeq ($(UT_FILES_PATH_NAME),)
UT_FILES_PATH_NAME=ut_log
endif
UT_FILES_DIR=$(dir $(OUTPUT_DIR))/$(UT_FILES_PATH_NAME)
ut_files:
	@echo "====== build ut files begin ======="
	# 清空目录
	@rm -fr $(UT_FILES_DIR)
	@mkdir -p $(UT_FILES_DIR)
	@cp -rf $(OUTPUT_DIR)/log -t $(UT_FILES_DIR)
	@cp -rf $(OUTPUT_DIR)/lcov -t $(UT_FILES_DIR)
	@cp -rf $(OUTPUT_DIR)/gcov -t $(UT_FILES_DIR)
	@echo "------ build ut files end ---------"

ifeq ($(UT_PACKAGE_FULLNAME),)
UT_PACKAGE_FULLNAME := $(dir $(OUTPUT_DIR))/dist/$(UT_FILES_PATH_NAME).tar.gz
endif
ut_pack: ut_files
	@echo "====== ut pack begin ======="
	@rm -fr $(UT_FILES_DIR)/../ut.tar.gz
	@cd $(UT_FILES_DIR)/../ && tar zcvf ut.tar.gz $(UT_FILES_PATH_NAME)
	@rm -rf $(UT_PACKAGE_FULLNAME)
	@mkdir -p `dirname $(UT_PACKAGE_FULLNAME)`
	@mv $(UT_FILES_DIR)/../ut.tar.gz $(UT_PACKAGE_FULLNAME)
	@echo "------ ut pack end ---------"

############################################################
# 外部依赖
############################################################
prepare: prepare_gtest

prepare_gtest: externals/gtest/lib/libgtest.a

externals/gtest/lib/libgtest.a:
	@rm -fr externals/gtest
	@git clone https://registry.code.tuya-inc.top/embed_ci_space_group/gtest.git externals/gtest
	@git clone https://registry.code.tuya-inc.top/embed_ci_space_group/mockcpp.git externals/mockcpp

############################################################
# 其他目标和依赖关系
############################################################
.PHONY: all clean ut ut_run prepare prepare_gtest

# 清空中间文件
clean:
	rm -fr $(OUTPUT_DIR)

