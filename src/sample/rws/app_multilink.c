/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "app_timer.h"
#include "gap_br.h"
#include "btm.h"
#include "bt_bond.h"
#include "remote.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_hfp.h"
#include "app_a2dp.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "audio_track.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_roleswap.h"
#include "app_bond.h"
#include "app_listening_mode.h"
#include "app_lea_adv.h"
#include "app_multilink.h"
#include "app_audio_policy.h"
#include "app_bt_policy_int.h"
#include "app_bt_policy_api.h"
#include "app_bt_point.h"
#include "app_bt_sniffing.h"
#include "audio_volume.h"
#include "audio_track.h"
#include "app_relay.h"
#include "app_iphone_abs_vol_handle.h"
#include "app_ble_gap.h"
#include "app_auto_power_off.h"
#include "app_device.h"
#include "app_bt_point.h"

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_QOL_MONITOR_SUPPORT
#include "app_qol.h"
#endif

#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO == 0
#include "app_sensor_mems.h"
#endif
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_POWER_TEST
#include "power_debug.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_gfps_msg.h"
#include "app_sass_policy.h"
#include "bt_gfps.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#include "app_harman_behaviors.h"
#include "app_harman_ble_ota.h"
#endif

#if F_APP_LEA_SUPPORT
#include "multitopology_ctrl.h"
#endif

static bool le_sniffing_stop = false;
static uint8_t multilink_timer_id = 0;
static uint8_t timer_idx_multilink_disconn_inctive_acl = 0;
static bool sco_wait_from_sco_sniffing_stop = false;
static bool multilink_is_on_by_mmi = false;
static uint8_t pending_sco_index_from_sco = 0xFF;
static uint8_t timer_idx_multilink_silence_packet_judge = 0;
static uint8_t timer_idx_multilink_silence_packet_detect = 0;

#if F_APP_MUTILINK_VA_PREEMPTIVE
static uint8_t timer_idx_multilink_disc_va_sco = 0;
static uint8_t stream_only[MAX_BR_LINK_NUM] = {false};

#if F_APP_DURIAN_SUPPORT
static uint8_t pre_avrcp[MAX_BR_LINK_NUM] = {0};
#endif

static bool va_wait_from_va_sniffing_stop = false;
static uint8_t pending_va_index_from_va = 0xFF;
static uint8_t pending_va_sniffing_type = 0xFF;
static uint8_t active_va_idx = MAX_BR_LINK_NUM;
void app_check_voice_dial(T_APP_BR_LINK *p_link);
#endif

static uint8_t active_hfp_transfer = 0xFF;
static uint8_t g_active_idx = MAX_BR_LINK_NUM;
static uint8_t multilink_silence_packet_idx = MAX_BR_LINK_NUM;

#if F_APP_HARMAN_FEATURE_SUPPORT
static uint8_t timer_idx_multilink_a2dp_stop = 0;
static uint8_t pending_id = 0xff;
#endif

typedef enum
{
    APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL,
    APP_TIMER_MULTILINK_DISC_VA_SCO,
    APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE,
    APP_TIMER_MULTILINK_SILENCE_PACKET_DETECT,
    APP_TIMER_MULTILINK_HARMAN_A2DP_STOP,
} T_MULTILINK_TIMER;

#if F_APP_HARMAN_FEATURE_SUPPORT
uint8_t app_multi_get_pending_id(void)
{
    return pending_id;
}
#endif

static void app_multi_a2dp_stop(uint8_t id)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(app_db.br_link[id].bd_addr);
    if (p_link != NULL)
    {
        p_link->streaming_fg = false;
        app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_STOP);
    }

    if (app_cfg_const.rws_gaming_mode == 0)
    {
        if (app_multi_check_in_sniffing())
        {
            app_bt_sniffing_stop(app_db.br_link[id].bd_addr, BT_SNIFFING_TYPE_A2DP);
        }
#if GFPS_SASS_SUPPORT
        else if ((extend_app_cfg_const.gfps_sass_support) && (p_link->id == g_active_idx))
        {
            app_multi_handle_sniffing_link_disconnect_event(p_link->id);
        }
#endif
    }

    app_bt_policy_primary_engage_action_adjust();
}

void app_multi_active_hfp_transfer(uint8_t idx, bool disc, bool force)
{
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    if (idx != active_hf_idx)
    {
        active_hfp_transfer = idx;

        if (force)
        {
            if (app_db.br_link[idx].call_status != BT_HFP_CALL_IDLE)
            {
                sco_wait_from_sco_sniffing_stop = true;
                pending_sco_index_from_sco = idx;
            }
            bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr);
        }

        if ((app_db.br_link[active_hf_idx].call_status == APP_CALL_INCOMING) ||
            (app_db.br_link[active_hf_idx].sco_handle == 0) && disc)
        {
            if (app_multi_check_in_sniffing())
            {
                app_bt_sniffing_stop(app_db.br_link[active_hf_idx].bd_addr,
                                     app_db.br_link[active_hf_idx].sniffing_type);
            }
            else
            {
                app_multi_handle_sniffing_link_disconnect_event(0xFF);
            }
        }
        else
        {
            if ((app_db.br_link[idx].call_status != BT_HFP_CALL_IDLE) &&
                (app_db.br_link[idx].call_status != APP_CALL_INCOMING))
            {
                sco_wait_from_sco_sniffing_stop = true;
                pending_sco_index_from_sco = idx;
            }

            if (disc)
            {
                if (app_multi_check_in_sniffing())
                {
                    app_bt_sniffing_stop(app_db.br_link[active_hf_idx].bd_addr,
                                         app_db.br_link[active_hf_idx].sniffing_type);
                }
                else
                {
                    app_multi_handle_sniffing_link_disconnect_event(0xFF);
                }
            }
        }
    }
}

bool app_multi_check_in_sniffing(void)
{
    uint8_t i;
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].bt_sniffing_state > APP_BT_SNIFFING_STATE_READY)
        {
            return true;
        }
    }
    return false;
}

bool app_multi_allow_a2dp_sniffing(uint8_t id)
{
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    if (mtc_is_lea_cis_stream(NULL) || app_db.remote_cis_link_is_streaming)
    {
        return false;
    }
#endif

    if ((app_hfp_get_call_status() == APP_CALL_IDLE) ||
        ((app_link_get_sco_conn_num() == 0) &&
         (app_hfp_get_active_idx() == id)))
    {
        return true;
    }
    return false;
}

void app_multi_stream_avrcp_set(uint8_t idx)
{
    app_a2dp_set_active_idx(idx);
#if F_APP_MUTILINK_VA_PREEMPTIVE
    if (app_db.br_link[idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING)
    {
#if F_APP_DURIAN_SUPPORT
        stream_only[idx] = (pre_avrcp[idx] != BT_AVRCP_PLAY_STATUS_PLAYING);
#else
        stream_only[idx] = true;
#endif
    }
#endif
    app_db.br_link[idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PLAYING;
    app_multi_a2dp_active_link_set(app_db.br_link[idx].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
    app_bond_set_priority(app_db.br_link[idx].bd_addr);
#endif
    app_multi_set_active_idx(idx);
}

bool app_multi_active_link_check(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);

    if (p_link)
    {
        APP_PRINT_TRACE4("app_multi_active_link_check: p_link %x, streaming_fg %d, sco_handle %d, call status %d",
                         p_link, p_link->streaming_fg, p_link->sco_handle, app_hfp_get_call_status());
        if (p_link->streaming_fg || p_link->sco_handle != 0)
        {
            if (p_link->sco_handle != 0 && app_hfp_get_active_idx() == p_link->id)
            {
                return true;
            }

            if (app_multi_allow_a2dp_sniffing(p_link->id))
            {
                if (p_link->streaming_fg && app_a2dp_get_active_idx() == p_link->id)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void app_multi_disconnect_inactive_link(uint8_t app_idx, uint8_t input_link_type)
{
    uint8_t call_num = 0;
    uint8_t i;
    T_APP_BR_LINK *p_link = NULL;
    uint8_t active_hf_idx = app_hfp_get_active_idx();
#if GFPS_SASS_SUPPORT
    T_SASS_LINK_TYPE link_type = SASS_TYPE_UNKNOW;
    uint8_t target_drop_index = app_multi_get_inactive_index(app_idx, input_link_type, call_num, false,
                                                             &link_type);
#endif

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            app_db.br_link[i].call_status != APP_CALL_IDLE)
        {
            call_num++;
        }
    }

#if GFPS_SASS_LEA_SUPPORT
    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used == true &&
            app_db.le_link[i].call_status != APP_CALL_IDLE)
        {
            call_num++;
        }
    }
#endif
    APP_PRINT_TRACE4("app_multi_disconnect_inactive_link: active_a2dp_idx %d, app_idx %d, "
                     "call_num %d, max_legacy_multilink_devices %d",
                     app_a2dp_get_active_idx(), app_idx, call_num,
                     app_cfg_const.max_legacy_multilink_devices);

    if (call_num >= app_cfg_const.max_legacy_multilink_devices)
    {
        app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, ALL_PROFILE_MASK);
        return;
    }

    if (call_num > 0)
    {
#if GFPS_SASS_SUPPORT
        if (link_type == SASS_EDR_LINK)
        {
            if (app_cfg_const.enable_multi_link)
            {
                //inactive index may change according to switch preference
                if (target_drop_index != MAX_BR_LINK_NUM)
                {
                    if (extend_app_cfg_const.gfps_sass_support)
                    {
                        app_bt_policy_disconnect(app_db.br_link[target_drop_index].bd_addr, ALL_PROFILE_MASK);
                    }
                    else
                    {
                        if (call_num >= app_cfg_const.max_legacy_multilink_devices)
                        {
                            app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, ALL_PROFILE_MASK);
                        }
                        else
                        {
                            app_bt_policy_disconnect(app_db.br_link[target_drop_index].bd_addr, ALL_PROFILE_MASK);
                        }
                    }
                }
            }
            else
            {
                //single link disconnect active/new link directly
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    app_bt_policy_disconnect(app_db.br_link[app_hfp_get_active_idx()].bd_addr, ALL_PROFILE_MASK);
                }
                else
                {
                    app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, ALL_PROFILE_MASK);
                }
            }
        }
