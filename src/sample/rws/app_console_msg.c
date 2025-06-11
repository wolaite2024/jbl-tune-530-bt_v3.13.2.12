/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include "stdlib_corecrt.h"
#include "bt_types.h"
#include "trace.h"
#include "os_mem.h"
#include "bqb.h"
#include "app_mmi.h"
#include "app_main.h"
#include "gap_br.h"
#include "cli_power.h"
#include "app_msg.h"
#include "app_console_msg.h"
#include "app_cmd.h"
#include "app_bt_policy_api.h"
#include "app_link_util.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "bt_avrcp.h"
#include "btm.h"
#include "gap_conn_le.h"
#if ISOC_TEST_SUPPORT
#include "cli_isoc.h"
#include "app_isoc_test.h"
#endif

#if (F_APP_BQB_CLI_HFP_AG_SUPPORT == 1)
#include "bt_hfp_ag.h"
#endif
#if (F_APP_BQB_CLI_MAP_SUPPORT == 1)
#include "os_sched.h"
#include "bt_map.h"
#endif


#if F_APP_CCP_SUPPORT
#include "ccp_client.h"
#endif
#if F_APP_MCP_SUPPORT
#include "mcp_client.h"
#endif
#if F_APP_VCS_SUPPORT
#include "vcs_mgr.h"
#endif

#if F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_CT_SUPPORT
#include "app_lea_adv.h"
#endif

#if F_APP_TMAP_BMR_SUPPORT
#if F_APP_LE_AUDIO_CLI_TEST
#include "app_cfg.h"
#include "cli_le_audio.h"
#endif
#include "bass_mgr.h"
#include "ble_isoch_def.h"
#include "gap_ext_scan.h"
#include "app_lea_broadcast_audio.h"
#include "app_lea_scan.h"
#endif

#if F_APP_TMAP_CT_SUPPORT
#include <string.h>
#include "app_lea_ccp.h"
#include "app_lea_pacs.h"
#endif

#if F_APP_CONSOLE_SUPPORT
#if F_APP_HAS_SUPPORT
#include "has_def.h"
#include "has_mgr.h"
#include "app_ble_common_adv.h"
#include "app_lea_has_preset_record.h"
#endif

#define SCO_PKT_TYPES_HV3_EV3_2EV3          (GAP_PKT_TYPE_HV3 | GAP_PKT_TYPE_EV3 | GAP_PKT_TYPE_NO_3EV3 | GAP_PKT_TYPE_NO_2EV5 | GAP_PKT_TYPE_NO_3EV5)

