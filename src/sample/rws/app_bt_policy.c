/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include <stdlib.h>
#include "stdlib_corecrt.h"
#include "trace.h"
#include "os_sched.h"
#include "stdlib.h"
#include "app_timer.h"
#include "gap_br.h"
#include "gap_le.h"
#include "btm.h"
#include "bt_avrcp.h"
#include "bt_spp.h"
#include "bt_bond.h"
#include "bt_rdtp.h"
#include "bt_hfp.h"
#include "sysm.h"
#include "pm.h"
#include "anc_tuning.h"
#include "engage.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_main.h"
#include "app_relay.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#include "app_sniff_mode.h"
#include "app_ble_gap.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_linkback.h"
#include "app_multilink.h"
#include "app_roleswap.h"
#include "app_audio_policy.h"
#include "app_mmi.h"
#include "app_relay.h"
#include "app_bond.h"
#include "app_auto_power_off.h"
#include "app_cmd.h"
#include "app_adv_stop_cause.h"
#include "app_ble_whitelist.h"

#if F_APP_GAMING_DONGLE_SUPPORT || F_APP_COMMON_DONGLE_SUPPORT
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"
#endif

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_ble.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#if F_APP_NFC_SUPPORT
#include "app_nfc.h"
#endif

#include "app_link_util.h"
#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xiaoai_transport.h"
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "app_xiaowei_transport.h"
#endif

#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_lea_adv.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_mcp.h"
#include "app_lea_unicast_audio.h"
#endif

#include "audio_type.h"
#include "app_adp_cmd.h"
#include "app_sniff_mode.h"
#include "app_sdp.h"
#include "app_hfp.h"
#include "app_ota.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_asp_device.h"
#include "app_teams_audio_policy.h"
#include "app_teams_cmd.h"
#if F_APP_TEAMS_BT_POLICY
#include "app_teams_bt_policy.h"
#endif
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "os_mem.h"
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
#include "app_ble_swift_pair.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_spp.h"
#include "app_dongle_record.h"
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_QOL_MONITOR_SUPPORT
#include "app_qol.h"
#endif

#if F_APP_USB_AUDIO_SUPPORT
#include "app_usb.h"
#endif

#if F_APP_CCP_SUPPORT
#include "app_lea_ccp.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO
#include "app_sensor_mems.h"
#endif

#if F_APP_AIRPLANE_SUPPORT
#include "app_airplane.h"
#endif

#if F_APP_SAIYAN_MODE
#include "app_data_capture.h"
#endif

#if GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#include "app_ipc.h"

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#include "app_harman_behaviors.h"
#endif

#include "app_bt_point.h"

#if HARMAN_DISCONN_ACTIVE_A2DP_WHEN_OTA
#include "app_harman_ble_ota.h"
#endif

#define ROLE_SWITCH_COUNT_MAX                 (5)
#define ROLE_SWITCH_DELAY_MS                  (250)
#define ROLE_SWITCH_WINDOW_DELAY_MS           (2000)

#define NORMAL_PAGESCAN_INTERVAL              (0x800)
#define NORMAL_PAGESCAN_WINDOW                (0x12)

#define PRI_FAST_PAGESCAN_INTERVAL            (0x200)
#define PRI_FAST_PAGESCAN_WINDOW              (0x12)

#define PRI_VERY_FAST_PAGESCAN_INTERVAL       (0x40)
#define PRI_VERY_FAST_PAGESCAN_WINDOW         (0x12)

#define SEC_FAST_PAGESCAN_INTERVAL            (0x300)
#define SEC_FAST_PAGESCAN_WINDOW              (0x90)

#define PRIMARY_BLE_ADV_INTERVAL_VOICE        (0x20)
#define PRIMARY_BLE_ADV_INTERVAL_AUDIO        (0xa0)
#define PRIMARY_BLE_ADV_INTERVAL_NORMAL       (0xd0)

#define SECONDARY_BLE_ADV_INTERVAL_NORMAL     (0xb0)

#define PRIMARY_WAIT_COUPLING_TIMEOUT_MS      (500)

#define SRC_CONN_IND_MAX_NUM                  (2)
#define SRC_CONN_IND_DELAY_MS                 (1500)

#define SHUT_DOWN_STEP_TIMER_MS               (100)
#define DISCONN_B2S_PROFILE_WAIT_TIMES        (4) /* wait time(ms): 4 * SHUT_DOWN_STEP_TIMER_MS */
#define DISCONN_B2S_LINK_WAIT_TIMES_MAX       (4) /* max wait time(ms): 4 * SHUT_DOWN_STEP_TIMER_MS */
#define DISCONN_B2B_LINK_WAIT_TIMES_MAX       (3) /* max wait time(ms): 3 * SHUT_DOWN_STEP_TIMER_MS */
#define TPOLL_RESET_MAX_NUM                   (1) /* max number for reset tpoll config */

#define GOLDEN_RANGE_B2B_MAX                  (-45)
#define GOLDEN_RANGE_B2B_MIN                  (-55)
#define GOLDEN_RANGE_B2S_MAX                  (10)
#define GOLDEN_RANGE_B2S_MIN                  (0)

#define GAMING_DONGLE_VID                     0x0bda
#define GAMING_DONGLE_PID1                    0x875a
#define GAMING_DONGLE_PID2                    0x875b

#define CONFERENCE_DONGLE_VID                 0x0bda
#define CONFERENCE_DONGLE_PID                 0x875c

#define GAMING_DONGLE_COD                     0x240100

extern T_LINKBACK_ACTIVE_NODE linkback_active_node;
static uint8_t app_bt_policy_freq_handle = 0;
static bool b2b_connected = 0;

T_STATE cur_state = STATE_SHUTDOWN;
T_BP_STATE bp_state = BP_STATE_IDLE;
T_BP_STATE pri_bp_state = BP_STATE_IDLE;
T_EVENT cur_event;

T_BT_DEVICE_MODE radio_mode = BT_DEVICE_MODE_IDLE;

typedef enum
{
    APP_TIMER_SHUTDOWN_STEP                   = 0x00,
    APP_TIMER_FIRST_ENGAGE_CHECK              = 0x01,
    APP_TIMER_PAIRING_MODE                    = 0x03,
    APP_TIMER_DISCOVERABLE                    = 0x04,
    APP_TIMER_ROLE_SWITCH                     = 0x05,
    APP_TIMER_BUD_LINKLOST                    = 0x06,
    APP_TIMER_LINKBACK                        = 0x07,
    APP_TIMER_LINKBACK_DELAY                  = 0x09,
    APP_TIMER_LATER_AVRCP                     = 0x0a,
    APP_TIMER_WAIT_COUPLING                   = 0x0b,
    APP_TIMER_ENGAGE_ACTION_ADJUST            = 0x0c,
    APP_TIMER_LATER_HID                       = 0x0d,
    APP_TIMER_RECONNECT                       = 0x0e,
    APP_TIMER_PAGE_SCAN_PARAM                 = 0x0f,
    APP_TIMER_SRC_CONN_IND                    = 0x10,
    APP_TIMER_B2S_CONN                        = 0x11,
    APP_TIMER_ROLESWAP                        = 0x12,
    APP_TIMER_SET_CPU_CLK                     = 0x13,
} T_APP_BT_POLICY_TIMER;

typedef struct
{
    uint8_t timer_idx;
    uint8_t bd_addr[6];
} T_SRC_CONN_IND;

static bool engage_done = false;
bool first_connect_sync_default_volume_to_src = false;

static bool dedicated_enter_pairing_mode_flag = false;
static bool is_visiable_flag = false;
static bool is_force_flag = false;
static bool startup_linkback_done_flag = false;
static bool is_pairing_timeout = false;
static bool bond_list_load_flag = false;
static bool linkback_flag = false;
static bool is_user_action_pairing_mode = false;
static uint16_t last_src_conn_idx = 0;
static bool disconnect_for_pairing_mode = false;
static uint16_t linkback_retry_timeout = 0;
static bool after_stop_sdp_todo_linkback_run_flag = false;
static bool discoverable_when_one_link = false;
static bool is_src_authed = false;
static bool roleswap_suc_flag = false;

static uint8_t bt_policy_timer_id = 0;
static uint8_t timer_idx_shutdown_step = 0;
static uint8_t timer_idx_first_engage_check = 0;
static uint8_t timer_idx_pairing_mode = 0;
static uint8_t timer_idx_discoverable = 0;
static uint8_t timer_idx_bud_linklost = 0;
static uint8_t timer_idx_linkback = 0;
static uint8_t timer_idx_linkback_delay = 0;
static uint8_t timer_idx_wait_coupling = 0;
static uint8_t timer_idx_engage_action_adjust = 0;
static uint8_t timer_idx_page_scan_param = 0;
static uint8_t timer_idx_reconnect = 0;
static uint8_t timer_idx_b2s_conn = 0;
// static uint8_t timer_idx_roleswap = 0;
static uint8_t timer_idx_increase_cpu_clk = 0;
static uint8_t timer_idx_role_switch[MAX_BR_LINK_NUM] = {0};
static uint8_t timer_idx_later_avrcp[MAX_BR_LINK_NUM] = {0};
static uint8_t timer_idx_check_role_switch[MAX_BR_LINK_NUM] = {0};
static uint8_t timer_idx_later_hid[MAX_BR_LINK_NUM] = {0};

static bool rws_link_lost = false;

static T_SRC_CONN_IND src_conn_ind[SRC_CONN_IND_MAX_NUM];

static T_BP_IND_FUN ind_fun = NULL;

static T_SHUTDOWN_STEP shutdown_step;
static uint8_t shutdown_step_retry_cnt;

static uint8_t old_peer_factory_addr[6];

static void app_bt_policy_engage_sched(void);
static void app_bt_policy_linkback_sched(void);
static void app_bt_policy_stable_sched(T_STABLE_ENTER_MODE mode);
static void app_bt_policy_linkback_run(void);
static void app_bt_policy_engage_ind(T_ENGAGE_IND ind);
static void app_bt_policy_disconnect_all_event_handle(void);
static bool app_bt_policy_judge_dedicated_enter_pairing_mode(void);
static void app_bt_policy_prepare_for_dedicated_enter_pairing_mode(void);
static void app_bt_policy_timer_cback(uint8_t timer_evt, uint16_t param);
static void app_bt_policy_stop_all_active_action(void);
static void app_bt_policy_save_engage_related_nv(void);

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_new_pri_apply_relay_info_when_roleswap_suc(void);
static T_BP_ROLESWAP_INFO bp_roleswap_info_temp = {0};
#endif


#if (F_APP_OTA_TOOLING_SUPPORT == 1)
static bool app_bt_policy_validate_bd_addr_with_dongle_id(uint8_t dongle_id, uint8_t *bd_addr);
static const uint32_t ota_dongle_addr[31] =
{
    0x56238821, 0x56050BC7, 0x56EBF7D2, 0x56F9BCF5, 0x56B31083, 0x5647927F, 0x566B389B, 0x560933ED,
    0x5629C8AC, 0x563A50E8, 0x568805F5, 0x56e31104, 0x564fc1c6, 0x5615a63f, 0x5678f37b, 0x56DF5FA8,
    0x5601535b, 0x564a7230, 0x5681941b, 0x565d1d94, 0x56570017, 0x56af0473, 0x563fc9dc, 0x5671418d,
    0x56247bf2, 0x5612f939, 0x56D5E2F9, 0x56B89C33, 0x56D90599, 0x56AA34d4, 0x5690f14d
};
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if F_APP_TEAMS_FEATURE_SUPPORT
uint8_t conn_to_new_device;
#endif

static void app_bt_policy_connected(void)
{
    uint32_t i;

    ENGAGE_PRINT_INFO3("connected: bud_role %d, b2b %d, b2s %d",
                       app_cfg_nv.bud_role, b2b_connected, app_bt_point_br_link_num_get());

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_link_check_b2s_link_by_id(i))
        {
            ENGAGE_PRINT_TRACE2("connected: b2s, bd_addr %s, profs 0x%08x",
                                TRACE_BDADDR(app_db.br_link[i].bd_addr),
                                app_db.br_link[i].connected_profile);
        }
    }
}

static void app_bt_policy_new_state(T_STATE state)
{
    ENGAGE_PRINT_INFO1("new_state: state 0x%02x", state);
}

static void app_bt_policy_event_info(T_EVENT event)
{
    ENGAGE_PRINT_INFO1("event_info: event 0x%02x", event);
}

static void app_bt_policy_new_radio_mode(T_BT_DEVICE_MODE mode)
{
    ENGAGE_PRINT_TRACE1("new_radio_mode: radio_mode 0x%02x", mode);
}

static void app_bt_policy_stable_enter_mode(uint8_t mode)
{
    ENGAGE_PRINT_TRACE1("stable_enter_mode: mode 0x%02x", mode);
}

static void app_bt_policy_shutdown_step_info(uint8_t step)
{
    ENGAGE_PRINT_TRACE1("shutdown_step_state: step 0x%02x", step);
}

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_new_pri_bp_state(uint8_t state)
{
    ENGAGE_PRINT_TRACE1("new_pri_bp_state: state 0x%02x", state);
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void app_bt_policy_b2s_conn_start_timer(void)
{
    app_start_timer(&timer_idx_b2s_conn, "b2s_conn",
                    bt_policy_timer_id, APP_TIMER_B2S_CONN, 0, false,
                    APP_BT_POLICY_B2S_CONN_TIMER_MS);
}

uint32_t app_bt_policy_get_profs_by_bond_flag(uint32_t bond_flag)
{
    uint32_t profs = 0;

#if 0
    if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_HFP_BASE)
    {
        if (bond_flag & BOND_FLAG_HFP)
        {
            profs |= HFP_PROFILE_MASK;

            if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
            {
                profs |= PBAP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_HSP)
        {
            profs |= HSP_PROFILE_MASK;

            if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
            {
                profs |= PBAP_PROFILE_MASK;
            }
        }

        if (bond_flag & BOND_FLAG_SPP)
        {
            if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
            {
                profs |= SPP_PROFILE_MASK;
            }
        }
        else if (bond_flag & BOND_FLAG_IAP)
        {
            if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
            {
                profs |= IAP_PROFILE_MASK;
            }
        }
    }
    else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_A2DP_BASE)
    {
        if (bond_flag & (BOND_FLAG_A2DP))
        {
            profs |= A2DP_PROFILE_MASK;
            profs |= AVRCP_PROFILE_MASK;
        }

#if F_APP_HID_SUPPORT
        if (bond_flag & (BOND_FLAG_HID))
        {
            if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
            {
                profs |= HID_PROFILE_MASK;
            }
        }
#endif
    }
    else
#endif
        if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_HF_A2DP_LAST_DEVICE)
        {
            if (bond_flag & BOND_FLAG_HFP)
            {
                profs |= HFP_PROFILE_MASK;

                if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
                {
                    profs |= PBAP_PROFILE_MASK;
                }
            }
            else if (bond_flag & BOND_FLAG_HSP)
            {
                profs |= HSP_PROFILE_MASK;

                if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
                {
                    profs |= PBAP_PROFILE_MASK;
                }
            }

            if (bond_flag & (BOND_FLAG_A2DP))
            {
                profs |= A2DP_PROFILE_MASK;
                profs |= AVRCP_PROFILE_MASK;
            }

#if F_APP_HID_SUPPORT
            if (bond_flag & (BOND_FLAG_HID))
            {
                if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
                {
                    profs |= HID_PROFILE_MASK;
                }
            }
#endif

            if (bond_flag & BOND_FLAG_SPP)
            {
                if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
                {
                    profs |= SPP_PROFILE_MASK;
                }
            }
            else if (bond_flag & BOND_FLAG_IAP)
            {
                if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
                {
                    profs |= IAP_PROFILE_MASK;
                }
            }
        }
#if 0
        else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_SPP_BASE)
        {
            if (bond_flag & BOND_FLAG_SPP)
            {
                if (app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK)
                {
                    profs |= SPP_PROFILE_MASK;
                }
            }
            else if (bond_flag & BOND_FLAG_IAP)
            {
                if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
                {
                    profs |= IAP_PROFILE_MASK;
                }
            }
        }
#endif
#if F_APP_HID_SUPPORT
        else if ((T_LINKBACK_SCENARIO)app_cfg_const.link_scenario == LINKBACK_SCENARIO_HID_BASE)
        {
            if (bond_flag & (BOND_FLAG_HID))
            {
                if (app_cfg_const.supported_profile_mask & HID_PROFILE_MASK)
                {
                    profs |= HID_PROFILE_MASK;
                }
            }
        }
#endif

    return profs;
}

static void app_bt_policy_set_bd_addr(void)
{
    ENGAGE_PRINT_INFO3("set_bd_addr: role %d, local_addr %s, peer_addr %s",
                       app_cfg_nv.bud_role,
                       TRACE_BDADDR(app_cfg_nv.bud_local_addr),
                       TRACE_BDADDR(app_cfg_nv.bud_peer_addr));

    gap_set_bd_addr(app_cfg_nv.bud_local_addr);

#if F_APP_ERWS_SUPPORT
    remote_local_addr_set(app_cfg_nv.bud_local_addr);
    remote_peer_addr_set(app_cfg_nv.bud_peer_addr);
    remote_session_role_set((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
#endif

    gap_br_vendor_data_rate_set(1); //link for 2M
}

static void app_bt_policy_modify_white_list(void)
{
    uint8_t wl_addr_type = GAP_REMOTE_ADDR_LE_PUBLIC;

    app_ble_whitelist_modify(GAP_WHITE_LIST_OP_ADD, app_cfg_nv.bud_peer_addr,
                             (T_GAP_REMOTE_ADDR_TYPE)wl_addr_type);
    app_ble_whitelist_modify(GAP_WHITE_LIST_OP_ADD, app_cfg_nv.bud_local_addr,
                             (T_GAP_REMOTE_ADDR_TYPE)wl_addr_type);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void app_bt_policy_src_conn_ind_init(void)
{
    memset(src_conn_ind, 0, sizeof(src_conn_ind));
}

static void app_bt_policy_src_conn_ind_add(uint8_t *bd_addr, bool is_update)
{
    uint16_t index;

    for (index = 0; index < SRC_CONN_IND_MAX_NUM; index++)
    {
        if (src_conn_ind[index].timer_idx != 0)
        {
            if (!memcmp(src_conn_ind[index].bd_addr, bd_addr, 6))
            {
                break;
            }
        }
    }

    if (!is_update)
    {
        if (index == SRC_CONN_IND_MAX_NUM)
        {
            for (index = 0; index < SRC_CONN_IND_MAX_NUM; index++)
            {
                if (src_conn_ind[index].timer_idx == 0)
                {
                    memcpy(src_conn_ind[index].bd_addr, bd_addr, 6);
                    break;
                }
            }
        }
    }

    if (index < SRC_CONN_IND_MAX_NUM)
    {
        app_stop_timer(&src_conn_ind[index].timer_idx);
        app_start_timer(&src_conn_ind[index].timer_idx, "src_conn_ind",
                        bt_policy_timer_id, APP_TIMER_SRC_CONN_IND, index, false,
                        SRC_CONN_IND_DELAY_MS);
    }
}

static void app_bt_policy_src_conn_ind_del(uint16_t index)
{
    memset(&src_conn_ind[index], 0, sizeof(T_SRC_CONN_IND));
}

bool app_bt_policy_src_conn_ind_check(uint8_t *bd_addr)
{
    uint16_t index;
    bool found = false;

    for (index = 0; index < SRC_CONN_IND_MAX_NUM; index++)
    {
        if (src_conn_ind[index].timer_idx != 0)
        {
            if (!memcmp(src_conn_ind[index].bd_addr, bd_addr, 6))
            {
                ENGAGE_PRINT_TRACE1("app_bt_policy_src_conn_ind_check: found index %d", index);
                found = true;
                break;
            }
        }
    }

    return found;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_b2b_connected_init(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        engage_done = true;
    }
    else
    {
        engage_done = false;

        if (app_cfg_nv.first_engaged)
        {
            app_bt_policy_modify_white_list();
        }
    }

    b2b_connected = false;
}
#else
static void app_bt_policy_b2b_connected_init(void)
{
    app_cfg_nv.bud_role = REMOTE_SESSION_ROLE_SINGLE;
    engage_done = true;
    b2b_connected = false;
}
#endif

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_b2b_connected_add_node(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link == NULL)
    {
        p_link = app_link_alloc_br_link(bd_addr);
    }

    ENGAGE_PRINT_TRACE2("app_bt_policy_b2b_connected_add_node: bd_addr %s, p_link %p",
                        TRACE_BDADDR(bd_addr), p_link);

    if (p_link != NULL)
    {
        engage_done = true;
        b2b_connected = true;
        rws_link_lost = false;
        app_bt_point_link_num_changed(BP_LINK_TYPE_B2B_BR, BP_LINK_EVENT_CONNECT, bd_addr);
    }
}

static void app_bt_policy_b2b_connected_del_node(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    ENGAGE_PRINT_TRACE2("app_bt_policy_b2b_connected_del_node: bd_addr %s, p_link %p",
                        TRACE_BDADDR(bd_addr), p_link);
    if (p_link != NULL)
    {
        app_link_free_br_link(p_link);
        engage_done = false;
        b2b_connected = false;
        app_bt_point_link_num_changed(BP_LINK_TYPE_B2B_BR, BP_LINK_EVENT_DISCONNECT, bd_addr);
    }
}

static void app_bt_policy_b2b_connected_add_prof(uint8_t *bd_addr, uint32_t prof)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->connected_profile |= prof;
    }
}

static void app_bt_policy_b2b_connected_del_prof(uint8_t *bd_addr, uint32_t prof)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->connected_profile &= ~prof;
    }
}

static void app_bt_policy_b2b_qos_param_update(void)
{
    if (b2b_connected && (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        if (app_link_get_sco_conn_num() == 0)
        {
            bt_link_qos_set(app_cfg_nv.bud_peer_addr, BT_QOS_TYPE_GUARANTEED, 38);
        }
        else
        {
            bt_link_qos_set(app_cfg_nv.bud_peer_addr, BT_QOS_TYPE_GUARANTEED, 26);
        }
    }
}
#endif

static void app_bt_policy_b2s_connected_init(void)
{
    if (app_cfg_const.enable_multi_link)
    {
        app_db.b2s_connected_num_max = app_cfg_const.max_legacy_multilink_devices;
    }
    else
    {
        app_db.b2s_connected_num_max = 1;
    }
}

static uint8_t app_bt_policy_b2s_connected_is_empty(void)
{
    if (app_bt_point_num_get() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool app_bt_policy_b2s_connected_is_over(void)
{
    if (app_bt_point_num_get() > app_db.b2s_connected_num_max)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool app_bt_policy_b2s_connected_add_node(uint8_t *bd_addr, uint8_t *id)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link == NULL)
    {
        p_link = app_link_alloc_br_link(bd_addr);
        if (p_link != NULL)
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_BR, BP_LINK_EVENT_CONNECT, bd_addr);
            }
        }
    }
    if (p_link != NULL)
    {
        *id = p_link->id;
        return true;
    }
    else
    {
        return false;
    }
}

static void app_bt_policy_b2s_connected_add_prof(uint8_t *bd_addr, uint32_t prof)
{
    T_APP_BR_LINK *p_link;
    //bool sync_flag = true;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->connected_profile |= prof;

        switch (prof)
        {
        case A2DP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_A2DP);
            break;

        case HFP_PROFILE_MASK:
            {
                bt_bond_flag_remove(bd_addr, BOND_FLAG_HSP);
                bt_bond_flag_add(bd_addr, BOND_FLAG_HFP);
            }
            break;

        case HSP_PROFILE_MASK:
            {
                bt_bond_flag_remove(bd_addr, BOND_FLAG_HFP);
                bt_bond_flag_add(bd_addr, BOND_FLAG_HSP);
            }
            break;

        case SPP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_SPP);
            break;

#if F_APP_BT_PROFILE_PBAP_SUPPORT
        case PBAP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_jj);
            break;
#endif

#if F_APP_HID_SUPPORT
        case HID_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_HID);
            break;
#endif

#if F_APP_IAP_SUPPORT
        case IAP_PROFILE_MASK:
            bt_bond_flag_add(bd_addr, BOND_FLAG_IAP);
            break;
#endif

        default:
            //sync_flag = false;
            break;
        }

#if F_APP_ERWS_SUPPORT
        if (sync_flag == true)
        {
            T_APP_REMOTE_MSG_PAYLOAD_PROFILE_CONNECTED msg;

            memcpy(msg.bd_addr, bd_addr, 6);
            msg.prof_mask = prof;
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_PROFILE_CONNECTED,
                                                (uint8_t *)&msg, sizeof(T_APP_REMOTE_MSG_PAYLOAD_PROFILE_CONNECTED));

        }
#endif
    }

}

static void app_bt_policy_b2s_connected_del_prof(uint8_t *bd_addr, uint32_t prof)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->connected_profile &= ~prof;
    }
}

static bool app_bt_policy_b2s_connected_del_node(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        app_stop_timer(&timer_idx_role_switch[p_link->id]);
        app_stop_timer(&timer_idx_later_avrcp[p_link->id]);
        app_stop_timer(&timer_idx_check_role_switch[p_link->id]);
        app_stop_timer(&timer_idx_later_hid[p_link->id]);
        app_link_free_br_link(p_link);

#if XM_XIAOAI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaoai_support)
        {
            app_xiaoai_free_br_link(bd_addr);
        }
#endif

#if F_APP_XIAOWEI_FEATURE_SUPPORT
        if (extend_app_cfg_const.xiaowei_support)
        {
            app_xiaowei_free_br_link(bd_addr);
        }
#endif
        if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
        {
            if (app_bt_point_br_link_num_get() > 0)
            {
                app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_BR, BP_LINK_EVENT_DISCONNECT, bd_addr);
                app_bt_point_link_tone_set(p_link->bd_addr, false);
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

static bool app_bt_policy_b2s_connected_find_node(uint8_t *bd_addr, uint32_t *profs)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if (profs != NULL)
        {
            *profs = p_link->connected_profile;
        }
        return true;
    }
    else
    {
        return false;
    }
}

static bool app_bt_policy_b2s_connected_no_profs(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    bool ret = true;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        if (p_link->connected_profile != 0)
        {
            ret = false;
        }
    }

    return ret;
}

void app_bt_policy_b2s_connected_set_last_conn_index(uint8_t conn_idx)
{
    last_src_conn_idx = conn_idx;
}

uint8_t app_bt_policy_b2s_connected_get_last_conn_index(void)
{
    return last_src_conn_idx;
}

static void app_bt_policy_b2s_connected_mark_index(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        ++last_src_conn_idx;

        if (app_bt_point_br_link_num_get() > 1)
        {
            p_link->src_conn_idx = last_src_conn_idx;
        }
        else if (app_bt_point_br_link_num_get() == 1)
        {
            p_link->src_conn_idx = last_src_conn_idx - 1;
        }
    }
}

static bool app_bt_policy_b2s_connected_is_first_src(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    bool ret = true;

    if (app_db.b2s_connected_num_max >= 2)
    {
        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            if (p_link->src_conn_idx >= last_src_conn_idx)
            {
                ret = false;
            }
        }
    }

    return ret;
}

static bool app_bt_policy_b2s_connected_vp_played(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    bool ret = false;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        ret = p_link->b2s_connected_vp_is_played;
    }

    return ret;
}

static void app_bt_policy_connected_node_auth_suc(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        p_link->auth_flag = true;
    }
}

static bool app_bt_policy_connected_node_is_authed(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    bool ret = false;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        ret = p_link->auth_flag;
    }

    return ret;
}

void app_bt_policy_update_cpu_freq(T_BP_CPU_FREQ_EVENT req_event)
{
    uint32_t actual_mhz = 0;
    uint8_t check_reason = 0;

    //
    // increase cpu freq when:
    // 1. acl link connected (3s)
    // 2. (two sco link) || (sco + le link) || (sco + a2dp streaming)
    // 3. lea bca state
    // 4. lea streaming
    // 5. roleswap
    // 6. linkback
    //

    if (req_event == BP_CPU_FREQ_EVENT_ACL_CONN_CMPL)
    {
        pm_cpu_freq_req(&app_bt_policy_freq_handle, 100, &actual_mhz);
        check_reason = 1;
        app_start_timer(&timer_idx_increase_cpu_clk, "increase_clk_temp",
                        bt_policy_timer_id, APP_TIMER_SET_CPU_CLK, 0, false,
                        3000);
    }
    else if (app_link_get_sco_conn_num() > 1 ||
             (app_link_get_sco_conn_num() > 0 && app_link_get_le_link_num() > 0) ||
             (app_link_get_sco_conn_num() > 0 && app_a2dp_is_streaming())
            )
    {
        pm_cpu_freq_req(&app_bt_policy_freq_handle, 100, &actual_mhz);
        check_reason = 2;
    }
#if F_APP_LEA_SUPPORT
    else if ((app_lea_bca_state() == LEA_BCA_STATE_SCAN) ||
             (app_lea_bca_state() == LEA_BCA_STATE_WAIT_RETRY))
    {
        pm_cpu_freq_req(&app_bt_policy_freq_handle, 100, &actual_mhz);
        check_reason = 3;
    }
#endif
#if TARGET_LE_AUDIO_GAMING
    else if (app_lea_mgr_is_streaming())
    {
        pm_cpu_freq_req(&app_bt_policy_freq_handle, 100, &actual_mhz);
        check_reason = 4;
    }
#endif
#if F_APP_ERWS_SUPPORT
    else if (app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_BUSY)
    {
        pm_cpu_freq_req(&app_bt_policy_freq_handle, 100, &actual_mhz);
        check_reason = 5;
    }
#endif
    else if (app_bt_policy_get_state() == BP_STATE_LINKBACK)
    {
        pm_cpu_freq_req(&app_bt_policy_freq_handle, 100, &actual_mhz);
        check_reason = 6;
    }
    else
    {
        pm_cpu_freq_clear(&app_bt_policy_freq_handle, &actual_mhz);
    }

    APP_PRINT_TRACE4("app_bt_policy_update_cpu_freq: freq_handle %d, actural_mhz %d, req_event %x, check_reason %d",
                     app_bt_policy_freq_handle, actual_mhz, req_event, check_reason);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void app_bt_policy_event_ind(T_BP_EVENT event, T_BP_EVENT_PARAM *event_param)
{
    T_BP_EVENT_PARAM null_param;

    if (ind_fun == NULL)
    {
        return;
    }

    if (event_param == NULL)
    {
        memset(&null_param, 0, sizeof(T_BP_EVENT_PARAM));
        event_param = &null_param;
    }

    if (cur_state >= STATE_SHUTDOWN_STEP)
    {
        switch (event)
        {
        case BP_EVENT_STATE_CHANGED:
        case BP_EVENT_RADIO_MODE_CHANGED:
        case BP_EVENT_PROFILE_DISCONN:
        case BP_EVENT_SRC_DISCONN_LOST:
        case BP_EVENT_SRC_DISCONN_NORMAL:
            ind_fun(event, event_param);
            break;

        default:
            ENGAGE_PRINT_TRACE0("event_ind: cur_state >= STATE_SHUTDOWN_STEP, not ind");
            break;
        }
    }
    else
    {
        ind_fun(event, event_param);
    }
}

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_relay_pri_bp_state(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
        app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (cur_state < STATE_AFE_SECONDARY)
        {
            app_bt_policy_new_pri_bp_state(bp_state);
            app_relay_async_single(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_PRI_BP_STATE);
        }
    }
}

