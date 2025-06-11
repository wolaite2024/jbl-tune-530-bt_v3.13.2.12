#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "rtl876x_pinmux.h"
#include "system_status_api.h"
#include "charger_utils.h"
#include "pm.h"
#include "app_dlps.h"
#include "app_cfg.h"
#include "app_timer.h"
#include "app_main.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "hal_adp.h"
#include "app_adp.h"
#include "app_adp_cmd.h"
#include "app_adp_cmd_parse.h"
#include "app_ota.h"
#include "app_ota_tooling.h"
#include "app_led.h"
#include "app_cmd.h"
#include "app_relay.h"
#include "app_ipc.h"
#include "test_mode.h"

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_gpio_on_off.h"
#endif
#if GFPS_FEATURE_SUPPORT
#include "app_gfps.h"
#include "app_gfps_battery.h"
#endif
#if F_APP_CAP_TOUCH_SUPPORT
#include "app_cap_touch.h"
#endif
#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA_2
#include "app_data_capture.h"
#endif

#define USB_STR_WAIT_1_TIM_CNT 1
#define USB_STR_WAIT_0_TIM_CNT 0

// Guard BIT, now we just use(6 bits), Spec define 8 bits.
#define GUARD_BIT 0x3F

#define ADP_FACTORY_RESET_LINK_DIS_TIMEOUT_MS           1500
#define ADP_FACTORY_RESET_LINK_DIS_HEADSET_BROADCAST_MS 200
#define ADP_CLOSE_CASE_TIMEOUT_MS                       3000
#define ADP_DISABLE_CHARGER_LED_MS                      3000
#define ADP_CLOSE_CASE_TO_DISC_PHONE_LINK_AVP_MS        3000
#define ADP_CLOSE_CASE_TO_DISC_PHONE_LINK_MS            5000
#define ADP_IN_CASE_AUTO_POWER_OFF_MS                   120000
#define ADP_CHECK_CLOSE_CASE_CMD_MS                     2000
#define MAX_TIMES_CHECK_B2B_CONNECTED_STATE             8

#define ADP_PAYLOAD_TYPE1   0x55    // For Normal Factory Reset and Enter Pairing mode
#define ADP_PAYLOAD_TYPE2   0xAA    // For Enter DUT test mode
#define ADP_PAYLOAD_TYPE3   0x46    // For DUT mode cmd, enter BAT off mode
#define ADP_PAYLOAD_TYPE4   0x45    // For Enter shipping mode
#define ADP_PAYLOAD_TYPE5   0xA5    // For SaiYan mode
#define ADP_PAYLOAD_TYPE6   0xA6    // For Captouch Calibration

/*40Mhz 40 devide, set 1ms period=1000 */
#define PERIOD_ADP_TIM      2000000 // 2s

/**
 * @brief ADAPTOR DET to APP cmd
 */
typedef enum
{
    AD2B_CMD_NONE                   = 0x00,
    AD2B_CMD_FACTORY_RESET          = 0x01,
    AD2B_CMD_OPEN_CASE              = 0x03,
    AD2B_CMD_CLOSE_CASE             = 0x04,
    AD2B_CMD_ENTER_PAIRING          = 0x06,
    AD2B_CMD_ENTER_DUT_MODE         = 0x08,
    AD2B_CMD_USB_STATE              = 0x0A,
    AD2B_CMD_OTA_TOOLING            = 0x0B,
    AD2B_CMD_SAIYAN                 = 0x0C,
    AD2B_CMD_CASE_CHARGER           = 0x0D,// case charger battery
    AD2B_CMD_ENTER_MP_MODE          = 0x0E,

    AD2B_CMD_RESV_1                 = 0x3E,// resv for hw reset --> customer MCU use
    AD2B_CMD_RESV_2                 = 0x3F,// resv for hw reset --> customer MCU use
} T_AD2B_CMD_ID;

typedef enum
{
    APP_TIMER_ADP_FACTORY_RESET_LINK_DIS,
    APP_TIMER_ADP_CLOSE_CASE,
    APP_TIMER_ADP_CMD_RECEIVED,
    APP_TIMER_ADP_POWER_ON_WAIT,
    APP_TIMER_DISALLOW_CHARGER_LED,
    APP_TIMER_ADP_POWER_OFF_DISC_PHONE_LINK,
    APP_TIMER_ADP_IN_CASE,
    APP_TIMER_ADP_WAIT_ROLESWAP,
    APP_TIMER_CHECK_ADP_CMD_DISABLE_CHARGER,
    APP_TIMER_CHECK_ADP_CMD_CLOSE_CASE,
    APP_TIMER_BOX_ZERO_VOLUME_IGNORE_ADP_OUT,
    APP_TIMER_POWE_OFF_ENTER_DUT_MODE,
    APP_TIMER_OTA_TOOLING_DELAY_START,
} T_APP_ADP_CMD_TIMER;

typedef struct
{
    uint32_t stop_bit: 1;
    uint32_t parity_bit: 1;
    uint32_t cmd_index: 6;
    uint32_t start_bit: 1;
    uint32_t reserved_bit: 23;
} T_APP_ADP_CMDPACK09;

typedef struct
{
    uint32_t stop_bit: 1;
    uint32_t parity_bit: 1;
    uint32_t payload: 8;
    uint32_t cmd_index: 4;
    uint32_t start_bit: 1;
    uint32_t reserved_bit: 17;
} T_APP_ADP_CMDPACK15;

typedef union
{
    uint32_t bit_data;
    T_APP_ADP_CMDPACK09 cmdpack09;
    T_APP_ADP_CMDPACK15 cmdpack15;
} T_APP_ADP_CMD_DECODE;

static bool out_case_power_on_led_en = false;
static bool in_case_is_power_off = false;

static uint8_t app_adp_cmd_timer_id = 0;
static uint8_t timer_idx_adp_close_case = 0;
static uint8_t timer_idx_adp_cmd_received = 0;
static uint8_t timer_idx_adp_power_on_wait = 0;
static uint8_t timer_idx_disallow_charger_led = 0;
static uint8_t timer_idx_adp_power_off_disc_phone_link = 0;
static uint8_t timer_idx_adp_in_case = 0;
static uint8_t timer_idx_adp_wait_roleswap = 0;
static uint8_t timer_idx_check_adp_cmd_disable_charger = 0;
static uint8_t timer_idx_check_adp_cmd_close_case = 0;
static uint8_t timer_idx_box_zero_volume_ignore_adp_out = 0;
static uint8_t timer_idx_power_off_enter_dut_mode = 0;
static uint8_t timer_idx_adp_factory_reset_link_dis = 0;
static uint8_t timer_idx_ota_tooling_delay_start = 0;

static uint8_t power_on_wait_counts = 0;
static uint8_t box_bat_volume = 0;
static bool power_on_wait_when_bt_ready = false;
static uint8_t adp_cmd_exec = AD2B_CMD_NONE;
static uint8_t adp_cmd_pending = AD2B_CMD_NONE;
static uint8_t adp_payload_pending = 0;
static bool case_batt_pre_updated = false;

static void app_adp_cmd_linkback_b2s_when_open_case(void);
static void app_adp_cmd_ota_tooling(uint8_t payload);