void app_console_handle_msg(T_IO_MSG console_msg)
{
    uint16_t  subtype;
    uint16_t  id;
    uint16_t cmd_len;
    uint8_t rx_seqn;
    uint8_t   action;
    uint8_t  *p;

    p       = console_msg.u.buf;
    subtype = console_msg.subtype;
    switch (subtype)
    {
    case IO_MSG_CONSOLE_STRING_RX:
        LE_STREAM_TO_UINT16(id, p);

        if (id == POWER_CMD)
        {
            LE_STREAM_TO_UINT8(action, p);

            if (action == POWER_ACTION_POWER_ON)
            {
                app_mmi_handle_action(MMI_DEV_POWER_ON);
            }
            else if (action == POWER_ACTION_POWER_OFF)
            {
                app_db.power_off_cause = POWER_OFF_CAUSE_CMD_SET;
                app_mmi_handle_action(MMI_DEV_POWER_OFF);
            }
        }
#if F_APP_BQB_CLI_SUPPORT
        else if (id == BQB_CMD_SDP)
        {
            T_GAP_UUID_DATA data;
            uint16_t uuid;

            LE_STREAM_TO_UINT8(action, p);
            LE_STREAM_TO_UINT16(uuid, p);
            data.uuid_16 = uuid;

            if (action == BQB_ACTION_SDP_SEARCH)
            {
                gap_br_start_sdp_discov(p, GAP_UUID16, data);
            }
        }
        else if (id == BQB_CMD_AVDTP)
        {
            LE_STREAM_TO_UINT8(action, p);

            if (action == BQB_ACTION_AVDTP_OPEN)
            {
                bt_a2dp_stream_open_req(p);
            }
            else if (action == BQB_ACTION_AVDTP_START)
            {
                bt_a2dp_stream_start_req(p);
            }
            else if (action == BQB_ACTION_AVDTP_CLOSE)
            {
                bt_a2dp_stream_close_req(p);
            }
            else if (action == BQB_ACTION_AVDTP_ABORT)
            {
                bt_a2dp_stream_abort_req(p);
            }
            else if (action == BQB_ACTION_AVDTP_CONNECT_SIGNAL)
            {
                app_bt_policy_default_connect(p, A2DP_PROFILE_MASK, false);
            }
            else if (action == BQB_ACTION_AVDTP_CONNECT_STREAM)
            {
                //api removed, use bt_a2dp_stream_open_req() to connect_stream_chann
            }
            else if (action == BQB_ACTION_AVDTP_DISCONNECT)
            {
                app_bt_policy_disconnect(p, A2DP_PROFILE_MASK);
            }
        }
        else if (id == BQB_CMD_AVRCP)
        {
            LE_STREAM_TO_UINT8(action, p);

            if (action == BQB_ACTION_AVRCP_CONNECT)
            {
                bt_avrcp_connect_req(p);
            }
            else if (action == BQB_ACTION_AVRCP_CONNECT_CONTROLLER)
            {
                T_LINKBACK_SEARCH_PARAM search_param;
                search_param.is_target = true;
                app_bt_policy_special_connect(p, AVRCP_PROFILE_MASK, &search_param);
            }
            else if (action == BQB_ACTION_AVRCP_CONNECT_TARGET)
            {
                T_LINKBACK_SEARCH_PARAM search_param;
                search_param.is_target = false;
                app_bt_policy_special_connect(p, AVRCP_PROFILE_MASK, &search_param);
            }
            else if (action == BQB_ACTION_AVRCP_DISCONNECT)
            {
                app_bt_policy_disconnect(p, AVRCP_PROFILE_MASK);
            }
            else if (action == BQB_ACTION_AVRCP_GET_PLAY_STATUS)
            {
                bt_avrcp_get_play_status_req(p);
            }
            else if (action == BQB_ACTION_AVRCP_GET_ELEMENT_ATTR)
            {
                uint8_t attr = BT_AVRCP_ELEM_ATTR_TITLE;
                bt_avrcp_get_element_attr_req(p, 1, &attr);
            }
            else if (action == BQB_ACTION_AVRCP_PLAY)
            {
                bt_avrcp_play(p);
            }
            else if (action == BQB_ACTION_AVRCP_PAUSE)
            {
                bt_avrcp_pause(p);
            }
            else if (action == BQB_ACTION_AVRCP_STOP)
            {
                bt_avrcp_stop(p);
            }
            else if (action == BQB_ACTION_AVRCP_REWIND)
            {
                bt_avrcp_rewind_start(p);
                bt_avrcp_rewind_stop(p);
            }
            else if (action == BQB_ACTION_AVRCP_FASTFORWARD)
            {
                bt_avrcp_fast_forward_start(p);
                bt_avrcp_fast_forward_stop(p);
            }
            else if (action == BQB_ACTION_AVRCP_FORWARD)
            {
                bt_avrcp_forward(p);
            }
            else if (action == BQB_ACTION_AVRCP_BACKWARD)
            {
                bt_avrcp_backward(p);
            }
            else if (action == BQB_ACTION_AVRCP_NOTIFY_VOLUME)
            {
                uint8_t   vol;
                LE_STREAM_TO_UINT8(vol, p);
                bt_avrcp_volume_change_req(p, vol);
            }
        }
        else if (id == BQB_CMD_RFCOMM)
        {
            LE_STREAM_TO_UINT8(action, p);

            switch (action)
            {
            case BQB_ACTION_RFCOMM_CONNECT_SPP:
                app_bt_policy_default_connect(p, SPP_PROFILE_MASK, false);
                break;
            case BQB_ACTION_RFCOMM_CONNECT_HFP:
                app_bt_policy_default_connect(p, HFP_PROFILE_MASK, false);
                break;
            case BQB_ACTION_RFCOMM_CONNECT_HSP:
                app_bt_policy_default_connect(p, HSP_PROFILE_MASK, false);
                break;
            case BQB_ACTION_RFCOMM_CONNECT_PBAP:
                app_bt_policy_default_connect(p, PBAP_PROFILE_MASK, false);
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_SPP:
                app_bt_policy_disconnect(p, SPP_PROFILE_MASK);
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_HFP:
                app_bt_policy_disconnect(p, HFP_PROFILE_MASK);
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_HSP:
                app_bt_policy_disconnect(p, HSP_PROFILE_MASK);
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_PBAP:
                app_bt_policy_disconnect(p, PBAP_PROFILE_MASK);
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_ALL:
                break;
#if F_APP_BQB_CLI_HFP_AG_SUPPORT
            case BQB_ACTION_RFCOMM_CONNECT_HFP_AG:
                break;
            case BQB_ACTION_RFCOMM_CONNECT_HSP_AG:
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_HFP_AG:
                break;
            case BQB_ACTION_RFCOMM_DISCONNECT_HSP_AG:
                break;
#endif
            default:

                break;
            }
        }
        else if (id == BQB_CMD_HFHS)
        {
            LE_STREAM_TO_UINT8(action, p);

            switch (action)
            {
            case BQB_ACTION_HFHS_CONNECT_SCO:
                bt_hfp_audio_connect_req(p);
                break;
            case BQB_ACTION_HFHS_DISCONNECT_SCO:
                bt_hfp_audio_disconnect_req(p);
                break;
            case BQB_ACTION_HFHS_CALL_ANSWER:
                bt_hfp_call_answer_req(p);
                break;
            case BQB_ACTION_HFHS_CALL_REDIAL:
                bt_hfp_dial_last_number_req(p);
                break;
            case BQB_ACTION_HFHS_CALL_ACTIVE:
                bt_hfp_release_active_call_accept_held_or_waiting_call_req(p);
                break;
            case BQB_ACTION_HFHS_CALL_END:
                bt_hfp_call_terminate_req(p);
                break;
            case BQB_ACTION_HFHS_CALL_REJECT:
                bt_hfp_call_terminate_req(p);
                break;
            case BQB_ACTION_HFHS_VOICE_RECOGNITION_ACTIVATE:
                bt_hfp_voice_recognition_enable_req(p);
                break;
            case BQB_ACTION_HFHS_VOICE_RECOGNITION_DEACTIVATE:
                bt_hfp_voice_recognition_disable_req(p);
                break;
            case BQB_ACTION_HFHS_SPK_GAIN_LEVEL_REPORT:
                {
                    uint8_t   level;
                    LE_STREAM_TO_UINT8(level, p);
                    bt_hfp_speaker_gain_level_report(p, level);
                }
                break;

            case BQB_ACTION_HFHS_MIC_GAIN_LEVEL_REPORT:
                {
                    uint8_t   level;
                    LE_STREAM_TO_UINT8(level, p);
                    bt_hfp_microphone_gain_level_report(p, level);
                }
                break;

            case BQB_ACTION_HFHS_SCO_CONN_REQ:
                {
                    uint16_t   voice_setting;
                    uint8_t    retrans_effort;
                    LE_STREAM_TO_UINT16(voice_setting, p);
                    LE_STREAM_TO_UINT8(retrans_effort, p);
                    gap_br_send_sco_conn_req(p, 8000, 8000, 12, voice_setting, retrans_effort,
                                             SCO_PKT_TYPES_HV3_EV3_2EV3);
                }
                break;

            default:
                break;
            }
        }
#if F_APP_BQB_CLI_HFP_AG_SUPPORT
        else if (id == BQB_CMD_HFHS_AG)
        {
            LE_STREAM_TO_UINT8(action, p);

            switch (action)
            {
            case BQB_ACTION_HFHS_AG_CONNECT_SCO:
                {
                    bt_hfp_ag_audio_connect_req(p);
                }
                break;
            case BQB_ACTION_HFHS_AG_DISCONNECT_SCO:
                bt_hfp_ag_audio_disconnect_req(p);
                break;
            case BQB_ACTION_HFHS_AG_CALL_INCOMING:
                {
                    char      call_num[20];
                    uint8_t   call_num_len;
                    uint8_t   call_type;
                    call_num_len = strlen((char *)p) + 1;
                    memcpy_s(call_num, sizeof(call_num), p, call_num_len);
                    p += call_num_len;
                    LE_STREAM_TO_UINT8(call_type, p);

                    bt_hfp_ag_call_incoming(p, call_num, call_num_len, call_type);
                }
                break;
            case BQB_ACTION_HFHS_AG_CALL_ANSWER:
                bt_hfp_ag_call_answer(p);
                break;
            case BQB_ACTION_HFHS_AG_CALL_TERMINATE:
                bt_hfp_ag_call_terminate(p);
                break;
            case  BQB_ACTION_HFHS_AG_CALL_DIAL:
                bt_hfp_ag_call_dial(p);
                break;
            case  BQB_ACTION_HFHS_AG_CALL_ALERTING:
                bt_hfp_ag_call_alert(p);
                break;
            case  BQB_ACTION_HFHS_AG_CALL_WAITING:
                {
                    char      call_num[20];
                    uint8_t   call_num_len;
                    uint8_t   call_type;

                    call_num_len = strlen((char *)p) + 1;
                    memcpy_s(call_num, sizeof(call_num), p, call_num_len);
                    p += call_num_len;
                    LE_STREAM_TO_UINT8(call_type, p);

                    bt_hfp_ag_call_waiting_send(p, call_num, call_num_len, call_type);
                }
                break;

            case BQB_ACTION_HFHS_AG_MIC_GAIN_LEVEL_REPORT:
                {
                    uint8_t   level;
                    LE_STREAM_TO_UINT8(level, p);
                    bt_hfp_ag_microphone_gain_set(p, level);
                }
                break;

            case BQB_ACTION_HFHS_AG_SPEAKER_GAIN_LEVEL_REPORT:
                {
                    uint8_t   level;
                    LE_STREAM_TO_UINT8(level, p);
                    bt_hfp_ag_speaker_gain_set(p, level);
                }
                break;

            case BQB_ACTION_HFHS_AG_RING_INTERVAL_SET:
                {
                    uint16_t   ring_interval;
                    LE_STREAM_TO_UINT16(ring_interval, p);
                    bt_hfp_ag_ring_interval_set(p, ring_interval);
                }
                break;

            case BQB_ACTION_HFHS_AG_INBAND_RINGING_SET:
                {
                    uint8_t   ring_enable;
                    LE_STREAM_TO_UINT8(ring_enable, p);
                    bt_hfp_ag_inband_ringing_set(p, (ring_enable == 1));
                }
                break;

            case BQB_ACTION_HFHS_AG_CURRENT_CALLS_LIST_SEND:
                {
                    uint8_t     clcc_call_idx;
                    uint8_t     clcc_call_dir;
                    uint8_t     clcc_call_status;
                    uint8_t     clcc_call_mode;
                    uint8_t     clcc_call_mpty;
                    char        call_num[20];
                    uint8_t     call_type;
                    uint8_t   call_num_len;

                    LE_STREAM_TO_UINT8(clcc_call_idx, p);
                    LE_STREAM_TO_UINT8(clcc_call_dir, p);
                    LE_STREAM_TO_UINT8(clcc_call_status, p);
                    LE_STREAM_TO_UINT8(clcc_call_mode, p);
                    LE_STREAM_TO_UINT8(clcc_call_mpty, p);

                    call_num_len = strlen((char *)p) + 1;
                    memcpy_s(call_num, sizeof(call_num), p, call_num_len);
                    p += call_num_len;
                    LE_STREAM_TO_UINT8(call_type, p);

                    bt_hfp_ag_current_calls_list_send(p,
                                                      clcc_call_idx,
                                                      clcc_call_dir,
                                                      (T_BT_HFP_AG_CURRENT_CALL_STATUS) clcc_call_status,
                                                      (T_BT_HFP_AG_CURRENT_CALL_MODE) clcc_call_mode,
                                                      clcc_call_mpty,
                                                      call_num, call_type);
                }
                break;

            case BQB_ACTION_HFHS_AG_SEVICE_STATUS_SEND:
                {
                    uint8_t   status;
                    LE_STREAM_TO_UINT8(status, p);
                    bt_hfp_ag_service_indicator_send(p, (T_BT_HFP_AG_SERVICE_INDICATOR) status);
                }
                break;

            case  BQB_ACTION_HFHS_AG_CALL_STATUS_SEND:
                {
                    uint8_t   status;
                    LE_STREAM_TO_UINT8(status, p);
                    bt_hfp_ag_call_indicator_send(p, (T_BT_HFP_AG_CALL_INDICATOR) status);
                }
                break;

            case BQB_ACTION_HFHS_AG_CALL_SETUP_STATUS_SEND:
                {
                    uint8_t   status;
                    LE_STREAM_TO_UINT8(status, p);
                    bt_hfp_ag_call_setup_indicator_send(p, (T_BT_HFP_AG_CALL_SETUP_INDICATOR) status);
                }
                break;

            case BQB_ACTION_HFHS_AG_CALL_HELD_STATUS_SEND:
                {
                    uint8_t   status;
                    LE_STREAM_TO_UINT8(status, p);
                    bt_hfp_ag_call_held_indicator_send(p, (T_BT_HFP_AG_CALL_HELD_INDICATOR) status);
                }
                break;

            case BQB_ACTION_HFHS_AG_SIGNAL_STRENGTH_SEND:
                {
                    uint8_t   value;
                    LE_STREAM_TO_UINT8(value, p);
                    bt_hfp_ag_signal_strength_send(p, value);
                }
                break;

            case BQB_ACTION_HFHS_AG_ROAMING_STATUS_SEND:
                {
                    uint8_t   value;
                    LE_STREAM_TO_UINT8(value, p);
                    bt_hfp_ag_roaming_indicator_send(p, (T_BT_HFP_AG_ROAMING_INICATOR) value);
                }
                break;

            case BQB_ACTION_HFHS_AG_BATTERY_CHANGE_SEND:
                {
                    uint8_t   value;
                    LE_STREAM_TO_UINT8(value, p);
                    bt_hfp_ag_battery_charge_send(p, value);
                }
                break;

            case BQB_ACTION_HFHS_AG_OK_SEND:
                {
                    bt_hfp_ag_ok_send(p);
                }
                break;

            case BQB_ACTION_HFHS_AG_ERROR_SEND:
                {
                    uint8_t   cme_error_code;
                    LE_STREAM_TO_UINT8(cme_error_code, p);
                    bt_hfp_ag_error_send(p, cme_error_code);
                }
                break;

            case  BQB_ACTION_HFHS_AG_NETWORK_OPERATOR_SEND:
                {
                    char      operator[17];
                    uint8_t   operator_len;

                    operator_len = strlen((char *)p) + 1;
                    memcpy_s(operator, sizeof(operator), p, operator_len);
                    p += operator_len;

                    bt_hfp_ag_network_operator_name_send(p, operator, operator_len);
                    bt_hfp_ag_ok_send(p);
                }
                break;

            case  BQB_ACTION_HFHS_AG_SUBSCRIBER_NUMBER_SEND:
                {
                    char      call_num[20];
                    uint8_t   call_num_len;
                    uint8_t   call_type;
                    uint8_t   service;

                    call_num_len = strlen((char *)p) + 1;
                    memcpy_s(call_num, sizeof(call_num), p, call_num_len);
                    p += call_num_len;
                    LE_STREAM_TO_UINT8(call_type, p);
                    LE_STREAM_TO_UINT8(service, p);

                    bt_hfp_ag_subscriber_number_send(p, call_num, call_num_len, call_type, service);
                }
                break;

            case  BQB_ACTION_HFHS_AG_INDICATORS_STATUS_SEND:
                {
                    uint8_t service_indicator;
                    uint8_t call_indicator;
                    uint8_t call_setup_indicator;
                    uint8_t call_held_indicator;
                    uint8_t signal_indicator;
                    uint8_t roaming_indicator;
                    uint8_t batt_chg_indicator;

                    LE_STREAM_TO_UINT8(service_indicator, p);
                    LE_STREAM_TO_UINT8(call_indicator, p);
                    LE_STREAM_TO_UINT8(call_setup_indicator, p);
                    LE_STREAM_TO_UINT8(call_held_indicator, p);
                    LE_STREAM_TO_UINT8(signal_indicator, p);
                    LE_STREAM_TO_UINT8(roaming_indicator, p);
                    LE_STREAM_TO_UINT8(batt_chg_indicator, p);
                    bt_hfp_ag_indicators_send(p,
                                              (T_BT_HFP_AG_SERVICE_INDICATOR)service_indicator,
                                              (T_BT_HFP_AG_CALL_INDICATOR)call_indicator,
                                              (T_BT_HFP_AG_CALL_SETUP_INDICATOR)call_setup_indicator,
                                              (T_BT_HFP_AG_CALL_HELD_INDICATOR)call_held_indicator,
                                              signal_indicator,
                                              (T_BT_HFP_AG_ROAMING_INICATOR)roaming_indicator,
                                              batt_chg_indicator);
                    bt_hfp_ag_ok_send(p);

                }
                break;

            case  BQB_ACTION_HFHS_AG_VENDOR_CMD_SEND:
                {
                    char      vnd_cmd[50];
                    uint8_t   vnd_cmd_len;

                    vnd_cmd_len = strlen((char *)p) + 1;
                    memcpy_s(call_num, sizeof(call_num), p, call_num_len);
                    p += vnd_cmd_len;

                    bt_hfp_ag_send_vnd_at_cmd_req(p, vnd_cmd);
                }
                break;

            case BQB_ACTION_HFHS_AG_SCO_CONN_REQ:
                {
                    uint16_t max_latency;
                    uint16_t   voice_setting;
                    uint8_t    retrans_effort;
                    LE_STREAM_TO_UINT16(max_latency, p);
                    LE_STREAM_TO_UINT16(voice_setting, p);
                    LE_STREAM_TO_UINT8(retrans_effort, p);
                    gap_br_send_sco_conn_req(p, 8000, 8000, max_latency, voice_setting, retrans_effort,
                                             SCO_PKT_TYPES_HV3_EV3_2EV3);
                }
                break;

            default:
                break;
            }
        }
#endif
        else if (id == BQB_CMD_PBAP)
        {
            LE_STREAM_TO_UINT8(action, p);
            switch (action)
            {
            case BQB_ACTION_PBAP_VCARD_SRM:
            case BQB_ACTION_PBAP_VCARD_NOSRM:
                {
                    /*
                    uint8_t *bd_addr;
                    uint8_t *folder;
                    uint16_t folder_len;
                    T_PBAP_TAG_ID_ORDER_VALUE order;
                    uint8_t *search_value;
                    uint8_t value_len;
                    T_PBAP_TAG_ID_SEARCH_PROP_VALUE search_attr;
                    uint16_t max_list_count;
                    uint16_t start_offset;

                    pbap_pull_vcard_listing(bd_addr, folder, folder_len, order, search_value, value_len,
                                            search_attr, max_list_count, start_offset);
                    */
                }
                break;
            case BQB_ACTION_PBAP_VCARD_ENTRY:
                {
                    /*
                    uint8_t *bd_addr;
                    uint8_t *p_name;
                    uint8_t name_len;
                    uint8_t *filter;
                    T_PBAP_TAG_ID_FORMAT_VALUE format;

                    pbap_pull_vcard_entry(bd_addr, p_name, name_len, filter, format);
                    */
                }
                break;
            default:

                break;
            }
        }
#if (F_APP_BQB_CLI_MAP_SUPPORT == 1)
        else if (id == BQB_CMD_MAP)
        {
            LE_STREAM_TO_UINT8(action, p);
            switch (action)
            {
            case BQB_ACTION_MAP_CONNECT_MAS:
                {
                    app_bt_policy_default_connect(p, MAP_PROFILE_MASK, false);
                }
                break;

            case BQB_ACTION_MAP_DISCONNECT_MAS:
                {
                    app_bt_policy_disconnect(p, MAP_PROFILE_MASK);
                }
                break;

            case BQB_ACTION_MAP_MNS_ON:
                {
                    bt_map_mas_msg_notification_set(p, true);
                }
                break;

            case BQB_ACTION_MAP_MNS_OFF:
                {
                    bt_map_mas_msg_notification_set(p, false);
                }
                break;

            case BQB_ACTION_MAP_FOLDER_LISTING:
                {
                    bt_map_mas_folder_listing_get(p, 10, 0);
                }
                break;

            case BQB_ACTION_MAP_SET_FOLDER_INBOX:
                {
                    bt_map_mas_folder_set(p, BT_MAP_FOLDER_INBOX);
                }
                break;

            case BQB_ACTION_MAP_SET_FOLDER_OUTBOX:
                {
                    bt_map_mas_folder_set(p, BT_MAP_FOLDER_OUTBOX);
                }
                break;

            case BQB_ACTION_MAP_MSG_LISTING:
                {
                    //NULL terminated UNICODE : inbox
                    uint8_t map_path_inbox[12] =
                    {
                        0x00, 0x69, 0x00, 0x6e, 0x00, 0x62, 0x00, 0x6f, 0x00, 0x78, 0x00, 0x00
                    };

                    bt_map_mas_msg_listing_get(p, map_path_inbox, 12, 10, 0);
                }
                break;

            case BQB_ACTION_MAP_GET_MSG:
                {
                    //msg handle = "0123456789000002"
                    uint8_t handle[34] =
                    {
                        0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36,
                        0x00, 0x37, 0x00, 0x38, 0x00, 0x39, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30,
                        0x00, 0x30, 0x00, 0x32, 0x00, 0x00
                    };

                    bt_map_mas_msg_get(p, handle, 34, false);
                }
                break;

            case BQB_ACTION_MAP_UPLOAD:
            case BQB_ACTION_MAP_UPLOAD_SRM:
                {
                    //NULL terminated UNICODE : outbox
                    uint8_t map_path_outbox[14] =
                    {
                        0x00, 0x6f, 0x00, 0x75, 0x00, 0x74, 0x00, 0x62, 0x00, 0x6f, 0x00, 0x78, 0x00, 0x00
                    };
                    uint8_t msg[248] =
                    {
                        0x42, 0x45, 0x47, 0x49, 0x4E, 0x3A, 0x42, 0x4D, 0x53, 0x47, 0xD, 0xA, //BEGIN:BMSG
                        0x56, 0x45, 0x52, 0x53, 0x49, 0x4F, 0x4E, 0x3A, 0x31, 0x2E, 0x30, 0xD, 0xA, //VERSION:1.0
                        0x53, 0x54, 0x41, 0x54, 0x55, 0x53, 0x3A, 0x52, 0x45, 0x41, 0x44, 0xD, 0xA, //STATUS:READ
                        0x54, 0x59, 0x50, 0x45, 0x3A, 0x53, 0x4D, 0x53, 0x5F, 0x43, 0x44, 0x4D, 0x41, 0xD, 0xA, //TYPE:SMS_CDMA
                        0x46, 0x4F, 0x4C, 0x44, 0x45, 0x52, 0x3A, 0x74, 0x65, 0x6C, 0x65, 0x63, 0x6F, 0x6D, 0x2F,
                        0x6D, 0x73, 0x67, 0x2F, 0x4F, 0x55, 0x54, 0x42, 0x4F, 0x58, 0xD, 0xA, //FOLDER:telecom/msg/outbox
                        0x42, 0x45, 0x47, 0x49, 0x4E, 0x3A, 0x20, 0x42, 0x45, 0x4E, 0x56, 0xD, 0xA, //BEGIN:BENV
                        0x42, 0x45, 0x47, 0x49, 0x4E, 0x3A, 0x20, 0x56, 0x43, 0x41, 0x52, 0x44, 0xD, 0xA, //BEGIN:VCARD
                        0x56, 0x45, 0x52, 0x53, 0x49, 0x4F, 0x4E, 0x3A, 0x20, 0x33, 0x2E, 0x30, 0xD, 0xA, //VERSION:3.0
                        0x46, 0x4E, 0x3A, 0xD, 0xA, //FN:
                        0x4E, 0x3A, 0xD, 0xA, //N:
                        0x54, 0x45, 0x4C, 0x3A, 0x31, 0x30, 0x30, 0x38, 0x36, 0xD, 0xA, //TEL:10086
                        0x45, 0x4E, 0x44, 0x3A, 0x20, 0x56, 0x43, 0x41, 0x52, 0x44, 0xD, 0xA, //END:VCARD
                        0x42, 0x45, 0x47, 0x49, 0x4E, 0x3A, 0x42, 0x42, 0x4F, 0x44, 0x59, 0xD, 0xA, //BEGIN:BBODY
                        0x4C, 0x45, 0x4E, 0x47, 0x54, 0x48, 0x3A, 0x32, 0x35, 0xD, 0xA, //LENGTH:25
                        0x43, 0x48, 0x41, 0x52, 0x53, 0x45, 0x54, 0x3A, 0x55, 0x54, 0x46, 0x2D, 0x38, 0xD, 0xA, //CHARSET:UTF-8
                        0x42, 0x45, 0x47, 0x49, 0x4E, 0x3A, 0x4D, 0x53, 0x47, 0xD, 0xA, //BEGIN:MSG
                        0x31, 0x32, 0x33, 0xD, 0xA, //123
                        0x45, 0x4E, 0x44, 0x3A, 0x4D, 0x53, 0x47, 0xD, 0xA, //END:MSG
                        0x45, 0x4E, 0x44, 0x3A, 0x42, 0x42, 0x4F, 0x44, 0x59, 0xD, 0xA, //END:BBODY
                        0x45, 0x4E, 0x44, 0x3A, 0x42, 0x45, 0x4E, 0x56, 0xD, 0xA, //END:BENV
                        0x45, 0x4E, 0x44, 0x3A, 0x42, 0x4D, 0x53, 0x47, 0xD, 0xA  //END:BMSG
                    };

                    if (action == BQB_ACTION_MAP_UPLOAD)
                    {
                        uint16_t repeat_count = 0;
                        uint16_t i;

                        LE_STREAM_TO_UINT16(repeat_count, p);
                        for (i = 0; i < repeat_count; i++)
                        {
                            bt_map_mas_msg_push(p, NULL, 0, false, true, msg, 248);
                            os_delay(20);
                        }

                        bt_map_mas_msg_push(p, NULL, 0, false, false, msg, 248);
                    }
                    else
                    {
                        bt_map_mas_msg_push(p, map_path_outbox, 14, false, true, msg, 248);
                    }
                }
                break;

            default:
                break;
            }
        }
#endif

#endif
#if F_APP_MCP_SUPPORT
        else if (id == BQB_CMD_MCP)
        {
            uint8_t conn_id;
            uint8_t srv_idx;
            uint8_t general;
            T_MCP_CLIENT_WRITE_MEDIA_CP_PARAM param =
            {
                .opcode = 0,

                .param.move_relative_opcode_offset = 0
            };

            LE_STREAM_TO_UINT8(action, p);
            LE_STREAM_TO_UINT8(conn_id, p);
            LE_STREAM_TO_UINT8(srv_idx, p);
            LE_STREAM_TO_UINT8(general, p);

            APP_PRINT_INFO4("app_console_handle_msg: action %x, conn_id %d, srv_idx %d, general %d",
                            action, conn_id, srv_idx, general);
            switch (action)
            {
            case BQB_ACTION_MCP_PLAY:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY;
                }
                break;

            case BQB_ACTION_MCP_PAUSE:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE;
                }
                break;

            case BQB_ACTION_MCP_STOP:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_STOP;
                }
                break;

            case BQB_ACTION_MCP_FASTFORWARD:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_FORWARD;
                }
                break;

            case BQB_ACTION_MCP_FASTFORWARD_STOP:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY;
                }
                break;

            case BQB_ACTION_MCP_REWIND:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_REWIND;
                }
                break;

            case BQB_ACTION_MCP_REWIND_STOP:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY;
                }
                break;

            case BQB_ACTION_MCP_FORWARD:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_TRACK;
                }
                break;

            case BQB_ACTION_MCP_BACKWARD:
                {
                    param.opcode = MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_TRACK;
                }
                break;

            default:
                break;
            }
            mcp_client_write_media_cp(le_get_conn_handle(conn_id), srv_idx, general, &param, true);
        }
