// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#include <stdint.h>
#include "params.h"
#include "ntt.h"
#include "basemul.h"
#include <arm_acle.h>

static const int32_t twiddles_plantard_ntt[128] = {
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

const int32_t twiddles_plantard_basemul[64] = {
    21932846, 3562152210, 752167598, 3417653460, 2112004045, 932791035, 2951903026, 1419184148,
    1817845876, 3434425636, 4233039261, 300609006, 975366560, 2781600929, 3889854731, 3935010590,
    2197155094, 2130066389, 3598276897, 2308109491, 2382939200, 1228239371, 1884934581, 3466679822,
    1211467195, 2977706375, 3144137970, 3080919767, 945692709, 3015121229, 345764865, 826997308,
    2043625172, 2964804700, 2628071007, 4154339049, 483812778, 3288636719, 2696449880, 2122325384,
    1371447954, 411563403, 3577634219, 976656727, 2708061387, 723783916, 3181552825, 3346694253,
    3617629408, 1408862808, 519937465, 1323711759, 1474661346, 2773859924, 3580214553, 1143088323,
    2221668274, 1563682897, 2417773720, 1327582262, 2722253228, 3786641338, 1141798155, 2779020594
};

static const int32_t twiddles_plantard_invntt[256] = {
    2064267851, 51606697, 2064267851, 966335388, 1290168, 3200905336,
    51606697, 3482161830, 2064267851, 1847519727, 966335388, 886345009,
    1290168, 2064267851, 1290168, 51606697, 2064267851, 966335388,
    2435836064, 290287667, 2944162022, 3021572066, 1802363867, 603798347, 3375077936, 2677097369,
    2042335005, 3235739856, 1748176836, 3120914957, 282546663, 2711931889, 1103093133,
    1659155285, 1785591691, 1941701947, 2704190884, 358666539, 793452955, 1461759672, 1673347127,
    3200905336, 2042335005, 3560862042, 3235739856, 580575333, 1748176836, 1207596693,
    3887274396, 2126195886, 872153167, 3443456808, 526388302, 299318839, 3875662889, 3382818940,
    3266703874, 2575174144, 1404992306, 1824296713, 4252391772, 2591946320, 598637677,
    1997179146, 2904166832, 2577754479, 202556283, 30964018, 3807284017, 1238560711, 1967505295,
    51606697, 3200905336, 1847519727, 2042335005, 89021552, 3560862042, 700560902,
    1633351937, 2191994424, 909568022, 1780431021, 2022982494, 2497764099, 3609888404, 1126316146,
    89021552, 576704831, 3604727734, 1195985186, 594767175, 2315850495, 2439706566,
    3633111417, 2908037335, 3590535893, 357376372, 1887514916, 1410152976, 2486152593, 571544162,
    3482161830, 3266703874, 4045964987, 2575174144, 4222717922, 1404992306, 365117377,
    4003389463, 2444867236, 1221788534, 3305408896, 1626901100, 3367336931, 651534541, 1549491056,
    1819136044, 2390680205, 2567433139, 1643673276, 1322421592, 1357256112, 2750636911,
    993428903, 3680847611, 1082450454, 1205016358, 348345200, 956014049, 1048906102, 3880823559,
    2064267851, 51606697, 966335388, 3200905336, 3482161830, 1847519727, 886345009,
    3342823751, 4258842609, 568963827, 2849979801, 1283716570, 2330042337, 4104022520, 3007380225,
    3560862042, 580575333, 1207596693, 3458938817, 918599194, 2384229368, 879894172,
    2217797772, 503165289, 2812564947, 2946742357, 833448145, 1905577260, 3273154711, 3208646340,
    1847519727, 89021552, 700560902, 576704831, 1593356747, 3604727734, 2455188575,
    3162200314, 2808694444, 1933960943, 678628056, 49026362, 1375318456, 1961054458, 3473130659,
    4045964987, 4222717922, 365117377, 3479581496, 1744306334, 1052776604, 3456358482,
    438656919, 1681088131, 366407544, 2819015784, 1771399850, 1091481626, 2136517226, 709592074,
    966335388, 3482161830, 886345009, 3266703874, 1819136044, 4045964987, 2924809511,
    25803349, 3888564563, 1032133926, 923759864, 2630651342, 2590656153, 2146838565, 547030981,
    700560902, 1593356747, 2455188575, 3711811629, 2443577068, 3253802200, 1303069081,
    254162980, 3513125848, 1576584571, 3086080437, 2933840683, 3184133160, 1389510297, 2811274779,
    886345009, 1819136044, 2924809511, 2390680205, 1137927653, 2567433139, 3913077744,
    2288756980, 459299597, 1355965945, 1192114684, 2699030215, 439947086, 587026170, 418014240,
    2924809511, 1137927653, 3913077744, 2029433331, 3867921885, 98052723, 3922108916, 639923034,
    2806114109, 4122084864, 575414664, 1674637294, 1541750051, 2560982302, 1540459884, 0
};

static int32_t plantard(int32_t a, int32_t twiddle) {
    int32_t qa = 26632;
    int32_t q = 3329;

    int32_t t1 = __smlawb(twiddle, a, 0);
    int32_t t2 = __smlawt(twiddle, a, 0);
    t1 = __smlabb(t1, q, qa);
    t2 = __smlabb(t2, q, qa);
    return __pkhtb(t2, t1);
}

static int32_t plantard_reduce(int32_t a) {
    return plantard(a, 0x13afb8);
}

static void doublebutterfly(int32_t *r1, int32_t *r2, int32_t z) {
    int32_t t = plantard(*r2, z);
    *r2 = __usub16(*r1, t);
    *r1 = __uadd16(*r1, t);
}

static void doublebutterfly_light(int32_t *r1, int32_t *r2) {
    int32_t t = *r2;
    *r2 = __ssub16(*r1, t);
    *r1 = __sadd16(*r1, t);
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
    unsigned int j, i;
    for (j = 0; j < 16; j++) {
        int32_t *p0 = (int32_t *) (r + 2 * j + 0 * 32);
        int32_t *p1 = (int32_t *) (r + 2 * j + 1 * 32);
        int32_t *p2 = (int32_t *) (r + 2 * j + 2 * 32);
        int32_t *p3 = (int32_t *) (r + 2 * j + 3 * 32);
        int32_t *p4 = (int32_t *) (r + 2 * j + 4 * 32);
        int32_t *p5 = (int32_t *) (r + 2 * j + 5 * 32);
        int32_t *p6 = (int32_t *) (r + 2 * j + 6 * 32);
        int32_t *p7 = (int32_t *) (r + 2 * j + 7 * 32);

        // Layer 1
        doublebutterfly(p0, p4, twiddles_plantard_ntt[0]);
        doublebutterfly(p1, p5, twiddles_plantard_ntt[0]);
        doublebutterfly(p2, p6, twiddles_plantard_ntt[0]);
        doublebutterfly(p3, p7, twiddles_plantard_ntt[0]);

        // Layer 2
        doublebutterfly(p0, p2, twiddles_plantard_ntt[1]);
        doublebutterfly(p1, p3, twiddles_plantard_ntt[1]);
        doublebutterfly(p4, p6, twiddles_plantard_ntt[2]);
        doublebutterfly(p5, p7, twiddles_plantard_ntt[2]);

        // Layer 3
        doublebutterfly(p0, p1, twiddles_plantard_ntt[3]);
        doublebutterfly(p2, p3, twiddles_plantard_ntt[4]);
        doublebutterfly(p4, p5, twiddles_plantard_ntt[5]);
        doublebutterfly(p6, p7, twiddles_plantard_ntt[6]);
    }

    const int32_t *twiddle_ptr = twiddles_plantard_ntt + 7;
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 2; i++) {
            int32_t *p0 = (int32_t *) (r + 2 * i + 32 * j + 0 * 4);
            int32_t *p1 = (int32_t *) (r + 2 * i + 32 * j + 1 * 4);
            int32_t *p2 = (int32_t *) (r + 2 * i + 32 * j + 2 * 4);
            int32_t *p3 = (int32_t *) (r + 2 * i + 32 * j + 3 * 4);
            int32_t *p4 = (int32_t *) (r + 2 * i + 32 * j + 4 * 4);
            int32_t *p5 = (int32_t *) (r + 2 * i + 32 * j + 5 * 4);
            int32_t *p6 = (int32_t *) (r + 2 * i + 32 * j + 6 * 4);
            int32_t *p7 = (int32_t *) (r + 2 * i + 32 * j + 7 * 4);

            // Layer 4
            doublebutterfly(p0, p4, twiddle_ptr[0]);
            doublebutterfly(p1, p5, twiddle_ptr[0]);
            doublebutterfly(p2, p6, twiddle_ptr[0]);
            doublebutterfly(p3, p7, twiddle_ptr[0]);

            // Layer 5
            doublebutterfly(p0, p2, twiddle_ptr[1]);
            doublebutterfly(p1, p3, twiddle_ptr[1]);
            doublebutterfly(p4, p6, twiddle_ptr[2]);
            doublebutterfly(p5, p7, twiddle_ptr[2]);

            // Layer 6
            doublebutterfly(p0, p1, twiddle_ptr[3]);
            doublebutterfly(p2, p3, twiddle_ptr[4]);
            doublebutterfly(p4, p5, twiddle_ptr[5]);
            doublebutterfly(p6, p7, twiddle_ptr[6]);
        }
        twiddle_ptr += 7;
    }

    // Layer 7
    for (j = 0; j < 64; j++) {
        int32_t *p0 = (int32_t *)(r + 4 * j + 0 * 2);
        int32_t *p1 = (int32_t *)(r + 4 * j + 1 * 2);
        doublebutterfly(p0, p1, twiddle_ptr[j]);
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
    for (int i = 0; i < 16; i++) {
        int32_t *p0 = (int32_t *)(r + i * 16 + 2 * 0);
        int32_t *p1 = (int32_t *)(r + i * 16 + 2 * 1);
        int32_t *p2 = (int32_t *)(r + i * 16 + 2 * 2);
        int32_t *p3 = (int32_t *)(r + i * 16 + 2 * 3);
        int32_t *p4 = (int32_t *)(r + i * 16 + 2 * 4);
        int32_t *p5 = (int32_t *)(r + i * 16 + 2 * 5);
        int32_t *p6 = (int32_t *)(r + i * 16 + 2 * 6);
        int32_t *p7 = (int32_t *)(r + i * 16 + 2 * 7);

        // Layer 7
        doublebutterfly_light(p0, p1);
        doublebutterfly_light(p2, p3);
        doublebutterfly_light(p4, p5);
        doublebutterfly_light(p6, p7);

        // Layer 6
        doublebutterfly_light(p0, p2);
        doublebutterfly(p1, p3, twiddles_plantard_invntt[0]);
        doublebutterfly_light(p4, p6);
        *p6 = plantard_reduce(*p6);
        doublebutterfly(p5, p7, twiddles_plantard_invntt[0]);

        // Layer 5
        doublebutterfly_light(p0, p4);
        doublebutterfly(p1, p5, twiddles_plantard_invntt[1]);
        doublebutterfly(p2, p6, twiddles_plantard_invntt[2]);
        doublebutterfly(p3, p7, twiddles_plantard_invntt[3]);
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int32_t *p0 = (int32_t *)(r + i * 32 + j * 2 + 16 * 0 );
            int32_t *p1  = (int32_t *)(r + i * 32 + j * 2 + 16 * 1 );

            // Layer 4
            doublebutterfly(p0, p1, twiddles_plantard_invntt[4 + j]);
        }
    }

    const int32_t *twiddle = twiddles_plantard_invntt + 12;
    for (int i = 0; i < 16; i++) {
        int32_t *p0 = (int32_t *)(r + i * 2 + 32 * 0);
        int32_t *p1 = (int32_t *)(r + i * 2 + 32 * 1);
        int32_t *p2 = (int32_t *)(r + i * 2 + 32 * 2);
        int32_t *p3 = (int32_t *)(r + i * 2 + 32 * 3);
        int32_t *p4 = (int32_t *)(r + i * 2 + 32 * 4);
        int32_t *p5 = (int32_t *)(r + i * 2 + 32 * 5);
        int32_t *p6 = (int32_t *)(r + i * 2 + 32 * 6);
        int32_t *p7 = (int32_t *)(r + i * 2 + 32 * 7);

        if (i == 0) {
            *p0 = plantard_reduce(*p0);
            *p1 = plantard_reduce(*p1);
            *p2 = plantard_reduce(*p2);
            *p3 = plantard_reduce(*p3);
            *p4 = plantard_reduce(*p4);
            *p5 = plantard_reduce(*p5);
            *p6 = plantard_reduce(*p6);
            *p7 = plantard_reduce(*p7);

            // Layer 3
            doublebutterfly_light(p0, p1);
            doublebutterfly_light(p2, p3);
            doublebutterfly_light(p4, p5);
            doublebutterfly_light(p6, p7);

            // Layer 2
            doublebutterfly_light(p0, p2);
            doublebutterfly(p1, p3, twiddle[1]);
            doublebutterfly_light(p4, p6);
            *p6 = plantard_reduce(*p6);
            doublebutterfly(p5, p7, twiddle[1]);

            // Layer 1
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

void basemul_plantard(int16_t r[256], const int16_t a[256], const int16_t b[256], int add) {
    unsigned int i;
    for (i = 0; i < MLKEM_N / 4; i++) {
        basemul((int32_t *) &r[4 * i],     (int32_t *) &a[4 * i],     (int32_t *) &b[4 * i], twiddles_plantard_basemul[i], add);
        basemul((int32_t *) &r[4 * i + 2], (int32_t *) &a[4 * i + 2], (int32_t *) &b[4 * i + 2], -twiddles_plantard_basemul[i], add);
    }
}

void frombytes_basemul_plantard(int16_t r[256], const int16_t b[256], const unsigned char *a, int add) {
    unsigned int i;
    int16_t ap[4];
    for (i = 0; i < MLKEM_N / 4; i++) {
        ap[0] = ((a[6 * i + 0] >> 0) | ((uint16_t)a[6 * i + 1] << 8)) & 0xFFF;
        ap[1] = ((a[6 * i + 1] >> 4) | ((uint16_t)a[6 * i + 2] << 4)) & 0xFFF;
        ap[2] = ((a[6 * i + 3] >> 0) | ((uint16_t)a[6 * i + 4] << 8)) & 0xFFF;
        ap[3] = ((a[6 * i + 4] >> 4) | ((uint16_t)a[6 * i + 5] << 4)) & 0xFFF;
        basemul((int32_t *) &r[4 * i],     (int32_t *) ap,     (int32_t *) &b[4 * i], twiddles_plantard_basemul[i], add);
        basemul((int32_t *) &r[4 * i + 2], (int32_t *) (ap + 2), (int32_t *) &b[4 * i + 2], -twiddles_plantard_basemul[i], add);
    }
}
