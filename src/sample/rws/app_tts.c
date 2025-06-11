/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_TTS_SUPPORT
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "trace.h"
#include "app_cfg.h"
#include "app_ble_gap.h"
#include "app_tts.h"
#include "app_ble_common_adv.h"
#include "ble_conn.h"
#include "app_audio_policy.h"
#include "app_cmd.h"
#include "app_link_util.h"
#include "app_hfp.h"
#include "app_main.h"
#include "stdlib.h"
//for CMD_TTS
#define TTS_TYPE_START                  0x00
#define TTS_TYPE_SEND_DATA              0x01

#define TTS_DATA_SINGLE                 0x00
#define TTS_DATA_START                  0x01
#define TTS_DATA_CONTINUE               0x02
#define TTS_DATA_END                    0x03

#define TTS_HEADER_LEN                  0x000A
#define TTS_SYNC_BYTE                   0xB8
#define TTS_INIT_SEQ                    0x00
#define TTS_FRAME_LEN                   0x02

void app_tts_handle_power_off(void)
{
    if (app_ble_common_adv_get_state() != BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        app_ble_common_adv_stop(APP_STOP_ADV_CAUSE_POWER_OFF);
    }
}

void app_tts_ble_disconnect_cb(uint8_t conn_id, uint8_t local_disc_cause, uint16_t disc_cause)
{
    if (disc_cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)
        || disc_cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)
        || disc_cause == (HCI_ERR | HCI_ERR_REMOTE_POWER_OFF))
    {
        //Do nothing
    }
    else
    {
        if (app_cfg_const.tts_support)
        {
            if (((SPP_PROFILE_MASK & app_link_conn_profiles()) == 0)
                && ((~RDTP_PROFILE_MASK & app_link_conn_profiles()) != 0))
            {
                APP_PRINT_TRACE0("app_tts_ble_disconnect_cb: start common adv");
                app_ble_common_adv_start(app_cfg_const.timer_tts_adv_timeout * 100);
            }
        }
    }
}

static bool app_tts_start_ble_adv(uint8_t *bd_addr)
{
    bool ret = false;

    if ((app_db.device_state != APP_DEVICE_STATE_OFF) &&
        (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY))
    {
        T_APP_LE_LINK *p_le_link;
        p_le_link = app_link_find_le_link_by_addr(bd_addr);

        if ((p_le_link == NULL) ||
            ((p_le_link != NULL) && (p_le_link->state == LE_LINK_STATE_DISCONNECTED)))
        {
            if (app_ble_common_adv_get_state() == BLE_EXT_ADV_MGR_ADV_DISABLED)
            {
                APP_PRINT_TRACE0("app_tts_start_ble_adv: start common adv");
                ret = app_ble_common_adv_start(app_cfg_const.timer_tts_adv_timeout * 100);
            }
        }
    }
    return ret;
}

static bool app_tts_stop_ble_link_and_adv(uint8_t *bd_addr)
{
    bool ret = false;

    //todo: multilink adv control
    if (app_ble_common_adv_get_state() != BLE_EXT_ADV_MGR_ADV_DISABLED)
    {
        if (app_cfg_const.rtk_app_adv_support && (app_cfg_const.timer_ota_adv_timeout != 0)
            && !app_cfg_const.enable_power_on_adv_with_timeout)
        {
            APP_PRINT_TRACE0("app_tts_stop_ble_link_and_adv: stop common adv");
            ret = app_ble_common_adv_stop(APP_STOP_ADV_CAUSE_COMMON_FOR_DEV_DISC);
        }
    }

    T_APP_LE_LINK *p_le_link;
    p_le_link = app_link_find_le_link_by_addr(bd_addr);

    if (p_le_link)
    {
        if (p_le_link->state == LE_LINK_STATE_CONNECTING || p_le_link->state == LE_LINK_STATE_CONNECTED)
        {
            APP_PRINT_TRACE0("app_tts_stop_ble_link_and_adv: stop le link");
            ret = app_ble_gap_disconnect(p_le_link, LE_LOCAL_DISC_CAUSE_TTS_DISC);
        }
    }

    return ret;
}

