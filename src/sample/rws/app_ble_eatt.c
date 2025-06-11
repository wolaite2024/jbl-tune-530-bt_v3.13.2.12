#if F_APP_EATT_SUPPORT
#include "trace.h"
#include "bt_gatt_client.h"
#include "gap_ecfc.h"
#include "app_link_util.h"


#define MAX_REMOTE_MTU 512
#define ECFC_LOCAL_MTU 247
#define ECFC_CREDITS   10
#define EATT_DEBUG_LOG 1


static uint8_t app_ecfc_le_proto_id = 0xFF;

static T_APP_RESULT app_ble_eatt_client_discov_cb(uint16_t conn_handle, T_GATT_CLIENT_EVENT type,
                                                  void *p_data)
{
    if (type == GATT_CLIENT_EVENT_DIS_ALL_STATE)
    {
        T_GATT_CLIENT_DIS_ALL_DONE *p_disc = (T_GATT_CLIENT_DIS_ALL_DONE *)p_data;
        APP_PRINT_INFO2("app_ble_eatt_client_discov_cb:is_success %d, load_from_ftl %d",
                        p_disc->state, p_disc->load_from_ftl);
    }
    else if (type == GATT_CLIENT_EVENT_GATT_SERVICE_INFO)
    {
#if EATT_DEBUG_LOG
        T_GATT_CLIENT_GATT_SERVICE_INFO *p_gatt_svc = (T_GATT_CLIENT_GATT_SERVICE_INFO *)p_data;
        APP_PRINT_INFO2("app_ble_eatt_client_discov_cb: GATT_CLIENT_EVENT_GATT_SERVICE_INFO, is_found %d, load_from_ftl %d",
                        p_gatt_svc->is_found, p_gatt_svc->load_from_ftl);
        if (p_gatt_svc->p_gatt_data)
        {
            APP_PRINT_INFO7("app_ble_eatt_client_discov_cb: char_exist 0x%x, data_exist 0x%x, database_hash %b, feature[%d] 0x%x,  server feature[%d] 0x%x",
                            p_gatt_svc->p_gatt_data->char_exist,
                            p_gatt_svc->p_gatt_data->data_exist,
                            TRACE_BINARY(GATT_SVC_DATABASE_HASH_LEN, p_gatt_svc->p_gatt_data->database_hash),
                            p_gatt_svc->p_gatt_data->client_supported_features_len,
                            p_gatt_svc->p_gatt_data->client_supported_features[0],
                            p_gatt_svc->p_gatt_data->server_supported_features_len,
                            p_gatt_svc->p_gatt_data->server_supported_features[0]);
        }
#endif
    }
    return APP_RESULT_SUCCESS;
}


