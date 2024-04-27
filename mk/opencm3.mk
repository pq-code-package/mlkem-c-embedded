# SPDX-License-Identifier: Apache-2.0
LIBHAL_SRC := hal/hal-opencm3.c

ifndef KATRNG
  LIBHAL_SRC += hal/randombytes.c
else
  ifeq ($(KATRNG),NIST)
    LIBHAL_SRC += \
	  test/common/nistkatrng.c \
	  test/common/aes.c
	CPPFLAGS += -Itest/common
  endif
endif

obj/libpqm4hal.a: $(call objs,$(LIBHAL_SRC))

LDLIBS += -lpqm4hal
LIBDEPS = obj/libpqm4hal.a

CFLAGS += $(ARCH_FLAGS)

LDFLAGS += \
	-nostartfiles \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

LINKDEPS += $(LDSCRIPT) $(LIBDEPS)
