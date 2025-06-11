#if F_APP_GAMING_DONGLE_SUPPORT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "gap_vendor.h"
#include "sysm.h"
#include "bt_bond.h"
#include "btm.h"
#include "app_cfg.h"
#include "app_dongle_data_ctrl.h"
#include "app_dongle_dual_mode.h"
#include "trace.h"
#include "ble_ext_adv.h"
#include "app_bt_policy_api.h"
#include "app_device.h"
#include "app_bond.h"
#include "app_mmi.h"
#include "app_auto_power_off.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_audio_policy.h"
#include "app_dongle_source_ctrl.h"
#include "app_dongle_common.h"
#include "app_relay.h"
#include "app_a2dp.h"
#include "app_link_util.h"
#include "app_timer.h"
#include "app_util.h"
#include "app_bt_policy_int.h"
#include "app_bt_policy_api.h"
#include "audio.h"
#include "app_ble_common_adv.h"
#include "app_adv_stop_cause.h"

#if F_APP_SLIDE_SWITCH_SUPPORT
#include "app_slide_switch.h"
#endif

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
#include "app_lea_adv.h"
#include "app_lea_mgr.h"
#include "eq.h"
#endif

#define DONGLE_ADV_INTERVAL 0xa0

#if F_APP_GAMING_DONGLE_DELAY_TX_WHEN_BT_PLAYING
#include "app_dongle_data_ctrl.h"
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
#include "multitopology_ctrl.h"
#include "app_bt_sniffing.h"
#endif

#define DONGLE_DISCONNECT_RSSI_THRESHOLD    -65


typedef enum
{
    APP_TIMER_DISALLOW_DONGLE_A2DP                   = 0x00,
    APP_TIMER_LOW_RSSI_DISCONNECT_DONGLE             = 0x01,
    APP_TIMER_CHANGE_HEADSET_ADV_INTERVAL            = 0x02,
} T_APP_DONGLE_TIMER;

typedef enum
{
    DONGLE_ENABLE_BLE_ADV,
    DONGLE_DISABLE_BLE_ADV,
    DONGLE_LINKBACK_DONGLE,
    DONGLE_LINKBACK_PHONE,
    DONGLE_ENABLE_INQUIRY_SCAN,
    DONGLE_DISABLE_INQUIRY_SCAN,
    DONGLE_DISABLE_LINKBACK,
} T_APP_DUAL_MODE_ACTION;

typedef enum
{
    GAMING_SETTING_NONE,
    GAMING_SETTING_LINKBACK,
    GAMING_SETTING_MULTI_SRC_PLAYING,
    GAMING_SETTING_MULTI_SRC_WITH_BLE_CONNECTED,
} T_DONGLE_GAMING_SETTING;

typedef enum
{
    DONGLE_SYNC_EVENT_A2DP_DISC,
    DONGLE_SYNC_EVENT_ACL_DISC,
    DONGLE_SYNC_EVENT_ACL_CONN,
    DONGLE_SYNC_EVENT_BT_A2DP_PLAY_START,
    DONGLE_SYNC_EVENT_BT_A2DP_PLAY_STOP,
    DONGLE_SYNC_EVNET_DISALLOW_DONGLE_TX,
} T_DONGLE_SYNC_STATUS_EVENT;

typedef enum
{
    PHONE_EVENT_A2DP_START,
    PHONE_EVENT_A2DP_STOP,
    PHONE_EVENT_HFP_START,
    PHONE_EVENT_HFP_STOP,
    PHONE_EVENT_A2DP_DISC,
    PHONE_EVENT_PHONE_DISC,
} T_PHONE_STREAM_EVENT;

typedef struct
{
    T_DONGLE_GAMING_SETTING setting;
    uint8_t interval;
    uint8_t rsvd_slot;
    uint8_t latency;
} T_DONGLE_GAMING_INFO;

T_HEADSET_STATUS headset_status;

static const T_DONGLE_GAMING_INFO gaming_setting[] =
{
    {GAMING_SETTING_NONE,                  0,  0,  0},
    {GAMING_SETTING_LINKBACK,              30, 22, ULTRA_LOW_LATENCY_MS_WITH_LINKBACK},
    {GAMING_SETTING_MULTI_SRC_PLAYING,     58, 50, ULTRA_LOW_LATENCY_MS_WITH_MULTI_SRC},
    {GAMING_SETTING_MULTI_SRC_WITH_BLE_CONNECTED, 58, 50, ULTRA_LOW_LATENCY_MS_WITH_MULTI_SRC},
};

T_DONGLE_CONTROL_DATA dongle_ctrl_data;

static T_APP_DONGLE_MODE dongle_mode = STANDBY_MODE_24G;
static uint8_t dongle_adv_handle = 0xff;
static T_DONGLE_BLE_DATA dongle_adv;
static uint8_t app_dongle_timer_id = 0;
static uint8_t timer_idx_disallow_dongle_a2dp = 0;
static uint8_t timer_idx_change_headset_adv_interval = 0;

#if F_APP_LOW_RSSI_DISCONNECT_DONGLE
static uint8_t timer_idx_low_rssi_disconnect_dongle = 0;
#endif

T_DONGLE_STATUS dongle_status;

void app_dongle_sync_headset_status(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_dongle_send_cmd(DONGLE_CMD_SYNC_STATUS, (uint8_t *)&headset_status, sizeof(headset_status));
    }
}

static void app_dongle_update_phone_stream_status(T_PHONE_STREAM_EVENT event)
{
    T_PHONE_STREAM_STATUS pre_status = headset_status.phone_status;
    T_PHONE_STREAM_STATUS new_status = headset_status.phone_status;

    if (event == PHONE_EVENT_A2DP_START)
    {
        new_status = PHONE_STREAM_A2DP;
    }
    else if (event == PHONE_EVENT_A2DP_STOP || event == PHONE_EVENT_A2DP_DISC)
    {
        if (pre_status == PHONE_STREAM_A2DP)
        {
            new_status = PHONE_STREAM_IDLE;
        }
    }
    else if (event == PHONE_EVENT_HFP_START)
    {
        new_status = PHONE_STREAM_HFP;
    }
    else if (event == PHONE_EVENT_HFP_STOP)
    {
        if (pre_status == PHONE_STREAM_HFP)
        {
            new_status = PHONE_STREAM_IDLE;
        }
    }
    else if (event == PHONE_EVENT_PHONE_DISC)
    {
        new_status = PHONE_STREAM_IDLE;
    }

    if (new_status != pre_status)
    {
        APP_PRINT_TRACE2("app_dongle_update_phone_stream_status: event %d status %d", event, new_status);

        headset_status.phone_status = new_status;

        app_dongle_sync_headset_status();

        app_relay_async_single(APP_MODULE_TYPE_DONGLE_DUAL_MODE, APP_REMOTE_MSG_SYNC_HEADSET_STATUS);
    }
}

static void app_dongle_cancel_linkback(void)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));
    bt_param.not_check_addr_flag = true;

    app_bt_policy_state_machine(EVENT_STOP_LINKBACK, &bt_param);

}

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
T_BT_EVENT_PARAM_A2DP_STREAM_START_IND a2dp_stream_para;
T_BT_EVENT_PARAM_SCO_CONN_CMPL sco_conn_cmpl_param;
#endif

static bool app_dongle_check_mode_24g(T_APP_DONGLE_MODE mode)
{
    bool ret = false;

    if (mode == STANDBY_MODE_24G || mode == LINKBACK_MODE_24G || mode == PAIRING_MODE_24G)
    {
        ret = true;
    }

    return ret;
}

static bool app_dongle_need_adv_when_power_on(void)
{
    bool ret = false;

    if (app_cfg_const.enable_dongle_dual_mode)
    {
        if (app_cfg_const.enable_multi_link)
        {
            ret = true;
        }
        else
        {
            if (app_dongle_is_24g_mode())
            {
                ret = true;
            }
        }

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
        /* always start cis adv when power on */
        ret = true;
#endif
    }

    return ret;
}

