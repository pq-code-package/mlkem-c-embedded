#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"
#include "kem.h"

#define NTESTS 1000

static int test_keys(void)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
  uint8_t key_a[CRYPTO_BYTES];
  uint8_t key_b[CRYPTO_BYTES];

  //Alice generates a public key
  crypto_kem_keypair(pk, sk);

  //Bob derives a secret key and creates a response
  crypto_kem_enc(ct, key_b, pk);

  //Alice uses Bobs response to get her shared key
  crypto_kem_dec(key_a, ct, sk);

  if(memcmp(key_a, key_b, CRYPTO_BYTES)) {
    hal_send_str("ERROR keys\n");
    return 1;
  }

  return 0;
}

int main(void)
{
  unsigned int i;
  int r = 0;

  hal_setup(CLOCK_FAST);
  hal_send_str("\n");

  char buf[16];
  snprintf(buf, 24, "k=%d\n", KYBER_K);
  hal_send_str(buf);

  snprintf(buf, 24, "|pk|=%d\n", CRYPTO_PUBLICKEYBYTES);
  hal_send_str(buf);

  snprintf(buf, 24, "|sk|=%d\n", CRYPTO_SECRETKEYBYTES);
  hal_send_str(buf);

  snprintf(buf, 24, "|ct|=%d\n", CRYPTO_CIPHERTEXTBYTES);
  hal_send_str(buf);

  for(i=0;i<NTESTS;i++) {
    r  |= test_keys();
  }

  if(r){
    hal_send_str("ERRORS\n");
  } else {
    hal_send_str("OK\n");
  }


  return 0;
}
