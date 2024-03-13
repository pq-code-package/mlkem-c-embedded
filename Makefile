# SPDX-License-Identifier: Apache-2.0
# temporary Makefile to be replaced by Arm cross-compilation

CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -O3 -fomit-frame-pointer \
  -Imlkem -Ifips202

MLKEM_SOURCES = $(wildcard mlkem/*.c)
MLKEM_HEADERS = $(wildcard mlkem/*.h)

FIPS202_SOURCES = $(wildcard fips202/*.c)
FIPS202_HEADERS = $(wildcard fips202/*.h)

SOURCES = $(MLKEM_SOURCES) $(FIPS202_SOURCES)
HEADERS = $(MLKEM_HEADERS) $(FIPS202_HEADERS)

.PHONY: all clean

all: mlkem-512-test mlkem-768-test mlkem-1024-test

mlkem-512-test: test/test.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -DKYBER_K=2 $< $(SOURCES) -o $@

mlkem-768-test: test/test.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -DKYBER_K=3 $< $(SOURCES) -o $@

mlkem-1024-test: test/test.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -DKYBER_K=4 $< $(SOURCES) -o $@

clean: 
	$(RM) mlkem-512-test mlkem-768-test mlkem-1024-test