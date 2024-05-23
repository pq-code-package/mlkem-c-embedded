# SPDX-License-Identifier: Apache-2.0
OPENCM3_DIR ?=
override LDSCRIPT := obj/generated.$(DEVICE).ld

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

LIBHAL_SRC += hal/hal-opencm3.c

obj/libhal.a: $(call objs,$(LIBHAL_SRC))

CFLAGS += $(ARCH_FLAGS)

LDFLAGS += \
	-nostartfiles \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
