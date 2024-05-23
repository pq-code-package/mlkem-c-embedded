# SPDX-License-Identifier: Apache-2.0

.PHONY: all

all: test speed stack


include mk/config.mk
include mk/schemes.mk
include mk/rules.mk

Q ?= @

test: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-test)
speed: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-speed)
stack: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-stack)
nistkat: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-nistkat)

.PHONY: emulate clean

emulate:
	$(Q)qemu-system-arm -machine $(PLATFORM) -nographic -semihosting -kernel $(ELF_FILE)

clean:
	$(Q)rm -rf elf/
	$(Q)rm -rf bin/
	$(Q)rm -rf obj/
