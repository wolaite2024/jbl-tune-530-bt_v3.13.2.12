/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_IAP_SUPPORT
#include <string.h>
#include "trace.h"
#include "os_mem.h"
#include "os_queue.h"
#include "rtl876x.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_i2c.h"
#include "platform_utils.h"
#include "sysm.h"
#include "bt_iap.h"
#include "iap_stream.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_iap.h"
#include "app_sdp.h"
#include "app_relay.h"
#include "app_flags.h"

#if AMA_FEATURE_SUPPORT
#include "app_ama_transport.h"
#endif

//cp device address
#define CP_DEVICE_ADDRESS 0x10

#define MSG_BLUETOOTH_COMPONENT_INFO        0x4E01
#define MSG_START_BT_CONNECTION_UPDATES     0x4E03
#define MSG_BT_CONNECTION_UPDATE            0x4E04
#define MSG_STOP_BT_CONNECTION_UPDATES      0x4E05
#define MSG_START_CALL_STATE_UPDATES        0x4154
#define MSG_CALL_STATE_UPDATE               0x4155
#define MSG_STOP_CALL_STATE_UPDATES         0x4156
#define MSG_START_COMM_UPDATES              0x4157
#define MSG_MUTE_STATUS_UPDATE              0x4160

I2C_TypeDef *p_i2c_cp = I2C0;

typedef struct t_remote_info
{
    uint32_t info_num;
    T_APP_IAP infos[MAX_BR_LINK_NUM];
} T_REMOTE_INFO;

typedef enum t_relay_msg
{
    REMOTE_SINGLE_SYNC  = 0,
    REMOTE_SYNC_ALL     = 1,
    MSG_MAX             = 2,
} T_RELAY_MSG;

static struct
{
    T_OS_QUEUE queue;
} app_iap_mgr;

T_APP_IAP *app_iap_search_by_addr(uint8_t bd_addr[6])
{
    T_APP_IAP *p_info = (T_APP_IAP *)app_iap_mgr.queue.p_first;

    for (; p_info != NULL; p_info = p_info->p_next)
    {
        if (memcmp(p_info->bd_addr, bd_addr, 6) == 0)
        {
            return p_info;
        }
    }

    return NULL;
}

void app_iap_cp_hw_init(void *p_i2c)
{
    p_i2c_cp = (I2C_TypeDef *)p_i2c;

    //initilize GPIO PINs
    I2C_InitTypeDef I2C_InitStructure = {0};

    //Init I2C

    if (p_i2c_cp == I2C0)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
    }

    if (p_i2c_cp == I2C1)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, ENABLE);
    }

    /* I2C configuration */
    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_InitStructure.I2C_DeviveMode = I2C_DeviveMode_Master;
    I2C_InitStructure.I2C_AddressMode = I2C_AddressMode_7BIT;
    I2C_InitStructure.I2C_SlaveAddress = CP_DEVICE_ADDRESS;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;

    /* I2C module Initialize */
    I2C_Init(p_i2c_cp, &I2C_InitStructure);
    /* I2C Peripheral Enable */
    I2C_Cmd(p_i2c_cp, ENABLE);

    I2C_INTConfig(p_i2c_cp, I2C_INT_TX_ABRT, ENABLE);
}

static bool app_iap_cp_check_i2c_flag(uint32_t flag)
{
    FlagStatus flag_status = RESET;
    volatile uint16_t retry_count = 1000;

    while (retry_count)
    {
        flag_status = I2C_GetFlagState(p_i2c_cp, flag);
        if (flag_status == SET)
        {
            //APP_PRINT_TRACE1("app_cp_check_i2c_flag: retry_count %d", retry_count);
            return true;
        }
        else
        {
            retry_count--;
        }
    }

    APP_PRINT_ERROR0("app_iap_cp_check_i2c_flag: 2 CP_CAUSE_TIMEOUT");
    return false;
}

