/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "os_mem.h"
#include "trace.h"
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "vector_table.h"
#include "section.h"
#include "mfb_api.h"
#include "app_key_gpio.h"
#include "app_io_msg.h"
#include "app_cfg.h"
#include "app_dlps.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_sensor.h"

#define GPIO_DETECT_DEBOUNCE_TIME   10

static T_KEY_GPIO_DATA *p_gpio_data = NULL;
uint8_t key_gpio_timer_id = 0;
uint8_t timer_idx_key0_debounce = 0;
uint8_t mfb_state = 1;
uint8_t key0_state = 1;

typedef enum
{
    APP_TIMER_KEY0_DEBOUNCE,
} T_APP_KEY_GPIO_TIMER;

bool key_gpio_get_press_state(uint8_t key_index)
{
    return (p_gpio_data->key_press[key_index] == KEY_PRESS);
}

void key_gpio_key0_debounce_start(void)
{
    app_start_timer(&timer_idx_key0_debounce, "key0_debounce",
                    key_gpio_timer_id, APP_TIMER_KEY0_DEBOUNCE, 0, false,
                    GPIO_DETECT_DEBOUNCE_TIME);
}

ISR_TEXT_SECTION
static void key_gpio_mfb_level_change_cb(void)
{
#if F_APP_SENSOR_SL7A20_SUPPORT
    if (app_cfg_const.gsensor_support && app_cfg_const.enable_mfb_pin_as_gsensor_interrupt_pin)
    {
        // do nothing
    }
    else
#endif
    {
        app_db.wake_up_reason |= WAKE_UP_MFB;
    }

    key_gpio_mfb_intr_handler();
}

void key_gpio_mfb_init(void)
{
    mfb_init((P_MFB_LEVEL_CHANGE_CBACK)key_gpio_mfb_level_change_cb);
    Pad_WakeUpCmd(MFB_MODE, POL_LOW, ENABLE);
}

#if 0
ISR_TEXT_SECTION
static void key_gpio_key0_intr_cb(uint32_t gpio_num)
{
    key_gpio_key0_intr_handler();
}
#endif

static void key_gpio_key0_debounce_handler(void)
{
    T_IO_MSG button_msg;
    uint8_t key_status;
    uint8_t key_status_update_fg = 0;

    key_status = p_gpio_data->key_status[0];

#if 0
    if (app_cfg_const.key_enable_mask & KEY0_MASK)
    {
        if (!app_cfg_const.mfb_replace_key0)
        {
            key_status = hal_gpio_get_input_level(app_cfg_const.key_pinmux[0]);

            if (key_status == key0_state)
            {
                /* Enable GPIO interrupt */
                APP_PRINT_ERROR1("key_gpio_key0_debounce_handler: ignore for same level, key0_state 0x%x",
                                 key0_state);
                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                return;
            }

            key0_state = key_status;
        }
    }
    else
#endif
    {
        key_status = mfb_get_level();

        if (key_status == mfb_state)
        {
            APP_PRINT_ERROR1("key_gpio_key0_debounce_handler: ignore for same level, mfb_state 0x%x",
                             mfb_state);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
            return;
        }

        mfb_state = key_status;
    }

    button_msg.type = IO_MSG_TYPE_GPIO;
    button_msg.subtype = IO_MSG_GPIO_KEY;

    if (app_cfg_const.key_high_active_mask & BIT0)
    {
        if (key_status) //Button pressed
        {
            if (!app_cfg_const.mfb_replace_key0)
            {
                hal_gpio_irq_change_polarity(app_cfg_const.key_pinmux[0], GPIO_IRQ_ACTIVE_LOW);
            }

            if (p_gpio_data->key_press[0] == KEY_RELEASE)
            {
                button_msg.u.param = (KEY0_MASK << 8) | KEY_PRESS;
                key_status_update_fg = 1;
                p_gpio_data->key_press[0] = KEY_PRESS;
            }
        }
        else //Button released
        {
            /* Change GPIO Interrupt Polarity, Enable Interrupt */
            if (!app_cfg_const.mfb_replace_key0)
            {
                hal_gpio_irq_change_polarity(app_cfg_const.key_pinmux[0], GPIO_IRQ_ACTIVE_HIGH);
            }

            if (p_gpio_data->key_press[0] == KEY_PRESS)
            {
                button_msg.u.param = (KEY0_MASK << 8) | KEY_RELEASE;
                key_status_update_fg = 1;
                p_gpio_data->key_press[0] = KEY_RELEASE;
            }
        }
    }
    else
    {
        if (key_status) //Button released
        {
            /* Change GPIO Interrupt Polarity, Enable Interrupt */
            if (!app_cfg_const.mfb_replace_key0)
            {
                hal_gpio_irq_change_polarity(app_cfg_const.key_pinmux[0], GPIO_IRQ_ACTIVE_LOW);
            }

            if (p_gpio_data->key_press[0] == KEY_PRESS)
            {
                button_msg.u.param = (KEY0_MASK << 8) | KEY_RELEASE;
                key_status_update_fg = 1;
                p_gpio_data->key_press[0] = KEY_RELEASE;
            }
        }
        else //Button pressed
        {
            if (!app_cfg_const.mfb_replace_key0)
            {
                hal_gpio_irq_change_polarity(app_cfg_const.key_pinmux[0], GPIO_IRQ_ACTIVE_HIGH);
            }

            if (p_gpio_data->key_press[0] == KEY_RELEASE)
            {
                button_msg.u.param = (KEY0_MASK << 8) | KEY_PRESS;
                key_status_update_fg = 1;
                p_gpio_data->key_press[0] = KEY_PRESS;
            }
        }
    }

    /* Send MSG to APP task */
    if (key_status_update_fg)
    {
        app_io_msg_send(&button_msg);
    }
    else
    {
        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
    }

#if 0
    if (!app_cfg_const.mfb_replace_key0)
    {
        if (app_cfg_const.key_enable_mask & KEY0_MASK)
        {
            /* Enable GPIO interrupt */
            hal_gpio_irq_enable(app_cfg_const.key_pinmux[0]);
        }
    }
#endif
}

