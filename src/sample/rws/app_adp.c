
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
*/

#include "trace.h"
#include "os_sched.h"
#include "section.h"
#include "system_status_api.h"
#include "hal_adp.h"
#include "app_dlps.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_io_msg.h"
#include "app_adp.h"
#include "app_adp_cmd.h"

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
#include "pmu_api.h"
#endif
#if F_APP_ADC_SUPPORT
#include "app_adc.h"
#endif
#if F_APP_IO_OUTPUT_SUPPORT
#include "app_io_output.h"
#endif
#if F_APP_ONE_WIRE_UART_SUPPORT
#include "app_one_wire_uart.h"
#endif

/** @defgroup  APP_ADP  App Adp
  * @brief
  * @{
  */
/**/

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup APP_ADP_Exported_Variables App adp Variables
    * @{
    */
static uint8_t adaptor_plug_in = ADAPTOR_UNPLUG;
static bool disallow_enable_charger = false;

/** End of Exported_Variables
    * @}
    */
///@endcond

/*============================================================================*
 *                              Functions
 *============================================================================*/
#if F_APP_ONE_WIRE_UART_SUPPORT
void app_adp_io_wakeup_pol_set(void)
{
    uint8_t adp_1wire_pinmux = app_cfg_const.one_wire_uart_data_pinmux;
    T_ADP_STATE adp_io_state = adp_get_current_state(ADP_DETECT_IO);

    if (adp_io_state == ADP_STATE_IN)
    {
        System_WakeUpPinEnable(adp_1wire_pinmux, POL_LOW);
    }
    else if (adp_io_state == ADP_STATE_OUT)
    {
        System_WakeUpPinEnable(adp_1wire_pinmux, POL_HIGH);
    }
}
#endif

static void app_adp_wakeup_pol_set(void)
{
    if (!app_cfg_const.smart_charger_control)
    {
        T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

        if (adp_5v_state == ADP_STATE_IN)
        {
            adp_wake_up_enable(WAKE_UP_GENERAL, POL_LOW);
        }
        else if (adp_5v_state == ADP_STATE_OUT)
        {
            adp_wake_up_enable(WAKE_UP_GENERAL, POL_HIGH);
        }

#if F_APP_ONE_WIRE_UART_SUPPORT
        if (app_cfg_const.one_wire_uart_support)
        {
            app_adp_io_wakeup_pol_set();
        }
#endif
    }
}

void app_adp_set_disable_charger_by_box_battery(bool flag)
{
    if (app_cfg_nv.disable_charger_by_box_battery != flag)
    {
        app_cfg_nv.disable_charger_by_box_battery = flag;
        app_cfg_store(&app_cfg_nv.offset_smart_chargerbox, 1);
    }
}

void app_adp_set_disallow_enable_charger(bool flag)
{
    disallow_enable_charger = flag;
}

void app_adp_smart_box_charger_control(bool enable)
{
    APP_PRINT_INFO2("app_adp_smart_box_charger_control: enable %d, disallow_enable_charger %d",
                    enable, disallow_enable_charger);

    if (disallow_enable_charger && enable)
    {
        return;
    }

    if (enable)
    {
        /* enable CHARGER_AUTO_ENABLE to enable charger interrupt again */
        sys_hall_charger_auto_enable(true);

        adp_wake_up_enable(WAKE_UP_GENERAL, POL_HIGH);
    }
    else
    {
        /* disable CHARGER_AUTO_ENABLE first to prevent interrupt to enable charger again */
        sys_hall_charger_auto_enable(false);

        adp_wake_up_enable(WAKE_UP_GENERAL, POL_LOW);
    }
}

#if F_APP_LOCAL_PLAYBACK_SUPPORT
void app_adp_usb_start_handle(void)
{
    set_cpu_sleep_en(0);
    app_dlps_stop_power_down_wdg_timer();
    app_sd_card_power_down_disable(APP_SD_POWER_DOWN_ENTER_CHECK_USB_MSC);
    app_sd_card_power_on(); //??
    usb_start(NULL);
    app_dlps_disable(APP_DLPS_ENTER_CHECK_USB);
}

void app_adp_usb_stop_handle(void)
{
    usb_stop(NULL);
    audio_fs_update(NULL);
    set_cpu_sleep_en(1);
    app_dlps_enable(APP_DLPS_ENTER_CHECK_USB);
    /*for customer MIFO work around sd en pin*/
    app_sd_card_power_down_enable(APP_SD_POWER_DOWN_ENTER_CHECK_USB_MSC);
    app_dlps_start_power_down_wdg_timer();
}
#endif

uint8_t app_adp_get_plug_state(void)
{
    return adaptor_plug_in;
}

void app_adp_detect(void)
{
    T_ADP_STATE adp_state = ADP_STATE_UNKNOWN;

    while (adp_get_current_state(ADP_DETECT_5V) == ADP_STATE_DETECTING)
    {
        os_delay(10);
    }

#if F_APP_ONE_WIRE_UART_SUPPORT
    if (app_cfg_const.one_wire_uart_support)
    {
        while (adp_get_current_state(ADP_DETECT_IO) == ADP_STATE_DETECTING)
        {
            os_delay(10);
        }
        //adp_state based on adaptor io state
        adp_state = adp_get_current_state(ADP_DETECT_IO);
    }
    else
    {
        //adp_state based on adaptor 5v state
        adp_state = adp_get_current_state(ADP_DETECT_5V);
    }
#else
    adp_state = adp_get_current_state(ADP_DETECT_5V);
#endif

    app_cfg_nv.adaptor_changed = 0;

    if (adp_state == ADP_STATE_IN)
    {
        adaptor_plug_in = ADAPTOR_PLUG;

        if (!app_cfg_nv.adaptor_is_plugged)
        {
            app_cfg_nv.adaptor_is_plugged = 1;
            app_cfg_nv.adaptor_changed = 1;
        }
    }
    else if (adp_state == ADP_STATE_OUT)
    {
        adaptor_plug_in = ADAPTOR_UNPLUG;

        if (app_cfg_nv.adaptor_is_plugged)
        {
            app_cfg_nv.adaptor_is_plugged = 0;
            app_cfg_nv.adaptor_changed = 1;
        }
    }

    if (app_cfg_nv.adaptor_changed)
    {
        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
        app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);
    }

    app_adp_wakeup_pol_set();

    if (!app_cfg_const.enable_power_off_to_dlps_mode)
    {
        app_dlps_power_mode_set();
    }
}

