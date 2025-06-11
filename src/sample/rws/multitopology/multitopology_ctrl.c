/**
*****************************************************************************************
*     Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      multitopology_ctrl.c
   * @brief     This file handles Multi-Topology controller(MTC) process application routines.
   * @author    mj.mengjie.han
   * @date      2021-10-28
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2021 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
*                              Header Files
*============================================================================*/
#if F_APP_LEA_SUPPORT
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "ascs_mgr.h"
#include "bass_mgr.h"
#include "ble_conn.h"
#include "ble_isoch_def.h"
#include "bt_direct_msg.h"
#include "ccp_client.h"
#include "mcp_client.h"
#include "vcs_mgr.h"
#include "app_lea_ccp.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_lea_adv.h"
#include "app_lea_ascs.h"
#include "app_lea_mcp.h"
#include "app_lea_mgr.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_mmi.h"
#include "app_multilink.h"
#include "app_auto_power_off.h"
#include "app_audio_policy.h"
#include "app_ble_gap.h"
#include "multitopology_ctrl.h"
#include "audio_type.h"
#include "app_bt_policy_api.h"
#include "app_bt_point.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_unicast_audio.h"
#include "app_lea_vol_policy.h"
#include "app_timer.h"

#if F_APP_LEA_REALCAST_SUPPORT
#include "app_lea_realcast.h"
#endif

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
#include "app_dongle_dual_mode.h"
#endif

#if GFPS_SASS_LEA_SUPPORT
#include "app_sass_policy.h"
#endif

#define MTC_MODE_GROUP_BREDR    (MTC_MODE_BREDR_IDLE|MTC_MODE_BREDR_PLAYBACK|MTC_MODE_BREDR_VOICE)
#define MTC_MODE_GROUP_CIS      (MTC_MODE_CIS_IDLE|MTC_MODE_CIS_PLAYBACK|MTC_MODE_CIS_VOICE)
#define MTC_MODE_GROUP_BIS      (MTC_MODE_BIS_IDLE|MTC_MODE_BIS_PLAYBACK)
#define MTC_MODE_GROUP_BIS_SD   (MTC_MODE_CIS_IDLE|MTC_MODE_BIS_PLAYBACK)

#define MTC_MODE_STREAM_TYPE_IDLE       (MTC_MODE_BREDR_IDLE|MTC_MODE_CIS_IDLE|MTC_MODE_BIS_IDLE)
#define MTC_MODE_STREAM_TYPE_PLAYBACK   (MTC_MODE_BREDR_PLAYBACK|MTC_MODE_CIS_PLAYBACK|MTC_MODE_BIS_PLAYBACK)
#define MTC_MODE_STREAM_TYPE_VOICE      (MTC_MODE_BREDR_VOICE|MTC_MODE_CIS_VOICE)

/*============================================================================*
 *                              Constants
 *============================================================================*/
typedef enum
{
    MTC_REMOTE_NO                       = 0x0000,
    MTC_REMOTE_SYNC_BIS_STOP            = 0x0001,
    MTC_REMOTE_SYNC_CIS_STOP            = 0x0002,
    MTC_REMOTE_SYNC_MTC_STATE           = 0x0003,
    MTC_REMOTE_SYNC_STOP_SNIFFING       = 0x0004,
    MTC_REMOTE_SYNC_PRI_INFO            = 0x0005,
    MTC_REMOTE_SYNC_CIS_REL_ASE         = 0x0006,
    MTC_REMOTE_SYNC_LEA_REL_A2DP        = 0x0007,
    MTC_REMOTE_SYNC_BIS_START           = 0x0008,
    MTC_REMOTE_SYNC_LEA_TRACK           = 0x0009,
    MTC_REMOTE_SYNC_LEA_CONTEXT_CHANGE  = 0x000A,
    MTC_REMOTE_SYNC_LEA_CON_UPDATE      = 0x000B,
    MTC_REMOTE_SYNC_CIS_RELEASE_SYNC    = 0x000C,
} T_MTC_REMOTE_MSG;

typedef enum t_mtc_bt_mask
{
    MTC_BT_MASK_NONE = 0x00,
    MTC_BT_MASK_CIS  = 0x01,
    MTC_BT_MASK_BIS  = 0x02,
    MTC_BT_MASK_EDR  = 0x04,
} T_MTC_BT_MASK;

typedef enum t_mtc_cis_ter
{
    MTC_CIS_TER_STOP_ADV = 0x00,
    MTC_CIS_TER_NOT_STOP_ADV = 0x01,

} T_MTC_CIS_TER;

typedef enum
{
    MTC_STREAM_A2DP                = 0x00,
    MTC_STREAM_SCO                 = 0x01,
    MTC_STREAM_RESUME_A2DP         = 0x02,
    MTC_STREAM_RESUME_OUTBAND      = 0x03,
    MTC_STREAM_BIS                 = 0x04,
} T_MTC_STREAM_TYPE;

typedef enum
{
    MTC_LINK_ACT_LEA_PRIO_HIGH     = 0x00,
    MTC_LINK_ACT_LEGACY_PRIO_HIGH  = 0x01,
    MTC_LINK_ACT_LAST_ONE          = 0x02,
    MTC_LINK_ACT_NONE              = 0x03,
    MTC_LINK_ACTIVE_TOTAL,

} T_MTC_LINK_ACT;

typedef enum
{
    MTC_MODE_IDLE                   = 0x00,
    MTC_MODE_BREDR_IDLE             = 0x01,
    MTC_MODE_BREDR_PLAYBACK         = 0x02,
    MTC_MODE_BREDR_VOICE            = 0x04,
    MTC_MODE_CIS_IDLE               = 0x08,
    MTC_MODE_CIS_PLAYBACK           = 0x10,
    MTC_MODE_CIS_VOICE              = 0x20,
    MTC_MODE_BIS_IDLE               = 0x40,
    MTC_MODE_BIS_PLAYBACK           = 0x80,
} T_MTC_ACTIVE_MODE;

typedef enum t_mtc_resume
{
    MULTI_RESUME_NONE,
    MULTI_RESUME_A2DP,
    MULTI_RESUME_CIS,
    MULTI_RESUME_BIS,
} T_MTC_RESUME_MODE;

typedef enum
{
    MTC_TIMER_DELAY_PACS_NOTIFY  = 0x00,
} T_MTC_TIMER;

typedef struct
{
    uint8_t wait_a2dp;
} T_MTC_BREDR_DB;

typedef struct
{
    T_MTC_BUD_STATUS b2b;
    T_MTC_BUD_STATUS b2s;
    T_MTC_AUDIO_MODE pending;
} T_MTC_BUS_STATUS;

typedef struct
{
    uint8_t bt_mode;
    bool mic_state;
} T_MTC_SYNC_INFO;

typedef struct
{
    uint8_t               source_bis_num : 2;
    uint8_t               b2b_conn : 1;
    uint8_t               beeptype : 5;

    uint8_t               mtc_pro_bt_mode;
    uint8_t               mtc_last_bt_mode : 6;
    uint8_t               sniffing_stop : 2;
    T_MTC_BUS_STATUS      bud_linkstatus;
    uint8_t               last_source;
    uint8_t               cur_dev_active_mode;
} T_MTC_DEVICE_DB;

typedef struct t_mtc_cback_item
{
    struct t_mtc_cback_item *p_next;
    P_MTC_IF_CB             cback;
} T_MTC_CBACK_ITEM;

typedef struct
{
    T_OS_QUEUE cback_list;
    T_MTC_BREDR_DB bredr_db;
    T_MTC_DEVICE_DB device;
} T_MTC_DB;

/*============================================================================*
 *                              Variables
 *============================================================================*/
static T_MTC_DB mtc_db =
{
    .bredr_db = {false},
    .device = {
        0, 0, MTC_PRO_BEEP_PROMPTLY, MULTI_PRO_BT_BREDR, MULTI_PRO_BT_BREDR, MULTI_PRO_SNIFI_NOINVO,
        {
            .b2b = MTC_BUD_STATUS_ACTIVE,
            .b2s = MTC_BUD_STATUS_ACTIVE,
            .pending = LE_AUDIO_NO,
        },
        .last_source = MULTI_RESUME_NONE,
        .cur_dev_active_mode = MULTI_PRO_BT_BREDR,
    },
};
//for keep cis voice and sco testing
//Right now, it must be 0
static uint8_t test_bd_addr[6] = {0};
static bool  mediator_allow_sniffing = false;
static bool  mediator_reopen_mic = false;
static bool mediator_allow_a2dp_preempt_mcp = true;
static bool mediator_ignore_hfp = false;
static uint8_t mtc_timer_id = 0;
static uint8_t timer_idx_mtc_delay_pacs_notify = 0;

static void mtc_state_bredr_transit_handler(T_MTC_ACTIVE_MODE state, uint8_t *param);
static void mtc_state_cis_transit_handler(T_MTC_ACTIVE_MODE state, uint8_t *param);
static void mtc_state_bis_transit_handler(T_MTC_ACTIVE_MODE state, uint8_t *param);

#if F_APP_ERWS_SUPPORT
T_REMOTE_RELAY_HANDLE mtc_relay_handle;
void (*(mtc_state_transit_handler[]))(T_MTC_ACTIVE_MODE, uint8_t *) =
{
    mtc_state_bredr_transit_handler,
    mtc_state_cis_transit_handler,
    mtc_state_bis_transit_handler,
};
#endif

T_BT_EVENT_PARAM_A2DP_STREAM_START_IND a2dp_para;//will move to gv
static bool mtc_mgr_dispatch(T_MTC_IF_MSG msg, void *inbuf, void *outbuf);

/*============================================================================*
 *                              Functions
 *============================================================================*/
uint8_t mtc_streaming_exist(void);
/**
 * @brief   Set limitation of bt link .
 *
 * @param[in] num Limitation of link quantity.
 * @retval true Success.
 * @retval false Failed, exceed limitation of bt-module.
 */
void mtc_get_budstatus(T_MTC_BUS_STATUS **status)
{
    *status = &mtc_db.device.bud_linkstatus;
}

void mtc_set_pending(T_MTC_AUDIO_MODE action)
{
    mtc_db.device.bud_linkstatus.pending = action;
}

T_MTC_AUDIO_MODE mtc_lea_get_pending(void)
{
    return mtc_db.device.bud_linkstatus.pending;
}

T_MTC_BUD_STATUS mtc_get_b2d_sniff_status(void)
{
    return mtc_db.device.bud_linkstatus.b2b;
}

T_MTC_BUD_STATUS mtc_get_b2s_sniff_status(void)
{
    return mtc_db.device.bud_linkstatus.b2s;
}

void mtc_check_reopen_mic(void)
{
    if (!mediator_reopen_mic &&
        app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        app_lea_uca_enable_mic();
    }
}

