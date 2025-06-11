#include <stdlib.h>
#include <string.h>
#include "os_mem.h"
#include "os_timer.h"
#include "os_sync.h"
#include "console.h"
#include "app_timer.h"
#include "remote.h"
#include "audio_passthrough.h"
#include "bt_bond.h"
#include "eq_utils.h"
#include "app_hfp.h"
#include "app_cfg.h"
#include "app_audio_passthrough.h"
#include "app_relay.h"
#include "app_eq.h"
#include "app_main.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#include "trace.h"
#include "audio.h"
#include "anc_tuning.h"
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#include "app_cmd.h"
#include "app_bt_policy_api.h"
#include "audio_type.h"
#include "app_audio_passthrough_brightness.h"

#if F_APP_BRIGHTNESS_SUPPORT

#define APT_BRIGHTNESS_CMD_STATUS_SUCCESS               0x00
#define APT_BRIGHTNESS_CMD_STATUS_FAILED                0x01
#define APT_BRIGHTNESS_CMD_STATUS_SPK2_NOT_EXIST        0x02
#define APT_BRIGHTNESS_CMD_STATUS_SPK2_TIMEOUT          0x03
#define APT_BRIGHTNESS_CMD_STATUS_UNKNOW_CMD            0x04
#define APT_BRIGHTNESS_CMD_STATUS_CHANNEL_NOT_MATCH     0x05
#define APT_BRIGHTNESS_CMD_STATUS_WRONG                 0x06
#define APT_BRIGHTNESS_CMD_STATUS_CRC_FAIL              0x07
#define APT_BRIGHTNESS_CMD_STATUS_LENGTH_FAIL           0x08
#define APT_BRIGHTNESS_CMD_STATUS_SEGMENT_FAIL          0x09
#define APT_BRIGHTNESS_CMD_STATUS_MEMORY_LACK           0x0A
#define APT_BRIGHTNESS_CMD_STATUS_FLASH_BURN_FAILED     0x0B
#define APT_BRIGHTNESS_CMD_STATUS_FLASH_COMFIRM_FAILED  0x0C
#define APT_BRIGHTNESS_CMD_STATUS_CMD_LENGTH_INVALID    0x0D

bool    dsp_config_support_brightness = false;
uint8_t brightness_level_max;
uint8_t brightness_level_min;
uint8_t brightness_level_default;
uint16_t brightness_gain_table[16];

static uint16_t apt_brightness_report_event;
static uint8_t apt_brightness_timer_id = 0;
static uint32_t llapt_brightness_support_bitmap = 0;
static uint8_t timer_idx_apt_brightness_cmd_wait_sec_respond = 0;

typedef enum
{
    APP_TIMER_APT_BRIGHTNESS_CMD_WAIT_SEC_RESPOND = 0x00,
} T_APP_APT_BRIGHTNESS_TIMER;

bool app_apt_brightness_relay(bool first_time_sync, bool primary_report_phone)
{
    if (app_db.remote_session_state != REMOTE_SESSION_STATE_CONNECTED)
    {
        return false;
    }

    T_APT_BRIGHTNESS_RELAY_MSG send_msg;

    send_msg.report_phone = primary_report_phone;
    send_msg.first_sync = first_time_sync;
    send_msg.brightness_type = app_db.brightness_type;
    send_msg.main_brightness_level = app_cfg_nv.main_brightness_level;
    send_msg.sub_brightness_level = app_cfg_nv.sub_brightness_level;
    send_msg.rws_disallow_sync_llapt_brightness = app_cfg_nv.rws_disallow_sync_llapt_brightness;

    return app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_BRIGHTNESS_MODE,
                                               APP_REMOTE_MSG_APT_BRIGHTNESS_SYNC,
                                               (uint8_t *)&send_msg, sizeof(T_APT_BRIGHTNESS_RELAY_MSG));
}

