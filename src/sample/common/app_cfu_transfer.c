/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_cfu_transfer->c
   * @brief     Source file for using cfu
   * @author    huan
   * @date      2022-10-19
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#if F_APP_CFU_FEATURE_SUPPORT
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include "trace.h"
#include "app_common_cfu.h"
#include "stdlib_corecrt.h"
#include "errno.h"
#if (F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_HID_SUPPORT)
#include "bt_types.h"
#include "bt_hid.h"
#endif
#if F_APP_CFU_SPP_SUPPORT
#include "bt_types.h"
#include "app_dongle_data_ctrl.h"
#endif
#if F_APP_USB_HID_SUPPORT
#include "usb_hid.h"
#endif
#if F_APP_CFU_BLE_CHANNEL_SUPPORT
#include "hids_cfu.h"
#include "gap_conn_le.h"
#endif

typedef enum t_app_cfu_error_code
{
    APP_CFU_ERROR_CODE_SUCCESS,
    APP_CFU_ERROR_CODE_REPORT_ID_ERROR,
    APP_CFU_ERROR_CODE_INVALID_PARAM,
    APP_CFU_ERROR_CODE_FORMAT_ERROR
} T_APP_CFU_ERROR_CODE;

#if F_APP_CFU_HID_SUPPORT
static bool app_cfu_send_classic_hid_data(uint8_t *bd_addr, uint8_t report_id, uint8_t report_type,
                                          T_APP_CFU_REPORT_ACTION_TYPE report_action, uint16_t len,
                                          uint8_t *data)
{
    uint8_t *header = NULL;
    uint16_t data_len = sizeof(report_id) + len;
    bool ret = false;

    header = malloc(data_len);
    if (!header)
    {
        APP_PRINT_ERROR0("app_cfu_send_classic_hid_data: no merroy for data");
        return ret;
    }

    header[0] = report_id;
    memcpy_s(&header[1], (data_len - 1), data, len);

    if (report_action == APP_CFU_REPORT_ACTION_GET)
    {
        ret = bt_hid_control_get_report_rsp(bd_addr, (T_BT_HID_REPORT_TYPE)report_type, header, data_len);
    }
    else if (report_action == APP_CFU_REPORT_ACTION_SET)
    {
        ret = bt_hid_interrupt_data_send(bd_addr, (T_BT_HID_REPORT_TYPE)report_type, header, data_len);
    }

    free(header);
    return ret;
}
#endif /*F_APP_CFU_HID_SUPPORT*/

#if (F_APP_CFU_SPP_SUPPORT || F_APP_CFU_BLE_CHANNEL_SUPPORT)
static const uint8_t d2h_sync_field[SPP_PKT_SYNC_FIELD_LEN] = {'R', 'T', 'K'};  /*sync from dongle to headset*/
static const uint8_t h2d_sync_field[SPP_PKT_SYNC_FIELD_LEN] = {'K', 'T', 'R'};  /*sync from headset to dongle*/

