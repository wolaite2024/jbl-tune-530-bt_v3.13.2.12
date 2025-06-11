/**
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_VAD_SUPPORT
#include "trace.h"
#include "audio.h"
#include "btm.h"
#include "vad.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_vad.h"

#if AMA_FEATURE_SUPPORT
#include "app_ama.h"
#endif

static void app_vad_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_VOICE_ACTIVITY_DETECTED:
        {
#if APP_AMA_VAD_SUPPORT
            if (app_cfg_const.enable_vad == 1)
            {
                vad_disable();
                app_ama_va_start();
            }
#else
            app_mmi_handle_action(MMI_HF_INITIATE_VOICE_DIAL);
#endif
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
        APP_PRINT_TRACE1("app_vad_audio_cback: event 0x%04x", event_type);
    }
}

static void app_vad_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_CMPL:
        {
            if (app_cfg_const.enable_vad == 1)
            {
                vad_enable();
            }
        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            if (app_cfg_const.enable_vad == 1)
            {
                vad_disable();
            }
        }
        break;

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            if (app_cfg_const.enable_vad == 1)
            {
                T_BT_EVENT_PARAM *param = event_buf;
                T_BT_ROLESWAP_STATUS event;

                event = param->remote_roleswap_status.status;

                if (event == BT_ROLESWAP_STATUS_SUCCESS)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        vad_enable();
                    }
                    else
                    {
                        vad_disable();
                    }
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_vad_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_vad_init(void)
{
    audio_mgr_cback_register(app_vad_audio_cback);
    bt_mgr_cback_register(app_vad_bt_cback);
}
#endif
