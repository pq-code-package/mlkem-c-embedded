# SPDX-License-Identifier: Apache-2.0
LIBHAL_SRC := hal/hal-opencm3.c

ifndef KATRNG
  LIBHAL_SRC += hal/randombytes.c
else
  ifeq ($(KATRNG),NIST)
    LIBHAL_SRC += \
	  test/common/nistkatrng.c \
	  test/common/aes.c
  endif
endif

obj/libpqm4hal.a: $(call objs,$(LIBHAL_SRC))

LDLIBS += -lpqm4hal
LIBDEPS += obj/libpqm4hal.a

LDLIBS += -lc -lgcc

ifeq ($(DEVICE),)
$(warning no DEVICE specified for linker script generator)
endif

DEVICES_DATA ?= hal/devices.data

genlink_family		:=$(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) FAMILY)
genlink_subfamily	:=$(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) SUBFAMILY)
genlink_cpu		:=$(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) CPU)
genlink_fpu		:=$(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) FPU)
genlink_cppflags	:=$(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) CPPFLAGS)

ifeq ($(genlink_family),)
$(warning $(DEVICE) not found in $(DEVICES_DATA))
endif

CPPFLAGS	+= $(genlink_cppflags)

ARCH_FLAGS	:=-mcpu=$(genlink_cpu)
ifeq ($(genlink_cpu),$(filter $(genlink_cpu),cortex-m0 cortex-m0plus cortex-m3 cortex-m4 cortex-m7))
ARCH_FLAGS    +=-mthumb
endif

ifeq ($(genlink_fpu),soft)
ARCH_FLAGS	+= -msoft-float
else ifeq ($(genlink_fpu),hard-fpv4-sp-d16)
ARCH_FLAGS	+= -mfloat-abi=hard -mfpu=fpv4-sp-d16
else ifeq ($(genlink_fpu),hard-fpv5-sp-d16)
ARCH_FLAGS      += -mfloat-abi=hard -mfpu=fpv5-sp-d16
else
$(warning No match for the FPU flags)
endif

LIBNAME = opencm3_$(genlink_family)

LDLIBS += -l$(LIBNAME)

LDFLAGS += -L$(OPENCM3_DIR)/lib
CPPFLAGS += -I$(OPENCM3_DIR)/include $(if $(KATRNG)==NIST,-Itest/common)

LDSCRIPT = obj/generated.$(DEVICE).ld
$(LDSCRIPT): $(OPENCM3_DIR)/ld/linker.ld.S $(OPENCM3_DIR)/ld/devices.data $(CONFIG)
	@printf "  GENLNK  $(DEVICE)\n"
	$(Q)mkdir -p $(@D)
	$(Q)$(CPP) $(ARCH_FLAGS) $(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) DEFS) -P -E $< -o $@

CROSS_PREFIX ?= arm-none-eabi
CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-cpp
AR := $(CROSS_PREFIX)-gcc-ar
LD := $(CC)
OBJCOPY := $(CROSS_PREFIX)-objcopy
SIZE := $(CROSS_PREFIX)-size

CFLAGS += \
	$(ARCH_FLAGS) \

LDFLAGS += \
	--specs=nosys.specs \
	-Wl,--wrap=_sbrk \
	-Wl,--wrap=_close \
	-Wl,--wrap=_isatty \
	-Wl,--wrap=_kill \
	-Wl,--wrap=_lseek \
	-Wl,--wrap=_read \
	-Wl,--wrap=_write \
	-Wl,--wrap=_fstat \
	-Wl,--wrap=_getpid \
	-Wl,--wrap=_open \
	-nostartfiles \
	-ffreestanding \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
