# SPDX-License-Identifier: Apache-2.0
SRCDIR := $(CURDIR)

###############
# Some Macros #
###############
objs = $(addprefix obj/,$(addsuffix .o,$(1)))

PLATFORM ?= stm32f4discovery

include mk/$(PLATFORM).mk

CFLAGS += \
	-std=gnu99 \
	--sysroot=$(SYSROOT) \
	-Wall -Wextra -Wshadow \
	-MMD \
	-fno-common \
	-Imlkem -Ifips202 -Ihal \
	$(CPPFLAGS)

LDFLAGS += \
	-Lobj \
	-Wl,--gc-sections

NTESTS ?= 1

KEM_SCHEMES=mlkem512 mlkem768 mlkem1024
# mlkem k
KEM_PARAMS=2 3 4