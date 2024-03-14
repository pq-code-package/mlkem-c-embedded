# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

.PHONY: all

all: mlkem512-test mlkem768-test mlkem1024-test mlkem512-speed mlkem768-speed mlkem1024-speed

define scheme-test
$(1)-test: CPPFLAGS += -DKYBER_K=$(2) -DMUPQ_NAMESPACE=$$(MUPQ_NAMESPACE) -DMUPQ_ITERATIONS=$$(MUPQ_ITERATIONS)
$(1)-test: bin/$(1)-test.hex

$(1)-speed: CPPFLAGS += -DKYBER_K=$(2) -DMUPQ_NAMESPACE=$$(MUPQ_NAMESPACE) -DMUPQ_ITERATIONS=$$(MUPQ_ITERATIONS)
$(1)-speed: bin/$(1)-speed.hex
endef

$(eval $(call scheme-test,mlkem512,2))
$(eval $(call scheme-test,mlkem768,3))
$(eval $(call scheme-test,mlkem1024,4))

include mk/config.mk
include mk/rules.mk

.PHONY: clean libclean

clean:
	rm -rf elf/
	rm -rf bin/
	rm -rf obj/

distclean: libclean clean
