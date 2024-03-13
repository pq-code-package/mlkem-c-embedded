# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -O3 -fomit-frame-pointer -z noexecstack \
  -Imlkem

MLKEM_SOURCES =  $(wildcard mlkem/*.c)
MLKEM_HEADERS = $(wildcard mlkem/*.h)

.PHONY: all clean

all: mlkem-512-test mlkem-768-test mlkem-1024-test

mlkem-512-test: test/test.c $(MLKEM_SOURCES) $(MLKEM_HEADERS)
	$(CC) $(CFLAGS) -DKYBER_K=2 $< $(MLKEM_SOURCES) -o $@

mlkem-768-test: test/test.c $(MLKEM_SOURCES) $(MLKEM_HEADERS)
	$(CC) $(CFLAGS) -DKYBER_K=3 $< $(MLKEM_SOURCES) -o $@

mlkem-1024-test: test/test.c $(MLKEM_SOURCES) $(MLKEM_HEADERS)
	$(CC) $(CFLAGS) -DKYBER_K=4 $< $(MLKEM_SOURCES) -o $@

clean: 
	$(RM) mlkem-512-test mlkem-768-test mlkem-1024-test