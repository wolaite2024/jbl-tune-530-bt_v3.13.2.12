/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _ENGAGE_H_
#define _ENGAGE_H_

#include <stdint.h>
//#include "app_msg.h"
#include "gap_msg.h"
#include "gap_callback_le.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_ENGAGE App Engage
  * @brief App Engage
  * @{
  */

/**  @brief engage lib ind to app */
typedef enum
{
    ENGAGE_IND_FE_SHAKING_DONE                = 0,
    ENGAGE_IND_AFE_SHAKING_TO                 = 1,
    ENGAGE_IND_AFE_SHAKING_DONE               = 2,
} T_ENGAGE_IND;

/**  @brief engage lib ind app to do callback function */
typedef void (*T_ENGAGE_IND_FUN)(T_ENGAGE_IND ind);

/**  @brief engage lib get bat and state from app */
typedef void (*T_BAT_STATE_FUN)(uint8_t *bat, uint8_t *state);

/**  @brief app share cfg with engage lib */
typedef struct
{
    uint8_t const_bud_role;
    uint8_t *nv_first_engaged;
    uint8_t *nv_bud_role;
    uint8_t *nv_bud_local_addr;
    uint8_t *nv_bud_peer_addr;
    uint8_t *nv_bud_peer_factory_addr;
    uint8_t *factory_addr;
    uint8_t const_stop_adv_cause;
    int8_t const_fe_mp_rssi;
    uint16_t const_rws_custom_uuid;
    uint8_t const_high_batt_pri;
    uint8_t const_addition;
    uint8_t ref_saiyan_mode_en;
    uint8_t ref_saiyan_mode_role;
} T_ENGAGE_CFG;

/**
    * @brief  engage lib init
    * @param  void
    * @return void
    */
void engage_init(void);

/**
    * @brief  engage lib startup
    * @param  cfg app share cfg with engage lib
    * @param  ind_fun engage lib ind app callback function
    * @param  get battery state function
    * @return void
    */
void engage_on(T_ENGAGE_CFG *cfg, T_ENGAGE_IND_FUN ind_fun, T_BAT_STATE_FUN bat_state_fun,
               uint8_t *remote_bat_vol);

/**
    * @brief  engage lib shutdown
    * @param  void
    * @return void
    */
void engage_off(void);

/**
    * @brief  start first engage shaking
    * @param  void
    * @return void
    */
void engage_fe_shaking_start(void);

/**
    * @brief  start timeout shaking(b2b is not connected)
    * @param  void
    * @return void
    */
void engage_afe_timeout_shaking_start(void);

/**
    * @brief  ble advertising primary
    * @param  adv_interval
    * @return void
    */
void engage_afe_primary_adv_start(uint8_t adv_interval);


/**
    * @brief  after first engage done(b2b is connected, stop ble's adv and scan)
    * @param  void
    * @return void
    */
void engage_afe_done(void);

/**
    * @brief  start forever shaking
    * @param  void
    * @return void
    */
void engage_afe_forever_shaking_start(void);

/**
    * @brief  change bud's role
    * @param  pri/sec
    * @return void
    */
void engage_afe_change_role(int change_role);

/** End of APP_ENGAGE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ENGAGE_H_ */
