/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "app_gfps_device.h"
#include "app_gfps.h"
#include "app_main.h"
#include "app_gfps_account_key.h"
#include "app_gfps_personalized_name.h"
#include "app_cfg.h"
#include "app_bt_policy_api.h"
#include "bt_bond.h"
#include "app_bond.h"
#include "gap_bond_le.h"
#if GFPS_FEATURE_SUPPORT
#include "app_cfg.h"
#include "app_gfps_battery.h"
#include "app_gfps_link.h"
#include "app_link_util.h"
#if GFPS_FINDER_SUPPORT
#include "app_gfps_finder.h"
#include "app_gfps_finder_adv.h"
#include "app_adv_stop_cause.h"
#endif

/**
 * @brief for GFPS legacy pairing, check legacy radio mode to start GFPS adv
 *
 */
void app_gfps_device_check_state(T_REMOTE_SESSION_ROLE device_role)
{
    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        T_BT_DEVICE_MODE radio_mode = app_bt_policy_get_radio_mode();
#if GFPS_SASS_SUPPORT
        if (extend_app_cfg_const.gfps_sass_support)
        {
            app_gfps_adv_check_sass_conn_status();
        }
#endif
        APP_PRINT_INFO3("app_gfps_device_check_state: radio_mode %d, bud_role %d, b2s link %d",
                        radio_mode, device_role, app_link_get_b2s_link_num());

        if (device_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            //initial pairing
            if (radio_mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
            {
                /*initial pairing*/
                app_gfps_next_action(GFPS_ACTION_ADV_DISCOVERABLE_MODE_WITH_MODEL_ID);
            }/*subsequent pairing*/
            else
            {
                app_gfps_enter_nondiscoverable_mode();
            }

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
            app_gfps_next_action(GFPS_ACTION_IDLE);

#if GFPS_FINDER_SUPPORT
            if (extend_app_cfg_const.gfps_finder_support)
            {
                T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_OFF;
                app_gfps_finder_enter_beacon_state(beacon_state);
            }
#endif
            app_gfps_link_disconnect_all_gfps_link();
        }
    }
}

void app_gfps_device_handle_power_off(void)
{
    APP_PRINT_TRACE0("app_gfps_device_handle_power_off");

    app_gfps_next_action(GFPS_ACTION_IDLE);
    app_gfps_link_disconnect_all_gfps_link();

#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        if (gfps_get_inuse_account_key_index() != 0xff)
        {
            app_cfg_nv.sass_recently_used_account_key_index = gfps_get_inuse_account_key_index();
        }
        else if (gfps_get_recently_inuse_account_key_index() != 0xff)
        {
            app_cfg_nv.sass_recently_used_account_key_index = gfps_get_recently_inuse_account_key_index();
        }
        app_cfg_store(&app_cfg_nv.sass_recently_used_account_key_index, 1);
    }
#endif

#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_gfps_finder_handle_power_off();
    }
#endif
}

void app_gfps_device_handle_power_on(bool is_pairing)
{
    APP_PRINT_TRACE0("app_gfps_device_handle_power_on");
    app_gfps_battery_show_ui(true);

#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        gfps_set_most_recently_inuse_account_key_index(app_cfg_nv.sass_recently_used_account_key_index);
    }
#endif

#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_gfps_finder_set_ring_param();
    }
#endif

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        uint8_t bond_num = app_bond_b2s_num_get();
        if ((!is_pairing) && (bond_num != 0))
        {
            app_gfps_next_action(GFPS_ACTION_ADV_NOT_DISCOVERABLE_MODE);
        }

#if GFPS_FINDER_SUPPORT
        if (extend_app_cfg_const.gfps_finder_support)
        {
            T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_ON;
            uint32_t adv_interval = extend_app_cfg_const.gfps_power_on_finder_adv_interval;

            gfps_finder_adv_update_adv_interval(adv_interval);
            app_gfps_finder_enter_beacon_state(beacon_state);
        }
#endif

