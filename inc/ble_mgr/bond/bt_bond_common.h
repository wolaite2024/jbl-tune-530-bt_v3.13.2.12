/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_common.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef     BT_BOND_COMMON_H
#define     BT_BOND_COMMON_H

#include "bt_bond_le.h"

#ifdef __cplusplus
extern "C"
{
#endif
/*bond state: indicate bond information exist or not*/
#define BT_BOND_INFO_EXIST_FLAG 0x01
/*bond state: indicate bond information need update or not*/
#define BT_BOND_INFO_NEED_UPDATE_FLAG 0x02
/*bond state: indicate re-pair or not*/
#define BT_BOND_INFO_REPAIR_FLAG      0x04

#define BT_BOND_MSG_LE_BOND_ADD       0x00
#define BT_BOND_MSG_LE_BOND_REMOVE    0x01
#define BT_BOND_MSG_LE_BOND_CLEAR     0x02
#define BT_BOND_MSG_LE_BOND_UPDATE    0x03
//#define BT_BOND_MSG_LE_SET_HIGH_PRIORITY    0x04
#define BT_BOND_MSG_LE_BOND_GET       0x10

typedef struct
{
    uint8_t         bond_state;
    uint8_t         key_type;
    uint8_t         remote_bd_type;
    uint8_t         local_bd_type;
    uint8_t         remote_bd[6];
    uint8_t         local_bd[6];
    uint32_t        bond_flag;
} T_BT_BOND_INFO;

typedef void(*P_FUN_BT_BOND_CB)(uint8_t cb_type, void *p_cb_data);

/**
 * @brief bt_bond_register_app_cb
 * APP can call bt_bond_register_app_cb to register callback to get bt bond modify information.
 * APP can register more than one callback function using bt_bond_register_app_cb.
 * APP will not send message BT_BOND_MSG_LE_BOND_GET.
 *
 * @param app_callback Callback function: @ref P_FUN_BT_BOND_CB.
 * @return true
 * @return false
* \code{.c}
    void ble_bond_sync_init(void)
    {
        ......
        bt_bond_register_app_cb(ble_bond_sync_cb);
        ......
    }

    void ble_bond_sync_cb(uint8_t cb_type, void *p_cb_data)
    {
        ......
        switch (cb_type)
        {
        case BT_BOND_MSG_LE_BOND_ADD:
            {

            }
            break;

        case BT_BOND_MSG_LE_BOND_REMOVE:
            {

            }
            break;

        case BT_BOND_MSG_LE_BOND_CLEAR:
            {

            }
            break;

        case BT_BOND_MSG_LE_BOND_UPDATE:
            {

            }
            break;

        default:
            break;
        }
        return;
    }
 * \endcode
 */
bool bt_bond_register_app_cb(P_FUN_BT_BOND_CB app_callback);

/**
 * @brief bt_bond_unregister_app_cb
 * APP can call bt_bond_unregister_app_cb to unregister callback to get bt bond modify information.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @param app_callback Callback function: @ref P_FUN_BT_BOND_CB.
 * @return true
 * @return false
 */
bool bt_bond_unregister_app_cb(P_FUN_BT_BOND_CB app_callback);

/**
 * @brief bt_bond_register_bond_get_cb
 * APP can call bt_bond_register_bond_get_cb to register callback to handle message BT_BOND_MSG_LE_BOND_GET.
 * APP can only register one callback function.
 * APP will send message BT_BOND_MSG_LE_BOND_GET when no matching key entry is found.
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @param app_callback Callback function: @ref P_FUN_BT_BOND_CB.
 * @return true
 * @return false
* \code{.c}
    void test(void)
    {
        ......
        bt_bond_register_bond_get_cb(ble_bond_get_cb);
    }

    void ble_bond_get_cb(uint8_t cb_type, void *p_cb_data)
    {
        ......
        switch (cb_type)
        {
        case BT_BOND_MSG_LE_BOND_GET:
            {
                T_BT_LE_BOND_CB_DATA cb_data;
                T_LE_BOND_ENTRY *p_entry = NULL;

                memcpy(&cb_data, p_cb_data, sizeof(T_BT_LE_BOND_CB_DATA));
                APP_PRINT_INFO5("BT_BOND_MSG_LE_BOND_GET: bd_addr %s, bd_type %d, local_bd_addr %s, local_bd_type %d, key_type %d",
                                TRACE_BDADDR(cb_data.p_le_bond_get->bd_addr),
                                cb_data.p_le_bond_get->bd_type,
                                TRACE_BDADDR(cb_data.p_le_bond_get->local_bd_addr),
                                cb_data.p_le_bond_get->local_bd_type,
                                cb_data.p_le_bond_get->key_type);

                p_entry = bt_le_find_key_entry_by_priority(1);
                if (p_entry != NULL)
                {
                    T_BT_LE_LTK le_ltk;
                    if(bt_le_dev_info_get_local_ltk(p_entry, (uint8_t *)&le_ltk))
                    {
                        cb_data.p_le_bond_get->cfm_cause = GAP_CFM_CAUSE_ACCEPT;
                        cb_data.p_le_bond_get->key_len = le_ltk.link_key_length;
                        memcpy(cb_data.p_le_bond_get->key_data,le_ltk.key, 28);
                    }
                }
            }
            break;

        default:
            break;
        }
        return;
    }
 * \endcode
 */
bool bt_bond_register_bond_get_cb(P_FUN_BT_BOND_CB app_callback);

#ifdef __cplusplus
}
#endif

#endif /* BT_BOND_COMMON_H */