static void app_dongle_adv_get(T_DONGLE_BLE_DATA *adv_data)
{
    adv_data->length = sizeof(T_DONGLE_BLE_DATA) - 1;
    adv_data->type = 0xff;

    adv_data->eir_data.length = sizeof(T_DONGLE_EIR_DATA) - 1;
    adv_data->eir_data.type = 0xff;
    adv_data->eir_data.magic_data[0] = 0x5d;
    adv_data->eir_data.magic_data[1] = 0x00;
    adv_data->eir_data.magic_data[2] = 0x08;
    adv_data->eir_data.headset_type = (remote_session_role_get() == REMOTE_SESSION_ROLE_SINGLE) ? 0x02 :
                                      0x03;
    adv_data->eir_data.sbc_frame_num = 0;
    adv_data->eir_data.pairing_id_1 = (app_cfg_const.rws_custom_uuid >> 8) & 0xFF;
    adv_data->eir_data.pairing_id_2 = app_cfg_const.rws_custom_uuid & 0xFF;

    adv_data->eir_data.feature_set = app_cfg_const.spp_voice_smaple_rate & 0x03;
    adv_data->eir_data.feature_set |= 0 << 2;

#if F_APP_A2DP_CODEC_LC3_SUPPORT
    adv_data->eir_data.feature_set |= 1 << 3;
#endif

    APP_PRINT_TRACE1("app_dongle_adv_get: %b", TRACE_BINARY(sizeof(T_DONGLE_BLE_DATA), adv_data));
}

void app_dongle_adv_stop(void)
{
    if (dongle_ctrl_data.dongle_adv_enabled)
    {
        APP_PRINT_TRACE0("app_dongle_adv_stop");

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
        app_lea_mgr_dev_sm(LEA_ADV_STOP, NULL);
#else
        ble_ext_adv_mgr_disable(dongle_adv_handle, 0);
#endif

        dongle_ctrl_data.dongle_adv_enabled = false;
        dongle_ctrl_data.enable_pairing = false;
    }
}

void app_dongle_adv_start(bool enable_pairing)
{
    uint8_t disallow_reason = 0;

    if (enable_pairing && dongle_ctrl_data.trigger_pairing_non_intentionally)
    {
        APP_PRINT_TRACE0("app_dongle_adv_start: disallow non-intentionally dongle pairing");

        enable_pairing = false;
    }
    dongle_ctrl_data.trigger_pairing_non_intentionally = false;

    if (app_cfg_const.enable_dongle_dual_mode == false)
    {
        disallow_reason = 1;
        goto exit;
    }

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
    if (app_link_get_lea_link_num() > 0)
    {
        disallow_reason = 2;
        goto exit;
    }
#else
    if (app_dongle_get_connected_dongle_link() != NULL &&
        dongle_ctrl_data.switch_pairing_triggered == false)
    {
        disallow_reason = 3;
        goto exit;
    }
#endif

#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
    if (enable_pairing == 0)
    {
        if (app_cfg_const.enable_dongle_dual_mode)
        {
            if (app_cfg_const.enable_multi_link == false)
            {
                if (app_dongle_is_24g_mode() == false)
                {
                    disallow_reason = 4;
                    goto exit;
                }
            }
        }
    }
#endif

    if (app_cfg_const.enable_dongle_multi_pairing)
    {
        if (app_dongle_get_mode() == PAIRING_MODE_BT_OR_BT_24G && enable_pairing == 0)
        {
            disallow_reason = 5;
            goto exit;
        }
    }

    if (app_db.device_state != APP_DEVICE_STATE_ON)
    {
        disallow_reason = 6;
        goto exit;
    }

    if (dongle_ctrl_data.dongle_adv_enabled)
    {
        if (enable_pairing != dongle_ctrl_data.enable_pairing)
        {
            /* for restart purpose */
            app_dongle_adv_stop();
        }
        else
        {
            /* already started */
            disallow_reason = 7;
            goto exit;
        }
    }

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
    app_db.pairing_bit = enable_pairing;

    app_lea_adv_update();
    app_lea_adv_start((uint8_t)LEA_ADV_MODE_PAIRING);
    //app_lea_mgr_tri_mmi_handle_action(MMI_CIG_START, true);
#else
    app_dongle_adv_get(&dongle_adv);
    dongle_adv.pairing_bit = enable_pairing;

    ble_ext_adv_mgr_set_adv_data(dongle_adv_handle, sizeof(dongle_adv), (uint8_t *)&dongle_adv);
    ble_ext_adv_mgr_enable(dongle_adv_handle, 0);
#endif
    dongle_ctrl_data.dongle_adv_enabled = true;
    dongle_ctrl_data.enable_pairing = enable_pairing;

exit:

    APP_PRINT_TRACE2("app_dongle_adv_start: pairing %d reason %d", enable_pairing,
                     disallow_reason);

}

static void app_dongle_change_headset_adv_interval(void)
{
    uint32_t timeout = 0;

    if (app_cfg_const.timer_ota_adv_timeout == 0)
    {
        timeout = 600;
    }
    else
    {
        timeout = app_cfg_const.timer_ota_adv_timeout;
    }

    app_start_timer(&timer_idx_change_headset_adv_interval, "change_headset_adv_interval",
                    app_dongle_timer_id, APP_TIMER_CHANGE_HEADSET_ADV_INTERVAL, 0, false,
                    timeout * 1000);
}

static void app_dongle_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = false;

    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            if (app_dongle_need_adv_when_power_on() &&
                (dongle_ctrl_data.dongle_adv_enabled == false))
            {
#if F_APP_B2B_ENGAGE_IMPROVE_BY_LEA_DONGLE
                // don't start cis adv when power on, start before shaking timeout
#else
                app_dongle_adv_start(false);
#endif
            }

            app_dongle_change_headset_adv_interval();

            handle = true;
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            app_dongle_adv_stop();
        }
        break;

    default:
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE1("app_dongle_dm_cback: event 0x%04x", event_type);

    }
}

static void app_dongle_adjust_gaming_latency(void)
{
    uint8_t active_idx = app_a2dp_get_active_idx();
    uint8_t *active_addr = app_db.br_link[active_idx].bd_addr;
    T_APP_BR_LINK *p_link = app_link_find_br_link(active_addr);

    /* adjust dongle gaming latency if it is active */
    if (p_link && app_link_check_dongle_link(active_addr))
    {
        uint16_t latency_value = app_dongle_get_gaming_latency();
        app_dongle_set_gaming_latency(p_link->a2dp_track_handle, latency_value);
    }
}

static uint8_t app_dongle_get_gaming_setting_idx(T_DONGLE_GAMING_SETTING setting)
{
    uint8_t i = 0;
    uint8_t ret = 0;

    for (i = 0; i < sizeof(gaming_setting) / sizeof(T_DONGLE_GAMING_INFO); i++)
    {
        if (gaming_setting[i].setting == setting)
        {
            ret = i;
            break;
        }
    }

    return ret;
}