#else
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if ((i != app_idx) && (i != active_hf_idx))
                {
                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    return;
                }
            }
        }
#endif
    }
    else
    {
#if GFPS_SASS_SUPPORT
        //disconnect one device if no bond and priority information when ACL connected
        if (//(extend_app_cfg_const.gfps_sass_support) &&
            (target_drop_index != MAX_BR_LINK_NUM) &&
            (link_type == SASS_EDR_LINK))
        {
            app_bt_policy_disconnect(app_db.br_link[target_drop_index].bd_addr, ALL_PROFILE_MASK);
        }
#else
        uint8_t priority;
        uint8_t bond_num;
        uint8_t bd_addr[6];

        bond_num = app_bond_b2s_num_get();
        if (bond_num)
        {
            for (priority = bond_num; priority > 0; priority--)
            {
                if (app_bond_b2s_addr_get(priority, bd_addr))
                {
                    p_link = app_link_find_b2s_link(bd_addr);
                    if (p_link != NULL)
                    {
                        if ((p_link->id != app_idx))
                        {
                            app_bt_policy_disconnect(bd_addr, ALL_PROFILE_MASK);
                            return;
                        }
                    }
                }
            }
        }
#endif
#if GFPS_SASS_SUPPORT
        //disconnect one device if no bond and priority information when ACL connected
        if (//(extend_app_cfg_const.gfps_sass_support) &&
            (target_drop_index == MAX_BR_LINK_NUM))
#endif
        {
            //disconnect one device if no bond and priority information when ACL connected
            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    if ((i != app_idx) || (input_link_type != SASS_EDR_LINK))
                    {
                        app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                        return;
                    }
                }
            }
        }
    }
}

void app_multi_roleswap_disconnect_inactive_link(void)
{
    uint8_t call_num = 0;
    uint8_t i;
    uint8_t exit_cause = 0;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    app_db.disconnect_inactive_link_actively = true;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used == true &&
            app_db.br_link[i].sco_handle != 0)
        {
            call_num++;
        }
    }

    app_db.connected_num_before_roleswap = app_link_get_b2s_link_num();

    if (call_num > 0)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (i != active_hf_idx)
                {
                    memcpy(app_db.resume_addr, app_db.br_link[i].bd_addr, 6);

                    if ((!app_db.br_link[i].connected_profile) &&
                        (memcmp(app_db.br_link[i].bd_addr, app_db.resume_addr, 6) == 0))
                    {
                        //If no profile connected before disconnecting acl, connected tone need.
                        app_audio_set_connected_tone_need(true);
                    }

                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    app_start_timer(&timer_idx_multilink_disconn_inctive_acl, "multilink_disconn_inctive_acl",
                                    multilink_timer_id, APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL, i, false,
                                    TIMER_TO_DISCONN_ACL);
                    exit_cause = 1;
                    goto exit;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (i != active_a2dp_idx)
                {
                    memcpy(app_db.resume_addr, app_db.br_link[i].bd_addr, 6);

                    if ((!app_db.br_link[i].connected_profile) &&
                        (memcmp(app_db.br_link[i].bd_addr, app_db.resume_addr, 6) == 0))
                    {
                        //If no profile connected before disconnecting acl, connected tone need.
                        app_audio_set_connected_tone_need(true);
                    }

                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    app_start_timer(&timer_idx_multilink_disconn_inctive_acl, "multilink_disconn_inctive_acl",
                                    multilink_timer_id, APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL, i, false,
                                    TIMER_TO_DISCONN_ACL);
                    exit_cause = 2;
                    goto exit;
                }
            }
        }
    }

exit:
    APP_PRINT_TRACE7("app_multi_roleswap_disconnect_inactive_link: active_a2dp_idx %d, "
                     "call_num %d, active_hf_idx %d, connected_num_before_roleswap %d, resume_addr %s exit_cause %d connected_tone_need %d",
                     active_a2dp_idx, call_num, active_hf_idx, app_db.connected_num_before_roleswap,
                     TRACE_BDADDR(app_db.resume_addr), exit_cause, app_db.connected_tone_need);
}

void app_multi_reconnect_inactive_link()
{
    uint8_t b2s_connected_num = app_link_get_b2s_link_num();

    APP_PRINT_TRACE3("app_multi_reconnect_inactive_link: connected_num_before_roleswap %d, resume_addr %s b2s_num %d",
                     app_db.connected_num_before_roleswap, TRACE_BDADDR(app_db.resume_addr), b2s_connected_num);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
        app_db.connected_num_before_roleswap > app_link_get_b2s_link_num())
    {
        uint32_t bond_flag;
        uint32_t plan_profs;

        app_db.connected_num_before_roleswap = 0;
        bt_bond_flag_get(app_db.resume_addr, &bond_flag);
        if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
        {
            plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
            app_bt_policy_default_connect(app_db.resume_addr, plan_profs, false);
        }
    }
}

void app_multi_stop_acl_disconn_timer(void)
{
    app_stop_timer(&timer_idx_multilink_disconn_inctive_acl);
}

void app_multi_switch_by_mmi(bool is_on_by_mmi)
{
    multilink_is_on_by_mmi = is_on_by_mmi;
}

bool app_multi_is_on_by_mmi(void)
{
    return multilink_is_on_by_mmi;
}

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#else
static uint8_t app_multi_find_new_active_a2dp_link_by_priority(uint8_t curr_idx)
{
    uint8_t priority;
    uint8_t bond_num;
    uint8_t bd_addr[6];
    T_APP_BR_LINK *p_link = NULL;
    uint8_t active_idx = 0;

    bond_num = app_bond_b2s_num_get();

    for (priority = 1; priority <= bond_num; priority++)
    {
        if (app_bond_b2s_addr_get(priority, bd_addr) == true)
        {
            p_link = app_link_find_b2s_link(bd_addr);
            if (p_link != NULL)
            {
                if (memcmp(p_link->bd_addr, app_db.br_link[curr_idx].bd_addr, 6))
                {
                    active_idx = p_link->id;
                    break;
                }
            }
        }
    }
    return active_idx;
}
#endif

bool app_multi_a2dp_active_link_set(uint8_t *bd_addr)
{
#if F_APP_MUTILINK_VA_PREEMPTIVE
    uint8_t active_a2dp_idx = 0;
#endif
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link)
    {
        app_a2dp_set_active_idx(p_link->id);
#if F_APP_MUTILINK_VA_PREEMPTIVE
        active_a2dp_idx = p_link->id;
#endif
        app_multi_set_active_idx(p_link->id);

#if F_APP_MUTILINK_VA_PREEMPTIVE
        if (app_cfg_const.enable_multi_link)
        {
            uint8_t pair_idx_mapping;
            app_bond_get_pair_idx_mapping(bd_addr, &pair_idx_mapping);
#if F_APP_IPHONE_ABS_VOL_HANDLE
            app_iphone_abs_vol_wrap_audio_track_volume_out_set(
                app_db.br_link[active_a2dp_idx].a2dp_track_handle,
                app_cfg_nv.audio_gain_level[pair_idx_mapping],
                app_cfg_nv.abs_vol[pair_idx_mapping]);
#else
            audio_track_volume_out_set(app_db.br_link[active_a2dp_idx].a2dp_track_handle,
                                       app_cfg_nv.audio_gain_level[pair_idx_mapping]);
#endif
            app_audio_track_spk_unmute(AUDIO_STREAM_TYPE_PLAYBACK);
#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                uint8_t cmd_ptr[9] = {0};
                memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                cmd_ptr[7] = app_cfg_nv.audio_gain_level[pair_idx_mapping];
                cmd_ptr[8] = 0;
                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY, APP_REMOTE_MSG_A2DP_VOLUME_SYNC,
                                                   cmd_ptr, 9, REMOTE_TIMER_HIGH_PRECISION, 0, false);
            }
#endif
        }
#endif
#if F_APP_LOCAL_PLAYBACK_SUPPORT
        if ((app_db.sd_playback_switch == 0) && (p_link->a2dp_track_handle))
#else
        if (p_link->a2dp_track_handle)
#endif
        {
#if F_APP_DONGLE_ALWAYS_GAMING_MODE
            if ((!app_db.gaming_mode) && (app_db.restore_gaming_mode == false) &&
                app_link_is_dongle_active_link())
            {
                app_mmi_handle_action(MMI_DEV_GAMING_MODE_SWITCH);
            }
#endif

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            audio_track_start(p_link->a2dp_track_handle);
#else
#if F_APP_LINEIN_SUPPORT
            if (app_cfg_const.line_in_support)
            {
                if (app_line_in_start_a2dp_check())
                {
                    audio_track_start(p_link->a2dp_track_handle);
                }
            }
            else
#endif
            {
                audio_track_start(p_link->a2dp_track_handle);
            }
#endif
        }

        app_audio_set_avrcp_status(p_link->avrcp_play_status);
        app_avrcp_sync_status();

        return bt_a2dp_active_link_set(bd_addr);
    }
    return false;
}

//multi-link a2dp
bool app_multi_pause_inactive_a2dp_link_stream(uint8_t keep_active_a2dp_idx, uint8_t resume_fg)
{
    uint8_t i;
    bool play_pause = false;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if ((i != keep_active_a2dp_idx) &&
            (app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)))
        {
            if ((app_db.br_link[i].streaming_fg == true) ||
                ((app_db.br_link[i].streaming_fg == false) &&
                 (app_db.br_link[i].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING) &&
                 (app_db.br_link[i].sniffing_type == BT_SNIFFING_TYPE_A2DP)))
            {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (resume_fg && app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING &&
                    !stream_only[i])
#else
                if (resume_fg)
#endif
                {
                    app_db.wait_resume_a2dp_idx = i;
                }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING &&
                    !stream_only[i])
#else
                if (app_db.br_link[i].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
#endif
                {
                    bool pause = true;

#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(app_db.br_link[i].bd_addr))
                    {
                        pause = false;
                    }
#endif

                    if (pause)
                    {
                        bt_avrcp_pause(app_db.br_link[i].bd_addr);
                    }
                }
#if F_APP_GAMING_DONGLE_SUPPORT
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_const.enable_multi_link))
                {
                    if (i != app_hfp_get_active_idx())
                    {
                        audio_track_stop(app_db.br_link[i].a2dp_track_handle);
                    }
                }
                else
                {
                    audio_track_stop(app_db.br_link[i].a2dp_track_handle);
                }
