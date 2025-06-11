/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "gap_le.h"
#include "btm.h"

#include "engage.h"
#include "bt_bond.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_bt_policy_api.h"
#include "app_bt_policy_int.h"
#include "app_linkback.h"
#include "app_bond.h"
#include "app_relay.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_multilink.h"
#if F_APP_TEAMS_BT_POLICY
#include "app_teams_hid.h"
#endif
#if F_APP_CFU_FEATURE_SUPPORT
#include "app_common_cfu.h"
#endif
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_common.h"
#include "app_dongle_dual_mode.h"
#endif

extern T_LINKBACK_ACTIVE_NODE linkback_active_node;

extern T_BP_STATE bp_state;
extern T_EVENT cur_event;

extern T_BT_DEVICE_MODE radio_mode;

extern bool first_connect_sync_default_volume_to_src;
static uint8_t enlarge_tpoll_state = false;

const T_BP_TPOLL_MAPPING tpoll_table[BP_TPOLL_MAX] =
{
    {BP_TPOLL_INIT, 0},
#if F_APP_TEAMS_FEATURE_SUPPORT
    {BP_TPOLL_IDLE, 16},
#else
    {BP_TPOLL_IDLE, 40},
#endif
    {BP_TPOLL_GAMING_A2DP, 22},
    {BP_TPOLL_GAMING_DONGLE_RWS, 6},
    {BP_TPOLL_GAMING_DONGLE_RWS_SINGLE, 12},
    {BP_TPOLL_GAMING_DONGLE_STEREO, 6},
#if F_APP_TEAMS_FEATURE_SUPPORT
    {BP_TPOLL_A2DP, 10},
#else
    {BP_TPOLL_A2DP, 16},
#endif
    {BP_TPOLL_TEAMS_UPDATE, 16},
    {BP_TPOLL_IDLE_SINGLE_LINKBACK, 120},
    /* legacy gaming dongle always poll; no need to set low tpoll */
    {BP_TPOLL_GAMING_DONGLE_SPP, 40},
    {BP_TPOLL_GAMING_DONGLE_A2DP, 40},
    /* LEA upstreaming is active, BR must proffer bandwidth*/
    {BP_TPOLL_LEA_SUSPEND_A2DP, 120},
};

void app_bt_policy_startup(T_BP_IND_FUN fun, bool at_once_trigger)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.startup_param.ind_fun = fun;
    bt_param.startup_param.at_once_trigger = at_once_trigger;

    app_bt_policy_state_machine(EVENT_STARTUP, &bt_param);
}

void app_bt_policy_shutdown(void)
{
    app_bt_policy_state_machine(EVENT_SHUTDOWN, NULL);
}

void app_bt_policy_stop(void)
{
    app_bt_policy_state_machine(EVENT_STOP, NULL);
}

void app_bt_policy_restore(void)
{
    app_bt_policy_state_machine(EVENT_RESTORE, NULL);
}

#if 0
void app_bt_policy_prepare_for_roleswap(void)
{
    app_bt_policy_state_machine(EVENT_PREPARE_FOR_ROLESWAP, NULL);
}
#endif

void app_bt_policy_msg_le_auth(uint8_t *bd_addr)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.is_b2b = false;
    bt_param.not_check_addr_flag = true;
    app_bt_policy_state_machine(EVENT_SRC_AUTH_SUC, &bt_param);
}

void app_bt_policy_msg_prof_conn(uint8_t *bd_addr, uint32_t prof)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = prof;

    app_bt_policy_state_machine(EVENT_PROFILE_CONN_SUC, &bt_param);
}

void app_bt_policy_msg_prof_disconn(uint8_t *bd_addr, uint32_t prof, uint16_t cause)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = prof;
    bt_param.cause = cause;

    app_bt_policy_state_machine(EVENT_PROFILE_DISCONN, &bt_param);
}

