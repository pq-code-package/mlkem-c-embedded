// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

#define poly_getnoise_eta1(p, seed, nonce) poly_noise_eta1(p, seed, nonce, 0)
#define poly_addnoise_eta1(p, seed, nonce) poly_noise_eta1(p, seed, nonce, 1)

/*
 * Elements of R_q = Z_q[X]/(X^n + 1). Represents polynomial
 * coeffs[0] + X*coeffs[1] + X^2*coeffs[2] + ... + X^{n-1}*coeffs[n-1]
 */
typedef struct {
    int16_t coeffs[MLKEM_N];
} poly;

#define poly_compress MLKEM_NAMESPACE(poly_compress)
void poly_compress(uint8_t r[MLKEM_POLYCOMPRESSEDBYTES], const poly *a);
#define poly_decompress MLKEM_NAMESPACE(poly_decompress)
void poly_decompress(poly *r, const uint8_t a[MLKEM_POLYCOMPRESSEDBYTES]);

#define poly_tobytes MLKEM_NAMESPACE(poly_tobytes)
void poly_tobytes(uint8_t r[MLKEM_POLYBYTES], const poly *a);
#define poly_frombytes MLKEM_NAMESPACE(poly_frombytes)
void poly_frombytes(poly *r, const uint8_t a[MLKEM_POLYBYTES]);

#define poly_frommsg MLKEM_NAMESPACE(poly_frommsg)
void poly_frommsg(poly *r, const uint8_t msg[MLKEM_INDCPA_MSGBYTES]);
#define poly_tomsg MLKEM_NAMESPACE(poly_tomsg)
void poly_tomsg(uint8_t msg[MLKEM_INDCPA_MSGBYTES], const poly *r);

#define poly_noise_eta1 MLKEM_NAMESPACE(poly_noise_eta1)
void poly_noise_eta1(poly *r, const unsigned char *seed, unsigned char nonce, int add);

#define poly_getnoise_eta2 MLKEM_NAMESPACE(poly_getnoise_eta2)
void poly_getnoise_eta2(poly *r, const uint8_t seed[MLKEM_SYMBYTES], uint8_t nonce);

#define poly_ntt MLKEM_NAMESPACE(poly_ntt)
void poly_ntt(poly *r);
#define poly_invntt_tomont MLKEM_NAMESPACE(poly_invntt_tomont)
void poly_invntt_tomont(poly *r);
#define poly_basemul_montgomery MLKEM_NAMESPACE(poly_basemul_montgomery)
void poly_basemul_montgomery(poly *r, const poly *a, const poly *b);
#define poly_tomont MLKEM_NAMESPACE(poly_tomont)
void poly_tomont(poly *r);

#define poly_reduce MLKEM_NAMESPACE(poly_reduce)
void poly_reduce(poly *r);

#define poly_add MLKEM_NAMESPACE(poly_add)
void poly_add(poly *r, const poly *a, const poly *b);
#define poly_sub MLKEM_NAMESPACE(poly_sub)
void poly_sub(poly *r, const poly *a, const poly *b);

void poly_frommont(poly *r);

#endif
