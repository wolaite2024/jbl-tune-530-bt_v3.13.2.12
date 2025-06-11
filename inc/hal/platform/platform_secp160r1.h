/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    platform_uecc.h
  * @brief   micro_ecc Platform abstraction layer
  * @author
  * @date    2023-4-27
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

#ifndef _PLATFORM_SECP160R1_H_
#define _PLATFORM_SECP160R1_H_

/** @defgroup  HAL_UECC_API    uecc api
    * @brief This file introduces the uecc APIs
    * @{
    */

/** @defgroup HAL_uecc_Exported_Functions uecc Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    SECP160R1 = 0,
    SECP256R1,
    SECP256K1,/*to do*/
    UECC_ALGORITHE_NUM
} T_UECC_ALGORITHM_TYPE;

/**
    * @brief  calculate pulic key on the base of aes_key
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @param aes_key: the point of input of private key to calculate
    * @param public_key: the pulic key calculated based on aes key
    * @param r : the result of pubic key mod k of curve parameter
    * @param uecc_type : curve type selected to calculate public key
    * @return void
    */
int platform_uecc_compute_public_key(uint8_t *aes_key, uint8_t *public_key, uint8_t *r,
                                     T_UECC_ALGORITHM_TYPE uecc_type);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group HAL_Log_Control_Exported_Functions */
/** @} */ /* End of group HAL_LOG_API */
#endif /* _PLATFORM_UECC_H_ */