#else
                audio_track_stop(app_db.br_link[i].a2dp_track_handle);
#endif
                app_db.br_link[i].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                play_pause = true;
                // Stop Sniffing Now
                app_bt_sniffing_stop(app_db.br_link[i].bd_addr, BT_SNIFFING_TYPE_A2DP);
            }

            APP_PRINT_TRACE4("app_multi_pause_inactive_a2dp_link_stream: streaming %d play_status %d keep_active_a2dp_idx %d wait_resume_a2dp_idx %d",
                             app_db.br_link[i].streaming_fg, app_db.br_link[i].avrcp_play_status, keep_active_a2dp_idx,
                             app_db.wait_resume_a2dp_idx);
        }
    }

    return play_pause;
}

void app_multi_resume_a2dp_link_stream(uint8_t app_idx)
{
    if (app_db.wait_resume_a2dp_idx < MAX_BR_LINK_NUM)
    {
        //if (app_db.wait_resume_a2dp_idx != app_idx)
        {
            if (app_db.br_link[app_db.wait_resume_a2dp_idx].connected_profile &
                (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK))
            {
                app_multi_a2dp_active_link_set(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                app_bond_set_priority(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
#endif
                if (app_db.br_link[app_db.wait_resume_a2dp_idx].avrcp_play_status !=
                    BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    bool play = true;

#if F_APP_GAMING_DONGLE_SUPPORT
                    if (app_link_check_dongle_link(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr))
                    {
                        play = false;
                    }
#endif
                    if (play)
                    {
                        app_db.br_link[app_db.wait_resume_a2dp_idx].avrcp_play_status =
                            BT_AVRCP_PLAY_STATUS_PLAYING;
                        bt_avrcp_play(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                    }
                }
            }
        }
        //all sco are removed, the variable must be clear
        app_db.wait_resume_a2dp_idx = MAX_BR_LINK_NUM;
    }
    APP_PRINT_TRACE4("app_multi_resume_a2dp_link_stream: active_a2dp_idx %d, wait_resume_a2dp_idx %d, "
                     "app_idx %d, avrcp_play_status 0x%02x",
                     app_a2dp_get_active_idx(), app_db.wait_resume_a2dp_idx, app_idx,
                     app_db.br_link[app_a2dp_get_active_idx()].avrcp_play_status);
}

void app_multi_handle_sniffing_link_disconnect_event(uint8_t id)
{
    APP_PRINT_TRACE6("app_multi_handle_sniffing_link_disconnect_event:  %d, %d, %d, %d, %d, %d",
                     sco_wait_from_sco_sniffing_stop, pending_sco_index_from_sco,
                     app_db.sco_wait_a2dp_sniffing_stop, app_db.pending_sco_idx,
                     app_db.a2dp_wait_a2dp_sniffing_stop, app_db.pending_a2dp_idx);

    if (active_hfp_transfer != 0xFF)
    {
        app_hfp_set_active_idx(app_db.br_link[active_hfp_transfer].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
        app_bond_set_priority(app_db.br_link[active_hfp_transfer].bd_addr);
#endif
        if (app_db.br_link[active_hfp_transfer].sco_handle)
        {
            app_multi_set_active_idx(active_hfp_transfer);
            if (!app_bt_sniffing_start(app_db.br_link[active_hfp_transfer].bd_addr,
                                       BT_SNIFFING_TYPE_SCO))
            {

            }
        }
        else if (app_db.br_link[active_hfp_transfer].streaming_fg) //hfp data go a2dp
        {
            app_multi_set_active_idx(active_hfp_transfer);
            if (app_bt_sniffing_start(app_db.br_link[active_hfp_transfer].bd_addr,
                                      BT_SNIFFING_TYPE_A2DP)) {}
        }
        else if (sco_wait_from_sco_sniffing_stop == true)
        {
            app_multi_set_active_idx(pending_sco_index_from_sco);
            app_bt_sniffing_hfp_connect(app_db.br_link[pending_sco_index_from_sco].bd_addr);
        }
        sco_wait_from_sco_sniffing_stop = false;
        pending_sco_index_from_sco = 0xFF;
        active_hfp_transfer = 0xFF;

        for (int inactive_hf_index = 0; inactive_hf_index < MAX_BR_LINK_NUM; inactive_hf_index++)
        {
            if (inactive_hf_index != active_hfp_transfer)
            {
                if (app_db.br_link[inactive_hf_index].call_status >= BT_HFP_CALL_ACTIVE &&
                    app_db.br_link[inactive_hf_index].streaming_fg == false)
                {
                    sco_wait_from_sco_sniffing_stop = true;
                    pending_sco_index_from_sco = inactive_hf_index;
                    break;
                }
            }
        }
    }
    else if (sco_wait_from_sco_sniffing_stop == true && id != pending_sco_index_from_sco)
    {
        app_multi_set_active_idx(pending_sco_index_from_sco);
        app_bt_sniffing_hfp_connect(app_db.br_link[pending_sco_index_from_sco].bd_addr);
        sco_wait_from_sco_sniffing_stop = false;
        pending_sco_index_from_sco = 0xFF;
        active_hfp_transfer = 0xFF;
    }
    else if (app_db.sco_wait_a2dp_sniffing_stop == true)
    {
        app_multi_set_active_idx(app_db.pending_sco_idx);
        if (!app_bt_sniffing_start(app_db.br_link[app_db.pending_sco_idx].bd_addr, BT_SNIFFING_TYPE_SCO))
        {
            bt_hfp_audio_connect_cfm(app_db.br_link[app_db.pending_sco_idx].bd_addr, true);
        }
        app_db.sco_wait_a2dp_sniffing_stop = false;
    }
    else if (app_db.a2dp_wait_a2dp_sniffing_stop == true)
    {
#if GFPS_SASS_SUPPORT
        if (!extend_app_cfg_const.gfps_sass_support)
#endif
        {
            app_multi_set_active_idx(app_db.pending_a2dp_idx);
            if (app_bt_sniffing_start(app_db.br_link[app_db.pending_a2dp_idx].bd_addr,
                                      BT_SNIFFING_TYPE_A2DP)) {};
            app_db.a2dp_wait_a2dp_sniffing_stop = false;

            app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
        }

    }
#if F_APP_MUTILINK_VA_PREEMPTIVE
    else if (va_wait_from_va_sniffing_stop == true &&
             (pending_va_sniffing_type == BT_SNIFFING_TYPE_SCO ||
              pending_va_sniffing_type == BT_SNIFFING_TYPE_A2DP))
    {
        APP_PRINT_TRACE2("app_multi_handle_sniffing_link_disconnect_event: va_wait_va %d %d",
                         pending_va_index_from_va,
                         pending_va_sniffing_type);
        app_multi_set_active_idx(pending_va_index_from_va);
        if (pending_va_sniffing_type == BT_SNIFFING_TYPE_SCO)
        {
            if (!app_bt_sniffing_start(app_db.br_link[pending_va_index_from_va].bd_addr,
                                       BT_SNIFFING_TYPE_SCO))
            {

            }
        }
        else if (pending_va_sniffing_type == BT_SNIFFING_TYPE_A2DP)
        {
            if (app_bt_sniffing_start(app_db.br_link[pending_va_index_from_va].bd_addr,
                                      BT_SNIFFING_TYPE_A2DP)) {}

            app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
        }
        va_wait_from_va_sniffing_stop = false;
        pending_va_index_from_va = 0xFF;
        pending_va_sniffing_type = 0xFF;
    }
#endif
    else
    {
        // Check what ???
        // will cause single link problem: https://jira.realtek.com/browse/BBPRO2BUG-3827
        if ((app_db.br_link[app_hfp_get_active_idx()].sco_handle != NULL) &&
            ((app_cfg_const.enable_rtk_charging_box && !app_db.remote_case_closed) ||
             (!app_cfg_const.enable_rtk_charging_box && app_db.remote_loc != BUD_LOC_IN_CASE)))
        {
            app_multi_set_active_idx(app_hfp_get_active_idx());
            if (!app_bt_sniffing_start(app_db.br_link[app_hfp_get_active_idx()].bd_addr,
                                       BT_SNIFFING_TYPE_SCO))
            {
                APP_PRINT_TRACE0("app_multi_handle_sniffing_link_disconnect_event: app_bt_sniffing_start error");
            }
        }
        else
        {
            app_multi_judge_active_a2dp_idx_and_qos(app_a2dp_get_active_idx(), JUDGE_EVENT_SNIFFING_STOP);
        }
        if (sco_wait_from_sco_sniffing_stop == true && id != pending_sco_index_from_sco)
        {
            sco_wait_from_sco_sniffing_stop = false;
            pending_sco_index_from_sco = 0xFF;
        }
#if F_APP_MUTILINK_VA_PREEMPTIVE
        va_wait_from_va_sniffing_stop = false;
        pending_va_index_from_va = 0xFF;
        pending_va_sniffing_type = 0xFF;
#endif
    }

#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_notify_conn_status();
    }
#endif
}

bool app_multi_check_pend(void)
{
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    if (app_db.br_link[active_a2dp_idx].streaming_fg == false &&
        app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING)
    {
        return false;
    }
    return true;
}

void app_multi_update_a2dp_play_status(T_APP_JUDGE_A2DP_EVENT event)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (event == JUDGE_EVENT_A2DP_START)
        {
            app_audio_a2dp_play_status_update(APP_A2DP_STREAM_A2DP_START);

            if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_AUDIO);
            }
        }
        else if (event == JUDGE_EVENT_A2DP_STOP)
        {
            uint8_t idx;

            for (idx = 0; idx < MAX_BR_LINK_NUM; idx++)
            {
                if (app_db.br_link[idx].streaming_fg)
                {
                    break;
                }
            }

            if (idx == MAX_BR_LINK_NUM) // all links no streaming
            {
                app_audio_a2dp_play_status_update(APP_A2DP_STREAM_A2DP_STOP);

                if (app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off)
                {
                    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_AUDIO,
                                              app_cfg_const.timer_auto_power_off_while_phone_connected_and_anc_apt_off);
                }
            }
        }
    }
}