static void app_bt_policy_recv_relay_pri_bp_state(void *buf, uint16_t len)
{
    uint8_t data = *((uint8_t *)buf);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_bt_policy_new_pri_bp_state(data);

        pri_bp_state = (T_BP_STATE)data;
    }
}

static void app_bt_policy_relay_pri_req(uint8_t req)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
        app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        ENGAGE_PRINT_TRACE1("relay_pri_req: 0x%x", req);
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_PRI_REQ, &req, 1);
    }
}

static void app_bt_policy_recv_relay_pri_req(void *buf, uint16_t len)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        uint8_t req = *((uint8_t *)buf);

        ENGAGE_PRINT_TRACE1("recv_relay_pri_req: 0x%x", req);

        if (req)
        {
            switch (req)
            {
            case PRI_REQ_LET_SEC_TO_DISCONN:
                {
                    gap_br_send_acl_disconn_req(app_cfg_nv.bud_peer_addr);
                }
                break;
#if F_APP_LEA_SUPPORT
            case PRI_REQ_LET_SEC_ENABLE_ADV:
                {
                    app_lea_adv_start();
                }
                break;

            case PRI_REQ_LET_SEC_DISENABLE_ADV:
                {
                    app_lea_adv_stop();
                }
                break;
#endif
            default:
                break;
            }
        }
    }
}

static void app_bt_policy_relay_pri_linkback_node(void)
{
    uint32_t node_num;
    T_LINKBACK_NODE *p_node;
    uint32_t i;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY &&
        app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        node_num = linkback_todo_queue_node_num();
        if (node_num != 0)
        {
            p_node = malloc(node_num * sizeof(T_LINKBACK_NODE));
            if (p_node != NULL)
            {
                for (i = 0; i < node_num; i++)
                {
                    linkback_todo_queue_take_first_node(p_node + i);
                }

                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_LINKBACK_NODE,
                                                    (uint8_t *)p_node, node_num * sizeof(T_LINKBACK_NODE));

                free(p_node);
            }
        }
    }
}

static void app_bt_policy_recv_relay_pri_linkback_node(void *buf, uint16_t len)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        T_LINKBACK_NODE *p_node = (T_LINKBACK_NODE *)buf;
        uint32_t node_num = len / sizeof(T_LINKBACK_NODE);
        T_LINKBACK_NODE *node;
        uint32_t i;

        ENGAGE_PRINT_TRACE1("recv_relay_pri_linkback_node: node_num %d", node_num);

        startup_linkback_done_flag = true;
        bond_list_load_flag = true;

        linkback_todo_queue_init();
        linkback_active_node_init();

        for (i = 0; i < node_num; i++)
        {
            node = p_node + i;

            linkback_todo_queue_insert_normal_node(node->bd_addr, node->plan_profs, node->retry_timeout,
                                                   node->is_group_member);
        }
    }
}
#endif

#if 0
static void app_bt_policy_primary_adv(void)
{
    uint8_t adv_interval;

    if (app_link_get_sco_conn_num() != 0)
    {
        adv_interval = PRIMARY_BLE_ADV_INTERVAL_VOICE;
    }
    else if (app_link_get_a2dp_start_num() != 0)
    {
        adv_interval = PRIMARY_BLE_ADV_INTERVAL_AUDIO;
    }
    else
    {
        adv_interval = PRIMARY_BLE_ADV_INTERVAL_NORMAL;
    }

    ENGAGE_PRINT_TRACE1("app_bt_policy_primary_adv: adv_interval 0x%x", adv_interval);

#if F_APP_ERWS_SUPPORT
    engage_afe_primary_adv_start(adv_interval);
#endif
}
#endif

void app_bt_policy_primary_engage_action_adjust(void)
{
#if F_APP_ERWS_SUPPORT
    if (!engage_done && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_stop_timer(&timer_idx_engage_action_adjust);

        switch (cur_state)
        {
        case STATE_AFE_LINKBACK:
            {
                app_bt_policy_primary_adv();
            }
            break;

        case STATE_AFE_CONNECTED:
        case STATE_AFE_PAIRING_MODE:
        case STATE_AFE_STANDBY:
            {
                if (app_bt_point_br_link_num_get() == 0 && rws_link_lost)
                {
                    engage_afe_forever_shaking_start();
                }
                else
                {
                    app_bt_policy_primary_adv();
                }
            }
            break;

        default:
            break;
        }
    }
#endif
}

static void app_bt_policy_enter_state(T_STATE state, T_BT_DEVICE_MODE mode)
{
    bool state_changed = false;
    T_BP_EVENT_PARAM event_param;
    uint8_t to;

#if F_APP_GAMING_DONGLE_SUPPORT
    uint8_t b2s_num = app_link_get_b2s_link_num();
#endif

    if (state != STATE_AFE_LINKBACK)
    {
        app_stop_timer(&timer_idx_linkback_delay);
        app_db.pre_recovery_connected = false;
    }

    if (cur_state != state || roleswap_suc_flag)
    {
        roleswap_suc_flag = false;

        app_bt_policy_new_state(state);

        cur_state = state;
        state_changed = true;
    }

    if (STATE_AFE_LINKBACK == cur_state)
    {
        if (!app_cfg_const.enable_not_discoverable_when_linkback)
        {
            mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;
        }
    }

    if (STATE_AFE_STANDBY == cur_state)
    {
        if (app_cfg_const.enable_discoverable_in_standby_mode)
        {
            mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;
        }
    }

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY &&
        app_bt_point_br_link_num_get() == 1)
    {
        if (app_cfg_const.enable_multi_link && app_cfg_const.enable_always_discoverable)
        {
            mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;
        }
    }

    if (STATE_AFE_CONNECTED == cur_state ||
        STATE_AFE_PAIRING_MODE == cur_state ||
        STATE_AFE_STANDBY == cur_state)
    {
        if (discoverable_when_one_link)
        {
            discoverable_when_one_link = false;

            if (mode != BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE &&
                app_hfp_get_call_status() == APP_CALL_IDLE)
            {
                mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;

                bool enable_discoverable = true;

#if F_APP_GAMING_DONGLE_SUPPORT
                if (b2s_num == 1 && app_dongle_is_streaming())
                {
                    enable_discoverable = false;
                }
#endif

#if F_APP_LEA_SUPPORT
                if (!app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_GET_LEGACY, NULL))
                {
                    enable_discoverable = false;
                }
#endif

                if (enable_discoverable)
                {
                    app_start_timer(&timer_idx_discoverable, "discoverable",
                                    bt_policy_timer_id, APP_TIMER_DISCOVERABLE, 0, false,
                                    app_cfg_const.timer_pairing_while_one_conn_timeout * 1000);
                }
            }
        }
#if GFPS_SASS_SUPPORT
        else if (extend_app_cfg_const.gfps_sass_support)
        {
            if (mode != BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
            {
                mode = BT_DEVICE_MODE_CONNECTABLE;
            }
        }
#endif

        if ((app_hfp_get_call_status() != APP_CALL_IDLE) ||
            app_bt_point_br_link_is_full())
        {
            app_stop_timer(&timer_idx_discoverable);
        }
    }
    else
    {
        app_stop_timer(&timer_idx_discoverable);
    }

    if (timer_idx_discoverable != 0)
    {
        mode = BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE;
    }

#if F_APP_GAMING_DONGLE_SUPPORT
#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
#else
    if (app_cfg_const.enable_dongle_dual_mode)
    {
        bool change_mode_to_connectable = false;

        if (mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
        {
            if (app_dongle_is_24g_mode())
            {
                change_mode_to_connectable = true;
            }
        }

        if (change_mode_to_connectable)
        {
            mode = BT_DEVICE_MODE_CONNECTABLE;
        }
        else if (app_cfg_const.enable_multi_link)
        {
            if (app_dongle_is_24g_mode() && (app_link_get_b2s_link_num() >= 1))
            {
                mode = BT_DEVICE_MODE_IDLE;
            }
        }
    }
#endif

#if F_APP_DONGLE_MULTI_PAIRING
    if (app_cfg_const.enable_dongle_multi_pairing && app_cfg_nv.is_bt_pairing == false)
    {
        if (mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
        {
            mode = BT_DEVICE_MODE_CONNECTABLE;
        }
    }
#endif

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
#else
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE && app_cfg_const.enable_multi_link)
    {
        if (b2s_num == 1 && app_dongle_is_streaming())
        {
            mode = BT_DEVICE_MODE_IDLE;
        }
    }
#endif
#endif

    if (radio_mode != mode)
    {
        app_bt_policy_new_radio_mode(mode);
        radio_mode = mode;

#if (F_APP_HARMAN_FEATURE_SUPPORT && HARMAN_ONLY_CONN_ONE_DEVICE_WHEN_PAIRING)
        if (app_cfg_const.enable_multi_link)
        {
            if (radio_mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
            {
                app_harman_pairable_mode_set(true);
                app_harman_is_already_connect_one_set(false);
            }
            else
            {
                app_harman_pairable_mode_set(false);
                if (app_link_get_connected_src_num())
                {
                    app_harman_is_already_connect_one_set(true);
                }
            }
        }
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
        uint16_t swift_adv_keep_vendor_section_timeout = (app_cfg_const.timer_pairing_timeout ?
                                                          (app_cfg_const.timer_pairing_timeout - APP_SWIFT_PAIR_DEFAULT_ADV_DATA_CHANGE_PRIOR_TIMEOUT) :
                                                          0xffff);
        if (radio_mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
        {
            if (!swift_adv_keep_vendor_section_timeout)
            {
                app_swift_pair_start_adv_change_timer(APP_SWIFT_PAIR_ADV_KEEP_VENDOR_SECTUON_MIN_TIMEOUT);
            }
            else if (swift_adv_keep_vendor_section_timeout != 0xffff)
            {
                app_swift_pair_start_adv_change_timer(swift_adv_keep_vendor_section_timeout);
            }
            app_swift_pair_adv_start(APP_SWIFT_PAIR_DEFAULT_TIMEOUT);
        }
        else
        {
            app_swift_pair_stop_adv_change_timer();
            app_swift_pair_adv_stop(APP_STOP_ADV_CAUSE_EXIT_PAIRING_MODE);
        }
#endif

#if F_APP_LEA_SUPPORT
        app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_SET_RADIO, &mode);
#if F_APP_ERWS_SUPPORT
        if (mode == BT_DEVICE_MODE_CONNECTABLE ||
            mode == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
        {
            app_bt_policy_relay_pri_req(PRI_REQ_LET_SEC_ENABLE_ADV);
        }
        else if (mode == BT_DEVICE_MODE_IDLE)
        {
            app_bt_policy_relay_pri_req(PRI_REQ_LET_SEC_DISENABLE_ADV);
        }
#endif
#elif F_APP_GAMING_DONGLE_SUPPORT
        bool ignore_radio_mode = (mode == BT_DEVICE_MODE_IDLE &&
                                  dongle_ctrl_data.ignore_radio_mode_idle_when_shutdown);

        if (ignore_radio_mode == false)
        {
            bt_device_mode_set(mode);
        }
#else
        bt_device_mode_set(mode);
#endif
        app_bt_policy_event_ind(BP_EVENT_RADIO_MODE_CHANGED, NULL);
    }

    if (is_user_action_pairing_mode)
    {
        is_user_action_pairing_mode = false;
#if F_APP_DURIAN_SUPPORT
//rsv for avp
#else
        state_changed = true;
#endif
    }

    if (state_changed)
    {
#if F_APP_GAMING_DONGLE_SUPPORT
        if (app_cfg_const.enable_dongle_dual_mode)
        {
            if (state == STATE_AFE_LINKBACK)
            {
                app_dongle_dual_mode_switch(SWITCH_EVENT_ENTER_LINKBACK);
            }
            else if (state == STATE_AFE_PAIRING_MODE)
            {
                app_dongle_dual_mode_switch(SWITCH_EVENT_ENTER_PAIRING);
            }
            else
            {
                app_dongle_dual_mode_switch(SWITCH_EVENT_ENTER_STANDBY);
            }
        }
#endif

        switch (cur_state)
        {
        case STATE_STARTUP:
        case STATE_SHUTDOWN:
            bp_state = BP_STATE_IDLE;
            break;
#if F_APP_ERWS_SUPPORT
        case STATE_FE_SHAKING:
            bp_state = BP_STATE_FIRST_ENGAGE;

            engage_fe_shaking_start();

            to = app_cfg_const.timer_first_engage;
            if (to < FE_TO_MIN)
            {
                to = FE_TO_MIN;
            }

#if F_APP_SAIYAN_MODE
            if (data_capture_saiyan.saiyan_enable)
            {
                bt_device_mode_set(BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
            }
            else
#endif
            {
                app_start_timer(&timer_idx_first_engage_check, "first_engage_check",
                                bt_policy_timer_id, APP_TIMER_FIRST_ENGAGE_CHECK, 0, false,
                                (to * FE_TO_UNIT));
            }
            app_auto_power_off_disable(AUTO_POWER_OFF_MASK_BUD_COUPLING);
            break;

        case STATE_FE_COUPLING:
            bp_state = BP_STATE_FIRST_ENGAGE;
            break;

        case STATE_AFE_TIMEOUT_SHAKING:
            bp_state = BP_STATE_ENGAGE;
            engage_afe_timeout_shaking_start();
            break;

        case STATE_AFE_COUPLING:
            bp_state = BP_STATE_ENGAGE;
            startup_linkback_done_flag = false;
            bond_list_load_flag = false;
            linkback_todo_queue_init();
            linkback_active_node_init();
            break;

        case STATE_AFE_WAIT_COUPLING:
            bp_state = BP_STATE_ENGAGE;
            break;
#endif

        case STATE_AFE_LINKBACK:
            bp_state = BP_STATE_LINKBACK;
            app_bt_policy_qos_param_update(app_db.br_link[app_a2dp_get_active_idx()].bd_addr,
                                           BP_TPOLL_LINKBACK_START);
            app_bt_policy_primary_engage_action_adjust();
            break;

        case STATE_AFE_CONNECTED:
            bp_state = BP_STATE_CONNECTED;
            app_bt_policy_primary_engage_action_adjust();
            break;

        case STATE_AFE_PAIRING_MODE:
            bp_state = BP_STATE_PAIRING_MODE;
            app_bt_policy_primary_engage_action_adjust();
            break;

        case STATE_AFE_STANDBY:
            bp_state = BP_STATE_STANDBY;
            app_bt_policy_primary_engage_action_adjust();
            break;

        case STATE_AFE_SECONDARY:
            bp_state = BP_STATE_SECONDARY;
            break;

        case STATE_DUT_TEST_MODE:
            bp_state = BP_STATE_CONNECTED;
            engage_done = true;
            break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
        case STATE_OTA_SHAKING:
            engage_afe_forever_shaking_start();
            break;
#endif

        case STATE_PREPARE_FOR_ROLESWAP:
            bp_state = BP_STATE_PREPARE_ROLESWAP;
            break;

        case STATE_SHUTDOWN_STEP:
        case STATE_STOP:
            bp_state = BP_STATE_IDLE;
#if F_APP_ERWS_SUPPORT
            engage_off();
#endif
            break;

        default:
            break;
        }

#if F_APP_LEA_SUPPORT
        app_lea_adv_update();
#endif

        if (app_cfg_const.timer_pairing_timeout != 0)
        {
            if (STATE_AFE_PAIRING_MODE == cur_state)
            {
#if F_APP_LEA_SUPPORT
                if (app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_GET_LEGACY, NULL) ||
                    app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_GET_CIS_POLICY, NULL))
#endif
                    app_start_timer(&timer_idx_pairing_mode, "pairing_mode",
                                    bt_policy_timer_id, APP_TIMER_PAIRING_MODE, 0, false,
                                    app_cfg_const.timer_pairing_timeout * 1000);
            }
            else
            {
                app_stop_timer(&timer_idx_pairing_mode);
            }
        }

        event_param.is_shut_down = false;
        if (STATE_SHUTDOWN == cur_state)
        {
            event_param.is_shut_down = true;
        }

        event_param.is_ignore = true;
         if (STATE_AFE_PAIRING_MODE == cur_state)
         {
             event_param.is_ignore = false;
		 }

        app_bt_policy_event_ind(BP_EVENT_STATE_CHANGED, &event_param);

        if (STATE_AFE_LINKBACK == cur_state)
        {
            app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_LINKBACK);

            app_bt_policy_linkback_run();
        }
        else
        {
            app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_LINKBACK);
        }
    }
}

static void app_bt_policy_linkback_sched(void)
{
    ENGAGE_PRINT_INFO3("linkback_sched: enable_power_on_linkback %d, startup_linkback_done_flag %d, bond_list_load_flag %d",
                       app_cfg_const.enable_power_on_linkback,
                       startup_linkback_done_flag,
                       bond_list_load_flag);

    linkback_flag = false;

    if (dedicated_enter_pairing_mode_flag)
    {
        dedicated_enter_pairing_mode_flag = false;

        if (app_bt_policy_judge_dedicated_enter_pairing_mode())
        {
            app_bt_policy_prepare_for_dedicated_enter_pairing_mode();
            return;
        }
    }

    if (app_cfg_const.enable_power_on_linkback)
    {
        if (!startup_linkback_done_flag)
        {
            if (!bond_list_load_flag)
            {
                bond_list_load_flag = true;

                linkback_load_bond_list(0, linkback_retry_timeout);
            }
        }
    }

    if (0 != linkback_todo_queue_node_num() || linkback_active_node.is_valid)
    {
        if (linkback_active_node.is_valid)
        {
            app_stop_timer(&timer_idx_linkback);
            linkback_active_node_use_left_time_insert_to_queue_again(true);
            linkback_active_node.is_valid = false;
        }

        app_bt_policy_enter_state(STATE_AFE_LINKBACK, BT_DEVICE_MODE_CONNECTABLE);
    }
    else
    {
        app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
    }
}

static void app_bt_policy_linkback_run(void)
{
    uint8_t pair_idx;
    T_LINKBACK_NODE node;
    uint32_t profs;

    ENGAGE_PRINT_TRACE0("linkback_run: start");

RETRY:
    if (!linkback_active_node.is_valid)
    {
        if (linkback_todo_queue_take_first_node(&node))
        {
            linkback_active_node_load(&node);

            if (linkback_active_node.is_valid)
            {
                linkback_active_node.linkback_node.gtiming = os_sys_time_get();

                app_start_timer(&timer_idx_linkback, "linkback",
                                bt_policy_timer_id, APP_TIMER_LINKBACK, 0, false,
                                linkback_active_node.retry_timeout);
                app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_BP_STATE_LINKBACK);

                if (bt_bond_index_get(linkback_active_node.linkback_node.bd_addr, &pair_idx))
                {
                    if (app_cfg_nv.last_btmode[pair_idx] == BP_LINK_LAST_BTMODE_LEA)
                    {
                        goto EXIT;
                    }
                }
            }
        }
    }

    if (!linkback_active_node.is_valid)
    {
        ENGAGE_PRINT_TRACE0("linkback_run: have no valid node, finish");

#if GFPS_SASS_SUPPORT
        if (extend_app_cfg_const.gfps_sass_support)
        {
            app_sass_policy_link_back_end();
        }
#endif
        app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
        app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_BP_STATE_LINKBACK);

        goto EXIT;
    }

    if (linkback_active_node.is_exit)
    {
        linkback_active_node.is_valid = false;

        goto RETRY;
    }

    if (0 == linkback_active_node.remain_profs)
    {
        linkback_active_node.is_valid = false;

        goto RETRY;
    }

    if (app_bt_policy_src_conn_ind_check(linkback_active_node.linkback_node.bd_addr))
    {
        app_stop_timer(&timer_idx_linkback_delay);
#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
        app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                        bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, 0, false,
                        1500);
#else
        app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                        bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, 0, false,
                        500);
#endif

        goto EXIT;
    }

    if (app_bt_policy_b2s_connected_find_node(linkback_active_node.linkback_node.bd_addr, &profs))
    {
        if (profs & linkback_active_node.doing_prof)
        {
            ENGAGE_PRINT_TRACE1("linkback_run: prof 0x%08x, already connected",
                                linkback_active_node.doing_prof);

            linkback_active_node_step_suc_adjust_remain_profs();

            goto RETRY;
        }
        else
        {
            goto LINKBACK;
        }
    }
    else
    {
        if (linkback_active_node.linkback_node.is_force)
        {
            goto LINKBACK;
        }
        else
        {
            if (app_bt_point_br_link_is_full())
            {
                ENGAGE_PRINT_TRACE0("linkback_run: b2s is full, abort this node");

                linkback_active_node.is_valid = false;

                goto RETRY;
            }
            else
            {
                goto LINKBACK;
            }
        }
    }

LINKBACK:
    linkback_flag = true;

    if (linkback_active_node.linkback_node.check_bond_flag)
    {
        if (!linkback_check_bond_flag(linkback_active_node.linkback_node.bd_addr,
                                      linkback_active_node.doing_prof))
        {
            linkback_active_node_step_fail_adjust_remain_profs();

            goto RETRY;
        }
    }

    if (app_link_find_br_link(linkback_active_node.linkback_node.bd_addr) == NULL)
    {
        //set before acl connected
        if (app_db.recovery_connected)
        {
            app_db.pre_recovery_connected = true;
        }
        else
        {
            app_db.pre_recovery_connected = false;
        }
    }

    if (linkback_profile_is_need_search(linkback_active_node.doing_prof))
    {
        app_hfp_adjust_sco_window(linkback_active_node.linkback_node.bd_addr, APP_SCO_ADJUST_LINKBACK_EVT);

        if (!linkback_profile_search_start(linkback_active_node.linkback_node.bd_addr,
                                           linkback_active_node.doing_prof, linkback_active_node.linkback_node.is_special,
                                           &linkback_active_node.linkback_node.search_param))
        {
            linkback_active_node_step_fail_adjust_remain_profs();

            goto RETRY;
        }
        else
        {
            goto EXIT;
        }
    }
    else
    {
        linkback_active_node.is_sdp_ok = true;

        app_hfp_adjust_sco_window(linkback_active_node.linkback_node.bd_addr, APP_SCO_ADJUST_LINKBACK_EVT);

        if (linkback_profile_connect_start(linkback_active_node.linkback_node.bd_addr,
                                           linkback_active_node.doing_prof, &linkback_active_node.linkback_conn_param))
        {
            goto EXIT;
        }
        else
        {
            linkback_active_node_step_fail_adjust_remain_profs();

            goto RETRY;
        }
    }

EXIT:
    return;
}

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_roleswap_event_handle(T_BT_PARAM *param)
{
    uint8_t app_idx;

    ENGAGE_PRINT_INFO2("roleswap_event_handle: bud_role %d, is_suc %d", param->bud_role,
                       param->is_suc);

    if (param->is_suc)
    {
        T_APP_BR_LINK *p_link = NULL;

        app_cfg_nv.bud_role = param->bud_role;

        cur_event = EVENT_ROLESWAP;

        engage_done = true;
        b2b_connected = true;

        app_bt_policy_connected();

#if F_APP_LEA_SUPPORT
        app_lea_adv_update();
        app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_SET_IDLE, NULL);

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            app_db.remote_cis_link_is_streaming = 0;
            app_lea_uca_sync_cis_streaming_state(mtc_is_lea_cis_stream(NULL));
        }
#endif

        dedicated_enter_pairing_mode_flag = false;
        is_visiable_flag = false;
        is_force_flag = false;
        is_user_action_pairing_mode = false;
        disconnect_for_pairing_mode = false;
        discoverable_when_one_link = false;

        linkback_flag = false;
        linkback_retry_timeout = app_cfg_const.timer_linkback_timeout;
        after_stop_sdp_todo_linkback_run_flag = false;

        app_stop_timer(&timer_idx_pairing_mode);
        app_stop_timer(&timer_idx_discoverable);
        app_stop_timer(&timer_idx_linkback);
        app_stop_timer(&timer_idx_linkback_delay);
        app_stop_timer(&timer_idx_wait_coupling);
        app_stop_timer(&timer_idx_engage_action_adjust);
        app_stop_timer(&timer_idx_page_scan_param);

        rws_link_lost = false;

        app_bt_policy_src_conn_ind_init();

        app_bt_policy_new_pri_apply_relay_info_when_roleswap_suc();

        ENGAGE_PRINT_TRACE1("roleswap_event_handle: last_src_conn_idx %d", last_src_conn_idx);

        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2s_link_by_id(app_idx))
            {
                ENGAGE_PRINT_TRACE2("roleswap_event_handle: app_db.br_link[%d].src_conn_idx %d", app_idx,
                                    app_db.br_link[app_idx].src_conn_idx);
                p_link = &app_db.br_link[app_idx];
                if (p_link)
                {
                    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                        (p_link->sniffing_type == BT_SNIFFING_TYPE_A2DP))
                    {
                        /*for a2dp sniffing type, set flag to keep small htpoll value whatever happens,
                        until receive 7 a2dp packets from src*/
                        app_db.recover_param = true;
                        app_db.down_count = 0;
                    }

                    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                        ((p_link->connected_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK)) != 0))
                    {
                        app_db.first_hf_index = app_idx;
                        p_link->app_hf_state = APP_HF_STATE_CONNECTED;
                    }

                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_BR, BP_LINK_EVENT_CONNECT, p_link->bd_addr);
                        app_bt_policy_sync_b2s_connected();
                    }

                    ENGAGE_PRINT_TRACE2("roleswap_event_handle: b2s_tpoll_value %d, b2s_tpoll_ctx 0x%x",
                                        p_link->tpoll_value, (*(uint16_t *)&tpoll_ctx));
                }
            }
        }

        linkback_active_node_init();
        app_sniff_mode_roleswap_suc();

        if (cur_state != STATE_SHUTDOWN_STEP &&
            cur_state != STATE_SHUTDOWN)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                startup_linkback_done_flag = true;
                bond_list_load_flag = true;

                if ((app_cfg_const.enter_pairing_while_only_one_device_connected) &&
                    (app_bt_point_br_link_num_get() == 1))
                {
                    discoverable_when_one_link = true;
                }

                if (linkback_todo_queue_node_num() != 0)
                {
                    app_bt_policy_enter_state(STATE_AFE_LINKBACK, BT_DEVICE_MODE_CONNECTABLE);
                }
                else
                {
                    linkback_todo_queue_init();

                    if (BP_STATE_PAIRING_MODE == pri_bp_state)
                    {
                        cur_state = STATE_AFE_PAIRING_MODE;
                    }
                    else if (BP_STATE_STANDBY == pri_bp_state)
                    {
                        cur_state = STATE_AFE_STANDBY;
                    }
                    else
                    {
                        cur_state = STATE_AFE_CONNECTED;
                    }

                    app_bt_policy_stable_sched(STABLE_ENTER_MODE_AGAIN);
                }

                roleswap_suc_flag = true;
                pri_bp_state = BP_STATE_IDLE;
            }
            else
            {
                linkback_todo_queue_init();
                startup_linkback_done_flag = false;
                bond_list_load_flag = false;

                pri_bp_state = bp_state;

                app_bt_policy_enter_state(STATE_AFE_SECONDARY, BT_DEVICE_MODE_IDLE);
            }
        }
    }
    else
    {
        if (cur_state != STATE_SHUTDOWN_STEP &&
            cur_state != STATE_SHUTDOWN)
        {
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_AGAIN);
        }
    }

    if (cur_state != STATE_SHUTDOWN_STEP && cur_state != STATE_SHUTDOWN)
    {
        app_db.disallow_connected_tone_after_inactive_connected = (app_db.connected_tone_need == true) ?
                                                                  false : true;
        app_audio_set_connected_tone_need(false);
        app_multi_reconnect_inactive_link();
    }

    if (STATE_SHUTDOWN_STEP == cur_state)
    {
        app_bt_policy_disconnect_all_event_handle();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void app_bt_policy_roleswitch_b2b_check(T_APP_BR_LINK *p_link)
{
    if (p_link->acl_decrypted && cur_state < STATE_SHUTDOWN_STEP)
    {
        if ((app_link_get_b2s_link_num() == 0) && (app_link_get_le_link_num() == 0) &&
            !app_cfg_const.rws_disable_codec_mute_when_linkback)
        {
            if (BT_LINK_ROLE_MASTER == p_link->acl_link_role)
            {
                app_sniff_mode_b2b_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);
                app_db.b2b_role_switch_led_pending = 1;

                if (p_link->acl_link_in_sniffmode_flg)
                {
                    p_link->roleswitch_check_after_unsniff_flg = true;
                }
                else
                {
                    ENGAGE_PRINT_TRACE0("roleswitch_b2b_check: switch to slave");
                    bt_link_role_switch(p_link->bd_addr, false);
                }
            }
        }
        else
        {
            if (BT_LINK_ROLE_SLAVE == p_link->acl_link_role)
            {
                app_sniff_mode_b2b_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);
                app_db.b2b_role_switch_led_pending = 1;

                if (p_link->acl_link_in_sniffmode_flg)
                {
                    p_link->roleswitch_check_after_unsniff_flg = true;
                }
                else
                {
                    ENGAGE_PRINT_TRACE0("roleswitch_b2b_check: switch to master");
                    bt_link_role_switch(p_link->bd_addr, true);
                }
            }
        }
    }
}
#endif

