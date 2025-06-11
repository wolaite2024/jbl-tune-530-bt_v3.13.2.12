#if F_APP_BOND_MGR_BLE_SYNC
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "trace.h"
#include "stdlib.h"
#include "ble_bond_sync.h"
#include "app_timer.h"
#include "bt_bond_le_sync.h"
#include "app_relay.h"
#include "app_cfg.h"

#define BLE_BOND_MTU    (APP_RELAY_MTU)
#define BT_BOND_INFO_HANDLED 0x40
#define BT_BOND_INFO_ADD     0x80

#define  APP_BLE_BOND_SYNC_TIMER_CCCD_INFO_UPDATE 1

typedef enum
{
    APP_BT_BOND_MERGE             = 0x01,
    APP_BT_BOND_UPDATE            = 0x02,
    APP_BT_BOND_ADD               = 0x03,
    APP_BT_BOND_REMOVE            = 0x04,
    APP_BT_BOND_CLEAR             = 0x05,
    APP_BT_BOND_SET_HIGH_PRIORITY = 0x06,
    APP_BT_BOND_EVT_MAX           = 0x07,
} T_APP_BT_BOND_REMOTE_MSG;

// APP_BT_BOND_MERGE
typedef enum
{
    BT_BOND_DATA_STATUS_COMPLETE  = 0x00,
    BT_BOND_DATA_STATUS_MORE      = 0x01
} T_BT_BOND_DATA_STATUS;

typedef struct
{
    T_BT_BOND_INFO bond_info;
    uint8_t        key[32];
} T_BT_BOND_RECORD;

typedef struct
{
    uint8_t bond_num;
    uint8_t total_bond_num;
    T_BT_BOND_DATA_STATUS status;
    T_BT_BOND_RECORD bond_tbl[];
} T_BT_BOND_MERGE;

typedef union
{
    T_BT_BOND_INFO   bond_info;//APP_BT_BOND_SET_HIGH_PRIORITY, APP_BT_BOND_REMOVE
    T_BT_BOND_MERGE  bond_merge; //APP_BT_BOND_MERGE
    T_BT_LE_DEV_DATA bond_add; //APP_BT_BOND_ADD
    T_BT_LE_DEV_DATA bond_update; //APP_BT_BOND_UPDATE
} T_BT_BOND_REMOTE_INFO;

typedef enum
{
    SYNC_IDLE                = 0,
    SYNC_MERGE_INFO_SEND   = 1,
    SYNC_MERGE_INFO_RECEIVED = 2,
} T_SYNC_STATE;

typedef struct
{
    bool remote_connected;
    T_SYNC_STATE  state;
    uint8_t cccd_bond_idx;
    T_BT_BOND_MERGE *p_bond_mgr;
    uint8_t ble_bond_sync_timer_idx;
    uint8_t ble_bond_sync_timer_module_id;
} T_BLE_BOND_SYNC_MGR;

T_BLE_BOND_SYNC_MGR scan_mgr;

bool ble_bond_sync_send_update(T_LE_BOND_ENTRY *p_entry);

inline static bool ble_bond_sync_send_info(uint8_t msg_type,
                                           uint8_t *payload_buf, uint16_t payload_len)
{
    return app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BLE_BOND, msg_type, payload_buf,
                                               payload_len);
}

static void ble_bond_sync_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    if (timer_evt == APP_BLE_BOND_SYNC_TIMER_CCCD_INFO_UPDATE)
    {
        T_LE_BOND_ENTRY *p_entry = bt_le_find_key_entry_by_idx(scan_mgr.cccd_bond_idx);
        APP_PRINT_INFO0("[BTBond] APP_BLE_BOND_SYNC_TIMER_CCCD_INFO_UPDATE");
        ble_bond_sync_send_update(p_entry);
    }
}

bool ble_bond_sync_timer_start(uint8_t bond_idx)
{
    APP_PRINT_INFO1("[BTBond] ble_bond_sync_timer_start: bond_idx %d", bond_idx);

    if (app_start_timer(&scan_mgr.ble_bond_sync_timer_idx, "ble_bond_sync_timer",
                        scan_mgr.ble_bond_sync_timer_module_id,
                        APP_BLE_BOND_SYNC_TIMER_CCCD_INFO_UPDATE, 0, false,  5000) == 0)
    {
        scan_mgr.cccd_bond_idx = bond_idx;
        return true;
    }
    APP_PRINT_ERROR0("[BTBond] ble_bond_sync_timer_start: failed");
    return false;
}

