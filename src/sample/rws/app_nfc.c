

/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#if F_APP_NFC_SUPPORT
#include "trace.h"
#include "rtl876x_pinmux.h"
#include "wdg.h"
#include "app_timer.h"
#include "app_mmi.h"
#include "sysm.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "app_cfg.h"
#include "app_io_msg.h"
#include "app_dlps.h"
#include "app_nfc.h"
#include "app_audio_policy.h"
#if F_APP_BUZZER_SUPPORT
#include "app_buzzer.h"
#endif
#include "section.h"

/**NFC timer unit, unit : ms*/
#define NFC_STABLE_TIMER_UNIT_MS    100
#define NFC_TRIGGER_TIMER_UNIT_MS   30 * 1000

static uint8_t timer_idx_nfc_stable = 0;
static uint8_t timer_idx_nfc_multi_link_switch = 0;
static uint8_t app_nfc_timer_id = 0;
T_NFC_DATA nfc_data;                       /**<record nfc variable */


typedef enum
{
    APP_TIMER_NFC_STABLE,
    APP_TIMER_NFC_MULTI_LINK_SWITCH,
} T_APP_NFC_TIMER;

static void app_nfc_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_nfc_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_NFC_STABLE:
        {
            if (nfc_data.nfc_detected == 1)
            {
                nfc_data.nfc_stable_count++;
                if (nfc_data.nfc_stable_count > app_cfg_const.nfc_stable_interval)
                {
                    app_stop_timer(&timer_idx_nfc_stable);
                    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
#if F_APP_BUZZER_SUPPORT
                    buzzer_set_mode(ALARM_BUZZER);
#endif
                    app_audio_tone_type_play(TONE_ALARM, false, false);
                    app_mmi_handle_action(MMI_DEV_NFC_DETECT);
                    nfc_data.nfc_stable_count = 0;
                }
            }
            else
            {
                app_stop_timer(&timer_idx_nfc_stable);
                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
                nfc_data.nfc_stable_count = 0;
            }
        }
        break;

    case APP_TIMER_NFC_MULTI_LINK_SWITCH:
        {
            app_stop_timer(&timer_idx_nfc_multi_link_switch);
            nfc_data.nfc_multi_link_switch = 0;
        }
        break;

    default:
        break;
    }
}

/*============================================================================*
 *                              Public Functions
 *============================================================================*/
void app_nfc_handle_msg(T_IO_MSG *io_driver_msg_recv)
{
    nfc_data.nfc_detected = io_driver_msg_recv->u.param;
    app_start_timer(&timer_idx_nfc_stable, "nfc_stable",
                    app_nfc_timer_id, APP_TIMER_NFC_STABLE, 0, true,
                    NFC_STABLE_TIMER_UNIT_MS);
}

void app_nfc_multi_link_switch_trigger(void)
{
    nfc_data.nfc_multi_link_switch |= (NFC_MULTI_LINK_SWITCH_A2DP | NFC_MULTI_LINK_SWITCH_HF);
    app_start_timer(&timer_idx_nfc_multi_link_switch, "nfc_multi_link_switch",
                    app_nfc_timer_id, APP_TIMER_NFC_MULTI_LINK_SWITCH, 0, false,
                    NFC_TRIGGER_TIMER_UNIT_MS);
}

/**
    * @brief  NFC GPIO interrupt will be handle in this function.
    *         First disable app enter dlps mode and read current NFC GPIO input data bit.
    *         Disable NFC GPIO interrupt and send IO_MSG_TYPE_GPIO message to app task.
    *         Then enable NFC GPIO interrupt.
    * @param  void
    * @return void
    */
ISR_TEXT_SECTION
void app_nfc_intr_cb(uint32_t param)
{
    uint8_t gpio_status;
    T_IO_MSG gpio_msg;

    /* Control of entering DLPS */
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
    gpio_status = hal_gpio_get_input_level(app_cfg_const.nfc_pinmux);

    /* Disable GPIO interrupt */
    hal_gpio_irq_disable(app_cfg_const.nfc_pinmux);
    /* Change GPIO Interrupt Polarity */
    if (gpio_status) //GPIO detected and Polarity low
    {
        hal_gpio_irq_change_polarity(app_cfg_const.nfc_pinmux, GPIO_IRQ_ACTIVE_LOW);
        gpio_msg.u.param = 1;
    }
    else //GPIO released and Polarity High
    {
        hal_gpio_irq_change_polarity(app_cfg_const.nfc_pinmux, GPIO_IRQ_ACTIVE_HIGH);
        gpio_msg.u.param = 0;
    }

    gpio_msg.type = IO_MSG_TYPE_GPIO;
    gpio_msg.subtype = IO_MSG_GPIO_NFC;

    app_io_msg_send(&gpio_msg);

    /* Enable GPIO interrupt */
    hal_gpio_irq_enable(app_cfg_const.nfc_pinmux);
}

/**
    * @brief  NFC GPIO initial.
    *         Include APB peripheral clock config, NFC GPIO parameter config and
    *         NFC GPIO interrupt mark config. Enable NFC GPIO interrupt.
    * @param  void
    * @return void
    */
void app_nfc_init(void)
{
    /*gpio init*/
    hal_gpio_init_pin(app_cfg_const.nfc_pinmux, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_DOWN);
    hal_gpio_set_up_irq(app_cfg_const.nfc_pinmux, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);
    hal_gpio_register_isr_callback(app_cfg_const.nfc_pinmux, app_nfc_intr_cb, 0);
    hal_gpio_irq_enable(app_cfg_const.nfc_pinmux);
    /*register gpio nfc timer callback queue,device manager callback*/
    app_timer_reg_cb(app_nfc_timeout_cb, &app_nfc_timer_id);
}

#endif
