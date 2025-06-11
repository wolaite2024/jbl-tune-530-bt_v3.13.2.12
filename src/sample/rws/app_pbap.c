/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_BT_PROFILE_PBAP_SUPPORT
#include <stdlib.h>
#include <string.h>
#include "os_mem.h"
#include "trace.h"
#include "app_timer.h"
#include "btm.h"
#include "remote.h"
#include "app_cfg.h"
#include "app_main.h"
#include "app_report.h"
#include "app_cmd.h"
#include "app_hfp.h"

static uint8_t app_pbap_timer_id = 0;

typedef enum
{
    APP_PBAP_TIMER_PBAP_PULL_CONTINUE,
} T_APP_PBAP_TIMER;

#if F_APP_PBAP_CMD_SUPPORT
//for CMD_PBAP_DOWNLOAD
typedef enum
{
    PBAP_DOWNLOAD_METHOD_NORMAL         = 0x01,
    PBAP_DOWNLOAD_METHOD_SET_RANGE      = 0x02,
    PBAP_DOWNLOAD_METHOD_ALL            = 0x03, //combined method for download the phonebook of ME/SM and CCH
    PBAP_DOWNLOAD_METHOD_ALL_PB         = 0x04, //combined method for download the phonebook of ME/SM
    PBAP_DOWNLOAD_METHOD_CCH            = 0x05, //The Combined Calls History (cch) contians incoming/outgoing/missed call history
} T_PBAP_DOWNLOAD_METHOD;

#define PBAP_DOWNLOAD_ME_PB                     0x07
#define PBAP_DOWNLOAD_SM_PB                     0x08

#define PBAP_DOWNLOAD_ME_PB_MASK                0x01 //bitmask for download ME (phone memory) phonebook
#define PBAP_DOWNLOAD_SM_PB_MASK                0x02 //bitmask for download SM (sim card) phonebook
#define PBAP_DOWNLOAD_CCH_MASK                  0x04 //bitmask for download the Combined Calls History (cch)

#define PBAP_PULL_CONTINUE_TIMER                200

//for CMD_PBAP_DOWNLOAD_CONTROL
typedef enum
{
    PBAP_DOWNLOAD_CONTROL_ABORT     = 0x01,
    PBAP_DOWNLOAD_CONTROL_SUSPEND   = 0x02,
    PBAP_DOWNLOAD_CONTROL_CONTINUE  = 0x03,
} T_PBAP_DOWNLOAD_CONTROL;

typedef struct
{
    uint8_t method;
    uint8_t storage;
    uint8_t repos;
    uint8_t phone_book;
    uint8_t download_flag;
    uint16_t browser_start;
    uint16_t browser_end;
    uint16_t list_count;
    uint64_t filter;
} T_PBAP_DOWNLOAD_INFO;

static uint8_t timer_idx_pbap_pull_continue = 0;

static bool enable_auto_pbap_download_continue_flag = true;
static T_PBAP_DOWNLOAD_INFO pbap_download_info;

bool app_pbap_get_auto_pbap_download_continue_flag(void)
{
    return enable_auto_pbap_download_continue_flag;
}

void app_pbap_pull_continue_timer_start(void)
{
    // start timer to prevent data from being overwritten in console_write
    app_start_timer(&timer_idx_pbap_pull_continue, "pbap_pull_continue",
                    app_pbap_timer_id, APP_PBAP_TIMER_PBAP_PULL_CONTINUE, 0, false,
                    PBAP_PULL_CONTINUE_TIMER);
}
#endif