void app_dongle_htpoll_control(T_APP_DONGLE_HTPOLL_EVENT event)
{
    uint8_t connected_b2s_num = app_link_get_b2s_link_num();
    uint8_t connected_le_num = app_link_get_le_link_num();
    uint8_t dongle_addr[6] = {0};
    uint8_t target_addr[6] = {0};
    uint8_t cause = 0;
    T_DONGLE_GAMING_SETTING setting = GAMING_SETTING_NONE;
    uint8_t idx = 0;
    static bool htpoll_active = false;
    static uint8_t htpoll_active_addr[6] = {0};
    T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();
    uint8_t connected_device_num = 0;

    connected_device_num = connected_b2s_num + connected_le_num;

    if (p_dongle_link != NULL)
    {
        uint8_t active_idx = app_a2dp_get_active_idx();
        bool is_streaming = false;

        memcpy(dongle_addr, p_dongle_link->bd_addr, 6);

        if (app_db.br_link[active_idx].streaming_fg == true)
        {
            is_streaming = true;
        }

        if (is_streaming && app_hfp_sco_is_connected() == false)
        {
            if (connected_device_num == 1)
            {
#if F_APP_SUPPORT_LINKBACK_WHEN_DONGLE_STREAMING
                if (app_dongle_get_mode() == LINKBACK_MODE_BT_OR_BT_24G)
                {
                    memcpy(target_addr, dongle_addr, 6);

                    setting = GAMING_SETTING_LINKBACK;
                    cause = 1;
                }
                else
#endif
                {
                    cause = 2;
                }
            }
            else if (connected_device_num > 1)
            {
                memcpy(target_addr, app_db.br_link[active_idx].bd_addr, 6);

                if (connected_le_num == 0)
                {
                    setting = GAMING_SETTING_MULTI_SRC_PLAYING;
                    cause = 3;
                }
                else
                {
                    setting = GAMING_SETTING_MULTI_SRC_WITH_BLE_CONNECTED;
                    cause = 4;
                }
            }
        }
        else
        {
            cause = 5;
        }
    }

    idx = app_dongle_get_gaming_setting_idx(setting);

    if (setting != GAMING_SETTING_NONE)
    {
        /* adjust gaming dongle latency */
        app_dongle_adjust_gaming_latency();

        if (htpoll_active)
        {
            /* clear current active and set new one */
            bt_link_traffic_qos_clear(htpoll_active_addr);

            bt_link_periodic_traffic_qos_set(target_addr, gaming_setting[idx].interval,
                                             gaming_setting[idx].rsvd_slot);

            memcpy(htpoll_active_addr, target_addr, 6);
        }
        else
        {
            bt_link_periodic_traffic_qos_set(target_addr, gaming_setting[idx].interval,
                                             gaming_setting[idx].rsvd_slot);

            htpoll_active = true;
            memcpy(htpoll_active_addr, target_addr, 6);
        }
    }
    else
    {
        if (htpoll_active)
        {
            bt_link_traffic_qos_clear(htpoll_active_addr);

            htpoll_active = false;
            memset(htpoll_active_addr, 0, 6);
        }
    }

    APP_PRINT_TRACE8("app_dongle_htpoll_control: event 0x%02x setting %d target_addr %s htpoll_active %d htpoll_active_addr %s (%d:%d) cause %d",
                     event, setting, TRACE_BDADDR(target_addr), htpoll_active, TRACE_BDADDR(htpoll_active_addr),
                     gaming_setting[idx].interval, gaming_setting[idx].rsvd_slot, cause);

}

/*
* subcommand : 0x23
* bd address(6 byte)
* afh policy mode (1 byte): BT = 0, 2.4G = 1
* afh policy priority (1 byte): remote first = 0, local first = 1
*/
static void app_dongle_send_link_info_to_lowerstack(uint8_t *addr)
{
    uint8_t params[9] = {0};

    params[0] = 0x23;

    memcpy(params + 1, addr, 6);

    if (!memcmp(app_db.connected_dongle_addr, addr, 6) ||
        app_link_check_dongle_link(addr))
    {
        params[7] = 1; /* 2.4g */
    }
    else
    {
        params[7] = 0; /* bt */
    }

    params[8] = 1; /* local first; due to earbud is rx */

    APP_PRINT_TRACE1("app_dongle_send_link_info_to_lowerstack: %b", TRACE_BINARY(sizeof(params),
                                                                                 params));

    gap_vendor_cmd_req(0xfd81, sizeof(params), params);
}

static void app_dongle_handle_acl_disc_event(T_BT_EVENT_PARAM *param)
{
    if (app_link_check_dongle_link(param->acl_conn_disconn.bd_addr))
    {
        memset(app_db.connected_dongle_addr, 0, 6);
#if F_APP_LOW_RSSI_DISCONNECT_DONGLE
        app_stop_timer(&timer_idx_low_rssi_disconnect_dongle);
        dongle_ctrl_data.low_rssi_disconnet_dongle = false;
        bt_link_rssi_report(param->acl_conn_disconn.bd_addr, false, 0);
#endif
    }

    /* prevent 2.4g pairing being interrupted when force enter pairing in 2.4g mode only */
    bool curent_is_2_4g_pairing = (dongle_ctrl_data.enable_pairing &&
                                   dongle_ctrl_data.dongle_adv_enabled);

    if (app_cfg_const.enable_dongle_dual_mode)
    {
        if (app_cfg_const.enable_multi_link)
        {
            uint32_t bond_flag;

            if (bt_bond_flag_get(param->acl_conn_disconn.bd_addr, &bond_flag) &&
                (bond_flag & BOND_FLAG_DONGLE) &&
                curent_is_2_4g_pairing == false)
            {
                app_dongle_adv_start(false);
            }
        }
        else
        {
            if (app_dongle_is_24g_mode() && curent_is_2_4g_pairing == false)
            {
                app_dongle_adv_start(false);
            }
        }
    }

#if F_APP_DONGLE_MULTI_PAIRING
    if (dongle_ctrl_data.switch_pairing_triggered)
    {
        app_dongle_switch_pairing_mode();
    }
#endif

    if (dongle_ctrl_data.linkback_src_after_disc)
    {
        dongle_ctrl_data.linkback_src_after_disc = false;
        app_bt_policy_conn_all_b2s();
    }

    app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_ACL_DISC);

    app_dongle_adjust_gaming_latency();
}

static void app_dongle_adv_control(void)
{
    if (dongle_ctrl_data.legacy_dongle_streaming)
    {
        app_dongle_handle_headset_adv(false);
    }
    else
    {
        app_dongle_handle_headset_adv(true);
    }
}

static void app_dongle_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = false;
    T_APP_BR_LINK *p_link = NULL;

    switch (event_type)
    {
    case BT_EVENT_SPP_CONN_CMPL:
        {
            if (app_link_check_dongle_link(param->spp_conn_cmpl.bd_addr))
            {
                app_dongle_sync_headset_status();
            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            if (app_link_check_phone_link(param->hfp_call_status.bd_addr))
            {
                if (param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
                {
                    app_dongle_update_phone_stream_status(PHONE_EVENT_HFP_STOP);
                }
                else
                {
                    app_dongle_update_phone_stream_status(PHONE_EVENT_HFP_START);
                }
            }
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_SCO_CONNECTED);

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            memcpy(&sco_conn_cmpl_param, (uint8_t *)&param->sco_conn_cmpl,
                   sizeof(T_BT_EVENT_PARAM_SCO_CONN_CMPL));
#endif
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_SCO_DISCONNECTED);
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_A2DP_START);

            if (app_link_check_phone_link(param->a2dp_stream_start_ind.bd_addr))
            {
                app_dongle_update_phone_stream_status(PHONE_EVENT_A2DP_START);
            }

            if (app_link_check_dongle_link(param->a2dp_stream_start_ind.bd_addr))
            {
                dongle_ctrl_data.legacy_dongle_streaming = true;
                app_dongle_adv_control();
            }

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            memcpy(&a2dp_stream_para, (uint8_t *)&param->a2dp_stream_start_ind,
                   sizeof(T_BT_EVENT_PARAM_A2DP_STREAM_START_IND));
#endif
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_A2DP_STOP);

            if (app_link_check_phone_link(param->a2dp_stream_start_ind.bd_addr))
            {
                app_dongle_update_phone_stream_status(PHONE_EVENT_A2DP_STOP);
            }

            if (app_link_check_dongle_link(param->a2dp_stream_stop.bd_addr))
            {
                dongle_ctrl_data.legacy_dongle_streaming = false;
                app_dongle_adv_control();
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            if (app_link_check_phone_link(param->a2dp_disconn_cmpl.bd_addr))
            {
                if (headset_status.phone_status == PHONE_STREAM_A2DP)
                {
                    app_dongle_update_phone_stream_status(PHONE_EVENT_A2DP_DISC);
                }
            }

            if (app_link_check_dongle_link(param->a2dp_disconn_cmpl.bd_addr))
            {
                dongle_ctrl_data.legacy_dongle_streaming = false;
                app_dongle_adv_control();
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            if (app_link_check_phone_link(param->acl_conn_disconn.bd_addr))
            {
                app_dongle_update_phone_stream_status(PHONE_EVENT_PHONE_DISC);
            }

#if (F_APP_LEA_SUPPORT == 0)
            app_dongle_handle_acl_disc_event(param);
#endif
            handle = true;
        }
        break;

    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            uint8_t *addr = param->acl_conn_success.bd_addr;

            if (app_link_check_b2s_link(addr))
            {
                app_dongle_send_link_info_to_lowerstack(addr);
            }

            if (app_link_check_dongle_link(addr))
            {
                memcpy(app_db.connected_dongle_addr, addr, 6);
            }

            if (/* consider the first time connection has no bond flag yet
                   (connected_dongle_addr will be updated when recieve conn ind with gaming cod) */
                !memcmp(app_db.connected_dongle_addr, addr, 6) ||
                app_link_check_dongle_link(addr))
            {
#if F_APP_LOW_RSSI_DISCONNECT_DONGLE
                bt_link_rssi_report(param->acl_conn_success.bd_addr, true, 200);
#endif

                T_APP_BR_LINK *p_phone_link = app_dongle_get_connected_phone_link();
                bool disallow_a2dp = true;

                if (p_phone_link && p_phone_link->streaming_fg)
                {
                    disallow_a2dp = false;
                }

                if (disallow_a2dp)
                {
                    dongle_ctrl_data.disallow_dongle_a2dp = true;
                    app_start_timer(&timer_idx_disallow_dongle_a2dp,
                                    "disallow dongle a2dp",
                                    app_dongle_timer_id, APP_TIMER_DISALLOW_DONGLE_A2DP, 0, false,
                                    2000);
                }
            }

            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_ACL_CONN);

            app_dongle_adjust_gaming_latency();

            handle = true;
        }
        break;

