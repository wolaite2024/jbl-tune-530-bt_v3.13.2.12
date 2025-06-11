#ifndef _APP_LEA_ADV_H_
#define _APP_LEA_ADV_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "ble_ext_adv.h"

#define APP_LEA_ADV_INTERVAL_FAST   0x30    //For quicker connection setup
#define APP_LEA_ADV_INTERVAL_SLOW   0x140   //For reduced power

/** @defgroup APP_LEA_ADV App LE Audio ADV
  * @brief this file handle lea adv related process
  * @{
  */

/**@brief Set le audio advertising started flag
 *
 * @param[in] flag
 *            started flag is true when lea adv start, and false when lea link connected
 * @return void
 */
void app_lea_adv_set_started_flag(bool flag);

/**@brief Check le audio advertising whether need to start again.

 * @param  No parameter.
 * @return void
 */
void app_lea_adv_check_restart_adv(void);

/**@brief Start BLE advertising
 *
 * @param  No parameter.
 * @return void
 */
void app_lea_adv_start(void);

/**@brief Stop BLE advertising
 *
 * @param  No parameter.
 * @return void
 */
void app_lea_adv_stop(void);

/**@brief Update LEA advertising interval
 *
 * @param[in] interval
 *            interval value for both advertising interval min and advertising interval max
 * @return void
 */
void app_lea_adv_update_interval(uint16_t interval);

/**@brief Update BLE advertising interval by hfp call state

 * @param  No parameter.
 * @return void
 */
void app_lea_adv_update_interval_hfp(void);

/**@brief Get state of BLE advertising in lea scenario

 * @param  No parameter.
 * @return T_BLE_EXT_ADV_MGR_STATE state of BLE advertising
 */
T_BLE_EXT_ADV_MGR_STATE app_lea_adv_get_state(void);

/**@brief get le audio advertising handle

 * @param  No parameter.
 * @return le audio advertising handle
 */
uint8_t app_lea_adv_get_handle(void);

/**@brief Update BLE advertising data

 * @param  No parameter.
 * @return void
 */
void app_lea_adv_update(void);
bool app_lea_adv_linkbacking(void);
void app_lea_adv_linkback_set(bool linkback);

/**
 * @brief  Initialize parameter of advertising data
 *         with TMAP role, csis,...etc. Also setting
 *         adv interval and bt address type.
 * @param  No parameter.
 * @return void
 */
void app_lea_adv_init(void);

/** @} */ /* End of group APP_LEA_ADV_Exported_Functions */
/** End of APP_LEA_ADV
* @}
*/
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