static uint16_t app_ble_eatt_ecfc_cb(void *p_buf, T_GAP_ECFC_MSG msg)
{
    uint16_t result = 0;

    APP_PRINT_TRACE1("app_ble_eatt_ecfc_cb: msg %d", msg);
    switch (msg)
    {
    case GAP_ECFC_PROTO_REG_RSP:
        {
#if EATT_DEBUG_LOG
            T_GAP_ECFC_PROTO_REG_RSP *p_rsp = (T_GAP_ECFC_PROTO_REG_RSP *)p_buf;
            APP_PRINT_INFO3("GAP_ECFC_PROTO_REG_RSP: proto_id %d, psm 0x%x, cause 0x%x",
                            p_rsp->proto_id,
                            p_rsp->psm,
                            p_rsp->cause);
#endif
        }
        break;

    case GAP_ECFC_CONN_IND:
        {
            T_GAP_ECFC_CONN_IND *p_ind = (T_GAP_ECFC_CONN_IND *)p_buf;
            T_GAP_ECFC_CONN_CFM_CAUSE cause = GAP_ECFC_CONN_ACCEPT;

            APP_PRINT_INFO8("GAP_ECFC_CONN_IND: proto_id %d, conn_handle 0x%x, remote_mtu %d, cid_num %d, cis %b, bd_addr %s, bd_type 0x%x, identity_id 0x%x",
                            p_ind->proto_id,
                            p_ind->conn_handle,
                            p_ind->remote_mtu,
                            p_ind->cid_num,
                            TRACE_BINARY(GAP_ECFC_CREATE_CHANN_MAX_NUM * 2, p_ind->cid),
                            TRACE_BDADDR(p_ind->bd_addr),
                            p_ind->bd_type,
                            p_ind->identity_id);
            if (p_ind->remote_mtu > MAX_REMOTE_MTU)
            {
                cause = GAP_ECFC_CONN_UNACCEPTABLE_PARAMS;
            }

            gap_ecfc_send_conn_cfm(p_ind->conn_handle, p_ind->identity_id,
                                   cause, p_ind->cid, p_ind->cid_num, ECFC_LOCAL_MTU);
        }
        break;

    case GAP_ECFC_CONN_CMPL:
        {
            T_GAP_ECFC_CONN_CMPL_INFO *p_info = (T_GAP_ECFC_CONN_CMPL_INFO *)p_buf;
            T_APP_LE_LINK *p_link = app_link_find_le_link_by_addr(p_info->bd_addr);

            if (p_link)
            {
                if (p_link->start_discover == false)
                {
                    if (gatt_client_start_discovery_all(p_info->conn_handle,
                                                        app_ble_eatt_client_discov_cb) == GAP_CAUSE_SUCCESS)
                    {
                        p_link->start_discover = true;
                    }
                }
            }
            APP_PRINT_INFO8("GAP_ECFC_CONN_CMPL: proto_id %d, cause 0x%x, conn_handle 0x%x, ds_data_offset %d, bd_addr %s, remote_mtu %d, local_mtu %d, local_mps %d",
                            p_info->proto_id,
                            p_info->cause,
                            p_info->conn_handle,
                            p_info->ds_data_offset,
                            TRACE_BDADDR(p_info->bd_addr),
                            p_info->remote_mtu,
                            p_info->local_mtu,
                            p_info->local_mps);
            for (uint8_t i = 0; i < p_info->cid_num; i++)
            {
                APP_PRINT_INFO2("GAP_ECFC_CONN_CMPL: cis[%d] 0x%x", i, p_info->cid[i]);
            }
        }
        break;

    case GAP_ECFC_DISCONN_IND:
        {
#if EATT_DEBUG_LOG
            T_GAP_ECFC_DISCONN_IND *p_ind = (T_GAP_ECFC_DISCONN_IND *)p_buf;
            APP_PRINT_INFO4("GAP_ECFC_DISCONN_IND: proto_id %d, conn_handle 0x%x, cid 0x%x, cause 0x%x",
                            p_ind->proto_id,
                            p_ind->conn_handle,
                            p_ind->cid,
                            p_ind->cause);
#endif
        }
        break;

    case GAP_ECFC_SEC_REG_RSP:
        {
#if EATT_DEBUG_LOG
            T_GAP_ECFC_SEC_REG_RSP *p_rsp = (T_GAP_ECFC_SEC_REG_RSP *)p_buf;
            APP_PRINT_INFO5("GAP_ECFC_SEC_REG_RSP: is_le %d, psm 0x%x, active %d, uuid 0x%x, cause 0x%x",
                            p_rsp->is_le,
                            p_rsp->psm,
                            p_rsp->active,
                            p_rsp->uuid,
                            p_rsp->cause);
#endif
        }
        break;

    case GAP_ECFC_RECONFIGURE_IND:
        {
#if EATT_DEBUG_LOG
            T_GAP_ECFC_RECONFIGURE_IND *p_ind = (T_GAP_ECFC_RECONFIGURE_IND *)p_buf;
            APP_PRINT_INFO6("GAP_ECFC_RECONFIGURE_IND: proto_id %d, conn_handle 0x%x, remote_mtu %d, remote_mps %d,cid_num %d, cis %b",
                            p_ind->proto_id,
                            p_ind->conn_handle,
                            p_ind->remote_mtu,
                            p_ind->remote_mps,
                            p_ind->cid_num,
                            TRACE_BINARY(GAP_ECFC_CREATE_CHANN_MAX_NUM * 2, p_ind->cid));
#endif
            result = GAP_ECFC_RCFG_ACCEPT;
        }
        break;

    default:
        break;
    }

    return result;
}

void app_ble_eatt_ecfc_init(void)
{
    uint16_t initial_credits = ECFC_CREDITS;

    gap_ecfc_set_param(GAP_ECFC_PARAM_INIT_CREDITS, 2, &initial_credits);

    gap_ecfc_init(1);
    gap_ecfc_reg_proto(PSM_EATT, app_ble_eatt_ecfc_cb, true, &app_ecfc_le_proto_id,
                       GAP_ECFC_DATA_PATH_GATT);
}


#endif