static void app_adp_cmd_case_battery(uint8_t battery)
{
    static uint8_t case_batt_pre = 0, case_batt = 0;

#if F_APP_DURIAN_SUPPORT
    static uint8_t case_status = 0;
#endif

    if ((battery & 0x7f) == 0)
    {
        /* case battery 0 is used to inform drop 5v; not a real value */
        return;
    }

    if (app_adp_cmd_case_bat_check(&battery, &case_batt))
    {
        if (case_batt_pre_updated)
        {
            if (abs((case_batt & 0x7f) - case_batt_pre) >= 10)//remove debounce
            {
                return;
            }
        }

        app_db.case_battery = case_batt;
        app_cfg_nv.case_battery = app_db.case_battery;
        case_batt_pre = case_batt & 0x7f;
        case_batt_pre_updated = true;

#if F_APP_DURIAN_SUPPORT
        if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY) && (case_status != (case_batt >> 7)))
        {
            case_status = case_batt >> 7;
            app_durian_adp_batt_report();
        }
#endif

        app_relay_async_single(APP_MODULE_TYPE_CHARGER, APP_CHARGER_EVENT_BATT_CASE_LEVEL);

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_report_rws_bud_info();
        }
    }
}

bool app_adp_cmd_case_bat_check(uint8_t *bat_in, uint8_t *bat_out)
{
    bool is_valid = false;

    APP_PRINT_TRACE2("app_adp_cmd_case_bat_check: bat_in 0x%02x, bat_out 0x%02x",
                     *bat_in, *bat_out);

    if ((*bat_in & 0x7f) > 100)//invalid battery level
    {
        *bat_out = *bat_in & 0x80;
    }
    else
    {
        *bat_out = *bat_in;
        is_valid = true;
    }

    return is_valid;
}

static void app_adp_cmd_power_on_handle(void)
{
    app_db.power_on_by_cmd = true;
    app_db.power_off_by_cmd = false;

    app_db.local_loc = app_loc_mgr_local_detect();

    app_cfg_nv.power_off_cause_cmd = 0;
    in_case_is_power_off = false;

    if (app_db.device_state == APP_DEVICE_STATE_OFF)
    {
        out_case_power_on_led_en = false;
        app_stop_timer(&timer_idx_adp_close_case);//stop here, don't do power off
        app_stop_timer(&timer_idx_adp_power_off_disc_phone_link);
        app_mmi_handle_action(MMI_DEV_POWER_ON);
    }
    else
    {
        if (app_cfg_const.enable_power_off_immediately_when_close_case == false)
        {
            app_adp_cmd_linkback_b2s_when_open_case();
        }
        else
        {
            app_stop_timer(&timer_idx_adp_close_case);//stop here, don't do power off
        }
    }

    app_loc_mgr_state_machine(EVENT_OPEN_CASE, 0, CAUSE_ACTION_NON);
}

void app_adp_cmd_clear_pending(void)
{
    adp_cmd_pending = AD2B_CMD_NONE;
    adp_payload_pending = 0;
}

void app_adp_cmd_delay_charger_enable(void)
{
    if (app_cfg_const.smart_charger_control && charger_utils_get_thermistor_enable_state())
    {
        app_adp_smart_box_charger_control(false);

        app_adp_set_disallow_enable_charger(true);

        app_start_timer(&timer_idx_check_adp_cmd_disable_charger, "check_adp_cmd_disable_charger",
                        app_adp_cmd_timer_id, APP_TIMER_CHECK_ADP_CMD_DISABLE_CHARGER, 0, false,
                        1000);
    }
}

static void app_adp_cmd_delay_power_on_when_bt_ready(void)
{
    app_start_timer(&timer_idx_adp_power_on_wait, "adp_power_on_wait",
                    app_adp_cmd_timer_id, APP_TIMER_ADP_POWER_ON_WAIT, 0, false,
                    100);
}

static void app_adp_cmd_close_case_cmd_check(void)
{
    if ((app_db.local_loc != BUD_LOC_IN_CASE) || (timer_idx_adp_close_case != 0) ||
        (app_db.device_state == APP_DEVICE_STATE_OFF))
    {
        return;
    }

    app_start_timer(&timer_idx_check_adp_cmd_close_case, "check_adp_cmd_close_case",
                    app_adp_cmd_timer_id, APP_TIMER_CHECK_ADP_CMD_CLOSE_CASE, 0, false,
                    ADP_CHECK_CLOSE_CASE_CMD_MS);
}

static void app_adp_cmd_open_case_cmd_check(void)
{
    app_stop_timer(&timer_idx_check_adp_cmd_close_case);
}

static void app_adp_cmd_open_case_start(void)
{
    APP_PRINT_INFO0("app_adp_cmd_open_case_start");

    app_stop_timer(&timer_idx_adp_in_case);
    app_stop_timer(&timer_idx_adp_power_on_wait);

    app_cfg_nv.power_on_cause_cmd = 0;

    if (app_db.bt_is_ready)
    {
        if ((app_db.device_state == APP_DEVICE_STATE_OFF_ING) || app_mmi_reboot_check_timer_started())
        {
            app_cfg_nv.power_on_cause_cmd = 1;
            app_cfg_store(&app_cfg_nv.offset_smart_chargerbox, 1);
        }
        else
        {
            app_adp_cmd_power_on_handle();
        }
    }
    else
    {
        power_on_wait_when_bt_ready = true;
        power_on_wait_counts = 0;
        adp_cmd_exec = AD2B_CMD_NONE; //rec the next power on cmd
        app_adp_cmd_delay_power_on_when_bt_ready();
    }
}

static void app_adp_cmd_close_case_start(void)
{
    uint32_t time_to_power_off = 0, time_to_disc = 0;

    APP_PRINT_INFO0("app_adp_cmd_close_case_start");

    app_stop_timer(&timer_idx_check_adp_cmd_close_case);
    app_stop_timer(&timer_idx_adp_in_case);

    if (app_cfg_nv.power_on_cause_cmd)
    {

        app_cfg_nv.power_on_cause_cmd = false;
        app_cfg_store(&app_cfg_nv.offset_smart_chargerbox, 1);
    }

    if (app_db.device_state == APP_DEVICE_STATE_OFF)
    {
        in_case_is_power_off = true;
        app_cfg_nv.power_off_cause_cmd = 1;
        app_cfg_store(&app_cfg_nv.remote_loc, 4);
        APP_PRINT_INFO0("app_adp_cmd_close_case_start: already power off,wdt rst later");
    }
    else
    {
        power_on_wait_when_bt_ready = false;

        if (app_cfg_const.enable_power_off_immediately_when_close_case == false)
        {
#if F_APP_DURIAN_SUPPORT
            time_to_disc = ADP_CLOSE_CASE_TO_DISC_PHONE_LINK_AVP_MS;
            time_to_power_off = ADP_CLOSE_CASE_TIMEOUT_MS * 5;
#else
            time_to_disc = ADP_CLOSE_CASE_TO_DISC_PHONE_LINK_MS;
            time_to_power_off = ADP_CLOSE_CASE_TIMEOUT_MS * 7;

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_bt_policy_stop();
            }
#endif
            app_start_timer(&timer_idx_adp_power_off_disc_phone_link, "adp_power_off_disc_phone_link",
                            app_adp_cmd_timer_id, APP_TIMER_ADP_POWER_OFF_DISC_PHONE_LINK, 0, false,
                            time_to_disc);
        }
        else
        {
            time_to_power_off = ADP_CLOSE_CASE_TIMEOUT_MS;
        }

        app_start_timer(&timer_idx_adp_close_case, "adp_close_case",
                        app_adp_cmd_timer_id, APP_TIMER_ADP_CLOSE_CASE, 0, false,
                        time_to_power_off);

        app_loc_mgr_state_machine(EVENT_CLOSE_CASE, 0, CAUSE_ACTION_NON);
    }
}

