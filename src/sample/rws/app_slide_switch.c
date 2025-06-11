#if (F_APP_SLIDE_SWITCH_SUPPORT == 1)
#include "trace.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "hal_gpio_int.h"
#include "app_io_msg.h"
#include "app_timer.h"
#include "sysm.h"
#include "hal_gpio.h"
#include "app_cfg.h"
#include "app_dlps.h"
#include "app_mmi.h"
#include "app_slide_switch.h"
#include "section.h"
#include "app_main.h"
#include "app_adp.h"
#include "single_tone.h"
#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_dual_mode.h"
#endif
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif
#include "app_roleswap.h"

typedef enum
{
    APP_SLIDE_SWITCH_0,
    APP_SLIDE_SWITCH_1,//0x01

    APP_SLIDE_SWITCH_TOTAL
} T_APP_SLIDE_SWITCH_ID;

#define APP_SLIDE_SWITCH_DEBOUCE_TIME     500

#define APP_SLIDE_SWITCH_HIGH_LEVEL 1
#define APP_SLIDE_SWITCH_LOW_LEVEL  0

static uint8_t timer_idx_slide_switch_debounce[APP_SLIDE_SWITCH_TOTAL] = {0};

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
static uint8_t timer_idx_slide_switch_power_on_cycle_check = 0;
#endif

static uint8_t app_slide_switch_timer_id = 0xFF;

#if (F_APP_SLIDE_SWITCH_LISTENING_MODE == 1)
static T_APP_SLIDE_SWITCH_ANC_APT_STATE slide_switch_anc_apt_state = APP_SLIDE_SWITCH_ALL_OFF;
#endif

typedef enum
{
    APP_TIMER_SLIDE_SWITCH_0_LOW_DEBOUNCE  = 0,
    APP_TIMER_SLIDE_SWITCH_0_HIGH_DEBOUNCE = 1,
    APP_TIMER_SLIDE_SWITCH_1_LOW_DEBOUNCE  = 2,
    APP_TIMER_SLIDE_SWITCH_1_HIGH_DEBOUNCE = 3,
    APP_TIMER_SLIDE_SWITCH_POWER_ON_CYCLE_CHECK = 4,
} T_APP_SLIDE_SWITCH_TIMER;

typedef enum
{
    ACTION_NONE,
    ACTION_POWER_ON,                //0x01
    ACTION_POWER_OFF,               //0x02
    ACTION_ANC_ON,                  //0x03
    ACTION_ANC_OFF,                 //0x04
    ACTION_APT_ON,                  //0x05
    ACTION_APT_OFF,                 //0x06
    ACTION_MIC_UNMUTE,              //0x07
    ACTION_MIC_MUTE,                //0x08
    ACTION_DONGLE_DUAL_MODE_2_4G,   //0x09
    ACTION_DONGLE_DUAL_MODE_BT,     //0x0a
} T_SWITCH_ACTION;

typedef struct
{
    uint8_t support;
    T_SWITCH_ACTION low_action;
    T_SWITCH_ACTION high_action;
    uint8_t pinmux;
} T_APP_SLIDE_SWITCH_CFG;

static void app_slide_switch_intr_cb(uint32_t param);

T_APP_SLIDE_SWITCH_CFG slide_switch[APP_SLIDE_SWITCH_TOTAL];

static void app_slide_switch_update_intpolarity(uint8_t pin_num, uint8_t gpio_status)
{
    if (gpio_status)
    {
        hal_gpio_irq_change_polarity(pin_num, GPIO_IRQ_ACTIVE_LOW);//Polarity Low
    }
    else
    {
        hal_gpio_irq_change_polarity(pin_num, GPIO_IRQ_ACTIVE_HIGH);//Polarity HIGH
    }
}

