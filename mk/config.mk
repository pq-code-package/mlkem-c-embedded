# SPDX-License-Identifier: Apache-2.0
ifndef _CONFIG
_CONFIG :=

SRCDIR := $(CURDIR)

# GCC config
CROSS_PREFIX ?= arm-none-eabi
CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-cpp
AR := $(CROSS_PREFIX)-ar
LD := $(CC)
OBJCOPY := $(CROSS_PREFIX)-objcopy
SIZE := $(CROSS_PREFIX)-size

##############################
# Include retained variables #
##############################

RETAINED_VARS :=

CONFIG := obj/.config.mk

-include $(CONFIG)

$(CONFIG):
	@echo "  GEN     $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	@echo "# These variables are retained and can't be changed without a clean" > $@
	@$(foreach var,$(RETAINED_VARS),echo "$(var) := $($(var))" >> $@; echo "LAST_$(var) := $($(var))" >> $@;)

###############
# Some Macros #
###############
objs = $(addprefix obj/,$(addsuffix .o,$(1)))

PLATFORM ?= stm32f4discovery
RNG ?= HAL

RETAINED_VARS += PLATFORM RNG

# RNG config
ifeq ($(RNG),HAL)
	LIBHAL_SRC = hal/randombytes.c
else ifeq ($(RNG),NOTRAND)
	LIBHAL_SRC = hal/notrandombytes.c
else ifeq ($(RNG),NISTKAT)
	LIBHAL_SRC = \
		test/common/nistkatrng.c \
		test/common/aes.c
	CPPFLAGS += -Itest/common
endif

LDLIBS += -lhal
LIBDEPS += obj/libhal.a

# HAL config

# Common config
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
RETAINED_VARS += NTESTS

KEM_SCHEMES=mlkem512 mlkem768 mlkem1024
# mlkem k
KEM_PARAMS=2 3 4

define VAR_CHECK
ifneq ($$(origin LAST_$(1)),undefined)
ifneq "$$($(1))" "$$(LAST_$(1))"
$$(info Variable $(1) changed, forcing rebuild!)
.PHONY: $(CONFIG)
endif
endif
endef

$(foreach VAR,$(RETAINED_VARS),$(eval $(call VAR_CHECK,$(VAR))))
endif