void app_multi_judge_active_a2dp_idx_and_qos(uint8_t app_idx, T_APP_JUDGE_A2DP_EVENT event)
{
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
    app_teams_multilink_music_priority_rearrangment(app_idx, event);

#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    app_multilink_customer_music_event_handle(app_idx, event);

#else

    switch (event)
    {
    case JUDGE_EVENT_A2DP_CONNECTED:
        {
            uint8_t link_number = app_link_get_b2s_link_num_with_profile(A2DP_PROFILE_MASK);
            if (link_number <= 1)
            {
                app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                if (link_number <= 0)
                {
                    //exception
                }
            }
            else
            {
                if ((app_db.br_link[active_a2dp_idx].streaming_fg == false) &&
                    (app_hfp_get_call_status() == APP_CALL_IDLE))
                {
                    if (app_cfg_const.enable_multi_link)
                    {
#if F_APP_HARMAN_FEATURE_SUPPORT
                        app_bond_set_priority(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                                 app_idx)].bd_addr);
                        app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                        if (app_harman_is_record_a2dp_active_ever_get() != true)
                        {
                            app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                        }
#else
                        app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                        app_bond_set_priority(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                                 app_idx)].bd_addr);
#endif
                    }
                    else
                    {
                        app_multi_set_active_idx(app_idx);
                        app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                        app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                    }
                }
            }
        }
        break;

    case JUDGE_EVENT_A2DP_START:
        {
            APP_PRINT_TRACE3("JUDGE_EVENT_A2DP_START: active_a2dp_idx %d, avrcp %d, stream %d",
                             active_a2dp_idx,
                             app_db.br_link[active_a2dp_idx].avrcp_play_status,
                             app_db.br_link[active_a2dp_idx].streaming_fg);

            if (app_multi_allow_a2dp_sniffing(app_idx))
            {
#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO == 0
                app_sensor_mems_start(app_idx);
#endif
#endif
#endif
                app_multi_preemptive_judge(app_idx, SASS_EDR_LINK, MULTILINK_SASS_A2DP_PREEM);
            }

            app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_A2DP_PLAY_EVENT);

#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_connect_idle_to_power_off(ACTIVE_NEED_STOP_COUNT, app_idx);
#endif
        }
        break;

    case JUDGE_EVENT_A2DP_DISC:
        {
            if (active_a2dp_idx == app_idx)
            {
#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO == 0
                app_sensor_mems_stop(active_a2dp_idx);
#endif
#endif
#endif
                if (app_multi_check_in_sniffing())
                {
                    app_bt_sniffing_stop(app_db.br_link[app_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                }
#if GFPS_SASS_SUPPORT
                else if ((extend_app_cfg_const.gfps_sass_support) && (app_idx == g_active_idx))
                {
                    app_multi_handle_sniffing_link_disconnect_event(app_idx);
                }
#endif
                if (app_link_get_b2s_link_num_with_profile(A2DP_PROFILE_MASK))
                {
                    app_multi_a2dp_active_link_set(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                                      app_idx)].bd_addr);
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                    app_bond_set_priority(app_db.br_link[app_multi_find_new_active_a2dp_link_by_priority(
                                                             app_idx)].bd_addr);
#endif
                }
#if F_APP_HARMAN_FEATURE_SUPPORT
                app_harman_is_record_a2dp_active_ever_set(false);
#endif
            }
            app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_A2DP_STOP_EVENT);
        }
        break;

    case JUDGE_EVENT_A2DP_STOP:
        {
            app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;

#if F_APP_MUTILINK_VA_PREEMPTIVE
            stream_only[app_idx] = false;
            if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == app_idx)
            {
                va_wait_from_va_sniffing_stop = false;
                pending_va_index_from_va = 0xFF;
                pending_va_sniffing_type = 0xFF;
            }
#endif

#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO == 0
            if (active_a2dp_idx == app_idx)
            {
                app_sensor_mems_stop(active_a2dp_idx);
            }
#endif
#endif
#endif
            if ((active_a2dp_idx == app_idx) && (app_db.wait_resume_a2dp_idx == MAX_BR_LINK_NUM))
            {
                app_stop_timer(&timer_idx_multilink_silence_packet_judge);
                app_stop_timer(&timer_idx_multilink_silence_packet_detect);
                uint8_t idx = app_multi_find_new_active_a2dp_link_by_priority(active_a2dp_idx);
                uint8_t i;
                for (i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_db.br_link[idx].connected_profile &
                        (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK))
                    {
                        if ((idx != active_a2dp_idx) &&
                            (app_db.br_link[idx].streaming_fg == true))
                        {
                            APP_PRINT_TRACE2("JUDGE_EVENT_A2DP_STOP: active_a2dp_idx %d, idx %d", active_a2dp_idx, idx);
                            app_multi_stream_avrcp_set(idx);
                            break;
                        }
                    }
                }
            }
            /*  a2dp pause to resume
            else if ((active_a2dp_idx == app_idx) && (app_db.wait_resume_a2dp_idx != active_a2dp_idx))
            {
                app_multi_a2dp_active_link_set(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
                APP_PRINT_TRACE2("JUDGE_EVENT_A2DP_STOP: active_a2dp_idx %d, wait_resume_a2dp_idx %d",
                                 active_a2dp_idx, app_db.wait_resume_a2dp_idx);
                app_db.active_media_paused = false;
                app_a2dp_set_active_idx(app_db.wait_resume_a2dp_idx);
                app_multi_resume_a2dp_link_stream(app_idx);
                app_bond_set_priority(app_db.br_link[app_db.wait_resume_a2dp_idx].bd_addr);
            }*/

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                //Clear a2dp preemptive flag if any link is disconnected during preemptive process.
                app_db.a2dp_preemptive_flag = false;

#if F_APP_ERWS_SUPPORT
                if (app_idx != active_a2dp_idx &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    //preemp ends, htpoll param return to normal value
                    app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE);
                    app_bt_sniffing_judge_dynamic_set_gaming_mode();
                }

#if F_APP_QOL_MONITOR_SUPPORT
                if (app_a2dp_is_streaming() == false)
                {
                    //close b2b link monitor when no a2dp stream
                    app_qol_link_monitor(app_cfg_nv.bud_peer_addr, false);
                }
#endif
#endif
            }
            app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_A2DP_STOP_EVENT);

#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_a2dp_stop_handle(app_idx);
#endif
        }
        break;

    case JUDGE_EVENT_MEDIAPLAYER_PLAYING:
        {
            app_multi_preemptive_judge(app_idx, SASS_EDR_LINK, MULTILINK_SASS_AVRCP_PREEM);

            app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_A2DP_PLAY_EVENT);
        }
        break;

    case JUDGE_EVENT_MEDIAPLAYER_PAUSED:
        {
            if (app_multi_allow_a2dp_sniffing(app_idx))
            {
                APP_PRINT_TRACE4("JUDGE_EVENT_MEDIAPLAYER_PAUSED: active_a2dp_idx %d, app_idx %d, "
                                 "streaming_fg %d, harman_silent_check: %d",
                                 active_a2dp_idx, app_idx, app_db.br_link[active_a2dp_idx].streaming_fg,
                                 app_db.br_link[active_a2dp_idx].harman_silent_check);
                if (active_a2dp_idx == app_idx)
                {
                    app_db.active_media_paused = true;
                    ///TODO: mute Music ???
                    // if (app_db.br_link[active_a2dp_idx].streaming_fg == true)
                    // {
                    //     app_db.br_link[active_a2dp_idx].streaming_fg = false;
                    //     app_bt_sniffing_stop(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                    // }
                    if (app_db.wait_resume_a2dp_idx == MAX_BR_LINK_NUM)
                    {
                        uint8_t idx = app_multi_find_new_active_a2dp_link_by_priority(active_a2dp_idx);
                        if ((app_db.br_link[idx].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK))
                            && (idx != active_a2dp_idx)
#if F_APP_HARMAN_FEATURE_SUPPORT
                            && (app_db.br_link[active_a2dp_idx].harman_silent_check)
#endif
                           )
                        {
                            multilink_silence_packet_idx = app_idx;
#if F_APP_GAMING_DONGLE_SUPPORT
                            if (!app_cfg_const.enable_multi_link)
                            {
                                app_start_timer(&timer_idx_multilink_silence_packet_judge, "multilink_silence_packet_judge",
                                                multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE, idx, false,
                                                750);
                                break;
                            }
#else
#if F_APP_HARMAN_FEATURE_SUPPORT
                            app_start_timer(&timer_idx_multilink_silence_packet_judge, "multilink_silence_packet_judge",
                                            multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE, idx, false,
                                            5000);
#else
                            app_start_timer(&timer_idx_multilink_silence_packet_judge, "multilink_silence_packet_judge",
                                            multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE, idx, false,
                                            750);
#endif
                            break;
#endif
                        }
                    }
                }
            }

            app_bt_policy_qos_param_update(app_db.br_link[app_idx].bd_addr, BP_TPOLL_A2DP_STOP_EVENT);
        }
        break;

    case JUDGE_EVENT_SNIFFING_STOP:
        {
#if F_APP_LEA_SUPPORT
            bool need_return = false;
#endif
            ///TODO: check Sniffing state for active link
            APP_PRINT_TRACE4("JUDGE_EVENT_SNIFFING_STOP: active_a2dp_idx %d, bd_addr %s, stream %d, remote_loc %d",
                             active_a2dp_idx,
                             TRACE_BDADDR(app_db.br_link[active_a2dp_idx].bd_addr),
                             app_db.br_link[active_a2dp_idx].streaming_fg,
                             app_db.remote_loc);
            uint8_t inactive_a2dp_idx = MAX_BR_LINK_NUM;
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if ((app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK)) &&
                    (i != active_a2dp_idx))
                {
                    inactive_a2dp_idx = i;
                    break;
                }
            }
