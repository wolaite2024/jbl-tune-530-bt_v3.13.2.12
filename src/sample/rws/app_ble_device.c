/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "gap_bond_le.h"
#include "sysm.h"
#include "btm.h"
#include "app_cfg.h"
#include "app_key_process.h"
#include "app_ble_gap.h"
#include "app_bt_policy_api.h"
#include "app_ble_common_adv.h"
#include "app_main.h"
#include "app_ble_bond.h"
#include "app_charger.h"
#include "ble_adv_data.h"
#include "app_tts.h"
#include "iap_stream.h"
#include "spp_stream.h"
#include "app_ble_rand_addr_mgr.h"
#include "app_device.h"
#include "string.h"
#include "platform_utils.h"
#if F_APP_BOND_MGR_BLE_SYNC
#include "ble_bond_sync.h"
#endif
#if F_APP_BOND_MGR_SUPPORT
#include "bt_bond_le.h"
#include "app_bond.h"
#endif
#if F_APP_IAP_SUPPORT
#include "app_iap.h"
#endif

#if F_APP_IAP_RTK_SUPPORT
#include "app_iap_rtk.h"
#endif

#if GFPS_FEATURE_SUPPORT
#include "app_gfps_device.h"
#include "app_gfps_finder_adv.h"
#include "app_gfps.h"
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
#include "app_ble_swift_pair.h"
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
#include "app_rtk_fast_pair_adv.h"
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
#include "fmna_connection.h"
#include "app_findmy_ble_adv.h"
#endif

#include "app_harman_ble.h"

#if F_APP_RWS_BLE_USE_RPA_SUPPORT
static void app_ble_device_irk_handle_power_on(void);
#endif

void app_ble_device_handle_factory_reset(void)
{
    APP_PRINT_INFO0("app_ble_device_handle_factory_reset");
#if F_APP_BOND_MGR_SUPPORT

#if F_APP_COMMON_DONGLE_SUPPORT
#if F_APP_KEEP_LEA_BOND && (F_APP_GAMING_DONGLE_SUPPORT == 0)
    app_bond_le_clear_non_lea_keys();
#endif

#else
    bt_le_clear_all_keys();
#endif

#else
    le_bond_clear_all_keys();
#endif
#if GFPS_FEATURE_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_device_handle_factory_reset();
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_factory_reset();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_factory_reset();
    }
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
    fmna_connection_set_is_fmna_paired(false);
#endif
}

void app_ble_rtk_adv_start(void)
{
    APP_PRINT_INFO2("app_ble_rtk_adv_start: timer_ota_adv_timeout: %d, enable_power_on_adv_with_timeout: %d",
                    app_cfg_const.timer_ota_adv_timeout,
                    app_cfg_const.enable_power_on_adv_with_timeout);
    if (app_cfg_const.timer_ota_adv_timeout == 0)
    {
        /*power on always advertising*/
        app_ble_common_adv_start_rws(0);
    }
    else if (app_cfg_const.enable_power_on_adv_with_timeout)
    {
        /*power on advertisng with timeout*/
        app_ble_common_adv_start_rws(app_cfg_const.timer_ota_adv_timeout * 100);
    }

#if HARMAN_REQ_REMOTE_DEVICE_NAME_TIME
    T_APP_LE_LINK *p_le_link;

    p_le_link = app_link_find_le_link_by_conn_id(app_ble_common_adv_get_conn_id());
    if ((app_link_get_b2s_link_num()) &&
        ((app_ble_common_adv_get_conn_id() == 0xFF) ||
         ((p_le_link != NULL) && (p_le_link->state != LE_LINK_STATE_CONNECTED))))
    {
        app_harman_req_remote_device_name_timer_start();
    }
#endif
}

void app_ble_device_handle_power_on(void)
{
    APP_PRINT_INFO0("app_ble_device_handle_power_on");
    bool is_pairing = app_key_is_enter_pairing();
    ble_ext_adv_print_info();
    ble_adv_data_enable();

#if F_APP_RWS_BLE_USE_RPA_SUPPORT
    app_ble_device_irk_handle_power_on();
#endif

    if ((app_cfg_const.rtk_app_adv_support)
        && (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
#if F_APP_HARMAN_FEATURE_SUPPORT
        && (app_link_get_connected_src_num())
#endif
       )
    {
        app_ble_rtk_adv_start();
    }

#if GFPS_FEATURE_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_device_handle_power_on(is_pairing);
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_accessory_handle_power_on(is_pairing);
    }
#endif

#if DMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.dma_support)
    {
        app_dma_ble_adv_start();
    }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_power_on(is_pairing);
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_power_on(is_pairing);
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_handle_power_on(is_pairing);
    }
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
    if (app_cfg_const.swift_pair_support)
    {
        app_swift_pair_handle_power_on(app_cfg_const.timer_swift_pair_adv_timeout * 100);
    }
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
    if (app_cfg_const.enable_rtk_fast_pair_adv)
    {
        app_rtk_fast_pair_handle_power_on();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_power_on();
    }
