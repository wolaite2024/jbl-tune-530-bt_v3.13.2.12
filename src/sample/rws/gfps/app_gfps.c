/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FEATURE_SUPPORT
#include "stdlib.h"
#include "trace.h"
#include "gap_br.h"
#include "gap_bond_le.h"
#include "bt_gfps.h"
#include "gfps.h"
#include "app_gfps_timer.h"
#include "ble_ext_adv.h"
#include "ble_conn.h"
#include "app_gfps.h"
#include "app_gfps_account_key.h"
#include "app_gfps_personalized_name.h"
#include "app_ble_gap.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_bt_policy_api.h"
#include "app_hfp.h"
#include "app_gfps_battery.h"
#include "app_gfps_msg.h"
#include "app_multilink.h"
#include "app_gfps_device.h"
#include "app_bond.h"
#include "app_gfps_sync.h"
#include "app_gfps_link.h"
#if GFPS_LE_DEVICE_SUPPORT
#include "cap.h"
#include "bt_bond_le.h"
#include "app_gfps_psm.h"
#include "app_lea_adv.h"
#include "app_lea_mgr.h"
#endif
#if GFPS_FINDER_SUPPORT
#include "gfps_find_my_device.h"
#include "app_gfps_finder.h"
#endif
#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#include "gfps_sass_conn_status.h"
#if GFPS_SASS_LEA_SUPPORT
#include "app_lea_unicast_audio.h"
#endif
#endif

T_GFPS_DB gfps_db;

uint8_t gfps_adv_len;
uint8_t gfps_adv_data[GAP_MAX_LEGACY_ADV_LEN];

void app_gfps_ble_conn_info_init(uint8_t conn_id);
static T_APP_RESULT app_gfps_cb(T_SERVER_ID service_id, void *p_data);
static void app_gfps_le_disconnect_cb(uint8_t conn_id, uint8_t local_disc_cause,
                                      uint16_t disc_cause);

#define GATT_UUID_CAS        0x1853
static uint8_t gfps_scan_rsp_data[] =
{
//    0x09,/* length */
//    GAP_ADTYPE_LOCAL_NAME_COMPLETE,/* type="device name" */
//    'G', 'F', 'P', 'S', '_', 'A', 'D', 'V',
    0x04,/* length */
    GAP_ADTYPE_SERVICE_DATA,/* type="service uuid" */
    LO_WORD(GATT_UUID_CAS),
    HI_WORD(GATT_UUID_CAS),
    0x00,
};

static uint8_t gfps_get_scan_rsp_data_len(void)
{
    uint8_t gfps_scan_rsp_data_len = 0;

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

        if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
             gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA))
        {
            gfps_scan_rsp_data_len = sizeof(gfps_scan_rsp_data);
        }
    }
#endif
    return gfps_scan_rsp_data_len;
}

void app_gfps_update_announcement(uint8_t announcement)
{
    APP_PRINT_INFO1("app_gfps_update_announcement: %d", announcement);
    gfps_scan_rsp_data[4] = announcement;
    ble_ext_adv_mgr_set_scan_response_data(gfps_db.gfps_adv_handle, sizeof(gfps_scan_rsp_data),
                                           gfps_scan_rsp_data);
}

/*google Fast pair initialize*/
void app_gfps_init(void)
{
    uint8_t sec_req_enable = false;
    bool is_tag = extend_app_cfg_const.gfps_device_type == GFPS_LOCATOR_TRACKER ? true : false;

    if (app_gfps_account_key_init(extend_app_cfg_const.gfps_account_key_num) == false)
    {
        goto error;
    }
#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        gfps_sass_conn_status_init();
    }
#endif

#if GFPS_PERSONALIZED_NAME_SUPPORT
    app_gfps_personalized_name_init();
#endif

    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(uint8_t), &sec_req_enable);
    if (gfps_init(extend_app_cfg_const.gfps_model_id, extend_app_cfg_const.gfps_public_key,
                  extend_app_cfg_const.gfps_private_key) == false)
    {
        goto error;
    }

#if GFPS_FINDER_SUPPORT
    gfps_set_finder_enable(extend_app_cfg_const.gfps_finder_support);
#endif

#if GFPS_SASS_SUPPORT
    gfps_set_sass_enable(extend_app_cfg_const.gfps_sass_support);
#endif

    gfps_set_tx_power(extend_app_cfg_const.gfps_enable_tx_power, extend_app_cfg_const.gfps_tx_power);
    app_gfps_battery_info_init();
    gfps_db.current_conn_id = 0xFF;

    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        T_APP_LE_LINK *le_link = &app_db.le_link[i];

        le_link->gfps_link.gfps_conn_id = 0xFF;
        le_link->gfps_link.gfps_conn_state = GAP_CONN_STATE_DISCONNECTED;
    }

    gfps_db.gfps_id = gfps_add_service(app_gfps_cb);

#if F_APP_ERWS_SUPPORT
    app_gfps_sync_relay_init();
#endif

    gfps_ecc_manager_malloc();
    app_gfps_link_priority_queue_init();

#if GFPS_LE_DEVICE_SUPPORT
    gfps_le_device_init(extend_app_cfg_const.gfps_le_device_support,
                        extend_app_cfg_const.gfps_le_device_mode, is_tag); // TODO: work around
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        app_gfps_psm_init();
    }
#endif

    gap_get_param(GAP_PARAM_BOND_IO_CAPABILITIES, &(gfps_db.additional_default_io_cap));
    gap_get_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, &(gfps_db.additional_default_auth_flags));

    app_gfps_timer_init();
    return;
error:
    APP_PRINT_ERROR0("app_gfps_init: failed");
}

void app_gfps_get_random_addr(uint8_t *random_bd)
{
    memcpy(random_bd, gfps_db.random_address, GAP_BD_ADDR_LEN);
}

//ysc start
extern bool bt_connected_flag;
//ysc end

bool app_gfps_adv_start(T_GFPS_ADV_MODE mode, bool show_ui)
{
    bool update_addr = true;
    uint8_t random_address[6] = {0};

    app_gfps_timer_stop_update_rpa();
    app_gfps_timer_start_update_rpa();

#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        //if (app_gfps_finder_provisoned() != true)
        //{
        //    update_addr = false;
        //}
        update_addr = false;
    }
#endif

    if (gfps_gen_adv_data(mode, gfps_adv_data, &gfps_adv_len, show_ui))
    {
        uint16_t interval = extend_app_cfg_const.gfps_discov_adv_interval;
        //ysc start
        if(bt_connected_flag)
        {
            interval = 400;
            APP_PRINT_TRACE2("ysc bt_connected_flag: %d,1 interval: %d", bt_connected_flag, interval);
        }
        //ysc end

        if (mode == NOT_DISCOVERABLE_MODE)
        {
            ble_ext_adv_mgr_change_own_address_type(gfps_db.gfps_adv_handle, GAP_LOCAL_ADDR_LE_RANDOM);
            interval = extend_app_cfg_const.gfps_not_discov_adv_interval;
            //ysc start
            if(bt_connected_flag)
            {
                interval = 400;
                APP_PRINT_TRACE2("ysc bt_connected_flag: %d,2 interval: %d", bt_connected_flag, interval);
            }
            //ysc end
        }
        else if (mode == DISCOVERABLE_MODE_WITH_MODEL_ID)
        {
            ble_ext_adv_mgr_change_own_address_type(gfps_db.gfps_adv_handle, GAP_LOCAL_ADDR_LE_PUBLIC);
        }

        ble_ext_adv_mgr_set_multi_param(gfps_db.gfps_adv_handle, NULL,
                                        interval, gfps_adv_len, gfps_adv_data, 0, NULL);
    }
    else
    {
        APP_PRINT_ERROR0("app_gfps_adv_start: gfps_gen_adv_data failed");
        return false;
    }

    if (gfps_db.gfps_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (update_addr && (gfps_get_pairing_status() == GFPS_PAIRING_STATUS_IDLE))
        {
            //if gfps pairing is ongoing, do not update random address
            le_gen_rand_addr(GAP_RAND_ADDR_RESOLVABLE, random_address);
            memcpy(gfps_db.random_address, random_address, GAP_BD_ADDR_LEN);
            /*should sent ble address to the Seeker when RFCOMM/LE_L2CAP connects and whenever the address is rotated*/
            app_gfps_msg_update_ble_addr();
            ble_ext_adv_mgr_set_random(gfps_db.gfps_adv_handle, random_address);
        }

        APP_PRINT_INFO1("app_gfps_adv_start: with RPA %s", TRACE_BDADDR(gfps_db.random_address));

        if (ble_ext_adv_mgr_enable(gfps_db.gfps_adv_handle, 0) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }
    return true;
}

