/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "btm.h"
#include "bt_a2dp.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_multilink.h"
#include "app_link_util.h"
#include "app_a2dp.h"
#include "app_bt_sniffing.h"
#include "app_sniff_mode.h"
#include "app_hfp.h"
#include "app_audio_policy.h"

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_behaviors.h"
#endif

static uint8_t active_a2dp_idx = 0;

static void app_a2dp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_active_a2dp_link = &app_db.br_link[active_a2dp_idx];
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_A2DP_CONN_IND:
        {
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->a2dp_conn_ind.bd_addr);
            if (p_link != NULL)
            {
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
                bool is_dongle_link = false;

                if (!memcmp(app_db.connected_dongle_addr, param->a2dp_conn_ind.bd_addr, 6))
                {
                    is_dongle_link = true;
                }

                if (app_cfg_const.enable_24g_bt_audio_source_switch &&
                    (((app_cfg_nv.allowed_source == ALLOWED_SOURCE_DONGLE) && !is_dongle_link) ||
                     ((app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT) && is_dongle_link)))
                {
                    /* audio route select 2.4G but bt a2dp conn ind or
                       audio route select bt but 2.4G a2dp conn ind,
                       reject this conn */
                    APP_PRINT_TRACE3("app_a2dp_bt_cback: reject a2dp connect, bd_addr %s, allowed_source %d, is_dongle_link %d",
                                     TRACE_BDADDR(param->a2dp_conn_ind.bd_addr), app_cfg_nv.allowed_source, is_dongle_link);

                    bt_a2dp_connect_cfm(p_link->bd_addr, false);
                }
                else
#endif
                {
                    bt_a2dp_connect_cfm(p_link->bd_addr, true);
                }
            }
        }
        break;

    case BT_EVENT_A2DP_CONFIG_CMPL:
        {
        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_A2DP_SNIFFING_CONN_CMPL:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                T_APP_BR_LINK *p_link = app_link_find_br_link(param->a2dp_sniffing_conn_cmpl.bd_addr);
                if (p_link != NULL)
                {
                    app_a2dp_set_active_idx(p_link->id);
                }
            }
        }
        break;
#endif

    case BT_EVENT_A2DP_STREAM_OPEN:
        {
            // Stream Channel connected
            // For Gaming mode
            if (app_cfg_const.rws_gaming_mode == 1)
            {
                if (app_bt_sniffing_start(param->a2dp_stream_open.bd_addr, BT_SNIFFING_TYPE_A2DP)) {};
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            if ((p_active_a2dp_link->streaming_fg == false ||
                 p_active_a2dp_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED) ||
                (memcmp(p_active_a2dp_link->bd_addr,
                        param->a2dp_stream_start_ind.bd_addr, 6) == 0))
            {
                APP_PRINT_INFO3("app_a2dp_bt_cback: BT_EVENT_A2DP_STREAM_START_IND active_a2dp_idx %d, streaming_fg %d, avrcp_play_status %d",
                                active_a2dp_idx, p_active_a2dp_link->streaming_fg, p_active_a2dp_link->avrcp_play_status);
                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_A2DP);

                app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_RSP:
        {
            if (p_active_a2dp_link->streaming_fg == false ||
                (memcmp(p_active_a2dp_link->bd_addr,
                        param->a2dp_stream_start_ind.bd_addr, 6) == 0))
            {
                APP_PRINT_INFO2("app_a2dp_bt_cback: BT_EVENT_A2DP_STREAM_START_RSP active_a2dp_idx %d, streaming_fg %d",
                                active_a2dp_idx, p_active_a2dp_link->streaming_fg);

                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_A2DP);

                app_audio_set_bud_stream_state(BUD_STREAM_STATE_AUDIO);
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_STOP:
        {
            if (memcmp(p_active_a2dp_link->bd_addr,
                       param->a2dp_stream_stop.bd_addr, 6) == 0)
            {
                if (app_link_get_a2dp_start_num() <= 1)
                {
                    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_A2DP);
                }

                if (app_hfp_get_call_status() == APP_CALL_IDLE)
                {
                    app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
                }

            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_CLOSE:
        {
            if (memcmp(p_active_a2dp_link->bd_addr,
                       param->a2dp_stream_close.bd_addr, 6) == 0)
            {
                if (app_hfp_get_call_status() == APP_CALL_IDLE)
                {
                    app_audio_set_bud_stream_state(BUD_STREAM_STATE_IDLE);
                }
            }
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
        APP_PRINT_INFO1("app_a2dp_bt_cback: event_type 0x%04x", event_type);
    }
}

bool app_a2dp_is_streaming(void)
{
    T_APP_BR_LINK *p_link = NULL;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        p_link = &app_db.br_link[i];

        if (p_link->streaming_fg == true)
        {
            return true;
        }
    }

    return false;
}

void app_a2dp_set_active_idx(uint8_t idx)
{
#if F_APP_GAMING_DONGLE_SUPPORT
    bool update = (active_a2dp_idx != idx);
#endif

    active_a2dp_idx = idx;

#if F_APP_HARMAN_FEATURE_SUPPORT
    if ((app_cfg_const.enable_multi_link) &&
        (app_link_get_connected_src_num() == MULTILINK_SRC_CONNECTED) &&
        (app_db.br_link[active_a2dp_idx].harman_silent_check == true))
    {
        APP_PRINT_TRACE1("[SD_CHECK] silent_dont_update1 %d", active_a2dp_idx);
    }
    else
    {
        app_harman_aling_active_hfp_idx_set(active_a2dp_idx);
    }
#endif
    APP_PRINT_TRACE1("app_a2dp_set_active_idx to %d", active_a2dp_idx);

#if F_APP_GAMING_DONGLE_SUPPORT
    if (update)
    {
        app_dongle_htpoll_control(DONGLE_HTPOLL_EVENT_ACTIVE_A2DP_IDX_CHANGED);
    }
#endif
}

uint8_t app_a2dp_get_active_idx(void)
{
    return active_a2dp_idx;
}

uint8_t app_a2dp_get_inactive_idx(void)
{
    uint8_t inactive_a2dp_idx = MAX_BR_LINK_NUM;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_link_check_b2s_link_by_id(i))
        {
            if (i != active_a2dp_idx)
            {
                inactive_a2dp_idx = i;
            }
        }
    }
    return inactive_a2dp_idx;
}

