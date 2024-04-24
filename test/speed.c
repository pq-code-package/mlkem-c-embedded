// SPDX-License-Identifier: Apache-2.0
#include "hal.h"
#include "kem.h"
#include "ntt.h"
#include "poly.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void) {
    unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
    poly r, a, b;

    unsigned long long t0, t1;
    int i;

    hal_setup(CLOCK_BENCHMARK);

    hal_send_str("==========================");

    for (i = 0; i < NTESTS; i++) {
        // Key-pair generation
        t0 = hal_get_time();
        crypto_kem_keypair(pk, sk);
        t1 = hal_get_time();
        printcycles("keypair cycles:", t1 - t0);

        // Encapsulation
        t0 = hal_get_time();
        crypto_kem_enc(ct, key_a, pk);
        t1 = hal_get_time();
        printcycles("encaps cycles:", t1 - t0);

        // Decapsulation
        t0 = hal_get_time();
        crypto_kem_dec(key_b, ct, sk);
        t1 = hal_get_time();
        printcycles("decaps cycles:", t1 - t0);

        // NTT
        t0 = hal_get_time();
        ntt(a.coeffs);
        t1 = hal_get_time();
        printcycles("ntt cycles:", t1 - t0);

        // INVNTT
        t0 = hal_get_time();
        invntt(a.coeffs);
        t1 = hal_get_time();
        printcycles("invntt cycles:", t1 - t0);

        // Basemul
        t0 = hal_get_time();
        poly_basemul(&r, &a, &b);
        t1 = hal_get_time();
        printcycles("poly_basemul cycles:", t1 - t0);

        // Basemul_acc
        t0 = hal_get_time();
        poly_basemul_acc(&r, &a, &b);
        t1 = hal_get_time();
        printcycles("poly_basemul_acc cycles:", t1 - t0);

        if (memcmp(key_a, key_b, CRYPTO_BYTES)) {
            hal_send_str("ERROR KEYS\n");
        } else {
            hal_send_str("OK KEYS\n");
        }
        hal_send_str("+");
    }

    hal_send_str("#");

    return 0;
}