static bool app_cfu_check_report_id(uint8_t report_id)
{
    if ((report_id >= REPORT_ID_CFU_FEATURE) &&
        (report_id <= REPORT_ID_CFU_OFFER))
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif

#if F_APP_CFU_SPP_SUPPORT
bool app_cfu_spp_header_parse(uint16_t data_len, uint8_t *p_data, T_APP_CFU_DATA *cfu_data)
{
    uint8_t *data_temp = p_data;
    uint16_t data_len_temp = data_len;
    uint16_t data_field_len = 0;
    uint8_t report_id = 0;
    T_APP_CFU_ERROR_CODE error_code;
    uint8_t *bd_addr = cfu_data->spp_data.bd_addr;

    if (!bd_addr || !data_len || !p_data)
    {
        error_code = APP_CFU_ERROR_CODE_INVALID_PARAM;
        goto pkt_analysis_error;
    }

    /* pkt format: 3byte sync + 2byte len + 1byte report id + (len-1)byte data*/
    if (data_len_temp < SPP_PKT_HEADER_LEN)
    {
        error_code = APP_CFU_ERROR_CODE_FORMAT_ERROR;
        goto pkt_analysis_error;
    }

    /* check sync field */
    if (memcmp(d2h_sync_field, data_temp, SPP_PKT_SYNC_FIELD_LEN))
    {
        error_code = APP_CFU_ERROR_CODE_FORMAT_ERROR;
        goto pkt_analysis_error;
    }

    data_temp += SPP_PKT_SYNC_FIELD_LEN;
    data_len_temp -= SPP_PKT_SYNC_FIELD_LEN;

    LE_STREAM_TO_UINT16(data_field_len, data_temp);
    data_len_temp -= SPP_PKT_LENGTH_LEN;

    if (data_field_len != data_len_temp)
    {
        error_code = APP_CFU_ERROR_CODE_FORMAT_ERROR;
        goto pkt_analysis_error;
    }

    report_id = *data_temp;
    data_temp += SPP_PKT_REPORT_ID_LEN;
    data_len_temp -= SPP_PKT_REPORT_ID_LEN;

    APP_PRINT_TRACE2("app_cfu_spp_header_parse: rcv report data, report id %d, report len %d",
                     report_id, data_len_temp);

    if (!app_cfu_check_report_id(report_id))
    {
        error_code = APP_CFU_ERROR_CODE_REPORT_ID_ERROR;
        goto pkt_analysis_error;
    }

    cfu_data->spp_data.data_len = data_len_temp;
    cfu_data->spp_data.p_data = data_temp;
    cfu_data->spp_data.report_id = report_id;
    return true;

pkt_analysis_error:
    APP_PRINT_ERROR2("app_cfu_spp_header_parse: bd_addr(%s), error code %d", TRACE_BDADDR(bd_addr),
                     error_code);
    return false;
}

static bool app_cfu_spp_send_data(uint8_t *bd_addr, uint8_t report_id, uint16_t data_len,
                                  uint8_t *p_data)
{
    uint8_t *p_pkt, *p_pkt_head = NULL;
    uint16_t pkt_len = data_len + SPP_PKT_HEADER_LEN;
    bool ret = false;
    int8_t error_code = 0;

    p_pkt = malloc(pkt_len);
    if (!p_pkt)
    {
        error_code = EFAULT;
        goto error;
    }
    p_pkt_head = p_pkt;

    memcpy(p_pkt, h2d_sync_field, SPP_PKT_SYNC_FIELD_LEN);
    p_pkt += SPP_PKT_SYNC_FIELD_LEN;

    LE_UINT16_TO_STREAM(p_pkt, data_len + SPP_PKT_REPORT_ID_LEN);

    LE_UINT8_TO_STREAM(p_pkt, report_id);

    memcpy(p_pkt, p_data, data_len);

    ret = app_dongle_send_cmd(DONGLE_CMD_CFU_DATA, p_pkt_head, pkt_len);

    free(p_pkt_head);
    return ret;
error:
    APP_PRINT_ERROR1("app_cfu_spp_send_data: error_code %d", error_code);
    return false;
}
#endif /*F_APP_CFU_SPP_SUPPORT*/

#if F_APP_USB_HID_SUPPORT
static void *tpl_handle = NULL;
static void *app_cfu_tpl_open(void)
{
    return usb_hid_data_pipe_open(HID_INT_IN_EP_1);
}
static bool app_cfu_tpl_xmit(void *buf, uint32_t len)
{
    APP_PRINT_INFO2("app_cfu_tpl_xmit:%d-%b", len, TRACE_BINARY(len, buf));
    if (tpl_handle == NULL)
    {
        tpl_handle = app_cfu_tpl_open();
    }

    return usb_hid_data_pipe_send(tpl_handle, buf, len, NULL);
}

static bool app_cfu_usb_send_data(uint8_t report_id, uint8_t *data, uint16_t len)
{
    uint8_t *header = NULL;
    uint16_t data_len = sizeof(report_id) + len;
    bool ret = false;

    header = malloc(data_len);
    if (!header)
    {
        APP_PRINT_ERROR0("app_cfu_usb_send_data: no memory for data");
        return ret;
    }
    header[0] = report_id;
    memcpy(&header[1], data, len);
    app_cfu_tpl_xmit(header, data_len);
    free(header);
    return ret;
}

bool app_cfu_get_report(uint8_t report_id, uint8_t *data, uint16_t *len)
{
    switch (report_id)
    {
    case REPORT_ID_CFU_FEATURE:
    case REPORT_ID_CFU_FEATURE_EX:
        {
            data[0] = REPORT_ID_CFU_FEATURE;
            app_cfu_get_version_parse((FW_UPDATE_VER_RESPONSE *)&data[1]);
            *len =  sizeof(FW_UPDATE_VER_RESPONSE) + 1;
        }
        break;
    default:
        break;
    }
    APP_PRINT_TRACE2("app_cfu_get_report:report_id 0x%x len 0x%x", report_id, *len);
    return true;
}

void app_cfu_set_report(uint8_t *data, uint16_t len)
{
    uint8_t report_id = data[0];
    T_APP_CFU_DATA cfu_data;

    APP_PRINT_INFO2("app_cfu_set_report: report_id 0x%x, data_len %d", report_id, len);
    cfu_data.usb_data.data_len = len - 1;
    cfu_data.usb_data.p_data = &data[1];
    app_cfu_handle_report(report_id, APP_CFU_REPORT_SOURCE_USB, &cfu_data);
}
#endif /*F_APP_USB_HID_SUPPORT*/

#if F_APP_CFU_BLE_CHANNEL_SUPPORT
bool app_cfu_ble_header_parse(T_APP_CFU_DATA *cfu_data)
{
    uint8_t *data_temp = cfu_data->ble_data.p_data;
    uint16_t data_len_temp = cfu_data->ble_data.data_len;
    uint16_t data_field_len = 0;
    uint8_t report_id = 0;

    T_APP_CFU_ERROR_CODE error_code;

    if (!data_len_temp || !cfu_data->ble_data.p_data)
    {
        error_code = APP_CFU_ERROR_CODE_INVALID_PARAM;
        goto pkt_analysis_error;
    }

    /* pkt format: 3byte sync + 2byte len + 1byte report id + (len-1)byte data*/
    if (data_len_temp < SPP_PKT_HEADER_LEN)
    {
        error_code = APP_CFU_ERROR_CODE_FORMAT_ERROR;
        goto pkt_analysis_error;
    }

    /* check sync field */
    if (memcmp(d2h_sync_field, data_temp, SPP_PKT_SYNC_FIELD_LEN))
    {
        error_code = APP_CFU_ERROR_CODE_FORMAT_ERROR;
        goto pkt_analysis_error;
    }

    data_temp += SPP_PKT_SYNC_FIELD_LEN;
    data_len_temp -= SPP_PKT_SYNC_FIELD_LEN;

    LE_STREAM_TO_UINT16(data_field_len, data_temp);
    data_len_temp -= SPP_PKT_LENGTH_LEN;

    if (data_field_len != data_len_temp)
    {
        error_code = APP_CFU_ERROR_CODE_FORMAT_ERROR;
        goto pkt_analysis_error;
    }

    report_id = *data_temp;
    data_temp += SPP_PKT_REPORT_ID_LEN;
    data_len_temp -= SPP_PKT_REPORT_ID_LEN;

    APP_PRINT_TRACE2("app_cfu_ble_header_parse: rcv report data, report id %d, report len %d",
                     report_id, data_len_temp);

    if (!app_cfu_check_report_id(report_id))
    {
        error_code = APP_CFU_ERROR_CODE_REPORT_ID_ERROR;
        goto pkt_analysis_error;
    }

    cfu_data->ble_data.data_len = data_len_temp;
    cfu_data->ble_data.p_data = data_temp;
    return true;

pkt_analysis_error:
    APP_PRINT_ERROR1("app_cfu_ble_header_parse: error code %d", error_code);
    return false;
}

static bool app_cfu_ble_send_data(uint16_t conn_id, uint8_t report_id, uint16_t data_len,
                                  uint8_t *p_data)
{
    uint8_t *p_pkt, *p_pkt_head = NULL;
    uint16_t pkt_len = data_len + SPP_PKT_HEADER_LEN;
    uint16_t conn_handle;
    bool ret = false;
    int8_t error_code = 0;

    p_pkt = malloc(pkt_len);
    if (!p_pkt)
    {
        error_code = EFAULT;
        goto error;
    }
    p_pkt_head = p_pkt;

    memcpy(p_pkt, h2d_sync_field, SPP_PKT_SYNC_FIELD_LEN);
    p_pkt += SPP_PKT_SYNC_FIELD_LEN;

    LE_UINT16_TO_STREAM(p_pkt, data_len + SPP_PKT_REPORT_ID_LEN);

    LE_UINT8_TO_STREAM(p_pkt, report_id);

    memcpy(p_pkt, p_data, data_len);

    conn_handle = le_get_conn_handle(conn_id);
    ret = hids_cfu_send_report(conn_handle, report_id, p_pkt_head, pkt_len);

    free(p_pkt_head);
    return ret;
error:
    APP_PRINT_ERROR1("app_cfu_ble_send_data: error_code %d", error_code);
    return false;
}

#endif /*F_APP_CFU_BLE_CHANNEL_SUPPORT*/

static bool app_cfu_send_data(uint8_t report_id, T_APP_CFU_REPORT_SOURCE source_type,
                              T_APP_CFU_DATA *cfu_data)
{
    APP_PRINT_TRACE2("app_cfu_send_data: report_id %d, source_type %d",
                     report_id, source_type);
    bool ret = false;

    switch (source_type)
    {
#if F_APP_CFU_SPP_SUPPORT
    case APP_CFU_REPORT_SOURCE_SPP:
        ret = app_cfu_spp_send_data(cfu_data->spp_data.bd_addr, report_id,
                                    cfu_data->spp_data.data_len, cfu_data->spp_data.p_data);
        break;
#endif
#if F_APP_CFU_HID_SUPPORT
    case APP_CFU_REPORT_SOURCE_CLASSIC_HID:
        ret = app_cfu_send_classic_hid_data(cfu_data->classic_hid_data.bd_addr,
                                            cfu_data->classic_hid_data.report_id,
                                            cfu_data->classic_hid_data.report_type,
                                            cfu_data->classic_hid_data.report_action,
                                            cfu_data->classic_hid_data.data_len,
                                            cfu_data->classic_hid_data.p_data);
        break;
#endif
#if F_APP_USB_HID_SUPPORT
    case APP_CFU_REPORT_SOURCE_USB:
        ret = app_cfu_usb_send_data(report_id, cfu_data->usb_data.p_data, cfu_data->usb_data.data_len);
        break;
#endif

#if F_APP_CFU_BLE_CHANNEL_SUPPORT
    case APP_CFU_REPORT_SOURCE_BLE:
        ret = app_cfu_ble_send_data(cfu_data->ble_data.conn_id, report_id, cfu_data->ble_data.data_len,
                                    cfu_data->ble_data.p_data);
        break;
#endif

    default:
        break;
    }
    return ret;
}

void app_cfu_handle_report(uint8_t report_id, T_APP_CFU_REPORT_SOURCE source_type,
                           T_APP_CFU_DATA *p_cfu_data)
{
    APP_PRINT_INFO2("app_cfu_handle_report: report_id 0x%x, source_type %d",
                    report_id, source_type);
    uint8_t *data = NULL;
    uint16_t len = 0;
    T_APP_CFU_DATA cfu_rsp = {0};

    switch (source_type)
    {
#if F_APP_CFU_SPP_SUPPORT
    case APP_CFU_REPORT_SOURCE_SPP:
        app_cfu_save_link_info(p_cfu_data->spp_data.bd_addr);
        data = p_cfu_data->spp_data.p_data;
        len = p_cfu_data->spp_data.data_len;
        memcpy(cfu_rsp.spp_data.bd_addr, p_cfu_data->spp_data.bd_addr, 6);

        if (report_id == REPORT_ID_CFU_FEATURE)
        {
            if (len == 0)
            {
                FW_UPDATE_VER_RESPONSE verRsp = {0};
                app_cfu_get_version_parse(&verRsp);

                cfu_rsp.spp_data.data_len = sizeof(verRsp);
                cfu_rsp.spp_data.p_data = (uint8_t *)&verRsp;
                app_cfu_send_data(REPORT_ID_CFU_FEATURE, APP_CFU_REPORT_SOURCE_SPP, &cfu_rsp);
            }
            else
            {
                FW_UPDATE_CONTENT_RESPONSE contentRsp = {0};

                if (app_cfu_content_parse(data, len, &contentRsp) == CFU_OTH_COMPONENT_RSP)
                {
                    return;
                }

                cfu_rsp.spp_data.data_len = sizeof(contentRsp);
                cfu_rsp.spp_data.p_data = (uint8_t *)&contentRsp;
                app_cfu_send_data(REPORT_ID_CFU_CONTENT, APP_CFU_REPORT_SOURCE_SPP, &cfu_rsp);
            }
        }
        else if (report_id == REPORT_ID_CFU_OFFER)
        {
            FW_UPDATE_OFFER_RESPONSE offerRsp = {0};
            if (app_cfu_offer_parse(data, len, &offerRsp) == CFU_OTH_COMPONENT_RSP)
            {
                return;
            }

            cfu_rsp.spp_data.data_len = sizeof(offerRsp);
            cfu_rsp.spp_data.p_data = (uint8_t *)&offerRsp;
            app_cfu_send_data(report_id, APP_CFU_REPORT_SOURCE_SPP, &cfu_rsp);
        }
        break;
#endif /*F_APP_CFU_SPP_SUPPORT*/

#if (F_APP_TEAMS_HID_SUPPORT || F_APP_CFU_HID_SUPPORT)
    case APP_CFU_REPORT_SOURCE_CLASSIC_HID:
        {
            uint8_t report_type = 0;
            T_APP_CFU_REPORT_ACTION_TYPE report_action;
            app_cfu_save_link_info(p_cfu_data->classic_hid_data.bd_addr);
            report_type = p_cfu_data->classic_hid_data.report_type;
            report_action = p_cfu_data->classic_hid_data.report_action;
            data = p_cfu_data->classic_hid_data.p_data;
            len = p_cfu_data->classic_hid_data.data_len;
            memcpy(cfu_rsp.classic_hid_data.bd_addr, p_cfu_data->classic_hid_data.bd_addr, 6);

            if (report_action == APP_CFU_REPORT_ACTION_GET)
            {
                if (report_id == REPORT_ID_CFU_FEATURE)
                {
                    FW_UPDATE_VER_RESPONSE pVerRsp = {0};
                    app_cfu_get_version_parse(&pVerRsp);

                    cfu_rsp.classic_hid_data.data_len = sizeof(FW_UPDATE_VER_RESPONSE);
                    cfu_rsp.classic_hid_data.p_data = (uint8_t *)&pVerRsp;
                    cfu_rsp.classic_hid_data.report_id = report_id;
                    cfu_rsp.classic_hid_data.report_type = report_type;
                    cfu_rsp.classic_hid_data.report_action = report_action;
                    app_cfu_send_data(report_id, APP_CFU_REPORT_SOURCE_CLASSIC_HID, &cfu_rsp);
                }
            }
            else if (report_action == APP_CFU_REPORT_ACTION_SET)
            {
                if (report_id == REPORT_ID_CFU_OFFER)
                {
                    FW_UPDATE_OFFER_RESPONSE offerRsp = {0};

                    if (app_cfu_offer_parse(data, len, &offerRsp) == CFU_OTH_COMPONENT_RSP)
                    {
                        return;
                    }

                    cfu_rsp.classic_hid_data.data_len = sizeof(FW_UPDATE_OFFER_RESPONSE);
                    cfu_rsp.classic_hid_data.p_data = (uint8_t *)&offerRsp;
                    cfu_rsp.classic_hid_data.report_id = report_id;
                    cfu_rsp.classic_hid_data.report_type = BT_HID_REPORT_TYPE_INPUT;
                    cfu_rsp.classic_hid_data.report_action = report_action;
                    app_cfu_send_data(REPORT_ID_CFU_OFFER, APP_CFU_REPORT_SOURCE_CLASSIC_HID,
                                      &cfu_rsp);
                }
                else if (report_id == REPORT_ID_CFU_FEATURE)
                {
                    FW_UPDATE_CONTENT_RESPONSE contentRsp = {0};
                    if (app_cfu_content_parse(data, len, &contentRsp) == CFU_OTH_COMPONENT_RSP)
                    {
                        return;
                    }

                    cfu_rsp.classic_hid_data.data_len = sizeof(contentRsp);
                    cfu_rsp.classic_hid_data.p_data = (uint8_t *)&contentRsp;
                    cfu_rsp.classic_hid_data.report_id = REPORT_ID_CFU_CONTENT;
                    cfu_rsp.classic_hid_data.report_type = BT_HID_REPORT_TYPE_INPUT;
                    cfu_rsp.classic_hid_data.report_action = report_action;
                    app_cfu_send_data(REPORT_ID_CFU_CONTENT, APP_CFU_REPORT_SOURCE_CLASSIC_HID,
                                      &cfu_rsp);
                }
            }
            break;
        }
#endif /*F_APP_TEAMS_HID_SUPPORT | F_APP_CFU_HID_SUPPORT*/

#if F_APP_USB_HID_SUPPORT
    case APP_CFU_REPORT_SOURCE_USB:
        data = p_cfu_data->usb_data.p_data;
        len = p_cfu_data->usb_data.data_len;

        if (report_id == REPORT_ID_CFU_OFFER)
        {
            FW_UPDATE_OFFER_RESPONSE offerRsp = {0};

            if (app_cfu_offer_parse(data, len, &offerRsp) == CFU_OTH_COMPONENT_RSP)
            {
                return;
            }
            cfu_rsp.usb_data.data_len = sizeof(offerRsp);
            cfu_rsp.usb_data.p_data = (uint8_t *)&offerRsp;
            app_cfu_send_data(REPORT_ID_CFU_OFFER, APP_CFU_REPORT_SOURCE_USB, &cfu_rsp);
        }
        else if (report_id == REPORT_ID_CFU_FEATURE)
        {
            FW_UPDATE_CONTENT_RESPONSE contentRsp = {0};
            if (app_cfu_content_parse(data, len, &contentRsp) == CFU_OTH_COMPONENT_RSP)
            {
                return;
            }
            cfu_rsp.usb_data.data_len = sizeof(contentRsp);
            cfu_rsp.usb_data.p_data = (uint8_t *)&contentRsp;
            app_cfu_send_data(REPORT_ID_CFU_CONTENT, APP_CFU_REPORT_SOURCE_USB,
                              &cfu_rsp);
        }

        break;
#endif /*F_APP_USB_HID_SUPPORT*/

#if F_APP_CFU_BLE_CHANNEL_SUPPORT
    case APP_CFU_REPORT_SOURCE_BLE:
        data = p_cfu_data->ble_data.p_data;
        len = p_cfu_data->ble_data.data_len;
        cfu_rsp.ble_data.conn_id = p_cfu_data->ble_data.conn_id;

        if (report_id == REPORT_ID_CFU_FEATURE)
        {
            if (p_cfu_data->ble_data.report_type == HID_FEATURE_TYPE)
            {
                FW_UPDATE_VER_RESPONSE verRsp = {0};
                app_cfu_get_version_parse(&verRsp);

                cfu_rsp.ble_data.data_len = sizeof(verRsp);
                cfu_rsp.ble_data.p_data = (uint8_t *)&verRsp;
                app_cfu_send_data(REPORT_ID_CFU_FEATURE, APP_CFU_REPORT_SOURCE_BLE, &cfu_rsp);
            }
            else
            {
                FW_UPDATE_CONTENT_RESPONSE contentRsp = {0};

                if (app_cfu_content_parse(data, len, &contentRsp) == CFU_OTH_COMPONENT_RSP)
                {
                    return;
                }

                cfu_rsp.ble_data.data_len = sizeof(contentRsp);
                cfu_rsp.ble_data.p_data = (uint8_t *)&contentRsp;
                app_cfu_send_data(REPORT_ID_CFU_CONTENT, APP_CFU_REPORT_SOURCE_BLE, &cfu_rsp);
            }
        }
        else if (report_id == REPORT_ID_CFU_OFFER)
        {
            FW_UPDATE_OFFER_RESPONSE offerRsp = {0};
            if (app_cfu_offer_parse(data, len, &offerRsp) == CFU_OTH_COMPONENT_RSP)
            {
                return;
            }

            cfu_rsp.ble_data.data_len = sizeof(offerRsp);
            cfu_rsp.ble_data.p_data = (uint8_t *)&offerRsp;
            app_cfu_send_data(report_id, APP_CFU_REPORT_SOURCE_BLE, &cfu_rsp);
        }
        break;
#endif /*F_APP_CFU_BLE_CHANNEL_SUPPORT*/

    default:
        break;
    }
}

#endif /*F_APP_CFU_FEATURE_SUPPORT*/