#if F_APP_LOW_RSSI_DISCONNECT_DONGLE
    case BT_EVENT_LINK_RSSI_INFO:
        {
            if (param->link_rssi_info.rssi != 0 &&
                !memcmp(param->link_rssi_info.bd_addr, app_db.connected_dongle_addr, 6))
            {
                if (param->link_rssi_info.rssi < DONGLE_DISCONNECT_RSSI_THRESHOLD)
                {
                    if (timer_idx_low_rssi_disconnect_dongle == 0 &&
                        dongle_ctrl_data.low_rssi_disconnet_dongle == false)
                    {
                        app_start_timer(&timer_idx_low_rssi_disconnect_dongle,
                                        "low_rssi_disconnect_dongle",
                                        app_dongle_timer_id, APP_TIMER_LOW_RSSI_DISCONNECT_DONGLE, 0, false,
                                        2000);
                    }
                }
                else
                {
                    if (timer_idx_low_rssi_disconnect_dongle)
                    {
                        app_stop_timer(&timer_idx_low_rssi_disconnect_dongle);
                    }
                }
            }

            handle = true;
        }
        break;
#endif

    case BT_EVENT_A2DP_SNIFFING_START_IND:
        {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            memcpy(&a2dp_stream_para, (uint8_t *)&param->a2dp_sniffing_start_ind,
                   sizeof(T_BT_EVENT_PARAM_A2DP_SNIFFING_START_IND));
#endif
        }
        break;

    default:
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE1("app_dongle_bt_cback: event 0x%04x", event_type);
    }
}

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
static void app_dongle_handle_switch_a2dp_source(T_APP_DONGLE_SWITCH_EVENT evt)
{
    /* handle switch audio src */
    if (evt == SWITCH_EVENT_ENTER_PAIRING)
    {
        if (app_cfg_nv.is_bt_pairing)
        {
            if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_DONGLE)
            {
                app_dongle_switch_allowed_source();
            }
        }
        else
        {
            if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
            {
                app_dongle_switch_allowed_source();
            }
        }
    }
    else if (evt == SWITCH_EVENT_SWITCH_MODE)
    {
        if (app_cfg_nv.is_dual_mode == false)
        {
            if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
            {
                app_dongle_switch_allowed_source();
            }
        }
    }
}
#endif

#if F_APP_DONGLE_MULTI_PAIRING
static bool app_dongle_check_disc_src_before_switch_pairing(void)
{
    bool ret = false;

    uint8_t i = 0;
    uint32_t bond_flag;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        T_APP_BR_LINK *p_link = &app_db.br_link[i];

        if (p_link->used == true &&
            app_link_check_b2s_link(p_link->bd_addr) &&
            bt_bond_flag_get(p_link->bd_addr, &bond_flag))
        {
            bool disc_src = false;

            if (bond_flag & BOND_FLAG_DONGLE)
            {
                if (app_cfg_nv.is_bt_pairing)
                {
                    disc_src = true;
                }
            }
            else
            {
                if (app_cfg_nv.is_bt_pairing == false)
                {
                    disc_src = true;
                }
            }

            if (disc_src)
            {
                app_bt_policy_disconnect(p_link->bd_addr, ALL_PROFILE_MASK);
                ret = true;
                break;
            }
        }
    }


    APP_PRINT_TRACE1("app_dongle_check_disc_src_before_switch_pairing: ret %d", ret);

    return ret;
}

void app_dongle_switch_pairing_mode(void)
{
    int8_t err = 0;

    if (app_cfg_const.enable_dongle_multi_pairing == false)
    {
        err = -1;
        goto exit;
    }

    if (dongle_mode != PAIRING_MODE_BT_OR_BT_24G)
    {
        err = -2;
        goto exit;
    }

    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        err = -3;
        goto exit;
    }

#if F_APP_LEA_SUPPORT
    if (app_link_get_lea_link_num() != 0)
    {
        /* Don't switch pairing mode when 2.4G already connected */
        err = -4;
        goto exit;
    }
#endif

    dongle_ctrl_data.switch_pairing_triggered = true;

    if (app_link_get_b2s_link_num() != 0)
    {
        if (app_dongle_check_disc_src_before_switch_pairing())
        {
            err = -5;
            goto exit;
        }
    }

    app_cfg_nv.is_bt_pairing ^= 1;

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        if (app_cfg_nv.is_bt_pairing)
        {
            app_cfg_nv.allowed_source = ALLOWED_SOURCE_BT;
        }
        else
        {
            app_cfg_nv.allowed_source = ALLOWED_SOURCE_DONGLE;
        }
    }
#endif

    app_bt_policy_enter_pairing_mode(true, true);

    if (app_cfg_nv.is_bt_pairing == true)
    {
        app_dongle_adv_start(false);
    }
    else
    {
        app_dongle_adv_start(true);
    }

    dongle_ctrl_data.switch_pairing_triggered = false;

exit:
    APP_PRINT_TRACE2("app_dongle_switch_pairing_mode: err %d is_bt_pairing %d",
                     err, app_cfg_nv.is_bt_pairing);
}
#endif

void app_dongle_handle_headset_adv(bool enable_adv)
{
    if (app_cfg_const.rtk_app_adv_support)
    {
        if (enable_adv && (app_dongle_is_streaming() == false))
        {
            if (app_cfg_const.timer_ota_adv_timeout == 0)
            {
                /*power on always advertising*/
                app_ble_common_adv_start_rws(0);
            }
            else
            {
                if (timer_idx_change_headset_adv_interval != 0)
                {
                    app_ble_common_adv_start_rws(app_cfg_const.timer_ota_adv_timeout * 100);
                }
            }
        }
        else //disable adv
        {
            if (app_ble_common_adv_get_state() != BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                app_ble_common_adv_stop(APP_STOP_ADV_CAUSE_LEGACY_GAMING_STREAMING);
            }
        }
    }
}