bool ble_bond_sync_timer_stop(void)
{
    APP_PRINT_INFO0("[BTBond] ble_bond_sync_timer_stop");
    if (scan_mgr.ble_bond_sync_timer_idx != 0)
    {
        if (app_stop_timer(&scan_mgr.ble_bond_sync_timer_idx) == 0)
        {
            return true;
        }
    }
    return false;
}

void ble_bond_sync_recovery(void)
{
    APP_PRINT_INFO0("[BTBond] ble_bond_sync_recovery");
    scan_mgr.state = SYNC_IDLE;
    ble_bond_sync_timer_stop();
    if (scan_mgr.p_bond_mgr)
    {
        free(scan_mgr.p_bond_mgr);
        scan_mgr.p_bond_mgr = NULL;
    }
}

bool ble_bond_sync_send_add(T_LE_BOND_ENTRY *p_entry)
{
    T_BT_LE_DEV_DATA *p_info = NULL;
    uint16_t info_len;
    bool ret = false;
    if (bt_le_dev_info_get(p_entry, &p_info, &info_len))
    {
        ret = ble_bond_sync_send_info(APP_BT_BOND_ADD,
                                      (uint8_t *)p_info, info_len);
    }
    if (p_info)
    {
        free(p_info);
    }
    return ret;
}

bool ble_bond_sync_send_update(T_LE_BOND_ENTRY *p_entry)
{
    T_BT_LE_DEV_DATA *p_info = NULL;
    uint16_t info_len;
    bool ret = false;
    if (bt_le_dev_info_get(p_entry, &p_info, &info_len))
    {
        ret = ble_bond_sync_send_info(APP_BT_BOND_UPDATE,
                                      (uint8_t *)p_info, info_len);
    }
    if (p_info)
    {
        free(p_info);
    }
    return ret;
}

void ble_bond_sync_send_merge_info(void)
{
    T_BT_BOND_MERGE *p_info = NULL;
    uint8_t err_idx = 0;
    uint32_t bond_num_per_pkt = (BLE_BOND_MTU - offsetof(T_BT_BOND_MERGE,
                                                         bond_tbl)) / sizeof(T_BT_BOND_RECORD);
    uint8_t local_bond_num = bt_le_get_bond_dev_num();

    if (local_bond_num > 0)
    {
        uint32_t batch_num = local_bond_num / bond_num_per_pkt;
        uint32_t remains = local_bond_num % bond_num_per_pkt;
        p_info = calloc(1, sizeof(*p_info) + bond_num_per_pkt * sizeof(T_BT_BOND_RECORD));
        if (p_info == NULL)
        {
            err_idx = 2;
            goto ret;
        }
        if (remains)
        {
            batch_num++;
        }
        for (uint32_t batch_idx = 0; batch_idx < batch_num; batch_idx++)
        {
            uint32_t dev_info_num = 0;
            p_info->status = BT_BOND_DATA_STATUS_MORE;
            if ((batch_idx == batch_num - 1) && (remains != 0))
            {
                dev_info_num = remains;
            }
            else
            {
                dev_info_num = bond_num_per_pkt;
            }
            if (batch_idx == batch_num - 1)
            {
                p_info->status = BT_BOND_DATA_STATUS_COMPLETE;
            }
            p_info->bond_num = dev_info_num;
            p_info->total_bond_num = local_bond_num;

            for (uint32_t i = 0; i < dev_info_num; i++)
            {
                T_LE_BOND_ENTRY *p_entry = NULL;
                uint32_t pri = bond_num_per_pkt * batch_idx + i + 1;

                p_entry = bt_le_find_key_entry_by_priority(pri);
                if (p_entry == NULL)
                {
                    err_idx = 3;
                    goto ret;
                }
                memcpy(&p_info->bond_tbl[i].bond_info, &p_entry->bond_info, sizeof(T_BT_BOND_INFO));
                bt_le_dev_info_get_local_ltk(p_entry, p_info->bond_tbl[i].key);
            }
            if (ble_bond_sync_send_info(APP_BT_BOND_MERGE,
                                        (uint8_t *)p_info, sizeof(*p_info) + dev_info_num * sizeof(T_BT_BOND_RECORD)) == false)
            {
                err_idx = 4;
                goto ret;
            }
        }
    }
    else
    {
        p_info = calloc(1, sizeof(T_BT_BOND_MERGE));
        if (p_info == NULL)
        {
            err_idx = 6;
            goto ret;
        }
        p_info->bond_num = 0;
        p_info->total_bond_num = 0;
        p_info->status = BT_BOND_DATA_STATUS_COMPLETE;
        if (ble_bond_sync_send_info(APP_BT_BOND_MERGE,
                                    (uint8_t *)p_info, sizeof(T_BT_BOND_MERGE)) == false)
        {
            err_idx = 7;
            goto ret;
        }
    }
ret:
    if (p_info)
    {
        free(p_info);
    }
    if (err_idx != 0)
    {
        APP_PRINT_ERROR1("[BTBond] ble_bond_sync_send_merge_info: failed, err_idx %d", err_idx);
    }
    else
    {
        scan_mgr.state = SYNC_MERGE_INFO_SEND;
    }
    return;
}

