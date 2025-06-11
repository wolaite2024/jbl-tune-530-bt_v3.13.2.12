/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_SWIFT_PAIR_H_
#define _APP_BLE_SWIFT_PAIR_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble_ext_adv.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_SWIFT_PAIR App BLE Swift Pair
  * @brief App BLE Deviceswift pair module
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BLE_SWIFT_PAIR_Exported_Macros App Ble Swift Pair Macros
   * @{
   */
#define APP_SWIFT_PAIR_DEFAULT_TIMEOUT (0)

#define APP_SWIFT_PAIR_DEFAULT_ADV_DATA_CHANGE_PRIOR_TIMEOUT (30)
#define APP_SWIFT_PAIR_ADV_KEEP_VENDOR_SECTUON_MIN_TIMEOUT (1)
/** End of APP_BLE_SWIFT_PAIR_Exported_Macros
    * @}
    */

typedef enum
{
    APP_TIMER_SWIFT_PAIR_CHANGE_ADV_DATA
} T_APP_SWIFT_PAIR_TIMER;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_SWIFT_PAIR_Exported_Functions App Ble TTS OTA Functions
    * @{
    */

/**
    * @brief  start ble common advertising
    * @param  duration_ms advertising duration time
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool app_swift_pair_adv_start(uint16_t duration_10ms);

/**
    * @brief  stop ble common advertising
    * @param  app_cause cause
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool app_swift_pair_adv_stop(int8_t app_cause);

/**
    * @brief  init ble swift pair advertising parameters
    * @param  void
    * @return void
    */
void app_swift_pair_adv_init(void);

void app_swift_pair_handle_power_on(int16_t duration_10ms);

void app_swift_pair_handle_power_off(void);

bool app_swift_pair_adv_start(uint16_t duration_10ms);

bool app_swift_pair_adv_stop(int8_t app_cause);

void app_swift_pair_start_adv_change_timer(uint16_t timeout);

void app_swift_pair_stop_adv_change_timer(void);

void app_swift_pair_init(void);


/** @} */ /* End of group APP_BLE_SWIFT_PAIR_Exported_Functions */

/** End of APP_BLE_SWIFT_PAIR
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_SWIFT_PAIR_H_ */