static void app_dongle_adv_init()
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop =
        LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED;
    uint16_t adv_interval = DONGLE_ADV_INTERVAL;
    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_PUBLIC;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    ble_ext_adv_mgr_init_adv_params(&dongle_adv_handle,
                                    adv_event_prop,
                                    adv_interval,
                                    adv_interval,
                                    own_address_type,
                                    peer_address_type,
                                    peer_address,
                                    filter_policy,
                                    0, NULL, 0, NULL, NULL);

}

uint8_t app_dongle_get_phone_bond_num(void)
{
    uint8_t i;
    uint8_t bond_num = app_bond_b2s_num_get();
    uint32_t bond_flag;
    uint8_t addr[6] = {0};
    uint8_t num = 0;

    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (app_bond_b2s_addr_get(i, addr))
        {
            bt_bond_flag_get(addr, &bond_flag);

            if ((bond_flag & BOND_FLAG_DONGLE) == 0)
            {
                ++num;
            }
        }
    }

    return num;
}

uint8_t app_dongle_get_dongle_num(void)
{
    uint8_t i;
    uint8_t bond_num = app_bond_b2s_num_get();
    uint32_t bond_flag;
    uint8_t addr[6] = {0};
    uint8_t num = 0;

    for (i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (app_bond_b2s_addr_get(i, addr))
        {
            bt_bond_flag_get(addr, &bond_flag);

            if (bond_flag & BOND_FLAG_DONGLE)
            {
                ++num;
            }
        }
    }

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
    /* dongle num always 1 if have le audio dongle record */
    num = app_cfg_nv.le_audio_dongle_connected_record ? 1 : 0;
#endif

    return num;
}

T_APP_DONGLE_MODE app_dongle_get_mode(void)
{
    return dongle_mode;
}

bool app_dongle_is_24g_mode(void)
{
    bool ret = false;

    if (app_cfg_const.enable_dongle_dual_mode)
    {
        if (dongle_mode == STANDBY_MODE_24G ||
            dongle_mode == LINKBACK_MODE_24G ||
            dongle_mode == PAIRING_MODE_24G)
        {
            ret = true;
        }
    }

    return ret;
}

static bool app_dongle_shutdown_is_synchronous(void)
{
    bool ret = false;

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED) &&
        (app_link_get_b2s_link_num() == 0))
    {
        ret = true;
    }

    return ret;
}

static void app_dongle_check_disc_and_linkback(uint8_t new_mode)
{
    uint8_t i = 0;
    uint32_t bond_flag = 0;

    APP_PRINT_TRACE1("app_dongle_check_disc_and_linkback: new_mode %d", new_mode);

    if (app_cfg_const.enable_multi_link)
    {
        /* new mode is bt mode */
        if (new_mode == 1)
        {
            /* linkback phone */
            app_bt_policy_conn_all_b2s();
        }
        else
        {
            /* disc phone */
            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                T_APP_BR_LINK *p_link = &app_db.br_link[i];

                if (p_link->used &&
                    app_link_check_b2s_link(p_link->bd_addr) &&
                    bt_bond_flag_get(p_link->bd_addr, &bond_flag) &&
                    (bond_flag & BOND_FLAG_DONGLE) == 0)
                {
                    app_bt_policy_disconnect(p_link->bd_addr, ALL_PROFILE_MASK);
                }
            }
        }
    }
    else
    {
        if (app_link_get_b2s_link_num() == 0)
        {
            app_bt_policy_conn_all_b2s();
        }
        else
        {
            dongle_ctrl_data.linkback_src_after_disc = true;
            app_bt_policy_disc_all_b2s();
        }
    }
}

static T_APP_DONGLE_MODE app_dongle_next_mode_get(T_APP_DONGLE_SWITCH_EVENT evt,
                                                  T_APP_DONGLE_MODE prev_mode)
{
    T_APP_DONGLE_MODE next_mode = prev_mode;

    if (prev_mode == STANDBY_MODE_24G)
    {
        if (evt == SWITCH_EVENT_ENTER_PAIRING)
        {
            next_mode = PAIRING_MODE_24G;
        }
        else if (evt == SWITCH_EVENT_ENTER_LINKBACK)
        {
            next_mode = LINKBACK_MODE_24G;
        }
    }
    else if (prev_mode == PAIRING_MODE_24G)
    {
        if (evt == SWITCH_EVENT_ENTER_STANDBY)
        {
            next_mode = STANDBY_MODE_24G;
        }
    }
    else if (prev_mode == LINKBACK_MODE_24G)
    {
        if (evt == SWITCH_EVENT_ENTER_STANDBY)
        {
            next_mode = STANDBY_MODE_24G;
        }
        else if (evt == SWITCH_EVENT_ENTER_PAIRING)
        {
            next_mode = PAIRING_MODE_24G;
        }
    }
    else if (prev_mode == STANDBY_MODE_BT_OR_BT_24G)
    {
        if (evt == SWITCH_EVENT_ENTER_PAIRING)
        {
            next_mode = PAIRING_MODE_BT_OR_BT_24G;
        }
        else if (evt == SWITCH_EVENT_ENTER_LINKBACK)
        {
            next_mode = LINKBACK_MODE_BT_OR_BT_24G;
        }
    }
    else if (prev_mode == PAIRING_MODE_BT_OR_BT_24G)
    {
        if (evt == SWITCH_EVENT_ENTER_STANDBY)
        {
            next_mode = STANDBY_MODE_BT_OR_BT_24G;
        }
    }
    else if (prev_mode == LINKBACK_MODE_BT_OR_BT_24G)
    {
        if (evt == SWITCH_EVENT_ENTER_STANDBY)
        {
            next_mode = STANDBY_MODE_BT_OR_BT_24G;
        }
        else if (evt == SWITCH_EVENT_ENTER_PAIRING)
        {
            next_mode = PAIRING_MODE_BT_OR_BT_24G;
        }
    }

    if (evt == SWITCH_EVENT_SWITCH_MODE)
    {
        if (app_dongle_check_mode_24g(prev_mode))
        {
            next_mode = LINKBACK_MODE_BT_OR_BT_24G;
        }
        else
        {
            next_mode = LINKBACK_MODE_24G;
        }
    }

    return next_mode;
}

#if (F_APP_ERWS_SUPPORT == 0)
bool app_dongle_dual_mode_linkback(void)
{
    bool linkback = false;
    uint8_t dongle_num = app_dongle_get_dongle_num();
    uint8_t b2s_num = app_bond_b2s_num_get();
    bool ret = false;

    if (app_cfg_const.enable_dongle_dual_mode == false)
    {
        return false;
    }

    if (app_dongle_is_24g_mode())
    {
        if (dongle_num > 0)
        {
            linkback = true;
        }
    }
    else
    {
        if (app_cfg_const.enable_multi_link)
        {
            if (b2s_num > 0)
            {
                linkback = true;
            }
        }
        else
        {
            if ((b2s_num - dongle_num) > 0)
            {
                linkback = true;
            }
        }
    }

    APP_PRINT_TRACE3("app_dongle_dual_mode_linkback: b2s %d dongle %d linkback %d",
                     b2s_num, dongle_num, linkback);

    if (linkback)
    {
        app_bt_policy_conn_all_b2s();
        ret = true;
    }

    return ret;
}
#endif

void app_dongle_shutdown_end_check(void)
{
    if (dongle_ctrl_data.start_linkback_when_shutdown)
    {
        if (app_cfg_const.enable_dongle_dual_mode && app_cfg_const.enable_multi_link == false)
        {
            if (app_dongle_get_phone_bond_num() == 0)
            {
                dongle_ctrl_data.enter_pairing_after_mode_switch = true;
            }
        }

        dongle_ctrl_data.start_linkback_when_shutdown = false;
        app_device_bt_policy_startup(true);
    }
}

