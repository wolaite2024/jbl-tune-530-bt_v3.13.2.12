/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>

#include "os_mem.h"
#include "trace.h"
#include "btm.h"
#include "bt_spp.h"
#include "app_link_util.h"
#include "app_spp.h"
#include "app_cfg.h"
#include "app_transfer.h"
#include "app_main.h"
#include "app_cmd.h"
#include "app_sdp.h"

#if BISTO_FEATURE_SUPPORT
#include "app_bisto_bt.h"
#endif

#if XM_XIAOAI_FEATURE_SUPPORT
#include "app_xiaoai_transport.h"
#endif

#if AMA_FEATURE_SUPPORT
#include "app_ama_transport.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_spp.h"
#endif

static const uint8_t spp_service_class_uuid128[16] =
{
    0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
};

static const uint8_t rtk_vendor_spp_service_class_uuid128[16] =
{
    0x6a, 0x24, 0xee, 0xab, 0x4b, 0x65, 0x46, 0x93, 0x98, 0x6b, 0x3c, 0x26, 0xc3, 0x52, 0x26, 0x4f
};

static void app_spp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_SPP_CONN_CMPL:
        {
#if F_APP_GAMING_DONGLE_SUPPORT
            if (app_link_check_dongle_link(param->spp_conn_cmpl.bd_addr))
            {
                return;
            }
#endif

            if (param->spp_conn_cmpl.local_server_chann != RFC_SPP_CHANN_NUM
                && param->spp_conn_cmpl.local_server_chann != RFC_RTK_VENDOR_CHANN_NUM)
            {
                return;
            }

            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->spp_conn_cmpl.bd_addr);

            if (p_link != NULL)
            {
                if (param->spp_conn_cmpl.local_server_chann == RFC_RTK_VENDOR_CHANN_NUM)
                {
                    p_link->rtk_vendor_spp_active = 1;
                }

                p_link->rfc_credit = param->spp_conn_cmpl.link_credit;
                p_link->rfc_frame_size = param->spp_conn_cmpl.frame_size;

                //check credit
                if (p_link->rfc_credit)
                {
                    app_transfer_pop_data_queue(CMD_PATH_SPP, false);
                }
            }
        }
        break;

    case BT_EVENT_SPP_CREDIT_RCVD:
        {
            if (param->spp_credit_rcvd.local_server_chann != RFC_SPP_CHANN_NUM
                && param->spp_credit_rcvd.local_server_chann != RFC_RTK_VENDOR_CHANN_NUM)
            {
                return;
            }

            p_link = app_link_find_br_link(param->spp_credit_rcvd.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_spp_bt_cback: no acl link found");
                return;
            }
            if ((p_link->rfc_credit == 0) && (param->spp_credit_rcvd.link_credit))
            {
                app_transfer_pop_data_queue(CMD_PATH_SPP, false);
            }
            p_link->rfc_credit = param->spp_credit_rcvd.link_credit;
        }
        break;