static void app_bt_policy_roleswitch_b2s_check(T_APP_BR_LINK *p_link)
{
#if F_APP_GAMING_DONGLE_SUPPORT
    T_APP_BR_LINK *p_dongle_link = app_link_find_br_link(app_db.connected_dongle_addr);
#endif

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        if (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)
        {
            uint8_t active_hf_idx = app_hfp_get_active_idx();
            uint8_t inactive_hf_idx = app_multi_find_inacitve(app_hfp_get_active_idx());
#if F_APP_GAMING_DONGLE_SUPPORT
            T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();
#endif

            if (app_hfp_sco_is_connected())
            {
#if F_APP_GAMING_DONGLE_SUPPORT
                if ((p_dongle_link != NULL) && (p_dongle_link->id == inactive_hf_idx))
                {
                    APP_PRINT_ERROR0("app_bt_policy_roleswitch_b2s_check: inactive link is dongle, do not role switch");
                    return;
                }
#endif
                if (app_db.br_link[active_hf_idx].acl_link_role != BT_LINK_ROLE_SLAVE)
                {
                    bt_link_role_switch(app_db.br_link[active_hf_idx].bd_addr, false);
                }

                if (app_db.br_link[inactive_hf_idx].acl_link_role != BT_LINK_ROLE_MASTER)
                {
                    bt_link_role_switch(app_db.br_link[inactive_hf_idx].bd_addr, true);
                }
            }
            else
            {
                if (app_db.br_link[app_multi_find_inacitve(p_link->id)].acl_link_role != BT_LINK_ROLE_SLAVE)
                {
                    bt_link_role_switch(app_db.br_link[app_multi_find_inacitve(p_link->id)].bd_addr, false);
                }
            }
        }
    }
#if F_APP_ERWS_SUPPORT
    else if (BT_LINK_ROLE_MASTER == p_link->acl_link_role)
    {
        app_sniff_mode_b2s_disable(p_link->bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);

        if (p_link->acl_link_in_sniffmode_flg)
        {
            p_link->roleswitch_check_after_unsniff_flg = true;
        }
        else
        {
            //bud is as master on b2s link
            if ((app_db.recovery_connected == true) && (app_db.pre_recovery_connected == false))
            {
                //clk may be adjusted during b2s connecting
                app_multi_roleswap_disconnect_inactive_link();
                app_start_timer(&timer_idx_reconnect, "reconnect",
                                bt_policy_timer_id, APP_TIMER_RECONNECT, 0, false,
                                1000);
                return;
            }

            ENGAGE_PRINT_TRACE0("roleswitch_b2s_check: switch to slave");

            bt_link_role_switch(p_link->bd_addr, false);
        }
    }
#endif
}

static void app_bt_policy_roleswitch_handle(uint8_t *bd_addr, uint32_t event)
{
    T_APP_BR_LINK *p_link;

    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE))
    {
        if (ROLESWITCH_EVENT_LINK_DISCONNECTED == event)
        {
            ENGAGE_PRINT_TRACE1("roleswitch_handle: b2s, event %d, role_switch_count 0", event);

#if F_APP_ERWS_SUPPORT
            p_link = app_link_find_br_link(app_cfg_nv.bud_peer_addr);
            if (p_link != NULL)
            {
                p_link->link_role_switch_count = 0;

                app_bt_policy_roleswitch_b2b_check(p_link);
            }
#endif
            return;
        }

        p_link = app_link_find_br_link(bd_addr);
        if (p_link != NULL)
        {
            if (event != ROLESWITCH_EVENT_FAIL_RETRY)
            {
                p_link->link_role_switch_count = 0;
            }

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(bd_addr))
            {
                ENGAGE_PRINT_TRACE2("roleswitch_handle: b2b, event %d, role_switch_count %d", event,
                                    p_link->link_role_switch_count);

                switch (event)
                {
                case ROLESWITCH_EVENT_LINK_CONNECTED:
                case ROLESWITCH_EVENT_LINK_ACTIVE:
                case ROLESWITCH_EVENT_FAIL_RETRY:
                    {
                        app_bt_policy_roleswitch_b2b_check(p_link);
                    }
                    break;

                case ROLESWITCH_EVENT_FAIL_RETRY_MAX:
                    {
                        app_sniff_mode_b2b_enable(bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);

                        app_bt_policy_relay_pri_req(PRI_REQ_LET_SEC_TO_DISCONN);
                    }
                    break;

                case ROLESWITCH_EVENT_ROLE_CHANGED:
                    {
                        if (0 == app_link_get_b2s_link_num() &&
                            !app_cfg_const.rws_disable_codec_mute_when_linkback)
                        {
                            if (BT_LINK_ROLE_SLAVE == p_link->acl_link_role)
                            {
                                app_sniff_mode_b2b_enable(bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);
                            }
                        }
                        else
                        {
                            if (BT_LINK_ROLE_MASTER == p_link->acl_link_role)
                            {
                                app_sniff_mode_b2b_enable(bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);

                                app_bt_sniffing_process(bd_addr);
                            }
                            else if (0 != app_link_get_b2s_link_num())
                            {
                                app_bt_policy_roleswitch_b2b_check(p_link);
                            }
                        }
                    }
                    break;

                default:
                    break;
                }
            }
            else
#endif
            {
                ENGAGE_PRINT_TRACE2("roleswitch_handle: b2s, event %d, role_switch_count %d", event,
                                    p_link->link_role_switch_count);

                switch (event)
                {
                case ROLESWITCH_EVENT_LINK_CONNECTED:
                case ROLESWITCH_EVENT_LINK_ACTIVE:
                case ROLESWITCH_EVENT_SCO_CHANGED:
                    {
                        app_bt_policy_roleswitch_b2s_check(p_link);

#if F_APP_ERWS_SUPPORT
                        p_link = app_link_find_br_link(app_cfg_nv.bud_peer_addr);
                        if (p_link != NULL)
                        {
                            app_bt_policy_roleswitch_b2b_check(p_link);
                        }
#endif
                    }
                    break;

                case ROLESWITCH_EVENT_FAIL_RETRY:
                    {
                        if ((app_db.recovery_connected == true) && (app_db.pre_recovery_connected == false))
                        {
                            app_multi_roleswap_disconnect_inactive_link();
                            app_start_timer(&timer_idx_reconnect, "reconnect",
                                            bt_policy_timer_id, APP_TIMER_RECONNECT, 0, false,
                                            1000);
                        }
                        else
                        {
                            app_bt_policy_roleswitch_b2s_check(p_link);
                        }
                    }
                    break;

                case ROLESWITCH_EVENT_FAIL_RETRY_MAX:
                    {
                        p_link->link_role_switch_count = 0;
                        app_sniff_mode_b2s_enable(bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);
                        app_start_timer(&timer_idx_role_switch[p_link->id], "role_switch",
                                        bt_policy_timer_id, APP_TIMER_ROLE_SWITCH, p_link->id, false,
                                        ROLE_SWITCH_WINDOW_DELAY_MS);
                    }
                    break;

                case ROLESWITCH_EVENT_ROLE_CHANGED:
                    {
                        if (BT_LINK_ROLE_SLAVE == p_link->acl_link_role)
                        {
                            app_sniff_mode_b2s_enable(bd_addr, SNIFF_DISABLE_MASK_ROLESWITCH);

                            app_bt_sniffing_process(bd_addr);

                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }
}

static void app_bt_policy_roleswitch_fail_event_handle(T_BT_PARAM *param)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(param->bd_addr);
    if (p_link != NULL)
    {
        p_link->link_role_switch_count++;

        if (p_link->link_role_switch_count < ROLE_SWITCH_COUNT_MAX)
        {
            app_start_timer(&timer_idx_role_switch[p_link->id], "role_switch",
                            bt_policy_timer_id, APP_TIMER_ROLE_SWITCH, p_link->id, false,
                            ROLE_SWITCH_DELAY_MS);
        }
        else
        {
            app_bt_policy_roleswitch_handle(param->bd_addr, ROLESWITCH_EVENT_FAIL_RETRY_MAX);
        }
    }
}

static void app_bt_policy_roleswitch_timeout_event_handle(T_BT_PARAM *param)
{
    T_APP_BR_LINK *p_link = param->p_link;

    if (p_link)
    {
        app_bt_policy_roleswitch_handle(p_link->bd_addr, ROLESWITCH_EVENT_FAIL_RETRY);
    }
}

static void app_bt_policy_role_master_event_handle(T_BT_PARAM *param)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(param->bd_addr);
    if (p_link != NULL)
    {
        p_link->acl_link_role = BT_LINK_ROLE_MASTER;

        app_bt_policy_roleswitch_handle(param->bd_addr, ROLESWITCH_EVENT_ROLE_CHANGED);
    }
}

static void app_bt_policy_role_slave_event_handle(T_BT_PARAM *param)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(param->bd_addr);
    if (p_link != NULL)
    {
        p_link->acl_link_role = BT_LINK_ROLE_SLAVE;

        app_bt_policy_roleswitch_handle(param->bd_addr, ROLESWITCH_EVENT_ROLE_CHANGED);
    }
}

void app_bt_policy_roleswitch_when_sco(uint8_t *bd_addr)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        app_bt_policy_roleswitch_handle(bd_addr, ROLESWITCH_EVENT_SCO_CHANGED);
    }
}

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_prepare_for_roleswap_event_handle(void)
{
    if (!memcmp(linkback_active_node.linkback_node.bd_addr, app_db.resume_addr, 6) &&
        (app_db.disallow_connected_tone_after_inactive_connected == true))
    {
        //roleswap is triggerred when bud is reconnecting inactive src with no acl
        uint8_t cmd_ptr[7];
        memcpy(&cmd_ptr[0], app_db.resume_addr, 6);
        cmd_ptr[6] = app_db.connected_num_before_roleswap;
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_RESUME_BT_ADDRESS,
                                            cmd_ptr, 7);
        app_audio_set_connected_tone_need(false);
        //clear app_db.disallow_connected_tone_after_inactive_connected
        app_db.disallow_connected_tone_after_inactive_connected = false;
    }
    app_bt_policy_stop_all_active_action();

    app_bt_policy_enter_state(STATE_PREPARE_FOR_ROLESWAP, BT_DEVICE_MODE_IDLE);
}


static void app_bt_policy_engage_bat_state(uint8_t *bat, uint8_t *state)
{
    uint8_t local_loc_batt = app_db.local_batt_level & 0x7f;

    if (app_db.local_loc == BUD_LOC_IN_CASE)
    {
        local_loc_batt = app_db.local_batt_level | 0x80;
    }

    if (*bat == 0)
    {
        *bat = local_loc_batt;
    }
    *state = 0x66;
}
#endif

static void app_bt_policy_conn_sniff_event_handle(T_BT_PARAM *param)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(param->bd_addr);
    if (p_link != NULL)
    {
        p_link->acl_link_in_sniffmode_flg = true;
#if F_APP_HARMAN_FEATURE_SUPPORT
        uint8_t to_do_connect_idle_power_off = true;
        T_APP_BR_LINK *p_link_check;

        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                p_link_check = app_link_find_br_link(app_db.br_link[i].bd_addr);
                if (p_link_check->acl_link_in_sniffmode_flg == false)
                {
                    to_do_connect_idle_power_off = false;
                }
            }
        }
        APP_PRINT_INFO1("[SD_CHECK] conn_sniff_event_handle =%d", to_do_connect_idle_power_off);

        if (!app_harman_power_off_option_timer_get() && to_do_connect_idle_power_off)
        {
            app_harman_connect_idle_to_power_off(CONNECT_IDLE_POWER_OFF_START, p_link->id);
        }
#endif
    }
}

static void app_bt_policy_conn_active_event_handle(T_BT_PARAM *param)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(param->bd_addr);
    if (p_link != NULL)
    {
        p_link->acl_link_in_sniffmode_flg = false;

        if (p_link->roleswitch_check_after_unsniff_flg)
        {
            p_link->roleswitch_check_after_unsniff_flg = false;

            app_bt_policy_roleswitch_handle(param->bd_addr, ROLESWITCH_EVENT_LINK_ACTIVE);
        }
#if F_APP_HARMAN_FEATURE_SUPPORT
        //app_harman_connect_idle_to_power_off(ACTIVE_NEED_STOP_COUNT, p_link->id);
#endif
    }
}

void app_bt_policy_stop_reconnect_timer()
{
    APP_PRINT_TRACE1("app_stop_reconnect_timer: connected_num_before_roleswap %d",
                     app_db.connected_num_before_roleswap);
    app_stop_timer(&timer_idx_reconnect);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void app_bt_policy_startup_event_handle(T_BT_PARAM *bt_param)
{
    ind_fun = bt_param->startup_param.ind_fun;

    radio_mode = BT_DEVICE_MODE_IDLE;

    bp_state = BP_STATE_IDLE;
    pri_bp_state = BP_STATE_IDLE;

    dedicated_enter_pairing_mode_flag = false;
    startup_linkback_done_flag = false;
    bond_list_load_flag = false;
    linkback_flag = false;
    is_user_action_pairing_mode = false;
    last_src_conn_idx = 0;
    disconnect_for_pairing_mode = false;
    linkback_retry_timeout = app_cfg_const.timer_linkback_timeout;
    after_stop_sdp_todo_linkback_run_flag = false;
    discoverable_when_one_link = false;

    timer_idx_shutdown_step = 0;
    timer_idx_first_engage_check = 0;
    timer_idx_pairing_mode = 0;
    timer_idx_discoverable = 0;
    timer_idx_bud_linklost = 0;
    timer_idx_linkback = 0;
    timer_idx_linkback_delay = 0;
    timer_idx_wait_coupling = 0;
    timer_idx_engage_action_adjust = 0;
    timer_idx_page_scan_param = 0;
    memset(timer_idx_role_switch, 0, MAX_BR_LINK_NUM);
    memset(timer_idx_later_avrcp, 0, MAX_BR_LINK_NUM);
    memset(timer_idx_check_role_switch, 0, MAX_BR_LINK_NUM);
    memset(timer_idx_later_hid, 0, MAX_BR_LINK_NUM);

    rws_link_lost = false;

    app_bt_policy_src_conn_ind_init();

    app_bt_policy_enter_state(STATE_STARTUP, BT_DEVICE_MODE_IDLE);

#if F_APP_ANC_SUPPORT
    if (anc_tool_check_resp_meas_mode() == ANC_RESP_MEAS_MODE_NONE)
#endif
    {
        memcpy(app_cfg_nv.bud_local_addr, app_db.factory_addr, 6);
        memcpy(app_cfg_nv.bud_peer_addr, app_cfg_nv.bud_peer_factory_addr, 6);
    }

    app_cfg_nv.bud_role = app_cfg_const.bud_role;
    app_cfg_nv.ori_bud_role = app_cfg_const.bud_role;

    app_bt_policy_set_bd_addr();

    memcpy(old_peer_factory_addr, app_cfg_nv.bud_peer_factory_addr, 6);

#if F_APP_ERWS_SUPPORT
    {
        T_ENGAGE_CFG cfg;
        cfg.const_bud_role           = app_cfg_const.bud_role;
        cfg.nv_first_engaged         = &app_cfg_nv.first_engaged;
        cfg.nv_bud_role              = &app_cfg_nv.bud_role;
        cfg.nv_bud_local_addr        = app_cfg_nv.bud_local_addr;
        cfg.nv_bud_peer_addr         = app_cfg_nv.bud_peer_addr;
        cfg.nv_bud_peer_factory_addr = app_cfg_nv.bud_peer_factory_addr;
        cfg.factory_addr             = app_db.factory_addr;
        cfg.const_stop_adv_cause     = APP_STOP_ADV_CAUSE_ENGAGE;
        cfg.const_fe_mp_rssi         = (0 - app_cfg_const.rws_pairing_required_rssi);
        cfg.const_rws_custom_uuid    = app_cfg_const.rws_custom_uuid;
        cfg.const_high_batt_pri      = app_cfg_const.enable_high_batt_primary;
        cfg.const_addition           = app_db.jig_dongle_id;
#if F_APP_SAIYAN_MODE
        cfg.ref_saiyan_mode_en       = data_capture_saiyan.saiyan_enable;
        cfg.ref_saiyan_mode_role     = app_cfg_const.bud_side | 0x80;
#else
        cfg.ref_saiyan_mode_en       = 0;
        cfg.ref_saiyan_mode_role     = 0;
#endif

        engage_on(&cfg, app_bt_policy_engage_ind, app_bt_policy_engage_bat_state, &app_db.remote_bat_adv);
    }
#endif

    app_bt_policy_b2b_connected_init();
    app_bt_policy_b2s_connected_init();

    linkback_todo_queue_init();
    linkback_active_node_init();

    if (bt_param->startup_param.at_once_trigger)
    {
        app_bt_policy_engage_sched();
    }
}

void app_bt_policy_enter_dut_test_mode_event_handle(void)
{
    app_bt_policy_enter_state(STATE_DUT_TEST_MODE, BT_DEVICE_MODE_IDLE);
}

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
static void app_bt_policy_start_ota_shaking_event_handle(void)
{
    app_bt_policy_enter_state(STATE_OTA_SHAKING, BT_DEVICE_MODE_IDLE);
}

static void app_bt_policy_enter_ota_mode_event_handle(T_BT_PARAM *param)
{
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
        ((app_db.jig_subcmd == APP_ADP_SPECIAL_CMD_OTA) ||
         (app_db.jig_subcmd == APP_ADP_SPECIAL_CMD_FACTORY_MODE)))
    {
        // Secondary role should be transer to Primary role during OTA TOOLING
        engage_afe_change_role(REMOTE_SESSION_ROLE_PRIMARY);
        app_bt_policy_set_bd_addr();
        app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);
    }

    if (param->is_connectable)
    {
        if (app_db.jig_subcmd == APP_ADP_SPECIAL_CMD_FACTORY_MODE)
        {
            app_bt_policy_enter_state(STATE_OTA_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
        }
        else
        {
            app_bt_policy_enter_state(STATE_OTA_MODE, BT_DEVICE_MODE_CONNECTABLE);
        }
    }
    else
    {
        app_bt_policy_enter_state(STATE_OTA_MODE, BT_DEVICE_MODE_IDLE);
    }
}
#endif

static void app_bt_policy_shutdown_step_handle(void)
{
    uint32_t i;

    app_bt_policy_shutdown_step_info(shutdown_step);

    switch (shutdown_step)
    {
    case SHUTDOWN_STEP_START_DISCONN_B2S_PROFILE:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                for (i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        app_db.br_link[i].disconn_acl_flg = false;
#if F_APP_HID_SUPPORT
                        app_db.br_link[i].connected_profile &= ~HID_PROFILE_MASK;
#endif
                        linkback_profile_disconnect_start(app_db.br_link[i].bd_addr, app_db.br_link[i].connected_profile);
                    }
                }
            }
            shutdown_step = SHUTDOWN_STEP_WAIT_DISCONN_B2S_PROFILE;
            shutdown_step_retry_cnt = 0;
        }
        break;

    case SHUTDOWN_STEP_WAIT_DISCONN_B2S_PROFILE:
        {
            if (++shutdown_step_retry_cnt >= DISCONN_B2S_PROFILE_WAIT_TIMES)
            {
                shutdown_step = SHUTDOWN_STEP_START_DISCONN_B2S_LINK;
            }
        }
        break;

    case SHUTDOWN_STEP_START_DISCONN_B2S_LINK:
        {
            for (i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        if (app_db.br_link[i].sco_handle)
                        {
                            gap_br_send_sco_disconn_req(app_db.br_link[i].bd_addr);
                        }
                        gap_br_send_acl_disconn_req(app_db.br_link[i].bd_addr);
                    }
#if 0
                    else
                    {
                        bt_sniffing_link_disconnect(app_db.br_link[i].bd_addr);
                    }
#endif
                }
            }
            shutdown_step = SHUTDOWN_STEP_WAIT_DISCONN_B2S_LINK;
            shutdown_step_retry_cnt = 0;
        }
        break;

    case SHUTDOWN_STEP_WAIT_DISCONN_B2S_LINK:
        {
            ++shutdown_step_retry_cnt;
            if ((0 == app_bt_point_br_link_num_get()) ||
                shutdown_step_retry_cnt >= DISCONN_B2S_LINK_WAIT_TIMES_MAX)
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE)
                {
                    shutdown_step = SHUTDOWN_STEP_START_DISCONN_B2B_LINK;
                }
                else
                {
                    shutdown_step = SHUTDOWN_STEP_END;
                }
            }
        }
        break;
#if F_APP_ERWS_SUPPORT
    case SHUTDOWN_STEP_START_DISCONN_B2B_LINK:
        {
            if (b2b_connected)
            {
                gap_br_send_acl_disconn_req(app_cfg_nv.bud_peer_addr);

                shutdown_step = SHUTDOWN_STEP_WAIT_DISCONN_B2B_LINK;
                shutdown_step_retry_cnt = 0;
            }
            else
            {
                shutdown_step = SHUTDOWN_STEP_END;
            }
        }
        break;

    case SHUTDOWN_STEP_WAIT_DISCONN_B2B_LINK:
        {
            ++shutdown_step_retry_cnt;

            if ((b2b_connected == false) || shutdown_step_retry_cnt >= DISCONN_B2B_LINK_WAIT_TIMES_MAX)
            {
                shutdown_step = SHUTDOWN_STEP_END;
            }
        }
        break;
#endif

    default:
        break;
    }

    if (SHUTDOWN_STEP_END == shutdown_step)
    {
        app_bt_policy_enter_state(STATE_SHUTDOWN, BT_DEVICE_MODE_IDLE);
#if F_APP_AIRPLANE_SUPPORT
        app_airplane_bt_shutdown_end_check();
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
        /* for asynchorous */
        if (app_cfg_const.enable_dongle_dual_mode)
        {
            app_dongle_shutdown_end_check();
        }
#endif
    }
    else
    {
        app_start_timer(&timer_idx_shutdown_step, "shutdown_step",
                        bt_policy_timer_id, APP_TIMER_SHUTDOWN_STEP, 0, false,
                        SHUT_DOWN_STEP_TIMER_MS);
    }
}

static void app_bt_policy_stop_all_active_action(void)
{
    uint8_t *bd_addr;

    app_stop_timer(&timer_idx_first_engage_check);
    app_stop_timer(&timer_idx_pairing_mode);
    app_stop_timer(&timer_idx_discoverable);
    app_stop_timer(&timer_idx_bud_linklost);
    app_stop_timer(&timer_idx_linkback);
    app_stop_timer(&timer_idx_linkback_delay);
    app_stop_timer(&timer_idx_wait_coupling);
    app_stop_timer(&timer_idx_engage_action_adjust);
    app_stop_timer(&timer_idx_page_scan_param);

#if F_APP_ERWS_SUPPORT
    if (!engage_done)
    {
        remote_session_close(app_cfg_nv.bud_peer_addr);
    }
#endif

    if (linkback_active_node.is_valid)
    {
        bd_addr = linkback_active_node.linkback_node.bd_addr;

        after_stop_sdp_todo_linkback_run_flag = false;
        gap_br_stop_sdp_discov(bd_addr);
        if (!app_bt_policy_b2s_connected_find_node(bd_addr, NULL))
        {
            gap_br_send_acl_disconn_req(bd_addr);
        }

        linkback_active_node_use_left_time_insert_to_queue_again(false);

        linkback_active_node.is_valid = false;
    }
}

static void app_bt_policy_shutdown_event_handle(void)
{
#if F_APP_LEA_SUPPORT
    mtc_if_fm_ap_handle(AP_TO_MTC_SHUTDOWN, NULL, NULL);
#endif
    app_bt_policy_stop_all_active_action();

#if F_APP_ERWS_SUPPORT
    app_bt_policy_relay_pri_bp_state();
    app_bt_policy_relay_pri_linkback_node();


    if (cur_state == STATE_FE_SHAKING || cur_state == STATE_FE_COUPLING)
    {
        memcpy(app_cfg_nv.bud_peer_addr, old_peer_factory_addr, 6);
        memcpy(app_cfg_nv.bud_peer_factory_addr, old_peer_factory_addr, 6);

        if (PEER_VALID_MAGIC == app_cfg_nv.peer_valid_magic)
        {
            app_cfg_nv.first_engaged = 1;
        }

        app_bt_policy_save_engage_related_nv();
    }
#endif
    app_bt_policy_enter_state(STATE_SHUTDOWN_STEP, BT_DEVICE_MODE_IDLE);

    if ((b2b_connected == false) && (app_bt_point_br_link_num_get() == 0))
    {
        shutdown_step = SHUTDOWN_STEP_END;
    }
    else
    {
        shutdown_step = SHUTDOWN_STEP_START_DISCONN_B2S_PROFILE;
    }
    app_bt_policy_shutdown_step_handle();
}

static void app_bt_policy_stop_event_handle(void)
{
    app_bt_policy_stop_all_active_action();

    app_bt_policy_enter_state(STATE_STOP, BT_DEVICE_MODE_IDLE);
}

static void app_bt_policy_restore_event_handle(void)
{
    if (cur_state == STATE_STOP)
    {
        startup_linkback_done_flag = false;
        bond_list_load_flag = false;

        linkback_todo_queue_init();
        linkback_active_node_init();

        app_bt_policy_engage_sched();
    }
}

static void app_bt_policy_state_shutdown_step_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_SHUTDOWN_STEP_TIMEOUT:
        {
            app_bt_policy_shutdown_step_handle();
        }
        break;

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static void app_bt_policy_legacy_set_page_scan_param(uint16_t interval, uint16_t window)
{
    ENGAGE_PRINT_TRACE2("legacy_set_page_scan_param: interval 0x%x, window 0x%x",
                        interval, window);

    app_stop_timer(&timer_idx_page_scan_param);

    gap_br_cfg_page_scan_param(GAP_PAGE_SCAN_TYPE_INTERLACED, interval, window);
}




static void app_bt_policy_primary_page_scan_param_adjust(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (engage_done)
        {
#if GFPS_SASS_SUPPORT
            if (extend_app_cfg_const.gfps_sass_support)
            {
                app_bt_policy_legacy_set_page_scan_param(SASS_PAGESCAN_INTERVAL, SASS_PAGESCAN_WINDOW);
            }
            else
#endif
            {
                app_bt_policy_legacy_set_page_scan_param(NORMAL_PAGESCAN_INTERVAL, NORMAL_PAGESCAN_WINDOW);
            }
        }
        else
        {
            app_bt_policy_legacy_set_page_scan_param(PRI_FAST_PAGESCAN_INTERVAL, PRI_FAST_PAGESCAN_WINDOW);

            app_start_timer(&timer_idx_page_scan_param, "page_scan_param",
                            bt_policy_timer_id, APP_TIMER_PAGE_SCAN_PARAM, 0, false,
                            30 * 1000);
        }
    }
}

static void app_bt_policy_save_engage_related_nv(void)
{
    app_cfg_store(&app_cfg_nv.le_single_random_addr[4], 4);
    app_cfg_store(&app_cfg_nv.bud_local_addr[0], 20);
}

void app_bt_policy_update_bud_coupling_info(void)
{
    bt_bond_delete(app_cfg_nv.bud_local_addr);
    bt_bond_delete(app_cfg_nv.bud_peer_addr);

    app_cfg_nv.peer_valid_magic = PEER_VALID_MAGIC;
    app_cfg_nv.first_engaged = 1;
    app_bt_policy_save_engage_related_nv();
}


static void app_bt_policy_fe_timeout_handle(bool is_coupling)
{
    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_BUD_COUPLING, app_cfg_const.timer_auto_power_off);

    if (is_coupling)
    {
        memcpy(app_cfg_nv.bud_peer_addr, old_peer_factory_addr, 6);
        memcpy(app_cfg_nv.bud_peer_factory_addr, old_peer_factory_addr, 6);

        if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            remote_session_close(app_cfg_nv.bud_peer_addr);
        }
    }

    if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_bt_policy_legacy_set_page_scan_param(NORMAL_PAGESCAN_INTERVAL, NORMAL_PAGESCAN_WINDOW);
    }

    if ((PEER_VALID_MAGIC == app_cfg_nv.peer_valid_magic)
#if F_APP_SAIYAN_MODE
        && (data_capture_saiyan.saiyan_enable == 0)
#endif
       )
    {
        app_cfg_nv.first_engaged = 1;
        app_bt_policy_save_engage_related_nv();
        app_bt_policy_modify_white_list();

        app_bt_policy_engage_sched();
    }
    else
    {
        engage_afe_done();
        engage_done = true;

        if (app_cfg_const.still_linkback_if_first_engage_fail)
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                engage_afe_change_role(REMOTE_SESSION_ROLE_PRIMARY);
                app_bt_policy_set_bd_addr();
                app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);
            }

            app_bt_policy_linkback_sched();
        }
#if F_APP_SAIYAN_MODE
        else if (data_capture_saiyan.saiyan_enable)
        {
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
        }
#endif
        else
        {
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_DIRECT_PAIRING);
        }
    }
}

