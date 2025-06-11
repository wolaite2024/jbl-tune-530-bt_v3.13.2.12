/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     led_demo.c
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
#include "led_demo.h"
#include "app_timer.h"
#include "app_cfg.h"

#define LED_OUT_0       P2_1
#define LED_OUT_1       P2_2
#define LED_OUT_2       P2_7
#define LED_OUT_3       P1_6
#define LED_OUT_4       P1_7
#define LED_OUT_5       P0_1

static T_LED_TABLE LED_TABLE[2][LED_DEMO_NUM] =
{
    //normal led
    {
        {LED_TYPE_ON_OFF, 10, 10, 2, 20},
        {LED_TYPE_ON_OFF, 10, 10, 2, 20},
        {LED_TYPE_ON_OFF, 10, 10, 2, 20},
        {LED_TYPE_ON_OFF, 10, 10, 2, 20},
        {LED_TYPE_ON_OFF, 10, 10, 2, 20},
        {LED_TYPE_ON_OFF, 10, 10, 2, 20},
    },

    //breath led
    {
        {LED_TYPE_BREATH, 200, 200, 2, 20},
        {LED_TYPE_BREATH, 200, 200, 2, 20},
        {LED_TYPE_BREATH, 200, 200, 2, 20},
        {LED_TYPE_BREATH, 200, 200, 2, 20},
        {LED_TYPE_BREATH, 200, 200, 2, 20},
        {LED_TYPE_BREATH, 200, 200, 2, 20},
    },
};

typedef struct
{
    uint8_t led_breath_timer_count;
    uint8_t led_breath_blink_count;
    uint8_t pwm_id;
} T_LED_SW_BREATH_MODE;

#define PWM_OUT_COUNT 10000 //time Unit: 10ms(10000/1MHz)

static T_LED_SW_BREATH_MODE sw_breathe_mode[LED_DEMO_NUM];
static T_PWM_HANDLE pwm_handle[LED_DEMO_NUM];
static T_LED_TABLE led_setting[LED_DEMO_NUM];
static uint32_t led_create_timer_point[LED_DEMO_NUM];
static uint32_t led_create_timer_record[LED_DEMO_NUM];
static uint8_t led_polarity[LED_DEMO_NUM];
static bool led_driver_by_sw;
static const char *led_timer_name[LED_DEMO_NUM] = {"led0_on_off", "led1_on_off", "led2_on_off", "led3_on_off", "led4_on_off", "led5_on_off"};
static uint8_t timer_idx_led_blink[LED_DEMO_NUM] = {0};
static uint8_t led_module_timer_id = 0;
static T_LED_TABLE led_setting_record[LED_DEMO_NUM];
static T_LED_TABLE led_count_record[LED_DEMO_NUM];

static void led_demo_clear_para(T_LED_CHANNEL led_ch);
static void led_demo_blink_handle(T_LED_CHANNEL led_ch);
static void led_demo_deinit(T_LED_CHANNEL led_ch);
static void led_demo_breath_timer_control(TIM_TypeDef *index, FunctionalState new_state);

static inline uint8_t log2_u32(uint32_t val)
{
    return (31 - __builtin_clz(val));
}

static void led_demo_set_active_polarity(T_LED_CHANNEL led_ch, T_LED_ACTIVE_POLARITY polarity)
{
    uint8_t led_idx = log2_u32(led_ch);

    //SleepLed_SetIdleMode((SLEEP_LED_CHANNEL)channel, polarity);

    led_polarity[led_idx] = polarity;
}

