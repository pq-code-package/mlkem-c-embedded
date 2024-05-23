# SPDX-License-Identifier: Apache-2.0
LDSCRIPT = obj/generated.$(PLATFORM).ld

CFLAGS += $(ARCH_FLAGS)

LDFLAGS += \
	-Wl,--no-warn-rwx-segments \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LIBHAL_SRC += hal/hal-mps2.c $(MBED_OS_TARGET_DIR)/TOOLCHAIN_GCC_ARM/startup_MPS2.S

MPS2_DEPS += -I$(MBED_OS_DIR)/Include -I$(MBED_OS_TARGET_DIR)

obj/libhal.a: $(call objs,$(LIBHAL_SRC))
obj/libhal.a: CPPFLAGS += $(MPS2_DEPS) $(if $(RNG)==NISTKAT,-Itest/common)

$(LDSCRIPT): 	$(MBED_OS_TARGET_DIR)/TOOLCHAIN_GCC_ARM/MPS2.ld
	@printf "  GENLNK  $@\n"; \
	[ -d $(@D) ] || $(Q)mkdir -p $(@D); \
	$(CC) -x assembler-with-cpp -E -Wp,-P $(CPPFLAGS) $< -o $@

$(LDSCRIPT): CPPFLAGS += $(MPS2_DEPS) $(if $(RNG)==NISTKAT,-Itest/common)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)