#if F_APP_LEA_SUPPORT
            if ((mtc_if_fm_ml_handle(ML_TO_MTC_CH_SNIFFING, NULL, &need_return) == MTC_RESULT_SUCCESS) &&
                need_return)
            {
                APP_PRINT_TRACE0("JUDGE_EVENT_SNIFFING_STOP: STOP SNIFFING");
                le_sniffing_stop = true;
                break;
            }
#endif
            if ((app_db.br_link[active_a2dp_idx].streaming_fg) &&
                ((app_cfg_const.enable_rtk_charging_box && !app_db.remote_case_closed) ||
                 (!app_cfg_const.enable_rtk_charging_box && app_db.remote_loc != BUD_LOC_IN_CASE)))
            {
                app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);

#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (app_multi_allow_a2dp_sniffing(active_a2dp_idx))
                {
                    app_multi_set_active_idx(active_a2dp_idx);

#if F_APP_QOL_MONITOR_SUPPORT
                    int8_t b2b_rssi = 0;
                    app_qol_get_aggregate_rssi(true, &b2b_rssi);

                    if (((b2b_rssi != 0) && (b2b_rssi > B2B_RSSI_THRESHOLD_START_SNIFFING)) || (!app_db.sec_going_away))
                    {
                        if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                    }
#else
                    if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
#endif
#if F_APP_ERWS_SUPPORT
                    if (remote_session_role_get() == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        if (app_db.br_link[active_a2dp_idx].a2dp_track_handle)
                        {
                            audio_track_start(app_db.br_link[active_a2dp_idx].a2dp_track_handle);
                        }
                    }
#endif
                }
#else

#if F_APP_QOL_MONITOR_SUPPORT
                int8_t b2b_rssi = 0;
                app_qol_get_aggregate_rssi(true, &b2b_rssi);

                if (((b2b_rssi != 0) && (b2b_rssi > B2B_RSSI_THRESHOLD_START_SNIFFING)) || (!app_db.sec_going_away))
                {
                    if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                }
#else
                if (app_bt_sniffing_start(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
#endif

#endif
            }
            else if ((app_db.br_link[active_a2dp_idx].used) &&
                     (app_db.br_link[active_a2dp_idx].streaming_fg == false))
            {
                if (inactive_a2dp_idx == MAX_BR_LINK_NUM)
                {
                    app_multi_set_active_idx(active_a2dp_idx);
                }
                else if (app_db.br_link[inactive_a2dp_idx].streaming_fg == false)
                {
                    APP_PRINT_TRACE3("JUDGE_EVENT_SNIFFING_STOP: inactive_a2dp_idx %d, bd_addr %s, stream %d",
                                     inactive_a2dp_idx,
                                     TRACE_BDADDR(app_db.br_link[inactive_a2dp_idx].bd_addr),
                                     app_db.br_link[inactive_a2dp_idx].streaming_fg);
                }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                else if (app_db.br_link[inactive_a2dp_idx].streaming_fg == true)
                {
                    if (app_db.remote_loc != BUD_LOC_IN_CASE)
                    {
                        app_multi_stream_avrcp_set(inactive_a2dp_idx);

                        app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
                        if (app_multi_allow_a2dp_sniffing(inactive_a2dp_idx))
                        {
                            app_multi_set_active_idx(inactive_a2dp_idx);
                            if (app_bt_sniffing_start(app_db.br_link[inactive_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
                        }
                    }
                }
#endif
            }
            else
            {
                uint8_t idx = MAX_BR_LINK_NUM;
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i) &&
                        app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK | HFP_PROFILE_MASK |
                                                               HSP_PROFILE_MASK))
                    {
                        idx = i;
                        break;
                    }
                }
                app_multi_set_active_idx(idx);
            }
        }
        break;

    default:
        break;
    }

    app_multi_update_a2dp_play_status(event);

#endif
}

static void app_multi_hfp_audio_connect_confirm(bool need_start_sniffing, uint8_t *bd_addr)
{
    bool ret = false;

    if (need_start_sniffing)
    {
        ret = app_bt_sniffing_start(bd_addr, BT_SNIFFING_TYPE_SCO);
    }

    if (!ret)
    {
#if F_APP_LEA_SUPPORT
        bool need_return = false;
        if (mtc_if_fm_ml_handle(ML_TO_MTC_CH_LEA_CALL, NULL, &need_return) == MTC_RESULT_SUCCESS)
        {
            if (need_return)
            {
                bt_hfp_audio_connect_cfm(bd_addr, false);
                return;
            }
        }
#endif
        bt_hfp_audio_connect_cfm(bd_addr, true);
    }
}

static void app_multi_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    T_APP_BR_LINK *p_link = NULL;
    T_BT_EVENT_PARAM *param = event_buf;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            p_link = app_link_find_br_link(param->acl_conn_success.bd_addr);
            if (p_link != NULL)
            {
                if (app_multi_get_active_idx() >= MAX_BR_LINK_NUM)
                {
                    app_multi_set_active_idx(p_link->id);
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            //b2b or b2s link disconnected => reset preemptive flag
            app_db.a2dp_preemptive_flag = false;
#if F_APP_HARMAN_FEATURE_SUPPORT
            app_harman_acl_disconn_handle(param->acl_conn_disconn.bd_addr);
#endif
            if (memcmp(app_db.active_hfp_addr, param->acl_conn_disconn.bd_addr, 6) == 0)
            {
                if (app_link_get_b2s_link_num() == 0)
                {
                    app_multi_set_active_idx(MAX_BR_LINK_NUM);
                }
                else if (!app_multi_check_in_sniffing())
                {
                    app_multi_handle_sniffing_link_disconnect_event(0xff);
                }
            }
        }
        break;

    case BT_EVENT_SCO_CONN_IND:
        {
            p_link = app_link_find_br_link(param->sco_conn_ind.bd_addr);
            if (p_link != NULL)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_hfp_adjust_sco_window(param->sco_conn_ind.bd_addr, APP_SCO_ADJUST_SCO_CONN_IND_EVT);
                    if (app_cfg_const.enable_multi_sco_disc_resume)
                    {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == p_link->id)
                        {
                            pending_va_sniffing_type = BT_SNIFFING_TYPE_SCO;
                        }

                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, true) && app_multi_check_in_sniffing()
#if GFPS_SASS_SUPPORT
                            && (uint8_t)app_sass_preempt_policy(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM, false)
#endif
                           )
#else
                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, true))
#endif
                        {
                            // A2DP Paused
                            app_db.sco_wait_a2dp_sniffing_stop = true;
                            app_db.pending_sco_idx = p_link->id;
                        }
                        else
                        {
                            // No A2DP
                            app_multi_hfp_audio_connect_confirm(true, param->sco_conn_ind.bd_addr);
                        }
                    }
                    else
                    {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == p_link->id)
                        {
                            pending_va_sniffing_type = BT_SNIFFING_TYPE_SCO;
                        }

                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, false) && app_multi_check_in_sniffing()
#if GFPS_SASS_SUPPORT
                            && (uint8_t)app_sass_preempt_policy(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM, false)
#endif
                           )
#else
                        if (app_multi_pause_inactive_a2dp_link_stream(p_link->id, false))
#endif
                        {
                            // A2DP Paused
                            app_db.sco_wait_a2dp_sniffing_stop = true;
                            app_db.pending_sco_idx = p_link->id;
                        }
                        else
                        {
                            // No A2DP
                            app_multi_hfp_audio_connect_confirm(true, param->sco_conn_ind.bd_addr);
                        }
                    }
                }
                else
                {
                    app_multi_hfp_audio_connect_confirm(false, param->sco_conn_ind.bd_addr);
                }
            }
        }
        break;

    case BT_EVENT_SCO_CONN_RSP:
        {
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->sco_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (param->sco_conn_cmpl.cause == 0)
                {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                    //if (!p_link->call_status)
                    //{
                    app_teams_multilink_handle_bt_stream_event(p_link->id, true);
                    app_bt_policy_qos_param_update(param->sco_conn_cmpl.bd_addr, BP_TPOLL_SCO_CONN_EVENT);
                    //app_teams_multilink_high_app_action_trigger_low_app_action(p_link->id, T_APP_TEAMS_MULTILINK_HIGH_PRIORITY_APP_START);
                    //}
#else
#if F_APP_GAMING_DONGLE_SUPPORT
                    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) &&
                        (app_cfg_const.enable_multi_link) &&
                        (app_link_check_dongle_link(app_db.br_link[active_hf_idx].bd_addr)))
                    {
                        APP_PRINT_TRACE0("BT_EVENT_SCO_CONN_CMPL: PAUSE SCO_CONN");
                        gap_br_vendor_set_active_sco(p_link->sco_handle, 0, 2);
                    }
                    else if (app_link_get_sco_conn_num() == 1)
#else
                    if (app_link_get_sco_conn_num() == 1)
#endif
                    {
                        bt_sco_link_switch(param->sco_conn_cmpl.bd_addr);
                    }
                    else
                    {
                        APP_PRINT_TRACE2("app_multi_bt_cback: active sco link %s, current link %s",
                                         TRACE_BDADDR(app_db.br_link[active_hf_idx].bd_addr),
                                         TRACE_BDADDR(param->sco_conn_cmpl.bd_addr));

                        bt_sco_link_switch(app_db.br_link[active_hf_idx].bd_addr);
                    }

                    bool disc = app_multi_preemptive_judge(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM);

#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (p_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        if (p_link->id != active_va_idx
                            //  || ((active_hf_idx != p_link->id) &&
                            //      (p_link->sco_handle) &&
                            //      (app_db.br_link[active_hf_idx].call_status != APP_VOICE_ACTIVATION_ONGOING))
                           )
                        {
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                            bt_hfp_voice_recognition_disable_req(p_link->bd_addr);
                            app_stop_timer(&timer_idx_multilink_disc_va_sco);
                            app_start_timer(&timer_idx_multilink_disc_va_sco, "multilink_disc_va_sco",
                                            multilink_timer_id, APP_TIMER_MULTILINK_DISC_VA_SCO, p_link->id, false,
                                            1500);
#endif
                        }
                    }
                    else if (((active_hf_idx != p_link->id) && (p_link->sco_handle))
                             || (!disc))
                    {
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                        if (app_db.br_link[p_link->id].call_status != APP_CALL_INCOMING &&
                            p_link->id != active_hfp_transfer)
                        {
                            bt_hfp_audio_disconnect_req(p_link->bd_addr);
                            if (app_cfg_const.enable_multi_link)
                            {
                                sco_wait_from_sco_sniffing_stop = true;
                                pending_sco_index_from_sco = p_link->id;
                            }
                        }
#endif
                    }
