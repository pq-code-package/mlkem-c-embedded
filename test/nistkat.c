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

// NOTE: used Kyber in the nistkat rsp file for now to avoid changing the checksum
#if   (MLKEM_K == 2)
#define OLD_CRYPTO_ALGNAME "Kyber512"
#elif (MLKEM_K == 3)
#define OLD_CRYPTO_ALGNAME "Kyber768"
#elif (MLKEM_K == 4)
#define OLD_CRYPTO_ALGNAME "Kyber1024"
#endif

static void hal_send_Bstr(const char *S, const uint8_t *A, size_t L) {
    size_t i;
    char buf[16384];
    memcpy(buf, S, strlen(S));
    for (i = 0; i < L; i++) {
        snprintf(buf + strlen(S) + 2 * i, 3, "%02X", A[i]);
    }
    if (L == 0) {
        snprintf(buf + strlen(S) + 2 * L, 3, "00");
    }

    hal_send_str(buf);
}

int hal_send_format(const char *S, ...) {
    int result;
    char buf[1024];
    va_list args;
    va_start(args, S);
    result = vsnprintf(buf, 1024, S, args);
    va_end(args);

    hal_send_str(buf);

    return result;
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

    hal_send_format("# %s\n", OLD_CRYPTO_ALGNAME);

    for (size_t i = 0; i < counts; i++) {
        hal_send_format("count = %d", i);
        randombytes_init(seeds[i], NULL, 256);
        hal_send_Bstr("seed = ", seeds[i], 48);

        rc = crypto_kem_keypair(public_key, secret_key);
        if (rc != 0) {
            hal_send_format("[kat_kem] %s ERROR: crypto_kem_keypair failed!\n", CRYPTO_ALGNAME);
            return -1;
        }
        hal_send_Bstr("pk = ", public_key, CRYPTO_PUBLICKEYBYTES);
        hal_send_Bstr("sk = ", secret_key, CRYPTO_SECRETKEYBYTES);

        rc = crypto_kem_enc(ciphertext, shared_secret_e, public_key);
        if (rc != 0) {
            hal_send_format("[kat_kem] %s ERROR: crypto_kem_enc failed!\n", CRYPTO_ALGNAME);
            return -2;
        }
        hal_send_Bstr("ct = ", ciphertext, CRYPTO_CIPHERTEXTBYTES);
        hal_send_Bstr("ss = ", shared_secret_e, CRYPTO_BYTES);

        rc = crypto_kem_dec(shared_secret_d, ciphertext, secret_key);
        if (rc != 0) {
            hal_send_format("[kat_kem] %s ERROR: crypto_kem_dec failed!\n", CRYPTO_ALGNAME);
            return -3;
        }

        rc = memcmp(shared_secret_e, shared_secret_d, CRYPTO_BYTES);
        if (rc != 0) {
            hal_send_str("ERROR shared secrets are not equal");
            return -4;
        }

        hal_send_str("");
    }
    SERIAL_MARKER();

    return 0;

}
