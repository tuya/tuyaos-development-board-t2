############################################################
# 需要外部传入的变量
############################################################

# 编译输出目录，默认为 xmake 同级的 output 目录
XMAKE_OUTDIR ?= $(dir $(lastword $(MAKEFILE_LIST)))/../../output

# 额外增加的编译选项，默认为空
XMAKE_STATIC_CFLAGS ?=
XMAKE_SHARED_CFLAGS ?=
XMAKE_SHARED_LDFLAGS ?=
XMAKE_EXECUTABLE_CFLAGS ?=
XMAKE_EXECUTABLE_LDFLAGS ?=

# 特殊变量定义
COMMA:= ,
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
QUOTA:= \"

############################################################
# 传出的变量和符号
############################################################

.PHONY: xmake_static xmake_shared xmake_executable xmake_out

############################################################
# 内部全局变量
############################################################

# xmake 所在目录
_XMAKE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# obj 文件目录
_XMAKE_OBJS_DIR := $(XMAKE_OUTDIR)/.objs

# info 文件目录
_XMAKE_INFO_DIR := $(XMAKE_OUTDIR)/.info

# 计算 TOPDIR
_XMAKE_TMP_PATH_LIST1 := $(shell d=$(_XMAKE_DIR);while [ $$d != '/' ];do d=`dirname $$d`;echo $$d;done)
_XMAKE_TMP_PATH_LIST2 := $(foreach p,$(_XMAKE_TMP_PATH_LIST1),$(patsubst $(p)/%,%,$(abspath .)))
_XMAKE_TMP_PATH := $(firstword $(_XMAKE_TMP_PATH_LIST2))
_XMAKE_TOPDIR ?= $(patsubst %/$(_XMAKE_TMP_PATH),%,$(abspath .))

############################################################
# 组件构建include宏
############################################################

CLEAR_VARS := $(_XMAKE_DIR)/xmake_clear.mk
BUILD_STATIC_LIBRARY := $(_XMAKE_DIR)/xmake_static.mk
BUILD_SHARED_LIBRARY := $(_XMAKE_DIR)/xmake_shared.mk
BUILD_EXECUTABLE := $(_XMAKE_DIR)/xmake_executable.mk
OUT_COMPILE_INFO := $(_XMAKE_DIR)/xmake_info.mk
BUILD_PACKAGE := $(_XMAKE_DIR)/xmake_package.mk

############################################################
# 实用函数
############################################################

# 当前 makefile 文件所在目录
define my-dir
$(strip	\
	$(eval _md_file_:=$$(lastword $$(MAKEFILE_LIST)))	\
	$(if $(filter $(CLEAR_VARS),$(_md_file_)),	\
		$(error LOCAL_PATH must be set before including $$(CLEAR_VARS))	\
		,	\
		$(patsubst %/,%,$(dir $(_md_file_)))	\
	)	\
)
endef

