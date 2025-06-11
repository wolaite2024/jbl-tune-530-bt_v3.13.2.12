/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_TTS_OTA_H_
#define _APP_BLE_TTS_OTA_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble_ext_adv.h"
#include "remote.h"
#include "os_queue.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_TTS_OTA App BLE TTS OTA
  * @brief App BLE Device
  * @{
  */

typedef struct
{
    bool use_static_addr_type;
    uint8_t adv_handle;
    uint8_t conn_id;
    T_BLE_EXT_ADV_MGR_STATE state;
    T_OS_QUEUE cb_queue;
} T_LE_COMMON_ADV;

typedef void (*LE_COMMON_ADV_CB)(uint8_t cb_type, T_BLE_EXT_ADV_CB_DATA cb_data);


/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BLE_TTS_OTA_Exported_Macros App Ble TTS OTA Macros
   * @{
   */

/** End of APP_BLE_TTS_OTA_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_TTS_OTA_Exported_Functions App Ble TTS OTA Functions
    * @{
    */
/**
    * @brief  get ble common advertising state
    * @param  void
    * @return ble advertising state
    */
T_BLE_EXT_ADV_MGR_STATE app_ble_common_adv_get_state(void);

/**
    * @brief  get ble common advertising handle
    * @param  void
    * @return ble advertising handle
    */
uint8_t app_ble_common_adv_get_adv_handle(void);

/**
    * @brief  get ble common advertising conn id
    * @param  void
    * @return ble advertising conn id
    */
uint8_t app_ble_common_adv_get_conn_id(void);

/**
    * @brief  reset ble common advertising conn id
    * @param  void
    * @return void
    */
void app_ble_common_adv_reset_conn_id(void);

/**
    * @brief  start rws ble common advertising
    * @param  duration_ms advertising duration time
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool app_ble_common_adv_start_rws(uint16_t duration_10ms);

/**
    * @brief  Update OTA advertising data
    * This api can only be used for secondary ear ota update adv data.
    * Note: Ensure that the advertising address of the secondary ear and the primary ear are different.
    *       If the advertising addresses on both sides are the same, the phone may not know which ear to be upgrade.
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool app_ble_common_adv_ota_data_update(void);

/**
    * @brief  update bud role in rws ble common advertising data when role swap and start advertising
    * @param  T_REMOTE_SESSION_ROLE
    * @return void
    */
void app_ble_common_adv_handle_roleswap(T_REMOTE_SESSION_ROLE role);

/**
    * @brief  role swap fail, primary start advertising
    * @param  T_REMOTE_SESSION_ROLE
    * @return void
    */
void app_ble_common_adv_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role);
/**
    * @brief  sync le adv start flag
    * @return void
    */
void app_ble_common_adv_sync_start_flag(void);

/**
    * @brief  update bud role in rws ble common advertising data when engage role decided and start advertising
    * @param  T_REMOTE_SESSION_ROLE
    * @return void
    */
void app_ble_common_adv_handle_engage_role_decided(void);

/**
    * @brief  start ble common advertising
    * @param  duration_ms advertising duration time
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool app_ble_common_adv_start(uint16_t duration_10ms);

/**
    * @brief  stop ble common advertising
    * @param  app_cause cause
    * @return true  Command has been sent successfully.
    * @return false Command was fail to send.
    */
bool app_ble_common_adv_stop(int8_t app_cause);

/**
    * @brief  set ble common advertising random address
    * @param  random_address random address
    * @return void
    */
void app_ble_common_adv_set_random(uint8_t *random_address);

/**
    * @brief  init ble common advertising parameters
    * @param  void
    * @return void
    */
void app_ble_common_adv_init(void);



void app_ble_common_adv_cb_reg(LE_COMMON_ADV_CB cb);

/**
    * @brief  update ble common advertising scan response data
    * @param  void
    * @return true update success
    */
bool app_ble_common_adv_update_scan_rsp_data(void);

/**
 * @brief set common advertising connection handle
 *
 * @param conn_id
 */
void app_ble_common_adv_set_conn_id(uint8_t conn_id);

void app_harman_adv_data_update(uint8_t *p_adv_data, uint8_t adv_data_len);
/** @} */ /* End of group APP_BLE_TTS_OTA_Exported_Functions */

/** End of APP_BLE_TTS_OTA
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_TTS_H_ */