void app_bt_policy_enter_pairing_mode(bool force, bool visiable)
{
    T_BT_PARAM bt_param;

#if F_APP_GAMING_DONGLE_SUPPORT
    if (app_dongle_is_streaming())
    {
        APP_PRINT_TRACE0("disallow pairing when dongle streaming");
        return;
    }
#endif

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.is_force = force;
    bt_param.is_visiable = visiable;

    app_bt_policy_state_machine(EVENT_DEDICATED_ENTER_PAIRING_MODE, &bt_param);
}

void app_bt_policy_exit_pairing_mode(void)
{
    app_bt_policy_state_machine(EVENT_DEDICATED_EXIT_PAIRING_MODE, NULL);
}

void app_bt_policy_enter_dut_test_mode(void)
{
    app_bt_policy_state_machine(EVENT_ENTER_DUT_TEST_MODE, NULL);
}

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
void app_bt_policy_start_ota_shaking(void)
{
    app_bt_policy_state_machine(EVENT_START_OTA_SHAKING, NULL);
}

void app_bt_policy_enter_ota_mode(bool connectable)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.is_connectable = connectable;

    app_bt_policy_state_machine(EVENT_ENTER_OTA_MODE, &bt_param);
}
#endif

void app_bt_policy_default_connect(uint8_t *bd_addr, uint32_t plan_profs, bool check_bond_flag)
{
    T_BT_PARAM bt_param;
    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = plan_profs;
    bt_param.is_special = false;
    bt_param.check_bond_flag = check_bond_flag;
    app_bt_policy_state_machine(EVENT_DEDICATED_CONNECT, &bt_param);
}

#if 0
void app_bt_policy_special_connect(uint8_t *bd_addr, uint32_t plan_prof,
                                   T_LINKBACK_SEARCH_PARAM *search_param)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = plan_prof;
    bt_param.is_special = true;
    bt_param.search_param = search_param;
    bt_param.check_bond_flag = false;
    app_bt_policy_state_machine(EVENT_DEDICATED_CONNECT, &bt_param);
}
#endif

void app_bt_policy_disconnect(uint8_t *bd_addr, uint32_t plan_profs)
{
    T_BT_PARAM bt_param;

    memset(&bt_param, 0, sizeof(T_BT_PARAM));

    bt_param.bd_addr = bd_addr;
    bt_param.prof = plan_profs;
    app_bt_policy_state_machine(EVENT_DEDICATED_DISCONNECT, &bt_param);
}

void app_bt_policy_disc_all_b2s(void)
{
    uint32_t plan_profs = 0;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_link_check_b2s_link_by_id(i))
        {
            plan_profs = (app_db.br_link[i].connected_profile & (~RDTP_PROFILE_MASK));
            app_bt_policy_disconnect(app_db.br_link[i].bd_addr, plan_profs);
        }
    }
}

#if F_APP_GAMING_DONGLE_SUPPORT
static bool app_bt_policy_check_dual_mode_disallow_linkback(uint32_t bond_flag)
{
    bool ret = false;

    if (app_cfg_const.enable_dongle_dual_mode)
    {
        if (app_cfg_nv.is_dual_mode == false)
        {
            /* disallow linkback phone */
            if ((bond_flag & BOND_FLAG_DONGLE) == 0)
            {
                ret = true;
            }
        }

        if (app_cfg_const.enable_multi_link == false)
        {
            if (app_cfg_nv.is_dual_mode)
            {
                /* disallow linkback dongle */
                if (bond_flag & BOND_FLAG_DONGLE)
                {
                    ret = true;
                }
            }
        }
    }

    return ret;
}
#endif

void app_bt_policy_conn_all_b2s(void)
{
    uint8_t bd_addr[6] = {0};
    uint8_t bond_num = bt_bond_num_get();
    uint32_t bond_flag = 0, plan_profs = 0;
    uint8_t max_load_num = (app_cfg_const.maximum_linkback_number_high_bit << 3) |
                           app_cfg_const.maximum_linkback_number;
    uint8_t num = 0;

    for (uint8_t i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, bd_addr))
        {
            bt_bond_flag_get(bd_addr, &bond_flag);

#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_bt_policy_check_dual_mode_disallow_linkback(bond_flag))
            {
                continue;
            }
#endif

            if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
            {
                plan_profs = app_bt_policy_get_profs_by_bond_flag(bond_flag);
                app_bt_policy_default_connect(bd_addr, plan_profs, false);
                if (++num >= max_load_num)
                {
                    break;
                }
            }
        }
    }
}

