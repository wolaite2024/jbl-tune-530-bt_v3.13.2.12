/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "bt_sdp.h"
#include "btm.h"
#include "bt_opp.h"
#include "app_opp.h"
#include "app_sdp.h"
#include "app_bt_audio_link.h"

#define RFC_OPP_CHANN_NUM               24
#define L2CAP_OPP_PSM                   0x1003


const uint8_t opp_sdp_record[] =
{
    SDP_DATA_ELEM_SEQ_HDR,
    0x50,
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_OBEX_OBJECT_PUSH >> 8),
    (uint8_t)(UUID_OBEX_OBJECT_PUSH),

    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x11,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_L2CAP >> 8),
    (uint8_t)(UUID_L2CAP),
    SDP_DATA_ELEM_SEQ_HDR,
    0x05,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_RFCOMM >> 8),
    (uint8_t)(UUID_RFCOMM),
    SDP_UNSIGNED_ONE_BYTE,
    RFC_OPP_CHANN_NUM,
    SDP_DATA_ELEM_SEQ_HDR,
    0x03,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_OBEX >> 8),
    (uint8_t)(UUID_OBEX),

    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8),
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST,
    SDP_DATA_ELEM_SEQ_HDR,
    0x08,
    SDP_DATA_ELEM_SEQ_HDR,
    0x06,
    SDP_UUID16_HDR,
    (uint8_t)(UUID_OBEX_OBJECT_PUSH >> 8),
    (uint8_t)UUID_OBEX_OBJECT_PUSH,
    SDP_UNSIGNED_TWO_BYTE,
    0x01,
    0x02,   /* version 1.2 */

    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),
    SDP_STRING_HDR,
    0x0B,
    'R', 'e', 'a', 'l', 't', 'e', 'k', '-', 'O', 'P', 'P',

    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_L2C_PSM) >> 8),
    (uint8_t)(SDP_ATTR_L2C_PSM),
    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)(L2CAP_OPP_PSM >> 8),
    (uint8_t)(L2CAP_OPP_PSM),

    SDP_UNSIGNED_TWO_BYTE,
    (uint8_t)((SDP_ATTR_SUPPORTED_FORMATS_LIST) >> 8),
    (uint8_t)(SDP_ATTR_SUPPORTED_FORMATS_LIST),
    SDP_DATA_ELEM_SEQ_HDR,
    0x0a,
    SDP_UNSIGNED_ONE_BYTE,
    0x01, /* vCard 2.1 */
    SDP_UNSIGNED_ONE_BYTE,
    0x02, /* vCard 3.0 */
    SDP_UNSIGNED_ONE_BYTE,
    0x03, /* vCal 1.0 */
    SDP_UNSIGNED_ONE_BYTE,
    0x04, /* vCal 2.0 */
    SDP_UNSIGNED_ONE_BYTE,
    0xff, /* any type of object */
};

uint32_t total_len = 18;
uint8_t  name[12] = {0x00, 0x31, 0x00, 0x2e, 0x00, 0x74, 0x00, 0x78, 0x00, 0x74, 0x00, 0x00}; /*1.txt*/
uint8_t  type[11] = {0x74, 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0x00}; /* text/plain*/
uint8_t  data[18] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
                     0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
                     0x31, 0x32, 0x33, 0x34, 0x0d, 0x0a,
                    };
uint8_t data_len = 6;
uint8_t data_idx = 0;

static void app_opp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_OPP_CONN_IND:
        {
            bt_opp_connect_cfm(param->opp_conn_ind.bd_addr, true);
        }
        break;

    case BT_EVENT_OPP_CONN_CMPL:
        {
            // bt_opp_push_data_header_req(param->opp_conn_cmpl.bd_addr, total_len, name, sizeof(name), type, sizeof(type), false);
            // data_idx += data_len;
        }
        break;

    case BT_EVENT_OPP_DATA_HEADER_IND:
        {
            APP_PRINT_INFO5("app_opp_bt_cback: DATA_HEADER_IND %x, %s, %s, %x, %x",
                            param->opp_data_header_ind.total_data_len,
                            TRACE_STRING(param->opp_data_header_ind.name), TRACE_STRING(param->opp_data_header_ind.type),
                            param->opp_data_header_ind.name_len, param->opp_data_header_ind.type_len);
        }
        break;

    case BT_EVENT_OPP_DATA_IND:
        {
            if (!param->opp_data_ind.data_end)
            {
                //bt_opp_send_data_rsp(param->opp_data_ind.bd_addr, BT_OPP_RSP_CONTINUE);
            }
            else
            {
                //bt_opp_send_data_rsp(param->opp_data_ind.bd_addr, BT_OPP_RSP_SUCCESS);
            }

            APP_PRINT_INFO3("app_opp_bt_cback: DATA_IND 0x%04x, end %x, first_data %x",
                            param->opp_data_ind.data_len,
                            param->opp_data_ind.data_end, *(uint8_t *)param->opp_data_ind.p_data);
        }
        break;

    case BT_EVENT_OPP_DATA_RSP:
        {
            if (param->opp_data_rsp.cause == BT_OPP_RSP_CONTINUE)
            {
                if (data_idx != 0)
                {
                    if (data_idx + data_len < total_len)
                    {
                        bt_opp_push_data_req(param->opp_data_rsp.bd_addr, &data[data_idx], data_len, true);
                        data_idx += data_len;
                    }
                    else
                    {
                        bt_opp_push_data_req(param->opp_data_rsp.bd_addr, &data[data_idx], total_len - data_idx, false);
                        data_idx = 0;
                    }
                }
            }
        }
        break;

    case BT_EVENT_OPP_DISCONN_CMPL:
        {

        }
        break;

    case BT_EVENT_SDP_ATTR_INFO:
        {
            T_BT_SDP_ATTR_INFO *sdp_info = &param->sdp_attr_info.info;

            if (sdp_info->srv_class_uuid_data.uuid_16 == UUID_OBEX_OBJECT_PUSH)
            {
                if (sdp_info->profile_version >= 0x0102)
                {
                    bt_opp_connect_over_l2c_req(param->sdp_attr_info.bd_addr, sdp_info->l2c_psm);
                }
                else
                {
                    bt_opp_connect_over_rfc_req(param->sdp_attr_info.bd_addr, sdp_info->server_channel);
                }
            }
        }
        break;

    case BT_EVENT_ACL_CONN_DISCONN:
        {
            bt_device_mode_set(BT_DEVICE_MODE_DISCOVERABLE_CONNECTABLE);
        }
        break;

    case BT_EVENT_READY:
        {
            //app_opp_connect_req(app_db.remote_addr);
        }

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_opp_bt_cback: event_type 0x%04x", event_type);
    }
}

bool app_opp_connect_req(uint8_t *bd_addr)
{
    T_BT_SDP_UUID_DATA uuid;

    uuid.uuid_16 = UUID_OBEX_OBJECT_PUSH;

    return bt_sdp_discov_start(bd_addr, BT_SDP_UUID16, uuid);
}

void app_opp_init(void)
{
    bt_sdp_record_add((void *)opp_sdp_record);
    bt_opp_init(2, RFC_OPP_CHANN_NUM, L2CAP_OPP_PSM);
    bt_mgr_cback_register(app_opp_bt_cback);
}
