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

LIBHAL_SRC := \
	hal/mps2/startup_MPS2.S \
	hal/hal-mps2.c

ifndef KATRNG
	LIBHAL_SRC += hal/notrandombytes.c
else
  ifeq ($(KATRNG),NIST)
    LIBHAL_SRC += \
	  test/common/nistkatrng.c \
	  test/common/aes.c
	CPPFLAGS += -Itest/common
  endif
endif

obj/libpqm4hal.a: $(call objs,$(LIBHAL_SRC))
obj/libpqm4hal.a: CPPFLAGS += -Ihal/mps2 $(if $(KATRNG)==NIST,-Itest/common)

$(LDSCRIPT): CPPFLAGS += $(if $(MPS2_DATA_IN_FLASH),-DDATA_IN_FLASH)
obj/hal/mps2/startup_MPS2.S.o: CPPFLAGS += $(if $(MPS2_DATA_IN_FLASH),-DDATA_IN_FLASH)

LDLIBS += -lpqm4hal
LIBDEPS += obj/libpqm4hal.a

$(LDSCRIPT): hal/mps2/MPS2.ld
	@printf "  GENLNK  $@\n"; \
	[ -d $(@D) ] || $(Q)mkdir -p $(@D); \
	$(CC) -x assembler-with-cpp -E -Wp,-P $(CPPFLAGS) $< -o $@

$(LDSCRIPT): CPPFLAGS += -Ihal/mps2 $(if $(KATRNG)==NIST,-Itest/common)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