T_GAP_CAUSE app_gfps_adv_update_adv_interval(uint32_t adv_interval)
{
    T_GAP_CAUSE result = GAP_CAUSE_SUCCESS;

    result = ble_ext_adv_mgr_change_adv_interval(gfps_db.gfps_adv_handle, adv_interval);
    APP_PRINT_TRACE2("app_gfps_adv_update_adv_interval: result %d, adv_interval %d", result,
                     adv_interval);
    return result;
}

void app_gfps_update_rpa(void)
{
    uint8_t random_address[6] = {0};
    le_gen_rand_addr(GAP_RAND_ADDR_RESOLVABLE, random_address);
    memcpy(gfps_db.random_address, random_address, GAP_BD_ADDR_LEN);
    app_gfps_msg_update_ble_addr();
    ble_ext_adv_mgr_set_random(gfps_db.gfps_adv_handle, random_address);
    APP_PRINT_INFO1("app_gfps_update_rpa: RPA %b", TRACE_BDADDR(gfps_db.random_address));
}

bool app_gfps_sec_adv_start(T_GFPS_ADV_MODE mode, bool show_ui, uint16_t duration_10ms)
{
    APP_PRINT_INFO1("app_gfps_sec_adv_start: duration_10ms %d", duration_10ms);
    uint8_t random_address[6] = {0};
    le_gen_rand_addr(GAP_RAND_ADDR_RESOLVABLE, random_address);

    if (gfps_gen_adv_data(mode, gfps_adv_data, &gfps_adv_len, show_ui))
    {
        uint16_t interval = extend_app_cfg_const.gfps_discov_adv_interval;

        memcpy(gfps_db.random_address, random_address, GAP_BD_ADDR_LEN);
        if (mode == DISCOVERABLE_MODE_WITH_MODEL_ID)
        {
            interval = extend_app_cfg_const.gfps_discov_adv_interval;
        }
        else if (mode == NOT_DISCOVERABLE_MODE)
        {
            interval = extend_app_cfg_const.gfps_not_discov_adv_interval;
        }
        ble_ext_adv_mgr_set_multi_param(gfps_db.gfps_adv_handle, random_address,
                                        interval, gfps_adv_len, gfps_adv_data, 0, NULL);
    }
    else
    {
        APP_PRINT_ERROR0("app_gfps_sec_adv_start: gfps_gen_adv_data failed");
        return false;
    }

    if (gfps_db.gfps_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (ble_ext_adv_mgr_enable(gfps_db.gfps_adv_handle, duration_10ms) == GAP_CAUSE_SUCCESS)
        {
            return true;
        }
    }
    return true;
}

bool app_gfps_next_action(T_GFPS_ACTION gfps_next_action)
{
    uint8_t link_num = app_gfps_link_find_gfps_link();

    APP_PRINT_TRACE4("app_gfps_next_action: gfps_curr_action %d, gfps_next_action %d, gfps_adv_state %d, link_num %d",
                     gfps_db.gfps_curr_action, gfps_next_action,
                     gfps_db.gfps_adv_state, link_num);

#if GFPS_LE_DEVICE_SUPPORT
#else
    if ((gfps_next_action != GFPS_ACTION_IDLE) && (link_num != 0))
    {
        return false;
    }
#endif

    gfps_db.gfps_curr_action = gfps_next_action;

    switch (gfps_db.gfps_curr_action)
    {
    case GFPS_ACTION_IDLE:
        {
            if (gfps_db.gfps_adv_state == BLE_EXT_ADV_MGR_ADV_ENABLED)
            {
                ble_ext_adv_mgr_disable(gfps_db.gfps_adv_handle, APP_STOP_ADV_CAUSE_GFPS_ACTION_IDLE);
            }
            else if (link_num != 0)
            {
                app_gfps_link_disconnect_all_gfps_link();
            }
        }
        break;

    case GFPS_ACTION_ADV_DISCOVERABLE_MODE_WITH_MODEL_ID:
        {
            app_gfps_adv_start(DISCOVERABLE_MODE_WITH_MODEL_ID, true);
        }
        break;

    case GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE:
        {
            app_gfps_adv_start(NOT_DISCOVERABLE_MODE, true);
            app_gfps_battery_info_report(GFPS_BATTERY_REPORT_ADV_START);
        }
        break;

    case GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI:
        {
#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                //app_gfps_sec_adv_start(NOT_DISCOVERABLE_MODE, false, 18000);
            }
            else
#endif
            {
                app_gfps_adv_start(NOT_DISCOVERABLE_MODE, false);
            }
        }
        break;

    default:
        break;
    }

    return true;
}

bool app_gfps_ble_conn_info_handle(uint8_t *remote_bd_addr, uint8_t remote_addr_type)
{
    bool receive_ble_link = true;

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        uint8_t link_num = app_gfps_link_find_gfps_link();
        bool addr_reslove_ret = false;
        uint8_t resolved_addr[6] = {0};
        uint8_t resolved_bd_type = 0xFF;

        if (link_num >= GFPS_LE_MAX_LINK_NUMBER)
        {
            receive_ble_link = false;
            return receive_ble_link;
        }

        if (remote_addr_type == GAP_REMOTE_ADDR_LE_RANDOM)
        {
#if F_APP_BOND_MGR_SUPPORT
            addr_reslove_ret = bt_le_resolve_random_address(remote_bd_addr, resolved_addr, &resolved_bd_type);
#else
            addr_reslove_ret = le_resolve_random_address(remote_bd_addr, resolved_addr, &resolved_bd_type);
#endif
        }

        if ((gfps_db.force_enter_pair_mode == false) && (addr_reslove_ret == true))
        {
            T_ACCOUNT_KEY *p_key_info = app_gfps_account_key_get_table();

            for (uint8_t i = 0; i < p_key_info->num; i++)
            {
                if (memcmp(resolved_addr, p_key_info->account_info[i].addr, 6) == 0)
                {
                    gfps_db.gfps_linkback_init = true;
                }
            }
        }
    }
#endif

    APP_PRINT_INFO1("app_gfps_ble_conn_info_handle: receive_ble_link %d", receive_ble_link);
    return receive_ble_link;
}