void mtc_set_beep(T_MTC_PRO_BEEP para)
{
    APP_PRINT_TRACE1("mtc_set_beep: para %d",
                     para);
    mtc_db.device.beeptype = para;
}
T_MTC_PRO_BEEP mtc_get_beep(void)
{
    APP_PRINT_TRACE1("mtc_get_beep: para %d",
                     (T_MTC_PRO_BEEP)mtc_db.device.beeptype);
    return (T_MTC_PRO_BEEP)mtc_db.device.beeptype;
}

void mtc_set_btmode(T_MTC_BT_MODE para)
{
    static uint8_t pre_bt_mode = MULTI_PRO_BT_BREDR;

    pre_bt_mode = mtc_db.device.mtc_pro_bt_mode;
    mtc_db.device.mtc_pro_bt_mode = para;
    APP_PRINT_TRACE2("mtc_set_btmode: mode %d -> %d", pre_bt_mode, para);

#if GFPS_SASS_LEA_SUPPORT
    if (pre_bt_mode != para)
    {
        if (extend_app_cfg_const.gfps_sass_support)
        {
            app_gfps_notify_conn_status();
        }
    }
#endif
}

void mtc_set_active_bt_mode(T_MTC_BT_MODE para)
{
    APP_PRINT_TRACE1("mtc_set_active_bt_mode: para %d",
                     para);
    mtc_db.device.cur_dev_active_mode = para;
}

T_MTC_BT_MODE mtc_get_btmode(void)
{
    // APP_PRINT_TRACE1("mtc_get_btmode: mode %d",
    //                  mtc_db.device.mtc_pro_bt_mode);
    return (T_MTC_BT_MODE)mtc_db.device.mtc_pro_bt_mode;
}

T_MTC_SNIFI_STATUS mtc_get_sniffing(void)
{
    return (T_MTC_SNIFI_STATUS)mtc_db.device.sniffing_stop;
}

void  mtc_set_sniffing(T_MTC_SNIFI_STATUS para)
{
    mtc_db.device.sniffing_stop = para;
}

void mtc_set_resume_dev(T_MTC_RESUME_MODE mode)
{
    APP_PRINT_TRACE2("mtc_set_resume_dev: mode %d, %d",
                     mode, app_cfg_const.callend_resume_media);
    if (app_cfg_const.callend_resume_media)
    {
        mtc_db.device.last_source = mode;
    }
}

static bool mtc_check_lea_le_link(void)
{
    uint8_t i;
    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used &&
            app_db.le_link[i].lea_link_state == LEA_LINK_CONNECTED)
        {
            return true;
        }
    }
    return false;
}

uint8_t mtc_get_active_idx(uint8_t *bt_mode)
{
    uint8_t idx = MAX_BR_LINK_NUM > MAX_BLE_LINK_NUM ? MAX_BR_LINK_NUM : MAX_BLE_LINK_NUM;
    T_MTC_BT_MODE active_mode = mtc_get_btmode();

#if F_APP_LEA_SUPPORT
    uint8_t b2s_link_num = app_link_get_b2s_link_num() + app_link_get_le_link_num();
#else
    uint8_t b2s_link_num = app_link_get_b2s_link_num();
#endif

    if (b2s_link_num == 0)
    {
        goto end;
    }

    if (active_mode == MULTI_PRO_BT_BREDR)
    {
        uint8_t active_edr_idx = app_multi_get_active_idx();

        if (active_edr_idx != MAX_BR_LINK_NUM)
        {
            if (app_db.br_link[active_edr_idx].used)
            {
                idx = active_edr_idx;
                if (app_db.br_link[active_edr_idx].connected_profile)
                {
                    goto end;
                }
                else
                {
                    active_mode = MULTI_PRO_BT_CIS;
                }
            }
        }
    }

    if (active_mode == MULTI_PRO_BT_CIS)
    {
        T_APP_LE_LINK *p_le_link = app_lea_uca_get_active_link();
        if (p_le_link)
        {
            idx = p_le_link->id;
            goto end;
        }
        else if (mtc_get_btmode() == MULTI_PRO_BT_BREDR)
        {
            active_mode = MULTI_PRO_BT_BREDR;
        }
    }
    else
    {
        //TODO: which one is the active device? source or assistant
    }

end:
    *bt_mode = active_mode;
    APP_PRINT_INFO3("mtc_get_active_idx: b2s_link_num %d, idx %d, bt_mode %d",
                    b2s_link_num, idx, active_mode);
    return idx;
}

bool mtc_ase_release(void)
{
    T_LEA_ASE_ENTRY *p_ase_entry = NULL;
    uint16_t conn_handle = app_lea_mcp_get_active_conn_handle();

    p_ase_entry = app_lea_ascs_find_ase_entry(LEA_ASE_CONN, conn_handle, NULL);
    if (p_ase_entry != NULL)
    {
        app_lea_uca_link_sm(conn_handle, LEA_PAUSE_LOCAL, NULL);
        return true;
    }

    return false;
}

#if F_APP_ERWS_SUPPORT
void mtc_a2dp_release(void)
{
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        remote_sync_msg_relay(mtc_relay_handle,
                              MTC_REMOTE_SYNC_LEA_REL_A2DP,
                              NULL,
                              0,
                              REMOTE_TIMER_HIGH_PRECISION,
                              0,
                              false);
    }
}
#endif

bool mtc_terminate_cis(T_MTC_CIS_TER option)
{
    if (app_lea_adv_get_state() == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        app_link_disallow_legacy_stream(false);

#if F_APP_LE_AUDIO_DONGLE_DUAL_MODE_UI
        if (app_cfg_const.enable_dongle_dual_mode)
        {
            option = MTC_CIS_TER_NOT_STOP_ADV;
        }
#endif

        if (option == MTC_CIS_TER_NOT_STOP_ADV)
        {
            return true;
        }

#if F_APP_ERWS_SUPPORT
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
        {
            remote_sync_msg_relay(mtc_relay_handle,
                                  MTC_REMOTE_SYNC_CIS_STOP,
                                  NULL,
                                  0,
                                  REMOTE_TIMER_HIGH_PRECISION,
                                  0,
                                  false);
        }
        else
#endif
        {
            app_lea_adv_stop();
            mtc_topology_dm(MTC_TOPO_EVENT_LEA_ADV_STOP);
        }
        return true;
    }
    if (mtc_check_lea_le_link())
    {
        return true;
    }
    return false;
}

static void mtc_terminate_bis(void)
{
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        T_MTC_PRO_BEEP para = MTC_PRO_BEEP_NONE;
        remote_sync_msg_relay(mtc_relay_handle,
                              MTC_REMOTE_SYNC_BIS_STOP,
                              &para,
                              1,
                              REMOTE_TIMER_HIGH_PRECISION,
                              0,
                              false);
    }
    else
#endif
    {
        mtc_set_beep(MTC_PRO_BEEP_NONE);
        app_lea_mgr_mmi_handle(MMI_BIG_STOP);
        mtc_mgr_dispatch(ML_FM_MTC_LEA_STOP, NULL, NULL);
    }
}

void mtc_resume_bis(uint8_t para)
{
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_link_disallow_legacy_stream(true);
        remote_sync_msg_relay(mtc_relay_handle,
                              MTC_REMOTE_SYNC_BIS_START,
                              &para,
                              1,
                              REMOTE_TIMER_HIGH_PRECISION,
                              2000,
                              false);
    }
    else
#endif
    {
        app_lea_mgr_mmi_handle(MMI_BIG_START);
        mtc_set_resume_dev(MULTI_RESUME_NONE);
    }
}

void mtc_check_lea_track(void)
{
    if (mtc_is_lea_cis_stream(NULL))
    {
        mtc_mgr_dispatch((T_MTC_IF_MSG)LCIS_FM_MTC_LEA_TRACK, NULL, NULL);
    }
}

void mtc_resume_cis_track(bool resume)
{
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        remote_sync_msg_relay(mtc_relay_handle,
                              MTC_REMOTE_SYNC_LEA_TRACK,
                              &resume,
                              1,
                              REMOTE_TIMER_HIGH_PRECISION,
                              200,
                              false);
    }
    else
#endif
    {
        if (!resume)
        {
            mtc_check_lea_track();
        }

        mtc_set_resume_dev(MULTI_RESUME_NONE);
    }
}

bool mtc_term_sd_sniffing(void)
{
#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        T_MTC_PRO_BEEP para = MTC_PRO_BEEP_NONE;
        remote_sync_msg_relay(mtc_relay_handle,
                              MTC_REMOTE_SYNC_STOP_SNIFFING,
                              &para,
                              1,
                              REMOTE_TIMER_HIGH_PRECISION,
                              0,
                              false);
    }
    else
#endif
    {
        mtc_stream_switch(false);
    }
    return true;
}

bool mtc_check_call_sate(void)
{
    if (app_hfp_get_call_status() != APP_CALL_IDLE ||
        app_lea_ccp_get_call_status() != APP_CALL_IDLE)
    {
        return true;
    }

    return false;
}

#if F_APP_ERWS_SUPPORT
void mtc_sync_info(T_MTC_SYNC_INFO *para)
{
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        remote_async_msg_relay(mtc_relay_handle, MTC_REMOTE_SYNC_PRI_INFO,
                               para, sizeof(T_MTC_SYNC_INFO), false);
    }
}
#endif

void mtc_align_pri_info(void)
{
#if F_APP_ERWS_SUPPORT
    T_MTC_SYNC_INFO para;

    para.bt_mode = mtc_db.device.mtc_pro_bt_mode;
    para.mic_state = app_lea_uca_get_mic_mute_state();
    mtc_sync_info(&para);
#endif
}

void mtc_shutdown(void)
{
    if (mtc_get_btmode()  == MULTI_PRO_BT_BIS)
    {
        app_lea_bca_bmr_terminate();
    }

    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used &&
            app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED)
        {
            app_lea_uca_link_sm(app_db.le_link[i].conn_handle, LEA_DISCON_LOCAL, NULL);
        }
    }

    app_lea_adv_stop();

    mtc_set_btmode(MULTI_PRO_BT_BREDR);
    app_auto_power_off_enable((AUTO_POWER_OFF_MASK_BLE_AUDIO | AUTO_POWER_OFF_MASK_BLE_LINK_EXIST),
                              app_cfg_const.timer_auto_power_off);
}

bool mtc_check_sniff(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    bool result = false;
    T_AUDIO_TRACK_STATE state;

    p_link = app_link_find_br_link(bd_addr);

    if (p_link == NULL)
    {
        mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_NONE;
        result = true;
        goto END_CHECK;
    }

    APP_PRINT_TRACE1("mtc_check_sniff bt_sniffing_state: %d",
                     p_link->bt_sniffing_state);

    audio_track_state_get(p_link->a2dp_track_handle, &state);
    if (state == AUDIO_TRACK_STATE_CREATED ||
        state == AUDIO_TRACK_STATE_STARTED ||
        state == AUDIO_TRACK_STATE_RESTARTED)
    {
        mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_NONE;
        result = true;
        goto END_CHECK;
    }

#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (p_link->bt_sniffing_state == APP_BT_SNIFFING_STATE_STARTING ||
         p_link->bt_sniffing_state == APP_BT_SNIFFING_STATE_SNIFFING))
    {
        mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_NONE;
        app_link_disallow_legacy_stream(false);
        result = true;
    }
