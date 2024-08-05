# SPDX-License-Identifier: Apache-2.0

.PHONY: all

all: test speed stack nistkat

include mk/config.mk
include mk/schemes.mk
include mk/rules.mk

Q ?= @

$(TESTS): % : $(call SCHEMES,%)

.PHONY: emulate clean

emulate:
	$(Q)qemu-system-arm -machine $(PLATFORM) -nographic -semihosting -kernel $(ELF_FILE)

clean:
	$(Q)rm -rf elf/
	$(Q)rm -rf bin/
	$(Q)rm -rf obj/
