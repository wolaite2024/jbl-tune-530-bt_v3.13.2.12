/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "sysm.h"
#include "btm.h"
#include "audio.h"
#include "pm.h"
#include "app_cfg.h"
#include "app_link_util.h"
#include "app_main.h"

#if F_APP_AUTO_POWER_TEST_LOG

static void app_power_test_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            TEST_PRINT_INFO4("app_listening_apply_when_power_on: listening_mode_power_on_status %d, anc_apt_state 0x%x, local_in_case %d, cycle %d",
                             app_cfg_const.listening_mode_power_on_status, app_cfg_nv.anc_both_setting,
                             app_db.local_in_case, app_cfg_nv.listening_mode_cycle);

        }
        break;
    case SYS_EVENT_POWER_OFF:
        {
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        TEST_PRINT_INFO1("app_power_test_dm_cback: event_type 0x%04x", event_type);
    }

}

static void app_power_test_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;
    switch (event_type)
    {
    case BT_EVENT_READY:
        {
        }
        break;

    case BT_EVENT_ACL_CONN_ACTIVE:
        {
        }
        break;

    case BT_EVENT_ACL_CONN_SNIFF:
        {
            TEST_PRINT_INFO2("app_power_test_bt_cback: sniff_interval= %d, bd_addr %s",
                             param->acl_conn_sniff.interval,
                             TRACE_BDADDR(param->acl_conn_sniff.bd_addr));
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            TEST_PRINT_INFO3("set_bd_addr: role %d, local_addr %s, peer_addr %s",
                             app_cfg_nv.bud_role,
                             TRACE_BDADDR(app_cfg_nv.bud_local_addr),
                             TRACE_BDADDR(app_cfg_nv.bud_peer_addr));
            TEST_PRINT_INFO3("connected: bud_role %d, b2b %d, b2s %d",
                             app_cfg_nv.bud_role,
                             (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED),
                             app_link_get_b2s_link_num());
        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {
            T_APP_BR_LINK *plink = app_link_find_b2s_link(param->avrcp_conn_cmpl.bd_addr);
            if (plink)
            {
                TEST_PRINT_INFO3("connected: bud_role %d, b2b %d, b2s %d",
                                 app_cfg_nv.bud_role,
                                 (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED),
                                 app_link_get_b2s_link_num());
                TEST_PRINT_INFO2("connected: b2s, bd_addr %s, profs 0x%08x",
                                 TRACE_BDADDR(plink->bd_addr),
                                 plink->connected_profile);
            }
        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
            T_APP_BR_LINK *plink = app_link_find_b2s_link(param->hfp_conn_cmpl.bd_addr);
            if (plink)
            {
                TEST_PRINT_INFO3("connected: bud_role %d, b2b %d, b2s %d",
                                 app_cfg_nv.bud_role,
                                 (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED),
                                 app_link_get_b2s_link_num());
                TEST_PRINT_INFO2("connected: b2s, bd_addr %s, profs 0x%08x",
                                 TRACE_BDADDR(plink->bd_addr),
                                 plink->connected_profile);
            }
        }
        break;

    case BT_EVENT_A2DP_CONN_CMPL:
        {
            T_APP_BR_LINK *plink = app_link_find_b2s_link(param->a2dp_conn_cmpl.bd_addr);
            if (plink)
            {
                TEST_PRINT_INFO3("connected: bud_role %d, b2b %d, b2s %d",
                                 app_cfg_nv.bud_role,
                                 (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED),
                                 app_link_get_b2s_link_num());
                TEST_PRINT_INFO2("connected: b2s, bd_addr %s, profs 0x%08x",
                                 TRACE_BDADDR(plink->bd_addr),
                                 plink->connected_profile);
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            TEST_PRINT_INFO1("app_power_test_bt_cback: play status= %d",
                             param->avrcp_play_status_changed.play_status);
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            TEST_PRINT_INFO1("app_power_test_bt_cback: call status= 0x%02x",
                             param->hfp_call_status.curr_status);
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        TEST_PRINT_INFO1("app_power_test_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_power_test_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    switch (event_type)
    {
    case AUDIO_EVENT_PASSTHROUGH_ENABLED:
        {
        }
        break;

    case AUDIO_EVENT_PASSTHROUGH_DISABLED:
        {
        }
        break;

    case AUDIO_EVENT_ANC_ENABLED:
        {
        }
        break;

    case AUDIO_EVENT_ANC_DISABLED:
        {
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        TEST_PRINT_INFO1("app_power_test_audio_cback: event_type 0x%04x", event_type);
    }
}

void app_power_test_init(void)
{
    sys_mgr_cback_register(app_power_test_dm_cback);
    bt_mgr_cback_register(app_power_test_bt_cback);
    audio_mgr_cback_register(app_power_test_audio_cback);
}

#endif