uint8_t app_bt_policy_valid_b2s_bond_bum(void)
{
    uint8_t bd_addr[6] = {0};
    uint8_t bond_num = bt_bond_num_get();
    uint32_t bond_flag = 0;
    uint8_t num = 0;

    for (uint8_t i = 1; i <= bond_num; i++)
    {
        bond_flag = 0;
        if (bt_bond_addr_get(i, bd_addr))
        {
            bt_bond_flag_get(bd_addr, &bond_flag);

            if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
            {
                num++;
            }
        }
    }

    APP_PRINT_INFO1("app_bt_policy_valid_b2s_bond_bum: bond_num %d", num);
    return num;
}

void app_bt_policy_disconnect_all_link(void)
{
    app_bt_policy_state_machine(EVENT_DISCONNECT_ALL, NULL);
}

T_BP_STATE app_bt_policy_get_state(void)
{
    return bp_state;
}

T_BT_DEVICE_MODE app_bt_policy_get_radio_mode(void)
{
    return radio_mode;
}

void app_bt_policy_sync_b2s_connected(void)
{
#if F_APP_ERWS_SUPPORT
    APP_PRINT_TRACE1("app_bt_policy_sync_b2s_connected: b2s_connected_num %d",
                     app_link_get_b2s_link_num());

    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        app_relay_async_single(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_PHONE_CONNECTED);
    }
#endif
}

bool app_bt_policy_get_first_connect_sync_default_vol_flag(void)
{
    return first_connect_sync_default_volume_to_src;
}

void app_bt_policy_set_first_connect_sync_default_vol_flag(bool flag)
{
    first_connect_sync_default_volume_to_src = flag;
}

static bool app_bt_policy_bt_link_qos_set(uint8_t *bd_addr, T_BT_QOS_TYPE type, uint16_t tpoll)
{
#if F_APP_GAMING_DONGLE_SUPPORT && !LEA_SUPPORT
    if (app_link_check_dongle_link(bd_addr))
    {
        /* dongle link tpoll will be decided by dongle */
        return false;
    }
#endif

    bt_link_qos_set(bd_addr, type, tpoll);

    return true;
}

