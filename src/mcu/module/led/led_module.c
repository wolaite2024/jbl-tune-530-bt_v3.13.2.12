/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_module.c
* @brief    This file provides functions to set some LED working mode.
* @details
* @author   brenda_li
* @date     2016-12-15
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include "trace.h"
#include "board.h"
#include "os_sched.h"
#include "os_mem.h"
#include "os_sync.h"
#include "rtl876x_pinmux.h"
#include "rtl876x.h"
#include "rtl876x_sleep_led.h"
#include "pwm.h"
#include "hw_tim.h"
#include "led_module.h"
#include "app_timer.h"
#include "app_cfg.h"

typedef struct
{
    uint16_t led_breath_timer_count;// uint8_t  pwm interrupt cb api overflow
    uint16_t led_breath_blink_count;
    uint8_t pwm_id;
} T_LED_SW_BREATH_MODE;

#define PWM_OUT_COUNT         10000 //time Unit: 10ms(10000/1MHz)
#define MAX_UPDATE_RATE       15
#define MAX_PHASE_DUTY_STEP   63 //phase duty step is only 6 bits

static T_LED_SW_BREATH_MODE sw_breathe_mode[LED_NUM];
static T_PWM_HANDLE pwm_handle[LED_NUM];
static uint32_t led_create_timer_point[LED_NUM];
static uint32_t led_create_timer_record[LED_NUM];
static uint8_t led_polarity[LED_NUM];
static bool led_driver_by_sw;
static uint8_t *led0_pinmux = &app_cfg_const.led_0_pinmux;
static const char *led_timer_name[LED_NUM] = {"led0_on_off", "led1_on_off", "led2_on_off"};
static uint8_t timer_idx_led_blink[LED_NUM] = {0};
static uint8_t led_module_timer_id = 0;
static T_LED_TABLE led_setting_record[LED_NUM];
static T_LED_TABLE led_count_record[LED_NUM];

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
const uint8_t sleep_led_pinmux[SLEEP_LED_PINMUX_NUM] = {ADC_0, ADC_1, P0_6, P0_7, P1_0, P1_1, P1_4, P2_0, P2_1, P2_2};
#else
const uint8_t sleep_led_pinmux[SLEEP_LED_PINMUX_NUM] = {ADC_0, ADC_1, P1_0, P1_1, P2_1, P2_2, P2_3, P2_4, P3_0, P3_1};
#endif

static void led_clear_para(T_LED_CHANNEL led_ch);
static void led_blink_handle(T_LED_CHANNEL led_ch);
static void led_sw_breath_timer_control(uint8_t i, FunctionalState new_state);
static void led_sw_breath_init(uint8_t i);

static inline uint8_t log2_u32(uint32_t val)
{
    return (31 - __builtin_clz(val));
}

void led_set_active_polarity(T_LED_CHANNEL led_ch, T_LED_ACTIVE_POLARITY polarity)
{
    uint8_t led_idx = log2_u32(led_ch);

    //SleepLed_SetIdleMode((SLEEP_LED_CHANNEL)channel, polarity);

    led_polarity[led_idx] = polarity;
}

static void led_cmd_handle(T_LED_CHANNEL led_ch, bool state)
{
    uint8_t led_idx = log2_u32(led_ch);
    uint32_t s;

#if 0
    if ((led_driver_by_sw) && (!led_is_sleep_pinmux(*(led0_pinmux + led_idx))))
    {
        //LED should be handled simultaneously
        //Avoid APP be preempted by higher priority task
        s = os_lock();

        if (state)
        {
            //set pad and start timer
            led_blink_handle(led_ch);
        }
        else
        {
            //reset pad and stop timer
            led_deinit(led_ch);
            led_clear_para(led_ch);
            app_stop_timer(&timer_idx_led_blink[led_idx]);
        }

        os_unlock(s);
    }
    else
#endif
    {
        SleepLed_Cmd(led_ch, (FunctionalState)state);
    }
}

