APPLICATION = openwsn_test
include ../Makefile.tests_common

BOARD ?= iotlab-m3

USEPKG += openwsn
USEMODULE += ipv6_addr
USEMODULE += ps
USEMODULE += od_string
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += event

# Enable this to use single channel debugging mode
#CFLAGS=-DOW_RIOT_SINGLE_CHANNEL=26

CFLAGS += -DDEVELHELP
CFLAGS += -DOW_MAC_ONLY
WERROR = 0

include $(RIOTBASE)/Makefile.include
