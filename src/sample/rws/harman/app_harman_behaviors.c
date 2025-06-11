#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "transmit_service.h"
#include "gap_conn_le.h"
#include "pm.h"
#include "bt_bond.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_bond.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_multilink.h"
#include "app_ble_common_adv.h"
#include "app_harman_vendor_cmd.h"
#include "app_harman_ble.h"
#include "app_ble_gap.h"
#include "app_harman_behaviors.h"
#include "app_harman_ble_ota.h"
#include "app_ble_device.h"

static uint8_t harman_timer_id = 0;
static uint8_t timer_idx_harman_notify_device_info = 0;
static uint8_t timer_idx_harman_power_off_option = 0;
static uint8_t timer_idx_harman_heartbeat_check = 0;

uint8_t aling_active_a2dp_hfp_index = 0xff;
static bool is_record_a2dp_active_ever = false;
static bool power_on_link_back_fg = false;
static bool is_harman_connect_only_one = false;
bool gfps_finder_adv_ei_and_hash = false;

static uint8_t heartbeat_check_times = 0;

typedef enum
{
    APP_TIMER_HARMAN_POWER_OFF_OPTION,
    APP_TIMER_HARMAN_NOTIFY_DEVICEINFO,
    APP_TIMER_MULTILINK_HARMAN_A2DP_STOP,
    APP_TIMER_HEARTBEAT_CHECK,
} T_HARMAN_TIMER;

void app_harman_set_cpu_clk(bool evt)
{
    APP_PRINT_TRACE2("app_harman_set_cpu_clk %d, %d", gfps_finder_adv_ei_and_hash, evt);
    gfps_finder_adv_ei_and_hash = evt;
}

void app_harman_cpu_clk_improve(void)
{
    uint32_t default_cpu_freq = pm_cpu_freq_get();
    uint32_t cpu_freq = 0;
    int32_t ret = 0;

    if ((app_db.tone_vp_status.state == APP_TONE_VP_STOP) &&
        (gfps_finder_adv_ei_and_hash == false))
    {
        ret = pm_cpu_freq_set(40, &cpu_freq);
    }
    else
    {
        if (default_cpu_freq != 100)
        {
            ret = pm_cpu_freq_set(100, &cpu_freq);
        }
    }

    APP_PRINT_TRACE5("cpu freq config ret %x default freq %dMHz real freq %dMHz %d,%d", ret,
                     default_cpu_freq, cpu_freq, app_db.tone_vp_status.state, gfps_finder_adv_ei_and_hash);
}

void app_harman_heartbeat_check_times_set(uint8_t count)
{
    heartbeat_check_times = count;
}

void app_harman_heartbeat_check_timer_start(uint32_t time)
{
    app_start_timer(&timer_idx_harman_heartbeat_check, "heartbeat_check",
                    harman_timer_id, APP_TIMER_HEARTBEAT_CHECK, 0, false,
                    time);
}

void app_harman_heartbeat_check_timer_stop(void)
{
    if (timer_idx_harman_heartbeat_check)
    {
        app_stop_timer(&timer_idx_harman_heartbeat_check);
    }
}

static void app_harman_power_off_option_timer_start(void)
{
    app_start_timer(&timer_idx_harman_power_off_option, "harman_power_off_option",
                    harman_timer_id, APP_TIMER_HARMAN_POWER_OFF_OPTION, 0, false,
                    app_cfg_nv.auto_power_off_time * 1000);
}

static void app_harman_power_off_option_timer_stop(void)
{
    if (timer_idx_harman_power_off_option)
    {
        app_stop_timer(&timer_idx_harman_power_off_option);
    }
}

uint8_t app_harman_power_off_option_timer_get(void)
{
    return timer_idx_harman_power_off_option;
}

void app_harman_notify_device_info_timer_start(uint32_t time)
{
    app_start_timer(&timer_idx_harman_notify_device_info, "harman_notify_device_info",
                    harman_timer_id, APP_TIMER_HARMAN_NOTIFY_DEVICEINFO, 0, false,
                    time);
}

