#if GFPS_SASS_SUPPORT
#include <stdlib.h>
#include <string.h>
#include "bt_bond.h"
#include "trace.h"
#include "gap_br.h"
#include "app_cfg.h"
#include "app_link_util.h"
#include "app_main.h"
#include "app_bt_policy_api.h"
#include "app_multilink.h"
#include "app_mmi.h"
#include "app_sass_policy.h"
#include "app_bond.h"
#include "bt_hfp.h"
#include "app_gfps.h"
#include "app_a2dp.h"
#include "app_hfp.h"
#include "app_multilink.h"
#if F_APP_OTA_SUPPORT
#include "app_ota.h"
#endif
#include "gfps_sass_conn_status.h"

#if GFPS_SASS_LEA_SUPPORT
#include "metadata_def.h"
#include "mcp_def.h"
#include "mcp_client.h"
#include "app_main.h"
#include "app_timer.h"
#include "app_ble_gap.h"
#include "app_gfps_msg.h"
#include "app_lea_adv.h"
#include "app_lea_ascs.h"
#include "app_lea_unicast_audio.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_mcp.h"
#include "app_lea_mgr.h"
#endif

typedef enum
{
    LINK_SWITCH_IDLE,
    LINK_SWITCH_SWITCHING,
} T_APP_SASS_LINK_SWITCH_STATE;

typedef enum
{
    SWITCH_BACK = 0x01,
    SWITCH_BACK_AND_RESUME_PLAYING = 0x02,
} T_APP_SASS_SWITCH_BACK_EVENT;

typedef enum
{
    APP_SASS_TIMER_SWITCH_BACK  = 0x00,
} T_APP_SASS_TIMER;

static uint8_t original_enable_multi_link = 0xff;
static T_APP_SASS_LINK_SWITCH_STATE link_switch_state = LINK_SWITCH_IDLE;
static bool link_switch_resume = false;
static uint8_t prev_conn_addr[6] = {0};
static T_SASS_LINK_TYPE prev_conn_addr_type = SASS_TYPE_UNKNOW;
static uint8_t prev_disc_addr[6] = {0};
static T_SASS_LINK_TYPE prev_disc_addr_type = SASS_TYPE_UNKNOW;
static uint8_t app_sass_timer_id = 0;
static uint8_t timer_idx_switch_back = 0;

/* Switching active audio source event flags */
#define GFPS_SWITCH_TO_THIS_DEVICE  0x80    // Bit 0: 1 switch to this device, 0 switch to second connected device
#define GFPS_RESUME_PLAYING         0x40    // Bit 1: 1 resume playing on switch to device after switching, 0 otherwise.
#define GFPS_REJECT_SCO             0x20    // Bit 2: 1 reject SCO on switched away device, 0 otherwise
#define GFPS_DISCONN_BLUETOOTH      0x10    // Bit 3: 1 disconnect Bluetooth on switch away device, 0 otherwise.

void app_sass_policy_reset_prev_disc_device(void)
{
    APP_PRINT_INFO0("app_sass_policy_reset_prev_disc_device");
    memset(prev_disc_addr, 0, sizeof(prev_disc_addr));
    prev_disc_addr_type = SASS_TYPE_UNKNOW;
}

void app_sass_policy_sync_conn_bit_map(uint8_t bitmap)
{
    app_db.conn_bitmap = bitmap;
    APP_PRINT_TRACE1("app_sass_policy_sync_conn_bit_map: %d",  app_db.conn_bitmap);
}

uint8_t app_sass_policy_get_conn_bit_map(void)
{
    APP_PRINT_TRACE2("app_sass_policy_get_conn_bit_map: %d %d", app_cfg_nv.sass_bitmap,
                     app_db.conn_bitmap);
    return app_db.conn_bitmap;
}

bool app_sass_policy_update_conn_bit_map(uint8_t *bd_addr, bool is_conn)
{
    bool ret = false;
#if F_APP_ERWS_SUPPORT
    uint8_t peer_addr[6] = {0};
    remote_peer_addr_get(peer_addr);

    if (memcmp(peer_addr, bd_addr, 6))
#endif
    {
        uint8_t idx = 0;

        if (app_bond_get_pair_idx_mapping(bd_addr, &idx))
        {
            if (is_conn)
            {
                app_db.conn_bitmap |= (1 << idx);
            }
            else
            {
                app_db.conn_bitmap &= ~(1 << idx);
            }

#if F_APP_ERWS_SUPPORT
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_relay_async_single(APP_MODULE_TYPE_GFPS_SASS,
                                       APP_REMOTE_MSG_SASS_DEVICE_BITMAP_SYNC);
            }
#endif
            ret = true;
        }
    }

    return ret;
}

static void app_sass_policy_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    bool handle = true;
    T_APP_BR_LINK *p_link = NULL;
    T_BT_EVENT_PARAM *param = event_buf;

    APP_PRINT_INFO2("app_sass_policy_bt_cback: event_type 0x%x, switch_state 0x%x", event_type,
                    link_switch_state);

    switch (event_type)
    {
    case BT_EVENT_ACL_CONN_SUCCESS:
        // case BT_EVENT_LINK_KEY_INFO:
        {
            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
            {
                if (app_sass_policy_update_conn_bit_map(param->acl_conn_success.bd_addr, true))
                {
                    app_gfps_notify_conn_status();
                }

                if ((prev_disc_addr_type == SASS_EDR_LINK) &&
                    !memcmp(prev_disc_addr, param->acl_conn_success.bd_addr, 6))
                {
                    app_sass_policy_reset_prev_disc_device();
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            if (param->acl_conn_disconn.cause != (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) ||
                    (app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE))
                {
                    uint8_t idx;

                    if (app_sass_policy_update_conn_bit_map(param->acl_conn_disconn.bd_addr, false))
                    {
                        app_gfps_notify_conn_status();
                    }

                    app_bond_get_pair_idx_mapping(param->acl_conn_disconn.bd_addr, &idx);

                    if (link_switch_state == LINK_SWITCH_SWITCHING)
                    {
                        if (prev_disc_addr_type == SASS_EDR_LINK)
                        {
                            app_bt_policy_default_connect(prev_disc_addr,
                                                          (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK | HFP_PROFILE_MASK), true);
                        }
                        else
                        {
                            app_lea_adv_start();
                        }
                    }
                    else
                    {
                        memcpy(prev_disc_addr, param->acl_conn_disconn.bd_addr, 6);
                        prev_disc_addr_type = SASS_EDR_LINK;
                        APP_PRINT_INFO1("app_sass_policy_bt_cback: edr prev_disc_addr %s", TRACE_BDADDR(prev_disc_addr));
                    }
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
        APP_PRINT_INFO1("app_sass_policy_bt_cback: event_type 0x%04x", event_type);
    }
}
#if GFPS_SASS_LEA_SUPPORT
void app_sass_handle_le_dm(uint8_t conn_id, T_SASS_LE_EVENT event, uint8_t *p_data)
{
    APP_PRINT_INFO2("app_sass_handle_le_dm: conn_id %d, event 0x%x", conn_id, event);

    T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_id(conn_id);

    switch (event)
    {
    case SASS_LE_DISCONNECTING:
        {
            T_SASS_DISCONNECT_PARAM *p_disc = (T_SASS_DISCONNECT_PARAM *)p_data;
            if (p_disc)
            {
                if (!memcmp(p_disc->bd_addr, app_db.sass_target_drop_device, 6))
                {
                    uint8_t reset_value[6] = {0};
                    app_gfps_msg_update_target_drop_device(0xFF, reset_value);
                }

                if (app_sass_policy_update_conn_bit_map(p_disc->bd_addr, false))
                {
                    // app_gfps_notify_conn_status();
                }

                if (link_switch_state == LINK_SWITCH_SWITCHING)
                {
                    if (prev_disc_addr_type == SASS_LE_LINK)
                    {
                        app_lea_adv_start();
                    }
                    else
                    {
                        app_bt_policy_default_connect(prev_disc_addr,
                                                      (A2DP_PROFILE_MASK | AVRCP_PROFILE_MASK | HFP_PROFILE_MASK), true);
                    }
                }
                else
                {
                    memcpy(prev_disc_addr, p_disc->bd_addr, 6);
                    prev_disc_addr_type = SASS_LE_LINK;
                    APP_PRINT_INFO1("app_sass_policy_bt_cback: le prev_disc_addr %s", TRACE_BDADDR(prev_disc_addr));
                }
            }
        }
        break;

    case SASS_LE_DISCONNECTED:
        {
            app_gfps_notify_conn_status();
        }
        break;

    case SASS_LE_CONNECTED:
        {
        }
        break;

    case SASS_LE_AUTHEN_COMPL:
        {
            if (p_link)
            {
                if (app_link_get_encryption_device_num() > app_db.b2s_connected_num_max)
                {
                    app_multi_disconnect_inactive_link(p_link->id, SASS_LE_LINK);

                    app_lea_mgr_disc_inactive_le_link(p_link->id, SASS_LE_LINK);
                }

                if (app_sass_policy_update_conn_bit_map(p_link->bd_addr, true))
                {
                    app_gfps_notify_conn_status();
                }

                if ((prev_disc_addr_type == SASS_LE_LINK) && !memcmp(prev_disc_addr, p_link->bd_addr, 6))
                {
                    app_stop_timer(&timer_idx_switch_back);
                    app_sass_policy_reset_prev_disc_device();
                    app_sass_policy_link_back_end();
                }
            }
        }
        break;

    case SASS_LE_DSICOV_DONE:
        {
            if (p_link)
            {
                app_sass_policy_profile_conn_handle(p_link->id, SASS_LE_LINK);
            }
        }
        break;

    default:
        break;
    }
}
#endif

void app_sass_policy_link_back_end(void)
{
    link_switch_state = LINK_SWITCH_IDLE;
}

uint8_t app_sass_find_other_idx(uint8_t app_idx)
{
    uint8_t another_idx = MAX_BR_LINK_NUM;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if ((i != app_idx) &&
            (app_db.br_link[i].connected_profile & (A2DP_PROFILE_MASK | HFP_PROFILE_MASK | HSP_PROFILE_MASK)))
        {
            another_idx = i;
            break;
        }
    }

    return another_idx;
}

void *app_sass_find_other_link(uint8_t active_id, T_SASS_LINK_TYPE input_link_type,
                               T_SASS_LINK_TYPE *found_link_type)
{
    void *other_link = NULL;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (((input_link_type == SASS_LE_LINK) || (i != active_id)) && app_db.br_link[i].connected_profile)
        {
            other_link = &app_db.br_link[i];
            if (found_link_type)
            {
                *found_link_type = SASS_EDR_LINK;
            }
            return other_link;
        }
    }

#if GFPS_SASS_LEA_SUPPORT
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (((input_link_type == SASS_EDR_LINK) || (i != active_id)) &&
            (app_db.le_link[i].state == LE_LINK_STATE_CONNECTED))
        {
            other_link = (void *)&app_db.le_link[i];
            if (found_link_type)
            {
                *found_link_type = SASS_LE_LINK;
            }
            return other_link;
        }
    }
#endif

    return NULL;
}