static void app_gfps_adv_callback(uint8_t cb_type, void *p_cb_data)
{
    T_BLE_EXT_ADV_CB_DATA cb_data;
    memcpy(&cb_data, p_cb_data, sizeof(T_BLE_EXT_ADV_CB_DATA));

    switch (cb_type)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            gfps_db.gfps_adv_state = cb_data.p_ble_state_change->state;

            if (cb_data.p_ble_state_change->state == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                gfps_db.gfps_curr_action = GFPS_ACTION_IDLE;
            }

            APP_PRINT_TRACE4("app_gfps_adv_callback: adv_state %d, adv_handle %d, stop_cause %d, app_cause 0x%02x",
                             cb_data.p_ble_state_change->state,
                             cb_data.p_ble_state_change->adv_handle,
                             cb_data.p_ble_state_change->stop_cause,
                             cb_data.p_ble_state_change->app_cause);
        }
        break;

    case BLE_EXT_ADV_SET_CONN_INFO:
        {
            APP_PRINT_TRACE4("app_gfps_adv_callback: BLE_EXT_ADV_SET_CONN_INFO conn_id 0x%x, adv_handle %d, local_addr_type %d, local_bd %s",
                             cb_data.p_ble_conn_info->conn_id,
                             cb_data.p_ble_conn_info->adv_handle,
                             cb_data.p_ble_conn_info->local_addr_type,
                             TRACE_BDADDR(cb_data.p_ble_conn_info->local_addr));

            app_link_reg_le_link_disc_cb(cb_data.p_ble_conn_info->conn_id, app_gfps_le_disconnect_cb);

#if GFPS_LE_DEVICE_SUPPORT
            if (extend_app_cfg_const.gfps_le_device_support)
            {
                if (cb_data.p_ble_conn_info->adv_handle == gfps_db.gfps_adv_handle)
                {
                    app_gfps_ble_conn_info_init(cb_data.p_ble_conn_info->conn_id);
                }
            }
            else
#endif
            {
                gfps_db.current_conn_id = cb_data.p_ble_conn_info->conn_id;
            }

        }
        break;

    default:
        break;
    }

    return;
}

void app_gfps_ble_conn_info_init(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(conn_id);

    if (p_link == NULL)
    {
        return;
    }

    p_link->gfps_link.gfps_conn_state = GAP_CONN_STATE_CONNECTED;
    p_link->gfps_link.gfps_conn_id = conn_id;
    app_gfps_link_add_link_into_priority_queue(p_link->gfps_link.gfps_conn_id);
    //gfps_db.gfps_curr_action = GFPS_ACTION_IDLE;
    gfps_db.current_conn_id = conn_id;

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        app_gfps_force_enter_pairing_mode(GFPS_EXIT_PAIR_MODE);
    }
#endif
}

void app_gfps_linkback_info_init(uint8_t conn_id)
{
    if (gfps_db.gfps_linkback_init == true)
    {
        app_gfps_ble_conn_info_init(conn_id);
        gfps_db.gfps_linkback_init = false;

#if GFPS_LE_DEVICE_SUPPORT
        //need to start adv for LE audio multilink reconnect
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            T_ACCOUNT_KEY *p_key_info = app_gfps_account_key_get_table();

            if (p_key_info->num != 0)
            {
                app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
            }
        }
#endif
    }

    // app_gfps_set_ble_conn_param(conn_id);
}

void app_gfps_adv_init(void)
{
    bool ret = false;
    uint8_t random_address[6] = {0};
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_RANDOM;
    uint16_t adv_interval_min = extend_app_cfg_const.gfps_discov_adv_interval;
    uint16_t adv_interval_max = extend_app_cfg_const.gfps_discov_adv_interval;
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop = LE_EXT_ADV_LEGACY_ADV_CONN_SCAN_UNDIRECTED;

    T_GAP_CAUSE cause = le_gen_rand_addr(GAP_RAND_ADDR_RESOLVABLE, random_address);
    memcpy(gfps_db.random_address, random_address, GAP_BD_ADDR_LEN);

    ble_ext_adv_mgr_init_adv_params(&gfps_db.gfps_adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type,
                                    peer_address, filter_policy, 0, NULL,
                                    gfps_get_scan_rsp_data_len(), gfps_scan_rsp_data, random_address);

    ret = ble_ext_adv_mgr_register_callback(app_gfps_adv_callback, gfps_db.gfps_adv_handle);

    APP_PRINT_INFO4("app_gfps_adv_init: gfps_adv_handle %d, cause %d, ret %d, gfps version %b",
                    gfps_db.gfps_adv_handle, cause, ret, TRACE_STRING(extend_app_cfg_const.gfps_version));
}

static void app_gfps_le_disconnect_cb(uint8_t conn_id, uint8_t local_disc_cause,
                                      uint16_t disc_cause)
{
    APP_PRINT_TRACE3("app_gfps_le_disconnect_cb: conn_id %d, local_disc_cause %d, disc_cause 0x%x",
                     conn_id, local_disc_cause, disc_cause);

    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(conn_id);

    if (p_link == NULL)
    {
        return;
    }

#if GFPS_FINDER_SUPPORT
    /*gfps finder maybe connected by gfps ble link to do provising or provisioned actions,
    so when gfps ble link disconencted, we shall reset gfps finder link here*/
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_gfps_finder_reset_conn();
    }
#endif

    APP_PRINT_ERROR3("app_gfps_le_disconnect_cb: plink connid %d not gfps link connid %d, gfps curr conn_id %d",
                     p_link->conn_id, p_link->gfps_link.gfps_conn_id, gfps_db.current_conn_id);

    if (p_link->conn_id != p_link->gfps_link.gfps_conn_id && p_link->conn_id != gfps_db.current_conn_id)
    {
        return;
    }

    if (p_link->conn_id == gfps_db.current_conn_id)
    {
        //reset current link aes key and pair status
        gfps_db.current_conn_id = 0xFF;
        gfps_reset_aeskey_and_pairing_status();

        //provider recover to original io capability
        if (p_link->gfps_link.auth_param_change)
        {
            APP_PRINT_INFO2("app_gfps_le_disconnect_cb: gap_set_pairable_mode, io_cap %d, authen_flag 0x%x",
                            p_link->gfps_link.io_cap, p_link->gfps_link.auth_flags);

            gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &(p_link->gfps_link.io_cap));
            gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t),
                          &(p_link->gfps_link.auth_flags));
            gap_set_pairable_mode();
        }
    }

    app_gfps_link_delete_link_from_priority_queue(p_link->gfps_link.gfps_conn_id);
    p_link->gfps_link.gfps_conn_state = GAP_CONN_STATE_DISCONNECTED;
    p_link->gfps_link.gfps_conn_id = 0xff;

    memset(&p_link->gfps_link, 0, sizeof(T_GFPS_LINK));
    app_gfps_device_handle_ble_link_disconnected(local_disc_cause);
}

void app_gfps_send_le_bond_confirm(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(conn_id);
    if (p_link == NULL)
    {
        return;
    }

    if (p_link->gfps_link.le_bond_passkey == p_link->gfps_link.gfps_raw_passkey)
    {
        le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        gfps_send_passkey(conn_id, gfps_db.gfps_id, p_link->gfps_link.le_bond_passkey);
    }
    else
    {
        le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_REJECT);
    }
    p_link->gfps_link.le_bond_confirm_pending = false;
}

void app_gfps_send_legacy_bond_confirm(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(conn_id);
    if (p_link == NULL)
    {
        return;
    }

    if (p_link->gfps_link.edr_bond_passkey == p_link->gfps_link.gfps_raw_passkey)
    {
        gap_br_user_cfm_req_cfm(p_link->gfps_link.edr_bond_bd_addr, GAP_CFM_CAUSE_ACCEPT);
        gfps_send_passkey(conn_id, gfps_db.gfps_id, p_link->gfps_link.edr_bond_passkey);
    }
    else
    {
        gap_br_user_cfm_req_cfm(p_link->gfps_link.edr_bond_bd_addr, GAP_CFM_CAUSE_REJECT);
    }
    p_link->gfps_link.edr_bond_confirm_pending = false;
}

void app_gfps_handle_bt_user_confirm(T_BT_EVENT_PARAM_LINK_USER_CONFIRMATION_REQ confirm_req)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(gfps_db.current_conn_id);

    if (p_link && p_link->gfps_link.is_gfps_pairing)
    {
        APP_PRINT_INFO3("app_gfps_handle_bt_user_confirm: CB_EVENT_USER_CONFIRM_REQ is_gfps_pairing %d, passkey %d, gfps_raw_passkey %d",
                        p_link->gfps_link.is_gfps_pairing, confirm_req.display_value, p_link->gfps_link.gfps_raw_passkey);

        memcpy(p_link->gfps_link.edr_bond_bd_addr, confirm_req.bd_addr, 6);
        p_link->gfps_link.edr_bond_passkey = confirm_req.display_value;

        if (p_link->gfps_link.gfps_raw_passkey_received)
        {
            app_gfps_send_legacy_bond_confirm(gfps_db.current_conn_id);
            p_link->gfps_link.gfps_raw_passkey_received = false;
        }
        else
        {
            p_link->gfps_link.edr_bond_confirm_pending = true;
            //TODO: start 10s timer
        }
    }
    else
    {
        gap_br_user_cfm_req_cfm(confirm_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
    }
}

