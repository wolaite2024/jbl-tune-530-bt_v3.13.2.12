/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     aes_cmac_api.c
* @brief    This file provides aes cmac api wrapper for sdk customer.
* @details
* @author   jane_zhang
* @date
* @version  v1.0
*********************************************************************************************************
*/
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __AES_CMAC_API_H_
#define __AES_CMAC_API_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>

/** @defgroup  HAL_87x3e_AES_CMAC    AES CMAC
    * @brief aes cmac api
    * @{
    */
/*============================================================================*
 *                              Variables
*============================================================================*/
/** @defgroup HAL_87x3e_AES_CMAC_Exported_Macros AES CMAC Exported Macros
    * @brief
    * @{
    */
#define AES_CMAC128_LSB_MSG_LEN_MAX (255)

/** End of HAL_87x3e_AES_CMAC_Exported_Macros
    * @}
    */
/*============================================================================*
 *                              Functions
*============================================================================*/

/** @defgroup HAL_87x3e_AES_CMAC_Exported_Functions AES CMAC Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Implementation of aes_cmac_msb function.
 *
 * \param key   aes cmac key value.
 * \param input pointer to the msb input data buffer.
 * \param length input data length.
 * \param mac   pointer to the msb output data buffer.
 *
 * @return aes cmac calculation results
 * @retval true      successful
 * @retval false     fail
 */
bool aes_cmac128_msb(unsigned char *key, unsigned char *input, int length,
                     unsigned char *mac);

/**
 * Implementation of aes_cmac_lsb function.
 *
 * \param key   aes cmac key value.
 * \param input pointer to the lsb input data buffer.
 * \param length input data length,it should be less than AES_CMAC128_LSB_MSG_LEN_MAX.
 * \param mac   pointer to the lsb output data buffer.
 *
 * @return aes cmac calculation results
 * @retval true      successful
 * @retval false     fail
 */
bool aes_cmac128_lsb(unsigned char key[16], unsigned char *msg,
                     int msg_len, unsigned char mac[16]);

#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_87x3e_AES_CMAC_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_AES_CMAC */
#endif