uint8_t app_sass_policy_switch_active_audio_source(uint8_t conn_id, uint8_t *bd_addr,
                                                   uint8_t switch_flag, bool self)
{
    // Switch active audio source (to connected device)
    uint8_t ret = SWITCH_RET_OK;
    uint8_t active_id = 0xFF;
    T_SASS_LINK_TYPE active_link_type = SASS_TYPE_UNKNOW;
    uint8_t switched_id = 0xFF;
    T_SASS_LINK_TYPE switched_link_type = SASS_TYPE_UNKNOW;
    T_SASS_LINK_TYPE other_link_type;

    APP_PRINT_TRACE3("app_sass_policy_switch_active: %d, %s, %x", conn_id, TRACE_BDADDR(bd_addr),
                     switch_flag);

    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
        active_id = p_link->id;
        active_link_type = SASS_EDR_LINK;
    }
    else
    {
        T_APP_LE_LINK *p_link = app_link_find_le_link_by_conn_id(conn_id);
        active_id = p_link->id;
        active_link_type = SASS_LE_LINK;
    }

    void *p_other_link = app_sass_find_other_link(active_id, active_link_type, &other_link_type);

    if (!self)
    {
        if (!p_other_link)
        {
            ret = SWITCH_RET_FAILED;
            return ret;
        }
        switched_id = active_id;
        switched_link_type = active_link_type;

        if (other_link_type == SASS_EDR_LINK)
        {
            T_APP_BR_LINK *p_found_link = (T_APP_BR_LINK *)p_other_link;
            active_id = p_found_link->id;
            active_link_type = SASS_EDR_LINK;
        }
        else
        {
            T_APP_LE_LINK *p_found_link = (T_APP_LE_LINK *)p_other_link;
            active_id = p_found_link->id;
            active_link_type = SASS_LE_LINK;
        }
    }
    else
    {
        if (p_other_link)
        {
            if (other_link_type == SASS_EDR_LINK)
            {
                T_APP_BR_LINK *p_found_link = (T_APP_BR_LINK *)p_other_link;
                switched_id = p_found_link->id;
                switched_link_type = SASS_EDR_LINK;
            }
            else
            {
                T_APP_LE_LINK *p_found_link = (T_APP_LE_LINK *)p_other_link;
                switched_id = p_found_link->id;
                switched_link_type = SASS_LE_LINK;
            }
        }

        if (!mtc_get_btmode())
        {
            if ((active_link_type == SASS_EDR_LINK) && (app_multi_get_active_idx() == active_id))
            {
                ret = SWITCH_RET_REDUNDANT;
            }
            app_multi_preemptive_judge(active_id, SASS_EDR_LINK, MULTILINK_SASS_FORCE_PREEM);
        }
        else
        {
            if (active_link_type == SASS_LE_LINK && app_lea_uca_get_stream_link() &&
                (app_lea_uca_get_stream_link()->id == active_id))
            {
                ret = SWITCH_RET_REDUNDANT;
            }
            app_multi_preemptive_judge(active_id, SASS_LE_LINK, MULTILINK_SASS_FORCE_PREEM);// add le action
        }
    }

    if (switch_flag & GFPS_RESUME_PLAYING)
    {
        //need record the avrcp/mcp state before switched away
        if (active_link_type == SASS_EDR_LINK)
        {
            bt_avrcp_play(app_db.br_link[active_id].bd_addr);
        }
        else
        {
            app_lea_mcp_play_pause(app_db.le_link[active_id].conn_handle, true);
        }
    }

    if (switch_flag & GFPS_REJECT_SCO &&
        !(switch_flag & GFPS_SWITCH_TO_THIS_DEVICE) &&
        !(switch_flag & GFPS_DISCONN_BLUETOOTH))
    {
        //reject sco means disconnect sco or end the call?
        if (switched_link_type == SASS_EDR_LINK)
        {
            if (app_db.br_link[switched_id].sco_handle)
            {
                bt_hfp_audio_disconnect_req(app_db.br_link[switched_id].bd_addr);
            }
        }
        else
        {
            app_lea_uca_link_sm(app_db.le_link[switched_id].conn_handle, LEA_PAUSE_LOCAL, NULL);
        }
    }

    if (switch_flag & GFPS_DISCONN_BLUETOOTH)
    {
        if (switched_link_type == SASS_EDR_LINK)
        {
            app_bt_policy_disconnect(app_db.br_link[switched_id].bd_addr, ALL_PROFILE_MASK);
        }
        else
        {
#if F_APP_ERWS_SUPPORT
            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
            {
                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_GFPS_SASS, APP_REMOTE_MSG_SASS_DISC_LE_LINK_SYNC,
                                                   app_db.br_link[switched_id].bd_addr, 6, REMOTE_TIMER_HIGH_PRECISION,
                                                   0, false);
            }
            else
#endif
            {
                app_ble_gap_disconnect(&app_db.le_link[switched_id], LE_LOCAL_DISC_CAUSE_GFPS_SASS_DISC);
            }

        }
    }

    return ret;
}

void app_sass_policy_set_multipoint_state(uint8_t enable)
{
    // Multipoint state:    0: switch off multipoint  1: switch on multipoint
    if (app_cfg_nv.sass_multipoint_state == enable)
    {
        return;
    }
    APP_PRINT_INFO1("app_sass_policy_set_multipoint_state: %d", enable);

    if (enable)
    {
        app_cfg_const.enable_multi_link = 1;
        app_cfg_nv.sass_multipoint_state = 1;
        app_cfg_const.max_legacy_multilink_devices = 2;
        app_db.b2s_connected_num_max = app_cfg_const.max_legacy_multilink_devices;
        app_mmi_handle_action(MMI_DEV_ENTER_PAIRING_MODE);
    }
    else
    {
        bool need_disc = app_sass_get_available_connection_num() == 0;
        app_cfg_const.enable_multi_link = 0;
        app_cfg_nv.sass_multipoint_state = 0;
        app_cfg_const.max_legacy_multilink_devices = 1;
        app_db.b2s_connected_num_max = app_cfg_const.max_legacy_multilink_devices;

        if (need_disc)
        {
            app_multi_disconnect_inactive_link(MAX_BR_LINK_NUM, SASS_TYPE_UNKNOW);
#if GFPS_SASS_LEA_SUPPORT
            app_lea_mgr_disc_inactive_le_link(MAX_BR_LINK_NUM, SASS_TYPE_UNKNOW);
#endif
        }
    }

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single(APP_MODULE_TYPE_GFPS_SASS, APP_REMOTE_MSG_SASS_MULTILINK_STATE_SYNC);
    }
#endif
}

void app_sass_policy_profile_conn_handle(uint8_t idx, T_SASS_LINK_TYPE link_type)
{
    APP_PRINT_TRACE2("app_sass_policy_profile_conn_handle %s, %d", TRACE_BDADDR(prev_conn_addr),
                     link_type);

    if (link_switch_resume)
    {
        if ((prev_conn_addr_type == SASS_EDR_LINK) &&
            (memcmp(prev_conn_addr, app_db.br_link[idx].bd_addr, 6) == 0))
        {
            if ((app_db.br_link[idx].connected_profile & A2DP_PROFILE_MASK) &&
                (app_db.br_link[idx].connected_profile & AVRCP_PROFILE_MASK))
            {
                bt_avrcp_play(app_db.br_link[idx].bd_addr);
            }
        }
        else if ((prev_conn_addr_type == SASS_LE_LINK) &&
                 (memcmp(prev_conn_addr, app_db.le_link[idx].bd_addr, 6) == 0))
        {
            app_lea_mcp_play_pause(app_db.le_link[idx].conn_handle, true);
        }
        link_switch_resume = false;
        memset(prev_conn_addr, 0, 6);
    }
}

uint8_t app_sass_policy_get_disc_link(uint8_t *link_type)
{
    uint8_t count = 0;
    uint8_t bit = app_sass_policy_get_conn_bit_map();
    uint8_t disc_link = MAX_BR_LINK_NUM;
    uint8_t max_link_num = app_cfg_const.enable_multi_link ?
                           app_cfg_const.max_legacy_multilink_devices : 1;

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_sass_policy_is_target_drop_device(i, SASS_EDR_LINK))
        {
            disc_link = i;
            *link_type = SASS_EDR_LINK;
            goto ret;
        }
    }

