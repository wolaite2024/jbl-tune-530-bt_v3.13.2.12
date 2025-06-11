/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_IO_OUTPUT_SUPPORT
#include "app_cfg.h"
#include "rtl876x_pinmux.h"
#include "sysm.h"
#include "trace.h"
#include "app_timer.h"
#include "app_main.h"
#include "app_io_output.h"
#include "hal_adp.h"
#include "hal_gpio.h"
#include "sysm.h"

static bool app_io_output_enable_10hz_pulse = false;
static uint8_t timer_idx_create_10hz_pulse = 0;
static uint8_t io_output_timer_id = 0;

/**  @brief  Define IO output time event */
typedef enum
{
    APP_TIMER_CREATE_10HZ_PULSE,
} T_APP_IO_OUTPUT_TIMER;

static void app_io_output_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_CREATE_10HZ_PULSE:
        {
            static uint8_t current_state = 0;
            bool config_output_pin = false;

            /* control output pin only in case and device on */
            if (app_db.device_state == APP_DEVICE_STATE_ON && app_db.local_in_case)
            {
                config_output_pin = true;
            }

            if (app_io_output_enable_10hz_pulse)
            {
                if (config_output_pin)
                {
                    current_state ^= 1;

                    Pad_OutputControlValue(app_cfg_const.external_mcu_input_pinmux, (PAD_OUTPUT_VAL)current_state);
                }
            }
            else
            {
                app_stop_timer(&timer_idx_create_10hz_pulse);

                Pad_OutputControlValue(app_cfg_const.external_mcu_input_pinmux, PAD_OUT_LOW);
                current_state = 0;
            }
        }
        break;

    default:
        break;
    }
}

void app_io_output_ctrl_ext_mcu_pin(uint8_t enable)
{
    APP_PRINT_TRACE2("app_io_output_ctrl_ext_mcu_pin: enable_external_mcu_reset: 0x%x, ENABLE: %d",
                     app_cfg_const.enable_external_mcu_reset, enable);

    if (app_cfg_const.enable_external_mcu_reset)
    {
        if (enable)
        {
            if (app_io_output_enable_10hz_pulse == false)
            {
                app_io_output_enable_10hz_pulse = true;
                app_start_timer(&timer_idx_create_10hz_pulse, "create_10hz_pulse",
                                io_output_timer_id, APP_TIMER_CREATE_10HZ_PULSE, 0, true,
                                50);
            }
        }
        else
        {
            app_io_output_enable_10hz_pulse = false;
        }
    }
}

static void app_io_output_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_OFF:
        {
#if F_APP_IO_OUTPUT_SUPPORT
            if (app_cfg_const.enable_external_mcu_reset)
            {
                app_io_output_ctrl_ext_mcu_pin(DISABLE);
            }
#endif

            if (app_cfg_const.enable_power_supply_adp_in)
            {
                if (!adp_get_level(ADP_DETECT_5V)) //adp plug still output power supply
                {
                    app_io_output_power_supply(false);
                }
            }
            else
            {
                if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
                {
                    /* Disable GPIO interrupt to prevent system from being blocked by sensor */
                    hal_gpio_irq_disable(app_cfg_const.sensor_detect_pinmux);
                }

                app_io_output_power_supply(false);
            }

            app_io_output_out_indication_pinmux_init();
        }
        break;

    case SYS_EVENT_POWER_ON:
        {
#if F_APP_IO_OUTPUT_SUPPORT
            if (app_cfg_const.enable_external_mcu_reset && app_db.local_in_case)
            {
                app_io_output_ctrl_ext_mcu_pin(ENABLE);
            }
#endif

            if (app_cfg_const.output_indication_support == 1 &&
                app_cfg_const.enable_output_power_supply == 1)
            {
                if (app_cfg_const.enable_output_ind1_high_active == true)
                {
                    Pad_OutputControlValue(app_cfg_const.output_indication1_pinmux, PAD_OUT_HIGH);
                }
                else
                {
                    Pad_OutputControlValue(app_cfg_const.output_indication1_pinmux, PAD_OUT_LOW);
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_io_output_init(void)
{
    app_timer_reg_cb(app_io_output_timeout_cb, &io_output_timer_id);

    sys_mgr_cback_register(app_io_output_dm_cback);
}

void app_io_output_out_indication_pinmux_init(void)
{
    if (app_cfg_const.output_indication_support) //output_indication_support
    {
        if (app_cfg_const.output_indication1_pinmux != 0xFF)
        {
            PAD_OUTPUT_VAL tmp_output_indication = PAD_OUT_HIGH;

            if (app_cfg_const.enable_output_power_supply == 0 &&
                app_cfg_const.enable_output_ind1_high_active == 1)
            {
                tmp_output_indication = PAD_OUT_LOW;
            }

            if (app_cfg_const.enable_output_power_supply == 1)
            {
                tmp_output_indication = PAD_OUT_LOW;
            }

            Pad_Config(app_cfg_const.output_indication1_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, tmp_output_indication);
        }

        if (app_cfg_const.output_indication2_pinmux != 0xFF)
        {
            PAD_OUTPUT_VAL tmp_output_indication = PAD_OUT_HIGH;

            if (app_cfg_const.enable_output_ind2_high_active)
            {
                tmp_output_indication = PAD_OUT_LOW;
            }
            Pad_Config(app_cfg_const.output_indication2_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, tmp_output_indication);
        }

        if (app_cfg_const.output_indication3_pinmux != 0xFF)
        {
            PAD_OUTPUT_VAL tmp_output_indication = PAD_OUT_HIGH;

            if (app_cfg_const.enable_output_ind3_high_active)
            {
                tmp_output_indication = PAD_OUT_LOW;
            }
            Pad_Config(app_cfg_const.output_indication3_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, tmp_output_indication);
        }
#if F_APP_IO_OUTPUT_SUPPORT
        if (adp_get_level(ADP_DETECT_5V) &&
            app_cfg_const.enable_power_supply_adp_in) //adp plug still output power supply
        {
            app_io_output_power_supply(true);
        }
        else
        {
            app_io_output_power_supply(false);
        }
#endif
    }
}

void app_io_output_power_supply(bool enable)
{
    PAD_OUTPUT_VAL output_val;
    output_val = (!(enable ^ (app_cfg_const.enable_output_ind1_high_active))) ? PAD_OUT_HIGH :
                 PAD_OUT_LOW;
    if (app_cfg_const.output_indication_support == 1 &&
        app_cfg_const.enable_output_power_supply == 1)
    {
        APP_PRINT_TRACE2("app_io_output_power_supply indication1_pin %s output_val=%d",
                         TRACE_STRING(Pad_GetPinName(app_cfg_const.output_indication1_pinmux)), output_val);
        Pad_OutputControlValue(app_cfg_const.output_indication1_pinmux, output_val);

        Pad_SetPinDrivingCurrent(app_cfg_const.output_indication1_pinmux, LEVEL3);
    }
}
#endif