void app_apt_brightness_relay_report_handle(void)
{
    if (!app_cfg_nv.rws_disallow_sync_llapt_brightness &&
        app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        //this mechanism is going to avoid report phone twice

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
        {
            //secondary do nothing, waiting for primary sync data
        }
        else
        {
            app_apt_brightness_relay(true, false);
            app_apt_brightness_set_remote_level(app_cfg_nv.main_brightness_level,
                                                app_cfg_nv.sub_brightness_level);
            app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_STATUS, NULL, 0);
        }
    }
    else
    {
        if (app_apt_brightness_relay(false, true) == false)
        {
            app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_STATUS, NULL, 0);
        }
    }
}

void app_apt_brightness_sync_handle(T_APT_BRIGHTNESS_RELAY_MSG *rev_msg)
{
    APP_PRINT_TRACE4("app_apt_brightness_sync_handle first_sync = %x, remote_main = %x, remote_sub = %x, disallow_sync = %x",
                     rev_msg->first_sync, rev_msg->main_brightness_level, rev_msg->sub_brightness_level,
                     rev_msg->rws_disallow_sync_llapt_brightness);

    //first time sync, brightness of primary is applied to secondary
    if (rev_msg->first_sync)
    {
        app_cfg_nv.rws_disallow_sync_llapt_brightness = rev_msg->rws_disallow_sync_llapt_brightness;

        if (!app_cfg_nv.rws_disallow_sync_llapt_brightness)
        {
            if (rev_msg->brightness_type == MAIN_TYPE_LEVEL)
            {
                app_apt_brightness_main_level_set(rev_msg->main_brightness_level);
                app_cfg_nv.sub_brightness_level = rev_msg->sub_brightness_level;
            }
            else // SUB_TYPE_LEVEL
            {
                app_apt_brightness_sub_level_set(rev_msg->sub_brightness_level);
                app_cfg_nv.main_brightness_level = rev_msg->main_brightness_level;
            }
        }
    }

    app_apt_brightness_set_remote_level(rev_msg->main_brightness_level, rev_msg->sub_brightness_level);
}

void app_apt_brightness_set_remote_level(uint8_t main_level, uint16_t sub_level)
{
    app_db.remote_main_brightness_level = main_level;
    app_db.remote_sub_brightness_level = sub_level;
}

void app_apt_brightness_update_main_level(void)
{
    uint8_t mapping_main_he_level;
    float percentage;

    percentage = (float)app_cfg_nv.sub_brightness_level / (float)(LLAPT_SUB_BRIGHTNESS_LEVEL_MAX -
                                                                  LLAPT_SUB_BRIGHTNESS_LEVEL_MIN);

    mapping_main_he_level = brightness_level_min + (uint8_t)((brightness_level_max -
                                                              brightness_level_min) * percentage);

    app_cfg_nv.main_brightness_level = mapping_main_he_level;
}

void app_apt_brightness_update_sub_level(void)
{
    uint16_t mapping_sub_he_level;
    float percentage;

    percentage = (float)app_cfg_nv.main_brightness_level / (float)(brightness_level_max -
                                                                   brightness_level_min);

    mapping_sub_he_level = LLAPT_SUB_BRIGHTNESS_LEVEL_MIN + (uint16_t)((LLAPT_SUB_BRIGHTNESS_LEVEL_MAX -
                                                                        LLAPT_SUB_BRIGHTNESS_LEVEL_MIN) * percentage);

    app_cfg_nv.sub_brightness_level = mapping_sub_he_level;
}

void app_apt_brightness_main_level_set(uint8_t level)
{
    float alpha;

    alpha = (float)brightness_gain_table[level] * 0.0025f;

    audio_passthrough_brightness_set(alpha);

    app_cfg_nv.main_brightness_level = level;

    app_apt_brightness_update_sub_level();

    app_db.brightness_type = MAIN_TYPE_LEVEL;

    APP_PRINT_TRACE2("app_apt_brightness_main_level_set main_level = 0x%x, sub_level = 0x%x", level,
                     app_cfg_nv.sub_brightness_level);
}

