# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

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

emulate%: PLATFORM = mps2-an386
emulate%: NTESTS = 10

emulate\ nistkat: KATRNG=NIST
emulate\ nistkat:
	$(Q)$(MAKE) PLATFORM=$(PLATFORM) NTESTS=$(NTESTS) KATRNG=$(KATRNG) nistkat

# emulate for test, speed, stack
emulate%:
	$(Q)$(MAKE) PLATFORM=$(PLATFORM) NTESTS=$(NTESTS) $*

emulate\ run: PLATFORM=mps2-an386
emulate\ run:
	$(Q)qemu-system-arm -machine $(PLATFORM) -nographic -semihosting -kernel $(ELF_FILE)


clean:
	$(Q)rm -rf elf/
	$(Q)rm -rf bin/
	$(Q)rm -rf obj/