#endif

END_CHECK:
    return result;
}

bool mtc_stream_pause(bool param_update)
{
    T_APP_BR_LINK *p_link = NULL;
    bool stop_a2dp = false;
#if (F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST == 0)
    bool pause = true;
#endif

    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_link->used == false)
    {
        return stop_a2dp;
    }
    APP_PRINT_TRACE6("mtc_stream_pause: app_idx %d, p_link->id=%d, %d, %d, %d, bd_addr %s",
                     app_a2dp_get_active_idx(), p_link->id, p_link->avrcp_play_status, app_db.remote_session_state,
                     app_cfg_nv.bud_role, TRACE_BDADDR(p_link->bd_addr));

#if F_APP_GAMING_DONGLE_SUPPORT && (F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST == 0)
    if (app_link_check_dongle_link(p_link->bd_addr))
    {
        pause = false;
    }
#endif

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            if (param_update)
            {
                app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_ISO_SUSPEND);
                mtc_db.device.sniffing_stop = MULTI_PRO_SNIFI_INVO;
            }
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            // don't pause BT stream, only release track and stop sniffing
#else
            if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                if (pause)
                {
                    bt_avrcp_pause(p_link->bd_addr);
                    p_link->avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                }
            }
            app_link_disallow_legacy_stream(true);
            mtc_a2dp_release();
            mtc_mgr_dispatch(AP_FM_MTC_STOP_A2DP, p_link->bd_addr, NULL);
#endif
            stop_a2dp = true;
        }
    }
    else
#endif
    {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
        // don't pause BT stream, only release track and stop sniffing
#else
        if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
        {
            if (pause)
            {
                bt_avrcp_pause(p_link->bd_addr);
                p_link->avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
            }
            stop_a2dp = true;
        }
        mtc_mgr_dispatch(AP_FM_MTC_STOP_A2DP, p_link->bd_addr, NULL);
#endif
    }
    return stop_a2dp;
}

bool mtc_stream_switch(bool iscall)
{
    T_APP_BR_LINK *p_link = NULL;
    bool stop_a2dp = false;

    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_link->used == false)
    {
        return stop_a2dp;
    }

    APP_PRINT_TRACE7("mtc_stream_switch: app_idx %d, p_link->id=%d, %d, %d, %d, bd_addr %s, source %d",
                     app_a2dp_get_active_idx(), p_link->id, p_link->streaming_fg, app_db.remote_session_state,
                     app_cfg_nv.bud_role, TRACE_BDADDR(p_link->bd_addr), app_cfg_nv.allowed_source);

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
        {
            return stop_a2dp;
        }
    }
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    // switch to 2.4G, disallow a2dp
#else
    if ((p_link->streaming_fg || p_link->pending_ind_confirm) &&
        p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
#endif
    {
        stop_a2dp = mtc_stream_pause(true);
#if 1
//Wait for check
        if (iscall && stop_a2dp && app_cfg_const.callend_resume_media)
        {
            mtc_set_resume_dev(MULTI_RESUME_A2DP);
        }
#endif
    }
    return stop_a2dp;
}

bool mtc_ccp_check(void)
{
    if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
    {
        mtc_stream_pause(false);
        return true;
    }
    else
    {
        return false;
    }
}

void mtc_cis_audio_conext_change(bool enable)
{
    mtc_mgr_dispatch(LCIS_FM_MTC_LEA_PACS_CHANGE, &enable, NULL);
}

static void mtc_cis_pacs_enable(bool para)
{
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            remote_async_msg_relay(mtc_relay_handle,
                                   MTC_REMOTE_SYNC_LEA_CONTEXT_CHANGE,
                                   &para,
                                   1,
                                   false);
        }
    }
    else
#endif
    {
        mtc_cis_audio_conext_change(para);
    }
}

static void mtc_request_le_conn_update(void)
{
    uint8_t        i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used &&
            app_db.le_link[i].lea_link_state >= LEA_LINK_CONNECTED)
        {
            ble_set_prefer_conn_param(app_db.le_link[i].conn_handle, 0x1f, 0x20, 0, 500);
        }
    }
}

static void mtc_cis_ase_release(T_MTC_STREAM_TYPE type)
{
    if (app_lea_ccp_get_call_status() != APP_CALL_IDLE && type == MTC_STREAM_SCO)
    {
        return;
    }
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
    if (app_cfg_const.enable_24g_bt_audio_source_switch)
    {
        if ((app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT) && (type == MTC_STREAM_A2DP))
        {
            return;
        }
    }
#endif

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    if (type == MTC_STREAM_A2DP)
    {
        // don't release ase when BT a2dp start, because dongle priority higher than BT
        return;
    }
#endif

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            uint8_t para = type;

            if (type == MTC_STREAM_A2DP || type == MTC_STREAM_RESUME_A2DP)
            {
                remote_sync_msg_relay(mtc_relay_handle,
                                      MTC_REMOTE_SYNC_CIS_RELEASE_SYNC,
                                      &para,
                                      1,
                                      REMOTE_TIMER_HIGH_PRECISION,
                                      0,
                                      false);
            }
            else
            {
                remote_async_msg_relay(mtc_relay_handle,
                                       MTC_REMOTE_SYNC_CIS_REL_ASE,
                                       &para,
                                       1,
                                       false);
            }
        }
    }
    else
#endif
    {
        if (type == MTC_STREAM_A2DP)
        {
            mtc_ase_release();
            mtc_mgr_dispatch(AP_FM_MTC_RESUME_A2DP, &a2dp_para, NULL);
            mtc_cis_audio_conext_change(true);
        }
        else if (type == MTC_STREAM_SCO)
        {
            mtc_cis_audio_conext_change(false);
            mtc_ase_release();
        }
        else if (type == MTC_STREAM_RESUME_A2DP)
        {
            mtc_ase_release();
            mtc_set_btmode(MULTI_PRO_BT_BREDR);
            app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
        }
        else if (type == MTC_STREAM_RESUME_OUTBAND)
        {
            mtc_ase_release();
        }
    }
}

static void mtc_release_lea_stream(void)
{
    T_MTC_BT_MODE mode = mtc_get_btmode();

    if (mode == MULTI_PRO_BT_BIS)
    {
        mtc_set_beep(MTC_PRO_BEEP_NONE);
        app_lea_mgr_mmi_handle(MMI_BIG_STOP);
        mtc_set_resume_dev(MULTI_RESUME_BIS);
        mtc_mgr_dispatch(ML_FM_MTC_LEA_STOP, NULL, NULL);
    }
    else  if ((app_lea_ccp_get_call_status() == APP_CALL_IDLE) &&
              (mode == MULTI_PRO_BT_CIS || mtc_is_lea_cis_stream(NULL)))
    {
        mtc_cis_audio_conext_change(false);
        mtc_ase_release();
    }
}

static void mtc_le_connection_update(bool ckeck_stream)
{
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            remote_async_msg_relay(mtc_relay_handle,
                                   MTC_REMOTE_SYNC_LEA_CON_UPDATE,
                                   &ckeck_stream,
                                   1,
                                   false);
        }
    }
    else
#endif
    {
        if (ckeck_stream)
        {
            mtc_release_lea_stream();
        }
        mtc_request_le_conn_update();
    }
}

void mtc_legacy_update_call_status(uint8_t *call_status_old)
{
    T_MTC_BT_MODE cur_state = mtc_get_btmode();
    T_MTC_RESUME_MODE mode = MULTI_RESUME_NONE;

    if (*call_status_old == APP_CALL_IDLE && app_hfp_get_call_status() != APP_CALL_IDLE)
    {
        app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_HFP_CALL_STATE, NULL);
        if (cur_state == MULTI_PRO_BT_CIS)
        {
            mode = MULTI_RESUME_CIS;
        }
        else if (cur_state == MULTI_PRO_BT_BIS)
        {
            mtc_terminate_bis();
            mode = MULTI_RESUME_BIS;
        }

        if (app_lea_ccp_get_call_status() == APP_CALL_IDLE)
        {
            mtc_set_resume_dev(mode);
            mtc_cis_pacs_enable(false);
        }
    }
    else if (*call_status_old != APP_CALL_IDLE && app_hfp_get_call_status() == APP_CALL_IDLE)
    {
        bool is_stream = mtc_is_lea_cis_stream(NULL);

        mediator_ignore_hfp = false;

        if (is_stream)
        {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            // don't release ase if hfp call end when cis is still stream
#else
            mtc_cis_ase_release(MTC_STREAM_RESUME_OUTBAND);
#endif
        }

        if (!is_stream)
        {
            app_start_timer(&timer_idx_mtc_delay_pacs_notify, "delay_pacs_notify",
                            mtc_timer_id, MTC_TIMER_DELAY_PACS_NOTIFY, 0, true, 2000);
        }
        app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_HFP_CALL_STATE, NULL);
    }

    app_lea_adv_update_interval_hfp();
}

void mtc_terminate_hfp_call(void)
{
    T_APP_CALL_STATUS callstate = app_hfp_get_call_status();

    mediator_ignore_hfp = true;

    if (callstate == APP_VOICE_ACTIVATION_ONGOING)
    {
        app_mmi_hf_cancel_voice_dial();
    }
    else  if (callstate == APP_CALL_INCOMING)
    {
        app_mmi_hf_reject_call();
    }
    else if (callstate == APP_CALL_OUTGOING)
    {
        app_mmi_hf_end_outgoing_call();
    }
    else if (callstate >= APP_CALL_ACTIVE)
    {
        app_mmi_hf_end_active_call();
    }
    else
    {
        mediator_ignore_hfp = false;
    }

    app_start_timer(&timer_idx_mtc_delay_pacs_notify, "delay_pacs_notify",
                    mtc_timer_id, MTC_TIMER_DELAY_PACS_NOTIFY, 0, true, 2000);
}

void mtc_audio_policy_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                            T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE1("mtc_audio_policy_cback: msg_type %d",
                     msg_type);
    switch (msg_type)
    {
    case APP_REMOTE_MSG_SYNC_CALL_STATUS:
        {
            app_lea_adv_update_interval_hfp();

            if (app_hfp_get_call_status() == APP_CALL_IDLE)
            {
                app_lea_ccp_read_all_links_state();
                break;
            }
            if (mtc_get_btmode() == MULTI_PRO_BT_CIS)
            {
                app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_HFP_CALL_STATE, NULL);
            }
        }
        break;
    }
}