static uint8_t led_demo_get_pin_num(T_LED_CHANNEL led_ch)
{
    uint8_t pin_num;

    switch (led_ch)
    {
    case LED_CH_0:
        pin_num = LED_OUT_0;
        break;

    case LED_CH_1:
        pin_num = LED_OUT_1;
        break;

    case LED_CH_2:
        pin_num = LED_OUT_2;
        break;

    case LED_CH_3:
        pin_num = LED_OUT_3;
        break;

    case LED_CH_4:
        pin_num = LED_OUT_4;
        break;

    case LED_CH_5:
        pin_num = LED_OUT_5;
        break;

    default:
        break;
    }

    return pin_num;
}

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
static void led_demo_board_led_init(void)
{
    led_demo_set_active_polarity(LED_CH_0, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_0,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    led_demo_set_active_polarity(LED_CH_1, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_1,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    led_demo_set_active_polarity(LED_CH_2, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_2,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    led_demo_set_active_polarity(LED_CH_3, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_3,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    led_demo_set_active_polarity(LED_CH_4, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_4,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    led_demo_set_active_polarity(LED_CH_5, LED_ACTIVE_POLARITY_HIGH);
    Pad_Config(LED_OUT_5,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}

static void led_demo_cmd_handle(T_LED_CHANNEL led_ch, bool state)
{
    uint8_t led_idx = log2_u32(led_ch);
    uint32_t s;

    if (led_driver_by_sw)
    {
        //LED should be handled simultaneously
        //Avoid APP be preempted by higher priority task
        s = os_lock();

        if (state)
        {
            //set pad and start timer
            led_demo_blink_handle(led_ch);
        }
        else
        {
            //reset pad and stop timer
            led_demo_deinit(led_ch);
            led_demo_clear_para(led_ch);
            app_stop_timer(&timer_idx_led_blink[led_idx]);
        }

        os_unlock(s);
    }
}

static void led_demo_deinit(T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);
    uint8_t pin_num = led_demo_get_pin_num(led_ch);

    if (led_driver_by_sw)
    {
        if (led_setting_record[led_idx].type == LED_TYPE_BREATH)
        {
            pwm_pin_config(pwm_handle[led_idx], pin_num, PWM_FUNC);
        }
        else
        {
            //reset pad
            Pad_Config(pin_num,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, (PAD_OUTPUT_VAL)led_polarity[led_idx]);
        }
    }
}

static void led_demo_config(T_LED_CHANNEL led_ch, T_LED_TABLE *table)
{
    uint8_t led_idx = log2_u32(led_ch);

    if (led_driver_by_sw)
    {
        led_setting_record[led_idx].type = table->type;// new set type
        led_setting_record[led_idx].on_time = table->on_time;
        led_setting_record[led_idx].off_time = table->off_time;
        led_setting_record[led_idx].blink_count = 2 * table->blink_count;
        led_setting_record[led_idx].blink_interval = table->blink_interval;

        led_count_record[led_idx].blink_count = 2 * table->blink_count;
        led_demo_deinit(led_ch);
    }
}

static void led_demo_create_timer(uint32_t time, uint16_t led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);

    led_create_timer_point[led_idx] = os_sys_time_get();
    led_create_timer_record[led_idx] = time;

    app_start_timer(&timer_idx_led_blink[led_idx], led_timer_name[led_idx],
                    led_module_timer_id, 0, led_ch, false,
                    time);
}

static void led_demo_blink_handle(T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);
    uint8_t pin_num = led_demo_get_pin_num(led_ch);

    if (pin_num == 0xFF)
    {
        return;
    }

    if ((led_setting_record[led_idx].type == LED_TYPE_KEEP_ON) ||
        (led_setting_record[led_idx].type == LED_TYPE_ON_OFF))
    {
        if (led_polarity[led_idx])
        {
            Pad_OutputControlValue(pin_num, PAD_OUT_LOW);
        }
        else
        {
            Pad_OutputControlValue(pin_num, PAD_OUT_HIGH);
        }
    }
    else
    {
        led_demo_deinit(led_ch);
    }

    if ((led_setting_record[led_idx].type == LED_TYPE_ON_OFF) ||
        (led_setting_record[led_idx].type == LED_TYPE_OFF_ON))
    {
        led_demo_create_timer(led_setting_record[led_idx].on_time * 10, led_ch);
    }
    else if (led_setting_record[led_idx].type == LED_TYPE_BREATH)
    {
        led_demo_breath_init(led_idx);
        led_demo_breath_timer_control(LED_BREATH_TIMER, ENABLE);
    }
}

static void led_demo_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    T_LED_CHANNEL led_ch = (T_LED_CHANNEL)param;
    uint8_t led_idx = log2_u32(led_ch);
    uint8_t pin_num = led_demo_get_pin_num(led_ch);

    if ((os_sys_time_get() - led_create_timer_point[led_idx]) >= led_create_timer_record[led_idx])
    {
        //restart repeat blink when blink_interval is not zero
        if (led_setting_record[led_idx].blink_count == 0)
        {
            led_demo_deinit(led_ch);
            led_setting_record[led_idx].blink_count = led_count_record[led_idx].blink_count;
            led_demo_cmd_handle(led_ch, ENABLE);
        }
        else if (led_setting_record[led_idx].blink_count != 1)
        {
            led_setting_record[led_idx].blink_count--;
            if (Pad_GetOutputCtrl(pin_num) == PAD_OUT_LOW)
            {
                Pad_OutputControlValue(pin_num, PAD_OUT_HIGH);
            }
            else
            {
                Pad_OutputControlValue(pin_num, PAD_OUT_LOW);
            }

            if (led_setting_record[led_idx].blink_count % 2 == 1) //start off timer when blink_cnt is odd
            {
                led_demo_create_timer(led_setting_record[led_idx].off_time * 10, led_ch);
            }
            else
            {
                led_demo_create_timer(led_setting_record[led_idx].on_time * 10, led_ch);
            }
        }
        else
        {
            if (led_setting_record[led_idx].blink_interval != 0) //blink_interval is not zero
            {
                led_demo_deinit(led_ch);
                led_setting_record[led_idx].blink_count = 0;
                led_demo_create_timer(led_setting_record[led_idx].blink_interval * 100, led_ch);
            }
            else
            {
                led_demo_deinit(led_ch);
                led_setting_record[led_idx].blink_count = led_count_record[led_idx].blink_count;
                led_demo_cmd_handle(led_ch, ENABLE);
            }
        }
    }
}

static void led_demo_clear_para(T_LED_CHANNEL led_ch)
{
    uint8_t led_idx = log2_u32(led_ch);

    memset(&led_setting_record[led_idx], 0, sizeof(T_LED_TABLE));
}

static void led_demo_set_driver_mode(void)
{
    led_driver_by_sw = true;
    app_timer_reg_cb(led_demo_timeout_cb, &led_module_timer_id);
}

/*
**
  * @brief  enable or disable led breath imer.
    * @param  TIM_TypeDef *index TIM_Type.
    * @param  FunctionalState NewState.
  * @return  void
  */
void led_demo_breath_timer_control(uint8_t i, FunctionalState new_state)
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

void led_demo_breath_timer_handler(void)
{
    uint8_t i = 0;
    uint8_t led_num = 0;
    uint32_t high_count = 0;
    uint32_t low_count = 0;

    for (i = 0; i < LED_DEMO_NUM; i++)
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

void led_demo_breath_init(uint8_t i)
{
    if (pwm_handle[i] == NULL)
    {
        pwm_handle[i] = pwm_create("ext_led", PWM_OUT_COUNT, PWM_OUT_COUNT, false);
        if (pwm_handle[i] == NULL)
        {
            APP_PRINT_ERROR0("Could not create extend led pwm!!!");
            return;
        }

        sw_breathe_mode[i].pwm_id = hw_timer_get_id(pwm_handle[i]);
        pwm_register_timeout_callback(pwm_handle[i], led_demo_breath_timer_handler);
    }
}

void led_demo_blink(void)
{
    uint8_t i;

    for (i = 0; i < LED_DEMO_NUM; i++)
    {
        led_demo_config((T_LED_CHANNEL)BIT(i), &led_setting[i]);
        led_demo_cmd_handle((T_LED_CHANNEL)BIT(i), true);
    }
}

static void led_demo_load_table(uint8_t i)
{
    memcpy((void *)&led_setting,
           (const void *)&LED_TABLE[i], LED_DEMO_NUM * sizeof(T_LED_TABLE));
}

/**
  * @brief  demo code of operation about LED breathe mode.
  * @param   No parameter.
  * @return  void
  */
void led_demo_init(void)
{
    led_demo_set_driver_mode();
    led_demo_board_led_init();

    //index 0-->normal
    //index 1-->breath
    led_demo_load_table(1);
}