void app_apt_brightness_sub_level_set(uint16_t level)
{
    float alpha;

    alpha = level / (float)(LLAPT_SUB_BRIGHTNESS_LEVEL_MAX - LLAPT_SUB_BRIGHTNESS_LEVEL_MIN);

    audio_passthrough_brightness_set(alpha);

    app_cfg_nv.sub_brightness_level = level;

    app_apt_brightness_update_main_level();

    app_db.brightness_type = SUB_TYPE_LEVEL;

    APP_PRINT_TRACE2("app_apt_brightness_sub_level_set sub_level = 0x%x, main_level = 0x%x", level,
                     app_cfg_nv.main_brightness_level);
}

uint32_t app_apt_brightness_get_support_bitmap(void)
{
    return llapt_brightness_support_bitmap;
}

bool app_apt_brightness_llapt_scenario_support(T_ANC_APT_STATE state)
{
    bool ret = false;

    if (app_apt_is_apt_on_state(state))
    {
        if (app_cfg_const.llapt_support)
        {
            uint32_t scenario_index;

            scenario_index = (uint32_t)(state - ANC_OFF_LLAPT_ON_SCENARIO_1);

            if (llapt_brightness_support_bitmap & BIT(app_apt_llapt_get_coeff_idx(scenario_index)))
            {
                ret = true;
            }
        }
    }

    return ret;
}

static void app_apt_brightness_timeout_cb(uint8_t timer_id, uint16_t timer_chann)
{
    switch (timer_id)
    {
    case APP_TIMER_APT_BRIGHTNESS_CMD_WAIT_SEC_RESPOND:
        {
            app_stop_timer(&timer_idx_apt_brightness_cmd_wait_sec_respond);

            uint8_t report_status;

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                report_status = APT_BRIGHTNESS_CMD_STATUS_SPK2_TIMEOUT;
            }
            else
            {
                report_status = APT_BRIGHTNESS_CMD_STATUS_SPK2_NOT_EXIST;
            }

            app_apt_brightness_report(apt_brightness_report_event, &report_status, 1);
        }
        break;

    default:
        break;
    }
}

static void app_apt_brightness_parameter_init(void)
{
    if (dsp_config_support_brightness)
    {
#if F_APP_ANC_SUPPORT
        if (app_cfg_const.llapt_support)
        {
            uint8_t scenario_mode[12];
            uint8_t scenario_apt_effect[12] = {0};

            uint8_t scenario_cnt = anc_tool_get_llapt_scenario_info(scenario_mode, scenario_apt_effect);

            //establish he support table
            for (uint32_t i = 0; i < scenario_cnt; i++)
            {
                if (scenario_apt_effect[i] != 0)
                {
                    llapt_brightness_support_bitmap |= BIT(i);
                }
            }
        }
#endif
    }

    app_db.brightness_type = MAIN_TYPE_LEVEL;

    if (app_cfg_nv.sub_brightness_level == INVALID_LEVEL_VALUE)
    {
        app_apt_brightness_update_sub_level();
    }

    app_db.remote_main_brightness_level = (uint8_t)INVALID_LEVEL_VALUE;
    app_db.remote_sub_brightness_level = INVALID_LEVEL_VALUE;

    APP_PRINT_TRACE5("app_apt_brightness_init dsp_config_support = %x, llapt = %x, bitmap = 0x%x, local_main_level = 0x%x, local_sub_level = 0x%x",
                     dsp_config_support_brightness, app_cfg_const.llapt_support, llapt_brightness_support_bitmap,
                     app_cfg_nv.main_brightness_level, app_cfg_nv.sub_brightness_level);
}