void mtc_legacy_link_status(T_BT_EVENT event_type, T_BT_EVENT_PARAM *param)
{
    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        {
#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_success.bd_addr))
            {
                mtc_db.device.bud_linkstatus.b2b = MTC_BUD_STATUS_ACTIVE;
                mtc_set_pending(LE_AUDIO_NO);

                if (mtc_get_btmode() != MULTI_PRO_BT_BREDR && (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
                {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
#else
                    mediator_allow_sniffing = true;
#endif
                }

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY &&
                    app_link_get_lea_link_num() == 0 &&
                    app_lea_bca_state() != LEA_BCA_STATE_STREAMING)
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                }

                app_lea_mgr_dev_ctrl(LEA_DEV_CTRL_SET_IDLE, NULL);
            }
            else
#endif
            {
                mtc_topology_dm(MTC_TOPO_EVENT_LEGACY_CONN);
                mtc_db.device.bud_linkstatus.b2s = MTC_BUD_STATUS_ACTIVE;
                mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_NONE;
#if F_APP_LEA_SUPPORT
                if (app_bt_point_lea_link_is_full())
                {
                    app_lea_adv_stop();
                }
                else
                {
                    app_lea_adv_start();
                }
#endif
            }

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                mtc_db.device.cur_dev_active_mode = mtc_get_btmode();
            }

            mtc_align_pri_info();
        }
        break;

    case BT_EVENT_ACL_CONN_FAIL:
        {

        }
        break;

    case BT_EVENT_ACL_AUTHEN_SUCCESS:
        {

        }
        break;

    case BT_EVENT_ACL_AUTHEN_FAIL:
        {

        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_disconn.bd_addr))
            {
                mediator_allow_sniffing = false;
                if (mediator_reopen_mic)
                {
                    app_lea_uca_enable_mic();
                }
                mediator_reopen_mic = false;
            }
            else
#endif
            {
                if (param->acl_conn_disconn.cause != (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
                {
                    mtc_topology_dm(MTC_TOPO_EVENT_LEGACY_DISCONN);
                    mtc_align_pri_info();
#if F_APP_LEA_SUPPORT
                    if (app_bt_point_lea_link_is_full())
                    {
                        app_lea_adv_stop();
                    }
                    else
                    {
                        app_lea_adv_start();
                    }
#endif
                }
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_MASTER:
        {

        }
        break;

    case BT_EVENT_ACL_ROLE_SLAVE:
        {

        }
        break;

    case BT_EVENT_ACL_CONN_SNIFF:
        {
#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_sniff.bd_addr))
            {
                mtc_db.device.bud_linkstatus.b2b = MTC_BUD_STATUS_SNIFF;
            }
            else
#endif
            {
                mtc_db.device.bud_linkstatus.b2s = MTC_BUD_STATUS_SNIFF;
            }
        }
        break;

    case BT_EVENT_ACL_CONN_ACTIVE:
        {
#if F_APP_ERWS_SUPPORT
            if (app_link_check_b2b_link(param->acl_conn_active.bd_addr))
            {
                mtc_db.device.bud_linkstatus.b2b = MTC_BUD_STATUS_ACTIVE;

                if (mtc_lea_get_pending() == LE_AUDIO_BIS)
                {
                    app_lea_mgr_mmi_handle(MMI_BIG_START);
                }
                mtc_set_pending(LE_AUDIO_NO);
            }
            else
#endif
            {
                mtc_db.device.bud_linkstatus.b2s = MTC_BUD_STATUS_ACTIVE;
            }
        }
        break;

    default:
        break;
    }
}

void mtc_resume_sniffing(void)
{
    T_APP_BR_LINK *p_link = NULL;

#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
    if (mtc_is_lea_cis_stream(NULL) || app_db.remote_cis_link_is_streaming)
    {
        APP_PRINT_TRACE0("mtc_resume_sniffing: cis streaming, don't sniffing");
        return;
    }
#endif

    app_link_disallow_legacy_stream(false);
    mediator_allow_sniffing = false;

    mtc_mgr_dispatch(AP_FM_MTC_RESUME_A2DP, &a2dp_para, NULL);
    if (app_link_get_b2s_link_num() >= 2)
    {
        return;
    }

    APP_PRINT_TRACE0("mtc_resume_sniffing: start");

    p_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (app_link_find_br_link(p_link->bd_addr) == NULL)
    {
        APP_PRINT_TRACE0("mtc_resume_sniffing: a2dp idx ir wrong");
        return;
    }

    if (p_link->bt_sniffing_state < APP_BT_SNIFFING_STATE_READY)
    {
        app_bt_sniffing_process(p_link->bd_addr);
    }

    if (!app_bt_sniffing_start(p_link->bd_addr, BT_SNIFFING_TYPE_A2DP))
    {
        APP_PRINT_TRACE0("mtc_resume_sniffing: fail");
    }
    //app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_BLE_DEV_STATE_CHANGE);

}

void mtc_disconnect_cis_link_and_stop_cis_adv(void)
{
    uint16_t ccp_conn_handle = app_lea_ccp_get_active_conn_handle();
    uint16_t mcp_conn_handle = app_lea_mcp_get_active_conn_handle();
    if (app_lea_adv_get_state() == BLE_EXT_ADV_MGR_ADV_ENABLED)
    {
        mtc_terminate_cis(MTC_CIS_TER_STOP_ADV);
    }
    else
    {
        T_APP_LE_LINK *p_link = NULL;
        p_link = app_link_find_le_link_by_conn_handle(mcp_conn_handle);
        if (p_link != NULL)
        {
            app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_ENGAGE_STREAM_PRIOIRTY);
        }
        p_link = app_link_find_le_link_by_conn_handle(ccp_conn_handle);
        if (p_link != NULL)
        {
            app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_ENGAGE_STREAM_PRIOIRTY);
        }
    }

}

static T_MTC_ACTIVE_MODE mtc_get_active_mode(void)
{
    T_MTC_BT_MODE cur_state = mtc_get_btmode();
    T_MTC_ACTIVE_MODE ret = MTC_MODE_IDLE;
    uint16_t ccp_conn_handle = app_lea_ccp_get_active_conn_handle();
    uint16_t mcp_conn_handle = app_lea_mcp_get_active_conn_handle();
    T_APP_LEA_BCA_STATE bis_state = app_lea_bca_state();
    T_APP_BR_LINK *p_link = NULL;
    uint8_t a2dp_active_idx = app_a2dp_get_active_idx();

    if (app_db.br_link[a2dp_active_idx].used)
    {
        p_link = &app_db.br_link[a2dp_active_idx];
    }

    switch (cur_state)
    {
    case MULTI_PRO_BT_BREDR:
        {
            if (p_link)
            {
                if (p_link->streaming_fg)
                {
                    ret |= MTC_MODE_BREDR_PLAYBACK;
                }
                else if (app_hfp_sco_is_connected())
                {
                    ret |= MTC_MODE_BREDR_VOICE;
                }
                else
                {
                    if (app_link_get_b2s_link_num())
                    {
                        ret |= MTC_MODE_BREDR_IDLE;
                    }
                }
            }
            if ((app_lea_adv_get_state()) == BLE_EXT_ADV_MGR_ADV_ENABLED ||
                (app_link_get_lea_link_num() != 0))
            {
                ret |= MTC_MODE_CIS_IDLE;
            }

            if (bis_state == LEA_BCA_STATE_STREAMING ||
                bis_state == LEA_BCA_STATE_PRE_SCAN ||
                bis_state == LEA_BCA_STATE_SCAN)
            {
                ret |= MTC_MODE_BIS_PLAYBACK;
            }
        }
        break;

    case MULTI_PRO_BT_BIS:
        {
            if (p_link)
            {
                if (app_link_get_b2s_link_num())
                {
                    ret |= MTC_MODE_BREDR_IDLE;
                }
            }

            if (bis_state == LEA_BCA_STATE_STREAMING ||
                bis_state == LEA_BCA_STATE_PRE_SCAN ||
                bis_state == LEA_BCA_STATE_SCAN)
            {
                ret |= MTC_MODE_BIS_PLAYBACK;
            }

            if ((app_lea_adv_get_state()) == BLE_EXT_ADV_MGR_ADV_ENABLED ||
                (app_link_get_lea_link_num() != 0))
            {
                ret |= MTC_MODE_CIS_IDLE;
            }
        }
        break;

    case MULTI_PRO_BT_CIS:
        {
            if (p_link)
            {
                if (app_link_get_b2s_link_num())
                {
                    ret |= MTC_MODE_BREDR_IDLE;
                }
            }
            if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
            {
                ret |= MTC_MODE_CIS_VOICE;
            }
            else if (mtc_is_lea_cis_stream(NULL))
            {
                ret |= MTC_MODE_CIS_PLAYBACK;
            }
            else
            {
                ret |= MTC_MODE_CIS_IDLE;
            }
        }
        break;
    }
    return ret;
}
bool mtc_check_stream_priority(T_MTC_ACTIVE_MODE remote_active_mode)
{
    T_MTC_ACTIVE_MODE current_active_mode = mtc_get_active_mode();
    bool switch_state = false;
    if (remote_active_mode & MTC_MODE_STREAM_TYPE_VOICE)
    {
        //no matter what should check the active connection
        if (current_active_mode & MTC_MODE_STREAM_TYPE_VOICE)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                //follow primary
            }
            else
            {
                switch_state = true;
            }
        }
        else
        {
            switch_state = true;
        }
    }
    else if (remote_active_mode & MTC_MODE_STREAM_TYPE_PLAYBACK)
    {
        if (current_active_mode & MTC_MODE_STREAM_TYPE_VOICE)
        {
            //remote should change state
        }
        else if (current_active_mode & MTC_MODE_STREAM_TYPE_PLAYBACK)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                //follow primary
            }
            else
            {
                switch_state = true;
            }
        }
        else
        {
            switch_state = true;
        }

    }
    else if (remote_active_mode & MTC_MODE_STREAM_TYPE_IDLE)
    {
        if (!(current_active_mode & (MTC_MODE_STREAM_TYPE_PLAYBACK | MTC_MODE_STREAM_TYPE_VOICE)))
        {
            //both earbud idle state
            if ((remote_active_mode & MTC_MODE_GROUP_CIS) &&
                !(current_active_mode & MTC_MODE_GROUP_CIS))
            {
                switch_state = true;
            }
            else if (!(remote_active_mode & MTC_MODE_GROUP_CIS) &&
                     (current_active_mode & MTC_MODE_GROUP_CIS))
            {
                //remote should start adv
            }
            else
            {
                //CIS state is the same
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    //Secondary follow primary
                }
                else
                {
                    switch_state = true;
                }
            }
        }
    }
    APP_PRINT_TRACE4("mtc_check_stream_priority ret %d cur_bt_mode %d,active 0x%x/0x%x", switch_state,
                     mtc_get_btmode(), current_active_mode, remote_active_mode);

    return switch_state;
}

