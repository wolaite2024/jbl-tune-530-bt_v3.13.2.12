/**
*****************************************************************************************
*     Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    app_gfps_psm.c
  * @brief   To support Message stream for BLE devices,
  *          Fast Pair will build and maintain a BLE L2CAP channel for sending and receiving messages.
  * @details
  * @author  danni
  * @date    2021-04-06
  * @version v1.0
  * *************************************************************************************
*/

/*============================================================================*
 *                         Header files
 *============================================================================*/
#include "gfps.h"
#include "trace.h"
#include "string.h"
#include "stdlib.h"
#include "app_main.h"
#include "bt_gfps.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_gfps_psm.h"
#include "app_gfps_msg.h"
#include "app_link_util.h"
#include "app_gfps_battery.h"
#include "gap_credit_based_conn.h"
#include "app_timer.h"

#if GFPS_LE_DEVICE_SUPPORT
#include "gap_ecfc.h"
/*============================================================================*
 *                         Variables
 *============================================================================*/
/*use GAP_ECFC_DATA_PATH_GATT to reduce service discovery time*/
static uint8_t  gfps_eatt_le_proto_id = 0xff;
static uint16_t gfps_eatt_local_mtu   = 247;
//static uint16_t gfps_eatt_conn_handle = 0;

/*use GAP_ECFC_DATA_PATH_APP to handle gfps message*/
static uint8_t  gfps_msg_le_proto_id = 0xff;
static uint16_t gfps_msg_local_mtu   = 247;
//static uint16_t gfps_msg_conn_handle = 0;

/*============================================================================*
 *                         Functions
 *============================================================================*/

