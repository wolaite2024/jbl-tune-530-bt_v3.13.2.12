/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_DURIAN_SUPPORT
#include <string.h>
#include "trace.h"
#include "app_durian.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_cmd.h"

static const T_BT_AVP_BUD_LOCATION  loc_map[4] = {BT_AVP_BUD_UNKNOWN, BT_AVP_BUD_IN_CASE, BT_AVP_BUD_OUT_OF_CASE, BT_AVP_BUD_IN_EAR};/*refer to T_BUD_LOCATION*/

void app_durian_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                           uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    app_durian_sync_fast_pair();

    switch (cmd_id)
    {
    case CMD_SET_CFG:
        {
            if (cmd_ptr[2] == CFG_SET_SERIAL_ID)
            {
#if DURIAN_AIRMAX
                app_durian_cfg_set_avp_id(&cmd_ptr[3]);
#else
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_durian_cfg_set_avp_id(&cmd_ptr[3]);
                }
#endif
            }
#if F_APP_ERWS_SUPPORT
            else if (cmd_ptr[2] == CFG_SET_SERIAL_SINGLE_ID)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_durian_cfg_set_single_id(&cmd_ptr[3]);
                }
            }
#endif
            else if (cmd_ptr[2] == CFG_SET_DISABLE_REPORT_AVP_ID)
            {
                uint8_t disable_report_avp_id = cmd_ptr[3] ? 0 : 1;
                app_cfg_nv.disable_report_avp_id = disable_report_avp_id;
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AVP,
                                                    SYNC_EVENT_DURIAN_DISABLE_DISPLAY_AVP_ID, &disable_report_avp_id, 1);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_GET_CFG_SETTING:
        {
            uint8_t p_name[12 + 2] = {0};
            uint8_t *p_buf = NULL;
            uint8_t  id[13] = {0};
            if (cmd_ptr[2] == CFG_GET_AVP_ID)
            {
                memcpy(id, durian_cfg.avp_id.id, sizeof(durian_cfg.avp_id.id));
                p_buf = id;
            }
            else if (cmd_ptr[2] == CFG_GET_AVP_LEFT_SINGLE_ID)
            {
                if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
                {
                    memcpy(id, durian_cfg.avp_id.single_id, sizeof(durian_cfg.avp_id.single_id));
                }
                else
                {
                    memcpy(id, durian_cfg.avp_id.remote_single_id,
                           sizeof(durian_cfg.avp_id.remote_single_id));
                }
                p_buf = id;
            }
            else if (cmd_ptr[2] == CFG_GET_AVP_RIGHT_SINGLE_ID)
            {
                if (app_cfg_const.bud_side == DEVICE_ROLE_RIGHT)
                {
                    memcpy(id, durian_cfg.avp_id.single_id, sizeof(durian_cfg.avp_id.single_id));
                }
                else
                {
                    memcpy(id, durian_cfg.avp_id.remote_single_id,
                           sizeof(durian_cfg.avp_id.remote_single_id));
                }
                p_buf = id;
            }

            if (p_buf != NULL)
            {
                p_name[0] = cmd_ptr[2];
                memcpy(&p_name[1], p_buf, 12);
                app_report_event(cmd_path, EVENT_REPORT_CFG_TYPE, app_idx, &p_name[0], 13);
            }
        }
        break;

    case CMD_GET_STATUS:
        {
            uint8_t buf[3];
            uint8_t report_len = 2;

            buf[0] = cmd_ptr[2]; //status_index

            if (cmd_ptr[2] == GET_STATUS_AVP_RWS_VER)
            {
                buf[1] = durian.gen;
                buf[2] = 0x02;  //Compatible with multilink sync for ios+android
                report_len = 3;
            }

            else if (cmd_ptr[2] == GET_STATUS_AVP_MULTILINK_ON_OFF)
            {
                if ((app_cfg_nv.enable_multi_link == 1) || (app_cfg_const.enable_multi_link == 1))
                {
                    buf[1] = AVP_MULTILINK_STATUS_ON;
                }
                else
                {
                    buf[1] = AVP_MULTILINK_STATUS_OFF;
                }
            }
            else if (cmd_ptr[2] == GET_STATUS_AVP_PROFILE_CONNECT)
            {
                buf[1] = (app_link_get_misc_num(APP_LINK_AVP) > 0) ? 1 : 0;
            }
#if DURIAN_TWO
            else if (cmd_ptr[2] == GET_STATUS_AVP_CONTROL_SET)
            {
                if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
                {
                    buf[1] = durian_db.local_double_click_action;
                    buf[2] = durian_db.remote_double_click_action;
                }
                else
                {
                    buf[1] = durian_db.remote_double_click_action;
                    buf[2] = durian_db.local_double_click_action;
                }
                report_len = 3;
            }
#endif
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
            else if (cmd_ptr[2] == GET_STATUS_AVP_CLICK_SET)
            {
                uint8_t left_right_action = 0;

                if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
                {
                    durian_db.local_long_action = (durian_db.local_long_action << 4) | durian_db.remote_long_action;
                }
                else
                {
                    durian_db.local_long_action = (durian_db.remote_long_action << 4) | durian_db.local_long_action;

                }
                buf[1] = left_right_action;
            }
#endif
#if DURIAN_PRO|DURIAN_AIRMAX|DURIAN_BEATS_FIT|DURIAN_PRO2
            else if (cmd_ptr[2] == GET_STATUS_AVP_ANC_APT_CYCLE)
            {
                buf[1] = durian_db.anc_cycle_setting;
            }
            else if (cmd_ptr[2] == GET_STATUS_AVP_ANC_SETTINGS)
            {
                buf[1] = durian_db.anc_cur_setting;
            }
#endif
            else if (cmd_ptr[2] == GET_STATUS_AVP_BUD_LOCAL)
            {
                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    buf[1] = loc_map[durian_db.local_loc];
                    buf[2] = loc_map[durian_db.remote_loc];
                }
                else
                {
                    buf[1] = loc_map[durian_db.remote_loc];
                    buf[2] = loc_map[durian_db.local_loc];
                }
                report_len = 3;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                break;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_REPORT_STATUS, app_idx, buf, report_len);
        }
        break;

    case CMD_GET_EAR_DETECTION_STATUS:
        {
            uint8_t status = durian_db.ear_detect_en;

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_EAR_DETECTION_STATUS, app_idx, &status, sizeof(status));
        }
        break;

    case CMD_AVP_LD_EN:
        {
            app_durian_loc_in_ear_detect(cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_EAR_DETECTION_STATUS, app_idx, &durian_db.ear_detect_en,
                             sizeof(durian_db.ear_detect_en));
        }
        break;

#if (F_APP_LISTENING_MODE_SUPPORT == 1)
    case CMD_AVP_ANC_SETTINGS:
        {
            app_durian_anc_set(cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_AVP_ANC_CYCLE_SET:
        {
            app_durian_anc_cylce_set(cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif
#if DURIAN_PRO|DURIAN_BEATS_FIT|DURIAN_PRO2
    case CMD_AVP_CLICK_SET:
        {
            app_durian_key_click_set(cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif
#if DURIAN_TWO
    case CMD_AVP_CONTROL_SET:
        {
            app_durian_key_control_set(cmd_ptr[2], cmd_ptr[3]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
#endif

    default:
        {
            ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    }
}
#endif