static bool app_iap_cp_write(uint8_t start_addr, uint8_t *data, uint16_t length)
{
    int32_t ret = 0;
    uint8_t no_ack;
    uint16_t i, j;

    APP_PRINT_TRACE0("app_iap_cp_write");

    if (p_i2c_cp == NULL)
    {
        ret = 1;
        goto TAG_END;
    }

    I2C_SetSlaveAddress(p_i2c_cp, CP_DEVICE_ADDRESS);
    j = 100;

    //write device address(W) & start register address
    do
    {
        no_ack = 0;
        if (length == 0)
        {
            p_i2c_cp->IC_DATA_CMD = start_addr | (1 << 9);
        }
        else
        {
            p_i2c_cp->IC_DATA_CMD = start_addr;
        }

        //wait tx fifo empty
        if (app_iap_cp_check_i2c_flag(I2C_FLAG_TFE) == false)
        {
            ret = 2;
            goto TAG_END;
        }

        i = 200; //can not too short!!!!!
        while (1)
        {
            no_ack = I2C_CheckEvent(p_i2c_cp, ABRT_7B_ADDR_NOACK);
            i--;
            if (no_ack == 1 || i == 0)
            {
                break;
            }
        }

        if (no_ack)
        {
            //clear ABRT_7B_ADDR_NOACK bit
            I2C_ClearINTPendingBit(p_i2c_cp, I2C_INT_TX_ABRT);
            //delay min 500us and retry
            platform_delay_us(500);
            j--;
            if (j == 0)
            {
                break;
            }
        }
    }

    while (no_ack);

    //write data
    if (length > 0 && data != NULL)
    {
        while (length > 1)
        {
            p_i2c_cp->IC_DATA_CMD = *data++;
            //wait tx fifo empty
            if (app_iap_cp_check_i2c_flag(I2C_FLAG_TFE) == false)
            {
                ret = 3;
                goto TAG_END;
            }

            length--;
        }

        //stop
        p_i2c_cp->IC_DATA_CMD = ((*data++) | (1 << 9));
        if (app_iap_cp_check_i2c_flag(I2C_FLAG_TFE) == false)
        {
            ret = 4;
            goto TAG_END;
        }

    }

    return true;

TAG_END:
    APP_PRINT_TRACE1("app_iap_cp_write: ret: %d", -ret);
    return false;
}

static bool app_iap_cp_burst_read(uint8_t start_addr, uint8_t *pbuf, uint16_t length)
{
    int32_t ret = 0;
    uint8_t no_ack = 0;
    uint16_t i, j;

    APP_PRINT_TRACE0("app_iap_cp_burst_read");

    if (p_i2c_cp == NULL)
    {
        ret = 1;
        goto TAG_END;
    }

    //write start register address
    if (app_iap_cp_write(start_addr, NULL, 0) == false)
    {
        ret = 2;
        goto TAG_END;
    }

    I2C_SetSlaveAddress(p_i2c_cp, CP_DEVICE_ADDRESS);

    //read command
    j = 100;

    do
    {
        no_ack = 0;
        if (length == 1)
        {
            /* genarate stop signal */
            p_i2c_cp->IC_DATA_CMD = ((0x00) | (1 << 8) | (1 << 9));
        }
        else
        {
            p_i2c_cp->IC_DATA_CMD = (0x00) | (1 << 8);
        }

        //wait tx fifo empty
        if (app_iap_cp_check_i2c_flag(I2C_FLAG_TFE) == false)
        {
            ret = 3;
            goto TAG_END;
        }

        i = 100;
        while (1)
        {
            no_ack = I2C_CheckEvent(p_i2c_cp, ABRT_7B_ADDR_NOACK);
            i--;
            if (no_ack == 1 || i == 0)
            {
                break;
            }
        }

        if (no_ack)
        {
            //clear ABRT_7B_ADDR_NOACK bit
            I2C_ClearINTPendingBit(p_i2c_cp, I2C_INT_TX_ABRT);
            //delay min 500us and retry
            platform_delay_us(500);
            j--;
            if (j == 0)
            {
                break;
            }
        }
    }

    while (no_ack);

    while (length > 1)
    {
        if (length == 2)
        {
            /* genarate stop signal */
            p_i2c_cp->IC_DATA_CMD = ((0x00) | (1 << 8) | (1 << 9));
        }
        else
        {
            p_i2c_cp->IC_DATA_CMD = (0x00) | (1 << 8);
        }

        //wait rx fifo data
        if (app_iap_cp_check_i2c_flag(I2C_FLAG_RFNE) == false)
        {
            ret = 4;
            goto TAG_END;
        }

        *pbuf++ = I2C_ReceiveData(p_i2c_cp);
        length--;
    }

    //wait rx fifo data, last byte
    if (app_iap_cp_check_i2c_flag(I2C_FLAG_RFNE) == false)
    {
        ret = 5;
        goto TAG_END;
    }

    *pbuf++ = (uint8_t)p_i2c_cp->IC_DATA_CMD;

    return true;

TAG_END:
    APP_PRINT_TRACE1("app_iap_cp_burst_read: ret %d", -ret);
    return false;
}

