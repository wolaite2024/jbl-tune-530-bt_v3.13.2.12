/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_EXT_CHARGER_FEATURE_SUPPORT
#include "app_ext_charger.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"
#include "trace.h"
#include "section.h"
#include "pm.h"
#include "app_main.h"
#include "app_charger.h"
#include "app_adp.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "hal_adp.h"
#include "app_io_msg.h"
#include "app_dlps.h"
#include "charger_utils.h"
#include "app_cfg.h"
#include "system_status_api.h"
#include "charger_api.h"
#include "app_harman_adc.h"
#include "app_timer.h"

#if HARMAN_USB_CONNECTOR_PROTECT
#include "app_harman_usb_connector_protect.h"
#endif

#if HARMAN_BATTERY_ID_VERIFICATION
#include "Battery_Verification.h"
#endif

static uint8_t app_ext_charger_timer_id = 0;

#if HARMAN_DELAY_CHARGING_FINISH_SUPPORT
#define DELAY_CHARGING_FINISH_TIME      3000 // 3S
static uint8_t timer_idx_delay_charging_finish = 0;
#endif

#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
#define DELAY_ADP_OUT_HANDLE_TIME       (60 * 1000) // 1min
static uint8_t timer_idx_delay_adp_out = 0;
#endif

#if HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
#define HARMAN_HIGH_VOLTAGE_CHECK_TOTLE_TIMES       30
static uint8_t high_voltage_check_times = 0;
#endif

typedef enum
{
    APP_EXT_CHARGER_TIMER_DELAY_CHARGING_FINISH   = 0x00,
    APP_EXT_CHARGER_TIMER_DELAY_ADP_OUT_HANDLE    = 0x01,
} APP_EXT_CHARGER_TIMER_MSG_TYPE;

static APP_EXT_CHARGER_CONFIG ext_charger_cfg = {0};
static APP_EXT_CHARGER_MGR ext_charger_mgr =
{
    false, true, false, 0, 0xFF, 0xFF, 0xFF, 0xFF,
    APP_CHARGER_STATE_NO_CHARGE,
    CURRENT_SPEED_0_C,
    APP_EXT_CHARGER_NTC_NORMAL
};

T_APP_CHARGER_STATE(*(ext_charger_charging_state_handler[]))(APP_EXT_CHARGER_EVENT_TYPE,
                                                             uint8_t *) =
{
    app_ext_charger_charging_handler,
    app_ext_charger_charging_finish_handler,
    app_ext_charger_charging_error_handler,
    app_ext_charger_no_charge_handler,
};

static void app_ext_charger_cfg_load(void)
{
    charger_utils_get_auto_enable(&ext_charger_cfg.inter_charger_auto_enable);

    charger_utils_get_high_temp_warn_voltage(&ext_charger_cfg.bat_high_temp_warn_voltage);
    charger_utils_get_high_temp_error_voltage(&ext_charger_cfg.bat_high_temp_error_voltage);
    charger_utils_get_low_temp_warn_voltage(&ext_charger_cfg.bat_low_temp_warn_voltage);
    charger_utils_get_low_temp_error_voltage(&ext_charger_cfg.bat_low_temp_error_voltage);
    charger_utils_get_bat_temp_hysteresis_voltage(&ext_charger_cfg.bat_temp_hysteresis_voltage);

    // Add hysteresis voltage protect, to avoid charging status changed frequently
    ext_charger_cfg.bat_high_temp_warn_recovery_vol = ext_charger_cfg.bat_high_temp_warn_voltage +
                                                      ext_charger_cfg.bat_temp_hysteresis_voltage;
    ext_charger_cfg.bat_low_temp_warn_recovery_vol = ext_charger_cfg.bat_low_temp_warn_voltage -
                                                     ext_charger_cfg.bat_temp_hysteresis_voltage;
    ext_charger_cfg.bat_high_temp_error_recovery_vol = ext_charger_cfg.bat_high_temp_error_voltage +
                                                       ext_charger_cfg.bat_temp_hysteresis_voltage;
    ext_charger_cfg.bat_low_temp_error_recovery_vol = ext_charger_cfg.bat_low_temp_error_voltage -
                                                      ext_charger_cfg.bat_temp_hysteresis_voltage;

    charger_utils_thermistor_enable(&ext_charger_cfg.thermistor_1_enable);
    ext_charger_cfg.thermistor_1_adc_channel = 0xFF;
    if (ext_charger_cfg.thermistor_1_enable)
    {
        charger_utils_get_thermistor_pin(&ext_charger_cfg.thermistor_1_adc_channel);
    }

    ext_charger_cfg.recharge_threshold = 3800;
    charger_utils_get_recharge_voltage(&ext_charger_cfg.recharge_threshold);

    APP_PRINT_TRACE6("app_ext_charger_cfg_load: high_warn_voltage: %d, high_error_voltage: %d, "
                     "low_warn_voltage: %d, low_error_voltage: %d, "
                     "hysteresis_voltage: %d, recharge_threshold: %d",
                     ext_charger_cfg.bat_high_temp_warn_voltage,
                     ext_charger_cfg.bat_high_temp_error_voltage,
                     ext_charger_cfg.bat_low_temp_warn_voltage,
                     ext_charger_cfg.bat_low_temp_error_voltage,
                     ext_charger_cfg.bat_temp_hysteresis_voltage,
                     ext_charger_cfg.recharge_threshold);
}

