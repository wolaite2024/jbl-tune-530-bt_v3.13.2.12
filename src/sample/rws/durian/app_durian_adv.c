/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "ble_adv.h"
#include "ble_ext_adv.h"
#include "sysm.h"
#include "app_cfg.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_adp_cmd.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_adv_stop_cause.h"
#include "app_timer.h"


#define STOP_ADV_CAUSE_TIMEOUT  (0x31)
#define ADV_PARSE_SUPPORT  0

#define ADV_INTERVAL_MIN_MS           0x50
#define ADV_INTERVAL_MAX_MS           0x50
#define ADV_SELF_INTERVAL_MS          0x50
#define ADV_DISALLOW_UPDATE_BAT_MS    60000
#define ADV_LINKBACK_MS               500

#define ADV_FAST_PAIR_INTERVAL_MS     0x100

static T_AVP_ADV_DAT avp_adv_dat = {0};
static uint8_t adv_handle = 0xff;
static uint8_t fast_pair_adv_handle = 0xff;

static uint8_t app_durian_adv_timer_id = 0;
static uint8_t timer_idx_durian_adv_report_battery = 0;
static uint8_t timer_idx_durian_adv_disallow_update_bat = 0;
static uint8_t timer_idx_durian_adv_linkack = 0;

#define ADV_SELF_TRIGGER_MS               100
#define ADV_SELF_CASE_CLOSE_TRIGGER_MS    1
#define ADV_TRIGGER_MS                    100
#define ADV_TIMEOUT_MS                    1000
#define ADV_INTERVAL_COUNTS               15
#define ADV_COMMON_DUR_S                  30
#define ADV_AIRMAX_DUR_S                  30
#define ADV_OUT_BOX_DUR_S                 2
#define ADV_CLOSE_BOX_DUR_S               2
#define ADV_FAST_PAIR_DUR_S               5

#if (DURIAN_AIRMAX == 0)
static bool case_is_open = false;
static bool force_trig_open_case_adv = false;
#endif

static void app_durian_adv_gen_data(void);
static void app_durian_adv_start(void);

static uint8_t adv_data_fast_pair[5] =
{
    0x04,       /* length */
    0xFF,       /* type = Manufacturer Specific Data */
    0x5D, 0x00, /* company id:RTK */
    0x00,       /* user id */
};

typedef enum
{
    APP_TIMER_DURIAN_ADV_IDLE,
    APP_TIMER_DURIAN_ADV_DISALLOW_UPDATE_BAT,
    APP_TIMER_DURIAN_ADV_REPORT_BATTERY,
    APP_TIMER_DURIAN_ADV_LINKBACK,
} T_APP_DURIAN_ADV_TIMER;

static void app_durian_adv_stop(void)
{
    APP_PRINT_TRACE0("app_durian_adv_stop");
    // app_dlps_enable_auto_poweroff_stop_wdg_timer();
    ble_ext_adv_mgr_disable(adv_handle, APP_STOP_ADV_CAUSE_AVP);
    ble_ext_adv_mgr_disable(fast_pair_adv_handle, APP_STOP_ADV_CAUSE_AVP);
    durian_db.power_on_from_factory_reset = false;
}

