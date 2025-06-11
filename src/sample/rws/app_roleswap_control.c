/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if (F_APP_ERWS_SUPPORT == 1)
#include <string.h>
#include "sysm.h"
#include "trace.h"
#include "wdg.h"
#include "remote.h"
#include "audio_track.h"
#include "app_adp_cmd.h"
#include "app_timer.h"
#include "app_audio_policy.h"
#include "app_loc_mgr.h"
#include "app_ipc.h"
#include "app_roleswap_control.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_roleswap.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_multilink.h"
#include "app_ota.h"
#include "app_anc.h"
#include "app_ble_gap.h"
#include "app_mmi.h"
#include "app_ble_device.h"
#include "app_ble_common_adv.h"
#include "app_qol.h"
#include "app_key_process.h"

#if F_APP_LEA_SUPPORT
#include "app_lea_adv.h"
#endif

#if (F_APP_AIRPLANE_SUPPORT == 1)
#include "app_airplane.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_CFU_FEATURE_SUPPORT
#include "app_common_cfu.h"
#endif

#include "app_sensor.h"

#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_ota.h"
#endif

typedef struct
{
    uint8_t case_closed_power_off : 1;
    uint8_t in_case_power_off : 1;
    uint8_t single_power_off : 1; /* single power off by key, cmd set .... */
    uint8_t wait_profile_connected : 1;
    uint8_t wait_a2dp_stream_chann : 1;
    uint8_t mmi_trigger_roleswap : 1;
    uint8_t rsv : 2;
} T_ROLESWAP_SYS_STATUS;

typedef struct
{
    uint8_t disc_ble : 1;
    uint8_t disc_inactive_link : 1; /* multilink */
    uint8_t disc_inactive_link_ing : 1;
    uint8_t roleswap_is_going_to_do : 1;
    uint8_t lea_2nd_roleswap : 1;
    uint8_t rsv : 3;
} T_ROLESWAP_TRIGGER_STATUS;

typedef enum
{
    ROLESWAP_TRIGGER_REASON_NONE = 0,                   //0x00
    ROLESWAP_TRIGGER_REASON_IN_NON_SMART_CASE,          //0x01
    ROLESWAP_TRIGGER_REASON_CALL_IN_SMART_CASE,         //0x02
    ROLESWAP_TRIGGER_REASON_CALL_OUT_EAR,               //0x03
    ROLESWAP_TRIGGER_REASON_RSSI,                       //0x04
    ROLESWAP_TRIGGER_REASON_BATTERY,                    //0x05
    ROLESWAP_TRIGGER_REASON_SINGLE_BUD_TO_POWER_OFF,    //0x06
    ROLESWAP_TRIGGER_REASON_CLOSE_CASE_POWER_OFF,       //0x07
    ROLESWAP_TRIGGER_REASON_DURIAN_FIX_MIC,             //0x08
    ROLESWAP_TRIGGER_REASON_MMI_TRIGGER,                //0x09
    ROLESWAP_TRIGGER_REASON_IN_CASE_TIMEOUT_POWER_OFF,  //0x0A
    ROLESWAP_TRIGGER_REASON_LEA_2ND_ROLESWAP            //0x0B
} T_APP_ROLESWAP_TRIGGER_REASON;

typedef enum
{
    ROLESWAP_REJECT_REASON_NONE = 0,                    //0x00
    ROLESWAP_REJECT_REASON_BASIC_CHECK,                 //0x01
    ROLESWAP_REJECT_REASON_ONGOING,                     //0x02
    ROLESWAP_REJECT_REASON_BLE_EXIST,                   //0x03
    ROLESWAP_REJECT_REASON_ACL_ROLE_SLAVE,              //0x04
    ROLESWAP_REJECT_REASON_OTA,                         //0x05
    ROLESWAP_REJECT_REASON_TEAMS_CFU,                   //0x06
    ROLESWAP_REJECT_REASON_ANC_HANDLING,                //0x07
    ROLESWAP_REJECT_REASON_SNIFFING_STATE,              //0x08
    ROLESWAP_REJECT_REASON_PROFILE_NOT_CONN,            //0x09
    ROLESWAP_REJECT_REASON_PROFILE_CONNECTING,          //0x0A
    ROLESWAP_REJECT_REASON_REMOTE_IN_BOX,               //0x0B
    ROLESWAP_REJECT_REASON_CALL_REMOTE_IN_BOX,          //0x0C
    ROLESWAP_REJECT_REASON_CALL_REMOTE_OUT_EAR,         //0x0D
    ROLESWAP_REJECT_REASON_MEDIA_BUFFER_NOT_ENOUGH,     //0x0E
    ROLESWAP_REJECT_REASON_MULTILINK_EXIST,             //0x0F
    ROLESWAP_REJECT_REASON_DISC_BLE_OR_INACTIVE_LINK,   //0x10
    ROLESWAP_REJECT_REASON_DURIAN_FIX_MIC_VIOLATION,    //0x11
    ROLESWAP_REJECT_REASON_CHECK_ABS_VOL,               //0x12
    ROLESWAP_REJECT_REASON_A2DP_STREAM_CHANN_CONNECTING,//0x13
    ROLESWAP_REJECT_REASON_MEMS_START,                  //0x14
    ROLESWAP_REJECT_REASON_AIRPLANE_MODE,               //0x15
    ROLESWAP_REJECT_REASON_CALL_COMING,                 //0x16
    ROLESWAP_REJECT_REASON_STOP_LINKBACK_ING,           //0x17
    ROLESWAP_REJECT_REASON_LEA_REMOTE_NOT_CONNECTED     //0x18
} T_APP_ROLESWAP_REJECT_REASON;

typedef enum
{
    APP_TIMER_ROLESWAP_RETRY,
    APP_TIMER_ROLESWAP_HANDLE_EXCEPTION,
    APP_TIMER_ROLESWAP_WAIT_PROFILE_CONNECTED,
    APP_TIMER_ROLESWAP_POWER_OFF_PROTECT,
    APP_TIMER_ROLESWAP_STOP_LINKBACK,
} T_APP_ROLESWAP_CTRL_TIMER;

typedef enum
{
    APP_REMOTE_MSG_ROLESWAP_CTRL_TERMINATED,
    APP_REMOTE_MSG_ROLESWAP_CTRL_RESET_ORI_BUD_ROLE,
    APP_REMOTE_MSG_ROLESWAP_CTRL_TOTAL,
} T_ROLESWAP_CTRL_REMOTE_MSG;

typedef enum
{
    ROLESWAP_TRIGGER_LEVEL_HIGH,    /* high priority: need to power off after roleswap */
    ROLESWAP_TRIGGER_LEVEL_MEDIUM,  /* medium priority: to make the mic correct in call */
    ROLESWAP_TRIGGER_LEVEL_LOW,     /* low priority: rssi, bat roleswap etc.... */
} T_APP_ROLESWAP_TRIGGER_LEVEL;

static T_ROLESWAP_SYS_STATUS roleswap_sys_status;
static T_ROLESWAP_TRIGGER_STATUS roleswap_trigger_status;
static T_APP_ROLESWAP_STATUS roleswap_status;
static uint8_t app_roleswap_ctrl_timer_id = 0;
static uint8_t timer_idx_roleswap_retry = 0;
static uint8_t timer_idx_roleswap_handle_exception = 0;
static uint8_t timer_idx_roleswap_wait_profile_connected = 0;
static uint8_t timer_idx_roleswap_power_off_protect = 0;
static uint8_t timer_idx_roleswap_stop_linkback = 0;

