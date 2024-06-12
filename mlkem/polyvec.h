// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#ifndef POLYVEC_H
#define POLYVEC_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

typedef struct {
    poly vec[MLKEM_K];
} polyvec;

#define polyvec_tobytes MLKEM_NAMESPACE(polyvec_tobytes)
void polyvec_tobytes(uint8_t r[MLKEM_POLYVECBYTES], const polyvec *a);

#define polyvec_ntt MLKEM_NAMESPACE(polyvec_ntt)
void polyvec_ntt(polyvec *r);

#endif