static void app_iap_send_ident_info(uint8_t *bd_addr)
{
    uint8_t *p_buf;
    uint8_t *p;
    uint8_t *p_head;

    p_buf = malloc(300);
    if (p_buf == NULL)
    {
        return;
    }

    APP_PRINT_TRACE1("app_iap_send_ident_info: p_buf %p", p_buf);

    p_head = p_buf;

    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_NAME, (void *)app_cfg_nv.device_name_legacy,
                               strlen((const char *)app_cfg_nv.device_name_legacy) + 1);

    char mudule_identifier[] = "1.0";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_MODEL_IDENTIFIER, mudule_identifier,
                               sizeof(mudule_identifier));

    char manufacturer[] = "realtek";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_MANUFACTURER, manufacturer, sizeof(manufacturer));

    char serial_num[] = "1.0";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_SERIAL_NUMBER, serial_num, sizeof(serial_num));

    char fw_version[6] = "1.0.0";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_FIRMWARE_VERSION, fw_version, sizeof(fw_version));

    char hw_version[6] = "2.0.0";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_HARDWARE_VERSION, hw_version, sizeof(hw_version));

    uint8_t msg_sent_by_accessory[8];
    p = msg_sent_by_accessory;
    BE_UINT16_TO_STREAM(p, MSG_BLUETOOTH_COMPONENT_INFO);
    BE_UINT16_TO_STREAM(p, MSG_START_BT_CONNECTION_UPDATES);
    BE_UINT16_TO_STREAM(p, MSG_STOP_BT_CONNECTION_UPDATES);
    BE_UINT16_TO_STREAM(p, BT_IAP_MSG_EA_PROTOCOL_SESSION_STATUS);
    //BE_UINT16_TO_STREAM(p, MSG_START_CALL_STATE_UPDATES);
    //BE_UINT16_TO_STREAM(p, MSG_STOP_CALL_STATE_UPDATES);
    //BE_UINT16_TO_STREAM(p, MSG_MUTE_STATUS_UPDATE);
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_MESSAGES_SENT_BY_ACCESSORY, msg_sent_by_accessory,
                               sizeof(msg_sent_by_accessory));

    uint8_t msg_received_from_device[6];
    p = msg_received_from_device;
    BE_UINT16_TO_STREAM(p, BT_IAP_MSG_START_EA_PROTOCOL_SESSION);
    BE_UINT16_TO_STREAM(p, BT_IAP_MSG_STOP_EA_PROTOCOL_SESSION);
    BE_UINT16_TO_STREAM(p, MSG_BT_CONNECTION_UPDATE);
    //BE_UINT16_TO_STREAM(p, BT_IAP_MSG_EA_PROTOCOL_SESSION_STATUS);
    //BE_UINT16_TO_STREAM(p, MSG_CALL_STATE_UPDATE);
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_MESSAGES_RECEIVED_FROM_DEVICE, msg_received_from_device,
                               sizeof(msg_received_from_device));

    //ID_POWER_SOURCETYPE
    uint8_t power_source = 0;
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_POWER_SOURCETYPE, &power_source, sizeof(power_source));

    //ID_MAXIMUM_CURRENT_DRAWN_FROM_DEVICE

    uint16_t max_current_drawn_from_device = 0;
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_MAXIMUM_CURRENT_DRAWN_FROM_DEVICE,
                               &max_current_drawn_from_device,
                               sizeof(max_current_drawn_from_device));

#if AMA_FEATURE_SUPPORT
    if (extend_app_cfg_const.ama_support)
    {
        //Supported EA protocol group
        if (app_ama_transport_supported(AMA_IAP_STREAM))
        {
            uint8_t *p = p_buf;
            p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_SUPPORTED_EXTERNAL_ACCESSORY_PROTOCOL, NULL, 0);

            //group : EA_PROTOCOL_ID
            uint8_t ea_protocol_id = EA_PROTOCOL_ID_ALEXA;
            p_buf = bt_iap2_prep_param(p_buf, BT_IAP_EXT_ACC_PROTOCOL_ID, &ea_protocol_id,
                                       sizeof(ea_protocol_id));

            char ea_protocol_name[] = "com.amazon.btalexa";
            p_buf = bt_iap2_prep_param(p_buf, BT_IAP_EXT_ACC_PROTOCOL_NAME, ea_protocol_name,
                                       sizeof(ea_protocol_name));

            //group EA_PROTOCOL_MATCHACTION
            uint8_t ea_protocol_match_action = 0x01;
            p_buf = bt_iap2_prep_param(p_buf, BT_IAP_EXT_ACC_PROTOCOL_MATCHACTION, &ea_protocol_match_action,
                                       sizeof(ea_protocol_match_action));

            //calculate total Length
            uint16_t total_len = p_buf - p;
            BE_UINT16_TO_STREAM(p, total_len);
        }
    }