#if F_APP_ERWS_SUPPORT
void mtc_state_bredr_transit_handler(T_MTC_ACTIVE_MODE state, uint8_t *param)
{
    //bt mode == bredr ,
    //-> BREDR only
    //-> BREDR + CIS adv
    //-> BREDR + CIS link
    T_MTC_ACTIVE_MODE local_state = mtc_get_active_mode();
    if (state & MTC_MODE_GROUP_BREDR)
    {
        //follow original RWS flow
    }
    if (state & MTC_MODE_GROUP_CIS)
    {
        if (local_state & MTC_MODE_GROUP_CIS)
        {
        }
        else
        {
            if (app_lea_adv_get_state() == BLE_EXT_ADV_MGR_ADV_DISABLED && !(state & MTC_MODE_GROUP_BIS))
            {
                app_lea_adv_start();
            }
        }
        if (state & (MTC_MODE_CIS_PLAYBACK | MTC_MODE_CIS_VOICE))
        {
            mtc_set_btmode(MULTI_PRO_BT_CIS);
        }
    }
    else
    {
        if (local_state & MTC_MODE_GROUP_CIS)
        {
            mtc_disconnect_cis_link_and_stop_cis_adv();
        }
    }
    if (state & MTC_MODE_GROUP_BIS)
    {
        if (state & MTC_MODE_BIS_PLAYBACK)
        {
            uint8_t sets = param[0];
            uint8_t active_index = param[1];
            T_LEA_BRS_INFO src_info;

            app_lea_mgr_mmi_handle(MMI_BIG_START);

            memset(&src_info, 0, sizeof(T_LEA_BRS_INFO));
            memcpy(src_info.adv_addr, &param[active_index * 11 + 2], 6);
            src_info.adv_addr_type = param[active_index * 11 + 8];
            src_info.advertiser_sid = param[active_index * 11 + 9];
            memcpy(src_info.broadcast_id, &param[active_index * 11 + 10], 3);

            if (sets != 0)
            {
                app_lea_bca_bs_handle_pa_info_sync(&src_info);
            }

            mtc_set_btmode(MULTI_PRO_BT_BIS);
        }
    }
}
void mtc_state_cis_transit_handler(T_MTC_ACTIVE_MODE state, uint8_t *param)
{
    //bt mode == cis ,
    //-> BREDR + CIS link

    T_MTC_ACTIVE_MODE local_state = mtc_get_active_mode();

    if (state & MTC_MODE_GROUP_BREDR)
    {

    }
    if (state & MTC_MODE_GROUP_CIS)
    {
        //for now, earbud could not handle different CIS source engage
    }
    else
    {
        mtc_disconnect_cis_link_and_stop_cis_adv();
    }
    if (state & MTC_MODE_GROUP_BIS)
    {
        app_lea_mgr_mmi_handle(MMI_BIG_START);
        mtc_set_btmode(MULTI_PRO_BT_BIS);
    }
}
void mtc_state_bis_transit_handler(T_MTC_ACTIVE_MODE state, uint8_t *param)
{
    //bt mode == bredr ,
    //-> BREDR + bis scan (+ CIS adv) (+ CIS link)
    //-> BREDR + bis stream (+ CIS adv) (+ CIS link)
    T_MTC_ACTIVE_MODE local_state = mtc_get_active_mode();
    uint8_t bis_sets = param[0];
    uint8_t bis_active_index = param[1];
    T_LEA_BRS_INFO src_info;

    memset(&src_info, 0, sizeof(T_LEA_BRS_INFO));
    memcpy(src_info.adv_addr, &param[bis_active_index * 11 + 2], 6);
    src_info.adv_addr_type = param[bis_active_index * 11 + 8];
    src_info.advertiser_sid = param[bis_active_index * 11 + 9];
    memcpy(src_info.broadcast_id, &param[bis_active_index * 11 + 10], 3);

    if (state & MTC_MODE_GROUP_BREDR)
    {
        if (state & (MTC_MODE_BREDR_PLAYBACK | MTC_MODE_BREDR_VOICE))
        {
            app_lea_mgr_mmi_handle(MMI_BIG_STOP);
            mtc_set_btmode(MULTI_PRO_BT_BREDR);
        }
        else
        {
            //nothing to do
        }
    }
    if (state & MTC_MODE_GROUP_CIS)
    {
        if (state & (MTC_MODE_CIS_PLAYBACK | MTC_MODE_CIS_VOICE))
        {
            app_lea_mgr_mmi_handle(MMI_BIG_STOP);
        }
        if (local_state & MTC_MODE_GROUP_CIS)
        {
        }
        else
        {
            app_lea_adv_start();
        }
    }
    else
    {
        if (local_state & MTC_MODE_GROUP_CIS)
        {
            mtc_disconnect_cis_link_and_stop_cis_adv();
        }
    }
    if (state & MTC_MODE_GROUP_BIS)
    {
        if (state & MTC_MODE_BIS_PLAYBACK)
        {
            uint8_t local_bis_pa_info[46];
            uint8_t *sets = &local_bis_pa_info[0];
            uint8_t *active_index = &local_bis_pa_info[1];
            uint8_t *active_bd_addr = NULL;
            app_lea_bca_get_pa_info(&local_bis_pa_info[0], &local_bis_pa_info[1], &local_bis_pa_info[2]);
            if (*sets)
            {
                active_bd_addr = &local_bis_pa_info[2 + *active_index * 11];
                if (bis_sets)
                {
                    if (memcmp(active_bd_addr, src_info.adv_addr, 6))
                    {
                        app_lea_mgr_mmi_handle(MMI_BIG_STOP);
                        app_lea_mgr_mmi_handle(MMI_BIG_START);
                        mtc_set_btmode(MULTI_PRO_BT_BIS);
                    }
                }
            }
            app_lea_bca_bs_handle_pa_info_sync(&src_info);
        }
        else
        {
            //both under scan, do nothing
        }
    }
    else
    {
        app_lea_mgr_mmi_handle(MMI_BIG_STOP);
    }
}

bool mtc_get_link_info(uint8_t *sets, uint8_t *active_index, uint8_t *para)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t offset = 0;
    bool ret = false;
    uint16_t active_conn_handle = app_lea_mcp_get_active_conn_handle();

    *sets = 0;

    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        p_link = &app_db.le_link[i];

        if (p_link->used)
        {
            (*sets)++;
            if (p_link->conn_handle == active_conn_handle)
            {
                *active_index = *sets;
            }

            memcpy(&para[offset], p_link->bd_addr, 6);
            offset += 6;
        }
    }

    return ret;
}

bool mtc_sync_active_mode(void)
{
    bool ret = false;
    uint16_t active_mode = mtc_get_active_mode();
    uint8_t buf[62] = {0};

    //active mode 2, BIS src count 1 + active index 1 + 4*(bdaddr 6 + addr type 1+ adv sid 1+ src id 3)
    //CIS src count 1+ CIS active index 1 + 2*(bdaddr 6)
    buf[0] = active_mode;
    buf[1] = active_mode >> 8;

    app_lea_bca_get_pa_info(&buf[2], &buf[3], &buf[4]);
    mtc_get_link_info(&buf[48], &buf[49], &buf[50]);
    ret = remote_async_msg_relay(mtc_relay_handle,
                                 MTC_REMOTE_SYNC_MTC_STATE,
                                 buf,
                                 62,
                                 false);
    return ret;
}
void mtc_handle_sync_btmote(T_MTC_ACTIVE_MODE state, uint8_t *param)
{
    T_MTC_BT_MODE cur_state = mtc_get_btmode();
    if (mtc_check_stream_priority(state))
    {
        //add PA info
        uint8_t sets = param[0];
        uint8_t *pa_info = &param[2];
        for (uint8_t i = 0 ; i < sets; i++)
        {
            uint8_t offset = i * 11;
            app_lea_bca_bs_update_device_info((T_LEA_BRS_INFO *)&pa_info[offset]);
        }
        mtc_state_transit_handler[cur_state](state, param);
    }
}
#endif

void mtc_legacy_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_MTC_BT_MODE mode = mtc_get_btmode();
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
    case BT_EVENT_ACL_CONN_DISCONN:
    case BT_EVENT_ACL_CONN_FAIL:
    case BT_EVENT_ACL_AUTHEN_SUCCESS:
    case BT_EVENT_ACL_AUTHEN_FAIL:
    case BT_EVENT_ACL_ROLE_MASTER:
    case BT_EVENT_ACL_ROLE_SLAVE:
    case BT_EVENT_ACL_CONN_SNIFF:
    case BT_EVENT_ACL_CONN_ACTIVE:
        {
            mtc_legacy_link_status(event_type, param);
        }
        break;

    case BT_EVENT_A2DP_SNIFFING_CONN_CMPL:
        {
            memcpy(&a2dp_para, (uint8_t *)&param->a2dp_sniffing_conn_cmpl,
                   sizeof(T_BT_EVENT_PARAM_A2DP_STREAM_START_IND));
        }
        break;

    case BT_EVENT_A2DP_STREAM_START_IND:
        {
            memcpy(&a2dp_para, (uint8_t *)&param->a2dp_stream_start_ind,
                   sizeof(T_BT_EVENT_PARAM_A2DP_STREAM_START_IND));
            mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_NONE;
#if GFPS_SASS_LEA_SUPPORT
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->a2dp_stream_start_ind.bd_addr);
            if (p_link)
            {
                //run preem if the sass switch policy is different with original
                uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_EDR_LINK,
                                                                MULTILINK_SASS_A2DP_PREEM, false);

                if (sass_preempt >> 8)
                {
                    app_multi_preemptive_judge(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_A2DP_PREEM);
                    break;
                }
            }
#endif
            mediator_allow_a2dp_preempt_mcp = true;

            if (mode == MULTI_PRO_BT_BREDR)
            {
                mtc_terminate_cis(MTC_CIS_TER_NOT_STOP_ADV);
            }
            else if (mode == MULTI_PRO_BT_BIS && !mtc_db.device.last_source)
            {
                mtc_resume_sniffing();
                mtc_terminate_bis();
            }
            else if (mode == MULTI_PRO_BT_CIS && !mtc_db.device.last_source)
            {
                if (mtc_ccp_check())
                {
                    break;
                }
                app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_AVRCP_PLAYING, NULL);
                mtc_cis_ase_release(MTC_STREAM_A2DP);
            }
#if F_APP_LEA_REALCAST_SUPPORT
            app_lea_realcast_stop_scan();
