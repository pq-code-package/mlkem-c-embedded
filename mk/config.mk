# SPDX-License-Identifier: Apache-2.0
ifndef _CONFIG
_CONFIG :=

SRCDIR := $(CURDIR)

##############################
# Include retained variables #
##############################
PLATFORM ?= stm32f4discovery
RNG ?= HAL
RETAINED_VARS := PLATFORM RNG

BUILD_DIR := build
BUILD_PLATFORM_DIR := $(BUILD_DIR)/$(PLATFORM)
OBJ_DIR := $(BUILD_PLATFORM_DIR)/obj
BIN_DIR := $(BUILD_PLATFORM_DIR)/bin

CONFIG := $(BUILD_PLATFORM_DIR)/config.mk

-include $(CONFIG)

$(CONFIG):
	@echo "  GEN     $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	@echo "# These variables are retained and can't be changed without a clean" > $@
	@$(foreach var,$(RETAINED_VARS),echo "$(var) := $($(var))" >> $@; echo "LAST_$(var) := $($(var))" >> $@;)

###############
# Some Macros #
###############

LDLIBS += -lpqcphal -L$(OBJ_DIR)/hal
LIBDEPS += $(OBJ_DIR)/hal/libpqcphal.a
LINKDEPS += $(LIBDEPS)

# Common config
objs = $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(1)))

include mk/$(PLATFORM).mk

CFLAGS += \
	-O3 \
	-std=gnu17 \
	--sysroot=$(SYSROOT) \
	-Wall -Wextra -Wshadow -Werror \
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
	-Wl,--gc-sections

NTESTS ?= 1
RETAINED_VARS += NTESTS

TESTS = test speed stack nistkat

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