void app_bt_policy_set_idle_tpoll(T_BP_TPOLL_STATE tpoll_state)
{
    uint8_t inactive_a2dp_idx = app_a2dp_get_inactive_idx();

    if (tpoll_state == BP_TPOLL_GAMING_A2DP ||
        tpoll_state == BP_TPOLL_A2DP ||
        tpoll_state == BP_TPOLL_GAMING_DONGLE_A2DP)
    {
        if ((inactive_a2dp_idx != MAX_BR_LINK_NUM) &&
            (app_db.br_link[inactive_a2dp_idx].tpoll_status !=
             tpoll_table[BP_TPOLL_IDLE].state))
        {
            if (app_bt_policy_bt_link_qos_set(app_db.br_link[inactive_a2dp_idx].bd_addr, BT_QOS_TYPE_GUARANTEED,
                                              (tpoll_table[BP_TPOLL_IDLE].tpoll_value + (inactive_a2dp_idx * 2))))
            {
                app_db.br_link[inactive_a2dp_idx].tpoll_status = tpoll_table[BP_TPOLL_IDLE].state;
            }
        }
    }
    else if (tpoll_state == BP_TPOLL_GAMING_DONGLE_SPP)
    {
        uint8_t find_hfp_inactive_idx = MAX_BR_LINK_NUM;

        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (i != app_hfp_get_active_idx())
                {
                    find_hfp_inactive_idx = i;
                }
            }
        }

        if (find_hfp_inactive_idx != MAX_BR_LINK_NUM)
        {
            //tpoll 40
            if (app_db.br_link[find_hfp_inactive_idx].tpoll_status !=
                tpoll_table[BP_TPOLL_IDLE].state)
            {
                if (app_bt_policy_bt_link_qos_set(app_db.br_link[find_hfp_inactive_idx].bd_addr,
                                                  BT_QOS_TYPE_GUARANTEED,
                                                  (tpoll_table[BP_TPOLL_IDLE].tpoll_value + (find_hfp_inactive_idx * 2))))
                {
                    app_db.br_link[find_hfp_inactive_idx].tpoll_status = tpoll_table[BP_TPOLL_IDLE].state;
                }
            }
        }
    }
    else if (tpoll_state == BP_TPOLL_LEA_SUSPEND_A2DP)
    {
        uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

        if ((active_a2dp_idx != MAX_BR_LINK_NUM) &&
            (app_db.br_link[active_a2dp_idx].tpoll_status !=
             tpoll_table[BP_TPOLL_LEA_SUSPEND_A2DP].state))
        {
            bt_link_qos_set(app_db.br_link[active_a2dp_idx].bd_addr, BT_QOS_TYPE_GUARANTEED,
                            (tpoll_table[BP_TPOLL_LEA_SUSPEND_A2DP].tpoll_value));
            app_db.br_link[app_a2dp_get_inactive_idx()].tpoll_status = tpoll_table[BP_TPOLL_IDLE].state;
        }
    }
    else
    {
        for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (app_link_check_b2s_link_by_id(i))
            {
                if (app_db.br_link[i].tpoll_status != tpoll_table[tpoll_state].state)
                {
                    if (app_bt_policy_bt_link_qos_set(app_db.br_link[i].bd_addr, BT_QOS_TYPE_GUARANTEED,
                                                      (tpoll_table[tpoll_state].tpoll_value + (i * 2))))
                    {
                        app_db.br_link[i].tpoll_status = tpoll_table[tpoll_state].state;
                    }
                }
            }
        }
    }
}

void app_bt_policy_set_active_tpoll_only(uint8_t idx, T_BP_TPOLL_STATE tpoll_state)
{
    if (app_link_check_b2s_link_by_id(idx))
    {
        if (app_db.br_link[idx].tpoll_status != tpoll_table[tpoll_state].state)
        {
            if (app_bt_policy_bt_link_qos_set(app_db.br_link[idx].bd_addr, BT_QOS_TYPE_GUARANTEED,
                                              tpoll_table[tpoll_state].tpoll_value))
            {
                app_db.br_link[idx].tpoll_status = tpoll_table[tpoll_state].state;
            }
        }
    }
}

uint8_t app_bt_policy_get_enlarge_tpoll_state(void)
{
    return enlarge_tpoll_state;
}

void app_bt_policy_set_enlarge_tpoll_state(T_BP_TPOLL_EVENT event)
{
    if (((event == BP_TPOLL_ACL_CONN_EVENT) ||
         (event == BP_TPOLL_LINKBACK_START)) &&
        (app_link_get_b2s_link_num() == 1) &&
        (app_bt_policy_get_state() == BP_STATE_LINKBACK) &&
        (app_db.br_link[app_a2dp_get_active_idx()].streaming_fg == false))
    {
        //page b2s when already conn_1_b2s
        enlarge_tpoll_state = true;
    }
    else if ((event != BP_TPOLL_B2B_CONN_EVENT) &&
             (event != BP_TPOLL_B2B_DISC_EVENT))
    {
        enlarge_tpoll_state = false;
    }
}

