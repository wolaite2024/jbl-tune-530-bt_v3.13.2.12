#include "trace.h"
#include "gap_conn_le.h"
#include "audio_track.h"
#include "ble_audio.h"
#include "metadata_def.h"
#include "vcs_mgr.h"
#include "app_lea_ascs.h"
#include "app_lea_vcs.h"
#include "app_lea_vol_def.h"
#include "app_lea_mcp.h"
#include "app_lea_ccp.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_vol_policy.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_types.h"
#include "app_relay.h"
#include "multitopology_ctrl.h"

#if F_APP_HARMAN_FEATURE_SUPPORT
#include "app_harman_vendor_cmd.h"
#endif

typedef struct
{
    uint8_t vol_gain;
    bool    mute;
} T_LEA_VOL_SYNC_INFO;

typedef enum
{
    LEA_VOL_MSG_VOLUME_SYNC = 0x01,
    LEA_VOL_MSG_TOTAL
} T_LEA_VOL_REMOTE_MSG;

uint8_t app_lea_vol_gain_get(void)
{
    uint8_t vol_level = VOLUME_LEVEL(app_cfg_nv.lea_vol_setting);

    if ((vol_level == 0) && app_cfg_nv.lea_vol_setting)
    {
        vol_level = 1;
    }

    return vol_level;
}

T_LEA_VOL_CHG_RESULT app_lea_vol_local_volume_change(T_LEA_VOL_CHG_TYPE type)
{
    uint8_t level;
    T_LEA_VOL_CHG_RESULT result = LEA_VOL_NONE;
#if F_APP_VCS_SUPPORT
    T_VCS_PARAM vcs_param;

    if (vcs_get_param(&vcs_param) == false)
    {
        goto fail;
    }
    app_cfg_nv.lea_vol_setting = vcs_param.volume_setting;
    app_cfg_nv.lea_vol_out_mute = vcs_param.mute;
#endif

    switch (type)
    {
    case LEA_SPK_UP:
        {
            if (app_cfg_nv.lea_vol_setting != MAX_VOLUME_SETTING)
            {
                if ((MAX_VOLUME_SETTING - app_cfg_nv.lea_vol_setting) > VOLUME_STEP_SIZE)
                {
                    app_cfg_nv.lea_vol_setting += VOLUME_STEP_SIZE;
                }
                else
                {
                    app_cfg_nv.lea_vol_setting = MAX_VOLUME_SETTING;
                }
                result = LEA_VOL_LEVEL_CHANGE;
            }
            else
            {
                result = LEA_VOL_LEVEL_LIMIT;
                goto level_limit;
            }
        }
        break;

    case LEA_SPK_DOWN:
        {
            if (app_cfg_nv.lea_vol_setting != 0)
            {
                if (app_cfg_nv.lea_vol_setting > VOLUME_STEP_SIZE)
                {
                    app_cfg_nv.lea_vol_setting -= VOLUME_STEP_SIZE;
                }
                else
                {
                    app_cfg_nv.lea_vol_setting = 0;
                }
                result = LEA_VOL_LEVEL_CHANGE;
            }
            else
            {
                result = LEA_VOL_LEVEL_LIMIT;
                goto level_limit;
            }
        }
        break;

    case LEA_SPK_MUTE:
        {
            if (app_cfg_nv.lea_vol_out_mute != VCS_MUTED)
            {
                app_cfg_nv.lea_vol_out_mute = VCS_MUTED;
                result = LEA_VOL_MUTE_CHANGE;
            }
            else
            {
                result = LEA_VOL_MUTE_UNCHANGE;
                goto mute_unchange;
            }
        }
        break;

    case LEA_SPK_UNMUTE:
        {
            if (app_cfg_nv.lea_vol_out_mute != VCS_NOT_MUTED)
            {
                app_cfg_nv.lea_vol_out_mute = VCS_NOT_MUTED;
                result = LEA_VOL_MUTE_CHANGE;
            }
            else
            {
                result = LEA_VOL_MUTE_UNCHANGE;
                goto mute_unchange;
            }
        }
        break;

    default:
        break;
    }

    if (result == LEA_VOL_LEVEL_CHANGE)
    {
        level = VOLUME_LEVEL(app_cfg_nv.lea_vol_setting);
        if ((level == 0) && app_cfg_nv.lea_vol_setting)
        {
            level = 1;
        }

        if (level > 0)
        {
#if F_APP_VCS_SUPPORT
            vcs_param.mute = VCS_NOT_MUTED;
#endif
            app_cfg_nv.lea_vol_out_mute = VCS_NOT_MUTED;
        }
    }

#if F_APP_VCS_SUPPORT
    vcs_param.volume_setting = app_cfg_nv.lea_vol_setting;
    vcs_param.mute = app_cfg_nv.lea_vol_out_mute;
    vcs_param.volume_flags = VCS_USER_SET_VOLUME_SETTING;
    vcs_param.change_counter++;

    app_cfg_nv.lea_vcs_vol_flag = vcs_param.volume_flags;
    app_cfg_nv.lea_vcs_change_cnt = vcs_param.change_counter;

    vcs_set_param(&vcs_param);
#endif

fail:
level_limit:
mute_unchange:
    APP_PRINT_TRACE2("app_lea_vol_local_volume_change: type 0x%02X, ret %d", type, result);

    return result;
}

