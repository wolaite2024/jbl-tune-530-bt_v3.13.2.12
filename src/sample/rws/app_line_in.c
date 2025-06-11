
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "rtl876x_pinmux.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "trace.h"
#include "sysm.h"
#include "bt_avrcp.h"
#include "app_timer.h"
#include "audio_track.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_io_msg.h"
#include "app_dlps.h"
#include "app_line_in.h"
#include "section.h"
#include "app_device.h"
#include "app_mmi.h"
#include "app_multilink.h"
#include "app_main.h"
#include "app_hfp.h"
#include "app_auto_power_off.h"
#include "line_in.h"
#include "audio.h"
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif
#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "app_airplane.h"
#endif
#include "app_audio_policy.h"

#if (F_APP_SLIDE_SWITCH_SUPPORT == 1)
#include "app_slide_switch.h"
#endif

#include "eq_utils.h"
#include "app_eq.h"
#include "app_a2dp.h"

#if F_APP_DUAL_AUDIO_EFFECT
#include "app_dual_audio_effect.h"
#endif

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

typedef enum
{
    APP_TIMER_LINE_IN_DEBOUNCE = 0x00,
} T_APP_LINE_IN_TIMER;

#if F_APP_LINEIN_SUPPORT

static bool is_playing = false;
static bool line_in_plug_state = false;
static bool line_in_start_wait_anc_state_flag = false;
static uint8_t app_line_in_timer_id = 0;
static uint8_t timer_idx_line_in_debounce = 0;

#if F_APP_DUAL_AUDIO_EFFECT
static void app_line_in_dual_audio_channel_set(bool is_playing);
#endif

#if F_APP_ANC_SUPPORT
static uint8_t anc_mode[ANC_SCENARIO_MAX_NUM] = {0};
static T_ANC_APT_STATE old_final_anc_state = ANC_OFF_APT_OFF;
#endif

bool app_line_in_start_line_in_check(void);

bool app_line_in_start(void)
{
    bool ret = false;

    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_LINE_IN);
    if (line_in_start())
    {
        is_playing = true;
        ret = true;
    }

    return ret;
}

bool app_line_in_stop(void)
{
    bool ret = false;

    if (line_in_stop())
    {
        is_playing = false;
        ret = true;
    }
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_LINE_IN, app_cfg_const.timer_auto_power_off);

    return ret;
}

static bool app_line_in_start_line_in_check(void)
{
    bool ret = false;

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    ret = app_multilink_customer_line_in_start_check();
#else
    uint8_t app_idx = app_a2dp_get_active_idx();

    if (app_db.br_link[app_idx].streaming_fg == true && !app_cfg_const.enable_aux_in_supress_a2dp)
    {
        ret = false;
    }

    ret = true;
#endif

    return ret;
}

static void app_line_in_start_line_in(void)
{
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    app_multilink_customer_line_in_start();
#else
    app_line_in_start();
#endif
}

static void app_line_in_stop_line_in(void)
{
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    app_multilink_customer_line_in_stop();
#else
    app_line_in_stop();
#endif
}

static void app_line_in_plug(void)
{
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    app_multilink_customer_line_in_plug();
#endif
}

static void app_line_in_unplug(void)
{
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
    app_multilink_customer_line_in_unplug();
#endif
}

#if F_APP_ANC_SUPPORT
//return true:doing anc mode switch , false:not doing anc mode switch
static bool app_line_in_plug_switch_anc_mode(void)
{
    if (app_anc_is_anc_on_state(*app_db.final_listening_state))
    {
        if (app_line_in_anc_line_in_mode_state_get())
        {
            if (app_db.current_listening_state == app_line_in_anc_line_in_mode_state_get())
            {
                //anc mode is right
            }
            else
            {
                old_final_anc_state = *app_db.final_listening_state;
                app_listening_state_machine(ANC_STATE_TO_EVENT(app_line_in_anc_line_in_mode_state_get()), false,
                                            true);
                return true;
            }
        }
    }
    return false;
}