void app_durian_adv_purpose_set(uint8_t purpose)
{
    bool trig_en = false;
    uint8_t adv_purpose_pre = durian_db.adv_purpose;
    durian_db.adv_purpose = purpose;

    if (app_cfg_const.disallow_avp_advertising != 0)
    {
        goto adv_purpose_set;
    }

    if ((app_cfg_const.avp_enable_one_key_trigger != 0) && (durian_db.id_is_display == 0))
    {
        goto adv_purpose_set;
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        goto adv_purpose_set;
    }

    switch (purpose)
    {
    case ADV_PURPOSE_PAIR_MODE:
        {
            if (adv_purpose_pre != ADV_PURPOSE_CLOSE_CASE)
            {
                trig_en = true;
            }
        }
        break;

#if (DURIAN_AIRMAX == 0)
    case ADV_PURPOSE_CLOSE_CASE:
        {
            trig_en = true;
        }
        break;
#endif

    case ADV_PURPOSE_OPEN_CASE:
        {
#if (DURIAN_AIRMAX == 0)
            if (case_is_open)
            {
                uint8_t remote_bat_adv = app_db.remote_bat_adv & 0x7f;

                if (remote_bat_adv <= 100)
                {
                    app_db.remote_batt_level = remote_bat_adv;

                    if (force_trig_open_case_adv)
                    {
                        force_trig_open_case_adv = false;
                        trig_en = true;
                    }
                    else
                    {

                        if ((durian_db.local_loc == BUD_LOC_IN_CASE) && ((app_db.remote_bat_adv >> 7) != 0) &&
                            (durian_db.remote_loc == BUD_LOC_IN_CASE))
                        {
                            trig_en = true;
                        }
                    }
                }
            }
#endif
#if DURIAN_AIRMAX
            trig_en = true;
#endif
        }
        break;

#if (DURIAN_AIRMAX == 0)
    case ADV_PURPOSE_OUT_BOX:
    case ADV_PURPOSE_IN_BOX:
        {
            if ((adv_purpose_pre != ADV_PURPOSE_PAIR_MODE) &&
                (adv_purpose_pre != ADV_PURPOSE_CLOSE_CASE) && (durian_db.remote_loc != BUD_LOC_UNKNOWN))
            {
                trig_en = true;
            }
        }
        break;
#endif
    case ADV_PURPOSE_NONE:
        {
#if DURIAN_AIRMAX
            if (adv_purpose_pre != ADV_PURPOSE_PAIR_MODE)
#endif
            {
                app_durian_adv_stop();
            }
        }
        break;

    default:
        break;
    }

    if (trig_en)
    {
        app_durian_adv_start();
    }
#if (F_APP_ERWS_SUPPORT == 1)
    app_relay_async_single(APP_MODULE_TYPE_AVP, SYNC_EVENT_DURIAN_ADV_PURPOSE);
#endif

adv_purpose_set:
#if DURIAN_AIRMAX
    APP_PRINT_TRACE4("app_durian_adv_purpose_set: %d -> %d local_loc %d trig_en %d",
                     adv_purpose_pre, durian_db.adv_purpose, durian_db.local_loc, trig_en);
#else
    APP_PRINT_TRACE7("app_durian_adv_purpose_set: %d -> %d case_is_open %d local_loc %d remote_loc %d remote_bat_adv 0x%02x trig_en %d",
                     adv_purpose_pre, durian_db.adv_purpose, case_is_open, durian_db.local_loc,
                     durian_db.remote_loc, app_db.remote_bat_adv, trig_en);
#endif
}

static uint8_t app_durian_adv_purpose_get(void)
{
    return durian_db.adv_purpose;
}

void app_durian_adv_set_power_on_cause(void)
{
    durian_db.power_on_from_factory_reset = true;

    app_adp_cmd_case_bat_check(&app_cfg_nv.case_battery, &app_db.case_battery);
    app_db.local_batt_level = app_cfg_nv.local_level;
    app_db.remote_batt_level = app_cfg_nv.remote_level;
    durian_db.local_loc = app_cfg_nv.local_loc;
    durian_db.remote_loc = app_cfg_nv.remote_loc;
}

static void app_durian_adv_fast_pair_set_dat(void)
{
    adv_data_fast_pair[2] = 0x5D;
    adv_data_fast_pair[3] = 0x00;
    adv_data_fast_pair[4] = app_cfg_const.fast_pair_id;

    ble_ext_adv_mgr_set_adv_data(fast_pair_adv_handle, sizeof(adv_data_fast_pair), adv_data_fast_pair);
}