#endif
#if F_APP_CCP_SUPPORT
        else if (id == BQB_CMD_CCP)
        {
            uint8_t conn_id;
            uint8_t srv_idx;
            uint8_t call_idx;
            uint8_t is_cmd;
            uint8_t general;
            uint16_t conn_handle;
            T_CCP_CLIENT_WRITE_CALL_CP_PARAM write_call_cp_param = {0};

            LE_STREAM_TO_UINT8(action, p);
            LE_STREAM_TO_UINT8(conn_id, p);
            LE_STREAM_TO_UINT8(srv_idx, p);
            LE_STREAM_TO_UINT8(call_idx, p);
            LE_STREAM_TO_UINT8(is_cmd, p);
            LE_STREAM_TO_UINT8(general, p);

            conn_handle = le_get_conn_handle(conn_id);

            APP_PRINT_INFO6("app_console_handle_msg: action %x, conn_id %d, srv_idx %d, call_idx %d, is_cmd %d, general %d",
                            action, conn_id, srv_idx, call_idx, is_cmd, general);
            switch (action)
            {
            case BQB_ACTION_CAP_ACCEPT:
                {
                    write_call_cp_param.opcode = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ACCEPT;
                    write_call_cp_param.param.accept_opcode_call_index = call_idx;

                    ccp_client_write_call_cp(conn_handle, srv_idx, general, (!is_cmd), &write_call_cp_param);
                }
                break;

            case BQB_ACTION_CAP_TERMINATE:
                {
                    write_call_cp_param.opcode = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_TERMINATE;
                    write_call_cp_param.param.terminate_opcode_call_index = call_idx;

                    ccp_client_write_call_cp(conn_handle, srv_idx, general, (!is_cmd), &write_call_cp_param);
                }
                break;

            case BQB_ACTION_CAP_LOCAL_HOLD:
                {
                    write_call_cp_param.opcode = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_HOLD;
                    write_call_cp_param.param.local_hold_opcode_call_index = call_idx;

                    ccp_client_write_call_cp(conn_handle, srv_idx, general, (!is_cmd), &write_call_cp_param);
                }
                break;

            case BQB_ACTION_CAP_LOCAL_RETRIEVE:
                {
                    write_call_cp_param.opcode = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_LOCAL_RETRIEVE;
                    write_call_cp_param.param.local_retrieve_opcode_call_index = call_idx;

                    ccp_client_write_call_cp(conn_handle, srv_idx, general, (!is_cmd), &write_call_cp_param);
                }
                break;

            default:
                break;
            }
        }