#endif
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            T_APP_BR_LINK *p_link = NULL;

            p_link = app_link_find_br_link(param->avrcp_play_status_changed.bd_addr);
            if (p_link != NULL)
            {
                if (mode == MULTI_PRO_BT_BIS)
                {
                    if (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        mtc_set_resume_dev(MULTI_RESUME_NONE);
                        mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_BIS;
                    }
                }
                else if (mode == MULTI_PRO_BT_CIS)
                {
                    if (mediator_allow_a2dp_preempt_mcp &&
                        param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        if (mtc_ccp_check())
                        {
                            break;
                        }

                        mtc_set_resume_dev(MULTI_RESUME_NONE);
                        mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_CIS;
                    }
                    else if (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_STOPPED ||
                             param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
                    {
                        app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_AVRCP_PAUSE, NULL);
                    }
                }
                else if (mode == MULTI_PRO_BT_BREDR)
                {
                    if ((p_link->id == app_a2dp_get_active_idx()) &&
                        (param->avrcp_play_status_changed.play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
                    {
                        mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_EDR;
                    }
                }
            }
        }
        break;

#if F_APP_ERWS_SUPPORT
    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            if (param->remote_roleswap_status.status == BT_ROLESWAP_STATUS_SUCCESS)
            {
#if F_APP_BUD_LOCATION_SUPPORT
                if (app_lea_mgr_get_media_suspend_by_out_ear())
                {
                    app_lea_mgr_update_media_suspend_by_out_ear(true);
                }
#endif
                mtc_topology_dm(MTC_TOPO_EVENT_BR_ROLE_SWAP);
                mediator_reopen_mic = true;
                mtc_sync_active_mode();
                app_lea_mgr_update_sniff_mask();
            }
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
#if F_APP_LEA_REALCAST_SUPPORT
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_lea_realcast_relay_device_info();
            }
#endif
            mtc_sync_active_mode();
            app_lea_mgr_update_sniff_mask();
            mtc_align_pri_info();
            app_lea_vol_sync_info();

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_lea_uca_sync_cis_streaming_state(mtc_is_lea_cis_stream(NULL));
            }
        }
        break;

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
            app_db.remote_cis_link_is_streaming = 0;

            //Avoid LEA central connect 2 LE ACLs to the same bud
            if (mtc_get_btmode() != MULTI_PRO_BT_BREDR)
            {
                app_lea_adv_stop();
            }
        }
        break;
#endif

    case BT_EVENT_A2DP_STREAM_DATA_IND:
        {
            if ((mtc_db.bredr_db.wait_a2dp & MTC_BT_MASK_BIS) &&
                !mtc_db.device.last_source)
            {
                mtc_resume_sniffing();
                mtc_terminate_bis();
            }
            else if ((mtc_db.bredr_db.wait_a2dp & MTC_BT_MASK_CIS) &&
                     !mtc_db.device.last_source)
            {
                if (mtc_ccp_check())
                {
                    break;
                }
                app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_AVRCP_PLAYING, NULL);
                mtc_cis_ase_release(MTC_STREAM_A2DP);
            }
            else if (mtc_db.bredr_db.wait_a2dp & MTC_BT_MASK_EDR)
            {
                T_APP_BR_LINK *p_link = NULL;

                p_link = app_link_find_br_link(param->a2dp_stream_data_ind.bd_addr);
                if (p_link == NULL || p_link->id != app_a2dp_get_active_idx())
                {
                    break;
                }

                if (mtc_ccp_check())
                {
                    break;
                }

                if (mtc_terminate_cis(MTC_CIS_TER_NOT_STOP_ADV))
                {
                    app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_ISO_SUSPEND);
                }

                if (mtc_check_sniff(param->a2dp_stream_data_ind.bd_addr))
                {
                    break;
                }
                mtc_resume_sniffing();
            }
            mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_NONE;
            handle = false;
        }
        break;

    case BT_EVENT_SCO_CONN_CMPL:
        {
            if (param->sco_conn_cmpl.cause == 0)
            {
                mtc_le_connection_update(true);
            }
#if F_APP_LEA_REALCAST_SUPPORT
            app_lea_realcast_stop_scan();
#endif
        }
        break;

    case BT_EVENT_SCO_DATA_IND:
        {
#if GFPS_SASS_LEA_SUPPORT
            T_APP_BR_LINK *p_link = app_link_find_br_link(param->sco_data_ind.bd_addr);
            if (p_link)
            {
                //run preem if the sass switch policy is different with original
                uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM,
                                                                false);

                if (sass_preempt >> 8)
                {
                    app_multi_preemptive_judge(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM);
                    break;
                }
            }
#endif
            T_MTC_BT_MODE mode = mtc_get_btmode();

            if (mode == MULTI_PRO_BT_BIS && app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                mtc_terminate_bis();
            }
            else if ((mode == MULTI_PRO_BT_CIS || mtc_is_lea_cis_stream(NULL)) &&
                     app_hfp_get_call_status() != APP_CALL_IDLE)
            {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
                // 2.4G call need to preempt HFP call, don't release ase
#else
                mtc_cis_ase_release(MTC_STREAM_SCO);
#endif
            }
            handle = false;
        }
        break;

    case BT_EVENT_SCO_SNIFFING_STOPPED:
    case BT_EVENT_SCO_SNIFFING_DISCONN_CMPL:
        {
            mtc_check_lea_track();
            app_start_timer(&timer_idx_mtc_delay_pacs_notify, "delay_pacs_notify",
                            mtc_timer_id, MTC_TIMER_DELAY_PACS_NOTIFY, 0, true, 2000);
        }
        break;

    case BT_EVENT_SCO_DISCONNECTED:
        {
            app_start_timer(&timer_idx_mtc_delay_pacs_notify, "delay_pacs_notify",
                            mtc_timer_id, MTC_TIMER_DELAY_PACS_NOTIFY, 0, true, 2000);
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
        APP_PRINT_INFO1("mtc_legacy_bt_cback: event %x", event_type);
    }
}

bool mtc_is_lea_cis_stream(uint8_t *set_mcp)
{
    uint8_t i;
    bool streaming = false;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        T_APP_LE_LINK *link = &app_db.le_link[i];

        if (link->used &&
            link->lea_link_state == LEA_LINK_STREAMING)
        {
            if (set_mcp != NULL)
            {
                for (uint8_t j = 0; j < ASCS_ASE_ENTRY_NUM; j++)
                {
                    if (link->lea_ase_entry[j].used == true &&
                        link->lea_ase_entry[j].audio_context == AUDIO_CONTEXT_MEDIA)
                    {
                        link->media_state = *set_mcp;
                        break;
                    }
                }
            }
            streaming = true;
            break;
        }
    }

    return streaming;
}

uint8_t mtc_streaming_exist(void)
{
    uint8_t i, result = 0;
    T_APP_BR_LINK *p_br_link = NULL;
    T_MTC_BT_MODE cur_mode = mtc_get_btmode();
    T_APP_LEA_BCA_STATE bis_state = app_lea_bca_state();

    p_br_link = &app_db.br_link[app_a2dp_get_active_idx()];

    if (p_br_link->used &&
        ((p_br_link->streaming_fg && (p_br_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)) ||
         p_br_link->sco_handle != 0) || app_db.detect_suspend_by_out_ear)
    {
        if (cur_mode != MULTI_PRO_BT_BREDR)
        {
            mtc_set_btmode(MULTI_PRO_BT_BREDR);
        }

        app_link_disallow_legacy_stream(false);
        result = 1;
        goto CHECK_RESULT;
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_db.le_link[i].used &&
            app_db.le_link[i].lea_link_state == LEA_LINK_STREAMING)
        {
            if (cur_mode != MULTI_PRO_BT_CIS)
            {
                mtc_set_btmode(MULTI_PRO_BT_CIS);
            }
            result = 2;
            goto CHECK_RESULT;
        }
    }

    if ((bis_state == LEA_BCA_STATE_STREAMING) ||
        (bis_state == LEA_BCA_STATE_PRE_SCAN) ||
        (bis_state == LEA_BCA_STATE_SCAN))
    {
        if (cur_mode != MULTI_PRO_BT_BIS)
        {
            mtc_set_btmode(MULTI_PRO_BT_BIS);
        }
        result = 3;
        goto CHECK_RESULT;
    }

    if (app_lea_mgr_get_media_suspend_by_out_ear())
    {
        mtc_set_btmode(MULTI_PRO_BT_CIS);
        result = 4;
    }

CHECK_RESULT:
    APP_PRINT_TRACE6("mtc_streaming_exist: idx %d, %d, %d, %d, %d, %d",
                     app_a2dp_get_active_idx(),
                     app_db.remote_session_state,
                     app_cfg_nv.bud_role,
                     bis_state,
                     app_db.detect_suspend_by_out_ear,
                     result);
    return result;
}

bool mtc_topology_dm(uint8_t event)
{
    bool ret = true;

    APP_PRINT_INFO4("mtc_topology_dm: curr mode %x, event %x, bud_role %x, %d",
                    mtc_get_btmode(), event, app_cfg_nv.bud_role, app_cfg_const.active_prio_connected_device_after_bis);

#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        if (remote_session_state_get() == REMOTE_SESSION_STATE_DISCONNECTED &&
            event == MTC_TOPO_EVENT_LEA_DISCONN &&
            app_link_get_lea_link_num() == 0 &&
            app_lea_bca_state() != LEA_BCA_STATE_STREAMING)
        {
            mtc_set_btmode(MULTI_PRO_BT_BREDR);
        }

        return false;
    }