static void app_roleswap_ctrl_set_status(T_APP_ROLESWAP_STATUS status, uint16_t line)
{
    APP_PRINT_TRACE2("app_roleswap_ctrl_set_status: status %d line %d", status, line);

    if (status == APP_ROLESWAP_STATUS_BUSY &&
        timer_idx_roleswap_handle_exception == 0)
    {
        app_start_timer(&timer_idx_roleswap_handle_exception, "roleswap_handle_exception",
                        app_roleswap_ctrl_timer_id, APP_TIMER_ROLESWAP_HANDLE_EXCEPTION, 0, false,
                        5000);
    }
    else if (status == APP_ROLESWAP_STATUS_IDLE)
    {
        app_stop_timer(&timer_idx_roleswap_handle_exception);
    }

    roleswap_status = status;
    app_bt_policy_update_cpu_freq(BP_CPU_FREQ_EVENT_ROLESWAP);

}

static void app_roleswap_ctrl_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                          T_REMOTE_RELAY_STATUS status)
{
    bool handle = false;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_ROLESWAP_CTRL_TERMINATED:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_roleswap_ctrl_set_status(APP_ROLESWAP_STATUS_IDLE, __LINE__);
            }

            handle = true;
        }
        break;

    case APP_REMOTE_MSG_ROLESWAP_CTRL_RESET_ORI_BUD_ROLE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_cfg_nv.ori_bud_role = buf[0];
            }

            handle = true;
        }
        break;

    default:
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE2("app_roleswap_ctrl_parse_cback: msg_type 0x%02X status 0x%02X", msg_type, status);
    }

}

T_APP_ROLESWAP_STATUS app_roleswap_ctrl_get_status(void)
{
    return roleswap_status;
}

static void app_roleswap_ctrl_exec_roleswap(void)
{
    APP_PRINT_TRACE0("app_roleswap_ctrl_exec_roleswap");

    if (app_bt_sniffing_roleswap(false))
    {
        if (app_db.connected_num_before_roleswap > app_link_get_b2s_link_num())
        {
            app_bt_policy_stop_reconnect_timer();
        }

        app_roleswap_ctrl_set_status(APP_ROLESWAP_STATUS_BUSY, __LINE__);
    }
    else
    {
        app_start_timer(&timer_idx_roleswap_retry, "roleswap_retry",
                        app_roleswap_ctrl_timer_id, APP_TIMER_ROLESWAP_RETRY, 0, false,
                        300);
    }
}

static void app_roleswap_ctrl_reconnect_inactive_link(void)
{
    //If no profile connected when disconnect inactive link, connected tone is need.
    app_db.disallow_connected_tone_after_inactive_connected = (app_db.connected_tone_need == false) ?
                                                              true : false;

    app_multi_reconnect_inactive_link();
    app_audio_set_connected_tone_need(false);
}


static void app_roleswap_ctrl_cancel_restore_link(void)
{
    APP_PRINT_TRACE2("app_roleswap_ctrl_cancel_restore_link: restore_ble %d restore_multilink %d",
                     roleswap_trigger_status.disc_ble, roleswap_trigger_status.disc_inactive_link);

    /* restore ble or multilink if need */
    if (roleswap_trigger_status.disc_ble)
    {
        roleswap_trigger_status.disc_ble = 0;

        T_BT_EVENT_PARAM_REMOTE_ROLESWAP_STATUS remote_roleswap_status;

        /* force restore ble */
        remote_roleswap_status.device_role = REMOTE_SESSION_ROLE_PRIMARY;
        remote_roleswap_status.status = BT_ROLESWAP_STATUS_FAIL;
        app_ble_device_handle_role_swap(&remote_roleswap_status);

        /* ble_common_adv_after_roleswap is set false in app_ble_device_handle_role_swap */
        app_ble_common_adv_sync_start_flag();
    }

    if (roleswap_trigger_status.disc_inactive_link)
    {
        roleswap_trigger_status.disc_inactive_link = 0;

        if (app_db.connected_num_before_roleswap > app_link_get_b2s_link_num())
        {
            app_roleswap_ctrl_reconnect_inactive_link();
        }
        else if (app_db.connected_num_before_roleswap == app_link_get_b2s_link_num())
        {
            roleswap_trigger_status.disc_inactive_link_ing = 1;
        }
    }
}

#if (F_APP_QOL_MONITOR_SUPPORT == 1)
static bool app_roleswap_ctrl_media_buffer_enough(void)
{
    T_APP_BR_LINK *p_link = NULL;
    bool ret = true;

    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_link && p_link->a2dp_track_handle)
    {
        audio_track_buffer_level_get(p_link->a2dp_track_handle, &app_db.buffer_level_local);

        if ((app_db.buffer_level_remote < BUFFER_LEVEL_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP) ||
            (app_db.buffer_level_local < BUFFER_LEVEL_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP))
        {
            APP_PRINT_TRACE2("app_roleswap_ctrl_media_buffer_enough: local %d remote %d",
                             app_db.buffer_level_local, app_db.buffer_level_remote);

            ret = false;
        }
    }

    return ret;
}
#endif

/* trigger poweroff after roleswap */
static bool app_roleswap_ctrl_poweroff_triggered(void)
{
    bool ret = false;

    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        if (roleswap_sys_status.in_case_power_off || roleswap_sys_status.single_power_off ||
            roleswap_sys_status.case_closed_power_off)
        {
            ret = true;
        }
    }

    return ret;
}

static void app_roleswap_ctrl_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = false;

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_CMPL:
    case BT_EVENT_A2DP_CONN_CMPL:
    case BT_EVENT_AVRCP_CONN_CMPL:
    /* disallow roleswap immediately before seamless join finished */
    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            handle = true;

            if (app_link_get_b2s_link_num() != 0) //No need to check profile if no ACL connected
            {
                roleswap_sys_status.wait_profile_connected = true;
                app_start_timer(&timer_idx_roleswap_wait_profile_connected, "roleswap_wait_profile_connected",
                                app_roleswap_ctrl_timer_id, APP_TIMER_ROLESWAP_WAIT_PROFILE_CONNECTED, 0, false,
                                3500);
            }

            if (event_type == BT_EVENT_A2DP_CONN_CMPL)
            {
                roleswap_sys_status.wait_a2dp_stream_chann = true;
            }

#if F_APP_LEA_SUPPORT
            if (event_type == BT_EVENT_REMOTE_CONN_CMPL)
            {
                if (app_db.ever_roleswapped == 0)
                {
                    app_cfg_nv.ori_bud_role = app_cfg_nv.bud_role;
                }

                if (roleswap_trigger_status.lea_2nd_roleswap == 0)
                {
                    //LEA adv is stopped when B2B disconnect, restart when B2B connected
                    app_lea_adv_start();
                }
            }