void app_lea_vol_update_track_volume(void)
{
    T_MTC_BT_MODE bt_mode = MULTI_PRO_BT_BREDR;
    T_AUDIO_TRACK_HANDLE track_handle = NULL;
    uint8_t level = 0;
    uint8_t curr_vol;
    uint8_t ret = 0;

#if F_APP_LEA_SUPPORT
    bt_mode = mtc_get_btmode();
#endif
    if (bt_mode == MULTI_PRO_BT_CIS)
    {
        T_APP_LE_LINK *p_link = NULL;
        T_LEA_ASE_ENTRY *p_ase_entry;
        uint16_t conn_handle = 0;

#if F_APP_CCP_SUPPORT
        if (app_lea_ccp_get_call_status() != APP_CALL_IDLE)
        {
            conn_handle = app_lea_ccp_get_active_conn_handle();
        }
        else
#endif
        {
#if F_APP_MCP_SUPPORT
            conn_handle = app_lea_mcp_get_active_conn_handle();
#endif
        }

        p_link = app_link_find_le_link_by_conn_handle(conn_handle);
        if (p_link == NULL)
        {
            ret = 1;
            goto fail;
        }

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT
        p_ase_entry = app_lea_ascs_find_ase_entry_by_direction(p_link, DATA_PATH_OUTPUT_FLAG);
        if (p_ase_entry == NULL)
        {
            ret = 2;
            goto fail;
        }

        track_handle = p_ase_entry->track_handle;
#endif
        if (track_handle == NULL)
        {
            ret = 3;
            goto fail;
        }
    }
    else if (bt_mode == MULTI_PRO_BT_BIS)
    {
        track_handle = app_lea_bca_get_track_handle();
        if (track_handle == NULL)
        {
            ret = 5;
            goto fail;
        }
    }
    else
    {
        ret = 6;
        goto fail;
    }

    level = VOLUME_LEVEL(app_cfg_nv.lea_vol_setting);
    if ((level == 0) && app_cfg_nv.lea_vol_setting)
    {
        level = 1;
    }

    if (audio_track_volume_out_get(track_handle, &curr_vol) && (level != curr_vol))
    {
#if HARMAN_OPEN_LR_FEATURE
        app_harman_lr_balance_set(AUDIO_STREAM_TYPE_PLAYBACK, level, __func__, __LINE__);
#endif
        audio_track_volume_out_set(track_handle, level);
    }

    if (app_cfg_nv.lea_vol_out_mute == VCS_MUTED)
    {
        audio_track_volume_out_mute(track_handle);
    }
    else
    {
        audio_track_volume_out_unmute(track_handle);
    }

fail:
    APP_PRINT_ERROR4("app_lea_vol_update_track_volume: bt_mode 0x%02X, level %d, mute %d, ret %d",
                     bt_mode, level, app_cfg_nv.lea_vol_out_mute, -ret);
}

