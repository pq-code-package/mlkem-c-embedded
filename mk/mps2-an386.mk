# SPDX-License-Identifier: Apache-2.0
MPS2_DATA_IN_FLASH = 1
CROSS_PREFIX ?= arm-none-eabi
CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-cpp
AR := $(CROSS_PREFIX)-ar
LD := $(CC)
OBJCOPY := $(CROSS_PREFIX)-objcopy
SIZE := $(CROSS_PREFIX)-size

LDSCRIPT = obj/ldscript.ld

ARCH_FLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += \
	$(ARCH_FLAGS) \
	--specs=nosys.specs

CPPFLAGS += \
	-DMPS2_AN386

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
	-Wl,--no-warn-rwx-segments \
	-ffreestanding \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LIBHAL_SRC := \
	hal/mps2/startup_MPS2.S \
	hal/hal-mps2.c \
	hal/randombytes.c

obj/libpqm4hal.a: $(call objs,$(LIBHAL_SRC))
obj/libpqm4hal.a: CPPFLAGS += -Ihal/mps2
$(LDSCRIPT): CPPFLAGS += $(if $(MPS2_DATA_IN_FLASH),-DDATA_IN_FLASH)
obj/hal/mps2/startup_MPS2.S.o: CPPFLAGS += $(if $(MPS2_DATA_IN_FLASH),-DDATA_IN_FLASH)

LDLIBS += -lpqm4hal
LIBDEPS += obj/libpqm4hal.a

$(LDSCRIPT): hal/mps2/MPS2.ld
	@printf "  GENLNK  $@\n"; \
	[ -d $(@D) ] || $(Q)mkdir -p $(@D); \
	$(CC) -x assembler-with-cpp -E -Wp,-P $(CPPFLAGS) $< -o $@

$(LDSCRIPT): CPPFLAGS += -Ihal/mps2

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