static void app_line_in_unplug_resume_anc(void)
{
    if ((*app_db.final_listening_state) == app_line_in_anc_line_in_mode_state_get() &&
        *app_db.final_listening_state != ANC_OFF_APT_OFF)
    {
        if (old_final_anc_state == ANC_OFF_APT_OFF)
        {
            //apply the first non-line-in mode anc state
            if (app_line_in_anc_non_line_in_mode_state_get() != ANC_OFF_APT_OFF)
            {
                app_listening_state_machine(ANC_STATE_TO_EVENT(app_line_in_anc_non_line_in_mode_state_get()), false,
                                            true);
            }
            else
            {
                //no non-line in anc state found, stay on current anc state
            }
        }
        else
        {
            app_listening_state_machine(ANC_STATE_TO_EVENT(old_final_anc_state), false, true);
            old_final_anc_state = ANC_OFF_APT_OFF;
        }

    }
}
#endif

#if F_APP_APT_SUPPORT
static void app_line_in_plug_close_apt(void)
{
    if (app_apt_is_apt_on_state(*app_db.final_listening_state) ||
        app_apt_is_apt_on_state(app_db.current_listening_state))
    {
        app_listening_state_machine(EVENT_APT_OFF, false, true);
    }
}
#endif


static void app_line_in_handler(bool is_in, bool first_power_on)
{
    uint8_t app_idx = app_a2dp_get_active_idx();
    bool allow_line_in_start = true;

    APP_PRINT_INFO2("app_line_in_handler: is_in %d, is_playing %d", is_in, is_playing);

    if (is_in)
    {
        if (app_cfg_const.line_in_plug_enter_airplane_mode)
        {
#if (F_APP_AIRPLANE_SUPPORT == 1)
            if (first_power_on)
            {
                if (!app_airplane_mode_get())
                {
                    app_airplane_power_on_handle();
                }
            }
            else
            {
                if (!app_airplane_mode_get())
                {
                    app_airplane_mmi_handle();
                }
            }
#endif
        }
        else
        {
            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                //sco has higher priority than line in
                allow_line_in_start = false;
            }
            else
            {
                if (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING ||
                    app_db.br_link[app_idx].streaming_fg == true)
                {
                    if (app_cfg_const.enable_aux_in_supress_a2dp)
                    {
                        if (app_db.br_link[app_idx].connected_profile & AVRCP_PROFILE_MASK)
                        {
                            app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                        }
                        if (app_db.br_link[app_idx].a2dp_track_handle)
                        {
                            audio_track_stop(app_db.br_link[app_idx].a2dp_track_handle);
                        }
                    }
                    else
                    {
                        allow_line_in_start = false;
                    }
                }
            }
        }

        /*  if anc is on, need switch Scenario to close FF mic
        *  otherwise, make sure when anc open later, use the right scenario
        */
#if F_APP_ANC_SUPPORT
        if (app_line_in_plug_switch_anc_mode())
        {
            if (!is_playing && allow_line_in_start)
            {
                line_in_start_wait_anc_state_flag = true;
            }
        }
#endif
#if F_APP_APT_SUPPORT
        app_line_in_plug_close_apt();
#endif

        if (!is_playing &&
            allow_line_in_start &&
            !line_in_start_wait_anc_state_flag)
        {
            app_line_in_start_line_in();
        }
    }
    else
    {
        line_in_start_wait_anc_state_flag = false;

        if (is_playing)
        {
            app_line_in_stop_line_in();
        }
        else
        {
#if F_APP_ANC_SUPPORT
            app_line_in_unplug_resume_anc();
#endif
        }

        if (app_cfg_const.line_in_plug_enter_airplane_mode)
        {
#if (F_APP_AIRPLANE_SUPPORT == 1)
            if (app_airplane_mode_get())
            {
                app_airplane_mmi_handle();
            }
#endif
        }
        else
        {
            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                return;
            }
        }
    }
}

static void app_line_in_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_LINE_IN_DEBOUNCE:
        {
            uint8_t gpio_status = param;

            app_stop_timer(&timer_idx_line_in_debounce);

            if (0 == gpio_status)
            {
                line_in_plug_state = true;
                if (app_db.device_state == APP_DEVICE_STATE_ON)
                {
                    app_line_in_plug();
                    app_line_in_handler(true, false);
                }
            }
            else
            {
                line_in_plug_state = false;
                if (app_db.device_state == APP_DEVICE_STATE_ON)
                {
                    app_line_in_handler(false, false);
                    app_line_in_unplug();
                }
            }

            app_dlps_enable(APP_DLPS_ENTER_CHECK_LINEIN);
        }
        break;

    default:
        break;
    }
}

