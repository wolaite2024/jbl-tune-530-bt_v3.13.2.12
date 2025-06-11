/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "trace.h"
#include "btm.h"
#include "bt_pbap.h"
#include "gap.h"
#include "gap_br.h"
#include "audio.h"
#include "audio_volume.h"
#include "audio_probe.h"
#include "remote.h"
#include "app_link_util.h"
#include "app_report.h"
#include "app_main.h"
#include "app_test.h"
#include "app_cfg.h"
#include "app_dsp_cfg.h"
#include "app_cmd.h"
#include "app_hfp.h"
#include "app_pbap.h"
#include "app_timer.h"

typedef enum
{
    IAP_STATUS_AUTHEN_SUCCESS        = 0x00,
    IAP_STATUS_DATA_SESSION_OPEN     = 0x01,
    IAP_STATUS_DATA_SESSION_CLOSE    = 0x02,
} T_EVENT_IAP_STATUS;

/**  @brief acl link state */
typedef enum
{
    ACL_LINK_STATE_STANDBY           = 0x00,
    ACL_LINK_STATE_CONNECTED         = 0x01,
    ACL_LINK_STATE_LOST              = 0x02,
    ACL_LINK_STATE_CONN_FAIL         = 0x03,
    ACL_LINK_STATE_KEY_MISSING       = 0x04,
} T_ACL_LINK_STATE;

#if F_APP_HFP_CMD_SUPPORT
typedef enum
{
    SCO_STATE_DISCONNECTED           = 0x00,
    SCO_STATE_CONNECTED              = 0x01,
} T_EVENT_SCO_STATE;

T_HFP_CURRENT_CALL_LIST current_call_list;
#endif

#if F_APP_TEST_SUPPORT
#if F_APP_DEVICE_CMD_SUPPORT
typedef enum
{
    SERVICES_SEARCH_STATE_SUCCESS    = 0x00,
    SERVICES_SEARCH_STATE_FAIL       = 0x01,
    SERVICES_SEARCH_STATE_STOP       = 0x02,
} T_EVENT_SERVICES_SEARCH_STATE;

typedef enum
{
    INQUIRY_STATE_CANCEL             = 0x00,
    INQUIRY_STATE_VALID              = 0x01,
    INQUIRY_STATE_ERROR              = 0x02,
} T_EVENT_INQUIRY_STATE;

static uint8_t acl_conn_ind_bd_addr[6]      = {0};
static uint8_t user_confirmation_bd_addr[6] = {0};

uint8_t *app_test_get_acl_conn_ind_bd_addr(void)
{
    return acl_conn_ind_bd_addr;
}

uint8_t *app_test_get_user_confirmation_bd_addr(void)
{
    return user_confirmation_bd_addr;
}
#endif

#if F_APP_AVRCP_CMD_SUPPORT
static uint8_t attr_list[7] = {1, 2, 3, 4, 5, 6, 7};
#endif

typedef enum
{
    APP_TIMER_REPORT_CURRENT_CALLS,
} T_APP_TSET_TIMER;

#define CYCLE_REPORT_CURRENT_CALLS_TIMER 2000

static uint8_t app_test_timer_id = 0;
#if F_APP_HFP_CMD_SUPPORT
static uint8_t timer_idx_report_current_calls = 0;
#endif

static void app_test_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_REPORT_CURRENT_CALLS:
        {
#if F_APP_HFP_CMD_SUPPORT
            app_report_event(CMD_PATH_UART, EVENT_CURRENT_CALLS, 0, &(current_call_list.call_idx),
                             current_call_list.num_len + 8);
#endif
        }
        break;

    default:
        break;
    }
}

void app_test_report_event(uint8_t *bd_addr, uint16_t event_id, uint8_t *data, uint16_t len)
{
    T_APP_BR_LINK *br_link;
    T_APP_LE_LINK *le_link;

    br_link = app_link_find_br_link(bd_addr);
    le_link = app_link_find_le_link_by_addr(bd_addr);

    if (br_link != NULL)
    {
        if (br_link->connected_profile & SPP_PROFILE_MASK)
        {
            app_report_event(CMD_PATH_SPP, event_id, br_link->id, data, len);
        }
        else if (br_link->connected_profile & IAP_PROFILE_MASK)
        {
            app_report_event(CMD_PATH_IAP, event_id, br_link->id, data, len);
        }
        else if (le_link != NULL)
        {
            app_report_event(CMD_PATH_LE, event_id, le_link->id, data, len);
        }
        else
        {
            app_report_event(CMD_PATH_UART, event_id, br_link->id, data, len);
        }
    }
}