#endif
        }
        break;

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
            handle = true;
            app_roleswap_ctrl_set_status(APP_ROLESWAP_STATUS_IDLE, __LINE__);

            if (app_roleswap_ctrl_poweroff_triggered())
            {
                /* b2b disconnected during roleswap triggered; poweroff directly */
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            handle = true;
            bool addr_match = !memcmp(param->acl_conn_disconn.bd_addr, app_db.resume_addr, 6);

            APP_PRINT_TRACE5("BT_EVENT_ACL_CONN_DISCONN: %d cause 0x%04X %d (%s %s)", addr_match,
                             param->acl_conn_disconn.cause, roleswap_trigger_status.disc_inactive_link,
                             TRACE_BDADDR(param->acl_conn_disconn.bd_addr), TRACE_BDADDR(app_db.resume_addr));

            if (roleswap_trigger_status.disc_inactive_link)
            {
                if (addr_match &&
                    (param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)  ||
                     param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE) ||
                     param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)  ||
                     param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)))
                {
                    app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_ACL_DISC);
                }
            }
            else if (roleswap_trigger_status.disc_inactive_link_ing)
            {
                roleswap_trigger_status.disc_inactive_link_ing = 0;

                app_roleswap_ctrl_reconnect_inactive_link();
            }
        }
        break;

#if F_APP_LEA_SUPPORT
    case BT_EVENT_ACL_ROLE_MASTER:
        {
            handle = true;

            //Switch back to original bud role if source is LEA device
            if ((app_db.ever_roleswapped == 1) &&
                (app_link_check_b2b_link(param->acl_role_master.bd_addr)))
            {
                if (app_cfg_nv.ori_bud_role != app_cfg_nv.bud_role)
                {
                    roleswap_trigger_status.lea_2nd_roleswap = 1;
                    app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LEA_2ND_ROLESWAP);
                }
            }
        }
        break;
#endif

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            handle = true;

            T_BT_ROLESWAP_STATUS roleswap_status = param->remote_roleswap_status.status;

            APP_PRINT_TRACE4("app_roleswap_ctrl_bt_cback: roleswap status 0x%02X, last_bud_loc_event 0x%02X, ori_bud_role %d, bud_role %d",
                             roleswap_status, app_db.last_bud_loc_event, app_cfg_nv.ori_bud_role, app_cfg_nv.bud_role);

            if (roleswap_status == BT_ROLESWAP_STATUS_SUCCESS)
            {
#if F_APP_LEA_SUPPORT
                if (app_db.is_lea_device_connected == 1)
                {
                    app_db.ever_roleswapped = (app_cfg_nv.ori_bud_role != app_cfg_nv.bud_role) ? 1 : 0;
                }

                if (roleswap_trigger_status.lea_2nd_roleswap == 1) //Original secondary bud
                {
                    uint8_t buf = REMOTE_SESSION_ROLE_PRIMARY;

                    //LEA 2nd roleswap is triggered by new primary bud,
                    //app_cfg_nv.ori_bud_role of original primary bud is set to secondary when BT_EVENT_REMOTE_CONN_CMPL
                    //Reset app_cfg_nv.ori_bud_role of original primary bud to primary
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_ROLESWAP_CTRL,
                                                        APP_REMOTE_MSG_ROLESWAP_CTRL_RESET_ORI_BUD_ROLE, &buf, 1);

                    //LEA adv is stopped when B2B disconnect,
                    //restart after LEA 2nd roleswap to avoid LEA central connect 2 LE ACLs to the same bud
                    app_lea_adv_start();
                }
#endif

                app_roleswap_ctrl_set_status(APP_ROLESWAP_STATUS_IDLE, __LINE__);
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_ROLESWAP_SUCCESS);

                /*check whether to transfer call when roleswap ends*/
#if F_APP_DURIAN_SUPPORT
                app_durian_mmi_call_transfer_check();
#else
                if (app_audio_call_transfer_check())
                {
                    app_mmi_handle_action(MMI_HF_TRANSFER_CALL);
                }
#endif
            }
            else if (roleswap_status == BT_ROLESWAP_STATUS_TERMINATED)
            {
                app_roleswap_ctrl_set_status(APP_ROLESWAP_STATUS_IDLE, __LINE__);
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_ROLESWAP_TERMINATED);

                uint8_t null_data = 0;
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_ROLESWAP_CTRL,
                                                    APP_REMOTE_MSG_ROLESWAP_CTRL_TERMINATED, &null_data, sizeof(null_data));
            }
            else if (roleswap_status == BT_ROLESWAP_STATUS_START_REQ)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    if (app_db.device_state == APP_DEVICE_STATE_ON)
                    {
                        app_roleswap_ctrl_set_status(APP_ROLESWAP_STATUS_BUSY, __LINE__);

                        remote_roleswap_cfm(true, 0);
                    }
                    else
                    {
                        remote_roleswap_cfm(false, 0);
                    }
                }
            }
            else if (roleswap_status == BT_ROLESWAP_STATUS_START_RSP)
            {
                /* role swap is rejected by peer ; direct power off */
                if (!param->remote_roleswap_status.u.start_rsp.accept)
                {
                    app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_REJCT_BY_PEER);
                }
            }
            else if (roleswap_status == BT_ROLESWAP_STATUS_FAIL)
            {
                uint16_t fail_cause = param->remote_roleswap_status.cause;

                if ((fail_cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
                    (fail_cause == (HCI_ERR | HCI_ERR_CONTROLLER_BUSY)))
                {
                    app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_ROLESWAP_FAILED_RETRY);
                }
            }

            if (app_db.pending_mmi_action_in_roleswap)
            {
                /* roleswap fail will reboot */
                if (roleswap_status == BT_ROLESWAP_STATUS_SUCCESS ||
                    roleswap_status == BT_ROLESWAP_STATUS_TERMINATED)
                {
                    app_key_execute_action(app_db.pending_mmi_action_in_roleswap);
                    app_db.pending_mmi_action_in_roleswap = 0;
                }
            }
        }
        break;

    case BT_EVENT_A2DP_STREAM_OPEN_FAIL:
    case BT_EVENT_A2DP_DISCONN_CMPL:
    case BT_EVENT_A2DP_STREAM_OPEN:
        {
            handle = true;

            roleswap_sys_status.wait_a2dp_stream_chann = false;
        }
        break;

    default:
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE1("app_roleswap_ctrl_bt_cback: event 0x%04X", event_type);
    }
}

static void app_roleswap_ctrl_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_INFO2("app_roleswap_ctrl_timeout_cb: timer_evt %d param 0x%02X", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_ROLESWAP_RETRY:
        {
            app_stop_timer(&timer_idx_roleswap_retry);

            app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_RETRY);
        }
        break;

    case APP_TIMER_ROLESWAP_HANDLE_EXCEPTION:
        {
            app_stop_timer(&timer_idx_roleswap_handle_exception);

            chip_reset(RESET_ALL);
        }
        break;

    case APP_TIMER_ROLESWAP_WAIT_PROFILE_CONNECTED:
        {
            app_stop_timer(&timer_idx_roleswap_wait_profile_connected);
            roleswap_sys_status.wait_profile_connected = false;

            app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_ALL_PROFILE_CONNECTED);
        }
        break;

    case APP_TIMER_ROLESWAP_POWER_OFF_PROTECT:
        {
            app_stop_timer(&timer_idx_roleswap_power_off_protect);
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
        }
        break;

    case APP_TIMER_ROLESWAP_STOP_LINKBACK:
        {
            app_stop_timer(&timer_idx_roleswap_stop_linkback);

            app_roleswap_ctrl_exec_roleswap();
        }
        break;

    default:
        break;
    }
}