#endif
}

void app_ble_device_handle_power_off(void)
{
    APP_PRINT_INFO0("app_ble_device_handle_power_off");

    ble_adv_data_disable();

    if (app_cfg_const.rtk_app_adv_support)
    {
        if (app_ble_common_adv_get_state() != BLE_EXT_ADV_MGR_ADV_DISABLED)
        {
            app_ble_common_adv_stop(APP_STOP_ADV_CAUSE_POWER_OFF);
        }
    }
#if F_APP_TTS_SUPPORT
    if (app_cfg_const.tts_support)
    {
        app_tts_handle_power_off();
    }
#endif

#if GFPS_FEATURE_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_device_handle_power_off();
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_accessory_handle_power_off();
    }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_power_off();
    }
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_power_off();
    }
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
    if (app_cfg_const.swift_pair_support)
    {
        app_swift_pair_handle_power_off();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_power_off();
    }
#endif

    app_ble_gap_disconnect_all(LE_LOCAL_DISC_CAUSE_POWER_OFF);
}

void app_ble_device_handle_radio_mode(T_BT_DEVICE_MODE radio_mode)
{
    static T_BT_DEVICE_MODE radio_mode_pre = BT_DEVICE_MODE_IDLE;
    APP_PRINT_INFO2("app_ble_device_handle_radio_mode: radio_mode %d radio_mode_pre %d", radio_mode,
                    radio_mode_pre);
#if GFPS_FEATURE_SUPPORT
    if (extend_app_cfg_const.gfps_support &&
        !extend_app_cfg_const.gfps_le_device_support)
    {
        app_gfps_device_handle_radio_mode(radio_mode);
    }
#endif

    if (radio_mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
    {
        //rtk_adv_enable();
    }
    else
    {
        //rtk_adv_disable();
#if F_APP_DURIAN_SUPPORT
        if (radio_mode_pre == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
        {
            app_durian_adv_pairing_finish();
        }
#endif
    }

    radio_mode_pre = radio_mode;
}

void app_ble_device_handle_enter_pair_mode(void)
{
    APP_PRINT_INFO0("app_ble_device_handle_enter_pair_mode");

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_enter_pair_mode();
    }
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_enter_pair_mode();
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_accessory_handle_enter_pair_mode();
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_handle_enter_pair_mode();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_enter_pair_mode();
    }
#endif

#if GFPS_LE_DEVICE_SUPPORT
    if (extend_app_cfg_const.gfps_le_device_support)
    {
        APP_PRINT_INFO0("app_ble_device_handle_enter_pair_mode: gfps force enter pairing mode");
        app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
    }
#endif

    if ((app_cfg_const.rtk_app_adv_support)
#if F_APP_HARMAN_FEATURE_SUPPORT
        && (app_link_get_connected_src_num())
#endif
       )
    {
        app_ble_rtk_adv_start();
    }
}

#if F_APP_RWS_BLE_USE_RPA_SUPPORT
static void app_ble_device_irk_handle_power_on(void)
{
    uint8_t temp_irk[16] = {0};

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        if (memcmp(app_cfg_nv.primary_irk, temp_irk, 16) == 0)
        {
            uint8_t primary_irk[16] = {0x00, 0x11, 0x22, 0x33, 0x00, 0x11, 0x22, 0x33, 0x00, 0x11, 0x22, 0x33, 0x00, 0x11, 0x22, 0x33};
            uint8_t *p = primary_irk;

            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));

            memcpy(app_cfg_nv.primary_irk, primary_irk, 16);
            app_cfg_store(app_cfg_nv.primary_irk, 16);
        }

        bt_le_set_local_irk(GAP_IDENT_ADDR_PUBLIC, app_cfg_nv.bud_local_addr, app_cfg_nv.primary_irk);
        /*patch need modify le_bond_set_param and call sm_setup_local_irk when set GAP_PARAM_BOND_SET_LOCAL_IRK so it can work after stack ready*/
        le_bond_set_param(GAP_PARAM_BOND_SET_LOCAL_IRK, GAP_KEY_LEN, app_cfg_nv.primary_irk);

        /*use app_cfg_nv.primary_irk generate RPA*/