void app_gfps_handle_ble_user_confirm(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(conn_id);

    if (p_link == NULL)
    {
        return;
    }

    uint32_t passkey = 0;
    le_bond_get_display_key(conn_id, &passkey);
    p_link->gfps_link.le_bond_passkey = passkey;

    APP_PRINT_INFO3("app_gfps_handle_ble_user_confirm: passkey %d, gfps_raw_passkey %d, is_gfps_pairing %d",
                    passkey, p_link->gfps_link.gfps_raw_passkey, p_link->gfps_link.is_gfps_pairing);

    if (p_link->gfps_link.is_gfps_pairing)
    {
        if (p_link->gfps_link.gfps_raw_passkey_received)
        {
            app_gfps_send_le_bond_confirm(conn_id);
            p_link->gfps_link.gfps_raw_passkey_received = false;
        }
        else
        {
            p_link->gfps_link.le_bond_confirm_pending = true;
            //TODO: start 10s timer
        }
    }
    else
    {
        le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
}

#if GFPS_LE_DEVICE_SUPPORT
void app_gfps_handle_additional_ble_user_confirm(uint8_t conn_id)
{
    uint32_t passkey = 0;
    le_bond_get_display_key(conn_id, &passkey);
    gfps_db.gfps_additional_passkey_from_stack = passkey;

    APP_PRINT_INFO1("app_gfps_handle_additional_ble_user_confirm: additional passkey %d", passkey);

    if (gfps_db.is_gfps_additional_pairing)
    {
        gfps_db.gfps_additional_conn_id = conn_id;

        if (gfps_db.gfps_additional_passkey_from_gfps_received)
        {
            if (gfps_db.gfps_additional_passkey_from_gfps == gfps_db.gfps_additional_passkey_from_stack)
            {
                le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
            }
            else
            {
                le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_REJECT);
            }

            gfps_db.gfps_additional_passkey_from_gfps_received = false;
            app_gfps_sec_handle_additional_io_cap(false);
        }
        else
        {
            gfps_db.gfps_additional_passkey_from_gfps_pending = true;
            //TODO: start 10s timer
        }
    }
    else
    {
        le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
}

void app_gfps_sec_handle_addition_bond_info(uint8_t *buf)
{
    APP_PRINT_INFO0("app_gfps_sec_handle_addition_bond_info");
    T_GFPS_ADDITIONAL_BOND_DATA gfps_additional_bond_data = {0};
    memcpy(&gfps_additional_bond_data, buf, sizeof(T_GFPS_ADDITIONAL_BOND_DATA));

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        gfps_db.gfps_additional_passkey_from_gfps = gfps_additional_bond_data.bond_passkey;
        gfps_db.gfps_additional_passkey_from_gfps_received = true;

        if (gfps_db.gfps_additional_passkey_from_gfps_pending == true)
        {
            if (gfps_db.gfps_additional_passkey_from_gfps == gfps_db.gfps_additional_passkey_from_stack)
            {
                le_bond_user_confirm(gfps_db.gfps_additional_conn_id, GAP_CFM_CAUSE_ACCEPT);
                gfps_additional_bond_data.status_code = GFPS_PASSKEY_STATUS_CODE_SUCCESS;
            }
            else
            {
                le_bond_user_confirm(gfps_db.gfps_additional_conn_id, GAP_CFM_CAUSE_REJECT);
                gfps_additional_bond_data.status_code = GFPS_PASSKEY_STATUS_CODE_FAIL;
            }

            /*secondary bud send additional bond result to primary bud*/
            gfps_additional_bond_data.bond_passkey = gfps_db.gfps_additional_passkey_from_stack;

            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS,
                                                APP_REMOTE_MSG_GFPS_ADDITIONAL_BOND_INFO,
                                                (uint8_t *)&gfps_additional_bond_data, sizeof(T_GFPS_ADDITIONAL_BOND_DATA));

            gfps_db.gfps_additional_passkey_from_gfps_received = false;
            gfps_db.gfps_additional_passkey_from_gfps_pending = false;

            app_gfps_sec_handle_additional_io_cap(false);
        }
    }

    APP_PRINT_INFO2("app_gfps_sec_handle_addition_bond_info: gfps passkey %d, stack passkey %d",
                    gfps_db.gfps_additional_passkey_from_gfps,
                    gfps_db.gfps_additional_passkey_from_stack);
}

void app_gfps_pri_handle_additional_bond_info(uint8_t *buf)
{
    APP_PRINT_INFO0("app_gfps_pri_handle_additional_bond_info");
    T_GFPS_ADDITIONAL_BOND_DATA gfps_additional_bond_data = {0};
    memcpy(&gfps_additional_bond_data, buf, sizeof(T_GFPS_ADDITIONAL_BOND_DATA));

    T_GFPS_ADDITIONAL_PASSKEY passkey_info;
    passkey_info.passkey = gfps_additional_bond_data.bond_passkey;
    passkey_info.status_code = (T_GFPS_PASSKEY_STATUS_CODE)gfps_additional_bond_data.status_code;

    memcpy(passkey_info.target_bonding_addr, gfps_additional_bond_data.bond_bd_addr, 6);

    gfps_set_additional_passkey_info(passkey_info);

    gfps_send_additional_passkey_response(gfps_db.current_conn_id, gfps_db.gfps_id, passkey_info);
}

void app_gfps_sec_handle_additional_io_cap(bool change_io_cap)
{
    APP_PRINT_INFO1("app_gfps_sec_handle_additional_io_cap: change_io_cap %d", change_io_cap);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (change_io_cap)
        {
            uint8_t io_cap = GAP_IO_CAP_DISPLAY_YES_NO;
            uint16_t auth_flags;

            gfps_db.is_gfps_additional_pairing = true;
            auth_flags = (gfps_db.additional_default_auth_flags | GAP_AUTHEN_BIT_MITM_FLAG);
            gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &io_cap);
            gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t), &auth_flags);
            gap_set_pairable_mode();
            APP_PRINT_INFO2("app_gfps_sec_handle_additional_io_cap: gap_set_pairable_mode, io_cap %d, authen_flag 0x%x",
                            io_cap, auth_flags);
        }
        else
        {
            gfps_db.is_gfps_additional_pairing = false;
            //change to original io capability
            gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t),
                          &(gfps_db.additional_default_io_cap));
            gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t),
                          &(gfps_db.additional_default_auth_flags));
            gap_set_pairable_mode();
        }
    }
}

T_APP_RESULT app_gfps_cb_handle_additional_passkey(T_GFPS_CALLBACK_DATA *p_callback)
{
    APP_PRINT_INFO0("app_gfps_cb_handle_additional_passkey");
    T_GFPS_ADDITIONAL_BOND_DATA gfps_additional_bond_data = {0};

    memcpy(gfps_additional_bond_data.bond_bd_addr,
           p_callback->msg_data.additional_passkey.target_bonding_addr, 6);

    gfps_additional_bond_data.bond_passkey = p_callback->msg_data.additional_passkey.passkey;
    gfps_db.status_code = GFPS_PASSKEY_STATUS_CODE_PENDING;

    /*primary bud send passkey get from gfps additional passkey characteristic to secondary bud*/
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS,
                                        APP_REMOTE_MSG_GFPS_ADDITIONAL_BOND_INFO,
                                        (uint8_t *)&gfps_additional_bond_data, sizeof(T_GFPS_ADDITIONAL_BOND_DATA));

    return APP_RESULT_SUCCESS;
}
#endif