void led_cmd(uint8_t channel, bool state)
{
    uint8_t i;

    for (i = 0; i < LED_NUM; i++)
    {
        if (channel & BIT(i))
        {
            led_cmd_handle((T_LED_CHANNEL)BIT(i), state);
        }
    }
}

void led_reset_sleep_led(void)
{
    SleepLed_Reset();
}

void led_deinit(T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);

#if 0
    if ((led_driver_by_sw) && (!led_is_sleep_pinmux(*(led0_pinmux + led_idx))))
    {
        // stop breath timer at first
        led_sw_breath_timer_control(led_idx, DISABLE);

        if (led_setting_record[led_idx].type == LED_TYPE_BREATH)
        {
            pwm_pin_config(pwm_handle[led_idx], *(led0_pinmux + led_idx), PWM_FUNC);
        }
        else
        {
            //reset pad
            Pad_Config(*(led0_pinmux + led_idx),
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, (PAD_OUTPUT_VAL)led_polarity[led_idx]);
        }
    }
    else
#endif
    {
        if (led_polarity[led_idx] == LED_ACTIVE_POLARITY_LOW)
        {
            SLEEP_LED_InitTypeDef led_param;

            SleepLed_StructInit(&led_param);

            led_param.phase_phase_tick[0] = 1023;
            led_param.phase_initial_duty[0] = LED_TIME_TICK_SCALE;
            //Bypass other phases

            SleepLed_Init((SLEEP_LED_CHANNEL)led_ch, &led_param);
            SleepLed_Cmd((SLEEP_LED_CHANNEL)led_ch, ENABLE);
        }
        else
        {
            SleepLed_DeInit((SLEEP_LED_CHANNEL)led_ch);
        }
    }
}