#if GFPS_SASS_LEA_SUPPORT
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (app_sass_policy_is_target_drop_device(i, SASS_LE_LINK))
        {
            disc_link = i;
            *link_type = SASS_LE_LINK;
            goto ret;
        }

        if ((app_db.le_link[i].used) && (app_db.le_link[i].gfps_inuse_account_key != 0xFF))
        {
            count++;
        }
    }
#endif

    for (uint8_t i = 0; i < 8; i++)
    {
        if (1 << i & bit)
        {
            count++;
        }
    }

    if ((count == 0) || (count == max_link_num)) //all sass or no sass disc inactive device
    {
        disc_link = MAX_BR_LINK_NUM;
    }
    else
    {
#if GFPS_SASS_LEA_SUPPORT
        if (mtc_get_btmode())
        {
            T_APP_LE_LINK *p_le_link = app_lea_uca_get_active_link();

            if (p_le_link && (p_le_link->gfps_inuse_account_key != 0xFF))
            {
                if ((p_le_link->lea_link_state == LEA_LINK_STREAMING) ||
                    (p_le_link->media_state == MCS_MEDIA_STATE_PLAYING) ||
                    (p_le_link->call_status != APP_CALL_IDLE))
                {
                    disc_link = MAX_BLE_LINK_NUM;
                }
                else
                {
                    disc_link = p_le_link->id;//??
                    *link_type = SASS_LE_LINK;
                }
            }
        }

        else
#endif
        {
            T_APP_BR_LINK *p_link;
            T_APP_BR_LINK *other_link;

            p_link = app_link_find_br_link(app_db.br_link[app_multi_get_active_idx()].bd_addr);

            if (app_sass_policy_is_sass_device(p_link->bd_addr))
            {
                //active SASS, disconnect non sass if SASS is streaming or call
                if (p_link->streaming_fg || p_link->call_status != BT_HFP_CALL_IDLE)
                {
                    disc_link = MAX_BR_LINK_NUM;
                }
                else
                {
                    disc_link = p_link->id;//??
                    *link_type = SASS_EDR_LINK;
                }
            }
            else //active is Non SASS, or SASS active bur idle, means always disconnect inactive(SASS)
            {
                disc_link = MAX_BR_LINK_NUM;
            }
        }
    }

ret:
    APP_PRINT_TRACE6("app_sass_policy_get_disc_link %d %d %d %d %d %d", bit, count, max_link_num,
                     disc_link, app_multi_get_active_idx(), *link_type);
    return disc_link;
}

void app_sass_policy_switch_back(uint8_t event)
{
    bool resume = false;
    uint8_t resume_idx = 0xFF;


    if (event == SWITCH_BACK_AND_RESUME_PLAYING)
    {
        resume = true;
    }

    APP_PRINT_TRACE5("app_sass_policy_switch_back: %s, %d, %d, %d, %d", TRACE_BDADDR(prev_disc_addr),
                     prev_disc_addr_type, resume, link_switch_state, app_cfg_const.enable_multi_link);

    if (link_switch_state != LINK_SWITCH_IDLE)
    {
        return;
    }

    link_switch_state = LINK_SWITCH_SWITCHING;
    app_start_timer(&timer_idx_switch_back, "sass_switch_back",
                    app_sass_timer_id, APP_SASS_TIMER_SWITCH_BACK, 0, false,
                    180000);//3min


    if (resume)
    {
        if (app_cfg_const.enable_multi_link)
        {
            APP_PRINT_TRACE2("app_sass_policy_switch_back: num %d, %d", app_link_get_b2s_link_num(),
                             app_link_get_lea_link_num());

            if (app_link_get_b2s_link_num()
#if GFPS_SASS_LEA_SUPPORT
                + app_link_get_lea_link_num()
#endif
                > 1)
            {
                if (prev_disc_addr_type == SASS_EDR_LINK)
                {
                    for (uint8_t i = 0 ; i < MAX_BR_LINK_NUM; i++)
                    {
                        if (app_link_check_b2s_link_by_id(i) && //app_link_check_b2b_link_by_id(i)
                            (memcmp(prev_disc_addr, app_db.br_link[i].bd_addr, 6) != 0))
                        {
                            resume_idx = i;
                            break;
                        }
                    }

                    if (resume_idx != MAX_BR_LINK_NUM)
                    {
                        bt_avrcp_play(app_db.br_link[resume_idx].bd_addr);
                        app_multi_preemptive_judge(resume_idx, SASS_EDR_LINK, MULTILINK_SASS_FORCE_PREEM);
                    }
                }
                else if (prev_disc_addr_type == SASS_LE_LINK)
                {
                    for (uint8_t i = 0 ; i < MAX_BLE_LINK_NUM; i++)
                    {
                        if (app_db.le_link[i].used &&
                            (memcmp(prev_disc_addr, app_db.le_link[i].bd_addr, 6) != 0))
                        {
                            resume_idx = i;
                            break;
                        }
                    }

                    if (resume_idx != MAX_BLE_LINK_NUM)
                    {
                        app_lea_mcp_play_pause(app_db.le_link[resume_idx].conn_handle, true);
                        app_multi_preemptive_judge(resume_idx, SASS_LE_LINK, MULTILINK_SASS_FORCE_PREEM);
                    }
                }
                else
                {
                    T_SASS_LINK_TYPE other_link_type = SASS_TYPE_UNKNOW;
                    uint8_t active_idx = 0xFF;
                    T_SASS_LINK_TYPE active_link_type = SASS_TYPE_UNKNOW;

                    if (!mtc_get_btmode())
                    {
                        active_idx = app_multi_get_active_idx();
                        active_link_type = SASS_EDR_LINK;
                    }
                    else
                    {
                        active_idx = (app_lea_uca_get_stream_link() == NULL) ? 0xFF : app_lea_uca_get_stream_link()->id;
                        active_link_type = SASS_LE_LINK;
                    }

                    void *p_other_link = app_sass_find_other_link(active_idx, active_link_type, &other_link_type);
                    APP_PRINT_INFO1("app_sass_policy_switch_back: other type %d", other_link_type);

                    if (p_other_link && (other_link_type != SASS_TYPE_UNKNOW))
                    {
                        if (other_link_type == SASS_EDR_LINK)
                        {
                            T_APP_BR_LINK *p_found_link = (T_APP_BR_LINK *)p_other_link;
                            resume_idx = p_found_link->id;
                            APP_PRINT_INFO1("app_sass_policy_switch_back: edr resume_idx %d", resume_idx);
                            bt_avrcp_play(app_db.br_link[resume_idx].bd_addr);
                            // app_multi_preemptive_judge(resume_idx, SASS_EDR_LINK, MULTILINK_SASS_FORCE_PREEM);
                        }
                        else
                        {
                            T_APP_LE_LINK *p_found_link = (T_APP_LE_LINK *)p_other_link;
                            resume_idx = p_found_link->id;
                            APP_PRINT_INFO1("app_sass_policy_switch_back: le resume_idx %d", resume_idx);
                            app_lea_mcp_play_pause(app_db.le_link[resume_idx].conn_handle, true);
                            // app_multi_preemptive_judge(resume_idx, SASS_LE_LINK, MULTILINK_SASS_FORCE_PREEM);

                        }
                    }
                    link_switch_state = LINK_SWITCH_IDLE;
                }
            }
        }
        else
        {
            memcpy(prev_conn_addr, prev_disc_addr, 6);
            prev_conn_addr_type = prev_disc_addr_type;
            link_switch_resume = resume;
        }
    }
}

void app_sass_policy_set_capability(uint8_t conn_id, uint8_t *addr)
{
    uint8_t idx = 0;

    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(addr);
        if (p_link)
        {
            app_bond_get_pair_idx_mapping(addr, &idx);
        }
    }
    else
    {
        T_APP_LE_LINK *p_le_link = app_link_find_le_link_by_conn_id(conn_id);
        if (p_le_link)
        {
            app_bond_get_pair_idx_mapping(p_le_link->bd_addr, &idx);
        }
    }
    app_cfg_nv.sass_bitmap |= (1 << idx);//??

#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single(APP_MODULE_TYPE_GFPS_SASS, APP_REMOTE_MSG_SASS_DEVICE_SUPPORT_SYNC);
    }
#endif
    app_gfps_notify_conn_status();

    APP_PRINT_TRACE3("app_sass_policy_set_capability conn_id 0x%x, addr %s, sass_bitmap 0x%x", conn_id,
                     TRACE_BDADDR(addr), app_cfg_nv.sass_bitmap);
}

void app_sass_policy_initiated_connection(uint8_t conn_id, uint8_t *addr,
                                          bool triggered_by_audio_switch)
{
    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(addr);

        APP_PRINT_TRACE4("app_sass_policy_initiated_connection_edr addr %s, %d, %x, %x", TRACE_BDADDR(addr),
                         p_link->streaming_fg,
                         app_cfg_nv.sass_bitmap, triggered_by_audio_switch);

        if (triggered_by_audio_switch && p_link->streaming_fg)
        {
            app_multi_preemptive_judge(p_link->id, SASS_EDR_LINK, MULTILINK_SASS_FORCE_PREEM);
        }
    }
    else
    {
        T_APP_LE_LINK *p_le_link = app_link_find_le_link_by_conn_id(conn_id);

        APP_PRINT_TRACE3("app_sass_policy_initiated_connection_le 0x%x, %x, %x", p_le_link->conn_handle,
                         app_cfg_nv.sass_bitmap, triggered_by_audio_switch);

        if (triggered_by_audio_switch)
        {
            app_multi_preemptive_judge(p_le_link->id, SASS_LE_LINK, MULTILINK_SASS_FORCE_PREEM);
        }
    }
}