static void app_pbap_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_PBAP_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->pbap_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                bt_pbap_phone_book_set(p_link->bd_addr, BT_PBAP_REPOSITORY_LOCAL, BT_PBAP_PATH_PB);
            }
        }
        break;

    case BT_EVENT_PBAP_CALLER_ID_NAME:
        {
            if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
            {
                T_APP_BR_LINK *br_link;
                T_APP_LE_LINK *le_link;

                br_link = app_link_find_br_link(param->pbap_caller_id_name.bd_addr);
                le_link = app_link_find_le_link_by_addr(param->pbap_caller_id_name.bd_addr);

                if (br_link != NULL)
                {
                    /* Sanity check if BR/EDR TTS session is ongoing */
                    if (br_link->cmd_set_enable == true &&
                        br_link->tts_info.tts_handle != NULL)
                    {
                        break;
                    }

                    /* Sanity check if BLE TTS session is ongoing */
                    if (le_link != NULL &&
                        le_link->cmd_set_enable == true &&
                        le_link->tts_info.tts_handle != NULL)
                    {
                        break;
                    }

                    if (param->pbap_caller_id_name.name_len)
                    {
                        if (br_link->call_id_type_check == true)
                        {
                            if (br_link->call_id_type_num == false)
                            {
                                uint8_t *data;
                                uint16_t len;

                                len = param->pbap_caller_id_name.name_len + 3;
                                data = malloc(len);
                                if (data != NULL)
                                {
                                    data[1] = CALLER_ID_TYPE_NAME;
                                    data[2] = param->pbap_caller_id_name.name_len;
                                    memcpy(data + 3, param->pbap_caller_id_name.name_ptr, param->pbap_caller_id_name.name_len);

                                    if (br_link->connected_profile & (SPP_PROFILE_MASK | IAP_PROFILE_MASK))
                                    {
                                        data[0] = br_link->id;

                                        app_report_event(CMD_PATH_SPP, EVENT_CALLER_ID, br_link->id, data, len);
                                    }
                                    else if (le_link != NULL)
                                    {
                                        data[0] = le_link->id;

                                        app_report_event(CMD_PATH_LE, EVENT_CALLER_ID, le_link->id, data, len);
                                    }

                                    free(data);
                                }
                            }
                        }
                    }
                    else
                    {
                        br_link->call_id_type_check = false;
                        br_link->call_id_type_num = true;
                    }
                }
            }
        }
        break;

    case BT_EVENT_SDP_ATTR_INFO:
        {
            T_BT_SDP_ATTR_INFO *sdp_info = &param->sdp_attr_info.info;

            if (sdp_info->srv_class_uuid_data.uuid_16 == UUID_PBAP_PSE)
            {
                T_APP_BR_LINK *p_link;

                p_link = app_link_find_br_link(param->sdp_attr_info.bd_addr);
                if (p_link != NULL)
                {
                    p_link->pbap_repos = sdp_info->supported_repos;
                }

#if F_APP_DEVICE_CMD_SUPPORT
                if (app_cmd_get_report_attr_info_flag())
                {
                    uint8_t temp_buff[8];

                    temp_buff[0] = GET_PBAP_ATTR_INFO;
                    memcpy(&temp_buff[1], &(sdp_info->profile_version), 2);
                    temp_buff[3] = sdp_info->supported_repos;
                    memcpy(&temp_buff[4], &(sdp_info->pbap_supported_feat), 4);

                    app_cmd_set_report_attr_info_flag(false);
                    app_report_event(CMD_PATH_UART, EVENT_REPORT_REMOTE_DEV_ATTR_INFO, 0, temp_buff,
                                     sizeof(temp_buff));
                }
#endif
            }
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_pbap_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_pbap_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
#if F_APP_PBAP_CMD_SUPPORT
    case APP_PBAP_TIMER_PBAP_PULL_CONTINUE:
        {
            uint8_t app_index = app_hfp_get_active_idx();

            app_stop_timer(&timer_idx_pbap_pull_continue);
            bt_pbap_pull_continue(app_db.br_link[app_index].bd_addr);
        }
        break;
#endif

    default:
        break;
    }
}

