/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "trace.h"
#include "wdg.h"
#include "sysm.h"
#include "ringtone.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_mmi.h"
#include "audio_volume.h"
#include "app_bt_policy_api.h"
#include "app_in_out_box.h"
#include "app_audio_policy.h"
#include "app_dlps.h"
#include "app_adp.h"
#if F_APP_ADP_CMD_SUPPORT
#include "app_adp_cmd.h"
#endif
#include "app_key_process.h"
#include "app_led.h"
#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "app_airplane.h"
#endif

/* BBPro2 specialized feature */
#if F_APP_HEARABLE_SUPPORT
#include "app_listening_mode.h"
#endif
// end of BBPro2 specialized feature

#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#include "anc.h"
#include "anc_tuning.h"
#endif

#if F_APP_IO_OUTPUT_SUPPORT
#include "app_io_output.h"
#endif

typedef enum
{
    APP_IN_OUT_BOX_IGNORE_NONE,
    APP_IN_OUT_BOX_IGNORE_BY_OTA_TOOLING,
    APP_IN_OUT_BOX_IGNORE_BY_ADP_FACTORY_RESET,
    APP_IN_OUT_BOX_IGNORE_BY_DISABLE_OUTBOX_POWER_ON,
} T_APP_IN_OUT_BOX_IGNORE_REASON;

#if F_APP_BUD_LOCATION_SUPPORT
static void app_in_out_box_bud_loc_event_cback(uint32_t event, void *msg)
{
    uint8_t *p_info = msg;
    bool from_remote = p_info[0];

    if (!from_remote)
    {
        if (event == EVENT_IN_CASE)
        {
            app_in_out_box_handle(IN_CASE);
        }
        else if (event == EVENT_OUT_CASE)
        {
            app_in_out_box_handle(OUT_CASE);
        }
    }
}

static void app_in_out_box_handle(T_CASE_LOCATION_STATUS local)
{
    T_APP_IN_OUT_BOX_IGNORE_REASON reason = APP_IN_OUT_BOX_IGNORE_NONE;

    app_key_stop_timer();
    APP_PRINT_INFO1("app_in_out_box_handle: %d", local);

    if (local == OUT_CASE)
    {
        if (app_cfg_const.enable_rtk_charging_box)
        {
            app_dlps_stop_power_down_wdg_timer();
#if F_APP_ADP_CMD_SUPPORT
            app_adp_cmd_clear_pending();
#endif
        }
        else
        {
            if (app_cfg_const.enable_outbox_power_on)
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_IN_BOX, app_cfg_const.timer_auto_power_off);
            }
        }

#if F_APP_IO_OUTPUT_SUPPORT
        {
            app_io_output_ctrl_ext_mcu_pin(DISABLE);
        }
#endif

        if (app_db.device_state == APP_DEVICE_STATE_ON)
        {
            audio_volume_in_unmute(AUDIO_STREAM_TYPE_RECORD);
        }

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
        if (app_cfg_nv.ota_tooling_start)
        {
            reason = APP_IN_OUT_BOX_IGNORE_BY_OTA_TOOLING;
            goto exit;
        }
#endif

        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);

        if (app_cfg_nv.adp_factory_reset_power_on)
        {
            reason = APP_IN_OUT_BOX_IGNORE_BY_ADP_FACTORY_RESET;
            goto exit;
        }

        if (!app_cfg_const.enable_outbox_power_on)
        {
            reason = APP_IN_OUT_BOX_IGNORE_BY_DISABLE_OUTBOX_POWER_ON;
            app_dlps_start_power_down_wdg_timer();
            goto exit;
        }

        if (app_cfg_nv.factory_reset_done)
        {
            if (app_cfg_const.enable_rtk_charging_box)
            {
#if F_APP_ADP_CMD_SUPPORT
                app_adp_cmd_power_on_when_out_case();
#endif
            }
            else
            {
                app_mmi_handle_action(MMI_DEV_POWER_ON);
            }
        }
        else
        {
            APP_PRINT_TRACE0("app_in_out_box_handle: do nothing");
        }
    }
    else
    {
        if (app_cfg_const.enable_rtk_charging_box)
        {
#if F_APP_ADP_CMD_SUPPORT
            app_adp_cmd_pending_exec();
#endif
        }
        else
        {
            if (app_cfg_const.enable_inbox_power_off)
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_IN_BOX);
            }
        }

#if F_APP_IO_OUTPUT_SUPPORT
        if (app_db.device_state == APP_DEVICE_STATE_ON)
        {
            app_io_output_ctrl_ext_mcu_pin(ENABLE);
        }
#endif

        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);
        app_adp_set_disable_charger_by_box_battery(false);

#if F_APP_ANC_SUPPORT
        if (anc_tool_check_resp_meas_mode() != ANC_RESP_MEAS_MODE_NONE)
        {
            anc_tool_set_resp_meas_mode(ANC_RESP_MEAS_MODE_NONE);
        }
#endif

#if (F_APP_AIRPLANE_SUPPORT == 1)
        if (app_cfg_const.enable_rtk_charging_box)
        {
            app_airplane_in_box_handle();
        }
#endif

        //Enable immediately blink charging led and stop vp when adp in during power off
        app_led_disable_non_repeat_mode();
        app_audio_tone_flush(false);
        app_audio_tone_type_stop();

        /* BBPro2 specialized feature */
#if F_APP_HEARABLE_SUPPORT
        if (app_cfg_const.enable_ha)
        {
#if F_APP_APT_SUPPORT
            app_listening_state_machine(EVENT_APT_OFF, false, false);
#endif
        }
#endif
        // end of BBPro2 specialized feature

        if (app_db.device_state == APP_DEVICE_STATE_ON)
        {
            audio_volume_in_mute(AUDIO_STREAM_TYPE_RECORD);
        }
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);

exit:
    if (reason != 0)
    {
        APP_PRINT_TRACE1("app_in_out_box_handle: ignore by reason %d", reason);
    }
}

void app_in_out_box_init(void)
{
    app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_in_out_box_bud_loc_event_cback);
}
#endif
