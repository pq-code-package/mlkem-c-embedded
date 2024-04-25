// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"

/*************************************************
* Name:        polyvec_tobytes
*
* Description: Serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for MLKEM_POLYVECBYTES)
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void polyvec_tobytes(uint8_t r[MLKEM_POLYVECBYTES], const polyvec *a) {
    unsigned int i;
    for (i = 0; i < MLKEM_K; i++) {
        poly_tobytes(r + i * MLKEM_POLYBYTES, &a->vec[i]);
    }
}

/*************************************************
* Name:        polyvec_ntt
*
* Description: Apply forward NTT to all elements of a vector of polynomials
*
* Arguments:   - polyvec *r: pointer to in/output vector of polynomials
**************************************************/
void polyvec_ntt(polyvec *r) {
    unsigned int i;
    for (i = 0; i < MLKEM_K; i++) {
        poly_ntt(&r->vec[i]);
    }
}
