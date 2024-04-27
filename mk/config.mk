# SPDX-License-Identifier: Apache-2.0
SRCDIR := $(CURDIR)

# GCC config
CROSS_PREFIX ?= arm-none-eabi
CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-cpp
AR := $(CROSS_PREFIX)-ar
LD := $(CC)
OBJCOPY := $(CROSS_PREFIX)-objcopy
SIZE := $(CROSS_PREFIX)-size

###############
# Some Macros #
###############
objs = $(addprefix obj/,$(addsuffix .o,$(1)))

PLATFORM ?= stm32f4discovery
KATRNG ?=

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
	--specs=nosys.specs \
	-Wl,--wrap=_sbrk \
	-Wl,--wrap=_open \
	-Wl,--wrap=_close \
	-Wl,--wrap=_isatty \
	-Wl,--wrap=_kill \
	-Wl,--wrap=_lseek \
	-Wl,--wrap=_read \
	-Wl,--wrap=_write \
	-Wl,--wrap=_fstat \
	-Wl,--wrap=_getpid \
	-ffreestanding \
	-Lobj \
	-Wl,--gc-sections

NTESTS ?= 1

KEM_SCHEMES=mlkem512 mlkem768 mlkem1024
# mlkem k
KEM_PARAMS=2 3 4