void app_bt_policy_qos_param_update(uint8_t *bd_addr, T_BP_TPOLL_EVENT event)
{
#if F_APP_GAMING_DONGLE_SUPPORT
    T_APP_BR_LINK *bau_p_link;
    bau_p_link = app_link_find_br_link(app_db.connected_dongle_addr);
#endif
    uint8_t find_bau_app_idx = MAX_BR_LINK_NUM;
    uint8_t app_idx = app_a2dp_get_active_idx();
#if F_APP_TEAMS_BT_POLICY || F_APP_CFU_FEATURE_SUPPORT
    T_APP_BR_LINK *p_teams_link = NULL;
#endif

    uint8_t j;
    //ignore cmd
    if (event == BP_TPOLL_ACL_CONN_EVENT)
    {
        if (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)
        {
            for (j = 0; j < MAX_BR_LINK_NUM; j++)
            {
                if (app_link_check_b2s_link_by_id(j))
                {
                    bt_link_tpoll_range_set(app_db.br_link[j].bd_addr, 0, 0);
                }
            }
        }
    }
    else if (event == BP_TPOLL_ACL_DISC_EVENT)
    {
        if (app_link_get_b2s_link_num() != MULTILINK_SRC_CONNECTED)
        {
            for (j = 0; j < MAX_BR_LINK_NUM; j++)
            {
                if (app_link_check_b2s_link_by_id(j))
                {
                    bt_link_tpoll_range_set(app_db.br_link[j].bd_addr, 0x06, 0x1000);
                }
            }
        }
    }

    app_bt_policy_set_enlarge_tpoll_state(event);

    if (event == BP_TPOLL_HID_CONN_EVENT || event == BP_TPOLL_QOS_SET_FAIL_EVENT)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);

        if (p_link)
        {
            uint8_t req_tpoll = 0;
            if (p_link->tpoll_status == BP_TPOLL_IDLE)
            {
                req_tpoll = tpoll_table[BP_TPOLL_IDLE].tpoll_value + (p_link->id * 2);
            }
            else
            {
                req_tpoll = tpoll_table[p_link->tpoll_status].tpoll_value;
            }

            bt_link_qos_set(bd_addr, BT_QOS_TYPE_GUARANTEED, req_tpoll);
        }
    }
#if F_APP_GAMING_DONGLE_SUPPORT
    else if ((app_db.gaming_mode) && (app_db.remote_is_dongle))
    {
        //only stereo now.
        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) &&
            (app_cfg_const.enable_multi_link))
        {
            if (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)
            {

                if (app_hfp_sco_is_connected())//phone sco
                {
                    //tpoll 40 42
                    app_bt_policy_set_idle_tpoll(BP_TPOLL_IDLE);
                }
                else if ((bau_p_link != NULL) &&
                         (app_db.dongle_is_enable_mic) &&
                         (bau_p_link->id == app_hfp_get_active_idx()))//BAU SPP
                {
                    //tpoll active=12 inactive=40
                    app_bt_policy_set_idle_tpoll(BP_TPOLL_GAMING_DONGLE_SPP);

                    app_bt_policy_set_active_tpoll_only(app_hfp_get_active_idx(), BP_TPOLL_GAMING_DONGLE_SPP);
                }
                else if (app_db.br_link[app_idx].streaming_fg == true)//BAU A2DP or phone A2DP
                {
                    //tpoll active=12 inactive=40
                    app_bt_policy_set_idle_tpoll(BP_TPOLL_GAMING_DONGLE_A2DP);

                    app_bt_policy_set_active_tpoll_only(app_a2dp_get_active_idx(), BP_TPOLL_GAMING_DONGLE_A2DP);
                }
                else //idle idle
                {
                    //tpoll 40 42
                    app_bt_policy_set_idle_tpoll(BP_TPOLL_IDLE);
                }
            }
            else //single link
            {

                if ((bau_p_link != NULL) &&
                    (((app_db.dongle_is_enable_mic) &&
                      (bau_p_link->id == app_hfp_get_active_idx())) ||   //BAU SPP
                     ((app_db.br_link[bau_p_link->id].streaming_fg == true) &&
                      (bau_p_link->id == app_a2dp_get_active_idx()))))         //BAU A2DP
                {
                    //tpoll 12
                    app_bt_policy_set_active_tpoll_only(bau_p_link->id, BP_TPOLL_GAMING_DONGLE_A2DP);
                }
                else
                {
                    //tpoll 40
                    app_bt_policy_set_idle_tpoll(BP_TPOLL_IDLE);
                }
            }
        }
        else
        {
            //only one link now, TODO: support multilink tpoll should be modified to other setting.
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    find_bau_app_idx = i;
                    break;
                }
            }

            if (find_bau_app_idx != MAX_BR_LINK_NUM)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) //RWS
                {
                    app_bt_policy_set_active_tpoll_only(find_bau_app_idx, BP_TPOLL_GAMING_DONGLE_RWS);
                }
                else
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) //STEREO
                    {
                        app_bt_policy_set_active_tpoll_only(find_bau_app_idx, BP_TPOLL_GAMING_DONGLE_STEREO);
                    }
                    else//RWS_SINGLE
                    {
                        app_bt_policy_set_active_tpoll_only(find_bau_app_idx, BP_TPOLL_GAMING_DONGLE_RWS_SINGLE);
                    }
                }
            }
        }
    }
