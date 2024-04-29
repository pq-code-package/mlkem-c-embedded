// SPDX-License-Identifier: Apache-2.0

#include <assert.h>
#include <string.h>

#include "aes.h"
#include "randombytes.h"
#include "nistkatrng.h"

typedef struct {
    unsigned char key[AES256_KEYBYTES];
    unsigned char ctr[AES_BLOCKBYTES];
} nistkatctx;

static nistkatctx ctx;

static void _aes256_ecb(unsigned char key[AES256_KEYBYTES], unsigned char ctr[AES_BLOCKBYTES], unsigned char buffer[AES_BLOCKBYTES]) {
    aes256ctx aesctx;
    aes256_ecb_keyexp(&aesctx, key);
    aes256_ecb(buffer, ctr, 1, &aesctx);
    aes256_ctx_release(&aesctx);
}

static void aes256_block_update(uint8_t block[AES_BLOCKBYTES]) {
    for (int j = AES_BLOCKBYTES - 1; j >= 0; j--) {
        ctx.ctr[j]++;

        if (ctx.ctr[j] != 0x00) {
            break;
        }
    }

    _aes256_ecb(ctx.key, ctx.ctr, block);
}

static void nistkat_update(const unsigned char *provided_data, unsigned char *key, unsigned char *ctr) {
    int len = AES256_KEYBYTES + AES_BLOCKBYTES;
    uint8_t tmp[len];

    for (int i = 0; i < len / AES_BLOCKBYTES; i++) {
        aes256_block_update(tmp + AES_BLOCKBYTES * i);
    }

    if (provided_data) {
        for (int i = 0; i < len; i++) {
            tmp[i] ^= provided_data[i];
        }
    }

    memcpy(key, tmp, AES256_KEYBYTES);
    memcpy(ctr, tmp + AES256_KEYBYTES, AES_BLOCKBYTES);
}

void randombytes_init(unsigned char entropy_input[AES256_KEYBYTES + AES_BLOCKBYTES], const unsigned char personalization_string[AES256_KEYBYTES + AES_BLOCKBYTES], int security_strength) {
    int len = AES256_KEYBYTES + AES_BLOCKBYTES;
    uint8_t seed_material[len];
    (void) security_strength;

    memcpy(seed_material, entropy_input, len);
    if (personalization_string) {
        for (int i = 0; i < len; i++) {
            seed_material[i] ^= personalization_string[i];
        }
    }
    memset(ctx.key, 0x00, AES256_KEYBYTES);
    memset(ctx.ctr, 0x00, AES_BLOCKBYTES);
    nistkat_update(seed_material, ctx.key, ctx.ctr);
}

int randombytes(uint8_t *buf, size_t n) {
    uint8_t block[AES_BLOCKBYTES];

    size_t nb = n / AES_BLOCKBYTES;
    size_t tail = n % AES_BLOCKBYTES;

    for (size_t i = 0; i < nb; i++) {
        aes256_block_update(block);
        memcpy(buf + i * AES_BLOCKBYTES, block, AES_BLOCKBYTES);
    }

    if (tail > 0) {
        aes256_block_update(block);
        memcpy(buf + nb * AES_BLOCKBYTES, block, tail);
    }

    nistkat_update(NULL, ctx.key, ctx.ctr);
    return 0;
}
