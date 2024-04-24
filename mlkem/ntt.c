// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#include <stdint.h>
#include "params.h"
#include "ntt.h"
#include "reduce.h"
#include <arm_acle.h>

/* Code to generate zetas and zetas_inv used in the number-theoretic transform:

#define MLKEM_ROOT_OF_UNITY 17

static const uint8_t tree[128] = {
  0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120,
  4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124,
  2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122,
  6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126,
  1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121,
  5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125,
  3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123,
  7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127
};

void init_ntt() {
  unsigned int i;
  int16_t tmp[128];

  tmp[0] = MONT;
  for(i=1;i<128;i++)
    tmp[i] = fqmul(tmp[i-1],MONT*MLKEM_ROOT_OF_UNITY % MLKEM_Q);

  for(i=0;i<128;i++) {
    zetas[i] = tmp[tree[i]];
    if(zetas[i] > MLKEM_Q/2)
      zetas[i] -= MLKEM_Q;
    if(zetas[i] < -MLKEM_Q/2)
      zetas[i] += MLKEM_Q;
  }
}
*/

const int16_t zetas[128] = {
    -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
    -171,   622,  1577,   182,   962, -1202, -1474,  1468,
    573, -1325,   264,   383,  -829,  1458, -1602,  -130,
    -681,  1017,   732,   608, -1542,   411,  -205, -1571,
    1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
    516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
    -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
    -398,   961, -1508,  -725,   448, -1065,   677, -1275,
    -1103,   430,   555,   843, -1251,   871,  1550,   105,
    422,   587,   177,  -235,  -291,  -460,  1574,  1653,
    -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
    -1590,   644,  -872,   349,   418,   329,  -156,   -75,
    817,  1097,   603,   610,  1322, -1285, -1465,   384,
    -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
    -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
    -108,  -308,   996,   991,   958, -1460,  1522,  1628
};

static const int32_t zetas_plant[] = {
    2230699446, 3328631909, 4243360600, 3408622288, 812805467, 2447447570, 1094061961,
    1370157786, 381889553, 3157039644, 372858381, 427045412, 4196914574, 2265533966,
    2475831253, 1727534158, 1904287092, 1544330386, 2972545705, 2937711185, 2651294021,
    249002310, 3929849920, 72249375, 838608815, 2550660963, 3242190693, 815385801,
    1028263423, 2889974991, 1719793153, 3696329620, 42575525, 1703020977, 2470670584,
    3594406395, 1839778722, 2701610550, 2991898216, 1851390229, 1041165097, 583155668,
    4205945745, 690239563, 3718262466, 1855260731, 3700200122, 1979116802, 3098982111,
    734105255, 3087370604, 3714391964, 3415073125, 3376368103, 1910737929, 836028480,
    2252632292, 2546790461, 1059227441, 3191874164, 4012420634, 1583035408, 1174052340,
    21932846, 3562152210, 752167598, 3417653460, 2112004045, 932791035, 2951903026,
    1419184148, 1817845876, 3434425636, 4233039261, 300609006, 975366560, 2781600929,
    3889854731, 3935010590, 2197155094, 2130066389, 3598276897, 2308109491, 2382939200,
    1228239371, 1884934581, 3466679822, 1211467195, 2977706375, 3144137970, 3080919767,
    945692709, 3015121229, 345764865, 826997308, 2043625172, 2964804700, 2628071007,
    4154339049, 483812778, 3288636719, 2696449880, 2122325384, 1371447954, 411563403,
    3577634219, 976656727, 2708061387, 723783916, 3181552825, 3346694253, 3617629408,
    1408862808, 519937465, 1323711759, 1474661346, 2773859924, 3580214553, 1143088323,
    2221668274, 1563682897, 2417773720, 1327582262, 2722253228, 3786641338, 1141798155,
    2779020594
};

static const int32_t zetas_plant_basemul[64] = {21932846, 3562152210, 752167598, 3417653460, 2112004045, 932791035, 2951903026, 1419184148, 1817845876, 3434425636, 4233039261, 300609006, 975366560, 2781600929, 3889854731, 3935010590, 2197155094, 2130066389, 3598276897, 2308109491, 2382939200, 1228239371, 1884934581, 3466679822, 1211467195, 2977706375, 3144137970, 3080919767, 945692709, 3015121229, 345764865, 826997308, 2043625172, 2964804700, 2628071007, 4154339049, 483812778, 3288636719, 2696449880, 2122325384, 1371447954, 411563403, 3577634219, 976656727, 2708061387, 723783916, 3181552825, 3346694253, 3617629408, 1408862808, 519937465, 1323711759, 1474661346, 2773859924, 3580214553, 1143088323, 2221668274, 1563682897, 2417773720, 1327582262, 2722253228, 3786641338, 1141798155, 2779020594};