#endif
#if F_APP_LEA_SUPPORT
    else if (app_lea_mgr_is_upstreaming() && event == BP_TPOLL_LEA_SUSPEND_A2DP_EVENT)
    {
        app_bt_policy_set_active_tpoll_only(app_idx, BP_TPOLL_LEA_SUSPEND_A2DP);
    }
#endif
    else if (((app_cfg_const.enable_multi_link) &&
              (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED)) || extend_app_cfg_const.teams_support)
    {
#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        if ((app_hfp_sco_is_connected()) ||
            (app_teams_multilink_check_device_record_sco_stream_exist()))
#else
#if F_APP_GAMING_DONGLE_SUPPORT
        if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE) &&
            (bau_p_link != NULL) &&
            (app_db.remote_is_dongle) &&
            (app_db.dongle_is_enable_mic) &&
            (bau_p_link->id == app_hfp_get_active_idx())) //SPP
        {
            //active tpoll 26 40
            app_bt_policy_set_idle_tpoll(BP_TPOLL_A2DP);

            app_bt_policy_set_active_tpoll_only(app_hfp_get_active_idx(), BP_TPOLL_A2DP);
        }
        else if (app_hfp_sco_is_connected()) //sco
#else
        if (app_hfp_sco_is_connected()) //sco
#endif
#endif
        {
            app_bt_policy_set_idle_tpoll(BP_TPOLL_IDLE);
        }
        else if (app_db.br_link[app_idx].streaming_fg == true)//a2dp
        {
            if (app_db.gaming_mode)
            {
                app_bt_policy_set_idle_tpoll(BP_TPOLL_GAMING_A2DP);

                app_bt_policy_set_active_tpoll_only(app_idx, BP_TPOLL_GAMING_A2DP);
            }
            else
            {
                app_bt_policy_set_idle_tpoll(BP_TPOLL_A2DP);

                app_bt_policy_set_active_tpoll_only(app_idx, BP_TPOLL_A2DP);
            }
        }
#if F_APP_TEAMS_BT_POLICY
        /* if there is anyone link is cfu or vp update running*/
        else if (app_teams_hid_vp_update_is_process_check(&p_teams_link))
        {
            /*set qos of teams update running link to 16 */
            uint8_t index = p_teams_link->id;
            bt_link_qos_set(p_teams_link->bd_addr, BT_QOS_TYPE_GUARANTEED,
                            tpoll_table[BP_TPOLL_TEAMS_UPDATE].tpoll_value);
            app_db.br_link[index].tpoll_status = tpoll_table[BP_TPOLL_TEAMS_UPDATE].state;

            /* set the other link qos to 40*/
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    if ((index != i) && (app_db.br_link[i].tpoll_status != tpoll_table[BP_TPOLL_IDLE].state))
                    {
                        bt_link_qos_set(app_db.br_link[i].bd_addr, BT_QOS_TYPE_GUARANTEED,
                                        (tpoll_table[BP_TPOLL_IDLE].tpoll_value + (i * 2)));
                        app_db.br_link[i].tpoll_status = tpoll_table[BP_TPOLL_IDLE].state;
                    }
                }
            }
        }