void app_line_in_driver_init(void)
{
    /*gpio init*/
    hal_gpio_init_pin(app_cfg_const.line_in_pinmux, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    hal_gpio_set_up_irq(app_cfg_const.line_in_pinmux, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);
    hal_gpio_register_isr_callback(app_cfg_const.line_in_pinmux, app_line_in_detect_intr_cb, 0);
    hal_gpio_irq_disable(app_cfg_const.line_in_pinmux);
}

static void app_line_in_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_LINE_IN_STARTED:
        {
            if (app_db.line_in_eq_instance != NULL)
            {
                eq_release(app_db.line_in_eq_instance);
                app_db.line_in_eq_instance = NULL;
            }

            app_eq_change_audio_eq_mode(true);
            app_db.line_in_eq_instance = app_eq_create(EQ_CONTENT_TYPE_AUDIO, EQ_STREAM_TYPE_AUDIO, SPK_SW_EQ,
                                                       app_db.spk_eq_mode, app_cfg_nv.eq_idx);

            if (app_db.line_in_eq_instance != NULL)
            {
                eq_enable(app_db.line_in_eq_instance);
                line_in_effect_attach(app_db.line_in_eq_instance);
            }
#if F_APP_DUAL_AUDIO_EFFECT
            app_line_in_dual_audio_channel_set(true);
#endif
        }
        break;

    case AUDIO_EVENT_LINE_IN_STOPPED:
        {
            uint8_t app_idx = app_a2dp_get_active_idx();

            /*
                When line in stopped, should restore anc and a2dp
            */
#if F_APP_ANC_SUPPORT
            if (!line_in_plug_state)
            {
                app_line_in_unplug_resume_anc();
            }
#endif

#if (F_APP_SLIDE_SWITCH_LISTENING_MODE == 1)
#if F_APP_APT_SUPPORT
            if (!line_in_plug_state)
            {
                app_slide_switch_resume_apt();
            }
#endif
#endif

            if (app_cfg_const.enable_aux_in_supress_a2dp &&
                app_db.br_link[app_idx].streaming_fg == true)
            {
                audio_track_start(app_db.br_link[app_idx].a2dp_track_handle);
            }

            if (app_db.line_in_eq_instance != NULL)
            {
                eq_release(app_db.line_in_eq_instance);
                app_db.line_in_eq_instance = NULL;
            }

#if F_APP_DUAL_AUDIO_EFFECT
            app_line_in_dual_audio_channel_set(false);
#endif
            app_eq_change_audio_eq_mode(true);
        }
        break;

#if F_APP_ANC_SUPPORT
    case AUDIO_EVENT_ANC_ENABLED:
        {
            /*
                When anc enabed, should make sure anc is line in mode unless no line in mode anc found
            */
            if (app_db.device_state == APP_DEVICE_STATE_ON && line_in_start_wait_anc_state_flag)
            {
                if (app_db.current_listening_state == app_line_in_anc_line_in_mode_state_get() ||
                    app_line_in_anc_line_in_mode_state_get() == ANC_OFF_APT_OFF)
                {
                    line_in_start_wait_anc_state_flag = false;
                    if (app_line_in_start_line_in_check())
                    {
                        app_line_in_start_line_in();
                    }
                }
            }
        }
        break;

    case AUDIO_EVENT_ANC_DISABLED:
        {
            /*
                When anc disabed, check if need open line in
            */
            if (app_db.device_state == APP_DEVICE_STATE_ON && line_in_start_wait_anc_state_flag)
            {
                line_in_start_wait_anc_state_flag = false;

                if (app_line_in_start_line_in_check())
                {
                    app_line_in_start_line_in();
                }
            }
        }
        break;
#endif

    case AUDIO_EVENT_LINE_IN_VOLUME_OUT_CHANGED:
        {
            uint8_t cur_volume = param->line_in_volume_out_changed.curr_volume;

            if (app_line_in_playing_state_get())
            {
                if (cur_volume == app_dsp_cfg_vol.line_in_volume_out_max)
                {
                    app_audio_tone_type_play(TONE_VOL_MAX, false, false);
                }
                else if (cur_volume == app_dsp_cfg_vol.line_in_volume_out_min)
                {
                    app_audio_tone_type_play(TONE_VOL_MIN, false, false);
                }
                else
                {
                    app_audio_tone_type_play(TONE_VOL_CHANGED, false, false);
                }
            }
        }
        break;

    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_state_changed.handle, &stream_type) == false)
            {
                break;
            }

            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STOPPED ||
                    param->track_state_changed.state == AUDIO_TRACK_STATE_PAUSED)
                {
                    if (line_in_plug_state)
                    {
                        if (!app_cfg_const.enable_aux_in_supress_a2dp)
                        {
                            if (!is_playing && !line_in_start_wait_anc_state_flag)
                            {
                                if (app_line_in_start_line_in_check())
                                {
                                    app_line_in_start_line_in();
                                }
                            }
                        }
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
        APP_PRINT_INFO1("app_line_in_audio_cback: event_type 0x%04x", event_type);
    }
}