#endif

#if F_APP_IAP_RTK_SUPPORT
    //Supported EA protocol group
    {
        uint8_t *p = p_buf;
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_SUPPORTED_EXTERNAL_ACCESSORY_PROTOCOL, NULL, 0);

        //group : EA_PROTOCOL_ID
        uint8_t ea_protocol_id = EA_PROTOCOL_ID_RTK;
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_EXT_ACC_PROTOCOL_ID, &ea_protocol_id,
                                   sizeof(ea_protocol_id));

        char ea_protocol_name[] = "com.rtk.datapath";
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_EXT_ACC_PROTOCOL_NAME, ea_protocol_name,
                                   sizeof(ea_protocol_name));

        //group EA_PROTOCOL_MATCHACTION
        uint8_t ea_protocol_match_action = 0x02;
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_EXT_ACC_PROTOCOL_MATCHACTION, &ea_protocol_match_action,
                                   sizeof(ea_protocol_match_action));

        //calculate total Length
        uint16_t total_len = p_buf - p;
        BE_UINT16_TO_STREAM(p, total_len);
    }
#endif

    //ID_PREFERRED_APP_MATCH_TEAM_ID
    char app_match_team_id[] = "C6P64J2MZX";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_PREFERRED_APP_MATCH_TEAM_ID, app_match_team_id,
                               sizeof(app_match_team_id));

    //ID_CURRENT_LANGUAGE
    char current_language[] = "en";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_CURRENT_LANGUAGE, current_language,
                               sizeof(current_language));

    //ID_SUPPORTED_LANGUAGE
    char supported_language[] = "en";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_SUPPORTED_LANGUAGE, &supported_language,
                               sizeof(supported_language));

    //BT_COMPONENT_ID group
    {
        uint8_t *p = p_buf;
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_BLUETOOTH_TRANSPORT_COMPONENT, NULL, 0);

        //group : BT_COMPONENT_ID

        uint16_t component_id = 0x5442;
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_TRANSPORT_COMPONENT_IDENTIFIER, &component_id,
                                   sizeof(component_id));

        //group TRANSPORT_COMPONENT_NAME
        char acc_name[] = "component_name";
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_TRANSPORT_COMPONENT_NAME, acc_name, sizeof(acc_name));

        //group TRANSPORT_SUPPORTS_IAP2_CONNECTION
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_TRANSPORT_SUPPORTS_IAP2_CONNECTION, NULL, 0);

        uint8_t local_bt_addr[6] = {0x00};
        uint8_t mac_addr[6] = {0x00};
        gap_get_param(GAP_PARAM_BD_ADDR, local_bt_addr);
        mac_addr[0] = local_bt_addr[5];
        mac_addr[1] = local_bt_addr[4];
        mac_addr[2] = local_bt_addr[3];
        mac_addr[3] = local_bt_addr[2];
        mac_addr[4] = local_bt_addr[1];
        mac_addr[5] = local_bt_addr[0];
        p_buf = bt_iap2_prep_param(p_buf, BT_IAP_BLUETOOTH_TRANSPORT_MEDIA_ACCESS_CONTROL_ADDRESS, mac_addr,
                                   sizeof(mac_addr));

        //calculate total Length
        uint16_t total_len = p_buf - p;
        BE_UINT16_TO_STREAM(p, total_len);
    }

    //param hid component
    {
#if 0
        BE_UINT16_TO_STREAM(p, IAP2_PARA_HDR_SIZE + IAP2_PARA_HDR_SIZE + 2 + IAP2_PARA_HDR_SIZE + sizeof(
                                acc_iap2_hid_comp_name) + IAP2_PARA_HDR_SIZE + 1);
        BE_UINT16_TO_STREAM(p, ID_IAP2_HID_COMPONENT);

        BE_UINT16_TO_STREAM(p, IAP2_PARA_HDR_SIZE + 2);
        BE_UINT16_TO_STREAM(p, IAP2_HID_COMPONENT_IDENTIFIER);
        BE_UINT16_TO_STREAM(p, HID_COMPONENT_ID);

        BE_UINT16_TO_STREAM(p, IAP2_PARA_HDR_SIZE + sizeof(acc_iap2_hid_comp_name));
        BE_UINT16_TO_STREAM(p, IAP2_HID_COMPONENT_NAME);
        ARRAY_TO_STREAM(p, acc_iap2_hid_comp_name, sizeof(acc_iap2_hid_comp_name));

        BE_UINT16_TO_STREAM(p, IAP2_PARA_HDR_SIZE + 1);
        BE_UINT16_TO_STREAM(p, IAP2_HID_COMPONENT_FUNCTION);
        BE_UINT8_TO_STREAM(p, HID_COMPONENT_FUNC_MEDIA_PLAYBACK_REMOTE);
#endif
    }

    //ID_PRODUCT_PLAN_UID
    char product_plan_uid[] = "d82bb2d497d51b45";
    p_buf = bt_iap2_prep_param(p_buf, BT_IAP_ID_PRODUCT_PLAN_UID, &product_plan_uid,
                               sizeof(product_plan_uid));

    APP_PRINT_TRACE2("app_iap_send_ident_info: p_buf %p, p_head %p", p_buf, p_head);

    bt_iap_ident_info_send(bd_addr, p_head, p_buf - p_head);
    free(p_head);
}

