_LOCAL_PKG := $(abspath $(LOCAL_PATH))
_LOCAL_PKG_OUT := $(_XMAKE_OBJS_DIR)/package/$(LOCAL_PKG_NAME)
_LOCAL_PKG_TARGET_PREPARE := $(_LOCAL_PKG_OUT)/.tuya-prepared
_LOCAL_PKG_TARGET_CONF := $(_LOCAL_PKG_OUT)/.tuya-configured
_LOCAL_PKG_TARGET_BUILD := $(_LOCAL_PKG_OUT)/.tuya-build
_LOCAL_PKG_DEPS := \
	$(if $(wildcard $(_LOCAL_PKG)/local.mk), \
		$(_LOCAL_PKG)/local.mk, $(_LOCAL_PKG)/app.mk) \
	$(if $(wildcard $(_LOCAL_PKG_TARGET_BUILD)), \
		$(shell find -L "$(_LOCAL_PKG)/" -follow \
		-cnewer "$(_LOCAL_PKG_TARGET_BUILD)" -type f \
		) \
	) \
	$(if $(strip $(LOCAL_PKG_DEPS)), \
		$(foreach DEPS,$(LOCAL_PKG_DEPS),$(_XMAKE_OBJS_DIR)/package/$(DEPS)/.tuya-build) \
	) \
	$(if $(strip $(LOCAL_STATIC_LIBRARIES)), \
		$(foreach LIB,$(LOCAL_STATIC_LIBRARIES),$(abspath $(XMAKE_OUTDIR))/lib/$(LIB).a) \
	) \
	$(if $(strip $(LOCAL_SHARED_LIBRARIES)), \
		$(foreach LIB,$(LOCAL_SHARED_LIBRARIES),$(abspath $(XMAKE_OUTDIR))/lib/$(LIB).so) \
	)

######## Varibles ###################
TARGET_SYSROOT := $(strip $(shell readlink -f `$(COMPILE_PREX)gcc --print-sysroot`))
CROSS_TARGET := $(notdir $(COMPILE_PREX))
CROSS_TARGET := $(CROSS_TARGET:-=)

# Make sure pkg-config doesn't look outside of the tree
unexport PKG_CONFIG_PATH
unexport PKG_CONFIG_LIBDIR
unexport PKG_CONFIG_SYSROOT_DIR
export PKG_CONFIG := 
export PKG_CONFIG_PATH := 
export PKG_CONFIG_LIBDIR := 

# export PKG_CONFIG_SYSROOT_DIR := $(TARGET_SYSROOT)
# $(error "LOCAL_PKG_OPTS=$(LOCAL_PKG_OPTS)  PKG_CONFIG=$(PKG_CONFIG) PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) PKG_CONFIG_LIBDIR=$(PKG_CONFIG_LIBDIR)")


