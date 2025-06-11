/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_GPIO_ONOFF_SUPPORT
#include "trace.h"
#include "app_gpio_on_off.h"
#include "rtl876x_pinmux.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "wdg.h"
#include "ftl.h"
#include "pm.h"
#include "sysm.h"
#include "remote.h"
#include "ringtone.h"
#include "app_mmi.h"
#include "board.h"
#include "app_cfg.h"
#include "app_io_msg.h"
#include "app_loc_mgr.h"
#include "app_main.h"
#include "app_dlps.h"
#include "section.h"
#include "app_in_out_box.h"
#include "app_auto_power_off.h"
#include "app_timer.h"

static uint8_t timer_idx_gpio_detect_debounce = 0;
static uint8_t app_gpio_on_off_timer_id = 0;
static uint8_t app_gpio_on_off_pin_num = 0xFF;

typedef enum
{
    APP_TIMER_GPIO_DETECT_DEBOUNCE,
} T_APP_GPIO_ON_OFF_DETECT_TIMER;

static void app_gpio_on_off_special_pin_handle(FunctionalState state)
{
    if (app_cfg_const.key_gpio_support)
    {
        if (app_cfg_const.key_enable_mask & KEY0_MASK)
        {
            if (state)
            {
                hal_gpio_irq_enable(app_cfg_const.key_pinmux[0]);
            }
            else
            {
                hal_gpio_irq_disable(app_cfg_const.key_pinmux[0]);
            }
        }
    }
    if (state)
    {
        app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
    }
}

static void app_gpio_on_off_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_GPIO_DETECT_DEBOUNCE:
        {
            T_CASE_LOCATION_STATUS local;

            app_stop_timer(&timer_idx_gpio_detect_debounce);

            local = app_gpio_on_off_get_location();

            app_db.local_in_case = (local == IN_CASE) ? true : false;

            {
                T_IO_MSG adp_msg;
                bool enable_handle = false;
                APP_PRINT_TRACE1("app_gpio_on_off_timeout_cb: bud loc detect %d", local);

                adp_msg.type = IO_MSG_TYPE_GPIO;
                adp_msg.subtype = IO_MSG_GPIO_BUD_LOC_CHANGE;

                app_gpio_on_off_special_pin_handle(ENABLE);

                switch (local)
                {
                case IN_CASE:
                    {
                        if (app_cfg_nv.pre_3pin_status_unplug)
                        {
                            adp_msg.u.param = ((CAUSE_ACTION_IO << 8) | EVENT_IN_CASE);
                            enable_handle = true;
                            app_cfg_nv.pre_3pin_status_unplug = 0;
                        }
                        else
                        {
                            //do nothing
                        }
                    }
                    break;

                case OUT_CASE:
                    {
                        if (!app_cfg_nv.pre_3pin_status_unplug)
                        {
                            adp_msg.u.param = ((CAUSE_ACTION_IO << 8) | EVENT_OUT_CASE);
                            enable_handle = true;
                            app_cfg_nv.pre_3pin_status_unplug = 1;

                            //Should be able to power on during power off ing after debounce timeout
                            //Keep pre_3pin_status_unplug to correct condition
                            if (app_db.device_state == APP_DEVICE_STATE_OFF_ING)
                            {
                                app_cfg_nv.pre_3pin_status_unplug = 0;
                            }

                            if (app_cfg_const.smart_charger_control && app_db.disallow_adp_out_auto_power_on)
                            {
                                //disallow adp out power on feature when receive box zero volume
                                APP_PRINT_TRACE0("ignore 3pin out when get box zero volume");
                                enable_handle = false;
                                if (!app_cfg_nv.adp_factory_reset_power_on)
                                {
                                    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                                }
                            }
                        }
                        else
                        {
                            //do nothing
                        }
                    }
                    break;
                }

                if (enable_handle)
                {
                    app_io_msg_send(&adp_msg);
                }
            }
        }
        break;

    default:
        break;
    }
}

ISR_TEXT_SECTION T_CASE_LOCATION_STATUS app_gpio_on_off_get_location(void)
{
    T_CASE_LOCATION_STATUS location = IN_CASE;

    if (hal_gpio_get_input_level(app_gpio_on_off_pin_num) == 1)
    {
        location = OUT_CASE;
    }
    return location;
}

/**
    * @brief  change pad wake up polartiy and enbale gpio on off pad wakup
    * @param  none
    * @return void
    */
static ISR_TEXT_SECTION void app_gpio_on_off_set_pad_wkup_polarity(void)
{
    /*GPIO detect pin - High: power on, Low: power off*/
    PAD_WAKEUP_POL_VAL wake_up_val;

    wake_up_val = (hal_gpio_get_input_level(app_gpio_on_off_pin_num)) ? PAD_WAKEUP_POL_LOW :
                  PAD_WAKEUP_POL_HIGH;
    Pad_WakeupPolarityValue(app_gpio_on_off_pin_num, wake_up_val);

    APP_PRINT_INFO2("app_gpio_on_off_set_pad_wkup_polarity SET detct_pin = %d wake_up_val =%d",
                    app_gpio_on_off_pin_num, wake_up_val);
}