bool app_sass_policy_is_sass_device(uint8_t *addr)
{
    bool ret = true;
    uint8_t idx;

    if (app_multi_get_active_idx() < MAX_BR_LINK_NUM)
    {
        app_bond_get_pair_idx_mapping(addr, &idx);

        if (!(app_cfg_nv.sass_bitmap & (1 << idx)))
        {
            ret = false;
        }
    }

    APP_PRINT_TRACE2("app_sass_policy_is_sass_device: %d %d", idx, ret);

    return ret;
}

bool app_sass_policy_is_target_drop_device(uint8_t idx, T_SASS_LINK_TYPE link_type)
{
    bool ret = false;
    uint8_t null_addr[6] = {0};

    if (link_type == SASS_EDR_LINK)
    {
        if (app_db.br_link[idx].used &&
            !memcmp(app_db.br_link[idx].bd_addr, app_db.sass_target_drop_device, 6) &&
            memcmp(app_db.sass_target_drop_device, null_addr, 6))
        {
            APP_PRINT_TRACE1("app_sass_policy_is_target_drop_device_edr %b",
                             TRACE_BDADDR(app_db.br_link[idx].bd_addr));
            ret = true;
        }
    }
#if GFPS_SASS_LEA_SUPPORT
    else
    {
        if (app_db.le_link[idx].used &&
            !memcmp(app_db.le_link[idx].bd_addr, app_db.sass_target_drop_device, 6) &&
            memcmp(app_db.sass_target_drop_device, null_addr, 6))
        {
            APP_PRINT_TRACE1("app_sass_policy_is_target_drop_device_le %b",
                             TRACE_BDADDR(app_db.le_link[idx].bd_addr));
            ret = true;
        }
    }
#endif

    return ret;
}

void app_sass_policy_set_switch_preference(uint8_t flag)
{
    uint16_t bit_mask = 0xFF0F;//0b11111111 1111 0000;//0xFFF0

    app_cfg_nv.sass_switching_preference &= bit_mask;
    app_cfg_nv.sass_switching_preference |= (uint16_t)(flag & ~bit_mask);//??MSB
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single(APP_MODULE_TYPE_GFPS_SASS, APP_REMOTE_MSG_SASS_PREEM_BIT_SYNC);
    }
#endif
    APP_PRINT_TRACE2("app_sass_policy_set_switch_preference %x, flag %x",
                     app_cfg_nv.sass_switching_preference, flag);
}

uint8_t app_sass_policy_get_switch_preference(void)
{
    uint8_t bit_mask = 0xF0;//0b00001111;//wrong
    return (uint8_t) app_cfg_nv.sass_switching_preference & (bit_mask);//!!wrong
}

void app_sass_policy_set_advanced_switching_setting(uint8_t flag)
{
    app_cfg_nv.sass_advanced_switching_setting = flag;
#if F_APP_ERWS_SUPPORT
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single(APP_MODULE_TYPE_GFPS_SASS, APP_REMOTE_MSG_SASS_SWITCH_SYNC);
    }
#endif
}

#if GFPS_SASS_LEA_SUPPORT
T_SASS_CONN_STATE app_sass_policy_context_to_conn_state(T_APP_LE_LINK *p_link)
{
    T_SASS_CONN_STATE conn_state = SASS_CONN_NO_DATA_TRANS;

    if (!p_link)
    {
        conn_state = SASS_NO_CONN;
        return conn_state;
    }

    if (p_link->call_status != APP_CALL_IDLE)
    {
        conn_state = SASS_LEA_CALL;
    }
    // else if (p_link->media_state == MCS_MEDIA_STATE_PLAYING)
    // {
    //     conn_state = SASS_LEA_MEDIA_WITH_CONTROL;
    // }
    else
    {
        T_LEA_ASE_ENTRY *p_ase_entry;

        for (uint8_t i = 0; i < ASCS_ASE_ENTRY_NUM; i++)
        {
            p_ase_entry = &p_link->lea_ase_entry[i];
            if (!p_ase_entry->used)
            {
                continue;
            }

            uint16_t audio_context = p_ase_entry->audio_context;
            APP_PRINT_TRACE2("app_sass_policy_context_to_conn_state: i %d, context 0x%x", i, audio_context);

            if (audio_context & (AUDIO_CONTEXT_CONVERSATIONAL | AUDIO_CONTEXT_VOICE_ASSISTANTS |
                                 AUDIO_CONTEXT_LIVE | AUDIO_CONTEXT_RINGTONE | AUDIO_CONTEXT_EMERGENCY_ALERT))
            {
                conn_state = SASS_LEA_CALL;
                break;
            }
            else if (audio_context & AUDIO_CONTEXT_MEDIA)
            {
                conn_state = SASS_LEA_MEDIA_WITH_CONTROL;
                break;
            }
            else if (audio_context & (AUDIO_CONTEXT_GAME | AUDIO_CONTEXT_INSTRUCTIONAL | AUDIO_CONTEXT_ALERTS))
            {
                conn_state = SASS_LEA_MEDIA_WITHOUT_CONTROL;
                break;
            }
            else // if (audio_context & (AUDIO_CONTEXT_SOUND_EFFECTS | AUDIO_CONTEXT_NOTIFICATIONS | AUDIO_CONTEXT_UNSPECIFIED))
            {
                conn_state = SASS_CONN_NO_DATA_TRANS;
                break;
            }
        }
    }

    APP_PRINT_TRACE1("app_sass_policy_context_to_conn_state %d", conn_state);
    return conn_state;
}
#endif

uint8_t app_sass_policy_get_advanced_switching_setting(void)
{
    return app_cfg_nv.sass_advanced_switching_setting;
}

T_SASS_CONN_STATE app_sass_policy_get_connection_state(void)
{
    if (extend_app_cfg_const.gfps_sass_support == 0)
    {
        APP_PRINT_TRACE0("Not support sass");
        return SASS_DISABLE_CONN_SWITCH;
    }

    T_SASS_CONN_STATE ret = SASS_DISABLE_CONN_SWITCH;
    APP_PRINT_INFO2("app_sass_policy_get_connection_state: b2s %d, le %d", app_link_get_b2s_link_num(),
                    app_link_get_le_link_num());

    if ((app_link_get_b2s_link_num() == 0)
#if GFPS_SASS_LEA_SUPPORT
        && (app_link_get_le_link_num() == 0)
#endif
       )
    {
        if (linkback_todo_queue_node_num())
        {
            ret = SASS_PAGING;
        }

#if F_APP_OTA_SUPPORT
        else if (app_ota_dfu_is_busy())
        {
            ret = SASS_DISABLE_CONN_SWITCH;
        }
        else
#endif
        {
            ret = SASS_NO_CONN;
        }
    }
    else
    {
#if F_APP_OTA_SUPPORT
        if (app_ota_dfu_is_busy())
        {
            ret = SASS_DISABLE_CONN_SWITCH;
        }
        else
#endif
        {

            APP_PRINT_INFO1("app_sass_policy_get_connection_state: mode %d", mtc_get_btmode());
            ret = SASS_CONN_NO_DATA_TRANS;
            if (mtc_get_btmode() == MULTI_PRO_BT_BREDR)
            {
                uint8_t active_idx = app_multi_get_active_idx();
                T_APP_BR_LINK *p_link = app_link_find_br_link(app_db.br_link[active_idx].bd_addr);

                if (p_link)
                {
                    APP_PRINT_INFO4("app_sass_policy_get_connection_state: id %d, call_state %d, stream %d,stream_only %d",
                                    active_idx, p_link->call_status, p_link->streaming_fg, app_multi_get_stream_only(active_idx));
                    if (p_link->call_status != BT_HFP_CALL_IDLE)
                    {
                        ret = SASS_HFP;
                    }
                    else if (p_link->streaming_fg)
                    {
                        if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING &&
                            !app_multi_get_stream_only(active_idx))
                        {
                            ret = SASS_A2DP_WITH_AVRCP;
                        }
                        else
                        {
                            ret = SASS_A2DP_ONLY;
                        }
                    }
                }
            }
#if GFPS_SASS_LEA_SUPPORT
            else if (mtc_get_btmode() == MULTI_PRO_BT_CIS)
            {
                ret = app_sass_policy_context_to_conn_state(app_lea_uca_get_active_link());
            }
            else
            {
                if (app_lea_bca_state() == LEA_BCA_STATE_STREAMING)
                {
                    ret = SASS_LEA_BIS;
                }
            }
#endif
        }
    }

    APP_PRINT_TRACE1("app_sass_policy_get_connection_state ret 0x%x", ret);
    return ret;
}

bool app_sass_policy_get_original_enable_multi_link(void)
{
    return original_enable_multi_link;
}

uint8_t app_sass_get_available_connection_num(void)
{
    uint8_t b2s_link_num = app_link_get_b2s_link_num();
#if GFPS_SASS_LEA_SUPPORT
    b2s_link_num = app_link_get_encryption_device_num();
#endif

    uint8_t max_link_num = app_cfg_const.enable_multi_link ?
                           app_cfg_const.max_legacy_multilink_devices : 1;

    uint8_t ret = (max_link_num > b2s_link_num) ? (max_link_num - b2s_link_num) : 0;

    APP_PRINT_TRACE1("app_sass_get_available_connection_num %d", ret);

    return ret;
}