static void app_adp_cmd_other_feature_handle_when_close_case(void)
{
    if (app_db.device_state != APP_DEVICE_STATE_OFF)
    {
#if GFPS_FEATURE_SUPPORT
        if (extend_app_cfg_const.gfps_support)
        {
            app_gfps_handle_case_status(false);
        }
#endif

#if F_APP_DURIAN_SUPPORT
        app_durian_adv_close_case(false);
#endif
    }
}

static void app_adp_cmd_smart_box_update_bat_lv(uint8_t payload, bool report_flag)
{
    uint8_t rcv_bat_vol = (payload & 0x7F);
    uint8_t tmp_bat = 0;

    if (rcv_bat_vol == 0)
    {
        tmp_bat = app_cfg_nv.case_battery;
    }

    box_bat_volume = rcv_bat_vol;
    app_cfg_nv.case_battery = payload;

    if (report_flag && (rcv_bat_vol != 0))
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_report_rws_bud_info();
        }
        else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            if (app_db.remote_loc != BUD_LOC_IN_CASE)
            {
                app_db.need_sync_case_battery_to_pri = true;
            }
        }
    }

    if (app_cfg_const.smart_charger_control)
    {
        if (box_bat_volume <= app_cfg_const.smart_charger_disable_threshold)
        {
            if (app_db.device_state != APP_DEVICE_STATE_OFF)
            {
                /* this flag will be applied after sw reset after power off in order to disable charger */
                app_cfg_nv.report_box_bat_lv_again_after_sw_reset = true;
            }

            app_adp_smart_box_charger_control(false);

            app_adp_set_disable_charger_by_box_battery(true);

            if (box_bat_volume == 0)
            {
                /* means receive box 0% when bud still power on */
                if (timer_idx_check_adp_cmd_close_case != 0 &&
                    (app_db.device_state != APP_DEVICE_STATE_OFF))
                {
                    /* role swap will be triggerd when get close case cmd or in box */
                    app_stop_timer(&timer_idx_adp_power_off_disc_phone_link);
                    app_stop_timer(&timer_idx_check_adp_cmd_close_case);

                    app_adp_cmd_close_case_start();
                    app_adp_cmd_other_feature_handle_when_close_case();
                }
                else
                {
                    /* restart power_down_wdg protect timer */
                    app_dlps_start_power_down_wdg_timer();

                    /* when get box bat vol 0 %; start a 5sec timer to change to adp high wake up */
                    app_dlps_disable(APP_DLPS_ENTER_CHECK_BOX_BAT);

                    app_start_timer(&timer_idx_box_zero_volume_ignore_adp_out, "box_zero_volume_ignore_adp_out",
                                    app_adp_cmd_timer_id, APP_TIMER_BOX_ZERO_VOLUME_IGNORE_ADP_OUT, 0, false,
                                    5000);
                }
            }
        }
        else if (box_bat_volume >= app_cfg_const.smart_charger_enable_threshold)
        {
            app_cfg_nv.report_box_bat_lv_again_after_sw_reset = false;
            app_adp_smart_box_charger_control(true);

            app_adp_set_disable_charger_by_box_battery(false);
        }

        if (box_bat_volume == 0)
        {
            app_db.disallow_adp_out_auto_power_on = true;
        }
        else
        {
            app_db.disallow_adp_out_auto_power_on = false;
        }
    }

    if (rcv_bat_vol == 0)
    {
        /* bat lv 0 is used to inform charger box will drop 5v later and disable charger,
           not a real value, so no need to save to flash */
        app_cfg_nv.case_battery = tmp_bat;
    }

    app_cfg_store(&app_cfg_nv.case_battery, 4);
    app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);

    APP_PRINT_INFO2("app_adp_cmd_smart_box_update_bat_lv: box_bat_volume %d, case_battery %d",
                    box_bat_volume, app_cfg_nv.case_battery);
}

static void app_adp_cmd_exec(uint8_t cmd, uint8_t payload)
{
    adp_cmd_exec = cmd; //current executing cmd

    APP_PRINT_INFO4("app_adp_cmd_exec: adp_cmd_exec 0x%x payload %d device_state %d bt_is_ready %d",
                    adp_cmd_exec, payload, app_db.device_state, app_db.bt_is_ready);

    if (cmd == AD2B_CMD_OPEN_CASE)
    {
        app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_OPEN_CASE);
    }

    if ((cmd == AD2B_CMD_OPEN_CASE) || (cmd == AD2B_CMD_CASE_CHARGER))
    {
        if ((payload & 0x7F) > 100)//invalid battery level
        {
            payload = (payload & 0x80) | 0x50;
        }
        app_adp_cmd_smart_box_update_bat_lv(payload, true);
    }

    if (cmd != AD2B_CMD_CASE_CHARGER)
    {
        app_auto_power_off_enable(AUTO_POWER_OFF_MASK_IN_BOX, app_cfg_const.timer_auto_power_off);
    }

    if (app_cfg_const.enable_rtk_charging_box)
    {
        switch (cmd)
        {
        case AD2B_CMD_FACTORY_RESET:
            {
                uint16_t link_dis_delay = 0;

                if (app_cfg_nv.adp_factory_reset_power_on != 0)
                {
                    APP_PRINT_INFO0("app_adp_cmd_exec power on by factory reset,ignore");
                    break;
                }

#if F_APP_DURIAN_SUPPORT
                app_durian_adv_factory_reset();
#endif

                link_dis_delay = ADP_FACTORY_RESET_LINK_DIS_HEADSET_BROADCAST_MS;

                if (payload == ADP_PAYLOAD_TYPE1)
                {
                    app_db.disallow_charging_led = 0;
                    app_stop_timer(&timer_idx_disallow_charger_led);
                    app_adp_cmd_factory_reset_link_dis(link_dis_delay);
                }
                else if (payload == ADP_PAYLOAD_TYPE2)
                {
                    app_cfg_nv.adp_factory_reset_power_on = 1;

                    app_mmi_handle_action(MMI_DEV_PHONE_RECORD_RESET);
                }
            }
            break;

        case AD2B_CMD_OPEN_CASE:
            {
                if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
                {
                    app_adp_cmd_case_battery(payload);
                }

#if LOCAL_PLAYBACK_FEATURE_SUPPORT
                usb_stop(NULL);
                audio_fs_update(AD2B_CMD_OPEN_CASE);
                app_dlps_enable(APP_DLPS_ENTER_CHECK_USB);
#endif

                app_adp_cmd_open_case_start();
#if GFPS_FEATURE_SUPPORT
                if (extend_app_cfg_const.gfps_support)
                {
                    app_gfps_handle_case_status(true);
                }
#endif
#if F_APP_DURIAN_SUPPORT
                if (app_db.bt_is_ready && !app_cfg_nv.power_on_cause_cmd)
                {
                    app_durian_adv_open_case();
                }
#endif
            }
            break;

        case AD2B_CMD_CLOSE_CASE:
            {
                app_adp_cmd_close_case_start();
                app_adp_cmd_other_feature_handle_when_close_case();
            }
            break;

        case AD2B_CMD_ENTER_PAIRING:
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY &&
                    app_cfg_nv.first_engaged)
                {
                    if (app_db.device_state == APP_DEVICE_STATE_OFF)
                    {
                        sys_mgr_power_on();
                    }
                }
                else
                {
#if F_APP_DURIAN_SUPPORT
                    app_durian_adv_enter_pairing();
#else
                    app_mmi_handle_action(MMI_DEV_FORCE_ENTER_PAIRING_MODE);
#endif
                }
            }
            break;

        case AD2B_CMD_ENTER_DUT_MODE:
            {
                if (payload == ADP_PAYLOAD_TYPE2)
                {
                    if (app_db.device_state == APP_DEVICE_STATE_ON)
                    {
                        app_db.ad2b_enter_dut_mode = true;
                        app_mmi_handle_action(MMI_DUT_TEST_MODE);
                    }
                    else
                    {
                        app_cfg_nv.trigger_dut_mode_from_power_off = 1;
                        app_cfg_store(&app_cfg_nv.eq_idx_gaming_mode_record, 4);

                        app_mmi_handle_action(MMI_DEV_ENTER_PAIRING_MODE);

                        app_start_timer(&timer_idx_power_off_enter_dut_mode, "power_off_enter_dut_mode",
                                        app_adp_cmd_timer_id, APP_TIMER_POWE_OFF_ENTER_DUT_MODE, 0, false,
                                        POWER_OFF_ENTER_DUT_MODE_INTERVAL);
                    }
                }
                else if (payload == ADP_PAYLOAD_TYPE3)
                {
                    app_led_change_mode(LED_MODE_ENTER_PCBA_SHIPPING_MODE, true, false);
                }
#if F_APP_CAP_TOUCH_SUPPORT
                else if (payload == ADP_PAYLOAD_TYPE6)
                {
                    app_cap_touch_calibration_start();
                }
#endif
#if F_APP_DURIAN_SUPPORT
                else if (payload == ADP_PAYLOAD_TYPE4)
                {
                    power_mode_set(POWER_SHIP_MODE);
                }
                else
                {
                    app_durian_cfg_switch_one_key_trig(payload);
                }
#endif
            }
            break;

        case AD2B_CMD_USB_STATE:
            {
                app_adp_cmd_parse_handle_usb_cmd(cmd, payload);
            }
            break;