ISR_TEXT_SECTION void app_gpio_on_off_intr_cb(uint32_t param)
{
    T_IO_MSG gpio_msg;

    APP_PRINT_TRACE1("app_gpio_on_off_intr_cb pin_level=%d",
                     hal_gpio_get_input_level(app_gpio_on_off_pin_num));

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(app_gpio_on_off_pin_num);


    /* Change GPIO Interrupt Polarity */
    if (hal_gpio_get_input_level(app_gpio_on_off_pin_num))
    {
        app_gpio_on_off_special_pin_handle(DISABLE);
        hal_gpio_irq_change_polarity(app_gpio_on_off_pin_num, GPIO_IRQ_ACTIVE_LOW);//Polarity Low
        gpio_msg.u.param = 1;
    }
    else
    {

        hal_gpio_irq_change_polarity(app_gpio_on_off_pin_num, GPIO_IRQ_ACTIVE_HIGH);//Polarity High
        gpio_msg.u.param = 0;
    }

    /*add  pad status check update for miss gpio interrupt when enter dlps*/
    if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        app_gpio_on_off_set_pad_wkup_polarity();
    }

    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_CHARGERBOX_DETECT;

    app_io_msg_send(&gpio_msg);

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(app_gpio_on_off_pin_num);
}

void app_gpio_on_off_handle_msg(T_IO_MSG *io_driver_msg_recv)
{
    uint32_t in_out_box_detect_time = 0;
    app_stop_timer(&timer_idx_gpio_detect_debounce);

    if (app_cfg_const.enable_rtk_charging_box)
    {
        if (io_driver_msg_recv->u.param && app_db.local_in_case) //detect out
        {
            in_out_box_detect_time = SMART_CHARGERBOX_DETECT_OUT_TIMER;
        }
        else if (!io_driver_msg_recv->u.param && (!app_db.local_in_case)) //detect in
        {
            if (app_cfg_const.smart_charger_box_bit_length == 0) // 1: 20ms, 0 40ms
            {
                in_out_box_detect_time = SMART_CHARGERBOX_DETECT_IN_TIMER_40MS;
            }
            else
            {
                in_out_box_detect_time = SMART_CHARGERBOX_DETECT_IN_TIMER_20MS;
            }
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        if (io_driver_msg_recv->u.param)
        {
            if (app_cfg_const.enable_outbox_power_on)
            {
                app_auto_power_off_enable(AUTO_POWER_OFF_MASK_IN_BOX, app_cfg_const.timer_auto_power_off);
            }
        }
        else
        {
            if (app_cfg_const.enable_inbox_power_off) //for smart box, plug not to power off
            {
                app_auto_power_off_disable(AUTO_POWER_OFF_MASK_IN_BOX);
            }
        }

        in_out_box_detect_time = CHARGERBOX_DETECT_TIMER;
    }

    if (in_out_box_detect_time != 0)
    {
        // Disallow DLPS when debouncing.
        app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
        app_start_timer(&timer_idx_gpio_detect_debounce, "app_gpio_detect_debounce",
                        app_gpio_on_off_timer_id, APP_TIMER_GPIO_DETECT_DEBOUNCE, 0, false,
                        in_out_box_detect_time);
    }
}

bool app_gpio_on_off_debounce_timer_started(void)
{
    return ((timer_idx_gpio_detect_debounce != 0) ? true : false);
}

static bool app_gpio_on_off_dlps_check_cb(void)
{
    return (timer_idx_gpio_detect_debounce == 0) ? true : false;
}

void app_gpio_on_off_driver_init(uint8_t pin_num)
{
    /*gpio init*/
    app_gpio_on_off_pin_num = pin_num;
    hal_gpio_init_pin(pin_num, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    hal_gpio_set_up_irq(pin_num, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);
    hal_gpio_register_isr_callback(pin_num, app_gpio_on_off_intr_cb, 0);
    hal_gpio_irq_enable(pin_num);
    /*register gpio detect timer callback queue,device manager callback*/
    app_timer_reg_cb(app_gpio_on_off_timeout_cb, &app_gpio_on_off_timer_id);

    power_check_cb_register(app_gpio_on_off_dlps_check_cb);

    app_gpio_on_off_set_pad_wkup_polarity();
}

void  app_gpio_on_off_enter_dlps(void)
{
    app_dlps_pad_wake_up_enable(app_gpio_on_off_pin_num);
}

void app_gpio_on_off_exit_dlps(void)
{
    app_dlps_restore_pad(app_gpio_on_off_pin_num);
}
#endif
