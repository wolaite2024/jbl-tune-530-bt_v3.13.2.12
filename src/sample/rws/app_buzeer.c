/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_BUZZER_SUPPORT
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "trace.h"
#include "app_buzzer.h"
#include "app_cfg.h"
#include "rtl876x_pinmux.h"
#include "board.h"
#include "app_dlps.h"
#include "section.h"
#include "pwm.h"

/*============================================================================*
 *                              Constants
 *============================================================================*/
typedef struct
{
    uint16_t on_duration;       /**< buzzer on duration  unit:10ms*/
    uint16_t off_duration;      /**< buzzer off duration  unit:10ms*/
    uint8_t count;              /**< buzzer cycle times*/
} T_BUZZER_TABLE;

typedef struct
{
    uint8_t buzzer_mode;
    uint8_t buzzer_count;
    uint8_t buzzer_on;
    uint16_t timer_count;
    uint16_t timer_count_on;
    uint32_t timer_count_off;
    T_PWM_HANDLE pwm_handle;
    uint32_t pwm_high_count;
    uint32_t pwm_low_count;
} T_BUZZER_DATA;

const T_BUZZER_TABLE BUZZER_TABLE[] =
{
    //Unit: 10ms

    {0xFF},             //0x00 :NO Buzzer

    {5, 5, 1},          //0x01 :Single      50ms / 50ms buzzer
    {5, 5, 2},          //0x02 :Dual        50ms / 50ms buzzer
    {5, 5, 3},          //0x03 :Tripple     50ms / 50ms buzzer

    {10, 10, 1},        //0x04 :Single      100ms / 100ms buzzer
    {10, 10, 2},        //0x05 :Dual        100ms / 100ms buzzer
    {10, 10, 3},        //0x06 :Tripple     100ms / 100ms buzzer

    {20, 20, 1},        //0x07 :Single      200ms / 200ms buzzer
    {20, 20, 2},        //0x08 :Dual        200ms / 200ms buzzer
    {20, 20, 3},        //0x09 :Tripple     200ms / 200ms buzzer

    {50, 50, 1},        //0x0A :Single      500ms / 500ms buzzer
    {50, 50, 2},        //0x0B :Dual        500ms / 500ms buzzer
    {50, 50, 3},        //0x0C :Tripple     500ms / 500ms buzzer

    {100, 100, 1},      //0x0D :Single      1s / 1s buzzer
    {100, 100, 2},      //0x0E :Dual        1s / 1s buzzer
    {100, 100, 3},      //0x0F :Tripple     1s / 1s buzzer

    {200, 200, 1},      //0x10 :Single      2s / 2s buzzer
    {200, 200, 2},      //0x11 :Dual        2s / 2s buzzer
    {200, 200, 3},      //0x12 :Tripple     2s / 2s buzzer

    {150, 300, 1},      //0x13 :Single      1.5s / 3s buzzer
};

T_BUZZER_DATA buzzer_data;

ISR_TEXT_SECTION
void buzzer_pwm_isr_timeout_callback(T_PWM_HANDLE pwm_handle)
{
    if (app_cfg_const.buzzer_support)
    {
        if (buzzer_data.buzzer_on)
        {
            buzzer_data.timer_count++;

            if (buzzer_data.timer_count >= buzzer_data.timer_count_on)
            {
                buzzer_data.buzzer_on = 0;
                buzzer_data.timer_count = 0;

                pwm_pin_config(buzzer_data.pwm_handle, app_cfg_const.pwm_pinmux, PWM_FUNC_DISABLE);
                pwm_stop(buzzer_data.pwm_handle);
                pwm_change_duty_and_frequency(buzzer_data.pwm_handle, buzzer_data.timer_count_off, 1);
                pwm_start(buzzer_data.pwm_handle);
            }
        }
        else //Buzzer off
        {
            buzzer_data.buzzer_count++;

            if (buzzer_data.buzzer_count ==
                BUZZER_TABLE[buzzer_data.buzzer_mode].count)
            {
                buzzer_data.buzzer_count = 0;
                pwm_stop(buzzer_data.pwm_handle);
                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
            }
            else
            {
                buzzer_data.buzzer_on = 1;
                pwm_pin_config(buzzer_data.pwm_handle, app_cfg_const.pwm_pinmux, PWM_FUNC);
                pwm_change_duty_and_frequency(buzzer_data.pwm_handle, buzzer_data.pwm_high_count,
                                              buzzer_data.pwm_low_count);
            }
        }
    }

}

void buzzer_check_freq(uint8_t mode)
{
    switch (app_cfg_const.buzzer_pwm_freq)
    {
    case BUZZER_PWM_FREQ_1K: //0.5ms timeout
        buzzer_data.timer_count_on = BUZZER_TABLE[mode].on_duration * 10 * 10 / 5;
        buzzer_data.pwm_high_count = 500; //clock @ 1MHz for BB3
        buzzer_data.pwm_low_count = 500;
        break;

    case BUZZER_PWM_FREQ_2K: //0.25ms timeout
        buzzer_data.timer_count_on = BUZZER_TABLE[mode].on_duration * 10 * 100 / 25;
        buzzer_data.pwm_high_count = 250; //clock @ 1MHz for BB3
        buzzer_data.pwm_low_count = 250;
        break;

    case BUZZER_PWM_FREQ_3K: //0.16ms timeout
        buzzer_data.timer_count_on = BUZZER_TABLE[mode].on_duration * 10 * 100 / 16;
        buzzer_data.pwm_high_count = 500 / 3; //clock @ 1MHz for BB3
        buzzer_data.pwm_low_count = 500 / 3;
        break;
    default:
        buzzer_data.pwm_high_count = 500;
        buzzer_data.pwm_low_count = 500;
    }
}

/*============================================================================*
 *                              Public Functions
 *============================================================================*/
void buzzer_set_mode(uint8_t mode)
{
    if (app_cfg_const.buzzer_support)
    {
        if (mode == BUZZER_MODE_NONE)
        {
            pwm_stop(buzzer_data.pwm_handle);

        }
        else
        {
            //get the buzzer settings
            buzzer_check_freq(mode);

            if (buzzer_data.pwm_handle == NULL)
            {
                buzzer_data.pwm_handle = pwm_create("app_buzzer", buzzer_data.pwm_high_count,
                                                    buzzer_data.pwm_low_count, false);
                if (buzzer_data.pwm_handle == NULL)
                {

                    APP_PRINT_ERROR0("could not create pwm!!");
                    return;
                }
            }
            else
            {
                pwm_change_duty_and_frequency(buzzer_data.pwm_handle, buzzer_data.pwm_high_count,
                                              buzzer_data.pwm_low_count);
            }

            buzzer_data.buzzer_mode = mode;
            buzzer_data.buzzer_on = 1;
            buzzer_data.timer_count = 0;
            buzzer_data.timer_count_off = BUZZER_TABLE[mode].off_duration * 10 *
                                          1000;  //Use timer timeout @ 1MHz for PWM OFF
            buzzer_data.timer_count_on = BUZZER_TABLE[mode].on_duration * 10 * 10 / 5;

            app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

            pwm_register_timeout_callback(buzzer_data.pwm_handle, buzzer_pwm_isr_timeout_callback);
            pwm_pin_config(buzzer_data.pwm_handle, app_cfg_const.pwm_pinmux, PWM_FUNC);
            pwm_start(buzzer_data.pwm_handle);
        }
    }
}

#endif