/* basic condition to do roleswap */
static bool app_roleswap_ctrl_basic_check(void)
{
    bool ret = true;

#if F_APP_LEA_SUPPORT
    if ((app_link_get_b2s_link_num() == 0 && app_link_get_lea_link_num() == 0) ||
#else
    if (app_link_get_b2s_link_num() == 0 ||
#endif
        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED ||
        app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY ||
        app_db.device_state != APP_DEVICE_STATE_ON)
    {
        ret = false;
    }

    return ret;
}

#if (F_APP_POWER_TEST == 0)
static bool app_roleswap_ctrl_battery_check(void)
{
    bool ret = false;

    if ((app_cfg_const.disable_low_bat_role_swap == 0) &&
        (app_db.remote_batt_level > BUD_BATT_BOTH_ROLESWAP_THRESHOLD) &&
        (app_db.local_batt_level < BUD_BATT_BOTH_ROLESWAP_THRESHOLD))
    {
        ret = true;
    }

    APP_PRINT_TRACE3("app_roleswap_ctrl_battery_check: %d (local %d remote %d)", ret,
                     app_db.local_batt_level, app_db.remote_batt_level);

    return ret;
}

#if (F_APP_QOL_MONITOR_SUPPORT == 1)
static bool app_roleswap_ctrl_rssi_check(T_APP_ROLESWAP_CTRL_EVENT event)
{
    bool ret = false;

    if (app_cfg_const.disable_link_monitor_roleswap == 1)
    {
        return ret;
    }

    T_APP_BR_LINK *p_link = NULL;

    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_link && p_link->a2dp_track_handle)
    {
        audio_track_buffer_level_get(p_link->a2dp_track_handle, &app_db.buffer_level_local);

        if ((app_db.rssi_local + app_cfg_const.roleswap_rssi_threshold < app_db.rssi_remote) &&
            (app_db.rssi_remote >= RSSI_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP) &&
            (app_db.rssi_local <= RSSI_MAXIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP) &&
            (app_db.rssi_local >= RSSI_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP))
        {
            if (event == APP_ROLESWAP_CTRL_EVENT_DECODE_EMPTY)
            {
                ret = true;
            }
            else
            {
                /* check buffer level to prevent carton */
                if ((app_db.buffer_level_remote >= BUFFER_LEVEL_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP) &&
                    (app_db.buffer_level_local >= BUFFER_LEVEL_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP))
                {
                    ret = true;
                }
            }
        }
    }

    if (event == APP_ROLESWAP_CTRL_EVENT_BUFFER_LEVEL_CHANGED)
    {
        APP_PRINT_TRACE6("app_roleswap_ctrl_rssi_check: ret %d rssi local %d remote %d threshold %d buffer local %d remote %d",
                         ret, app_db.rssi_local, app_db.rssi_remote, app_cfg_const.roleswap_rssi_threshold,
                         app_db.buffer_level_local, app_db.buffer_level_remote);
    }

    return ret;
}
#endif
#endif

static bool app_roleswap_ctrl_profile_check(void)
{
    uint32_t connected_profile = app_link_conn_profiles();
    bool ret = false;

    if (/* at least one profile connected (a2dp, hfp, spp) with legacy link*/
        ((app_link_get_b2s_link_num() != 0) &&
         ((connected_profile & A2DP_PROFILE_MASK) || (connected_profile & HFP_PROFILE_MASK) ||
          (connected_profile & SPP_PROFILE_MASK)))
#if F_APP_LEA_SUPPORT
        || ((app_link_get_b2s_link_num() == 0) && (app_link_get_lea_link_num() != 0))
#endif
    )
    {
        ret = true;
    }
#if F_APP_LEA_SUPPORT
    else if (mtc_get_btmode() != MULTI_PRO_BT_BREDR)
    {
        ret = true;
    }
#endif

    return ret;
}

static bool app_roleswap_ctrl_call_triggered(void)
{
    bool ret = false;

    if (app_bt_policy_get_call_status() != APP_CALL_IDLE)
    {
        ret = true;
    }
#if F_APP_DURIAN_SUPPORT
    /* for voice assistant */
    else if (app_durian_avp_get_opus_status() == AVP_VIOCE_RECOGNITION_ENCODE_START)
    {
        ret = true;
    }
#endif

    return ret;
}

#if F_APP_DURIAN_SUPPORT
static T_APP_ROLESWAP_REJECT_REASON app_roleswap_ctrl_durian_fix_mic_violation(void)
{
    T_APP_ROLESWAP_REJECT_REASON ret = ROLESWAP_REJECT_REASON_NONE;

    if (app_roleswap_ctrl_call_triggered())
    {
        if (app_durian_audio_get_mic_set() == BT_AVP_MIC_ALLWAYS_LEFT)
        {
            if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
            {
                ret = ROLESWAP_REJECT_REASON_DURIAN_FIX_MIC_VIOLATION;
            }
        }
        else if (app_durian_audio_get_mic_set() == BT_AVP_MIC_ALLWAYS_RIGHT)
        {
            if (app_cfg_const.bud_side == DEVICE_ROLE_RIGHT)
            {
                ret = ROLESWAP_REJECT_REASON_DURIAN_FIX_MIC_VIOLATION;
            }
        }
    }

    return ret;
}

static bool app_roleswap_ctrl_durian_mic_check(void)
{
    bool ret = false;

    if (app_roleswap_ctrl_call_triggered())
    {
        if (app_durian_audio_get_mic_set() == BT_AVP_MIC_ALLWAYS_LEFT)
        {
            if (app_cfg_const.bud_side == DEVICE_ROLE_RIGHT)
            {
                ret = true;
            }
        }
        else if (app_durian_audio_get_mic_set() == BT_AVP_MIC_ALLWAYS_RIGHT)
        {
            if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
            {
                ret = true;
            }
        }
    }

    return ret;
}
#endif

/* in non smart box to power off*/
static bool app_roleswap_ctrl_in_non_smart_box(void)
{
    bool ret = false;

    if (app_cfg_const.enable_rtk_charging_box == false && app_cfg_const.enable_inbox_power_off &&
        app_db.local_loc == BUD_LOC_IN_CASE && app_db.remote_loc != BUD_LOC_IN_CASE)
    {
        ret = true;
    }

    return ret;
}

/* in smart box to power off*/
static bool app_roleswap_ctrl_in_smart_box_timeout(void)
{
    bool ret = false;

    if ((app_adp_cmd_in_case_timeout() == true) && (app_db.remote_loc != BUD_LOC_IN_CASE))
    {
        ret = true;
    }

    return ret;
}

/* in smart box when call */
static bool app_roleswap_ctrl_in_box_when_call(void)
{
    bool ret = false;

    if (app_cfg_const.enable_rtk_charging_box == true &&
        app_roleswap_ctrl_call_triggered() &&
        app_db.local_loc == BUD_LOC_IN_CASE && app_db.remote_loc != BUD_LOC_IN_CASE)
    {
        ret = true;
    }

    return ret;
}