void ble_bond_sync_cb(uint8_t cb_type, void *p_cb_data)
{
    APP_PRINT_INFO3("[BTBond] ble_bond_sync_cb: cb_type = %d, app_cfg_nv.bud_role %d, remote %d",
                    cb_type, app_cfg_nv.bud_role, scan_mgr.remote_connected);
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE || scan_mgr.remote_connected == false)
    {
        return;
    }
    T_BT_LE_BOND_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BT_LE_BOND_CB_DATA));
    switch (cb_type)
    {
    case BT_BOND_MSG_LE_BOND_ADD:
        {
            if (cb_data.p_le_bond_add->p_entry &&
                (cb_data.p_le_bond_add->modify_flags & LE_BOND_MODIFY_REMOTE_SYNC) == 0)
            {
                ble_bond_sync_send_add(cb_data.p_le_bond_add->p_entry);
            }
        }
        break;

    case BT_BOND_MSG_LE_BOND_REMOVE:
        {
            if (cb_data.p_le_bond_remove->p_entry &&
                (cb_data.p_le_bond_remove->modify_flags & LE_BOND_MODIFY_REMOTE_SYNC) == 0)
            {
                ble_bond_sync_send_info(APP_BT_BOND_REMOVE,
                                        (uint8_t *)&cb_data.p_le_bond_remove->p_entry->bond_info, sizeof(T_BT_BOND_INFO));
            }
        }
        break;

    case BT_BOND_MSG_LE_BOND_CLEAR:
        {
            if ((cb_data.p_le_bond_clear->modify_flags & LE_BOND_MODIFY_REMOTE_SYNC) == 0)
            {
                ble_bond_sync_send_info(APP_BT_BOND_CLEAR, NULL, 0);
            }
        }
        break;

    case BT_BOND_MSG_LE_BOND_UPDATE:
        {
            if (cb_data.p_le_bond_update->p_entry)
            {
                if (cb_data.p_le_bond_update->modify_flags == LE_BOND_MODIFY_CCCD)
                {
                    ble_bond_sync_timer_start(cb_data.p_le_bond_update->p_entry->idx);
                }
                else
                {
                    ble_bond_sync_send_update(cb_data.p_le_bond_update->p_entry);
                }
            }
        }
        break;
#if 0
    case BT_BOND_MSG_LE_SET_HIGH_PRIORITY:
        {
            //Todo
        }
        break;
#endif
    default:
        break;
    }
    return;
}