#else
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
                    if ((active_hf_idx != p_link->id) && (p_link->sco_handle))
                    {
                        bt_hfp_audio_disconnect_req(p_link->bd_addr);
                        sco_wait_from_sco_sniffing_stop = true;
                        pending_sco_index_from_sco = p_link->id;
                    }
#endif
#endif
#endif
#if F_APP_POWER_TEST
                    power_test_dump_register(POWER_DEBUG_STAGE_HFP_START);
#endif
                }
                else
                {
                    if ((active_hf_idx != p_link->id))
                    {
                        sco_wait_from_sco_sniffing_stop = true;
                        pending_sco_index_from_sco = p_link->id;
                    }
                }
            }
            if (app_cfg_const.always_play_hf_incoming_tone_when_incoming_call)
            {
                app_hfp_inband_tone_mute_ctrl();
            }

            app_bt_policy_primary_engage_action_adjust();

            // app_bt_policy_roleswitch_when_sco(app_db.br_link[app_multi_find_inacitve(active_hf_idx)].bd_addr);
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
            if (param->sco_disconnected.cause == (HCI_ERR | HCI_ERR_CMD_DISALLOWED))
            {
                break;
            }

            app_bt_sniffing_stop(param->sco_disconnected.bd_addr, BT_SNIFFING_TYPE_SCO);

            p_link = app_link_find_br_link(param->sco_disconnected.bd_addr);
            if (p_link != NULL)
            {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
                app_teams_multilink_handle_bt_stream_event(p_link->id, false);
                //if (app_teams_multiink_get_record_status_by_link_id(p_link->id))
                //{
                //    app_teams_multilink_record_priority_rearrangment(p_link->id, false);
                //}
#else

                //if (p_link->id == active_hf_idx)
                {
                    uint8_t i;

                    //set actvie sco
                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_db.br_link[i].sco_handle)
                        {
                            bt_sco_link_switch(app_db.br_link[i].bd_addr);
                            break;
                        }
                    }
                }

                if (active_hfp_transfer != 0xFF)
                {
                    if (!app_multi_check_in_sniffing())
                    {
                        app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                    }
                }

                if (app_cfg_const.enable_multi_sco_disc_resume)
                {
                    if ((app_cfg_const.enable_multi_link) &&
                        (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
                    {
                        APP_PRINT_TRACE4("sco_disc_resume: sco_num: %d, hfp_active: %d, p_link->id: %d, hfp_status: %d",
                                         app_link_get_sco_conn_num(),
                                         active_hf_idx,
                                         p_link->id,
                                         app_hfp_get_call_status());
                        if ((app_link_get_sco_conn_num() == 0) &&
                            (app_hfp_get_call_status() == APP_CALL_IDLE))
                        {
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
                            app_multilink_customer_sco_disconneted();
#else
                            app_multi_resume_a2dp_link_stream(p_link->id);
#endif
                        }
                    }
                }

#endif
#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (p_link->id == pending_va_index_from_va)
                {
                    va_wait_from_va_sniffing_stop = false;
                    pending_va_index_from_va = 0xFF;
                    pending_va_sniffing_type = 0xFF;
                }
#endif
#if F_APP_POWER_TEST
                power_test_dump_register(POWER_DEBUG_STAGE_HFP_STOP);
#endif
#if F_APP_HARMAN_FEATURE_SUPPORT
                app_harman_sco_handle(p_link->id, false);
#endif
            }

            if (app_cfg_const.always_play_hf_incoming_tone_when_incoming_call)
            {
                app_hfp_inband_tone_mute_ctrl();
            }
            app_bt_policy_primary_engage_action_adjust();

            // app_bt_policy_roleswitch_when_sco(app_db.br_link[app_multi_find_inacitve(active_hf_idx)].bd_addr);

            app_bt_policy_qos_param_update(param->sco_disconnected.bd_addr, BP_TPOLL_SCO_DISC_EVENT);
        }
        break;

    case BT_EVENT_A2DP_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->a2dp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_CONNECTED);
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(param->a2dp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_DISC);
            }
        }
        break;


    case BT_EVENT_A2DP_STREAM_OPEN:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_open.bd_addr);
            if (p_link != NULL)
            {
                if (app_multi_get_active_idx() >= MAX_BR_LINK_NUM)
                {
                    app_multi_set_active_idx(p_link->id);
                }
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_CONNECTED);
            }
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            p_link = app_link_find_br_link(param->avrcp_play_status_changed.bd_addr);

            if (p_link != NULL)
            {
                APP_PRINT_TRACE2("BT_EVENT_AVRCP_PLAY_STATUS_CHANGED %d ,%d", p_link->id,
                                 param->avrcp_play_status_changed.play_status);

#if F_APP_DURIAN_SUPPORT
                pre_avrcp[p_link->id] = param->avrcp_play_status_changed.play_status;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
                if (event_type == BT_EVENT_AVRCP_PLAY_STATUS_RSP &&
                    app_link_check_dongle_link(param->avrcp_play_status_changed.bd_addr))
                {
                    break;
                }
#endif

                if (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                {
                    app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    stream_only[p_link->id] = false;
#endif
                    if (p_link->id == multilink_silence_packet_idx)
                    {
                        app_stop_timer(&timer_idx_multilink_silence_packet_judge);
                    }

                    app_stop_timer(&timer_idx_multilink_silence_packet_detect);
                    app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_MEDIAPLAYER_PLAYING);
#if F_APP_HARMAN_FEATURE_SUPPORT
                    app_harman_is_record_a2dp_active_ever_set(true);
#endif
                }
                else if (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
                {
                    app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_MEDIAPLAYER_PAUSED);
                }
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            uint8_t pair_idx;
            p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);
#if F_APP_DUAL_AUDIO_EFFECT
#if F_APP_SENSOR_MEMS_SUPPORT
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO == 0
            if (p_link != NULL)
            {
                app_sensor_mems_start(p_link->id);
            }
#endif
#endif
#endif
            if (p_link != NULL)
            {
                if (app_cfg_const.rws_gaming_mode == 0)
                {
                    APP_PRINT_TRACE5("app_multi_bt_cback: p_link %p, id %d, active id %d, streaming_fg %d / %d",
                                     p_link, p_link->id, active_a2dp_idx, p_link->streaming_fg,
                                     app_db.br_link[active_a2dp_idx].streaming_fg);
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    uint8_t i;
                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_link_check_b2s_link_by_id(i))
                        {
                            if (i != p_link->id)
                            {
                                break;
                            }
                        }
                    }

                    if ((app_cfg_const.enable_multi_link) &&
                        (i < MAX_BR_LINK_NUM) &&
                        (app_db.br_link[i].streaming_fg == true))
                    {
                        app_db.a2dp_preemptive_flag = true;
                        app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE);
                    }

                    if (va_wait_from_va_sniffing_stop && pending_va_index_from_va == p_link->id)
                    {
                        pending_va_sniffing_type = BT_SNIFFING_TYPE_A2DP;
                    }
#endif
                    if (p_link->id != active_a2dp_idx)
                    {
                        // not active link
                        if (app_db.br_link[active_a2dp_idx].streaming_fg == false)
                        {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                            if (!app_multi_allow_a2dp_sniffing(p_link->id) ||
                                !app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP))
#else
                            if (!app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP))
#endif
                            {
                                bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                                p_link->streaming_fg = true;
                                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                            }

                            if (app_cfg_const.disable_multilink_preemptive)
                            {
                                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                            }
                        }
                        else
                        {
                            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                            p_link->streaming_fg = true;
                            app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                        }
                    }
                    else
                    {
#if F_APP_HARMAN_FEATURE_SUPPORT
                        if (pending_id == active_a2dp_idx)
                        {
                            app_stop_timer(&timer_idx_multilink_a2dp_stop);
                            pending_id = 0xff;
                            app_multi_a2dp_stop(active_a2dp_idx);
                        }
#endif
                        // active link
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        if (p_link->streaming_fg == false && (va_wait_from_va_sniffing_stop ||
                                                              !app_multi_allow_a2dp_sniffing(p_link->id) ||
                                                              !app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP)))
                        {
                            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                            p_link->streaming_fg = true;
                            app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                        }
#else
                        if (p_link->streaming_fg == false &&
                            !app_bt_sniffing_start(param->a2dp_stream_start_ind.bd_addr, BT_SNIFFING_TYPE_A2DP))
                        {
                            bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true);
                            p_link->streaming_fg = true;
                            app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                        }
#endif
                    }
#if F_APP_POWER_TEST
                    power_test_dump_register(POWER_DEBUG_STAGE_A2DP_START);
#endif
                }
                else
                {
                    p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);
                    if (p_link != NULL)
                    {
                        if (bt_a2dp_stream_start_cfm(param->a2dp_stream_start_ind.bd_addr, true))
                        {
                            p_link->streaming_fg = true;
                        }
                        app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
                    }
                }
            }
            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_A2DP_STREAM);
            app_bt_policy_primary_engage_action_adjust();

            if (p_link)
            {
                if (bt_bond_index_get(p_link->bd_addr, &pair_idx))
                {
                    app_cfg_nv.last_btmode[pair_idx] = BP_LINK_LAST_BTMODE_LEGACY;
                    APP_PRINT_INFO1("set last mode to legacy: %s", TRACE_BDADDR(p_link->bd_addr));
                }
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_RSP:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_start_rsp.bd_addr);
            if (p_link != NULL)
            {
                p_link->streaming_fg = true;
                app_multi_judge_active_a2dp_idx_and_qos(p_link->id, JUDGE_EVENT_A2DP_START);
            }
            app_bt_policy_primary_engage_action_adjust();

        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            p_link = app_link_find_br_link(param->a2dp_stream_stop.bd_addr);
            if (p_link != NULL && p_link->streaming_fg)
            {
                app_harman_total_playback_time_update();
                p_link->streaming_fg = false;
#if F_APP_HARMAN_FEATURE_SUPPORT
                if (timer_idx_multilink_a2dp_stop == 0 && p_link->id == active_a2dp_idx)
                {
                    app_start_timer(&timer_idx_multilink_a2dp_stop, "multilink_a2dp_stop",
                                    multilink_timer_id, APP_TIMER_MULTILINK_HARMAN_A2DP_STOP, p_link->id, false,
                                    1000);
                    pending_id = p_link->id;
                }
                else
#endif
                {
                    app_multi_a2dp_stop(p_link->id);
                }
                app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_A2DP_STREAM);
            }