#if GFPS_FEATURE_SUPPORT
        app_gfps_update_rpa();
#if GFPS_FINDER_SUPPORT
        if (extend_app_cfg_const.gfps_finder_support)
        {
            gfps_finder_adv_update_rpa();
        }
#endif
#endif
    }

    APP_PRINT_INFO4("app_ble_device_irk_handle_power_on: bud_role %d, local_addr %b, primary_irk %b, secondary_irk %b",
                    app_cfg_nv.bud_role, TRACE_BDADDR(app_cfg_nv.bud_local_addr),
                    TRACE_BINARY(16, app_cfg_nv.primary_irk), TRACE_BINARY(16, app_cfg_nv.secondary_irk));
}

static void app_ble_device_rpa_handle_engage_role_decided(void)
{
    uint8_t temp_irk[16] = {0};

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (memcmp(app_cfg_nv.primary_irk, temp_irk, 16) == 0)
        {
            uint8_t primary_irk[16] = {0x00, 0x11, 0x22, 0x33, 0x00, 0x11, 0x22, 0x33, 0x00, 0x11, 0x22, 0x33, 0x00, 0x11, 0x22, 0x33};
            uint8_t *p = primary_irk;

            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));

            memcpy(app_cfg_nv.primary_irk, primary_irk, 16);
            app_cfg_store(app_cfg_nv.primary_irk, 16);
            bt_le_set_local_irk(GAP_IDENT_ADDR_PUBLIC, app_cfg_nv.bud_local_addr, app_cfg_nv.primary_irk);
            /*patch need modify le_bond_set_param and call sm_setup_local_irk when set GAP_PARAM_BOND_SET_LOCAL_IRK so it can work after stack ready*/
            le_bond_set_param(GAP_PARAM_BOND_SET_LOCAL_IRK, GAP_KEY_LEN, app_cfg_nv.primary_irk);
        }
    }
    else
    {
        if (memcmp(app_cfg_nv.secondary_irk, temp_irk, 16) == 0)
        {
            uint8_t secondary_irk[16] = {0x44, 0x55, 0x66, 0x77, 0x44, 0x55, 0x66, 0x77, 0x44, 0x55, 0x66, 0x77, 0x44, 0x55, 0x66, 0x77};
            uint8_t *p = secondary_irk;

            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
            BE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));

            memcpy(app_cfg_nv.secondary_irk, secondary_irk, 16);
            app_cfg_store(app_cfg_nv.secondary_irk, 16);
            bt_le_set_local_irk(GAP_IDENT_ADDR_PUBLIC, app_cfg_nv.bud_local_addr, app_cfg_nv.secondary_irk);
            le_bond_set_param(GAP_PARAM_BOND_SET_LOCAL_IRK, GAP_KEY_LEN, app_cfg_nv.secondary_irk);
        }
    }

    APP_PRINT_INFO4("app_ble_device_rpa_handle_engage_role_decided: bud_role %d, local_addr %b, primary_irk %b, secondary_irk %b",
                    app_cfg_nv.bud_role, TRACE_BDADDR(app_cfg_nv.bud_local_addr),
                    TRACE_BINARY(16, app_cfg_nv.primary_irk), TRACE_BINARY(16, app_cfg_nv.secondary_irk));
}

static void app_ble_device_rpa_handle_role_swap(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_bond_set_param(GAP_PARAM_BOND_SET_LOCAL_IRK, GAP_KEY_LEN, app_cfg_nv.primary_irk);
    }
    else
    {
        le_bond_set_param(GAP_PARAM_BOND_SET_LOCAL_IRK, GAP_KEY_LEN, app_cfg_nv.secondary_irk);
    }

    APP_PRINT_INFO4("app_ble_device_rpa_handle_role_swap: bud_role %d, local_addr %b, primary_irk %b, secondary_irk %b",
                    app_cfg_nv.bud_role, TRACE_BDADDR(app_cfg_nv.bud_local_addr),
                    TRACE_BINARY(16, app_cfg_nv.primary_irk), TRACE_BINARY(16, app_cfg_nv.secondary_irk));
}
#endif