/* out ear when call */
static bool app_roleswap_ctrl_out_ear_when_call(void)
{
    bool ret = false;

    if (LIGHT_SENSOR_ENABLED &&
        app_roleswap_ctrl_call_triggered() &&
        app_db.local_loc != BUD_LOC_IN_EAR && app_db.remote_loc == BUD_LOC_IN_EAR)
    {
        ret = true;
    }

    return ret;
}

/* non smart box remote in box */
static bool app_roleswap_ctrl_remote_in_non_smart_box(void)
{
    bool ret = false;

    if (app_cfg_const.enable_rtk_charging_box == false &&
        app_db.local_loc != BUD_LOC_IN_CASE && app_db.remote_loc == BUD_LOC_IN_CASE)
    {
        ret = true;
    }

    return ret;
}

/* smart box remote in box when call */
static bool app_roleswap_ctrl_call_remote_in_smart_box(void)
{
    bool ret = false;

    if (app_cfg_const.enable_rtk_charging_box == true &&
        app_roleswap_ctrl_call_triggered() &&
        app_db.local_loc != BUD_LOC_IN_CASE && app_db.remote_loc == BUD_LOC_IN_CASE)
    {
        ret = true;
    }

    return ret;
}

/* remote out ear when call */
static bool app_roleswap_ctrl_call_remote_out_ear(void)
{
    bool ret = false;

    if (LIGHT_SENSOR_ENABLED &&
        app_roleswap_ctrl_call_triggered() &&
        app_db.local_loc == BUD_LOC_IN_EAR && app_db.remote_loc != BUD_LOC_IN_EAR)
    {
        ret = true;
    }

    return ret;
}

static T_APP_ROLESWAP_REJECT_REASON app_roleswap_ctrl_1st_stage_reject_check(void)
{
    T_APP_ROLESWAP_REJECT_REASON reject_reason = ROLESWAP_REJECT_REASON_NONE;
    T_APP_BR_LINK *p_b2b_link = app_link_find_br_link(app_cfg_nv.bud_peer_addr);

    if (app_roleswap_ctrl_basic_check() == false)
    {
        reject_reason = ROLESWAP_REJECT_REASON_BASIC_CHECK;
        goto exit;
    }

    if (roleswap_sys_status.wait_profile_connected)
    {
        reject_reason = ROLESWAP_REJECT_REASON_PROFILE_CONNECTING;
        goto exit;
    }

    if (app_roleswap_ctrl_profile_check() == false)
    {
        reject_reason = ROLESWAP_REJECT_REASON_PROFILE_NOT_CONN;
        goto exit;
    }

    if (roleswap_sys_status.wait_a2dp_stream_chann)
    {
        reject_reason = ROLESWAP_REJECT_REASON_A2DP_STREAM_CHANN_CONNECTING;
        goto exit;
    }

    if (roleswap_status != APP_ROLESWAP_STATUS_IDLE)
    {
        reject_reason = ROLESWAP_REJECT_REASON_ONGOING;
        goto exit;
    }

    if (p_b2b_link->acl_link_role == BT_LINK_ROLE_SLAVE)
    {
        reject_reason = ROLESWAP_REJECT_REASON_ACL_ROLE_SLAVE;
        goto exit;
    }

    if (app_bt_sniffing_roleswap_check() == false)
    {
        reject_reason = ROLESWAP_REJECT_REASON_SNIFFING_STATE;
        goto exit;
    }

#if F_APP_LEA_SUPPORT
    if (app_link_get_le_link_num() - app_link_get_lea_link_num() > 0)
#else
    if (app_link_get_le_link_num() != 0)
#endif
    {
        reject_reason = ROLESWAP_REJECT_REASON_BLE_EXIST;
        goto exit;
    }

    if (app_link_get_b2s_link_num() > 1)
    {
        reject_reason = ROLESWAP_REJECT_REASON_MULTILINK_EXIST;
        goto exit;
    }

    if (app_avrcp_abs_vol_check_timer_exist())
    {
        reject_reason = ROLESWAP_REJECT_REASON_CHECK_ABS_VOL;
        goto exit;
    }

#if F_APP_LEA_SUPPORT
    if ((roleswap_trigger_status.lea_2nd_roleswap == 0) &&
        (app_db.is_lea_device_connected == 1) && (app_db.remote_is_lea_device_connected == 0))
    {
        reject_reason = ROLESWAP_REJECT_REASON_LEA_REMOTE_NOT_CONNECTED;
        goto exit;
    }
#endif

exit:
    return reject_reason;
}

static T_APP_ROLESWAP_REJECT_REASON app_roleswap_ctrl_2nd_stage_reject_check(
    T_APP_ROLESWAP_CTRL_EVENT event)
{
    T_APP_ROLESWAP_REJECT_REASON reject_reason = ROLESWAP_REJECT_REASON_NONE;
#if F_APP_ANC_SUPPORT
    T_ANC_FEATURE_MAP feature_map;
    feature_map.d32 = anc_tool_get_feature_map();
#endif

#if (F_APP_QOL_MONITOR_SUPPORT == 1)
    bool media_buffer_enough = app_roleswap_ctrl_media_buffer_enough();
#else
    bool media_buffer_enough = true;
#endif

    if (app_roleswap_ctrl_remote_in_non_smart_box())
    {
        reject_reason = ROLESWAP_REJECT_REASON_REMOTE_IN_BOX;
    }
    else if (app_roleswap_ctrl_call_remote_in_smart_box())
    {
        reject_reason = ROLESWAP_REJECT_REASON_CALL_REMOTE_IN_BOX;
    }
    else if (app_roleswap_ctrl_call_remote_out_ear())
    {
        reject_reason = ROLESWAP_REJECT_REASON_CALL_REMOTE_OUT_EAR;
    }
    else if (app_ota_dfu_is_busy()
#if F_APP_TUYA_SUPPORT
             || tuya_ota_is_busy()
#endif
            )
    {
        reject_reason = ROLESWAP_REJECT_REASON_OTA;
    }
    else if ((app_hfp_get_call_status() == APP_CALL_OUTGOING) ||
             (app_hfp_get_call_status() == APP_CALL_INCOMING))
    {
        /* to prevent sco con happened during roleswap */
        reject_reason = ROLESWAP_REJECT_REASON_CALL_COMING;
    }
#if (F_APP_AIRPLANE_SUPPORT == 1)
    else if (app_airplane_mode_get())
    {
        reject_reason = ROLESWAP_REJECT_REASON_AIRPLANE_MODE;
    }
#endif
#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
    else if (app_link_get_misc_num(APP_LINK_MEMS) > 0)
    {
        reject_reason = ROLESWAP_REJECT_REASON_MEMS_START;
    }
#endif
    else if (timer_idx_roleswap_stop_linkback != 0)
    {
        reject_reason = ROLESWAP_REJECT_REASON_STOP_LINKBACK_ING;
    }
#if F_APP_CFU_FEATURE_SUPPORT
    else if (app_cfu_is_in_process())
    {
        reject_reason = ROLESWAP_REJECT_REASON_TEAMS_CFU;
    }
#endif
#if F_APP_ANC_SUPPORT
    else if (app_anc_ramp_tool_is_busy() || (feature_map.user_mode == DISABLE))
    {
        reject_reason = ROLESWAP_REJECT_REASON_ANC_HANDLING;
    }
#endif
    else if (event != APP_ROLESWAP_CTRL_EVENT_DECODE_EMPTY /* buffer empty */ &&
             media_buffer_enough == false)
    {
        reject_reason = ROLESWAP_REJECT_REASON_MEDIA_BUFFER_NOT_ENOUGH;
    }
#if F_APP_LEA_SUPPORT
    else if (app_link_get_le_link_num() - app_link_get_lea_link_num() > 0)
#else
    else if (app_link_get_le_link_num() != 0)
#endif
    {
        reject_reason = ROLESWAP_REJECT_REASON_BLE_EXIST;
    }
    else if (app_link_get_b2s_link_num() > 1)
    {
        reject_reason = ROLESWAP_REJECT_REASON_MULTILINK_EXIST;
    }

    return reject_reason;
}

