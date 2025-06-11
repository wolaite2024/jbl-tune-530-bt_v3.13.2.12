#include <string.h>
#include "trace.h"
#include "gatt.h"
#include "bt_bond_mgr.h"
#include "ble_audio_bond.h"
#include "bt_bond_legacy.h"
#include "bt_bond_le.h"
#include "gap_bond_le.h"
#include "ble_audio.h"

#if F_APP_BOND_MGR_SUPPORT

extern bool le_get_conn_local_addr(uint16_t conn_handle, uint8_t *bd_addr, uint8_t *bd_type);
extern bool bt_bond_handle_cccd_pending(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                                        uint8_t *local_bd_addr, uint8_t local_bd_type,
                                        uint16_t cccd_handle, bool data_pending);

bool ble_audio_bond_check(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle)
{
    T_LE_BOND_ENTRY *p_key_entry;
    uint8_t local_bd[GAP_BD_ADDR_LEN];
    uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;
    le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);
    p_key_entry = bt_le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type, local_bd,
                                       local_bd_type);
    if (p_key_entry)
    {
        return true;
    }

    return false;
}

bool ble_audio_bond_get_key(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                            bool remote, uint8_t *p_key_len, uint8_t *p_key)
{
    T_LE_BOND_ENTRY *p_entry;
    if (bd_addr == NULL || p_key_len == NULL || p_key == NULL)
    {
        return false;
    }
    uint8_t local_bd[GAP_BD_ADDR_LEN];
    uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;
    le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);
    p_entry = bt_le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type, local_bd, local_bd_type);
    if (p_entry)
    {
        if ((remote == true && (p_entry->flags & LE_KEY_STORE_REMOTE_LTK_BIT)) ||
            (remote == false && (p_entry->flags & LE_KEY_STORE_LOCAL_LTK_BIT)))
        {
            if (bt_le_get_dev_ltk(p_entry, remote, p_key_len, p_key))
            {
                return true;
            }
        }
    }
    return false;
}

uint8_t ble_audio_bond_get_max_num(bool is_le)
{
    if (is_le)
    {
        return bt_le_get_max_le_paired_device_num();
    }
    else
    {
        return bt_legacy_get_max_bond_num();
    }
}

bool ble_audio_bond_get_addr(bool is_le, uint8_t bond_idx, uint8_t *bd_addr, uint8_t *p_bd_type,
                             uint8_t *local_bd_addr, uint8_t *p_local_bd_type)
{
    if (bd_addr == NULL || p_bd_type == NULL ||
        local_bd_addr == NULL || p_local_bd_type == NULL)
    {
        return false;
    }
    if (is_le)
    {
        T_LE_BOND_ENTRY *p_key_entry;
        p_key_entry = bt_le_find_key_entry_by_idx(bond_idx);
        if (p_key_entry != NULL)
        {
            memcpy(bd_addr, p_key_entry->bond_info.remote_bd, 6);
            *p_bd_type = p_key_entry->bond_info.remote_bd_type;
            memcpy(local_bd_addr, p_key_entry->bond_info.local_bd, 6);
            *p_local_bd_type = p_key_entry->bond_info.local_bd_type;
            return true;
        }
    }
    return false;
}

bool ble_audio_bond_set_cccd_flag(uint8_t *bd_addr, uint8_t bd_type,
                                  uint8_t *local_bd_addr, uint8_t local_bd_type,
                                  uint16_t cccd_handle, uint16_t flags)
{
    bool data_pending = false;
    if (flags & GATT_CLIENT_CHAR_NOTIF_IND_DATA_PENGDING)
    {
        data_pending = true;
    }
    return bt_bond_handle_cccd_pending(bd_addr, bd_type, 0,
                                       local_bd_addr, local_bd_type, cccd_handle, data_pending);
}

bool ble_audio_bond_clear_cccd_flag(uint8_t *bd_addr, uint8_t bd_type,
                                    uint16_t conn_handle, uint16_t cccd_handle)
{
    uint8_t local_bd[GAP_BD_ADDR_LEN];
    uint8_t local_bd_type = LE_BOND_LOCAL_ADDRESS_ANY;
    le_get_conn_local_addr(conn_handle, local_bd, &local_bd_type);
    return bt_bond_handle_cccd_pending(bd_addr, bd_type, conn_handle,
                                       local_bd, local_bd_type, cccd_handle, false);
}

bool ble_audio_bond_le_resolve_rpa(uint8_t *unresolved_addr, uint8_t *identity_addr,
                                   uint8_t *p_identity_addr_type)
{
    return bt_le_resolve_random_address(unresolved_addr, identity_addr, p_identity_addr_type);
}

void ble_audio_bt_bond_cb(uint8_t cb_type, void *p_cb_data)
{
    PROTOCOL_PRINT_INFO1("[BTBond] ble_audio_bt_bond_cb: cb_type %d", cb_type);
    T_BT_LE_BOND_CB_DATA cb_data;
    T_BT_BOND_MODIFY bond_modify;
    memset(&bond_modify, 0, sizeof(bond_modify));
    memcpy(&cb_data, p_cb_data, sizeof(T_BT_LE_BOND_CB_DATA));
    if (cb_type == BT_BOND_MSG_LE_BOND_ADD)
    {
        if (cb_data.p_le_bond_add->p_entry &&
            (cb_data.p_le_bond_add->modify_flags & LE_BOND_MODIFY_REMOTE_SYNC) == 0)
        {
            bond_modify.type = BT_BOND_ADD;
            memcpy(bond_modify.remote_addr, cb_data.p_le_bond_add->p_entry->remote_bd, 6);
            bond_modify.remote_bd_type = cb_data.p_le_bond_add->p_entry->remote_bd_type;
        }
        else
        {
            return;
        }
    }
    else if (cb_type == BT_BOND_MSG_LE_BOND_REMOVE)
    {
        if (cb_data.p_le_bond_remove->p_entry &&
            (cb_data.p_le_bond_remove->modify_flags & LE_BOND_MODIFY_REMOTE_SYNC) == 0)
        {
            bond_modify.type = BT_BOND_DELETE;
            memcpy(bond_modify.remote_addr, cb_data.p_le_bond_remove->p_entry->remote_bd, 6);
            bond_modify.remote_bd_type = cb_data.p_le_bond_remove->p_entry->remote_bd_type;
        }
        else
        {
            return;
        }
    }
    else if (cb_type == BT_BOND_MSG_LE_BOND_CLEAR)
    {
        bond_modify.type = BT_BOND_CLEAR;
    }
    else
    {
        return;
    }
    ble_audio_bond_modify(&bond_modify);
    return;
}

const T_BT_BOND_MGR ble_audio_bond =
{
    .bond_check             = ble_audio_bond_check,
    .bond_get_key           = ble_audio_bond_get_key,
    .bond_get_max_num       = ble_audio_bond_get_max_num,
    .bond_get_addr          = ble_audio_bond_get_addr,
    .bond_set_cccd_flag     = ble_audio_bond_set_cccd_flag,
    .bond_clear_cccd_flag   = ble_audio_bond_clear_cccd_flag,
    .bond_le_resolve_rpa    = ble_audio_bond_le_resolve_rpa,
};

void ble_audio_bond_init(void)
{
    ble_audio_reg_bond_mgr(&ble_audio_bond);
    bt_bond_register_app_cb(ble_audio_bt_bond_cb);
}
#endif