void app_a2dp_init(void)
{
    if (app_cfg_const.supported_profile_mask & A2DP_PROFILE_MASK)
    {
        uint16_t latency_value;
#if F_APP_HARMAN_FEATURE_SUPPORT
        latency_value = app_cfg_nv.cmd_latency_in_ms;
        app_cfg_const.aac_bit_rate = (app_cfg_nv.cmd_aac_bit_rate * 1000);
        app_cfg_const.sbc_max_bitpool = app_cfg_nv.cmd_sbc_max_bitpool;
#else
        app_audio_get_latency_value_by_level(AUDIO_STREAM_MODE_NORMAL, AUDIO_FORMAT_TYPE_AAC, 0,
                                             &latency_value);
#endif
        bt_a2dp_init(app_cfg_const.a2dp_link_number, latency_value);
        bt_a2dp_role_set(BT_A2DP_ROLE_SNK);
        if (app_cfg_const.a2dp_codec_type_sbc)
        {
            T_BT_A2DP_STREAM_ENDPOINT sep;

            sep.codec_type = BT_A2DP_CODEC_TYPE_SBC;
            bt_a2dp_stream_endpoint_delete(sep);

            sep.u.codec_sbc.sampling_frequency_mask = app_cfg_const.sbc_sampling_frequency;
            sep.u.codec_sbc.channel_mode_mask = app_cfg_const.sbc_channel_mode;
            sep.u.codec_sbc.block_length_mask = app_cfg_const.sbc_block_length;
            sep.u.codec_sbc.subbands_mask = app_cfg_const.sbc_subbands;
            sep.u.codec_sbc.allocation_method_mask = app_cfg_const.sbc_allocation_method;
            sep.u.codec_sbc.min_bitpool = app_cfg_const.sbc_min_bitpool;
            sep.u.codec_sbc.max_bitpool = app_cfg_const.sbc_max_bitpool;
            bt_a2dp_stream_endpoint_add(sep);
        }

        if (app_cfg_const.a2dp_codec_type_aac)
        {
            T_BT_A2DP_STREAM_ENDPOINT sep;

            sep.codec_type = BT_A2DP_CODEC_TYPE_AAC;
            bt_a2dp_stream_endpoint_delete(sep);

            sep.u.codec_aac.object_type_mask = app_cfg_const.aac_object_type;
            sep.u.codec_aac.sampling_frequency_mask = app_cfg_const.aac_sampling_frequency;
            sep.u.codec_aac.channel_number_mask = app_cfg_const.aac_channel_number;
            sep.u.codec_aac.vbr_supported = app_cfg_const.aac_vbr_supported;
            sep.u.codec_aac.bit_rate = app_cfg_const.aac_bit_rate;
            bt_a2dp_stream_endpoint_add(sep);
        }

#if (F_APP_A2DP_CODEC_LDAC_SUPPORT == 1)
        if (app_cfg_const.a2dp_codec_type_ldac)
        {
            T_BT_A2DP_STREAM_ENDPOINT sep;

            sep.codec_type = BT_A2DP_CODEC_TYPE_LDAC;
            bt_a2dp_stream_endpoint_delete(sep);
            sep.u.codec_ldac.sampling_frequency_mask = app_cfg_const.ldac_sampling_frequency;
            sep.u.codec_ldac.channel_mode_mask = app_cfg_const.ldac_channel_mode;
            bt_a2dp_stream_endpoint_add(sep);
        }
#endif

#if (F_APP_A2DP_CODEC_LC3_SUPPORT == 1)
        //if(app_cfg_const.a2dp_codec_type_lc3)
        {
            T_BT_A2DP_STREAM_ENDPOINT sep;

            sep.codec_type = BT_A2DP_CODEC_TYPE_LC3;
            bt_a2dp_stream_endpoint_delete(sep);

            sep.u.codec_lc3.sampling_frequency_mask = BT_A2DP_LC3_SAMPLING_FREQUENCY_48KHZ |
                                                      BT_A2DP_LC3_SAMPLING_FREQUENCY_44_1KHZ;
            sep.u.codec_lc3.channel_num_mask = BT_A2DP_LC3_CHANNEL_NUM_2 |
                                               BT_A2DP_LC3_CHANNEL_NUM_1;
            sep.u.codec_lc3.frame_duration_mask = BT_A2DP_LC3_FRAME_DURATION_10MS |
                                                  BT_A2DP_LC3_FRAME_DURATION_7_5MS;
            sep.u.codec_lc3.frame_length = 240;
            bt_a2dp_stream_endpoint_add(sep);
        }
#endif

        bt_mgr_cback_register(app_a2dp_bt_cback);
    }
}
