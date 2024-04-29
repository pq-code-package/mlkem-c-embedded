// SPDX-License-Identifier: Apache-2.0

#ifndef NISTKATRNG_H
#define NISTKATRNG_H

#include "aes.h"

void randombytes_init(unsigned char entropy_input[AES256_KEYBYTES + AES_BLOCKBYTES], const unsigned char personalization_string[AES256_KEYBYTES + AES_BLOCKBYTES], int security_strength);

#endif