T_APP_RESULT app_gfps_cb_handle_kbp_write_req(T_SERVER_ID service_id,
                                              T_GFPS_CALLBACK_DATA *p_callback)
{
    uint8_t ret = 0;
    T_APP_LE_LINK *p_le_link;
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    p_le_link = app_link_find_le_link_by_conn_id(p_callback->conn_id);

    if (p_le_link == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    app_gfps_ble_conn_info_init(p_callback->conn_id);

    if (p_callback->msg_data.kbp.result == GFPS_WRITE_RESULT_SUCCESS)
    {
#if GFPS_LE_DEVICE_SUPPORT
#else
        if (gfps_db.current_conn_id != p_callback->conn_id)
        {
            /*Invalid connection, disconnect*/
            app_ble_gap_disconnect(p_le_link, LE_LOCAL_DISC_CAUSE_GFPS_FAILED);
            app_result = APP_RESULT_VALUE_NOT_ALLOWED;
            ret = 1;
            goto error;
        }
#endif
        APP_PRINT_INFO7("app_gfps_cb_handle_kbp_write_req: provider bond %d, notify name %d, "
                        "retroactively %d, pk exist %d, accountkey idx %d, provider addr %s, seek addr %s",
                        p_callback->msg_data.kbp.provider_init_bond,
                        p_callback->msg_data.kbp.notify_existing_name,
                        p_callback->msg_data.kbp.retroactively_account_key,
                        p_callback->msg_data.kbp.pk_field_exist,
                        p_callback->msg_data.kbp.account_key_idx,
                        TRACE_BDADDR(p_callback->msg_data.kbp.provider_addr),
                        TRACE_BDADDR(p_callback->msg_data.kbp.seek_br_edr_addr)
                       );

        if (p_callback->msg_data.kbp.retroactively_account_key == 1)
        {
            memcpy(p_le_link->gfps_link.edr_bond_bd_addr, p_callback->msg_data.kbp.seek_br_edr_addr, 6);
        }
        else
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
#if GFPS_FINDER_SUPPORT
                if (extend_app_cfg_const.gfps_finder_support &&
                    (extend_app_cfg_const.gfps_device_type == GFPS_LOCATOR_TRACKER))
                {
                    return APP_RESULT_VALUE_NOT_ALLOWED;
                }
#endif
                uint8_t io_cap = GAP_IO_CAP_DISPLAY_YES_NO;
                uint16_t auth_flags;

                /*provider shall set io capability to DISPLAY YES/NO for triggering numeric comparison pairing*/
                p_le_link->gfps_link.is_gfps_pairing = true;
                gap_get_param(GAP_PARAM_BOND_IO_CAPABILITIES, &(p_le_link->gfps_link.io_cap));
                gap_get_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, &(p_le_link->gfps_link.auth_flags));
                p_le_link->gfps_link.auth_param_change = true;
                auth_flags = (p_le_link->gfps_link.auth_flags | GAP_AUTHEN_BIT_MITM_FLAG);
                gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &io_cap);
                gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t), &auth_flags);
                gap_set_pairable_mode();
                APP_PRINT_INFO2("app_gfps_cb_handle_kbp_write_req: gap_set_pairable_mode, io_cap %d, authen_flag 0x%x",
                                io_cap, auth_flags);

                if (p_callback->msg_data.kbp.provider_init_bond)
                {
                    gap_br_pairing_req(p_callback->msg_data.kbp.seek_br_edr_addr);
                }
#if F_APP_ERWS_SUPPORT
#if GFPS_LE_DEVICE_SUPPORT
                if (extend_app_cfg_const.gfps_le_device_support)
                {
                    T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

                    if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
                         gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA))
                    {
                        uint8_t change_io_cap = true;
                        /*primary bud send change_io_cap to secondary bud*/
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS,
                                                            APP_REMOTE_MSG_GFPS_ADDITIONAL_IO_CAP,
                                                            &change_io_cap, 1);
                    }
                }
#endif
#endif
            }
        }

#if GFPS_PERSONALIZED_NAME_SUPPORT
        //If the Request's Flags byte has bit 2 set to 1, notify the personalized name characteristic(p_callback->msg_data.kbp.notify_existing_name)
        if (p_callback->msg_data.kbp.notify_existing_name)
        {
            /*need get personalized name from ftl and send the name into gfps*/
            app_gfps_personalized_name_send(p_callback->conn_id, service_id);
        }
#endif

        if (p_callback->msg_data.kbp.pk_field_exist == 0)
        {

            if ((app_cfg_const.enable_multi_link &&
                 (app_link_get_b2s_link_num() == app_cfg_const.max_legacy_multilink_devices))
                || (!app_cfg_const.enable_multi_link && (app_link_get_b2s_link_num() > 0)))
            {
                if (app_hfp_get_call_status() == APP_CALL_IDLE)
                {
                    APP_PRINT_INFO1("app_gfps_cb_handle_kbp_write_req: disconnect b2s for subsequent pair, multilink enable %d",
                                    app_cfg_const.enable_multi_link);
                    app_bt_policy_enter_pairing_mode(true, false);
                }
            }
        }
    }
    else
    {
        ret = 3;
        goto error;
    }

error:
    APP_PRINT_INFO2("app_gfps_cb_handle_kbp_write_req: ret %d, app_result %d", ret, app_result);
    return app_result;
}

T_APP_RESULT app_gfps_cb_handle_action_req(T_GFPS_CALLBACK_DATA *p_callback)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    if (p_callback->msg_data.action_req.result == GFPS_WRITE_RESULT_SUCCESS)
    {
        APP_PRINT_INFO5("app_gfps_cb_handle_action_req: pk_field_exist %d, account_key_idx %d, "
                        "device_action %d, additional_data %d, provider_addr %s",
                        p_callback->msg_data.action_req.pk_field_exist,
                        p_callback->msg_data.action_req.account_key_idx,
                        p_callback->msg_data.action_req.device_action,
                        p_callback->msg_data.action_req.additional_data,
                        TRACE_BDADDR(p_callback->msg_data.action_req.provider_addr)
                       );
    }
    else
    {
        APP_PRINT_ERROR1("app_gfps_cb_handle_action_req: failed, result %d",
                         p_callback->msg_data.action_req.result);
    }

    return app_result;
}

T_APP_RESULT app_gfps_cb_handle_passkey(T_GFPS_CALLBACK_DATA *p_callback)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(p_callback->conn_id);
    if (p_link == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    p_link->gfps_link.gfps_raw_passkey = p_callback->msg_data.passkey;
    p_link->gfps_link.gfps_raw_passkey_received = true;

    APP_PRINT_INFO3("app_gfps_cb_handle_passkey: gfps_raw_passkey %d ,le_bond_confirm_pending %d, edr_bond_confirm_pending %d",
                    p_link->gfps_link.gfps_raw_passkey,
                    p_link->gfps_link.le_bond_confirm_pending,
                    p_link->gfps_link.edr_bond_confirm_pending);

    if (p_link->gfps_link.le_bond_confirm_pending)
    {
        app_gfps_send_le_bond_confirm(p_callback->conn_id);
        p_link->gfps_link.gfps_raw_passkey_received = false;
    }
    else if (p_link->gfps_link.edr_bond_confirm_pending)
    {
        app_gfps_send_legacy_bond_confirm(p_callback->conn_id);
        p_link->gfps_link.gfps_raw_passkey_received = false;
    }

#if GFPS_LE_DEVICE_SUPPORT
    if (gfps_get_pairing_status() == GFPS_PAIRING_STATUS_SUBSEQUENT &&
        extend_app_cfg_const.gfps_le_device_support)
    {
        T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

        if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA  ||
             gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
            && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            APP_PRINT_INFO0("app_gfps_cb_handle_passkey: start gfps and le audio adv for subsequent pairing");
            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }

        if (p_link->gfps_link.auth_param_change)
        {
            APP_PRINT_INFO2("app_gfps_cb_handle_passkey: gap_set_pairable_mode, io_cap %d, authen_flag 0x%x",
                            p_link->gfps_link.io_cap, p_link->gfps_link.auth_flags);
            //provider recover to original io capability
            gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &(p_link->gfps_link.io_cap));
            gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t),
                          &(p_link->gfps_link.auth_flags));
            gap_set_pairable_mode();
            p_link->gfps_link.auth_param_change = false;
        }

        /*The current procedure was a subsequent pair, seeker will not write account key,
        so primary need relay APP_REMOTE_MSG_GFPS_SUBPAIR_SEC_ADV_ENABLE to let secondary enable gfps/le_audio adv */
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS,
                                            APP_REMOTE_MSG_GFPS_SUBPAIR_SEC_ADV_ENABLE,
                                            NULL, 0);
    }