#if F_APP_PBAP_CMD_SUPPORT
// split pbap data with "END:VCARD<CR><LF>"
void app_pbap_split_pbap_data(uint8_t *p_data, uint16_t data_len)
{
    if (p_data != NULL)
    {
        char *data_ptr = (char *)p_data;
        char *temp_ptr = NULL;
        char *sp_end = "END:VCARD";
        char *sp_begin = "BEGIN:VCARD";
        uint16_t temp_len;
        uint16_t report_data_len = 0;
        bool check_flag = true;

        temp_ptr = strstr(data_ptr, sp_end);

        while (report_data_len < data_len)
        {
            check_flag = true;
            if (temp_ptr != NULL)
            {
                // check if the data_ptr is begin with "BEGIN:VCARD"
                if (strncmp(data_ptr, sp_begin, strlen(sp_begin)))  //is not begin with sp_begin
                {
                    char *temp_pos = strstr(data_ptr, sp_begin);   //find sp_begin

                    if ((temp_pos != NULL) && (temp_pos < temp_ptr))
                    {
                        temp_ptr = strstr(data_ptr, sp_begin);    //split if the data_ptr is not begin with "BEGIN:VCARD"
                        temp_len = temp_ptr - data_ptr;
                        check_flag = false;
                    }
                }

                if (check_flag)
                {
                    temp_len = temp_ptr - data_ptr + strlen(sp_end) + 2;

                    //if end with "END:VCARD<CR>", cannot plus 2
                    if ((report_data_len + temp_len) > data_len)
                    {
                        temp_len = temp_len - (report_data_len + temp_len - data_len);
                    }
                }
            }
            else
            {
                char *temp_begin = strstr(data_ptr, sp_begin);  //find sp_begin

                if ((temp_begin != NULL) &&
                    (temp_begin != data_ptr))          //can not find sp_end and this is first packet data
                {
                    //split data again
                    temp_ptr = strstr(data_ptr, sp_begin);
                    temp_len = temp_ptr - data_ptr;        //split if the data_ptr is not begin with "BEGIN:VCARD"
                    check_flag = false;
                }
                else
                {
                    temp_len = data_len - report_data_len; // send the rest of data
                }
            }

            uint8_t temp_buff[temp_len + 2];

            memcpy(&temp_buff[0], &(temp_len), 2);
            memcpy(&temp_buff[2], (uint8_t *)data_ptr, temp_len);
            app_report_event(CMD_PATH_UART, EVENT_PBAP_REPORT_DATA, 0, temp_buff,
                             sizeof(temp_buff));

            report_data_len += temp_len;

            if (temp_ptr != NULL)
            {
                //data_ptr point to next sp_begin
                if (check_flag)
                {
                    data_ptr = temp_ptr + strlen(sp_end) + 2;
                }
                else
                {
                    data_ptr = temp_ptr;
                }

                //temp_ptr point to next sp_end
                temp_ptr = strstr(data_ptr, sp_end);
            }
        }
    }
}

// for CMD_PBAP_DOWNLOAD combined methods
static void app_pbap_download_info_set_default(void)
{
    enable_auto_pbap_download_continue_flag = true;
    pbap_download_info.method = 0x00;
    pbap_download_info.storage = PBAP_DOWNLOAD_ME_PB;
    pbap_download_info.repos = BT_PBAP_REPOSITORY_LOCAL;
    pbap_download_info.phone_book = BT_PBAP_PHONE_BOOK_PB;
    pbap_download_info.download_flag = 0x00;
    pbap_download_info.browser_start = 0x00;
    pbap_download_info.browser_end = 0x00;
    pbap_download_info.list_count = 0xFFFF;
    pbap_download_info.filter = 0;
}

// if set pbap download method, then send cmd to get pb data
// else only report pb size
void app_pbap_cmd_pbap_download(uint8_t *bd_addr, uint16_t pb_size)
{
    if (pbap_download_info.method)
    {
        if (pbap_download_info.browser_end == 0x00)
        {
            memcpy(&pbap_download_info.browser_end, &pb_size, 2);
        }

        uint8_t temp_buff[17];

        temp_buff[0] = pbap_download_info.storage;
        memcpy(&temp_buff[1], &pb_size, 2);// total pb_size
        memcpy(&temp_buff[3], &pbap_download_info.browser_start, 2);
        memcpy(&temp_buff[5], &pbap_download_info.browser_end, 2);// report current download range
        memcpy(&temp_buff[7], &pbap_download_info.filter, 4);
        memcpy(&temp_buff[11], bd_addr, 6);

        app_report_event(CMD_PATH_UART, EVENT_PBAP_DOWNLOAD_START, 0, temp_buff, sizeof(temp_buff));

        bt_pbap_phone_book_pull(bd_addr, (T_BT_PBAP_REPOSITORY)pbap_download_info.repos,
                                (T_BT_PBAP_PHONE_BOOK)pbap_download_info.phone_book, pbap_download_info.browser_start,
                                pbap_download_info.list_count, pbap_download_info.filter);

        pbap_download_info.browser_end = 0x00;
    }
    else
    {
        uint8_t temp_buff[9];

        temp_buff[0] = pbap_download_info.storage;
        memcpy(&temp_buff[1], &pb_size, 2);// total pb_size
        memcpy(&temp_buff[3], bd_addr, 6);

        app_report_event(CMD_PATH_UART, EVENT_PBAP_REPORT_SIZE, 0, temp_buff, sizeof(temp_buff));
    }
}

