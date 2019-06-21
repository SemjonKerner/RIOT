PKG_NAME=openwsn-fw
PKG_URL=https://github.com/PeterKietzmann/openwsn-fw.git
PKG_VERSION=0995d9108b1ab8f97029dcfcea4a670a17f05dde
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)
PKG_LICENSE=BSD-3-Clause

#TODO documentation
#not needed now due to WERROR=0
CFLAGS += -Wno-implicit-function-declaration
CFLAGS += -Wno-return-type
CFLAGS += -Wno-builtin-declaration-mismatch
CFLAGS += -Wno-array-bounds
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-char-subscripts

CFLAGS += -Wno-implicit-fallthrough
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-maybe-uninitialized

.PHONY: all

all:
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
