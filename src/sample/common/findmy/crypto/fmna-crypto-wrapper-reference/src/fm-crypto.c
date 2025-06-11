/*
 * need a license header here
 */

/*
 * WolfSSL shim, providing the core cryptographic functionality
 * required to implement the FindMy protocol.
 */

#include <assert.h>
#include <string.h>
#include <trace.h>
#include "fm-crypto.h"

#include "user_settings.h"
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/hmac.h>

const byte KDF_LABEL_UPDATE[] = "update";
const byte KDF_LABEL_DIVERSIFY[] = "diversify";
const byte KDF_LABEL_INTERMEDIATE[] = "intermediate";
const byte KDF_LABEL_CONNECT[] = "connect";
const byte KDF_LABEL_SERVERSS[] = "ServerSharedSecret";
const byte KDF_LABEL_PAIRINGSESS[] = "PairingSession";
const byte KDF_LABEL_SNPROTECTION[] = "SerialNumberProtection";

#define CHECK_RV_RET(_rv_, _val_) if (_rv_) return _val_;
#define CHECK_RV(_rv_) CHECK_RV_RET(_rv_, _rv_);
#define CHECK_RV_GOTO(_rv_, _label_) if (_rv_) goto _label_;

/*
 * OpenSSL: SHA256()
 */
int fm_crypto_sha256(word32 msg_nbytes, const byte *msg, byte out[32])
{
    wc_Sha256 sha256;

    int rv = wc_InitSha256(&sha256);
    CHECK_RV(rv);

    rv = wc_Sha256Update(&sha256, msg, msg_nbytes);
    CHECK_RV_GOTO(rv, cleanup);

    // out := SHA256(msg).
    rv = wc_Sha256Final(&sha256, out);

cleanup:
    wc_Sha256Free(&sha256);
    return rv;
}

int fm_crypto_ckg_init(fm_crypto_ckg_context_t ctx)
{
    XMEMSET(ctx, 0, sizeof(struct fm_crypto_ckg_context));

    WC_RNG rng;
    int rv = wc_InitRng(&rng);
    CHECK_RV(rv);

    /*
     * OpenSSL: EC_GROUP_new_by_curve_name(NID_secp224r1) + EC_POINT_new()
     */
    ctx->p = wc_ecc_new_point();
    if (ctx->p == NULL)
    {
        rv = -1;
        goto cleanup;
    }

    /*
     * OpenSSL: RAND_bytes()
     */
    rv = wc_RNG_GenerateBlock(&rng, ctx->r1, sizeof(ctx->r1));
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_KEY_new_by_curve_name(NID_secp224r1)
     */
    rv = wc_ecc_init(&ctx->key);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_KEY_generate_key()
     */
    rv = wc_ecc_make_key_ex(&rng, 28, &ctx->key, ECC_SECP224R1);
    if (rv)
    {
        wc_ecc_free(&ctx->key);
    }

cleanup:
    if (rv)
    {
        if (ctx->p)
        {
            wc_ecc_del_point(ctx->p);
        }

        XMEMSET(ctx, 0, sizeof(struct fm_crypto_ckg_context));
    }

    wc_FreeRng(&rng);
    return rv;
}

void fm_crypto_ckg_free(fm_crypto_ckg_context_t ctx)
{
    wc_ecc_free(&ctx->key);
    wc_ecc_del_point(ctx->p);

    XMEMSET(ctx, 0, sizeof(struct fm_crypto_ckg_context));
}

int fm_crypto_ckg_gen_c1(fm_crypto_ckg_context_t ctx, byte out[32])
{
    // Construct s || r.
    byte msg[28 + sizeof(ctx->r1)];
    XMEMCPY(msg + 28, ctx->r1, sizeof(ctx->r1));

    /*
     * OpenSSL: BN_bn2bin() + EC_KEY_get0_private_key()
     */
    int rv = mp_to_unsigned_bin_len(&ctx->key.k, msg, 28);
    CHECK_RV(rv);

    /*
     * OpenSSL: SHA256()
     */
    return fm_crypto_sha256(sizeof(msg), msg, out);
}