static void app_slide_switch_action_handle(T_SWITCH_ACTION action)
{
    APP_PRINT_INFO1("app_slide_switch_action_handle: action %d", action);
    switch (action)
    {
#if (F_APP_SLIDE_SWITCH_LISTENING_MODE == 1)
    case ACTION_ANC_ON:
        {
            slide_switch_anc_apt_state = APP_SLIDE_SWITCH_ANC_ON;
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                break;
            }
#if F_APP_ANC_SUPPORT
            if (!app_anc_is_anc_on_state(*app_db.final_listening_state))
            {
                app_mmi_handle_action(MMI_ANC_ON_OFF);
            }
#endif
        }
        break;

    case ACTION_ANC_OFF:
        {
            slide_switch_anc_apt_state = APP_SLIDE_SWITCH_ALL_OFF;
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                break;
            }
#if F_APP_ANC_SUPPORT
            if (app_anc_is_anc_on_state(*app_db.final_listening_state))
            {
                app_mmi_handle_action(MMI_ANC_ON_OFF);
            }
#endif
        }
        break;

    case ACTION_APT_ON:
        {
            slide_switch_anc_apt_state = APP_SLIDE_SWITCH_APT_ON;
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                break;
            }
#if F_APP_APT_SUPPORT
            if (!app_apt_is_apt_on_state(*app_db.final_listening_state))
            {
                app_mmi_handle_action(MMI_AUDIO_APT);
            }
#endif
        }
        break;

    case ACTION_APT_OFF:
        {
            slide_switch_anc_apt_state = APP_SLIDE_SWITCH_ALL_OFF;
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                break;
            }
#if F_APP_APT_SUPPORT
            if (app_apt_is_apt_on_state(*app_db.final_listening_state))
            {
                app_mmi_handle_action(MMI_AUDIO_APT);
            }
#endif
        }
        break;
#endif

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
    case ACTION_POWER_ON:
        {
            if (app_cfg_const.discharger_support &&
                (app_charger_get_charge_state() == APP_CHARGER_STATE_NO_CHARGE) &&
                (app_charger_get_soc() == BAT_CAPACITY_0))
            {
                //disallow power on
            }
            else if ((app_cfg_const.charging_disallow_power_on && (app_adp_get_plug_state() == ADAPTOR_PLUG)) ||
                     (app_cfg_const.enable_inbox_power_off && app_device_is_in_the_box())
                     || !app_slide_switch_power_on_valid_check())
            {
                //disallow power on
            }
            else
            {
                app_start_timer(&timer_idx_slide_switch_power_on_cycle_check, "slide_switch_power_on_cycle_check",
                                app_slide_switch_timer_id, APP_TIMER_SLIDE_SWITCH_POWER_ON_CYCLE_CHECK, 0, true,
                                100);
            }

        }
        break;

    case ACTION_POWER_OFF:
        {
            app_db.power_off_cause = POWER_OFF_CAUSE_SLIDE_SWITCH;
            app_cfg_store(&app_cfg_nv.remote_loc, 4);

#if F_APP_ERWS_SUPPORT
            app_roleswap_poweroff_handle(false);
#else
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
#endif
        }
        break;
#endif

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
    case ACTION_MIC_UNMUTE:
        {
            app_mmi_handle_action(MMI_DEV_MIC_UNMUTE);
        }
        break;

    case ACTION_MIC_MUTE:
        {
            app_mmi_handle_action(MMI_DEV_MIC_MUTE);
        }
        break;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    case ACTION_DONGLE_DUAL_MODE_2_4G:
        {
            if (app_cfg_nv.is_dual_mode)
            {
                app_dongle_dual_mode_switch(SWITCH_EVENT_SWITCH_MODE);
            }
        }
        break;

    case ACTION_DONGLE_DUAL_MODE_BT:
        {
            if (app_cfg_nv.is_dual_mode == false)
            {
                app_dongle_dual_mode_switch(SWITCH_EVENT_SWITCH_MODE);
            }
        }
        break;
#endif

    default:
        break;
    }
}