static void app_iap_get_bddr_by_bt_param(T_BT_EVENT event_type, T_BT_EVENT_PARAM *param,
                                         uint8_t *bd_addr)
{
    uint8_t *tmp_bd_addr = NULL;

    switch (event_type)
    {
    case BT_EVENT_IAP_AUTHEN_CMPL:
        tmp_bd_addr = param->iap_authen_cmpl.bd_addr;
        break;

    case BT_EVENT_IAP_DATA_SESSION_OPEN:
        tmp_bd_addr = param->iap_authen_cmpl.bd_addr;
        break;

    case BT_EVENT_IAP_DATA_SESSION_CLOSE:
        tmp_bd_addr = param->iap_data_session_close.bd_addr;
        break;

    case BT_EVENT_IAP_DATA_SENT:
        tmp_bd_addr = param->iap_data_sent.bd_addr;
        break;

    case BT_EVENT_IAP_CONN_IND:
        tmp_bd_addr = param->iap_conn_ind.bd_addr;
        break;

    case BT_EVENT_IAP_CONN_FAIL:
        tmp_bd_addr = param->iap_conn_fail.bd_addr;
        break;

    case BT_EVENT_IAP_CONN_CMPL:
        tmp_bd_addr = param->iap_conn_cmpl.bd_addr;
        break;

    case BT_EVENT_IAP_DATA_IND:
        tmp_bd_addr = param->iap_data_ind.bd_addr;
        break;

    case BT_EVENT_IAP_IDENTITY_INFO_REQ:
        tmp_bd_addr = param->iap_identity_info_req.bd_addr;
        break;

    case BT_EVENT_IAP_DISCONN_CMPL:
        tmp_bd_addr = param->iap_disconn_cmpl.bd_addr;
        break;

    case BT_EVENT_IAP_CTRL_MSG_IND:
        tmp_bd_addr = param->iap_ctrl_msg_ind.bd_addr;
        break;

    default:
        break;
    }

    if (tmp_bd_addr)
    {
        memcpy(bd_addr, tmp_bd_addr, 6);
    }

    return;
}

static void app_iap_single_remote_send(T_APP_IAP *p_iap, T_BT_EVENT evt)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        T_REMOTE_INFO rmt_info = {0};

        memcpy(rmt_info.infos[0].bd_addr, p_iap->bd_addr, 6);
        memcpy(&rmt_info.infos[0], p_iap, sizeof(rmt_info.infos[0]));
        rmt_info.infos[0].bt_event = evt;

        APP_PRINT_TRACE5("app_iap_single_remote_send: evt 0x%04x, bd_addr %s, frame_size %d, credit %d, authen_flag %d",
                         evt, TRACE_BDADDR(p_iap->bd_addr), p_iap->frame_size, p_iap->credit, p_iap->authen_flag);

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_APP_IAP, REMOTE_SINGLE_SYNC,
                                            (uint8_t *)&rmt_info, sizeof(rmt_info));
    }
}