/*! @function _fm_crypto_points_add
 @abstract Adds two given points on an elliptic curve.

 @param r  Resulting EC point r = s + t.
 @param s  EC point s.
 @param t  EC point t.
 @param dp Curve parameters.

 @return 0 on success, a negative value on error.
 */
static int _fm_crypto_points_add(ecc_point *r,
                                 ecc_point *s,
                                 ecc_point *t,
                                 const ecc_set_type *dp)
{
    mp_int prime, a, mu;
    mp_digit mp;

    // Read the curve's prime field's modulus.
    int rv = mp_init(&prime);
    CHECK_RV(rv);

    rv = mp_init(&mu);
    if (rv)
    {
        mp_clear(&prime);
        return -1;
    }

    rv = mp_read_radix(&prime, dp->prime, MP_RADIX_HEX);
    if (rv)
    {
        mp_clear(&prime);
        mp_clear(&mu);
        return -1;
    }

    // Read the curve's a parameter.
    rv = mp_init(&a);
    if (rv)
    {
        mp_clear(&prime);
        mp_clear(&mu);
        return -1;
    }

    rv = mp_read_radix(&a, dp->Af, MP_RADIX_HEX);
    CHECK_RV_GOTO(rv, cleanup);

    // Bring s and t into Montgomery space.
    rv = mp_montgomery_calc_normalization(&mu, &prime);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_mulmod(s->x, &mu, &prime, s->x);
    CHECK_RV_GOTO(rv, cleanup);
    rv = mp_mulmod(s->y, &mu, &prime, s->y);
    CHECK_RV_GOTO(rv, cleanup);
    rv = mp_mulmod(s->z, &mu, &prime, s->z);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_mulmod(t->x, &mu, &prime, t->x);
    CHECK_RV_GOTO(rv, cleanup);
    rv = mp_mulmod(t->y, &mu, &prime, t->y);
    CHECK_RV_GOTO(rv, cleanup);
    rv = mp_mulmod(t->z, &mu, &prime, t->z);
    CHECK_RV_GOTO(rv, cleanup);

    // Setup Montgomery multiplication.
    rv = mp_montgomery_setup(&prime, &mp);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * Compute s + t.
     *
     * OpenSSL: EC_POINT_add()
     */
    rv = ecc_projective_add_point(s, t, r, &a, &prime, mp);
    CHECK_RV_GOTO(rv, cleanup);

    rv = ecc_map_ex(r, &prime, mp, 1);
    CHECK_RV_GOTO(rv, cleanup);

cleanup:
    mp_clear(&prime);
    mp_clear(&mu);
    mp_clear(&a);
    return rv;
}

int fm_crypto_ckg_gen_c3(fm_crypto_ckg_context_t ctx,
                         const byte c2[89],
                         byte out[60])
{
    ecc_key S;

    /*
     * OpenSSL: EC_KEY_new_by_curve_name(NID_secp224r1)
     */
    int rv = wc_ecc_init(&S);
    CHECK_RV(rv);

    /*
     * Import and check S'.
     *
     * OpenSSL: EC_POINT_set_affine_coordinates_GFp()
     */
    rv = wc_ecc_import_x963_ex(c2, 1 + 28 * 2, &S, ECC_SECP224R1);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_POINT_is_on_curve()
     */
    rv = wc_ecc_check_key(&S);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * Compute S + S'.
     *
     * OpenSSL: EC_POINT_add()
     */
    rv = _fm_crypto_points_add(ctx->p, &ctx->key.pubkey, &S.pubkey, S.dp);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * C3 := s || r
     *
     * OpenSSL: BN_bn2bin() + EC_KEY_get0_private_key()
     */
    rv = mp_to_unsigned_bin_len(&ctx->key.k, out, 28);
    CHECK_RV_GOTO(rv, cleanup);

    XMEMCPY(out + 28, ctx->r1, sizeof(ctx->r1));

    // Store r'.
    XMEMCPY(ctx->r2, c2 + 1 + 28 * 2, sizeof(ctx->r2));

cleanup:
    wc_ecc_free(&S);
    return rv;
}