#if F_APP_POWER_TEST
            power_test_dump_register(POWER_DEBUG_STAGE_A2DP_STOP);
#endif
        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
            p_link = app_link_find_br_link(param->hfp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (app_multi_get_active_idx() >= MAX_BR_LINK_NUM)
                {
                    app_multi_set_active_idx(p_link->id);
                }
            }
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_ACTIVATION:
        {
            if (param->hfp_voice_recognition_activation.result == BT_HFP_CMD_OK)
            {
                uint8_t app_idx;
                T_APP_BR_LINK *p_link;
                app_idx = app_a2dp_get_active_idx();
                p_link = &(app_db.br_link[app_idx]);

                if (p_link != NULL)
                {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    T_APP_BR_LINK *p_link_va;

                    p_link_va = app_link_find_br_link(param->hfp_voice_recognition_activation.bd_addr);
                    if (p_link_va != NULL)
                    {
                        active_va_idx = p_link_va->id;
                        app_check_voice_dial(p_link_va);
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_DEACTIVATION:
        {
            if (param->hfp_voice_recognition_deactivation.result == BT_HFP_CMD_OK)
            {

            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            uint8_t cmd_ptr[7];
            T_APP_BR_LINK *p_link, *p_link_call;

            p_link = &(app_db.br_link[active_a2dp_idx]);
            p_link_call = app_link_find_br_link(param->hfp_call_status.bd_addr);

            if (p_link_call != NULL && p_link_call->call_status == BT_HFP_CALL_IDLE)
            {
                if (p_link_call->id == pending_sco_index_from_sco)
                {
                    sco_wait_from_sco_sniffing_stop = false;
                    pending_sco_index_from_sco = 0xFF;
                }
                if (p_link->id == active_hfp_transfer)
                {
                    active_hfp_transfer = 0xFF;
                }
            }

            if (p_link != NULL)
            {
                if (param->hfp_call_status.curr_status != BT_HFP_CALL_IDLE)
                {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (p_link_call != NULL)
                    {
                        if (p_link_call->call_status == BT_HFP_CALL_INCOMING)
                        {
                            active_va_idx = p_link_call->id;
                        }
                        app_multi_preemptive_judge(p_link_call->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM);
                    }

#endif
#if F_APP_GAMING_DONGLE_SUPPORT
                    if ((!app_cfg_const.enable_multi_link) || (p_link->id != app_hfp_get_active_idx()))
#endif
                    {
#if F_APP_ERWS_SUPPORT
                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                        {
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY
#if GFPS_SASS_SUPPORT
                                && (uint8_t)app_sass_preempt_policy(app_link_find_br_link(param->hfp_call_status.bd_addr)->id,
                                                                    SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM, false)
#endif
                               )
                            {
#if F_APP_MUTILINK_VA_PREEMPTIVE && F_APP_ERWS_SUPPORT
                                if (app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING)
                                {
                                    app_bt_sniffing_a2dp_mute(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                                }
                                else
#endif
                                {
                                    memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                                    cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK,
                                                                        APP_REMOTE_MSG_MUTE_AUDIO_WHEN_MULTI_CALL_NOT_IDLE,
                                                                        cmd_ptr, 7);
                                }
                            }
                        }
                        else
#endif
                        {
#if F_APP_TEAMS_BT_POLICY || F_APP_GAMING_DONGLE_SUPPORT
#else
                            audio_track_volume_out_mute(p_link->a2dp_track_handle);
#endif
                        }
                    }
                }
                else
                {
#if F_APP_ERWS_SUPPORT
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                        {
#if F_APP_MUTILINK_VA_PREEMPTIVE && F_APP_ERWS_SUPPORT
                            if (app_db.br_link[active_a2dp_idx].bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING)
                            {
                                app_bt_sniffing_a2dp_unmute(app_db.br_link[active_a2dp_idx].bd_addr, BT_SNIFFING_TYPE_A2DP);
                            }
#endif
                            memcpy(&cmd_ptr[0], app_db.br_link[active_a2dp_idx].bd_addr, 6);
                            cmd_ptr[6] = AUDIO_STREAM_TYPE_PLAYBACK;
                            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK,
                                                                APP_REMOTE_MSG_UNMUTE_AUDIO_WHEN_MULTI_CALL_IDLE,
                                                                cmd_ptr, 7);
                        }
                    }
                    else
#endif
                    {
#if F_APP_TEAMS_BT_POLICY
#else
                        audio_track_volume_out_unmute(p_link->a2dp_track_handle);
                        p_link->playback_muted = false;
#endif
                    }
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    if (!app_multi_check_in_sniffing())
                    {
                        T_APP_BR_LINK *p_link;
                        p_link = app_link_find_br_link(param->hfp_call_status.bd_addr);
                        if (p_link != NULL)
                        {
                            app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                        }
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            p_link = app_link_find_br_link(param->hfp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->id == pending_sco_index_from_sco)
                {
                    sco_wait_from_sco_sniffing_stop = false;
                    pending_sco_index_from_sco = 0xFF;
                }

                if (p_link->id == active_hfp_transfer)
                {
                    active_hfp_transfer = 0xFF;
                }

#if F_APP_MUTILINK_VA_PREEMPTIVE
                if (p_link->id == pending_va_index_from_va)
                {
                    va_wait_from_va_sniffing_stop = false;
                    pending_va_index_from_va = 0xFF;
                    pending_va_sniffing_type = 0xFF;
                }
#endif
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_multi_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_multi_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_multi_timeout_cb: timer_evt %d, param %d", timer_evt, param);
    switch (timer_evt)
    {
    case APP_TIMER_MULTILINK_DISCONN_INACTIVE_ACL:
        {
            app_stop_timer(&timer_idx_multilink_disconn_inctive_acl);

            if (app_db.br_link[param].used == true)
            {
                gap_br_send_acl_disconn_req(app_db.br_link[param].bd_addr);
            }
        }
        break;

#if F_APP_MUTILINK_VA_PREEMPTIVE
    case APP_TIMER_MULTILINK_DISC_VA_SCO:
        {
            app_stop_timer(&timer_idx_multilink_disc_va_sco);
            if (app_db.br_link[param].sco_handle != NULL)
            {
#if F_APP_HARMAN_FEATURE_SUPPORT
#else
                bt_hfp_audio_disconnect_req(app_db.br_link[param].bd_addr);
                app_bt_sniffing_stop(app_db.br_link[param].bd_addr, BT_SNIFFING_TYPE_SCO);
#endif
            }
        }
        break;
#endif
    case APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE:
        {
            app_stop_timer(&timer_idx_multilink_silence_packet_judge);
#if F_APP_LEA_SUPPORT
            bool need_return = false;
            if ((mtc_if_fm_ml_handle(ML_TO_MTC_CH_SNIFFING, NULL, &need_return) == MTC_RESULT_SUCCESS) &&
                need_return)
            {
                APP_PRINT_TRACE0("APP_TIMER_MULTILINK_SILENCE_PACKET_JUDGE: STOP SNIFFING");
                le_sniffing_stop = true;
                break;

            }
#endif
            if (app_db.br_link[param].streaming_fg == true)
            {
                app_multi_set_active_idx(param);
                app_multi_stream_avrcp_set(param);
                app_multi_pause_inactive_a2dp_link_stream(param, false);
            }
            else
            {
                app_start_timer(&timer_idx_multilink_silence_packet_detect, "multilink_silence_packet_detect",
                                multilink_timer_id, APP_TIMER_MULTILINK_SILENCE_PACKET_DETECT, 0, false,
                                7500);
                app_multi_set_active_idx(app_multi_get_active_idx());
            }
        }
        break;

    case APP_TIMER_MULTILINK_SILENCE_PACKET_DETECT:
        {
            app_stop_timer(&timer_idx_multilink_silence_packet_detect);
        }
        break;

#if F_APP_HARMAN_FEATURE_SUPPORT
    case APP_TIMER_MULTILINK_HARMAN_A2DP_STOP:
        {
            app_stop_timer(&timer_idx_multilink_a2dp_stop);
            pending_id = 0xff;
            app_multi_a2dp_stop(param);
        }
        break;
#endif

    default:
        break;
    }
}

bool app_multi_checking_silence_pkts(void)
{
    if (timer_idx_multilink_silence_packet_detect)
    {
        return true;
    }
    return false;
}

void app_multi_stop_silence_check_timer(void)
{
    app_stop_timer(&timer_idx_multilink_silence_packet_detect);
}

#if F_APP_ERWS_SUPPORT
uint16_t app_multi_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC:
        {
            payload_len = 6;
            msg_ptr = (uint8_t *)app_db.br_link[app_hfp_get_active_idx()].bd_addr;
        }
        break;

    case APP_REMOTE_MSG_PHONE_CONNECTED:
        {
            static uint8_t num = 0;
            num = app_link_get_b2s_link_num();
            payload_len = 1;
            msg_ptr = &num;
        }
        break;

    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_MULTI_LINK, payload_len, msg_ptr, skip,
                              total);
}

static void app_multi_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                  T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_MUTE_AUDIO_WHEN_MULTI_CALL_NOT_IDLE:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t bd_addr[6];
                T_AUDIO_STREAM_TYPE stream_type;
                T_APP_BR_LINK *p_link;

                memcpy(bd_addr, &p_info[0], 6);
                p_link = app_link_find_br_link(bd_addr);
                stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];
                if (p_link != NULL)
                {
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                    {
                        audio_track_volume_out_mute(p_link->a2dp_track_handle);
                        p_link->playback_muted = true;
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_UNMUTE_AUDIO_WHEN_MULTI_CALL_IDLE:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                uint8_t bd_addr[6];
                T_AUDIO_STREAM_TYPE stream_type;
                T_APP_BR_LINK *p_link;

                memcpy(bd_addr, &p_info[0], 6);
                p_link = app_link_find_br_link(bd_addr);
                stream_type = (T_AUDIO_STREAM_TYPE)p_info[6];
                if (p_link != NULL)
                {
                    if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
                    {
                        audio_track_volume_out_unmute(p_link->a2dp_track_handle);
                        p_link->playback_muted = false;
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_RESUME_BT_ADDRESS:
        {
            memcpy(app_db.resume_addr, (uint8_t *)buf, 6);
            app_db.connected_num_before_roleswap = *((uint8_t *)buf + 6);
            APP_PRINT_TRACE2("app_multi_parse_cback: connected_num_before_roleswap %d, resume_addr %s",
                             app_db.connected_num_before_roleswap, TRACE_BDADDR(app_db.resume_addr));
        }
        break;

#if F_APP_MUTILINK_VA_PREEMPTIVE
    case APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC:
        {
            if (app_cfg_const.enable_multi_link)
            {
                if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                    (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
                {
                    uint8_t *p_info = (uint8_t *)buf;
                    memcpy(app_db.active_hfp_addr, &p_info[0], 6);
                }
            }
        }
        break;
#endif

    case APP_REMOTE_MSG_PROFILE_CONNECTED:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            app_bond_handle_remote_profile_connected_msg(buf);
        }
        break;

    case APP_REMOTE_MSG_PHONE_CONNECTED:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD &&
                app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t num = *((uint8_t *)buf);
                app_link_set_b2s_link_num(num);
                APP_PRINT_TRACE1("app_multi_parse_cback: b2s_connected_num %d", num);
#if F_APP_LEA_SUPPORT
                if (app_bt_point_lea_link_is_full())
                {
                    app_lea_adv_stop();
                }
                else
                {
                    app_lea_adv_start();
                }
#endif

#if F_APP_LISTENING_MODE_SUPPORT
                if (app_cfg_const.disallow_listening_mode_before_phone_connected)
                {
                    if (app_link_get_b2s_link_num())
                    {
                        app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_CONNECTED);
                    }
                    else
                    {
                        app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_DISCONNECTED);
                    }
                }
#endif
            }
        }
        break;

    default:
        break;
    }
}
#endif

