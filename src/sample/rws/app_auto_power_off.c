/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>
#include "trace.h"
#include "sysm.h"
#include "app_timer.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_main.h"
#include "app_relay.h"
#include "pm.h"
#if F_APP_OTA_SUPPORT
#include "app_ota.h"
#endif

typedef enum
{
    APP_TIMER_AUTO_POWER_OFF = 0x00,
} T_APP_AUTO_POWER_OFF_TIMER;

static uint8_t auto_power_off_timer_id = 0;
static uint8_t timer_idx_auto_power_off = 0;
static uint32_t poweroff_flag = 0;

static void app_auto_power_off_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_AUTO_POWER_OFF:
        {
            app_stop_timer(&timer_idx_auto_power_off);

#if F_APP_OTA_SUPPORT
            if (app_ota_mode_check())
            {
                APP_PRINT_INFO0("app_ota_mode_check is true, stop auto power off");
                break;
            }
#endif

            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
                app_db.power_off_cause = POWER_OFF_CAUSE_AUTO_POWER_OFF;

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_mmi_handle_action(MMI_DEV_POWER_OFF);
                }
                else
                {
                    uint8_t action = MMI_DEV_POWER_OFF;

#if F_APP_ERWS_SUPPORT
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_relay_sync_single(APP_MODULE_TYPE_MMI, action, REMOTE_TIMER_HIGH_PRECISION,
                                              0, false);
                    }
                    else
                    {
                        app_relay_sync_single(APP_MODULE_TYPE_MMI, action, REMOTE_TIMER_HIGH_PRECISION,
                                              0, true);
                    }
#endif
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_auto_power_off_enable(uint32_t flag, uint16_t timeout)
{
    APP_PRINT_INFO3("app_auto_power_off_enable: curr flag 0x%08x, clear flag 0x%08x, timeout %u",
                    poweroff_flag, flag, timeout);

    poweroff_flag &= ~flag;

    if (poweroff_flag == 0)
    {
        if (timeout == 0)
        {
            app_stop_timer(&timer_idx_auto_power_off);
        }
        else
        {
            app_start_timer(&timer_idx_auto_power_off, "auto_power_off",
                            auto_power_off_timer_id, APP_TIMER_AUTO_POWER_OFF, 0, false,
                            (timeout * 1000));
        }
    }
}

void app_auto_power_off_disable(uint32_t flag)
{
    APP_PRINT_TRACE2("app_auto_power_off_disable: curr flag 0x%08x, set flag 0x%08x",
                     poweroff_flag, flag);

    poweroff_flag |= flag;

    if (poweroff_flag != 0)
    {
        app_stop_timer(&timer_idx_auto_power_off);
    }
}

static void app_auto_power_off_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_OFF:
        {
            app_stop_timer(&timer_idx_auto_power_off);
        }
        break;

    default:
        break;
    }
}

void app_auto_power_off_init(void)
{
    poweroff_flag = 0;

    app_timer_reg_cb(app_auto_power_off_timeout_cb, &auto_power_off_timer_id);
    sys_mgr_cback_register(app_auto_power_off_dm_cback);
}