#endif

    switch (event)
    {
    case MTC_TOPO_EVENT_CCP_CALL:
        {
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
            if (app_cfg_const.enable_24g_bt_audio_source_switch)
            {
                if (app_cfg_nv.allowed_source == ALLOWED_SOURCE_BT)
                {
                    //ignore CCP events when source is phone linke.
                    break;
                }
            }
#endif
            if (app_lea_ccp_get_call_status() == APP_CALL_IDLE)
            {
                app_link_disallow_legacy_stream(false);
                if (app_hfp_get_call_status() != APP_CALL_IDLE)
                {
                    uint8_t zero_addr[6] = {0};
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                    if (memcmp(test_bd_addr, zero_addr, 6) != 0)
                    {
                        mtc_mgr_dispatch(AP_FM_MTC_RESUME_SCO, test_bd_addr, NULL);
                        memset(test_bd_addr, 0, 6);
                    }
                }

                if (mtc_db.device.last_source)
                {
                    if (mtc_db.device.last_source == MULTI_RESUME_A2DP)
                    {
                        mtc_cis_ase_release(MTC_STREAM_RESUME_A2DP);
                    }
                    else if (mtc_db.device.last_source == MULTI_RESUME_BIS)
                    {
                        mtc_resume_bis(mtc_db.device.last_source);
                    }

                    mtc_set_resume_dev(MULTI_RESUME_NONE);
                }
            }
            else
            {
#if GFPS_SASS_LEA_SUPPORT
                T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_handle(app_lea_ccp_get_active_conn_handle());
                if (p_link)
                {
                    //run preem if the sass switch policy is different with original
                    uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_LE_LINK,
                                                                    MULTILINK_SASS_LEA_CALL_PREEM, false);

                    if (sass_preempt >> 8)
                    {
                        app_multi_preemptive_judge(p_link->id, SASS_LE_LINK, MULTILINK_SASS_LEA_CALL_PREEM);
                        break;
                    }
                }
#endif
                app_link_disallow_legacy_stream(true);
                //There is error on ccp state, when voice data path is setup,
                //that means user is pick up cis voice
                mtc_set_btmode(MULTI_PRO_BT_CIS);
                mtc_terminate_hfp_call();
            }
        }
        break;

    case MTC_TOPO_EVENT_HFP_CALL:
        {
            if (app_hfp_get_call_status() == APP_CALL_IDLE)
            {
                if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
                {
                    mtc_set_btmode(MULTI_PRO_BT_CIS);
                }
                memset(test_bd_addr, 0, 6);
            }
            else
            {
#if GFPS_SASS_LEA_SUPPORT
                T_APP_BR_LINK *p_link = &app_db.br_link[app_hfp_get_active_idx()];
                if (p_link->used)
                {
                    //run preem if the sass switch policy is different with original
                    uint16_t sass_preempt = app_sass_preempt_policy(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM,
                                                                    false);

                    if (sass_preempt >> 8)
                    {
                        app_multi_preemptive_judge(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM);
                        break;
                    }
                }
#endif
                if ((app_lea_ccp_get_call_status() != APP_CALL_IDLE) && app_lea_uca_is_upstreaming_enabled())
                {
                    mtc_terminate_hfp_call();
                }
                else
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
                    // don't reject hfp call if we want hfp call preempt 2.4G call
                    app_dongle_handle_hfp_call();
#endif
                }
            }
        }
        break;

    case MTC_TOPO_EVENT_CIS_ENABLE:
        {
            mediator_allow_a2dp_preempt_mcp = false;

            if (app_lea_bca_state() != LEA_BCA_STATE_IDLE)
            {
                mtc_terminate_bis();
            }

            mtc_set_btmode(MULTI_PRO_BT_CIS);
        }
        break;

    case MTC_TOPO_EVENT_CIS_STREAMING:
        {
            mediator_allow_a2dp_preempt_mcp = true;
        }
        break;

    case MTC_TOPO_EVENT_CIS_TERMINATE:
        {
            if ((app_db.br_link[app_a2dp_get_active_idx()].streaming_fg == true) &&
                (app_db.avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                (app_bt_policy_get_call_status() == APP_CALL_IDLE))
            {
                if (mtc_get_btmode() == MULTI_PRO_BT_CIS)
                {
                    mtc_set_resume_dev(MULTI_RESUME_NONE);
                    mtc_db.bredr_db.wait_a2dp = MTC_BT_MASK_CIS;
                }
            }
            else if (app_hfp_get_call_status() != APP_CALL_IDLE &&
                     app_lea_ccp_get_call_status() == APP_CALL_IDLE)
            {
                mtc_set_btmode(MULTI_PRO_BT_BREDR);
            }
        }
        break;

    case MTC_TOPO_EVENT_A2DP_START:
        {
            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                return false;
            }

            if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
            {
                return false;
            }
            mtc_set_btmode(MULTI_PRO_BT_BREDR);
        }
        break;

    case MTC_TOPO_EVENT_LEA_CONN:
    case MTC_TOPO_EVENT_LEGACY_CONN:
    case MTC_TOPO_EVENT_LEA_DISCONN:
    case MTC_TOPO_EVENT_LEGACY_DISCONN:
    case MTC_TOPO_EVENT_BIS_STOP:
    case MTC_TOPO_EVENT_LEA_ADV_STOP:
    case MTC_TOPO_EVENT_BR_ROLE_SWAP:
        {
            bool edr_conn = false;
            bool le_conn = false;
            bool edr_prio_high = true;
            uint8_t prio_mode = app_cfg_const.active_prio_connected_device;
            bool disallow_sniffing = false;
            T_MTC_BT_MODE cur_mode = mtc_get_btmode();
            T_APP_LEA_BCA_STATE bis_state = app_lea_bca_state();

            if (event == MTC_TOPO_EVENT_LEGACY_CONN)
            {
                if (mediator_allow_a2dp_preempt_mcp == false)
                {
                    break;
                }
                mtc_db.device.mtc_last_bt_mode = MULTI_PRO_BT_BREDR;
            }
            else if (event == MTC_TOPO_EVENT_LEA_CONN)
            {
                mtc_db.device.mtc_last_bt_mode = MULTI_PRO_BT_CIS;
            }
            else
            {
                mtc_db.device.mtc_last_bt_mode = cur_mode;
            }

            if (mtc_streaming_exist())
            {
                break;
            }

            if (event == MTC_TOPO_EVENT_BIS_STOP)
            {
                prio_mode = app_cfg_const.active_prio_connected_device_after_bis;
            }
            else
            {
                prio_mode = app_cfg_const.active_prio_connected_device;
            }

            if ((app_link_get_b2s_link_num() != 0))
            {
                edr_conn = true;
            }

            if (mtc_check_lea_le_link())
            {
                le_conn = true;
            }

            if (prio_mode == MTC_LINK_ACT_LEA_PRIO_HIGH)
            {
                edr_prio_high = false;
            }
            else  if (prio_mode == MTC_LINK_ACT_LEGACY_PRIO_HIGH)
            {
                edr_prio_high = true;
            }
            else  if (prio_mode == MTC_LINK_ACT_LAST_ONE)
            {
                if (mtc_db.device.mtc_last_bt_mode == MULTI_PRO_BT_BREDR)
                {
                    edr_prio_high = true;
                }
                else if (mtc_db.device.mtc_last_bt_mode == MULTI_PRO_BT_CIS)
                {
                    edr_prio_high = false;
                }
            }

            if (edr_prio_high)
            {
                if (edr_conn)
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                }
                else if (le_conn)
                {
                    mtc_set_btmode(MULTI_PRO_BT_CIS);
                    disallow_sniffing = true;
                }
                else
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                }
            }
            else
            {
                if (le_conn)
                {
                    mtc_set_btmode(MULTI_PRO_BT_CIS);
                    disallow_sniffing = true;
                }
                else if (edr_conn)
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                }
                else
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                }
            }

            if (!disallow_sniffing)
            {
                app_link_disallow_legacy_stream(false);
            }

            APP_PRINT_INFO3("mtc_topology_dm: le_conn_fg %d, edr_conn_fg %d, bis_state %d",
                            le_conn, edr_conn, bis_state);
        }
        break;

    case MTC_TOPO_EVENT_BIS_START:
        {
            mtc_set_btmode(MULTI_PRO_BT_BIS);
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_lea_uca_link_sm(app_lea_mcp_get_active_conn_handle(), LEA_MCP_PAUSE, NULL);
            }
        }
        break;

    case MTC_TOPO_EVENT_SET_BR_MODE:
        {
            mtc_set_btmode(MULTI_PRO_BT_BREDR);
        }
        break;

    case MTC_TOPO_EVENT_BIS_STREAMING:
        {
            mtc_cis_ase_release(MTC_STREAM_BIS);
        }
        break;

    case MTC_TOPO_EVENT_MCP_PLAYING:
        {
            if (app_lea_bca_state() != LEA_BCA_STATE_IDLE)
            {
                mtc_terminate_bis();
            }

            mtc_set_btmode(MULTI_PRO_BT_CIS);
        }
        break;

    case MTC_TOPO_EVENT_MMI:
        {
            if (app_link_get_b2s_link_num() != 0 && mtc_check_lea_le_link() == true)
            {
                if (app_link_get_b2s_link_num() == 1 && mtc_get_btmode() == MULTI_PRO_BT_BREDR)
                {
                    uint8_t legacy_idx = app_a2dp_get_active_idx();
                    T_APP_LE_LINK *p_link = NULL;

                    p_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());
                    if (p_link != NULL)
                    {
                        if ((app_db.br_link[legacy_idx].connected_profile & A2DP_PROFILE_MASK) == 0 &&
                            memcmp(app_db.br_link[legacy_idx].bd_addr, p_link->bd_addr, 6) == 0)
                        {
                            mtc_set_btmode(MULTI_PRO_BT_CIS);
                        }
                    }
                }
            }
        }
        break;

    default:
        ret = false;
        break;
    }

    uint8_t bt_mode = mtc_get_btmode();

    APP_PRINT_INFO2("mtc_topology_dm: mode change %x to %x",
                    mtc_db.device.cur_dev_active_mode,
                    bt_mode);

    if (mtc_db.device.cur_dev_active_mode != bt_mode &&
        app_db.device_state == APP_DEVICE_STATE_ON)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
        {
            mtc_db.device.cur_dev_active_mode = bt_mode;
        }
#if F_APP_ERWS_SUPPORT
        else if (remote_session_state_get() == REMOTE_SESSION_STATE_CONNECTED)
        {
            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_LEA,
                                                LEA_REMOTE_MMI_SWITCH_SYNC,
                                                (uint8_t *)&bt_mode, 1);
            mtc_db.device.cur_dev_active_mode = bt_mode;
        }
#endif
    }

    APP_PRINT_INFO2("mtc_topology_dm: mode change %x to %x",
                    mtc_db.device.cur_dev_active_mode,
                    bt_mode);

    return ret;
}

