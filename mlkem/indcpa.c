// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "params.h"
#include "indcpa.h"
#include "polyvec.h"
#include "poly.h"
#include "ntt.h"
#include "symmetric.h"
#include "randombytes.h"
#include "matacc.h"

/*************************************************
 * Name:        pack_sk
 *
 * Description: Serialize the secret key
 *
 * Arguments:   - uint8_t *r: pointer to output serialized secret key
 *              - polyvec *sk: pointer to input vector of polynomials (secret key)
 **************************************************/
static void pack_sk(uint8_t r[MLKEM_INDCPA_SECRETKEYBYTES], polyvec *sk) {
    polyvec_tobytes(r, sk);
}

/*************************************************
 * Name:        unpack_sk
 *
 * Description: De-serialize the secret key; inverse of pack_sk
 *
 * Arguments:   - polyvec *sk: pointer to output vector of polynomials (secret key)
 *              - const uint8_t *packedsk: pointer to input serialized secret key
 **************************************************/
static void unpack_sk(polyvec *sk, const uint8_t packedsk[MLKEM_INDCPA_SECRETKEYBYTES]) {
    polyvec_frombytes(sk, packedsk);
}

/*************************************************
 * Name:        unpack_ciphertext
 *
 * Description: De-serialize and decompress ciphertext from a byte array;
 *              approximate inverse of pack_ciphertext
 *
 * Arguments:   - polyvec *b: pointer to the output vector of polynomials b
 *              - poly *v: pointer to the output polynomial v
 *              - const uint8_t *c: pointer to the input serialized ciphertext
 **************************************************/
static void unpack_ciphertext(polyvec *b, poly *v, const uint8_t c[MLKEM_INDCPA_BYTES]) {
    polyvec_decompress(b, c);
    poly_decompress(v, c + MLKEM_POLYVECCOMPRESSEDBYTES);
}

/*************************************************
 * Name:        rej_uniform
 *
 * Description: Run rejection sampling on uniform random bytes to generate
 *              uniform random integers mod q
 *
 * Arguments:   - int16_t *r: pointer to output buffer
 *              - unsigned int len: requested number of 16-bit integers (uniform mod q)
 *              - const uint8_t *buf: pointer to input buffer (assumed to be uniformly random bytes)
 *              - unsigned int buflen: length of input buffer in bytes
 *
 * Returns number of sampled 16-bit integers (at most len)
 **************************************************/
static unsigned int rej_uniform(int16_t *r,
                                unsigned int len,
                                const uint8_t *buf,
                                unsigned int buflen) {
    unsigned int ctr, pos;
    uint16_t val0, val1;

    ctr = pos = 0;
    while (ctr < len && pos + 3 <= buflen) {
        val0 = ((buf[pos + 0] >> 0) | ((uint16_t)buf[pos + 1] << 8)) & 0xFFF;
        val1 = ((buf[pos + 1] >> 4) | ((uint16_t)buf[pos + 2] << 4)) & 0xFFF;
        pos += 3;

        if (val0 < MLKEM_Q) {
            r[ctr++] = val0;
        }
        if (ctr < len && val1 < MLKEM_Q) {
            r[ctr++] = val1;
        }
    }

    return ctr;
}

#define gen_a(A, B) gen_matrix(A, B, 0)
#define gen_at(A, B) gen_matrix(A, B, 1)

/*************************************************
 * Name:        gen_matrix
 *
 * Description: Deterministically generate matrix A (or the transpose of A)
 *              from a seed. Entries of the matrix are polynomials that look
 *              uniformly random. Performs rejection sampling on output of
 *              a XOF
 *
 * Arguments:   - polyvec *a: pointer to ouptput matrix A
 *              - const uint8_t *seed: pointer to input seed
 *              - int transposed: boolean deciding whether A or A^T is generated
 **************************************************/
#define GEN_MATRIX_NBLOCKS ((12 * MLKEM_N / 8 * (1 << 12) / MLKEM_Q + XOF_BLOCKBYTES) / XOF_BLOCKBYTES)
// Not static for benchmarking
void gen_matrix(polyvec *a, const uint8_t seed[MLKEM_SYMBYTES], int transposed) {
    unsigned int ctr, i, j, k;
    unsigned int buflen, off;
    uint8_t buf[GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES + 2];
    xof_state state;

    for (i = 0; i < MLKEM_K; i++) {
        for (j = 0; j < MLKEM_K; j++) {
            if (transposed) {
                xof_absorb(&state, seed, i, j);
            } else {
                xof_absorb(&state, seed, j, i);
            }

            xof_squeezeblocks(buf, GEN_MATRIX_NBLOCKS, &state);
            buflen = GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES;
            ctr = rej_uniform(a[i].vec[j].coeffs, MLKEM_N, buf, buflen);

            while (ctr < MLKEM_N) {
                off = buflen % 3;
                for (k = 0; k < off; k++) {
                    buf[k] = buf[buflen - off + k];
                }
                xof_squeezeblocks(buf + off, 1, &state);
                buflen = off + XOF_BLOCKBYTES;
                ctr += rej_uniform(a[i].vec[j].coeffs + ctr, MLKEM_N - ctr, buf, buflen);
            }
        }
    }
}