void app_harman_notify_device_info_timer_stop(void)
{
    if (timer_idx_harman_notify_device_info)
    {
        app_stop_timer(&timer_idx_harman_notify_device_info);
    }
}

static void app_harman_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE3("app_harman_timeout_cb: timer_evt: %d, param: %d, heartbeat_check_times: %d",
                     timer_evt, param, heartbeat_check_times);
    switch (timer_evt)
    {
    case APP_TIMER_HARMAN_POWER_OFF_OPTION:
        {
            app_harman_power_off_option_timer_stop();
            if (app_db.device_state == APP_DEVICE_STATE_ON)
            {
                if (app_harman_ble_ota_get_upgrate_status())
                {
                    if ((app_hfp_sco_is_connected() == false && app_hfp_get_call_status() == BT_HFP_CALL_IDLE)
                        && (app_db.br_link[app_a2dp_get_active_idx()].streaming_fg == false))
                    {
                        app_harman_connect_idle_to_power_off(CONNECT_IDLE_POWER_OFF_START, 0xff);
                    }
                }
                else
                {
                    app_mmi_handle_action(MMI_DEV_POWER_OFF);
                }
            }
        }
        break;

    case APP_TIMER_HARMAN_NOTIFY_DEVICEINFO:
        {
            app_stop_timer(&timer_idx_harman_notify_device_info);

            uint16_t mtu_size;
            T_GAP_CAUSE cause = GAP_CAUSE_ERROR_UNKNOWN;
            uint8_t ble_conn_id = app_ble_common_adv_get_conn_id();
            uint8_t ble_paired_status = 0x00;

            if ((app_db.le_link[ble_conn_id].state == LE_LINK_STATE_CONNECTED) &&
                (app_db.le_link[ble_conn_id].used == true))
            {
                ble_paired_status = 0x01;
            }
            app_harman_ble_paired_status_notify(ble_paired_status, ble_conn_id);

            app_harman_ble_heartbeat_notify();

            cause = le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, &mtu_size, ble_conn_id);

            APP_PRINT_TRACE4("APP_TIMER_HARMAN_NOTIFY_DEVICEINFO: ble_conn_id: %d, ble_paired_status: %d, cause: %d, mtu_size: %d",
                             ble_conn_id, ble_paired_status, cause, mtu_size);
            if (40 <= (mtu_size - 3))
            {
                app_harman_devinfo_notify(ble_conn_id);
            }
            else
            {
                app_harman_notify_device_info_timer_start(HARMAN_NOTIFY_DEVICE_INFO_TIME);
            }
        }
        break;

    case APP_TIMER_HEARTBEAT_CHECK:
        {
            app_harman_heartbeat_check_timer_stop();

            uint8_t check_max_times = HARMAN_HEARTBEAT_CHECK_MAX_TIME / HARMAN_HEARTBEAT_CHECK_TIME;
            if (heartbeat_check_times >= check_max_times)
            {
                heartbeat_check_times = 0;
                app_ble_rtk_adv_start();
            }
            else
            {
                heartbeat_check_times ++;
                app_harman_ble_heartbeat_notify();
            }
        }
        break;

    default:
        break;
    }
}

void app_harman_aling_active_hfp_idx_set(uint8_t active_idx)
{
    if (active_idx == 0xff)
    {
        if (app_link_get_connected_src_num() == 0)
        {
            aling_active_a2dp_hfp_index = 0xff;
        }
        else if (app_link_get_connected_src_num() == 1)
        {
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    aling_active_a2dp_hfp_index = i;
                }
            }
        }
    }
    else
    {
        aling_active_a2dp_hfp_index = active_idx;
    }
    APP_PRINT_TRACE1("app_harman_aling_active_hfp_idx_set: %d", aling_active_a2dp_hfp_index);
}

