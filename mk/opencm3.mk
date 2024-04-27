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
