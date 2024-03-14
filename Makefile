# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

.PHONY: all

all: mlkem512-test mlkem768-test mlkem1024-test

mlkem512-test: CPPFLAGS += -DKYBER_K=2 -DMUPQ_NAMESPACE=$(MUPQ_NAMESPACE)
mlkem512-test: bin/mlkem512-test.hex

mlkem768-test: CPPFLAGS += -DKYBER_K=3 -DMUPQ_NAMESPACE=$(MUPQ_NAMESPACE)
mlkem768-test: bin/mlkem768-test.hex

mlkem1024-test: CPPFLAGS += -DKYBER_K=4 -DMUPQ_NAMESPACE=$(MUPQ_NAMESPACE) 
mlkem1024-test: bin/mlkem1024-test.hex

include mk/config.mk
include mk/rules.mk

.PHONY: clean libclean

clean:
	rm -rf elf/
	rm -rf bin/
	rm -rf obj/

distclean: libclean clean
