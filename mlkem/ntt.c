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

const int32_t zetas_inv_CT_asm[256] = {
    // LAYER 7+6+5+4
    1290168, 1290168, 2064267851, 1290168, 51606697, 2064267851, 966335388, 1290168, 3200905336, 51606697, 3482161830, 2064267851, 1847519727, 966335388, 886345009,
    // removed first "2285" + LAYER 3+2+1 - 1 - butterfly
    1290168, 2064267851, 1290168, 51606697, 2064267851, 966335388,
    // LAYER 3+2+1 - 1 - twist
    2435836064, 290287667, 2944162022, 3021572066, 1802363867, 603798347, 3375077936, 2677097369,
    // LAYER 3+2+1 - 2 - butterfly
    2042335005, 3235739856, 1748176836, 3120914957, 282546663, 2711931889, 1103093133,
    // LAYER 3+2+1 - 2 - twist
    1659155285, 1785591691, 1941701947, 2704190884, 358666539, 793452955, 1461759672, 1673347127,
    // LAYER 3+2+1 - 3 - butterfly
    3200905336, 2042335005, 3560862042, 3235739856, 580575333, 1748176836, 1207596693,
    // LAYER 3+2+1 - 3 - twist
    3887274396, 2126195886, 872153167, 3443456808, 526388302, 299318839, 3875662889, 3382818940,
    // LAYER 3+2+1 - 4 - butterfly
    3266703874, 2575174144, 1404992306, 1824296713, 4252391772, 2591946320, 598637677,
    // LAYER 3+2+1 - 4 - twist
    1997179146, 2904166832, 2577754479, 202556283, 30964018, 3807284017, 1238560711, 1967505295,
    // LAYER 3+2+1 - 5 - butterfly
    51606697, 3200905336, 1847519727, 2042335005, 89021552, 3560862042, 700560902,
    // LAYER 3+2+1 - 5 - twist
    1633351937, 2191994424, 909568022, 1780431021, 2022982494, 2497764099, 3609888404, 1126316146,
    // LAYER 3+2+1 - 6 - butterfly
    89021552, 576704831, 3604727734, 1195985186, 594767175, 2315850495, 2439706566,
    // LAYER 3+2+1 - 6 - twist
    3633111417, 2908037335, 3590535893, 357376372, 1887514916, 1410152976, 2486152593, 571544162,
    // LAYER 3+2+1 - 7 - butterfly
    3482161830, 3266703874, 4045964987, 2575174144, 4222717922, 1404992306, 365117377,
    // LAYER 3+2+1 - 7 - twist
    4003389463, 2444867236, 1221788534, 3305408896, 1626901100, 3367336931, 651534541, 1549491056,
    // LAYER 3+2+1 - 8 - butterfly
    1819136044, 2390680205, 2567433139, 1643673276, 1322421592, 1357256112, 2750636911,
    // LAYER 3+2+1 - 8 - twist
    993428903, 3680847611, 1082450454, 1205016358, 348345200, 956014049, 1048906102, 3880823559,
    // LAYER 3+2+1 - 9 - butterfly
    2064267851, 51606697, 966335388, 3200905336, 3482161830, 1847519727, 886345009,
    // LAYER 3+2+1 - 9 - twist
    3342823751, 4258842609, 568963827, 2849979801, 1283716570, 2330042337, 4104022520, 3007380225,
    // LAYER 3+2+1 - 10 - butterfly
    3560862042, 580575333, 1207596693, 3458938817, 918599194, 2384229368, 879894172,
    // LAYER 3+2+1 - 10 - twist
    2217797772, 503165289, 2812564947, 2946742357, 833448145, 1905577260, 3273154711, 3208646340,
    // LAYER 3+2+1 - 11 - butterfly
    1847519727, 89021552, 700560902, 576704831, 1593356747, 3604727734, 2455188575,
    // LAYER 3+2+1 - 11 - twist
    3162200314, 2808694444, 1933960943, 678628056, 49026362, 1375318456, 1961054458, 3473130659,
    // LAYER 3+2+1 - 12 - butterfly
    4045964987, 4222717922, 365117377, 3479581496, 1744306334, 1052776604, 3456358482,
    // LAYER 3+2+1 - 12 - twist
    438656919, 1681088131, 366407544, 2819015784, 1771399850, 1091481626, 2136517226, 709592074,
    // LAYER 3+2+1 - 13 - butterfly
    966335388, 3482161830, 886345009, 3266703874, 1819136044, 4045964987, 2924809511,
    // LAYER 3+2+1 - 13 - twist
    25803349, 3888564563, 1032133926, 923759864, 2630651342, 2590656153, 2146838565, 547030981,
    // LAYER 3+2+1 - 14 - butterfly
    700560902, 1593356747, 2455188575, 3711811629, 2443577068, 3253802200, 1303069081,
    // LAYER 3+2+1 - 14 - twist
    254162980, 3513125848, 1576584571, 3086080437, 2933840683, 3184133160, 1389510297, 2811274779,
    // LAYER 3+2+1 - 15 - butterfly
    886345009, 1819136044, 2924809511, 2390680205, 1137927653, 2567433139, 3913077744,
    // LAYER 3+2+1 - 15 - twist
    2288756980, 459299597, 1355965945, 1192114684, 2699030215, 439947086, 587026170, 418014240,
    // LAYER 3+2+1 - 16 - butterfly
    2924809511, 1137927653, 3913077744, 2029433331, 3867921885, 98052723, 3922108916, 639923034,
    // LAYER 3+2+1 - 16 - twist
    2806114109, 4122084864, 575414664, 1674637294, 1541750051, 2560982302, 1540459884, 0
};

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