bool app_sass_pause_inactive_link(void)
{
    uint8_t active_idx = app_multi_get_active_idx();
    uint8_t inactive_idx  = app_sass_find_other_idx(active_idx);
    uint8_t ret = false;

    APP_PRINT_TRACE4("app_sass_pause_inactive_link active %d,key 0x%x, inactive %d, key 0x%x",
                     active_idx, app_db.br_link[active_idx].gfps_inuse_account_key, inactive_idx,
                     app_db.br_link[inactive_idx].gfps_inuse_account_key);

    if ((app_db.br_link[inactive_idx].gfps_inuse_account_key != 0xFF) &&
        (app_db.br_link[active_idx].gfps_inuse_account_key != 0xFF))
    {
        //both sass device, do not pause directly
        ret = true;
    }
    else if ((app_db.br_link[inactive_idx].gfps_inuse_account_key == 0xFF) &&
             (app_db.br_link[active_idx].gfps_inuse_account_key == 0xFF))
    {
        //both non-sass, do not pause, follow orignial behavior
    }
    else
    {
        //non-sass and sass, pause directly
        ret = true;
        bt_avrcp_pause(app_db.br_link[inactive_idx].bd_addr);
        app_db.br_link[inactive_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
    }
    APP_PRINT_TRACE1("app_sass_pause_inactive_link ret %d", ret);

    return ret;
}


uint8_t app_multi_get_inactive_index(uint8_t new_link_idx, uint8_t link_type, uint8_t call_num,
                                     bool force, uint8_t *inactive_link_type)
{
    uint8_t inactive_index = app_sass_policy_get_disc_link(inactive_link_type);
    uint8_t active_index = 0xFF;
    T_SASS_LINK_TYPE active_link_type = SASS_TYPE_UNKNOW;

    if (*inactive_link_type != SASS_TYPE_UNKNOW)
    {
        goto end;
    }

#if GFPS_SASS_LEA_SUPPORT
    if (mtc_get_btmode())
    {
        T_APP_LE_LINK *p_active_link = app_lea_uca_get_active_link();
        if (p_active_link)
        {
            active_index = p_active_link->id;
            active_link_type = SASS_LE_LINK;
        }
    }
    else
#endif
    {
        active_index = app_multi_get_active_idx();
        active_link_type = SASS_EDR_LINK;
    }

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (((i != active_index) || (active_link_type != SASS_EDR_LINK)) &&
            ((i != new_link_idx) || (link_type != SASS_EDR_LINK)) &&
            app_db.br_link[i].connected_profile)
        {
            inactive_index = i;
            *inactive_link_type = SASS_EDR_LINK;
            goto process;
        }
    }

#if GFPS_SASS_LEA_SUPPORT
    for (uint8_t i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (((i != active_index) || (active_link_type != SASS_LE_LINK)) &&
            ((i != new_link_idx) || (link_type != SASS_LE_LINK)) &&
            app_db.le_link[i].used &&
            app_db.le_link[i].encryption_status == LE_LINK_ENCRYPTIONED)
        {
            inactive_index = i;
            *inactive_link_type = SASS_LE_LINK;
            goto process;
        }
    }
#endif

process:
    if ((inactive_index == MAX_BR_LINK_NUM) && ((app_sass_get_available_connection_num() == 0) ||
                                                force))
    {
        inactive_index = active_index;
        *inactive_link_type = active_link_type;
    }
end:
    APP_PRINT_INFO6("app_multi_get_inactive_index:id %d, type %d, %d, %d, %d, %d", inactive_index,
                    *inactive_link_type, active_index, active_link_type, new_link_idx, link_type);

    return inactive_index;
}

//modifing the return value, if 0, the link will NOT preempted by sass, if 1, the link will preempted the other edr link; if 2, the link will preempted the lea link
uint16_t app_sass_preempt_policy(uint8_t app_idx, T_SASS_LINK_TYPE link_type, uint8_t action,
                                 bool a2dp_check)
{
    uint16_t ret =
        SASS_PREEM_REJECT; //if true, the link will be preempted and will run the sass policy; otherwise, run the original policy
    T_SASS_LINK_TYPE other_link_type;
    uint8_t active_idx = 0xFF;
    T_SASS_LINK_TYPE active_link_type = SASS_TYPE_UNKNOW;
#if GFPS_SASS_LEA_SUPPORT == 0
    if (link_type != SASS_EDR_LINK)
    {
        return SASS_PREEM_NONE;
    }
#endif
    if (!mtc_get_btmode())
    {
        active_idx = app_multi_get_active_idx();
        active_link_type = SASS_EDR_LINK;
    }
    else
    {
        active_idx = (app_lea_uca_get_stream_link() == NULL) ? 0xFF : app_lea_uca_get_stream_link()->id;
        active_link_type = SASS_LE_LINK;
    }

    void *p_other_link = app_sass_find_other_link(app_idx, link_type, &other_link_type);

    if (!p_other_link || ((active_idx == app_idx) && (active_link_type == link_type)))
    {
        ret = SASS_PREEM_NONE;
        goto end;
    }
    else if (action == MULTILINK_SASS_FORCE_PREEM)
    {
        if (active_link_type == SASS_EDR_LINK)
        {
            ret = SASS_PREEM_EDR;//diff
        }
        else
        {
            ret = SASS_PREEM_LEA;//diff
        }
        goto end;
    }

    switch (action)
    {
    case MULTILINK_SASS_A2DP_PREEM:
    case MULTILINK_SASS_AVRCP_PREEM:
        {
            APP_PRINT_TRACE2("app_sass_preempt_policy: avrcp_play_status %d, stream %d",
                             app_db.br_link[app_idx].avrcp_play_status, app_db.br_link[app_idx].streaming_fg);
            if (other_link_type == SASS_EDR_LINK)
            {
                T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                if (action == MULTILINK_SASS_AVRCP_PREEM && app_db.br_link[app_idx].streaming_fg == false)
                {
                    ret = SASS_PREEM_WAITING;
                    goto end;
                }
                else if (app_multi_check_pend() &&
                         ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) ||
                          (app_multi_checking_silence_pkts()) || (a2dp_check == true) ||
                          (p_another_link->avrcp_play_status == false) ||
                          ((p_another_link->streaming_fg == false) &&
                           (p_another_link->call_status == BT_HFP_CALL_IDLE) &&
                           (app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING))))
                {
                    if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                    {
                        if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE) //hfp but has a2dp
                        {
                            if (app_hfp_get_active_idx() == app_idx)
                            {
                                ret = SASS_PREEM_NONE;
                            }
                            else
                            {
                                ret = app_sass_preempt_policy(app_idx, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM, false);
                            }
                        }
                        else if (p_another_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                        {
                            ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_VA) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_EDR)
                                  : SASS_PREEM_REJECT;
                        }
                        else
                        {
                            ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_EDR)
                                  : SASS_PREEM_REJECT;
                        }
                    }
                    else if (p_another_link->streaming_fg)
                    {
                        if ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                            (p_another_link->avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING))
                        {
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                            if ((app_db.br_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                                (p_another_link->gfps_inuse_account_key == 0xFF))
                            {
                                ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
                            }
                            else
#endif
                            {
                                ret = SASS_PREEM_EDR;
                            }
                        }
                        else if ((app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING) &&
                                 (p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
                        {
                            ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                        }
                        else
                        {
                            if ((app_db.br_link[app_idx].gfps_inuse_account_key != 0xFF) &&
                                (p_another_link->gfps_inuse_account_key != 0xFF))
                            {
                                ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_A2DP) ? SASS_PREEM_EDR :
                                      (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                            }
                            else if ((app_db.br_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                                     (p_another_link->gfps_inuse_account_key == 0xFF))
                            {
                                //default original policy
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                                ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
#else
                                ret = SASS_PREEM_EDR;
#endif
                            }
                            else
                            {
                                //for verify case
                                ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                            }
                        }

                        if (app_a2dp_get_active_idx() == app_idx
                            || app_multi_checking_silence_pkts()
#if F_APP_HARMAN_FEATURE_SUPPORT
                            || p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED
                            || p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_STOPPED
#endif
                           )
                        {
                            ret = SASS_PREEM_EDR;
                        }
                    }
                    else // other idle
                    {
#if F_APP_HARMAN_FEATURE_SUPPORT
                        if (p_another_link->id == app_multi_get_pending_id())
                        {
                            ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                        }
                        else
#endif
                        {
                            ret = SASS_PREEM_EDR;
                        }
                    }
                }
                else
                {
                    ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                }
            }
#if GFPS_SASS_LEA_SUPPORT
            else
            {
                //lea link
                T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                if (action == MULTILINK_SASS_AVRCP_PREEM && app_db.br_link[app_idx].streaming_fg == false)
                {
                    ret = SASS_PREEM_WAITING;
                    goto end;
                }
                else
                {
                    if (p_another_link->call_status != APP_CALL_IDLE)
                    {
                        if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE) //hfp but has a2dp
                        {
                            if (app_hfp_get_active_idx() == app_idx)
                            {
                                ret = SASS_PREEM_NONE;
                            }
                            else
                            {
                                ret = app_sass_preempt_policy(app_idx, SASS_EDR_LINK, MULTILINK_SASS_HFP_PREEM, false);
                            }
                        }
                        else
                        {
                            ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_LEA)
                                  : SASS_PREEM_REJECT;
                        }
                    }
                    else if (app_lea_ascs_get_enable_ase_num(p_another_link, DATA_PATH_OUTPUT_FLAG, true))
                    {
                        if ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                            (p_another_link->media_state != MCS_MEDIA_STATE_PLAYING))
                        {
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                            if ((app_db.br_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                                (p_another_link->gfps_inuse_account_key == 0xFF))
                            {
                                ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
                            }
                            else
#endif
                            {
                                ret = SASS_PREEM_LEA;
                            }
                        }
                        else if ((app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING) &&
                                 (p_another_link->media_state == MCS_MEDIA_STATE_PLAYING))
                        {
                            ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                        }
                        else
                        {
                            if ((app_db.br_link[app_idx].gfps_inuse_account_key != 0xFF) &&
                                (p_another_link->gfps_inuse_account_key != 0xFF))
                            {
                                ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_A2DP) ? SASS_PREEM_LEA :
                                      (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                            }
                            else if ((app_db.br_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                                     (p_another_link->gfps_inuse_account_key == 0xFF))
                            {
                                //default original policy
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                                ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
#else
                                ret = SASS_PREEM_LEA;
#endif
                            }
                            else
                            {
                                //for verify case
                                ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                            }
                        }
                    }
                    else
                    {
                        ret = SASS_PREEM_LEA;
                    }
                }
            }