static void app_slide_switch_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    uint8_t gpio_status = APP_SLIDE_SWITCH_LOW_LEVEL;
    T_APP_SLIDE_SWITCH_ID channel = APP_SLIDE_SWITCH_0;

    APP_PRINT_INFO1("app_slide_switch_timeout_cb timer_evt %d ", timer_evt);
    switch (timer_evt)
    {
    case APP_TIMER_SLIDE_SWITCH_0_LOW_DEBOUNCE:
    case APP_TIMER_SLIDE_SWITCH_0_HIGH_DEBOUNCE:
    case APP_TIMER_SLIDE_SWITCH_1_LOW_DEBOUNCE:
    case APP_TIMER_SLIDE_SWITCH_1_HIGH_DEBOUNCE:
        {
            if (timer_evt == APP_TIMER_SLIDE_SWITCH_0_HIGH_DEBOUNCE ||
                timer_evt == APP_TIMER_SLIDE_SWITCH_1_HIGH_DEBOUNCE)
            {
                gpio_status = APP_SLIDE_SWITCH_HIGH_LEVEL;
            }

            if (timer_evt == APP_TIMER_SLIDE_SWITCH_1_LOW_DEBOUNCE ||
                timer_evt == APP_TIMER_SLIDE_SWITCH_1_HIGH_DEBOUNCE)
            {
                channel = APP_SLIDE_SWITCH_1;
            }

            app_stop_timer(&timer_idx_slide_switch_debounce[channel]);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);

            if ((slide_switch[channel].low_action == ACTION_POWER_ON ||
                 slide_switch[channel].high_action == ACTION_POWER_ON) &&
                (app_db.device_state != APP_DEVICE_STATE_OFF_ING))
            {
                app_cfg_nv.slide_switch_power_on_off_gpio_status = gpio_status;
                app_cfg_store(&app_cfg_nv.remote_loc, 4);
            }

            if (gpio_status)
            {
                app_slide_switch_action_handle(slide_switch[channel].high_action);
            }
            else
            {
                app_slide_switch_action_handle(slide_switch[channel].low_action);
            }

            hal_gpio_irq_enable(slide_switch[channel].pinmux);
        }
        break;

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
    case APP_TIMER_SLIDE_SWITCH_POWER_ON_CYCLE_CHECK:
        {
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                /* dut mode, no bt_ready msg, do power on mmi action immediately*/
                if ((app_db.bt_is_ready || mp_hci_test_mode_is_running()) &&
                    app_db.device_state == APP_DEVICE_STATE_OFF)
                {
                    app_stop_timer(&timer_idx_slide_switch_power_on_cycle_check);
                    app_mmi_handle_action(MMI_DEV_POWER_ON);
                    app_device_unlock_vbat_disallow_power_on();
                }
            }
            else
            {
                app_stop_timer(&timer_idx_slide_switch_power_on_cycle_check);
            }
        }
        break;
#endif

    default:
        break;
    }
}

static void app_slide_switch_x_driver_init(T_APP_SLIDE_SWITCH_ID id)
{
    uint8_t gpio_status = 0;
    uint8_t pinmux = slide_switch[id].pinmux;
    T_GPIO_PULL_VALUE pull_mode = GPIO_PULL_DOWN;
    T_SWITCH_ACTION high_action = slide_switch[id].high_action;
    T_SWITCH_ACTION low_action = slide_switch[id].low_action;
    bool irq_enable_now = false;

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
    if (low_action == ACTION_POWER_ON)
    {
        //reduce power consumption
        pull_mode = GPIO_PULL_DOWN;
    }
    else if (high_action == ACTION_POWER_ON)
    {
        //reduce power consumption
        pull_mode = GPIO_PULL_UP;
    }
    else
#endif
    {
        pull_mode = GPIO_PULL_UP;
    }

    /*gpio init*/
    hal_gpio_init_pin(pinmux, GPIO_TYPE_CORE, GPIO_DIR_INPUT, pull_mode);
    hal_gpio_set_up_irq(pinmux, GPIO_IRQ_EDGE, GPIO_IRQ_ACTIVE_LOW, true);

    gpio_status = hal_gpio_get_input_level(pinmux);

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
    if (((high_action == ACTION_POWER_ON) && (low_action == ACTION_POWER_OFF)) ||
        ((high_action == ACTION_POWER_OFF) && (low_action == ACTION_POWER_ON)))
    {
        irq_enable_now = true;

        if ((low_action == ACTION_POWER_ON && gpio_status == 0) ||
            (high_action == ACTION_POWER_ON && gpio_status == 1))
        {
            if (app_cfg_nv.is_app_reboot && (app_cfg_nv.slide_switch_power_on_off_gpio_status == gpio_status))
            {
                //APP_PRINT_INFO0("app_slide_switch_x_driver_init: avoid power on interrupt");
                //slide_switch_state not changed, do not power on
            }
            else
            {
                app_slide_switch_intr_cb(id);
            }
        }

        /* Change GPIO Interrupt Polarity */
        app_slide_switch_update_intpolarity(pinmux, gpio_status);
    }
#endif

    hal_gpio_register_isr_callback(pinmux, app_slide_switch_intr_cb, id);

    if (irq_enable_now)
    {
        hal_gpio_irq_enable(pinmux);
    }

}