#if F_APP_SAIYAN_MODE
        case AD2B_CMD_SAIYAN:
            {
                app_data_capture_saiyan_mode_ctl(1, payload);
            }
            break;
#endif

        case AD2B_CMD_CASE_CHARGER:
            {
                if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
                {
                    app_adp_cmd_case_battery(payload);
#if GFPS_FEATURE_SUPPORT
                    if (extend_app_cfg_const.gfps_support || (app_cfg_const.supported_profile_mask & GFPS_PROFILE_MASK))
                    {
                        app_gfps_battery_info_report(GFPS_BATTERY_REPORT_CASE_BATTERY_CHANGE);
                    }
#endif
                }
            }
            break;

        case AD2B_CMD_ENTER_MP_MODE:
            {
                switch_into_hci_mode();
            }
            break;

        default:
            break;
        }
    }

#if F_APP_OTA_TOOLING_SUPPORT
    if (cmd == AD2B_CMD_OTA_TOOLING)
    {
        app_adp_cmd_ota_tooling(payload);
    }
#endif
}

void app_adp_cmd_pending_exec(void)
{
    if (adp_cmd_pending != AD2B_CMD_NONE)
    {
        APP_PRINT_INFO2("app_adp_cmd_pending_exec: cmd 0x%x payload 0x%x", adp_cmd_pending,
                        adp_payload_pending);
        app_adp_cmd_exec(adp_cmd_pending, adp_payload_pending);
        app_adp_cmd_clear_pending();
    }
}

#if F_APP_OTA_TOOLING_SUPPORT
static bool app_adp_cmd_ota_tooling_check(uint8_t payload)
{
    bool ret = true;
    uint8_t jig_op_code = payload >> 5 & 0x07;
    uint8_t jig_id = payload & 0x1f;

    if ((jig_op_code == APP_ADP_SPECIAL_CMD_NULL) ||
        (jig_op_code == APP_ADP_SPECIAL_CMD_RSV_6) ||
        (jig_op_code == APP_ADP_SPECIAL_CMD_RSV_7))
    {
        ret = false;
    }

    APP_PRINT_TRACE4("app_adp_cmd_ota_tooling_check %d, %d, %d, %d", ret, payload, jig_op_code, jig_id);

    return ret;
}

static void app_adp_cmd_ota_tooling_start(void)
{
    APP_PRINT_INFO2("app_adp_cmd_ota_tooling_start %d, %d", app_db.bt_is_ready, app_db.jig_subcmd);

    if (app_db.jig_subcmd == APP_ADP_SPECIAL_CMD_RWS_FORCE_ENGAGE)
    {
        app_mmi_handle_action(MMI_DEV_TOOLING_FACTORY_RESET);
    }
    else if (app_db.jig_subcmd == APP_ADP_SPECIAL_CMD_OTA)
    {
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_async_single(APP_MODULE_TYPE_OTA, APP_REMOTE_MSG_OTA_TOOLING_POWER_OFF);
        }
        app_mmi_handle_action(MMI_DEV_POWER_OFF);
    }

    app_db.ota_tooling_start = 1;
    app_mmi_modify_reboot_check_times(REBOOT_CHECK_OTA_TOOLING_MAX_TIMES);
    app_mmi_reboot_check_timer_start(2000);
}

static void app_adp_cmd_ota_tooling(uint8_t payload)
{
    bool is_valid_op_code = app_adp_cmd_ota_tooling_check(payload);

    if (is_valid_op_code == false)
    {
        // get invalid op code of jig, skip it!
        return ;
    }

    if (app_db.executing_charger_box_special_cmd == 0)
    {
#if F_APP_OTA_TOOLING_SUPPORT
        app_cmd_stop_ota_parking_power_off();
#endif

        app_db.executing_charger_box_special_cmd = app_cfg_nv.jig_subcmd;
        app_db.jig_subcmd = payload >> 5 & 0x07;
        app_db.jig_dongle_id = payload & 0x1f;

        app_dlps_stop_power_down_wdg_timer();

        if (app_db.bt_is_ready)
        {
            app_adp_cmd_ota_tooling_start();
        }
        else
        {
            app_start_timer(&timer_idx_ota_tooling_delay_start, "ota_tooling_delay_start",
                            app_adp_cmd_timer_id, APP_TIMER_OTA_TOOLING_DELAY_START, 0, false,
                            200);
        }
    }
}