bool ble_bond_sync_cccd_merge(T_LE_BOND_ENTRY *p_entry, T_BT_LE_DEV_INFO *p_primary_dev,
                              T_BT_LE_DEV_INFO *p_secondary_dev)
{
    T_BT_CCCD_DATA *p_primary_cccd;
    T_BT_CCCD_DATA *p_secondary_cccd;
    T_BT_CCCD_DATA empty_cccd;
    T_BT_CCCD_ELEM *p_merge_elem = NULL;
    uint8_t merge_elem_cnt = 0;
    T_BT_CCCD_ELEM *p_secondary_elem = NULL;
    uint8_t secondary_elem_cnt = 0;
    memset(&empty_cccd, 0, sizeof(T_BT_CCCD_DATA));
    if (p_secondary_dev->cccd_len == 0)
    {
        p_secondary_cccd = &empty_cccd;
    }
    else
    {
        p_secondary_cccd = (T_BT_CCCD_DATA *)p_secondary_dev->cccd;
        p_secondary_elem = (T_BT_CCCD_ELEM *)p_secondary_cccd->data;
        secondary_elem_cnt = p_secondary_cccd->data_length / sizeof(T_BT_CCCD_ELEM);
    }
    if (p_primary_dev->cccd_len == 0)
    {
        p_primary_cccd = &empty_cccd;
    }
    else
    {
        p_primary_cccd = (T_BT_CCCD_DATA *)p_primary_dev->cccd;
        merge_elem_cnt = p_primary_cccd->data_length / sizeof(T_BT_CCCD_ELEM);
    }

    T_BT_CCCD_DATA *p_cccd_data = calloc(1,
                                         4 + p_primary_cccd->data_length + p_secondary_cccd->data_length);

    if (p_cccd_data == NULL)
    {
        return false;
    }
    p_cccd_data->data_length = p_primary_cccd->data_length;
    if (p_primary_cccd->data_length)
    {
        memcpy(p_cccd_data->data, p_primary_cccd->data, p_primary_cccd->data_length);
    }
    p_merge_elem = (T_BT_CCCD_ELEM *)p_cccd_data->data;
    for (uint8_t i = 0; i < secondary_elem_cnt; i++)
    {
        uint8_t j;
        for (j = 0; j < merge_elem_cnt; j++)
        {
            if (p_secondary_elem[i].handle == p_merge_elem[j].handle)
            {
                if (p_secondary_elem[i].ccc_bits != p_merge_elem[j].ccc_bits)
                {
                    p_merge_elem[j].ccc_bits |= p_secondary_elem[i].ccc_bits;
                }
                break;
            }
        }
        if (j == merge_elem_cnt && p_secondary_elem[i].ccc_bits != 0)
        {
            p_merge_elem[j].handle = p_secondary_elem[i].handle;
            p_merge_elem[j].ccc_bits = p_secondary_elem[i].ccc_bits;
            merge_elem_cnt++;
            p_cccd_data->data_length += 4;
        }
    }
    bt_le_dev_info_set_cccd(p_entry, p_cccd_data);
    free(p_cccd_data);
    return true;
}