static void app_iap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;
    static bool bt_comp_info_send_flag = false;
    T_APP_IAP *p_iap = NULL;
    uint8_t bd_addr[6] = {0};

    app_iap_get_bddr_by_bt_param(event_type, param, bd_addr);
    p_iap = app_iap_search_by_addr(bd_addr);

    switch (event_type)
    {
    case BT_EVENT_IAP_AUTHEN_CMPL:
        {
            if (p_iap)
            {
                if (param->iap_authen_cmpl.result == true)
                {
                    bt_comp_info_send_flag = true;
#if 0
                    char boundle_id[] = "com.realtek.EADemo2";
                    T_BT_IAP_APP_LAUNCH_METHOD method = BT_IAP_APP_LAUNCH_WITH_USER_ALERT;
                    bt_iap_launch_app(param->iap_authen_cmpl.bd_addr, boundle_id, sizeof(boundle_id), method);
#endif
                    p_iap->authen_flag = true;
                }
                else
                {
                    APP_PRINT_ERROR0("BT_EVENT_IAP_AUTHEN_CMPL fail");
                    p_iap->authen_flag = false;
                }

                app_iap_single_remote_send(p_iap, event_type);
            }
        }
        break;

    case BT_EVENT_IAP_DATA_SESSION_OPEN:
        {
            if (p_iap)
            {
                if (bt_comp_info_send_flag == true)
                {
                    bt_comp_info_send_flag = false;
                    bt_iap_comp_info_send(param->iap_data_session_open.bd_addr, 0x4254, true);
                }

                bt_iap_conn_update_start(param->iap_data_session_open.bd_addr, 0x4254);

                app_iap_single_remote_send(p_iap, event_type);

#if F_APP_IAP_RTK_SUPPORT | AMA_FEATURE_SUPPORT
                T_IAP_STREAM_EVENT_PARAM iap_stream_param;

                memcpy(iap_stream_param.bd_addr, param->iap_data_session_open.bd_addr, 6);
                iap_stream_param.eap_session_id = param->iap_data_session_open.session_id;
                iap_stream_param.connect_param.ea_protocol_id = param->iap_data_session_open.protocol_id;
                iap_stream_param.connect_param.mtu = p_iap->frame_size;
                iap_stream_param.connect_param.remote_credits = 20;
                iap_stream_event_process(IAP_STREAM_CONNECT_EVENT, &iap_stream_param);
#endif
            }
        }
        break;

    case BT_EVENT_IAP_DATA_SESSION_CLOSE:
        {
            if (p_iap)
            {
                app_iap_single_remote_send(p_iap, event_type);

#if F_APP_IAP_RTK_SUPPORT | AMA_FEATURE_SUPPORT
                T_IAP_STREAM_EVENT_PARAM iap_stream_param;

                memcpy(iap_stream_param.bd_addr, param->iap_data_session_close.bd_addr, 6);
                iap_stream_param.eap_session_id = param->iap_data_session_close.session_id;
                iap_stream_param.disconnect_param.disc_all = false;
                iap_stream_event_process(IAP_STREAM_DISCONNECT_EVENT, &iap_stream_param);
#endif
            }
        }
        break;

    case BT_EVENT_IAP_CONN_IND:
        {
            if (!p_iap)
            {
                p_iap = calloc(1, sizeof(*p_iap));
                p_iap->frame_size = param->iap_conn_ind.frame_size;
                memcpy(p_iap->bd_addr, bd_addr, 6);
                os_queue_in(&app_iap_mgr.queue, p_iap);
                bt_iap_connect_cfm(param->iap_conn_ind.bd_addr, true, param->iap_conn_ind.frame_size, 7);
                app_iap_single_remote_send(p_iap, event_type);
            }
        }
        break;

    case BT_EVENT_IAP_CONN_FAIL:
        {
            if (p_iap)
            {
                os_queue_delete(&app_iap_mgr.queue, p_iap);
                app_iap_single_remote_send(p_iap, event_type);
                free(p_iap);
            }
        }
        break;

    case BT_EVENT_IAP_CONN_CMPL:
        {
            static uint8_t recur_cnt = 0;

            recur_cnt++;
            if (recur_cnt > 3)
            {
                APP_PRINT_ERROR0("app_iap_bt_cback: iap conn cannot recur called too much");
                recur_cnt = 0;
                break;
            }

            if (p_iap == NULL)
            {
                APP_PRINT_WARN0("app_iap_bt_cback: IAP_CONN_CMPL create a info");
                p_iap = calloc(1, sizeof(*p_iap));
                memcpy(p_iap->bd_addr, bd_addr, 6);
                os_queue_in(&app_iap_mgr.queue, p_iap);
                p_iap->frame_size = param->iap_conn_cmpl.frame_size;
                app_iap_single_remote_send(p_iap, event_type);
            }
            else
            {
                T_BT_EVENT_PARAM tmp_param = {};

                memcpy(tmp_param.iap_disconn_cmpl.bd_addr, bd_addr, 6);
                tmp_param.iap_disconn_cmpl.cause = HCI_ERR | HCI_ERR_TIMEOUT;
                app_iap_bt_cback(BT_EVENT_IAP_DISCONN_CMPL, &tmp_param, sizeof(tmp_param));
                app_iap_bt_cback(event_type, event_buf, buf_len);
            }
            recur_cnt--;
        }
        break;

    case BT_EVENT_IAP_DATA_IND:
        {
            APP_PRINT_INFO4("app_iap_bt_cback: DM_EVENT_BT_IAP_DATA_IND eap_session_id %d, dev_seq_num %d, len %d, p_data %b",
                            param->iap_data_ind.eap_session_id,
                            param->iap_data_ind.dev_seq_num,
                            param->iap_data_ind.len,
                            TRACE_BINARY(param->iap_data_ind.len, param->iap_data_ind.p_data));

            bt_iap_ack_send(param->iap_data_ind.bd_addr, param->iap_data_ind.dev_seq_num);
#if F_APP_IAP_RTK_SUPPORT | AMA_FEATURE_SUPPORT
            T_IAP_STREAM_EVENT_PARAM iap_stream_param;

            memcpy(iap_stream_param.bd_addr, param->iap_data_ind.bd_addr, 6);
            iap_stream_param.eap_session_id = param->iap_data_ind.eap_session_id;
            iap_stream_param.receive_data_param.data      = param->iap_data_ind.p_data;
            iap_stream_param.receive_data_param.length    = param->iap_data_ind.len;
            iap_stream_param.receive_data_param.credits   = 0;
            iap_stream_event_process(IAP_STREAM_RECEIVE_DATA_EVENT, &iap_stream_param);
#endif
        }
        break;

    case BT_EVENT_IAP_IDENTITY_INFO_REQ:
        {
            app_iap_send_ident_info(bd_addr);
        }
        break;

    case BT_EVENT_IAP_DISCONN_CMPL:
        {
            if (p_iap)
            {
                if (param->iap_disconn_cmpl.cause != (HCI_ERR | HCI_ERR_CONN_ROLESWAP))
                {
                    p_iap->authen_flag = false;
                    os_queue_delete(&app_iap_mgr.queue, p_iap);
                    app_iap_single_remote_send(p_iap, event_type);
                    free(p_iap);
                }
#if F_APP_IAP_RTK_SUPPORT | AMA_FEATURE_SUPPORT
                T_IAP_STREAM_EVENT_PARAM iap_stream_param = {0};

                memcpy(iap_stream_param.bd_addr, bd_addr, 6);
                iap_stream_param.disconnect_param.disc_all = true;
                iap_stream_param.disconnect_param.cause = param->iap_disconn_cmpl.cause;
                iap_stream_event_process(IAP_STREAM_DISCONNECT_EVENT, &iap_stream_param);
#endif
            }
        }
        break;

    case BT_EVENT_IAP_CTRL_MSG_IND:
        {
            APP_PRINT_INFO2("app_iap_bt_cback: BT_EVENT_IAP_CTRL_MSG_IND msg id 0x%04x, param len %d",
                            param->iap_ctrl_msg_ind.msg_id,
                            param->iap_ctrl_msg_ind.param_len);
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
        APP_PRINT_INFO2("app_iap_bt_cback: event_type 0x%04x, bd_addr %s", event_type,
                        TRACE_BDADDR(bd_addr));
    }
}