void app_ble_device_handle_engage_role_decided(T_BT_DEVICE_MODE radio_mode)
{
    APP_PRINT_INFO3("app_ble_device_handle_engage_role_decided: bud_role %d, radio_mode %d, first_engaged %d",
                    app_cfg_nv.bud_role, radio_mode, app_cfg_nv.first_engaged);
#if F_APP_RWS_BLE_USE_RPA_SUPPORT
    app_ble_device_rpa_handle_engage_role_decided();
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_roleswap_handle_engage_role_decided();
    }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_engage_role_decided(radio_mode);
    }
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_engage_role_decided(radio_mode);
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_roleswap_role_decide();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_engage_role_decided();
    }
#endif

    if (app_cfg_const.rtk_app_adv_support)
    {
        app_ble_common_adv_handle_engage_role_decided();
    }

    app_ble_rand_addr_handle_role_decieded();
}

#if F_APP_ERWS_SUPPORT
void app_ble_device_handle_role_swap(T_BT_EVENT_PARAM_REMOTE_ROLESWAP_STATUS *p_role_swap_status)
{
    APP_PRINT_INFO2("app_ble_device_handle_role_swap: status 0x%02x, device_role %d",
                    p_role_swap_status->status, p_role_swap_status->device_role);

#if F_APP_RWS_BLE_USE_RPA_SUPPORT
    app_ble_device_rpa_handle_role_swap();
#endif

    if (p_role_swap_status->status == BT_ROLESWAP_STATUS_SUCCESS)
    {
#if GFPS_FEATURE_SUPPORT
        if (extend_app_cfg_const.gfps_support)
        {
            app_gfps_device_handle_role_swap(p_role_swap_status->device_role);
        }
#endif

#if AMA_FEATURE_SUPPORT
        if (extend_app_cfg_const.ama_support)
        {
            app_ama_roleswap_handle_role_swap_success(p_role_swap_status->device_role);
        }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaoai_support)
        {
            app_xiaoai_handle_role_swap_success(p_role_swap_status->device_role);
        }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaowei_support)
        {
            app_xiaowei_handle_role_swap_success(p_role_swap_status->device_role);
        }
#endif

#if BISTO_FEATURE_SUPPORT
        if (extend_app_cfg_const.bisto_support)
        {
            app_bisto_roleswap_success(p_role_swap_status->device_role);
        }
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
        app_teams_rws_handle_role_swap_success(p_role_swap_status->device_role);
#endif

#if F_APP_TUYA_SUPPORT
        if (extend_app_cfg_const.tuya_support)
        {
            app_tuya_handle_role_swap_success(p_role_swap_status->device_role);
        }
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
        app_findmy_handle_role_swap_success(p_role_swap_status->device_role);
#endif

        if (app_cfg_const.rtk_app_adv_support)
        {
            app_ble_common_adv_handle_roleswap(p_role_swap_status->device_role);
        }
    }
    else if (p_role_swap_status->status == BT_ROLESWAP_STATUS_FAIL)
    {
#if XM_XIAOAI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaoai_support)
        {
            app_xiaoai_handle_role_swap_fail(p_role_swap_status->device_role);
        }
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaowei_support)
        {
            app_xiaowei_handle_role_swap_fail(p_role_swap_status->device_role);
        }
#endif

#if AMA_FEATURE_SUPPORT
        if (extend_app_cfg_const.ama_support)
        {
            app_ama_roleswap_handle_role_swap_fail(p_role_swap_status->device_role);
        }
#endif

#if BISTO_FEATURE_SUPPORT
        if (extend_app_cfg_const.bisto_support)
        {
            app_bisto_roleswap_failed();
        }
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
        app_teams_rws_handle_role_swap_failed(p_role_swap_status->device_role);
#endif

#if F_APP_TUYA_SUPPORT
        if (extend_app_cfg_const.tuya_support)
        {
            app_tuya_handle_role_swap_fail(p_role_swap_status->device_role);
        }
#endif

        if (app_cfg_const.rtk_app_adv_support)
        {
            app_ble_common_adv_handle_role_swap_fail(p_role_swap_status->device_role);
        }
    }
}

void app_ble_device_handle_remote_conn_cmpl(void)
{
#if AMA_FEATURE_SUPPORT | BISTO_FEATURE_SUPPORT | F_APP_IAP_RTK_SUPPORT
    iap_stream_send_all_info_to_sec();
    spp_stream_send_all_info_to_sec();
#endif

#if F_APP_BLE_BOND_SYNC_SUPPORT
    app_ble_bond_sync_all_info();
#endif

#if F_APP_BOND_MGR_BLE_SYNC
    ble_bond_sync_handle_remote_conn_cmpl();
#endif

#if GFPS_FEATURE_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_device_handle_remote_conn_cmpl();
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_roleswap_handle_remote_conn_cmpl();
    }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_remote_conn_cmpl();
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_remote_conn_cmpl();
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_remote_conn_cmpl();
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_roleswap_handle_remote_conn_cmpl();
    }
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
    if (app_cfg_const.enable_rtk_fast_pair_adv)
    {
        app_rtk_fast_pair_handle_remote_conn_cmpl();
    }