static uint8_t app_durian_adv_get_duration(void)
{
    uint8_t duration = 0;
    if (durian_db.adv_purpose == ADV_PURPOSE_OUT_BOX)
    {
        if ((durian_db.local_loc != BUD_LOC_IN_CASE) && (durian_db.remote_loc != BUD_LOC_IN_CASE))
        {
            duration = ADV_OUT_BOX_DUR_S;
        }
        else
        {
            duration = ADV_COMMON_DUR_S;
        }
    }
    else if (durian_db.adv_purpose == ADV_PURPOSE_CLOSE_CASE)
    {
        duration = ADV_CLOSE_BOX_DUR_S;
    }
    else if (durian_db.adv_purpose == ADV_PURPOSE_PAIR_MODE) //no timeout for pairng adv
    {
        duration = ADV_COMMON_DUR_S;
    }
    else
    {
        duration = ADV_COMMON_DUR_S;
    }

    return duration;
}

static void app_durian_adv_start(void)
{
    APP_PRINT_TRACE0("app_durian_adv_start");
    app_durian_adv_gen_data();
    ble_ext_adv_mgr_disable(adv_handle, APP_STOP_ADV_CAUSE_AVP);
    ble_ext_adv_mgr_enable(adv_handle, app_durian_adv_get_duration() * 100);

    if ((durian_db.adv_purpose == ADV_PURPOSE_OPEN_CASE) ||
        (durian_db.adv_purpose == ADV_PURPOSE_PAIR_MODE))
    {
        ble_ext_adv_mgr_disable(fast_pair_adv_handle, APP_STOP_ADV_CAUSE_AVP);
        ble_ext_adv_mgr_enable(fast_pair_adv_handle, 500);
    }
}

static void app_durian_adv_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    uint8_t adv_purpose = app_durian_adv_purpose_get();

    APP_PRINT_TRACE3("app_durian_adv_timeout_cb: timer_evt 0x%02x, param %d adv_purpose %d", timer_evt,
                     param, adv_purpose);

    switch (timer_evt)
    {
    case APP_TIMER_DURIAN_ADV_REPORT_BATTERY:
        {
            app_stop_timer(&timer_idx_durian_adv_report_battery);
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_durian_adp_batt_report();
            }
        }
        break;

    case APP_TIMER_DURIAN_ADV_DISALLOW_UPDATE_BAT:
        {
            app_stop_timer(&timer_idx_durian_adv_disallow_update_bat);
            durian_db.adv_disallow_update_batt = 0;
        }
        break;

    case APP_TIMER_DURIAN_ADV_LINKBACK:
        {
            app_stop_timer(&timer_idx_durian_adv_linkack);
            app_durian_link_back();
        }
        break;

    default:
        break;
    }
}

static void app_durian_adv_serial_switch(void)
{
    durian_db.adv_serial++;
    /* use lower 3 bits to store serial number */
    if (durian_db.adv_serial == 8)
    {
        durian_db.adv_serial = 0;
    }
}

