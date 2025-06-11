#if F_APP_USB_SUSPEND_SUPPORT
#include <stdint.h>
#include "trace.h"
#include "usb_msg.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_ipc.h"
#include "app_usb_suspend.h"
#include "app_timer.h"
#include "wdg.h"
#include "charger_api.h"
#include "ble_ext_adv.h"
#include "app_adv_stop_cause.h"
#include "app_led.h"
#include "system_status_api.h"
#include "app_dongle_dual_mode.h"

typedef enum
{
    APP_TIMER_PC_SUSPEND_CHECK,
} T_APP_USB_TIMER;

static uint8_t app_usb_suspend_timer_id = 0;
static uint8_t timer_idx_pc_suspend_check = 0;

static void app_usb_suspend_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_PC_SUSPEND_CHECK:
        {
            app_stop_timer(&timer_idx_pc_suspend_check);

            if (app_link_get_b2s_link_num() == 0)
            {
                app_usb_suspend_enable();
            }
        }
        break;

    default:
        break;
    }
}

void app_usb_suspend_enable(void)
{
    app_db.is_usb_suspend = 1;

#if F_APP_LISTENING_MODE_SUPPORT
    /* Disable listening mode during usb suspend mode */
    app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_USB_SUSPEND);
#endif

    /* BT Shutdown and disable ADV, LED, charger */
    app_bt_policy_shutdown();

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_cfg_const.enable_dongle_dual_mode)
    {
        app_dongle_adv_stop();
    }
#endif
    ble_ext_adv_mgr_disable_all(APP_STOP_ADV_CAUSE_USB_SUSPEND);
    app_led_change_mode(LED_MODE_ALL_OFF, true, false);
    charger_api_disable_charger();

    APP_PRINT_TRACE0("app_usb_suspend_enable");
}

void app_usb_suspend_disable(void)
{
    app_db.is_usb_suspend = 0;

#if F_APP_LISTENING_MODE_SUPPORT
    /* apply last listening mode after usb resume */
    app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_USB_SUSPEND_END);
#endif

    /* Enable charger and BT policy */
    if (app_cfg_nv.app_is_power_on)
    {
        app_device_bt_policy_startup(true);
    }
    sys_hall_charger_auto_enable(true);

    APP_PRINT_TRACE0("app_usb_suspend_disable");
}

static void app_usb_suspend_usb_ipc_cback(uint32_t id, void *msg)
{
    APP_PRINT_TRACE1("app_usb_suspend_usb_ipc_cback: 0x%02x", id);

    switch (id)
    {
    case USB_IPC_EVT_UNPLUG:
        {
            app_stop_timer(&timer_idx_pc_suspend_check);

            if (app_db.is_usb_suspend)
            {
                // unplug usb during usb suspend -> reset and stay power off
                app_cfg_nv.app_is_power_on = 0;
                app_usb_suspend_disable();
            }
        }
        break;

    case USB_IPC_EVT_SUSPEND:
        {
            app_start_timer(&timer_idx_pc_suspend_check, "pc_suspend_check", app_usb_suspend_timer_id,
                            APP_TIMER_PC_SUSPEND_CHECK, 0, false, 1000);
        }
        break;

    case USB_IPC_EVT_RESUME:
        {
            app_stop_timer(&timer_idx_pc_suspend_check);

            if (app_db.is_usb_suspend)
            {
                app_usb_suspend_disable();
            }

#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_cfg_const.enable_dongle_dual_mode)
            {
                app_dongle_adv_start(false);
            }
#endif
        }
        break;

    default:
        break;
    }
}

void app_usb_suspend_init(void)
{
    app_ipc_subscribe(USB_IPC_TOPIC, app_usb_suspend_usb_ipc_cback);
    app_timer_reg_cb(app_usb_suspend_timeout_cb, &app_usb_suspend_timer_id);
}
#endif