static void app_tts_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_HFP_CONN_CMPL:
        if (app_cfg_const.tts_support)
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->hfp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if ((SPP_PROFILE_MASK & p_link->connected_profile) == 0)
                {
                    app_tts_start_ble_adv(p_link->bd_addr);
                }
            }
        }
        break;

    case BT_EVENT_A2DP_CONN_CMPL:
        if (app_cfg_const.tts_support)
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if ((SPP_PROFILE_MASK & p_link->connected_profile) == 0)
                {
                    app_tts_start_ble_adv(p_link->bd_addr);
                }
            }
        }
        break;

    case BT_EVENT_SPP_CONN_CMPL:
        if (app_cfg_const.tts_support)
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->spp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (app_db.device_state != APP_DEVICE_STATE_OFF &&
                    app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    if (app_cfg_const.rtk_app_adv_support && (app_cfg_const.timer_ota_adv_timeout != 0)
                        && !app_cfg_const.enable_power_on_adv_with_timeout)
                    {
                        if (app_ble_common_adv_get_state() != BLE_EXT_ADV_MGR_ADV_DISABLED)
                        {
                            APP_PRINT_TRACE0("app_tts_bt_cback: spp connect, stop common adv");
                            app_ble_common_adv_stop(APP_STOP_ADV_CAUSE_COMMON_FOR_SPP_CONN);
                        }
                    }
                }
            }
        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        if (app_cfg_const.tts_support)
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->hfp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t remain_profile = (app_db.br_link[p_link->id].connected_profile &
                                              (~(HFP_PROFILE_MASK | HSP_PROFILE_MASK)));
                    if (!(remain_profile & A2DP_PROFILE_MASK))
                    {
                        app_tts_stop_ble_link_and_adv(p_link->bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        if (app_cfg_const.tts_support)
        {
            T_APP_BR_LINK *p_link;
            p_link = app_link_find_br_link(param->a2dp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t remain_profile = (app_db.br_link[p_link->id].connected_profile & (~A2DP_PROFILE_MASK));
                    if (!(remain_profile & (HFP_PROFILE_MASK | HSP_PROFILE_MASK)))
                    {
                        app_tts_stop_ble_link_and_adv(p_link->bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        if (app_cfg_const.tts_support)
        {
            T_APP_LE_LINK *p_le_link;
            p_le_link = app_link_find_le_link_by_addr(param->hfp_call_status.bd_addr);
            if ((p_le_link != NULL) && (p_le_link->state == LE_LINK_STATE_CONNECTED))
            {
                if (param->hfp_call_status.curr_status == BT_HFP_CALL_INCOMING)
                {
                    //TTS start, use small interval for TTS data
                    ble_set_prefer_conn_param(p_le_link->conn_id, 0x0C, 0x18, 0, 500); //30ms
                }
                else if (param->hfp_call_status.curr_status == BT_HFP_CALL_ACTIVE)
                {
                    //TTS end, use large interval to lower the chance overlap with eSCO
                    ble_set_prefer_conn_param(p_le_link->conn_id, 0x330, 0x348, 0, 500); //1050ms
                }
                else if (param->hfp_call_status.curr_status == BT_HFP_CALL_IDLE)
                {
                    //Call end, use medium interval in case next incoming call right after previous call
                    ble_set_prefer_conn_param(p_le_link->conn_id, 0x54, 0x60, 0, 500); //120ms
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
        APP_PRINT_INFO1("app_tts_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_tts_cmd_send_start_frame(T_TTS_INFO *tts_info, uint16_t frame_len, uint16_t len,
                                         uint8_t *cmd_ptr, uint8_t *ack_pkt)
{
    if (tts_info->tts_handle != NULL)
    {
        tts_info->tts_data_offset = 0;
        tts_info->tts_state = TTS_SESSION_SEND_START_FRAME;
        tts_info->tts_seq = TTS_INIT_SEQ;
        tts_info->tts_frame_len = frame_len + TTS_FRAME_LEN;

        if (tts_info->tts_frame_buf != NULL)
        {
            free(tts_info->tts_frame_buf);
        }

        tts_info->tts_frame_buf = malloc(frame_len + TTS_FRAME_LEN);
        if (tts_info->tts_frame_buf != NULL)
        {
            memcpy_s(tts_info->tts_frame_buf, tts_info->tts_frame_len,  &cmd_ptr[6], len);
            tts_info->tts_data_offset += len;
        }
        else
        {
            ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
        }
    }
    else
    {
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
}

static void app_tts_cmd_send_continue_frame(T_TTS_INFO *tts_info, uint8_t seq, uint16_t len,
                                            uint8_t *cmd_ptr, uint8_t *ack_pkt)
{
    if (tts_info->tts_handle != NULL)
    {
        if (seq == tts_info->tts_seq + 1)
        {
            tts_info->tts_seq = seq;
            tts_info->tts_state = TTS_SESSION_SEND_CONTINUE_FRAME;
            memcpy_s(tts_info->tts_frame_buf + tts_info->tts_data_offset,
                     (tts_info->tts_frame_len - tts_info->tts_data_offset), &cmd_ptr[6], len);
            tts_info->tts_data_offset += len;
        }
        else
        {
            ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        }
    }
    else
    {
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
}

static void app_tts_cmd_send_end_frame(T_TTS_INFO *tts_info, uint8_t seq, uint16_t len,
                                       uint8_t *cmd_ptr, uint8_t *ack_pkt)
{
    if (tts_info->tts_handle != NULL)
    {
        if (seq == tts_info->tts_seq + 1)
        {
            tts_info->tts_seq = seq;
            tts_info->tts_state = TTS_SESSION_SEND_END_FRAME;
            memcpy_s(tts_info->tts_frame_buf + tts_info->tts_data_offset,
                     (tts_info->tts_frame_len - tts_info->tts_data_offset), &cmd_ptr[6], len);
            tts_info->tts_data_offset = 0;
            tts_play(tts_info->tts_handle, tts_info->tts_frame_buf, tts_info->tts_frame_len);
            free(tts_info->tts_frame_buf);
            tts_info->tts_frame_buf = NULL;
        }
        else
        {
            ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
        }
    }
    else
    {
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
}
static void app_tts_cmd_session_open(T_TTS_INFO *tts_info, void *tts_handle, uint16_t sess_len,
                                     uint16_t frame_num, uint32_t tts_cfg, uint8_t *ack_pkt)
{
    tts_handle = tts_create(sess_len - TTS_HEADER_LEN, frame_num, tts_cfg);

    if (tts_handle != NULL)
    {
        tts_info->tts_handle = tts_handle;
        tts_info->tts_state  = TTS_SESSION_OPEN;
        tts_start(tts_info->tts_handle, false);
    }
    else
    {
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
}

static void app_tts_cmd_send_single_frame_or_session_close(uint8_t type, T_TTS_INFO *tts_info,
                                                           uint16_t len, uint8_t *cmd_ptr, uint8_t *ack_pkt)
{
    if (tts_info->tts_handle != NULL)
    {
        tts_info->tts_state = type;
        tts_play(tts_info->tts_handle, &cmd_ptr[6], len);
    }
    else
    {
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
}

static void app_tts_cmd_session_abort(void *tts_handle, uint8_t *tts_state, uint8_t *ack_pkt)
{
    if (tts_handle != NULL)
    {
        *tts_state = TTS_SESSION_ABORT;
        tts_stop(tts_handle);
    }
    else
    {
        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
    }
}

void app_tts_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                        uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_TTS:
        {
            uint8_t    type;
            uint8_t    seq;
            uint16_t   len;
            uint16_t   frame_len;
            bool       need_to_ack = false;
            T_TTS_INFO *tts_info = NULL;

            type = cmd_ptr[2];
            seq  = cmd_ptr[3];
            len  = (uint16_t)(cmd_ptr[4] | (cmd_ptr[5] << 8)); /* little endian */

            if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
            {
                tts_info = &app_db.br_link[app_idx].tts_info;
            }
            else if (cmd_path == CMD_PATH_LE)
            {
                tts_info = &app_db.le_link[app_idx].tts_info;
            }

            if (type == TTS_SESSION_OPEN)
            {
                uint8_t   sync_byte;
                uint32_t  sess_len;
                uint16_t  frame_num;
                uint32_t  tts_cfg;
                void     *tts_handle = NULL;

                sync_byte = cmd_ptr[6]; /* big endian */
                sess_len = (uint32_t)((cmd_ptr[7] << 16) | (cmd_ptr[8] << 8) | cmd_ptr[9]); /* big endian */
                frame_num = (uint16_t)((cmd_ptr[10] << 8) | cmd_ptr[11]); /* big endian */
                tts_cfg = (uint32_t)((cmd_ptr[12] << 24) | (cmd_ptr[13] << 16) |
                                     (cmd_ptr[14] << 8) | cmd_ptr[15]); /* big endian */

                if ((sync_byte != TTS_SYNC_BYTE) || (seq != TTS_INIT_SEQ) || (len != TTS_HEADER_LEN))
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }

                app_audio_set_tts_path(cmd_path);

                if (tts_info != NULL)
                {
                    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();

                    if ((tts_info->tts_handle == NULL) && (call_status == APP_CALL_INCOMING))
                    {
                        app_tts_cmd_session_open(tts_info, tts_handle, sess_len, frame_num, tts_cfg, ack_pkt);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    }

                    need_to_ack = true;
                }
            }
            else if ((type == TTS_SESSION_SEND_SINGLE_FRAME) || (type == TTS_SESSION_CLOSE))
            {
                uint16_t frame_len_add = 0;

                if (type == TTS_SESSION_SEND_SINGLE_FRAME)
                {
                    frame_len = (uint16_t)(cmd_ptr[6] << 8 | cmd_ptr[7]); /* big endian */
                    frame_len_add = frame_len;
                }

                if ((seq != TTS_INIT_SEQ) ||
                    (len != frame_len_add + TTS_FRAME_LEN))
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }

                if (tts_info != NULL)
                {
                    app_tts_cmd_send_single_frame_or_session_close(type, tts_info, len, cmd_ptr, ack_pkt);
                    need_to_ack = true;
                }
            }
            else if (type == TTS_SESSION_SEND_START_FRAME)
            {
                frame_len = (uint16_t)(cmd_ptr[6] << 8 | cmd_ptr[7]); /* big endian */

                if (seq != TTS_INIT_SEQ)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }

                if (tts_info != NULL)
                {
                    app_tts_cmd_send_start_frame(tts_info, frame_len, len, cmd_ptr, ack_pkt);
                    need_to_ack = true;
                }
            }
            else if (type == TTS_SESSION_SEND_CONTINUE_FRAME)
            {
                if (tts_info != NULL)
                {
                    app_tts_cmd_send_continue_frame(tts_info, seq, len, cmd_ptr, ack_pkt);
                    need_to_ack = true;
                }
            }
            else if (type == TTS_SESSION_SEND_END_FRAME)
            {
                if (tts_info != NULL)
                {
                    app_tts_cmd_send_end_frame(tts_info, seq, len, cmd_ptr, ack_pkt);
                    need_to_ack = true;
                }
            }
            else if (type == TTS_SESSION_ABORT)
            {
                if (seq != TTS_INIT_SEQ)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    break;
                }

                if (tts_info != NULL)
                {
                    app_tts_cmd_session_abort(&tts_info->tts_handle, &tts_info->tts_state, ack_pkt);
                    need_to_ack = true;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                need_to_ack = true;
            }

            if (need_to_ack)
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
        }
        break;

    default:
        break;
    }
}


static void app_tts_common_adv_cb(uint8_t cb_type, T_BLE_EXT_ADV_CB_DATA cb_data)
{
    APP_PRINT_TRACE0("app_tts_common_adv_cb");

    if (cb_type == BLE_EXT_ADV_SET_CONN_INFO)
    {
        app_link_reg_le_link_disc_cb(cb_data.p_ble_conn_info->conn_id, app_tts_ble_disconnect_cb);
    }
}


void app_tts_init(void)
{
    app_ble_common_adv_cb_reg(app_tts_common_adv_cb);
    bt_mgr_cback_register(app_tts_bt_cback);
}
#endif

