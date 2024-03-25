# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

.PHONY: all

all: test speed stack


include mk/config.mk
include mk/schemes.mk
include mk/rules.mk

test: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-test)
speed: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-speed)
stack: $(foreach scheme,$(KEM_SCHEMES),$(scheme)-stack)

.PHONY: clean libclean

clean:
	rm -rf elf/
	rm -rf bin/
	rm -rf obj/

distclean: libclean clean