#if F_APP_ERWS_SUPPORT
void mtc_relay_cback(uint16_t event, T_REMOTE_RELAY_STATUS status, void *buf,
                     uint16_t len)
{
    APP_PRINT_TRACE2("mtc_relay_cback: event 0x%04x, status %u", event, status);

    switch (event)
    {
    case MTC_REMOTE_SYNC_BIS_STOP:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_RCVD ||
                status == REMOTE_RELAY_STATUS_SYNC_SENT_OUT)
            {

                mtc_set_beep((T_MTC_PRO_BEEP) * ((uint8_t *)buf));
                app_lea_mgr_mmi_handle(MMI_BIG_STOP);
                mtc_mgr_dispatch(ML_FM_MTC_LEA_STOP, NULL, NULL);
            }
        }
        break;

    case MTC_REMOTE_SYNC_CIS_STOP:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_RCVD ||
                status == REMOTE_RELAY_STATUS_SYNC_SENT_OUT)
            {
                app_lea_adv_stop();
                mtc_topology_dm(MTC_TOPO_EVENT_LEA_ADV_STOP);
            }
        }
        break;

    case MTC_REMOTE_SYNC_MTC_STATE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD ||
                status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT)
            {
                T_MTC_ACTIVE_MODE active_mode = (T_MTC_ACTIVE_MODE)(((uint8_t *)buf)[0] | ((uint8_t *)buf)[1] << 8);
                mtc_handle_sync_btmote(active_mode, &((uint8_t *)buf)[2]);
            }

        }
        break;

    case MTC_REMOTE_SYNC_STOP_SNIFFING:
        {
            mtc_stream_switch(false);
        }
        break;

    case MTC_REMOTE_SYNC_PRI_INFO:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_MTC_SYNC_INFO *para = (T_MTC_SYNC_INFO *)buf;

                mtc_set_btmode((T_MTC_BT_MODE)para->bt_mode);
                app_lea_uca_set_mic_mute_state(para->mic_state);
            }
        }
        break;

    case MTC_REMOTE_SYNC_CIS_REL_ASE:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT ||
                 status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                T_MTC_STREAM_TYPE type = *(T_MTC_STREAM_TYPE *)buf;

                if (type == MTC_STREAM_SCO)
                {
                    mtc_cis_audio_conext_change(false);
                    mtc_ase_release();
                }
                else if (type == MTC_STREAM_RESUME_OUTBAND)
                {
                    mtc_ase_release();
                    mtc_cis_audio_conext_change(true);
                }
                else
                {
                    mtc_ase_release();
                }
            }
        }
        break;

    case MTC_REMOTE_SYNC_CIS_RELEASE_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_SEND_FAILED)
            {
                T_MTC_STREAM_TYPE type = *(T_MTC_STREAM_TYPE *)buf;

                mtc_ase_release();

                if (type == MTC_STREAM_A2DP)
                {
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        mtc_resume_sniffing();
                    }
                }
                else if (type == MTC_STREAM_RESUME_A2DP)
                {
                    mtc_set_btmode(MULTI_PRO_BT_BREDR);
                    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                    }
                }
                mtc_cis_audio_conext_change(true);
            }
        }
        break;

    case MTC_REMOTE_SYNC_LEA_REL_A2DP:
        {
            mtc_mgr_dispatch(AP_FM_MTC_STOP_A2DP, a2dp_para.bd_addr, NULL);
        }
        break;

    case MTC_REMOTE_SYNC_BIS_START:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_RCVD)
            {
                mtc_db.device.last_source = (T_MTC_RESUME_MODE) * ((uint8_t *)buf);

                if (mtc_db.device.last_source)
                {
                    app_lea_mgr_mmi_handle(MMI_BIG_START);
                }
                else
                {
                    app_link_disallow_legacy_stream(false);
                }
                mtc_set_resume_dev(MULTI_RESUME_NONE);
            }
        }
        break;

    case MTC_REMOTE_SYNC_LEA_TRACK:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_RCVD ||
                status == REMOTE_RELAY_STATUS_SYNC_SENT_OUT)
            {
                mtc_check_lea_track();
                mtc_set_resume_dev(MULTI_RESUME_NONE);
            }
        }
        break;

    case MTC_REMOTE_SYNC_LEA_CONTEXT_CHANGE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                mtc_cis_audio_conext_change(*((bool *)buf));
            }
        }
        break;

    case MTC_REMOTE_SYNC_LEA_CON_UPDATE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (*(bool *)buf)
                {
                    mtc_release_lea_stream();
                }
                mtc_request_le_conn_update();
            }
        }
        break;

    default:
        break;
    }
}
#endif

/*============================================================================*
 *                              Interface
 *============================================================================*/
T_MTC_RESULT mtc_if_fm_ap_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT mtc_result = MTC_RESULT_SUCCESS;

    APP_PRINT_INFO1("mtc_if_fm_ap_handle: msg %x", msg);

    switch (msg)
    {
    case AP_TO_MTC_SCO_CMPL:
        {
            if (inbuf == NULL || outbuf == NULL)
            {
                mtc_result = MTC_RESULT_REJECT;
                break;
            }
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
            // don't reject hfp call if we want hfp call preempt 2.4G call
#else
            // memcpy(test_bd_addr, (uint8_t *)inbuf, 6);
            //APP_PRINT_TRACE1("AP_TO_MTC_SCO_CMPL: test_bd_addr %s", TRACE_BDADDR(test_bd_addr));
            memset(test_bd_addr, 0, 6);
            if ((app_lea_ccp_get_call_status() != APP_CALL_IDLE) && app_lea_uca_is_upstreaming_enabled())
            {
                mtc_terminate_hfp_call();
                *((uint8_t *)outbuf) = true;
            }
            else
            {
                *((uint8_t *)outbuf) = false;
            }
#endif
        }
        break;

    case AP_TO_MTC_SNIFFING_CHECK:
        {
            if (mediator_allow_sniffing)
            {
                *((uint8_t *)outbuf) = true;
            }
            else
            {
                *((uint8_t *)outbuf) = false;
            }

        }
        break;

    case AP_TO_MTC_SHUTDOWN:
        {
            mtc_shutdown();
        }
        break;

    default:
        break;
    }
    return mtc_result;
}

T_MTC_RESULT mtc_if_fm_ml_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT mtc_result = MTC_RESULT_SUCCESS;

    APP_PRINT_INFO1("mtc_if_fm_ml_handle: msg %x", msg);

    switch (msg)
    {
    case ML_TO_MTC_CH_SNIFFING:
        {
            if (mtc_get_sniffing())
            {
                *(uint8_t *)outbuf = true;
            }
            else
            {
                *(uint8_t *)outbuf = false;

                if (mtc_is_lea_cis_stream(NULL))
                {
                    mtc_resume_cis_track(false);
                }
            }
        }
        break;

    case ML_TO_MTC_CH_LEA_CALL:
        {
            if ((app_lea_ccp_get_call_status() != APP_CALL_IDLE) && app_lea_uca_is_upstreaming_enabled())
            {
                *((uint8_t *)outbuf) = true;
            }
            else
            {
                *((uint8_t *)outbuf) = false;
            }
        }
        break;

    default:
        break;
    }
    return mtc_result;
}

T_MTC_RESULT mtc_if_fm_lcis_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT mtc_result = MTC_RESULT_SUCCESS;

    APP_PRINT_INFO1("mtc_if_fm_lcis_handle: msg %x", msg);

    switch (msg)
    {
    case LCIS_TO_MTC_ASCS_CP_ENABLE:
        {
            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                if (mediator_ignore_hfp == false)
                {
                    *((uint8_t *)outbuf) = true;
                }
                else
                {
#if F_APP_GAMING_LE_AUDIO_24G_STREAM_FIRST
                    // don't reject hfp call if we want hfp call preempt 2.4G call
#else
                    if (inbuf == NULL)
                    {
                        mtc_terminate_hfp_call();
                    }
#endif
                    *((uint8_t *)outbuf) = false;
                }
            }
            else
            {
                *((uint8_t *)outbuf) = false;
            }
        }
        break;

    case LCIS_TO_MTC_TER_CIS:
        {
            if (mtc_terminate_cis(MTC_CIS_TER_NOT_STOP_ADV))
            {
                app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_ISO_SUSPEND);
            }
        }
        break;

    default:
        break;
    }
    return mtc_result;
}

T_MTC_RESULT mtc_if_fm_lbis_handle(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_RESULT mtc_result = MTC_RESULT_SUCCESS;

    APP_PRINT_INFO1("mtc_if_fm_lbis_handle: msg %x", msg);

    switch (msg)
    {
    case LBIS_TO_MTC_CHECK_RESUME:
        {
            *((uint8_t *)outbuf) = mtc_db.device.last_source;
            mtc_set_resume_dev(MULTI_RESUME_NONE);
        }
        break;

    default:
        break;
    }
    return mtc_result;
}

static bool mtc_mgr_dispatch(T_MTC_IF_MSG msg, void *inbuf, void *outbuf)
{
    T_MTC_CBACK_ITEM *p_item;

    APP_PRINT_INFO1("mtc_mgr_dispatch: msg 0x%04x", msg);

    p_item = (T_MTC_CBACK_ITEM *)mtc_db.cback_list.p_first;
    while (p_item != NULL)
    {
        p_item->cback(msg, inbuf, outbuf);
        p_item = p_item->p_next;
    }

    return true;
}

bool mtc_cback_register(P_MTC_IF_CB cback)
{
    T_MTC_CBACK_ITEM *p_item;

    p_item = (T_MTC_CBACK_ITEM *)mtc_db.cback_list.p_first;
    while (p_item != NULL)
    {
        if (p_item->cback == cback)
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = malloc(sizeof(T_MTC_CBACK_ITEM));
    if (p_item != NULL)
    {
        p_item->cback = cback;
        os_queue_in(&mtc_db.cback_list, p_item);
        return true;
    }

    return false;
}

bool mtc_cback_unregister(P_MTC_IF_CB cback)
{
    bool ret = false;
    T_MTC_CBACK_ITEM *p_item;

    p_item = (T_MTC_CBACK_ITEM *)mtc_db.cback_list.p_first;
    while (p_item != NULL)
    {
        if (p_item->cback == cback)
        {
            os_queue_delete(&mtc_db.cback_list, p_item);
            free(p_item);
            ret = true;
            break;
        }
        p_item = p_item->p_next;
    }

    return ret;
}

static void mtc_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("mtc_timeout_cb: timer_evt %d, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case MTC_TIMER_DELAY_PACS_NOTIFY:
        {
            app_stop_timer(&timer_idx_mtc_delay_pacs_notify);

            if ((app_hfp_get_call_status() == APP_CALL_IDLE) &&
                app_hfp_sco_is_connected() == false)
            {
                if (mtc_db.device.last_source)
                {
                    if (mtc_db.device.last_source == MULTI_RESUME_CIS)
                    {
                        uint8_t mcp = MCS_MEDIA_STATE_PLAYING;

                        app_link_disallow_legacy_stream(true);
                        mtc_set_btmode(MULTI_PRO_BT_CIS);

                        if (!mtc_is_lea_cis_stream(&mcp))
                        {
                            T_APP_LE_LINK *p_link;

                            p_link = app_link_find_le_link_by_conn_handle(app_lea_mcp_get_active_conn_handle());

                            if (p_link != NULL && p_link->media_state == MCS_MEDIA_STATE_PAUSED)
                            {
                                app_mmi_handle_action(MMI_AV_PLAY_PAUSE);
                            }
                        }
                    }
                    else if (mtc_db.device.last_source == MULTI_RESUME_BIS)
                    {
                        mtc_resume_bis(mtc_db.device.last_source);
                    }
                }

                mtc_cis_pacs_enable(true);
            }
        }
        break;

    default:
        break;
    }
}

void mtc_init(void)
{
    bt_mgr_cback_register(mtc_legacy_bt_cback);
#if F_APP_ERWS_SUPPORT
    mtc_relay_handle = remote_relay_register(mtc_relay_cback);
#endif
    app_timer_reg_cb(mtc_timeout_cb, &mtc_timer_id);
}
#endif
