/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_ble_whitelist.c
   * @brief     This file handles BLE whitelist application routines.
   * @author    danni
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "app_ble_whitelist.h"
#include "os_queue.h"
#include "gap_le.h"
#include "stdlib.h"
#include "string.h"
#include "ble_scan.h"

/*============================================================================*
 *                              Variables
 *============================================================================*/

/** @brief whitelist operation element struct */
typedef struct t_ble_whitelist_op_elem
{
    struct t_ble_whitelist_op_elem *p_next;
    T_GAP_WHITE_LIST_OP operation;
    T_GAP_REMOTE_ADDR_TYPE bd_type;/* remote bd address type*/
    uint8_t bd_addr[6];/*remote bd address*/
} T_BLE_WHITELIST_OP_ELEM;

/*whitelist operation state: true is busy, false is not busy*/
static bool white_list_op_busy = false;
/*whitelist operation pending queue*/
static T_OS_QUEUE white_list_op_q;

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
 * @brief Set whitelist operation state is busy or not
 *
 * busy: Indicates that a whitelist operation is currently being executed.
 * not busy: Indicates that no whitelist operation is currently being executed.
 * @param busy   true is busy, false is not busy.
 */
void app_ble_whitelist_set_operation_state(bool busy)
{
    white_list_op_busy = busy;
}

/**
 * @brief Get whitelist operation state
 *
 * busy: Indicates that a whitelist operation is currently being executed.
 * not busy: Indicates that no whitelist operation is currently being executed.
 * @return true   busy
 * @return false  not busy
 */
bool app_ble_whitelist_get_operation_state(void)
{
    return white_list_op_busy;
}

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
                                     T_GAP_REMOTE_ADDR_TYPE bd_type)
{
    T_GAP_CAUSE ret;

    if (white_list_op_busy == false)
    {
        white_list_op_busy = true;
        ble_scan_pause();
        ret = le_modify_white_list(operation, bd_addr, bd_type);
        ble_scan_resume();
    }
    else
    {
        T_BLE_WHITELIST_OP_ELEM *op = calloc(1, sizeof(T_BLE_WHITELIST_OP_ELEM));

        if (!op)
        {
            ret = GAP_CAUSE_NO_RESOURCE;
            return ret;
        }

        op->operation = operation;
        op->bd_type = bd_type;
        memcpy(op->bd_addr, bd_addr, 6);
        os_queue_in(&white_list_op_q, op);
        ret = GAP_CAUSE_SUCCESS;
    }
    return ret;
}

/**
 * @brief APP handle pending operation.
 *
 * when APP received GAP_MSG_LE_MODIFY_WHITE_LIST,
 * APP shall call this API to handle pending operation in queue.
 * @return T_GAP_CAUSE
 */
T_GAP_CAUSE app_ble_whitelist_handle_pending_operation()
{
    T_GAP_CAUSE cause = GAP_CAUSE_SUCCESS;

    if (white_list_op_q.count != 0)
    {
        T_BLE_WHITELIST_OP_ELEM *op = os_queue_out(&white_list_op_q);
        if (op != NULL)
        {
            cause = app_ble_whitelist_modify(op->operation, op->bd_addr, op->bd_type);
            free(op);
            op = NULL;
        }
    }
    return cause;
}

/**
 * @brief initialize whitelist module
 *
 * initialize queue for pending whitelist
 */
void app_ble_whitelist_init(void)
{
    os_queue_init(&white_list_op_q);
}