int fm_crypto_ckg_finish(fm_crypto_ckg_context_t ctx,
                         byte p[57],
                         byte skn[32],
                         byte sks[32])
{
    byte x[28];

    /*
     * OpenSSL: BN_bn2bin() + EC_POINT_get_affine_coordinates_GFp()
     */
    mp_to_unsigned_bin_len(ctx->p->x, x, 28);

    byte r1r2[32 * 2];
    XMEMCPY(r1r2, ctx->r1, 32);
    XMEMCPY(r1r2 + 32, ctx->r2, 32);

    /*
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    byte sk[32 * 2];
    int rv = wc_X963_KDF(WC_HASH_TYPE_SHA256,
                         x, sizeof(x),       /* Z */
                         r1r2, sizeof(r1r2), /* sharedinfo */
                         sk, sizeof(sk));    /* derived key */
    CHECK_RV(rv);

    XMEMCPY(skn, sk, 32);
    XMEMCPY(sks, sk + 32, 32);

    /*
     * Write uncompressed point in X9.63 format.
     *
     * OpenSSL: BN_bn2bin() + EC_POINT_get_affine_coordinates_GFp()
     */
    p[0] = 0x04;
    rv = mp_to_unsigned_bin_len(ctx->p->x, p + 1, 28);
    CHECK_RV(rv);

    return mp_to_unsigned_bin_len(ctx->p->y, p + 1 + 28, 28);
}

int fm_crypto_roll_sk(const byte sk[32], byte out[32])
{
    /*
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    return wc_X963_KDF(WC_HASH_TYPE_SHA256,
                       sk, 32,                                         /* Z */
                       KDF_LABEL_UPDATE, sizeof(KDF_LABEL_UPDATE) - 1, /* sharedinfo */
                       out, 32);                                       /* derived key */
}

int fm_crypto_derive_ltk(const byte skn[32], byte out[16])
{
    /*
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    byte ik[32];
    int rv = wc_X963_KDF(WC_HASH_TYPE_SHA256,
                         skn, 32,                                                    /* Z */
                         KDF_LABEL_INTERMEDIATE, sizeof(KDF_LABEL_INTERMEDIATE) - 1, /* sharedinfo */
                         ik, sizeof(ik));                                            /* derived key */
    CHECK_RV(rv);

    /*
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    return wc_X963_KDF(WC_HASH_TYPE_SHA256,
                       ik, sizeof(ik),                                   /* Z */
                       KDF_LABEL_CONNECT, sizeof(KDF_LABEL_CONNECT) - 1, /* sharedinfo */
                       out, 16);                                         /* derived key */
}

/*! @function _fm_crypto_scmult
 @abstract Scalar multiplication on an elliptic curve.

 @param r  Resulting EC point r = s * B.
 @param s  Scalar s.
 @param B  Base point B.
 @param dp Curve parameters.

 @return 0 on success, a negative value on error.
 */
static int _fm_crypto_scmult(ecc_point *r,
                             mp_int *s,
                             ecc_point *B,
                             const ecc_set_type *dp)
{
    mp_int a, p;
    mp_digit mp;

    int rv = mp_init(&a);
    CHECK_RV(rv);

    rv = mp_init(&p);
    if (rv)
    {
        mp_clear(&a);
        return rv;
    }

    rv = mp_read_radix(&p, dp->prime, MP_RADIX_HEX);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_read_radix(&a, dp->Af, MP_RADIX_HEX);
    CHECK_RV_GOTO(rv, cleanup);

    rv = wc_ecc_mulmod(s, B, r, &a, &p, 0);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_montgomery_setup(&p, &mp);
    CHECK_RV_GOTO(rv, cleanup);

    rv = ecc_map_ex(r, &p, mp, 1);

cleanup:
    mp_clear(&a);
    mp_clear(&p);

    return rv;
}

/*! @function _fm_crypto_scmult_reduce
 @abstract Takes a 36-byte value uv, reduces it to a valid scalar s,
           and computes r = s * B.

 @param r  Resulting EC point r = (uv (mod q-1) + 1) * B.
 @param uv 36-byte pre-scalar value.
 @param B  Base point B.
 @param dp Curve parameters.

 @return 0 on success, a negative value on error.
 */
