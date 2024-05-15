// SPDX-License-Identifier: Apache-2.0
#include "hal.h"
#include "kem.h"
#include "sendfn.h"
#include "randombytes.h"

#include <stdint.h>
#include <string.h>

#define printcycles(S, U) send_unsignedll((S), (U))

static int cmp_uint64_t(const void *a, const void *b) {
    return (int)((*((const uint64_t *)a)) - (*((const uint64_t *)b)));
}

int main(void) {
    unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
    unsigned char kg_rand[2 * CRYPTO_BYTES], enc_rand[CRYPTO_BYTES];
    uint64_t cycles_kg[NTESTS], cycles_enc[NTESTS], cycles_dec[NTESTS];
    unsigned long long t0, t1;
    int i;

    hal_setup(CLOCK_BENCHMARK);

    SERIAL_MARKER();

    randombytes(kg_rand, 2 * CRYPTO_BYTES);
    randombytes(enc_rand, CRYPTO_BYTES);

    for (i = 0; i < NTESTS; i++) {
        // Key-pair generation
        t0 = hal_get_time();
        crypto_kem_keypair_derand(pk, sk, kg_rand);
        t1 = hal_get_time();
        cycles_kg[i] = t1 - t0;
    }

    for (i = 0; i < NTESTS; i++) {
        // Encapsulation
        t0 = hal_get_time();
        crypto_kem_enc_derand(ct, key_a, pk, enc_rand);
        t1 = hal_get_time();
        cycles_enc[i] = t1 - t0;
    }

    for (i = 0; i < NTESTS; i++) {
        // Decapsulation
        t0 = hal_get_time();
        crypto_kem_dec(key_b, ct, sk);
        t1 = hal_get_time();
        cycles_dec[i] = t1 - t0;
    }

    if (memcmp(key_a, key_b, CRYPTO_BYTES)) {
        hal_send_str("ERROR KEYS\n");
    } else {
        hal_send_str("OK KEYS\n");
    }
    hal_send_str("+");

    qsort(cycles_kg, NTESTS, sizeof(uint64_t), cmp_uint64_t);
    qsort(cycles_enc, NTESTS, sizeof(uint64_t), cmp_uint64_t);
    qsort(cycles_dec, NTESTS, sizeof(uint64_t), cmp_uint64_t);

    printcycles("keypair cycles", cycles_kg[NTESTS >> 1]);
    printcycles("encaps cycles", cycles_enc[NTESTS >> 1]);
    printcycles("decaps cycles", cycles_dec[NTESTS >> 1]);

    SERIAL_MARKER();

    return 0;
}
