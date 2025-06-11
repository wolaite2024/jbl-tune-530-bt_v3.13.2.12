#ifndef _APP_LEA_CCP_H_
#define _APP_LEA_CCP_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "app_link_util.h"
#include "app_types.h"
#include "app_lea_ccp_def.h"

/** @defgroup APP_LEA_CCP App LE Audio CCP
  * @brief this file handle CCP profile related process
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LEA_CCP_Exported_Functions App LE Audio CCP Functions
    * @{
    */

/**
    * @brief  ccp module init. Register ble audio callback
    *         to handle ccp message
    * @param  void
    * @return void
    */
void app_lea_ccp_init(void);

/**
    * @brief  Get ccp active link connection handle
    * @param  void
    * @return LE link connection handle
    */
uint16_t app_lea_ccp_get_active_conn_handle(void);

/**
    * @brief  Set ccp active link connection handle
    * @param  conn_handle LE link connection handle
    * @return true: success; false: fail
    */
bool app_lea_ccp_set_active_conn_handle(uint16_t conn_handle);

/**
    * @brief  Reset ccp active link connection handle
    * @param  void
    * @return void
    */
void app_lea_ccp_reset_active_conn_handle(void);

/**
    * @brief  Get LE Audio device call status
    * @param  void
    * @return LE Audio device call status
    */
T_APP_CALL_STATUS app_lea_ccp_get_call_status(void);

/**
    * @brief  Update LE Audio device CCP call status
    * @param  void
    * @return void
    */
void app_lea_ccp_update_call_status(void);

/**
    * @brief  Release current LE link active call
    * @param  p_link the LE link
    * @return true: success; false: fail
    */
bool app_lea_ccp_release_active_call(T_APP_LE_LINK *p_link);

/**
    * @brief  Accept current LE link hold or wait call
    * @param  p_link the LE link
    * @return true: success; false: fail
    */
bool app_lea_ccp_accept_hold_or_wait_call(T_APP_LE_LINK *p_link);

/**
    * @brief  Find call entry by call index
    * @param  conn_id LE link connection index
    * @param  call_index call index
    * @return call entry
    * @retval \ref T_LEA_CALL_ENTRY
    */
T_LEA_CALL_ENTRY *app_lea_ccp_find_call_entry_by_idx(uint8_t conn_id, uint8_t call_index);


/**
    * @brief  Read all lea links CCP call state
    * @param  void
    * @return void
    */
void app_lea_ccp_read_all_links_state(void);
/** @} */ /* End of group APP_LEA_CCP_Exported_Functions */
/** End of APP_LEA_CCP
* @}
*/

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