uint8_t app_harman_aling_active_hfp_idx_get(void)
{
    APP_PRINT_TRACE1("app_harman_aling_active_hfp_idx_get: %d", aling_active_a2dp_hfp_index);
    return aling_active_a2dp_hfp_index;
}

void app_harman_silent_check(uint8_t set_idx, uint8_t silent_check)
{
    APP_PRINT_TRACE3("app_harman_silent_check set_idx=%d, silent_check=%d, app_multi_get_active_idx=%d",
                     set_idx, silent_check, app_multi_get_active_idx());
    //silent = true
    //un_silent = false
    if (silent_check == 0xff)
    {
        app_db.br_link[set_idx].harman_silent_check = false;
    }
    else if (app_db.br_link[set_idx].harman_silent_check != silent_check)
    {
        if ((app_db.br_link[set_idx].harman_silent_check == true) &&
            (silent_check == false) &&
            (set_idx == app_multi_get_active_idx()))
        {
            //active silent->un_silent need update
            app_harman_aling_active_hfp_idx_set(app_multi_get_active_idx());
        }
        app_db.br_link[set_idx].harman_silent_check = silent_check;
    }
}

void app_harman_is_record_a2dp_active_ever_set(bool res)
{
    is_record_a2dp_active_ever = res;
    //APP_PRINT_TRACE1("app_harman_is_record_a2dp_active_ever_set: %d", is_record_a2dp_active_ever);
}

bool app_harman_is_record_a2dp_active_ever_get(void)
{
    //APP_PRINT_TRACE1("app_harman_is_record_a2dp_active_ever_get: %d", is_record_a2dp_active_ever);
    return is_record_a2dp_active_ever;
}

void app_harman_power_on_link_back_flag_set(bool res)
{
    power_on_link_back_fg = res;
//    APP_PRINT_TRACE1("app_harman_power_on_link_back_flag_set: %d", power_on_link_back_fg);
}

bool app_harman_power_on_link_back_flag_get(void)
{
    APP_PRINT_TRACE1("app_harman_power_on_link_back_flag_get: %d", power_on_link_back_fg);
    return power_on_link_back_fg;
}

void app_harman_connect_idle_to_power_off(T_CONNECT_IDLE_POWER_OFF action, uint8_t index)
{
    APP_PRINT_INFO4("app_harman_connect_idle_to_power_off: action: %d , index: %d, en: %d, timer: %d",
                    action, index, app_cfg_nv.auto_power_off_status,
                    app_cfg_nv.auto_power_off_time);

    if (action == CONNECT_IDLE_POWER_OFF_START)
    {
        if (app_cfg_nv.auto_power_off_status)
        {
            app_harman_power_off_option_timer_stop();
            app_harman_power_off_option_timer_start();
        }
    }
    else if (action == DISC_RESET_TO_COUNT)
    {
        uint8_t app_idx;
        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if ((app_link_check_b2s_link_by_id(app_idx)) &&
                ((app_db.br_link[app_idx].connected_profile & ALL_PROFILE_MASK) != 0))
            {
//                APP_PRINT_TRACE2("app_harman_connect_idle_to_power_off: find_index %d, sniffmode_fg %d",
//                                 app_idx, app_db.br_link[app_idx].acl_link_in_sniffmode_flg);
                break;
            }
        }

        if ((app_cfg_nv.auto_power_off_status) && (app_idx != MAX_BR_LINK_NUM))
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(app_db.br_link[app_idx].bd_addr);

            app_harman_power_off_option_timer_stop();
            if (p_link->acl_link_in_sniffmode_flg)
            {
                app_harman_power_off_option_timer_start();
            }
        }
    }
    else if (action == ACTIVE_NEED_STOP_COUNT)
    {
        app_harman_power_off_option_timer_stop();
    }
}

