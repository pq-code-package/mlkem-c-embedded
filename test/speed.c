// SPDX-License-Identifier: Apache-2.0
#include "hal.h"
#include "kem.h"
#include "sendfn.h"

#include <stdint.h>
#include <string.h>

#if !defined(MPS2_AN386)
#define SERIAL_MARKER() {\
        hal_send_str("$");\
    }
#else
#define SERIAL_MARKER()
#endif

#define printcycles(S, U) send_unsignedll((S), (U))

int main(void) {
    unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
    unsigned long long t0, t1;
    int i;

    hal_setup(CLOCK_BENCHMARK);

    SERIAL_MARKER();

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

        if (memcmp(key_a, key_b, CRYPTO_BYTES)) {
            hal_send_str("ERROR KEYS\n");
        } else {
            hal_send_str("OK KEYS\n");
        }
        hal_send_str("+");
    }

    SERIAL_MARKER();

    return 0;
}