#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            gfps_set_identity_address(app_cfg_nv.bud_local_addr, app_cfg_nv.bud_peer_addr, false);
            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);

            /*from google: rws primary bud or single mode shall distribute linkkey GFPS_LE_SMP_DIST_LINK_KEY*/
            uint8_t init_key = GFPS_LE_SMP_DIST_ENC_KEY | GFPS_LE_SMP_DIST_ID_KEY | GFPS_LE_SMP_DIST_LINK_KEY;
            uint8_t response_key = GFPS_LE_SMP_DIST_ENC_KEY | GFPS_LE_SMP_DIST_ID_KEY |
                                   GFPS_LE_SMP_DIST_LINK_KEY;
            le_bond_cfg_local_key_distribute(init_key, response_key);

            /*rws primary bud or single mode shall set GFPS_PSM_STATE_READY*/
            T_GFPS_PSM_RSP gfps_psm_rsp;
            gfps_psm_rsp.psm = GFPS_PSM_MSG_STREAM;
            gfps_psm_rsp.state = GFPS_PSM_STATE_READY;
            gfps_psm_rsp_data_set(&gfps_psm_rsp);
        }
        else
#endif
        {
            app_gfps_device_check_state((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }
    }
    else
    {
        //rws mode, wait role decided info, will be handled in app_gfps_device_handle_engage_role_decided
    }
}

/**
 * @brief handle legacy BT radio mode
 *
 * @param radio_mode
 */
void app_gfps_device_handle_radio_mode(T_BT_DEVICE_MODE radio_mode)
{
    APP_PRINT_TRACE1("app_gfps_device_handle_radio_mode: radio_mode %d", radio_mode);
    app_gfps_device_check_state((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
}

#if F_APP_ERWS_SUPPORT
void app_gfps_device_handle_role_swap(T_REMOTE_SESSION_ROLE device_role)
{
    APP_PRINT_TRACE1("app_gfps_device_handle_role_swap: device_role %d", device_role);

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        app_gfps_le_device_adv_start(device_role);
    }
    else
#endif
    {
        app_gfps_device_check_state(device_role);
    }

    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            gfps_set_identity_address(app_cfg_nv.bud_local_addr, app_cfg_nv.bud_peer_addr, true);
            //from google: primary bud shall distribute linkkey GFPS_LE_SMP_DIST_LINK_KEY
            uint8_t init_key = GFPS_LE_SMP_DIST_ENC_KEY | GFPS_LE_SMP_DIST_ID_KEY | GFPS_LE_SMP_DIST_LINK_KEY;
            uint8_t response_key = GFPS_LE_SMP_DIST_ENC_KEY | GFPS_LE_SMP_DIST_ID_KEY |
                                   GFPS_LE_SMP_DIST_LINK_KEY;
            le_bond_cfg_local_key_distribute(init_key, response_key);

            T_GFPS_PSM_RSP gfps_psm_rsp;
            gfps_psm_rsp.psm = GFPS_PSM_MSG_STREAM;
            gfps_psm_rsp.state = GFPS_PSM_STATE_READY;
            gfps_psm_rsp_data_set(&gfps_psm_rsp);
        }
#endif
        app_gfps_battery_info_report(GFPS_BATTERY_REPORT_ROLESWAP_SUCCESS);
    }
    else
    {
#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            gfps_set_identity_address(app_cfg_nv.bud_local_addr, app_cfg_nv.bud_peer_addr, true);
            //from google: secondary bud shall not distribute linkkey GFPS_LE_SMP_DIST_LINK_KEY
            uint8_t init_key = GFPS_LE_SMP_DIST_ENC_KEY | GFPS_LE_SMP_DIST_ID_KEY;
            uint8_t response_key = GFPS_LE_SMP_DIST_ENC_KEY | GFPS_LE_SMP_DIST_ID_KEY;
            le_bond_cfg_local_key_distribute(init_key, response_key);

            T_GFPS_PSM_RSP gfps_psm_rsp;
            gfps_psm_rsp.psm = GFPS_PSM_MSG_STREAM;
            gfps_psm_rsp.state = GFPS_PSM_STATE_NOT_AVAILABLE;
            gfps_psm_rsp_data_set(&gfps_psm_rsp);
        }
#endif
    }
}
#endif

