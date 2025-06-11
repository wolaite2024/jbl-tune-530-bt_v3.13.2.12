/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_A2DP_H_
#define _APP_A2DP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_A2DP App A2dp
  * @brief App A2dp
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_A2DP_Exported_Functions App A2dp Functions
    * @{
    */
/**
    * @brief  judge whether a2dp is streaming
    * @param  void
    * @return bool
    */
bool app_a2dp_is_streaming(void);

/**
    * @brief  a2dp module init
    * @param  void
    * @return void
    */
void app_a2dp_init(void);

/**
    * @brief  set current active a2dp link index
    * @param  idx
    * @return void
    */
void app_a2dp_set_active_idx(uint8_t idx);

/**
    * @brief  get current active a2dp link index
    * @param  void
    * @return active a2dp index
    */
uint8_t app_a2dp_get_active_idx(void);

/**
    * @brief  get current inactive a2dp link index
    * @param  void
    * @return inactive a2dp index
    */
uint8_t app_a2dp_get_inactive_idx(void);

/** @} */ /* End of group APP_A2DP_Exported_Functions */

/** End of APP_A2DP
* @}
*/



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_A2DP_H_ */
