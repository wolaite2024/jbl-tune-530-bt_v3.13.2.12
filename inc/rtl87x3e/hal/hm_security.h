/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     hm_security.h
* @brief    This file provides api for harman security.
* @details
* @author
* @date     2024-12-25
* @version  v1.0
*****************************************************************************************
*/
#ifndef __HM_SECURITY_H__
#define __HM_SECURITY_H__
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
/*   @brief     Define the function prototype to read public key.
 *
 *   @param     PublicKeyBuff Pointer to buffer public key.
 *   @param     PublicKeyBuffSize Length of buffer.
 *
 *   @retval    The number of data bytes read back,
 *              negative error code otherwise.
 */
typedef int32_t (*READ_PUB_KEY_FUNC)(uint8_t *public_key_buf, uint16_t buf_size);
/**
 *   @brief     Register the function to read the public key. It should be called before hm_read_public_key.
 *
 *   @param     rd_pub_key_func Specify the public key reading function with prototype READ_PUB_KEY_FUNC.
 *
 */
void register_chip_id_pub_key_read_func(READ_PUB_KEY_FUNC rd_pub_key_func);
/**
 *   @brief     Read the public key from efuse/opt or flash.
 *
 *   @param     PublicKeyBuff Pointer to buffer public key.
 *   @param     PublicKeyBuffSize Length of buffer.
 *
 *   @retval    The number of data bytes read back,
 *              negative error code otherwise.
 */
int32_t hm_read_public_key(uint8_t *public_key_buf, uint16_t buf_size);

/**
 *  @brief      Read the signature/security codes from efuse/opt or flash.
 *
 *  @param      SignatureBuff Pointer to the butter Signature.
 *  @param      SignatureBuffSize Length of buffer.
 *
 *  @retval     The number of data bytes read from the efuse/opt or flash on success,
 *              negative error code otherwise.
 */
int32_t hm_read_signature(uint8_t *signature_buf, uint16_t buf_size);

/**
 *  @brief      Generate hash function.
 *
 *  @param      input Pointer to the input bytes to be hashed.
 *  @param      input_len Length of the input bytes.
 *  @param      output Pointer to the output buffer.
 *  @param      output_size Length of buffer.
 *
 *  @retval     The number of data bytes read success, negative error code otherwise.
 */
int32_t hm_generate_hash256(const uint8_t *input, uint16_t input_len, uint8_t *output,
                            uint16_t output_size);

/**
 *  @brief      Signature(PKCS#1 v1.5) recover function.
 *
 *  @param      signature Pointer to the input bytes to be decrypted.
 *  @param      signature_len Length of the input bytes.
 *  @param      public_key Pointer to public key.
 *  @param      public_key_len Length of public key.
 *  @param      output_hash Pointer to the buffer where the decrypted data will be stored.
 *  @param      output_hash_len Length of buffer.
 *
 *  @retval     The number of data bytes read success, negative error code otherwise.
 */
int32_t hm_signature_recover(const uint8_t *signature, uint16_t signature_len,
                             const uint8_t *public_key, uint16_t public_key_len,
                             uint8_t *output_hash, uint16_t output_hash_len);


uint8_t harman_security_check(void);

#ifdef __cplusplus
}
#endif
#endif