static void app_slide_switch_power_on_check(void)
{
    uint8_t gpio_status = 0;
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            gpio_status = hal_gpio_get_input_level(slide_switch[i].pinmux);

            if (((slide_switch[i].low_action == ACTION_POWER_ON) &&
                 (slide_switch[i].high_action == ACTION_POWER_OFF))  ||
                ((slide_switch[i].low_action == ACTION_POWER_OFF) &&
                 (slide_switch[i].high_action == ACTION_POWER_ON)))
            {
                //do nothing
            }
            else
            {
                if (gpio_status)
                {
                    app_slide_switch_action_handle(slide_switch[i].high_action);
                }
                else
                {
                    app_slide_switch_action_handle(slide_switch[i].low_action);
                }
                app_slide_switch_update_intpolarity(slide_switch[i].pinmux, gpio_status);
                hal_gpio_irq_enable(slide_switch[i].pinmux);
            }
        }
    }
}

static void app_slide_switch_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            app_slide_switch_power_on_check();
        }
        break;

    default:
        break;
    }
}

void app_slide_switch_cfg_init(void)
{
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (i == 0)
        {
            slide_switch[i].support = app_cfg_const.slide_switch_0_support;
            slide_switch[i].low_action = (T_SWITCH_ACTION)app_cfg_const.slide_switch_0_low_action;
            slide_switch[i].high_action = (T_SWITCH_ACTION)app_cfg_const.slide_switch_0_high_action;
            slide_switch[i].pinmux = app_cfg_const.slide_switch_0_pinmux;
        }
        else if (i == 1)
        {
            slide_switch[i].support = app_cfg_const.slide_switch_1_support;
            slide_switch[i].low_action = (T_SWITCH_ACTION)app_cfg_const.slide_switch_1_low_action;
            slide_switch[i].high_action = (T_SWITCH_ACTION)app_cfg_const.slide_switch_1_high_action;
            slide_switch[i].pinmux = app_cfg_const.slide_switch_1_pinmux;
        }
    }
}

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
void app_slide_switch_power_on_off_gpio_status_reset(void)
{
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            if (((slide_switch[i].low_action == ACTION_POWER_ON) &&
                 (slide_switch[i].high_action == ACTION_POWER_OFF)))
            {
                app_cfg_nv.slide_switch_power_on_off_gpio_status = 1;
            }
            else if (((slide_switch[i].low_action == ACTION_POWER_OFF) &&
                      (slide_switch[i].high_action == ACTION_POWER_ON)))
            {
                app_cfg_nv.slide_switch_power_on_off_gpio_status = 0;
            }
        }
    }
}
#endif

void app_slide_switch_driver_init(void)
{
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            app_slide_switch_x_driver_init((T_APP_SLIDE_SWITCH_ID)i);
            app_dlps_pad_wake_up_polarity_invert(slide_switch[i].pinmux);
        }
    }
}

void app_slide_switch_init(void)
{
    sys_mgr_cback_register(app_slide_switch_dm_cback);
    app_timer_reg_cb(app_slide_switch_timeout_cb, &app_slide_switch_timer_id);
}