static void app_durian_adv_gen_data(void)
{
    uint8_t left_charging, right_charging;
    uint8_t left_batt, right_batt;
    uint8_t case_batt, case_charging;
    uint8_t left_batt_payload, right_batt_payload, case_batt_payload;
#if (DURIAN_AIRMAX == 0)
    uint8_t min_batt;
#endif

    if (durian_db.adv_disallow_update_batt == 0)
    {
        durian_db.local_batt = app_db.local_batt_level;
        durian_db.remote_batt = app_db.remote_batt_level;
    }
    else
    {
        /*after factory reset,nv remote battery is 0,need to updata it in time*/
        if (durian_db.remote_batt == 0)
        {
            durian_db.remote_batt = app_db.remote_batt_level;
        }
    }

    /* calculate adv serial */
    if (durian_db.adv_purpose != durian_db.adv_purpose_last)
    {
        app_durian_adv_serial_switch();

        durian_db.adv_purpose_last = durian_db.adv_purpose;

        durian_db.adv_disallow_update_batt = 1;
        app_start_timer(&timer_idx_durian_adv_disallow_update_bat, "durian_adv_disallow_update_bat",
                        app_durian_adv_timer_id, APP_TIMER_DURIAN_ADV_DISALLOW_UPDATE_BAT, 0, false,
                        ADV_DISALLOW_UPDATE_BAT_MS);
    }

#if DURIAN_AIRMAX
    left_charging = (durian_db.local_loc == BUD_LOC_IN_CASE) ? true : false;
    left_batt = durian_db.local_batt;
    left_batt_payload = (left_charging << 7) | left_batt;

    right_charging = left_charging;
    right_batt = left_batt;
    case_batt_payload = 0;
    right_batt_payload = left_batt_payload;
#else
    if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
    {
        left_charging = (durian_db.local_loc == BUD_LOC_IN_CASE) ? true : false;
        right_charging = (durian_db.remote_loc == BUD_LOC_IN_CASE) ? true : false;

        left_batt = durian_db.local_batt;
        right_batt = durian_db.remote_batt;
    }
    else
    {
        right_charging = (durian_db.local_loc == BUD_LOC_IN_CASE) ? true : false;
        left_charging = (durian_db.remote_loc == BUD_LOC_IN_CASE) ? true : false;

        right_batt = durian_db.local_batt;
        left_batt = durian_db.remote_batt;
    }

    if (app_cfg_const.smart_charger_box_cmd_set == CHARGER_BOX_CMD_SET_15BITS)
    {
        case_batt = app_db.case_battery & 0x7f;
        case_charging = ((app_db.case_battery >> 7) == 0) ? 1 : 0;
    }
    else
    {
        case_batt = 73;//for test
        case_charging = 1;
    }

    case_batt_payload = (case_charging << 7) | case_batt;
    if (abs(right_batt - left_batt) >= 10)
    {
        left_batt_payload = (left_charging << 7) | left_batt;
        right_batt_payload = (right_charging << 7) | right_batt;
    }
    else
    {
        min_batt = (right_batt <= left_batt) ? right_batt : left_batt;
        left_batt_payload = (left_charging << 7) | min_batt;
        right_batt_payload = (right_charging << 7) | min_batt;
    }
#endif

    avp_adv_dat.purpose = durian_db.adv_purpose;
    avp_adv_dat.serial = durian_db.adv_serial;

    avp_adv_dat.local_is_in_case = (durian_db.local_loc == BUD_LOC_IN_CASE) ? true : false;
    avp_adv_dat.remote_is_in_case = (durian_db.remote_loc == BUD_LOC_IN_CASE) ? true : false;

    avp_adv_dat.left_charging = left_charging;
    avp_adv_dat.left_batt = left_batt;
    avp_adv_dat.right_charging = right_charging;
    avp_adv_dat.right_batt = right_batt;

    avp_adv_dat.case_batt_payload = case_batt_payload;
    avp_adv_dat.left_batt_payload = left_batt_payload;
    avp_adv_dat.right_batt_payload = right_batt_payload;
    avp_adv_dat.local_addr = app_cfg_nv.bud_local_addr;

    durian_adv_gen_data(&avp_adv_dat);
    ble_ext_adv_mgr_set_adv_data(adv_handle, avp_adv_dat.len, avp_adv_dat.dat);
}

void app_durian_adv_out_box(void)
{
    app_durian_adv_purpose_set(ADV_PURPOSE_OUT_BOX);
}

void app_durian_adv_in_box(void)
{
    app_durian_adv_purpose_set(ADV_PURPOSE_IN_BOX);
}

void app_durian_adv_close_case(bool from_remote)
{
    app_durian_adv_purpose_set(ADV_PURPOSE_CLOSE_CASE);
    durian_db.adv_serial = 0;

#if (DURIAN_AIRMAX == 0)
    if (!from_remote)
    {
        case_is_open = false;
    }
#endif

    durian_db.power_on_from_factory_reset = false;

    app_start_timer(&timer_idx_durian_adv_report_battery, "durian_adv_report_battery",
                    app_durian_adv_timer_id, APP_TIMER_DURIAN_ADV_REPORT_BATTERY, 0, false,
                    10000);
}

void app_durian_adv_roleswap(void)
{
    uint8_t adv_purpose = app_durian_adv_purpose_get();

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_durian_adv_stop();
    }
    else
    {
        if (adv_purpose != ADV_PURPOSE_NONE)
        {
            app_durian_adv_purpose_set(adv_purpose);
        }
    }
}

