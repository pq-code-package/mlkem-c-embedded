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

// https://stackoverflow.com/a/1489985/1711232
#define PASTER(x, y) x##y
#define EVALUATOR(x, y) PASTER(x, y)
#define NAMESPACE(fun) EVALUATOR(MUPQ_NAMESPACE, fun)

// use different names so we can have empty namespaces
#define MUPQ_CRYPTO_BYTES           NAMESPACE(CRYPTO_BYTES)
#define MUPQ_CRYPTO_PUBLICKEYBYTES  NAMESPACE(CRYPTO_PUBLICKEYBYTES)
#define MUPQ_CRYPTO_SECRETKEYBYTES  NAMESPACE(CRYPTO_SECRETKEYBYTES)
#define MUPQ_CRYPTO_CIPHERTEXTBYTES NAMESPACE(CRYPTO_CIPHERTEXTBYTES)
#define MUPQ_CRYPTO_ALGNAME NAMESPACE(CRYPTO_ALGNAME)

#define MUPQ_crypto_kem_keypair NAMESPACE(crypto_kem_keypair)
#define MUPQ_crypto_kem_enc NAMESPACE(crypto_kem_enc)
#define MUPQ_crypto_kem_dec NAMESPACE(crypto_kem_dec)

#define send_stack_usage(S, U) send_unsigned((S), (U))

unsigned int canary_size;
volatile unsigned char *p;
unsigned int c;
uint8_t canary = 0x42;

unsigned char key_a[MUPQ_CRYPTO_BYTES], key_b[MUPQ_CRYPTO_BYTES];
unsigned char pk[MUPQ_CRYPTO_PUBLICKEYBYTES];
unsigned char sendb[MUPQ_CRYPTO_CIPHERTEXTBYTES];
unsigned char sk_a[MUPQ_CRYPTO_SECRETKEYBYTES];
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
  MUPQ_crypto_kem_keypair(pk, sk_a);
  stack_key_gen = hal_checkstack();

  // Bob derives a secret key and creates a response
  hal_spraystack();
  MUPQ_crypto_kem_enc(sendb, key_b, pk);
  stack_encaps = hal_checkstack();

  // Alice uses Bobs response to get her secret key
  hal_spraystack();
  MUPQ_crypto_kem_dec(key_a, sendb, sk_a);
  stack_decaps = hal_checkstack();

  if (memcmp(key_a, key_b, MUPQ_CRYPTO_BYTES)){
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
  hal_send_str("==========================");
  test_keys();
  // marker for automated benchmarks
  hal_send_str("#");

  return 0;
}
