ifeq ($(PALERA1N),1)
# palera1n targets arm64 only (A10/A10X have no PAC); use rootless scheme
# so standard Theos works — no need for roothide's Theos fork.
ARCHS = arm64
THEOS_PACKAGE_SCHEME = rootless
else
ARCHS = arm64 arm64e
THEOS_PACKAGE_SCHEME = roothide
endif

TARGET := iphone:clang:latest:16.0

# FINALPACKAGE ?= 1
# DEBUG ?= 0

include $(THEOS)/makefiles/common.mk

LIBRARY_NAME = libcommon
$(LIBRARY_NAME)_INSTALL=0 # do not copy to theos/lib

libcommon_CFLAGS = -fvisibility=hidden -I. -DFISHHOOK_EXPORT -fobjc-arc -Wno-unused-variable -Wno-unused-but-set-variable -Wno-deprecated-declarations -std=gnu17 -Wno-deprecated-non-prototype -DNDEBUG
libcommon_CCFLAGS = -fvisibility=hidden -std=c++17
libcommon_LFGLAGS = -L.

ifeq ($(PALERA1N),1)
# palera1n build: exclude jailbreakd XPC client files (dead code — no jailbreakd on palera1n).
# Also use -fno-implicit-module-maps so the local common/xpc/module.modulemap is not
# auto-discovered. Without this, ANY file importing Foundation triggers a fatal
# "redefinition of module 'XPC'" because the local map shadows the SDK's XPC module.
# With -fno-implicit-module-maps, #import <Foundation/Foundation.h> falls back to
# classic header inclusion (no module cache) — slightly slower but correct.
libcommon_FILES = $(filter-out jailbreakd.c jbclient.c, $(wildcard *.c *.m *.mm *.cpp))
libcommon_CFLAGS += -DPALERA1N_BUILD -I../palera1n_compat -fno-implicit-module-maps -I../kexploit
else
libcommon_FILES = $(wildcard *.c *.m *.mm *.cpp)
endif
libcommon_LINKAGE_TYPE = static
libcommon_INSTALL_PATH = /basebin

include $(THEOS_MAKE_PATH)/library.mk

after-all::
	mkdir -p ./lib
	cp $(THEOS_OBJ_DIR)/libcommon.a ./lib/

clean::
	rm -rf ./packages/*
	