/*************************************************
* Name:        fqmul
*
* Description: Multiplication followed by Montgomery reduction
*
* Arguments:   - int16_t a: first factor
*              - int16_t b: second factor
*
* Returns 16-bit integer congruent to a*b*R^{-1} mod q
**************************************************/
static int16_t fqmul(int16_t a, int16_t b) {
    return montgomery_reduce((int32_t)a * b);
}

#define __pkhtb(a, b) (((uint32_t)a) & 0xFFFF0000) ^ (((b) >> 16) & 0xFFFF)
static int32_t plantard(int32_t a, int32_t twiddle) {
    int32_t qa = 26632;
    int32_t q = 3329;

    int32_t t1 = __smlawb(twiddle, a, 0);
    int32_t t2 = __smlawt(twiddle, a, 0);
    t1 = __smlabb(t1, q, qa);
    t2 = __smlabb(t2, q, qa);
    return __pkhtb(t2, t1);
}

static void doublebutterfly(int16_t *r1_16, int16_t *r2_16, int32_t z) {
    int32_t *r1 = (int32_t *) r1_16;
    int32_t *r2 = (int32_t *) r2_16;

    int32_t t = plantard(*r2, z);
    *r2 = __usub16(*r1, t);
    *r1 = __uadd16(*r1, t);
}

/*************************************************
* Name:        ntt
*
* Description: Inplace number-theoretic transform (NTT) in Rq.
*              input is in standard order, output is in bitreversed order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
void ntt(int16_t r[256]) {
    unsigned int len, j, i;
    // layer 1+2+3
    for (j = 0; j < 16; j++) {
        len = 128;
        doublebutterfly(r + 2 * j + 0 * 32, r + 2 * j + len + 0 * 32, zetas_plant[0]);
        doublebutterfly(r + 2 * j + 1 * 32, r + 2 * j + len + 1 * 32, zetas_plant[0]);
        doublebutterfly(r + 2 * j + 2 * 32, r + 2 * j + len + 2 * 32, zetas_plant[0]);
        doublebutterfly(r + 2 * j + 3 * 32, r + 2 * j + len + 3 * 32, zetas_plant[0]);
        len = 64;
        doublebutterfly(r + 2 * j + 0 * 32, r + 2 * j + len + 0 * 32, zetas_plant[1]);
        doublebutterfly(r + 2 * j + 1 * 32, r + 2 * j + len + 1 * 32, zetas_plant[1]);
        doublebutterfly(r + 2 * j + 4 * 32, r + 2 * j + len + 4 * 32, zetas_plant[2]);
        doublebutterfly(r + 2 * j + 5 * 32, r + 2 * j + len + 5 * 32, zetas_plant[2]);
        len = 32;
        doublebutterfly(r + 2 * j + 0 * 32, r + 2 * j + len + 0 * 32, zetas_plant[3]);
        doublebutterfly(r + 2 * j + 2 * 32, r + 2 * j + len + 2 * 32, zetas_plant[4]);
        doublebutterfly(r + 2 * j + 4 * 32, r + 2 * j + len + 4 * 32, zetas_plant[5]);
        doublebutterfly(r + 2 * j + 6 * 32, r + 2 * j + len + 6 * 32, zetas_plant[6]);
    }

    // layer 4+5+6
    const int32_t *zeta_ptr = zetas_plant + 7;
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 2; i++) {
            len = 16;
            doublebutterfly(r + 2 * i + 32 * j + 0 * 4, r + 2 * i + 32 * j + len + 0 * 4, zeta_ptr[0]);
            doublebutterfly(r + 2 * i + 32 * j + 1 * 4, r + 2 * i + 32 * j + len + 1 * 4, zeta_ptr[0]);
            doublebutterfly(r + 2 * i + 32 * j + 2 * 4, r + 2 * i + 32 * j + len + 2 * 4, zeta_ptr[0]);
            doublebutterfly(r + 2 * i + 32 * j + 3 * 4, r + 2 * i + 32 * j + len + 3 * 4, zeta_ptr[0]);

            len = 8;
            doublebutterfly(r + 2 * i + 32 * j + 0 * 4, r + 2 * i + 32 * j + len + 0 * 4, zeta_ptr[1]);
            doublebutterfly(r + 2 * i + 32 * j + 1 * 4, r + 2 * i + 32 * j + len + 1 * 4, zeta_ptr[1]);
            doublebutterfly(r + 2 * i + 32 * j + 4 * 4, r + 2 * i + 32 * j + len + 4 * 4, zeta_ptr[2]);
            doublebutterfly(r + 2 * i + 32 * j + 5 * 4, r + 2 * i + 32 * j + len + 5 * 4, zeta_ptr[2]);

            len = 4;
            doublebutterfly(r + 2 * i +  32 * j + 0 * 4, r + 2 * i +  32 * j + len + 0 * 4, zeta_ptr[3]);
            doublebutterfly(r + 2 * i +  32 * j + 2 * 4, r + 2 * i +  32 * j + len + 2 * 4, zeta_ptr[4]);
            doublebutterfly(r + 2 * i +  32 * j + 4 * 4, r + 2 * i +  32 * j + len + 4 * 4, zeta_ptr[5]);
            doublebutterfly(r + 2 * i +  32 * j + 6 * 4, r + 2 * i +  32 * j + len + 6 * 4, zeta_ptr[6]);
        }
        zeta_ptr += 7;
    }
    zeta_ptr = zetas_plant + 63;

    // layer 7
    len = 2;
    for (j = 0; j < 64; j++) {
        doublebutterfly(r + 4 * j, r + 4 * j + len, zeta_ptr[j]);
    }
}

/*************************************************
* Name:        invntt_tomont
*
* Description: Inplace inverse number-theoretic transform in Rq and
*              multiplication by Montgomery factor 2^16.
*              Input is in bitreversed order, output is in standard order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
void invntt(int16_t r[256]) {
    unsigned int start, len, j, k;
    int16_t t, zeta;
    const int16_t f = 1441; // mont^2/128

    k = 127;
    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < 256; start = j + len) {
            zeta = zetas[k--];
            for (j = start; j < start + len; j++) {
                t = r[j];
                r[j] = barrett_reduce(t + r[j + len]);
                r[j + len] = r[j + len] - t;
                r[j + len] = fqmul(zeta, r[j + len]);
            }
        }
    }

    for (j = 0; j < 256; j++) {
        r[j] = fqmul(r[j], f);
    }
}

/*************************************************
* Name:        basemul
*
* Description: Multiplication of polynomials in Zq[X]/(X^2-zeta)
*              used for multiplication of elements in Rq in NTT domain
*
* Arguments:   - int16_t r[2]: pointer to the output polynomial
*              - const int16_t a[2]: pointer to the first factor
*              - const int16_t b[2]: pointer to the second factor
*              - int16_t zeta: integer defining the reduction polynomial
**************************************************/
void basemul(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta) {
    // copy to allow overlap between r and a,b
    int16_t a0 = a[0];
    int16_t a1 = a[1];
    int16_t b0 = b[0];
    int16_t b1 = b[1];

    r[0]  = fqmul(a1, b1);
    r[0]  = fqmul(r[0], zeta);
    r[0] += fqmul(a0, b0);
    r[1]  = fqmul(a0, b1);
    r[1] += fqmul(a1, b0);
}