/*************************************************
 * Name:        indcpa_keypair_derand
 *
 * Description: Generates public and private key for the CPA-secure
 *              public-key encryption scheme underlying Mlkem
 *
 * Arguments:   - uint8_t *pk: pointer to output public key
 *                             (of length MLKEM_INDCPA_PUBLICKEYBYTES bytes)
 *              - uint8_t *sk: pointer to output private key
 *                             (of length MLKEM_INDCPA_SECRETKEYBYTES bytes)
 *              - const uint8_t *coins: pointer to input randomness
 *                             (of length MLKEM_SYMBYTES bytes)
 **************************************************/
void indcpa_keypair_derand(uint8_t pk[MLKEM_INDCPA_PUBLICKEYBYTES],
                           uint8_t sk[MLKEM_INDCPA_SECRETKEYBYTES],
                           const uint8_t coins[MLKEM_SYMBYTES]) {
    unsigned int i;
    uint8_t buf[2 * MLKEM_SYMBYTES];
    const uint8_t *publicseed = buf;
    const uint8_t *noiseseed = buf + MLKEM_SYMBYTES;
    uint8_t nonce = 0;
    polyvec skpv;
    poly pkp;

    hash_g(buf, coins, MLKEM_SYMBYTES);

    for (i = 0; i < MLKEM_K; i++) {
        poly_getnoise_eta1(&skpv.vec[i], noiseseed, nonce++);
    }

    polyvec_ntt(&skpv);

    // matrix-vector multiplication
    for (i = 0; i < MLKEM_K; i++) {
        matacc(&pkp, &skpv, i, publicseed, 0);

        poly_invntt_tomont(&pkp);
        poly_addnoise_eta1(&pkp, noiseseed, nonce++);
        poly_ntt(&pkp);
        poly_reduce(&pkp);

        poly_tobytes(pk + i * MLKEM_POLYBYTES, &pkp);
    }

    pack_sk(sk, &skpv);
    memcpy(pk + MLKEM_POLYVECBYTES, publicseed, MLKEM_SYMBYTES);
}

/*************************************************
 * Name:        indcpa_enc
 *
 * Description: Encryption function of the CPA-secure
 *              public-key encryption scheme underlying Mlkem.
 *
 * Arguments:   - uint8_t *c: pointer to output ciphertext
 *                            (of length MLKEM_INDCPA_BYTES bytes)
 *              - const uint8_t *m: pointer to input message
 *                                  (of length MLKEM_INDCPA_MSGBYTES bytes)
 *              - const uint8_t *pk: pointer to input public key
 *                                   (of length MLKEM_INDCPA_PUBLICKEYBYTES)
 *              - const uint8_t *coins: pointer to input random coins used as seed
 *                                      (of length MLKEM_SYMBYTES) to deterministically
 *                                      generate all randomness
 **************************************************/
void indcpa_enc(uint8_t c[MLKEM_INDCPA_BYTES],
                const uint8_t m[MLKEM_INDCPA_MSGBYTES],
                const uint8_t pk[MLKEM_INDCPA_PUBLICKEYBYTES],
                const uint8_t coins[MLKEM_SYMBYTES]) {
    polyvec sp;
    poly b;
    poly *pkp = &b;
    poly *k = &b;
    poly *v = &sp.vec[0];
    const unsigned char *seed = pk + MLKEM_POLYVECBYTES;
    int i;
    unsigned char nonce = 0;

    for (i = 0; i < MLKEM_K; i++) {
        poly_getnoise_eta1(sp.vec + i, coins, nonce++);
    }

    polyvec_ntt(&sp);

    // matrix-vector multiplication
    for (i = 0; i < MLKEM_K; i++) {
        matacc(&b, &sp, i, seed, 1);
        poly_invntt_tomont(&b);

        poly_addnoise_eta2(&b, coins, nonce++);
        poly_reduce(&b);

        poly_packcompress(c, &b, i);
    }

    poly_frombytes(pkp, pk);
    poly_basemul(v, pkp, &sp.vec[0]);
    for (i = 1 ; i < MLKEM_K; i++) {
        poly_frombytes(pkp, pk + i * MLKEM_POLYBYTES);
        poly_basemul_acc(v, pkp, &sp.vec[i]);
    }
    poly_invntt_tomont(v);
    poly_addnoise_eta2(v, coins, nonce++);

    poly_frommsg(k, m);
    poly_add(v, v, k);
    poly_reduce(v);

    poly_compress(c + MLKEM_POLYVECCOMPRESSEDBYTES, v);
}

/*************************************************
 * Name:        indcpa_dec
 *
 * Description: Decryption function of the CPA-secure
 *              public-key encryption scheme underlying Mlkem.
 *
 * Arguments:   - uint8_t *m: pointer to output decrypted message
 *                            (of length MLKEM_INDCPA_MSGBYTES)
 *              - const uint8_t *c: pointer to input ciphertext
 *                                  (of length MLKEM_INDCPA_BYTES)
 *              - const uint8_t *sk: pointer to input secret key
 *                                   (of length MLKEM_INDCPA_SECRETKEYBYTES)
 **************************************************/
void indcpa_dec(uint8_t m[MLKEM_INDCPA_MSGBYTES],
                const uint8_t c[MLKEM_INDCPA_BYTES],
                const uint8_t sk[MLKEM_INDCPA_SECRETKEYBYTES]) {
    polyvec b, skpv;
    poly v, mp;

    unpack_ciphertext(&b, &v, c);
    unpack_sk(&skpv, sk);

    polyvec_ntt(&b);
    polyvec_basemul_acc_montgomery(&mp, &skpv, &b);
    poly_invntt_tomont(&mp);

    poly_sub(&mp, &v, &mp);
    poly_reduce(&mp);

    poly_tomsg(m, &mp);
}