void app_adp_cmd_special_cmd_handle(uint8_t jig_subcmd, uint8_t jig_dongle_id)
{
    APP_PRINT_INFO2("app_adp_cmd_special_cmd_handle: jig_subcmd 0x%x, jig_dongle_id 0x%x", jig_subcmd,
                    jig_dongle_id);

    // Do not auto power while OTA tooling mode
    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_OTA_TOOLING);

    adp_cmd_exec = AD2B_CMD_OTA_TOOLING;

    app_start_timer(&timer_idx_adp_cmd_received, "adp_cmd_received",
                    app_adp_cmd_timer_id, APP_TIMER_ADP_CMD_RECEIVED, 0, false,
                    3000);

    switch (jig_subcmd)
    {
    case APP_ADP_SPECIAL_CMD_CREATE_SPP_CON:
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
            {
                app_bt_policy_enter_ota_mode(true);
                app_ota_tooling_adv_start(app_db.jig_dongle_id, 30);
            }
            else
            {
                app_bt_policy_start_ota_shaking();
            }
        }
        break;

    case APP_ADP_SPECIAL_CMD_OTA:
    case APP_ADP_SPECIAL_CMD_FACTORY_MODE:
        {
            app_bt_policy_enter_ota_mode(true);
            app_ota_tooling_adv_start(app_db.jig_dongle_id, 30);
        }
        break;

    case APP_ADP_SPECIAL_CMD_RWS_FORCE_ENGAGE:
        {
            // TODO
        }
        break;

    default:
        break;
    }
}
#endif

static void app_adp_cmd_power_off(void)
{
    uint8_t b2s_num;
    bool roleswap_triggered;

    if (in_case_is_power_off)
    {
        in_case_is_power_off = false;
        return;
    }

    app_db.power_on_by_cmd = false;
    app_db.power_off_by_cmd = true;

    app_cfg_nv.power_on_cause_cmd = 0;
    app_cfg_nv.power_off_cause_cmd = 1;

    b2s_num = app_link_get_b2s_link_num();

    APP_PRINT_TRACE5("app_adp_cmd_power_off: bud_role %d case closed %d b2b %d remote_loc %d b2s %d",
                     app_cfg_nv.bud_role, app_db.remote_case_closed, app_db.remote_session_state, app_db.remote_loc,
                     b2s_num);

    roleswap_triggered = app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_CLOSE_CASE_POWER_OFF);

    if (roleswap_triggered == false)
    {
        app_mmi_handle_action(MMI_DEV_POWER_OFF);
    }
}

static void app_adp_cmd_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_OFF:
        {
            app_stop_timer(&timer_idx_adp_in_case);
            app_stop_timer(&timer_idx_adp_wait_roleswap);
            out_case_power_on_led_en = false;
            power_on_wait_when_bt_ready = false;
            app_db.local_in_ear = false;

            if (app_cfg_const.disallow_charging_led_before_power_off == 1)
            {
                app_db.disallow_charging_led = 0;
                APP_PRINT_INFO1("app_adp_cmd_dm_cback: disallow_charging_led = %d", app_db.disallow_charging_led);
            }

            if (app_cfg_const.enable_rtk_charging_box)
            {
                app_adp_cmd_case_bat_check(&app_db.case_battery, &app_cfg_nv.case_battery);
            }
        }
        break;

    case SYS_EVENT_POWER_ON:
        {
            app_cfg_nv.power_off_cause_cmd = 0;
            case_batt_pre_updated = false;
            in_case_is_power_off = false;

            app_db.disallow_adp_out_auto_power_on = false;

            if (app_cfg_const.disallow_charging_led_before_power_off == 1)
            {
                app_db.disallow_charging_led = 1;
            }

            if (app_cfg_const.enable_rtk_charging_box)
            {
                app_adp_cmd_case_bat_check(&app_cfg_nv.case_battery, &app_db.case_battery);
            }
        }
        break;

    default:
        break;
    }
}

static void app_adp_cmd_rtk_out_case_blink_power_on_led(bool led_on)
{
    APP_PRINT_INFO2("app_adp_cmd_rtk_out_case_blink_power_on_led: device_state %d, led_on %d",
                    app_db.device_state, led_on);

    if (led_on)
    {
        if (out_case_power_on_led_en)
        {
            out_case_power_on_led_en = false;
            app_led_change_mode(LED_MODE_POWER_ON, true, false);
        }
    }
    else
    {
        if (app_db.device_state == APP_DEVICE_STATE_ON)
        {
            out_case_power_on_led_en = true;
        }
    }
}

static void app_adp_cmd_linkback_b2s_when_open_case(void)
{
    APP_PRINT_INFO0("app_adp_cmd_linkback_b2s_when_open_case");

    app_stop_timer(&timer_idx_adp_close_case);
    app_stop_timer(&timer_idx_adp_power_off_disc_phone_link);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
#if F_APP_DURIAN_SUPPORT
        app_bt_policy_conn_all_b2s();
#else
        app_bt_policy_restore();
#endif
    }
}

void app_adp_cmd_power_on_when_out_case(void)
{
    APP_PRINT_INFO2("app_adp_cmd_power_on_when_out_case: app_db.device_state %d, power_on_wait_when_bt_ready %d",
                    app_db.device_state, power_on_wait_when_bt_ready);

    if (!app_cfg_nv.is_dut_test_mode && (power_on_wait_when_bt_ready == false))
    {
        app_mmi_handle_action(MMI_DEV_POWER_ON);
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
}

void app_adp_cmd_enter_pcba_shipping_mode(void)
{
    if (app_cfg_const.pcba_shipping_mode_pinmux != 0xFF)
    {
        Pad_OutputControlValue(app_cfg_const.pcba_shipping_mode_pinmux, PAD_OUT_HIGH);
    }
}

void app_adp_cmd_factory_reset_link_dis(uint16_t delay)
{
    app_start_timer(&timer_idx_adp_factory_reset_link_dis, "adp_facotory_reset_link_dis",
                    app_adp_cmd_timer_id, APP_TIMER_ADP_FACTORY_RESET_LINK_DIS, 0, false,
                    delay);
}

static void app_adp_cmd_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_adp_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_ADP_CLOSE_CASE:
        {
            app_stop_timer(&timer_idx_adp_in_case);
            app_stop_timer(&timer_idx_adp_close_case);

#if F_APP_DURIAN_SUPPORT
            app_durian_adv_purpose_set_none();
#endif

            app_db.power_off_cause = POWER_OFF_CAUSE_ADP_CLOSE_CASE_TIMEOUT;
            app_adp_cmd_power_off();
        }
        break;

    case APP_TIMER_ADP_CMD_RECEIVED:
        {
            app_stop_timer(&timer_idx_adp_cmd_received);
            adp_cmd_exec = AD2B_CMD_NONE;
            app_adp_cmd_clear_pending();
        }
        break;

    case APP_TIMER_ADP_POWER_ON_WAIT:
        {
            app_stop_timer(&timer_idx_adp_power_on_wait);
            power_on_wait_counts++;

            /*wait for power off state*/
            if (((app_db.device_state == APP_DEVICE_STATE_OFF) && app_db.bt_is_ready) ||
                (power_on_wait_counts > 20))
            {
                power_on_wait_counts = 0;

                if (power_on_wait_when_bt_ready)
                {
                    power_on_wait_when_bt_ready = false;
                    app_adp_cmd_power_on_handle();
#if F_APP_DURIAN_SUPPORT
                    app_durian_adv_open_case();
#endif
                }

                app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
            }
            else
            {
                app_adp_cmd_delay_power_on_when_bt_ready();
            }
        }
        break;

    case APP_TIMER_DISALLOW_CHARGER_LED:
        {
            app_stop_timer(&timer_idx_disallow_charger_led);
            app_db.disallow_charging_led = 0;
        }
        break;

    case APP_TIMER_ADP_POWER_OFF_DISC_PHONE_LINK:
        {
            app_stop_timer(&timer_idx_adp_power_off_disc_phone_link);

            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
            {
                if (app_db.remote_case_closed || (app_db.remote_loc == BUD_LOC_IN_CASE))
                {
                    //When both buds in case, src link will be disconnected 5 sec after close case.
                    app_bt_policy_disc_all_b2s();
                }
            }
        }
        break;

    case APP_TIMER_ADP_IN_CASE:
        {
            bool power_off_directly = true;

            app_stop_timer(&timer_idx_adp_in_case);
            app_db.power_off_cause = POWER_OFF_CAUSE_ADP_IN_CASE_TIMEOUT;

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                if (app_db.local_loc == BUD_LOC_IN_CASE && app_db.remote_loc == BUD_LOC_IN_CASE)
                {
                    /* when in case timeout and both buds in case; both buds need power off */
                    app_roleswap_sync_poweroff();
                    power_off_directly = false;
                }
                else
                {
                    if (app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_IN_CASE_TIMEOUT_TO_POWER_OFF))
                    {
                        /* power off after role swap */
                        power_off_directly = false;
                    }
                }
            }
