/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include "trace.h"
#include "bt_hfp.h"
#include "app_hfp.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_a2dp.h"
#include "app_audio_policy.h"

void app_durian_mmi_voice_recognition_enable(uint8_t app_idx)
{
    uint8_t audio_opus_status = app_db.br_link[app_idx].audio_opus_status;
    APP_PRINT_TRACE2("app_durian_mmi_voice_recognition_enable: audio_opus_status %d app_idx %d",
                     audio_opus_status, app_idx);

    if (audio_opus_status == AVP_VIOCE_RECOGNITION_IDLE)
    {
        if (app_db.br_link[app_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
        {
            bt_hfp_voice_recognition_enable_req(app_db.br_link[app_idx].bd_addr);

            app_audio_tone_type_play(TONE_HF_CALL_VOICE_DIAL, false, true);
        }
        else
        {
            APP_PRINT_TRACE0("avp_voice_recognition_enbale: no connected");
        }
    }
    else
    {
        app_durian_avp_voice_start(app_idx);
    }
}

void app_durian_mmi_voice_recognition_disable(uint8_t app_idx)
{
    uint8_t audio_opus_status = app_db.br_link[app_idx].audio_opus_status;
    APP_PRINT_TRACE2("app_durian_mmi_voice_recognition_disable: audio_opus_status %d app_idx %d",
                     audio_opus_status, app_idx);

    if (audio_opus_status == AVP_VIOCE_RECOGNITION_IDLE)
    {
        if (app_db.br_link[app_idx].connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK))
        {
            bt_hfp_voice_recognition_disable_req(app_db.br_link[app_idx].bd_addr);
        }
        else
        {
            APP_PRINT_TRACE0("app_durian_mmi_voice_recognition_disable: no connected");
        }
    }
    else if ((audio_opus_status >= AVP_VIOCE_RECOGNITION_ENCODE_START) &&
             (audio_opus_status < AVP_VIOCE_RECOGNITION_STOP))
    {
        if (app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK)
        {
            APP_PRINT_TRACE0("app_durian_mmi_voice_recognition_disable: bt_avp_voice_recognition_disable_req");
            bt_avp_voice_recognition_disable_req(app_db.br_link[app_idx].bd_addr);
        }
    }
}

void app_durian_mmi_play_fwd(void)
{
    uint8_t play_status;
    uint8_t app_idx = app_a2dp_get_active_idx();

    play_status = app_db.br_link[app_idx].avrcp_play_status;

    if ((play_status == BT_AVRCP_PLAY_STATUS_STOPPED) || (play_status == BT_AVRCP_PLAY_STATUS_PAUSED))
    {
        bt_avrcp_play(app_db.br_link[app_idx].bd_addr);
    }
    else if (play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
    {
        bt_avrcp_forward(app_db.br_link[app_idx].bd_addr);
    }
}

void app_durian_mmi_play_bwd(void)
{
    uint8_t play_status;
    uint8_t app_idx = app_a2dp_get_active_idx();

    play_status = app_db.br_link[app_idx].avrcp_play_status;

    if ((play_status == BT_AVRCP_PLAY_STATUS_STOPPED) || (play_status == BT_AVRCP_PLAY_STATUS_PAUSED))
    {
        bt_avrcp_play(app_db.br_link[app_idx].bd_addr);
    }
    else if (play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
    {
        bt_avrcp_backward(app_db.br_link[app_idx].bd_addr);
    }
}

void app_durian_mmi_call_transfer_check(void)
{
    uint8_t app_idx = app_hfp_get_active_idx();

    if (((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) == 0) &&
        app_cfg_const.enable_auto_a2dp_sco_control_for_android)
    {
        if (app_audio_call_transfer_check())
        {
            app_mmi_handle_action(MMI_HF_TRANSFER_CALL);
        }
    }
}
#endif