void app_dongle_dual_mode_switch(T_APP_DONGLE_SWITCH_EVENT evt)
{
    T_APP_DONGLE_MODE prev_mode = dongle_mode;
    T_APP_DONGLE_MODE next_mode = dongle_mode;
    int8_t err = 0;

    if (app_cfg_const.enable_dongle_dual_mode == false)
    {
        err = -1;
        goto exit;
    }

    if (evt == SWITCH_EVENT_SWITCH_MODE)
    {
        app_cfg_nv.is_dual_mode ^= 1;

        app_relay_async_single(APP_MODULE_TYPE_DONGLE_DUAL_MODE, APP_REMOTE_MSG_SYNC_BT_MODE);

        if (app_cfg_nv.is_dual_mode == 0)
        {
            app_audio_tone_type_play(TONE_ENTER_DONGLE_MODE, false, true);
        }
        else
        {
            app_audio_tone_type_play(TONE_EXIT_DONGLE_MODE, false, true);
        }
    }

    next_mode = app_dongle_next_mode_get(evt, prev_mode);

    dongle_mode = next_mode;

    if (dongle_mode == LINKBACK_MODE_BT_OR_BT_24G)
    {
        headset_status.headset_linkback = true;
        app_dongle_sync_headset_status();
    }
    else
    {
        if (headset_status.headset_linkback)
        {
            headset_status.headset_linkback = false;
            app_dongle_sync_headset_status();
        }
    }

    if (prev_mode != next_mode)
    {
        bool enable_pairing = false;

        /* 2.4G */
        if (app_dongle_check_mode_24g(next_mode))
        {
            if (next_mode == PAIRING_MODE_24G)
            {
                enable_pairing = true;
            }

            if ((prev_mode == LINKBACK_MODE_BT_OR_BT_24G) &&
                app_dongle_get_connected_dongle_link())
            {
                APP_PRINT_TRACE0("app_dongle_dual_mode_switch: cancel linkback");
                /* stop linkback phone if gaming dongle start streaming */
                app_dongle_cancel_linkback();
            }

            app_dongle_adv_start(enable_pairing);
        }
        else /* BT or BT+2.4G */
        {
            bool support_bt_and_24g = false;

            if (app_cfg_const.enable_multi_link)
            {
                support_bt_and_24g = true;
            }

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
            support_bt_and_24g = true;
#endif

            if (support_bt_and_24g && app_link_get_b2s_link_num() < 2)
            {
                if (next_mode == PAIRING_MODE_BT_OR_BT_24G)
                {
                    if (app_cfg_const.enable_dongle_multi_pairing && app_cfg_nv.is_bt_pairing)
                    {
                        enable_pairing = false;
                    }
                    else
                    {
                        enable_pairing = true;
                    }
                }

                app_dongle_adv_start(enable_pairing);
            }
            else
            {
                if (dongle_ctrl_data.dongle_adv_enabled)
                {
                    app_dongle_adv_stop();
                }
            }
        }

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
        if (app_cfg_const.enable_24g_bt_audio_source_switch)
        {
            if (app_cfg_const.enable_dongle_multi_pairing && evt == SWITCH_EVENT_ENTER_PAIRING)
            {
                app_dongle_handle_switch_a2dp_source(SWITCH_EVENT_ENTER_PAIRING);
            }
            else if (evt == SWITCH_EVENT_SWITCH_MODE)
            {
                app_dongle_handle_switch_a2dp_source(SWITCH_EVENT_SWITCH_MODE);
            }
        }
#endif

        if (next_mode == LINKBACK_MODE_BT_OR_BT_24G)
        {
            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_LINKBACK_START);
        }
        else if (prev_mode == LINKBACK_MODE_BT_OR_BT_24G)
        {
            app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_LINKBACK_STOP);

            app_dongle_adjust_gaming_latency();
        }
    }

    if (evt == SWITCH_EVENT_SWITCH_MODE)
    {
        app_dongle_check_disc_and_linkback(app_cfg_nv.is_dual_mode);
    }

exit:
    APP_PRINT_TRACE6("app_dongle_dual_mode_switch: evt 0x%02x next_mode 0x%02x err %d is_dual_mode %d is_bt_pairing %d allow_source %d",
                     evt, next_mode, err, app_cfg_nv.is_dual_mode, app_cfg_nv.is_bt_pairing, app_cfg_nv.allowed_source);

}

uint16_t app_dongle_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SYNC_BT_MODE:
        {
            msg_ptr = (uint8_t *)&app_cfg_nv.offset_rws_sync_notification_vol;
            payload_len = 1;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_SYNC_HEADSET_STATUS:
        {
            msg_ptr = (uint8_t *)&headset_status;
            payload_len = sizeof(headset_status);

            skip = false;
        }
        break;

    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_DONGLE_DUAL_MODE, payload_len, msg_ptr,
                              skip, total);
}

static void app_dongle_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                   T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_SYNC_BT_MODE:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                uint8_t *p_info = (uint8_t *)buf;

                uint8_t tmp = (p_info[0] & 0x10) >> 4;

                APP_PRINT_TRACE2("app_dongle_parse_cback: is_dual_mode %d -> %d", app_cfg_nv.is_dual_mode, tmp);

                app_cfg_nv.is_dual_mode = tmp;
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_HEADSET_STATUS:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                memcpy((void *)&headset_status, buf, sizeof(headset_status));

                APP_PRINT_TRACE1("headset_status %b", TRACE_BINARY(sizeof(headset_status), &headset_status));
            }
        }
        break;

    default:
        break;
    }
}

static void update_mode_according_to_slide_switch_status(void)
{
    T_APP_SLIDE_SWITCH_GPIO_ACTION action = app_slide_switch_get_dual_mode_gpio_action();

    if (action == APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_BT)
    {
        if (app_cfg_nv.is_dual_mode == false)
        {
            app_cfg_nv.is_dual_mode = true;
            app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 1);
        }
    }
    else if (action == APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_2_4G)
    {
        if (app_cfg_nv.is_dual_mode == true)
        {
            app_cfg_nv.is_dual_mode = false;
            app_cfg_store(&app_cfg_nv.offset_rws_sync_notification_vol, 1);
        }
    }
}

static void app_dongle_timer_cback(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
    case APP_TIMER_DISALLOW_DONGLE_A2DP:
        {
            app_stop_timer(&timer_idx_disallow_dongle_a2dp);

            dongle_ctrl_data.disallow_dongle_a2dp = false;
        }
        break;
#endif

#if F_APP_LOW_RSSI_DISCONNECT_DONGLE
    case APP_TIMER_LOW_RSSI_DISCONNECT_DONGLE:
        {
            uint8_t dongle_addr[6] = {0};

            app_stop_timer(&timer_idx_low_rssi_disconnect_dongle);

            if (app_dongle_get_connected_dongle_addr(dongle_addr))
            {
                dongle_ctrl_data.low_rssi_disconnet_dongle = true;
                app_bt_policy_disconnect(dongle_addr, ALL_PROFILE_MASK);
            }
        }
        break;
#endif

    case APP_TIMER_CHANGE_HEADSET_ADV_INTERVAL:
        {
            uint8_t le_common_adv_handle = app_ble_common_adv_get_adv_handle();

            app_stop_timer(&timer_idx_change_headset_adv_interval);
            ble_ext_adv_mgr_change_adv_interval(le_common_adv_handle, 0x0400);

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
            //set adv interval 200ms for saving bandwith
            ble_ext_adv_mgr_change_adv_interval(app_lea_adv_get_handle(), 0x0140);
#else
            ble_ext_adv_mgr_change_adv_interval(dongle_adv_handle, 0x0140);
#endif
        }
        break;

    default:
        break;
    }

}