static void app_bt_policy_state_fe_shaking_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_FE_TIMEOUT:
        {
            app_bt_policy_fe_timeout_handle(false);
        }
        break;

    case EVENT_FE_SHAKING_DONE:
        {
            app_bt_policy_set_bd_addr();

            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                remote_session_open(app_cfg_nv.bud_peer_addr);
                app_bt_policy_event_ind(BP_EVENT_BUD_CONN_START, NULL);
                app_bt_policy_enter_state(STATE_FE_COUPLING, BT_DEVICE_MODE_IDLE);
            }
            else
            {
                app_bt_policy_legacy_set_page_scan_param(SEC_FAST_PAGESCAN_INTERVAL, SEC_FAST_PAGESCAN_WINDOW);
                app_bt_policy_enter_state(STATE_FE_COUPLING, BT_DEVICE_MODE_CONNECTABLE);
            }

            app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_state_fe_coupling_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_FE_TIMEOUT:
        {
            if (b2b_connected == false)
            {
                app_bt_policy_fe_timeout_handle(true);
            }
            else
            {
                app_start_timer(&timer_idx_first_engage_check, "first_engage_check",
                                bt_policy_timer_id, APP_TIMER_FIRST_ENGAGE_CHECK, 0, false,
                                2000);
            }
        }
        break;

    case EVENT_BUD_CONN_SUC:
        {
            bt_bond_delete(app_cfg_nv.bud_peer_addr);
        }
        break;

    case EVENT_BUD_CONN_FAIL:
    case EVENT_BUD_AUTH_FAIL:
        {
            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                remote_session_open(app_cfg_nv.bud_peer_addr);
            }
        }
        break;

    case EVENT_BUD_REMOTE_CONN_CMPL:
        {
            app_stop_timer(&timer_idx_first_engage_check);

            if (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_bt_policy_legacy_set_page_scan_param(NORMAL_PAGESCAN_INTERVAL, NORMAL_PAGESCAN_WINDOW);
            }

            if (memcmp(app_cfg_nv.bud_peer_factory_addr, old_peer_factory_addr, 6))
            {
                bt_bond_delete(old_peer_factory_addr);
            }

            app_cfg_nv.peer_valid_magic = PEER_VALID_MAGIC;
            app_cfg_nv.first_engaged = 1;
            app_bt_policy_save_engage_related_nv();
            app_bt_policy_modify_white_list();

#if F_APP_COMMON_DONGLE_SUPPORT
            if (app_cfg_const.linkback_to_dongle_first
                && (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                && (app_device_has_dongle_record()))
            {
                app_bt_policy_linkback_sched();
            }
            else
#endif
            {
                app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
            }
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_shaking_done(bool is_shaking_to)
{
    app_bt_policy_set_bd_addr();

    app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        startup_linkback_done_flag = false;
        bond_list_load_flag = false;

        app_stop_timer(&timer_idx_linkback);
        dedicated_enter_pairing_mode_flag = false;

        app_bt_policy_enter_state(STATE_AFE_COUPLING, BT_DEVICE_MODE_IDLE);

        remote_session_open(app_cfg_nv.bud_peer_addr);

        app_bt_policy_event_ind(BP_EVENT_BUD_CONN_START, NULL);
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        if (is_shaking_to)
        {
            app_bt_policy_primary_page_scan_param_adjust();

            app_bt_policy_linkback_sched();
        }
        else
        {
            app_bt_policy_legacy_set_page_scan_param(PRI_VERY_FAST_PAGESCAN_INTERVAL,
                                                     PRI_VERY_FAST_PAGESCAN_WINDOW);

            app_bt_policy_enter_state(STATE_AFE_WAIT_COUPLING, BT_DEVICE_MODE_CONNECTABLE);

            app_start_timer(&timer_idx_wait_coupling, "wait_coupling",
                            bt_policy_timer_id, APP_TIMER_WAIT_COUPLING, 0, false,
                            PRIMARY_WAIT_COUPLING_TIMEOUT_MS);
        }
    }
}

static void app_bt_policy_state_afe_timeout_shaking_event_handle(T_EVENT event, T_BT_PARAM *param)
{
    switch (event)
    {
    case EVENT_BUD_CONN_SUC:
        {
            app_bt_policy_linkback_sched();
        }
        break;

    case EVENT_AFE_SHAKING_DONE:
        {
            app_bt_policy_shaking_done(param->is_shaking_to);
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_state_afe_coupling_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_BUD_CONN_FAIL:
    case EVENT_BUD_AUTH_FAIL:
    case EVENT_BUD_DISCONN_LOCAL:
    case EVENT_BUD_DISCONN_NORMAL:
        {
#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (app_db.ota_tooling_start)
            {
                app_bt_policy_enter_state(STATE_OTA_SHAKING, BT_DEVICE_MODE_IDLE);
            }
            else
#endif
            {
                app_bt_policy_enter_state(STATE_AFE_TIMEOUT_SHAKING, BT_DEVICE_MODE_IDLE);
            }
        }
        break;

    case EVENT_BUD_AUTH_SUC:
        {
#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (!app_db.ota_tooling_start)
#endif
            {
                app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
            }
        }
        break;

    default:
        break;
    }
}


static void app_bt_policy_state_afe_wait_coupling_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_BUD_CONN_SUC:
        {
            app_stop_timer(&timer_idx_wait_coupling);

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (!app_db.ota_tooling_start)
#endif
            {
                app_bt_policy_linkback_sched();
            }
        }
        break;

    case EVENT_BUD_WAIT_COUPLING_TO:
        {
#if (F_APP_OTA_TOOLING_SUPPORT == 1)
            if (app_db.ota_tooling_start)
            {
                app_bt_policy_enter_state(STATE_OTA_SHAKING, BT_DEVICE_MODE_IDLE);
            }
            else
#endif
            {
                app_bt_policy_primary_page_scan_param_adjust();

                app_bt_policy_linkback_sched();
            }
        }
        break;

    default:
        break;
    }
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void app_bt_policy_state_afe_linkback_event_handle(T_EVENT event, T_BT_PARAM *param)
{
    uint8_t *bd_addr;

    if (!param->not_check_addr_flag)
    {
        if (!linkback_active_node_judge_cur_conn_addr(param->bd_addr))
        {
            return;
        }
    }

    switch (event)
    {
    case EVENT_SRC_CONN_FAIL:
        {
            if (after_stop_sdp_todo_linkback_run_flag)
            {
                after_stop_sdp_todo_linkback_run_flag = false;
                linkback_active_node.is_valid = false;
            }

            if (0 == linkback_todo_queue_node_num())
            {
                linkback_active_node_src_conn_fail_adjust_remain_profs();

                if (rws_link_lost && app_bt_policy_b2s_connected_is_empty())
                {
                    app_bt_policy_engage_sched();
                }
                else
                {
                    app_bt_policy_linkback_run();
                }
            }
            else
            {
                if (rws_link_lost && app_bt_policy_b2s_connected_is_empty())
                {
                    app_bt_policy_engage_sched();
                }
                else
                {
                    app_stop_timer(&timer_idx_linkback);

                    linkback_active_node_use_left_time_insert_to_queue_again(true);

                    linkback_active_node.is_valid = false;

                    app_bt_policy_linkback_run();
                }
            }
        }
        break;

    case EVENT_SRC_CONN_FAIL_ACL_EXIST:
        {
            app_stop_timer(&timer_idx_linkback_delay);
            app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                            bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, event, false,
                            1300);
        }
        break;

    case EVENT_SRC_CONN_FAIL_CONTROLLER_BUSY:
        {
            app_stop_timer(&timer_idx_linkback_delay);
#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
            app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                            bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, event, false,
                            1500);
#else
            app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                            bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, event, false,
                            500);
#endif
        }
        break;

    case EVENT_LINKBACK_DELAY_TIMEOUT:
        {
            linkback_active_node_src_conn_fail_adjust_remain_profs();

            if (EVENT_SRC_CONN_FAIL_ACL_EXIST == param->cause)
            {
                if (!engage_done && app_bt_policy_b2s_connected_is_empty())
                {
                    app_bt_policy_engage_sched();
                }
                else
                {
                    app_bt_policy_linkback_run();
                }
            }
            else
            {
                app_bt_policy_linkback_run();
            }
        }
        break;

    case EVENT_SRC_AUTH_SUC:
        {
            if (!linkback_active_node_judge_cur_conn_addr(param->bd_addr))
            {
                if (app_bt_point_br_link_is_full())
                {
                    if (linkback_active_node.is_valid)
                    {
                        bd_addr = linkback_active_node.linkback_node.bd_addr;

                        if (!app_bt_policy_b2s_connected_find_node(bd_addr, NULL))
                        {
                            ENGAGE_PRINT_TRACE0("state_afe_linkback_event_handle: bring forward the linkback timout");

                            app_bt_policy_timer_cback(APP_TIMER_LINKBACK, 0);
                        }
                    }
                }
            }
        }
        break;

    case EVENT_SRC_AUTH_FAIL:
        {
            ENGAGE_PRINT_TRACE0("state_afe_linkback_event_handle: wait EVENT_PROFILE_CONN_FAIL");
        }
        break;

    case EVENT_SRC_DISCONN_LOST:
    case EVENT_SRC_DISCONN_NORMAL:
    case EVENT_SRC_DISCONN_ROLESWAP:
        {
            linkback_active_node.is_exit = true;
            after_stop_sdp_todo_linkback_run_flag = false;

            app_bt_policy_linkback_run();
        }
        break;

    case EVENT_SRC_CONN_TIMEOUT:
        {
            if (linkback_active_node.is_valid)
            {
                bd_addr = linkback_active_node.linkback_node.bd_addr;

                if (!app_bt_policy_b2s_connected_find_node(bd_addr, NULL))
                {
                    //linkback but acl not connected,
                    //the linkback addr is the multilink roleswap resume addr
                    if (!memcmp(bd_addr, app_db.resume_addr, 6))
                    {
                        memset(app_db.resume_addr, 0, 6);
                        app_audio_set_connected_tone_need(false);
                        app_db.disallow_connected_tone_after_inactive_connected = false;
                    }
                }

                if (timer_idx_linkback_delay != NULL)
                {
                    //linkback delay meet linkback timeout,
                    //stop linkback delay timer and do linkback sched directly
                    app_stop_timer(&timer_idx_linkback_delay);
                    app_bt_policy_linkback_run();
                }
                else
                {
                    if (!app_bt_policy_b2s_connected_find_node(bd_addr, NULL))
                    {
                        //linkback but acl not connected, stop connect acl,
                        //wait stop-sdp callback to continue sched
                        after_stop_sdp_todo_linkback_run_flag = true;
                        gap_br_stop_sdp_discov(bd_addr);
                        gap_br_send_acl_disconn_req(bd_addr);
                    }
                    else
                    {
                        //linkback and acl connected, stop connect left profile,
                        //wait current-profile-connect callback to continue sched
                        linkback_active_node.linkback_node.plan_profs = 0;
                    }
                }
            }
            else
            {
                //when linkback timeout, the linkback already finishd,
                //do linkback sched directly
                app_bt_policy_linkback_run();
            }
        }
        break;

    case EVENT_PROFILE_SDP_ATTR_INFO:
        {
            linkback_active_node.linkback_conn_param.protocol_version = param->sdp_info->protocol_version;
            uint8_t server_channel = param->sdp_info->server_channel;

            if (SPP_PROFILE_MASK == linkback_active_node.doing_prof)
            {
                uint8_t local_server_chann;

                if (bt_spp_registered_uuid_check((T_BT_SPP_UUID_TYPE)param->sdp_info->srv_class_uuid_type,
                                                 (T_BT_SPP_UUID_DATA *)(&param->sdp_info->srv_class_uuid_data), &local_server_chann))
                {
                    bool is_sdp_ok = false;
                    if (server_channel == RFC_SPP_CHANN_NUM)
                    {
#if F_APP_GAMING_DONGLE_SUPPORT
                        if (linkback_active_node.linkback_conn_param.server_channel != RFC_SPP_DONGLE_CHANN_NUM)
#endif
                        {
#if XM_XIAOAI_FEATURE_SUPPORT
                            if (linkback_active_node.linkback_conn_param.server_channel != RFC_SPP_XIAOAI_CHANN_NUM)
#endif
                            {
                                if (linkback_active_node.linkback_conn_param.server_channel != RFC_RTK_VENDOR_CHANN_NUM)
                                {
                                    is_sdp_ok = true;
                                }
                            }
                        }
                    }

#if F_APP_GAMING_DONGLE_SUPPORT
                    else if (server_channel == RFC_SPP_DONGLE_CHANN_NUM)
                    {
                        is_sdp_ok = true;
                    }
#endif
#if XM_XIAOAI_FEATURE_SUPPORT
                    else if (server_channel == RFC_SPP_XIAOAI_CHANN_NUM)
                    {
                        is_sdp_ok = true;
                    }
#endif
                    else if (server_channel == RFC_RTK_VENDOR_CHANN_NUM)
                    {
                        is_sdp_ok = true;
                    }

                    if (is_sdp_ok)
                    {
                        linkback_active_node.is_sdp_ok = true;
                        linkback_active_node.linkback_conn_param.server_channel = server_channel;
                        linkback_active_node.linkback_conn_param.local_server_chann = local_server_chann;
                    }
                }
            }
            else
            {
                linkback_active_node.is_sdp_ok = true;
                linkback_active_node.linkback_conn_param.server_channel = server_channel;

                if (PBAP_PROFILE_MASK == linkback_active_node.doing_prof)
                {
                    linkback_active_node.linkback_conn_param.feature = (param->sdp_info->supported_feat) ? true : false;
                }
            }
        }
        break;

    case EVENT_PROFILE_DID_ATTR_INFO:
        {
            linkback_active_node.is_sdp_ok = true;
        }
        break;

    case EVENT_PROFILE_SDP_DISCOV_CMPL:
        {
            if ((!app_bt_policy_b2s_connected_find_node(param->bd_addr, NULL)) ||
                (param->cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)) ||
                (param->cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)) ||
                (param->cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
                (param->cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
            {
                ENGAGE_PRINT_TRACE0("state_afe_linkback_event_handle: wait EVENT_SRC_CONN_FAIL");
            }
            else
            {
                if (linkback_active_node.is_sdp_ok)
                {
                    uint32_t profs = 0;

                    if ((DID_PROFILE_MASK == linkback_active_node.doing_prof) ||
                        (app_bt_policy_b2s_connected_find_node(param->bd_addr, &profs) &&
                         (profs & linkback_active_node.doing_prof)))
                    {
                        linkback_active_node_step_suc_adjust_remain_profs();
                        app_bt_policy_linkback_run();
                    }
                    else
                    {
                        if (linkback_profile_connect_start(linkback_active_node.linkback_node.bd_addr,
                                                           linkback_active_node.doing_prof, &linkback_active_node.linkback_conn_param))
                        {
                            if (PBAP_PROFILE_MASK == linkback_active_node.doing_prof)
                            {
                                linkback_active_node_step_suc_adjust_remain_profs();
                                app_bt_policy_linkback_run();
                            }
                        }
                        else
                        {
                            linkback_active_node_step_fail_adjust_remain_profs();
                            app_bt_policy_linkback_run();
                        }
                    }
                }
                else
                {
                    linkback_active_node_step_fail_adjust_remain_profs();
                    app_bt_policy_linkback_run();
                }
            }
        }
        break;

    case EVENT_PROFILE_SDP_DISCOV_STOP:
        {
        }
        break;

    case EVENT_PROFILE_CONN_SUC:
        {
            if (linkback_active_node_judge_cur_conn_prof(param->bd_addr, param->prof))
            {
#if GFPS_SASS_SUPPORT
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    T_APP_BR_LINK *p_link = app_link_find_b2s_link(param->bd_addr);
                    p_link->connected_by_linkback = true;
                }
#endif
                linkback_active_node_step_suc_adjust_remain_profs();
                app_bt_policy_linkback_run();
            }
        }
        break;

    case EVENT_PROFILE_CONN_FAIL:
        {
            ENGAGE_PRINT_TRACE1("state_afe_linkback_event_handle: EVENT_PROFILE_CONN_FAIL cause %x",
                                param->cause);
            if ((param->cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)) ||
                (param->cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
                (param->cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
            {
            }
            else if (param->cause == (L2C_ERR | L2C_ERR_SECURITY_BLOCK))
            {
                // ENGAGE_PRINT_TRACE0("state_afe_linkback_event_handle: L2C_ERR_SECURITY_BLOCK");

                gap_br_send_acl_disconn_req(param->bd_addr);
            }
            else if (param->cause == (L2C_ERR | L2C_ERR_NO_RESOURCE))
            {
                // ENGAGE_PRINT_TRACE0("state_afe_linkback_event_handle: L2C_ERR_NO_RESOURCE");

                app_stop_timer(&timer_idx_linkback_delay);
#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
                app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                                bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, event, false,
                                1500);
#else
                app_start_timer(&timer_idx_linkback_delay, "linkback_delay",
                                bt_policy_timer_id, APP_TIMER_LINKBACK_DELAY, event, false,
                                500);
#endif
            }
            else
            {
                if (linkback_active_node_judge_cur_conn_prof(param->bd_addr, param->prof))
                {
                    linkback_active_node_step_fail_adjust_remain_profs();
                    app_bt_policy_linkback_run();
                }
            }
        }
        break;

#if F_APP_GAMING_DONGLE_SUPPORT
#if F_APP_SUPPORT_LINKBACK_WHEN_DONGLE_STREAMING == 0
    case EVENT_ENTER_GAMING_DONGLE_STREAMING:
        {
            /* stop linkback phone if gaming dongle start ll streaming */
            if (app_dongle_get_mode() == LINKBACK_MODE_BT_OR_BT_24G)
            {
                linkback_todo_queue_delete_all_node();
                linkback_active_node.is_exit = true;
                app_bt_policy_timer_cback(APP_TIMER_LINKBACK, 0);
            }
        }
        break;
#endif
#endif
    case EVENT_STOP_LINKBACK:
        {
            linkback_todo_queue_delete_all_node();
            linkback_active_node.is_exit = true;
            app_bt_policy_timer_cback(APP_TIMER_LINKBACK, 0);
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_state_afe_stable_event_handle(T_EVENT event)
{
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        switch (event)
        {
        case EVENT_BUD_AUTH_SUC:
            {
                app_bt_policy_enter_state(STATE_AFE_SECONDARY, BT_DEVICE_MODE_IDLE);
            }
            break;

        case EVENT_BUD_DISCONN_LOCAL:
            {
                app_bt_policy_enter_state(STATE_AFE_TIMEOUT_SHAKING, BT_DEVICE_MODE_IDLE);
            }
            break;

        case EVENT_BUD_DISCONN_NORMAL:
            {
                linkback_retry_timeout = app_cfg_const.timer_linkback_timeout;

                if (pri_bp_state == BP_STATE_LINKBACK)
                {
                    engage_afe_change_role(REMOTE_SESSION_ROLE_PRIMARY);
                    app_bt_policy_set_bd_addr();
                    app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);

                    app_bt_policy_linkback_sched();
                }
                else if (BP_STATE_PAIRING_MODE == pri_bp_state ||
                         BP_STATE_STANDBY == pri_bp_state)
                {
                    engage_afe_change_role(REMOTE_SESSION_ROLE_PRIMARY);
                    app_bt_policy_set_bd_addr();
                    app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);

                    is_visiable_flag = false;
                    if (BP_STATE_PAIRING_MODE == pri_bp_state)
                    {
                        is_visiable_flag = true;
                    }
                    app_bt_policy_stable_sched(STABLE_ENTER_MODE_DEDICATED_PAIRING);
                }
                else
                {
                    app_bt_policy_enter_state(STATE_AFE_TIMEOUT_SHAKING, BT_DEVICE_MODE_IDLE);
                }
            }
            break;

        case EVENT_BUD_DISCONN_LOST:
            {
                linkback_retry_timeout = app_cfg_const.timer_link_back_loss;

                app_bt_policy_enter_state(STATE_AFE_TIMEOUT_SHAKING, BT_DEVICE_MODE_IDLE);
            }
            break;

        case EVENT_AFE_SHAKING_DONE:
            {
                app_bt_policy_set_bd_addr();
                app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);

                app_bt_policy_enter_state(STATE_AFE_COUPLING, BT_DEVICE_MODE_IDLE);

                remote_session_open(app_cfg_nv.bud_peer_addr);
                app_bt_policy_event_ind(BP_EVENT_BUD_CONN_START, NULL);
            }
            break;

        default:
            break;
        }
    }
    else
#endif
    {
        switch (event)
        {
        case EVENT_BUD_AUTH_SUC:
        case EVENT_BUD_DISCONN_LOCAL:
        case EVENT_BUD_DISCONN_NORMAL:
        case EVENT_BUD_DISCONN_LOST:
        case EVENT_SRC_AUTH_SUC:
            {
                app_bt_policy_stable_sched(STABLE_ENTER_MODE_AGAIN);
            }
            break;

        case EVENT_SRC_DISCONN_NORMAL:
            {
                if (is_src_authed || (cur_state == STATE_AFE_CONNECTED && app_bt_policy_b2s_connected_is_empty()))
                {
                    is_src_authed = false;

                    app_bt_policy_stable_sched(STABLE_ENTER_MODE_AGAIN);
                }
            }
            break;

        case EVENT_SRC_DISCONN_LOST:
            {
                app_bt_policy_enter_state(STATE_AFE_LINKBACK, BT_DEVICE_MODE_CONNECTABLE);
            }
            break;

        case EVENT_DISCOVERABLE_TIMEOUT:
            {
                app_bt_policy_stable_sched(STABLE_ENTER_MODE_AGAIN);
            }
            break;

#if F_APP_ERWS_SUPPORT
        case EVENT_AFE_SHAKING_DONE:
            {
                app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);

                app_start_timer(&timer_idx_engage_action_adjust, "engage_action_adjust",
                                bt_policy_timer_id, APP_TIMER_ENGAGE_ACTION_ADJUST, 0, false,
                                2500);
            }
            break;

        case EVENT_BUD_ENGAGE_ACTION_ADJUST:
            {
                app_bt_policy_primary_engage_action_adjust();
            }
            break;
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
        case EVENT_ENTER_GAMING_DONGLE_STREAMING:
        case EVENT_EXIT_GAMING_DONGLE_STREAMING:
            {
                app_bt_policy_stable_sched(STABLE_ENTER_MODE_AGAIN);
            }
            break;
#endif

        default:
            break;
        }
    }
}

