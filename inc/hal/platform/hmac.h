/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    hmac.h
  * @brief   sha256hkdf algorithm
  * @author
  * @date    2023-4-27
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

#ifndef HMAC_H
#define HMAC_H

/** @defgroup  HAL_sha256hkdf    sha256hkdf api
    * @brief This file introduces sha256hkdf APIs
    * @{
    */

/** @defgroup HAL_sha256hkdf_Exported_Functions sha256hkdf Exported Functions
    * @brief
    * @{
    */

#include <string.h>
#include <stddef.h>
#include <stdint.h>


#define HMAC_SELF_TEST  0

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief          Output = Generic_HMAC( hmac key, input buffer )
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 * \param md_info  message digest info
 * \param key      HMAC secret key
 * \param keylen   length of the HMAC key in bytes
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 * \param output   Generic HMAC-result
 *
 * \returns        0 on success, MBEDTLS_ERR_MD_BAD_INPUT_DATA if parameter
 *                 verification fails.
 */
uint32_t sha256_hkdf(const uint8_t *key, uint32_t key_len, const uint8_t *salt, uint32_t salt_len,
                     const uint8_t *info, uint32_t info_len,
                     uint8_t *out, uint32_t out_len);

#if (HMAC_SELF_TEST == 1)

void hkdf_test(void);

#endif /* HMAC_SELF_TEST */

#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_sha256hkdf_Exported_Functions */
/** @} */ /* End of group HAL_sha256hkdf */

#endif /* hmac.h */