#if F_APP_VENDOR_CMD_SUPPORT
    case BT_EVENT_SPP_DATA_IND:
        {
            if (param->spp_data_ind.local_server_chann != RFC_SPP_CHANN_NUM
                && param->spp_data_ind.local_server_chann != RFC_RTK_VENDOR_CHANN_NUM)
            {
                return;
            }

            uint8_t     *p_data;
            uint16_t    len;
            uint8_t     app_idx;
            uint16_t    data_len;
            uint16_t    total_len;

            p_link = app_link_find_br_link(param->spp_data_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_spp_bt_cback: no acl link found");
                return;
            }
            app_idx = p_link->id;
            p_data = param->spp_data_ind.data;
            len = param->spp_data_ind.len;
            data_len = len;

            bt_spp_credits_give(app_db.br_link[app_idx].bd_addr, param->spp_data_ind.local_server_chann,
                                1);

            if (app_cfg_const.enable_rtk_vendor_cmd)
            {
                if (app_db.br_link[app_idx].p_embedded_cmd == NULL)
                {
                    uint16_t cmd_len;

                    //ios will auto combine two cmd into one pkt
                    while (data_len)
                    {
                        if (p_data[0] == CMD_SYNC_BYTE)
                        {
                            cmd_len = (p_data[2] | (p_data[3] << 8)) + 4; //sync_byte, seqn, length
                            if (data_len >= cmd_len)
                            {
                                app_cmd_handler(&p_data[4], (cmd_len - 4), CMD_PATH_SPP, p_data[1], app_idx);
                                data_len -= cmd_len;
                                p_data += cmd_len;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            data_len--;
                            p_data++;
                        }
                    }

                    if (data_len)
                    {
                        app_db.br_link[app_idx].p_embedded_cmd = malloc(data_len);
                        memcpy(app_db.br_link[app_idx].p_embedded_cmd, p_data, data_len);
                        app_db.br_link[app_idx].embedded_cmd_len = data_len;
                    }
                }
                else
                {
                    uint8_t *p_temp;
                    uint16_t cmd_len;

                    p_temp = app_db.br_link[app_idx].p_embedded_cmd;
                    total_len = app_db.br_link[app_idx].embedded_cmd_len + data_len;
                    app_db.br_link[app_idx].p_embedded_cmd = malloc(total_len);
                    memcpy(app_db.br_link[app_idx].p_embedded_cmd, p_temp,
                           app_db.br_link[app_idx].embedded_cmd_len);
                    free(p_temp);
                    memcpy(app_db.br_link[app_idx].p_embedded_cmd +
                           app_db.br_link[app_idx].embedded_cmd_len,
                           p_data, data_len);
                    app_db.br_link[app_idx].embedded_cmd_len = total_len;

                    p_data = app_db.br_link[app_idx].p_embedded_cmd;
                    data_len = total_len;
                    p_temp = app_db.br_link[app_idx].p_embedded_cmd;
                    app_db.br_link[app_idx].p_embedded_cmd = NULL;
                    //ios will auto combine two cmd into one pkt
                    while (data_len)
                    {
                        if (p_data[0] == CMD_SYNC_BYTE)
                        {
                            cmd_len = (p_data[2] | (p_data[3] << 8)) + 4; //sync_byte, seqn, length
                            if (data_len >= cmd_len)
                            {
                                app_cmd_handler(&p_data[4], (cmd_len - 4), CMD_PATH_SPP, p_data[1], app_idx);
                                data_len -= cmd_len;
                                p_data += cmd_len;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            data_len--;
                            p_data++;
                        }
                    }

                    if (data_len)
                    {
                        app_db.br_link[app_idx].p_embedded_cmd = malloc(data_len);
                        memcpy(app_db.br_link[app_idx].p_embedded_cmd, p_data, data_len);
                    }
                    app_db.br_link[app_idx].embedded_cmd_len = data_len;
                    free(p_temp);
                }
            }
#if 0
            else if (app_cfg_const.enable_data_uart)
            {
                uint8_t     *tx_ptr;
                uint16_t    total_len;
                uint8_t     pkt_type;
                uint16_t    pkt_len;

                pkt_type = PKT_TYPE_SINGLE;
                total_len = len;
                while (len)
                {
                    if (len > (app_db.external_mcu_mtu - 12))
                    {
                        pkt_len = app_db.external_mcu_mtu - 12;
                        if (pkt_type == PKT_TYPE_SINGLE)
                        {
                            pkt_type = PKT_TYPE_START;
                        }
                        else
                        {
                            pkt_type = PKT_TYPE_CONT;
                        }
                    }
                    else
                    {
                        pkt_len = len;
                        if (pkt_type != PKT_TYPE_SINGLE)
                        {
                            pkt_type = PKT_TYPE_END;
                        }
                    }
                    tx_ptr = malloc((pkt_len + 6));
                    if (tx_ptr != NULL)
                    {
                        tx_ptr[0] = app_idx;
                        tx_ptr[1] = pkt_type;
                        tx_ptr[2] = (uint8_t)total_len;
                        tx_ptr[3] = (uint8_t)(total_len >> 8);
                        tx_ptr[4] = (uint8_t)pkt_len;
                        tx_ptr[5] = (uint8_t)(pkt_len >> 8);
                        memcpy(&tx_ptr[6], p_data, pkt_len);

                        app_report_event(CMD_PATH_UART, EVENT_LEGACY_DATA_TRANSFER, 0, tx_ptr, pkt_len + 6);

                        free(tx_ptr);
                        p_data += pkt_len;
                        len -= pkt_len;
                    }
                }
            }
#endif
        }
        break;
#endif

    case BT_EVENT_SPP_CONN_IND:
        {
            if (param->spp_conn_ind.local_server_chann != RFC_SPP_CHANN_NUM
                && param->spp_conn_ind.local_server_chann != RFC_RTK_VENDOR_CHANN_NUM)
            {
                return;
            }

            p_link = app_link_find_br_link(param->spp_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("app_spp_bt_cback: no acl link found");
                return;
            }
            uint8_t local_server_chann = param->spp_conn_ind.local_server_chann;
            uint16_t frame_size = param->spp_conn_ind.frame_size;
            bt_spp_connect_cfm(p_link->bd_addr, local_server_chann, true, frame_size, 7);
        }
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            if (param->spp_disconn_cmpl.local_server_chann != RFC_SPP_CHANN_NUM
                && param->spp_disconn_cmpl.local_server_chann != RFC_RTK_VENDOR_CHANN_NUM)
            {
                return;
            }

            p_link = app_link_find_br_link(param->spp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                p_link->rfc_credit = 0;
                if (param->spp_disconn_cmpl.local_server_chann == RFC_RTK_VENDOR_CHANN_NUM)
                {
                    p_link->rtk_vendor_spp_active = 0;
                }
            }

            app_transfer_queue_reset(CMD_PATH_SPP);
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_spp_bt_cback: event_type 0x%04x", event_type);
    }
}

