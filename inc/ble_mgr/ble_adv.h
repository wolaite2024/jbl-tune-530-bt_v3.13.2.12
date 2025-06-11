/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BLE_ADV_H_
#define _BLE_ADV_H_

#include "gap_le.h"
#include "gap_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

///@cond

/** @defgroup BLE_ADV Ble Adv
  * @brief Ble adv manager module
  * @{
  */


/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup BLE_ADV_Exported_Macros Ble Adv Macros
    * @{
    */

#define BLE_ADV_STATE_CHANGE 0x01 /**< used to notify adv set application callback function about the change of adv state*/
#define BLE_ADV_MGR_VERSION   1 /**< used to set ble adv manager module version*/

/** End of BLE_ADV_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup BLE_ADV_Exported_Types Ble Adv Types
    * @{
    */

/**
 * @brief define application adv sate, for application only have two adv state :BLE_ADV_STATE_IDLE or BLE_ADV_STATE_ADVERTISING
 */
typedef enum
{
    BLE_ADV_STATE_IDLE, /**< when call api le_adv_mgr_disable or le_adv_mgr_disable_all, the application adv state shall be set to BLE_ADV_STATE_IDLE*/
    BLE_ADV_STATE_ADVERTISING, /**< when call api  le_adv_mgr_enable or le_adv_mgr_enable_two, the application adv state shall be set to BLE_ADV_STATE_ADVERTISING*/
} T_BLE_ADV_STATE;

/**
 * @brief define the reason of  adv stop
 */
typedef enum
{
    BLE_ADV_STOP_CAUSE_UNKNOWN,
    BLE_ADV_STOP_CAUSE_APP,
    BLE_ADV_STOP_CAUSE_CONN,
    BLE_ADV_STOP_CAUSE_TIMEOUT,
} T_BLE_ADV_STOP_CAUSE;

/**
 * @brief used to notify application callback function about the change of adv state
 */
typedef struct
{
    uint8_t adv_handle;
    T_BLE_ADV_STATE state;
    T_BLE_ADV_STOP_CAUSE stop_cause; /**< Used when: BLE_ADV_STATE_IDLE*/
    uint8_t app_cause; /**< Used when: BLE_ADV_STATE_IDLE(BLE_ADV_STOP_CAUSE_APP)*/
    uint8_t conn_id;  /**< Used when: BLE_ADV_STATE_IDLE(BLE_ADV_STOP_CAUSE_CONN)*/
} T_BLE_ADV_STATE_CHANGE;

/**
 * @brief T_BLE_ADV_CB_DATA  @ref T_BLE_ADV_STATE_CHANGE
 */
typedef union
{
    T_BLE_ADV_STATE_CHANGE *p_ble_state_change;
} T_BLE_ADV_CB_DATA;

/** End of BLE_ADV_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup BLE_ADV_Exported_Functions Ble Adv Functions
    * @{
    */

/**
*@brief    Allocate storage space for ble_adv_set_table,the size of ble_adv_set_table = ble_adv_handle_num_max * sizeof(T_LE_ADV_SET)
*@note     This api shall be used first if you want to use the module of ble_adv_mgr
*@param[in] adv_handle_num the maximum of ble adv handle, each adv set has a adv handle
*@return   T_GAP_CAUSE
*@retval   GAP_CAUSE_SUCCESS   success
*@retval   GAP_CAUSE_INVALID_PARAM   the adv_handle_num is 0
*@retval   GAP_CAUSE_NO_RESOURCE   no space in RAM_TYPE_DATA_ON to allocate
*/
T_GAP_CAUSE ble_adv_mgr_init(uint8_t adv_handle_num);

/**
*@brief     used to handle @ref T_GAP_DEV_STATE
*@note
*@param[in] T_GAP_DEV_STATE new_state
*@param[in]  cause
*@return   none
*@retval   none
*/
void ble_adv_mgr_handle_adv_state(T_GAP_DEV_STATE new_state, uint16_t cause);

/**
*@brief     used to handle gap callback msg @ref T_LE_CB_DATA
*@note
*@param[in] cb_type
*@param[in]  T_LE_CB_DATA *p_data
*@return   T_APP_RESULT
*@retval   APP_RESULT_SUCCESS
*/
T_APP_RESULT ble_adv_mgr_handle_gap_callback(uint8_t cb_type, T_LE_CB_DATA *p_data);

/**
*@brief    create adv handle and register app callback
*@note     This api is used to create adv handle for an unused adv set
*@param[in] app_callback application callback function which want to be registered into adv set
*@return   adv_handle
*@retval  0xFF:    all adv set has been used in ble_adv_set_table or le_adv_mgr_init() not called before this api
*@retval  other values: the created adv handle
*/
uint8_t ble_adv_mgr_create_adv_handle(P_FUN_GAP_APP_CB app_callback);

/**
*@brief    set adv parameters into adv set
*@note     This api is used to set adv parameters into adv set, the adv set was found by adv handle
*          if there Only one adv set is enabled, and p_adv->action is BLE_ADV_ACTION_RUNNING then set p_adv->action = BLE_ADV_ACTION_UPDATE
*          to update adv param in ble_adv_mgr_check_next_step
*@param[in] adv_handle
*@param[in] adv_type
*@param[in] adv_interval_min
*@param[in] adv_interval_max
*@param[in] own_address_type
*@param[in] peer_address_type
*@param[in] p_peer_address
*@param[in] filter_policy
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_NOT_FIND: le_adv_mgr_set_adv_param: not find
*@retval  GAP_CAUSE_SUCCESS: le_adv_mgr_set_adv_param: success
*/
T_GAP_CAUSE ble_adv_mgr_set_adv_param(uint8_t adv_handle, T_GAP_ADTYPE adv_type,
                                      uint16_t adv_interval_min, uint16_t adv_interval_max,
                                      T_GAP_LOCAL_ADDR_TYPE own_address_type,
                                      T_GAP_REMOTE_ADDR_TYPE peer_address_type, uint8_t *p_peer_address,
                                      T_GAP_ADV_FILTER_POLICY filter_policy);

/**
*@brief    change advertising interval
*@note     none
*@param[in] adv_handle
*@param[in] adv_interval
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_NOT_FIND:  not find
*@retval  GAP_CAUSE_SUCCESS: success
*/
T_GAP_CAUSE ble_adv_mgr_change_adv_interval(uint8_t adv_handle, uint16_t adv_interval);

/**
*@brief    set adv data into adv set
*@note     This api is used to set adv data into adv set, the adv set was found by adv handle
*          if there Only one adv set is enabled, and p_adv->action is BLE_ADV_ACTION_RUNNING then set p_adv->action = BLE_ADV_ACTION_UPDATE
*          to update adv data in ble_adv_mgr_check_next_step
*@param[in] adv_handle
*@param[in] adv_data_len
*@param[in] p_adv_data
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_NOT_FIND: le_adv_mgr_set_adv_data: not find
*@retval  GAP_CAUSE_SUCCESS: le_adv_mgr_set_adv_data: success
*/
T_GAP_CAUSE ble_adv_mgr_set_adv_data(uint8_t adv_handle, uint16_t adv_data_len,
                                     uint8_t *p_adv_data);

/**
*@brief    set scan response data
*@note     none
*@param[in] scan_data_len
*@param[in] p_scan_data
*@return   T_GAP_CAUSE
*/
T_GAP_CAUSE ble_adv_mgr_set_scan_response_data(uint16_t scan_data_len, uint8_t *p_scan_data);

/**
*@brief    set random address into adv set
*@note     This api is used to set random address into adv set, the adv set was found by adv handle
*          if there Only one adv set is enabled, and p_adv->action is BLE_ADV_ACTION_RUNNING then set p_adv->action = BLE_ADV_ACTION_UPDATE
*          to update random address in ble_adv_mgr_check_next_step
*@param[in] adv_handle
*@param[in] random_address
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_NOT_FIND: le_adv_mgr_set_random: not find
*@retval  GAP_CAUSE_SUCCESS: le_adv_mgr_set_random: success
*/
T_GAP_CAUSE ble_adv_mgr_set_random(uint8_t adv_handle, uint8_t *random_address);

/**
*@brief    enable one adv set
*@note     This api is used to enbale conn adv set or unconn adv set
*@param[in] adv_handle
*@param[in] duration  ms
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_NOT_FIND: p_adv == NULL
*@retval  GAP_CAUSE_SUCCESS: success
*/
T_GAP_CAUSE ble_adv_mgr_enable(uint8_t adv_handle, uint32_t duration);

/**
*@brief    enable two adv set
*@note     This api is used to enbale conn adv set and unconn adv set
*@param[in] conn_adv_handle
*@param[in] nonconn_adv_handle
*@param[in] duration  ms
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_INVALID_STATE:p_conn_enabled_adv_set != NULL ||p_nonconn_enabled_adv_set != NULL
*@retval  GAP_CAUSE_INVALID_PARAM: adv_type error
*@retval  GAP_CAUSE_NOT_FIND:p_adv == NULL || p_non_conn_adv == NULL
*@retval  GAP_CAUSE_SUCCESS:success
*/
T_GAP_CAUSE ble_adv_mgr_enable_two(uint8_t conn_adv_handle, uint8_t nonconn_adv_handle,
                                   uint32_t duration);

/**
*@brief     disable one adv set
*@note     This api is used to disable conn adv set or unconn adv set, only one adv set can be disabled at one time
*@param[in] adv_handle
*@param[in] app_cause
*@return   T_GAP_CAUSE
*@retval  GAP_CAUSE_INVALID_STATE:adv_handle not running
*@retval  GAP_CAUSE_NOT_FIND:p_adv == NULL
*@retval  GAP_CAUSE_SUCCESS: success
*/
T_GAP_CAUSE ble_adv_mgr_disable(uint8_t adv_handle, uint8_t app_cause);

/**
*@brief     disable all adv set
*@note     This api is used to disable conn adv set and unconn adv set, if both conn adv set and unconn adv set are enabled
*@param[in] app_cause
*@return   T_GAP_CAUSE
*@retval   GAP_CAUSE_SUCCESS: success
*/
T_GAP_CAUSE ble_adv_mgr_disable_all(uint8_t app_cause);

/**
*@brief     print info in adv set
*@note     none
*@param[in] none
*@return   none
*@retval  none
*/
void ble_adv_mgr_print_info(void);


/** @} */ /* End of group APP_DEVICE_Exported_Functions */
/** End of BLE_ADV
* @}
*/
///@endcond

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ADV_H_ */