static void app_bt_policy_state_dut_test_mode_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_SRC_CONN_SUC:
        {
            app_bt_policy_enter_state(STATE_DUT_TEST_MODE, BT_DEVICE_MODE_IDLE);
        }
        break;

    case EVENT_SRC_DISCONN_NORMAL:
    case EVENT_SRC_DISCONN_LOST:
        {
            app_bt_policy_enter_state(STATE_DUT_TEST_MODE, BT_DEVICE_MODE_CONNECTABLE);
        }
        break;

    default:
        break;
    }
}

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
static void app_bt_policy_state_ota_shaking_event_handle(T_EVENT event, T_BT_PARAM *param)
{
    switch (event)
    {
    case EVENT_AFE_SHAKING_DONE:
        {
            app_bt_policy_shaking_done(param->is_shaking_to);
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_state_ota_mode_event_handle(T_EVENT event)
{
    switch (event)
    {
    case EVENT_BUD_AUTH_SUC:
        {
            app_cfg_nv.peer_valid_magic = PEER_VALID_MAGIC;
            app_cfg_nv.first_engaged = 1;
            app_bt_policy_save_engage_related_nv();
            app_bt_policy_modify_white_list();
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
        }
        break;

    default:
        break;
    }
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool app_bt_policy_judge_dedicated_enter_pairing_mode(void)
{
    bool ret = true;

    if (!is_force_flag)
    {
        if (app_bt_point_is_full() && app_bt_point_br_link_is_full() && app_bt_point_lea_link_is_full())
        {
            ret = false;
        }
    }

    ENGAGE_PRINT_TRACE1("judge_dedicated_enter_pairing_mode: ret %d", ret);

    return ret;
}

#if F_APP_GAMING_DONGLE_SUPPORT
static bool app_bt_policy_check_disc_src_when_src_full(uint8_t *addr)
{
    bool ret = false;
    uint8_t change_pairing = 0; /* 1: bt pairing, 2: 2.4g pairing */

    /* consider to disc which src when two src connected */
    if (app_cfg_const.enable_dongle_dual_mode && app_dongle_is_24g_mode() == false)
    {
        if (app_cfg_const.enable_dongle_multi_pairing)
        {
            if (app_link_check_phone_link(addr))
            {
                if (dongle_ctrl_data.switch_pairing_triggered == false)
                {
                    /* disc bt & enter bt pairing */
                    change_pairing = 1;
                    ret = true;
                }
            }
        }
        else
        {
            T_APP_BR_LINK *p_link = app_link_find_b2s_link(addr);

            if (p_link && p_link->streaming_fg == false)
            {
                ret = true;
            }
        }
    }
    else
    {
        ret = true;
    }

    if (dongle_ctrl_data.keep_pairing_mode_type == DONGLE_PAIRING_MODE_BT)
    {
        change_pairing = 1;
    }
    else if (dongle_ctrl_data.keep_pairing_mode_type == DONGLE_PAIRING_MODE_24G)
    {
        change_pairing = 2;
    }
    dongle_ctrl_data.keep_pairing_mode_type = DONGLE_PAIRING_MODE_NONE;

    if (change_pairing == 1)
    {
        app_cfg_nv.is_bt_pairing = true;
    }
    else if (change_pairing == 2)
    {
        app_cfg_nv.is_bt_pairing = false;
    }

    APP_PRINT_TRACE2("app_bt_policy_check_disc_src: ret %d change_pairing %d", ret, change_pairing);

    return ret;
}

static void app_bt_policy_check_switch_pairing_when_src_not_full(void)
{
    uint8_t change_pairing = 0; /* 1: bt pairing, 2: 2.4g pairing */

    if (app_cfg_const.enable_dongle_multi_pairing)
    {
        if (app_link_get_b2s_link_num() == 0)
        {
            change_pairing = 2;
        }
        else /* app_link_get_b2s_link_num == 1 for src not full */
        {
            if (app_dongle_get_connected_dongle_link() == NULL)
            {
                change_pairing = 2;
            }
            else
            {
                change_pairing = 1;
            }
        }
    }

    if (dongle_ctrl_data.keep_pairing_mode_type == DONGLE_PAIRING_MODE_BT)
    {
        change_pairing = 1;
    }
    else if (dongle_ctrl_data.keep_pairing_mode_type == DONGLE_PAIRING_MODE_24G)
    {
        change_pairing = 2;
    }
    dongle_ctrl_data.keep_pairing_mode_type = DONGLE_PAIRING_MODE_NONE;

    if (change_pairing != 0)
    {
        if (change_pairing == 1)
        {
            app_cfg_nv.is_bt_pairing = true;
        }
        else if (change_pairing == 2)
        {
            app_cfg_nv.is_bt_pairing = false;
        }
    }

    APP_PRINT_TRACE1("app_bt_policy_check_switch_pairing_when_src_not_full: change_pairing %d",
                     change_pairing);

}
#endif

static void app_bt_policy_prepare_for_dedicated_enter_pairing_mode(void)
{
    ENGAGE_PRINT_TRACE0("prepare_for_dedicated_enter_pairing_mode");

    disconnect_for_pairing_mode = false;

    if (!(app_bt_point_is_full()))
    {
#if F_APP_DURIAN_SUPPORT
        if (!app_durian_cfg_multi_is_on())
        {
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                    disconnect_for_pairing_mode = true;
                }
            }
        }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
        if (dongle_ctrl_data.switch_pairing_triggered == false)
        {
            app_bt_policy_check_switch_pairing_when_src_not_full();
        }
#endif

        if (!disconnect_for_pairing_mode)
        {
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_DEDICATED_PAIRING);
        }
    }
    else
    {
        if (is_force_flag)
        {
            if (mtc_get_btmode() == MULTI_PRO_BT_BREDR)
            {
                uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

                for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
                {
                    if (app_db.le_link[i].used == true &&
                        app_db.le_link[i].state == LE_LINK_STATE_CONNECTED &&
                        memcmp(app_db.le_link[i].bd_addr, app_db.br_link[active_a2dp_idx].bd_addr, 6) != 0)
                    {
                        T_APP_BR_LINK *p_link = app_link_find_br_link(app_db.le_link[i].bd_addr);

                        app_ble_gap_disconnect(&app_db.le_link[i], LE_LOCAL_DISC_CAUSE_ENTER_PAIRING);
                        is_force_flag = false;
                        if (p_link)
                        {
                            app_bt_policy_disconnect(p_link->bd_addr, ALL_PROFILE_MASK);
                            disconnect_for_pairing_mode = true;
                        }
                        else
                        {
                            app_bt_policy_stable_sched(STABLE_ENTER_MODE_DEDICATED_PAIRING);
                        }
                        return;
                    }
                }

#if F_APP_TEAMS_BT_POLICY
                disconnect_for_pairing_mode = app_teams_bt_policy_prepare_for_dedicated_enter_pairing_mode();
                if (disconnect_for_pairing_mode)
                {
                    is_force_flag = false;
                    return;
                }
#endif

#if F_APP_DURIAN_SUPPORT
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                        disconnect_for_pairing_mode = true;
                    }
                }
#else
                if (((app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                     || (app_db.br_link[active_a2dp_idx].streaming_fg == true))
                    && (app_cfg_const.enable_multi_link))
                {
                    for (uint8_t inactive_a2dp_idx = 0; inactive_a2dp_idx < MAX_BR_LINK_NUM; inactive_a2dp_idx++)
                    {
                        if (app_link_check_b2s_link_by_id(inactive_a2dp_idx))
                        {
                            if (inactive_a2dp_idx != active_a2dp_idx)
                            {
#if HARMAN_DISCONN_ACTIVE_A2DP_WHEN_OTA
                                if (app_harman_ble_ota_get_upgrate_status())
                                {
                                    for (int i = 0; i < MAX_BLE_LINK_NUM; i++)
                                    {
                                        if (app_db.le_link[i].used == true &&
                                            (memcmp(app_db.le_link[i].bd_addr, app_db.br_link[inactive_a2dp_idx].bd_addr, 6) == 0))
                                        {
                                            //disconn active a2dp, when inactive a2dp src is in OTA
                                            app_bt_policy_disconnect(app_db.br_link[active_a2dp_idx].bd_addr, ALL_PROFILE_MASK);
                                            disconnect_for_pairing_mode = true;
                                            break;
                                        }
                                    }
                                }
                                else
#endif
                                {
                                    app_bt_policy_disconnect(app_db.br_link[inactive_a2dp_idx].bd_addr, ALL_PROFILE_MASK);
                                    disconnect_for_pairing_mode = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_link_check_b2s_link_by_id(i))
                        {
#if HARMAN_DISCONN_ACTIVE_A2DP_WHEN_OTA
                            if (app_harman_ble_ota_get_upgrate_status())
                            {
                                for (int idx = 0; idx < MAX_BLE_LINK_NUM; idx++)
                                {
                                    if (app_db.le_link[idx].used == true &&
                                        (memcmp(app_db.le_link[idx].bd_addr, app_db.br_link[i].bd_addr, 6) != 0))
                                    {
                                        //disconn the link which is not in OTA
                                        app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                                        disconnect_for_pairing_mode = true;
                                        break;
                                    }
                                }
                            }
                            else
#endif
                            {
#if F_APP_GAMING_DONGLE_SUPPORT
                                if (app_bt_policy_check_disc_src_when_src_full(app_db.br_link[i].bd_addr))
#else
                                if (app_bt_policy_b2s_connected_is_first_src(app_db.br_link[i].bd_addr))
#endif
                                {
                                    app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                                    disconnect_for_pairing_mode = true;
                                    break;
                                }
                            }
                        }
                    }
                }
#endif
            }
            else if (mtc_get_btmode() == MULTI_PRO_BT_CIS)
            {
                T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());

                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i) &&
                        memcmp(app_db.br_link[i].bd_addr, p_link->bd_addr, 6) != 0)
                    {
                        T_APP_LE_LINK *p_le_link = NULL;

                        p_le_link = app_link_find_le_link_by_addr(app_db.br_link[i].bd_addr);
                        if (p_le_link)
                        {
                            app_ble_gap_disconnect(p_le_link, LE_LOCAL_DISC_CAUSE_ENTER_PAIRING);
                        }
                        app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                        disconnect_for_pairing_mode = true;
                        is_force_flag = false;
                        return;
                    }
                }

                if (p_link != NULL)
                {
                    if (p_link->media_state == MCS_MEDIA_STATE_PLAYING ||
                        p_link->media_state == MCS_MEDIA_STATE_SEEKING ||
                        p_link->lea_link_state == LEA_LINK_STREAMING)
                    {
                        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
                        {
                            if (app_db.le_link[i].used == true &&
                                app_db.le_link[i].state == LE_LINK_STATE_CONNECTED &&
                                app_db.le_link[i].conn_handle != p_link->conn_handle)
                            {
                                app_ble_gap_disconnect(&app_db.le_link[i], LE_LOCAL_DISC_CAUSE_ENTER_PAIRING);
                                app_bt_policy_stable_sched(STABLE_ENTER_MODE_DEDICATED_PAIRING);
                                is_force_flag = false;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
                        {
                            if (app_db.le_link[i].used == true &&
                                app_db.le_link[i].state == LE_LINK_STATE_CONNECTED &&
                                app_db.le_link[i].conn_handle != app_ble_gap_last_src_conn_handle())
                            {
                                app_ble_gap_disconnect(&app_db.le_link[i], LE_LOCAL_DISC_CAUSE_ENTER_PAIRING);
                                app_bt_policy_stable_sched(STABLE_ENTER_MODE_DEDICATED_PAIRING);
                                is_force_flag = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    is_force_flag = false;
}

static void app_bt_policy_dedicated_enter_pairing_mode_event_handle(T_BT_PARAM *param)
{
    is_visiable_flag = param->is_visiable;
    is_force_flag = param->is_force;

    switch (cur_state)
    {
    case STATE_STARTUP:
        {
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_DIRECT_PAIRING);
        }
        break;

    case STATE_AFE_LINKBACK:
        {
            if (app_bt_policy_judge_dedicated_enter_pairing_mode())
            {
                app_stop_timer(&timer_idx_linkback);

                if (linkback_active_node.is_valid)
                {
                    after_stop_sdp_todo_linkback_run_flag = false;
                    gap_br_stop_sdp_discov(linkback_active_node.linkback_node.bd_addr);
                    gap_br_send_acl_disconn_req(linkback_active_node.linkback_node.bd_addr);
                }

                linkback_todo_queue_init();
                linkback_active_node_init();

                app_bt_policy_prepare_for_dedicated_enter_pairing_mode();
            }
        }
        break;

    case STATE_AFE_CONNECTED:
    case STATE_AFE_PAIRING_MODE:
    case STATE_AFE_STANDBY:
        {
            if (app_bt_policy_judge_dedicated_enter_pairing_mode())
            {
                app_bt_policy_prepare_for_dedicated_enter_pairing_mode();
            }
        }
        break;

    default:
        {
            dedicated_enter_pairing_mode_flag = true;
        }
        break;
    }
}

static void app_bt_policy_dedicated_exit_pairing_mode_event_handle(void)
{
    dedicated_enter_pairing_mode_flag = false;

    switch (cur_state)
    {
    case STATE_AFE_CONNECTED:
    case STATE_AFE_PAIRING_MODE:
    case STATE_AFE_STANDBY:
        {
            app_bt_policy_stable_sched(STABLE_ENTER_MODE_NOT_PAIRING);
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_dedicated_connect_event_handle(T_BT_PARAM *bt_param)
{
    T_APP_BR_LINK *p_link;

    switch (cur_state)
    {
    case STATE_AFE_TIMEOUT_SHAKING:
    case STATE_AFE_WAIT_COUPLING:
        {
            linkback_todo_queue_insert_force_node(bt_param->bd_addr, bt_param->prof, bt_param->is_special,
                                                  bt_param->search_param, bt_param->check_bond_flag, app_cfg_const.timer_linkback_timeout * 1000,
                                                  false);
        }
        break;

    case STATE_AFE_LINKBACK:
        {
            if (!bt_param->is_special && linkback_active_node_judge_cur_conn_addr(bt_param->bd_addr))
            {
                linkback_active_node_remain_profs_add(bt_param->prof, bt_param->check_bond_flag,
                                                      app_cfg_const.timer_linkback_timeout * 1000);

                app_start_timer(&timer_idx_linkback, "linkback",
                                bt_policy_timer_id, APP_TIMER_LINKBACK, 0, false,
                                linkback_active_node.retry_timeout);
            }
            else
            {
                linkback_todo_queue_insert_force_node(bt_param->bd_addr, bt_param->prof, bt_param->is_special,
                                                      bt_param->search_param, bt_param->check_bond_flag, app_cfg_const.timer_linkback_timeout * 1000,
                                                      false);
            }
        }
        break;

    case STATE_AFE_CONNECTED:
    case STATE_AFE_PAIRING_MODE:
    case STATE_AFE_STANDBY:
        {
            if (bt_param->is_later_avrcp)
            {
                p_link = app_link_find_br_link(bt_param->bd_addr);
                if (p_link != NULL)
                {
                    if (0 == (AVRCP_PROFILE_MASK & p_link->connected_profile))
                    {
                        linkback_profile_connect_start(bt_param->bd_addr, AVRCP_PROFILE_MASK, NULL);
                    }
                }
            }
            else if (DID_PROFILE_MASK == bt_param->prof)
            {
                p_link = app_link_find_br_link(bt_param->bd_addr);
                if (p_link != NULL)
                {
                    gap_br_start_did_discov(bt_param->bd_addr);
                }
            }
            else if (bt_param->is_later_hid)
            {
                p_link = app_link_find_br_link(bt_param->bd_addr);
                if (p_link != NULL)
                {
                    if (0 == (HID_PROFILE_MASK & p_link->connected_profile))
                    {
                        linkback_profile_connect_start(bt_param->bd_addr, HID_PROFILE_MASK, NULL);
                    }
                }
            }
            else
            {
                linkback_todo_queue_insert_force_node(bt_param->bd_addr, bt_param->prof, bt_param->is_special,
                                                      bt_param->search_param, bt_param->check_bond_flag, app_cfg_const.timer_linkback_timeout * 1000,
                                                      false);

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_bt_policy_enter_state(STATE_AFE_LINKBACK, BT_DEVICE_MODE_CONNECTABLE);
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_dedicated_disconnect_event_handle(T_BT_PARAM *bt_param)
{
    uint8_t *bd_addr;
    uint32_t plan_profs;
    uint32_t exist_profs;
    T_APP_BR_LINK *p_link;

    bd_addr = bt_param->bd_addr;
    plan_profs = bt_param->prof;

    if (linkback_active_node_judge_cur_conn_addr(bd_addr))
    {
        linkback_active_node_remain_profs_sub(plan_profs);
    }

    linkback_todo_queue_remove_plan_profs(bd_addr, plan_profs);

    if (app_bt_policy_b2s_connected_find_node(bd_addr, &exist_profs))
    {
        p_link = app_link_find_br_link(bd_addr);

        if (p_link != NULL)
        {
            if (plan_profs & AVRCP_PROFILE_MASK)
            {
                app_stop_timer(&timer_idx_later_avrcp[p_link->id]);
            }

            if (plan_profs & HID_PROFILE_MASK)
            {
                app_stop_timer(&timer_idx_later_hid[p_link->id]);
            }

            p_link->disconn_acl_flg = false;
            plan_profs &= exist_profs;
            if (plan_profs == exist_profs)
            {
                p_link->disconn_acl_flg = true;
                p_link->plan_disconnect_profs = plan_profs;
            }

            linkback_profile_disconnect_start(bd_addr, plan_profs);
        }

    }
}

static void app_bt_policy_disconnect_all_event_handle(void)
{
    uint32_t i;

    ENGAGE_PRINT_TRACE0("disconnect_all_event_handle");

    if (b2b_connected)
    {
        gap_br_send_acl_disconn_req(app_cfg_nv.bud_peer_addr);
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (app_db.br_link[i].sco_handle)
                {
                    gap_br_send_sco_disconn_req(app_db.br_link[i].bd_addr);
                }
                gap_br_send_acl_disconn_req(app_db.br_link[i].bd_addr);
            }
        }
    }
}

#if F_APP_B2B_ENGAGE_IMPROVE_BY_LEA_DONGLE
void app_bt_policy_change_to_sec_and_conn_pri(void)
{
    // cancel linkback phone
    app_bt_policy_stop_all_active_action();

    // change to secondary
    engage_afe_change_role(REMOTE_SESSION_ROLE_SECONDARY);

    // connect to primary
    app_bt_policy_shaking_done(false);
}
#endif

static void app_bt_policy_engage_ind(T_ENGAGE_IND ind)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    switch (ind)
    {
    case ENGAGE_IND_FE_SHAKING_DONE:
        {
            app_bt_policy_state_machine(EVENT_FE_SHAKING_DONE, NULL);

#if F_APP_LEA_SUPPORT
            app_lea_adv_update();
#endif
        }
        break;

    case ENGAGE_IND_AFE_SHAKING_DONE:
        {
            bt_param.is_shaking_to = false;
            app_bt_policy_state_machine(EVENT_AFE_SHAKING_DONE, &bt_param);

#if F_APP_LEA_SUPPORT
            app_lea_adv_update();
#endif
        }
        break;

    case ENGAGE_IND_AFE_SHAKING_TO:
        {
            bt_param.is_shaking_to = true;
            app_bt_policy_state_machine(EVENT_AFE_SHAKING_DONE, &bt_param);

#if F_APP_LEA_SUPPORT
            //Role swap fail will trigger B2B link lost, and enter AFE shaking
            if ((app_db.is_lea_device_connected == 1) && rws_link_lost)
            {
                app_db.ever_roleswapped = (app_cfg_nv.ori_bud_role != app_cfg_nv.bud_role) ? 1 : 0;
            }
            app_lea_adv_update();
#endif
        }
        break;

    default:
        break;
    }
}

static void app_bt_policy_engage_sched(void)
{
    ENGAGE_PRINT_TRACE0("engage_sched");

#if F_APP_ERWS_SUPPORT
    if (!engage_done
#if F_APP_SAIYAN_MODE
        || data_capture_saiyan.saiyan_enable
#endif
       )
    {
        if (!app_cfg_nv.first_engaged
#if F_APP_SAIYAN_MODE
            || data_capture_saiyan.saiyan_enable
#endif
           )
        {
            if (PEER_VALID_MAGIC != app_cfg_nv.peer_valid_magic)
            {
                if (app_cfg_const.first_time_engagement_only_by_5v_command)
                {
                    /* first engage in factory */
                    if (app_db.jig_dongle_id == 0 && app_cfg_nv.one_wire_start_force_engage == 0)
                    {
                        ENGAGE_PRINT_TRACE0("disallow first engage not trigger by 5v command");
                        app_bt_policy_stable_sched(STABLE_ENTER_MODE_DIRECT_PAIRING);
                        return;
                    }
                }
            }

            app_bt_policy_enter_state(STATE_FE_SHAKING, BT_DEVICE_MODE_IDLE);
        }
        else
        {
            app_bt_policy_enter_state(STATE_AFE_TIMEOUT_SHAKING, BT_DEVICE_MODE_IDLE);
        }
    }
    else
#endif
    {
        app_bt_policy_linkback_sched();
    }
}

static bool app_bt_policy_enable_b2s_disconnect_enter_pairing(void)
{
    bool ret = true;
    uint8_t reason = 0;

    if (!app_cfg_const.rws_disconnect_enter_pairing)
    {
        reason = 1;
    }
    else if ((!app_cfg_const.enable_rtk_charging_box && app_device_is_in_the_box()))
    {
        reason = 2;
    }
    else if (anc_tool_check_resp_meas_mode() != ANC_RESP_MEAS_MODE_NONE)
    {
        reason = 3;
    }
#if F_APP_GAMING_DONGLE_SUPPORT
    else if (app_link_get_b2s_link_num() == 1)
    {
        bool enter_pairing = false;

        if (app_cfg_const.enter_pairing_while_only_one_device_connected)
        {
            enter_pairing = true;
        }
        else
        {
            if (app_dongle_get_connected_dongle_link())
            {
                enter_pairing = true;
            }
        }

        if (enter_pairing == false)
        {
            reason = 4;
        }
    }
    else if (app_link_get_b2s_link_num() == 0)
    {
        /* enter pairing; do nothing and return true */
    }
#else
    else if (((!app_bt_policy_b2s_connected_is_empty()) &&
              (!app_cfg_const.enter_pairing_while_only_one_device_connected)))
    {
        reason = 4;
    }
#endif
#if F_APP_HARMAN_FEATURE_SUPPORT
    else if (app_harman_power_on_link_back_flag_get() == false)
    {
        reason = 5;
    }
#endif

    if (reason != 0)
    {
        ret = false;
    }

#if F_APP_GAMING_DONGLE_SUPPORT
    if (ret)
    {
        dongle_ctrl_data.trigger_pairing_non_intentionally = true;
    }
#endif

    APP_PRINT_TRACE2("app_bt_policy_enable_b2s_disconnect_enter_pairing: ret %d reason %d", ret,
                     reason);

    return ret;
}

#if F_APP_GAMING_DONGLE_SUPPORT && F_APP_DISALLOW_AUTO_PAIRING == 0
static bool app_bt_policy_check_enter_standby(void)
{
    uint8_t reason = 0;

    if (app_cfg_const.enable_dongle_dual_mode)
    {
        /* disallow 2.4g auto pairing only */
        if (app_dongle_is_24g_mode())
        {
            reason = 1;
        }
        else if (app_cfg_const.enable_multi_link)
        {
            if (app_cfg_const.enable_dongle_multi_pairing && app_cfg_nv.is_bt_pairing == false)
            {
                reason = 2;
            }
            else if (app_dongle_is_streaming())
            {
                /* disallow enter pairing when dongle streaming */
                reason = 3;
            }
        }
    }

    APP_PRINT_TRACE1("app_bt_policy_check_enter_standby: reason %d", reason);

    return reason != 0 ? true : false;
}
#endif

static void app_bt_policy_stable_sched(T_STABLE_ENTER_MODE mode)
{
    app_bt_policy_stable_enter_mode(mode);

    app_stop_timer(&timer_idx_linkback);
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        startup_linkback_done_flag = false;
        bond_list_load_flag = false;

        switch (mode)
        {
        case STABLE_ENTER_MODE_NORMAL:
            {
                app_bt_policy_enter_state(STATE_AFE_SECONDARY, BT_DEVICE_MODE_IDLE);
            }
            break;

        case STABLE_ENTER_MODE_DIRECT_PAIRING:
            {
                engage_afe_change_role(REMOTE_SESSION_ROLE_PRIMARY);
                app_bt_policy_set_bd_addr();

                app_bt_policy_enter_state(STATE_AFE_PAIRING_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);

                app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);
            }
            break;

        default:
            break;
        }
    }
    else
#endif
    {
        startup_linkback_done_flag = true;

        switch (mode)
        {
        case STABLE_ENTER_MODE_NORMAL:
            {
                if (app_bt_point_is_empty()
#if F_APP_SAIYAN_MODE
                    && data_capture_saiyan.saiyan_enable == 0
#endif
                   )
                {
                    bool force_standby = false;

#if F_APP_GAMING_DONGLE_SUPPORT
#if F_APP_DISALLOW_AUTO_PAIRING
                    if (app_cfg_const.enable_dongle_dual_mode)
                    {
                        if (dongle_ctrl_data.enter_pairing_after_mode_switch == false)
                        {
                            force_standby = true;
                        }
                    }
#else
                    if (app_bt_policy_check_enter_standby())
                    {
                        force_standby = true;
                    }
#endif
#endif

                    if ((!force_standby)
                        && (!linkback_flag || app_cfg_const.enable_power_on_linkback_fail_enter_pairing)
                        && (app_hfp_get_call_status() == APP_CALL_IDLE)
                        && (app_lea_adv_linkbacking() == false)
#if F_APP_HARMAN_FEATURE_SUPPORT
                        && (app_harman_power_on_link_back_flag_get() == false)
#endif
                       )
                    {
                        app_bt_policy_enter_state(STATE_AFE_PAIRING_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
                    }
                    else
                    {
                        app_bt_policy_enter_state(STATE_AFE_STANDBY, BT_DEVICE_MODE_CONNECTABLE);
                    }
                }
                else
                {
                    if ((app_bt_point_is_full() && app_bt_point_br_link_is_full() &&
                         app_bt_point_lea_link_is_full()) && engage_done)
                    {
                        app_bt_policy_enter_state(STATE_AFE_CONNECTED, BT_DEVICE_MODE_IDLE);
                    }
                    else
                    {
                        app_bt_policy_enter_state(STATE_AFE_CONNECTED, BT_DEVICE_MODE_CONNECTABLE);
                    }
                }
                app_bt_policy_qos_param_update(app_db.br_link[app_a2dp_get_active_idx()].bd_addr,
                                               BP_TPOLL_LINKBACK_STOP);
                linkback_flag = false;

#if F_APP_GAMING_DONGLE_SUPPORT
                dongle_ctrl_data.enter_pairing_after_mode_switch = false;
#endif
            }
            break;

        case STABLE_ENTER_MODE_AGAIN:
            {
                if (app_bt_policy_b2s_connected_is_empty())
                {
                    bool force_standby = false;

#if F_APP_GAMING_DONGLE_SUPPORT
#if F_APP_DISALLOW_AUTO_PAIRING
                    if (app_cfg_const.enable_dongle_dual_mode)
                    {
                        force_standby = true;
                    }
#else
                    if (app_bt_policy_check_enter_standby())
                    {
                        force_standby = true;
                    }
#endif
#endif

                    if (!force_standby &&
                        ((cur_state == STATE_AFE_PAIRING_MODE) ||
                         (cur_event == EVENT_SRC_DISCONN_NORMAL && app_bt_policy_enable_b2s_disconnect_enter_pairing()) ||
                         (cur_event == EVENT_BUD_AUTH_SUC)) &&
                        (app_hfp_get_call_status() == APP_CALL_IDLE))
                    {
                        if (cur_event == EVENT_SRC_DISCONN_NORMAL && app_bt_policy_enable_b2s_disconnect_enter_pairing())
                        {
                            is_user_action_pairing_mode = true;
                        }
                        else
                        {
                            is_user_action_pairing_mode = false;
                        }
                        app_bt_policy_enter_state(STATE_AFE_PAIRING_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
                    }
                    else
                    {
                        app_bt_policy_enter_state(STATE_AFE_STANDBY, BT_DEVICE_MODE_CONNECTABLE);
                    }
                }
                else
                {
                    if (((cur_state == STATE_AFE_PAIRING_MODE && cur_event != EVENT_SRC_AUTH_SUC) ||
                         (cur_event == EVENT_SRC_DISCONN_NORMAL && app_bt_policy_enable_b2s_disconnect_enter_pairing())) &&
                        (app_hfp_get_call_status() == APP_CALL_IDLE))
                    {
                        if (cur_event == EVENT_SRC_DISCONN_NORMAL)
                        {
                            is_user_action_pairing_mode = true;
                        }
                        else
                        {
                            is_user_action_pairing_mode = false;
                        }
                        app_bt_policy_enter_state(STATE_AFE_PAIRING_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
                    }
                    else
                    {
                        if (((app_bt_point_is_full() && app_bt_point_br_link_is_full() &&
                              app_bt_point_lea_link_is_full()) && engage_done)
                            || (app_db.connected_num_before_roleswap > app_link_get_b2s_link_num()))
                        {
                            app_bt_policy_enter_state(STATE_AFE_CONNECTED, BT_DEVICE_MODE_IDLE);
                        }
                        else
                        {
                            app_bt_policy_enter_state(STATE_AFE_CONNECTED, BT_DEVICE_MODE_CONNECTABLE);
                        }
                    }
                    app_db.disconnect_inactive_link_actively = false;
                }
            }
            break;

        case STABLE_ENTER_MODE_DEDICATED_PAIRING:
            {
                app_bt_policy_event_ind(BP_EVENT_DEDICATED_PAIRING, NULL);

                is_user_action_pairing_mode = true;

                if (is_visiable_flag)
                {
                    app_bt_policy_enter_state(STATE_AFE_PAIRING_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
                }
                else
                {
                    app_bt_policy_enter_state(STATE_AFE_STANDBY, BT_DEVICE_MODE_CONNECTABLE);
                }
            }
            break;

        case STABLE_ENTER_MODE_NOT_PAIRING:
            {
#if F_APP_ANC_SUPPORT
                T_ANC_FEATURE_MAP feature_map;
                feature_map.d32 = anc_tool_get_feature_map();
#endif
                if ((is_pairing_timeout)
#if F_APP_HARMAN_FEATURE_SUPPORT
                    && (app_harman_ever_link_information() == 0)
#else
                    && (app_cfg_const.enable_pairing_timeout_to_power_off)
#endif
#if F_APP_ANC_SUPPORT
                    && (feature_map.user_mode == ENABLE)
                    && (!app_anc_ramp_tool_is_busy())
#endif
#if F_APP_LEA_SUPPORT
                    && (!app_lea_bca_state() && app_link_get_lea_link_num() == 0 && app_db.remote_lea_link_num == 0)
#endif
                    && (app_link_get_le_link_num() == 0)
                    && (app_bt_policy_b2s_connected_is_empty())
#if F_APP_LISTENING_MODE_SUPPORT
                    && ((app_cfg_const.enable_auto_power_off_when_listening_mode_is_not_off)
                        || (app_db.current_listening_state == ANC_OFF_APT_OFF))
#endif

#if F_APP_LINEIN_SUPPORT
                    && (!app_line_in_plug_state_get())
#endif
#if F_APP_USB_AUDIO_SUPPORT
                    && (!app_usb_connected())
#endif
                   )
                {
                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        app_db.power_off_cause = POWER_OFF_CAUSE_EXIT_PAIRING_MODE;
                        app_mmi_handle_action(MMI_DEV_POWER_OFF);
                    }
#if F_APP_ERWS_SUPPORT
                    else
                    {
                        uint8_t action = MMI_DEV_POWER_OFF;

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
                    }
#endif
                }
                else
                {
                    if (app_bt_policy_b2s_connected_is_empty())
                    {
                        app_bt_policy_enter_state(STATE_AFE_STANDBY, BT_DEVICE_MODE_CONNECTABLE);
                    }
                    else
                    {
                        if ((app_bt_point_is_full() && app_bt_point_br_link_is_full() && app_bt_point_lea_link_is_full()) &&
                            engage_done)
                        {
                            app_bt_policy_enter_state(STATE_AFE_CONNECTED, BT_DEVICE_MODE_IDLE);
                        }
                        else
                        {
                            app_bt_policy_enter_state(STATE_AFE_CONNECTED, BT_DEVICE_MODE_CONNECTABLE);
                        }
                    }
                }

                is_pairing_timeout = false;
            }
            break;

        case STABLE_ENTER_MODE_DIRECT_PAIRING:
            {
                app_bt_policy_enter_state(STATE_AFE_PAIRING_MODE, BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);

                app_bt_policy_event_ind(BP_EVENT_ROLE_DECIDED, NULL);
            }
            break;

        default:
            break;
        }
#if F_APP_HARMAN_FEATURE_SUPPORT
        app_harman_power_on_link_back_flag_set(true);
#endif
    }
}

static bool app_bt_policy_bt_event_handle(T_EVENT event, T_BT_PARAM *param)
{
    bool is_done = false;

    switch (event)
    {
    case EVENT_SRC_DISCONN_NORMAL:
        {
            if (disconnect_for_pairing_mode)
            {
                disconnect_for_pairing_mode = false;

                app_bt_policy_stable_sched(STABLE_ENTER_MODE_DEDICATED_PAIRING);

                is_done = true;
            }
        }
        break;

    case EVENT_PAIRING_MODE_TIMEOUT:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_bt_policy_stable_sched(STABLE_ENTER_MODE_NOT_PAIRING);
            }
        }
        break;

    default:
        break;
    }

    return is_done;
}

static bool app_bt_policy_bt_event_pre_handle(T_EVENT event, T_BT_PARAM *param)
{
    bool is_done = true;
    T_BP_EVENT_PARAM event_param;
    T_APP_BR_LINK *p_link;

    if (param != NULL)
    {
        event_param.bd_addr = param->bd_addr;
        event_param.cause = param->cause;
    }

    switch (event)
    {
#if F_APP_ERWS_SUPPORT
    case EVENT_BUD_CONN_SUC:
        {
            app_bt_policy_b2b_connected_add_node(param->bd_addr);

            engage_afe_done();

            app_bt_policy_primary_page_scan_param_adjust();

            if (STATE_FE_SHAKING == cur_state)
            {
                gap_br_send_acl_disconn_req(param->bd_addr);
            }
        }
        break;

    case EVENT_BUD_CONN_FAIL:
        {
            app_bt_policy_event_ind(BP_EVENT_BUD_CONN_FAIL, NULL);
        }
        break;

    case EVENT_BUD_AUTH_FAIL:
        {
            app_bt_policy_event_ind(BP_EVENT_BUD_AUTH_FAIL, NULL);
        }
        break;

    case EVENT_BUD_AUTH_SUC:
        {
            app_stop_timer(&timer_idx_bud_linklost);
            app_stop_timer(&timer_idx_engage_action_adjust);
            app_bt_policy_connected_node_auth_suc(param->bd_addr);
            app_bt_policy_event_ind(BP_EVENT_BUD_AUTH_SUC, NULL);
        }
        break;

    case EVENT_BUD_DISCONN_LOCAL:
    case EVENT_BUD_DISCONN_NORMAL:
        {
            bool is_authed;

            is_authed = app_bt_policy_connected_node_is_authed(param->bd_addr);

            app_bt_policy_b2b_connected_del_node(param->bd_addr);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_bt_policy_primary_page_scan_param_adjust();
            }
            else
            {
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        app_bt_policy_b2s_connected_del_node(app_db.br_link[i].bd_addr);
                    }
                }

                app_bt_point_br_link_num_set(0);
            }
            app_bt_policy_primary_engage_action_adjust();


            if (is_authed)
            {
                app_bt_policy_event_ind(BP_EVENT_BUD_DISCONN_NORMAL, NULL);
            }
        }
        break;

    case EVENT_BUD_DISCONN_LOST:
        {
            bool is_authed;

            is_authed = app_bt_policy_connected_node_is_authed(param->bd_addr);

            app_bt_policy_b2b_connected_del_node(param->bd_addr);
            rws_link_lost = true;

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_bt_policy_primary_page_scan_param_adjust();
            }
            else
            {
                for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
                {
                    if (app_link_check_b2s_link_by_id(i))
                    {
                        app_bt_policy_b2s_connected_del_node(app_db.br_link[i].bd_addr);
                    }
                }

                app_bt_point_br_link_num_set(0);
            }

            app_bt_policy_primary_engage_action_adjust();

            if (app_cfg_const.rws_linkloss_linkback_timeout != 0)
            {
                app_start_timer(&timer_idx_bud_linklost, "bud_linklost",
                                bt_policy_timer_id, APP_TIMER_BUD_LINKLOST, 0, false,
                                app_cfg_const.rws_linkloss_linkback_timeout * 5000);
            }

            if (is_authed)
            {
                app_bt_policy_event_ind(BP_EVENT_BUD_DISCONN_LOST, NULL);
            }
        }
        break;

    case EVENT_BUD_REMOTE_CONN_CMPL:
        {
            app_bt_policy_b2b_connected_add_prof(param->bd_addr, param->prof);

            app_bt_policy_event_ind(BP_EVENT_BUD_REMOTE_CONN_CMPL, NULL);
        }
        break;

    case EVENT_BUD_REMOTE_DISCONN_CMPL:
        {
            app_bt_policy_b2b_connected_del_prof(param->bd_addr, param->prof);

            app_bt_policy_event_ind(BP_EVENT_BUD_REMOTE_DISCONN_CMPL, NULL);
        }
        break;

    case EVENT_BUD_LINKLOST_TIMEOUT:
        {
            if ((b2b_connected == false) && app_bt_policy_b2s_connected_is_empty())
            {
                app_bt_policy_event_ind(BP_EVENT_BUD_LINKLOST_TIMEOUT, NULL);
            }
        }
        break;
#endif

    case EVENT_SRC_CONN_SUC:
        {
            bool suc;
            uint8_t id;

            suc = app_bt_policy_b2s_connected_add_node(param->bd_addr, &id);
            if (suc)
            {
                if (app_link_get_encryption_device_num() > app_db.b2s_connected_num_max)
                {
                    app_multi_disconnect_inactive_link(id, SASS_EDR_LINK);
#if GFPS_SASS_LEA_SUPPORT
                    app_lea_mgr_disc_inactive_le_link(id, SASS_EDR_LINK);
#endif
                }
                app_bt_policy_sync_b2s_connected();
            }

            if (BT_DEVICE_MODE_IDLE == radio_mode)
            {
                gap_br_send_acl_disconn_req(param->bd_addr);
            }

#if F_APP_LISTENING_MODE_SUPPORT
            if (app_cfg_const.disallow_listening_mode_before_phone_connected)
            {
                app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_CONNECTED);
            }
#endif
        }
        break;

    case EVENT_SRC_CONN_TIMEOUT:
        {
            if (linkback_active_node.is_valid)
            {
                if (linkback_active_node.linkback_node.is_group_member)
                {
                    linkback_todo_queue_delete_group_member();
                }
            }

            if (STATE_AFE_LINKBACK != cur_state)
            {
                linkback_active_node.is_valid = false;
            }
        }
        break;

    case EVENT_SRC_AUTH_LINK_KEY_INFO:
        {
            uint8_t mapping_idx;
            //uint8_t reset_volume = 0;

#if F_APP_TEAMS_BT_POLICY
            if (app_teams_bt_policy_handle_auth_link_key_info(param))
            {
                break;
            }
#endif
            app_bond_key_set(param->bd_addr, param->link_key, param->key_type);

            app_bt_policy_update_pair_idx_mapping();
            if (app_bond_get_pair_idx_mapping(param->bd_addr, &mapping_idx))
            {
                T_APP_BR_LINK *p_link = app_link_find_br_link(param->bd_addr);
                if (p_link)
                {
                    /*
                        Avoid LE CTKD link key update event
                    */
                    if (!(p_link->connected_profile & A2DP_PROFILE_MASK))
                    {
                        app_cfg_nv.audio_gain_level[mapping_idx] = app_dsp_cfg_vol.playback_volume_default;
                        app_cfg_nv.voice_gain_level[mapping_idx] = app_dsp_cfg_vol.voice_out_volume_default;
                        //reset_volume = 1;
                    }
                }
            }
#if F_APP_ERWS_SUPPORT
            {
                T_APP_REMOTE_MSG_PAYLOAD_LINK_KEY_ADDED link_key;
                link_key.key_type = param->key_type;
                memcpy(link_key.bd_addr, param->bd_addr, 6);
                memcpy(link_key.link_key, param->link_key, 16);
                link_key.reset_volume = reset_volume;
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_LINK_RECORD_ADD,
                                                    (uint8_t *)&link_key, sizeof(T_APP_REMOTE_MSG_PAYLOAD_LINK_KEY_ADDED));
            }
#endif
        }
        break;

#if F_APP_ERWS_SUPPORT
    case EVENT_B2B_AUTH_LINK_KEY_INFO:
        {
            uint8_t local_addr[6];

            bt_bond_key_set(param->bd_addr, param->link_key, param->key_type);

            if (remote_local_addr_get(local_addr))
            {
                bt_bond_key_set(local_addr, param->link_key, param->key_type);
            }
        }
        break;
#endif



    case EVENT_SRC_AUTH_LINK_KEY_REQ:
    case EVENT_B2B_AUTH_LINK_KEY_REQ:
        {
            uint8_t link_key[16];
            T_BT_LINK_KEY_TYPE type;

            if (bt_bond_key_get(param->bd_addr, link_key, (uint8_t *)&type))
            {
                bt_link_key_cfm(param->bd_addr, true, type, link_key);
            }
            else
            {
                bt_link_key_cfm(param->bd_addr, false, type, link_key);
            }
        }
        break;

    case EVENT_SRC_AUTH_LINK_PIN_CODE_REQ:
        {
            uint8_t pin_code[4] = {1, 2, 3, 4};

            bt_link_pin_code_cfm(param->bd_addr, pin_code, 4, true);
        }
        break;

    case EVENT_SRC_AUTH_FAIL:
        {
            if ((param->cause == (HCI_ERR | HCI_ERR_AUTHEN_FAIL)) ||
                (param->cause == (HCI_ERR | HCI_ERR_KEY_MISSING)))
            {
                T_APP_LINK_RECORD link_record;

                if (app_bond_pop_sec_diff_link_record(param->bd_addr, &link_record))
                {
                    app_bond_key_set(param->bd_addr, link_record.link_key, link_record.key_type);
                    bt_bond_flag_set(param->bd_addr, link_record.bond_flag);
                }
                else
                {
#if F_APP_TEAMS_BT_POLICY
                    app_teams_bt_policy_del_cod(param->bd_addr);
                    app_teams_bt_policy_delete_cod_temp_info_by_addr(param->bd_addr);
#endif
                    bt_bond_delete(param->bd_addr);
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_LINK_RECORD_DEL,
                                                        param->bd_addr, 6);
                }
            }

            app_bt_policy_event_ind(BP_EVENT_SRC_AUTH_FAIL, &event_param);
        }
        break;

    case EVENT_SRC_AUTH_SUC:
        {
            if ((app_cfg_const.enter_pairing_while_only_one_device_connected) &&
                (app_bt_point_br_link_num_get() == 1))
            {
                discoverable_when_one_link = true;
            }
            else
            {
                discoverable_when_one_link = false;
            }
#if F_APP_TEAMS_FEATURE_SUPPORT
            conn_to_new_device = 1;
            uint8_t bond_num = bt_bond_num_get();
            uint8_t bd_addr[6];
            for (uint8_t i = 1; i <= bond_num; i++)
            {
                bt_bond_addr_get(i, bd_addr);
                if (!memcmp(bd_addr, param->bd_addr, 6))
                {
                    conn_to_new_device = 0;
                }
            }
#endif
#if F_APP_TEAMS_BT_POLICY
            app_teams_bt_policy_handle_link_auth_cmpl(param->bd_addr);
#endif
            app_stop_timer(&timer_idx_bud_linklost);
            app_bt_policy_connected_node_auth_suc(param->bd_addr);
            app_bt_policy_b2s_connected_mark_index(param->bd_addr);
            app_bt_policy_event_ind(BP_EVENT_SRC_AUTH_SUC, &event_param);
        }
        break;

    case EVENT_SRC_DISCONN_LOST:
        {
            bool is_authed;
            bool is_first_src;
            bool is_b2s_connected_vp_played;
            uint32_t bond_flag = 0;
            uint32_t plan_profs = 0;

            is_authed = app_bt_policy_connected_node_is_authed(param->bd_addr);
            is_first_src = app_bt_policy_b2s_connected_is_first_src(param->bd_addr);
            is_b2s_connected_vp_played = app_bt_policy_b2s_connected_vp_played(param->bd_addr);

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            T_APP_BR_LINK *p_teams_link = app_link_find_br_link(param->bd_addr);
            if (p_teams_link)
            {
                app_teams_multilink_handle_link_disconnected(p_teams_link->id);
            }
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            T_APP_BR_LINK *p_speaker_link = app_link_find_br_link(param->bd_addr);
            if (p_speaker_link)
            {
                app_multilink_customer_handle_link_disconnected(p_speaker_link->id);
            }
#endif

            app_sniff_mode_b2s_check_left_flag_when_disconnect(param->bd_addr);

            if (app_bt_policy_b2s_connected_del_node(param->bd_addr))
            {
                if (memcmp(param->bd_addr, app_db.resume_addr, 6))
                {
                    app_multi_stop_acl_disconn_timer();
                }

#if F_APP_GAMING_DONGLE_SUPPORT
                if (app_cfg_const.enable_dongle_dual_mode)
                {
                    /* linkback all dongle record */
                    linkback_load_bond_list(0, app_cfg_const.timer_link_back_loss);
                }
                else
#endif
                {
                    bt_bond_flag_get(param->bd_addr, &bond_flag);

                    if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                    {
                        plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);

                        linkback_todo_queue_insert_normal_node(param->bd_addr, plan_profs,
                                                               app_cfg_const.timer_link_back_loss * 1000, false);
                    }

                    ENGAGE_PRINT_TRACE2("bt_event_pre_handle: bond_flag 0x%x, plan_profs 0x%x", bond_flag, plan_profs);
                }

                if (is_authed)
                {
                    event_param.is_first_src = is_first_src;
                    event_param.is_b2s_connected_vp_played = is_b2s_connected_vp_played;
                    app_bt_policy_event_ind(BP_EVENT_SRC_DISCONN_LOST, &event_param);
                }
                app_bt_policy_sync_b2s_connected();

#if F_APP_LISTENING_MODE_SUPPORT
                if (app_cfg_const.disallow_listening_mode_before_phone_connected)
                {
                    app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_DISCONNECTED);
                }
#endif

#if F_APP_TEAMS_BT_POLICY
                app_teams_bt_policy_delete_cod_temp_info_by_addr(param->bd_addr);
#endif

#if F_APP_ERWS_SUPPORT
                app_bt_policy_roleswitch_handle(param->bd_addr, ROLESWITCH_EVENT_LINK_DISCONNECTED);
#endif
            }
        }
        break;

    case EVENT_SRC_DISCONN_NORMAL:
        {
            bool is_first_src;
            bool is_b2s_connected_vp_played;

            is_src_authed = app_bt_policy_connected_node_is_authed(param->bd_addr);
            is_first_src = app_bt_policy_b2s_connected_is_first_src(param->bd_addr);
            is_b2s_connected_vp_played = app_bt_policy_b2s_connected_vp_played(param->bd_addr);

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
            T_APP_BR_LINK *p_teams_link = app_link_find_br_link(param->bd_addr);
            if (p_teams_link)
            {
                app_teams_multilink_handle_link_disconnected(p_teams_link->id);
            }
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
            T_APP_BR_LINK *p_speaker_link = app_link_find_br_link(param->bd_addr);
            if (p_speaker_link)
            {
                app_multilink_customer_handle_link_disconnected(p_speaker_link->id);
            }
#endif

            app_sniff_mode_b2s_check_left_flag_when_disconnect(param->bd_addr);

            if (app_bt_policy_b2s_connected_del_node(param->bd_addr))
            {
                if (is_src_authed)
                {
                    event_param.is_first_src = is_first_src;
                    event_param.is_b2s_connected_vp_played = is_b2s_connected_vp_played;
                    app_bt_policy_event_ind(BP_EVENT_SRC_DISCONN_NORMAL, &event_param);
                }
                app_bt_policy_sync_b2s_connected();

                if (memcmp(param->bd_addr, app_db.resume_addr, 6))
                {
                    app_multi_stop_acl_disconn_timer();
                }

#if F_APP_LISTENING_MODE_SUPPORT
                if (app_cfg_const.disallow_listening_mode_before_phone_connected)
                {
                    app_listening_judge_conn_disc_evnet(APPLY_LISTENING_MODE_SRC_DISCONNECTED);
                }
#endif
#if F_APP_ERWS_SUPPORT
#if F_APP_TEAMS_BT_POLICY
                app_teams_bt_policy_delete_cod_temp_info_by_addr(param->bd_addr);
#endif

                app_bt_policy_roleswitch_handle(param->bd_addr, ROLESWITCH_EVENT_LINK_DISCONNECTED);
#endif
            }

        }
        break;

#if F_APP_ERWS_SUPPORT
    case EVENT_SRC_DISCONN_ROLESWAP:
        {
            app_bt_policy_b2s_connected_del_node(param->bd_addr);

            linkback_todo_queue_delete_all_node();

            linkback_active_node.is_valid = false;

            app_bt_policy_event_ind(BP_EVENT_SRC_DISCONN_ROLESWAP, &event_param);
        }
        break;
#endif

    case EVENT_PROFILE_CONN_SUC:
        {
            event_param.is_first_prof = app_bt_policy_b2s_connected_no_profs(param->bd_addr);
            event_param.is_first_src = app_bt_policy_b2s_connected_is_first_src(param->bd_addr);

            app_bt_policy_b2s_connected_add_prof(param->bd_addr, param->prof);

            p_link = app_link_find_br_link(param->bd_addr);

            if (p_link != NULL)
            {
                app_bt_policy_src_conn_ind_add(param->bd_addr, true);

                if (A2DP_PROFILE_MASK == param->prof)
                {
                    if ((0 == (AVRCP_PROFILE_MASK & p_link->connected_profile)) && (!p_link->disconn_acl_flg))
                    {
                        app_stop_timer(&timer_idx_later_avrcp[p_link->id]);
                        app_start_timer(&timer_idx_later_avrcp[p_link->id], "later_avrcp",
                                        bt_policy_timer_id, APP_TIMER_LATER_AVRCP, p_link->id, false,
                                        app_cfg_const.timer_link_avrcp);
                    }
                }
                else if (AVRCP_PROFILE_MASK == param->prof)
                {
                    app_stop_timer(&timer_idx_later_avrcp[p_link->id]);
                }
#if F_APP_BT_PROFILE_PBAP_SUPPORT
                else if (HFP_PROFILE_MASK == param->prof || HSP_PROFILE_MASK == param->prof)
                {
                    if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
                    {
                        if (((PBAP_PROFILE_MASK & p_link->connected_profile) == 0) && (!p_link->disconn_acl_flg))
                        {
                            app_bt_policy_default_connect(param->bd_addr, PBAP_PROFILE_MASK, false);
                        }
                    }
                }
#endif
#if F_APP_HID_SUPPORT
                else if (HID_PROFILE_MASK == param->prof)
                {
                    app_stop_timer(&timer_idx_later_hid[p_link->id]);
                }
#endif

                if (event_param.is_first_prof)
                {
                    uint32_t bond_flag = 0;

                    app_sniff_mode_b2s_enable(param->bd_addr, SNIFF_DISABLE_MASK_ACL);

                    bt_bond_flag_get(param->bd_addr, &bond_flag);
#if F_APP_HID_SUPPORT
                    if (bond_flag & BOND_FLAG_HID)
                    {
                        if ((0 == (HID_PROFILE_MASK & p_link->connected_profile)) && (!p_link->disconn_acl_flg))
                        {
                            app_stop_timer(&timer_idx_later_hid[p_link->id]);
                            app_start_timer(&timer_idx_later_hid[p_link->id], "later_hid",
                                            bt_policy_timer_id, APP_TIMER_LATER_HID, p_link->id, false,
                                            2000);
                        }
                    }
#endif

#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
                    if (!memcmp(app_db.connected_dongle_addr, param->bd_addr, 6))
                    {
                        gap_br_start_did_discov(param->bd_addr);
                    }
#else
                    app_bt_policy_default_connect(param->bd_addr, DID_PROFILE_MASK, false);
#endif
                }
            }

            app_bt_policy_event_ind(BP_EVENT_PROFILE_CONN_SUC, &event_param);
        }
        break;

    case EVENT_PROFILE_DISCONN:
        {
            if (param->cause != (L2C_ERR | L2C_ERR_NO_RESOURCE))
            {
                if (A2DP_PROFILE_MASK == param->prof)
                {
                    bt_avrcp_disconnect_req(param->bd_addr);
                }

                app_bt_policy_b2s_connected_del_prof(param->bd_addr, param->prof);

                event_param.is_last_prof = false;
                event_param.prof = param->prof;

                p_link = app_link_find_br_link(param->bd_addr);
                if (p_link != NULL)
                {
                    if ((p_link->connected_profile & ~HID_PROFILE_MASK) == 0)
                    {
                        event_param.is_last_prof = true;

                        if (p_link->disconn_acl_flg
#if F_APP_ANC_SUPPORT
                            || app_anc_get_measure_mode()
#endif
                           )
                        {
                            p_link->disconn_acl_flg = false;
                            gap_br_send_acl_disconn_req(param->bd_addr);
                        }
                    }
                    else
                    {
                        if (p_link->disconn_acl_flg)
                        {
                            if ((p_link->connected_profile & p_link->plan_disconnect_profs) == 0)
                            {
                                p_link->disconn_acl_flg = false;
                                gap_br_send_acl_disconn_req(param->bd_addr);
                            }
                        }
                    }
                }

                app_bt_policy_event_ind(BP_EVENT_PROFILE_DISCONN, &event_param);
            }
        }
        break;

    case EVENT_PAIRING_MODE_TIMEOUT:
        {
            is_pairing_timeout = true;

            app_bt_policy_event_ind(BP_EVENT_PAIRING_MODE_TIMEOUT, NULL);
        }
        break;

#if F_APP_ERWS_SUPPORT
    case EVENT_PAGESCAN_PARAM_TIMEOUT:
        {
#if GFPS_SASS_SUPPORT
            if (extend_app_cfg_const.gfps_sass_support)
            {
                app_bt_policy_legacy_set_page_scan_param(SASS_PAGESCAN_INTERVAL, SASS_PAGESCAN_WINDOW);
            }
            else
#endif
            {
                app_bt_policy_legacy_set_page_scan_param(NORMAL_PAGESCAN_INTERVAL, NORMAL_PAGESCAN_WINDOW);
            }
        }
        break;
#endif

    case EVENT_SRC_CONN_IND_TIMEOUT:
        {
            app_bt_policy_src_conn_ind_del(param->cause);
        }
        break;

    default:
        {
            is_done = false;
        }
        break;
    }

    return is_done;
}