static int _fm_crypto_scmult_reduce(ecc_point *r,
                                    const byte uv[36],
                                    ecc_point *B,
                                    const ecc_set_type *dp)
{
    mp_int s, qm1;

    int rv = mp_init(&s);
    CHECK_RV(rv);

    rv = mp_init(&qm1);
    if (rv)
    {
        mp_clear(&s);
        return rv;
    }

    /*
     * OpenSSL: BN_bin2bn()
     */
    rv = mp_read_unsigned_bin(&s, uv, 28 + 8);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: BN_dup() + EC_GROUP_get0_order()
     */
    rv = mp_read_radix(&qm1, dp->order, MP_RADIX_HEX);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: BN_sub_word()
     */
    rv = mp_sub_d(&qm1, 1, &qm1);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: BN_nnmod()
     */
    rv = mp_mod(&s, &qm1, &s);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * s := u (mod q-1) + 1
     *
     * OpenSSL: BN_add_word()
     */
    rv = mp_add_d(&s, 1, &s);
    CHECK_RV_GOTO(rv, cleanup);

    rv = _fm_crypto_scmult(r, &s, B, dp);

cleanup:
    mp_clear(&s);
    mp_clear(&qm1);

    return rv;
}

/*! @function _fm_crypto_scmult_twin_reduce
 @abstract Takes two 36-byte values u and v, reduces them to valid scalars s
           and t, a and computes r = s * P + t * G.

 @param r  Resulting EC point r = (u (mod q-1) + 1) * P + (v (mod q-1) + 1) * G.
 @param u  36-byte pre-scalar value.
 @param v  36-byte pre-scalar value.
 @param P  EC point P.

 @return 0 on success, a negative value on error.
 */
static int _fm_crypto_scmult_twin_reduce(ecc_point *r,
                                         const byte u[36],
                                         const byte v[36],
                                         ecc_key *P)
{
    /*
     * OpenSSL: EC_GROUP_new_by_curve_name(NID_secp224r1) + EC_POINT_new()
     */
    ecc_point *r1 = wc_ecc_new_point();
    if (r1 == NULL)
    {
        return -1;
    }

    /*
     * OpenSSL: EC_GROUP_new_by_curve_name(NID_secp224r1) + EC_POINT_new()
     */
    ecc_point *r2 = wc_ecc_new_point();
    if (r2 == NULL)
    {
        wc_ecc_del_point(r1);
        return -1;
    }

    /*
     * OpenSSL: EC_GROUP_new_by_curve_name(NID_secp224r1) + EC_POINT_new()
     */
    ecc_point *g = wc_ecc_new_point();
    if (g == NULL)
    {
        wc_ecc_del_point(r1);
        wc_ecc_del_point(r2);
        return -1;
    }

    int rv = _fm_crypto_scmult_reduce(r1, u, &P->pubkey, P->dp);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_read_radix((mp_int *)&g->x, P->dp->Gx, MP_RADIX_HEX);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_read_radix((mp_int *)&g->y, P->dp->Gy, MP_RADIX_HEX);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_set(g->z, 1);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_POINT_mul()
     */
    rv = _fm_crypto_scmult_reduce(r2, v, g, P->dp);
    CHECK_RV_GOTO(rv, cleanup);

    rv = _fm_crypto_points_add(r, r1, r2, P->dp);

cleanup:
    wc_ecc_del_point(r1);
    wc_ecc_del_point(r2);
    wc_ecc_del_point(g);

    return rv;
}