#endif
        }
        break;

    case MULTILINK_SASS_HFP_PREEM:
        {
            APP_PRINT_TRACE3("app_multi_sass_bitmap VA: app_idx_status = %d %d, hfp_active_idx = %d",
                             app_idx, app_db.br_link[app_idx].call_status,
                             app_hfp_get_active_idx());
            if (other_link_type == SASS_EDR_LINK)
            {
                T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                if (p_another_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    if (app_hfp_get_active_idx() == app_idx)
                    {
                        ret = SASS_PREEM_NONE;
                    }
                    else if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_VA_VA) ? SASS_PREEM_EDR : SASS_PREEM_REJECT;
                    }
                    else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_VA) ? SASS_PREEM_EDR : SASS_PREEM_REJECT;
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                {
                    if (app_hfp_get_active_idx() == app_idx)
                    {
                        ret = SASS_PREEM_NONE;
                    }
                    else if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_VA_SCO) ? SASS_PREEM_EDR : SASS_PREEM_REJECT;
                    }
                    else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_SCO) ? SASS_PREEM_EDR : SASS_PREEM_REJECT;
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else if (p_another_link->streaming_fg)
                {
                    if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_VA_A2DP) ? SASS_PREEM_EDR :
                              (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                    }
                    else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_A2DP) ? SASS_PREEM_EDR :
                              (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else //other idle
                {
                    ret = SASS_PREEM_EDR;
                }
            }
#if GFPS_SASS_LEA_SUPPORT
            else
            {
                //lea link
                T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                if (p_another_link->call_status != APP_CALL_IDLE)
                {
                    if (app_hfp_get_active_idx() == app_idx)
                    {
                        ret = SASS_PREEM_NONE;
                    }
                    else if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_VA_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_LEA) :
                              SASS_PREEM_REJECT;
                    }
                    else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_LEA)
                              : SASS_PREEM_REJECT;
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else if ((p_another_link->media_state == MCS_MEDIA_STATE_PLAYING) &&
                         (app_lea_ascs_get_enable_ase_num(p_another_link, DATA_PATH_OUTPUT_FLAG, true)))
                {
                    if (app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_VA_A2DP) ? SASS_PREEM_LEA :
                              (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                    }
                    else if (app_db.br_link[app_idx].call_status != BT_HFP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_A2DP) ? SASS_PREEM_LEA :
                              (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else //other idle
                {
                    ret = SASS_PREEM_LEA;
                }
            }
#endif
        }
        break;

