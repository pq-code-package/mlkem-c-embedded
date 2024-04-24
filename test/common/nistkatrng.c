// SPDX-License-Identifier: Apache-2.0

#include <assert.h>
#include <string.h>

#include "aes.h"
#include "randombytes.h"
#include "nistkatrng.h"

static AES256_CTR_DRBG_struct DRBG_ctx;
static void AES256_CTR_DRBG_Update(const unsigned char *provided_data, unsigned char *key, unsigned char *ctr);
static void aes256_block_update(uint8_t *block);

// This uses AES from openSSL library
//    key - 256-bit AES key
//    ctr - a 128-bit plaintext value
//    buffer - a 128-bit ciphertext value
static void AES256_ECB(unsigned char *key, unsigned char *ctr, unsigned char *buffer) {
    aes256ctx ctx;
    aes256_ecb_keyexp(&ctx, key);
    aes256_ecb(buffer, ctr, 1, &ctx);
    aes256_ctx_release(&ctx);
}

void randombytes_init(unsigned char *entropy_input, const unsigned char *personalization_string, int security_strength) {
    int len = AES256_KEYBYTES + AES_BLOCKBYTES;
    uint8_t seed_material[len];

    assert(security_strength == 256);
    memcpy(seed_material, entropy_input, len);
    if (personalization_string) {
        for (int i = 0; i < len; i++) {
            seed_material[i] ^= personalization_string[i];
        }
    }
    memset(DRBG_ctx.key, 0x00, AES256_KEYBYTES);
    memset(DRBG_ctx.ctr, 0x00, AES_BLOCKBYTES);
    AES256_CTR_DRBG_Update(seed_material, DRBG_ctx.key, DRBG_ctx.ctr);
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

    AES256_CTR_DRBG_Update(NULL, DRBG_ctx.key, DRBG_ctx.ctr);
    return 0;
}

static void AES256_CTR_DRBG_Update(const unsigned char *provided_data, unsigned char *key, unsigned char *ctr) {
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

static void aes256_block_update(uint8_t *block) {
    for (int j = AES_BLOCKBYTES - 1; j >= 0; j--) {
        DRBG_ctx.ctr[j]++;

        if (DRBG_ctx.ctr[j] != 0x00) {
            break;
        }
    }

    AES256_ECB(DRBG_ctx.key, DRBG_ctx.ctr, block);
}
