# SPDX-License-Identifier: Apache-2.0
ifeq ($(RNG),NISTKAT)
	LIBHAL_SRC = \
		test/common/nistkatrng.c \
		test/common/aes.c
	CPPFLAGS += -Itest/common
else
	RNG := NOTRAND
	LIBHAL_SRC = hal/notrandombytes.c
endif

LDSCRIPT = $(OBJ_DIR)/generated.$(PLATFORM).ld

CFLAGS += $(ARCH_FLAGS)

LDFLAGS += \
	-Wl,--no-warn-rwx-segments \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LIBHAL_SRC += hal/hal-mps2.c
STARTUP_SRC = $(MBED_OS_TARGET_DIR)/TOOLCHAIN_GCC_ARM/startup_MPS2.S
STARTUP_OBJ = $(shell echo "$(STARTUP_SRC)" | sed -E 's~(.*/)(TARGET.*)~$(OBJ_DIR)/hal/\2.o~g')

MPS2_DEPS += -I$(MBED_OS_DIR)/Include -I$(MBED_OS_TARGET_DIR)

$(OBJ_DIR)/hal/libhal.a: $(call objs,$(LIBHAL_SRC)) $(STARTUP_OBJ)
$(OBJ_DIR)/hal/libhal.a: CPPFLAGS += $(MPS2_DEPS) $(if $(RNG)==NISTKAT,-Itest/common)

$(STARTUP_OBJ): $(STARTUP_SRC)
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

$(LDSCRIPT): 	$(MBED_OS_TARGET_DIR)/TOOLCHAIN_GCC_ARM/MPS2.ld
	@printf "  GENLNK  $@\n"; \
	[ -d $(@D) ] || $(Q)mkdir -p $(@D); \
	$(CC) -x assembler-with-cpp -E -Wp,-P $(CPPFLAGS) $< -o $@

$(LDSCRIPT): CPPFLAGS += $(MPS2_DEPS) $(if $(RNG)==NISTKAT,-Itest/common)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