void app_pbap_cmd_pbap_download_check(uint8_t *bd_addr)
{
    if (pbap_download_info.method)
    {
        uint8_t temp_buff[7];
        bool flag = true;

        temp_buff[0] = pbap_download_info.storage;
        memcpy(&temp_buff[1], bd_addr, 6);
        app_report_event(CMD_PATH_UART, EVENT_PBAP_DOWNLOAD_CMPL, 0, temp_buff, sizeof(temp_buff));

        if (pbap_download_info.download_flag & PBAP_DOWNLOAD_SM_PB_MASK)
        {
            pbap_download_info.storage = PBAP_DOWNLOAD_SM_PB;
            pbap_download_info.repos = BT_PBAP_REPOSITORY_SIM1;
            pbap_download_info.phone_book = BT_PBAP_PHONE_BOOK_PB;
            pbap_download_info.download_flag &= ~PBAP_DOWNLOAD_SM_PB_MASK;
        }
        else if (pbap_download_info.download_flag & PBAP_DOWNLOAD_CCH_MASK)
        {
            pbap_download_info.storage = BT_PBAP_PHONE_BOOK_CCH;
            pbap_download_info.repos = BT_PBAP_REPOSITORY_LOCAL;
            pbap_download_info.phone_book = BT_PBAP_PHONE_BOOK_CCH;
            pbap_download_info.download_flag &= ~PBAP_DOWNLOAD_CCH_MASK;
        }
        else
        {
            flag = false;
            app_pbap_download_info_set_default();
        }

        if (flag)
        {
            bt_pbap_phone_book_size_get(bd_addr, (T_BT_PBAP_REPOSITORY)pbap_download_info.repos,
                                        (T_BT_PBAP_PHONE_BOOK)pbap_download_info.phone_book);
        }
    }
}