#endif

            if (power_off_directly)
            {
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
            }
        }
        break;

    case APP_TIMER_ADP_WAIT_ROLESWAP:
        {
            app_stop_timer(&timer_idx_adp_wait_roleswap);
            app_mmi_handle_action(MMI_DEV_POWER_OFF);//roleswap failed,direct power off
        }
        break;

    case APP_TIMER_CHECK_ADP_CMD_DISABLE_CHARGER:
        {
            app_stop_timer(&timer_idx_check_adp_cmd_disable_charger);

            app_adp_set_disallow_enable_charger(false);

            if ((app_db.local_loc == BUD_LOC_IN_CASE) &&
                ((app_cfg_nv.case_battery & 0x7F) > app_cfg_const.smart_charger_disable_threshold))
            {
                app_adp_smart_box_charger_control(true);
            }
        }
        break;

    case APP_TIMER_CHECK_ADP_CMD_CLOSE_CASE:
        {
            app_stop_timer(&timer_idx_check_adp_cmd_close_case);

            if (app_db.local_loc == BUD_LOC_IN_CASE)
            {
                app_adp_cmd_close_case_start();
                app_adp_cmd_other_feature_handle_when_close_case();
            }
        }
        break;

    case APP_TIMER_BOX_ZERO_VOLUME_IGNORE_ADP_OUT:
        {
            app_stop_timer(&timer_idx_box_zero_volume_ignore_adp_out);

            app_db.disallow_adp_out_auto_power_on = false;

            /* change to adp high wake up before enter power down mode */
            adp_wake_up_enable(WAKE_UP_GENERAL, POL_HIGH);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_BOX_BAT);
        }
        break;

    case APP_TIMER_ADP_FACTORY_RESET_LINK_DIS:
        {
            app_stop_timer(&timer_idx_adp_factory_reset_link_dis);

            if (app_cfg_const.disallow_charging_led_before_power_off == 0 &&
                app_db.disallow_charging_led == 0)
            {
                app_db.disallow_charging_led = 1;
                app_start_timer(&timer_idx_disallow_charger_led, "disallow_charger_led",
                                app_adp_cmd_timer_id, APP_TIMER_DISALLOW_CHARGER_LED, 0, false,
                                ADP_DISABLE_CHARGER_LED_MS);
            }

            app_cfg_nv.adp_factory_reset_power_on = 1;
            app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);

            app_mmi_handle_action(MMI_DEV_FACTORY_RESET);
        }
        break;

    case APP_TIMER_POWE_OFF_ENTER_DUT_MODE:
        {
            app_stop_timer(&timer_idx_power_off_enter_dut_mode);
            app_mmi_handle_action(MMI_DUT_TEST_MODE);
        }
        break;

#if F_APP_OTA_TOOLING_SUPPORT
    case APP_TIMER_OTA_TOOLING_DELAY_START:
        {
            app_stop_timer(&timer_idx_ota_tooling_delay_start);
            if (app_db.bt_is_ready)
            {
                app_adp_cmd_ota_tooling_start();
            }
            else
            {
                app_start_timer(&timer_idx_ota_tooling_delay_start, "ota_tooling_delay_start",
                                app_adp_cmd_timer_id, APP_TIMER_OTA_TOOLING_DELAY_START, 0, false,
                                200);
            }
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static void app_adp_cmd_bud_loc_event_cback(uint32_t event, void *msg)
{
    bool from_remote = *((bool *)msg);

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.local_loc == BUD_LOC_IN_CASE) && (app_db.remote_loc == BUD_LOC_IN_CASE))
    {
        case_batt_pre_updated = false;
    }

    switch (event)
    {
    case EVENT_IN_CASE:
        {
            if (from_remote)
            {}
            else
            {
                if (app_cfg_const.do_not_auto_power_off_when_case_not_close == false)
                {
                    if (app_db.local_loc_pre != BUD_LOC_UNKNOWN)
                    {
                        if (app_db.device_state == APP_DEVICE_STATE_ON)
                        {
                            app_start_timer(&timer_idx_adp_in_case, "adp_in_case",
                                            app_adp_cmd_timer_id, APP_TIMER_ADP_IN_CASE, 0, false,
                                            ADP_IN_CASE_AUTO_POWER_OFF_MS);
                        }
                    }
                }
                app_adp_cmd_rtk_out_case_blink_power_on_led(false);

                if (app_cfg_const.smart_charger_control)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_report_rws_bud_info();
                    }
                }
            }
        }
        break;

    case EVENT_OUT_CASE:
        {
            if (from_remote)
            {}
            else
            {
                if (app_cfg_const.smart_charger_control)
                {
                    app_stop_timer(&timer_idx_check_adp_cmd_disable_charger);
                    app_adp_smart_box_charger_control(false);

                    if (app_cfg_nv.report_box_bat_lv_again_after_sw_reset)
                    {
                        /* clear this delay close charger flag if bud out box */
                        app_cfg_nv.report_box_bat_lv_again_after_sw_reset = false;
                        app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);
                    }
                }

                app_stop_timer(&timer_idx_adp_in_case);

                app_adp_cmd_rtk_out_case_blink_power_on_led(true);

                if (app_db.device_state == APP_DEVICE_STATE_ON)
                {
                    if (app_cfg_const.enable_power_off_immediately_when_close_case == false)
                    {
#if F_APP_DURIAN_SUPPORT
                        /*not linkback for out case*/
#else
                        app_adp_cmd_linkback_b2s_when_open_case();
#endif
                        app_loc_mgr_state_machine(EVENT_OPEN_CASE, 0, CAUSE_ACTION_NON);
                    }
                    else
                    {
                        app_stop_timer(&timer_idx_adp_close_case);
                    }
                }
            }
        }
        break;

    case EVENT_CLOSE_CASE:
        {
            if (from_remote)
            {
                app_adp_cmd_close_case_cmd_check();
            }
        }
        break;

    case EVENT_OPEN_CASE:
        {
            if (from_remote)
            {
                app_adp_cmd_open_case_cmd_check();
            }
        }
        break;
    default:
        break;
    }
}
#endif

