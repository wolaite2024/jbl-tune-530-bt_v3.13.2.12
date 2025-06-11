/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "os_mem.h"
#include "trace.h"
#include "gap_le_types.h"
#include "ble_ext_adv.h"
#include "app_cfg.h"
#include "app_main.h"

#define HARMAN_SWIFT_PAIR_TX_POWER_SET          1

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
#include "string.h"
#include "bt_types.h"
#include "app_timer.h"
#include "app_ble_swift_pair.h"
#include "app_adv_stop_cause.h"
#include "app_ble_rand_addr_mgr.h"
uint8_t swift_pair_adv_handle = 0xff;
static T_BLE_EXT_ADV_MGR_STATE swift_pair_adv_state = BLE_EXT_ADV_MGR_ADV_DISABLED;
static uint32_t appearance_major = MAJOR_DEVICE_CLASS_AUDIO;
static uint32_t appearance_minor = MINOR_DEVICE_CLASS_HEADSET;
static uint8_t swift_pair_verdon_section_length = 7;
static uint8_t swift_pair_tx_power_section_length = 3;
static uint8_t app_ble_swift_pair_timer_id = 0;
static uint8_t timer_idx_swift_pair_change_adv_data = 0;

static uint8_t swift_pair_adv_data[31] =
{
    /* TX Power Level */
    0x02,/* length */
    GAP_ADTYPE_POWER_LEVEL,
    0xfa,

    /* verdon section */
    0x00,       /* length, when get all data, count length */
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
    0x06,/*mocrosoftVendor ID*/
    0x00,/*mocrosoftVendor ID*/
    0x03,/*mocrosoftVendor ID*/

    0x01,/*microsoft beacon sub scenario*/
    0x80,/*reserved rssi byte*/
};

static void app_swift_pair_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));
    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            swift_pair_adv_state = cb_data.p_ble_state_change->state;
            if (swift_pair_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                APP_PRINT_TRACE1("app_swift_pair_adv_callback: BLE_EXT_ADV_MGR_ADV_ENABLED, adv_handle %d",
                                 cb_data.p_ble_state_change->adv_handle);
            }
            else if (swift_pair_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                APP_PRINT_TRACE1("app_swift_pair_adv_callback: BLE_EXT_ADV_MGR_ADV_DISABLED, adv_handle %d",
                                 cb_data.p_ble_state_change->adv_handle);
                switch (cb_data.p_ble_state_change->stop_cause)
                {
                case BLE_EXT_ADV_STOP_CAUSE_APP:
                    APP_PRINT_TRACE1("app_swift_pair_adv_callback: BLE_EXT_ADV_STOP_CAUSE_APP app_cause 0x%02x",
                                     cb_data.p_ble_state_change->app_cause);
                    break;

                case BLE_EXT_ADV_STOP_CAUSE_CONN:
                    APP_PRINT_TRACE0("app_swift_pair_adv_callback: BLE_EXT_ADV_STOP_CAUSE_CONN");
                    break;
                case BLE_EXT_ADV_STOP_CAUSE_TIMEOUT:
                    APP_PRINT_TRACE0("app_swift_pair_adv_callback: BLE_EXT_ADV_STOP_CAUSE_TIMEOUT");
                    break;
                default:
                    APP_PRINT_TRACE1("app_swift_pair_adv_callback: stop_cause %d",
                                     cb_data.p_ble_state_change->stop_cause);
                    break;
                }
            }
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        APP_PRINT_TRACE1("app_swift_pair_adv_callback: BLE_EXT_ADV_SET_CONN_INFO conn_id 0x%x",
                         cb_data.p_ble_conn_info->conn_id);
        swift_pair_adv_state = ble_ext_adv_mgr_get_adv_state(swift_pair_adv_handle);
        break;

    default:
        break;
    }
    return;
}

//start action must be after app_swift_pair_handle_power_on
bool app_swift_pair_adv_start(uint16_t duration_10ms)
{
    if (swift_pair_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (ble_ext_adv_mgr_enable(swift_pair_adv_handle, duration_10ms) == GAP_CAUSE_SUCCESS)
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
        APP_PRINT_TRACE0("app_swift_pair_adv_start: Already started");
        return true;
    }
}