static bool app_bt_policy_sec_src_handle(T_EVENT event, T_BT_PARAM *param)
{
    bool is_done = true;

    switch (event)
    {
    case EVENT_SRC_CONN_SUC:
        {
            uint8_t id;

            app_bt_policy_b2s_connected_add_node(param->bd_addr, &id);
        }
        break;

    case EVENT_SRC_DISCONN_LOST:
    case EVENT_SRC_DISCONN_NORMAL:
    case EVENT_SRC_DISCONN_ROLESWAP:
        {
            app_bt_policy_b2s_connected_del_node(param->bd_addr);
        }
        break;

    case EVENT_PROFILE_CONN_SUC:
        {
            app_bt_policy_b2s_connected_add_prof(param->bd_addr, param->prof);
        }
        break;

    case EVENT_PROFILE_DISCONN:
        {
            app_bt_policy_b2s_connected_del_prof(param->bd_addr, param->prof);
        }
        break;

    default:
        {
            is_done = false;
        }
        break;
    }

    return is_done;
}

static bool app_bt_policy_event_handle(T_EVENT event, T_BT_PARAM *param)
{
    bool ret = true;

    switch (event)
    {
    case EVENT_STARTUP:
        {
            app_bt_policy_startup_event_handle(param);
        }
        break;

    case EVENT_ENTER_DUT_TEST_MODE:
        {
            app_bt_policy_enter_dut_test_mode_event_handle();
        }
        break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    case EVENT_START_OTA_SHAKING:
        {
            app_bt_policy_start_ota_shaking_event_handle();
        }
        break;

    case EVENT_ENTER_OTA_MODE:
        {
            app_bt_policy_enter_ota_mode_event_handle(param);
        }
        break;
#endif

    case EVENT_SHUTDOWN:
        {
            app_bt_policy_shutdown_event_handle();
        }
        break;

    case EVENT_STOP:
        {
            app_bt_policy_stop_event_handle();
        }
        break;

    case EVENT_RESTORE:
        {
            app_bt_policy_restore_event_handle();
        }
        break;

    case EVENT_CONN_SNIFF:
        {
            app_bt_policy_conn_sniff_event_handle(param);
        }
        break;

    case EVENT_CONN_ACTIVE:
        {
            app_bt_policy_conn_active_event_handle(param);
        }
        break;

#if F_APP_ERWS_SUPPORT
    case EVENT_PREPARE_FOR_ROLESWAP:
        {
            app_bt_policy_prepare_for_roleswap_event_handle();
        }
        break;

    case EVENT_ROLESWAP:
        {
            app_bt_policy_roleswap_event_handle(param);
        }
        break;
#endif

    case EVENT_ROLE_MASTER:
        {
            app_bt_policy_role_master_event_handle(param);
        }
        break;

    case EVENT_ROLE_SLAVE:
        {
            app_bt_policy_role_slave_event_handle(param);
        }
        break;

    case EVENT_ROLESWITCH_FAIL:
        {
            app_bt_policy_roleswitch_fail_event_handle(param);
        }
        break;

    case EVENT_ROLESWITCH_TIMEOUT:
        {
            app_bt_policy_roleswitch_timeout_event_handle(param);
        }
        break;

    case EVENT_DEDICATED_ENTER_PAIRING_MODE:
        {
            app_bt_policy_dedicated_enter_pairing_mode_event_handle(param);
        }
        break;

    case EVENT_DEDICATED_EXIT_PAIRING_MODE:
        {
            app_bt_policy_dedicated_exit_pairing_mode_event_handle();
        }
        break;

    case EVENT_DEDICATED_CONNECT:
        {
            app_bt_policy_dedicated_connect_event_handle(param);
        }
        break;

    case EVENT_DEDICATED_DISCONNECT:
        {
            app_bt_policy_dedicated_disconnect_event_handle(param);
        }
        break;

    case EVENT_DISCONNECT_ALL:
        {
            app_bt_policy_disconnect_all_event_handle();
        }
        break;

    default:
        {
            ret = false;
        }
        break;
    }

    return ret;
}

#if F_APP_COMMON_DONGLE_SUPPORT
static bool app_bt_policy_need_to_reject_acl(uint8_t *addr)
{
    uint32_t bond_flag;
    uint8_t reject_reason = 0;

#if F_APP_ALLOW_ONE_DONGLE_AND_ONE_PHONE_ONLY
    /* at least one phone is already connected */
    if (app_dongle_get_connected_dongle_link() == NULL &&
        app_link_get_b2s_link_num() > 0)
    {
        if (app_link_check_phone_link(addr))
        {
            reject_reason = 1;
            goto exit;
        }
    }
#endif

#if F_APP_LINKBACK_LEGACY_DONGLE_BY_BLE_ADV
    if (app_cfg_const.enable_dongle_dual_mode)
    {
        if (app_cfg_const.enable_dongle_dual_mode &&
            app_link_check_dongle_link(addr))
        {
            if (app_cfg_const.enable_multi_link == false)
            {
                if (app_dongle_is_24g_mode() == false)
                {
                    reject_reason = 6;
                    goto exit;
                }
            }
            else
            {
                if (app_cfg_const.enable_dongle_multi_pairing &&
                    app_cfg_nv.is_bt_pairing &&
                    app_dongle_get_mode() == PAIRING_MODE_BT_OR_BT_24G)
                {
                    reject_reason = 7;
                    goto exit;
                }
            }
        }
    }
#endif

    if (bt_bond_flag_get(addr, &bond_flag) == false)
    {
        return false;
    }

    /* below procedure base on bond flag check */

#if F_APP_GAMING_DONGLE_SUPPORT
    /* consider single link first */
    if (app_cfg_const.enable_dongle_dual_mode && app_link_check_b2s_link(addr))
    {
        if (app_dongle_is_24g_mode())
        {
            if ((bond_flag & BOND_FLAG_DONGLE) == 0)
            {
                reject_reason = 2;
            }
        }
        else
        {
            if (app_cfg_const.enable_multi_link)
            {
                /* allow dongle connected for multilink */
            }
            else
            {
                if (bond_flag & BOND_FLAG_DONGLE)
                {
                    reject_reason = 3;
                }
            }
        }
    }
#endif

#if F_APP_ALLOW_ONE_DONGLE_AND_ONE_PHONE_ONLY
    if (app_dongle_get_connected_dongle_link() != NULL)
    {
        if (bond_flag & BOND_FLAG_DONGLE)
        {
            reject_reason = 4;
        }
    }
    else
    {
        if (app_link_get_b2s_link_num() > 0)
        {
            if ((bond_flag & BOND_FLAG_DONGLE) == 0)
            {
                reject_reason = 5;
            }
        }
    }
#endif

exit:
    if (reject_reason != 0)
    {
        APP_PRINT_TRACE3("app_bt_policy_need_to_reject_acl: addr %s, bond_flag 0x%x reject_reason %d",
                         TRACE_BDADDR(addr), bond_flag, reject_reason);

        return true;
    }
    else
    {
        return false;
    }
}
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
/* check keep pairing state when src conn/disc event happen */
bool app_bt_policy_check_keep_pairing_state(uint8_t *bd_addr)
{
    bool ret = false;
    uint32_t bond_flag = 0;
    uint8_t cause = 0;
    bool is_dongle_addr = false;
    bool is_src_addr = false;

#if F_APP_GAMING_DONGLE_SUPPORT && !F_APP_LEA_SUPPORT
    if (app_cfg_const.enable_multi_link == false)
    {
        cause = 5;
        goto exit;
    }
#endif

    if (app_link_check_b2s_link(bd_addr) || app_link_check_dongle_link(bd_addr))
    {
        is_src_addr = true;
    }

    if (is_src_addr && app_dongle_get_mode() == PAIRING_MODE_BT_OR_BT_24G)
    {
        if (app_cfg_const.enable_dongle_multi_pairing)
        {
#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
#else
            if (bt_bond_flag_get(bd_addr, &bond_flag) == false)
            {
                /* no bond flag exists, can not determine which type the src is */
                ret = false;
                cause = 1;
                goto exit;
            }
#endif
            if (app_link_get_connected_src_num() < 2)
            {
                is_dongle_addr = app_link_check_dongle_link(bd_addr);

                /* exit pairing when same type connected */
                if (app_cfg_nv.is_bt_pairing)
                {
                    if (is_dongle_addr)
                    {
                        ret = true;
                        cause = 2;
                        goto exit;
                    }
                }
                else
                {
                    if (is_dongle_addr == false)
                    {
                        ret = true;
                        cause = 3;
                        goto exit;
                    }
                }
            }
        }
        else
        {
            bool keep_pairing = false;

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
            if (app_link_get_connected_phone_num() == 0)
            {
                keep_pairing = true;
            }
#else
            if (app_cfg_const.enable_multi_link && app_link_get_connected_phone_num() == 0)
            {
                keep_pairing = true;
            }
#endif
            /* exit pairing when two src connected */
            if (keep_pairing)
            {
                ret = true;
                cause = 4;
                goto exit;
            }
        }
    }

exit:
    APP_PRINT_TRACE2("app_bt_policy_check_keep_pairing_state: ret %d cause %d", ret, cause);

    return ret;
}
#endif

bool app_bt_policy_is_startup_state(void)
{
    return cur_state == STATE_STARTUP ? true : false;
}

void app_bt_policy_state_machine(T_EVENT event, T_BT_PARAM *param)
{
    T_BT_PARAM null_param;

    cur_event = event;

    app_bt_policy_event_info(event);

    if (param == NULL)
    {
        memset(&null_param, 0, sizeof(T_BT_PARAM));
        param = &null_param;
    }
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (EVENT_SRC_CONN_SUC <= event && event <= EVENT_PROFILE_DISCONN)
        {
            if (app_bt_policy_sec_src_handle(event, param))
            {
                app_bt_policy_connected();
            }
            return;
        }
    }
#endif
    if (!app_bt_policy_event_handle(event, param))
    {
        if (app_bt_policy_bt_event_pre_handle(event, param))
        {
            app_bt_policy_connected();
        }

#if F_APP_GAMING_DONGLE_SUPPORT
        if (event == EVENT_ENTER_GAMING_DONGLE_STREAMING)
        {
            if (app_dongle_get_mode() == PAIRING_MODE_BT_OR_BT_24G)
            {
                app_bt_policy_exit_pairing_mode();
            }

            if (timer_idx_discoverable != 0 && app_dongle_is_streaming())
            {
                app_stop_timer(&timer_idx_discoverable);
            }
        }

        T_BT_PARAM *bt_param = (T_BT_PARAM *)param;

        if ((event == EVENT_SRC_AUTH_SUC || event == EVENT_SRC_DISCONN_NORMAL) &&
            cur_state == STATE_AFE_PAIRING_MODE &&
            app_bt_policy_check_keep_pairing_state(bt_param->bd_addr))
        {
            return;
        }
#endif

        if (app_bt_policy_bt_event_handle(event, param))
        {
            return;
        }

        switch (cur_state)
        {
        case STATE_SHUTDOWN_STEP:
            {
                app_bt_policy_state_shutdown_step_event_handle(event);
            }
            break;

#if F_APP_ERWS_SUPPORT
        case STATE_FE_SHAKING:
            {
                app_bt_policy_state_fe_shaking_event_handle(event);
            }
            break;

        case STATE_FE_COUPLING:
            {
                app_bt_policy_state_fe_coupling_event_handle(event);
            }
            break;

        case STATE_AFE_TIMEOUT_SHAKING:
            {
                app_bt_policy_state_afe_timeout_shaking_event_handle(event, param);
            }
            break;

        case STATE_AFE_COUPLING:
            {
                app_bt_policy_state_afe_coupling_event_handle(event);
            }
            break;

        case STATE_AFE_WAIT_COUPLING:
            {
                app_bt_policy_state_afe_wait_coupling_event_handle(event);
            }
            break;
#endif

        case STATE_AFE_LINKBACK:
            {
                app_bt_policy_state_afe_linkback_event_handle(event, param);
            }
            break;

        case STATE_AFE_CONNECTED:
        case STATE_AFE_PAIRING_MODE:
        case STATE_AFE_STANDBY:
#if F_APP_ERWS_SUPPORT
        case STATE_AFE_SECONDARY:
#endif
            {
                app_bt_policy_state_afe_stable_event_handle(event);
            }
            break;

        case STATE_DUT_TEST_MODE:
            {
                app_bt_policy_state_dut_test_mode_event_handle(event);
            }
            break;

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
        case STATE_OTA_SHAKING:
            {
                app_bt_policy_state_ota_shaking_event_handle(event, param);
            }
            break;

        case STATE_OTA_MODE:
            {
                app_bt_policy_state_ota_mode_event_handle(event);
            }
            break;
#endif

        default:
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void app_bt_policy_timer_cback(uint8_t timer_evt, uint16_t param)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    switch (timer_evt)
    {
    case APP_TIMER_SHUTDOWN_STEP:
        {
            app_stop_timer(&timer_idx_shutdown_step);
            app_bt_policy_state_machine(EVENT_SHUTDOWN_STEP_TIMEOUT, NULL);
        }
        break;


    case APP_TIMER_PAIRING_MODE:
        {
#if F_APP_LEA_SUPPORT
            if (app_cfg_const.legacy_enable == false)
            {
                bt_device_mode_set(BT_DEVICE_MODE_IDLE);
            }
#endif
            app_stop_timer(&timer_idx_pairing_mode);
            app_bt_policy_state_machine(EVENT_PAIRING_MODE_TIMEOUT, NULL);
        }
        break;

    case APP_TIMER_DISCOVERABLE:
        {
            app_stop_timer(&timer_idx_discoverable);
            app_bt_policy_state_machine(EVENT_DISCOVERABLE_TIMEOUT, NULL);
        }
        break;

    case APP_TIMER_ROLE_SWITCH:
        {
            T_APP_BR_LINK *p_link;

            p_link = &app_db.br_link[param];
            app_stop_timer(&timer_idx_role_switch[p_link->id]);
            bt_param.p_link = p_link;
            app_bt_policy_state_machine(EVENT_ROLESWITCH_TIMEOUT, &bt_param);
        }
        break;

    case APP_TIMER_BUD_LINKLOST:
        {
            app_stop_timer(&timer_idx_bud_linklost);
            app_bt_policy_state_machine(EVENT_BUD_LINKLOST_TIMEOUT, NULL);
        }
        break;

    case APP_TIMER_LINKBACK:
        {
            bt_param.not_check_addr_flag = true;

            app_stop_timer(&timer_idx_linkback);
            app_bt_policy_state_machine(EVENT_SRC_CONN_TIMEOUT, &bt_param);
        }
        break;

    case APP_TIMER_LINKBACK_DELAY:
        {
            bt_param.not_check_addr_flag = true;
            bt_param.cause = param;

            app_stop_timer(&timer_idx_linkback_delay);
            app_bt_policy_state_machine(EVENT_LINKBACK_DELAY_TIMEOUT, &bt_param);
        }
        break;

    case APP_TIMER_LATER_AVRCP:
        {
            T_APP_BR_LINK *p_link;

            p_link = &app_db.br_link[param];

            bt_param.bd_addr = p_link->bd_addr;
            bt_param.prof = AVRCP_PROFILE_MASK;
            bt_param.is_special = false;
            bt_param.check_bond_flag = false;
            bt_param.is_later_avrcp = true;

            app_stop_timer(&timer_idx_later_avrcp[p_link->id]);
            app_bt_policy_state_machine(EVENT_DEDICATED_CONNECT, &bt_param);
        }
        break;

    case APP_TIMER_WAIT_COUPLING:
        {
            app_stop_timer(&timer_idx_wait_coupling);
            app_bt_policy_state_machine(EVENT_BUD_WAIT_COUPLING_TO, NULL);
        }
        break;

#if 0
    case APP_TIMER_LATER_HID:
        {
            T_APP_BR_LINK *p_link;

            p_link = &app_db.br_link[param];

            bt_param.bd_addr = p_link->bd_addr;
            bt_param.prof = HID_PROFILE_MASK;
            bt_param.is_special = false;
            bt_param.check_bond_flag = true;
            bt_param.is_later_hid = true;

            app_stop_timer(&timer_idx_later_hid[p_link->id]);
            app_bt_policy_state_machine(EVENT_DEDICATED_CONNECT, &bt_param);
        }
        break;
#endif

#if F_APP_ERWS_SUPPORT
    case APP_TIMER_PAGE_SCAN_PARAM:
        {
            app_stop_timer(&timer_idx_page_scan_param);
            app_bt_policy_state_machine(EVENT_PAGESCAN_PARAM_TIMEOUT, NULL);
        }
        break;

    case APP_TIMER_ENGAGE_ACTION_ADJUST:
        {
            app_stop_timer(&timer_idx_engage_action_adjust);
            app_bt_policy_state_machine(EVENT_BUD_ENGAGE_ACTION_ADJUST, NULL);
        }
        break;
    case APP_TIMER_FIRST_ENGAGE_CHECK:
        {
            app_stop_timer(&timer_idx_first_engage_check);
            app_bt_policy_state_machine(EVENT_FE_TIMEOUT, NULL);
        }
        break;

#endif

    case APP_TIMER_RECONNECT:
        {
            app_stop_timer(&timer_idx_reconnect);

            app_db.disallow_connected_tone_after_inactive_connected = (app_db.connected_tone_need == true) ?
                                                                      false : true;
            app_audio_set_connected_tone_need(false);
            app_multi_reconnect_inactive_link();
        }
        break;

    case APP_TIMER_SRC_CONN_IND:
        {
            if (src_conn_ind[param].timer_idx != 0)
            {
                bt_param.cause = param;

                app_stop_timer(&src_conn_ind[param].timer_idx);

                app_hfp_adjust_sco_window(src_conn_ind[param].bd_addr, APP_SCO_ADJUST_ACL_CONN_END_EVT);

                app_bt_policy_state_machine(EVENT_SRC_CONN_IND_TIMEOUT, &bt_param);
            }
        }
        break;

    case APP_TIMER_B2S_CONN:
        {
            app_stop_timer(&timer_idx_b2s_conn);
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
#if F_APP_ERWS_SUPPORT
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    if (app_cfg_const.disable_low_bat_role_swap == 0)
                    {
                        app_roleswap_req_battery_level();
                    }
                }
#endif

            }
        }
        break;

#if F_APP_ERWS_SUPPORT
    case APP_TIMER_ROLESWAP:
        {
            app_stop_timer(&timer_idx_roleswap);
            app_bt_policy_roleswap_delay_handle();
        }
        break;
#endif

    case APP_TIMER_SET_CPU_CLK:
        {
            app_stop_timer(&timer_idx_increase_cpu_clk);
            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_SET_CPU_CLK_TIMEOUT);
        }
        break;

    default:
        break;
    }
}