#if (F_APP_SLIDE_SWITCH_LISTENING_MODE == 1)
#if F_APP_APT_SUPPORT
void app_slide_switch_resume_apt(void)
{
    bool gpio_status;
    uint8_t i = 0;

    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            gpio_status = hal_gpio_get_input_level(slide_switch[i].pinmux);

            if (((slide_switch[i].high_action == ACTION_APT_ON) && (gpio_status == 1)) ||
                ((slide_switch[i].low_action == ACTION_APT_ON) && (gpio_status == 0)))
            {
                if (!app_apt_is_apt_on_state(app_db.current_listening_state))
                {
                    if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_NORMAL_APT)
                    {
                        app_listening_state_machine(EVENT_NORMAL_APT_ON, false, true);
                    }
                    else if (app_apt_get_apt_support_type() == APT_SUPPORT_TYPE_LLAPT)
                    {
                        T_ANC_APT_STATE first_llapt_scenario;

                        if (app_apt_set_first_llapt_scenario(&first_llapt_scenario))
                        {
                            app_listening_state_machine(LLAPT_STATE_TO_EVENT(first_llapt_scenario), false, true);
                        }
                    }
                }
            }
        }
    }
}
#endif
#endif

#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
bool app_slide_switch_power_on_valid_check(void)
{
    uint8_t gpio_status = 0;
    bool ret = true;
    uint8_t i = 0;

    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            gpio_status = hal_gpio_get_input_level(slide_switch[i].pinmux);
            if (slide_switch[i].high_action == ACTION_POWER_OFF &&
                slide_switch[i].low_action == ACTION_NONE)
            {
                if (gpio_status)
                {
                    ret = false;
                    break;
                }
            }
            else if (slide_switch[i].low_action == ACTION_POWER_OFF &&
                     slide_switch[i].high_action == ACTION_NONE)
            {
                if (!gpio_status)
                {
                    ret = false;
                    break;
                }
            }
        }
    }
    return ret;
}
#endif

ISR_TEXT_SECTION
static void app_slide_switch_intr_cb(uint32_t param)
{
    T_APP_SLIDE_SWITCH_ID id = (T_APP_SLIDE_SWITCH_ID)param;
    T_IO_MSG gpio_msg;
    uint8_t pin_num = slide_switch[id].pinmux;
    uint8_t gpio_status = hal_gpio_get_input_level(pin_num);

    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
    hal_gpio_irq_disable(pin_num);

    app_slide_switch_update_intpolarity(pin_num, gpio_status);

    app_dlps_pad_wake_up_polarity_invert(pin_num);

    gpio_msg.type = IO_MSG_TYPE_GPIO;
    if (id == APP_SLIDE_SWITCH_0)
    {
        gpio_msg.subtype = IO_MSG_GPIO_SLIDE_SWITCH_0;
    }
    else if (id == APP_SLIDE_SWITCH_1)
    {
        gpio_msg.subtype = IO_MSG_GPIO_SLIDE_SWITCH_1;
    }

    gpio_msg.u.param = gpio_status;

    app_io_msg_send(&gpio_msg);
}

void app_slide_switch_handle_msg(T_IO_MSG *msg)
{
    uint16_t subtype = msg->subtype;
    uint8_t gpio_status = msg->u.param;
    uint16_t timer_chann = 0;
    T_APP_SLIDE_SWITCH_ID channel = APP_SLIDE_SWITCH_0;

    if (subtype == IO_MSG_GPIO_SLIDE_SWITCH_0)
    {
        channel = APP_SLIDE_SWITCH_0;
        timer_chann = (gpio_status) ? APP_TIMER_SLIDE_SWITCH_0_HIGH_DEBOUNCE :
                      APP_TIMER_SLIDE_SWITCH_0_LOW_DEBOUNCE;
    }
    else if (subtype == IO_MSG_GPIO_SLIDE_SWITCH_1)
    {
        channel = APP_SLIDE_SWITCH_1;
        timer_chann = (gpio_status) ? APP_TIMER_SLIDE_SWITCH_1_HIGH_DEBOUNCE :
                      APP_TIMER_SLIDE_SWITCH_1_LOW_DEBOUNCE;
    }

    if (timer_idx_slide_switch_debounce[channel])
    {
        app_stop_timer(&timer_idx_slide_switch_debounce[channel]);
    }

    app_start_timer(&timer_idx_slide_switch_debounce[channel], "slide_switch_debouce",
                    app_slide_switch_timer_id, timer_chann, 0, false,
                    APP_SLIDE_SWITCH_DEBOUCE_TIME);
}

