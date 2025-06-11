/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "bt_bond.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_roleswap_control.h"
#include "app_relay.h"
#include "app_bt_policy_int.h"
#include "app_bond.h"

#if (F_APP_ERWS_SUPPORT == 0)
static uint8_t app_durian_link_timer_id = 0;
static uint8_t timer_idx_avp_b2s_connect = 0;
#endif

typedef enum
{
    APP_TIMER_AVP_B2S_CONNECT,
} T_APP_AVP_TIMER;

#define B2S_CONN_DELAY_MS             5000


void app_durian_link_bud_disconn(void)
{
    app_durian_adv_purpose_set_none();
    durian_db.role_decided = 0;
    app_durian_loc_report(durian_db.local_loc, durian_db.remote_loc);
}

void app_durian_link_src_connect(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    T_APP_RELAY_MSG_LIST_HANDLE relay_msg_handle = app_relay_async_build();

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
    app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_AVP,
                        SYNC_EVENT_DURIAN_ANC_CUR_SETTING);
    app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_AVP,
                        SYNC_EVENT_DURIAN_ANC_ONE_SETTING);
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_AVP,
                        SYNC_EVENT_DURIAN_ANC_BOTH_SETTING);
#endif
#endif

    app_relay_async_send(relay_msg_handle);

    if ((app_cfg_const.enable_multi_link != 0) && (app_link_get_b2s_link_num() == 1))
    {
        app_durian_adv_purpose_set_open_case();
    }

#if (F_APP_ERWS_SUPPORT == 0)
    app_start_timer(&timer_idx_avp_b2s_connect, "avp_b2s_connect",
                    app_durian_link_timer_id, APP_TIMER_AVP_B2S_CONNECT, 0, false,
                    AVP_B2S_CONN_DELAY_MS);
#endif
}

void app_durian_link_src_connect_delay(void)
{
    if (app_link_get_misc_num(APP_LINK_AVP) == 0)
    {
        durian_db.click_speed_rec = false;
        durian_db.long_press_time_rec = false;
    }
}

bool app_durian_link_disconn_req(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    APP_PRINT_INFO0("app_avp_disconnect_req: avp and gatt");

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if (p_link->connected_profile & GATT_PROFILE_MASK)
        {
            bt_avp_audio_disconnect_req(bd_addr);
        }

        if (p_link->connected_profile & AVP_PROFILE_MASK)
        {
            bt_avp_control_disconnect_req(bd_addr);
        }

        return true;
    }

    return false;
}

void app_durian_link_src_disconn(void)
{
    uint8_t b2s_num = app_link_get_b2s_link_num();

    if ((b2s_num == 0) || (app_cfg_const.enable_multi_link == 0))
    {
        if (durian_db.local_loc == BUD_LOC_IN_EAR && durian_db.remote_loc == BUD_LOC_IN_EAR)
        {
            durian_db.both_in_ear_src_lost = true;
        }
        else
        {
            durian_db.both_in_ear_src_lost = false;
        }
        durian_db.local_loc_changed = false;
        durian_db.remote_loc_changed = false;
#if F_APP_LISTENING_MODE_SUPPORT
        durian_db.anc_pre_setting = AVP_ANC_NULL;
#endif
    }

    APP_PRINT_TRACE2("app_durian_link_src_disconn: both_in_ear_src_lost %d b2s_num %d",
                     durian_db.both_in_ear_src_lost, b2s_num);
}

void app_durian_link_profile_conn(void)
{
#if DURIAN_AIRMAX
    if (durian_db.need_open_case_adv != 0)
    {
        durian_db.need_open_case_adv = 0;
    }
    else
    {
        // app_durian_adv_purpose_set(ADV_PURPOSE_NONE);
    }
#endif
}

void app_durian_link_back(void)
{
    uint8_t max_load_num;
    uint8_t num = 0;
    T_APP_ROLESWAP_STATUS roleswap_state = app_roleswap_ctrl_get_status();
    uint8_t b2s_num = app_link_get_b2s_link_num();

    if (app_cfg_const.enable_multi_link == 0)
    {
        max_load_num = 1;
    }
    else
    {
        max_load_num = (app_cfg_const.maximum_linkback_number_high_bit << 3) |
                       app_cfg_const.maximum_linkback_number;
    }

    APP_PRINT_TRACE3("app_durian_link_back: roleswap_state %d b2s_num %d max_load_num %d",
                     roleswap_state,
                     b2s_num, max_load_num);

    if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY) && (b2s_num == 0)
        && (roleswap_state != APP_ROLESWAP_STATUS_BUSY))
    {
        uint32_t bond_flag;
        uint32_t plan_profs;
        uint8_t bond_num = app_bond_b2s_num_get();
        for (uint8_t i = 1; i <= bond_num; i++)
        {
            if (app_bond_b2s_addr_get(i, app_db.bt_addr_disconnect))
            {
                bt_bond_flag_get(app_db.bt_addr_disconnect, &bond_flag);
                if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                {
                    plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                    app_bt_policy_default_connect(app_db.bt_addr_disconnect,  plan_profs, false);

                    if (++num >= max_load_num)
                    {
                        break;
                    }
                }
            }
        }
        durian_db.both_in_ear_src_lost = false;
        durian_db.local_loc_changed = false;
        durian_db.remote_loc_changed = false;
    }
}

static void app_durian_link_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_durian_link_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
#if (F_APP_ERWS_SUPPORT == 0)
    case APP_TIMER_AVP_B2S_CONNECT:
        {
            app_stop_timer(&timer_idx_avp_b2s_connect);
            app_durian_link_src_connect_delay();
        }
        break;
#endif

    default:
        break;
    }
}

void app_durian_link_init(void)
{
#if (F_APP_ERWS_SUPPORT == 0)
    app_timer_reg_cb(app_durian_link_timeout_cb, &app_durian_link_timer_id);
#endif
}
#endif