void led_config(T_LED_CHANNEL led_ch, T_LED_TABLE *table)
{
    uint8_t led_idx = log2_u32(led_ch);
    uint8_t update_rate[2] = {0};
    uint8_t on_off_time[2] = {table->on_time, table->off_time};
    uint8_t on_off_min_remainder[2] = {0xFF, 0xFF};
    uint16_t interval = table->blink_interval * 10; // Unit: 100ms

#if 0
    if ((led_driver_by_sw)  && (!led_is_sleep_pinmux(*(led0_pinmux + led_idx))))
    {
        led_setting_record[led_idx].type = table->type;// new set type
        led_setting_record[led_idx].on_time = table->on_time;
        led_setting_record[led_idx].off_time = table->off_time;
        led_setting_record[led_idx].blink_count = 2 * table->blink_count;
        led_setting_record[led_idx].blink_interval = table->blink_interval;

        led_count_record[led_idx].blink_count = 2 * table->blink_count;
        led_deinit(led_ch);
    }
    else
#endif
    {
        if (table->type == LED_TYPE_KEEP_OFF)
        {
            if (led_polarity[led_idx] == LED_ACTIVE_POLARITY_LOW)
            {
                SLEEP_LED_InitTypeDef led_param;

                SleepLed_StructInit(&led_param);

                led_param.phase_phase_tick[0] = 1023;
                led_param.phase_initial_duty[0] = LED_TIME_TICK_SCALE;
                //Bypass other phases

                SleepLed_Init((SLEEP_LED_CHANNEL)led_ch, &led_param);
                SleepLed_Cmd((SLEEP_LED_CHANNEL)led_ch, ENABLE);
            }
            else
            {
                SleepLed_DeInit((SLEEP_LED_CHANNEL)led_ch);
            }
        }
        else if (table->type == LED_TYPE_KEEP_ON)
        {
            SLEEP_LED_InitTypeDef led_param;

            SleepLed_StructInit(&led_param);

            if (led_polarity[led_idx] == LED_ACTIVE_POLARITY_LOW)
            {
                led_param.polarity = LED_OUTPUT_INVERT;
            }

            led_param.phase_phase_tick[0] = 1023;
            led_param.phase_initial_duty[0] = LED_TIME_TICK_SCALE;
            //Bypass other phases

            SleepLed_Init((SLEEP_LED_CHANNEL)led_ch, &led_param);
        }
        else
        {
            SLEEP_LED_InitTypeDef led_param;

            SleepLed_StructInit(&led_param);

            if (led_polarity[led_idx] == LED_ACTIVE_POLARITY_LOW)
            {
                led_param.polarity = LED_OUTPUT_INVERT;
            }

            if (table->type == LED_TYPE_ON_OFF)
            {
                led_param.polarity ^= 1;
            }

            //Calculate update rate
            //Adjust update rate to avoid duty gap from ON phase to OFF phase and cause abnormal brightness
            for (uint8_t i = 0; i < 2; i++) //on and off time
            {
                for (uint8_t j = 0; j <= MAX_UPDATE_RATE; j++)
                {
                    uint8_t tmp_remainder = 0;

                    if (LED_TIME_TICK_SCALE * (j + 1) / on_off_time[i] <= MAX_PHASE_DUTY_STEP)
                    {
                        tmp_remainder = LED_TIME_TICK_SCALE * (j + 1) % on_off_time[i];
                        if (tmp_remainder == 0)
                        {
                            update_rate[i] = j;
                            on_off_min_remainder[i] = 0;
                            break;
                        }
                        else
                        {
                            if (on_off_min_remainder[i] > tmp_remainder)
                            {
                                on_off_min_remainder[i] = tmp_remainder;
                                update_rate[i] = j;
                            }
                        }
                    }
                }
            }

            //Always use phase registers to implement LED blink and breathe
            //Use 8 phases to achive 4 blinks
            for (uint8_t i = 0; i < table->blink_count; i++)
            {
                led_param.phase_phase_tick[(2 * i)] = table->on_time;
                led_param.phase_initial_duty[(2 * i) + 1] = LED_TIME_TICK_SCALE;
                led_param.phase_phase_tick[(2 * i) + 1] = table->off_time;

                if ((i + 1) == table->blink_count)
                {
                    led_param.phase_phase_tick[(2 * i) + 1] += interval;
                    if (led_param.phase_phase_tick[(2 * i) + 1] > 1023)
                    {
                        // Only 10 bits in HW register
                        led_param.phase_phase_tick[(2 * i) + 1] = 1023;
                    }
                }

                if (table->type == LED_TYPE_BREATH)
                {
                    led_param.phase_uptate_rate[2 * i] = update_rate[0];
                    led_param.phase_duty_step[2 * i] = LED_TIME_TICK_SCALE * (update_rate[0] + 1) / table->on_time;
                    led_param.phase_increase_duty[2 * i] = 1;
                    led_param.phase_uptate_rate[(2 * i) + 1] = update_rate[1];
                    led_param.phase_duty_step[(2 * i) + 1] = LED_TIME_TICK_SCALE * (update_rate[1] + 1) /
                                                             table->off_time;
                }
            }

            SleepLed_Init((SLEEP_LED_CHANNEL)led_ch, &led_param);
        }
    }
}

#if 0
static void led_create_timer(uint32_t time, T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);

    led_create_timer_point[led_idx] = os_sys_time_get();
    led_create_timer_record[led_idx] = time;

    app_start_timer(&timer_idx_led_blink[led_idx], led_timer_name[led_idx],
                    led_module_timer_id, 0, led_ch, false,
                    time);
}

static void led_blink_handle(T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);

    if (*(led0_pinmux + led_idx) == 0xFF)
    {
        return;
    }

    if ((led_setting_record[led_idx].type == LED_TYPE_KEEP_ON) ||
        (led_setting_record[led_idx].type == LED_TYPE_ON_OFF))
    {
        if (led_polarity[led_idx])
        {
            Pad_OutputControlValue(*(led0_pinmux + led_idx), PAD_OUT_LOW);
        }
        else
        {
            Pad_OutputControlValue(*(led0_pinmux + led_idx), PAD_OUT_HIGH);
        }
    }
    else
    {
        led_deinit(led_ch);
    }

    if ((led_setting_record[led_idx].type == LED_TYPE_ON_OFF) ||
        (led_setting_record[led_idx].type == LED_TYPE_OFF_ON))
    {
        led_create_timer(led_setting_record[led_idx].on_time * 10, led_ch);
    }
    else if (led_setting_record[led_idx].type == LED_TYPE_BREATH)
    {
        led_sw_breath_init(led_idx);
        pwm_pin_config(pwm_handle[led_idx], *(led0_pinmux + led_idx), PWM_FUNC);
        led_sw_breath_timer_control(led_idx, ENABLE);
    }
}

