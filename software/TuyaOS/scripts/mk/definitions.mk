###########################################################
## Strip quotes and then whitespaces
## $(1): a var to strip
###########################################################
qstrip = $(strip $(subst ",,$(1)))
#"))

# Variables for use in Make constructs
comma := ,
empty :=
space := $(empty) $(empty)
# build a comma-separated list of single quoted items, from a space-separated
# list of unquoted items:   a b c d  -->  'a', 'b', 'c', 'd'
make-sq-comma-list = $(subst $(space),$(comma)$(space),$(patsubst %,'%',$(strip $(1))))

###########################################################
## Utility functions for 'find'
## findfileclauses(filelist) => -name 'X' -o -name 'Y'
###########################################################
findfileclauses = $(call notfirstword,$(patsubst %,-o -name '%',$(1)))
## finddirclauses(base, dirlist) => -path 'base/dirX' -o -path 'base/dirY'
finddirclauses = $(call notfirstword,$(patsubst %,-o -path '$(1)/%',$(2)))

###########################################################
## Miscellaneous utility functions
## notfirstword(wordlist): returns all but the first word in wordlist
###########################################################
notfirstword = $(wordlist 2,$(words $(1)),$(1))

###########################################################
## Check if version A is greater or equal to versoin B
## $(1): version A
## $(2): version B
###########################################################
checkversion = $(shell expr `echo $(1) | awk -F- '{print $$NF}'` \>= $(2))

# Misc
IS_TTY=$(shell tty -s && echo 1 || echo 0)

###########################################################
## Needed for the foreach loops to loop over the list of hooks, so that
## each hook call is properly separated by a newline.
###########################################################
define sep


endef

PERCENT = %
QUOTE = '
# '

define PRINTF
	printf '$(subst $(sep),\n,\
		$(subst $(PERCENT),$(PERCENT)$(PERCENT),\
			$(subst $(QUOTE),$(QUOTE)\$(QUOTE)$(QUOTE),\
				$(subst \,\\,$(1)))))\n'
endef

###########################################################
## print messages
## $(1): message
###########################################################
ifeq ($(IS_TTY),1)
_Y:=\\033[33m
_R:=\\033[31m
_N:=\\033[m
endif

define MSG_INFO
@printf "$(_Y)########################################\n"
@printf "# %s\n" $(1)
@printf "########################################$(_N)\n"
endef

define MSG_ERROR
printf "$(_R)########################################\n"
printf "# %s\n" $(1)
printf "########################################$(_N)\n"
endef

############################################################
# 实用函数
############################################################

# 源代码路径补全
define source-add-prefix-bsp
$(foreach src,$(2),$(filter %.c %.cc %.cpp %.S %.s,$(wildcard $(1)/$(src)) $(wildcard $(src))))
endef

# 生成源代码的依赖文件
define source-dependcies-bsp
$(strip	\
	$(patsubst $(abspath $(1))/%,$(_XMAKE_OBJS_DIR)/static/$(2)/%.o.d,$(abspath $(3)))	\
	$(patsubst $(abspath $(1))/%,$(_XMAKE_OBJS_DIR)/shared/$(2)/%.o.d,$(abspath $(3)))
)
endef

# 生成源代码的目标文件
define source-objects-bsp
$(strip	\
	$(patsubst $(abspath $(1))/%,$(_XMAKE_OBJS_DIR)/$(2)/$(3)/%.o,$(abspath $(4)))
)
endef

###########################################################
## Add prefix to files
## $(1): prefix
## $(2): files
###########################################################
define file-add-prefix
$(foreach src,$(2),$(wildcard $(1)/$(src)) $(wildcard $(src)))
endef

###########################################################
## Set up phony targets that covers all modules under the given paths.
## This allows us to build everything in given paths by running mm command
## $(1): dir path
## $(2): targets
###########################################################
define modules-in-dir
local_path_components := $(subst /,$(space),$(1))
local_path_prefix := MODULES-IN
$$(foreach c, $$(local_path_components),\
	$$(eval local_path_prefix := $$(local_path_prefix)-$$(c))\
	$$(eval .PHONY : $$(local_path_prefix))\
	$$(eval $$(local_path_prefix) : $(2))\
)
endef

###########################################################
## This function rsyncs the directories
## $(1): source directory
## $(2): destination directory
###########################################################
define do_rsync
	@rsync -a --ignore-times  --exclude .git \
		--chmod=u=rwX,go=rX --exclude .empty --exclude '*~' \
		$(1)/ $(2)/
endef

###########################################################
## donwload package to $(DOWNLOAD_DIR)
## $(1): package name
###########################################################
define pkg_fetch
	@test -e $(DOWNLOAD_DIR)/$(1) || \
	wget --no-check-certificate -P $(DOWNLOAD_DIR) -c $(TUYA_BSP_PACKAGE_REPO)/$(1)
endef

###########################################################
## apply patch for package
## $(1): package name
## $(2): patch file path
###########################################################
define apply_patch
	@patch -g0 -p1 -E -d "$($(1)_src)" -t -N < $(2) || exit 1
	@echo $(2) >> $($(1)_src)/.applied_patches_list
endef

define do_bootstrap
	@cd $(1) ;\
		./bootstrap
endef

define do_autoreconf
	@cd $(1) && autoreconf --install
endef

define do_autogen
	@cd $(1) && ./autogen.sh
endef

define update_gnuconfig
	@for file in config.guess config.sub; do \
		for i in `find $(1) -name $$file`; do \
			cp -rf $(BUILD_SYSTEM)/gnuconfig/$$file $$i; \
		done; \
	done
endef
###########################################################
## Autotools configure function
## $(1): package name
###########################################################
define tuya_autotools_configure
	$(call update_gnuconfig, $($(1)_src))
	$(if $(wildcard $($(1)_src)/bootstrap),$(call do_bootstrap,$($(1)_src)))
	$(if $(wildcard $($(1)_src)/configure),,\
		$(if $(wildcard $($(1)_src)/configure.ac),\
			$(call do_autoreconf,$($(1)_src)), \
				$(if $(wildcard $($(1)_src)/autogen.sh),\
					$(call do_autogen,$($(1)_src)) \
				) \
		) \
	)
	@cd $($(1)_build); \
	CPP= \
	CFLAGS="$(PRIVATE_CFLAGS)" \
	CXXFLAGS="$(PRIVATE_CXXFLAGS)" \
	LDFLAGS="$(PRIVATE_LDFLAGS)" \
	$($(1)_src)/configure \
		--target=$(CROSS_TARGET) \
		--host=$(CROSS_TARGET) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--sysconfdir=/etc \
		--localstatedir=/var \
		--program-prefix="" \
		--enable-silent-rules \
		--disable-gtk-doc \
		--disable-doc \
		--disable-doc \
		--disable-docs \
		--disable-documentation \
		--with-xmlto=no \
		--with-fop=no \
		--silent \
		$($(1)_option)
endef

###########################################################
## CMake configure function
## $(1): package name
###########################################################
define tuya_cmake_configure
	@sed \
		-e 's#@@ARCH@@#$(TARGET_ARCH)#' \
		-e 's#@@SYSROOT@@#$(TARGET_SYSROOT)#' \
		-e 's#@@CC@@#$(CC)#' \
		-e 's#@@CXX@@#$(CXX)#' \
		-e 's#@@CFLAGS@@#$(PRIVATE_CFLAGS)#' \
		-e 's#@@CXXFLAGS@@#$(PRIVATE_CXXFLAGS)#' \
		-e 's#@@LDFLAGS@@#$(PRIVATE_LDFLAGS)#' \
		$(BUILD_SYSTEM)/cmake-cross-compilation.conf.in \
		> $($(1)_build)/cross-compilation.conf
	@cd $($(1)_build); \
	rm -f CMakeCache.txt; \
	cmake $($(1)_src) \
		-DCMAKE_TOOLCHAIN_FILE="$($(1)_build)/cross-compilation.conf" \
		-DCMAKE_INSTALL_PREFIX="/usr" \
		-DCMAKE_BUILD_TYPE=$(if $(CONFIG_TUYA_ENABLE_DEBUG),Debug,Release) \
		-DCMAKE_COLOR_MAKEFILE=OFF \
		-DBUILD_DOC=OFF \
		-DBUILD_DOCS=OFF \
		-DBUILD_EXAMPLE=OFF \
		-DBUILD_EXAMPLES=OFF \
		-DBUILD_TEST=OFF \
		-DBUILD_TESTS=OFF \
		-DBUILD_TESTING=OFF \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_RULE_MESSAGES=OFF \
		-DCMAKE_INSTALL_MESSAGE=NEVER \
		$($(1)_option)
endef

###########################################################
## meson configure function
## $(1): package name
###########################################################
define tuya_meson_configure
	@rm -rf $($(1)_build)/*
	@sed \
		-e 's#@ARCH@#$(TARGET_ARCH)#g' \
		-e 's#@CPU@#$(TARGET_CPU)#g' \
		-e 's#@ENDIAN@#$(TARGET_ENDIAN)#g' \
		-e 's#@SYSROOT@#$(TARGET_SYSROOT)#g' \
		-e 's#@CC@#$(CC)#' \
		-e 's#@CXX@#$(CXX)#' \
		-e 's#@AR@#$(AR)#' \
		-e 's#@STRIP@#$(STRIP)#' \
		-e "s#@CFLAGS@#$(call make-sq-comma-list,$(PRIVATE_CFLAGS))#g" \
		-e "s#@LDFLAGS@#$(call make-sq-comma-list,$(PRIVATE_LDFLAGS))#g" \
		-e "s#@CXXFLAGS@#$(call make-sq-comma-list,$(PRIVATE_CXXFLAGS))#g" \
		$(BUILD_SYSTEM)/meson-cross-compilation.conf.in \
		> $($(1)_build)/cross-compilation.conf
	@$(MESON) \
		--prefix=/usr \
		--libdir=lib \
		--default-library=shared \
		--buildtype=$(if $(CONFIG_TUYA_ENABLE_DEBUG),debug,release) \
		--cross-file=$($(1)_build)/cross-compilation.conf \
		-Dbuild.pkg_config_path=$(TARGET_SYSROOT)/usr/lib/pkgconfig \
		$($(1)_option) \
		$($(1)_src) $($(1)_build)
endef

###########################################################
## make build function
###########################################################
define tuya_make_compile
	@$(MAKE) -C $(@D)
endef

###########################################################
## make intall function
###########################################################
define tuya_make_install
	@$(MAKE) -C $(@D) DESTDIR=$(TARGET_ROOTFS) LIBTOOLFLAGS=--silent install
endef

###########################################################
## make intall_sysroot function
###########################################################
define tuya_make_install_sysroot
	$(call fix-libtool-files,$(@D))
	@$(MAKE) -C $(@D) DESTDIR=$(TARGET_SYSROOT) LIBTOOLFLAGS=--silent install
endef

###########################################################
## meson build function
###########################################################
define tuya_meson_compile
	@$(NINJA) -C $(@D)
endef

###########################################################
## meson intall function
###########################################################
define tuya_meson_install
	@DESTDIR=$(TARGET_ROOTFS) \
	$(NINJA) -C $(@D) install
endef

###########################################################
## meson intall_sysroot function
###########################################################
define tuya_meson_install_sysroot
	@DESTDIR=$(TARGET_SYSROOT) \
	$(NINJA) -C $(@D) install
endef

###########################################################
## Setup proprietary modules
## $(1) modules class
###########################################################
define prepare_proprietary_modules
$(foreach m,$(wildcard $(_XMAKE_OBJS_DIR)/$(1)/*),
	$(BUILD_SYSTEM)/scripts/setup_proprietary.py $(m) \
		$(TARGET_OUT)/proprietary/$(PROPRIETARY_MODULE.$(1).$(notdir $(m))) \
		$(1) \
		"$(PROPRIETARY_MODULE.$(1).$(notdir $(m)).TAGS)" \
		$(PROPRIETARY_MODULE.$(1).$(notdir $(m)).PATH)
)
endef

###########################################################
## Some packages install libtool .la files alongside any installed
## libraries. These .la files sometimes refer to paths relative to the
## sysroot, which libtool will interpret as absolute paths to host
## libraries instead of the target libraries. Since this is not what we
## want, these paths are fixed by prefixing them with $(TARGET_SYSROOT).
## As we configure with --prefix=/usr, this fix needs to be applied to
## any path that starts with /usr.
###########################################################
define fix-libtool-files
	@for lai in $$(find $(1) -name "*.lai"); do \
		sed -i -e "s:\(['= ]\)/usr:\\1@TARGET_SYSROOT@/usr:g" \
			-e "s:\(['= ]\)/lib:\\1@TARGET_SYSROOT@/lib:g" \
			-e "s:@TARGET_SYSROOT@:$(TARGET_SYSROOT):g" "$${lai}"; \
	done
endef

###########################################################
## Copy toolchain libraries to target rootfs
###########################################################
TOOLCHAIN_LIBS := ld*.so.* libgcc_s.so.* libatomic.so.* libpthread.so.* \
		 libc.so.* libcrypt.so.* libdl.so.* libm.so.* libstdc++.so.* \
		 libnsl.so.* libresolv.so.* librt.so.* libutil.so.*

ifeq ($(wildcard $(TARGET_SYSROOT)/usr/include/bits/uClibc_config.h),)
TOOLCHAIN_LIBS += libnss_files.so.* libnss_dns.so.* libmvec.so.* libanl.so.*
endif

define copy-toolchain-libs
	for LIBPATTERN in $(TOOLCHAIN_LIBS); do \
		LIBPATHS=`find $(TARGET_SYSROOT)/ -name "$${LIBPATTERN}" 2>/dev/null` ; \
		for LIBPATH in $${LIBPATHS} ; do \
			while true ; do \
				LIBNAME=`basename $${LIBPATH}`; \
				DESTDIR=`echo $${LIBPATH} | sed "s,^$(TARGET_SYSROOT)/,," | xargs dirname` ; \
				mkdir -p $(TARGET_ROOTFS)/$${DESTDIR}; \
				rm -fr $(TARGET_ROOTFS)/$${DESTDIR}/$${LIBNAME}; \
				if test -h $${LIBPATH} ; then \
					cp -d $${LIBPATH} $(TARGET_ROOTFS)/$${DESTDIR}/$${LIBNAME}; \
					LIBPATH="`readlink -f $${LIBPATH}`"; \
				elif test -f $${LIBPATH}; then \
					install -D -m0755 $${LIBPATH} $(TARGET_ROOTFS)/$${DESTDIR}/$${LIBNAME}; \
					break ; \
				else \
					exit -1; \
				fi; \
			done; \
		done; \
	done
endef