uint8_t app_adp_cmd_parity_count(uint32_t data, uint8_t bit_num)
{
    uint8_t parity_count = 0;

    for (int i = 0; i < bit_num; i++)
    {
        if (((data >> i) & BIT(0)) == 1)
        {
            parity_count++;
        }
    }
    return parity_count;
}

/**
* @brief: Process bit data, currently we use odd parity check
*
*   Payload of CMD_9
*   |bit 8  |bit 7  |bit 6  |bit 5  |bit 4  |bit 3  |bit 2  |bit 1  |bit 0  |
*   |START  |                     Data(6bits)               |PARITY |END    |
*
*   Payload of CMD_15
*   |bit 14 |bit 13 |               ~               |bit 2  |bit 1  |bit 0  |
*   |START  |                     Data(12bits)              |PARITY |END    |
*
*/
static bool app_adp_cmd_process(uint32_t cmd_data, uint8_t *p_cmd_index, uint8_t *p_cmd_payload)
{
    uint8_t cmd_index = 0;
    uint8_t payload = 0;
    uint8_t cmd_parity_cnt = 0;
    uint32_t cmd_guard = 0;
    bool ret = false;

    /* Handle adp_dat to app cmd here. */
    switch (app_cfg_const.smart_charger_box_cmd_set)
    {
    case CHARGER_BOX_CMD_SET_9BITS:
        {
            T_APP_ADP_CMDPACK09 cmd_pack09;

            memcpy(&cmd_pack09, &cmd_data, sizeof(uint32_t));

            cmd_index = cmd_pack09.cmd_index;
            payload = 0;
            cmd_parity_cnt += app_adp_cmd_parity_count(cmd_index, 6);
            cmd_parity_cnt += cmd_pack09.parity_bit;

            if ((cmd_pack09.start_bit == 0) && (cmd_pack09.stop_bit == 0) &&
                (app_adp_cmd_parity_count(cmd_parity_cnt, 1) == 1))
            {
                cmd_guard = (cmd_data >> 9) & GUARD_BIT;

                if (cmd_guard == GUARD_BIT)
                {
                    *p_cmd_index = cmd_pack09.cmd_index;
                    *p_cmd_payload = payload;
                    ret = true;
                }
            }
        }
        break;

    case CHARGER_BOX_CMD_SET_15BITS:
        {
            T_APP_ADP_CMDPACK15 cmd_pack15;

            memcpy(&cmd_pack15, &cmd_data, sizeof(uint32_t));
            APP_PRINT_INFO5("app_adp_cmd_process: cmd_pack15: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
                            cmd_pack15.start_bit, cmd_pack15.cmd_index, cmd_pack15.payload,
                            cmd_pack15.parity_bit, cmd_pack15.stop_bit);

            cmd_index = cmd_pack15.cmd_index;
            payload = cmd_pack15.payload;
            cmd_parity_cnt += app_adp_cmd_parity_count(cmd_index, 4);
            cmd_parity_cnt += app_adp_cmd_parity_count(payload, 8);
            cmd_parity_cnt += cmd_pack15.parity_bit;

            if ((cmd_pack15.start_bit == 0) && (cmd_pack15.stop_bit == 0) &&
                (app_adp_cmd_parity_count(cmd_parity_cnt, 1) == 1))
            {
                cmd_guard = (cmd_data >> 15) & GUARD_BIT;

                if (cmd_guard == GUARD_BIT)
                {
                    *p_cmd_index = cmd_pack15.cmd_index;
                    *p_cmd_payload = cmd_pack15.payload;
                    ret = true;
                }
            }
        }
        break;

    default:
        break;
    }

    APP_PRINT_INFO6("app_adp_cmd_process: cmd_bits: 0x%x, cmd_data: 0x%x, cmd_guard: 0x%08x, cmd_index:0x%x payload: 0x%x, ret:%d",
                    app_cfg_const.smart_charger_box_cmd_set, cmd_data, cmd_guard, *p_cmd_index, *p_cmd_payload, ret);

    if (*p_cmd_index == 0xFF)
    {
        ret = false;
    }

    return ret;
}

static bool app_adp_cmd_check_payload_valid(uint8_t cmd_exec_index, uint8_t pay_load)
{
    bool ret = true;

    if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
    {
        if ((cmd_exec_index == AD2B_CMD_FACTORY_RESET) || (cmd_exec_index == AD2B_CMD_ENTER_PAIRING))
        {
            if (pay_load != ADP_PAYLOAD_TYPE1 && pay_load != ADP_PAYLOAD_TYPE2)
            {
                ret = false;
            }
        }

        if (cmd_exec_index == AD2B_CMD_ENTER_DUT_MODE)
        {
            if ((pay_load != ADP_PAYLOAD_TYPE2) && (pay_load != ADP_PAYLOAD_TYPE3)
#if F_APP_DURIAN_SUPPORT
                && (pay_load != ADP_PAYLOAD_AVP_OPEN) && (pay_load != ADP_PAYLOAD_AVP_CLOSE)
                && (pay_load != ADP_PAYLOAD_TYPE4)
#endif
               )
            {
                ret = false;
            }
        }
    }

    APP_PRINT_TRACE3("app_adp_cmd_check_payload_valid: ret %d, cmd_index 0x%02x, payload 0x%02x",
                     ret, cmd_exec_index, pay_load);

    return ret;
}

static bool app_adp_cmd_pre_handle(uint32_t cmd_exec_index)
{
    APP_PRINT_INFO4("app_adp_cmd_pre_handle: device_state %d, local_loc %d, adp_factory_reset_power_on %d, power_off_cause_cmd %d",
                    app_db.device_state, app_db.local_loc, app_cfg_nv.adp_factory_reset_power_on,
                    app_cfg_nv.power_off_cause_cmd);
    if (app_cfg_nv.adp_factory_reset_power_on != 0)
    {
        if (!app_cfg_nv.power_off_cause_cmd && (adp_cmd_exec == AD2B_CMD_CLOSE_CASE))
        {
            //Disable power on when received power off cmd during factory reset
            app_cfg_nv.power_off_cause_cmd = 1;
            app_cfg_store(&app_cfg_nv.remote_loc, 4);

            return false;
        }
    }

    if (adp_cmd_exec == cmd_exec_index) //same cmd,ignore
    {
        if ((cmd_exec_index != AD2B_CMD_USB_STATE) &&
            (cmd_exec_index != AD2B_CMD_CASE_CHARGER)) //need to exec many times
        {
            return false;
        }
    }

    /*firstly,check inbox status*/
    if (app_db.local_loc != BUD_LOC_IN_CASE)
    {
#if F_APP_OTA_TOOLING_SUPPORT
        app_db.local_loc = BUD_LOC_IN_CASE;
#else
        app_db.local_in_case = true;
        app_db.local_loc = app_loc_mgr_local_detect();
#endif

        if (app_db.local_loc == BUD_LOC_IN_CASE)
        {
            //do nothing
            APP_PRINT_TRACE0("app_adp_cmd_handle_msg: change from outbox to in box");
        }
        app_relay_async_single(APP_MODULE_TYPE_LOC, BUD_MSG_LOC);
    }

    return true;
}

static void app_adp_cmd_disallow_charger_led_timer_start(void)
{
    if (app_cfg_const.disallow_charging_led_before_power_off == 0)
    {
        app_db.disallow_charging_led = 1;
        app_start_timer(&timer_idx_disallow_charger_led, "disallow_charger_led",
                        app_adp_cmd_timer_id, APP_TIMER_DISALLOW_CHARGER_LED, 0, false,
                        ADP_DISABLE_CHARGER_LED_MS);
    }
}