#endif
#if F_APP_VCS_SUPPORT
        else if (id == BQB_CMD_VCS)
        {
            uint8_t volume_setting;
            uint8_t mute;
            uint8_t change_counter;
            uint8_t volume_flags;
            uint8_t step_size;

            LE_STREAM_TO_UINT8(action, p);
            LE_STREAM_TO_UINT8(volume_setting, p);
            LE_STREAM_TO_UINT8(mute, p);
            LE_STREAM_TO_UINT8(change_counter, p);
            LE_STREAM_TO_UINT8(volume_flags, p);
            LE_STREAM_TO_UINT8(step_size, p);

            APP_PRINT_INFO6("app_console_handle_msg: action %x, volume_setting %d, mute %d, change_counter %d, volume_flags %d, step_size %d",
                            action, volume_setting, mute, change_counter, volume_flags, step_size);
            switch (action)
            {
            case BQB_ACTION_VCS_UP:
                {
                    T_VCS_PARAM vcs_param;

                    vcs_param.volume_setting = volume_setting;
                    vcs_param.mute = mute;
                    vcs_param.change_counter = change_counter;
                    vcs_param.volume_flags = volume_flags;
                    vcs_param.step_size = step_size;
                    vcs_set_param(&vcs_param);
                }
                break;

            case BQB_ACTION_VCS_DOWN:
                {
                    T_VCS_PARAM vcs_param;

                    vcs_param.volume_setting = volume_setting;
                    vcs_param.mute = mute;
                    vcs_param.change_counter = change_counter;
                    vcs_param.volume_flags = volume_flags;
                    vcs_param.step_size = step_size;
                    vcs_set_param(&vcs_param);
                }
                break;

            default:
                break;
            }
        }