static void app_test_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_AUDIO_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case AUDIO_EVENT_TRACK_VOLUME_OUT_CHANGED:
        {
            T_VOL_CHANGE vol_change = VOL_CHANGE_NONE;
            T_AUDIO_STREAM_TYPE stream_type;

            if (audio_track_stream_type_get(param->track_volume_out_changed.handle, &stream_type) == false)
            {
                break;
            }

            if (stream_type == AUDIO_STREAM_TYPE_PLAYBACK)
            {
                if (param->track_volume_out_changed.curr_volume == app_dsp_cfg_vol.playback_volume_max)
                {
                    vol_change = VOL_CHANGE_MAX;
                }
                else if (param->track_volume_out_changed.curr_volume == app_dsp_cfg_vol.playback_volume_min)
                {
                    vol_change = VOL_CHANGE_MIN;
                }
                else
                {
                    if (param->track_volume_out_changed.curr_volume > param->track_volume_out_changed.prev_volume)
                    {
                        vol_change = VOL_CHANGE_UP;
                    }
                    else if (param->track_volume_out_changed.curr_volume < param->track_volume_out_changed.prev_volume)
                    {
                        vol_change = VOL_CHANGE_DOWN;
                    }
                }
            }
            else if (stream_type == AUDIO_STREAM_TYPE_VOICE)
            {
                if (param->track_volume_out_changed.curr_volume == app_dsp_cfg_vol.voice_out_volume_max)
                {
                    vol_change = VOL_CHANGE_MAX;
                }
                else if (param->track_volume_out_changed.curr_volume == app_dsp_cfg_vol.voice_out_volume_min)
                {
                    vol_change = VOL_CHANGE_MIN;
                }
                else
                {
                    if (param->track_volume_out_changed.curr_volume > param->track_volume_out_changed.prev_volume)
                    {
                        vol_change = VOL_CHANGE_UP;
                    }
                    else if (param->track_volume_out_changed.curr_volume < param->track_volume_out_changed.prev_volume)
                    {
                        vol_change = VOL_CHANGE_DOWN;
                    }
                }
            }
            app_report_event(CMD_PATH_UART, EVENT_AUDIO_VOL_CHANGE, 0, (uint8_t *)&vol_change,
                             sizeof(vol_change));
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_test_audio_cback: event_type 0x%04x", event_type);
    }
}

