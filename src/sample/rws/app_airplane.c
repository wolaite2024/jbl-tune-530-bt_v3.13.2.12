#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "os_mem.h"
#include "os_timer.h"
#include "os_sync.h"
#include "console.h"
#include "trace.h"
#include "bt_types.h"
#include "rtl876x_gpio.h"
#include "anc_tuning.h"
#include "anc.h"
#include "audio_passthrough.h"
#include "app_cfg.h"
#include "app_cmd.h"
#include "app_report.h"
#include "app_main.h"
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#include "app_bt_policy_api.h"
#include "app_relay.h"
#include "app_mmi.h"
#include "gap_br.h"
#include "app_auto_power_off.h"
#include "app_audio_policy.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#include "app_airplane.h"
#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif
#include "ble_ext_adv.h"
#include "app_adv_stop_cause.h"

typedef enum
{
    APP_IO_TIMER_ENTER_AIRPLANE_MODE
} T_APP_AVP_TIMER_ID;

static uint8_t airplane_mode;
static uint8_t airplane_combine_key_power_on;

/**
    * @brief  control lowerstack to enter airplane mode
    * @param  void
    * @return void
    */
static void app_airplane_mode_enable(void)
{
    gap_write_airplan_mode(1);
}

/**
    * @brief  control lowerstack to exit airplane mode
    * @param  void
    * @return void
    */
static void app_airplane_mode_disable(void)
{
    gap_write_airplan_mode(0);
}

static void app_airplane_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_DISCONN:
    case BT_EVENT_ACL_CONN_FAIL:
        {
            if (airplane_mode)
            {
                if (app_airplane_all_link_idle())
                {
                    //make sure no link active, disable RF power
                    app_airplane_mode_enable();
                }
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_airplane_bt_cback: event_type 0x%04x", event_type);
    }
}

/**
    * @brief  get airplane mode status
    * @param  void
    * @return airplane_mode
    */
uint8_t app_airplane_mode_get(void)
{
    return airplane_mode;
}

/**
    * @brief  get variable to know system is powered on by airplane
              combine key
    * @param  void
    * @return airplane_combine_key_power_on
    */
uint8_t app_airplane_combine_key_power_on_get(void)
{
    return airplane_combine_key_power_on;
}

/**
    * @brief  set variable to know system power on by airplane
              combine key
    * @param  enable
    * @return void
    */
void app_airplane_combine_key_power_on_set(uint8_t enable)
{
    APP_PRINT_TRACE1("app_airplane_combine_key_power_on_set = %x, ", enable);
    airplane_combine_key_power_on = enable;
}

bool app_airplane_all_link_idle(void)
{
    bool ret = false;

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED) &&
        (app_link_get_b2s_link_num() == 0))
    {
        ret = true;
    }

    return ret;
}

/**
    * @brief  check mmi action be supported in airplane mode or not
    * @param  action
    * @return boolean
    */
bool app_airplane_mode_mmi_support(uint8_t action)
{
    bool ret = false;

    if ((action == MMI_AIRPLANE_MODE) ||
        (action == MMI_DEV_POWER_OFF) ||
        (action == MMI_DEV_FACTORY_RESET) ||
        (action == MMI_AUDIO_APT_EQ_SWITCH) ||
        (action == MMI_AUDIO_APT_VOL_UP) ||
        (action == MMI_AUDIO_APT_VOL_DOWN) ||
        (action == MMI_AUDIO_APT) ||
        (action == MMI_LISTENING_MODE_CYCLE) ||
        (action == MMI_ANC_ON_OFF) ||
        (action == MMI_ANC_CYCLE) ||
        (action == MMI_LLAPT_CYCLE) ||
        (action == MMI_DEFAULT_LISTENING_MODE_CYCLE) ||
        (action == MMI_ANC_APT_ON_OFF))
    {
        ret = true;
    }

#if F_APP_LINEIN_SUPPORT
    if (app_line_in_playing_state_get() &&
        (action == MMI_DEV_SPK_VOL_UP || action == MMI_DEV_SPK_VOL_DOWN ||
         action == MMI_AUDIO_EQ_SWITCH))
    {
        ret = true;
    }
#endif

    return ret;
}

/**
    * @brief  enter airplane action when system power on
    * @param  void
    * @return void
    */