static void app_adp_plug_handle(void)
{

#if F_APP_IO_OUTPUT_SUPPORT
    if (app_cfg_const.enable_power_supply_adp_in)
    {
        app_io_output_power_supply(true);
    }
#endif

    // app_adp_set_disable_charger_by_box_battery(false);

    if (app_cfg_const.smart_charger_control)
    {
        app_adp_smart_box_charger_control(true);
    }

#if F_APP_LOCAL_PLAYBACK_SUPPORT //& (!F_APP_ERWS_SUPPORT)
    if (app_cfg_const.local_playback_support && (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        if (app_db.device_state == APP_DEVICE_STATE_OFF)
        {
            app_adp_usb_start_handle();
        }
        else
        {
            if (app_cfg_nv.adaptor_is_plugged)
            {
                app_cfg_nv.usb_need_start = 1;
                app_cfg_store(&app_cfg_nv.pin_code[6], 4);
            }
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
        }
    }
#endif

}

static void app_adp_unplug_handle(void)
{
#if F_APP_IO_OUTPUT_SUPPORT
    if (app_cfg_const.enable_power_supply_adp_in)
    {
        if (app_db.device_state != APP_DEVICE_STATE_ON)
        {
            app_io_output_power_supply(false);
        }
    }
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT //& (!F_APP_ERWS_SUPPORT)
    if (app_cfg_const.local_playback_support && (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        app_adp_usb_stop_handle();
    }
#endif

}

void app_adp_msg_handler(uint16_t type)
{
    switch (type)
    {
    case IO_MSG_GPIO_ADAPTOR_PLUG:
        app_adp_plug_handle();
        break;

    case IO_MSG_GPIO_ADAPTOR_UNPLUG:
        app_adp_unplug_handle();
        break;
    }
}

static void app_adp_plug_in_out_handle(T_ADP_PLUG_EVENT status)
{
    T_IO_MSG loc_msg;
    T_IO_MSG adp_msg;
    bool loc_msg_handle = false;
    bool adp_msg_handle = false;

    if (!app_db.bt_is_ready && !app_db.ble_is_ready)
    {
        APP_PRINT_ERROR0("app_adp_plug_in_out_handle: BT or BLE is not ready");
        return;
    }

    app_cfg_nv.adaptor_changed = 0;

    app_db.local_in_case = (status == ADP_EVENT_PLUG_IN) ? true : false;

    // location msg
    loc_msg.type = IO_MSG_TYPE_GPIO;
    loc_msg.subtype = IO_MSG_GPIO_BUD_LOC_CHANGE;

    // adp msg
    adp_msg.type = IO_MSG_TYPE_GPIO;

    switch (status)
    {
    case ADP_EVENT_PLUG_IN:
        {
            if (app_cfg_nv.adaptor_is_plugged == 0)
            {
                loc_msg_handle = true;
                adp_msg_handle = true;
                app_cfg_nv.adaptor_is_plugged = 1;
                app_cfg_nv.adaptor_changed = 1;
                adaptor_plug_in = ADAPTOR_PLUG;

                loc_msg.u.param = ((CAUSE_ACTION_ADP << 8) | EVENT_IN_CASE);

                adp_msg.subtype = IO_MSG_GPIO_ADAPTOR_PLUG;

                app_adp_wakeup_pol_set();
            }
        }
        break;

    case ADP_EVENT_PLUG_OUT:
        {
            if (app_cfg_nv.adaptor_is_plugged == 1)
            {
                loc_msg_handle = true;
                adp_msg_handle = true;
                app_cfg_nv.adaptor_is_plugged = 0;
                app_cfg_nv.adaptor_changed = 1;
                adaptor_plug_in = ADAPTOR_UNPLUG;

                loc_msg.u.param = ((CAUSE_ACTION_ADP << 8) | EVENT_OUT_CASE);

                adp_msg.subtype = IO_MSG_GPIO_ADAPTOR_UNPLUG;

                //Should be able to power on during power off ing after debounce timeout
                //Keep adaptor_is_plugged to correct condition
                if (app_db.device_state == APP_DEVICE_STATE_OFF_ING)
                {
                    app_cfg_nv.adaptor_is_plugged = 1;
                }

                if (app_cfg_const.smart_charger_control && app_db.disallow_adp_out_auto_power_on)
                {
                    loc_msg_handle = false;
                    APP_PRINT_INFO0("ignore adp out when get box zero volume");
                }
            }
        }
        break;

    default:
        break;
    }

#if F_APP_ONE_WIRE_UART_SUPPORT
    if ((app_cfg_const.one_wire_uart_support &&
         app_one_wire_get_aging_test_state() == ONE_WIRE_AGING_TEST_STATE_TESTING))
    {
        /* don't handle in/out box when aging testing */
        loc_msg_handle = false;
    }
#endif

    if (adp_msg_handle)
    {
        app_io_msg_send(&adp_msg);
    }

    if (loc_msg_handle && (app_cfg_const.box_detect_method == ADAPTOR_DETECT))
    {
        app_io_msg_send(&loc_msg);
    }
}

/* Callback run in timer task, should not run too long */
static void app_adp_stage_change_cb(T_ADP_PLUG_EVENT status, void *user_data)
{
    uint32_t adp_type = (uint32_t)user_data;

    APP_PRINT_INFO3("app_adp_stage_change_cb: adp_type:%d, plug_event: %d, adaptor_is_plugged: %d",
                    adp_type, status, app_cfg_nv.adaptor_is_plugged);

    if ((app_cfg_const.ntc_power_by_pad_direct) && (app_cfg_const.thermistor_power_pinmux != 0xFF))
    {
        if ((adp_type == ADP_DETECT_5V) && (status == ADP_EVENT_PLUG_OUT))
        {
            Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_LOW);
        }
    }

#if F_APP_ONE_WIRE_UART_SUPPORT
    if (app_cfg_const.one_wire_uart_support)
    {
        T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);
        T_ADP_STATE adp_io_state = adp_get_current_state(ADP_DETECT_IO);

        if (adp_type == ADP_DETECT_IO)
        {
            if ((status == ADP_EVENT_PLUG_IN) && (adp_5v_state == ADP_STATE_OUT))
            {
                adp_close(ADP_DETECT_IO);
                app_one_wire_uart_open();
            }
            else
            {
                app_one_wire_uart_close();
                adp_open(ADP_DETECT_IO);
            }

            app_adp_plug_in_out_handle(status);
        }
        else if (adp_type == ADP_DETECT_5V)
        {
            if ((status == ADP_EVENT_PLUG_OUT) && (adp_io_state == ADP_STATE_IN))
            {
                adp_close(ADP_DETECT_IO);
                app_one_wire_uart_open();
            }
            else
            {
                app_one_wire_uart_close();
                adp_open(ADP_DETECT_IO);
            }
        }

        APP_PRINT_INFO2("app_adp_stage_change_cb: adp_5v_state: %d, adp_io_state: %d",
                        adp_5v_state, adp_io_state);
    }
    else
#endif
    {
#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
        if (app_cfg_const.thermistor_power_vpa_support)
        {
            if (status == ADP_EVENT_PLUG_IN)
            {
                pmu_ldo_pa_control(true);
            }
            else if (status == ADP_EVENT_PLUG_OUT)
            {
                pmu_ldo_pa_control(false);
            }
        }
#endif

        app_adp_plug_in_out_handle(status);
    }
}