void app_apt_brightness_report(uint16_t brightness_report_event, uint8_t *event_data,
                               uint16_t event_len)
{
    bool handle = true;

    switch (brightness_report_event)
    {
    case EVENT_LLAPT_BRIGHTNESS_INFO:
        {
            uint8_t report_data[10];

            LE_UINT8_TO_ARRAY(&report_data[0],   LLAPT_MAIN_BRIGHTNESS_LEVEL_MAX);
            LE_UINT16_TO_ARRAY(&report_data[1],  LLAPT_SUB_BRIGHTNESS_LEVEL_MAX);

            LE_UINT8_TO_ARRAY(&report_data[3],   L_CH_BRIGHTNESS_MAIN_LEVEL);
            LE_UINT16_TO_ARRAY(&report_data[4],  L_CH_BRIGHTNESS_SUB_LEVEL);
            LE_UINT8_TO_ARRAY(&report_data[6],   R_CH_BRIGHTNESS_MAIN_LEVEL);
            LE_UINT16_TO_ARRAY(&report_data[7],  R_CH_BRIGHTNESS_SUB_LEVEL);

            LE_UINT8_TO_ARRAY(&report_data[9],   RWS_SYNC_LLAPT_BRIGHTESS);

            app_report_event_broadcast(EVENT_LLAPT_BRIGHTNESS_INFO, &report_data[0], sizeof(report_data));
        }
        break;

    case EVENT_LLAPT_BRIGHTNESS_SET:
        {
            uint8_t report_data[8];
            uint8_t llapt_status = event_data[0];

            LE_UINT8_TO_ARRAY(&report_data[0], llapt_status);
            LE_UINT8_TO_ARRAY(&report_data[1], L_CH_BRIGHTNESS_MAIN_LEVEL);
            LE_UINT16_TO_ARRAY(&report_data[2], L_CH_BRIGHTNESS_SUB_LEVEL);
            LE_UINT8_TO_ARRAY(&report_data[4], R_CH_BRIGHTNESS_MAIN_LEVEL);
            LE_UINT16_TO_ARRAY(&report_data[5], R_CH_BRIGHTNESS_SUB_LEVEL);

            LE_UINT8_TO_ARRAY(&report_data[7],   RWS_SYNC_LLAPT_BRIGHTESS);

            app_report_event_broadcast(EVENT_LLAPT_BRIGHTNESS_SET, &report_data[0], sizeof(report_data));
        }
        break;

    case EVENT_LLAPT_BRIGHTNESS_STATUS:
        {
            uint8_t report_data[7];

            LE_UINT8_TO_ARRAY(&report_data[0], L_CH_BRIGHTNESS_MAIN_LEVEL);
            LE_UINT16_TO_ARRAY(&report_data[1], L_CH_BRIGHTNESS_SUB_LEVEL);
            LE_UINT8_TO_ARRAY(&report_data[3], R_CH_BRIGHTNESS_MAIN_LEVEL);
            LE_UINT16_TO_ARRAY(&report_data[4], R_CH_BRIGHTNESS_SUB_LEVEL);

            LE_UINT8_TO_ARRAY(&report_data[6], RWS_SYNC_LLAPT_BRIGHTESS);

            app_report_event_broadcast(EVENT_LLAPT_BRIGHTNESS_STATUS, &report_data[0], sizeof(report_data));
        }
        break;

    default:
        {
            handle = false;
        }
        break;
    }

    if (handle)
    {
        APP_PRINT_TRACE1("app_apt_brightness_report = %x", brightness_report_event);
    }
}

void app_apt_brightness_cmd_pre_handle(uint16_t brightness_cmd, uint8_t *param_ptr,
                                       uint16_t param_len, uint8_t path, uint8_t app_idx, uint8_t *ack_pkt)
{
    /*
        switch (brightness_cmd)
        {
        // reserved for some SKIP_OPERATE condition check
        default:
            break;
        }
    */
    app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
    app_apt_brightness_cmd_handle(brightness_cmd, param_ptr, param_len, path, app_idx);

    /*
    SKIP_OPERATE:
        app_report_event(path, EVENT_ACK, app_idx, ack_pkt, 3);
    */
}