#if GFPS_SASS_LEA_SUPPORT
    case MULTILINK_SASS_LEA_MEDIA_PREEM:
    case MULTILINK_SASS_LEA_MCP_PREEM:
        {
            if (action == MULTILINK_SASS_LEA_MCP_PREEM &&
                app_lea_ascs_get_enable_ase_num(&app_db.le_link[app_idx], DATA_PATH_OUTPUT_FLAG, true) == 0)
            {
                ret = SASS_PREEM_WAITING;
                goto end;
            }

            if (other_link_type == SASS_EDR_LINK)
            {
                T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                {
                    if (app_db.le_link[app_idx].call_status != APP_CALL_IDLE)
                    {
                        if (!mtc_get_btmode())
                        {
                            ret = SASS_PREEM_NONE;
                        }
                        else
                        {
                            ret = app_sass_preempt_policy(app_idx, SASS_LE_LINK, MULTILINK_SASS_LEA_CALL_PREEM, false);
                        }
                    }
                    else
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_EDR)
                              : SASS_PREEM_REJECT;
                    }
                }
                else if (p_another_link->streaming_fg)
                {
                    if ((app_db.le_link[app_idx].media_state == MCS_MEDIA_STATE_PLAYING) &&
                        (p_another_link->avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING))
                    {
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                        if ((app_db.le_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                            (p_another_link->gfps_inuse_account_key == 0xFF))
                        {
                            ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
                        }
                        else
#endif
                        {
                            ret = SASS_PREEM_EDR;
                        }
                    }
                    else if ((app_db.le_link[app_idx].media_state != MCS_MEDIA_STATE_PLAYING) &&
                             (p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
                    {
                        ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
                    }
                    else
                    {
                        if ((app_db.le_link[app_idx].gfps_inuse_account_key != 0xFF) &&
                            (p_another_link->gfps_inuse_account_key != 0xFF))
                        {
                            ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_A2DP) ? SASS_PREEM_EDR : SASS_PREEM_REJECT;
                        }
                        else if ((app_db.le_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                                 (p_another_link->gfps_inuse_account_key == 0xFF))
                        {
                            //default original policy
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                            ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
#else
                            ret = SASS_PREEM_EDR;
#endif
                        }
                        else
                        {
                            //for verify case
                            ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                        }
                    }
                }
                else // other idle
                {
                    ret = SASS_PREEM_LEA;
                }
            }
            else
            {
                //lea link
                T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                if (p_another_link->call_status != APP_CALL_IDLE)
                {
                    if (app_db.le_link[app_idx].call_status != APP_CALL_IDLE)
                    {
                        if (active_idx == app_idx)//no need
                        {
                            ret = SASS_PREEM_NONE;
                        }
                        else
                        {
                            ret = app_sass_preempt_policy(app_idx, SASS_LE_LINK, MULTILINK_SASS_LEA_CALL_PREEM, false);
                        }
                    }
                    else
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_LEA)
                              : SASS_PREEM_REJECT;
                    }
                }
                else if (app_lea_ascs_get_enable_ase_num(p_another_link, DATA_PATH_OUTPUT_FLAG, true))
                {
                    if ((app_db.le_link[app_idx].media_state == MCS_MEDIA_STATE_PLAYING) &&
                        (p_another_link->media_state != MCS_MEDIA_STATE_PLAYING))
                    {
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                        if ((app_db.le_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                            (p_another_link->gfps_inuse_account_key == 0xFF))
                        {
                            ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
                        }
                        else
#endif
                        {
                            ret = SASS_PREEM_LEA;
                        }
                    }
                    else if ((app_db.le_link[app_idx].media_state != MCS_MEDIA_STATE_PLAYING) &&
                             (p_another_link->media_state == MCS_MEDIA_STATE_PLAYING))
                    {
                        ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                    }
                    else
                    {
                        if ((app_db.le_link[app_idx].gfps_inuse_account_key != 0xFF) &&
                            (p_another_link->gfps_inuse_account_key != 0xFF))
                        {
                            ret = (app_cfg_nv.sass_switching_preference & SASS_A2DP_A2DP) ? SASS_PREEM_LEA :
                                  (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                        }
                        else if ((app_db.le_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                                 (p_another_link->gfps_inuse_account_key == 0xFF))
                        {
                            //default original policy
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                            ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;
#else
                            ret = SASS_PREEM_LEA;
#endif
                        }
                        else
                        {
                            //for verify case
                            ret = (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                        }
                    }
                }
                else
                {
                    ret = SASS_PREEM_LEA;
                }
            }
        }
        break;

    case MULTILINK_SASS_LEA_CALL_PREEM:
        {
            if ((app_db.le_link[app_idx].call_status == APP_CALL_IDLE) &&
                (app_lea_ascs_get_enable_ase_num(&app_db.le_link[app_idx], DATA_PATH_INPUT_FLAG, false) == 0))
            {
                ret = app_sass_preempt_policy(app_idx, SASS_LE_LINK, MULTILINK_SASS_LEA_MEDIA_PREEM, false);
                goto end;
            }

            if (other_link_type == SASS_EDR_LINK)
            {
                T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                if (p_another_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
                    if (!mtc_get_btmode())
                    {
                        ret = SASS_PREEM_NONE;
                    }
                    else if (app_db.le_link[app_idx].call_status != APP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_VA) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_EDR) :
                              SASS_PREEM_REJECT;
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                {
                    if (!mtc_get_btmode())
                    {
                        ret = SASS_PREEM_NONE;
                    }
                    else if (app_db.le_link[app_idx].call_status != APP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_EDR)
                              : SASS_PREEM_REJECT;
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else if ((p_another_link->streaming_fg) &&
                         (p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
                {
                    if ((app_db.le_link[app_idx].call_status != APP_CALL_IDLE) ||
                        (app_sass_policy_context_to_conn_state(&app_db.le_link[app_idx]) == SASS_LEA_CALL) ||
                        (app_lea_ascs_get_enable_ase_num(&app_db.le_link[app_idx], DATA_PATH_INPUT_FLAG, false)))
                    {
                        if ((app_db.le_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                            (p_another_link->gfps_inuse_account_key == 0xFF))
                        {
                            ret = SASS_PREEM_EDR;
                        }
                        else
                        {
                            if ((app_sass_policy_context_to_conn_state(&app_db.le_link[app_idx]) != SASS_LEA_CALL))
                            {
                                ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;//gaming
                            }
                            else
                            {
                                ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_A2DP) ? SASS_PREEM_EDR :
                                      (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                            }
                        }
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else //other idle
                {
                    ret = SASS_PREEM_EDR;
                }
            }
            else
            {
                //lea link
                T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                if (p_another_link->call_status != APP_CALL_IDLE)
                {
                    if (active_idx == app_idx)
                    {
                        ret = SASS_PREEM_NONE;
                    }
                    else if (app_db.le_link[app_idx].call_status != APP_CALL_IDLE)
                    {
                        ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_SCO) ? (SASS_ORIGINAL_DIFF | SASS_PREEM_LEA)
                              : SASS_PREEM_REJECT;
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else if ((p_another_link->media_state == MCS_MEDIA_STATE_PLAYING) &&
                         (app_lea_ascs_get_enable_ase_num(p_another_link, DATA_PATH_OUTPUT_FLAG, true)))
                {
                    if ((app_db.le_link[app_idx].call_status != APP_CALL_IDLE) ||
                        (app_sass_policy_context_to_conn_state(&app_db.le_link[app_idx]) == SASS_LEA_CALL) ||
                        (app_lea_ascs_get_enable_ase_num(&app_db.le_link[app_idx], DATA_PATH_INPUT_FLAG, false)))
                    {
                        if ((app_db.le_link[app_idx].gfps_inuse_account_key == 0xFF) &&
                            (p_another_link->gfps_inuse_account_key == 0xFF))
                        {
                            ret = SASS_PREEM_LEA;
                        }
                        else
                        {
                            if ((app_sass_policy_context_to_conn_state(&app_db.le_link[app_idx]) != SASS_LEA_CALL))
                            {
                                ret = SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT;//gaming
                            }
                            else
                            {
                                ret = (app_cfg_nv.sass_switching_preference & SASS_SCO_A2DP) ? SASS_PREEM_LEA :
                                      (SASS_ORIGINAL_DIFF | SASS_PREEM_REJECT);
                            }
                        }
                    }
                    else // wait hfp
                    {
                        ret = SASS_PREEM_REJECT;
                    }
                }
                else //other idle
                {
                    ret = SASS_PREEM_LEA;
                }
            }
        }
        break;
#endif

    default:
        break;
    }

end:
    APP_PRINT_TRACE5("app_sass_preempt_policy: action 0x%x, idx %d, active_idx %d, active_link_type %d, ret 0x%x",
                     action,
                     app_idx, active_idx, active_link_type, ret);
    return ret;
}


uint8_t app_multi_preemptive_judge(uint8_t app_idx, uint8_t link_type, uint8_t type)
{
    uint8_t org_type = type;
    uint8_t reason = 0;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();
    T_SASS_LINK_TYPE other_link_type = SASS_TYPE_UNKNOW;
    void *p_other_link = app_sass_find_other_link(app_idx, (T_SASS_LINK_TYPE)link_type,
                                                  &other_link_type);

    //If the original multi run the following logic, need consider the sass disabled condition
    T_SASS_PREEM sass_preem = (T_SASS_PREEM)app_sass_preempt_policy(app_idx,
                                                                    (T_SASS_LINK_TYPE)link_type, type, false);

    if (type == MULTILINK_SASS_FORCE_PREEM)
    {
#if GFPS_SASS_LEA_SUPPORT
        if (mtc_get_btmode()) //TODO: check BIS policy
        {
            T_APP_LE_LINK *p_le_link = app_link_find_le_link_by_conn_id(app_idx);
            if (p_le_link)
            {
                if (p_le_link->call_status != APP_CALL_IDLE)
                {
                    type = MULTILINK_SASS_LEA_CALL_PREEM;
                }
                else if (app_lea_uca_get_stream_link() && app_lea_uca_get_stream_link()->id == app_idx)
                {
                    type = MULTILINK_SASS_LEA_MEDIA_PREEM;
                }
                else if (p_le_link->media_state == MCS_MEDIA_STATE_PLAYING)
                {
                    type = MULTILINK_SASS_LEA_MCP_PREEM;
                }
            }
        }
        else
#endif
        {
            if (app_db.br_link[app_idx].streaming_fg)
            {
                type = MULTILINK_SASS_A2DP_PREEM;
            }
            else if (app_db.br_link[app_idx].sco_handle)
            {
                type = MULTILINK_SASS_HFP_PREEM;
            }
        }
    }

    APP_PRINT_TRACE3("app_multi_preemptive_judge: sass_preem = 0x%x, link_type 0x%x, type = 0x%x",
                     sass_preem,
                     link_type, type);

    switch (type)
    {
    case MULTILINK_SASS_A2DP_PREEM:
        {
            switch (sass_preem)
            {
            case SASS_PREEM_EDR:
                {
                    app_multi_stop_silence_check_timer();
                    app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                    app_multi_stream_avrcp_set(app_idx);
                }
                break;

            case SASS_PREEM_NONE:
                {
                    if (app_db.br_link[app_idx].avrcp_play_status != BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PLAYING;
#if F_APP_MUTILINK_VA_PREEMPTIVE
                        app_multi_set_stream_only(app_idx, true);
#endif
                    }

                    if ((app_cfg_const.enable_multi_link) &&
                        (app_link_get_b2s_link_num() == MULTILINK_SRC_CONNECTED))
                    {
                        app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                    }
                }
                break;

            case SASS_PREEM_LEA:
                {
                    //run here only policy diff with original
                    T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;
                    if (p_another_link)
                    {
                        if (p_another_link->call_status != APP_CALL_IDLE)
                        {
                            //reject sco
                            app_lea_uca_link_sm(p_another_link->conn_handle, LEA_PAUSE_LOCAL, NULL);
                        }
                    }
                }
                break;

            case SASS_PREEM_REJECT:
                {
                    //if the policy same with default. the logic need move to multilink
                    if (other_link_type == SASS_EDR_LINK)
                    {
                        T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                        if (!p_another_link)
                        {
                            reason = 1;
                            goto end;
                        }

                        if (p_another_link->streaming_fg == true)
                        {
                            if ((app_db.br_link[active_a2dp_idx].streaming_fg == false) && app_multi_check_pend())
                            {
                                active_a2dp_idx = app_idx;

                                app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                                app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                            }
                            else
                            {
                                if (app_db.active_media_paused)
                                {
                                    // A2DP Paused
                                    app_db.a2dp_wait_a2dp_sniffing_stop = true;
                                    app_db.pending_a2dp_idx = app_idx;
                                }
                                else
                                {
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                                    if ((app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                                        (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING))
                                    {
                                        bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);
                                        audio_track_stop(app_db.br_link[app_idx].a2dp_track_handle);
                                        app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                                    }
#endif
                                }
                            }
                        }
                        else if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                        {
                            if (app_cfg_const.enable_multi_sco_disc_resume)
                            {
                                app_multi_pause_inactive_a2dp_link_stream(p_another_link->id, true);
                            }
                            else
                            {
                                app_multi_pause_inactive_a2dp_link_stream(p_another_link->id, false);
                            }
                        }

                    }
                    else
                    {
                        //run here only policy diff with original
                    }

                    if (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                    {
                        bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case MULTILINK_SASS_AVRCP_PREEM:
        {
            switch (sass_preem)
            {
            case SASS_PREEM_EDR:
            case SASS_PREEM_NONE:
                {
                    APP_PRINT_TRACE3("JUDGE_EVENT_MEDIAPLAYER_PLAYING: preemptive active_a2dp_idx %d, app_idx %d, streaming_fg %d",
                                     active_a2dp_idx, app_idx, app_db.br_link[app_idx].streaming_fg);
                    if (active_a2dp_idx == app_idx)
                    {
                        app_db.active_media_paused = false;
                    }

                    app_multi_a2dp_active_link_set(app_db.br_link[app_idx].bd_addr);
                    app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                    app_bond_set_priority(app_db.br_link[app_idx].bd_addr);
                }
                break;

            case SASS_PREEM_LEA:
                {
                    //run here only policy diff with original
                    T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;
                    if (p_another_link)
                    {
                        if (p_another_link->call_status != APP_CALL_IDLE)
                        {
                            //reject sco
                            app_lea_uca_link_sm(p_another_link->conn_handle, LEA_PAUSE_LOCAL, NULL);
                        }
                    }
                }
                break;

            case SASS_PREEM_REJECT:
                {
                    if (other_link_type == SASS_EDR_LINK)
                    {
                        T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                        if (!p_another_link)
                        {
                            reason = 2;
                            goto end;
                        }

                        if (p_another_link->streaming_fg == true)
                        {
                            if (extend_app_cfg_const.gfps_sass_support)
                            {
                                if (app_sass_pause_inactive_link() == false)
                                {
#if F_APP_ENABLE_PAUSE_SECOND_LINK
                                    if (app_db.br_link[active_a2dp_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                                    {
                                        bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);
                                        audio_track_stop(app_db.br_link[app_idx].a2dp_track_handle);
                                        app_db.br_link[app_idx].avrcp_play_status = BT_AVRCP_PLAY_STATUS_PAUSED;
                                    }
#endif
                                }
                            }
                        }
                        else if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                        {
                            if (app_cfg_const.enable_multi_sco_disc_resume)
                            {
                                app_multi_pause_inactive_a2dp_link_stream(p_another_link->id, true);
                            }
                            else
                            {
                                app_multi_pause_inactive_a2dp_link_stream(p_another_link->id, false);
                            }
                        }
                    }
                    else
                    {
                        //run here only policy diff with original
                        if (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                        {
                            bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case MULTILINK_SASS_HFP_PREEM:
        {
            switch (sass_preem)
            {
            case SASS_PREEM_EDR:
            case SASS_PREEM_NONE:
                {
                    if (app_cfg_const.enable_multi_sco_disc_resume)
                    {
                        app_multi_pause_inactive_a2dp_link_stream(app_idx, true);
                    }
                    else
                    {
                        app_multi_pause_inactive_a2dp_link_stream(app_idx, false);
                    }

                    if (app_db.br_link[app_idx].sco_track_handle)
                    {
                        audio_track_start(app_db.br_link[app_idx].sco_track_handle);
                    }
                }
                break;

            case SASS_PREEM_LEA:
                {
                    //run here only policy diff with original
                    T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;
                    if (p_another_link)
                    {
                        if (p_another_link->call_status != APP_CALL_IDLE)
                        {
                            //reject sco
                            app_lea_uca_link_sm(p_another_link->conn_handle, LEA_PAUSE_LOCAL, NULL);
                        }
                    }
                }
                break;

            case SASS_PREEM_REJECT:
                {
                    if (other_link_type == SASS_EDR_LINK)
                    {
                        T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                        if (!p_another_link)
                        {
                            reason = 3;
                            goto end;
                        }

                        if (p_another_link->streaming_fg)
                        {
                            if (app_db.br_link[app_idx].sco_handle)// && app_db.br_link[app_idx].call_status == APP_VOICE_ACTIVATION_ONGOING)
                            {
                                gap_br_vendor_set_active_sco(app_db.br_link[app_idx].sco_handle, 0, 2);
                            }

                            app_db.a2dp_preemptive_flag = true;
                            app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_MULTILINK_CHANGE);
                        }
                    }
                    else
                    {
                        //run here only policy diff with original
                        bt_hfp_audio_disconnect_req(app_db.br_link[app_idx].bd_addr);
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case MULTILINK_SASS_LEA_MEDIA_PREEM:
    case MULTILINK_SASS_LEA_MCP_PREEM:
        {
            switch (sass_preem)
            {
            case SASS_PREEM_EDR:
                {
                    //run here only policy diff with original
                    T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                    if (p_another_link)
                    {
                        if ((p_another_link->call_status != BT_HFP_CALL_IDLE) && (p_another_link->sco_handle))
                        {
                            bt_hfp_audio_disconnect_req(p_another_link->bd_addr);
                        }
                        else if (p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                        {
                            app_lea_mcp_play_pause(app_db.le_link[app_idx].conn_handle, false);
                        }
                    }
                }
                break;

            case SASS_PREEM_LEA:
                {
                    //run here only policy diff with original
                    T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                    if (p_another_link)
                    {
                        if (p_another_link->call_status != APP_CALL_IDLE)
                        {
                            app_lea_uca_link_sm(p_another_link->conn_handle, LEA_PAUSE_LOCAL, NULL);
                        }
                        else if (p_another_link->media_state == MCS_MEDIA_STATE_PLAYING)
                        {
                            app_lea_mcp_play_pause(app_db.le_link[app_idx].conn_handle, false);
                        }
                    }
                }
                break;

            case SASS_PREEM_REJECT:
                {
                    //run here only policy diff with original

                    if (link_type == SASS_LE_LINK)
                    {
                        if (app_db.le_link[app_idx].media_state == MCS_MEDIA_STATE_PLAYING)
                        {
                            app_lea_mcp_play_pause(app_db.le_link[app_idx].conn_handle, false);
                        }
                    }
                    else
                    {
                        if (app_db.br_link[app_idx].avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
                        {
                            bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    case MULTILINK_SASS_LEA_CALL_PREEM:
        {
            //run here only policy diff with original
            switch (sass_preem)
            {
            case SASS_PREEM_EDR:
                {
                    T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;
                    if (p_another_link)
                    {
                        if (p_another_link->call_status != BT_HFP_CALL_IDLE)
                        {
                            bt_hfp_audio_disconnect_req(p_another_link->bd_addr);
                        }
                    }
                }
                break;
            case SASS_PREEM_LEA:
                {
                    T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                    if (p_another_link)
                    {
                        if (p_another_link->call_status != APP_CALL_IDLE)
                        {
                            app_lea_uca_link_sm(p_another_link->conn_handle, LEA_PAUSE_LOCAL, NULL);
                        }
                    }
                }
                break;

            case SASS_PREEM_REJECT:
                {
                    if (other_link_type == SASS_EDR_LINK)
                    {
                        T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;

                        if (p_another_link)
                        {
                            if ((p_another_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING) &&
                                (p_another_link->streaming_fg))
                            {
                                bt_hfp_audio_disconnect_req(app_db.br_link[app_idx].bd_addr);
                            }
                        }
                    }
                    else
                    {
                        T_APP_LE_LINK *p_another_link = (T_APP_LE_LINK *)p_other_link;

                        if ((p_another_link->media_state == MCS_MEDIA_STATE_PLAYING) && (app_lea_uca_get_stream_link() &&
                                                                                         (app_lea_uca_get_stream_link()->id == app_idx)))
                        {
                            app_lea_uca_link_sm(app_db.le_link[app_idx].conn_handle, LEA_PAUSE_LOCAL, NULL);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
        break;

    default:
        break;
    }


    if (sass_preem == SASS_PREEM_EDR)
    {
        if (link_type == SASS_EDR_LINK)
        {
            T_APP_BR_LINK *p_another_link = (T_APP_BR_LINK *)p_other_link;
            app_multi_set_active_idx(app_idx);

            if (p_another_link->call_status != BT_HFP_CALL_IDLE ||
                p_another_link->sco_handle)
            {
                if (p_another_link->sco_handle &&
                    app_db.br_link[app_idx].streaming_fg)
                {
                    gap_br_vendor_set_active_sco(p_another_link->sco_handle, 0, 2);
                }

                if (p_another_link->call_status == APP_VOICE_ACTIVATION_ONGOING)
                {
#if F_APP_MUTILINK_VA_PREEMPTIVE
                    app_multi_voice_ongoing_preemptive(p_another_link->id, app_idx);
#endif
                }
                else
                {
                    app_multi_active_hfp_transfer(app_idx, true, org_type == MULTILINK_SASS_FORCE_PREEM);
                }
            }
        }
    }

end:
    APP_PRINT_INFO2("app_multi_preemptive_judge reason %d, sas_preem %d", reason, sass_preem);

    if (extend_app_cfg_const.gfps_sass_support)
    {
        app_gfps_notify_conn_status();
    }

    return sass_preem;
}

#if F_APP_ERWS_SUPPORT
static uint16_t app_sass_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SASS_PREEM_BIT_SYNC:
        {
            skip = false;
            payload_len = 2;
            msg_ptr = (uint8_t *)&app_cfg_nv.sass_switching_preference;
        }
        break;

    case APP_REMOTE_MSG_SASS_SWITCH_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *)&app_cfg_nv.sass_advanced_switching_setting;
        }
        break;

    case APP_REMOTE_MSG_SASS_MULTILINK_STATE_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *) &app_cfg_nv.sass_multipoint_state;
        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_SUPPORT_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *) &app_cfg_nv.sass_bitmap;
        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_BITMAP_SYNC:
        {
            skip = false;
            payload_len = 1;
            msg_ptr = (uint8_t *) &app_db.conn_bitmap;
        }
        break;


    default:
        break;
    }

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_GFPS_SASS, 0, NULL, skip, total);
}

static void app_sass_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                 T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_sass_parse_cback: msg_type 0x%02X, status 0x%02X", msg_type, status);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SASS_PREEM_BIT_SYNC:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                memcpy(&app_cfg_nv.sass_switching_preference, &p_info[0], 2);
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_SWITCH_SYNC:
        {
            if ((status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT) ||
                (status == REMOTE_RELAY_STATUS_ASYNC_RCVD))
            {
                uint8_t *p_info = (uint8_t *)buf;
                memcpy(&app_cfg_nv.sass_advanced_switching_setting, &p_info[0], 1);
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_MULTILINK_STATE_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_cfg_nv.sass_multipoint_state = *p_info;
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_BITMAP_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_sass_policy_sync_conn_bit_map(*p_info);
            }

        }
        break;

    case APP_REMOTE_MSG_SASS_DEVICE_SUPPORT_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_cfg_nv.sass_bitmap = *p_info;
            }
        }
        break;

    case APP_REMOTE_MSG_SASS_DISC_LE_LINK_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                uint8_t *p_info = (uint8_t *)buf;
                app_ble_gap_disconnect(p_info, LE_LOCAL_DISC_CAUSE_GFPS_SASS_DISC);
            }
        }
        break;

    default:
        break;
    }
}
#endif

static void app_sass_policy_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_sass_policy_timeout_cb: timer_evt %d, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_SASS_TIMER_SWITCH_BACK:
        {
            app_stop_timer(&timer_idx_switch_back);
            app_sass_policy_link_back_end();
        }
        break;

    default:
        break;
    }
}

void app_sass_policy_reset(void)
{
    app_cfg_nv.sass_switching_preference = 0;

    // if (!app_cfg_const.disable_multilink_preemptive)
    {
        app_cfg_nv.sass_switching_preference |= SASS_A2DP_A2DP;
    }
    // app_cfg_const.disable_multilink_preemptive = false;

    app_cfg_nv.sass_switching_preference |= SASS_SCO_A2DP;
    app_cfg_nv.sass_switching_preference |= SASS_VA_A2DP;

    app_cfg_nv.sass_multipoint_state = 0xFF;
    app_cfg_store(&app_cfg_nv.sass_multipoint_state, 1);

    APP_PRINT_TRACE3("app_sass_policy_reset: sass_multipoint_state %x enable_multi_link %x, sass_switching_preference 0x%x",
                     app_cfg_nv.sass_multipoint_state, app_cfg_const.enable_multi_link,
                     app_cfg_nv.sass_switching_preference);

    app_cfg_nv.sass_bitmap = 0;
}

void app_sass_policy_init(void)
{
    original_enable_multi_link = app_cfg_const.enable_multi_link;
    if (app_cfg_nv.sass_multipoint_state == 0xFF)
    {
        app_cfg_nv.sass_multipoint_state = app_cfg_const.enable_multi_link;
    }
    else
    {
        app_cfg_const.enable_multi_link = app_cfg_nv.sass_multipoint_state;
    }

    bt_mgr_cback_register(app_sass_policy_bt_cback);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_sass_relay_cback, app_sass_parse_cback,
                             APP_MODULE_TYPE_GFPS_SASS, APP_REMOTE_MSG_SASS_TOTAL);
#endif
    app_timer_reg_cb(app_sass_policy_timeout_cb, &app_sass_timer_id);
}

#endif