#endif
#if F_APP_PBP_SUPPORT
        else if (id == BQB_CMD_PBP)
        {
            LE_STREAM_TO_UINT8(action, p);

            switch (action)
            {
#if F_APP_TMAP_BMR_SUPPORT
            case BQB_ACTION_PBP_BMR_START:
                {
                    if (app_lea_bca_state() == LEA_BCA_STATE_IDLE)
                    {
                        app_lea_bca_sm(LEA_BCA_MMI, NULL);
                        app_lea_bca_sm(LEA_BCA_BIS_SYNC, NULL);
                    }
                }
                break;

            case BQB_ACTION_PBP_BMR_STOP:
                {
                    app_lea_bca_bmr_terminate();
                }
                break;
#endif
            case BQB_ACTION_PBP_BROADCAST_CODE:
                {
                    app_cfg_nv.lea_encryp = 1;
                    app_cfg_nv.lea_valid = 1;
                    STREAM_TO_ARRAY(app_cfg_nv.lea_code, p, 16);
                }
                break;

            default:
                break;
            }
        }
#endif
#if F_APP_TMAP_CT_SUPPORT
        else if (id == BQB_CMD_TMAP)
        {
            LE_STREAM_TO_UINT8(action, p);

            switch (action)
            {
            case BQB_ACTION_TMAP_ADV_START:
                {
                    app_lea_adv_start();
                }
                break;

            case BQB_ACTION_TMAP_ADV_STOP:
                {
                    app_lea_adv_stop();
                }
                break;

            case BQB_ACTION_TMAP_GAMING_MODE:
                {
                    uint8_t gaming_mode;

                    LE_STREAM_TO_UINT8(gaming_mode, p);
                    app_lea_pacs_update_low_latency(gaming_mode);
                }
                break;

            case BQB_ACTION_TMAP_ORIGINATE_CALL:
                {
                    char *p_uri = "BLUETOOTH SIG";
                    uint16_t len = strlen(p_uri);
                    T_APP_LE_LINK *p_link;

                    p_link = app_link_find_le_link_by_conn_handle(app_lea_ccp_get_active_conn_handle());
                    if (p_link != NULL)
                    {
                        T_CCP_CLIENT_WRITE_CALL_CP_PARAM write_call_cp_param = {0};

                        write_call_cp_param.opcode = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE;
                        write_call_cp_param.param.originate_opcode_param.p_uri = (uint8_t *)p_uri;
                        write_call_cp_param.param.originate_opcode_param.uri_len = len;

                        ccp_client_write_call_cp(p_link->conn_handle, 0, true, false, &write_call_cp_param);
                    }
                }
                break;

            case BQB_ACTION_TMAP_TERMINATE_CALL:
                {
                    T_CCP_CLIENT_WRITE_CALL_CP_PARAM write_call_cp_param = {0};
                    T_APP_LE_LINK *p_link;

                    p_link = app_link_find_le_link_by_conn_handle(app_lea_ccp_get_active_conn_handle());
                    if (p_link != NULL)
                    {
                        T_LEA_CALL_ENTRY *p_call_entry;

                        p_call_entry = app_lea_ccp_find_call_entry_by_idx(p_link->conn_id, p_link->active_call_index);
                        if (p_call_entry != NULL)
                        {
                            write_call_cp_param.opcode = TBS_CALL_CONTROL_POINT_CHAR_OPCODE_TERMINATE;
                            write_call_cp_param.param.terminate_opcode_call_index = p_call_entry->call_index;

                            ccp_client_write_call_cp(p_link->conn_handle, 0, p_link->gtbs, false, &write_call_cp_param);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
#endif
#if F_APP_HAS_SUPPORT
        else if (id == BQB_CMD_HAP)
        {
            LE_STREAM_TO_UINT8(action, p);

            switch (action)
            {
            case BQB_ACTION_HAP_FEATURE:
                {
                    T_HAS_HA_FEATURES hearing_aid_features;

                    LE_STREAM_TO_UINT8(*(uint8_t *)&hearing_aid_features, p);
                    has_update_ha_features(hearing_aid_features);
                }
                break;

            case BQB_ACTION_HAP_ACTIVE_INDEX:
                {
                    uint8_t index;

                    LE_STREAM_TO_UINT8(index, p);
                    has_update_active_preset_idx(index);
                }
                break;

            case BQB_ACTION_HAP_PRESET_NAME:
                {
                    uint8_t index;
                    char name[20];
                    uint8_t len;

                    LE_STREAM_TO_UINT8(index, p);
                    LE_STREAM_TO_UINT8(len, p);
                    STREAM_TO_ARRAY(name, p, len);

                    for (uint8_t i = 0; i < len; i++)
                    {
                        if (name[i] == '_')
                        {
                            name[i] = ' ';
                        }
                    }

                    app_lea_has_modify_preset_record_list(PRESET_CHANGE_NAME_CHANGE,
                                                          index, 0, len, name, true);
                }
                break;

            case BQB_ACTION_HAP_PRESET_AVAILABLE:
                {
                    uint8_t index;

                    LE_STREAM_TO_UINT8(index, p);
                    app_lea_has_modify_preset_record_list(PRESET_CHANGE_AVAILABLE,
                                                          index, 0, 1, NULL, true);
                }
                break;

            case BQB_ACTION_HAP_PRESET_UNAVAILABLE:
                {
                    uint8_t index;

                    LE_STREAM_TO_UINT8(index, p);
                    app_lea_has_modify_preset_record_list(PRESET_CHANGE_UNAVAILABLE,
                                                          index, 0, 1, NULL, true);
                }
                break;

            case BQB_ACTION_HAP_PRESET_ADD:
                {
                    uint8_t index;
                    char name[20];
                    uint8_t len;
                    uint8_t preset_prop;

                    LE_STREAM_TO_UINT8(index, p);
                    LE_STREAM_TO_UINT8(len, p);
                    STREAM_TO_ARRAY(name, p, len);
                    LE_STREAM_TO_UINT8(preset_prop, p);

                    for (uint8_t i = 0; i < len; i++)
                    {
                        if (name[i] == '_')
                        {
                            name[i] = ' ';
                        }
                    }

                    app_lea_has_modify_preset_record_list(PRESET_CHANGE_ADD,
                                                          index, preset_prop, len, name, true);
                }
                break;

            case BQB_ACTION_HAP_PRESET_DELETE:
                {
                    uint8_t index;

                    LE_STREAM_TO_UINT8(index, p);

                    if (index == 0xff)
                    {
                        for (uint16_t i = 1; i <= 0xff; i++)
                        {
                            app_lea_has_modify_preset_record_list(PRESET_CHANGE_DELETE, i, 0,
                                                                  1, NULL, true);
                        }
                    }
                    else
                    {
                        app_lea_has_modify_preset_record_list(PRESET_CHANGE_DELETE,
                                                              index, 0, 1, NULL, true);
                    }
                }
                break;

            case BQB_ACTION_HAP_RTK_ADV_START:
                {
                    app_ble_common_adv_init();
                    app_ble_common_adv_start_rws(60 * 100);
                }
                break;

            default:
                break;
            }
        }
#endif
#if ISOC_TEST_SUPPORT
        else if (id & ISOC_CMD_SET)
        {
            APP_PRINT_TRACE1("app_console_handle_msg: ISOC_CMD_SET id 0x%04x", id);
            switch (id)
            {
            case ISOC_CMD_CONFIG:
                {
                    uint8_t session_id;
                    uint8_t scenario_idx;
                    uint8_t setting_group;
                    uint8_t setting_idx;
                    LE_STREAM_TO_UINT8(session_id, p);
                    LE_STREAM_TO_UINT8(scenario_idx, p);
                    LE_STREAM_TO_UINT8(setting_group, p);
                    LE_STREAM_TO_UINT8(setting_idx, p);
                    app_isoc_cmd_config(session_id, scenario_idx, setting_group, setting_idx);
                }
                break;
            case ISOC_CMD_ENABLE:
                {
                    uint8_t session_id;
                    LE_STREAM_TO_UINT8(session_id, p);
                    app_isoc_cmd_enable(session_id);
                }
                break;
            case ISOC_CMD_DISABLE:
                {
                    uint8_t session_id;
                    LE_STREAM_TO_UINT8(session_id, p);
                    app_isoc_cmd_disable(session_id);
                }
                break;
            case ISOC_CMD_RELEASE:
                {
                    uint8_t session_id;
                    LE_STREAM_TO_UINT8(session_id, p);
                    app_isoc_cmd_release(session_id);
                }
                break;
            case ISOC_CMD_STARTADV:
                {
                    uint8_t always_adv;
                    LE_STREAM_TO_UINT8(always_adv, p);
                    app_isoc_cmd_startadv(always_adv);
                }
                break;
            case ISOC_CMD_STOPADV:
                {
                    app_isoc_cmd_stopadv();
                }
                break;
            case ISOC_CMD_CON:
                {
                    uint8_t bt_addr[6];
                    memcpy(bt_addr, p, 6);
                    app_isoc_cmd_con(bt_addr);
                }
                break;
            case ISOC_CMD_DISC:
                {
                    uint8_t conn_id;
                    LE_STREAM_TO_UINT8(conn_id, p);
                    app_isoc_cmd_disc(conn_id);
                }
                break;
            case ISOC_CMD_DUMP:
                {
                    app_isoc_cmd_dump();
                }
                break;
#if ISOC_BIS_TEST
            case ISOC_CMD_BSENABLE:
                {
                    uint8_t mode;
                    uint8_t item;
                    LE_STREAM_TO_UINT8(mode, p);
                    LE_STREAM_TO_UINT8(item, p);
                    app_isoc_cmd_bsenable(mode, (T_CODEC_CFG_ITEM)item);
                }
                break;
            case ISOC_CMD_BSDISABLE:
                {
                    app_isoc_cmd_bsdisable();
                }
                break;
            case ISOC_CMD_BSYNCENABLE:
                {
                    uint8_t mode;
                    uint8_t item;
                    uint8_t bt_addr[6];
                    LE_STREAM_TO_UINT8(mode, p);
                    LE_STREAM_TO_UINT8(item, p);
                    memcpy(bt_addr, p, 6);
                    app_isoc_cmd_bsyncenable(mode, (T_CODEC_CFG_ITEM)item,  bt_addr);
                }
                break;
            case ISOC_CMD_BSYNCDISABLE:
                {
                    app_isoc_cmd_bsyncdisable();
                }
                break;
#endif
            default:
                break;
            }
        }
#endif
#if F_APP_LE_AUDIO_CLI_TEST
        else if (id & AUDIO_CMD_SET)
        {
            APP_PRINT_TRACE1("app_console_handle_msg: AUDIO_CMD_SET id 0x%04x", id);
            switch (id)
            {
            case AUDIO_CMD_STARTESCAN:
                {
                    uint8_t bis_channel;
                    LE_STREAM_TO_UINT8(bis_channel, p);
                    app_cfg_const.iso_mode = 1;
                    app_cfg_const.subgroup = bis_channel;
                    app_lea_scan_start();
                    //app_lea_bca_state_change(LEA_BCA_STATE_SCAN); for feature change
                    app_bt_sniffing_param_update(APP_BT_SNIFFING_EVENT_ISO_SUSPEND);
                }
                break;
            case AUDIO_CMD_STOPESCAN:
                {
                    le_ext_scan_stop();
                    //app_lea_bca_state_change(LEA_BCA_STATE_IDLE);
                }
                break;
            case AUDIO_CMD_PASYNC:
                {
                    uint8_t dev_idx, i;

                    LE_STREAM_TO_UINT8(dev_idx, p);
                    app_lea_bca_bs_set_active(dev_idx);
                    if (app_lea_bca_pa_sync(dev_idx) == false)
                    {
                        APP_PRINT_ERROR1("app_audio_cmd_pasync: failed: dev_idx: 0x%x", dev_idx);
                    }
                }
                break;
            case AUDIO_CMD_BIGSYNC:
                {
                    uint8_t dev_idx;

                    LE_STREAM_TO_UINT8(dev_idx, p);

                    if (app_lea_bca_bs_big_establish_dev_idex(dev_idx) == false)
                    {
                        APP_PRINT_ERROR0("app_audio_cmd_bigsync: failed");
                    }
                }
                break;

            case AUDIO_CMD_PATERMINATE:
                {
                    uint8_t dev_idx;

                    LE_STREAM_TO_UINT8(dev_idx, p);

                    if (app_lea_bca_bs_pa_terminate(dev_idx) == false)
                    {
                        APP_PRINT_ERROR0("app_audio_cmd_paterminate: failed");
                    }
                }
                break;

            case AUDIO_CMD_BIGTERMINATE:
                {
                    uint8_t dev_idx;

                    LE_STREAM_TO_UINT8(dev_idx, p);

                    if (app_lea_bca_big_terminate(dev_idx) == false)
                    {
                        APP_PRINT_ERROR0("app_audio_cmd_bigterminate: failed");
                    }
                }
                break;

            case AUDIO_CMD_BISOTERMINATE:
                {
                    //  uint8_t source_id;

                    //LE_STREAM_TO_UINT8(source_id, p);

                    //if (ble_audio_big_terminate(app_lea_bca_get_sd_sync_handle(source_id)) == false) //for feature change
                    // {
                    //      APP_PRINT_ERROR0("bass_iso_terminate: failed");
                    //  }
                }
                break;

            case AUDIO_CMD_BPATERMINATE:
                {
                    uint8_t source_id;

                    LE_STREAM_TO_UINT8(source_id, p);

                    if (app_lea_bca_sink_pa_terminate(source_id) == false)
                    {
                        APP_PRINT_ERROR0("app_bis_mgr_sink_pa_terminate: failed");
                    }
                }
                break;

            case AUDIO_CMD_A2DP_PAUSE:
                {
                    mtc_stream_switch(false);
                }
                break;

            case AUDIO_CMD_BIGSPATH:
                {
                    uint16_t bis_conn_handle;
                    uint8_t data_path_direction = DATA_PATH_ADD_OUTPUT;
                    uint8_t data_path_id = 0;
                    uint8_t codec_id[5] = {1, 0, 0, 0, 0};

                    LE_STREAM_TO_UINT16(bis_conn_handle, p);

                    if (gap_big_mgr_setup_data_path(bis_conn_handle, data_path_direction, data_path_id, codec_id,
                                                    0x1122,
                                                    0, NULL) != GAP_CAUSE_SUCCESS)
                    {
                        APP_PRINT_ERROR0("app_audio_cmd_bigspath: failed");
                    }
                }
                break;

            case AUDIO_CMD_BIGRPATH:
                {
                    uint16_t bis_conn_handle;
                    uint8_t data_path_direction = DATA_PATH_OUTPUT_FLAG;

                    LE_STREAM_TO_UINT16(bis_conn_handle, p);

                    if (gap_big_mgr_remove_data_path(bis_conn_handle, data_path_direction) != GAP_CAUSE_SUCCESS)
                    {
                        APP_PRINT_ERROR0("app_audio_cmd_bigrpath: failed");
                    }
                }
                break;

            case AUDIO_CMD_CIS_ADV:
                {
                    app_cfg_const.iso_mode = 1;
                    app_cfg_const.timer_pairing_timeout = 1200;
                    app_lea_adv_start();
                }
                break;

            default:
                break;
            }
        }
#endif
        free(console_msg.u.buf);
        break;

    case  IO_MSG_CONSOLE_BINARY_RX:
        {
            p += 1;
            LE_STREAM_TO_UINT8(rx_seqn, p);
            LE_STREAM_TO_UINT16(cmd_len, p);

            app_cmd_handler(p, cmd_len, CMD_PATH_UART, rx_seqn, 0);
        }
        free(console_msg.u.buf);
        break;

    default:
        break;
    }
}

#endif
