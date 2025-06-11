/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_SERVICE_H_
#define _APP_BLE_SERVICE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_BLE_SERVICE App Ble Service
  * @brief App Ble Service
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BLE_SERVICE_Exported_Macros App Ble Service Macros
   * @{
   */
#define TX_ENABLE_CCCD_BIT                  0x01
#define TX_ENABLE_AUTHEN_BIT                0x02
#define TX_ENABLE_READY                     (TX_ENABLE_CCCD_BIT|TX_ENABLE_AUTHEN_BIT)

/** End of APP_BLE_SERVICE_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_SERVICE_Exported_Functions App Ble Service Functions
    * @{
    */
/**
    * @brief  Ble Service module init
    * @param  void
    * @return void
    */
void app_ble_service_init(void);

/** @} */ /* End of group APP_BLE_SERVICE_Exported_Functions */

/** End of APP_BLE_SERVICE
* @}
*/



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_SERVICE_H_ */