static void key_gpio_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_KEY0_DEBOUNCE:
        {
            app_stop_timer(&timer_idx_key0_debounce);

#if (F_APP_SENSOR_SL7A20_SUPPORT == 1)
            if (app_cfg_const.gsensor_support && app_cfg_const.enable_mfb_pin_as_gsensor_interrupt_pin)
            {
                app_sensor_sl_mfb_intr_handler();
            }
            else
#endif
            {
                key_gpio_key0_debounce_handler();
            }
        }
        break;

    default:
        break;
    }
}

void key_gpio_initial(void)
{
    uint32_t i;
    T_GPIO_IRQ_POLARITY key0_irq_ploarity = GPIO_IRQ_ACTIVE_LOW;
    T_GPIO_PULL_VALUE pull_mode;

    if (MAX_KEY_NUM != 0)
    {
        if (p_gpio_data == NULL)
        {
            p_gpio_data = (T_KEY_GPIO_DATA *)calloc(1, sizeof(T_KEY_GPIO_DATA));
        }
        else
        {
            memset(p_gpio_data, 0, sizeof(T_KEY_GPIO_DATA));// set p_gpio_data->key_press[i] = KEY_RELEASE;
        }
        app_timer_reg_cb(key_gpio_timeout_cb, &key_gpio_timer_id);

        //Key 0 is used at POWER_POWERDOWN_MODE wake up. Need to set level trigger
        if (app_cfg_const.key_enable_mask & KEY0_MASK)
        {
            key0_state = ~(app_cfg_const.key_high_active_mask & BIT0);
            pull_mode = (app_cfg_const.key_high_active_mask & BIT(0)) ? GPIO_PULL_DOWN : GPIO_PULL_UP;
            hal_gpio_init_pin(app_cfg_const.key_pinmux[0], GPIO_TYPE_CORE, GPIO_DIR_INPUT, pull_mode);

            if (app_cfg_const.key_high_active_mask & BIT0)
            {
                key0_irq_ploarity = GPIO_IRQ_ACTIVE_HIGH;
            }

            hal_gpio_set_up_irq(app_cfg_const.key_pinmux[0], GPIO_IRQ_LEVEL, key0_irq_ploarity, false);
        }

        /*key0 key 2  key4  key6  make as high edge trigger*/
        for (i = 1; i < MAX_KEY_NUM; i++)
        {
            if ((app_cfg_const.key_enable_mask & BIT(i))
                && (app_cfg_const.key_high_active_mask & BIT(i)))
            {
                hal_gpio_init_pin(app_cfg_const.key_pinmux[i], GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_DOWN);
                hal_gpio_set_up_irq(app_cfg_const.key_pinmux[i], GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_HIGH, true);
            }
        }

        for (i = 1; i < MAX_KEY_NUM; i++)
        {
            if ((app_cfg_const.key_enable_mask & BIT(i))
                && ((app_cfg_const.key_high_active_mask & BIT(i)) == 0))
            {
                hal_gpio_init_pin(app_cfg_const.key_pinmux[i], GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
                hal_gpio_set_up_irq(app_cfg_const.key_pinmux[i], GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);
            }
        }
        /* Enable Interrupt (Peripheral, CPU NVIC) */
        for (i = 0; i < MAX_KEY_NUM; i++)
        {
            uint8_t gpio_num;

            if (app_cfg_const.key_enable_mask & BIT(i))
            {
                if (app_cfg_const.key_high_active_mask & BIT(i))
                {
                    Pad_WakeupPolarityValue(app_cfg_const.key_pinmux[i], PAD_WAKEUP_POL_HIGH);
                }
                else
                {
                    Pad_WakeupPolarityValue(app_cfg_const.key_pinmux[i], PAD_WAKEUP_POL_LOW);
                }

                if (i == 0)
                {
                    //hal_gpio_register_isr_callback(app_cfg_const.key_pinmux[0], key_gpio_key0_intr_cb, i);
                }
                else
                {
                    hal_gpio_register_isr_callback(app_cfg_const.key_pinmux[i], key_gpio_intr_cb, i);
                }
                hal_gpio_irq_enable(app_cfg_const.key_pinmux[i]);
            }
        }
    }
}

ISR_TEXT_SECTION
void key_gpio_intr_cb(uint32_t key_index)
{
    uint8_t key_mask = BIT(key_index);
    uint8_t gpio_index = app_cfg_const.key_pinmux[key_index];
    uint8_t key_status;
    uint8_t key_status_update_fg = 0;
    T_IO_MSG button_msg;

    /* Control of entering DLPS */
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
    app_dlps_pad_wake_up_polarity_invert(gpio_index);

    key_status = hal_gpio_get_input_level(gpio_index);

    APP_PRINT_TRACE3("key_gpio_intr_cb, key_mask: 0x%02X, gpio_index: %d, key_status: %d", key_mask,
                     gpio_index, key_status);

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(gpio_index);
    button_msg.type = IO_MSG_TYPE_GPIO;
    button_msg.subtype = IO_MSG_GPIO_KEY;

    if (key_mask & app_cfg_const.key_high_active_mask) // high active
    {
        if (key_status) //Button pressed
        {
            hal_gpio_irq_change_polarity(gpio_index, GPIO_IRQ_ACTIVE_LOW);
            if (p_gpio_data->key_press[key_index] == KEY_RELEASE)
            {
                button_msg.u.param = (key_mask << 8) | KEY_PRESS;
                key_status_update_fg = 1;
                p_gpio_data->key_press[key_index] = KEY_PRESS;
            }
        }
        else //Button released
        {
            /* Change GPIO Interrupt Polarity, Enable Interrupt */
            hal_gpio_irq_change_polarity(gpio_index, GPIO_IRQ_ACTIVE_HIGH);
            if (p_gpio_data->key_press[key_index] == KEY_PRESS)
            {
                button_msg.u.param = (key_mask << 8) | KEY_RELEASE;
                key_status_update_fg = 1;
                p_gpio_data->key_press[key_index] = KEY_RELEASE;
            }
        }
    }
    else // low active
    {
        if (key_status) //Button released
        {
            /* Change GPIO Interrupt Polarity, Enable Interrupt */
            hal_gpio_irq_change_polarity(gpio_index, GPIO_IRQ_ACTIVE_LOW);
            if (p_gpio_data->key_press[key_index] == KEY_PRESS)
            {
                button_msg.u.param = (key_mask << 8) | KEY_RELEASE;
                key_status_update_fg = 1;
                p_gpio_data->key_press[key_index] = KEY_RELEASE;
            }
        }
        else //Button pressed
        {
            hal_gpio_irq_change_polarity(gpio_index, GPIO_IRQ_ACTIVE_HIGH);
            if (p_gpio_data->key_press[key_index] == KEY_RELEASE)
            {
                button_msg.u.param = (key_mask << 8) | KEY_PRESS;
                key_status_update_fg = 1;
                p_gpio_data->key_press[key_index] = KEY_PRESS;
            }
        }
    }

    /* Send MSG to APP task */
    if (key_status_update_fg)
    {
        app_io_msg_send(&button_msg);
    }
    else
    {
        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
    }

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(gpio_index);
}

static void key_gpio_send_key0_msg(void)
{
    T_IO_MSG button_msg;

    button_msg.type = IO_MSG_TYPE_GPIO;
    button_msg.subtype = IO_MSG_GPIO_KEY0;
    app_io_msg_send(&button_msg);
}

#if 0
ISR_TEXT_SECTION
void key_gpio_key0_intr_handler(void)
{
    APP_PRINT_TRACE0("key_gpio_key0_intr_handler");
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

    p_gpio_data->key_status[0] = hal_gpio_get_input_level(app_cfg_const.key_pinmux[0]);
    app_dlps_pad_wake_up_polarity_invert(app_cfg_const.key_pinmux[0]);

    key_gpio_send_key0_msg();
}
#endif

ISR_TEXT_SECTION
void key_gpio_mfb_intr_handler(void)
{
    APP_PRINT_TRACE1("key_gpio_mfb_intr_handler, MFB level: %d", mfb_get_level());

#if F_APP_SENSOR_SL7A20_SUPPORT
    if (app_cfg_const.gsensor_support && app_cfg_const.enable_mfb_pin_as_gsensor_interrupt_pin)
    {
        app_dlps_disable(APP_DLPS_ENTER_CHECK_MFB_KEY);
    }
    else
#endif
    {
        app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
    }

    p_gpio_data->key_status[0] = mfb_get_level();

    key_gpio_send_key0_msg();
}

bool key_gpio_check_power_button_pressed(void)
{
    if (app_cfg_const.mfb_replace_key0)
    {
        return !((bool)mfb_get_level());
    }
    else
    {
        return !((bool)(app_cfg_const.key_high_active_mask & BIT0) ^ ((bool)hal_gpio_get_input_level(
                                                                          app_cfg_const.key_pinmux[0])));
    }
}