#endif

    return app_result;
}

T_APP_RESULT app_gfps_cb_handle_accountkey(T_GFPS_CALLBACK_DATA *p_callback)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(p_callback->conn_id);

    if (p_link == NULL)
    {
        return APP_RESULT_APP_ERR;
    }

    /*for initial pairing or retroactive write account key receive account key from seeker*/
    if (app_gfps_account_key_store(p_callback->msg_data.account_key,
                                   p_link->gfps_link.edr_bond_bd_addr))
    {
        /*The current procedure was a initial pair or retroactive write accountkey, seeker will write account key,
        so relay account key info to secondary, secondary need enable gfps/le_audio adv when receive account key sync info */
        app_gfps_account_key_remote_add(p_callback->msg_data.account_key,
                                        p_link->gfps_link.edr_bond_bd_addr);
#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);

            T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

            if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA  ||
                 gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
                && app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                APP_PRINT_INFO0("app_gfps_cb_handle_accountkey: start le audio adv for subsequent pairing");
                app_lea_adv_start();
            }
        }
        else
#endif
        {
            app_gfps_device_check_state((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }
    }

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        if (p_link->gfps_link.auth_param_change)
        {
            APP_PRINT_INFO2("app_gfps_cb_handle_accountkey: gap_set_pairable_mode, io_cap %d, authen_flag 0x%x",
                            p_link->gfps_link.io_cap, p_link->gfps_link.auth_flags);
            //provider recover to original io capability
            gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &(p_link->gfps_link.io_cap));
            gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t),
                          &(p_link->gfps_link.auth_flags));
            gap_set_pairable_mode();
            p_link->gfps_link.auth_param_change = false;
        }
    }
#endif

    return APP_RESULT_SUCCESS;
}

T_APP_RESULT app_gfps_cb_handle_additional_data(T_GFPS_CALLBACK_DATA *p_callback)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    //personalized name need to be stored in ftl
#if GFPS_PERSONALIZED_NAME_SUPPORT
    if (p_callback->msg_data.additional_data.data_id == GFPS_DATA_ID_PERSONALIZED_NAME)
    {
        T_GFPS_PERSON_NAME_RST result = app_gfps_personalized_name_store(
                                            p_callback->msg_data.additional_data.p_data,
                                            p_callback->msg_data.additional_data.data_len);
        if (result == APP_GFPS_PERSONALIZED_NAME_SUCCESS)
        {
            app_gfps_personalized_name_remote_sync();
        }

        APP_PRINT_INFO4("app_gfps_cb_handle_additional_data: personalized name %s, result %d, data_id 0x%x, data_len %d",
                        TRACE_STRING(p_callback->msg_data.additional_data.p_data), result,
                        p_callback->msg_data.additional_data.data_id,
                        p_callback->msg_data.additional_data.data_len);
    }
#endif

    return app_result;
}

void app_gfps_set_ble_conn_param(uint8_t conn_id)
{
    APP_PRINT_INFO0("app_gfps_set_ble_conn_param");
    uint16_t conn_interval_min = 0x06;
    uint16_t conn_interval_max = 0x06;
    uint16_t conn_latency = 0;
    uint16_t conn_supervision_timeout = 500;

    ble_set_prefer_conn_param(conn_id, conn_interval_min, conn_interval_max, conn_latency,
                              conn_supervision_timeout);
}

/*Fast pair service callback*/
static T_APP_RESULT app_gfps_cb(T_SERVER_ID service_id, void *p_data)
{
    uint8_t ret = 0;
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    T_GFPS_CALLBACK_DATA *p_callback = (T_GFPS_CALLBACK_DATA *)p_data;

    switch (p_callback->msg_type)
    {
    case GFPS_CALLBACK_TYPE_NOTIFICATION_ENABLE:
        {
            APP_PRINT_INFO1("app_gfps_cb: GFPS_CALLBACK_TYPE_NOTIFICATION_ENABLE, T_GFPS_NOTIFICATION_TYPE %d",
                            p_callback->msg_data.notify_type);
        }
        break;

    case GFPS_CALLBACK_TYPE_KBP_WRITE_REQ:
        {
            app_result = app_gfps_cb_handle_kbp_write_req(service_id, p_callback);
        }
        break;

    case GFPS_CALLBACK_TYPE_ACTION_REQ:
        {
            app_result = app_gfps_cb_handle_action_req(p_callback);
        }
        break;

    case GFPS_CALLBACK_TYPE_PASSKEY:
        {
            app_result = app_gfps_cb_handle_passkey(p_callback);
        }
        break;

    case GFPS_CALLBACK_TYPE_ACCOUNT_KEY:
        {
            app_result = app_gfps_cb_handle_accountkey(p_callback);
        }
        break;

#if GFPS_ADDTIONAL_DATA_SUPPORT
    case GFPS_CALLBACK_TYPE_ADDITIONAL_DATA:
        {
            app_result = app_gfps_cb_handle_additional_data(p_callback);
        }
        break;
#endif

#if GFPS_LE_DEVICE_SUPPORT
    case GFPS_CALLBACK_TYPE_ADDITIONAL_PASSKEY:
        {
            if (extend_app_cfg_const.gfps_le_device_support)
            {
                app_result = app_gfps_cb_handle_additional_passkey(p_callback);
            }
        }
        break;
#endif

    default:
        break;
    }

    return app_result;
}
/**
 * @brief non discoverable mode has two submode:show pop up ui or hide pop up ui
 * hide pop up ui: we want to stop showing the subsequent pairing notification since that pairing could be rejected
 *
 */
void app_gfps_enter_nondiscoverable_mode()
{
    APP_PRINT_INFO0("app_gfps_enter_nondiscoverable_mode");
#if 0
    if (app_cfg_const.enable_multi_link && app_cfg_const.max_legacy_multilink_devices > 1)
    {
        if (app_link_get_b2s_link_num() == app_cfg_const.max_legacy_multilink_devices)
        {
            APP_PRINT_INFO0("app_gfps_enter_nondiscoverable_mode: pop up hide ui");
            app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI);
        }
        else
        {
            app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE);
        }
    }
    else
#endif
    {
        app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE);
    }
}

void app_gfps_force_enter_pairing_mode(uint8_t status)
{
    switch (status)
    {
    case GFPS_KEY_FORCE_ENTER_PAIR_MODE:
    case GFPS_LE_DISCONN_FORCE_ENTER_PAIR_MODE:
        {
            gfps_db.force_enter_pair_mode = true;

        }
        break;

    case GFPS_EXIT_PAIR_MODE:
        {
            gfps_db.force_enter_pair_mode = false;
        }
        break;

    default:
        break;
    }

    APP_PRINT_INFO2("app_gfps_force_enter_pairing_mode: status %d, force_enter_pair_mode %d",
                    status, gfps_db.force_enter_pair_mode);

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        APP_PRINT_INFO0("app_gfps_force_enter_pairing_mode: gfps le");
        app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
    }
    else
