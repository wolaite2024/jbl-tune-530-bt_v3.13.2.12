/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_RTK_TUYA_BLE_ADV_H_
#define _APP_RTK_TUYA_BLE_ADV_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble_ext_adv.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_TUYA App Tuya
  * @brief App TUYA
  * @{
  */

/** @defgroup APP_TUYA_ADV App tuya adv
  * @brief App tuya adv
  * @{
  */
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_TUYA_ADV_Exported_Functions APP_TUYA_ADV_Exported_Functions
  * @{
  */

/**
  * @brief  start ble tuya advertising
  * @param  duration_s advertising duration time
  * @return true  Command has been sent successfully.
  * @return false Command was fail to send.
  */
bool le_tuya_adv_start(uint16_t timeout_sec);

/**
  * @brief  stop ble tuya advertising
  * @param  void
  * @return true  Command has been sent successfully.
  * @return false Command was fail to send.
  */
bool le_tuya_adv_stop(uint8_t app_cause);

/**
  * @brief  get connection id
  * @param  void
  * @return connection id
  */
uint8_t le_tuya_adv_get_conn_id(void);

/**
  * @brief  reset connection id to 0xFF
  * @param  void
  * @return void
  */
void le_tuya_adv_reset_conn_id(void);

/**
  * @brief  get tuya adv handle
  * @param  void
  * @return tuya adv handle
  */
uint8_t le_tuya_adv_get_adv_handle(void);

/**
  * @brief  init tuya adv param
  * @param  void
  * @return void
  */
void le_tuya_adv_init(void);
/** End of APP_TUYA_ADV_Exported_Functions
  * @}
  */

/** End of APP_TUYA_ADV
  * @}
  */

/** End of APP_RWS_TUYA
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TUYA_BLE_ADV_H_ */
