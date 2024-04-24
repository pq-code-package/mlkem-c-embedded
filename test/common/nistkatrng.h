// SPDX-License-Identifier: Apache-2.0

#ifndef NISTKATRNG_H
#define NISTKATRNG_H

#include "aes.h"

typedef struct {
    unsigned char key[AES256_KEYBYTES];
    unsigned char ctr[AES_BLOCKBYTES];
} AES256_CTR_DRBG_struct;

void randombytes_init(unsigned char *entropy_input, const unsigned char *personalization_string, int security_strength);

#endif