void app_airplane_power_on_handle(void)
{
    APP_PRINT_TRACE1("app_airplane_power_on_handle = %x", airplane_mode);

    app_airplane_combine_key_power_on_set(0);

    if (!airplane_mode)
    {
        airplane_mode = 1;

        //disable RF power
        app_airplane_mode_enable();

#if F_APP_LISTENING_MODE_SUPPORT
        //set listening mode state
        app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_AIRPLANE);
#endif

        if (app_cfg_const.disallow_auto_power_off_when_airplane_mode)
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_AIRPLANE_MODE);
        }
    }
}

/**
    * @brief  exit airplane action when system power off
    * @param  void
    * @return void
    */
void app_airplane_power_off_handle(void)
{
    APP_PRINT_TRACE1("app_airplane_power_off_handle = %x", airplane_mode);

    if (airplane_mode)
    {
        airplane_mode = 0;
        app_airplane_mode_disable();
#if F_APP_LISTENING_MODE_SUPPORT
        //recover listening mode state
        app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_AIRPLANE_END);
#endif
    }
}

static void app_airplane_bt_startup(void)
{
    //enable RF power
    app_airplane_mode_disable();

    //recover BT state
    app_device_bt_policy_startup(true);
}

/**
    * @brief  execute airplane action by trigger MMI
    * @param  void
    * @return void
    */
void app_airplane_mmi_handle(void)
{
    airplane_mode ^= 1;

    APP_PRINT_TRACE2("app_airplane_mmi_handle = %x, bt shutdown %d",
                     airplane_mode, app_db.airplane_bt_shutdown_busy);

    if (airplane_mode)
    {
        if (!app_audio_tone_type_play(TONE_ENTER_AIRPLANE, false, false)) // no airplane tone
        {
            app_airplane_enter_airplane_mode_handle();
        }

#if F_APP_LISTENING_MODE_SUPPORT
        //set listening mode state
        app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_AIRPLANE);
#endif
    }
    else
    {
        app_audio_tone_type_play(TONE_EXIT_AIRPLANE, false, false);

        if (!app_db.airplane_bt_shutdown_busy)
        {
            app_airplane_bt_startup();
        }

#if F_APP_LISTENING_MODE_SUPPORT
        //recover listening mode state
        app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_AIRPLANE_END);
#endif

        if (app_cfg_const.disallow_auto_power_off_when_airplane_mode)
        {
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_AIRPLANE_MODE, app_cfg_const.timer_auto_power_off);
        }
    }
}

/**
    * @brief  check and execute airplane bt startup action when bt shutdown ends
    * @param  void
    * @return void
    */
void app_airplane_bt_shutdown_end_check(void)
{
    APP_PRINT_TRACE3("app_airplane_bt_shutdown_end_check airplane %d bt shutdown %d device_state %d",
                     airplane_mode, app_db.airplane_bt_shutdown_busy, app_db.device_state);

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        if (app_db.airplane_bt_shutdown_busy && !airplane_mode)
        {
            // exit airplane mode, bt startup
            app_airplane_bt_startup();
        }
    }

    app_db.airplane_bt_shutdown_busy = 0;
}

/**
    * @brief  execute airplane action when in box
    * @param  void
    * @return void
    */
void app_airplane_in_box_handle(void)
{
    if (app_cfg_const.exit_airplane_when_into_charger_box && airplane_mode)
    {
        app_airplane_mmi_handle();
    }
}

/**
    * @brief  execute airplane action when ready to enter airplane mode
    * @param  void
    * @return void
    */
void app_airplane_enter_airplane_mode_handle(void)
{
    if (airplane_mode)
    {
        //shutdown BT state
        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
            (app_link_get_b2s_link_num() == 0))
        {
            app_auto_power_off_enable(AUTO_POWER_OFF_MASK_SOURCE_LINK, app_cfg_const.timer_auto_power_off);
        }

        app_db.airplane_bt_shutdown_busy = 1;
        app_bt_policy_shutdown();

        ble_ext_adv_mgr_disable_all(APP_STOP_ADV_CAUSE_AIRPLANE);

        if (app_airplane_all_link_idle())
        {
            //check all link disconnected, disable RF power
            app_airplane_mode_enable();
        }

        if (app_cfg_const.disallow_auto_power_off_when_airplane_mode)
        {
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_AIRPLANE_MODE);
        }

        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_BUD_COUPLING, app_cfg_const.timer_auto_power_off);
    }
    else
    {
        APP_PRINT_ERROR0("app_airplane_enter_airplane_mode_handle: not in airplane mode");
    }
}

void app_airplane_init(void)
{
    bt_mgr_cback_register(app_airplane_bt_cback);
}
#endif