int fm_crypto_derive_primary_or_secondary_x(const byte sk[32],
                                            const byte p[57],
                                            byte out[28])
{
    ecc_key P;

    /*
     * OpenSSL: EC_KEY_new_by_curve_name(NID_secp224r1)
     */
    int rv = wc_ecc_init(&P);
    CHECK_RV(rv);

    /*
     * OpenSSL: EC_GROUP_new_by_curve_name(NID_secp224r1) + EC_POINT_new()
     */
    ecc_point *r = wc_ecc_new_point();
    if (r == NULL)
    {
        wc_ecc_free(&P);
        return rv;
    }

    /*
     * OpenSSL: EC_POINT_set_affine_coordinates_GFp()
     */
    rv = wc_ecc_import_x963_ex(p, 1 + 28 * 2, &P, ECC_SECP224R1);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_POINT_is_on_curve()
     */
    rv = wc_ecc_check_key(&P);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    byte at[2 * (28 + 8)];
    rv = wc_X963_KDF(WC_HASH_TYPE_SHA256,
                     sk, 32,                                               /* Z */
                     KDF_LABEL_DIVERSIFY, sizeof(KDF_LABEL_DIVERSIFY) - 1, /* sharedinfo */
                     at, sizeof(at));                                      /* derived key */
    CHECK_RV_GOTO(rv, cleanup);

    rv = _fm_crypto_scmult_twin_reduce(r, at, &at[28 + 8], &P);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: BN_bn2bin() + EC_POINT_get_affine_coordinates_GFp()
     */
    rv = mp_to_unsigned_bin_len(r->x, out, 28);
cleanup:
    wc_ecc_free(&P);
    wc_ecc_del_point(r);

    return rv;
}

int fm_crypto_derive_server_shared_secret(const byte seeds[32],
                                          const byte seedk1[32],
                                          byte out[32])
{
    byte ikm[2 * 32];
    XMEMCPY(ikm, seeds, 32);
    XMEMCPY(ikm + 32, seedk1, 32);

    /*
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    return wc_X963_KDF(WC_HASH_TYPE_SHA256,
                       ikm, sizeof(ikm),                                   /* Z */
                       KDF_LABEL_SERVERSS, sizeof(KDF_LABEL_SERVERSS) - 1, /* sharedinfo */
                       out, 32);                                           /* derived key */
}

/*! @function _fm_crypto_aes128gcm_encrypt
 @abstract Encrypts a message using AES-128-GCM.

 @param key        128-bit AES key.
 @param iv         128-bit IV.
 @param msg_nbytes Byte length of message.
 @param msg        Message.
 @param out        Output buffer for the ciphertext.
 @param tag        Output buffer for the 128-bit authentication tag.

 @return 0 on success, a negative value on error.
 */
static int _fm_crypto_aes128gcm_encrypt(const byte key[16],
                                        const byte iv[16],
                                        word32 msg_nbytes,
                                        const byte *msg,
                                        byte *out,
                                        byte *tag)
{
    Aes aes;
    int rv = wc_AesInit(&aes, NULL, INVALID_DEVID);
    CHECK_RV(rv);

    /*
     * OpenSSL: EVP_EncryptInit_ex()
     */
    rv = wc_AesGcmSetKey(&aes, key, 16);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EVP_Encrypt*() + EVP_aes_128_gcm()
     */
    rv = wc_AesGcmEncrypt(&aes, out, msg, msg_nbytes, iv, 16, tag, 16, NULL, 0);

cleanup:
    wc_AesFree(&aes);
    return rv;
}


/*! @function _fm_crypto_aes128gcm_decrypt
 @abstract Decrypts a ciphertext using AES-128-GCM.

 @param key       128-bit AES key.
 @param iv        128-bit IV.
 @param ct_nbytes Byte length of ciphertext.
 @param ct        Ciphertext.
 @param tag       128-bit authentication tag.
 @param out       Output buffer for the plaintext.

 @return 0 on success, a negative value on error.
 */
static int _fm_crypto_aes128gcm_decrypt(const byte key[16],
                                        const byte iv[16],
                                        word32 ct_nbytes,
                                        const byte *ct,
                                        const byte *tag,
                                        byte *out)
{
    Aes aes;
    int rv = wc_AesInit(&aes, NULL, INVALID_DEVID);
    CHECK_RV(rv);

    /*
     * OpenSSL: EVP_DecryptInit_ex()
     */
    rv = wc_AesGcmSetKey(&aes, key, 16);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EVP_Decrypt*() + EVP_aes_128_gcm()
     */
    rv = wc_AesGcmDecrypt(&aes, out, ct, ct_nbytes, iv, 16, tag, 16, NULL, 0);

cleanup:
    wc_AesFree(&aes);
    return rv;
}