static void app_line_in_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            if (app_cfg_const.line_in_support)
            {
                app_line_in_power_on_check();
            }
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            if (app_cfg_const.line_in_support)
            {
                app_line_in_power_off_check();
            }
        }
        break;

    default:
        break;
    }
}

void app_line_in_init(void)
{
    line_in_create(48000);

    line_in_volume_out_max_set(app_dsp_cfg_vol.line_in_volume_out_max);
    line_in_volume_out_min_set(app_dsp_cfg_vol.line_in_volume_out_min);
    line_in_volume_out_set(app_cfg_nv.line_in_gain_level);
    line_in_volume_in_max_set(app_dsp_cfg_vol.line_in_volume_in_max);
    line_in_volume_in_min_set(app_dsp_cfg_vol.line_in_volume_in_min);
    line_in_volume_in_set(app_dsp_cfg_vol.line_in_volume_in_default);

    sys_mgr_cback_register(app_line_in_dm_cback);
    audio_mgr_cback_register(app_line_in_audio_cback);
    app_timer_reg_cb(app_line_in_timeout_cb, &app_line_in_timer_id);

#if F_APP_ANC_SUPPORT
    app_anc_scenario_mode_get(anc_mode);
#endif
}

ISR_TEXT_SECTION void app_line_in_intpolarity_update(void)
{
    uint8_t pin_num = app_cfg_const.line_in_pinmux;
    if (hal_gpio_get_input_level(pin_num))
    {
        hal_gpio_irq_change_polarity(pin_num, GPIO_IRQ_ACTIVE_LOW);//Polarity Low
    }
    else
    {
        hal_gpio_irq_change_polarity(pin_num, GPIO_IRQ_ACTIVE_HIGH);//Polarity High
    }
}

void app_line_in_power_on_check(void)
{
    T_IO_MSG gpio_msg;
    line_in_start_wait_anc_state_flag = false;
    line_in_plug_state = false;

    app_line_in_intpolarity_update();

    if (0 == hal_gpio_get_input_level(app_cfg_const.line_in_pinmux))
    {
        gpio_msg.u.param = 0;//gpio_status
        gpio_msg.type = IO_MSG_TYPE_GPIO;
        gpio_msg.subtype = IO_MSG_GPIO_LINE_IN;

        app_io_msg_send(&gpio_msg);
    }
    hal_gpio_irq_enable(app_cfg_const.line_in_pinmux);
}

void app_line_in_power_off_check(void)
{
    hal_gpio_irq_disable(app_cfg_const.line_in_pinmux);
    line_in_start_wait_anc_state_flag = false;

    if (is_playing)
    {
        app_line_in_stop_line_in();
    }
}

ISR_TEXT_SECTION void app_line_in_detect_intr_cb(uint32_t param)
{
    uint8_t pin_num;
    uint8_t gpio_status;
    T_IO_MSG gpio_msg;

    app_dlps_disable(APP_DLPS_ENTER_CHECK_LINEIN);

    app_line_in_intpolarity_update();

    pin_num = app_cfg_const.line_in_pinmux;
    gpio_status = hal_gpio_get_input_level(pin_num);

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(pin_num);
    app_dlps_pad_wake_up_polarity_invert(pin_num);

    gpio_msg.u.param = gpio_status;
    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_LINE_IN;

    app_io_msg_send(&gpio_msg);

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(pin_num);
}

void app_line_in_detect_msg_handler(T_IO_MSG *io_driver_msg_recv)
{
    uint8_t gpio_status;

    gpio_status = io_driver_msg_recv->u.param;
    app_stop_timer(&timer_idx_line_in_debounce);
    app_start_timer(&timer_idx_line_in_debounce, "line_in_debounce",
                    app_line_in_timer_id, APP_TIMER_LINE_IN_DEBOUNCE, gpio_status, false,
                    500);
}

bool app_line_in_playing_state_get(void)
{
    return is_playing;
}

bool app_line_in_plug_state_get(void)
{
    return line_in_plug_state;
}

