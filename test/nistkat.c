// SPDX-License-Identifier: CC0-1.0

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "kem.h"
#include "hal.h"
#include "randombytes.h"
#include "nistkatrng.h"
#include "fips202.h"

// NOTE: used Kyber in the nistkat rsp file for now to avoid changing the checksum
#if   (MLKEM_K == 2)
#define OLD_CRYPTO_ALGNAME "Kyber512"
#elif (MLKEM_K == 3)
#define OLD_CRYPTO_ALGNAME "Kyber768"
#elif (MLKEM_K == 4)
#define OLD_CRYPTO_ALGNAME "Kyber1024"
#endif

size_t format_bstr(char *buf, const char *S, const uint8_t *A, size_t L) {
    size_t len = strlen(S);
    memcpy(buf, S, strlen(S));

    for (size_t i = 0; i < L; i++) {
        len += snprintf(buf + strlen(S) + 2 * i, 3, "%02X", A[i]);
    }

    return len;
}

void inc_hash_bstr(shake256incctx *state, const char *S, const uint8_t *A, size_t L) {
    size_t len;
    char buf[strlen(S) + 2 * L + 1];

    len = format_bstr(buf, S, A, L);
    len += snprintf(buf + strlen(S) + 2 * L, 2, "\n");

    shake256_inc_absorb(state, (unsigned char *)buf, len);
}

void hal_send_bstr(const char *S, const uint8_t *A, size_t L) {
    char buf[strlen(S) + 2 * L + 1];
    format_bstr(buf, S, A, L);
    hal_send_str(buf);
}

size_t inc_hash_format(shake256incctx *state, const char *S, ...) {
    size_t len;
    char buf[1024];
    va_list args;
    va_start(args, S);
    len = vsnprintf(buf, 1024, S, args);
    va_end(args);

    shake256_inc_absorb(state, (unsigned char *)buf, len);

    return len;
}

int main(void) {
    uint8_t counts = 100;
    uint8_t entropy_input[48];
    uint8_t seeds[counts][48];
    uint8_t public_key[CRYPTO_PUBLICKEYBYTES];
    uint8_t secret_key[CRYPTO_SECRETKEYBYTES];
    uint8_t ciphertext[CRYPTO_CIPHERTEXTBYTES];
    uint8_t shared_secret_e[CRYPTO_BYTES];
    uint8_t shared_secret_d[CRYPTO_BYTES];
    uint8_t result[32];
    shake256incctx hash_state;
    int rc;

    hal_setup(CLOCK_FAST);
    SERIAL_MARKER();

    for (uint8_t i = 0; i < 48; i++) {
        entropy_input[i] = i;
    }

    randombytes_init(entropy_input, NULL, 256);

    for (size_t i = 0; i < counts; i++) {
        randombytes(seeds[i], 48);
    }

    shake256_inc_init(&hash_state);

    inc_hash_format(&hash_state, "# %s\n\n", OLD_CRYPTO_ALGNAME);

    for (size_t i = 0; i < counts; i++) {
        inc_hash_format(&hash_state, "count = %d\n", i);
        randombytes_init(seeds[i], NULL, 256);
        inc_hash_bstr(&hash_state, "seed = ", seeds[i], 48);

        rc = crypto_kem_keypair(public_key, secret_key);
        if (rc != 0) {
            hal_send_str("ERROR: crypto_kem_keypair failed!");
            return -1;
        }
        inc_hash_bstr(&hash_state, "pk = ", public_key, CRYPTO_PUBLICKEYBYTES);
        inc_hash_bstr(&hash_state, "sk = ", secret_key, CRYPTO_SECRETKEYBYTES);

        rc = crypto_kem_enc(ciphertext, shared_secret_e, public_key);
        if (rc != 0) {
            hal_send_str("ERROR: crypto_kem_enc failed!");
            return -2;
        }
        inc_hash_bstr(&hash_state, "ct = ", ciphertext, CRYPTO_CIPHERTEXTBYTES);
        inc_hash_bstr(&hash_state, "ss = ", shared_secret_e, CRYPTO_BYTES);

        rc = crypto_kem_dec(shared_secret_d, ciphertext, secret_key);
        if (rc != 0) {
            hal_send_str("ERROR: crypto_kem_dec failed!");
            return -3;
        }

        rc = memcmp(shared_secret_e, shared_secret_d, CRYPTO_BYTES);
        if (rc != 0) {
            hal_send_str("ERROR shared secrets are not equal");
            return -4;
        }

        inc_hash_format(&hash_state, "\n");
    }
    shake256_inc_finalize(&hash_state);
    shake256_inc_squeeze(result, 32, &hash_state);
    hal_send_bstr("", result, 32);
    SERIAL_MARKER();

    return 0;
}