int fm_crypto_decrypt_e3(const byte serverss[32],
                         word32 e3_nbytes,
                         const byte *e3,
                         word32 *out_nbytes,
                         byte *out)
{
    // E3 has the 16 byte tag appended.
    if (e3_nbytes <= 16)
    {
        return -1;
    }

    if (*out_nbytes < e3_nbytes - 16)
    {
        return -1;
    }

    /*
     * Derive K1 and IV1.
     *
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    byte key_iv[32];
    int rv = wc_X963_KDF(WC_HASH_TYPE_SHA256,
                         serverss, 32,                                             /* Z */
                         KDF_LABEL_PAIRINGSESS, sizeof(KDF_LABEL_PAIRINGSESS) - 1, /* sharedinfo */
                         key_iv, sizeof(key_iv));                                  /* derived key */
    CHECK_RV(rv);

    /*
     * OpenSSL: EVP_Decrypt*() + EVP_aes_128_gcm()
     */
    rv = _fm_crypto_aes128gcm_decrypt(key_iv, key_iv + 16, e3_nbytes - 16, e3,
                                      e3 + e3_nbytes - 16, out);
    CHECK_RV(rv);

    *out_nbytes = e3_nbytes - 16;
    return rv;
}

int fm_crypto_verify_s2(const byte pub[65],
                        word32 sig_nbytes,
                        const byte *sig,
                        word32 msg_nbytes,
                        const byte *msg)
{
    ecc_key Q;
    mp_int r, s;
    int valid = 0;

    /*
     * OpenSSL: EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)
     */
    int rv = wc_ecc_init(&Q);
    CHECK_RV(rv);

    rv = mp_init(&r);
    if (rv)
    {
        wc_ecc_free(&Q);
        return rv;
    }

    rv = mp_init(&s);
    if (rv)
    {
        mp_clear(&r);
        wc_ecc_free(&Q);
        return rv;
    }

    /*
     * OpenSSL: EC_POINT_set_affine_coordinates_GFp()
     */
    rv = wc_ecc_import_x963_ex(pub, 1 + 32 * 2, &Q, ECC_SECP256R1);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_POINT_is_on_curve()
     */
    rv = wc_ecc_check_key(&Q);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: SHA256()
     */
    byte hash[32];
    rv = fm_crypto_sha256(msg_nbytes, msg, hash);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: ECDSA_verify()
     */
    rv = wc_ecc_verify_hash(sig, sig_nbytes, hash, sizeof(hash), &valid, &Q);

    if (rv == 0 && valid != 1)
    {
        rv = -1;
    }

cleanup:
    mp_clear(&r);
    mp_clear(&s);
    wc_ecc_free(&Q);

    return rv;
}

int fm_crypto_authenticate_with_ksn(const byte serverss[32],
                                    word32 msg_nbytes,
                                    const byte *msg,
                                    byte out[32])
{
    Hmac hmac;

    int rv = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    CHECK_RV(rv);

    /*
     * Derive KSN.
     *
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    byte ksn[32];
    rv = wc_X963_KDF(WC_HASH_TYPE_SHA256,
                     serverss, 32,                                               /* Z */
                     KDF_LABEL_SNPROTECTION, sizeof(KDF_LABEL_SNPROTECTION) - 1, /* sharedinfo */
                     ksn, sizeof(ksn));                                          /* derived key */
    CHECK_RV_GOTO(rv, cleanup);

    rv = wc_HmacSetKey(&hmac, WC_HASH_TYPE_SHA256, ksn, sizeof(ksn));
    CHECK_RV_GOTO(rv, cleanup);

    rv = wc_HmacUpdate(&hmac, msg, msg_nbytes);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: HMAC(EVP_sha256())
     */
    rv = wc_HmacFinal(&hmac, out);
    CHECK_RV_GOTO(rv, cleanup);

cleanup:
    wc_HmacFree(&hmac);
    return rv;
}

