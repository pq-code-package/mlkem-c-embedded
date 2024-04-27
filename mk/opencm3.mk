# SPDX-License-Identifier: Apache-2.0
LIBHAL_SRC += hal/hal-opencm3.c

obj/libpqm4hal.a: $(call objs,$(LIBHAL_SRC))

CFLAGS += $(ARCH_FLAGS)

LDFLAGS += \
	-nostartfiles \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