void app_apt_brightness_cmd_handle(uint16_t brightness_cmd, uint8_t *param_ptr, uint16_t param_len,
                                   uint8_t path, uint8_t app_idx)
{
    bool only_report_status = false;
    uint8_t cmd_status = APT_BRIGHTNESS_CMD_STATUS_FAILED;

    apt_brightness_report_event = brightness_cmd;

    APP_PRINT_TRACE5("app_apt_brightness_cmd_handle: anc_cmd 0x%04X, param_ptr 0x%02X 0x%02X 0x%02X 0x%02X",
                     brightness_cmd, param_ptr[0], param_ptr[1], param_ptr[2], param_ptr[3]);

    switch (brightness_cmd)
    {
    case CMD_LLAPT_BRIGHTNESS_INFO:
        {
            app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_INFO, NULL, 0);
        }
        break;

    case CMD_LLAPT_BRIGHTNESS_SET:
        {
            cmd_status = APT_BRIGHTNESS_CMD_STATUS_SUCCESS;

            if (app_apt_brightness_llapt_scenario_support(app_db.current_listening_state))
            {
                /* brightness set command              *
                 * byte [0]    : brightness type  (M)  *
                 * byte [1,2]  : L_channel_volume (M)  *
                 * byte [3,4]  : R_channel_volume (M)  *
                 * byte [5]    : sync             (O)  */

                uint8_t  volume_type;
                uint16_t l_brightness_volume;
                uint16_t r_brightness_volume;
                uint16_t used_volume;
                uint8_t brightness_sync;

                volume_type = param_ptr[0];

                LE_ARRAY_TO_UINT16(l_brightness_volume, &param_ptr[1]);
                LE_ARRAY_TO_UINT16(r_brightness_volume, &param_ptr[3]);

                used_volume = (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? l_brightness_volume :
                              r_brightness_volume;

                brightness_sync = (param_len > 5) ? param_ptr[5] : (uint8_t)INVALID_LEVEL_VALUE;

                if (brightness_sync != (uint8_t)INVALID_LEVEL_VALUE)
                {
                    if (brightness_sync != RWS_SYNC_LLAPT_BRIGHTESS)
                    {
                        app_cfg_nv.rws_disallow_sync_llapt_brightness ^= 1;

                        if (!app_cfg_nv.rws_disallow_sync_llapt_brightness)
                        {
                            // set apt volume as medium value
                            volume_type = SUB_TYPE_LEVEL;
                            used_volume = LLAPT_SUB_BRIGHTNESS_LEVEL_MAX / 2;
                        }
                    }
                }

                APP_PRINT_TRACE5("app_apt_brightness_cmd_handle CMD_LLAPT_BRIGHTNESS_SET side = %d, type = %d, l_brightness = %d, r_brightness = %d, sync = 0x%x",
                                 app_cfg_const.bud_side, volume_type, l_brightness_volume, r_brightness_volume, brightness_sync);

                if (volume_type == MAIN_TYPE_LEVEL && used_volume <= LLAPT_MAIN_BRIGHTNESS_LEVEL_MAX)
                {
                    if (app_db.brightness_type == SUB_TYPE_LEVEL ||
                        (app_db.brightness_type == MAIN_TYPE_LEVEL && used_volume != app_cfg_nv.main_brightness_level))
                    {
                        app_apt_brightness_main_level_set(used_volume);
                        app_apt_brightness_relay(false, false);
                    }
                }
                else if (volume_type == SUB_TYPE_LEVEL && (used_volume <= LLAPT_SUB_BRIGHTNESS_LEVEL_MAX))
                {
                    if (app_db.brightness_type == MAIN_TYPE_LEVEL ||
                        (app_db.brightness_type == SUB_TYPE_LEVEL && used_volume != app_cfg_nv.sub_brightness_level))
                    {
                        app_apt_brightness_sub_level_set(used_volume);
                        app_apt_brightness_relay(false, false);
                    }
                }
            }

            if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (path != CMD_PATH_RWS_ASYNC))
            {
                //rws primary
                if (app_cmd_relay_command_set(brightness_cmd, &param_ptr[0], param_len,
                                              APP_MODULE_TYPE_BRIGHTNESS_MODE,
                                              APP_REMOTE_MSG_RELAY_APT_BRIGHTNESS_CMD, false))
                {
                    app_start_timer(&timer_idx_apt_brightness_cmd_wait_sec_respond,
                                    "apt_brightness_cmd_wait_sec_respond",
                                    apt_brightness_timer_id, APP_TIMER_APT_BRIGHTNESS_CMD_WAIT_SEC_RESPOND, 0, false,
                                    3000);
                }
                else
                {
                    cmd_status = APT_BRIGHTNESS_CMD_STATUS_MEMORY_LACK;
                    only_report_status = true;
                }
            }
            else if ((app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) &&
                     (path == CMD_PATH_RWS_ASYNC))
            {
                //rws secondary
                app_cmd_relay_event(brightness_cmd, &cmd_status, sizeof(cmd_status),
                                    APP_MODULE_TYPE_BRIGHTNESS_MODE,
                                    APP_REMOTE_MSG_RELAY_APT_BRIGHTNESS_EVENT);

            }
            else
            {
                //single primary
                app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_SET, &cmd_status, 1);
            }
        }
        break;

    case CMD_LLAPT_BRIGHTNESS_STATUS:
        {
            app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_STATUS, NULL, 0);
        }
        break;

    default:
        {
            only_report_status = true;
            cmd_status = APT_BRIGHTNESS_CMD_STATUS_UNKNOW_CMD;
        }
        break;
    }

    if (only_report_status == true)
    {
        app_report_event(path, brightness_cmd, app_idx, &cmd_status, 1);
    }
}

