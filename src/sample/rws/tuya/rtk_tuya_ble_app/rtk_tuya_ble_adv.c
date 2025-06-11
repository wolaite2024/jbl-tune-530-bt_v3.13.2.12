/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_adv.h"
#include "rtk_tuya_ble_service.h"
#include "string.h"
#include "trace.h"
#include "app_cfg.h"
#include "rtk_tuya_ble_device.h"
#include "app_ble_rand_addr_mgr.h"

static const uint8_t tuya_adv_data[31] =
{
    0x02,
    0x01,
    0x06,
    0x03,
    0x02,
    0x50, 0xFD,
    0x17,
    0x16,
    0x50, 0xFD,
    0x41, 0x00,//Frame Control
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t tuya_scan_rsp_data[31] =
{
    0x17,// length
    0xFF,
    0xD0,
    0x07,
    0x00, //Encry Mode(8)
    0x00, 0x00, //communication way bit0-mesh bit1-wifi bit2-zigbee bit3-NB
    0x00, //FLAG
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, //24
    0x09,
    'D', 'D' //0x54, 0x59,
};

typedef struct tuya_adv_struct
{
    uint8_t adv_handle;
    uint8_t conn_id;
    T_BLE_EXT_ADV_MGR_STATE state;
} T_LE_TUYA_ADV;

static T_LE_TUYA_ADV le_tuya_adv =
{
    .adv_handle = 0xFF,
    .conn_id = 0xFF,
    .state = BLE_EXT_ADV_MGR_ADV_DISABLED,
};

#if 0
typedef struct TUYA_ADV_DATA_FRAME_CONTROL
{
    uint16_t reserved : 7;
    //bit[7]: if bit[7] = 1 it means include dp data, if bit[7] = 0 it means not include dp data
    uint16_t dp_data_include : 1;
    //bit[8]: if bit[8] = 1 it means include id field, if bit[8] = 0 it means not include id field
    uint16_t id_include : 1;
    //bit[9]: if bit[9] = 1 it means request connection, if bit[9] = 0 it means not request connection
    uint16_t request_connection : 1;
    //bit[10]: if bit[10] = 1 it means shared device, if bit[10] = 0 it means not shared device
    uint16_t shared_device_flag : 1;
    //bit[11]: if bit[11] = 1 it means already bonded device, if bit[11] = 0 it means not bonded device
    uint16_t bond_flag : 1;
    //bit[12]-bit[15]:version
    uint16_t version : 4;
} T_TUYA_ADV_DATA_FRAME_CONTROL;

static T_TUYA_ADV_DATA_FRAME_CONTROL frame_control;
#endif

static void le_tuya_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            le_tuya_adv.state = cb_data.p_ble_state_change->state;
            if (le_tuya_adv.state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                APP_PRINT_TRACE1("le_tuya_adv_callback: BLE_EXT_ADV_MGR_ADV_ENABLED, adv_handle %d",
                                 cb_data.p_ble_state_change->adv_handle);
            }
            else if (le_tuya_adv.state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                APP_PRINT_TRACE1("le_tuya_adv_callback: BLE_EXT_ADV_MGR_ADV_DISABLED, adv_handle %d",
                                 cb_data.p_ble_state_change->adv_handle);
                switch (cb_data.p_ble_state_change->stop_cause)
                {
                case BLE_EXT_ADV_STOP_CAUSE_APP:
                    APP_PRINT_TRACE1("le_tuya_adv_callback: BLE_EXT_ADV_STOP_CAUSE_APP app_cause 0x%02x",
                                     cb_data.p_ble_state_change->app_cause);
                    break;

                case BLE_EXT_ADV_STOP_CAUSE_CONN:
                    APP_PRINT_TRACE0("le_tuya_adv_callback: BLE_EXT_ADV_STOP_CAUSE_CONN and Report to TUYA");
                    if (extend_app_cfg_const.tuya_support)
                    {
                        app_tuya_handle_b2s_ble_connected(cb_data.p_ble_conn_info->conn_id);
                    }
                    break;
                case BLE_EXT_ADV_STOP_CAUSE_TIMEOUT:
                    APP_PRINT_TRACE0("le_tuya_adv_callback: BLE_EXT_ADV_STOP_CAUSE_TIMEOUT");
                    break;
                default:
                    APP_PRINT_TRACE1("le_tuya_adv_callback: stop_cause %d",
                                     cb_data.p_ble_state_change->stop_cause);
                    break;
                }
            }
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        APP_PRINT_TRACE1("le_tuya_adv_callback: BLE_EXT_ADV_SET_CONN_INFO conn_id 0x%x",
                         cb_data.p_ble_conn_info->conn_id);

        le_tuya_adv.conn_id = cb_data.p_ble_conn_info->conn_id;
        break;

    default:
        break;
    }
    return;
}

bool le_tuya_adv_start(uint16_t timeout_sec)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        APP_PRINT_TRACE0("le_tuya_adv_start: bud_role 2, should not start adv");
        return false;
    }

    if (le_tuya_adv.state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (ble_ext_adv_mgr_enable(le_tuya_adv.adv_handle, timeout_sec * 100) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        APP_PRINT_TRACE0("le_tuya_adv_start: Already started");
        return true;
    }
}

bool le_tuya_adv_stop(uint8_t app_cause)
{
    if (le_tuya_adv.state == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        if (ble_ext_adv_mgr_disable(le_tuya_adv.adv_handle, app_cause) == GAP_CAUSE_SUCCESS)
        {
            APP_PRINT_TRACE0("le_tuya_adv_stop");
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        APP_PRINT_TRACE0("le_tuya_adv_stop: Already stoped");
        return true;
    }
}

uint8_t le_tuya_adv_get_conn_id(void)
{
    return le_tuya_adv.conn_id;
}

void le_tuya_adv_reset_conn_id(void)
{
    le_tuya_adv.conn_id = 0xff;
}

uint8_t le_tuya_adv_get_adv_handle(void)
{
    return le_tuya_adv.adv_handle;
}

void le_tuya_adv_init(void)
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;
    uint16_t adv_interval_min = 0x20;
    uint16_t adv_interval_max = 0x20;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;

    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    uint8_t random_addr[6] = {0};
    app_ble_rand_addr_get(random_addr);

    ble_ext_adv_mgr_init_adv_params(&le_tuya_adv.adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, sizeof(tuya_adv_data), (uint8_t *)tuya_adv_data,
                                    sizeof(tuya_scan_rsp_data), (uint8_t *)tuya_scan_rsp_data,
                                    random_addr);

    APP_PRINT_TRACE2("le_tuya_adv_init: tuya_adv_data %b,random address %b",
                     TRACE_BINARY(sizeof(tuya_adv_data), tuya_adv_data), TRACE_BDADDR(random_addr));

    ble_ext_adv_mgr_register_callback(le_tuya_adv_callback, le_tuya_adv.adv_handle);
}

#endif