#endif
    {
        if (gfps_db.force_enter_pair_mode)
        {
            APP_PRINT_INFO0("app_gfps_force_enter_pairing_mode: gfps legacy");
            app_gfps_next_action(GFPS_ACTION_ADV_DISCOVERABLE_MODE_WITH_MODEL_ID);
            gfps_db.force_enter_pair_mode = false;
        }
    }
}

#if GFPS_LE_DEVICE_SUPPORT
void app_gfps_le_device_adv_start(T_REMOTE_SESSION_ROLE device_role)
{
    APP_PRINT_INFO2("app_gfps_le_device_adv_start: bud_role %d, force enter pairing mode %d",
                    device_role, gfps_db.force_enter_pair_mode);

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        uint8_t link_num = app_gfps_link_find_gfps_link();
        T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

        if (link_num >= GFPS_LE_MAX_LINK_NUMBER)
        {
            if (device_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI);
            }

            return;
        }

        if (device_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            T_ACCOUNT_KEY *p_key_info = app_gfps_account_key_get_table();

            if (gfps_db.force_enter_pair_mode)
            {
                app_gfps_next_action(GFPS_ACTION_ADV_DISCOVERABLE_MODE_WITH_MODEL_ID);
            }
            else if (p_key_info->num == 0)
            {
                if (link_num == 0)
                {
                    app_gfps_next_action(GFPS_ACTION_ADV_DISCOVERABLE_MODE_WITH_MODEL_ID);
                }
                else
                {
                    app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE);
                }
            }
            else
            {
                app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE);
            }

            if (gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
                gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
            {
                app_lea_adv_start();
            }
        }
        else
        {
            if (gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
                gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
            {
#if F_APP_ERWS_SUPPORT
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    /*b2b must connected and pri relay SIRK to sec (LEA_REMOTE_SYNC_SIRK)
                    and then secondary start LE audio adv*/
                    app_lea_adv_start();
                }
#endif
            }
            else
            {
                app_gfps_next_action(GFPS_ACTION_IDLE);
                app_gfps_link_disconnect_all_gfps_link();
            }
        }
    }
}
#endif

/**
 * @brief Include the battery information in the advertisement with hide battery notification (0b0100) for at least 5 seconds
 * when case is closed and both buds are docked.
 * Include the battery information in the advertisement with show battery notification(0b0011) when the case has opened.
 * @param[in] open  if open = true means case open, if open = false means case close.
 */
void app_gfps_handle_case_status(bool open)
{
    APP_PRINT_INFO1("app_gfps_handle_case_status: open %d", open);
    T_BT_DEVICE_MODE radio_mode = app_bt_policy_get_radio_mode();

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (open)
        {
            /*show battery popup*/
            app_gfps_battery_show_ui(true);
            /*show pair popup*/
            app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE);

#if GFPS_FINDER_SUPPORT
            if (extend_app_cfg_const.gfps_finder_support)
            {
                T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_ON;
                app_gfps_finder_enter_beacon_state(beacon_state);
            }
#endif
        }
        else
        {
            /*hid battery popup*/
            app_gfps_battery_show_ui(false);
            /*hid pair popup*/
            app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI);

#if GFPS_FINDER_SUPPORT
            if (extend_app_cfg_const.gfps_finder_support)
            {
                T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_OFF;
                app_gfps_finder_enter_beacon_state(beacon_state);
            }
#endif
        }

        //app_gfps_enter_nondiscoverable_mode();
    }
}

void app_gfps_handle_b2s_connected(uint8_t *bd_addr)
{
    uint8_t account_key[16] = {0};
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(gfps_db.current_conn_id);

    /*(memcmp(bd_addr, gfps_link.edr_bond_bd_addr, 6) == 0): b2s is connected through gfps pair.
    gfps_get_subsequent_pairing_account_key(account_key) == true: is gfps subsequent pairing.
    */
    if ((memcmp(bd_addr, p_link->gfps_link.edr_bond_bd_addr, 6) == 0) &&
        (gfps_get_subsequent_pairing_account_key(account_key) == true))
    {
        APP_PRINT_INFO2("app_gfps_handle_b2s_connected:bd_addr_sub %b,gfps_link->edr_bond_bd_addr %b",
                        TRACE_BDADDR(bd_addr), TRACE_BDADDR(p_link->gfps_link.edr_bond_bd_addr));
        /*store subsequent pairing account key*/
        if (app_gfps_account_key_store(account_key, p_link->gfps_link.edr_bond_bd_addr))
        {
            app_gfps_account_key_remote_add(account_key, p_link->gfps_link.edr_bond_bd_addr);
        };
    }
    APP_PRINT_INFO2("app_gfps_handle_b2s_connected:bd_addr %b,gfps_link->edr_bond_bd_addr %b",
                    TRACE_BDADDR(bd_addr), TRACE_BDADDR(p_link->gfps_link.edr_bond_bd_addr));
}

#if GFPS_SASS_SUPPORT
/**
 * @brief Check if sass connection status changed.
 *        If changed, update the sass connection status
 *
 */
void app_gfps_adv_check_sass_conn_status(void)
{
    T_SASS_CONN_STATUS_FIELD cur_conn_status;
    //get full conn status
    gfps_sass_get_conn_status(&cur_conn_status);
    //generate new conn status
    app_gfps_gen_conn_status(&cur_conn_status);

    if (gfps_sass_set_conn_status(&cur_conn_status) == true)
    {
        //if new conn status different with old conn status, update conn status in adv data
        if (gfps_db.gfps_curr_action == GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE)
        {
            if (gfps_gen_adv_data(NOT_DISCOVERABLE_MODE, gfps_adv_data, &gfps_adv_len, true))
            {
                ble_ext_adv_mgr_set_adv_data(gfps_db.gfps_adv_handle, gfps_adv_len, gfps_adv_data);
            }
        }
        else if (gfps_db.gfps_curr_action == GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE_HIDE_UI)
        {
            if (gfps_gen_adv_data(NOT_DISCOVERABLE_MODE, gfps_adv_data, &gfps_adv_len, false))
            {
                ble_ext_adv_mgr_set_adv_data(gfps_db.gfps_adv_handle, gfps_adv_len, gfps_adv_data);
            }
        }
        else
        {
            APP_PRINT_INFO1("app_gfps_adv_check_sass_conn_status: invalid action %d", gfps_db.gfps_curr_action);
        }
    }
    else
    {
        APP_PRINT_INFO0("app_gfps_adv_check_sass_conn_status: no change, do not need update");
    }
}

/**
 * @brief update sass conn status
 *
 */