bool app_swift_pair_adv_stop(int8_t app_cause)
{
    if (ble_ext_adv_mgr_disable(swift_pair_adv_handle, app_cause) == GAP_CAUSE_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void app_swift_pair_adv_init(void)
{
    /* set adv parameter */
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop =
        LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED;
    uint16_t adv_interval_min = 0x20;
    uint16_t adv_interval_max = 0x20;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    uint8_t name_len;
    uint32_t icon = (appearance_major | appearance_minor);

    /* set adv data*/
    /* put bd_addr in adv data*/
    memcpy(&swift_pair_adv_data[swift_pair_tx_power_section_length + swift_pair_verdon_section_length],
           app_cfg_nv.bud_local_addr, 6);
    swift_pair_verdon_section_length += 6;

    /* put appearance in adv data*/
    memcpy(&swift_pair_adv_data[swift_pair_tx_power_section_length + swift_pair_verdon_section_length],
           &icon, 3);
    swift_pair_verdon_section_length += 3;

#if HARMAN_SWIFT_PAIR_TX_POWER_SET
#if 0
    int8_t swift_pair_tx_power = extend_app_cfg_const.gfps_tx_power;
#else
    int8_t swift_pair_tx_power = -4;
#endif

    swift_pair_adv_data[2] = (uint8_t)swift_pair_tx_power;

    APP_PRINT_TRACE2("swift_pair_adv_init: swift_pair_adv_data[2]: 0x%x, 0x%x", swift_pair_adv_data[2],
                     extend_app_cfg_const.gfps_tx_power);

    /* put device legacy name in adv data*/
    name_len = strlen((const char *)app_cfg_nv.device_name_legacy);
    if (name_len > (31 - swift_pair_tx_power_section_length - swift_pair_verdon_section_length))
    {
        name_len = 31 - swift_pair_tx_power_section_length - swift_pair_verdon_section_length;
    }

    memcpy(&swift_pair_adv_data[swift_pair_tx_power_section_length + swift_pair_verdon_section_length],
           app_cfg_nv.device_name_legacy, name_len);
    swift_pair_verdon_section_length += name_len;
#endif
    swift_pair_adv_data[swift_pair_tx_power_section_length] = swift_pair_verdon_section_length - 1;
    /* modify length field of adv data*/

    uint8_t random_addr[6] = {0};
    app_ble_rand_addr_get(random_addr);

    APP_PRINT_TRACE2("swift_pair_adv_init: swift_pair_adv_data %b, le_random_addr %s",
                     TRACE_BINARY(sizeof(swift_pair_adv_data), swift_pair_adv_data), TRACE_BDADDR(random_addr));

    /* build new adv*/
    ble_ext_adv_mgr_init_adv_params(&swift_pair_adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, swift_pair_verdon_section_length + swift_pair_tx_power_section_length,
                                    swift_pair_adv_data,
                                    0, NULL, random_addr);

    /* set adv event handle callback*/
    ble_ext_adv_mgr_register_callback(app_swift_pair_adv_callback, swift_pair_adv_handle);
}

void app_swift_pair_start_adv_change_timer(uint16_t timeout)
{
    app_start_timer(&timer_idx_swift_pair_change_adv_data, "swift_pair_change_adv_data",
                    app_ble_swift_pair_timer_id, APP_TIMER_SWIFT_PAIR_CHANGE_ADV_DATA, 0, false,
                    timeout * 1000);
}

void app_swift_pair_stop_adv_change_timer(void)
{
    app_stop_timer(&timer_idx_swift_pair_change_adv_data);
}

void app_swift_pair_handle_power_on(int16_t duration_10ms)
{
    app_swift_pair_adv_init();
}

void app_swift_pair_handle_power_off(void)
{
    app_swift_pair_stop_adv_change_timer();
    app_swift_pair_adv_stop(APP_STOP_ADV_CAUSE_POWER_OFF);
}

static void app_swift_pair_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_swift_pair_timeout_cb: timer_evt %d, param %d",
                     timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_SWIFT_PAIR_CHANGE_ADV_DATA:
        {
            ble_ext_adv_mgr_set_adv_data(swift_pair_adv_handle, 7, swift_pair_adv_data);

        }
        break;
    default:
        break;
    }
}

void app_swift_pair_init(void)
{
    app_timer_reg_cb(app_swift_pair_timeout_cb, &app_ble_swift_pair_timer_id);
}

#endif

