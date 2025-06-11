/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_HFP_AG_SUPPORT

#include <string.h>
#include <stdio.h>
#include "btm.h"
#include "trace.h"
#include "app_cfg.h"
#include "app_link_util.h"
#include "app_hfp_ag.h"
#include "app_sdp.h"

static T_BT_HFP_AG_CALL_STATUS call_status;
static uint8_t active_hfp_ag_index = 0;

static void app_hfp_ag_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_HFP_AG_CONN_IND:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_ag_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_hfp_ag_bt_cback: no acl link found");
                return;
            }

            bt_hfp_ag_connect_cfm(p_link->bd_addr, true);
        }
        break;

    case BT_EVENT_HFP_AG_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_ag_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                p_link->call_id_type_check = true;
                p_link->call_id_type_num = false;
                if (param->hfp_ag_conn_cmpl.is_hfp)
                {
                    p_link->connected_profile |= HFP_PROFILE_MASK;
                }
                else
                {
                    p_link->connected_profile |= HSP_PROFILE_MASK;
                }
                active_hfp_ag_index = p_link->id;
            }
        }
        break;

    case BT_EVENT_HFP_AG_DISCONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_ag_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                if (p_link->connected_profile & HFP_PROFILE_MASK)
                {
                    p_link->connected_profile &= ~HFP_PROFILE_MASK;
                }
                else
                {
                    p_link->connected_profile &= ~HSP_PROFILE_MASK;
                }
            }
        }
        break;

    case BT_EVENT_HFP_AG_MIC_VOLUME_CHANGED:
        {
            //update HF mic volume.
        }
        break;

    case BT_EVENT_HFP_AG_SPK_VOLUME_CHANGED:
        {
            //update HF speaker volume.
        }
        break;

    case BT_EVENT_HFP_AG_CALL_STATUS_CHANGED:
        {
            //update AG call_status.
            call_status = (T_BT_HFP_AG_CALL_STATUS)param->hfp_ag_call_status_changed.curr_status;

            //notify app to start or stop audio stream.
            if (param->hfp_ag_call_status_changed.curr_status == BT_HFP_AG_CALL_ACTIVE ||
                param->hfp_ag_call_status_changed.curr_status == BT_HFP_AG_CALL_INCOMING ||
                param->hfp_ag_call_status_changed.curr_status == BT_HFP_AG_CALL_OUTGOING)
            {
                //notify app to start audio stream.
                //app should check codec type with BT_EVENT_HFP_AG_AG_HF_ACTIVE_CODEC_TYPE.
            }
            else if (param->hfp_ag_call_status_changed.curr_status == BT_HFP_AG_CALL_IDLE)
            {
                //notify app to stop audio stream.
            }
        }
        break;

    case BT_EVENT_HFP_AG_INDICATORS_STATUS_REQ:
        {
#if 0
            //Td App provide current network status.
            T_BT_HFP_AG_SERVICE_INDICATOR service_indicator = BT_HFP_AG_SERVICE_STATUS_AVAILABLE;
            T_BT_HFP_AG_CALL_INDICATOR call_indicator = BT_HFP_AG_NO_CALL_IN_PROGRESS;
            T_BT_HFP_AG_CALL_SETUP_INDICATOR call_setup_indicator = BT_HFP_AG_CALL_SETUP_STATUS_IDLE;
            T_BT_HFP_AG_CALL_HELD_INDICATOR call_held_indicator = BT_HFP_AG_CALL_HELD_STATUS_IDLE;

            //Td App provide current signal status.
            uint8_t signal_indicator = 5;
            //Td App provide current roaming status.
            T_BT_HFP_AG_ROAMING_INICATOR roaming_indicator = BT_HFP_AG_ROAMING_STATUS_ACTIVE;
            //Td App provide current battery status.
            uint8_t batt_chg_indicator = 5;
            bool bt_hfp_ag_indicators_send(uint8_t                          *bd_addr,
                                           T_BT_HFP_AG_SERVICE_INDICATOR     service_indicator,
                                           T_BT_HFP_AG_CALL_INDICATOR        call_indicator,
                                           T_BT_HFP_AG_CALL_SETUP_INDICATOR  call_setup_indicator,
                                           T_BT_HFP_AG_CALL_HELD_INDICATOR   call_held_indicator,
                                           uint8_t                           signal_indicator,
                                           T_BT_HFP_AG_ROAMING_INICATOR      roaming_indicator,
                                           uint8_t                           batt_chg_indicator);
            bt_hfp_ag_ok_send(param->hfp_ag_indicators_status_req.bd_addr);
#endif
        }
        break;

    case BT_EVENT_HFP_AG_BATTERY_LEVEL:
        {
            //update HF battery level.
        }
        break;

    case BT_EVENT_HFP_AG_HF_CODEC_TYPE_SELECTED:
        //update app hfp codec type.
        break;

    case BT_EVENT_HFP_AG_SUPPORTED_FEATURES:
        {
        }
        break;

    case BT_EVENT_HFP_AG_INBAND_RINGING_REQ:
        {
        }
        break;

    case  BT_EVENT_HFP_AG_CALL_ANSWER_REQ:
        {
            bt_hfp_ag_call_answer(param->hfp_ag_call_answer_req.bd_addr);
            bt_hfp_ag_audio_connect_req(param->hfp_ag_call_answer_req.bd_addr);
        }
        break;

    case  BT_EVENT_HFP_AG_CALL_TERMINATE_REQ:
        {
            if ((call_status == BT_HFP_AG_CALL_INCOMING) ||
                (call_status == BT_HFP_AG_CALL_OUTGOING) ||
                (call_status == BT_HFP_AG_CALL_ACTIVE))
            {
                bt_hfp_ag_call_terminate(param->hfp_ag_call_terminate_req.bd_addr);
                bt_hfp_ag_audio_disconnect_req(param->hfp_ag_call_terminate_req.bd_addr);
            }

            if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_WAITING)
            {
                bt_hfp_ag_call_indicator_send(param->hfp_ag_call_terminate_req.bd_addr,
                                              BT_HFP_AG_NO_CALL_IN_PROGRESS);
            }

            if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD)
            {
                bt_hfp_ag_call_held_indicator_send(param->hfp_ag_call_terminate_req.bd_addr,
                                                   BT_HFP_AG_CALL_HELD_STATUS_HOLD_NO_ACTIVE_CALL);
            }
        }
        break;

    case  BT_EVENT_HFP_AG_CURR_CALLS_LIST_QUERY:
        {
            //send list of current calls.
#if 0
            bt_hfp_ag_current_calls_list_send(uint8_t                         *bd_addr,
                                              uint8_t                          call_idx,
                                              uint8_t                          call_dir,
                                              T_BT_HFP_AG_CURRENT_CALL_STATUS  call_status,
                                              T_BT_HFP_AG_CURRENT_CALL_MODE    call_mode,
                                              uint8_t                          mpty,
                                              const char                      *call_num,
                                              uint8_t                          call_type);
            bt_hfp_ok_send(param->hfp_ag_curr_calls_list_query.bd_addr);
#endif
        }
        break;

    case  BT_EVENT_HFP_AG_DTMF_CODE:
        {
        }
        break;

    case  BT_EVENT_HFP_AG_DIAL_WITH_NUMBER:
        {
            bt_hfp_ag_call_dial(param->hfp_ag_dial_with_number.bd_addr);
        }
        break;

    case  BT_EVENT_HFP_AG_DIAL_LAST_NUMBER:
        {
#if 0
            //dial last number when index of memory is valid,
            //otherwise, send error with BT_HFP_AG_ERR_INVALID_INDEX.

            if (invalid_mem_index)
            {
                bt_hfp_ag_ok_send(param->hfp_ag_dial_last_number.bd_addr);
                bt_hfp_ag_call_dial(param->hfp_ag_dial_last_number.bd_addr);
            }
            else
            {
                bt_hfp_ag_error_send(param->hfp_ag_dial_last_number.bd_addr, BT_HFP_AG_ERR_INVALID_INDEX);
            }
#endif
        }
        break;

    case  BT_EVENT_HFP_AG_3WAY_HELD_CALL_RELEASED:
        {
            //handle with AT cmd "AT+CHLD=0".
            //releases all held calls or sets User Determined User Busy (UDUB) for a waiting call.
            bt_hfp_ag_ok_send(param->hfp_ag_3way_held_call_released.bd_addr);
            if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_WAITING)
            {
                //sets User Determined User Busy (UDUB) for a waiting call.
                bt_hfp_ag_call_setup_indicator_send(param->hfp_ag_3way_held_call_released.bd_addr,
                                                    BT_HFP_AG_CALL_SETUP_STATUS_IDLE);
            }
            else if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD)
            {
                //releases all held calls
            }
            bt_hfp_ag_call_held_indicator_send(param->hfp_ag_3way_held_call_released.bd_addr,
                                               BT_HFP_AG_CALL_HELD_STATUS_IDLE);
        }
        break;

    case  BT_EVENT_HFP_AG_3WAY_ACTIVE_CALL_RELEASED:
        {
            //handle with AT cmd "AT+CHLD=1".
            //releases all active calls (if any exist) and accepts the other (held or waiting) call.
            if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_WAITING)
            {
                if (true)
                {
                    //if function supported.
                    bt_hfp_ag_ok_send(param->hfp_ag_3way_active_call_released.bd_addr);
                    bt_hfp_ag_call_setup_indicator_send(param->hfp_ag_3way_active_call_released.bd_addr,
                                                        BT_HFP_AG_CALL_SETUP_STATUS_IDLE);
                }
                else
                {
                    bt_hfp_ag_error_send(param->hfp_ag_3way_active_call_released.bd_addr,
                                         BT_HFP_AG_ERR_NO_NETWORK_SERVICE);
                }
            }
            else if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD)
            {
                bt_hfp_ag_ok_send(param->hfp_ag_3way_active_call_released.bd_addr);
                bt_hfp_ag_call_held_indicator_send(param->hfp_ag_3way_active_call_released.bd_addr,
                                                   BT_HFP_AG_CALL_HELD_STATUS_IDLE);

            }
        }
        break;

    case  BT_EVENT_HFP_AG_3WAY_SWITCHED:
        {
            //handle with AT cmd "AT+CHLD=2".
            //places all active calls (if any exist) on hold and accepts the other (held or waiting) call.
            if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_WAITING)
            {
                if (true)
                {
                    //if function supported.
                    bt_hfp_ag_ok_send(param->hfp_ag_3way_switched.bd_addr);
                    bt_hfp_ag_call_setup_indicator_send(param->hfp_ag_3way_switched.bd_addr,
                                                        BT_HFP_AG_CALL_SETUP_STATUS_IDLE);
                    bt_hfp_ag_call_held_indicator_send(param->hfp_ag_3way_switched.bd_addr,
                                                       BT_HFP_AG_CALL_HELD_STATUS_HOLD_AND_ACTIVE_CALL);
                }
                else
                {
                    bt_hfp_ag_error_send(param->hfp_ag_3way_switched.bd_addr,
                                         BT_HFP_AG_ERR_NO_NETWORK_SERVICE);
                }
            }
            else if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD)
            {
                bt_hfp_ag_ok_send(param->hfp_ag_3way_switched.bd_addr);
                bt_hfp_ag_call_held_indicator_send(param->hfp_ag_3way_switched.bd_addr,
                                                   BT_HFP_AG_CALL_HELD_STATUS_HOLD_AND_ACTIVE_CALL);

            }
        }
        break;

    case  BT_EVENT_HFP_AG_3WAY_MERGED:
        {
            //handle with AT cmd "AT+CHLD=3".
            //adds a held call to the conversation.
            if (call_status == BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD)
            {
                if (true)
                {
                    //if function supported.
                    bt_hfp_ag_ok_send(param->hfp_ag_3way_merged.bd_addr);
                    bt_hfp_ag_call_held_indicator_send(param->hfp_ag_3way_merged.bd_addr,
                                                       BT_HFP_AG_CALL_HELD_STATUS_IDLE);
                }
                else
                {
                    bt_hfp_ag_error_send(param->hfp_ag_3way_merged.bd_addr, BT_HFP_AG_ERR_NO_NETWORK_SERVICE);
                }
            }
        }
        break;
    case BT_EVENT_HFP_AG_UNKNOWN_AT_CMD:
        {
            bt_hfp_ag_error_send(param->hfp_ag_unknown_at_cmd.bd_addr, BT_HFP_AG_ERR_INVALID_CHAR_IN_TSTR);
        }
        break;

    case   BT_EVENT_HFP_AG_SUBSCRIBER_NUMBER_QUERY:
        {
        }
        break;

    case   BT_EVENT_HFP_AG_NETWORK_NAME_FORMAT_SET:
        {
        }
        break;

    case  BT_EVENT_HFP_AG_CURR_OPERATOR_QUERY:
        {
        }
        break;

    case  BT_EVENT_HFP_AG_ENHANCED_SAFETY_STATUS:
        {
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
        APP_PRINT_INFO1("app_hfp_ag_bt_cback: event_type 0x%04x", event_type);
    }
}

T_BT_HFP_AG_CALL_STATUS app_hfp_ag_get_call_status(void)
{
    return call_status;
}

uint8_t app_hfp_ag_get_active_hfp_ag_index(void)
{
    return active_hfp_ag_index;
}

bool app_hfp_ag_set_active_hfp_ag_index(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;

    p_link = app_link_find_br_link(bd_addr);
    if (p_link)
    {
        active_hfp_ag_index = p_link->id;
    }
    return false;
}

void app_hfp_ag_init(void)
{
    if (app_cfg_const.supported_profile_mask & HFP_PROFILE_MASK)
    {
        bt_hfp_ag_init(1, RFC_HFP_AG_CHANN_NUM, RFC_HSP_AG_CHANN_NUM, 0xf69);
        bt_mgr_cback_register(app_hfp_ag_bt_cback);
    }
}
#endif