/*************************************************
* Name:        basemul_acc
*
* Description: Multiplication of polynomials in Zq[X]/(X^2-zeta)
*              used for multiplication of elements in Rq in NTT domain.
*              Accumulating version
*
* Arguments:   - int16_t r[2]: pointer to the output polynomial
*              - const int16_t a[2]: pointer to the first factor
*              - const int16_t b[2]: pointer to the second factor
*              - int16_t zeta: integer defining the reduction polynomial
**************************************************/
void basemul_acc(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta) {
    // copy to allow overlap between r and a,b
    int16_t a0 = a[0];
    int16_t a1 = a[1];
    int16_t b0 = b[0];
    int16_t b1 = b[1];

    int16_t t  = fqmul(a1, b1);
    r[0]  += fqmul(t, zeta);
    r[0]  += fqmul(a0, b0);
    r[1]  += fqmul(a0, b1);
    r[1]  += fqmul(a1, b0);
}

static void basemul_plant(int32_t r[1], const int32_t a[1], const int32_t b[1], int32_t zeta) {
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
    r[0] = __pkhtb(t2, t1);
}

void basemul_montgomery(int16_t r[256], const int16_t a[256], const int16_t b[256], int add) {
    unsigned int i;
    if (!add) {
        for (i = 0; i < MLKEM_N / 4; i++) {
            basemul_plant((int32_t *) &r[4 * i],     (int32_t *) &a[4 * i],     (int32_t *) &b[4 * i], zetas_plant_basemul[i]);
            basemul_plant((int32_t *) &r[4 * i + 2], (int32_t *) &a[4 * i + 2], (int32_t *) &b[4 * i + 2], -zetas_plant_basemul[i]);

            // TODO: remove once iNTT can handle Plantard mul
            r[4 * i + 0] = montgomery_reduce((int32_t)r[4 * i + 0] * 1976);
            r[4 * i + 1] = montgomery_reduce((int32_t)r[4 * i + 1] * 1976);
            r[4 * i + 2] = montgomery_reduce((int32_t)r[4 * i + 2] * 1976);
            r[4 * i + 3] = montgomery_reduce((int32_t)r[4 * i + 3] * 1976);
        }
    } else {
        for (i = 0; i < MLKEM_N / 4; i++) {
            basemul_acc(&r[4 * i], &a[4 * i], &b[4 * i], zetas[64 + i]);
            basemul_acc(&r[4 * i + 2], &a[4 * i + 2], &b[4 * i + 2], -zetas[64 + i]);
        }
    }
}