static void app_iap_sync_single_info(T_APP_IAP *p_info)
{
    APP_PRINT_TRACE5("app_iap_sync_single_info: evt 0x%04x, bd_addr %s, frame_size %d, credit %d, authen_flag %d",
                     p_info->bt_event, TRACE_BDADDR(p_info->bd_addr), p_info->frame_size, p_info->credit,
                     p_info->authen_flag);

    T_APP_IAP *exist_pinfo = app_iap_search_by_addr(p_info->bd_addr);

    switch (p_info->bt_event)
    {
    case BT_EVENT_IAP_CONN_IND:
    case BT_EVENT_IAP_CONN_CMPL:
        if (exist_pinfo == NULL)
        {
            exist_pinfo = calloc(1, sizeof(*exist_pinfo));
            memcpy(exist_pinfo, p_info, sizeof(*exist_pinfo));
            os_queue_in(&app_iap_mgr.queue, exist_pinfo);
        }
        break;

    case BT_EVENT_IAP_DISCONN_CMPL:
    case BT_EVENT_IAP_CONN_FAIL:
        os_queue_delete(&app_iap_mgr.queue, exist_pinfo);
        free(exist_pinfo);
        break;

    case BT_EVENT_IAP_AUTHEN_CMPL...BT_EVENT_IAP_DATA_SENT:
    case BT_EVENT_IAP_IDENTITY_INFO_REQ...BT_EVENT_IAP_CTRL_MSG_IND:
        if (exist_pinfo)
        {
            T_APP_IAP *tmp = exist_pinfo->p_next;
            memcpy(exist_pinfo, p_info, sizeof(*exist_pinfo));
            exist_pinfo->p_next = tmp;
        }
        break;

    default:
        break;
    }
}