void app_durian_adv_pairing_finish(void)
{
    durian_db.power_on_from_factory_reset = false;

#if DURIAN_AIRMAX
    app_durian_adv_purpose_set(ADV_PURPOSE_OPEN_CASE);
    if (app_link_get_b2s_link_num() != 0)
    {
        durian_db.need_open_case_adv = 1;
    }
#else
    /*after pairing finish, need to stop adv and advertising open case*/
    if (case_is_open)
    {
        if ((durian_db.local_loc != BUD_LOC_IN_CASE) && (durian_db.remote_loc != BUD_LOC_IN_CASE))
        {
            app_durian_adv_purpose_set(ADV_PURPOSE_NONE);
        }
        else
        {
            force_trig_open_case_adv = true;
            /*after pairing finish, need to stop adv and advertising open case*/
            app_durian_adv_purpose_set(ADV_PURPOSE_OPEN_CASE);
        }
    }
#endif
}

void app_durian_adv_open_case(void)
{
#if (DURIAN_AIRMAX == 0)
    case_is_open = true;
#endif
    app_durian_adv_purpose_set(ADV_PURPOSE_OPEN_CASE);
}

void app_durian_adv_pairing_timeout(void)
{
    if (app_cfg_const.enable_power_on_linkback_fail_enter_pairing == 0)
    {
        app_start_timer(&timer_idx_durian_adv_linkack, "durian_adv_linkback",
                        app_durian_adv_timer_id, APP_TIMER_DURIAN_ADV_LINKBACK, 0, false,
                        ADV_LINKBACK_MS);
    }
}

void app_durian_adv_factory_reset(void)
{
    app_durian_adv_purpose_set(ADV_PURPOSE_NONE);
}