#if 0 //Todo: wait framework latency report
static void app_dongle_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    T_AUDIO_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_LATENCY_REPORT:
        {
            T_AUDIO_EVENT_PARAM *payload = (T_AUDIO_EVENT_PARAM *)event_buf;
            T_APP_BR_LINK *p_dongle_link = app_link_find_br_link(app_db.connected_dongle_addr);

            if (p_dongle_link &&
                payload->track_latency_report.handle == p_dongle_link->a2dp_track_handle)
            {
                /* current dsp buffer latency */
                uint16_t actual_latency = payload->track_latency_report.latency;
                uint16_t actual_latency_minus_offset = payload->track_latency_report.latency -
                                                       payload->track_latency_report.target_offset / 1000;

                /* target dsp buffer latency */
                uint16_t target_latency = payload->track_latency_report.latency_target / 1000;
                /* user setting latency : has a latency offset in framework */
                uint16_t target_latency_minus_offset = target_latency -
                                                       payload->track_latency_report.target_offset /
                                                       1000;

                APP_PRINT_TRACE4("app_dongle_audio_cback: actual_latency %d, actual_latency_minus_offset %d target_latency %d target_latency_minus_offset %d",
                                 actual_latency, actual_latency_minus_offset, target_latency, target_latency_minus_offset);
            }

            handle = true;
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_dongle_audio_cback: event 0x%04x", event_type);
    }
}
#endif

uint16_t app_dongle_get_gaming_latency(void)
{
    uint16_t latency_value;
    T_DONGLE_GAMING_SETTING setting = GAMING_SETTING_NONE;
    uint8_t legacy_src_num = app_link_get_b2s_link_num();
    uint8_t le_link_num = app_link_get_le_link_num();
    uint8_t connected_device_num = legacy_src_num + le_link_num;

    latency_value = (remote_session_role_get() == REMOTE_SESSION_ROLE_SINGLE ? ULTRA_LOW_LATENCY_MS :
                     TWS_ULTRA_LOW_LATENCY_MS);

    if (remote_session_role_get() == REMOTE_SESSION_ROLE_SINGLE)
    {
        if (connected_device_num > 1)
        {
            if (le_link_num == 0)
            {
                setting = GAMING_SETTING_MULTI_SRC_PLAYING;
            }
            else
            {
                setting = GAMING_SETTING_MULTI_SRC_WITH_BLE_CONNECTED;
            }
        }

#if F_APP_SUPPORT_LINKBACK_WHEN_DONGLE_STREAMING
        if (app_dongle_get_mode() == LINKBACK_MODE_BT_OR_BT_24G)
        {
            setting = GAMING_SETTING_LINKBACK;
        }
#endif
    }

    if (setting != GAMING_SETTING_NONE)
    {
        uint8_t idx = app_dongle_get_gaming_setting_idx(setting);

        latency_value = gaming_setting[idx].latency;
    }

    APP_PRINT_TRACE2("app_dongle_get_gaming_latency: setting %d latency %d", setting, latency_value);

    return latency_value;
}

void app_dongle_handle_ble_disconnected(uint8_t *bd_addr)
{
    APP_PRINT_TRACE1("app_dongle_handle_ble_disconnected: %s", TRACE_BDADDR(bd_addr));

#if TARGET_LE_AUDIO_GAMING
    if (app_link_get_lea_link_num() == 0)
    {
        app_dongle_adv_start(false);
    }
#endif

#if (TARGET_LE_AUDIO_GAMING == 0)
    app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_BLE_DISCONNECTED);

    T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();
    uint16_t latency_value = 0;

    if (p_dongle_link && p_dongle_link->a2dp_track_handle)
    {
        latency_value = app_dongle_get_gaming_latency();
        app_dongle_set_gaming_latency(p_dongle_link->a2dp_track_handle, latency_value);
    }
#endif
}

/* dongle ble connected handle */
void app_dongle_handle_ble_connected(uint8_t *bd_addr)
{
    APP_PRINT_TRACE1("app_dongle_handle_ble_connected: %s", TRACE_BDADDR(bd_addr));

    if (app_link_check_dongle_link(bd_addr))
    {
        app_dongle_sync_headset_status();
    }

#if (TARGET_LE_AUDIO_GAMING == 0)
    app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_BLE_CONNECTED);

    T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();
    uint16_t latency_value = 0;

    if (p_dongle_link && p_dongle_link->a2dp_track_handle)
    {
        latency_value = app_dongle_get_gaming_latency();
        app_dongle_set_gaming_latency(p_dongle_link->a2dp_track_handle, latency_value);
    }
#endif


#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    bool is_a2dp_streaming = app_a2dp_is_streaming();

    APP_PRINT_TRACE2("app_dongle_handle_ble_connected: bud_role %d, is_a2dp_streaming %d",
                     app_cfg_nv.bud_role, is_a2dp_streaming);

    if (is_a2dp_streaming)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            T_APP_BR_LINK *p_link = NULL;

            p_link = &app_db.br_link[app_a2dp_get_active_idx()];

            if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                bt_avrcp_pause(p_link->bd_addr);
                p_link->avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
            }
        }
    }
#endif

}

bool app_dongle_set_gaming_latency(T_AUDIO_TRACK_HANDLE p_handle, uint16_t latency_value)
{
    bool ret = false;
    uint8_t cause = 0;

    if (app_db.gaming_mode == false)
    {
        cause = 1;
        goto exit;
    }

    ret = audio_track_latency_set(p_handle, latency_value, true);

exit:
    APP_PRINT_TRACE3("app_dongle_set_gaming_latency: ret %d latency %d cause %d", ret, latency_value,
                     -cause);

    return ret;
}

void app_dongle_exit_pairing_mode(void)
{
    app_bt_policy_exit_pairing_mode();
}

bool app_dongle_enter_pairing_mode(T_APP_DONGLE_PAIRING_MODE mode)
{
    uint8_t cause = 0;
    bool ret = true;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        cause = 1;
        goto exit;
    }

    if (mode == DONGLE_PAIRING_MODE_NONE)
    {
        cause = 2;
        goto exit;
    }

    if (app_hfp_get_call_status() != APP_CALL_IDLE)
    {
        cause = 3;
        goto exit;
    }

    if (app_cfg_const.enable_dongle_multi_pairing == false)
    {
        if (mode != DONGLE_PAIRING_MODE_ALL)
        {
            cause = 4;
            goto exit;
        }
    }
    else
    {
        if (mode != DONGLE_PAIRING_MODE_24G && mode != DONGLE_PAIRING_MODE_BT)
        {
            cause = 5;
            goto exit;
        }
    }

#if F_APP_LEA_SUPPORT
    if (app_dongle_get_le_audio_link())
#else
    if (app_dongle_get_connected_dongle_link())
#endif
    {
        if (mode == DONGLE_PAIRING_MODE_24G)
        {
            cause = 6;
            goto exit;
        }
    }

#if F_APP_LEA_SUPPORT == 0
    if (app_dongle_is_streaming())
    {
        cause = 7;
        goto exit;
    }
#endif

    dongle_ctrl_data.keep_pairing_mode_type = mode;

    app_bt_policy_enter_pairing_mode(true, true);

exit:
    if (cause != 0)
    {
        ret = false;
    }

    APP_PRINT_TRACE3("app_dongle_enter_pairing_mode: ret %d mode %d cause -%d", ret, mode, cause);

    return ret;
}

#if F_APP_B2B_ENGAGE_IMPROVE_BY_LEA_DONGLE
void app_dongle_lea_handle_cis_link_number(void)
{
    bool bud_side = dongle_status.snk_audio_loc - 1;
    uint8_t cis_link_number = dongle_status.src_conn_num;
    bool is_engaged = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? true : false;
    bool is_phone_connected = app_link_get_b2s_link_num() > 0 ? true : false;

    if (bud_side != app_cfg_const.bud_side)
    {
        return;
    }

    if (cis_link_number == 2)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY && !is_engaged && !is_phone_connected)
        {
            app_bt_policy_change_to_sec_and_conn_pri();
        }
    }
}
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
void app_donlge_enable_cis_higher_priority(bool enable)
{
    uint8_t params[2] = {0};

    params[0] = 0x04;
    params[1] = (uint8_t)enable;

    gap_vendor_cmd_req(0xfda5, sizeof(params), params);
}