# 查找目录下所有源代码文件
define find-source-files
$(strip	\
	$(eval _file_paterns_:=$$(foreach d,$(1),$$(d)/*.c $$(d)/*.cc $$(d)/*.cpp $$(d)/*.S $$(d)/*.s))	\
	$(wildcard $(_file_paterns_))	\
)
endef

# 源代码路径补全
define source-add-prefix
$(foreach src,$(2),$(wildcard $(1)/$(src)) $(wildcard $(src)))
endef

# 生成源代码的依赖文件
define source-dependcies
$(strip	\
	$(patsubst $(_XMAKE_TOPDIR)/%,$(_XMAKE_OBJS_DIR)/static/%.o.d,$(abspath $(1)))	\
	$(patsubst $(_XMAKE_TOPDIR)/%,$(_XMAKE_OBJS_DIR)/shared/%.o.d,$(abspath $(1)))
)
endef

# 生成源代码的静态库目标文件
define source-static-objects
$(strip	\
	$(patsubst $(_XMAKE_TOPDIR)/%,$(_XMAKE_OBJS_DIR)/static/%.o,$(abspath $(1)))
)
endef

# 返回静态库目标文件根目录
define static-objects-dir
$(_XMAKE_OBJS_DIR)/static
endef

# 生成源代码的动态库目标文件
define source-shared-objects
$(strip \
	$(patsubst $(_XMAKE_TOPDIR)/%,$(_XMAKE_OBJS_DIR)/shared/%.o,$(abspath $(1)))
)
endef

# 生成源代码的可执行文件目标文件
define source-executable-objects
$(strip \
	$(patsubst $(_XMAKE_TOPDIR)/%,$(_XMAKE_OBJS_DIR)/exe/%.o,$(abspath $(1)))
)
endef

# 返回动态库目标文件根目录
define shared-objects-dir
$(_XMAKE_OBJS_DIR)/shared
endef

############################################################
# 通用目标生成规则
############################################################

define build-static-library-sections-by-dirs-files
	_objs=`for d in $(1); do find $$d -name *.o; done`;\
	for o in $(2); do for oo in $$_objs; do x=`echo $$oo | grep "$$o"`; if [ -n "$$x" ]; then echo "$(OBJCOPY) --prefix-alloc-sections $(3) $$oo"; $(OBJCOPY) --prefix-alloc-sections $(3) $$oo; fi; done; done;
	echo "------------------------------"
endef

define build-static-library
	#echo "building $@ ..."
	mkdir -p $(dir $(1))
	echo "AR -rcs $(1) $(2)"
	$(AR) -rcs $(1) $(2)
	echo "$(1) build done."
	echo "------------------------------"
endef

define build-static-library-by-dirs
	mkdir -p $(dir $(1)); \
	_objs=`for d in $(2); do find $$d -name *.o; done`;	\
		echo "AR -rcs $(1) $$_objs";	\
		$(AR) -rcs $(1) $$_objs; \
	echo "$(1) build done."; \
	echo "------------------------------"
endef

define build-shared-library
	#echo "building $@ ..."
	#echo "LDFLAGS: $(LDFLAGS) $(SHARED_LDFLAGS)"
	mkdir -p $(dir $(1))
	echo "CC -shared $(LDFLAGS) $(XMAKE_SHARED_LDFLAGS) $(PRIVATE_LDFLAGS) -rdynamic -Wl,-Bsymbolic -o $(1) $(2)"
	$(CC) -shared $(LDFLAGS) $(XMAKE_SHARED_LDFLAGS) $(PRIVATE_LDFLAGS) -rdynamic -Wl,-Bsymbolic -o $(1) $(2)
	echo "$(1) build done."
	echo "------------------------------"
endef

define build-shared-library-by-dirs
	mkdir -p $(dir $(1))
	_objs=`for d in $(2); do find $$d -name *.o; done`;	\
		echo "CC -shared $(LDFLAGS) $(XMAKE_SHARED_LDFLAGS) $(PRIVATE_LDFLAGS) -rdynamic -Wl,-Bsymbolic -o $(1) $$_objs";	\
		$(CC) -shared $(LDFLAGS) $(XMAKE_SHARED_LDFLAGS) $(PRIVATE_LDFLAGS) -rdynamic -Wl,-Bsymbolic -o $(1) $$_objs
	echo "$(1) build done."
	echo "------------------------------"
endef

define build-executable
	#echo "building $@ ..."
	#echo "LDFLAGS: $(LDFLAGS) $(SHARED_LDFLAGS)"
	mkdir -p $(dir $(1))
	echo "CXX -o $(1) -Wl,--start-group $(2) $(LDFLAGS) $(XMAKE_EXECUTABLE_LDFLAGS) $(PRIVATE_LDFLAGS) -Wl,--end-group"
	$(CXX) -o $(1) -Wl,--start-group $(2) $(LDFLAGS) $(XMAKE_EXECUTABLE_LDFLAGS) $(PRIVATE_LDFLAGS) -Wl,--end-group
	echo "$(1) build done."
	echo "------------------------------"
endef

define build-object
	echo "CC $(2)";
	#echo "CC $(CFLAGS) $(1) -o $@ -c $<"
	mkdir -p $(dir $(1));
	$(CC) $(CFLAGS) $(3) -D_THIS_FILE_NAME_=\"$(notdir $(2))\" -o $(1) -c $(2) || ( echo "compile command:"; echo "$(CC) $(CFLAGS) $(3) -o $(1) -c $(2)"; false )
	$(CC) $(CFLAGS) $(3) -MM $(2) -MT $(1) > $(1).d
	if [ -n "$(PRIVATE_SECTION_NAME)" ]; then \
		echo LOCAL_SECTION_NAME=$(PRIVATE_SECTION_NAME); \
		$(OBJCOPY) --prefix-alloc-sections $(PRIVATE_SECTION_NAME) $@ ;\
	fi
endef

define build-cxx-object
	echo "CXX $(2)";
	#echo "CXX $(CFLAGS) $(1) -o $@ -c $<"
	mkdir -p $(dir $(1));
	$(CXX) $(CXXFLAGS) $(3) -o $(1) -c $(2) || ( echo "compile command:"; echo "$(CXX) $(CXXFLAGS) $(3) -o $(1) -c $(2)"; false )
	$(CXX) $(CXXFLAGS) $(3) -MM $(2) -MT $(1) > $(1).d
endef

define clear-cxx-object-by-dir
	echo $(1)	
	for d in `ls -1d $(1)/*/`; do \
	    for o in `find $$d -name *.o`; do \
            so=$$(basename $$o);\
            rm -f $(1)/$$so;\
        done;\
    done
	echo "------------------------------"
endef

define out-json
	@echo "    \"$(1)\": \"$($(1))\"$(2)" >> $(3)
endef


#静态编译规则
$(_XMAKE_OBJS_DIR)/static/%.c.o: $(_XMAKE_TOPDIR)/%.c
	@$(call build-object,$@,$<,$(XMAKE_STATIC_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/static/%.cpp.o: $(_XMAKE_TOPDIR)/%.cpp
	@$(call build-cxx-object,$@,$<,$(XMAKE_STATIC_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/static/%.s.o: $(_XMAKE_TOPDIR)/%.s
	@$(call build-object,$@,$<,$(XMAKE_STATIC_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/static/%.S.o: $(_XMAKE_TOPDIR)/%.S
	@$(call build-object,$@,$<,$(XMAKE_STATIC_CFLAGS) $(PRIVATE_CFLAGS) -D__ASSEMBLER__)

#动态编译规则
$(_XMAKE_OBJS_DIR)/shared/%.c.o: $(_XMAKE_TOPDIR)/%.c
	@$(call build-object,$@,$<,$(XMAKE_SHARED_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/shared/%.cpp.o: $(_XMAKE_TOPDIR)/%.cpp
	@$(call build-cxx-object,$@,$<,$(XMAKE_SHARED_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/shared/%.s.o: $(_XMAKE_TOPDIR)/%.s
	@$(call build-object,$@,$<,$(XMAKE_SHARED_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/shared/%.S.o: $(_XMAKE_TOPDIR)/%.S
	@$(call build-object,$@,$<,$(XMAKE_SHARED_CFLAGS) $(PRIVATE_CFLAGS) -D__ASSEMBLER__)

#可执行程序编译规则
$(_XMAKE_OBJS_DIR)/exe/%.c.o: $(_XMAKE_TOPDIR)/%.c
	@$(call build-object,$@,$<,$(XMAKE_EXECUTABLE_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/exe/%.cpp.o: $(_XMAKE_TOPDIR)/%.cpp
	@$(call build-cxx-object,$@,$<,$(XMAKE_EXECUTABLE_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/exe/%.s.o: $(_XMAKE_TOPDIR)/%.s
	@$(call build-object,$@,$<,$(XMAKE_EXECUTABLE_CFLAGS) $(PRIVATE_CFLAGS))

$(_XMAKE_OBJS_DIR)/exe/%.S.o: $(_XMAKE_TOPDIR)/%.S
	@$(call build-object,$@,$<,$(XMAKE_EXECUTABLE_CFLAGS) $(PRIVATE_CFLAGS) -D__ASSEMBLER__)

