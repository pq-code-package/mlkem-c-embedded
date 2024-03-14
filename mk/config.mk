SRCDIR := $(CURDIR)

###############
# Some Macros #
###############
objs = $(addprefix obj/,$(addsuffix .o,$(1)))

PLATFORM ?= stm32f4discovery

include mk/$(PLATFORM).mk

CFLAGS += \
	-std=gnu99 \
	--sysroot=$(SYSROOT) \
	-Wall -Wextra -Wshadow \
	-MMD \
	-fno-common \
	-Imlkem -Ifips202 -Ihal \
	$(CPPFLAGS)

LDFLAGS += \
	-Lobj \
	-Wl,--gc-sections

MUPQ_ITERATIONS ?= 1