#endif

#if F_APP_IAP_SUPPORT
    app_iap_handle_remote_conn_cmpl();
#endif

#if F_APP_IAP_RTK_SUPPORT
    app_iap_rtk_handle_remote_conn_cmpl();
#endif

    app_ble_rand_addr_handle_remote_conn_cmpl();
}

void app_ble_device_handle_remote_disconn_cmpl(void)
{
#if F_APP_BLE_BOND_SYNC_SUPPORT
    app_ble_bond_sync_recovery();
#endif

#if F_APP_BOND_MGR_BLE_SYNC
    ble_bond_sync_handle_disconn_cmpl();
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_remote_disconn_cmpl();
    }
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_remote_disconn_cmpl();
    }
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_roleswap_handle_remote_disconn_cmpl();
    }
#endif

#if GFPS_FEATURE_SUPPORT
    app_gfps_device_handle_remote_disconnect_cmpl();
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_remote_disconn_cmpl();
    }
#endif
}
#endif

void app_ble_device_handle_b2s_bt_connected(uint8_t *bd_addr)
{
#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_device_handle_b2s_connected(bd_addr);
    }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_b2s_bt_connected(bd_addr);
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_bt_handle_b2s_connected(bd_addr);
    }
#endif

#if DMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.dma_support)
    {
        app_dma_device_handle_b2s_connected(bd_addr);
    }
#endif
#if GFPS_FEATURE_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        app_gfps_handle_b2s_connected(bd_addr);
    }
#endif
#if F_APP_TEAMS_BT_POLICY
    T_TEAMS_BLE_ADV_MGR *p_teams_ble_adv_mgr = NULL;
    T_APP_BR_LINK *p_link = NULL;
    if (le_find_key_entry(bd_addr, GAP_REMOTE_ADDR_LE_PUBLIC))
    {
        p_link = app_link_find_br_link(bd_addr);
        if (p_link)
        {
            p_teams_ble_adv_mgr = app_teams_ble_policy_find_ble_adv_mgr_by_idx(p_link->id);
            if (p_teams_ble_adv_mgr)
            {
                app_teams_ble_policy_set_adv_param(p_teams_ble_adv_mgr->handle, bd_addr);
                app_teams_ble_policy_adv_start(p_teams_ble_adv_mgr->handle, APP_TEAMS_BLE_POLICY_ADV_DURATION);
            }
        }
    }
#endif

}

void app_ble_device_handle_b2s_bt_disconnected(uint8_t *bd_addr)
{
#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        app_ama_device_handle_b2s_disconnected(bd_addr);
    }
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support)
    {
        app_xiaoai_handle_b2s_bt_disconnected(bd_addr);
    }
#endif
#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaowei_support)
    {
        app_xiaowei_handle_b2s_bt_disconnected(bd_addr);
    }
#endif

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_bt_handle_b2s_disconnected(bd_addr);
    }
#endif

#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        app_tuya_handle_b2s_bt_disconnected(bd_addr);
    }
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
    APP_PRINT_INFO1("app_ble_handle_b2s_bt_disconnected b2s:%d", app_link_get_connected_src_num());
#if HARMAN_BONDING_LEGACY_AND_BLE_LINK
    T_APP_LE_LINK *le_link = NULL;

    le_link = app_find_le_link_by_addr(bd_addr);
    if (le_link != NULL)
    {
        harman_le_common_disc();
    }
#else
    if (app_link_get_connected_src_num() == 0)
    {
        harman_le_common_disc();
    }
#endif
#endif
}

#if F_APP_ERWS_SUPPORT
static void app_ble_device_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        app_ble_device_handle_role_swap(&param->remote_roleswap_status);
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_ble_device_bt_cback: event_type 0x%04x", event_type);
    }
}
#endif

bool app_ble_device_is_active(void)
{
    bool ret = false;

#if XM_XIAOAI_FEATURE_SUPPORT
    if (app_xiaoai_is_recording())
    {
        ret = true;
    }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
    if (app_xiaowei_is_recording())
    {
        ret = true;
    }
#endif

    return ret;
}

void app_ble_device_init(void)
{
#if F_APP_ERWS_SUPPORT
    bt_mgr_cback_register(app_ble_device_bt_cback);
#endif
}