static void app_ext_charger_fully_charged_level_get(void)
{
    ext_charger_mgr.fully_charged_level = hal_gpio_get_input_level(ext_charger_mgr.status_det_pin);
    APP_PRINT_TRACE1("app_ext_charger_fully_charged_level_get: %d",
                     ext_charger_mgr.fully_charged_level);
}

static void app_ext_charger_enable_ctrl_gpio_set(void)
{
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
    if (ext_charger_mgr.enable)
    {
        Pad_Config(ext_charger_mgr.enable_ctrl_pin,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else
    {
        Pad_Config(ext_charger_mgr.enable_ctrl_pin,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    if (ext_charger_mgr.enable)
    {
        Pad_Config(ext_charger_mgr.enable_ctrl_pin,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
    else
    {
        app_ext_charger_fully_charged_level_get();
        if (ext_charger_mgr.fully_charged_level && (adp_5v_state == ADP_STATE_IN))
        {
            Pad_Config(ext_charger_mgr.enable_ctrl_pin,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        }
        else
        {
            Pad_Config(ext_charger_mgr.enable_ctrl_pin,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        }
    }
#else
#endif
    Pad_PullConfigValue(ext_charger_mgr.enable_ctrl_pin, PAD_STRONG_PULL);
}

static void app_ext_charger_current_speed_set(APP_EXT_CHARGER_CURRENT_SPEED speed)
{
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#if HARMAN_RUN3_SUPPORT
    if (speed == CURRENT_SPEED_0_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
    else if (speed == CURRENT_SPEED_2_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else if (speed == CURRENT_SPEED_0_5_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else if (speed == CURRENT_SPEED_1_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
#else
    if (speed == CURRENT_SPEED_0_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
    else if (speed == CURRENT_SPEED_0_5_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else if (speed == CURRENT_SPEED_1_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else if (speed == CURRENT_SPEED_2_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
#endif
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT

    app_ext_charger_enable_ctrl_gpio_set();

    if ((speed == CURRENT_SPEED_0_C) || (speed == CURRENT_SPEED_0_5_C))
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else if (speed == CURRENT_SPEED_1_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
    else if (speed == CURRENT_SPEED_2_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }
    else if (speed == CURRENT_SPEED_3_C)
    {
        Pad_Config(ext_charger_mgr.current_ctrl_pin1,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        Pad_Config(ext_charger_mgr.current_ctrl_pin2,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }
#else
#endif
    Pad_PullConfigValue(ext_charger_mgr.current_ctrl_pin1, PAD_STRONG_PULL);
    Pad_PullConfigValue(ext_charger_mgr.current_ctrl_pin2, PAD_STRONG_PULL);

    APP_PRINT_INFO2("app_ext_charger_current_speed_set: current_speed: %d, speed: %d",
                    ext_charger_mgr.current_speed, speed);

    if (ext_charger_mgr.current_speed != speed)
    {
        ext_charger_mgr.ignore_first_adc_sample = true;
        ext_charger_mgr.current_speed = speed;
    }
}

/* charger status handle */
ISR_TEXT_SECTION
static void app_ext_charger_status_det_gpio_intr_handler(uint32_t param)
{
    uint8_t pin_index = (uint8_t) param;
    uint8_t gpio_level = 0;
    T_IO_MSG gpio_msg;

    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
    hal_gpio_irq_disable(pin_index);

    gpio_level = hal_gpio_get_input_level(pin_index);
    ext_charger_mgr.fully_charged_level = gpio_level;

    APP_PRINT_TRACE1("app_ext_charger_status_det_gpio_intr_handler %d", gpio_level);

    gpio_msg.u.param = gpio_level;
    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_EXT_CHARGER_STATE;

    app_io_msg_send(&gpio_msg);

    if (gpio_level)
    {
        hal_gpio_irq_change_polarity(pin_index, GPIO_IRQ_ACTIVE_LOW);
    }
    else
    {
        hal_gpio_irq_change_polarity(pin_index, GPIO_IRQ_ACTIVE_HIGH);
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
    hal_gpio_irq_enable(pin_index);
}

static void app_ext_charger_enable_ctrl_gpio_init(uint8_t pin_index)
{
    ext_charger_mgr.enable_ctrl_pin = pin_index;
    ext_charger_mgr.enable = false;

#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
    Pad_Config(ext_charger_mgr.enable_ctrl_pin,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
    Pad_Config(ext_charger_mgr.enable_ctrl_pin,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
#else
#endif
    Pad_PullConfigValue(ext_charger_mgr.enable_ctrl_pin, PAD_STRONG_PULL);
}

static void app_ext_charger_status_det_gpio_init(uint8_t pin_index)
{
    APP_PRINT_TRACE1("app_ext_charger_status_det_gpio_init, pin_index: 0x%x", pin_index);

    ext_charger_mgr.status_det_pin = pin_index;

    hal_gpio_init_pin(pin_index, GPIO_TYPE_AUTO, GPIO_DIR_INPUT, GPIO_PULL_DOWN);
    hal_gpio_set_up_irq(pin_index, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_HIGH, true);
    hal_gpio_register_isr_callback(pin_index, app_ext_charger_status_det_gpio_intr_handler,
                                   (uint32_t)pin_index);
    // wakeup polarity
    Pad_WakeupPolarityValue(pin_index, PAD_WAKEUP_POL_LOW);
    // enable INT
    hal_gpio_irq_enable(pin_index);
}

static void app_ext_charger_current_ctrl_gpio_init(uint8_t crtl_pin1, uint8_t ctrl_pin2)
{
    ext_charger_mgr.current_ctrl_pin1 = crtl_pin1;
    ext_charger_mgr.current_ctrl_pin2 = ctrl_pin2;
}

static void app_ext_charger_enable(void)
{
    if (!ext_charger_mgr.enable)
    {
        APP_PRINT_TRACE0("app_ext_charger_enable");
        ext_charger_mgr.enable = true;

        app_ext_charger_enable_ctrl_gpio_set();
    }
}

static void app_ext_charger_disable(void)
{
    if (ext_charger_mgr.enable)
    {
        APP_PRINT_TRACE0("app_ext_charger_disable");
        ext_charger_mgr.enable = false;

        app_ext_charger_enable_ctrl_gpio_set();
    }
}

static bool app_ext_charger_check_recharge(void)
{
    bool ret = false;

    APP_PRINT_TRACE3("app_ext_charger_check_recharge: VBAT: %d, recharge_threshold: %d, fully_charged_level: %d",
                     app_harman_adc_voltage_battery_get(), ext_charger_cfg.recharge_threshold,
                     ext_charger_mgr.fully_charged_level);
    if ((!ext_charger_mgr.enable)
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
        && (app_harman_adc_voltage_battery_get() <= ext_charger_cfg.recharge_threshold)
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
        && (!ext_charger_mgr.fully_charged_level)
#else
#endif
       )
    {
        charger_api_disable_discharger();

#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#if HARMAN_RUN3_SUPPORT
        app_ext_charger_current_speed_set(CURRENT_SPEED_2_C);
#elif HARMAN_T135_SUPPORT
        app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
#else
#endif
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
        app_ext_charger_current_speed_set(CURRENT_SPEED_3_C);
#else
#endif
        app_ext_charger_enable();
        ret = true;
    }
    return ret;
}

#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
static void app_ext_charger_delay_adp_out_timer_start(uint32_t time)
{
    if (timer_handle_delay_adp_out != NULL)
    {
        app_stop_timer(&timer_idx_delay_adp_out);
    }
    gap_start_timer(&timer_idx_delay_adp_out, "delay_adp_out",
                    app_ext_charger_timer_queue_id, APP_EXT_CHARGER_TIMER_DELAY_ADP_OUT_HANDLE, 0, fasle,
                    time);
    power_register_excluded_handle(&timer_idx_delay_adp_out, PM_EXCLUDED_TIMER);
}
#endif

static T_APP_CHARGER_STATE app_ext_charger_handle_adp_in(void)
{
    T_APP_CHARGER_STATE new_state = APP_CHARGER_STATE_NO_CHARGE;
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    if (adp_5v_state == ADP_STATE_IN)
    {
        app_dlps_disable(APP_DLPS_ENTER_CHECK_ADP_VOLTAGE);

#if HARMAN_NTC_DETECT_PROTECT
        if (app_harman_adc_adp_in_handle())
        {
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
            app_ext_charger_enable();
            new_state = APP_CHARGER_STATE_CHARGING;
        }
#endif

#if HARMAN_USB_CONNECTOR_PROTECT
        app_harman_usb_connector_adp_in_handle();
#endif

#if HARMAN_BRIGHT_LED_WHEN_ADP_IN
        new_state = APP_CHARGER_STATE_CHARGING;
#endif

#if HARMAN_BATTERY_ID_VERIFICATION
        {
            unsigned int random_seed = 0;
            unsigned char ret = Battery_ID_Verification(random_seed);
            APP_PRINT_INFO1("app_ext_charger_handle_adp_in: Battery_ID_Verification ret = %d", ret);
        }
#endif

    }

    return new_state;
}

static void app_ext_charger_handle_adp_out(void)
{
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    if (adp_5v_state == ADP_STATE_OUT)
    {
        app_dlps_enable(APP_DLPS_ENTER_CHECK_ADP_VOLTAGE);
        app_ext_charger_disable();

#if HARMAN_NTC_DETECT_PROTECT
        app_harman_adc_adp_out_handle();
#endif

#if HARMAN_USB_CONNECTOR_PROTECT
        app_harman_usb_connector_adp_out_handle();
#endif

#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
        app_ext_charger_delay_adp_out_timer_start(DELAY_ADP_OUT_HANDLE_TIME);
#endif
    }
}

#if HARMAN_NTC_DETECT_PROTECT
static T_APP_CHARGER_STATE app_ext_charger_check_ntc_threshold(void)
{
    uint8_t error_code = 0;
    uint16_t ntc_value = 0;
    T_APP_CHARGER_STATE new_state = ext_charger_mgr.state;
    APP_EXT_CHARGER_CURRENT_SPEED current_speed = ext_charger_mgr.current_speed;
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

#if HARMAN_USB_CONNECTOR_PROTECT
    if (!app_harman_usb_connector_detect_is_normal())
    {
        error_code = 5;
        goto EXT_CHARGING_ERROR;
    }
#endif
    APP_PRINT_TRACE4("app_ext_charger_check_ntc_threshold: new_state: %d, vbat_is_normal: %d, "
                     " current_speed: %d, adp_5v_state: %d",
                     ext_charger_mgr.state,
                     ext_charger_mgr.vbat_is_normal,
                     ext_charger_mgr.current_speed,
                     adp_5v_state);

    app_harman_adc_update_cur_ntc_value(&ntc_value);

    if (adp_5v_state == ADP_STATE_OUT)
    {
        return APP_CHARGER_STATE_NO_CHARGE;
    }

    if (new_state != APP_CHARGER_STATE_ERROR)
    {
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
        if (current_speed == CURRENT_SPEED_1_C)
        {
            ntc_value = (ntc_value - 15);
        }
        else if (current_speed == CURRENT_SPEED_0_5_C)
        {
            ntc_value = (ntc_value - 8);
        }
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
        if (current_speed == CURRENT_SPEED_3_C)
        {
            ntc_value = (ntc_value - 20);
        }
#else
#endif
        APP_PRINT_TRACE1("app_ext_charger_check_ntc_threshold: refine ntc_value %d", ntc_value);
    }
    /*
       error_code:
           1: charging    -> low/high error  => disable charger
           1: no charging -> low/high warn  => disable charger
           2: charging   -> low/high warn   => modify charging current_speed
             no charging  -> low/high warn  => warn charge
           3: low error  -> warn recharge
           4: high error -> warn recharge
    */
    if (new_state != APP_CHARGER_STATE_ERROR)
    {
        /* normal to error */
        /* warn to error */
        if ((ntc_value >= ext_charger_cfg.bat_low_temp_error_voltage) ||
            (ntc_value <= ext_charger_cfg.bat_high_temp_error_voltage))
        {
            error_code = 1;
            goto EXT_CHARGING_ERROR;
        }

        /* normal to warn */
        /* warn to warn */
        if ((ntc_value >= ext_charger_cfg.bat_low_temp_warn_voltage) ||
            (ntc_value <= ext_charger_cfg.bat_high_temp_warn_voltage))
        {
            error_code = 2;
            goto EXT_CHARGING_WARN;
        }

        /* warn to normal if last state is warn */
        if ((ntc_value >= ext_charger_cfg.bat_high_temp_warn_recovery_vol) ||
            (ntc_value <= ext_charger_cfg.bat_low_temp_warn_recovery_vol))
        {
            error_code = 7;
        }
    }
    else
    {
        /* error to error */
        if ((ntc_value > ext_charger_cfg.bat_low_temp_error_recovery_vol) ||
            (ntc_value < ext_charger_cfg.bat_high_temp_error_recovery_vol))
        {
            error_code = 6;
            goto EXT_CHARGING_ERROR;
        }

        /* error to warn */
        if ((ntc_value <= ext_charger_cfg.bat_low_temp_error_recovery_vol) &&
            (ntc_value > ext_charger_cfg.bat_low_temp_warn_voltage))
        {
            error_code = 3;
            goto EXT_CHARGING_WARN;
        }

        /* error to warn */
        if ((ntc_value < ext_charger_cfg.bat_high_temp_warn_voltage) &&
            (ntc_value >= ext_charger_cfg.bat_high_temp_error_recovery_vol))
        {
            error_code = 4;
            goto EXT_CHARGING_WARN;
        }

        /* error to normal */
        if ((ntc_value >= ext_charger_cfg.bat_high_temp_warn_recovery_vol) ||
            (ntc_value <= ext_charger_cfg.bat_low_temp_warn_recovery_vol))
        {
            error_code = 8;
        }
    }

    if (adp_5v_state == ADP_STATE_OUT)
    {
        return APP_CHARGER_STATE_NO_CHARGE;
    }

    if (ext_charger_mgr.vbat_is_normal == VBAT_DETECT_STATUS_SUCCESS)
    {
        if (ext_charger_mgr.enable)
        {
            if (ext_charger_mgr.state == APP_CHARGER_STATE_CHARGING)
            {
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
                if ((ext_charger_mgr.current_speed == CURRENT_SPEED_0_5_C) && (error_code == 7))
                {
#if HARMAN_SECOND_NTC_DETECT_PROTECT
                    app_ext_charger_current_speed_set(CURRENT_SPEED_2_C);
#else
                    app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
#endif
                }
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
                if ((((ext_charger_mgr.current_speed == CURRENT_SPEED_0_5_C) && (error_code == 7)) ||
                     (ext_charger_mgr.current_speed == CURRENT_SPEED_1_C)) &&
                    (app_harman_adc_voltage_battery_get() >= CHARGING_VBAT_LOW_VOLTAGE_CHECK) &&
                    (app_harman_adc_voltage_battery_get() < CHARGING_VBAT_HIGH_VOLTAGE_CHECK))
                {
                    if (high_voltage_check_times == 0)
                    {
                        app_ext_charger_current_speed_set(CURRENT_SPEED_3_C);
                    }
                }
                else if ((ext_charger_mgr.current_speed == CURRENT_SPEED_3_C) &&
                         (app_harman_adc_voltage_battery_get() >= CHARGING_VBAT_HIGH_VOLTAGE_CHECK))
                {
                    if (high_voltage_check_times < HARMAN_HIGH_VOLTAGE_CHECK_TOTLE_TIMES)
                    {
                        high_voltage_check_times ++;
                    }
                    else
                    {
                        app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
                    }
                }
#else
#endif
            }
        }
        else
        {
            if ((ext_charger_mgr.state == APP_CHARGER_STATE_NO_CHARGE) ||
#if HARMAN_BRIGHT_LED_WHEN_ADP_IN
                (ext_charger_mgr.state == APP_CHARGER_STATE_CHARGING) ||
#endif
                (error_code == 8))
            {
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
                app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
                if (app_harman_adc_voltage_battery_get() < CHARGING_VBAT_LOW_VOLTAGE_CHECK)
                {
                    app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
                }
                else if ((app_harman_adc_voltage_battery_get() >= CHARGING_VBAT_LOW_VOLTAGE_CHECK) &&
                         (app_harman_adc_voltage_battery_get() < CHARGING_VBAT_HIGH_VOLTAGE_CHECK))
                {
                    app_ext_charger_current_speed_set(CURRENT_SPEED_3_C);
                }
                else if (app_harman_adc_voltage_battery_get() >= CHARGING_VBAT_HIGH_VOLTAGE_CHECK)
                {
                    app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
                }
#else
#endif
                app_ext_charger_enable();
                new_state = APP_CHARGER_STATE_CHARGING;
            }
        }
    }
    else if (ext_charger_mgr.vbat_is_normal == VBAT_DETECT_STATUS_FAIL)
    {
        if ((!ext_charger_mgr.enable) &&
            ((ext_charger_mgr.state == APP_CHARGER_STATE_NO_CHARGE) ||
#if HARMAN_BRIGHT_LED_WHEN_ADP_IN
             (ext_charger_mgr.state == APP_CHARGER_STATE_CHARGING) ||
#endif
             (error_code == 8)))
        {
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
            app_ext_charger_enable();
            new_state = APP_CHARGER_STATE_CHARGING;
        }
    }
    return new_state;

EXT_CHARGING_WARN:
    APP_PRINT_ERROR2("app_ext_charger_check_ntc_threshold: error_code: %d, adp_5v_state: %d",
                     error_code, adp_5v_state);
    if (adp_5v_state == ADP_STATE_OUT)
    {
        return APP_CHARGER_STATE_NO_CHARGE;
    }

    if (ext_charger_mgr.vbat_is_normal == VBAT_DETECT_STATUS_SUCCESS)
    {
        if (ext_charger_mgr.enable)
        {
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#if HARMAN_SECOND_NTC_DETECT_PROTECT
            app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
#else
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
#endif
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
#else
#endif
        }
        else
        {
#if HARMAN_EXTERNAL_CHARGER_DZ581_SUPPORT
#if HARMAN_SECOND_NTC_DETECT_PROTECT
            app_ext_charger_current_speed_set(CURRENT_SPEED_1_C);
#else
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
#endif
#elif HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
#else
#endif
            app_ext_charger_enable();
            new_state = APP_CHARGER_STATE_CHARGING;
        }
    }
    else if (ext_charger_mgr.vbat_is_normal == VBAT_DETECT_STATUS_FAIL)
    {
        if (!ext_charger_mgr.enable)
        {
            app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
            app_ext_charger_enable();
            new_state = APP_CHARGER_STATE_CHARGING;
        }
    }
    return new_state;

EXT_CHARGING_ERROR:
    APP_PRINT_ERROR1("app_ext_charger_check_ntc_threshold: error_code: %d", error_code);
    if (ext_charger_mgr.enable)
    {
        app_ext_charger_disable();
        new_state = APP_CHARGER_STATE_ERROR;
    }
    return new_state;
}
#endif

void app_ext_charger_vbat_is_normal_set(uint8_t enable)
{
    ext_charger_mgr.vbat_is_normal = enable;
}

uint8_t app_ext_charger_vbat_is_normal_get(void)
{
    return ext_charger_mgr.vbat_is_normal;
}

T_APP_CHARGER_STATE app_ext_charger_no_charge_handler(APP_EXT_CHARGER_EVENT_TYPE event,
                                                      uint8_t *param)
{
    T_APP_CHARGER_STATE new_state = APP_CHARGER_STATE_NO_CHARGE;
    APP_PRINT_INFO1("app_ext_charger_no_charge_handler: %d", ext_charger_mgr.ignore_first_adc_sample);

    switch (event)
    {
    case APP_EXT_CHARGER_EVENT_ADP_IN:
        {
#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
            if (timer_idx_delay_adp_out != 0)
            {
                app_stop_timer(&timer_idx_delay_adp_out);
            }
#endif
            new_state = app_ext_charger_handle_adp_in();
        }
        break;

    case APP_EXT_CHARGER_EVENT_ADP_OUT:
        {
            app_ext_charger_handle_adp_out();
            new_state = APP_CHARGER_STATE_NO_CHARGE;
        }
        break;

    case APP_EXT_CHARGER_EVENT_PERIOD_CHECK:
        {
            app_harman_adc_io_read();
        }
        break;

    case APP_EXT_CHARGER_EVENT_ADC_UPDATE:
        {
            if (ext_charger_mgr.ignore_first_adc_sample)
            {
                ext_charger_mgr.ignore_first_adc_sample = false;
            }
            else
            {
                if (ext_charger_cfg.thermistor_1_enable)
                {
                    new_state = app_ext_charger_check_ntc_threshold();
                }
            }
        }
        break;

    default:
        break;
    }

    return new_state;
}

T_APP_CHARGER_STATE app_ext_charger_charging_handler(APP_EXT_CHARGER_EVENT_TYPE event,
                                                     uint8_t *param)
{
    T_APP_CHARGER_STATE new_state = APP_CHARGER_STATE_CHARGING;
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    switch (event)
    {
    case APP_EXT_CHARGER_EVENT_ADP_IN:
        {
#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
            if (timer_idx_delay_adp_out != 0)
            {
                app_stop_timer(&timer_idx_delay_adp_out);
            }
#endif
        }
        break;

    case APP_EXT_CHARGER_EVENT_ADP_OUT:
        {
#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
            app_ext_charger_delay_adp_out_timer_start(DELAY_ADP_OUT_HANDLE_TIME);
#endif
            app_ext_charger_handle_adp_out();
            new_state = APP_CHARGER_STATE_NO_CHARGE;
        }
        break;

    case APP_EXT_CHARGER_EVENT_PERIOD_CHECK:
        {
            app_harman_adc_io_read();

            APP_PRINT_TRACE1("app_ext_charger_charging_handler: adp_5v_state: %d", adp_5v_state);
            if ((adp_5v_state == ADP_STATE_IN) && (ext_charger_mgr.enable))
            {
                app_ext_charger_fully_charged_level_get();
                if (ext_charger_mgr.fully_charged_level)
                {
#if HARMAN_DELAY_CHARGING_FINISH_SUPPORT
                    if (timer_idx_delay_charging_finish == NULL)
                    {
                        app_start_timer(&timer_idx_delay_charging_finish, "delay_charging_finish",
                                        app_ext_charger_timer_id, APP_EXT_CHARGER_TIMER_DELAY_CHARGING_FINISH, 0, false,
                                        DELAY_CHARGING_FINISH_TIME);
                    }
#else
                    app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
                    app_ext_charger_disable();
                    charger_api_enable_discharger();
                    app_harman_adc_ntc_check_timer_start(2 * CHARGING_NTC_CHECK_PERIOD);
                    new_state = APP_CHARGER_STATE_CHARGE_FINISH;
#endif
                }
            }
        }
        break;

    case APP_EXT_CHARGER_EVENT_CHARGING_FULL:
        {
            uint8_t gpio_level = *param;
            APP_PRINT_TRACE2("app_ext_charger_charging_handler: gpio level: %d, fully_charged_level: %d",
                             gpio_level, ext_charger_mgr.fully_charged_level);
#if HARMAN_BRIGHT_LED_WHEN_ADP_IN
            if (ext_charger_mgr.vbat_is_normal == VBAT_DETECT_STATUS_ING)
            {
                break;
            }
#endif
            if (gpio_level == 1)
            {
#if HARMAN_DELAY_CHARGING_FINISH_SUPPORT
                if (timer_idx_delay_charging_finish == NULL)
                {
                    app_start_timer(&timer_idx_delay_charging_finish, "delay_charging_finish",
                                    app_ext_charger_timer_id, APP_EXT_CHARGER_TIMER_DELAY_CHARGING_FINISH, 0, false,
                                    DELAY_CHARGING_FINISH_TIME);
                }
#else
                app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
                app_ext_charger_disable();
                charger_api_enable_discharger();
                app_harman_adc_ntc_check_timer_start(2 * CHARGING_NTC_CHECK_PERIOD);
                new_state = APP_CHARGER_STATE_CHARGE_FINISH;
#endif
            }
#if HARMAN_DELAY_CHARGING_FINISH_SUPPORT
            else if (gpio_level == 0xFF)
            {
                app_ext_charger_current_speed_set(CURRENT_SPEED_0_5_C);
                app_ext_charger_disable();
                charger_api_enable_discharger();
                app_harman_adc_ntc_check_timer_start(2 * CHARGING_NTC_CHECK_PERIOD);
                new_state = APP_CHARGER_STATE_CHARGE_FINISH;
            }
#endif
        }
        break;

    case APP_EXT_CHARGER_EVENT_ADC_UPDATE:
        {
            if (ext_charger_mgr.ignore_first_adc_sample)
            {
                ext_charger_mgr.ignore_first_adc_sample = false;
            }
            else
            {
                if (ext_charger_cfg.thermistor_1_enable)
                {
                    new_state = app_ext_charger_check_ntc_threshold();
                }
            }
        }
        break;

    default:
        break;
    }

    return new_state;
}

T_APP_CHARGER_STATE app_ext_charger_charging_finish_handler(APP_EXT_CHARGER_EVENT_TYPE
                                                            event, uint8_t *param)
{
    T_APP_CHARGER_STATE new_state = APP_CHARGER_STATE_CHARGE_FINISH;

    switch (event)
    {
    case APP_EXT_CHARGER_EVENT_ADP_OUT:
        {
            app_ext_charger_handle_adp_out();
            new_state = APP_CHARGER_STATE_NO_CHARGE;
        }
        break;

    case APP_EXT_CHARGER_EVENT_PERIOD_CHECK:
        {
            app_harman_adc_io_read();
            // app_ext_charger_fully_charged_level_get();
            if (app_ext_charger_check_recharge())
            {
                new_state = APP_CHARGER_STATE_CHARGING;
            }
        }
        break;

    default:
        break;
    }

    return new_state;
}

T_APP_CHARGER_STATE app_ext_charger_charging_error_handler(APP_EXT_CHARGER_EVENT_TYPE
                                                           event, uint8_t *param)
{
    T_APP_CHARGER_STATE new_state = APP_CHARGER_STATE_ERROR;

    switch (event)
    {
    case APP_EXT_CHARGER_EVENT_ADP_OUT:
        {
            app_ext_charger_handle_adp_out();
            new_state = APP_CHARGER_STATE_NO_CHARGE;
        }
        break;

    case APP_EXT_CHARGER_EVENT_PERIOD_CHECK:
        {
            app_harman_adc_io_read();
        }
        break;

    case APP_EXT_CHARGER_EVENT_ADC_UPDATE:
        {
            if (ext_charger_mgr.ignore_first_adc_sample)
            {
                ext_charger_mgr.ignore_first_adc_sample = false;
            }
            else
            {
                if (ext_charger_cfg.thermistor_1_enable)
                {
                    new_state = app_ext_charger_check_ntc_threshold();
                }
            }
        }
        break;

    default:
        break;
    }

    return new_state;
}

static void app_ext_charger_update_charger_state(T_APP_CHARGER_STATE charger_state)
{
    T_CHARGER_STATE rtk_charger_state = STATE_CHARGER_END;

    if (ext_charger_mgr.state != charger_state)
    {
        if (charger_state == APP_CHARGER_STATE_CHARGE_FINISH)
        {
            rtk_charger_state = STATE_CHARGER_FINISH;
        }
        else if (charger_state == APP_CHARGER_STATE_ERROR)
        {
            rtk_charger_state = STATE_CHARGER_ERROR;
        }
        else if (charger_state == APP_CHARGER_STATE_NO_CHARGE)
        {
            rtk_charger_state = STATE_CHARGER_END;
        }
        else if (charger_state == APP_CHARGER_STATE_CHARGING)
        {
            rtk_charger_state = STATE_CHARGER_FAST_CHARGE;
        }

        APP_PRINT_INFO4("app_ext_charger_update_charger_state: (%d) -> (%d), rtk_charger_state: %d, ignore_first_adc_sample: %d",
                        ext_charger_mgr.state, charger_state, rtk_charger_state, ext_charger_mgr.ignore_first_adc_sample);
        ext_charger_mgr.state = charger_state;

        app_charger_state_cb(rtk_charger_state);
        ext_charger_mgr.ignore_first_adc_sample = true;
    }
}

static void app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_TYPE event, uint8_t *param)
{
    T_APP_CHARGER_STATE new_state = ext_charger_mgr.state;
    APP_PRINT_TRACE2("app_ext_charger_state_machine: state %d, event: %d,",
                     ext_charger_mgr.state, event);

    new_state = ext_charger_charging_state_handler[ext_charger_mgr.state](event, param);
    app_ext_charger_update_charger_state(new_state);
}

static void app_ext_charger_timeout_cb(uint8_t timer_id, uint16_t timer_chann)
{
    APP_PRINT_TRACE2("app_ext_charger_timeout_cb: timer_id 0x%02x, timer_chann %d ",
                     timer_id, timer_chann);
    switch (timer_id)
    {
#if HARMAN_DELAY_CHARGING_FINISH_SUPPORT
    case APP_EXT_CHARGER_TIMER_DELAY_CHARGING_FINISH:
        {
            app_stop_timer(&timer_idx_delay_charging_finish);

            uint8_t param = 0xFF;

            app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_CHARGING_FULL, &param);
        }
        break;
#endif

#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
    case APP_EXT_CHARGER_TIMER_DELAY_ADP_OUT_HANDLE:
        {
            app_stop_timer(&timer_idx_delay_adp_out);
            app_harman_adc_ntc_check_timer_stop();
            app_harman_usb_adc_ntc_check_timer_stop();

            app_cfg_nv.vbat_detect_status = VBAT_DETECT_STATUS_ING;
            app_cfg_store(&app_cfg_nv.ota_continuous_packets_max_count, 4);
        }
        break;
#endif

    default:
        break;
    }
}

static void app_ext_charger_plug_status_cb(T_ADP_PLUG_EVENT plug_event, void *user_data)
{
    APP_PRINT_TRACE1("app_ext_charger_plug_status_cb: plug_event %d", plug_event);
#if HARMAN_EXTERNAL_CHARGER_DZ582_SUPPORT
    high_voltage_check_times = 0;
#endif

    if (plug_event == ADP_EVENT_PLUG_IN)
    {
        app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_ADP_IN, NULL);
        app_dlps_enable(APP_DLPS_ENTER_CHECK_ADAPTOR);
    }
    else if (plug_event == ADP_EVENT_PLUG_OUT)
    {
        app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_ADP_OUT, NULL);
        app_dlps_enable(APP_DLPS_ENTER_CHECK_ADAPTOR);
    }
    else
    {
        app_dlps_disable(APP_DLPS_ENTER_CHECK_ADAPTOR);
    }
}

static void app_ext_charger_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case BT_EVENT_READY:
        {
            T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

            if ((adp_5v_state == ADP_STATE_IN) && !app_harman_adc_ntc_check_timer_started())
            {
                T_IO_MSG adp_msg;
                adp_msg.type = IO_MSG_TYPE_GPIO;
                adp_msg.subtype = IO_MSG_GPIO_ADAPTOR_PLUG;
                adp_msg.u.param = ADAPTOR_PLUG;

                app_io_msg_send(&adp_msg);
            }
        }
        break;

    default:
        break;
    }
}

void app_ext_charger_ntc_check_timeout(void)
{
    app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_PERIOD_CHECK, NULL);
}

void app_ext_charger_handle_enter_dlps(void)
{
    uint8_t gpio_level = hal_gpio_get_input_level(ext_charger_mgr.status_det_pin);
    if (gpio_level)
    {
        app_dlps_set_pad_wake_up(ext_charger_mgr.status_det_pin, PAD_WAKEUP_POL_LOW);
    }
    else
    {
        app_dlps_set_pad_wake_up(ext_charger_mgr.status_det_pin, PAD_WAKEUP_POL_HIGH);
    }
}

void app_ext_charger_handle_exit_dlps(void)
{
    app_dlps_restore_pad(ext_charger_mgr.status_det_pin);
}

ISR_TEXT_SECTION
bool app_ext_charger_is_pin_status_det_pin(uint8_t gpio_num)
{
    if (GPIO_GetNum(ext_charger_mgr.status_det_pin) == gpio_num)
    {
        return true;
    }
    return false;
}

uint8_t app_ext_charger_check_status(void)
{
    uint8_t ret = false;
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    APP_PRINT_TRACE4("app_ext_charger_check_status: %08x, extcharge=%d, ADP=%d, charger_en=%d",
                     app_dlps_get_dlps_bitmap(),
                     ext_charger_cfg.ext_charger_support,
                     adp_5v_state,
                     ext_charger_mgr.enable);
    if ((app_ext_charger_check_support()) &&
        ((adp_5v_state == ADP_STATE_IN)
#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
         || (timer_idx_delay_adp_out != 0)
#endif
        ))
    {
        ret = true;
    }
    return ret;
}

void app_ext_charger_ntc_adc_update(void)
{
    app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_ADC_UPDATE, NULL);
}

void app_ext_charger_handle_io_msg(T_IO_MSG console_msg)
{
    uint8_t gpio_level = (uint8_t) console_msg.u.param;
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    if ((gpio_level == 1) && (ext_charger_mgr.enable) && (adp_5v_state == ADP_STATE_IN))
    {
        app_ext_charger_state_machine(APP_EXT_CHARGER_EVENT_CHARGING_FULL, &gpio_level);
    }
#if HARMAN_DELAY_CHARGING_FINISH_SUPPORT
    else if ((gpio_level == 0) && (timer_idx_delay_charging_finish != NULL))
    {
        app_stop_timer(&timer_idx_delay_charging_finish);
    }
#endif
}

ISR_TEXT_SECTION
bool app_ext_charger_check_support(void)
{
    return ext_charger_cfg.ext_charger_support;
}

static bool app_ext_charger_dlps_check_cb(void)
{
    T_ADP_STATE adp_5v_state = adp_get_current_state(ADP_DETECT_5V);

    APP_PRINT_TRACE4("app_ext_charger_dlps_check_cb: dlps_bitmap: 0x%08x, ext_charger_support: %d, "
                     "adp_5v_state: %d, ext_charger_enable: %d",
                     app_dlps_get_dlps_bitmap(), ext_charger_cfg.ext_charger_support,
                     adp_5v_state, ext_charger_mgr.enable);
    if ((app_ext_charger_check_support()) &&
        ((adp_5v_state == ADP_STATE_IN)
#if HARMAN_DELAY_HANDLE_ADP_OUT_SUPPORT
         || (timer_handle_delay_adp_out != NULL)
#endif
#if HARMAN_USB_CONNECTOR_PROTECT
         || app_harman_usb_hiccup_check_timer_started()
#endif
        ))
    {
        return false;
    }

    return true;
}

void app_ext_charger_init(void)
{
    app_ext_charger_cfg_load();

    if (!ext_charger_cfg.inter_charger_auto_enable)
    {
        ext_charger_cfg.ext_charger_support = true;

        app_cfg_const.charger_support = false;

        app_ext_charger_enable_ctrl_gpio_init(EXTERNAL_CHARGING_ENABLE_CTL_PIN);
        app_ext_charger_status_det_gpio_init(EXTERNAL_CHARGING_STATUS_DET_PIN);
        app_ext_charger_current_ctrl_gpio_init(EXTERNAL_CHARGING_CURRENT_CTL_PIN1,
                                               EXTERNAL_CHARGING_CURRENT_CTL_PIN2);

        adp_register_state_change_cb(ADP_DETECT_5V, app_ext_charger_plug_status_cb, (void *)ADP_DETECT_5V);

        app_discharger_init();

        bt_mgr_cback_register(app_ext_charger_bt_cback);

        app_timer_reg_cb(app_ext_charger_timeout_cb, &app_ext_charger_timer_id);

        power_check_cb_register(app_ext_charger_dlps_check_cb);

#if !(HARMAN_SECOND_NTC_DETECT_PROTECT | HARMAN_VBAT_ADC_DETECTION)
        app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_SUCCESS);
#endif

#if HARMAN_VBAT_ADC_DETECTION
        if ((!app_cfg_nv.vbat_detect_normal) &&
            (!app_cfg_nv.store_adc_ntc_value_when_vbat_in))
        {
            app_ext_charger_vbat_is_normal_set(VBAT_DETECT_STATUS_FAIL);
        }
#endif

#if HARMAN_USB_CONNECTOR_PROTECT
        app_harman_usb_connector_protect_init();
#endif
    }
}
#endif