static void doublebutterfly_light(int16_t *r1_16, int16_t *r2_16) {
    int32_t *r1 = (int32_t *) r1_16;
    int32_t *r2 = (int32_t *) r2_16;

    int32_t t = *r2;
    *r2 = __ssub16(*r1, t);
    *r1 = __sadd16(*r1, t);
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

    int32_t *r32 = r;
    int32_t t0, t1;

    // TODO: try different layer merge
    // TODO: why skip the twiddle 0,1,3
    // Layer 7+6+5+4
    for (int i = 0; i < 8; i++) {
        int32_t *p0 = r + i * 32 + 2 * 0;
        int32_t *p1 = r + i * 32 + 2 * 1;
        int32_t *p2 = r + i * 32 + 2 * 2;
        int32_t *p3 = r + i * 32 + 2 * 3;
        int32_t *p4 = r + i * 32 + 2 * 4;
        int32_t *p5 = r + i * 32 + 2 * 5;
        int32_t *p6 = r + i * 32 + 2 * 6;
        int32_t *p7 = r + i * 32 + 2 * 7;

        int32_t *p8  = r + i * 32 + 2 * 8;
        int32_t *p9  = r + i * 32 + 2 * 9;
        int32_t *p10 = r + i * 32 + 2 * 10;
        int32_t *p11 = r + i * 32 + 2 * 11;
        int32_t *p12 = r + i * 32 + 2 * 12;
        int32_t *p13 = r + i * 32 + 2 * 13;
        int32_t *p14 = r + i * 32 + 2 * 14;
        int32_t *p15 = r + i * 32 + 2 * 15;

        // Layer 7
        doublebutterfly_light(p0, p1);
        doublebutterfly_light(p2, p3);
        doublebutterfly_light(p4, p5);
        doublebutterfly_light(p6, p7);
        doublebutterfly_light(p8, p9);
        doublebutterfly_light(p10, p11);
        doublebutterfly_light(p12, p13);
        doublebutterfly_light(p14, p15);

        // Layer 6
        doublebutterfly_light(p0, p2);
        doublebutterfly(p1, p3, zetas_inv_CT_asm[2]);
        doublebutterfly_light(p4, p6);
        *p6 = plantard(*p6, 0x13afb8);
        doublebutterfly(p5, p7, zetas_inv_CT_asm[2]);
        doublebutterfly_light(p8, p10);
        doublebutterfly(p9, p11, zetas_inv_CT_asm[2]);
        doublebutterfly_light(p12, p14);
        *p14 = plantard(*p14, 0x13afb8);
        doublebutterfly(p13, p15, zetas_inv_CT_asm[2]);

        // Layer 5
        doublebutterfly_light(p0, p4);
        doublebutterfly(p1, p5, zetas_inv_CT_asm[4]);
        doublebutterfly(p2, p6, zetas_inv_CT_asm[5]);
        doublebutterfly(p3, p7, zetas_inv_CT_asm[6]);
        doublebutterfly_light(p8, p12);
        doublebutterfly(p9, p13, zetas_inv_CT_asm[4]);
        doublebutterfly(p10, p14, zetas_inv_CT_asm[5]);
        doublebutterfly(p11, p15, zetas_inv_CT_asm[6]);

        // Layer 4
        doublebutterfly(p0, p8, zetas_inv_CT_asm[7]);
        doublebutterfly(p1, p9, zetas_inv_CT_asm[8]);
        doublebutterfly(p2, p10, zetas_inv_CT_asm[9]);
        doublebutterfly(p3, p11, zetas_inv_CT_asm[10]);
        doublebutterfly(p4, p12, zetas_inv_CT_asm[11]);
        doublebutterfly(p5, p13, zetas_inv_CT_asm[12]);
        doublebutterfly(p6, p14, zetas_inv_CT_asm[13]);
        doublebutterfly(p7, p15, zetas_inv_CT_asm[14]);
    }

    // TODO: why skip the 15?
    int32_t *twiddle = zetas_inv_CT_asm + 15;
    for (int i = 0; i < 16; i++) {
        int32_t *p0 = r + i * 2 + 32 * 0;
        int32_t *p1 = r + i * 2 + 32 * 1;
        int32_t *p2 = r + i * 2 + 32 * 2;
        int32_t *p3 = r + i * 2 + 32 * 3;

        int32_t *p4 = r + i * 2 + 32 * 4;
        int32_t *p5 = r + i * 2 + 32 * 5;
        int32_t *p6 = r + i * 2 + 32 * 6;
        int32_t *p7 = r + i * 2 + 32 * 7;

        if (i == 0) {
            // TODO: move this to a separate plantard_reduce function
            *p0 = plantard(*p0, 0x13afb8);
            *p1 = plantard(*p1, 0x13afb8);
            *p2 = plantard(*p2, 0x13afb8);
            *p3 = plantard(*p3, 0x13afb8);
            *p4 = plantard(*p4, 0x13afb8);
            *p5 = plantard(*p5, 0x13afb8);
            *p6 = plantard(*p6, 0x13afb8);
            *p7 = plantard(*p7, 0x13afb8);

            // Layer 3
            doublebutterfly_light(p0, p1);
            doublebutterfly_light(p2, p3);
            doublebutterfly_light(p4, p5);
            doublebutterfly_light(p6, p7);

            // Layer 2
            doublebutterfly_light(p0, p2);
            doublebutterfly(p1, p3, twiddle[1]);
            doublebutterfly_light(p4, p6);
            *p6 = plantard(*p6, 0x13afb8);
            doublebutterfly(p5, p7, twiddle[1]);

            doublebutterfly_light(p0, p4);
            doublebutterfly(p1, p5, twiddle[3]);
            doublebutterfly(p2, p6, twiddle[4]);
            doublebutterfly(p3, p7, twiddle[5]);

            twiddle += 6;
        } else {
            // Layer 3
            doublebutterfly(p0, p1, twiddle[0]);
            doublebutterfly(p2, p3, twiddle[0]);
            doublebutterfly(p4, p5, twiddle[0]);
            doublebutterfly(p6, p7, twiddle[0]);

            // Layer 2
            doublebutterfly(p0, p2, twiddle[1]);
            doublebutterfly(p1, p3, twiddle[2]);
            doublebutterfly(p4, p6, twiddle[1]);
            doublebutterfly(p5, p7, twiddle[2]);

            // Layer 1
            doublebutterfly(p0, p4, twiddle[3]);
            doublebutterfly(p1, p5, twiddle[4]);
            doublebutterfly(p2, p6, twiddle[5]);
            doublebutterfly(p3, p7, twiddle[6]);

            twiddle += 7;
        }

        // Twist
        *p0 = plantard(*p0, twiddle[0]);
        *p1 = plantard(*p1, twiddle[1]);
        *p2 = plantard(*p2, twiddle[2]);
        *p3 = plantard(*p3, twiddle[3]);
        *p4 = plantard(*p4, twiddle[4]);
        *p5 = plantard(*p5, twiddle[5]);
        *p6 = plantard(*p6, twiddle[6]);
        *p7 = plantard(*p7, twiddle[7]);
        twiddle += 8;
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
