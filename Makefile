# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

.PHONY: all

all: test speed


include mk/config.mk
include mk/schemes.mk
include mk/rules.mk

test: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-test)
speed: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-speed)

.PHONY: emulate clean libclean

emulate: PLATFORM = mps2-an386
emulate:
	@echo $(PLATFORM)
	$(MAKE) PLATFORM=$(PLATFORM)
ifdef ELF_FILE
	qemu-system-arm -machine $(PLATFORM) -nographic -semihosting -kernel $(ELF_FILE)
endif

clean:
	rm -rf elf/
	rm -rf bin/
	rm -rf obj/

distclean: libclean clean