static void led_module_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    T_LED_CHANNEL led_ch = (T_LED_CHANNEL)param;
    uint8_t led_idx = log2_u32(led_ch);

    if ((os_sys_time_get() - led_create_timer_point[led_idx]) >= led_create_timer_record[led_idx])
    {
        //restart repeat blink when blink_interval is not zero
        if (led_setting_record[led_idx].blink_count == 0)
        {
            led_deinit(led_ch);
            led_setting_record[led_idx].blink_count = led_count_record[led_idx].blink_count;
            led_cmd(led_ch, ENABLE);
        }
        else if (led_setting_record[led_idx].blink_count != 1)
        {
            led_setting_record[led_idx].blink_count--;
            if (Pad_GetOutputCtrl(*(led0_pinmux + led_idx)) == PAD_OUT_LOW)
            {
                Pad_OutputControlValue(*(led0_pinmux + led_idx), PAD_OUT_HIGH);
            }
            else
            {
                Pad_OutputControlValue(*(led0_pinmux + led_idx), PAD_OUT_LOW);
            }

            if (led_setting_record[led_idx].blink_count % 2 == 1) //start off timer when blink_cnt is odd
            {
                led_create_timer(led_setting_record[led_idx].off_time * 10, led_ch);
            }
            else
            {
                led_create_timer(led_setting_record[led_idx].on_time * 10, led_ch);
            }
        }
        else
        {
            if (led_setting_record[led_idx].blink_interval != 0) //blink_interval is not zero
            {
                led_deinit(led_ch);
                led_setting_record[led_idx].blink_count = 0;
                led_create_timer(led_setting_record[led_idx].blink_interval * 100, led_ch);
            }
            else
            {
                led_deinit(led_ch);
                led_setting_record[led_idx].blink_count = led_count_record[led_idx].blink_count;
                led_cmd(led_ch, ENABLE);
            }
        }
    }
}
#endif

static void led_clear_para(T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);

    memset(&led_setting_record[led_idx], 0, sizeof(T_LED_TABLE));
}

#if 0
void led_set_driver_mode(void)
{
    if (app_cfg_const.sw_led_support) //led setting using sw timer
    {
        led_driver_by_sw = true;
        app_timer_reg_cb(led_module_timeout_cb, &led_module_timer_id);
    }
    else
    {
        led_driver_by_sw = false;
    }
}
#endif

bool led_is_sleep_pinmux(uint8_t pinmux)
{
    bool ret = false;
    int i;

    for (i = 0; i <  SLEEP_LED_PINMUX_NUM ; i++)
    {
        if (pinmux == sleep_led_pinmux[i])
        {
            ret = true;
            break;
        }
    }

    return ret;
}

#if 0
static void led_sw_breath_timer_control(uint8_t i, FunctionalState new_state)
{
    if (new_state)
    {
        pwm_start(pwm_handle[i]);
    }
    else
    {
        pwm_stop(pwm_handle[i]);
    }
}