void app_gfps_device_handle_b2s_link_disconnected(void)
{
    APP_PRINT_TRACE0("app_gfps_device_handle_b2s_link_disconnected");

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

void app_gfps_device_handle_ble_link_disconnected(uint8_t local_disc_cause)
{
    APP_PRINT_TRACE2("app_gfps_device_handle_ble_link_disconnected: local_disc_cause %d, force enter pair mode %d",
                     local_disc_cause, extend_app_cfg_const.gfps_le_disconn_force_enter_pairing_mode);

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        if (local_disc_cause == LE_LOCAL_DISC_CAUSE_GFPS_LINK_FULL)
        {
            return;
        }

        if (extend_app_cfg_const.gfps_le_disconn_force_enter_pairing_mode)
        {
            app_gfps_force_enter_pairing_mode(GFPS_LE_DISCONN_FORCE_ENTER_PAIR_MODE);
            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }
        else
        {
            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
        }
    }
    else
#endif
    {
        app_gfps_device_check_state((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
    }
}

void app_gfps_device_handle_b2s_ble_bonded(uint8_t conn_id, uint8_t *p_remote_identity_addr)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(conn_id);

    if (p_link && p_link->gfps_link.is_gfps_pairing)
    {
        APP_PRINT_INFO2("app_gfps_device_handle_b2s_ble_bonded: is_gfps_pairing %d, remote identity address %b",
                        p_link->gfps_link.is_gfps_pairing, TRACE_BDADDR(p_remote_identity_addr));

        memcpy(p_link->gfps_link.edr_bond_bd_addr, p_remote_identity_addr, 6);

        T_GFPS_PAIRING_STATUS gfps_pair_status = GFPS_PAIRING_STATUS_IDLE;
        gfps_set_pairing_status(gfps_pair_status);
    }
}

#if F_APP_ERWS_SUPPORT
void app_gfps_device_handle_remote_conn_cmpl(void)
{
#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_gfps_finder_set_ring_param();
    }
#endif

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("app_gfps_device_handle_remote_conn_cmpl: pri sync account key and name");
#if GFPS_FINDER_SUPPORT
        if (extend_app_cfg_const.gfps_finder_support)
        {
//            app_gfps_remote_owner_address_sync();
        }
#endif
        app_gfps_account_key_remote_sync();

#if GFPS_PERSONALIZED_NAME_SUPPORT
        app_gfps_personalized_name_remote_sync();
#endif

        app_gfps_battery_info_report(GFPS_BATTERY_REPORT_b2b_CONNECT_SUCCESS);

#if GFPS_FINDER_SUPPORT
        if (extend_app_cfg_const.gfps_finder_support)
        {
            T_GFPS_FINDER_BEACON_STATE beacon_state = GFPS_FINDER_BEACON_STATE_ON;
            app_gfps_finder_enter_beacon_state(beacon_state);
        }
#endif

#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            gfps_set_identity_address(app_cfg_nv.bud_local_addr, app_cfg_nv.bud_peer_addr, true);

            T_GFPS_PSM_RSP gfps_psm_rsp;
            gfps_psm_rsp.psm = GFPS_PSM_MSG_STREAM;
            gfps_psm_rsp.state = GFPS_PSM_STATE_READY;
            gfps_psm_rsp_data_set(&gfps_psm_rsp);
        }
#endif
    }
    else
    {
        T_GFPS_PSM_RSP gfps_psm_rsp;
        gfps_psm_rsp.psm = GFPS_PSM_MSG_STREAM;
        gfps_psm_rsp.state = GFPS_PSM_STATE_NOT_AVAILABLE;
        gfps_psm_rsp_data_set(&gfps_psm_rsp);
    }
}

void app_gfps_device_handle_remote_disconnect_cmpl(void)
{
#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_gfps_finder_set_ring_param();
    }
#endif

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_gfps_battery_info_report(GFPS_BATTERY_REPORT_b2b_DISCONNECT);

#if GFPS_LE_DEVICE_SUPPORT
        if (extend_app_cfg_const.gfps_le_device_support)
        {
            gfps_set_identity_address(app_cfg_nv.bud_local_addr, app_cfg_nv.bud_peer_addr, false);
        }
#endif
    }
}
#endif

void app_gfps_device_handle_factory_reset(void)
{
    app_gfps_account_key_clear();

#if GFPS_PERSONALIZED_NAME_SUPPORT
    app_gfps_personalized_name_clear();
#endif

#if GFPS_FINDER_SUPPORT
    if (extend_app_cfg_const.gfps_finder_support)
    {
        app_gfps_finder_handle_factory_reset();
    }
#endif
}
#endif
