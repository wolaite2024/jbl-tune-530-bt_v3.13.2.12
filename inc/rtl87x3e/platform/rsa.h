/**
 * \file rsa.h
 *
 * \brief The RSA public-key cryptosystem
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#ifndef MBEDTLS_RSA_H
#define MBEDTLS_RSA_H


#include "bignum.h"

/*
 * RSA Error codes
 */
#define MBEDTLS_ERR_RSA_BAD_INPUT_DATA                    -0x4080  /**< Bad input parameters to function. */
#define MBEDTLS_ERR_RSA_INVALID_PADDING                   -0x4100  /**< Input data contains invalid padding and is rejected. */
#define MBEDTLS_ERR_RSA_KEY_GEN_FAILED                    -0x4180  /**< Something failed during generation of a key. */
#define MBEDTLS_ERR_RSA_KEY_CHECK_FAILED                  -0x4200  /**< Key failed to pass the library's validity check. */
#define MBEDTLS_ERR_RSA_PUBLIC_FAILED                     -0x4280  /**< The public key operation failed. */
#define MBEDTLS_ERR_RSA_PRIVATE_FAILED                    -0x4300  /**< The private key operation failed. */
#define MBEDTLS_ERR_RSA_VERIFY_FAILED                     -0x4380  /**< The PKCS#1 verification failed. */
#define MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE                  -0x4400  /**< The output buffer for decryption is not large enough. */
#define MBEDTLS_ERR_RSA_RNG_FAILED                        -0x4480  /**< The random generator failed to generate non-zeros. */

/*
 * The above constants may be used even if the RSA module is compile out,
 * eg for alternative (PKCS#11) RSA implemenations in the PK layers.
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          RSA context structure
 */
typedef struct
{
    int ver;                    /*!<  always 0          */
    size_t len;                 /*!<  size(N) in chars  */

    mbedtls_mpi N;                      /*!<  public modulus    */
    mbedtls_mpi E;                      /*!<  public exponent   */
    mbedtls_mpi RN;                     /*!<  cached R^2 mod N  */
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;    /*!<  Thread-safety mutex       */
#endif
} mbedtls_rsa_context;
/**
 * \brief          This function initializes an RSA context.
 *
 * \param ctx      The RSA context to initialize. This must not be NULL.
 */
void mbedtls_rsa_init(mbedtls_rsa_context *ctx);
/**
 * \brief          This function frees the components of an RSA key.
 *
 * \param ctx      It points to an initialized RSA context.
 */
void mbedtls_rsa_free(mbedtls_rsa_context *ctx);
/**
 * \brief          This function performs an RSA public key operation.
 *

 * \param ctx      The initialized RSA context to use.
 * \param input    The input buffer. This must be a readable buffer of length ctx->len Bytes. For example, 256 Bytes for an 2048-bit RSA modulus.
 * \param output   The output buffer. This must be a writable buffer of length ctx->len Bytes. For example, 256 Bytes for an 2048-bit RSA modulus.
 * \return         0 if the verify operation was successful, or an MBEDTLS_ERR_RSA_XXX error code.
 */
int mbedtls_rsa_public(mbedtls_rsa_context *ctx, const unsigned char *input, unsigned char *output);

/**
 * \brief          Generic wrapper to perform a PKCS#1 verification using the
 *                 mode from the context. Do a public RSA operation and check
 *                 the message digest
 *
 * \param ctx      points to an RSA public key
 * \param hashlen  message digest length (for MBEDTLS_MD_NONE only)
 * \param hash     buffer holding the message digest
 * \param sig      buffer holding the ciphertext
 *
 * \return         0 if the verify operation was successful,
 *                 or an MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 128 bytes if RSA-1024 is used).
 */
int mbedtls_rsa_pkcs1_verify(mbedtls_rsa_context *ctx,
                             unsigned int hashlen,
                             const unsigned char *hash,
                             const unsigned char *sig);
/**
 * \brief          Generic wrapper to perform a PKCS#1 verification using the
 *                 mode from RSA public Key N and E value. Do a public RSA operation and check
 *                 the message digest
 *
 * \param keyN     points to the N value of an RSA public key.
 * \param keyN     points to the E value of an RSA public key.
 * \param hashlen  message digest length (for MBEDTLS_MD_NONE only, eg. 32 if SHA256 is used).
 * \param hash     buffer holding the message digest.
 * \param sig      buffer holding the ciphertext.
 *
 * \return         0 if the verify operation was successful,
 *                 or an MBEDTLS_ERR_RSA_XXX error code
 *
 * \note           The "sig" buffer must be as large as the size
 *                 of ctx->N (eg. 256 bytes if RSA-2048 is used).
 */
int mbedtls_rsa_pkcs1_sha256_validate(uint8_t *keyN, uint8_t *keyE, uint32_t hashlen,
                                      const uint8_t *hash, const uint8_t *sig);

#ifdef __cplusplus
}
#endif


#endif /* rsa.h */