#if F_APP_SLIDE_SWITCH_LISTENING_MODE || F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
static bool app_slide_switch_check_function_is_used(uint8_t func1, uint8_t func2)
{
    uint8_t i = 0;

    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            if (((slide_switch[i].high_action == func1) && (slide_switch[i].low_action == func2)) ||
                ((slide_switch[i].high_action == func2) && (slide_switch[i].low_action == func1)))
            {
                return true;
            }
        }
    }

    return false;
}
#endif

#if F_APP_SLIDE_SWITCH_MIC_MUTE_TOGGLE
bool app_slide_switch_mic_mute_toggle_support(void)
{
    return app_slide_switch_check_function_is_used(ACTION_MIC_UNMUTE, ACTION_MIC_MUTE);
}
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
T_APP_SLIDE_SWITCH_GPIO_ACTION app_slide_switch_get_dual_mode_gpio_action(void)
{
    T_APP_SLIDE_SWITCH_GPIO_ACTION ret = APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_NONE;
    uint8_t i = 0;
    uint8_t gpio_status = 0;

    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            if (slide_switch[i].high_action == ACTION_DONGLE_DUAL_MODE_2_4G &&
                slide_switch[i].low_action == ACTION_DONGLE_DUAL_MODE_BT)
            {
                gpio_status = hal_gpio_get_input_level(slide_switch[i].pinmux);

                if (gpio_status == 0)
                {
                    ret = APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_BT;
                }
                else
                {
                    ret = APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_2_4G;
                }
                break;
            }
            else if (slide_switch[i].high_action == ACTION_DONGLE_DUAL_MODE_BT &&
                     slide_switch[i].low_action == ACTION_DONGLE_DUAL_MODE_2_4G)
            {
                gpio_status = hal_gpio_get_input_level(slide_switch[i].pinmux);

                if (gpio_status == 0)
                {
                    ret = APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_2_4G;
                }
                else
                {
                    ret = APP_SLIDE_SWITCH_DUAL_MODE_GPIO_ACTION_BT;
                }
                break;
            }
        }
    }

    return ret;
}
#endif

#if (F_APP_SLIDE_SWITCH_LISTENING_MODE == 1)
T_APP_SLIDE_SWITCH_ANC_APT_STATE app_slide_switch_anc_apt_state_get(void)
{
    return slide_switch_anc_apt_state;
}

bool app_slide_switch_between_anc_apt_support(void)
{
    return app_slide_switch_check_function_is_used(ACTION_ANC_ON, ACTION_APT_ON);
}

bool app_slide_switch_anc_on_off_support(void)
{
    return app_slide_switch_check_function_is_used(ACTION_ANC_ON, ACTION_ANC_OFF);
}

bool app_slide_switch_apt_on_off_support(void)
{
    return app_slide_switch_check_function_is_used(ACTION_APT_ON, ACTION_APT_OFF);
}
#endif

void  app_slide_switch_enter_dlps_pad_set(void)
{
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            app_dlps_pad_wake_up_enable(slide_switch[i].pinmux);
        }
    }
}

void  app_slide_switch_exit_dlps_pad_set(void)
{
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
        if (slide_switch[i].support)
        {
            app_dlps_restore_pad(slide_switch[i].pinmux);
        }
    }
}

void  app_slide_switch_enter_power_down_pad_set(void)
{
    uint8_t i = 0;
    for (i = 0; i < APP_SLIDE_SWITCH_TOTAL; i++)
    {
#if (F_APP_SLIDE_SWITCH_POWER_ON_OFF == 1)
        if (slide_switch[i].support &&
            ((slide_switch[i].low_action == ACTION_POWER_ON) ||
             (slide_switch[i].high_action == ACTION_POWER_ON)))
        {
            app_dlps_pad_wake_up_enable(slide_switch[i].pinmux);
        }
#endif

        //to reduce power consumption
        if (slide_switch[i].support)
        {
            if (hal_gpio_get_input_level(slide_switch[i].pinmux))
            {
                Pad_Config(slide_switch[i].pinmux,
                           PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
            }
            else
            {
                Pad_Config(slide_switch[i].pinmux,
                           PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_HIGH);
            }
        }
    }
}

#endif
