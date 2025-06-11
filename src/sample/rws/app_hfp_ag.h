/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_HFP_AG_AG_H_
#define _APP_HFP_AG_AG_H_

#include "bt_hfp_ag.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_HFP_AG App HFP AG
  * @brief this file handle hfp profile related process
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_HFP_AG_Exported_Functions App Hfp Functions
    * @{
    */
/**
    * @brief  HFP AG module init.
    * @param  void.
    * @return void.
    */
void app_hfp_ag_init(void);

/**
    * @brief  get current call status.
    * @param  void.
    * @return @ref T_APP_HFP_CALL_STATUS.
    */
T_BT_HFP_AG_CALL_STATUS app_hfp_ag_get_call_status(void);

/**
    * @brief  get active hfp ag connection br link id.
    * @param  void.
    * @return link id.
    */
uint8_t app_hfp_ag_get_active_hfp_ag_index(void);

/**
    * @brief  set active hfp ag connection br link .
    * @param  bd_addr active link bt device address.
    * @return link id.
    */
bool app_hfp_ag_set_active_hfp_ag_index(uint8_t *bd_addr);

/** @} */ /* End of group APP_HFP_AG_Exported_Functions */
/** End of APP_HFP_AG
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_HFP_AG_AG_H_ */