void ble_bond_sync_handle_bond_update(bool is_add, T_BT_LE_DEV_DATA *p_dev_info)
{
    T_LE_BOND_ENTRY *p_entry = bt_le_dev_info_find(&p_dev_info->bond_info);
    if (p_entry)
    {
        if (p_entry->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG)
        {
            T_BT_LE_DEV_DATA *p_local_info = NULL;
            uint16_t info_len;
            bool is_equal = false;
            if (bt_le_dev_info_get(p_entry, &p_local_info, &info_len) == false)
            {
                return;
            }
            if (p_local_info == NULL)
            {
                return;
            }
            is_equal = bt_le_dev_info_cmp(p_local_info, p_dev_info);
            if (p_dev_info->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG)
            {
                if (is_equal)
                {
                    bt_le_clear_bond_state(p_entry);
                }
                else
                {
                    if (memcmp(p_local_info->dev_info.local_ltk, p_dev_info->dev_info.local_ltk, 32) != 0)
                    {
                        if ((p_dev_info->bond_info.bond_state & BT_BOND_INFO_REPAIR_FLAG) == 0 &&
                            (p_local_info->bond_info.bond_state & BT_BOND_INFO_REPAIR_FLAG) == 0)
                        {
                            bt_le_dev_info_remove(&p_local_info->bond_info);
                        }
                        if ((p_dev_info->bond_info.bond_state & BT_BOND_INFO_REPAIR_FLAG) != 0 &&
                            (p_local_info->bond_info.bond_state & BT_BOND_INFO_REPAIR_FLAG) == 0)
                        {
                            p_entry = bt_le_dev_info_set(&p_dev_info->bond_info, &p_dev_info->dev_info);
                            if (p_entry)
                            {
                                ble_bond_sync_send_update(p_entry);
                            }
                        }
                    }
                    else
                    {
                        if (p_local_info->dev_info.cccd_len != p_dev_info->dev_info.cccd_len ||
                            memcmp(p_local_info->dev_info.cccd, p_dev_info->dev_info.cccd,
                                   p_local_info->dev_info.cccd_len) != 0)
                        {
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
                                if (ble_bond_sync_cccd_merge(p_entry, &p_local_info->dev_info,
                                                             &p_dev_info->dev_info))
                                {
                                    ble_bond_sync_send_update(p_entry);
                                }
                            }
                            else
                            {
                                if (ble_bond_sync_cccd_merge(p_entry, &p_dev_info->dev_info,
                                                             &p_local_info->dev_info))
                                {
                                    ble_bond_sync_send_update(p_entry);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (is_equal)
                {
                    bt_le_clear_bond_state(p_entry);
                }
            }
            if (p_local_info)
            {
                free(p_local_info);
            }
        }
        else
        {
            if (p_dev_info->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG)
            {
                p_entry = bt_le_dev_info_set(&p_dev_info->bond_info, &p_dev_info->dev_info);
                if (p_entry)
                {
                    ble_bond_sync_send_update(p_entry);
                }
            }
        }
    }
    else
    {
        p_entry = bt_le_dev_info_set(&p_dev_info->bond_info, &p_dev_info->dev_info);
        if (p_entry)
        {
            ble_bond_sync_send_update(p_entry);
        }
    }
}

T_BT_BOND_MERGE *ble_bond_sync_get_local_info(void)
{
    T_BT_BOND_MERGE *p_info = NULL;
    uint8_t err_idx = 0;
    uint8_t local_bond_num = bt_le_get_bond_dev_num();

    if (local_bond_num > 0)
    {
        p_info = calloc(1, sizeof(*p_info) + local_bond_num * sizeof(T_BT_BOND_RECORD));
        if (p_info == NULL)
        {
            err_idx = 2;
            goto failed;
        }
        p_info->bond_num = local_bond_num;
        p_info->total_bond_num = local_bond_num;

        for (uint32_t i = 0; i < local_bond_num; i++)
        {
            T_LE_BOND_ENTRY *p_entry = NULL;
            p_entry = bt_le_find_key_entry_by_priority(i + 1);
            if (p_entry == NULL)
            {
                err_idx = 3;
                goto failed;
            }
            memcpy(&p_info->bond_tbl[i].bond_info, &p_entry->bond_info, sizeof(T_BT_BOND_INFO));
            bt_le_dev_info_get_local_ltk(p_entry, p_info->bond_tbl[i].key);
        }
    }
    else
    {
        p_info = calloc(1, sizeof(*p_info));
        if (p_info == NULL)
        {
            err_idx = 4;
            goto failed;
        }
        p_info->bond_num = 0;
        p_info->total_bond_num = 0;
    }
    return p_info;
failed:
    if (p_info)
    {
        free(p_info);
    }
    if (err_idx != 0)
    {
        APP_PRINT_ERROR1("[BTBond] ble_bond_sync_get_local_info: failed, err_idx %d", err_idx);
    }
    return NULL;
}

T_BT_BOND_RECORD *ble_bond_sync_find_bond_info(T_BT_BOND_MERGE *p_info, T_BT_BOND_INFO *p_bond_info)
{
    for (uint8_t i = 0; i < p_info->total_bond_num; i++)
    {
        if (p_info->bond_tbl[i].bond_info.remote_bd_type == p_bond_info->remote_bd_type &&
            p_info->bond_tbl[i].bond_info.local_bd_type == p_bond_info->local_bd_type &&
            memcmp(p_info->bond_tbl[i].bond_info.remote_bd, p_bond_info->remote_bd, 6) == 0 &&
            memcmp(p_info->bond_tbl[i].bond_info.local_bd, p_bond_info->local_bd, 6) == 0
           )
        {
            return &p_info->bond_tbl[i];
        }
    }
    return NULL;
}

void ble_bond_sync_handle_merge_info(T_BT_BOND_MERGE *p_info)
{
    T_BT_BOND_MERGE *p_local_info = NULL;
    T_BT_BOND_MERGE *p_merged_info = NULL;
    uint8_t i;
    uint8_t max_bond_num = bt_le_get_max_le_paired_device_num();
    T_BT_BOND_MERGE *p_prim_info = NULL;
    T_BT_BOND_MERGE *p_secondary_info = NULL;
    if (p_info->status == BT_BOND_DATA_STATUS_MORE)
    {
        if (scan_mgr.p_bond_mgr)
        {
            if (scan_mgr.p_bond_mgr->status == BT_BOND_DATA_STATUS_COMPLETE)
            {
                free(scan_mgr.p_bond_mgr);
                scan_mgr.p_bond_mgr = NULL;
            }
            else
            {
                if (scan_mgr.p_bond_mgr->bond_num + p_info->bond_num >= p_info->total_bond_num)
                {
                    goto failed;
                }
            }
        }
        if (scan_mgr.p_bond_mgr == NULL)
        {
            scan_mgr.p_bond_mgr = calloc(1,
                                         sizeof(*p_info) + p_info->total_bond_num * sizeof(T_BT_BOND_RECORD));
            if (scan_mgr.p_bond_mgr)
            {
                scan_mgr.p_bond_mgr->total_bond_num = p_info->total_bond_num;
            }
            else
            {
                goto failed;
            }
        }
        scan_mgr.p_bond_mgr->status = BT_BOND_DATA_STATUS_MORE;
        memcpy(&scan_mgr.p_bond_mgr->bond_tbl[scan_mgr.p_bond_mgr->bond_num], p_info->bond_tbl,
               p_info->bond_num * sizeof(T_BT_BOND_RECORD));
        scan_mgr.p_bond_mgr->bond_num += p_info->bond_num;
        goto ret;
    }
    else
    {
        if (p_info->bond_num != p_info->total_bond_num)
        {
            if (scan_mgr.p_bond_mgr)
            {
                if (scan_mgr.p_bond_mgr->bond_num + p_info->bond_num != p_info->total_bond_num)
                {
                    goto failed;
                }
                scan_mgr.p_bond_mgr->status = BT_BOND_DATA_STATUS_COMPLETE;
                memcpy(&scan_mgr.p_bond_mgr->bond_tbl[scan_mgr.p_bond_mgr->bond_num], p_info->bond_tbl,
                       p_info->bond_num * sizeof(T_BT_BOND_RECORD));
                scan_mgr.p_bond_mgr->bond_num += p_info->bond_num;
                p_info = scan_mgr.p_bond_mgr;
            }
            else
            {
                goto failed;
            }
        }
    }
#if F_APP_BOND_MGR_DEBUG
    for (i = 0; i < p_info->total_bond_num; i++)
    {
        APP_PRINT_TRACE6("[BTBond] remote queue[%d]: bond_state 0x%x, remote bd 0x%x [%s], local bd 0x%x [%s]",
                         i, p_info->bond_tbl[i].bond_info.bond_state,
                         p_info->bond_tbl[i].bond_info.remote_bd_type,
                         TRACE_BDADDR(p_info->bond_tbl[i].bond_info.remote_bd),
                         p_info->bond_tbl[i].bond_info.local_bd_type,
                         TRACE_BDADDR(p_info->bond_tbl[i].bond_info.local_bd));
    }
#endif
    p_local_info = ble_bond_sync_get_local_info();
    if (p_local_info == NULL)
    {
        goto failed;
    }
#if F_APP_BOND_MGR_DEBUG
    for (i = 0; i < p_local_info->total_bond_num; i++)
    {
        APP_PRINT_TRACE6("[BTBond] local queue[%d]: bond_state 0x%x, remote bd 0x%x [%s], local bd 0x%x [%s]",
                         i, p_local_info->bond_tbl[i].bond_info.bond_state,
                         p_local_info->bond_tbl[i].bond_info.remote_bd_type,
                         TRACE_BDADDR(p_local_info->bond_tbl[i].bond_info.remote_bd),
                         p_local_info->bond_tbl[i].bond_info.local_bd_type,
                         TRACE_BDADDR(p_local_info->bond_tbl[i].bond_info.local_bd));
    }
#endif
    if (p_info->total_bond_num == p_local_info->total_bond_num)
    {
        for (i = 0; i < p_info->total_bond_num; i++)
        {
            if (p_info->bond_tbl[i].bond_info.remote_bd_type !=
                p_local_info->bond_tbl[i].bond_info.remote_bd_type ||
                p_info->bond_tbl[i].bond_info.local_bd_type != p_local_info->bond_tbl[i].bond_info.local_bd_type ||
                memcmp(p_info->bond_tbl[i].bond_info.remote_bd, p_local_info->bond_tbl[i].bond_info.remote_bd,
                       6) != 0 ||
                memcmp(p_info->bond_tbl[i].bond_info.local_bd, p_local_info->bond_tbl[i].bond_info.local_bd, 6) != 0
               )
            {
                break;
            }
        }
        if (i == p_info->total_bond_num)
        {
            scan_mgr.state = SYNC_MERGE_INFO_RECEIVED;
            APP_PRINT_TRACE0("[BTBond] ble_bond_sync_handle_merge_info: bond info is equal");
            goto completed;
        }
    }
    p_merged_info = calloc(1, sizeof(*p_info) + max_bond_num * sizeof(T_BT_BOND_RECORD));
    if (p_merged_info == NULL)
    {
        goto failed;
    }
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        p_prim_info = p_info;
        p_secondary_info = p_local_info;
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        p_prim_info = p_local_info;
        p_secondary_info = p_info;
    }
    else
    {
        goto failed;
    }
    for (i = 0; i < p_prim_info->total_bond_num; i++)
    {
        T_BT_BOND_RECORD *p_primary_record = &p_prim_info->bond_tbl[i];
        T_BT_BOND_RECORD *p_secondary_record = ble_bond_sync_find_bond_info(p_secondary_info,
                                                                            &p_primary_record->bond_info);
        if (p_secondary_record)
        {
            p_secondary_record->bond_info.bond_state |= BT_BOND_INFO_HANDLED;
            if ((p_secondary_record->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG) &&
                (p_primary_record->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG))
            {
                if (memcmp(p_secondary_record->key, p_primary_record->key, 32) != 0)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
                        (p_primary_record->bond_info.bond_state & BT_BOND_INFO_REPAIR_FLAG) == 0)
                    {
                        bt_le_dev_info_remove(&p_primary_record->bond_info);
                        continue;
                    }
                    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY &&
                             (p_secondary_record->bond_info.bond_state & BT_BOND_INFO_REPAIR_FLAG) == 0)
                    {
                        bt_le_dev_info_remove(&p_secondary_record->bond_info);
                        continue;
                    }
                }
            }
            memcpy(&p_merged_info->bond_tbl[p_merged_info->bond_num], p_primary_record,
                   sizeof(T_BT_BOND_RECORD));
            p_merged_info->bond_num++;
        }
        else
        {
            if (p_primary_record->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    p_primary_record->bond_info.bond_state |= BT_BOND_INFO_ADD;
                }
                memcpy(&p_merged_info->bond_tbl[p_merged_info->bond_num], p_primary_record,
                       sizeof(T_BT_BOND_RECORD));
                p_merged_info->bond_num++;
            }
            else
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    bt_le_dev_info_remove(&p_primary_record->bond_info);
                }
            }
        }
    }
    for (i = 0; i < p_secondary_info->total_bond_num; i++)
    {
        T_BT_BOND_RECORD *p_secondary_record = &p_secondary_info->bond_tbl[i];
        if ((p_secondary_record->bond_info.bond_state & BT_BOND_INFO_HANDLED) == 0)
        {
            if (p_secondary_record->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG)
            {
                if (p_merged_info->bond_num < max_bond_num)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        p_secondary_record->bond_info.bond_state |= BT_BOND_INFO_ADD;
                    }
                    memcpy(&p_merged_info->bond_tbl[p_merged_info->bond_num], p_secondary_record,
                           sizeof(T_BT_BOND_RECORD));
                    p_merged_info->bond_num++;
                }
            }
            else
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    bt_le_dev_info_remove(&p_secondary_record->bond_info);
                }
            }
        }
    }