void app_line_in_volume_up_handle(void)
{
    uint8_t max_volume = 0;
    uint8_t curr_volume = 0;
    curr_volume = app_cfg_nv.line_in_gain_level;
    max_volume = app_dsp_cfg_vol.line_in_volume_out_max;

    if (curr_volume < max_volume)
    {
        curr_volume++;
    }
    else
    {
        if (app_cfg_const.enable_circular_vol_up == 1)
        {
            curr_volume = app_dsp_cfg_vol.line_in_volume_out_min;
        }
        else
        {
            curr_volume = max_volume;
        }
    }

    app_cfg_nv.line_in_gain_level = curr_volume;
    line_in_volume_out_set(curr_volume);
}

void app_line_in_volume_down_handle(void)
{
    uint8_t min_volume = 0;
    uint8_t curr_volume = 0;
    curr_volume = app_cfg_nv.line_in_gain_level;
    min_volume = app_dsp_cfg_vol.line_in_volume_out_min;

    if (curr_volume > min_volume)
    {
        curr_volume--;
    }
    else
    {
        curr_volume = min_volume;
    }

    app_cfg_nv.line_in_gain_level = curr_volume;
    line_in_volume_out_set(curr_volume);
}

#if F_APP_ANC_SUPPORT
T_ANC_APT_STATE app_line_in_anc_line_in_mode_state_get(void)
{
    uint8_t i = 0;

    for (i = 0; i < ANC_SCENARIO_MAX_NUM; i++)
    {
        if (anc_mode[i] == APP_ANC_MODE_TYPE_LINE_IN)
        {
            return (T_ANC_APT_STATE)(i + ANC_ON_SCENARIO_1_APT_OFF);
        }
    }

    APP_PRINT_ERROR0("app_line_in_anc_line_in_mode_state_get: no line in mode anc bin found");
    return ANC_OFF_APT_OFF;
}

T_ANC_APT_STATE app_line_in_anc_non_line_in_mode_state_get(void)
{
    uint8_t i = 0;

    for (i = 0; i < ANC_SCENARIO_MAX_NUM; i++)
    {
        if ((i < app_anc_get_selected_scenario_cnt()) && (anc_mode[i] != APP_ANC_MODE_TYPE_LINE_IN))
        {
            return (T_ANC_APT_STATE)(i + ANC_ON_SCENARIO_1_APT_OFF);
        }
    }

    APP_PRINT_ERROR0("app_line_in_anc_non_line_in_mode_state_get: no non-line in mode anc bin found");
    return ANC_OFF_APT_OFF;
}
#endif

bool app_line_in_start_a2dp_check(void)
{
    if (line_in_plug_state)
    {
        if (app_cfg_const.enable_aux_in_supress_a2dp)
        {
            return false;
        }
        else if (is_playing)
        {
            app_line_in_stop_line_in();
        }
    }
    return true;
}

void app_line_in_call_status_update(bool is_idle)
{
    if (!app_cfg_const.line_in_plug_enter_airplane_mode)
    {
        if (line_in_plug_state)
        {
            APP_PRINT_INFO1("app_line_in_call_status_update: is_idle %d", is_idle);

            if (is_idle)
            {
                if (!is_playing && !line_in_start_wait_anc_state_flag)
                {
                    if (app_line_in_start_line_in_check())
                    {
                        app_line_in_start_line_in();
                    }
                }
            }
            else
            {
                if (is_playing)
                {
                    app_line_in_stop_line_in();
                }
            }
        }
    }
}

#if F_APP_DUAL_AUDIO_EFFECT
static void app_line_in_dual_audio_channel_set(bool is_playing)
{
    if (is_playing)
    {
        app_dual_audio_lr_info(MODE_INFO_LINE_IN, false, app_cfg_const.solo_speaker_channel);
    }
    else
    {
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
            {
                app_dual_audio_lr_info(MODE_INFO_L, false, app_cfg_const.couple_speaker_channel);
            }
            else
            {
                app_dual_audio_lr_info(MODE_INFO_R, false, app_cfg_const.couple_speaker_channel);
            }
        }
        else
        {
            app_dual_audio_lr_info(MODE_INFO_L_R_DIV2, false, app_cfg_const.solo_speaker_channel);
        }
    }
}
#endif
#else
#if F_APP_DUAL_AUDIO_EFFECT
bool app_line_in_playing_state_get(void)
{
    return false;
}
#endif
#endif