static T_APP_ROLESWAP_TRIGGER_LEVEL app_roleswap_ctrl_trigger_level(T_APP_ROLESWAP_TRIGGER_REASON
                                                                    reason)
{
    T_APP_ROLESWAP_TRIGGER_LEVEL level = ROLESWAP_TRIGGER_LEVEL_HIGH;

    /* power off related or mmi force roleswap */
    if (reason == ROLESWAP_TRIGGER_REASON_IN_NON_SMART_CASE ||
        reason == ROLESWAP_TRIGGER_REASON_SINGLE_BUD_TO_POWER_OFF ||
        reason == ROLESWAP_TRIGGER_REASON_CLOSE_CASE_POWER_OFF ||
        reason == ROLESWAP_TRIGGER_REASON_MMI_TRIGGER ||
        reason == ROLESWAP_TRIGGER_REASON_IN_CASE_TIMEOUT_POWER_OFF ||
        reason == ROLESWAP_TRIGGER_REASON_LEA_2ND_ROLESWAP)
    {
        level = ROLESWAP_TRIGGER_LEVEL_HIGH;
    }
    /* call related */
    else if (reason == ROLESWAP_TRIGGER_REASON_CALL_OUT_EAR ||
             reason == ROLESWAP_TRIGGER_REASON_CALL_IN_SMART_CASE)
    {
        level = ROLESWAP_TRIGGER_LEVEL_MEDIUM;
    }
    /* rssi .. */
    else
    {
        level = ROLESWAP_TRIGGER_LEVEL_LOW;
    }

    return level;
}

static T_APP_ROLESWAP_TRIGGER_REASON app_roleswap_ctrl_trigger_check(T_APP_ROLESWAP_CTRL_EVENT
                                                                     event)
{
    T_APP_ROLESWAP_TRIGGER_REASON trigger_reason = ROLESWAP_TRIGGER_REASON_NONE;

    if (app_roleswap_ctrl_basic_check() == false)
    {
        goto exit;
    }

    /* mandatory roleswap start */
    if (roleswap_sys_status.single_power_off)
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_SINGLE_BUD_TO_POWER_OFF;
        goto exit;
    }
    else if (roleswap_sys_status.case_closed_power_off)
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_CLOSE_CASE_POWER_OFF;
        goto exit;
    }

    if (app_roleswap_ctrl_in_non_smart_box())
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_IN_NON_SMART_CASE;
        goto exit;
    }
    else if (app_roleswap_ctrl_in_smart_box_timeout())
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_IN_CASE_TIMEOUT_POWER_OFF;
        goto exit;
    }

    if (roleswap_sys_status.mmi_trigger_roleswap)
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_MMI_TRIGGER;
        goto exit;
    }

    /* below is roleswap for call */
#if F_APP_DURIAN_SUPPORT
    if (app_roleswap_ctrl_durian_mic_check())
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_DURIAN_FIX_MIC;
        goto exit;
    }
#endif

    if (app_roleswap_ctrl_in_box_when_call())
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_CALL_IN_SMART_CASE;
        goto exit;
    }

    if (app_roleswap_ctrl_out_ear_when_call())
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_CALL_OUT_EAR;
        goto exit;
    }

#if F_APP_LEA_SUPPORT
    if (roleswap_trigger_status.lea_2nd_roleswap == 1)
    {
        trigger_reason = ROLESWAP_TRIGGER_REASON_LEA_2ND_ROLESWAP;
        goto exit;
    }
#endif
    /* mandatory roleswap end */

    /*  non mandatory roleswap start */
#if (F_APP_POWER_TEST == 0)
    //disable rssi roleswap and low batt roleswap when test power consumption
    if (app_db.local_batt_level < BUD_BATT_BOTH_ROLESWAP_THRESHOLD ||
        app_db.remote_batt_level < BUD_BATT_BOTH_ROLESWAP_THRESHOLD)
    {
        /* check low voltage swap */
        if ((event == APP_ROLESWAP_CTRL_EVENT_BATTERY) && app_roleswap_ctrl_battery_check())
        {
            /* handle low bat role swap */
            trigger_reason = ROLESWAP_TRIGGER_REASON_BATTERY;
        }
    }
#if (F_APP_QOL_MONITOR_SUPPORT == 1)
    else
    {
        /* handle rssi role swap */
        if (app_roleswap_ctrl_rssi_check(event))
        {
            trigger_reason = ROLESWAP_TRIGGER_REASON_RSSI;
        }
    }
#endif
#endif

exit:
    return trigger_reason;
}

static bool app_roleswap_ctrl_need_poweroff_after_roleswap(void)
{
    uint8_t reason = 0;
    bool ret = false;

    if (roleswap_sys_status.in_case_power_off)
    {
        reason = 1;
    }
    else if (roleswap_sys_status.single_power_off)
    {
        reason = 2;
    }
    else if (roleswap_sys_status.case_closed_power_off)
    {
        reason = 3;
    }

    if (reason != 0)
    {
        APP_PRINT_TRACE1("app_roleswap_ctrl_need_poweroff_after_roleswap: reason %d", reason);

        ret = true;
    }

    return ret;
}