void app_harman_dump_link_information(void)
{
    uint8_t bd_addr[6];
    uint8_t bond_num;
    uint32_t bond_flag;
    uint32_t plan_profs;

    bond_num = app_bond_b2s_num_get();

    for (uint8_t i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (app_bond_b2s_addr_get(i, bd_addr))
        {
            if (memcmp(bd_addr, app_cfg_nv.bud_peer_addr, 6) &&
                memcmp(bd_addr, app_cfg_nv.bud_local_addr, 6))
            {
                bt_bond_flag_get(bd_addr, &bond_flag);
            }
        }

        APP_PRINT_INFO3("app_harman_dump_link_information: dump priority: %d, bond_flag: %d, addr: %s",
                        i, bond_flag, TRACE_BDADDR(bd_addr));
    }
}

uint8_t app_harman_ever_link_information(void)
{
    uint8_t bd_addr[6];
    uint8_t count_ever_link = 0;
    uint8_t bond_num = app_bond_b2s_num_get();
    uint32_t bond_flag;
    uint32_t plan_profs;

    for (uint8_t i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (app_bond_b2s_addr_get(i, bd_addr))
        {
            if (memcmp(bd_addr, app_cfg_nv.bud_peer_addr, 6) &&
                memcmp(bd_addr, app_cfg_nv.bud_local_addr, 6))
            {
                bt_bond_flag_get(bd_addr, &bond_flag);

                if (bond_flag != 0)
                {
                    count_ever_link++;
                }
            }
        }
    }

//    APP_PRINT_INFO1("app_harman_ever_link_information count =%d", count_ever_link);
    return count_ever_link;
}

void app_harman_pairable_mode_set(uint8_t enable)
{
    uint8_t pair_mode = enable;
//    APP_PRINT_INFO1("app_harman_pairable_mode_set %d", pair_mode);
    gap_set_param(GAP_PARAM_BOND_BR_PAIRING_MODE, sizeof(uint8_t), &pair_mode);
    gap_set_pairable_mode();
}

void app_harman_is_already_connect_one_set(bool res)
{
    //pairing mode need reset
    is_harman_connect_only_one = res;
//    APP_PRINT_TRACE1("app_harman_is_already_connect_one_set =%d", is_harman_connect_only_one);
}

bool app_harman_is_already_connect_one_get(void)
{
    return is_harman_connect_only_one;
}

void app_harman_acl_disconn_handle(uint8_t *bd_addr)
{
    app_harman_connect_idle_to_power_off(DISC_RESET_TO_COUNT, 0xff);
    app_harman_aling_active_hfp_idx_set(0xff);

    app_harman_remote_device_name_crc_set(0xFF, bd_addr, false);
    app_harman_le_common_adv_update();
}

void app_harman_sco_handle(uint8_t app_idx, bool is_sco_conn)
{
    app_harman_sco_status_notify();

    if (is_sco_conn)
    {
        app_harman_connect_idle_to_power_off(ACTIVE_NEED_STOP_COUNT, app_idx);
    }
    else
    {
        if ((app_hfp_sco_is_connected() == false && app_hfp_get_call_status() == APP_CALL_IDLE)
            && (app_db.br_link[app_a2dp_get_active_idx()].streaming_fg == false))
        {
            app_harman_connect_idle_to_power_off(CONNECT_IDLE_POWER_OFF_START, app_idx);
        }
    }
}

void app_harman_a2dp_stop_handle(uint8_t app_idx)
{
    app_harman_silent_check(app_idx, 0xff);

    if ((app_hfp_sco_is_connected() == false && app_hfp_get_call_status() == APP_CALL_IDLE)
        && (app_db.br_link[app_a2dp_get_active_idx()].streaming_fg == false))
    {
        app_harman_connect_idle_to_power_off(CONNECT_IDLE_POWER_OFF_START, app_idx);
    }
}

void app_harman_behaviors_init(void)
{
    app_timer_reg_cb(app_harman_timeout_cb, &harman_timer_id);
}