void app_lea_vol_sync_info(void)
{
#if F_APP_VCS_SUPPORT
    if (app_lea_vcs_find_vc_link())
    {
        return;
    }
#endif

#if F_APP_ERWS_SUPPORT
    if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        T_LEA_VOL_SYNC_INFO vol_sync_info;

        vol_sync_info.vol_gain = app_cfg_nv.lea_vol_setting;
        vol_sync_info.mute = app_cfg_nv.lea_vol_out_mute;
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_LEA_VOL_POLICY,
                                            LEA_VOL_MSG_VOLUME_SYNC,
                                            (uint8_t *)&vol_sync_info, sizeof(T_LEA_VOL_SYNC_INFO));
    }
#endif
}

#if F_APP_ERWS_SUPPORT

static uint16_t app_lea_vol_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_LEA_VOL_POLICY, 0, NULL, true, total);
}

static void app_lea_vol_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                    T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_lea_vol_parse_cback: msg_type 0x%02X, status 0x%02X", msg_type, status);
    switch (msg_type)
    {
    case LEA_VOL_MSG_VOLUME_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                T_LEA_VOL_SYNC_INFO *info = (T_LEA_VOL_SYNC_INFO *)buf;

                if ((app_cfg_nv.lea_vol_setting != info->vol_gain) || (app_cfg_nv.lea_vol_out_mute != info->mute))
                {
                    app_cfg_nv.lea_vol_setting = info->vol_gain;
                    app_cfg_nv.lea_vol_out_mute = info->mute;

#if F_APP_VCS_SUPPORT
                    T_VCS_PARAM vcs_param;

                    if (vcs_get_param(&vcs_param))
                    {
                        vcs_param.volume_setting = app_cfg_nv.lea_vol_setting;
                        vcs_param.mute = app_cfg_nv.lea_vol_out_mute;
                        vcs_param.volume_flags = VCS_USER_SET_VOLUME_SETTING;
                        vcs_param.change_counter++;

                        app_cfg_nv.lea_vcs_vol_flag = vcs_param.volume_flags;
                        app_cfg_nv.lea_vcs_change_cnt = vcs_param.change_counter;
                        vcs_set_param(&vcs_param);
                    }
#endif
                    app_lea_vol_update_track_volume();
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

static uint16_t app_lea_vol_ble_audio_cback(T_LE_AUDIO_MSG msg, void *buf)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    bool handle = true;

    switch (msg)
    {
    case LE_AUDIO_MSG_VCS_VOLUME_CP_IND:
        {
            T_VCS_VOLUME_CP_IND *p_vcs_vol_state = (T_VCS_VOLUME_CP_IND *)buf;

            APP_PRINT_TRACE3("app_lea_vol_ble_audio_cback: LE_AUDIO_MSG_VCS_VOLUME_CP_IND \
conn_handle 0x%02X, volume_setting 0x%02X, mute %d",
                             p_vcs_vol_state->conn_handle,
                             p_vcs_vol_state->volume_setting,
                             p_vcs_vol_state->mute);

            app_cfg_nv.lea_vol_setting = p_vcs_vol_state->volume_setting;
            app_cfg_nv.lea_vol_out_mute = p_vcs_vol_state->mute;
            app_cfg_nv.lea_vcs_vol_flag = VCS_USER_SET_VOLUME_SETTING;

            app_lea_vol_update_track_volume();
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_TRACE1("app_lea_vol_ble_audio_cback: msg 0x%04X", msg);
    }

    return cb_result;
}

void app_lea_vol_init(void)
{
    ble_audio_cback_register(app_lea_vol_ble_audio_cback);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_lea_vol_relay_cback, app_lea_vol_parse_cback,
                             APP_MODULE_TYPE_LEA_VOL_POLICY, LEA_VOL_MSG_TOTAL);
#endif
}