void app_spp_init(void)
{
    if ((app_cfg_const.supported_profile_mask & SPP_PROFILE_MASK) == 0)
    {
        APP_PRINT_WARN0("app_spp_init: not support spp");
        return;
    }

    uint8_t service_num = 2;

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        service_num += BISTO_SPP_CHANN_NUM;
    }
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
    service_num++;
#endif

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support && app_ama_transport_supported(AMA_SPP_STREAM))
    {
        service_num += 2;
    }
#endif

    bt_spp_init(app_cfg_const.spp_link_number, service_num);
    bt_mgr_cback_register(app_spp_bt_cback);

#if XM_XIAOAI_FEATURE_SUPPORT
    if (extend_app_cfg_const.xiaoai_support && app_xiaoai_over_spp_supported())
    {
        bt_spp_service_register((uint8_t *)rtk_vendor_spp_service_class_uuid128, RFC_SPP_CHANN_NUM);
    }
    else
    {
        bt_spp_service_register((uint8_t *)spp_service_class_uuid128, RFC_SPP_CHANN_NUM);
    }
#else
    bt_spp_service_register((uint8_t *)spp_service_class_uuid128, RFC_SPP_CHANN_NUM);
#endif

#if F_APP_SPECIFIC_UUID_SUPPORT
    if (app_cfg_const.enable_specific_service_uuid)
    {
        bt_spp_service_register((uint8_t *)app_cfg_const.specific_service_uuid,
                                RFC_RTK_VENDOR_CHANN_NUM);
    }
    else
    {
        bt_spp_service_register((uint8_t *)rtk_vendor_spp_service_class_uuid128, RFC_RTK_VENDOR_CHANN_NUM);
    }
#else
    bt_spp_service_register((uint8_t *)rtk_vendor_spp_service_class_uuid128, RFC_RTK_VENDOR_CHANN_NUM);
#endif
}
