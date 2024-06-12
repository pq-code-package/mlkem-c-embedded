// SPDX-License-Identifier: Apache-2.0
#ifndef BASEMUL_H
#define BASEMUL_H

#include <stdint.h>
#include <arm_acle.h>

#define __pkhtb(a, b) (((uint32_t)a) & 0xFFFF0000) ^ (((b) >> 16) & 0xFFFF)

static inline void basemul(int32_t r[1], const int32_t a[1], const int32_t b[1], int32_t zeta, int add) {
    int32_t t1, t2;
    int32_t qa = 26632;
    int32_t q = 3329;
    int32_t qinv = 0x6ba8f301;
    int32_t a0 = a[0];
    int32_t b0 = b[0];

    t1 = __smlawt(zeta, b0, 0);
    t1 = __smlabb(t1, q, qa);
    t1 = __smlatt(a0, t1, 0);
    t1 = __smlabb(a0, b0, t1);
    t1 = qinv * t1;
    t1 = __smlatb(t1, q, qa);

    t2 = __smuadx(a0, b0);
    t2 = qinv * t2;
    t2 = __smlatb(t2, q, qa);

    if (add) {
        t1 = __pkhtb(t2, t1);
        r[0] = __uadd16(t1, r[0]);
    } else {
        r[0] = __pkhtb(t2, t1);
    }
}

#endif