###########################################################
## Strip quotes and then whitespaces
## $(1): a var to strip
###########################################################
qstrip = $(strip $(subst \",,$(1)))
#"))

###########################################################
## Needed for the foreach loops to loop over the list of hooks, so that
## each hook call is properly separated by a newline.
###########################################################
define sep


endef

###########################################################
## apply patch for package
## $(1): package name
## $(2): patch file path
###########################################################
define apply_patch
	patch -g0 -p1 -E -d "$($(1)_src)" -t -N < $(2) || exit 1
	echo $(2) >> $($(1)_src)/.applied_patches_list
endef

define do_bootstrap
	cd $(1) ;\
		./bootstrap
endef

###########################################################
## Autotools configure function
## $(1): package name
###########################################################
define tuya_autotools_configure
	$(if $(wildcard $($(1)_src)/bootstrap),$(call do_bootstrap,$($(1)_src)))
	cd $($(1)_build); \
	CC="$(CC)" \
	CFLAGS="$(PRIVATE_CFLAGS)" \
	CXXFLAGS="$(PRIVATE_CXXFLAGS)" \
	LDFLAGS="$(PRIVATE_LDFLAGS)" \
	$($(1)_src)/configure \
		--target=$(CROSS_TARGET) \
		--host=$(CROSS_TARGET) \
		--prefix=$(XMAKE_OUTDIR) \
		$($(1)_option)
endef

###########################################################
## CMake configure function
## $(1): package name
###########################################################
define tuya_cmake_configure
	@sed \
		-e 's#@@SYSROOT@@#$(TARGET_SYSROOT)#' \
		-e 's#@@CC@@#$(CC)#' \
		-e 's#@@CXX@@#$(CXX)#' \
		-e 's#@@CFLAGS@@#$(PRIVATE_CFLAGS)#' \
		-e 's#@@CXXFLAGS@@#$(PRIVATE_CXXFLAGS)#' \
		-e 's#@@LDFLAGS@@#$(PRIVATE_LDFLAGS)#' \
		$(_XMAKE_DIR)/toolchainfile.cmake.in \
		> $($(1)_build)/cross-compilation.conf
	cd $($(1)_build); \
	rm -f CMakeCache.txt; \
	cmake $($(1)_src) \
		-DCMAKE_TOOLCHAIN_FILE="$($(1)_build)/cross-compilation.conf" \
		-DCMAKE_INSTALL_PREFIX="/$(XMAKE_OUTDIR)" \
		-DCMAKE_COLOR_MAKEFILE=OFF \
		-DBUILD_DOC=OFF \
		-DBUILD_DOCS=OFF \
		-DBUILD_EXAMPLE=OFF \
		-DBUILD_EXAMPLES=OFF \
		-DBUILD_TEST=OFF \
		-DBUILD_TESTS=OFF \
		-DBUILD_TESTING=OFF \
		-DBUILD_SHARED_LIBS=ON \
		$($(1)_option)
endef


###############################################
# default functions
# $(1): package name
# $(2): type (target or host), not used now
###############################################
$(LOCAL_PKG_NAME)_src := $(if $(LOCAL_PKG_SRC),$(LOCAL_PKG_SRC),$(_LOCAL_PKG))
$(LOCAL_PKG_NAME)_build := $(_LOCAL_PKG_OUT)
$(LOCAL_PKG_NAME)_option := $(LOCAL_PKG_OPTS)
$(LOCAL_PKG_NAME)_patches := $(if $(LOCAL_PKG_PATCHES),$(call source-add-prefix,$(LOCAL_PATH),$(LOCAL_PKG_PATCHES)))
$(LOCAL_PKG_NAME)_cflags :=  $(LOCAL_CFLAGS)
$(LOCAL_PKG_NAME)_ldflags :=  $(LOCAL_LDFLAGS)

ifndef prepare/$(LOCAL_PKG_NAME)
define prepare/$(LOCAL_PKG_NAME)
	@mkdir -p $($(1)_src)
	@rm -rf $($(1)_src)/.applied_patches_list
	$(if $($(1)_patches), \
		$(foreach p,$($(1)_patches),$(call apply_patch,$(1),$(p))$(sep)) \
	)
endef
endif

ifndef configure/$(LOCAL_PKG_NAME)
define configure/$(LOCAL_PKG_NAME)
	$(if $(wildcard $($(1)_src)/configure*), \
		$(call tuya_autotools_configure,$(1)), \
		$(call tuya_cmake_configure,$(1)))
endef
endif

ifndef compile/$(LOCAL_PKG_NAME)
define compile/$(LOCAL_PKG_NAME)
	make -C $($(1)_build) -j$(CPUS)
endef
endif

ifndef install/$(LOCAL_PKG_NAME)
define install/$(LOCAL_PKG_NAME)
	make -C $($(1)_build) install
endef
endif

###############################################
# Package tasks
# $(1): package name
# $(2): type (target or host), not used now
###############################################
define PackgePrepareTask
	$(call prepare/$(1),$(1),$(2))
endef

define PackgeConfigureTask
	$(call configure/$(1),$(1),$(2))
endef

define PackgeBuildTask
	$(call compile/$(1),$(1),$(2))
	$(call install/$(1),$(1),$(2))
endef

###############################################
#                   Rules
###############################################
xmake_package: $(_LOCAL_PKG_TARGET_BUILD)

$(_LOCAL_PKG_TARGET_BUILD): $(_LOCAL_PKG_TARGET_CONF)
	$(call PackgeBuildTask,$(notdir $(@D)))
	@touch $@

$(_LOCAL_PKG_TARGET_CONF): PRIVATE_CFLAGS := $(LOCAL_CFLAGS)
$(_LOCAL_PKG_TARGET_CONF): PRIVATE_CXXFLAGS := $(LOCAL_CFLAGS)
$(_LOCAL_PKG_TARGET_CONF): PRIVATE_LDFLAGS := $(LOCAL_LDFLAGS)

$(_LOCAL_PKG_TARGET_CONF): $(_LOCAL_PKG_TARGET_PREPARE) $(_LOCAL_PKG_DEPS)
	$(call PackgeConfigureTask,$(notdir $(@D)))
	@touch $@

$(_LOCAL_PKG_TARGET_PREPARE):
	@mkdir -p $(@D)
	$(call PackgePrepareTask,$(notdir $(@D)))
	@touch $@