void led_sw_breath_timer_handler(void *handle)
{
    uint8_t i = 0;
    uint8_t led_num = 0;
    uint32_t high_count = 0;
    uint32_t low_count = 0;

    for (i = 0; i < LED_NUM; i++)
    {
        if (sw_breathe_mode[i].pwm_id == hw_timer_get_id(handle) &&
            (led_setting_record[i].type == LED_TYPE_BREATH) &&
            (led_setting_record[i].on_time != 0))
        {
            led_num = i + 1;
            break;
        }
    }

    if (led_num != 0)
    {
        sw_breathe_mode[i].led_breath_timer_count++;

        if (sw_breathe_mode[i].led_breath_timer_count <=
            2 * led_setting_record[led_num - 1].on_time) //set led breath duty during on time
        {
            high_count = PWM_OUT_COUNT * sw_breathe_mode[i].led_breath_timer_count /
                         led_setting_record[led_num - 1].on_time / 2;
            low_count = PWM_OUT_COUNT - PWM_OUT_COUNT * sw_breathe_mode[i].led_breath_timer_count /
                        led_setting_record[led_num - 1].on_time / 2;
        }
        else if ((sw_breathe_mode[i].led_breath_timer_count >
                  2 * led_setting_record[led_num - 1].on_time) && //set led breath duty during off time
                 (sw_breathe_mode[i].led_breath_timer_count <=
                  2 * (led_setting_record[led_num - 1].on_time + led_setting_record[led_num - 1].off_time)))
        {
            high_count = PWM_OUT_COUNT - PWM_OUT_COUNT * (sw_breathe_mode[i].led_breath_timer_count -
                                                          2 * led_setting_record[led_num - 1].on_time) / led_setting_record[led_num - 1].off_time / 2;
            low_count = PWM_OUT_COUNT * (sw_breathe_mode[i].led_breath_timer_count -
                                         2 * led_setting_record[led_num - 1].on_time) / led_setting_record[led_num - 1].off_time / 2;
        }
        else if (sw_breathe_mode[i].led_breath_timer_count >
                 2 * (led_setting_record[led_num - 1].on_time + led_setting_record[led_num - 1].off_time))
        {
            //restart blink when led_breath_blink_count is less than blink_count
            if (sw_breathe_mode[i].led_breath_blink_count < led_setting_record[led_num - 1].blink_count / 2)
            {
                sw_breathe_mode[i].led_breath_timer_count = 0;
            }
            else
            {
                if (sw_breathe_mode[i].led_breath_timer_count <= (2 * (led_setting_record[led_num - 1].on_time +
                                                                       led_setting_record[led_num - 1].off_time) +
                                                                  led_setting_record[led_num - 1].blink_interval *
                                                                  10)) // disable led breath when blink_interval is not zero
                {
                    high_count = 0;
                    low_count = PWM_OUT_COUNT;
                }
                else
                {
                    sw_breathe_mode[i].led_breath_timer_count = 0;
                    sw_breathe_mode[i].led_breath_blink_count = 0;
                }
            }
        }

        if (sw_breathe_mode[i].led_breath_timer_count == 2 * (led_setting_record[led_num - 1].on_time +
                                                              led_setting_record[led_num - 1].off_time))
        {
            sw_breathe_mode[i].led_breath_blink_count++;
        }

        /*up the change duty cnt */
        if (sw_breathe_mode[i].led_breath_timer_count != 0)
        {
            /*change duty */
            pwm_change_duty_and_frequency(pwm_handle[i], high_count, low_count);
        }
    }
}

static void led_sw_breath_init(uint8_t i)
{
    if (pwm_handle[i] == NULL)
    {
        // init high  low level period should not to be too long or led will be light for a while
        if (led_polarity[i] == LED_ACTIVE_POLARITY_LOW)
        {
            pwm_handle[i] = pwm_create("ext_led_low_active", PWM_OUT_COUNT << 1, 1, false);
        }
        else
        {
            pwm_handle[i] = pwm_create("ext_led_high_active", 1, PWM_OUT_COUNT << 1, false);
        }

        if (pwm_handle[i] == NULL)
        {
            APP_PRINT_ERROR0("Could not create extend led pwm!!!");
            return;
        }

        sw_breathe_mode[i].pwm_id = hw_timer_get_id(pwm_handle[i]);
        pwm_register_timeout_callback(pwm_handle[i], led_sw_breath_timer_handler);
    }
}
#endif

