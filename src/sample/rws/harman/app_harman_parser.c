#if F_APP_HARMAN_FEATURE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "audio.h"
#include "app_audio_policy.h"
#include "app_main.h"
#include "app_harman_parser.h"
#include "app_relay.h"
#include "app_mmi.h"
#include "app_hfp.h"
#include "app_multilink.h"
#include "app_cfg.h"
#include "transmit_service.h"
#include "app_harman_vendor_cmd.h"
#include "app_cmd.h"

static void app_harman_cmd_set_handle(uint8_t *p_data, uint16_t *data_len, uint8_t id)
{
    uint16_t cmd_len = 0;
    T_HARMAN_CMD_HEADER cmd_header = {0};
    uint16_t data_len_temp = *data_len;

    while (data_len_temp >= HARMAN_CMD_HEADER_SIZE)
    {
        memcpy(&cmd_header, p_data, HARMAN_CMD_HEADER_SIZE);
        if ((cmd_header.sync_word == HARMAN_CMD_DIRECT_SEND) ||
            (cmd_header.sync_word == HARMAN_CMD_FORWARD))
        {
            cmd_len = cmd_header.payload_len + HARMAN_CMD_HEADER_SIZE;
            if (data_len_temp >= cmd_len)
            {
                if (app_harman_cmd_support_check(cmd_header.cmd_id, cmd_header.payload_len, id))
                {
                    app_harman_vendor_cmd_process(p_data, cmd_len, CMD_PATH_LE, 0, id);
                }
                data_len_temp -= cmd_len;
                p_data += cmd_len;
            }
            else
            {
                break;
            }
        }
        else
        {
            data_len_temp--;
            p_data++;
        }
    }
    *data_len = data_len_temp;
}

uint8_t app_harman_parser_process(uint8_t entry_point, void *msg)
{
    uint8_t ret = 0;
    uint8_t *rcv_msg = msg;

    switch (entry_point)
    {
    case HARMAN_PARSER_WRITE_CMD:
        {
            T_APP_LE_LINK *p_link;
            T_TRANSMIT_SRV_CALLBACK_DATA *p_callback = (T_TRANSMIT_SRV_CALLBACK_DATA *)msg;
            uint8_t         *p_data;
            uint16_t        data_len;
            uint16_t        total_len;

            p_data = p_callback->msg_data.rx_data.p_value;
            data_len = p_callback->msg_data.rx_data.len;
            APP_PRINT_INFO3("app_harman_parser_process: %d, len %d, data %b",
                            p_callback->attr_index,
                            p_callback->msg_data.rx_data.len,
                            TRACE_BINARY(data_len, p_data));
            p_link = app_link_find_le_link_by_conn_id(p_callback->conn_id);

            if (p_callback->msg_type == SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE)
            {
                if (p_callback->attr_index == TRANSMIT_SVC_RX_DATA_INDEX)
                {
                    if (p_link->p_embedded_cmd == NULL)
                    {
                        app_harman_cmd_set_handle(p_data, &data_len, p_link->id);

                        if (data_len)
                        {
                            p_link->p_embedded_cmd = malloc(data_len);
                            if (p_link->p_embedded_cmd != NULL)
                            {
                                memcpy(p_link->p_embedded_cmd, p_data, data_len);
                                p_link->embedded_cmd_len = data_len;
                            }
                            APP_PRINT_ERROR2("app_harman_parser_process: data_len: %d, embedded_cmd_len: %d",
                                             data_len, p_link->embedded_cmd_len);
                        }
                    }
                    else
                    {
                        uint8_t *p_temp;
                        uint16_t cmd_len;

                        p_temp = p_link->p_embedded_cmd;
                        total_len = p_link->embedded_cmd_len + data_len;
                        p_link->p_embedded_cmd = malloc(total_len);
                        if (p_link->p_embedded_cmd != NULL)
                        {
                            memcpy(p_link->p_embedded_cmd, p_temp, p_link->embedded_cmd_len);
                            free(p_temp);
                            memcpy(p_link->p_embedded_cmd + p_link->embedded_cmd_len, p_data, data_len);
                            p_link->embedded_cmd_len = total_len;
                            data_len = total_len;
                        }
                        else
                        {
                            p_link->p_embedded_cmd = p_temp;
                            data_len = p_link->embedded_cmd_len;
                        }
                        p_data = p_link->p_embedded_cmd;

                        //ios will auto combine two cmd into one pkt
                        app_harman_cmd_set_handle(p_data, &data_len, p_link->id);
                        p_link->embedded_cmd_len = data_len;
                        if (data_len && p_data != NULL)
                        {
                            p_temp = p_link->p_embedded_cmd;
                            p_link->p_embedded_cmd = malloc(data_len);
                            if (p_link->p_embedded_cmd != NULL)
                            {
                                memcpy(p_link->p_embedded_cmd, p_data, data_len);
                                p_link->embedded_cmd_len = data_len;
                                free(p_temp);
                            }
                        }
                    }
                }
                else if (p_callback->attr_index == TRANSMIT_SVC_DELAY_MODE_INDEX)
                {
                    //handle  p_data data_len here
                    APP_PRINT_INFO2("app_harman_parser_process: TRANSMIT_SVC_DELAY_MODE_INDEX: data_len: %d, data: %b",
                                    data_len, TRACE_BINARY(data_len, p_data));
                }
            }

            ret = 1;
        }
        break;

    default:
        break;
    }

    APP_PRINT_TRACE2("app_harman_parser_process: entry_point: 0x%x, ret: %d", entry_point, ret);

    return ret;
}
#endif