static void app_adp_cmd_received_timer_start(void)
{
    app_start_timer(&timer_idx_adp_cmd_received, "adp_cmd_received",
                    app_adp_cmd_timer_id, APP_TIMER_ADP_CMD_RECEIVED, 0, false,
                    4000);
}

#if F_APP_ONE_WIRE_UART_SUPPORT
void app_adp_one_wire_cmd_handle_msg(uint32_t cmd_exec_index, uint8_t pay_load)
{
    APP_PRINT_INFO3("app_adp_one_wire_cmd_handle_msg: adp_cmd_rec 0x%0x, pay_load 0x%0x, adp_cmd_exec 0x%0x",
                    cmd_exec_index, pay_load, adp_cmd_exec);

    if (app_adp_cmd_pre_handle(cmd_exec_index))
    {
        app_adp_cmd_disallow_charger_led_timer_start();
        app_adp_cmd_received_timer_start();

        /*exe cmd if arrive here*/
        app_adp_cmd_exec(cmd_exec_index, pay_load);
    }
}
#endif

/**
    * @brief  App handle rx data message from peripherals of adp_det.
    * @param  cmd_data The parsed data of adp signal
    * @return void
    */
static void app_adp_cmd_handle_msg(uint32_t cmd_data)
{
    uint8_t cmd_exec_index = 0;
    uint8_t pay_load = 0;

    /* Handle adp_dat to app cmd here. */
    if (app_adp_cmd_process(cmd_data, &cmd_exec_index, &pay_load) == false ||
        app_adp_cmd_check_payload_valid(cmd_exec_index, pay_load) == false)
    {
        APP_PRINT_WARN2("app_adp_cmd_handle_msg: rec invalid cmd 0x%0x, pay_load 0x%0x", cmd_exec_index,
                        pay_load);
        return;
    }

    APP_PRINT_INFO3("app_adp_cmd_handle_msg: adp_cmd_rec 0x%0x, pay_load 0x%0x, adp_cmd_exec 0x%0x",
                    cmd_exec_index, pay_load, adp_cmd_exec);

    if (app_adp_cmd_pre_handle(cmd_exec_index))
    {
        app_adp_cmd_disallow_charger_led_timer_start();
        app_adp_cmd_received_timer_start();

        if ((app_db.local_loc != BUD_LOC_IN_CASE) && (cmd_exec_index != AD2B_CMD_OPEN_CASE))
        {
            bool pending_cmd = false;

            /* this should not happen: set cmd as pending and exe after plug*/
#if F_APP_GPIO_ONOFF_SUPPORT
            if ((app_cfg_const.box_detect_method == GPIO_DETECT) && app_device_is_in_the_box() &&
                app_gpio_on_off_debounce_timer_started())
            {
                pending_cmd = true;
            }
#endif

            if (pending_cmd)
            {
                adp_cmd_pending = cmd_exec_index;
                adp_payload_pending = pay_load;
            }
            else
            {
                app_adp_cmd_clear_pending();
            }

            if (app_adp_cmd_parse_get_usb_cmd_rec() == false)
            {
                APP_PRINT_TRACE0("app_adp_cmd_handle_msg false return");
                return;
            }
        }

        /*exe cmd if arrive here*/
        app_adp_cmd_exec(cmd_exec_index, pay_load);
    }
}

static void app_adp_cmd_device_event_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_STACK_READY:
        {
            if (app_cfg_const.smart_charger_control)
            {
                /* means reset in box happened */
                if (app_device_is_in_the_box() &&
                    (((app_cfg_const.box_detect_method == ADAPTOR_DETECT) && (app_cfg_nv.adaptor_changed == 0))
#if F_APP_GPIO_ONOFF_SUPPORT
                     || ((app_cfg_const.box_detect_method == GPIO_DETECT) && (app_cfg_nv.pre_3pin_status_unplug == 0))
#endif
                    ))
                {
                    /* disable charger according by the box bat volume */
                    if (app_cfg_nv.factory_reset_done && app_cfg_nv.disable_charger_by_box_battery)
                    {
                        app_adp_smart_box_charger_control(false);
                    }
                }

                app_adp_set_disable_charger_by_box_battery(false);

                /* this flag is set before get box bat lv before last power off;
                   we need to apply the low box bat lv to disable charger after sw reset */
                if (app_cfg_nv.report_box_bat_lv_again_after_sw_reset)
                {
                    app_cfg_nv.report_box_bat_lv_again_after_sw_reset = false;
                    app_cfg_store(&app_cfg_nv.offset_notification_vol, 8);
                    app_cfg_store(&app_cfg_nv.case_battery, 4);

                    app_adp_cmd_smart_box_update_bat_lv(app_cfg_nv.case_battery, true);
                }
            }

            if (app_cfg_const.enable_rtk_charging_box)
            {
                app_adp_cmd_case_bat_check(&app_cfg_nv.case_battery, &app_db.case_battery);
            }
        }
        break;

    default:
        break;
    }
}

bool app_adp_cmd_in_case_timeout(void)
{
    bool ret = false;

    if ((app_cfg_const.enable_rtk_charging_box == true) &&
        (app_cfg_const.do_not_auto_power_off_when_case_not_close == false) &&
        (app_db.local_loc == BUD_LOC_IN_CASE && timer_idx_adp_in_case == 0))
    {
        ret = true;
    }

    return ret;
}

/************************************ init *****************************************/
static void app_adp_cmd_parse_para_init(void)
{
    uint8_t cmd_bit_num = 0;
    uint8_t cmd_bit_length = 0;

    /* bit number */
    if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_9BITS)
    {
        cmd_bit_num = 9;
    }
    else if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
    {
        cmd_bit_num = 15;
    }

    /* bit length */
    if (app_cfg_const.smart_charger_box_bit_length == CHARGER_BOX_BIT_LENGTH_20MS)
    {
        cmd_bit_length = 20;
    }
    else if (app_cfg_const.smart_charger_box_bit_length == CHARGER_BOX_BIT_LENGTH_40MS)
    {
        cmd_bit_length = 40;
    }
    app_adp_cmd_parse_para_set(cmd_bit_num, cmd_bit_length, app_adp_cmd_handle_msg);
}

static bool app_adp_cmd_parse_enable(void)
{
    bool ret = false;

    if (app_cfg_const.enable_rtk_charging_box)
    {
        ret = true;
    }
    else
    {
#if F_APP_OTA_TOOLING_SUPPORT
        ret = true;
#endif
    }

    return ret;
}

void app_adp_cmd_init(void)
{
    if (app_adp_cmd_parse_enable())
    {
        if (!app_cfg_const.one_wire_uart_support)
        {
            app_adp_cmd_parse_para_init();
            app_adp_cmd_parse_init();
        }

        app_timer_reg_cb(app_adp_cmd_timeout_cb, &app_adp_cmd_timer_id);

        if (app_cfg_const.enable_rtk_charging_box)
        {
            sys_mgr_cback_register(app_adp_cmd_dm_cback);
            app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_adp_cmd_device_event_cback);
#if (F_APP_ERWS_SUPPORT == 1)
            app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_adp_cmd_bud_loc_event_cback);
#endif
        }
    }
}

