# SPDX-License-Identifier: Apache-2.0

.PHONY: all test speed stack nistkat

all: test speed stack nistkat

include mk/config.mk
include mk/schemes.mk
include mk/rules.mk

Q ?= @

nistkat:
	$(Q)$(MAKE) RNG=NISTKAT PLATFORM=$(PLATFORM) mlkem512-$@
	$(Q)$(MAKE) RNG=NISTKAT PLATFORM=$(PLATFORM) mlkem768-$@
	$(Q)$(MAKE) RNG=NISTKAT PLATFORM=$(PLATFORM) mlkem1024-$@

test speed stack:
	$(Q)$(MAKE) PLATFORM=$(PLATFORM) mlkem512-$@
	$(Q)$(MAKE) PLATFORM=$(PLATFORM) mlkem768-$@
	$(Q)$(MAKE) PLATFORM=$(PLATFORM) mlkem1024-$@

.PHONY: emulate clean

emulate:
	$(Q)qemu-system-arm -machine $(PLATFORM) -nographic -semihosting -kernel $(ELF_FILE)

clean:
	$(Q)rm -rf build/