#if F_APP_BOND_MGR_DEBUG
    for (i = 0; i < p_merged_info->bond_num; i++)
    {
        APP_PRINT_TRACE6("[BTBond] merge queue[%d]: bond_state 0x%x, remote bd 0x%x [%s], local bd 0x%x [%s]",
                         i, p_merged_info->bond_tbl[i].bond_info.bond_state,
                         p_merged_info->bond_tbl[i].bond_info.remote_bd_type,
                         TRACE_BDADDR(p_merged_info->bond_tbl[i].bond_info.remote_bd),
                         p_merged_info->bond_tbl[i].bond_info.local_bd_type,
                         TRACE_BDADDR(p_merged_info->bond_tbl[i].bond_info.local_bd));
    }
#endif
    for (i = p_merged_info->bond_num; i > 0; i--)
    {
        T_BT_BOND_RECORD *p_merge_record = &p_merged_info->bond_tbl[i - 1];
        if (p_merge_record->bond_info.bond_state & BT_BOND_INFO_ADD)
        {
            bt_le_dev_info_pre_set(&p_merge_record->bond_info, p_merge_record->key);
        }
        else
        {
            bt_le_dev_info_set_high_priority(&p_merge_record->bond_info);
        }
    }
    scan_mgr.state = SYNC_MERGE_INFO_RECEIVED;