#if F_APP_ONE_WIRE_UART_SUPPORT
void app_adp_io_plug_out(void)
{
    APP_PRINT_INFO0("app_adp_io_plug_out");

    app_one_wire_uart_close();
    adp_open(ADP_DETECT_IO);
}

static void app_adp_io_init(void)
{
    adp_register_state_change_cb(ADP_DETECT_IO, app_adp_stage_change_cb, (void *)ADP_DETECT_IO);
}
#endif

RAM_TEXT_SECTION static void app_adp_5v_handler(void)
{
    if ((app_cfg_const.ntc_power_by_pad_direct) && (app_cfg_const.thermistor_power_pinmux != 0xFF))
    {
        Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_HIGH);
    }
}

void app_adp_init(void)
{
    if ((app_cfg_const.ntc_power_by_pad_direct) && (app_cfg_const.thermistor_power_pinmux != 0xFF))
    {
        /* The first apd interrupt after the system reset cannot execute adp interrupt callback registered by APP, so check adp level here. */
        if (adp_get_level(ADP_DETECT_5V) == ADP_LEVEL_HIGH)
        {
            Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_HIGH);
        }
    }

    adp_wake_up_enable(WAKE_UP_GENERAL, POL_HIGH);
    adp_register_state_change_cb(ADP_DETECT_5V, app_adp_stage_change_cb, (void *)ADP_DETECT_5V);
    adp_update_isr_cb(ADP_DETECT_5V, (P_ADP_ISR_CBACK)app_adp_5v_handler);

#if F_APP_ONE_WIRE_UART_SUPPORT
    if (app_cfg_const.one_wire_uart_support)
    {
        app_adp_io_init();
    }
#endif
}