void app_dongle_handle_hfp_call(void)
{
    APP_PRINT_TRACE1("app_dongle_handle_hfp_call: call_status %d", app_hfp_get_call_status());

    if (app_hfp_get_call_status() != APP_CALL_IDLE)
    {
        mtc_cis_audio_conext_change(false);
        mtc_ase_release();
        mtc_topology_dm(MTC_TOPO_EVENT_SET_BR_MODE);
    }
}

void app_dongle_le_audio_handle_mic(bool mic_enable)
{
    APP_PRINT_TRACE1("app_dongle_le_audio_handle_mic: mic_enable %d", mic_enable);

    if (mic_enable)
    {
        app_db.dongle_is_enable_mic = true;
        app_dongle_handle_stream_preempt(EVENT_2_4G_CALL_ACTIVE);
    }
    else
    {
        app_db.dongle_is_enable_mic = false;
        app_dongle_handle_stream_preempt(EVENT_2_4G_CALL_END);
    }
}

void app_dongle_handle_stream_preempt(T_APP_DONGLE_PREEMPT_EVENT event)
{
    bool is_a2dp_streaming = false;
    bool is_bt_sniffing = false;
    T_APP_DONGLE_PREEMPT_ACTION action = ACTION_ALL;
    uint8_t active_hf_idx = app_hfp_get_active_idx();
    T_APP_CALL_STATUS hfp_call_status = app_hfp_get_call_status();
    uint8_t ret = 0;
    bool b2b_connected = (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? true : false;

    T_APP_BR_LINK *p_link = NULL;
    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_link)
    {
        T_AUDIO_TRACK_STATE state;
        audio_track_state_get(p_link->a2dp_track_handle, &state);

        if (state == AUDIO_TRACK_STATE_STARTED ||
            state == AUDIO_TRACK_STATE_RESTARTED)
        {
            is_a2dp_streaming = true;
        }
    }

    if (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING || app_a2dp_is_streaming())
    {
        is_a2dp_streaming = true;
    }

    if (b2b_connected)
    {
        if (p_link->bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING ||
            p_link->bt_sniffing_state == APP_BT_SNIFFING_STATE_SNIFFING)
        {
            is_bt_sniffing = true;
        }
    }

    APP_PRINT_TRACE4("app_dongle_handle_stream_preempt: event 0x%02x, is_a2dp_streaming %d, is_bt_sniffing %d, hfp_call_status %d",
                     event, is_a2dp_streaming, is_bt_sniffing, hfp_call_status);

    // handle event
    switch (event)
    {
    case EVENT_2_4G_STREAM_START:
        {
            // disallow sniffing whether a2dp is streaming or not
            app_link_disallow_legacy_stream(true);

            if (is_a2dp_streaming)
            {
                action = ACTION_2_4G_STREAM_PREEMPT_A2DP_STREAM;
            }
        }
        break;

    case EVENT_2_4G_STREAM_STOP:
        {
            // allow sniffing
            app_link_disallow_legacy_stream(false);

            if (is_a2dp_streaming && hfp_call_status == APP_CALL_IDLE)
            {
                action = ACTION_2_4G_STREAM_STOP_RESUME_A2DP_STREAM;
            }
        }
        break;

    case EVENT_2_4G_CALL_ACTIVE:
        {
            if (hfp_call_status != APP_CALL_IDLE)
            {
                action = ACTION_2_4G_CALL_ACTIVE_PREEMPT_HFP_CALL;
            }
        }
        break;

    case EVENT_2_4G_CALL_END:
        {
            if (hfp_call_status != APP_CALL_IDLE)
            {
                action = ACTION_2_4G_CALL_END_RESUME_HFP_CALL;
            }
        }
        break;
    }

    APP_PRINT_TRACE3("app_dongle_handle_stream_preempt: action 0x%02x, b2b_connected %d, bud_role %d",
                     action, b2b_connected, app_cfg_nv.bud_role);

    // handle preempt action
    switch (action)
    {
    case ACTION_2_4G_STREAM_PREEMPT_A2DP_STREAM:
        {
            T_APP_BR_LINK *p_link = NULL;

            p_link = &app_db.br_link[app_a2dp_get_active_idx()];
            if (b2b_connected)
            {
                //app_bt_sniffing_stop(p_link->bd_addr, BT_SNIFFING_TYPE_A2DP);
                bt_sniffing_link_disconnect(p_link->bd_addr);
            }

            // release a2dp track
            if (p_link != NULL)
            {
                if (p_link->a2dp_track_handle != NULL)
                {
                    audio_track_release(p_link->a2dp_track_handle);
                    p_link->a2dp_track_handle = NULL;
                }

                if (p_link->eq_instance != NULL)
                {
                    eq_release(p_link->eq_instance);
                    p_link->eq_instance = NULL;
                }
                p_link->playback_muted = 0;
            }
        }
        break;

    case ACTION_2_4G_STREAM_STOP_RESUME_A2DP_STREAM:
        {
            mtc_topology_dm(MTC_TOPO_EVENT_SET_BR_MODE);

            ret = app_audio_a2dp_stream_resume((uint8_t *)&a2dp_stream_para);
        }
        break;

    case ACTION_2_4G_CALL_ACTIVE_PREEMPT_HFP_CALL:
        {
            if (b2b_connected && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr);
                //app_bt_sniffing_stop(app_db.br_link[active_hf_idx].bd_addr, BT_SNIFFING_TYPE_SCO);
            }

            mtc_cis_audio_conext_change(true);
            mtc_set_btmode(MULTI_PRO_BT_CIS);
        }
        break;

    case ACTION_2_4G_CALL_END_RESUME_HFP_CALL:
        {
            if (b2b_connected && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_bt_sniffing_hfp_connect(app_db.br_link[active_hf_idx].bd_addr);
            }

            mtc_cis_audio_conext_change(false);
            mtc_ase_release();
            mtc_topology_dm(MTC_TOPO_EVENT_SET_BR_MODE);
        }
        break;
    }

    if (ret)
    {
        APP_PRINT_TRACE1("app_dongle_handle_stream_preempt: ret %d", ret);
    }
}
#endif

void app_dongle_lea_handle_dongle_status(void)
{
    APP_PRINT_TRACE1("app_dongle_lea_handle_dongle_status: dongle_status %b",
                     TRACE_BINARY(sizeof(T_DONGLE_STATUS), &dongle_status));

#if F_APP_B2B_ENGAGE_IMPROVE_BY_LEA_DONGLE
    app_dongle_lea_handle_cis_link_number();
#endif
}

void app_dongle_dual_mode_init(void)
{

#if F_APP_SLIDE_SWITCH_SUPPORT
    update_mode_according_to_slide_switch_status();
#endif

    if (app_cfg_nv.is_dual_mode == 0)
    {
        dongle_mode = STANDBY_MODE_24G;
    }
    else
    {
        dongle_mode = STANDBY_MODE_BT_OR_BT_24G;
    }

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
#else
    app_dongle_adv_init();
#endif

#if F_APP_GAMING_CHAT_MIXING_SUPPORT
    headset_status.audio_mixing_support = true;
#endif

    bt_mgr_cback_register(app_dongle_bt_cback);

    app_relay_cback_register(app_dongle_relay_cback, app_dongle_parse_cback,
                             APP_MODULE_TYPE_DONGLE_DUAL_MODE, APP_REMOTE_MSG_DONGLE_MAX_MSG_NUM);

    sys_mgr_cback_register(app_dongle_dm_cback);

#if 0 //Todo: wait framework latency report
    audio_mgr_cback_register(app_dongle_audio_cback);
#endif

    app_timer_reg_cb(app_dongle_timer_cback, &app_dongle_timer_id);

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    app_donlge_enable_cis_higher_priority(true);
#endif
}
#endif
