# SPDX-License-Identifier: Apache-2.0
MPS2_DATA_IN_FLASH = 1

LDSCRIPT = obj/generated.$(PLATFORM).ld

ARCH_FLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += $(ARCH_FLAGS)

CPPFLAGS += \
	-DMPS2_AN386

LDFLAGS += \
	-Wl,--no-warn-rwx-segments \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LIBHAL_SRC += \
	hal/mps2/startup_MPS2.S \
	hal/hal-mps2.c

obj/libhal.a: $(call objs,$(LIBHAL_SRC))
obj/libhal.a: CPPFLAGS += -Ihal/mps2 $(if $(RNG)==NISTKAT,-Itest/common)

$(LDSCRIPT): CPPFLAGS += $(if $(MPS2_DATA_IN_FLASH),-DDATA_IN_FLASH)
obj/hal/mps2/startup_MPS2.S.o: CPPFLAGS += $(if $(MPS2_DATA_IN_FLASH),-DDATA_IN_FLASH)

$(LDSCRIPT): hal/mps2/MPS2.ld
	@printf "  GENLNK  $@\n"; \
	[ -d $(@D) ] || $(Q)mkdir -p $(@D); \
	$(CC) -x assembler-with-cpp -E -Wp,-P $(CPPFLAGS) $< -o $@

$(LDSCRIPT): CPPFLAGS += -Ihal/mps2 $(if $(RNG)==NISTKAT,-Itest/common)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
