# PKG_SOURCE_LOCAL ?= $(RIOTBASE)/../openwsn-fw # enable when using local copy
PKG_NAME=openwsn-fw
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw.git
PKG_VERSION=dcf26e99b65ace4b1d8551a9962ee976fe7f0359
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)
PKG_LICENSE=BSD-3-Clause
include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-implicit-function-declaration
CFLAGS += -Wno-return-type
CFLAGS += -Wno-builtin-declaration-mismatch
CFLAGS += -Wno-array-bounds
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-implicit-function-declaration
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-pointer-sign
CFLAGS += -Wno-int-conversion
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-char-subscripts
CFLAGS += -Wno-implicit-fallthrough
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-maybe-uninitialized

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/