void app_bt_policy_update_pair_idx_mapping(void)
{
    uint8_t bond_num = app_bond_b2s_num_get();
    uint8_t pair_idx[8] = {0};
    uint8_t bd_addr[6] = {0};
    uint8_t pair_idx_temp = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    bool mapping_flag = false;

    APP_PRINT_TRACE1("app_bt_update_pair_idx_mapping bond_num = %d", bond_num);

    // APP_PRINT_TRACE8("start app_bt_update_pair_idx_mapping: app_pair_idx_mapping %d,  %d, %d, %d, %d, %d, %d, %d",
    //                  app_cfg_nv.app_pair_idx_mapping[0], app_cfg_nv.app_pair_idx_mapping[1],
    //                  app_cfg_nv.app_pair_idx_mapping[2], app_cfg_nv.app_pair_idx_mapping[3],
    //                  app_cfg_nv.app_pair_idx_mapping[4], app_cfg_nv.app_pair_idx_mapping[5],
    //                  app_cfg_nv.app_pair_idx_mapping[6], app_cfg_nv.app_pair_idx_mapping[7]);

    if (bond_num > 8)
    {
        bond_num = 8;
    }

    for (i = 1; i <= bond_num; i++)
    {
        if (app_bond_b2s_addr_get(i, bd_addr))
        {
            if (bt_bond_index_get(bd_addr, &pair_idx_temp))
            {
                pair_idx[i - 1] = pair_idx_temp;
            }
        }
    }

    for (j = 0; j < 8; j++)
    {
        for (i = 0; i < bond_num; i++)
        {
            if (app_cfg_nv.app_pair_idx_mapping[j] == pair_idx[i])
            {
                mapping_flag = true;
                break;
            }
        }

        if (!mapping_flag)
        {
            app_cfg_nv.audio_gain_level[j] = app_dsp_cfg_vol.playback_volume_default;
            app_cfg_nv.voice_gain_level[j] = app_dsp_cfg_vol.voice_out_volume_default;
            app_cfg_nv.app_pair_idx_mapping[j] = 0xFF;
#if GFPS_SASS_SUPPORT
            if (extend_app_cfg_const.gfps_sass_support)
            {
                app_cfg_nv.sass_bitmap &= ~(1 << j);
            }
#endif
        }

        mapping_flag = false;
    }

    for (i = 0; i < bond_num; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (app_cfg_nv.app_pair_idx_mapping[j] == pair_idx[i])
            {
                mapping_flag = true;
                break;
            }
        }

        if (!mapping_flag)
        {
            for (j = 0; j < 8; j++)
            {
                if (app_cfg_nv.app_pair_idx_mapping[j] == 0xFF)
                {
                    app_cfg_nv.app_pair_idx_mapping[j] = pair_idx[i];
                    app_cfg_nv.audio_gain_level[j] = app_dsp_cfg_vol.playback_volume_default;
                    app_cfg_nv.voice_gain_level[j] = app_dsp_cfg_vol.voice_out_volume_default;
#if GFPS_SASS_SUPPORT
                    if (extend_app_cfg_const.gfps_sass_support)
                    {
                        //new device, it's sass bit should always be 0
                        app_cfg_nv.sass_bitmap &= ~(1 << j);
                    }
#endif
                    break;
                }
            }
        }
        mapping_flag = false;
    }

    // APP_PRINT_TRACE8("end app_bt_update_pair_idx_mapping: app_pair_idx_mapping %d,  %d, %d, %d, %d, %d, %d, %d",
    //                  app_cfg_nv.app_pair_idx_mapping[0], app_cfg_nv.app_pair_idx_mapping[1],
    //                  app_cfg_nv.app_pair_idx_mapping[2], app_cfg_nv.app_pair_idx_mapping[3],
    //                  app_cfg_nv.app_pair_idx_mapping[4], app_cfg_nv.app_pair_idx_mapping[5],
    //                  app_cfg_nv.app_pair_idx_mapping[6], app_cfg_nv.app_pair_idx_mapping[7]);

    // APP_PRINT_TRACE8("end app_bt_update_pair_idx_mapping: pair_idx %d,  %d, %d, %d, %d, %d, %d, %d",
    //                  pair_idx[0], pair_idx[1],
    //                  pair_idx[2], pair_idx[3],
    //                  pair_idx[4], pair_idx[5],
    //                  pair_idx[6], pair_idx[7]);
}

//ysc start
bool bt_connected_flag = false;
//ysc end

static void app_bt_policy_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_BT_PARAM bt_param;
    T_APP_BR_LINK *p_link = NULL;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            bt_param.bd_addr = param->acl_conn_success.bd_addr;
            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_ACL_CONN_CMPL);

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_success.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_BUD_CONN_SUC, &bt_param);
#if (F_APP_GOLDEN_RANGE == 1)
                bt_link_rssi_golden_range_set(param->acl_conn_success.bd_addr, GOLDEN_RANGE_B2B_MAX,
                                              GOLDEN_RANGE_B2B_MIN);
#endif
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                //app_bt_policy_legacy_set_page_scan_param(NORMAL_PAGESCAN_INTERVAL*2, NORMAL_PAGESCAN_WINDOW);

                if (app_cfg_const.enable_align_default_volume_from_bud_to_phone)
                {
                    first_connect_sync_default_volume_to_src = true;
                }
                else if (app_cfg_const.enable_align_default_volume_after_factory_reset)
                {
                    uint8_t key_type;
                    uint8_t link_key[16];

                    first_connect_sync_default_volume_to_src = false;
                    if (bt_bond_key_get(param->acl_conn_success.bd_addr, link_key,
                                        (uint8_t *)&key_type) == false)
                    {
                        first_connect_sync_default_volume_to_src = true;
                    }
                }
                else
                {
                    first_connect_sync_default_volume_to_src = false;
                }
#if F_APP_SAIYAN_MODE
                if (data_capture_saiyan.saiyan_enable)
                {
                    app_bt_policy_state_machine(EVENT_FE_TIMEOUT, NULL);
                }
#endif
                app_bt_policy_state_machine(EVENT_SRC_CONN_SUC, &bt_param);
                app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_LEGACY_LINK_CHANGE);
#if (F_APP_GOLDEN_RANGE == 1)
                bt_link_rssi_golden_range_set(param->acl_conn_success.bd_addr, GOLDEN_RANGE_B2S_MAX,
                                              GOLDEN_RANGE_B2S_MIN);
#endif
#if F_APP_HARMAN_FEATURE_SUPPORT
                if (app_cfg_const.enable_multi_link)
                {
                    if (app_harman_is_already_connect_one_get() == false)
                    {
                        APP_PRINT_ERROR1("acl link connect success ,bdaddr %s, phone is first_connected",
                                         TRACE_BDADDR(param->acl_conn_success.bd_addr));
                        app_harman_is_already_connect_one_set(true);
                    }
                }
#endif
            }
        }

        //app_gfps_adv_update_adv_interval(400);
        //ENGAGE_PRINT_TRACE0("------------  app_bt_policy_cback app_gfps_adv_update_adv_interval (400) -------------");        
        
        break;

    case BT_EVENT_ACL_CONN_FAIL:
        {
            bt_param.bd_addr = param->acl_conn_fail.bd_addr;
#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_fail.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_BUD_CONN_FAIL, &bt_param);

            }
            else
#endif
            {
                bt_param.is_b2b = false;

                if (param->acl_conn_fail.cause == (HCI_ERR | HCI_ERR_ACL_CONN_EXIST))
                {
                    app_bt_policy_state_machine(EVENT_SRC_CONN_FAIL_ACL_EXIST, &bt_param);
                }
                else if (param->acl_conn_fail.cause == (HCI_ERR | HCI_ERR_CONTROLLER_BUSY))
                {
                    app_bt_policy_state_machine(EVENT_SRC_CONN_FAIL_CONTROLLER_BUSY, &bt_param);
                }
                else
                {
                    app_bt_policy_state_machine(EVENT_SRC_CONN_FAIL, &bt_param);
                }
            }
        }
        break;

    case BT_EVENT_ACL_AUTHEN_SUCCESS:
        {
            bt_param.bd_addr = param->acl_authen_success.bd_addr;

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_authen_success.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_BUD_AUTH_SUC, &bt_param);
                app_sniff_mode_b2b_disable(bt_param.bd_addr, SNIFF_DISABLE_MASK_ACL);
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                bt_param.not_check_addr_flag = true;
                app_bt_policy_state_machine(EVENT_SRC_AUTH_SUC, &bt_param);
                app_sniff_mode_b2s_disable(bt_param.bd_addr, SNIFF_DISABLE_MASK_ACL);
            }
        }
        break;

    case BT_EVENT_ACL_AUTHEN_FAIL:
        {
#if F_APP_ERWS_SUPPORT
            uint8_t   local_addr[6];
            uint8_t   peer_addr[6];

            remote_local_addr_get(local_addr);
            remote_peer_addr_get(peer_addr);

            if ((param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_AUTHEN_FAIL)) ||
                (param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_KEY_MISSING)))
            {
                if (memcmp(param->acl_authen_fail.bd_addr, peer_addr, 6) == 0)
                {
#if F_APP_HARMAN_FEATURE_SUPPORT
                    if (((app_link_check_b2b_link(param->acl_authen_fail.bd_addr)) == false) &&
                        (param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_KEY_MISSING)))
                    {
                    }
                    else
#endif
                    {
                        bt_bond_delete(local_addr);
                        bt_bond_delete(peer_addr);
                    }
                }
            }

            bt_param.bd_addr = param->acl_authen_fail.bd_addr;

            if (app_link_check_b2b_link(param->acl_authen_fail.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_BUD_AUTH_FAIL, &bt_param);
            }
            else
            {
                bt_param.is_b2b = false;
                bt_param.cause = param->acl_authen_fail.cause;
                app_bt_policy_state_machine(EVENT_SRC_AUTH_FAIL, &bt_param);
            }
#else
            if ((param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_AUTHEN_FAIL)) ||
                (param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_KEY_MISSING)))
            {
                bt_bond_delete(param->acl_authen_fail.bd_addr);
            }
            bt_param.is_b2b = false;
            bt_param.cause = param->acl_authen_fail.cause;
            app_bt_policy_state_machine(EVENT_SRC_AUTH_FAIL, &bt_param);
#endif
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            //ysc start
            bt_connected_flag = false;
            //ysc end
            bt_param.bd_addr = param->acl_conn_disconn.bd_addr;

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_disconn.bd_addr))
            {
                bt_param.is_b2b = true;
                app_db.src_roleswitch_fail_no_sniffing = false;
                p_link = app_link_find_br_link(param->acl_conn_disconn.bd_addr);
                if (p_link != NULL)
                {
                    p_link->acl_decrypted = false;
                }

#if F_APP_QOL_MONITOR_SUPPORT
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    //disable link monitor when b2b link disconn
                    app_qol_link_monitor(param->acl_conn_disconn.bd_addr, false);
                }
#endif
                if ((param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
                    (param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
                {
                    app_bt_policy_state_machine(EVENT_BUD_DISCONN_LOST, &bt_param);
                }
                else if (param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE))
                {
                    app_bt_policy_state_machine(EVENT_BUD_DISCONN_LOCAL, &bt_param);
                }
                else
                {
                    app_bt_policy_state_machine(EVENT_BUD_DISCONN_NORMAL, &bt_param);
                }
                if (memcmp(param->acl_conn_disconn.bd_addr, app_cfg_nv.bud_peer_addr, 6) == 0)
                {
                    app_db.sec_is_power_off = false;
                }
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO
                if (p_link != NULL)
                {
                    app_sensor_mems_stop(p_link->id);
                }
#endif
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                bt_param.cause = param->acl_conn_disconn.cause;

#if F_APP_HARMAN_FEATURE_SUPPORT
                app_harman_remote_device_name_crc_set(0xFF, param->acl_conn_disconn.bd_addr, false);
                app_harman_le_common_adv_update();
#endif

                app_db.a2dp_preemptive_flag = false;

                p_link = app_link_find_br_link(param->acl_conn_disconn.bd_addr);
                if (p_link != NULL)
                {
#if F_APP_BUD_LOCATION_SUPPORT
                    if ((param->acl_conn_disconn.cause != (HCI_ERR | HCI_ERR_CONN_ROLESWAP)) &&
                        (p_link->id == app_a2dp_get_active_idx()))
                    {
                        app_db.resume_a2dp_by_in_ear = false;
                        app_audio_update_in_ear_recover_a2dp(false);
                    }
#endif

                    if (!app_multi_check_in_sniffing())
                    {
                        app_multi_handle_sniffing_link_disconnect_event(p_link->id);
                    }
                }

                if ((param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
                    (param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
                {
                    app_bt_policy_state_machine(EVENT_SRC_DISCONN_LOST, &bt_param);
                }
                else if (param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
                {
                    app_bt_policy_state_machine(EVENT_SRC_DISCONN_ROLESWAP, &bt_param);
                }
                else
                {
                    app_bt_policy_state_machine(EVENT_SRC_DISCONN_NORMAL, &bt_param);
                }

#if F_APP_HARMAN_FEATURE_SUPPORT
                if (app_cfg_const.enable_multi_link)
                {
                    if (app_link_get_connected_src_num() == 0)
                    {
                        app_harman_is_already_connect_one_set(false);
                    }
                }
#endif
                app_bt_policy_qos_param_update(bt_param.bd_addr, BP_TPOLL_ACL_DISC_EVENT);
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_MASTER:
        {
            bt_param.bd_addr = param->acl_role_master.bd_addr;

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_role_master.bd_addr))
            {
                bt_param.is_b2b = true;
                // set b2b link Supervision timeout
                gap_br_cfg_acl_link_supv_tout(bt_param.bd_addr, 0xc80);
                app_bt_policy_state_machine(EVENT_ROLE_MASTER, &bt_param);
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                app_bt_policy_state_machine(EVENT_ROLE_MASTER, &bt_param);
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_SLAVE:
        {
            bt_param.bd_addr = param->acl_role_slave.bd_addr;

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_role_slave.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_ROLE_SLAVE, &bt_param);
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                app_bt_policy_state_machine(EVENT_ROLE_SLAVE, &bt_param);
            }
        }
        break;

    case BT_EVENT_ACL_CONN_ACTIVE:
        {
            bt_param.bd_addr = param->acl_conn_active.bd_addr;

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_active.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_CONN_ACTIVE, &bt_param);
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                app_bt_policy_state_machine(EVENT_CONN_ACTIVE, &bt_param);
            }
        }
        break;

    case BT_EVENT_ACL_CONN_SNIFF:
        {
            bt_param.bd_addr = param->acl_conn_sniff.bd_addr;

#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_sniff.bd_addr))
            {
                bt_param.is_b2b = true;
                app_bt_policy_state_machine(EVENT_CONN_SNIFF, &bt_param);
            }
            else
#endif
            {
                bt_param.is_b2b = false;
                app_bt_policy_state_machine(EVENT_CONN_SNIFF, &bt_param);
            }
        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_ACL_CONN_ENCRYPTED:
        {
            if (app_link_check_b2b_link(param->acl_conn_encrypted.bd_addr) == false)
            {
                app_bt_policy_qos_param_update(param->acl_conn_encrypted.bd_addr, BP_TPOLL_ACL_CONN_EVENT);
            }
        }
        break;
#endif

    case BT_EVENT_ACL_CONN_NOT_ENCRYPTED:
        {
            p_link = app_link_find_br_link(param->acl_conn_not_encrypted.bd_addr);
            if (p_link != NULL)
            {
                p_link->acl_decrypted = true;
            }

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_bt_sniffing_process(param->acl_conn_not_encrypted.bd_addr);
            }

#if F_APP_ERWS_SUPPORT
            if (app_cfg_nv.first_engaged)
            {
                app_bt_policy_roleswitch_handle(param->acl_conn_not_encrypted.bd_addr,
                                                ROLESWITCH_EVENT_LINK_CONNECTED);
            }
#endif
        }
        break;

    case BT_EVENT_ACL_CONN_READY:
        {
#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_ready.bd_addr) == true)
            {
                bt_acl_pkt_type_set(param->acl_conn_ready.bd_addr, BT_ACL_PKT_TYPE_2M);
                app_bt_policy_b2b_qos_param_update();
            }
            else
#endif
            {
                /* TODO not set pkt 2m in DUT Test Mode */
                bt_acl_pkt_type_set(param->acl_conn_ready.bd_addr, BT_ACL_PKT_TYPE_2M);
#if F_APP_ERWS_SUPPORT
                app_bt_policy_roleswitch_handle(param->acl_conn_ready.bd_addr, ROLESWITCH_EVENT_LINK_CONNECTED);
#endif
            }
        }
        break;

    case BT_EVENT_ACL_CONN_IND:
        {
            //ysc start
            bt_connected_flag = true;
            //ysc end
            APP_PRINT_TRACE1("app_bt_policy_cback: conn ind device cod 0x%08x", param->acl_conn_ind.cod);

            p_link = app_link_find_br_link(param->acl_conn_ind.bd_addr);

            if (p_link != NULL)
            {
                bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
            }
            else if (app_link_check_b2s_link(param->acl_conn_ind.bd_addr) &&
                     !app_bt_point_link_permission_check(BP_LINK_TYPE_B2S_BR, param->acl_conn_ind.bd_addr))
            {
#if GFPS_SASS_SUPPORT
                if (extend_app_cfg_const.gfps_sass_support)
                {
#if (F_APP_OTA_TOOLING_SUPPORT == 1)
                    if (app_bt_policy_validate_bd_addr_with_dongle_id(app_db.jig_dongle_id,
                                                                      param->acl_conn_ind.bd_addr) == true)
#endif
                    {
#if F_APP_TEAMS_BT_POLICY
                        app_teams_bt_policy_handle_acl_conn_ind(param->acl_conn_ind.bd_addr, param->acl_conn_ind.cod);
#endif
                        app_bt_policy_src_conn_ind_add(param->acl_conn_ind.bd_addr, false);
                        app_hfp_adjust_sco_window(param->acl_conn_ind.bd_addr, APP_SCO_ADJUST_ACL_CONN_IND_EVT);
                        bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                    }
                }
                else
#endif
                {
                    bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
                }
            }
#if F_APP_COMMON_DONGLE_SUPPORT
#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
#else
            else if (app_link_check_b2s_link(param->acl_conn_ind.bd_addr) &&
                     app_bt_policy_need_to_reject_acl(param->acl_conn_ind.bd_addr))
            {
                bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
            }
#endif
#endif
            else
            {
#if F_APP_DEVICE_CMD_SUPPORT
                if (app_cmd_get_auto_accept_conn_req_flag() == true)
#endif
                {
                    if (app_link_check_b2s_link(param->acl_conn_ind.bd_addr))
                    {
#if (F_APP_OTA_TOOLING_SUPPORT == 1)
                        if (app_bt_policy_validate_bd_addr_with_dongle_id(app_db.jig_dongle_id,
                                                                          param->acl_conn_ind.bd_addr) == true)
#endif
                        {
#if F_APP_TEAMS_BT_POLICY
                            app_teams_bt_policy_handle_acl_conn_ind(param->acl_conn_ind.bd_addr, param->acl_conn_ind.cod);
#endif
#if (F_APP_HARMAN_FEATURE_SUPPORT && HARMAN_ONLY_CONN_ONE_DEVICE_WHEN_PAIRING)
                            APP_PRINT_TRACE3("[SD_CHECK] ACL_CONN_REQ_IND bd_addr %s, state %d, %d",
                                             TRACE_BDADDR(param->acl_conn_ind.bd_addr),
                                             app_bt_policy_get_state(),
                                             app_harman_is_already_connect_one_get());
                            uint8_t addr_temp[6];
                            uint32_t bond_flag;
                            uint32_t plan_profs;
                            uint8_t bond_num = app_bond_b2s_num_get();
                            uint8_t ACL_CONN_REQ_EVER_CONN = false;

                            for (uint8_t i = 1; i <= bond_num; i++)
                            {
                                bond_flag = 0;
                                if (app_bond_b2s_addr_get(i, addr_temp))
                                {
                                    if (memcmp(addr_temp, app_cfg_nv.bud_peer_addr, 6) &&
                                        memcmp(addr_temp, app_cfg_nv.bud_local_addr, 6))
                                    {
                                        bt_bond_flag_get(addr_temp, &bond_flag);
                                        APP_PRINT_INFO3("[SD_CHECK] app_harman_ever_link_information, dump priority: %d, bond_flag: %d, addr: %s",
                                                        i, bond_flag, TRACE_BDADDR(addr_temp));
                                        if (bond_flag != 0)
                                        {
                                            if (memcmp(addr_temp, param->acl_conn_ind.bd_addr, 6) == 0)
                                            {
                                                ACL_CONN_REQ_EVER_CONN = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }

                            APP_PRINT_ERROR1("[SD_CHECK] ACL_CONN_REQ_IND allow connect %d", ACL_CONN_REQ_EVER_CONN);

                            if (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE)
                            {
                                if (app_harman_is_already_connect_one_get() == false)
                                {
                                    src_conn_ind_add(param->acl_conn_ind.bd_addr, false);

                                    app_hfp_adjust_sco_window(param->acl_conn_ind.bd_addr, APP_SCO_ADJUST_ACL_CONN_IND_EVT);

                                    bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                                }
                                else
                                {
                                    bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
                                }
                            }
                            else
                            {
                                if (ACL_CONN_REQ_EVER_CONN == true)
                                {
                                    src_conn_ind_add(param->acl_conn_ind.bd_addr, false);

                                    app_hfp_adjust_sco_window(param->acl_conn_ind.bd_addr, APP_SCO_ADJUST_ACL_CONN_IND_EVT);

                                    bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                                }
                                else
                                {
                                    bt_acl_conn_reject(param->acl_conn_ind.bd_addr, BT_ACL_REJECT_UNACCEPTABLE_ADDR);
                                }
                            }
#else
                            app_bt_policy_src_conn_ind_add(param->acl_conn_ind.bd_addr, false);

                            app_hfp_adjust_sco_window(param->acl_conn_ind.bd_addr, APP_SCO_ADJUST_ACL_CONN_IND_EVT);

#if F_APP_GAMING_DONGLE_SUPPORT
                            if (param->acl_conn_ind.cod == GAMING_DONGLE_COD)
                            {
                                memcpy(app_db.connected_dongle_addr, param->acl_conn_ind.bd_addr, 6);
                            }
#endif
                            bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
#endif
                        }
                    }
                    else
                    {
                        if (0 == app_link_get_b2s_link_num())
                        {
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
                                bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                            }
                            else
                            {
                                bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_MASTER);
                            }
                        }
                        else
                        {
                            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                            {
                                bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_MASTER);
                            }
                            else
                            {
                                bt_acl_conn_accept(param->acl_conn_ind.bd_addr, BT_LINK_ROLE_SLAVE);
                            }
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_SWITCH_FAIL:
        {
            bt_param.bd_addr = param->sdp_discov_cmpl.bd_addr;
            bt_param.cause = param->acl_role_switch_fail.cause;

            app_bt_policy_state_machine(EVENT_ROLESWITCH_FAIL, &bt_param);
        }
        break;

    case BT_EVENT_LINK_KEY_INFO:
        {
            bt_param.bd_addr = param->link_key_info.bd_addr;
            bt_param.key_type = param->link_key_info.key_type;
            bt_param.link_key = param->link_key_info.link_key;

#if F_APP_ERWS_SUPPORT
            uint8_t   peer_addr[6];
            remote_peer_addr_get(peer_addr);
            if (memcmp(peer_addr, bt_param.bd_addr, 6) == 0)
            {
                app_bt_policy_state_machine(EVENT_B2B_AUTH_LINK_KEY_INFO, &bt_param);
            }
            else
#endif
            {
                app_bt_policy_state_machine(EVENT_SRC_AUTH_LINK_KEY_INFO, &bt_param);
            }
        }
        break;

    case BT_EVENT_LINK_KEY_REQ:
        {
            bt_param.bd_addr = param->link_key_req.bd_addr;

#if F_APP_ERWS_SUPPORT
            uint8_t   peer_addr[6];

            remote_peer_addr_get(peer_addr);
            if (memcmp(peer_addr, bt_param.bd_addr, 6) == 0)
            {
                app_bt_policy_state_machine(EVENT_B2B_AUTH_LINK_KEY_REQ, &bt_param);
            }
            else
#endif
            {
                app_bt_policy_state_machine(EVENT_SRC_AUTH_LINK_KEY_REQ, &bt_param);
            }
        }
        break;

    case BT_EVENT_LINK_PIN_CODE_REQ:
        {
            bt_param.bd_addr = param->link_pin_code_req.bd_addr;
            bt_link_pin_code_cfm(param->link_pin_code_req.bd_addr, app_cfg_nv.pin_code,
                                 app_cfg_nv.pin_code_size, true);
            app_bt_policy_state_machine(EVENT_SRC_AUTH_LINK_PIN_CODE_REQ, &bt_param);
        }
        break;

    case BT_EVENT_SCO_CONN_IND:
        {
            app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_SCO);
        }
        break;

    case BT_EVENT_SCO_CONN_RSP:
        {
            if (param->sco_conn_rsp.cause == 0)
            {
                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_SCO);
            }
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            uint8_t pair_idx;
            T_APP_BR_LINK *p_link = NULL;
            p_link = app_link_find_br_link(param->sco_conn_cmpl.bd_addr);

            if (p_link != NULL && param->sco_conn_cmpl.cause == 0)
            {
                p_link->sco_handle = param->sco_conn_cmpl.handle;
                app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_SCO, BP_LINK_EVENT_CONNECT, p_link->bd_addr);

                if (bt_bond_index_get(p_link->bd_addr, &pair_idx))
                {
                    app_cfg_nv.last_btmode[pair_idx] = BP_LINK_LAST_BTMODE_LEGACY;
                    APP_PRINT_INFO1("set last mode to legacy: %s", TRACE_BDADDR(p_link->bd_addr));
                }

                if (app_bt_policy_get_radio_mode() == BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE)
                {
                    app_bt_policy_exit_pairing_mode();
                }

                app_bt_policy_state_machine(EVENT_SCO_CONN_CMPL, NULL);
                app_bt_policy_qos_param_update(param->sco_conn_cmpl.bd_addr, BP_TPOLL_SCO_CONN_EVENT);
                app_hfp_adjust_sco_window(param->sco_conn_cmpl.bd_addr, APP_SCO_ADJUST_SCO_CONN_CMPL_EVT);
#if F_APP_HARMAN_FEATURE_SUPPORT
                app_harman_sco_handle(p_link->id, true);
#endif
            }
            else
            {
                if (app_link_get_sco_conn_num() == 0)
                {
                    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SCO);
                }
            }

            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_SCO_CONN);
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
            T_APP_BR_LINK *p_link = NULL;
            p_link = app_link_find_br_link(param->sco_disconnected.bd_addr);

            if (p_link != NULL)
            {
                p_link->sco_handle = 0;
                app_bt_point_link_num_changed(BP_LINK_TYPE_B2S_SCO, BP_LINK_EVENT_DISCONNECT, p_link->bd_addr);
            }

            if (app_link_get_sco_conn_num() == 0)
            {
                app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SCO);
            }

            app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_SCO_CONN);
            app_bt_policy_state_machine(EVENT_SCO_DISCONNECTED, NULL);

#if F_APP_ERWS_SUPPORT
            app_bt_policy_b2b_qos_param_update();
#endif
        }
        break;

    case BT_EVENT_SDP_ATTR_INFO:
        {
            bt_param.bd_addr = param->sdp_attr_info.bd_addr;
            bt_param.sdp_info = &param->sdp_attr_info.info;

#if F_APP_TEAMS_BT_POLICY
            app_teams_bt_policy_handle_sdp_info(bt_param.bd_addr, bt_param.sdp_info);
#endif

            app_bt_policy_state_machine(EVENT_PROFILE_SDP_ATTR_INFO, &bt_param);
        }
        break;

    case  BT_EVENT_DID_ATTR_INFO:
        {
            T_APP_BR_LINK *p_link = NULL;
            uint8_t pair_idx_mapping;

            p_link = app_link_find_br_link(param->did_attr_info.bd_addr);

            if (p_link != NULL)
            {
                if (param->did_attr_info.vendor_id == 0x004c)
                {
                    p_link->remote_device_vendor_id = APP_REMOTE_DEVICE_IOS;
                }
                else
                {
                    p_link->remote_device_vendor_id = APP_REMOTE_DEVICE_OTHERS;
                }

                app_bond_get_pair_idx_mapping(p_link->bd_addr, &pair_idx_mapping);
                app_cfg_nv.remote_device_vendor_id[pair_idx_mapping] = p_link->remote_device_vendor_id;

                bt_param.bd_addr = param->did_attr_info.bd_addr;
                app_bt_policy_state_machine(EVENT_PROFILE_DID_ATTR_INFO, &bt_param);

#if F_APP_ERWS_SUPPORT
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    uint8_t temp_buff[7] = {0};

                    temp_buff[0] = (uint8_t)p_link->remote_device_vendor_id;
                    memcpy(&temp_buff[1], param->did_attr_info.bd_addr, 6);

                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_SPK1_REPLY_SRC_IS_IOS,
                                                        temp_buff, sizeof(temp_buff));
                }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
                if ((param->did_attr_info.vendor_id == GAMING_DONGLE_VID) &&
                    ((param->did_attr_info.product_id == GAMING_DONGLE_PID1) ||
                     (param->did_attr_info.product_id == GAMING_DONGLE_PID2)))
                {
                    app_dongle_record_init();
                    memcpy(app_db.connected_dongle_addr, param->did_attr_info.bd_addr, 6);
                    app_bond_add_dongle_bond_info(param->acl_conn_success.bd_addr);

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
                    app_multilink_customer_set_dongle_priority(p_link->id);
#endif

                    app_dongle_adv_stop();

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
                    if (app_cfg_const.enable_24g_bt_audio_source_switch
                        && (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT))
                    {
                        //current streaming on bt, do nothing
                        break;
                    }
#endif
                    app_audio_update_dongle_flag(true);
                    app_bt_policy_qos_param_update(param->did_attr_info.bd_addr, BP_TPOLL_DONGLE_CONN);

#if F_APP_DONGLE_ALWAYS_GAMING_MODE
                    app_db.restore_gaming_mode = true;
#endif
#if 0
                    if (app_db.gaming_mode)
                    {
#if (F_APP_DONGLE_ALWAYS_GAMING_MODE==0)
                        app_db.restore_gaming_mode = true;
#endif
                        app_db.disallow_play_gaming_mode_vp = true;

                        app_mmi_switch_gaming_mode();

#if F_APP_ERWS_SUPPORT
                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                        {
                            uint8_t cmd = true;

                            app_relay_async_single(APP_MODULE_TYPE_AUDIO_POLICY,
                                                   APP_REMOTE_MSG_SYNC_DISALLOW_PLAY_GAMING_MODE_VP);
                            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AUDIO_POLICY,
                                                                APP_REMOTE_MSG_ASK_TO_EXIT_GAMING_MODE,
                                                                (uint8_t *)&cmd, sizeof(cmd));
                        }
#endif

                        if (p_link->connected_profile & SPP_PROFILE_MASK)
                        {
                            //Exception handle: bau had sent cmd before receiving DID info.
                            app_db.restore_gaming_mode = false;
                            app_db.gaming_mode_request_is_received = false;
                            app_dongle_handle_gaming_mode_cmd(MMI_DEV_GAMING_MODE_SWITCH);
                        }
                    }
#endif

                }
