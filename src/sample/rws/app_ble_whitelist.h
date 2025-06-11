#ifndef _APP_BLE_WHITELIST_H_
#define _APP_BLE_WHITELIST_H_

#include "gap.h"
#include "gap_le_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief This api used to modify whitelist
 *
 * modify operations include:
 * GAP_WHITE_LIST_OP_CLEAR or
 * GAP_WHITE_LIST_OP_ADD or
 * GAP_WHITE_LIST_OP_REMOVE
 * @param operation  @ref T_GAP_WHITE_LIST_OP
 * @param bd_addr  remote bd address
 * @param bd_type  remote bd address type
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE app_ble_whitelist_modify(T_GAP_WHITE_LIST_OP operation, uint8_t *bd_addr,
                                     T_GAP_REMOTE_ADDR_TYPE bd_type);

/**
 * @brief Set whitelist operation state is busy or not
 *
 * busy: Indicates that a whitelist operation is currently being executed.
 * not busy: Indicates that no whitelist operation is currently being executed.
 * @param busy   true is busy, false is not busy.
 */
void app_ble_whitelist_set_operation_state(bool busy);

/**
 * @brief APP handle pending operation.
 *
 * when APP received GAP_MSG_LE_MODIFY_WHITE_LIST,
 * APP shall call this API to handle pending operation in queue.
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE app_ble_whitelist_handle_pending_operation(void);

/**
 * @brief initialize whitelist module
 *
 * initialize queue for pending whitelist
 */
void app_ble_whitelist_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _APP_BLE_WHITELIST_H_ */