completed:
    if (p_local_info)
    {
        free(p_local_info);
    }
    if (p_merged_info)
    {
        free(p_merged_info);
    }
    if (scan_mgr.p_bond_mgr)
    {
        free(scan_mgr.p_bond_mgr);
        scan_mgr.p_bond_mgr = NULL;
    }
    return;
ret:
    return;
failed:
    if (p_local_info)
    {
        free(p_local_info);
    }
    if (p_merged_info)
    {
        free(p_merged_info);
    }
    if (scan_mgr.p_bond_mgr)
    {
        free(scan_mgr.p_bond_mgr);
        scan_mgr.p_bond_mgr = NULL;
    }
    return;
}

void ble_bond_sync_handle_remote_msg(T_APP_BT_BOND_REMOTE_MSG msg_type, uint8_t *buf, uint16_t len,
                                     T_REMOTE_RELAY_STATUS status)
{
    T_BT_BOND_REMOTE_INFO *p_info = NULL;
    uint8_t *aligned_buf = NULL;

    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        goto failed;
    }


    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        goto failed;
    }

    if (buf == NULL)
    {
        goto failed;
    }

    if ((uint32_t)buf & 0b11)
    {
        aligned_buf = malloc(len);
        if (aligned_buf == NULL)
        {
            goto failed;
        }

        memcpy(aligned_buf, buf, len);
        p_info = (T_BT_BOND_REMOTE_INFO *)aligned_buf;
    }
    else
    {
        p_info = (T_BT_BOND_REMOTE_INFO *)buf;
    }

    switch (msg_type)
    {
    case APP_BT_BOND_REMOVE:
        {
            APP_PRINT_INFO0("[BTBond] APP_BT_BOND_REMOVE");
            bt_le_dev_info_remove(&p_info->bond_info);
        }
        break;

    case APP_BT_BOND_ADD:
        {
            APP_PRINT_INFO0("[BTBond] APP_BT_BOND_ADD");
            if (len != (sizeof(T_BT_LE_DEV_DATA) + p_info->bond_add.dev_info.cccd_len))
            {
                goto failed;
            }
            ble_bond_sync_handle_bond_update(true, &p_info->bond_add);
        }
        break;

    case APP_BT_BOND_CLEAR:
        {
            APP_PRINT_INFO0("[BTBond] APP_BT_BOND_CLEAR");
            bt_le_dev_info_clear();
        }
        break;

    case APP_BT_BOND_SET_HIGH_PRIORITY:
        {
            //APP_PRINT_INFO0("[BTBond] APP_BT_BOND_SET_HIGH_PRIORITY");
            //bt_le_dev_info_set_high_priority(&p_info->bond_info);
        }
        break;

    case APP_BT_BOND_UPDATE:
        {
            APP_PRINT_INFO0("[BTBond] APP_BT_BOND_UPDATE");
            ble_bond_sync_handle_bond_update(false, &p_info->bond_update);
        }
        break;

    case APP_BT_BOND_MERGE:
        {
            APP_PRINT_INFO0("[BTBond] APP_BT_BOND_MERGE");
            if (scan_mgr.state == SYNC_IDLE)
            {
                ble_bond_sync_send_merge_info();
            }
            ble_bond_sync_handle_merge_info(&p_info->bond_merge);
            if (scan_mgr.state == SYNC_MERGE_INFO_RECEIVED)
            {
                T_LE_BOND_ENTRY *p_entry = NULL;
                uint8_t bond_num = bt_le_get_bond_dev_num();
                for (uint8_t i = 1; i <= bond_num; i++)
                {
                    p_entry = bt_le_find_key_entry_by_priority(i);
                    if (p_entry && (p_entry->bond_info.bond_state & BT_BOND_INFO_NEED_UPDATE_FLAG))
                    {
                        ble_bond_sync_send_update(p_entry);
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    if (aligned_buf)
    {
        free(aligned_buf);
    }
    return;
failed:
    if (aligned_buf)
    {
        free(aligned_buf);
    }
    APP_PRINT_ERROR0("[BTBond] ble_bond_sync_handle_remote_msg: failed");
    return;
}

void ble_bond_sync_handle_remote_conn_cmpl(void)
{
    scan_mgr.remote_connected = true;
    if (scan_mgr.state == SYNC_IDLE)
    {
        ble_bond_sync_send_merge_info();
    }
}

void ble_bond_sync_handle_disconn_cmpl(void)
{
    scan_mgr.remote_connected = false;
    ble_bond_sync_recovery();
}

void ble_bond_sync_init(void)
{
    ble_bond_sync_recovery();
#if F_APP_ERWS_SUPPORT
    app_timer_reg_cb(ble_bond_sync_timeout_cb, &scan_mgr.ble_bond_sync_timer_module_id);
    bt_bond_register_app_cb(ble_bond_sync_cb);
    app_relay_cback_register(NULL, (P_APP_PARSE_CBACK) ble_bond_sync_handle_remote_msg,
                             APP_MODULE_TYPE_BLE_BOND, APP_BT_BOND_EVT_MAX);
#endif
}
#endif
