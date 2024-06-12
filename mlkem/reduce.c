// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#include <stdint.h>
#include "params.h"
#include "reduce.h"

/*************************************************
* Name:        barrett_reduce
*
* Description: Barrett reduction; given a 16-bit integer a, computes
*              centered representative congruent to a mod q in {-(q-1)/2,...,(q-1)/2}
*
* Arguments:   - int16_t a: input integer to be reduced
*
* Returns:     integer in {-(q-1)/2,...,(q-1)/2} congruent to a modulo q.
**************************************************/
int16_t barrett_reduce(int16_t a) {
    int16_t t;
    const int16_t v = ((1 << 26) + MLKEM_Q / 2) / MLKEM_Q;

    t  = ((int32_t)v * a + (1 << 25)) >> 26;
    t *= MLKEM_Q;
    return a - t;
}
