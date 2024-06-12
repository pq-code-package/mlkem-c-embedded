// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

#define twiddles_plantard_basemul MLKEM_NAMESPACE(twiddles_plantard_basemul)
extern const int32_t twiddles_plantard_basemul[64];

#define ntt MLKEM_NAMESPACE(ntt)
void ntt(int16_t poly[256]);

#define invntt MLKEM_NAMESPACE(invntt)
void invntt(int16_t poly[256]);

#define basemul_plantard MLKEM_NAMESPACE(basemul_plantard)
void basemul_plantard(int16_t r[256], const int16_t a[256], const int16_t b[256], int add);

#define frombytes_basemul_plantard MLKEM_NAMESPACE(frombytes_basemul_plantard)
void frombytes_basemul_plantard(int16_t r[256], const int16_t b[256], const unsigned char *a, int add);

#endif