void app_gfps_adv_update_conn_status(void)
{
    app_gfps_adv_check_sass_conn_status();

    if (gfps_db.gfps_adv_state == BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }
        else
#endif
        {
            app_gfps_device_check_state((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }
    }
}

void app_gfps_check_inuse_account_key(void)
{
    uint8_t inuse_account_key = gfps_get_inuse_account_key_index();
    uint8_t recently_inuse_account_key = gfps_get_recently_inuse_account_key_index();
    T_APP_BR_LINK *p_edr_link = NULL;
    T_APP_LE_LINK *p_le_link = NULL;
    uint8_t active_idx = MAX_BR_LINK_NUM;
    uint8_t active_mode;

#if GFPS_SASS_LEA_SUPPORT
    uint8_t b2s_link_num = app_link_get_b2s_link_num() + app_link_get_le_link_num();
#else
    uint8_t b2s_link_num = app_link_get_b2s_link_num();
#endif
    APP_PRINT_TRACE4("app_gfps_check_inuse_account_key: multipoint_state: %d, b2s: %d, "
                     "inuse_account_key: %d, recently_inuse_account_key: %d",
                     app_cfg_nv.sass_multipoint_state, b2s_link_num,
                     inuse_account_key, recently_inuse_account_key);

    if (b2s_link_num != 0)
    {
        uint8_t gfps_inuse_account_key = 0xFF;
        active_idx = mtc_get_active_idx(&active_mode);
        if (active_idx != MAX_BR_LINK_NUM)
        {
            if (active_mode == MULTI_PRO_BT_BREDR)
            {
                p_edr_link = &app_db.br_link[active_idx];
            }
            else
            {
                p_le_link = &app_db.le_link[active_idx];
            }
        }

#if GFPS_SASS_LEA_SUPPORT
        if (p_le_link != NULL)
        {
            gfps_inuse_account_key = p_le_link->gfps_inuse_account_key;
        }
#endif
        if (p_edr_link != NULL)
        {
            gfps_inuse_account_key = p_edr_link->gfps_inuse_account_key;
        }

        if (app_cfg_nv.sass_multipoint_state)
        {
            if (b2s_link_num == 1)
            {
                if (inuse_account_key != gfps_inuse_account_key)
                {
                    if (gfps_inuse_account_key != 0xff)//sass device
                    {
                        gfps_set_most_recently_inuse_account_key_index(0xff);
                    }
                    else //non-sass device
                    {
                        if (inuse_account_key != 0xff)
                        {
                            gfps_set_most_recently_inuse_account_key_index(inuse_account_key);
                            app_cfg_nv.sass_recently_used_account_key_index = inuse_account_key;
                        }
                    }
                    gfps_set_inuse_account_key_index(gfps_inuse_account_key);
                }
            }
            else if (b2s_link_num == 2)
            {
                uint8_t inactive_gfps_inuse_account_key = 0xFF;
#if GFPS_SASS_LEA_SUPPORT
                if (app_link_get_b2s_link_num() && app_link_get_lea_link_num())
                {
                    if (p_le_link)
                    {
                        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                        {
                            if (app_db.br_link[i].used && app_link_check_b2s_link_by_id(i))
                            {
                                inactive_gfps_inuse_account_key = app_db.br_link[i].gfps_inuse_account_key;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
                        {
                            if (app_db.le_link[i].used == true &&
                                app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED)
                            {
                                inactive_gfps_inuse_account_key = app_db.le_link[i].gfps_inuse_account_key;
                                break;
                            }
                        }
                    }
                }
                else if (app_link_get_lea_link_num())//both le link
                {
                    if (p_le_link)
                    {
                        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
                        {
                            if (app_db.le_link[i].used == true &&
                                app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED &&
                                app_db.le_link[i].conn_id != p_le_link->conn_id)
                            {
                                inactive_gfps_inuse_account_key = app_db.le_link[i].gfps_inuse_account_key;
                                break;
                            }
                        }

                    }
                }
                else //both edr link
#endif
                {
                    if (p_edr_link)
                    {
                        if (app_db.br_link[app_multi_find_inacitve(p_edr_link->id)].used)
                        {
                            inactive_gfps_inuse_account_key = app_db.br_link[app_multi_find_inacitve(
                                                                                 p_edr_link->id)].gfps_inuse_account_key;
                        }
                    }
                }

                //sass device
                if (gfps_inuse_account_key != 0xff)
                {
                    gfps_set_most_recently_inuse_account_key_index(0xff);
                    gfps_set_inuse_account_key_index(gfps_inuse_account_key);
                }
                else//inused non sass device
                {
                    if (inactive_gfps_inuse_account_key != 0xff)
                    {
                        gfps_set_most_recently_inuse_account_key_index(inactive_gfps_inuse_account_key);
                        app_cfg_nv.sass_recently_used_account_key_index = inactive_gfps_inuse_account_key;
                    }
                    else
                    {
                        gfps_set_most_recently_inuse_account_key_index(app_cfg_nv.sass_recently_used_account_key_index);
                    }
                    gfps_set_inuse_account_key_index(0xff);
                }
            }
        }
        else
        {

            if ((gfps_inuse_account_key != 0xFF) && (inuse_account_key != gfps_inuse_account_key))
            {
                gfps_set_most_recently_inuse_account_key_index(inuse_account_key);
                if (inuse_account_key != 0xff)
                {
                    app_cfg_nv.sass_recently_used_account_key_index = inuse_account_key;
                }
                gfps_set_inuse_account_key_index(gfps_inuse_account_key);
            }
        }
    }
    else
    {
        if (inuse_account_key != 0xff)
        {
            gfps_set_most_recently_inuse_account_key_index(inuse_account_key);
            app_cfg_nv.sass_recently_used_account_key_index = inuse_account_key;
            gfps_set_inuse_account_key_index(0xff);
        }
    }
    inuse_account_key = gfps_get_inuse_account_key_index();
    recently_inuse_account_key = gfps_get_recently_inuse_account_key_index();
    if ((inuse_account_key != 0xff) && (recently_inuse_account_key != 0xff))
    {
        APP_PRINT_TRACE4("app_gfps_check_inuse_account_key abnormal key setting 0x%,0x%x,0x%x,0x%x",
                         inuse_account_key, recently_inuse_account_key,
                         p_edr_link ? p_edr_link->gfps_inuse_account_key : 0xff,
                         p_le_link ? p_le_link->gfps_inuse_account_key : 0xff);
        gfps_set_most_recently_inuse_account_key_index(0xff);
    }
}

void app_gfps_notify_conn_status(void)
{
    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_check_inuse_account_key();
        app_gfps_adv_update_conn_status();
        app_gfps_msg_notify_connection_status();
    }
}

bool app_gfps_gen_conn_status(T_SASS_CONN_STATUS_FIELD *conn_status)
{
    uint8_t length = 3;
    T_SASS_FOCUS_MODE focus_mode = SASS_NOT_IN_FOCUS;
    T_SASS_ON_HEAD_DETECTION on_head = SASS_NOT_ON_HEAD;
    T_SASS_AUTO_RECONN auto_reconnect = SASS_AUTO_RECONN_DISABLE;
    T_SASS_CONN_STATE conn_state = SASS_DISABLE_CONN_SWITCH;
    T_SASS_CONN_AVAILABLE conn_available = SASS_CONN_NOT_FULL;
    uint8_t active_index = 0;
    if (conn_status == NULL)
    {
        return false;
    }
    active_index = app_multi_get_active_idx();
#if 0
    if (app_sass_policy_get_focus_mode())
    {
        focus_mode = SASS_IN_FOCUS;
    }
#endif
    if ((app_db.local_loc == BUD_LOC_IN_EAR) || (app_db.remote_loc == BUD_LOC_IN_EAR))
    {
        on_head = SASS_ON_HEAD;
    }
    if (mtc_get_btmode() == MULTI_PRO_BT_BREDR)
    {
        if (app_db.br_link[active_index].connected_by_linkback)
        {
            auto_reconnect = SASS_AUTO_RECONN_ENABLE;
        }
    }
    else
    {
        auto_reconnect = SASS_AUTO_RECONN_DISABLE;
    }
    conn_state = (T_SASS_CONN_STATE) app_sass_policy_get_connection_state();
    conn_available = app_sass_get_available_connection_num() <= 0 ? SASS_CONN_FULL :
                     SASS_CONN_NOT_FULL;
    conn_status->conn_status_info.auto_reconn = auto_reconnect;
    conn_status->conn_status_info.conn_availability = conn_available;
    conn_status->conn_status_info.conn_state = (uint8_t)conn_state;
    conn_status->conn_status_info.focus_mode = focus_mode;
    conn_status->conn_status_info.on_head_detection = on_head;
    conn_status->conn_dev_bitmap = app_sass_policy_get_conn_bit_map();
    APP_PRINT_INFO6("app_gfps_gen_conn_status: auto_reconn %d, conn_availability %d, "
                    "conn_state %d, focus_mode %d, on_head_detection %d, conn_dev_bitmap 0x%x",
                    conn_status->conn_status_info.auto_reconn,
                    conn_status->conn_status_info.conn_availability,
                    conn_status->conn_status_info.conn_state,
                    conn_status->conn_status_info.focus_mode,
                    conn_status->conn_status_info.on_head_detection,
                    conn_status->conn_dev_bitmap);
    return true;
}
#endif

#endif