static void app_test_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_A2DP_CONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->a2dp_conn_cmpl) + 1];

            temp_buff[0] = A2DP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->a2dp_conn_cmpl, sizeof(param->a2dp_conn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_A2DP_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->a2dp_disconn_cmpl) + 1];

            temp_buff[0] = A2DP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->a2dp_disconn_cmpl, sizeof(param->a2dp_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_A2DP_CONN_FAIL:
        {
            uint8_t temp_buff[sizeof(param->a2dp_conn_fail) + 1];

            temp_buff[0] = A2DP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->a2dp_conn_fail, sizeof(param->a2dp_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_TRACK_CHANGED:
        {
            uint8_t temp_buff[14];

            memcpy(&temp_buff[0], param->avrcp_track_changed.bd_addr, 6);;
            memcpy(&temp_buff[6], &param->avrcp_track_changed.track_id,
                   sizeof(param->avrcp_track_changed.track_id));
            app_report_event(CMD_PATH_UART, EVENT_TRACK_CHANGED, 0, temp_buff, sizeof(temp_buff));
#if F_APP_AVRCP_CMD_SUPPORT
            bt_avrcp_get_element_attr_req(param->avrcp_track_changed.bd_addr, 7, attr_list);
#endif
        }
        break;

    case BT_EVENT_AVRCP_PLAY_STATUS_RSP:
    case BT_EVENT_AVRCP_PLAY_STATUS_CHANGED:
        {
            uint8_t temp_buff[8];

            memcpy(&temp_buff[0], param->avrcp_play_status_changed.bd_addr, 6);
            temp_buff[6] = param->avrcp_play_status_changed.play_status;
            temp_buff[7] = param->a2dp_config_cmpl.codec_type;

            app_report_event(CMD_PATH_UART, EVENT_PLAYER_STATUS, 0, temp_buff, sizeof(temp_buff));
#if F_APP_AVRCP_CMD_SUPPORT
            bt_avrcp_get_element_attr_req(param->avrcp_play_status_changed.bd_addr, 7, attr_list);
#endif
        }
        break;

    case BT_EVENT_AVRCP_CONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->avrcp_conn_cmpl) + 1];

            temp_buff[0] = AVRCP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->avrcp_conn_cmpl, sizeof(param->avrcp_conn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->avrcp_disconn_cmpl) + 1];

            temp_buff[0] = AVRCP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->avrcp_disconn_cmpl, sizeof(param->avrcp_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_CONN_FAIL:
        {
            uint8_t temp_buff[sizeof(param->avrcp_conn_fail) + 1];

            temp_buff[0] = AVRCP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->avrcp_conn_fail, sizeof(param->avrcp_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            uint8_t temp_buff[8];
            T_APP_BR_LINK *p_link = NULL;

            if (app_link_check_b2s_link(param->acl_conn_disconn.bd_addr))
            {
                p_link = app_link_find_br_link(param->acl_conn_disconn.bd_addr);
                if (p_link)
                {
                    if ((param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_CONN_TIMEOUT)) ||
                        (param->acl_conn_disconn.cause == (HCI_ERR | HCI_ERR_LMP_RESPONSE_TIMEOUT)))
                    {
                        temp_buff[0] = ACL_LINK_STATE_LOST;
                    }
                    else
                    {
                        temp_buff[0] = ACL_LINK_STATE_STANDBY;
                    }

                    memcpy(&temp_buff[1], p_link->bd_addr, 6);
                    temp_buff[7] = p_link->id;
                    app_report_event(CMD_PATH_UART, EVENT_LINK_STATUS, 0, temp_buff, sizeof(temp_buff));
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_SUCCESS:
        {
            uint8_t temp_buff[8];
            T_APP_BR_LINK *p_link = NULL;

            if (app_link_check_b2s_link(param->acl_conn_success.bd_addr))
            {
                p_link = app_link_find_br_link(param->acl_conn_success.bd_addr);
                if (p_link)
                {
                    temp_buff[0] = ACL_LINK_STATE_CONNECTED;
                    memcpy(&temp_buff[1], p_link->bd_addr, 6);
                    temp_buff[7] = p_link->id;
                    app_report_event(CMD_PATH_UART, EVENT_LINK_STATUS, 0, temp_buff, sizeof(temp_buff));
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_FAIL:
        {
            uint8_t temp_buff[8];

            temp_buff[0] = ACL_LINK_STATE_CONN_FAIL;
            memcpy(&temp_buff[1], param->acl_conn_fail.bd_addr, 6);
            temp_buff[7] = 0xFF;
            app_report_event(CMD_PATH_UART, EVENT_LINK_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_ACL_AUTHEN_FAIL:
        {
            uint8_t temp_buff[8];
            T_APP_BR_LINK *p_link = NULL;

            if (app_link_check_b2s_link(param->acl_authen_fail.bd_addr))
            {
                p_link = app_link_find_br_link(param->acl_authen_fail.bd_addr);
                if (p_link)
                {
                    if (param->acl_authen_fail.cause == (HCI_ERR | HCI_ERR_KEY_MISSING))
                    {
                        temp_buff[0] = ACL_LINK_STATE_KEY_MISSING;
                        memcpy(&temp_buff[1], p_link->bd_addr, 6);
                        temp_buff[7] = p_link->id;
                        app_report_event(CMD_PATH_UART, EVENT_LINK_STATUS, 0, temp_buff, sizeof(temp_buff));
                    }
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_NAME_RSP:
        {
            uint8_t temp_buff[47];

            temp_buff[0] = (uint8_t)param->remote_name_rsp.cause;
            memcpy(&temp_buff[1], param->remote_name_rsp.bd_addr, 6);
            memcpy(&temp_buff[7], param->remote_name_rsp.name, 40);
            app_report_event(CMD_PATH_UART, EVENT_REPLY_REMOTE_NAME, 0, temp_buff, sizeof(temp_buff));
        }
        break;

#if F_APP_HFP_CMD_SUPPORT
    case BT_EVENT_HFP_CALLER_ID_IND:
        {
            uint8_t num_len = strlen(param->hfp_caller_id_ind.number);
            uint8_t temp_buff[num_len + 3];
            T_APP_BR_LINK *br_link;

            br_link = app_link_find_br_link(param->hfp_caller_id_ind.bd_addr);
            if (br_link)
            {
                temp_buff[0] = br_link->id;
                temp_buff[1] = CALLER_ID_TYPE_NUMBER;
                temp_buff[2] = num_len;
                memcpy(&temp_buff[3], param->hfp_caller_id_ind.number, num_len);
                app_report_event(CMD_PATH_UART, EVENT_CALLER_ID, 0, temp_buff, sizeof(temp_buff));
            }
        }
        break;

    case BT_EVENT_HFP_CALL_WAITING_IND:
        {
            uint8_t num_len = strlen(param->hfp_call_waiting_ind.number);
            uint8_t temp_buff[num_len + 1];

            temp_buff[0] = num_len;
            memcpy(&temp_buff[1], (uint8_t *)param->hfp_call_waiting_ind.number, num_len);
            app_report_event(CMD_PATH_UART, EVENT_CALL_WAITING, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_SIGNAL_IND:
        {
            uint8_t temp_buff[7];

            temp_buff[0] = param->hfp_signal_ind.state;
            memcpy(&temp_buff[1], &param->hfp_signal_ind.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_HFP_SIGNAL, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_ROAM_IND:
        {
            uint8_t temp_buff[7];

            temp_buff[0] = param->hfp_roam_ind.state;
            memcpy(&temp_buff[1], &param->hfp_roam_ind.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_HFP_ROAM, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_SERVICE_STATUS:
        {
            uint8_t temp_buff[7];

            temp_buff[0] = param->hfp_service_status.status;
            memcpy(&temp_buff[1], &param->hfp_service_status.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_HFP_SERVICE, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_NETWORK_OPERATOR_IND:
        {
            uint8_t name_len = strlen(param->hfp_network_operator_ind.name);
            uint8_t temp_buff[name_len + 2];

            temp_buff[0] = param->hfp_network_operator_ind.mode;
            temp_buff[1] = name_len;
            memcpy(&temp_buff[2], (uint8_t *)param->hfp_network_operator_ind.name, name_len);
            app_report_event(CMD_PATH_UART, EVENT_NETWORK_OPERATOR, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_SUBSCRIBER_NUMBER_IND:
        {
            uint8_t num_len = strlen(param->hfp_subscriber_number_ind.number);
            uint8_t temp_buff[num_len + 3];

            temp_buff[0] = param->hfp_subscriber_number_ind.service;
            temp_buff[1] = param->hfp_subscriber_number_ind.type;
            temp_buff[2] = num_len;
            memcpy(&temp_buff[3], (uint8_t *)param->hfp_subscriber_number_ind.number, num_len);
            app_report_event(CMD_PATH_UART, EVENT_SUBSCRIBER_NUMBER, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_CURRENT_CALL_LIST_IND:
        {
            current_call_list.call_idx = param->hfp_current_call_list_ind.call_idx;
            current_call_list.dir_incoming = param->hfp_current_call_list_ind.dir_incoming;
            current_call_list.status = param->hfp_current_call_list_ind.status;
            current_call_list.mode = param->hfp_current_call_list_ind.mode;
            current_call_list.mpty = param->hfp_current_call_list_ind.mpty;
            current_call_list.type = ((param->hfp_current_call_list_ind.type) & 0xE0) >> 5; // mapping to spec
            current_call_list.num_len = strlen(param->hfp_current_call_list_ind.number);
            memcpy(&current_call_list.number,
                   (const char *)param->hfp_current_call_list_ind.number, current_call_list.num_len + 1);

            app_report_event(CMD_PATH_UART, EVENT_CURRENT_CALLS, 0, &(current_call_list.call_idx),
                             current_call_list.num_len + 8);

            // when call active, cycle report current calls
            if (app_hfp_get_call_status() >= APP_VOICE_ACTIVATION_ONGOING)
            {
                app_start_timer(&timer_idx_report_current_calls, "report_current_calls",
                                app_test_timer_id, APP_TIMER_REPORT_CURRENT_CALLS, 0, true,
                                CYCLE_REPORT_CURRENT_CALLS_TIMER);
            }
        }
        break;

    case BT_EVENT_HFP_BATTERY_IND:
        {
            uint8_t temp_buff[1];

            temp_buff[0] = param->hfp_battery_ind.state;
            app_report_event(CMD_PATH_UART, EVENT_REPORT_HFP_BATTERY, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_SUPPORTED_FEATURES_IND:
        {
            uint8_t temp_buff[10];

            memcpy(&temp_buff[0], &(param->hfp_supported_features_ind.ag_bitmap), 4);
            memcpy(&temp_buff[4], &param->hfp_supported_features_ind.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_REPORT_AG_BRSF, 0, temp_buff, sizeof(temp_buff));
        }
        break;
#endif

    case BT_EVENT_HFP_CONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->hfp_conn_cmpl) + 1];

            if (param->hfp_conn_cmpl.is_hfp)
            {
                temp_buff[0] = HFP_PROFILE_MASK;
            }

            else
            {
                temp_buff[0] = HSP_PROFILE_MASK;
            }

            memcpy(&temp_buff[1], &param->hfp_conn_cmpl, sizeof(param->hfp_conn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_SPK_VOLUME_CHANGED:
        {
            app_report_event(CMD_PATH_UART, EVENT_VOLUME_SYNC, 0, (uint8_t *)&param->hfp_spk_volume_changed,
                             sizeof(param->hfp_spk_volume_changed));
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_ACTIVATION:
        {
            if (param->hfp_voice_recognition_activation.result == BT_HFP_CMD_OK)
            {
                T_APP_CALL_STATUS call_status = APP_VOICE_ACTIVATION_ONGOING;

                app_report_event(CMD_PATH_UART, EVENT_CALL_STATUS, 0, (uint8_t *)&call_status,
                                 sizeof(call_status));
            }
        }
        break;

    case BT_EVENT_HFP_VOICE_RECOGNITION_DEACTIVATION:
        {
            if (param->hfp_voice_recognition_deactivation.result == BT_HFP_CMD_OK)
            {
                T_APP_CALL_STATUS call_status = APP_CALL_IDLE;

                app_report_event(CMD_PATH_UART, EVENT_CALL_STATUS, 0, (uint8_t *)&call_status,
                                 sizeof(call_status));
            }
        }
        break;

    case BT_EVENT_HFP_CALL_STATUS:
        {
            T_APP_CALL_STATUS call_status = APP_CALL_IDLE;

            switch (param->hfp_call_status.curr_status)
            {
            case BT_HFP_CALL_IDLE:
                {
                    call_status = APP_CALL_IDLE;
                }
                break;

            case BT_HFP_CALL_INCOMING:
                {
                    call_status = APP_CALL_INCOMING;
                }
                break;

            case BT_HFP_CALL_OUTGOING:
                {
                    call_status = APP_CALL_OUTGOING;
                }
                break;

            case BT_HFP_CALL_ACTIVE:
                {
                    call_status = APP_CALL_ACTIVE;
                }
                break;

            case BT_HFP_CALL_HELD:
                {
                    //call_status = APP_HFP_CALL_HELD;
                }
                break;

            case BT_HFP_CALL_ACTIVE_WITH_CALL_WAITING:
                {
                    call_status = APP_CALL_ACTIVE_WITH_CALL_WAITING;
                }
                break;

            case BT_HFP_CALL_ACTIVE_WITH_CALL_HELD:
                {
                    call_status = APP_CALL_ACTIVE_WITH_CALL_HELD;
                }
                break;

            default:
                break;
            }

            app_report_event(CMD_PATH_UART, EVENT_CALL_STATUS, 0, (uint8_t *)&call_status,
                             sizeof(call_status));
#if F_APP_HFP_CMD_SUPPORT
            // when call active, send current calls list req
            if (param->hfp_call_status.curr_status != BT_HFP_VOICE_IDLE)
            {
                bt_hfp_current_call_list_req(param->hfp_call_status.bd_addr);
            }
            else
            {
                app_stop_timer(&timer_idx_report_current_calls);
            }
#endif
        }
        break;

    case BT_EVENT_HFP_VENDOR_COMMAND_RESULT:
        {
            app_report_event(CMD_PATH_UART, EVENT_VENDOR_AT_RESULT, 0,
                             (uint8_t *)&param->hfp_vendor_command_result,
                             sizeof(param->hfp_vendor_command_result));
        }
        break;

    case BT_EVENT_HFP_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->hfp_disconn_cmpl) + 1];

#if F_APP_HFP_CMD_SUPPORT
            app_stop_timer(&timer_idx_report_current_calls);
#endif

            if (param->hfp_disconn_cmpl.is_hfp)
            {
                temp_buff[0] = HFP_PROFILE_MASK;
            }
            else
            {
                temp_buff[0] = HSP_PROFILE_MASK;
            }
            memcpy(&temp_buff[1], &param->hfp_disconn_cmpl, sizeof(param->hfp_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_HFP_CONN_FAIL:
        {
            uint8_t temp_buff[sizeof(param->hfp_conn_fail) + 1];

            if (param->hfp_conn_fail.is_hfp)
            {
                temp_buff[0] = HFP_PROFILE_MASK;
            }
            else
            {
                temp_buff[0] = HSP_PROFILE_MASK;
            }
            memcpy(&temp_buff[1], &param->hfp_conn_fail, sizeof(param->hfp_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_IAP_AUTHEN_CMPL:
        {
            uint8_t temp_buff[2];

            temp_buff[0] = IAP_STATUS_AUTHEN_SUCCESS;
            app_report_event(CMD_PATH_UART, EVENT_IAP_STATUS, 0, temp_buff, 2);
        }
        break;

    case BT_EVENT_IAP_DATA_SESSION_OPEN:
        {
            uint8_t temp_buff[sizeof(param->iap_data_session_open) + 1];

            temp_buff[0] = IAP_STATUS_DATA_SESSION_OPEN;
            memcpy(&temp_buff[1], &param->iap_data_session_open, sizeof(param->iap_data_session_open));
            app_report_event(CMD_PATH_UART, EVENT_IAP_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_IAP_DATA_SESSION_CLOSE:
        {
            uint8_t temp_buff[sizeof(param->iap_data_session_close) + 1];

            temp_buff[0] = IAP_STATUS_DATA_SESSION_CLOSE;
            memcpy(&temp_buff[1], &param->iap_data_session_close, sizeof(param->iap_data_session_close));
            app_report_event(CMD_PATH_UART, EVENT_IAP_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_IAP_DATA_SENT:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->iap_data_sent.bd_addr);

            if (p_link != NULL)
            {
                if (p_link->resume_fg)
                {
                    app_report_event(CMD_PATH_UART, EVENT_RESUME_DATA_TRANSFER, 0, &p_link->id, 1);
                }
            }
        }
        break;

    case BT_EVENT_IAP_CONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->iap_conn_cmpl) + 1];

            temp_buff[0] = IAP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->iap_conn_cmpl, sizeof(param->iap_conn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_IAP_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->iap_disconn_cmpl) + 1];

            temp_buff[0] = IAP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->iap_disconn_cmpl, sizeof(param->iap_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_IAP_CONN_FAIL:
        {
            uint8_t temp_buff[sizeof(param->iap_conn_fail) + 1];

            temp_buff[0] = IAP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->iap_conn_fail, sizeof(param->iap_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_PBAP_CONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->pbap_conn_cmpl) + 1];

            temp_buff[0] = PBAP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->pbap_conn_cmpl, sizeof(param->pbap_conn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_PBAP_CONN_FAIL:
        {
#if F_APP_PBAP_CMD_SUPPORT
            uint8_t temp_buff[sizeof(param->pbap_conn_fail) + 1];

            temp_buff[0] = PBAP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->pbap_conn_fail, sizeof(param->pbap_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
#endif
        }
        break;

    case BT_EVENT_PBAP_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->pbap_disconn_cmpl) + 1];

            temp_buff[0] = PBAP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->pbap_disconn_cmpl, sizeof(param->pbap_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_SPP_CONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->spp_conn_cmpl) + 1];

            temp_buff[0] = SPP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->spp_conn_cmpl, sizeof(param->spp_conn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->spp_disconn_cmpl) + 1];

            temp_buff[0] = SPP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->spp_disconn_cmpl, sizeof(param->spp_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_SPP_CONN_FAIL:
        {
            uint8_t temp_buff[sizeof(param->spp_conn_fail) + 1];

            temp_buff[0] = SPP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->spp_conn_fail, sizeof(param->spp_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            uint8_t temp_buff1[sizeof(param->remote_conn_cmpl) + 1];
            uint8_t temp_buff2[7];

            temp_buff1[0] = RDTP_PROFILE_MASK;
            temp_buff2[0] = app_cfg_nv.bud_role;

            memcpy(&temp_buff1[1], &param->remote_conn_cmpl, sizeof(param->remote_conn_cmpl));
            memcpy(&temp_buff2[1], app_cfg_nv.bud_local_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_CONNECT_STATUS, 0, temp_buff1, sizeof(temp_buff1));
            app_report_event(CMD_PATH_UART, EVENT_DEVICE_ROLE, 0, temp_buff2, sizeof(temp_buff2));
        }
        break;

    case BT_EVENT_REMOTE_DISCONN_CMPL:
        {
            uint8_t temp_buff[sizeof(param->remote_disconn_cmpl) + 1];

            temp_buff[0] = RDTP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->remote_disconn_cmpl, sizeof(param->remote_disconn_cmpl));
            app_report_event(CMD_PATH_UART, EVENT_DISCONNECT_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_REMOTE_CONN_FAIL:
        {
            uint8_t temp_buff[sizeof(param->remote_conn_fail) + 1];

            temp_buff[0] = RDTP_PROFILE_MASK;
            memcpy(&temp_buff[1], &param->remote_conn_fail, sizeof(param->remote_conn_fail));
            app_report_event(CMD_PATH_UART, EVENT_PROFILE_CONNECT_FAIL_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_REMOTE_ROLESWAP_STATUS:
        {
            T_BT_ROLESWAP_STATUS event;
            uint8_t temp_buff[7];

            event = param->remote_roleswap_status.status;
            if (event == BT_ROLESWAP_STATUS_SUCCESS)
            {
                temp_buff[0] = app_cfg_nv.bud_role;
                memcpy(&temp_buff[1], app_cfg_nv.bud_local_addr, 6);
                app_report_event(CMD_PATH_UART, EVENT_DEVICE_ROLE, 0, temp_buff, sizeof(temp_buff));
            }
        }
        break;

    case BT_EVENT_LINK_RSSI_INFO:
        {
            uint8_t temp_buff[1];

            temp_buff[0] = param->link_rssi_info.rssi;
            app_report_event(CMD_PATH_SPP, EVENT_LEGACY_RSSI, 0, temp_buff, sizeof(temp_buff));
        }
        break;

#if F_APP_DEVICE_CMD_SUPPORT
    case BT_EVENT_INQUIRY_RESULT:
        {
            uint8_t temp_buff[11 + GAP_DEVICE_NAME_LEN];
            uint8_t name_len;

            name_len = strlen(param->inquiry_result.name);

            memcpy(&temp_buff[0], &param->inquiry_result.bd_addr, 6);
            memcpy(&temp_buff[6], &(param->inquiry_result.cod), 4);
            temp_buff[10] = name_len;
            memcpy(&temp_buff[11], &(param->inquiry_result.name), name_len);
            app_report_event(CMD_PATH_UART, EVENT_INQUIRY_RESULT, 0, temp_buff, name_len + 11);
        }
        break;

    case BT_EVENT_INQUIRY_RSP:
        {
            uint8_t temp_buff[1];

            if (param->inquiry_rsp.cause == 0x00)
            {
                temp_buff[0] = INQUIRY_STATE_VALID;
            }
            else
            {
                temp_buff[0] = INQUIRY_STATE_ERROR;
            }
            app_report_event(CMD_PATH_UART, EVENT_INQUIRY_STATE, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_PERIODIC_INQUIRY_RSP:
        {
            uint8_t temp_buff[1];

            if (param->periodic_inquiry_rsp.cause == 0x00)
            {
                temp_buff[0] = INQUIRY_STATE_VALID;
            }
            else
            {
                temp_buff[0] = INQUIRY_STATE_ERROR;
            }
            app_report_event(CMD_PATH_UART, EVENT_INQUIRY_STATE, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_INQUIRY_CANCEL_RSP:
    case BT_EVENT_PERIODIC_INQUIRY_CANCEL_RSP:
        {
            uint8_t temp_buff[1];

            temp_buff[0] = INQUIRY_STATE_CANCEL;
            app_report_event(CMD_PATH_UART, EVENT_INQUIRY_STATE, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_SDP_DISCOV_CMPL:
        {
            uint8_t temp_buff[7];

            if (param->sdp_discov_cmpl.cause == 0x00)
            {
                temp_buff[0] = SERVICES_SEARCH_STATE_SUCCESS;
            }
            else
            {
                temp_buff[0] = SERVICES_SEARCH_STATE_FAIL;
            }

            memcpy(&temp_buff[1], &param->sdp_discov_cmpl.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_SERVICES_SEARCH_STATE, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_SDP_DISCOV_STOP:
        {
            uint8_t temp_buff[7];

            temp_buff[0] = SERVICES_SEARCH_STATE_STOP;
            memcpy(&temp_buff[1], &param->sdp_discov_stop.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_SERVICES_SEARCH_STATE, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_ACL_CONN_IND:
        {
            uint8_t temp_buff[6];

            memcpy(&temp_buff[0], &param->acl_conn_ind.bd_addr, 6);
            memcpy(acl_conn_ind_bd_addr, &param->acl_conn_ind.bd_addr, 6);
            app_report_event(CMD_PATH_UART, EVENT_PAIRING_REQUEST, 0, temp_buff, sizeof(temp_buff));

            if (app_cmd_get_auto_reject_conn_req_flag() == true)
            {
                gap_br_reject_acl_conn_req(param->acl_conn_ind.bd_addr, GAP_ACL_REJECT_LIMITED_RESOURCE);
            }
        }
        break;

    case BT_EVENT_LINK_USER_CONFIRMATION_REQ:
        {
            uint8_t temp_buff[10];
            uint8_t io_cap;

            gap_get_param(GAP_PARAM_BOND_IO_CAPABILITIES, &io_cap);
            if ((io_cap == GAP_IO_CAP_DISPLAY_ONLY ||
                 io_cap == GAP_IO_CAP_KEYBOARD_ONLY ||
                 io_cap == GAP_IO_CAP_NO_INPUT_NO_OUTPUT) &&
                param->link_user_confirmation_req.just_works == true)
            {
                gap_br_user_cfm_req_cfm(param->link_user_confirmation_req.bd_addr, GAP_CFM_CAUSE_ACCEPT);
            }
            else
            {
                temp_buff[0] = (uint8_t)param->link_user_confirmation_req.display_value;
                temp_buff[1] = (uint8_t)(param->link_user_confirmation_req.display_value >> 8);
                temp_buff[2] = (uint8_t)(param->link_user_confirmation_req.display_value >> 16);
                temp_buff[3] = (uint8_t)(param->link_user_confirmation_req.display_value >> 24);
                memcpy(&temp_buff[4], &param->link_user_confirmation_req.bd_addr, 6);
                memcpy(user_confirmation_bd_addr, &param->link_user_confirmation_req.bd_addr, 6);

                app_report_event(CMD_PATH_UART, EVENT_REPORT_SSP_NUMERIC_VALUE, 0, temp_buff, sizeof(temp_buff));
            }
        }
        break;
#endif

#if F_APP_AVRCP_CMD_SUPPORT
    case BT_EVENT_AVRCP_APP_SETTING_ATTRS_LIST_RSP:
        {
            uint8_t num_of_attr = param->avrcp_app_setting_attrs_list_rsp.num_of_attr;
            uint8_t temp_buff[num_of_attr + 9];

            // temp_buff[0] ~ temp_buff[5]: bd_addr; temp_buff[6]: state
            memcpy(&temp_buff[0], &param->avrcp_app_setting_attrs_list_rsp, 7);
            temp_buff[7] = 0x00; //Single packet
            temp_buff[8] = num_of_attr;
            memcpy(&temp_buff[9], param->avrcp_app_setting_attrs_list_rsp.p_attr_id, num_of_attr);

            app_report_event(CMD_PATH_UART, EVENT_AVRCP_REPORT_LIST_SETTING_ATTR, 0, temp_buff,
                             sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_APP_SETTING_VALUES_LIST_RSP:
        {
            uint8_t num_of_value = param->avrcp_app_setting_values_list_rsp.num_of_value;
            uint8_t temp_buff[num_of_value + 9];

            // temp_buff[0] ~ temp_buff[5]: bd_addr; temp_buff[6]: state
            memcpy(&temp_buff[0], &param->avrcp_app_setting_values_list_rsp, 7);
            temp_buff[7] = 0x00; //Single packet
            temp_buff[8] = num_of_value;
            memcpy(&temp_buff[9], param->avrcp_app_setting_values_list_rsp.p_value, num_of_value);

            app_report_event(CMD_PATH_UART, EVENT_AVRCP_REPORT_LIST_SETTING_VALUE, 0, temp_buff,
                             sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_APP_SETTING_GET_RSP:
        {
            uint8_t num_of_attr = param->avrcp_app_setting_get_rsp.num_of_attr;
            uint8_t temp_buff[num_of_attr * 2 + 9];

            // temp_buff[0] ~ temp_buff[5]: bd_addr; temp_buff[6]: state
            memcpy(&temp_buff[0], &param->avrcp_app_setting_get_rsp, 7);
            temp_buff[7] = 0x00; //Single packet
            temp_buff[8] = num_of_attr;
            memcpy(&temp_buff[9], param->avrcp_app_setting_get_rsp.p_app_setting, num_of_attr * 2);

            app_report_event(CMD_PATH_UART, EVENT_AVRCP_REPORT_CURRENT_VALUE, 0, temp_buff,
                             sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_APP_SETTING_CHANGED:
        {
            uint8_t num_of_attr = param->avrcp_app_setting_changed.num_of_attr;
            uint8_t temp_buff[num_of_attr * 2 + 7];

            // temp_buff[0] ~ temp_buff[5]: bd_addr; temp_buff[6]: num_of_attr
            memcpy(&temp_buff[0], &param->avrcp_app_setting_changed, 7);
            memcpy(&temp_buff[7], param->avrcp_app_setting_changed.p_app_setting, num_of_attr * 2);

            app_report_event(CMD_PATH_UART, EVENT_AVRCP_REPORT_SETTING_CHANGED, 0, temp_buff,
                             sizeof(temp_buff));
            bt_avrcp_get_element_attr_req(param->avrcp_app_setting_changed.bd_addr, 7, attr_list);
        }
        break;

    case BT_EVENT_AVRCP_ABSOLUTE_VOLUME_SET:
        {
            //This event will be sent when the phone volume changes
            app_report_event(CMD_PATH_UART, EVENT_VOLUME_SYNC, 0,
                             (uint8_t *)&param->avrcp_absolute_volume_set,
                             sizeof(param->avrcp_absolute_volume_set));
        }
        break;

    case BT_EVENT_AVRCP_REG_VOLUME_CHANGED:
        {
            //This event will be sent when the BT volume changes
            uint8_t temp_buff[7];

            memcpy(&temp_buff[0], &param->avrcp_reg_volume_changed, 6);
            //0~0F corresponds to 0~7F
            temp_buff[6] = (audio_volume_out_get(AUDIO_STREAM_TYPE_PLAYBACK) * 0x7F +
                            app_dsp_cfg_vol.playback_volume_max / 2) / app_cfg_volume.playback_volume_max;

            app_report_event(CMD_PATH_UART, EVENT_VOLUME_SYNC, 0, temp_buff,
                             sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_GET_PLAY_STATUS_RSP:
        {
            uint8_t temp_buff[10];

            temp_buff[0] = 0x00; //Single packet;
            memcpy(&temp_buff[1], &(param->avrcp_get_play_status_rsp.length_ms), 4);
            memcpy(&temp_buff[5], &(param->avrcp_get_play_status_rsp.position_ms), 4);
            temp_buff[9] = param->avrcp_get_play_status_rsp.play_status;

            app_report_event(CMD_PATH_UART, EVENT_AVRCP_REPORT_PLAYER_STATUS, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case BT_EVENT_AVRCP_ELEM_ATTR:
        {
            if (param->avrcp_elem_attr.state == 0)
            {
                uint8_t num_of_attr = param->avrcp_elem_attr.num_of_attr;
                uint8_t i;

                for (i = 0; i < num_of_attr; i++)
                {
                    uint8_t attr_len = param->avrcp_elem_attr.attr[i].length;
                    uint8_t temp_buff[9 + attr_len] ;

                    temp_buff[0] = 0x00; //Single packet
                    memcpy(&temp_buff[1], &(param->avrcp_elem_attr.attr[i].attribute_id), 4);
                    memcpy(&temp_buff[5], &(param->avrcp_elem_attr.attr[i].character_set_id), 2);
                    memcpy(&temp_buff[7], &(param->avrcp_elem_attr.attr[i].length), 2);
                    memcpy(&temp_buff[9], param->avrcp_elem_attr.attr[i].p_buf, attr_len);

                    app_report_event(CMD_PATH_UART, EVENT_AVRCP_REPORT_ELEMENT_ATTR, 0, temp_buff, sizeof(temp_buff));
                }
            }
        }
        break;
#endif

#if F_APP_PBAP_CMD_SUPPORT
    case BT_EVENT_PBAP_GET_PHONE_BOOK_CMPL:
        {
            app_pbap_split_pbap_data(param->pbap_get_phone_book_cmpl.p_data,
                                     param->pbap_get_phone_book_cmpl.data_len);

            // when received end data, check if the next obj needs to be downloaded
            if (param->pbap_get_phone_book_cmpl.data_end)
            {
                app_pbap_cmd_pbap_download_check(param->pbap_get_phone_book_cmpl.bd_addr);
            }
            else
            {
                if (app_pbap_get_auto_pbap_download_continue_flag())
                {
                    app_pbap_pull_continue_timer_start();
                }
            }
        }
        break;

    case BT_EVENT_PBAP_GET_PHONE_BOOK_SIZE_CMPL:
        {
            app_pbap_cmd_pbap_download(param->pbap_get_phone_book_size_cmpl.bd_addr,
                                       param->pbap_get_phone_book_size_cmpl.pb_size);
        }
        break;
#endif

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_test_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_test_audio_probe_dsp_cback(T_AUDIO_PROBE_EVENT event, void *buf)
{
    uint8_t app_idx;
    uint32_t *event_buff = (uint32_t *)buf;

    if (event != PROBE_SCENARIO_STATE)
    {
        return;
    }

    for (app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
    {
        if (app_db.br_link[app_idx].connected_profile & SPP_PROFILE_MASK)
        {
            app_report_event(CMD_PATH_SPP, EVENT_AUDIO_DSP_CTRL_INFO, app_idx, (uint8_t *)event_buff, 4);
        }
        else if (app_db.br_link[app_idx].connected_profile & IAP_PROFILE_MASK)
        {
            app_report_event(CMD_PATH_IAP, EVENT_AUDIO_DSP_CTRL_INFO, app_idx, (uint8_t *)event_buff, 4);
        }
    }
}

void app_test_init(void)
{
    if (app_cfg_const.enable_rtk_vendor_cmd)
    {
        audio_mgr_cback_register(app_test_audio_cback);
        bt_mgr_cback_register(app_test_bt_cback);
        app_timer_reg_cb(app_test_timeout_cb, &app_test_timer_id);
        audio_probe_dsp_cback_register(app_test_audio_probe_dsp_cback);
    }
}
#endif