static void app_iap_sync_all_info(T_REMOTE_INFO *p_rmt_info)
{
    uint32_t cnt = app_iap_mgr.queue.count;

    for (uint32_t i = 0; i < cnt; i++)
    {
        T_APP_IAP *p_iap = os_queue_out(&app_iap_mgr.queue);
        free(p_iap);
    }

    cnt = p_rmt_info->info_num;

    APP_PRINT_TRACE1("app_iap_sync_all_info: sync info num %d", cnt);

    for (uint32_t i = 0; i < cnt; i++)
    {
        T_APP_IAP *p_iap = calloc(1, sizeof(*p_iap));

        if (p_iap)
        {
            memcpy(p_iap, &p_rmt_info->infos[i], sizeof(*p_iap));
            os_queue_in(&app_iap_mgr.queue, p_iap);
        }
    }
}

#if F_APP_ERWS_SUPPORT
static void app_iap_parse_cback(uint8_t msg_type, T_REMOTE_INFO *p_rmt_info, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    APP_PRINT_TRACE1("app_iap_parse_cback: msg_type %d", msg_type);

    switch (msg_type)
    {
    case REMOTE_SINGLE_SYNC:
        app_iap_sync_single_info(&p_rmt_info->infos[0]);
        break;

    case REMOTE_SYNC_ALL:
        app_iap_sync_all_info(p_rmt_info);
        break;

    default:
        break;
    }
}
#endif

void app_iap_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY)
    {
        return;
    }

    T_REMOTE_INFO info = {0};
    T_APP_IAP *p_tmp_info = (T_APP_IAP *)app_iap_mgr.queue.p_first;

    for (; p_tmp_info != NULL; p_tmp_info = p_tmp_info->p_next)
    {
        memcpy(&info.infos[info.info_num], p_tmp_info, sizeof(info.infos[0]));
        info.info_num++;
    }

    APP_PRINT_TRACE1("app_iap_handle_remote_conn_cmpl: info_num %d", info.info_num);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_APP_IAP, REMOTE_SYNC_ALL,
                                        (uint8_t *)&info, sizeof(info));
}

bool app_iap_set_roleswap_info(T_APP_IAP *p_iap, uint8_t credit, uint16_t frame_size)
{
    if (p_iap)
    {
        APP_PRINT_TRACE2("app_iap_set_roleswap_info: new credit %d, frame_size %d", credit, frame_size);
        p_iap->credit = credit;
        p_iap->frame_size = frame_size;
        return true;
    }

    return false;
}

uint8_t app_iap_get_credit(T_APP_IAP *p_iap)
{
    if (p_iap)
    {
        return p_iap->credit;
    }

    return 0;
}

bool app_iap_is_authened(T_APP_IAP *p_iap)
{
    if (p_iap)
    {
        return p_iap->authen_flag;
    }

    return false;
}

uint16_t app_iap_get_frame_size(T_APP_IAP *p_iap)
{
    if (p_iap)
    {
        return p_iap->frame_size;
    }

    return 0;
}

void app_iap_init(void)
{
    os_queue_init(&app_iap_mgr.queue);

    if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
    {
        if (app_cfg_const.enable_multi_link)
        {
            bt_iap_init(2, RFC_IAP_CHANN_NUM, app_iap_cp_write, app_iap_cp_burst_read);//max support 2 iap links
        }
        else
        {
            bt_iap_init(1, RFC_IAP_CHANN_NUM, app_iap_cp_write, app_iap_cp_burst_read);
        }

        bt_mgr_cback_register(app_iap_bt_cback);

#if F_APP_ERWS_SUPPORT
        app_relay_cback_register(NULL, (P_APP_PARSE_CBACK)app_iap_parse_cback, APP_MODULE_TYPE_APP_IAP,
                                 MSG_MAX);
#endif
    }
}
#endif