static void app_durian_adv_cb(uint8_t event, void *dat)
{
    T_BLE_EXT_ADV_CB_DATA adv_state = {0};
    memcpy(&adv_state, dat, sizeof(T_BLE_EXT_ADV_CB_DATA));

    if (adv_state.p_ble_state_change == NULL)
    {
        return;
    }

    APP_PRINT_INFO4("app_durian_adv_cb: handle %d event %d state %d stop_cause %d",
                    adv_state.p_ble_state_change->adv_handle, event,
                    adv_state.p_ble_state_change->state, adv_state.p_ble_state_change->stop_cause);

    switch (event)
    {
    case BLE_EXT_ADV_STATE_CHANGE:
        {
            if ((adv_state.p_ble_state_change->state == BLE_ADV_STATE_IDLE) &&
                (adv_state.p_ble_state_change->stop_cause == BLE_ADV_STOP_CAUSE_TIMEOUT) &&
                (adv_state.p_ble_state_change->adv_handle == adv_handle))
            {
                if (durian_db.adv_purpose == ADV_PURPOSE_CLOSE_CASE)
                {
                    app_stop_timer(&timer_idx_durian_adv_report_battery);
                    app_start_timer(&timer_idx_durian_adv_report_battery, "durian_adv_report_battery",
                                    app_durian_adv_timer_id, APP_TIMER_DURIAN_ADV_REPORT_BATTERY, 0, false,
                                    10000);
                    app_durian_adp_batt_report();
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_durian_adv_le_init(void)
{
    T_LE_EXT_ADV_LEGACY_ADV_PROPERTY adv_event_prop =
        LE_EXT_ADV_LEGACY_ADV_NON_SCAN_NON_CONN_UNDIRECTED;

    uint16_t adv_interval_min = ADV_INTERVAL_MIN_MS, adv_interval_max = ADV_INTERVAL_MAX_MS;

    adv_interval_min = ADV_INTERVAL_MIN_MS;
    adv_interval_max = ADV_INTERVAL_MAX_MS;

    T_GAP_LOCAL_ADDR_TYPE own_address_type = GAP_LOCAL_ADDR_LE_PUBLIC;
    T_GAP_REMOTE_ADDR_TYPE peer_address_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  peer_address[6] = {0, 0, 0, 0, 0, 0};
    T_GAP_ADV_FILTER_POLICY filter_policy = GAP_ADV_FILTER_ANY;

    ble_ext_adv_mgr_init_adv_params(&adv_handle, adv_event_prop, adv_interval_min,
                                    adv_interval_max, own_address_type, peer_address_type, peer_address,
                                    filter_policy, sizeof(avp_adv_dat.dat), (uint8_t *) &avp_adv_dat.dat,
                                    0, NULL, NULL);

    ble_ext_adv_mgr_init_adv_params(&fast_pair_adv_handle, adv_event_prop, ADV_FAST_PAIR_INTERVAL_MS,
                                    ADV_FAST_PAIR_INTERVAL_MS, own_address_type, peer_address_type, peer_address,
                                    filter_policy, sizeof(adv_data_fast_pair), (uint8_t *) &adv_data_fast_pair,
                                    0, NULL, NULL);
    app_durian_adv_fast_pair_set_dat();

    ble_ext_adv_mgr_register_callback(app_durian_adv_cb, adv_handle);
    ble_ext_adv_mgr_register_callback(app_durian_adv_cb, fast_pair_adv_handle);

    ble_ext_adv_mgr_change_adv_tx_power(adv_handle, 10);
}
static void app_durian_adv_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;

    switch (event_type)
    {
    case SYS_EVENT_POWER_OFF:
        {
            app_durian_adv_purpose_set(ADV_PURPOSE_NONE);
#if (DURIAN_AIRMAX == 0)
            case_is_open = false;
#endif
        }
        break;

    case SYS_EVENT_POWER_ON:
        {
            durian_db.adv_purpose = ADV_PURPOSE_NONE;
            durian_db.adv_purpose_last = ADV_PURPOSE_NONE;
            durian_db.adv_serial = 0;
            durian_db.remote_loc_received = false;

#if (DURIAN_AIRMAX == 0)
            case_is_open = true;
#endif

#if DURIAN_AIRMAX
            if (app_durian_adv_purpose_get() != ADV_PURPOSE_PAIR_MODE)
            {
                app_durian_adv_purpose_set(ADV_PURPOSE_OPEN_CASE);
            }
#endif
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_durian_adv_dm_cback: event_type 0x%04x", event_type);
    }
}

#if (F_APP_ERWS_SUPPORT == 1)
void app_durian_adv_bud_connected(void)
{
    uint8_t remote_bat_adv = app_db.remote_bat_adv & 0x7f;

    durian_db.role_decided = 1;

    if (remote_bat_adv <= 100)
    {
        app_db.remote_batt_level = remote_bat_adv;
    }

    if (durian_db.power_on_from_factory_reset)
    {
        app_durian_adv_purpose_set(ADV_PURPOSE_PAIR_MODE);
    }
    else
    {
        app_durian_adv_purpose_set(ADV_PURPOSE_OPEN_CASE);
    }
}
#endif

void app_durian_adv_enter_pairing(void)
{
    app_mmi_handle_action(MMI_DEV_FORCE_ENTER_PAIRING_MODE);
    app_durian_adv_purpose_set(ADV_PURPOSE_PAIR_MODE);
}

void app_durian_mmi_adv_enter_pairing(void)
{
#if DURIAN_AIRMAX
    app_durian_adv_purpose_set(ADV_PURPOSE_PAIR_MODE);
#endif
}

void app_durian_adv_purpose_set_none(void)
{
    app_durian_adv_purpose_set(ADV_PURPOSE_NONE);
}

void app_durian_adv_purpose_set_open_case(void)
{
    app_durian_adv_purpose_set(ADV_PURPOSE_OPEN_CASE);
}

void app_durian_adv_init(void)
{
    app_durian_adv_le_init();
    app_timer_reg_cb(app_durian_adv_timeout_cb, &app_durian_adv_timer_id);
    sys_mgr_cback_register(app_durian_adv_dm_cback);
}
#endif