#if F_APP_LEA_SUPPORT
T_MTC_RESULT app_multi_mtc_if_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT app_result = MTC_RESULT_SUCCESS;
    APP_PRINT_INFO2("app_multi_mtc_if_handle: msg %x, %d", msg, app_cfg_nv.bud_role);

    switch (msg)
    {
    case ML_FM_MTC_LEA_START:
        {
        }
        break;

    case ML_FM_MTC_LEA_STOP:
        {
            if (le_sniffing_stop == true)
            {
                le_sniffing_stop = false;
                if (!app_multi_check_in_sniffing())
                {
                    app_multi_handle_sniffing_link_disconnect_event(0xFF);
                }
            }
        }
        break;

    default:
        break;
    }
    return app_result;
}
#endif

void app_multi_init(void)
{
    app_db.wait_resume_a2dp_idx = MAX_BR_LINK_NUM;
    bt_mgr_cback_register(app_multi_bt_cback);
    app_timer_reg_cb(app_multi_timeout_cb, &multilink_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_multi_relay_cback, app_multi_parse_cback,
                             APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_MULTILINK_TOTAL);
#endif

#if F_APP_LEA_SUPPORT
    mtc_cback_register(app_multi_mtc_if_handle);
#endif
}

#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
void app_multilink_sco_preemptive(uint8_t inactive_idx)
{
    APP_PRINT_TRACE1("app_multilink_sco_preemptive:%d", inactive_idx);
    bt_hfp_audio_disconnect_req(app_db.br_link[inactive_idx].bd_addr);
    if (app_cfg_const.enable_multi_link)
    {
        sco_wait_from_sco_sniffing_stop = true;
        pending_sco_index_from_sco = inactive_idx;
    }
}
#endif

#if F_APP_MUTILINK_VA_PREEMPTIVE

#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
void app_multi_voice_ongoing_preemptive(uint8_t disable_idx, uint8_t idx)
{
#if F_APP_DURIAN_SUPPORT
    if (app_db.br_link[disable_idx].connected_profile & AVP_PROFILE_MASK)
    {
        app_durian_mmi_voice_recognition_disable(disable_idx);
    }
    else
#endif
    {
        bt_hfp_voice_recognition_disable_req(app_db.br_link[disable_idx].bd_addr);

        va_wait_from_va_sniffing_stop = true;
        pending_va_index_from_va = idx;
        pending_va_sniffing_type = 0xFF;
        app_stop_timer(&timer_idx_multilink_disc_va_sco);
        app_start_timer(&timer_idx_multilink_disc_va_sco, "multilink_disc_va_sco",
                        multilink_timer_id, APP_TIMER_MULTILINK_DISC_VA_SCO, disable_idx, false,
                        1500);
    }
}
#endif

void app_check_voice_dial(T_APP_BR_LINK *p_link)
{
#if F_APP_MUTILINK_ALLOW_TWO_SCO
#else
    uint8_t active_hf_idx = app_hfp_get_active_idx();
    uint8_t idx = p_link->id;

    if (idx != active_hf_idx)
    {
        if (app_db.br_link[active_hf_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
        {
#if F_APP_MUTILINK_VA_PREEMPTIVE
            app_multi_voice_ongoing_preemptive(active_hf_idx, idx);
#endif
        }
        else if (app_db.br_link[active_hf_idx].call_status == APP_CALL_IDLE)
        {
            if (app_db.br_link[active_hf_idx].sco_handle)
            {
                bt_hfp_audio_disconnect_req(app_db.br_link[active_hf_idx].bd_addr);
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (i != active_hf_idx)
            {
                if (app_db.br_link[i].call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    app_multi_voice_ongoing_preemptive(i, idx);
#endif
                }
                else if ((app_db.br_link[i].sco_handle) &&
                         (app_db.br_link[i].call_status != APP_CALL_INCOMING))
                {
                    bt_hfp_audio_disconnect_req(app_db.br_link[i].bd_addr);
                }
            }
        }
    }
#endif
}
#endif

uint8_t app_multi_find_inacitve(uint8_t idx)
{
    uint8_t inactive_idx = MAX_BR_LINK_NUM;

    if (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)
    {
        for (uint8_t j = 0; j < MAX_BR_LINK_NUM; j++)
        {
            if ((app_link_check_b2s_link_by_id(j)) && (j != idx))
            {
                inactive_idx = j;
            }
        }
    }
    return inactive_idx;
}

uint8_t app_multi_get_acl_connect_num(void)
{
    uint8_t conn_num = 0;
    uint8_t i = 0;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        APP_PRINT_TRACE3("get_acl_le_link_connect_num %d %d %d",
                         i,
                         app_db.le_link[i].state,
                         app_db.le_link[i].used);

        if ((app_db.le_link[i].state == LE_LINK_STATE_CONNECTED) &&
            (app_db.le_link[i].used == true) &&
            (app_db.le_link[i].cmd_set_enable == true))
        {
            conn_num++;
        }
    }
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        APP_PRINT_TRACE3("get_acl_br_link_connect_num %d %d 0x%x",
                         i,
                         app_db.br_link[i].acl_handle,
                         app_db.br_link[i].connected_profile);

        if ((app_db.br_link[i].connected_profile & (SPP_PROFILE_MASK | IAP_PROFILE_MASK)) &&
            (app_db.br_link[i].cmd_set_enable == true))
        {
            conn_num++;
        }
    }
    APP_PRINT_TRACE1("CONN_NUM = %d", conn_num);
    return conn_num;
}

void app_multi_set_active_idx(uint8_t idx)
{
    APP_PRINT_TRACE2("app_multi_set_active_idx: %d, %d", g_active_idx, idx);

    if (g_active_idx != idx)
    {
        g_active_idx = idx;

#if F_APP_MUTILINK_VA_PREEMPTIVE
        if (idx < MAX_BR_LINK_NUM)
        {
            memcpy(app_db.active_hfp_addr, app_db.br_link[idx].bd_addr, 6);
        }
        else
        {
            memset(app_db.active_hfp_addr, 0, 6);
        }
#endif

#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_async_single(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_ACTIVE_BD_ADDR_SYNC);
        }
#endif
    }

#if GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_support)
    {
        if (!mtc_get_btmode())
        {
            app_gfps_notify_conn_status();

            if (app_db.br_link[idx].call_status != BT_HFP_CALL_IDLE)
            {
                app_gfps_msg_notify_multipoint_switch(idx, SASS_EDR_LINK, GFPS_MULTIPOINT_SWITCH_REASON_HFP);
            }
            else if (app_db.br_link[idx].streaming_fg)
            {
                app_gfps_msg_notify_multipoint_switch(idx, SASS_EDR_LINK, GFPS_MULTIPOINT_SWITCH_REASON_A2DP);
            }
            else
            {
                app_gfps_msg_notify_multipoint_switch(idx, SASS_EDR_LINK, GFPS_MULTIPOINT_SWITCH_REASON_UNKNOWN);
            }
        }
    }
#endif
}

uint8_t app_multi_get_active_idx(void)
{
    return g_active_idx;
}

bool app_multi_get_stream_only(uint8_t idx)
{
    if (idx < MAX_BR_LINK_NUM)
    {
#if F_APP_MUTILINK_VA_PREEMPTIVE
        return stream_only[idx];
#endif
    }

    return false;
}

void app_multi_set_stream_only(uint8_t idx, bool streaming)
{
    if (idx < MAX_BR_LINK_NUM)
    {
#if F_APP_MUTILINK_VA_PREEMPTIVE
        stream_only[idx] = streaming;
#endif
    }
}

