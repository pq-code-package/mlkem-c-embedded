// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#include "kem.h"
#include "randombytes.h"
#include "hal.h"
#include "sendfn.h"

#include <string.h>

#ifndef MAX_STACK_SIZE
#define MAX_STACK_SIZE hal_get_stack_size()
#endif

#ifndef STACK_SIZE_INCR
#define STACK_SIZE_INCR 0x1000
#endif

#define send_stack_usage(S, U) send_unsigned((S), (U))

unsigned int canary_size;
volatile unsigned char *p;
unsigned int c;
uint8_t canary = 0x42;

unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
unsigned char pk[CRYPTO_PUBLICKEYBYTES];
unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
unsigned char sk_a[CRYPTO_SECRETKEYBYTES];
unsigned int stack_key_gen, stack_encaps, stack_decaps;

#define FILL_STACK()                                                           \
    p = &a;                                                                      \
    while (p > &a - canary_size)                                                 \
        *(p--) = canary;
#define CHECK_STACK()                                                          \
    c = canary_size;                                                             \
    p = &a - canary_size + 1;                                                    \
    while (*p == canary && p < &a) {                                             \
        p++;                                                                       \
        c--;                                                                       \
    }

static int test_keys(void) {
    // Alice generates a public key
    hal_spraystack();
    crypto_kem_keypair(pk, sk_a);
    stack_key_gen = hal_checkstack();

    // Bob derives a secret key and creates a response
    hal_spraystack();
    crypto_kem_enc(sendb, key_b, pk);
    stack_encaps = hal_checkstack();

    // Alice uses Bobs response to get her secret key
    hal_spraystack();
    crypto_kem_dec(key_a, sendb, sk_a);
    stack_decaps = hal_checkstack();

    if (memcmp(key_a, key_b, CRYPTO_BYTES)) {
        return -1;
    } else {
        send_stack_usage("keypair stack usage:", stack_key_gen);
        send_stack_usage("encaps stack usage:", stack_encaps);
        send_stack_usage("decaps stack usage:", stack_decaps);
        hal_send_str("OK KEYS\n");
        return 0;
    }
}

int main(void) {
    hal_setup(CLOCK_FAST);

    // marker for automated benchmarks
    SERIAL_MARKER();
    test_keys();
    // marker for automated benchmarks
    SERIAL_MARKER();

    return 0;
}