#endif

#if F_APP_CONFERENCE_DONGLE_SUPPORT
                APP_PRINT_TRACE2("BT_EVENT_DID_ATTR_INFO: vendor_id %d, product_id %d",
                                 param->did_attr_info.vendor_id, param->did_attr_info.product_id);
                if ((param->did_attr_info.vendor_id == CONFERENCE_DONGLE_VID) &&
                    (param->did_attr_info.product_id == CONFERENCE_DONGLE_PID))
                {
                    app_audio_update_dongle_flag(true);
                    app_bond_add_dongle_bond_info(param->acl_conn_success.bd_addr);
                }
#endif
            }
        }
        break;

    case BT_EVENT_SDP_DISCOV_CMPL:
        {
            bt_param.bd_addr = param->sdp_discov_cmpl.bd_addr;
            bt_param.cause = param->sdp_discov_cmpl.cause;
#if F_APP_TEAMS_BT_POLICY
            app_teams_bt_policy_handle_sdp_cmpl_or_stop(param->sdp_discov_cmpl.bd_addr);
#endif

            app_bt_policy_state_machine(EVENT_PROFILE_SDP_DISCOV_CMPL, &bt_param);
        }
        break;

    case BT_EVENT_SDP_DISCOV_STOP:
        {
            bt_param.not_check_addr_flag = true;
#if F_APP_TEAMS_BT_POLICY
            app_teams_bt_policy_handle_sdp_cmpl_or_stop(param->sdp_discov_stop.bd_addr);
#endif

            app_bt_policy_state_machine(EVENT_PROFILE_SDP_DISCOV_STOP, &bt_param);
        }
        break;

    case BT_EVENT_HFP_CONN_CMPL:
        {
#if F_APP_NFC_SUPPORT
            uint8_t i;
            uint8_t app_idx;
#endif
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_conn_cmpl.bd_addr);

            if (p_link != NULL)
            {
                bt_param.bd_addr = param->hfp_conn_cmpl.bd_addr;
                if (param->hfp_conn_cmpl.is_hfp)
                {
                    bt_param.prof = HFP_PROFILE_MASK;
                }
                else
                {
                    bt_param.prof = HSP_PROFILE_MASK;
                }
#if F_APP_NFC_SUPPORT
                app_idx = p_link->id;
                if (nfc_data.nfc_multi_link_switch & NFC_MULTI_LINK_SWITCH_HF)
                {
                    nfc_data.nfc_multi_link_switch &= ~NFC_MULTI_LINK_SWITCH_HF;

                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if ((app_db.br_link[app_idx].connected_profile) &&
                            (i != app_idx))
                        {
                            app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                        }
                    }
                }
#endif
            }
            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_HFP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_HFP_DISCONN_CMPL:
        {
#if F_APP_NFC_SUPPORT
            nfc_data.nfc_multi_link_switch &= ~NFC_MULTI_LINK_SWITCH_HF;
#endif
            bt_param.bd_addr = param->hfp_disconn_cmpl.bd_addr;
            if (param->hfp_conn_cmpl.is_hfp)
            {
                bt_param.prof = HFP_PROFILE_MASK;
            }
            else
            {
                bt_param.prof = HSP_PROFILE_MASK;
            }
            bt_param.cause = param->hfp_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_HFP_CONN_FAIL:
        {
            bt_param.bd_addr = param->hfp_conn_fail.bd_addr;
            if (param->hfp_conn_fail.is_hfp)
            {
                bt_param.prof = HFP_PROFILE_MASK;
            }
            else
            {
                bt_param.prof = HSP_PROFILE_MASK;
            }
            bt_param.cause = param->hfp_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;

#if F_APP_BT_PROFILE_PBAP_SUPPORT
    case BT_EVENT_PBAP_CONN_CMPL:
        {
            bt_param.bd_addr = param->pbap_conn_cmpl.bd_addr;
            bt_param.prof = PBAP_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_PBAP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_PBAP_DISCONN_CMPL:
        {
            bt_param.bd_addr = param->pbap_disconn_cmpl.bd_addr;
            bt_param.prof = PBAP_PROFILE_MASK;
            bt_param.cause = param->pbap_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_PBAP_CONN_FAIL:
        {
            bt_param.bd_addr = param->pbap_conn_fail.bd_addr;
            bt_param.prof = PBAP_PROFILE_MASK;
            bt_param.cause = param->pbap_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;
#endif

#if F_APP_HID_SUPPORT
    case BT_EVENT_HID_CONN_CMPL:
        {
            bt_param.bd_addr = param->hid_conn_cmpl.bd_addr;
            bt_param.prof = HID_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_HID_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_HID_DISCONN_CMPL:
        {
            bt_param.bd_addr = param->hid_disconn_cmpl.bd_addr;
            bt_param.prof = HID_PROFILE_MASK;
            bt_param.cause = param->hid_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_HID_CONN_FAIL:
        {
            bt_param.bd_addr = param->hid_conn_fail.bd_addr;
            bt_param.prof = HID_PROFILE_MASK;
            bt_param.cause = param->hid_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;
#endif

    case BT_EVENT_A2DP_CONN_CMPL:
        {
#if F_APP_NFC_SUPPORT
            uint8_t i;
            uint8_t app_idx;
#endif
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->a2dp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                bt_param.bd_addr = param->a2dp_conn_cmpl.bd_addr;
                bt_param.prof = A2DP_PROFILE_MASK;
#if F_APP_NFC_SUPPORT
                app_idx = p_link->id;
                if (nfc_data.nfc_multi_link_switch & NFC_MULTI_LINK_SWITCH_A2DP)
                {
                    nfc_data.nfc_multi_link_switch &= ~NFC_MULTI_LINK_SWITCH_A2DP;

                    for (i = 0; i < MAX_BR_LINK_NUM; i++)
                    {
                        if ((app_db.br_link[app_idx].connected_profile) &&
                            (i != app_idx))
                        {
                            app_bt_policy_disconnect(app_db.br_link[i].bd_addr, ALL_PROFILE_MASK);
                        }
                    }
                }
#endif
            }

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
#if F_APP_NFC_SUPPORT
            nfc_data.nfc_multi_link_switch &= ~NFC_MULTI_LINK_SWITCH_A2DP;
#endif
            bt_param.bd_addr = param->a2dp_disconn_cmpl.bd_addr;
            bt_param.prof = A2DP_PROFILE_MASK;
            bt_param.cause = param->a2dp_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_A2DP_CONN_FAIL:
        {
            bt_param.bd_addr = param->a2dp_conn_fail.bd_addr;
            bt_param.prof = A2DP_PROFILE_MASK;
            bt_param.cause = param->a2dp_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {
            bt_param.bd_addr = param->avrcp_conn_cmpl.bd_addr;
            bt_param.prof = AVRCP_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_AVRCP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_AVRCP_DISCONN_CMPL:
        {
            bt_param.bd_addr = param->avrcp_disconn_cmpl.bd_addr;
            bt_param.prof = AVRCP_PROFILE_MASK;
            bt_param.cause = param->avrcp_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_AVRCP_CONN_FAIL:
        {
            bt_param.bd_addr = param->avrcp_conn_fail.bd_addr;
            bt_param.prof = AVRCP_PROFILE_MASK;
            bt_param.cause = param->avrcp_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;

    case BT_EVENT_SPP_CONN_CMPL:
        {
            bt_param.bd_addr = param->spp_conn_cmpl.bd_addr;
            bt_param.prof = SPP_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_SPP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            bt_param.bd_addr = param->spp_disconn_cmpl.bd_addr;
            bt_param.prof = SPP_PROFILE_MASK;
            bt_param.cause = param->spp_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_SPP_CONN_FAIL:
        {
            bt_param.bd_addr = param->spp_conn_fail.bd_addr;
            bt_param.prof = SPP_PROFILE_MASK;
            bt_param.cause = param->spp_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;

#if F_APP_IAP_SUPPORT
    case BT_EVENT_IAP_CONN_CMPL:
        {
            bt_param.bd_addr = param->iap_conn_cmpl.bd_addr;
            bt_param.prof = IAP_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
        }
        break;

    case BT_EVENT_IAP_SNIFFING_DISCONN_CMPL:
    case BT_EVENT_IAP_DISCONN_CMPL:
        {
            bt_param.bd_addr = param->iap_disconn_cmpl.bd_addr;
            bt_param.prof = IAP_PROFILE_MASK;
            bt_param.cause = param->iap_disconn_cmpl.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
        }
        break;

    case BT_EVENT_IAP_CONN_FAIL:
        {
            bt_param.bd_addr = param->iap_conn_fail.bd_addr;
            bt_param.prof = IAP_PROFILE_MASK;
            bt_param.cause = param->iap_conn_fail.cause;

            app_bt_policy_state_machine(EVENT_PROFILE_CONN_FAIL, &bt_param);
        }
        break;
#endif

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_ACL_SNIFFING_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link = NULL;
            uint8_t temp_buff[7] = {0};

            bt_param.bd_addr = param->acl_sniffing_conn_cmpl.bd_addr;

            if (param->acl_sniffing_conn_cmpl.cause == HCI_SUCCESS)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_bt_policy_state_machine(EVENT_SRC_CONN_SUC, &bt_param);
                }
                else
                {
                    p_link = app_link_find_br_link(bt_param.bd_addr);
                    if (p_link != NULL)
                    {
                        temp_buff[0] = (uint8_t)p_link->remote_device_vendor_id;
                        memcpy(&temp_buff[1], bt_param.bd_addr, 6);

                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_SPK1_REPLY_SRC_IS_IOS,
                                                            temp_buff, sizeof(temp_buff));

                        app_avrcp_sync_abs_vol_state(bt_param.bd_addr, p_link->abs_vol_state);
                    }
                }
            }
        }
        break;

    case BT_EVENT_ACL_SNIFFING_DISCONN_CMPL:
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            bt_param.bd_addr = param->acl_sniffing_disconn_cmpl.bd_addr;

            app_bt_policy_state_machine(EVENT_SRC_DISCONN_NORMAL, &bt_param);
        }
        break;


    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_SUCCESS)
            {
                bt_param.is_suc = true;
                bt_param.bud_role = param->remote_roleswap_status.device_role;
                app_bt_policy_state_machine(EVENT_ROLESWAP, &bt_param);
            }
            else if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_FAIL ||
                     param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_TERMINATED ||
                     ((param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_START_RSP) &&
                      (!param->remote_roleswap_status.u.start_rsp.accept)))
            {
                bt_param.is_suc = false;
                bt_param.bud_role = param->remote_roleswap_status.device_role;
                app_bt_policy_state_machine(EVENT_ROLESWAP, &bt_param);
            }
            else if ((param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_START_RSP) &&
                     (param->remote_roleswap_status.u.start_rsp.accept))
            {
                app_relay_async_single(APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_ROLESWAP_INFO);
                app_sniff_mode_send_roleswap_info();
                app_bt_policy_relay_pri_linkback_node();
            }
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            app_db.remote_session_state = REMOTE_SESSION_STATE_CONNECTED;

            bt_param.bd_addr = param->remote_conn_cmpl.bd_addr;
            bt_param.prof = RDTP_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_BUD_REMOTE_CONN_CMPL, &bt_param);
            app_sniff_mode_b2b_enable(bt_param.bd_addr, SNIFF_DISABLE_MASK_ACL);

            if (app_cfg_const.rws_remote_link_encryption_off)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    bt_link_encryption_set(bt_param.bd_addr, false);
                }
            }

            if (app_db.need_sync_case_battery_to_pri &&
                (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
            {
                app_db.need_sync_case_battery_to_pri = false;
                app_relay_async_single(APP_MODULE_TYPE_CHARGER, APP_CHARGER_EVENT_BATT_CASE_LEVEL);
            }

            p_link = app_link_find_br_link(bt_param.bd_addr);
            if (p_link != NULL)
            {
                app_report_rws_state();
                app_report_rws_bud_info();
                app_bond_sync_b2s_link_record();
            }
            app_bt_policy_qos_param_update(bt_param.bd_addr, BP_TPOLL_B2B_CONN_EVENT);
        }
        break;

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            app_db.remote_session_state = REMOTE_SESSION_STATE_DISCONNECTED;

            bt_param.bd_addr = param->remote_disconn_cmpl.bd_addr;
            bt_param.prof = RDTP_PROFILE_MASK;

            app_bt_policy_state_machine(EVENT_BUD_REMOTE_DISCONN_CMPL, &bt_param);

            p_link = app_link_find_br_link(bt_param.bd_addr);
            if (p_link != NULL)
            {
                app_report_rws_state();
                app_report_rws_bud_info();
            }
            app_bt_policy_qos_param_update(bt_param.bd_addr, BP_TPOLL_B2B_DISC_EVENT);
        }
        break;
#endif

    case BT_EVENT_LINK_QOS_SET_CMPL:
        {
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->link_qos_set_cmpl.bd_addr);
            //reset need two condition:
            //1. The worked tpoll in controller is different value requested by host
            //2. Retry count is beyond limite number
            if (p_link)
            {
                if (param->link_qos_set_cmpl.cause)
                {
                    APP_PRINT_ERROR3("set qoe failed: cause %d, real tpoll %d, reset num %d",
                                     param->link_qos_set_cmpl.cause, param->link_qos_set_cmpl.tpoll,
                                     p_link->tpoll_reset_num);

                    if (p_link->tpoll_reset_num < TPOLL_RESET_MAX_NUM)
                    {
                        p_link->tpoll_reset_num++;
                    }
                    else
                    {
                        bt_link_tpoll_range_set(param->link_qos_set_cmpl.bd_addr, 0, 0);
                        p_link->tpoll_reset_num = 0;
                    }

                    app_bt_policy_qos_param_update(param->link_qos_set_cmpl.bd_addr, BP_TPOLL_QOS_SET_FAIL_EVENT);
                }
                else
                {
                    p_link->tpoll_reset_num = 0;
                }
            }
        }
        break;

    default:
        break;
    }
}

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
static bool app_bt_policy_validate_bd_addr_with_dongle_id(uint8_t dongle_id, uint8_t *bd_addr)
{
    bool ret = false;

    if ((dongle_id == 0) || (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY))
    {
        // not OTA tooling mode or secondary
        ret = true;
    }
    else if (dongle_id >= 1)
    {
        uint8_t dongle_bd_addr[6] = {0, 0, 0, 0, 0xff, 0xaf};

        memcpy(dongle_bd_addr, &ota_dongle_addr[dongle_id - 1], 4);

        if (!memcmp(bd_addr, dongle_bd_addr, 6))
        {
            ret = true;
        }

        APP_PRINT_TRACE3("validate_bd_addr_with_dongle_id: ret = %d, dongle_id = %d, bd_addr = %s", ret,
                         dongle_id, TRACE_BDADDR(bd_addr));
    }

    return ret;
}
#endif

#if F_APP_ERWS_SUPPORT
void app_bt_policy_abandon_engage(void)
{
    if (!engage_done)
    {
        engage_done = true;
        engage_afe_done();
    }
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if F_APP_ERWS_SUPPORT

static void app_bt_policy_pri_collect_relay_info_for_roleswap(T_BP_ROLESWAP_INFO *bp_roleswap_info)
{
    uint8_t app_idx;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        bp_roleswap_info->last_src_conn_idx = last_src_conn_idx;
        bp_roleswap_info->bp_state = bp_state;
        bp_roleswap_info->a2dp_focus_toggle_state = app_cfg_const.disable_multilink_preemptive;

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
        app_sensor_mems_atti_get(bp_roleswap_info->mems_atti);
#endif

        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2s_link_by_id(app_idx))
            {
                bp_roleswap_info->link.link_src_conn_idx = app_db.br_link[app_idx].src_conn_idx;
                bp_roleswap_info->link.remote_device_vendor_id = app_db.br_link[app_idx].remote_device_vendor_id;
                bp_roleswap_info->link.abs_vol_state = app_db.br_link[app_idx].abs_vol_state;
                bp_roleswap_info->link.rtk_vendor_spp_active = app_db.br_link[app_idx].rtk_vendor_spp_active;
                bp_roleswap_info->link.tx_event_seqn = app_db.br_link[app_idx].tx_event_seqn;
                bp_roleswap_info->link.b2s_connected_vp_is_played =
                    app_db.br_link[app_idx].b2s_connected_vp_is_played;
                bp_roleswap_info->link.call_status = app_db.br_link[app_idx].call_status;

#if F_APP_DURIAN_SUPPORT
                bp_roleswap_info->link.audio_opus_status = app_db.br_link[app_idx].audio_opus_status;
#if F_APP_TWO_GAIN_TABLE
                bp_roleswap_info->link.audio_sharing = app_db.br_link[app_idx].audio_sharing;
#endif
#endif

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
                bp_roleswap_info->link.mems_is_start = app_db.br_link[app_idx].mems_is_start;
#endif
#if GFPS_SASS_SUPPORT
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    bp_roleswap_info->link.connected_by_linkback = app_db.br_link[app_idx].connected_by_linkback;
                    bp_roleswap_info->link.gfps_rfc_chann = app_db.br_link[app_idx].gfps_rfc_chann;
                    bp_roleswap_info->link.gfps_inuse_account_key = app_db.br_link[app_idx].gfps_inuse_account_key;
                    memcpy(bp_roleswap_info->link.gfps_session_nonce, app_db.br_link[app_idx].gfps_session_nonce, 8);
                    if (app_db.br_link[app_idx].connected_profile & GFPS_PROFILE_MASK)
                    {
                        bp_roleswap_info->link.gfps_rfc_connected = true;
                    }
                }
#endif
            }
        }
    }
}

static void app_bt_policy_sec_recv_relay_info_for_roleswap(void *buf, uint16_t len)
{
    T_BP_ROLESWAP_INFO *p_info = (T_BP_ROLESWAP_INFO *)buf;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (sizeof(*p_info) == len)
        {
            memcpy_s(&bp_roleswap_info_temp, sizeof(T_BP_ROLESWAP_INFO), p_info, len);
        }
    }
}

static void app_bt_policy_new_pri_apply_relay_info_when_roleswap_suc(void)
{
    uint8_t app_idx;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        last_src_conn_idx = bp_roleswap_info_temp.last_src_conn_idx;
        pri_bp_state = bp_roleswap_info_temp.bp_state;
        app_cfg_const.disable_multilink_preemptive = bp_roleswap_info_temp.a2dp_focus_toggle_state;

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
        app_sensor_mems_atti_save(bp_roleswap_info_temp.mems_atti);
#endif

        for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2s_link_by_id(app_idx))
            {
                app_db.br_link[app_idx].src_conn_idx = bp_roleswap_info_temp.link.link_src_conn_idx;
                app_db.br_link[app_idx].remote_device_vendor_id =
                    bp_roleswap_info_temp.link.remote_device_vendor_id;
                app_db.br_link[app_idx].abs_vol_state = bp_roleswap_info_temp.link.abs_vol_state;
                app_db.br_link[app_idx].rtk_vendor_spp_active = bp_roleswap_info_temp.link.rtk_vendor_spp_active;
                app_db.br_link[app_idx].tx_event_seqn = bp_roleswap_info_temp.link.tx_event_seqn;
                app_db.br_link[app_idx].b2s_connected_vp_is_played =
                    bp_roleswap_info_temp.link.b2s_connected_vp_is_played;
                app_db.br_link[app_idx].call_status = bp_roleswap_info_temp.link.call_status;
#if F_APP_DURIAN_SUPPORT
                app_db.br_link[app_idx].audio_opus_status = bp_roleswap_info_temp.link.audio_opus_status;
#if F_APP_TWO_GAIN_TABLE
                app_db.br_link[app_idx].audio_sharing = bp_roleswap_info_temp.link.audio_sharing;
#endif
#endif

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
                app_db.br_link[app_idx].mems_is_start = bp_roleswap_info_temp.link.mems_is_start;
#endif

#if GFPS_SASS_SUPPORT
                if (extend_app_cfg_const.gfps_sass_support)
                {
                    app_db.br_link[app_idx].connected_by_linkback = bp_roleswap_info_temp.link.connected_by_linkback;
                    app_db.br_link[app_idx].gfps_rfc_chann = bp_roleswap_info_temp.link.gfps_rfc_chann;
                    app_db.br_link[app_idx].gfps_inuse_account_key = bp_roleswap_info_temp.link.gfps_inuse_account_key;
                    memcpy(app_db.br_link[app_idx].gfps_session_nonce, bp_roleswap_info_temp.link.gfps_session_nonce,
                           8);
                    if (bp_roleswap_info_temp.link.gfps_rfc_connected)
                    {
                        app_db.br_link[app_idx].connected_profile |= GFPS_PROFILE_MASK;
                    }
                }
#endif
            }
        }
    }
}

static uint16_t app_bt_policy_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    T_BP_ROLESWAP_INFO bp_roleswap_info;

    switch (msg_type)
    {
    case BT_POLICY_MSG_ROLESWAP_INFO:
        {
            app_bt_policy_pri_collect_relay_info_for_roleswap(&bp_roleswap_info);

            payload_len = sizeof(bp_roleswap_info);
            msg_ptr = (uint8_t *)&bp_roleswap_info;
        }
        break;

    case BT_POLICY_MSG_PRI_BP_STATE:
        {
            payload_len = sizeof(bp_state);
            msg_ptr = (uint8_t *)&bp_state;
        }
        break;

    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_BT_POLICY, payload_len, msg_ptr, skip,
                              total);
}

static void app_bt_policy_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                      T_REMOTE_RELAY_STATUS status)
{
    if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        switch (msg_type)
        {
        case BT_POLICY_MSG_ROLESWAP_INFO:
            {
                app_bt_policy_sec_recv_relay_info_for_roleswap(buf, len);
            }
            break;

        case BT_POLICY_MSG_PRI_BP_STATE:
            {
                app_bt_policy_recv_relay_pri_bp_state(buf, len);
            }
            break;

        case BT_POLICY_MSG_PRI_REQ:
            {
                app_bt_policy_recv_relay_pri_req(buf, len);
            }
            break;

        case BT_POLICY_MSG_SNIFF_MODE:
            {
                app_sniff_mode_recv_roleswap_info(buf, len);
            }
            break;

        case BT_POLICY_MSG_LINKBACK_NODE:
            {
                app_bt_policy_recv_relay_pri_linkback_node(buf, len);
            }
            break;

        case BT_POLICY_MSG_B2S_ACL_CONN:
            {
                app_bt_policy_b2s_conn_start_timer();
            }
            break;

        case BT_POLICY_MSG_SYNC_SERVICE_STATUS:
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    bool all_service_status = *((bool *)buf);
                    app_hfp_set_no_service_timer(all_service_status);
                }
            }
            break;

        default:
            break;
        }
    }
}
#endif

static void app_bt_policy_device_event_cback(uint32_t event, void *msg)
{
    switch (event)
    {
    case APP_DEVICE_IPC_EVT_STACK_READY:
        {
            app_bt_policy_update_pair_idx_mapping();
        }
        break;

    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void app_bt_policy_stop_a2dp_and_sco(void)
{
    T_APP_BR_LINK *p_link = NULL;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_link_check_b2s_link_by_id(i))
        {
            p_link = &app_db.br_link[i];

            if (p_link)
            {
                if (p_link->a2dp_track_handle)
                {
                    audio_track_stop(p_link->a2dp_track_handle);
                }

                if (p_link->sco_track_handle)
                {
                    audio_track_stop(p_link->sco_track_handle);
                }
            }
        }
    }
}

#if F_APP_LEA_SUPPORT

void app_bt_policy_enter_sched(void)
{
    app_bt_policy_stable_sched(STABLE_ENTER_MODE_NORMAL);
}

void app_bt_policy_set_legacy(uint8_t para)
{
    APP_PRINT_INFO2("app_bt_policy_set_legacy: para %d, state %d", para, app_bt_policy_get_state());
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }
#endif

    if (para == 0)
    {
        app_stop_timer(&timer_idx_pairing_mode);
    }
    else if (para == 1)
    {
        if ((app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_GET_LEGACY, NULL) ||
             app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_GET_CIS_POLICY, NULL)) &&
            app_cfg_const.timer_pairing_timeout  &&
            (app_bt_policy_get_state() == BP_STATE_PAIRING_MODE))
        {
            app_start_timer(&timer_idx_pairing_mode, "pairing_mode",
                            bt_policy_timer_id, APP_TIMER_PAIRING_MODE, 0, false,
                            app_cfg_const.timer_pairing_timeout * 1000);
        }
    }
}

bool app_bt_policy_is_pairing(void)
{
    if (timer_idx_pairing_mode != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool app_bt_policy_listening_allow_poweroff(void)
{
#if F_APP_LISTENING_MODE_SUPPORT

    if (app_cfg_const.enable_auto_power_off_when_listening_mode_is_not_off
        || (app_db.current_listening_state == ANC_OFF_APT_OFF))
    {
        return true;
    }
    else
    {
        return false;
    }
#else
    return true;
#endif
}

#endif

T_APP_CALL_STATUS app_bt_policy_get_call_status(void)
{
#if F_APP_LEA_SUPPORT
    if (mtc_get_btmode() == MULTI_PRO_BT_CIS)
    {
        return app_lea_ccp_get_call_status();
    }
    else
#endif
    {
        return app_hfp_get_call_status();
    }
}

void app_bt_policy_init(void)
{
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_bt_policy_relay_cback, app_bt_policy_parse_cback,
                             APP_MODULE_TYPE_BT_POLICY, BT_POLICY_MSG_MAX);
#endif

    bt_mgr_cback_register(app_bt_policy_cback);
    app_timer_reg_cb(app_bt_policy_timer_cback, &bt_policy_timer_id);
    app_ipc_subscribe(APP_DEVICE_IPC_TOPIC, app_bt_policy_device_event_cback);

#if F_APP_ERWS_SUPPORT
    {
        engage_init();
    }
#endif
}