#endif
#if (F_APP_CFU_FEATURE_SUPPORT &&(F_APP_CFU_HID_SUPPORT || F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT))
        else if (app_cfu_is_process_check(&p_teams_link))
        {
            /*set qos of teams update running link to 16 */
            uint8_t index = p_teams_link->id;
            bt_link_qos_set(p_teams_link->bd_addr, BT_QOS_TYPE_GUARANTEED,
                            tpoll_table[BP_TPOLL_TEAMS_UPDATE].tpoll_value);
            app_db.br_link[index].tpoll_status = tpoll_table[BP_TPOLL_TEAMS_UPDATE].state;

            /* set the other link qos to 40*/
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    if ((index != i) && (app_db.br_link[i].tpoll_status != tpoll_table[BP_TPOLL_IDLE].state))
                    {
                        bt_link_qos_set(app_db.br_link[i].bd_addr, BT_QOS_TYPE_GUARANTEED,
                                        (tpoll_table[BP_TPOLL_IDLE].tpoll_value + (i * 2)));
                        app_db.br_link[i].tpoll_status = tpoll_table[BP_TPOLL_IDLE].state;
                    }
                }
            }
        }
#endif
        else //idle
        {
            app_bt_policy_set_idle_tpoll(BP_TPOLL_IDLE);
        }
    }
    else
    {
        if (app_bt_policy_get_enlarge_tpoll_state() == true)
        {
            app_bt_policy_set_active_tpoll_only(app_idx, BP_TPOLL_IDLE_SINGLE_LINKBACK);
        }
        else
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE
                && (app_db.br_link[app_idx].streaming_fg == true))
            {
                // only for stereo
                if (app_db.gaming_mode)
                {
                    app_bt_policy_set_active_tpoll_only(app_idx, BP_TPOLL_GAMING_A2DP);
                }
                else
                {
                    app_bt_policy_set_active_tpoll_only(app_idx, BP_TPOLL_A2DP);
                }
            }
            else
            {
                app_bt_policy_set_idle_tpoll(BP_TPOLL_IDLE);
            }
        }
    }

    APP_PRINT_TRACE8("app_bt_policy_qos_param_update: event %u, sco %d, a2dp_idx %d, avrcp %d, stream %d, active_tpoll %d, inactive_idx %d, bau_idx %d",
                     event,
                     app_hfp_sco_is_connected(),
                     app_idx,
                     app_db.br_link[app_idx].avrcp_play_status,
                     app_db.br_link[app_idx].streaming_fg,
                     app_db.br_link[app_idx].tpoll_status,
                     app_a2dp_get_inactive_idx(),
                     find_bau_app_idx);
#if F_APP_GAMING_DONGLE_SUPPORT
    APP_PRINT_TRACE2("app_bt_policy_qos_param_update: enable_mic %d, gaming %d",
                     app_db.dongle_is_enable_mic,
                     app_db.gaming_mode);
#endif
}

uint8_t *app_bt_policy_get_linkback_device(void)
{
    if (linkback_active_node.is_valid)
    {
        return linkback_active_node.linkback_node.bd_addr;
    }
    else
    {
        return NULL;
    }
}

#if F_APP_GAMING_DONGLE_SUPPORT
void app_bt_policy_msg_gaming_dongle_streaming(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE && app_cfg_const.enable_multi_link)
    {
        T_APP_BR_LINK *p_dongle_link = app_dongle_get_connected_dongle_link();

        if (p_dongle_link != NULL)
        {
            T_BT_PARAM bt_param;

            memset(&bt_param, 0, sizeof(T_BT_PARAM));
            bt_param.not_check_addr_flag = true;

            if (app_db.a2dp_play_status == true && app_db.gaming_mode)
            {
                app_bt_policy_state_machine(EVENT_ENTER_GAMING_DONGLE_STREAMING, &bt_param);
            }
            else
            {
                app_bt_policy_state_machine(EVENT_EXIT_GAMING_DONGLE_STREAMING, &bt_param);
            }
        }
    }
}
#endif