static uint16_t app_gfps_psm_msg_ecfc_callback(void *p_buf, T_GAP_ECFC_MSG msg)
{
    uint16_t result = 0;
    APP_PRINT_TRACE1("app_gfps_psm_msg_ecfc_callback: msg %d", msg);

    switch (msg)
    {
    case GAP_ECFC_DATA_IND:
        {
            T_GAP_ECFC_DATA_IND *p_ind = (T_GAP_ECFC_DATA_IND *)p_buf;
            uint8_t *p_data = p_ind->data + p_ind->gap;
        }
        break;

    case GAP_ECFC_PROTO_REG_RSP:
        {
            T_GAP_ECFC_PROTO_REG_RSP *p_rsp = (T_GAP_ECFC_PROTO_REG_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_CONN_IND:
        {
            T_GAP_ECFC_CONN_IND *p_ind = (T_GAP_ECFC_CONN_IND *)p_buf;
            T_GAP_ECFC_CONN_CFM_CAUSE cause = GAP_ECFC_CONN_ACCEPT;

            if (p_ind->remote_mtu > 512)
            {
                cause = GAP_ECFC_CONN_UNACCEPTABLE_PARAMS;
            }

            gap_ecfc_send_conn_cfm(p_ind->conn_handle, p_ind->identity_id,
                                   cause, p_ind->cid, p_ind->cid_num, gfps_msg_local_mtu);
        }
        break;

    case GAP_ECFC_CONN_RSP:
        {
            T_GAP_ECFC_CONN_RSP *p_rsp = (T_GAP_ECFC_CONN_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_CONN_CMPL:
        {
            T_GAP_ECFC_CONN_CMPL_INFO *p_info = (T_GAP_ECFC_CONN_CMPL_INFO *)p_buf;
//            gfps_msg_conn_handle = p_info->conn_handle;

            for (uint8_t i = 0; i < p_info->cid_num; i++)
            {
                APP_PRINT_INFO2("app_gfps_psm_msg_ecfc_callback: GAP_ECFC_CONN_CMPL %d, cid 0x%x",
                                i, p_info->cid[i]);
            }
        }
        break;

    case GAP_ECFC_DISCONN_IND:
        {
            T_GAP_ECFC_DISCONN_IND *p_ind = (T_GAP_ECFC_DISCONN_IND *)p_buf;
        }
        break;

    case GAP_ECFC_DISCONN_RSP:
        {
            T_GAP_ECFC_DISCONN_RSP *p_rsp = (T_GAP_ECFC_DISCONN_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_SEC_REG_RSP:
        {
            T_GAP_ECFC_SEC_REG_RSP *p_rsp = (T_GAP_ECFC_SEC_REG_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_RECONFIGURE_IND:
        {
            T_GAP_ECFC_RECONFIGURE_IND *p_ind = (T_GAP_ECFC_RECONFIGURE_IND *)p_buf;
            result = GAP_ECFC_RCFG_ACCEPT;
        }
        break;

    case GAP_ECFC_RECONFIGURE_RSP:
        {
            T_GAP_ECFC_RECONFIGURE_RSP *p_rsp = (T_GAP_ECFC_RECONFIGURE_RSP *)p_buf;
        }
        break;

    default:
        break;
    }

    return result;
}

static uint16_t app_gfps_psm_eatt_callback(void *p_buf, T_GAP_ECFC_MSG msg)
{
    uint16_t result = 0;
    APP_PRINT_TRACE1("app_gfps_psm_eatt_callback: msg %d", msg);

    switch (msg)
    {
    case GAP_ECFC_DATA_IND:
        {
            T_GAP_ECFC_DATA_IND *p_ind = (T_GAP_ECFC_DATA_IND *)p_buf;
            uint8_t *p_data = p_ind->data + p_ind->gap;
        }
        break;

    case GAP_ECFC_PROTO_REG_RSP:
        {
            T_GAP_ECFC_PROTO_REG_RSP *p_rsp = (T_GAP_ECFC_PROTO_REG_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_CONN_IND:
        {
            T_GAP_ECFC_CONN_IND *p_ind = (T_GAP_ECFC_CONN_IND *)p_buf;
            T_GAP_ECFC_CONN_CFM_CAUSE cause = GAP_ECFC_CONN_ACCEPT;

            if (p_ind->remote_mtu > 512)
            {
                cause = GAP_ECFC_CONN_UNACCEPTABLE_PARAMS;
            }

            gap_ecfc_send_conn_cfm(p_ind->conn_handle, p_ind->identity_id,
                                   cause, p_ind->cid, p_ind->cid_num, gfps_eatt_local_mtu);
        }
        break;

    case GAP_ECFC_CONN_RSP:
        {
            T_GAP_ECFC_CONN_RSP *p_rsp = (T_GAP_ECFC_CONN_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_CONN_CMPL:
        {
            T_GAP_ECFC_CONN_CMPL_INFO *p_info = (T_GAP_ECFC_CONN_CMPL_INFO *)p_buf;
//            gfps_eatt_conn_handle = p_info->conn_handle;

            for (uint8_t i = 0; i < p_info->cid_num; i++)
            {
                APP_PRINT_INFO2("app_gfps_psm_eatt_callback: GAP_ECFC_CONN_CMPL %d, cid 0x%x", i, p_info->cid[i]);
            }
        }
        break;

    case GAP_ECFC_DISCONN_IND:
        {
            T_GAP_ECFC_DISCONN_IND *p_ind = (T_GAP_ECFC_DISCONN_IND *)p_buf;
        }
        break;

    case GAP_ECFC_DISCONN_RSP:
        {
            T_GAP_ECFC_DISCONN_RSP *p_rsp = (T_GAP_ECFC_DISCONN_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_SEC_REG_RSP:
        {
            T_GAP_ECFC_SEC_REG_RSP *p_rsp = (T_GAP_ECFC_SEC_REG_RSP *)p_buf;
        }
        break;

    case GAP_ECFC_RECONFIGURE_IND:
        {
            T_GAP_ECFC_RECONFIGURE_IND *p_ind = (T_GAP_ECFC_RECONFIGURE_IND *)p_buf;

            result = GAP_ECFC_RCFG_ACCEPT;
        }
        break;

    case GAP_ECFC_RECONFIGURE_RSP:
        {
            T_GAP_ECFC_RECONFIGURE_RSP *p_rsp = (T_GAP_ECFC_RECONFIGURE_RSP *)p_buf;
        }
        break;

    default:
        break;
    }

    return result;
}

//L2CAP credit based flow control mode
T_APP_RESULT app_gfps_psm_msg_coc_conn_callback(uint8_t coc_type, void *p_coc_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_COC_DATA cb_data;
    memcpy(&cb_data, p_coc_data, sizeof(T_LE_COC_DATA));
    APP_PRINT_TRACE2("app_gfps_psm_msg_coc_conn_callback: coc_type = %d, bud_role %d", coc_type,
                     app_cfg_nv.bud_role);
#if F_APP_ERWS_SUPPORT
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return APP_RESULT_REJECT;
    }
#endif
    switch (coc_type)
    {
    case GAP_COC_MSG_LE_CHANN_STATE:
        {
            T_APP_LE_LINK *p_link;
            p_link = app_link_find_le_link_by_conn_id(cb_data.p_le_chann_state->conn_id);

            if (p_link == NULL)
            {
                return APP_RESULT_APP_ERR;
            }

            if (cb_data.p_le_chann_state->conn_state == GAP_CHANN_STATE_CONNECTED)
            {
                uint16_t mtu;
                le_coc_get_chann_param(COC_CHANN_PARAM_MTU, &mtu, cb_data.p_le_chann_state->cid);

                if (p_link->gfps_link.gfps_conn_id == cb_data.p_le_chann_state->conn_id)
                {
                    p_link->gfps_link.gfps_msg_cid = cb_data.p_le_chann_state->cid;
                }

                uint8_t ble_addr[6];
                bt_gfps_send_model_id(cb_data.p_le_chann_state->conn_id,
                                      cb_data.p_le_chann_state->cid,
                                      NULL, 0, extend_app_cfg_const.gfps_model_id,
                                      GFPS_MODEL_ID_LEN);

                app_gfps_get_random_addr(ble_addr);
                app_gfps_msg_reverse_data(ble_addr, 6);
                bt_gfps_send_ble_addr(cb_data.p_le_chann_state->conn_id,
                                      cb_data.p_le_chann_state->cid,
                                      NULL, 0, ble_addr);

                //check sys patch, upperstack_cfg.le_tx_sdu_long_buf_num shall >= 3;
                bt_gfps_send_firmware_revision(cb_data.p_le_chann_state->conn_id,
                                               cb_data.p_le_chann_state->cid,
                                               NULL, 0,
                                               extend_app_cfg_const.gfps_version,
                                               sizeof(extend_app_cfg_const.gfps_version));

                app_gfps_battery_info_report(GFPS_BATTERY_REPORT_LE_L2CAP);

#if GFPS_SASS_LEA_SUPPORT

                uint16_t max_credit;
                le_coc_get_chann_param(COC_CHANN_PARAM_MAX_CREDITS, &max_credit, cb_data.p_le_chann_state->cid);
                APP_PRINT_TRACE2("app_gfps_psm_msg_coc_conn_callback11: connection success, cid: 0x%x, max_credit %d",
                                 cb_data.p_le_chann_state->cid, max_credit);
                app_gfps_msg_report_session_nonce(cb_data.p_le_chann_state->conn_id, cb_data.p_le_chann_state->cid,
                                                  NULL, 0);
                app_gfps_notify_conn_status();
#endif
                APP_PRINT_TRACE2("app_gfps_psm_msg_coc_conn_callback: connection success, cid: 0x%x, mtu %d \r\n",
                                 cb_data.p_le_chann_state->cid, mtu);
            }
            else if (cb_data.p_le_chann_state->conn_state == GAP_CHANN_STATE_DISCONNECTED)
            {
#if F_APP_ERWS_SUPPORT
                /*stop left and right ear Find Me tone when b2s disconnect*/
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    uint8_t event =  GFPS_ALL_STOP;
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS_MSG,
                                                        GFPS_REMOTE_MSG_FINDME_STOP,
                                                        &event, 1);
                }
                else
#endif
                {
                    app_gfps_msg_ring_stop();
                }

                T_GFPS_MSG_RING_STATE ring_state = GFPS_MSG_RING_STOP;
                app_gfps_msg_set_ring_state(ring_state);

                if (p_link->gfps_link.p_gfps_cmd != NULL)
                {
                    free(p_link->gfps_link.p_gfps_cmd);
                    p_link->gfps_link.p_gfps_cmd = NULL;
                }

                APP_PRINT_TRACE0("app_gfps_psm_msg_coc_conn_callback: disconnect \r\n");
            }
        }
        break;

    case GAP_COC_MSG_LE_REG_PSM:
        break;

    case GAP_COC_MSG_LE_SET_PSM_SECURITY:
        break;

    case GAP_COC_MSG_LE_SEND_DATA:
        APP_PRINT_INFO4("app_gfps_psm_msg_coc_conn_callback: GAP_COC_MSG_LE_SEND_DATA conn_id %d, cid 0x%x, cause 0x%x, credit %d",
                        cb_data.p_le_send_data->conn_id,
                        cb_data.p_le_send_data->cid,
                        cb_data.p_le_send_data->cause,
                        cb_data.p_le_send_data->credit);
        break;

    case GAP_COC_MSG_LE_RECEIVE_DATA:
        app_gfps_msg_le_loop_check_data_complete(cb_data.p_le_receive_data->conn_id,
                                                 cb_data.p_le_receive_data->cid,
                                                 cb_data.p_le_receive_data->p_data,
                                                 cb_data.p_le_receive_data->value_len);
        break;

    default:
        {
        }
        break;
    }
    return result;
}

void app_gfps_psm_msg_coc_init(void)
{
    le_coc_init(2);
    le_coc_register_app_cb(app_gfps_psm_msg_coc_conn_callback);
    le_coc_reg_psm(GFPS_PSM_MSG_STREAM, 1);
    uint8_t credit_threshold = 10;
    le_coc_set_param(COC_PARAM_CREDITS_THRESHOLD, 1, &credit_threshold);

    T_GFPS_PSM_RSP gfps_psm_rsp;
    gfps_psm_rsp.state = GFPS_PSM_STATE_READY;
    gfps_psm_rsp.psm = GFPS_PSM_MSG_STREAM;
    gfps_psm_rsp_data_set(&gfps_psm_rsp);
}

void app_gfps_psm_init(void)
{
    uint8_t initial_credits = 10;
    gap_ecfc_set_param(GAP_ECFC_PARAM_INIT_CREDITS, sizeof(uint16_t), &initial_credits);

#if GFPS_LE_MSG_USE_ECFC
    gap_ecfc_init(2);
#else
    gap_ecfc_init(1);
#endif

//    gap_ecfc_reg_proto(PSM_EATT, app_gfps_psm_eatt_callback, true,
//                       &gfps_eatt_le_proto_id, GAP_ECFC_DATA_PATH_GATT);

#if GFPS_LE_MSG_USE_ECFC
    gap_ecfc_reg_proto(GFPS_PSM_MSG_STREAM, app_gfps_psm_msg_ecfc_callback, true,
                       &gfps_msg_le_proto_id, GAP_ECFC_DATA_PATH_APP);
#else
    app_gfps_psm_msg_coc_init();
#endif

    APP_PRINT_INFO0("app_gfps_psm_init");
}
#endif

