/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     aes_cbc_api.c
* @brief    This file provides aes cbc api wrapper for sdk customer.
* @details
* @author   jane_zhang
* @date     2021-9-24
* @version  v1.0
*********************************************************************************************************
*/
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __AES_CBC_API_H_
#define __AES_CBC_API_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>
/*============================================================================*
 *                              Variables
*============================================================================*/


/*============================================================================*
 *                              Functions
*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  128 bit AES CBC encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @param  p_iv         initialization vector (IV) for CBC mode
    * @param  data_word_len    word length of the data to be descrypted, must be multiples of 4
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
    */
bool aes128_cbc_encrypt(uint8_t *plaintext, const uint8_t key[16], uint8_t *encrypted,
                        uint32_t *p_iv, uint32_t data_word_len);

/**
    * @brief  128 bit AES CBC descryption on speicified plaintext and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @param  p_iv         initialization vector (IV) for CBC mode
    * @param  data_word_len    word length of the data to be descrypted, must be multiples of 4
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
    */
bool aes128_cbc_decrypt(uint8_t *input, const uint8_t key[16], uint8_t *output, uint32_t *p_iv,
                        uint32_t data_word_len);

/**
    * @brief  128 bit AES CBC encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @param  p_iv         initialization vector (IV) for CBC mode
    * @param  data_word_len    word length of the data to be descrypted, must be multiples of 4
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   most significant octet of encrypted data corresponds to encypted[0]
    */
bool aes128_cbc_encrypt_msb2lsb(uint8_t plaintext[16], const uint8_t key[16], uint8_t *encrypted,
                                uint32_t *p_iv, uint32_t data_word_len);

/**
    * @brief  128 bit AES CBC descryption on speicified plaintext and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @param  p_iv         initialization vector (IV) for CBC mode
    * @param  data_word_len    word length of the data to be descrypted, must be multiples of 4
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   most significant octet of encrypted data corresponds to encypted[0]
    */
bool aes128_cbc_decrypt_msb2lsb(uint8_t *input, const uint8_t key[16], uint8_t *output,
                                uint32_t *p_iv, uint32_t data_word_len);

/**
    * @brief  256 bit AES CBC encryption on speicified plaintext and keys
    * @param  plaintext    specifed plain text to be encypted
    * @param  key          keys to encrypt the plaintext
    * @param  encrypted    output buffer to store encrypted data
    * @param  p_iv         initialization vector (IV) for CBC mode
    * @param  data_word_len    word length of the data to be descrypted, must be multiples of 4
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
    */
bool aes256_cbc_encrypt(uint8_t *plaintext, const uint8_t key[32], uint8_t *encrypted,
                        uint32_t *p_iv, uint32_t data_word_len);

/**
    * @brief  256 bit AES CBC descryption on speicified plaintext and keys
    * @param  input    specifed encypted data to be decypted
    * @param  key          keys to decrypt the data
    * @param  output    output buffer to store plain data
    * @param  p_iv         initialization vector (IV) for CBC mode
    * @param  data_word_len    word length of the data to be descrypted, must be multiples of 4
    * @return encryption results
    * @retval true      successful
    * @retval false     fail
    * @note   least significant octet of encrypted data corresponds to encypted[0]
    */
bool aes256_cbc_decrypt(uint8_t *input, const uint8_t key[32], uint8_t *output, uint32_t *p_iv,
                        uint32_t data_word_len);


#ifdef __cplusplus
}
#endif

#endif