#if F_APP_ERWS_SUPPORT
static uint16_t app_apt_brightness_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    // uint16_t payload_len = 0;
    // uint8_t *msg_ptr = NULL;
    // bool skip = true;

    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_BRIGHTNESS_MODE, 0, NULL,
                              true, total);
}


static void app_apt_brightness_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                           T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_apt_brightness_parse_cback: msg = 0x%x, status = %x", msg_type, status);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_APT_BRIGHTNESS_SYNC:
        {
            T_APT_BRIGHTNESS_RELAY_MSG *relay_msg = (T_APT_BRIGHTNESS_RELAY_MSG *)buf;

            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_apt_brightness_sync_handle(relay_msg);

                if (relay_msg->first_sync)
                {
                    //secondary relay first sync result
                    app_apt_brightness_relay(false, true);
                }
            }

            if (status == REMOTE_RELAY_STATUS_ASYNC_SENT_OUT ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    if (relay_msg->report_phone)
                    {
                        app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_STATUS, NULL, 0);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_RELAY_APT_BRIGHTNESS_CMD:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                /* bypass_cmd
                 * byte [0,1]  : cmd_id   *
                 * byte [2,3]  : cmd_len  *
                 * byte [4]    : cmd_path *
                 * byte [5-N]  : cmd      */

                uint16_t cmd_id;
                uint16_t anc_cmd_len;

                cmd_id = (buf[0] | buf[1] << 8);
                anc_cmd_len = (buf[2] | buf[3] << 8);

                app_apt_brightness_cmd_handle(cmd_id, &buf[5], anc_cmd_len, buf[4], 0);
            }
        }
        break;

    case APP_REMOTE_MSG_RELAY_APT_BRIGHTNESS_EVENT:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                /* report
                 * byte [0,1] : event_id    *
                 * byte [2,3] : report_len  *
                 * byte [4-N] : report      */

                uint16_t event_id;
                uint16_t event_len;

                event_id = (buf[0] | buf[1] << 8);
                event_len = (buf[2] | buf[3] << 8);

                app_stop_timer(&timer_idx_apt_brightness_cmd_wait_sec_respond);

                if (event_id == CMD_LLAPT_BRIGHTNESS_SET)
                {
                    app_apt_brightness_report(EVENT_LLAPT_BRIGHTNESS_SET, &buf[4], event_len);
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

static void app_apt_brightness_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->remote_conn_cmpl.bd_addr);

            if (p_link != NULL)
            {
                if (app_db.device_state != APP_DEVICE_STATE_OFF)
                {
                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                    {
                        app_apt_brightness_relay(true, false);
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
        APP_PRINT_INFO1("app_apt_brightness_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_apt_brightness_init(void)
{
    app_apt_brightness_parameter_init();
    app_timer_reg_cb(app_apt_brightness_timeout_cb, &apt_brightness_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_apt_brightness_relay_cback, app_apt_brightness_parse_cback,
                             APP_MODULE_TYPE_BRIGHTNESS_MODE, APP_REMOTE_MSG_APT_BRIGHTNESS_TOTAL);
#endif
    bt_mgr_cback_register(app_apt_brightness_bt_cback);
}

#endif