static void app_roleswap_ctrl_update_sys_status(T_APP_ROLESWAP_CTRL_EVENT event)
{
    bool cancel_power_off_protect = false;

    APP_PRINT_TRACE1("app_roleswap_ctrl_update_sys_status: event 0x%02X", event);

    if (event == APP_ROLESWAP_CTRL_EVENT_SINGLE_BUD_TO_POWER_OFF)
    {
        roleswap_sys_status.single_power_off = true;
    }
    else if ((event == APP_ROLESWAP_CTRL_EVENT_LOCAL_IN_CASE &&
              app_cfg_const.enable_rtk_charging_box == false && app_cfg_const.enable_inbox_power_off) ||
             (event == APP_ROLESWAP_CTRL_EVENT_IN_CASE_TIMEOUT_TO_POWER_OFF &&
              app_cfg_const.enable_rtk_charging_box == true &&
              app_cfg_const.do_not_auto_power_off_when_case_not_close == false))
    {
        //non-smart case & smart case timeout
        roleswap_sys_status.in_case_power_off = true;
    }
    else if (event == APP_ROLESWAP_CTRL_EVENT_CLOSE_CASE_POWER_OFF)
    {
        roleswap_sys_status.case_closed_power_off = true;
    }
    else if (event == APP_ROLESWAP_CTRL_EVENT_LOCAL_OUT_CASE ||
             event == APP_ROLESWAP_CTRL_EVENT_OPEN_CASE)
    {
        if (event == APP_ROLESWAP_CTRL_EVENT_LOCAL_OUT_CASE)
        {
            roleswap_sys_status.in_case_power_off = false;
        }

        roleswap_sys_status.case_closed_power_off = false;
        cancel_power_off_protect = true;
    }
    else if (event == APP_ROLESWAP_CTRL_EVENT_MMI_TRIGGER_ROLESWAP)
    {
        if (app_roleswap_ctrl_basic_check())
        {
            roleswap_sys_status.mmi_trigger_roleswap = true;
        }
    }

    if (app_roleswap_ctrl_poweroff_triggered())
    {
        app_start_timer(&timer_idx_roleswap_power_off_protect, "roleswap_power_off_protect",
                        app_roleswap_ctrl_timer_id, APP_TIMER_ROLESWAP_POWER_OFF_PROTECT, 0, false,
                        5000);
    }

    if (cancel_power_off_protect)
    {
        app_stop_timer(&timer_idx_roleswap_power_off_protect);
    }

}

static bool app_roleswap_ctrl_must_wait_condition(T_APP_ROLESWAP_REJECT_REASON reject_reason)
{
    bool ret = false;

    /* Note: this condition must only block for a while */
    if (reject_reason == ROLESWAP_REJECT_REASON_ONGOING ||
        reject_reason == ROLESWAP_REJECT_REASON_ACL_ROLE_SLAVE ||
        reject_reason == ROLESWAP_REJECT_REASON_PROFILE_NOT_CONN ||
        reject_reason == ROLESWAP_REJECT_REASON_PROFILE_CONNECTING ||
        reject_reason == ROLESWAP_REJECT_REASON_SNIFFING_STATE ||
        reject_reason == ROLESWAP_REJECT_REASON_A2DP_STREAM_CHANN_CONNECTING ||
        reject_reason == ROLESWAP_REJECT_REASON_LEA_REMOTE_NOT_CONNECTED)
    {
        ret = true;
    }

    return ret;
}

bool app_roleswap_ctrl_check(T_APP_ROLESWAP_CTRL_EVENT event)
{
    bool disallow_call_roleswap_in_out_ear_case = false;
    T_APP_ROLESWAP_TRIGGER_REASON trigger_reason = ROLESWAP_TRIGGER_REASON_NONE;
    T_APP_ROLESWAP_REJECT_REASON reject_reason = ROLESWAP_REJECT_REASON_NONE;
    bool roleswap_immediately = false;
    bool roleswap_later = false;
    bool roleswap_triggered = false;
    T_APP_ROLESWAP_TRIGGER_LEVEL trigger_level = ROLESWAP_TRIGGER_LEVEL_HIGH;
    bool both_buds_in_case = (app_db.local_loc == BUD_LOC_IN_CASE &&
                              app_db.remote_loc == BUD_LOC_IN_CASE);

    if (event == APP_ROLESWAP_CTRL_EVENT_ROLESWAP_SUCCESS ||
        event == APP_ROLESWAP_CTRL_EVENT_REJCT_BY_PEER ||
        event == APP_ROLESWAP_CTRL_EVENT_ROLESWAP_TERMINATED ||
        event == APP_ROLESWAP_CTRL_EVENT_ROLESWAP_FAILED_RETRY)
    {
        bool need_to_power_off = app_roleswap_ctrl_need_poweroff_after_roleswap();
        bool clear_flags = true;

        if ((event != APP_ROLESWAP_CTRL_EVENT_ROLESWAP_SUCCESS) && (need_to_power_off == false))
        {
            clear_flags = false;
        }

        /* clear pending disc flags */
        if (clear_flags)
        {
            roleswap_trigger_status.disc_ble = 0;
            roleswap_trigger_status.disc_inactive_link = 0;
            roleswap_trigger_status.roleswap_is_going_to_do = 0;
            roleswap_trigger_status.lea_2nd_roleswap = 0;
            roleswap_sys_status.mmi_trigger_roleswap = 0;
        }

        /* handle power off after roleswap or reject by peer */
        if (need_to_power_off)
        {
            app_mmi_handle_action(MMI_DEV_POWER_OFF);
            goto exit;
        }
    }
    else
    {
        app_roleswap_ctrl_update_sys_status(event);
    }

    trigger_reason = app_roleswap_ctrl_trigger_check(event);
    reject_reason = app_roleswap_ctrl_1st_stage_reject_check();
    trigger_level = app_roleswap_ctrl_trigger_level(trigger_reason);

    if (reject_reason == ROLESWAP_REJECT_REASON_NONE)
    {
        if (trigger_level == ROLESWAP_TRIGGER_LEVEL_LOW)
        {
            /* need to check more */
            reject_reason = app_roleswap_ctrl_2nd_stage_reject_check(event);

            if (trigger_reason == ROLESWAP_TRIGGER_REASON_BATTERY)
            {
                if (reject_reason == ROLESWAP_REJECT_REASON_BLE_EXIST ||
                    reject_reason == ROLESWAP_REJECT_REASON_MULTILINK_EXIST)
                {
                    /* low bat roleswap is allowed when multilink or ble connected */
                    reject_reason = ROLESWAP_REJECT_REASON_NONE;
                }
            }

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
            if (trigger_reason != ROLESWAP_TRIGGER_REASON_RSSI)
            {
                if (reject_reason == ROLESWAP_REJECT_REASON_MEMS_START)
                {
                    reject_reason = ROLESWAP_REJECT_REASON_NONE;
                }
            }
#endif
        }
#if F_APP_DURIAN_SUPPORT
        else if (trigger_level == ROLESWAP_TRIGGER_LEVEL_MEDIUM)
        {
            reject_reason = app_roleswap_ctrl_durian_fix_mic_violation();

            if (reject_reason != ROLESWAP_REJECT_REASON_NONE)
            {
                disallow_call_roleswap_in_out_ear_case = true;
            }
        }
#endif
    }

    if (trigger_reason != ROLESWAP_TRIGGER_REASON_NONE && both_buds_in_case == false)
    {
        if (reject_reason == ROLESWAP_REJECT_REASON_NONE ||
            trigger_level == ROLESWAP_TRIGGER_LEVEL_HIGH ||
            (trigger_level == ROLESWAP_TRIGGER_LEVEL_MEDIUM && disallow_call_roleswap_in_out_ear_case == false))
        {
            if (app_roleswap_ctrl_must_wait_condition(reject_reason))
            {
                roleswap_later = true;
            }
            else
            {
                roleswap_immediately = true;
            }
        }
        else
        {
            roleswap_later = true;
        }
    }

    if (roleswap_immediately)
    {
        roleswap_trigger_status.roleswap_is_going_to_do = 1;

        /* handle restart common adv after role swap */
        if ((app_ble_common_adv_get_conn_id() != 0xff) ||
            (app_ble_common_adv_get_state() == BLE_EXT_ADV_MGR_ADV_ENABLED))
        {
            //ble link or ble adv exist before roleswap, need start ble adv after roleswap
            if (app_db.ble_common_adv_after_roleswap == false)
            {
                app_db.ble_common_adv_after_roleswap = true;
                app_ble_common_adv_sync_start_flag();
            }
        }

        bool disc_ble_or_inactive_link = false;

        /* disconnect multilink */
        if (roleswap_trigger_status.disc_inactive_link == 0 &&
            app_link_get_b2s_link_num() > 1)
        {
            if (roleswap_trigger_status.disc_inactive_link == 0)
            {
                app_multi_roleswap_disconnect_inactive_link();

                uint8_t cmd_ptr[7];
                memcpy(&cmd_ptr[0], app_db.resume_addr, 6);
                cmd_ptr[6] = app_db.connected_num_before_roleswap;
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_MULTI_LINK, APP_REMOTE_MSG_RESUME_BT_ADDRESS,
                                                    cmd_ptr, 7);
            }

            roleswap_trigger_status.disc_inactive_link = 1;
            disc_ble_or_inactive_link = true;
        }

        /* disconnect ble */
#if F_APP_LEA_SUPPORT
        if (roleswap_trigger_status.disc_ble == 0 &&
            (app_link_get_le_link_num() - app_link_get_lea_link_num() > 0))
#else
        if (roleswap_trigger_status.disc_ble == 0 && app_link_get_le_link_num() != 0)
#endif
        {
            if (roleswap_trigger_status.disc_ble == 0)
            {
                app_ble_gap_disconnect_all(LE_LOCAL_DISC_CAUSE_ROLESWAP);
            }

            roleswap_trigger_status.disc_ble = 1;
            disc_ble_or_inactive_link = true;
        }

        if (disc_ble_or_inactive_link ||
            /* make sure ble and mutilink disc before roleswap;
               this must check the real condition to prevent priority check issue */
#if F_APP_LEA_SUPPORT
            ((app_link_get_le_link_num() - app_link_get_lea_link_num() > 0) ||
             app_link_get_b2s_link_num() > 1)
#else
            (app_link_get_le_link_num() != 0 || app_link_get_b2s_link_num() > 1)
#endif
           )
        {
            reject_reason = ROLESWAP_REJECT_REASON_DISC_BLE_OR_INACTIVE_LINK;
            goto exit;
        }

        /* roleswap is triggered */
        roleswap_trigger_status.roleswap_is_going_to_do = 0;

        if (roleswap_status == APP_ROLESWAP_STATUS_IDLE)
        {
            if (app_bt_policy_get_state() == BP_STATE_LINKBACK)
            {
                /* cancel linkback before roleswap */
                app_bt_policy_prepare_for_roleswap();

                /* start a timer to exec roleswap after linkback stop */
                app_start_timer(&timer_idx_roleswap_stop_linkback, "roleswap_stop_linkback",
                                app_roleswap_ctrl_timer_id, APP_TIMER_ROLESWAP_STOP_LINKBACK, 0, false,
                                100);

            }
            else
            {
                app_roleswap_ctrl_exec_roleswap();
            }
        }
    }