int fm_crypto_generate_seedk1(byte out[32])
{
    WC_RNG rng;
    int rv = wc_InitRng(&rng);
    CHECK_RV(rv);

    /*
     * OpenSSL: RAND_bytes()
     */
    rv = wc_RNG_GenerateBlock(&rng, out, 32);
    wc_FreeRng(&rng);
    return rv;
}

int fm_crypto_encrypt_to_server(const byte pub[65],
                                word32 msg_nbytes,
                                const byte *msg,
                                word32 *out_nbytes,
                                byte *out)
{
    ecc_key key, Q;
    byte x[32];
    word32 x_len = (word32)sizeof(x);

    // Need space for Q || C || T.
    if (*out_nbytes < 65 + msg_nbytes + 16)
    {
        return -1;
    }

    WC_RNG rng;
    int rv = wc_InitRng(&rng);
    CHECK_RV(rv);

    /*
     * OpenSSL: EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)
     */
    rv = wc_ecc_init(&key);
    if (rv)
    {
        wc_FreeRng(&rng);
        return rv;
    }

    /*
     * OpenSSL: EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)
     */
    rv = wc_ecc_init(&Q);
    if (rv)
    {
        wc_ecc_free(&key);
        wc_FreeRng(&rng);
        return rv;
    }

    /*
     * Import and check Q_E.
     *
     * OpenSSL: EC_POINT_set_affine_coordinates_GFp()
     */
    rv = wc_ecc_import_x963_ex(pub, 1 + 32 * 2, &Q, ECC_SECP256R1);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * OpenSSL: EC_POINT_is_on_curve()
     */
    rv = wc_ecc_check_key(&Q);
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * Generate ephemeral key.
     *
     * OpenSSL: EC_KEY_generate_key()
     */
    rv = wc_ecc_make_key_ex(&rng, 32, &key, ECC_SECP256R1);
    CHECK_RV_GOTO(rv, cleanup);

#if defined(ECC_TIMING_RESISTANT)
    rv = wc_ecc_set_rng(&key, &rng);
    CHECK_RV_GOTO(rv, cleanup);
#endif

    /*
     * Generate shared secret.
     *
     * OpenSSL: ECDH_compute_key()
     */
    rv = wc_ecc_shared_secret(&key, &Q, x, &x_len);
    CHECK_RV_GOTO(rv, cleanup);
    assert(x_len == 32);

    // Compute sharedinfo.
    byte info[2 * 65];
    XMEMCPY(info + 65, pub, 65);

    /*
     * OpenSSL: BN_bn2bin() + EC_POINT_get_affine_coordinates_GFp()
     */
    info[0] = 0x04;
    rv = mp_to_unsigned_bin_len(key.pubkey.x, info + 1, 32);
    CHECK_RV_GOTO(rv, cleanup);

    rv = mp_to_unsigned_bin_len(key.pubkey.y, info + 1 + 32, 32);
    CHECK_RV_GOTO(rv, cleanup);

    // Copy Q into out.
    XMEMCPY(out, info, 65);

    /*
     * Derive key and IV.
     *
     * OpenSSL: Custom X9.63 KDF implementation using SHA256()
     */
    byte key_iv[32];
    rv = wc_X963_KDF(WC_HASH_TYPE_SHA256,
                     x, sizeof(x),            /* Z */
                     info, sizeof(info),      /* sharedinfo */
                     key_iv, sizeof(key_iv)); /* derived key */
    CHECK_RV_GOTO(rv, cleanup);

    /*
     * Encrypt.
     *
     * OpenSSL: EVP_Encrypt*() + EVP_aes_128_gcm()
     */
    rv = _fm_crypto_aes128gcm_encrypt(key_iv, key_iv + 16, msg_nbytes, msg,
                                      out + 65, out + 65 + msg_nbytes);
    CHECK_RV_GOTO(rv, cleanup);

    *out_nbytes = 65 + msg_nbytes + 16;

cleanup:
    wc_ecc_free(&Q);
    wc_ecc_free(&key);
    wc_FreeRng(&rng);
    return rv;
}
