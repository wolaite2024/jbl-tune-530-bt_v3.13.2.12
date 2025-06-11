/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_le_sync.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef     BT_BOND_LE_SYNC_H
#define     BT_BOND_LE_SYNC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <bt_bond_le.h>

typedef struct
{
    uint16_t flags;
    uint16_t cccd_len;
    uint8_t local_ltk[32];
    uint8_t remote_ltk[32];
    uint8_t remote_irk[24];
    uint8_t client_supported_feature[4];
    uint8_t srv_change_aware_state[4];
    uint8_t cccd[];
} T_BT_LE_DEV_INFO;

typedef struct
{
    T_BT_BOND_INFO bond_info;
    T_BT_LE_DEV_INFO dev_info;
} T_BT_LE_DEV_DATA;

typedef struct
{
    uint8_t link_key_length;
    uint8_t padding[3];
    uint8_t key[28];
} T_BT_LE_LTK;

/**
 * @brief bt_le_dev_info_clear
 * clear all device information
 */
void bt_le_dev_info_clear(void);

/**
 * @brief bt_le_dev_info_cmp
 * comapre device information
 * @param p_info1
 * @param p_info2
 * @return true
 * @return false
 */
bool bt_le_dev_info_cmp(T_BT_LE_DEV_DATA *p_info1, T_BT_LE_DEV_DATA *p_info2);

/**
 * @brief bt_le_dev_info_get
 * get device information by entry
 * @param p_entry
 * @param pp_info
 * @param p_info_len
 * @return true
 * @return false
 */
bool bt_le_dev_info_get(T_LE_BOND_ENTRY *p_entry, T_BT_LE_DEV_DATA **pp_info, uint16_t *p_info_len);

/**
 * @brief bt_le_dev_info_set
 * set device information
 * @param p_bond_info
 * @param p_info
 * @return T_LE_BOND_ENTRY*
 */
T_LE_BOND_ENTRY *bt_le_dev_info_set(T_BT_BOND_INFO *p_bond_info, T_BT_LE_DEV_INFO *p_info);

/**
 * @brief bt_le_dev_info_set_cccd
 * set cccd information by entry
 * @param p_entry
 * @param p_cccd
 * @return true
 * @return false
 */
bool bt_le_dev_info_set_cccd(T_LE_BOND_ENTRY *p_entry, T_BT_CCCD_DATA *p_cccd);

T_LE_BOND_ENTRY *bt_le_dev_info_pre_set(T_BT_BOND_INFO *p_bond_info, uint8_t *p_key);

/**
 * @brief bt_le_dev_info_get_local_ltk
 * get local LTK by entry
 * @param p_entry
 * @param p_key
 * @return true
 * @return false
 */
bool bt_le_dev_info_get_local_ltk(T_LE_BOND_ENTRY *p_entry, uint8_t *p_key);

/**
 * @brief bt_le_dev_info_find
 * find device information
 * @param p_info
 * @return T_LE_BOND_ENTRY*
 */
T_LE_BOND_ENTRY *bt_le_dev_info_find(T_BT_BOND_INFO *p_info);

/**
 * @brief bt_le_dev_info_set_high_priority
 * set high priority device information
 * @param p_info
 * @return true
 * @return false
 */
bool bt_le_dev_info_set_high_priority(T_BT_BOND_INFO *p_info);

/**
 * @brief bt_le_dev_info_remove
 * remove device information
 * @param p_info
 * @return true
 * @return false
 */
bool bt_le_dev_info_remove(T_BT_BOND_INFO *p_info);

/**
 * @brief bt_le_clear_bond_state
 * clear bond state
 * @param p_entry
 * @return true
 * @return false
 */
bool bt_le_clear_bond_state(T_LE_BOND_ENTRY *p_entry);

#ifdef __cplusplus
}
#endif

#endif /* BT_BOND_LE_SYNC_H */