exit:
    if (roleswap_immediately == false)
    {
        roleswap_trigger_status.roleswap_is_going_to_do = 0;
    }

    if (roleswap_immediately || roleswap_later || roleswap_status != APP_ROLESWAP_STATUS_IDLE)
    {
        roleswap_triggered = true;
    }
    else
    {
        if (roleswap_trigger_status.roleswap_is_going_to_do == 0 &&
            (roleswap_trigger_status.disc_ble || roleswap_trigger_status.disc_inactive_link))
        {
            app_roleswap_ctrl_cancel_restore_link();
        }

        /* restore multilink inactive link */
        if (event == APP_ROLESWAP_CTRL_EVENT_ROLESWAP_SUCCESS && app_db.connected_num_before_roleswap)
        {
            /* only primary */
            app_roleswap_ctrl_reconnect_inactive_link();
        }
    }

    APP_PRINT_TRACE7("app_roleswap_ctrl_check: roleswap_immediately (%d %d) event 0x%02X, level 0x%02X, reason 0x%02X, reject 0x%02X, both_in_case %d",
                     roleswap_immediately, roleswap_later, event, trigger_level, trigger_reason, reject_reason,
                     both_buds_in_case);

    return roleswap_triggered;
}

static void app_roleswap_ctrl_bud_loc_event_cback(uint32_t event, void *msg)
{
    bool from_remote = *((bool *)msg);

    if (from_remote)
    {
        switch (event)
        {
        case EVENT_IN_CASE:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_REMOTE_IN_CASE);
            }
            break;

        case EVENT_OUT_CASE:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_REMOTE_OUT_CASE);
            }
            break;

        case EVENT_IN_EAR:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_REMOTE_IN_EAR);
            }
            break;

        case EVENT_OUT_EAR:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_REMOTE_OUT_EAR);
            }
            break;

#if F_APP_LEA_SUPPORT
        case EVENT_RMEOTE_SYNCED:
            {
                //Switch back to original bud role if source is LEA device
                if (app_db.ever_roleswapped == 1)
                {
                    if (app_cfg_nv.ori_bud_role != app_cfg_nv.bud_role)
                    {
                        roleswap_trigger_status.lea_2nd_roleswap = 1;
                        app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LEA_2ND_ROLESWAP);
                    }
                }
            }
            break;
#endif
        }
    }
    else
    {
        switch (event)
        {
        case EVENT_IN_CASE:
            {
                bool roleswap_is_triggered = app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LOCAL_IN_CASE);

                /* if dut mode in box; it must power off */
                if (app_cfg_const.enable_inbox_power_off && roleswap_is_triggered == false)
                {
                    app_mmi_handle_action(MMI_DEV_POWER_OFF);
                }
            }
            break;

        case EVENT_OUT_CASE:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LOCAL_OUT_CASE);
            }
            break;

        case EVENT_IN_EAR:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LOCAL_IN_EAR);
            }
            break;

        case EVENT_OUT_EAR:
            {
                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_LOCAL_OUT_EAR);
            }
            break;
        }
    }

    APP_PRINT_TRACE2("app_roleswap_ctrl_bud_loc_event_cback 0x%02X, from_remote %d", event,
                     from_remote);
}

void app_roleswap_ctrl_init(void)
{
    app_timer_reg_cb(app_roleswap_ctrl_timeout_cb, &app_roleswap_ctrl_timer_id);
    bt_mgr_cback_register(app_roleswap_ctrl_bt_cback);

    app_relay_cback_register(NULL, app_roleswap_ctrl_parse_cback,
                             APP_MODULE_TYPE_ROLESWAP_CTRL, APP_REMOTE_MSG_ROLESWAP_CTRL_TOTAL);
    app_ipc_subscribe(BUD_LOCATION_IPC_TOPIC, app_roleswap_ctrl_bud_loc_event_cback);
}
#else

#include <stdbool.h>
#include "app_roleswap_control.h"

T_APP_ROLESWAP_STATUS app_roleswap_ctrl_get_status(void)
{
    return APP_ROLESWAP_STATUS_IDLE;
}

bool app_roleswap_ctrl_check(T_APP_ROLESWAP_CTRL_EVENT event)
{
    return false;
}

void app_roleswap_ctrl_init(void)
{
    return;
}

#endif