void app_pbap_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                         uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    uint8_t active_hf_idx = app_hfp_get_active_idx();

    switch (cmd_id)
    {
    case CMD_PBAP_DOWNLOAD:
        {
            app_pbap_download_info_set_default();

            switch (cmd_ptr[2])
            {
            case PBAP_DOWNLOAD_METHOD_NORMAL:
            case PBAP_DOWNLOAD_METHOD_SET_RANGE:
                {
                    /*cmd_ptr[7] refer to T_BT_PBAP_PHONE_BOOK
                    0x01 ~ 0x06 for download ich ~ fav
                    0x07 for download ME (phone memory) phonebook
                    0x08 for download SM (sim card) phonebook
                    cause pb download needs distinguish two paths*/
                    pbap_download_info.storage = cmd_ptr[7];

                    if ((cmd_ptr[7] >= BT_PBAP_PHONE_BOOK_ICH) && (cmd_ptr[7] <= BT_PBAP_PHONE_BOOK_FAV))
                    {
                        pbap_download_info.phone_book = cmd_ptr[7];
                    }
                    else if (cmd_ptr[7] == PBAP_DOWNLOAD_ME_PB)
                    {
                    }
                    else if (cmd_ptr[7] == PBAP_DOWNLOAD_SM_PB)
                    {
                        pbap_download_info.repos = BT_PBAP_REPOSITORY_SIM1;
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                        break;
                    }

                    if (cmd_ptr[2] == PBAP_DOWNLOAD_METHOD_SET_RANGE)
                    {
                        pbap_download_info.browser_start = (uint16_t)(cmd_ptr[8] | (cmd_ptr[9] << 8));
                        pbap_download_info.browser_end = (uint16_t)(cmd_ptr[10] | (cmd_ptr[11] << 8));
                        pbap_download_info.list_count =  pbap_download_info.browser_end - pbap_download_info.browser_start +
                                                         1;
                    }
                }
                break;

            case PBAP_DOWNLOAD_METHOD_ALL:
                {
                    pbap_download_info.download_flag |= (PBAP_DOWNLOAD_ME_PB_MASK | PBAP_DOWNLOAD_SM_PB_MASK |
                                                         PBAP_DOWNLOAD_CCH_MASK);
                }
                break;

            case PBAP_DOWNLOAD_METHOD_ALL_PB:
                {
                    pbap_download_info.download_flag |= (PBAP_DOWNLOAD_ME_PB_MASK | PBAP_DOWNLOAD_SM_PB_MASK);
                }
                break;

            case PBAP_DOWNLOAD_METHOD_CCH:
                {
                    pbap_download_info.storage = BT_PBAP_PHONE_BOOK_CCH;
                    pbap_download_info.phone_book = BT_PBAP_PHONE_BOOK_CCH;
                }
                break;

            default:
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                break;
            }

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                pbap_download_info.method = cmd_ptr[2];
                pbap_download_info.filter = (uint64_t)(cmd_ptr[3] | (cmd_ptr[4] << 8) | (cmd_ptr[5] << 16) |
                                                       (cmd_ptr[6] << 24));

                if (bt_pbap_phone_book_size_get(app_db.br_link[active_hf_idx].bd_addr,
                                                (T_BT_PBAP_REPOSITORY)pbap_download_info.repos,
                                                (T_BT_PBAP_PHONE_BOOK)pbap_download_info.phone_book) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_PBAP_DOWNLOAD_CONTROL:
        {
            switch (cmd_ptr[2])
            {
            case PBAP_DOWNLOAD_CONTROL_ABORT:
                {
                    if (bt_pbap_pull_abort(app_db.br_link[active_hf_idx].bd_addr) == false)
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                    else
                    {
                        app_stop_timer(&timer_idx_pbap_pull_continue);
                    }
                }
                break;

            case PBAP_DOWNLOAD_CONTROL_SUSPEND:
                {
                    enable_auto_pbap_download_continue_flag = false;
                }
                break;

            case PBAP_DOWNLOAD_CONTROL_CONTINUE:
                {
                    if (bt_pbap_pull_continue(app_db.br_link[active_hf_idx].bd_addr))
                    {
                        enable_auto_pbap_download_continue_flag = true;
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
                break;

            default:
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                break;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                uint8_t temp_buff[1];

                temp_buff[0] = cmd_ptr[2];
                app_report_event(cmd_path, EVENT_PBAP_REPORT_SESSION_STATUS, app_idx, temp_buff, sizeof(temp_buff));
            }
        }
        break;

    case CMD_PBAP_DOWNLOAD_GET_SIZE:
        {
            app_pbap_download_info_set_default();

            pbap_download_info.storage = cmd_ptr[2];

            if ((cmd_ptr[2] >= BT_PBAP_PHONE_BOOK_ICH) && (cmd_ptr[2] <= BT_PBAP_PHONE_BOOK_FAV))
            {
                pbap_download_info.phone_book = cmd_ptr[2];
            }
            else if (cmd_ptr[2] == PBAP_DOWNLOAD_ME_PB)
            {
            }
            else if (cmd_ptr[2] == PBAP_DOWNLOAD_SM_PB)
            {
                pbap_download_info.repos = BT_PBAP_REPOSITORY_SIM1;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                if (bt_pbap_phone_book_size_get(app_db.br_link[active_hf_idx].bd_addr,
                                                (T_BT_PBAP_REPOSITORY)pbap_download_info.repos,
                                                (T_BT_PBAP_PHONE_BOOK)pbap_download_info.phone_book) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    default:
        break;
    }
}
#endif

void app_pbap_init(void)
{
    if (app_cfg_const.supported_profile_mask & PBAP_PROFILE_MASK)
    {
        bt_pbap_init(app_cfg_const.pbap_link_number);
        bt_mgr_cback_register(app_pbap_bt_cback);
        app_timer_reg_cb(app_pbap_timeout_cb, &app_pbap_timer_id);
    }
}
#endif
