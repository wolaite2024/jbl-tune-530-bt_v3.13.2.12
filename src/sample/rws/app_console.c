
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "os_mem.h"
#include "os_msg.h"
#include "app_console.h"
#include "app_cfg.h"
#include "app_io_msg.h"
#include "console_uart.h"
#include "app_io_msg.h"
#include "console.h"
#include "app_cmd.h"
#include "app_util.h"
#include "app_main.h"
#include "trace.h"


#include "cli_power.h"
#if F_APP_CLI_CFG_SUPPORT
#include "cli_cfg.h"
#endif
#if F_APP_CLI_STRING_MP_SUPPORT
#include "cli_mp.h"
#endif
#if F_APP_ONE_WIRE_UART_SUPPORT
#include "app_one_wire_uart.h"
#endif
#if F_APP_BQB_CLI_SUPPORT
#include "bqb.h"
#endif
#if F_APP_SS_SUPPORT
#include "app_ss_cmd.h"
#endif
#if ISOC_TEST_SUPPORT
#include "cli_isoc.h"
#endif
#if F_APP_LEA_SUPPORT
#include "cli_le_audio.h"
#endif

#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_device.h"
#endif

#define CONSOLE_TX_BUFFER_LARGE         512
#define CONSOLE_RX_BUFFER_LARGE         512
#define CONSOLE_TX_BUFFER_SMALL         256
#define CONSOLE_RX_BUFFER_SMALL         256

#define CONSOLE_PARSER_RX_BUFFER_SIZE  512

static void app_console_handle_wake_up(T_CONSOLE_UART_EVENT event)
{
    T_IO_MSG dlps_msg;

    dlps_msg.type = IO_MSG_TYPE_GPIO;
    dlps_msg.subtype = IO_MSG_GPIO_UART_WAKE_UP;

    /* Send MSG to APP task */
    app_io_msg_send(&dlps_msg);
}

void app_console_uart_driver_init(void)
{
    console_uart_driver_init();
}

static void app_console_uart_init(void)
{
#if F_APP_CONSOLE_SUPPORT
    T_CONSOLE_PARAM console_param;
    T_CONSOLE_OP console_op;
    T_CONSOLE_UART_CONFIG console_uart_config;

    console_uart_config.one_wire_uart_support = app_cfg_const.one_wire_uart_support;
    console_uart_config.uart_tx_pinmux = console_uart_config.one_wire_uart_support ?
                                         app_cfg_const.one_wire_uart_data_pinmux : app_cfg_const.data_uart_tx_pinmux;
    console_uart_config.uart_rx_pinmux = app_cfg_const.data_uart_rx_pinmux;
    console_uart_config.rx_wake_up_pinmux = app_cfg_const.rx_wake_up_pinmux;
    console_uart_config.enable_rx_wake_up = app_cfg_const.enable_rx_wake_up;
    console_uart_config.data_uart_baud_rate = app_cfg_const.data_uart_baud_rate;
    console_uart_config.callback = app_console_handle_wake_up;

#if (TARGET_RTL8773DO == 1 || TARGET_RTL8773DFL == 1)
    console_uart_config.uart_rx_dma_enable = true;
#else
    console_uart_config.uart_rx_dma_enable = false;
#endif
    console_uart_config.uart_tx_dma_enable = false;

    console_param.tx_buf_size = CONSOLE_TX_BUFFER_SMALL;
    console_param.rx_buf_size = CONSOLE_RX_BUFFER_SMALL;
    console_param.tx_wakeup_pin = app_cfg_const.tx_wake_up_pinmux;
    console_param.rx_wakeup_pin = app_cfg_const.rx_wake_up_pinmux;

    console_op.init = console_uart_init;
    console_op.tx_wakeup_enable = NULL; //console_uart_tx_wakeup_enable;
    console_op.rx_wakeup_enable = NULL; //console_uart_rx_wakeup_enable;
    console_op.write = console_uart_write;
    console_op.wakeup = console_uart_wakeup;
    console_op.tx_empty = NULL;

    console_uart_config_init(&console_uart_config);
    console_init(&console_param, &console_op);
    console_set_mode((T_CONSOLE_MODE)app_cfg_const.console_mode);
#endif
}

bool app_console_send_msg(T_IO_CONSOLE subtype, void *param_buf)
{
    T_IO_MSG msg;

    msg.type    = IO_MSG_TYPE_CONSOLE;
    msg.subtype = subtype;
    msg.u.buf   = param_buf;

    return app_io_msg_send(&msg);
}

typedef struct
{
    uint16_t            rx_count;
    uint16_t            rx_process_offset;
    uint8_t             *rx_buffer;
    uint32_t            rx_w_idx;
    uint32_t            rx_buf_size;
} T_CONSOLE_PARSER;

T_CONSOLE_PARSER console_parser;

bool app_console_cmd_set_parser(uint8_t *buf, uint32_t len)
{
    uint16_t cmd_len;
    uint16_t temp_index;
    //uint8_t rx_seqn;

    if (console_parser.rx_w_idx + len <= console_parser.rx_buf_size)
    {
        memcpy(&console_parser.rx_buffer[console_parser.rx_w_idx], buf, len);
        console_parser.rx_count += len;
        console_parser.rx_w_idx += len;
        if (console_parser.rx_w_idx == console_parser.rx_buf_size)
        {
            console_parser.rx_w_idx = 0;
        }
    }
    else
    {
        uint32_t temp;

        temp = console_parser.rx_buf_size - console_parser.rx_w_idx;
        memcpy(&console_parser.rx_buffer[console_parser.rx_w_idx], buf, temp);
        memcpy(&console_parser.rx_buffer[0], &buf[temp], len - temp);
        console_parser.rx_count += len;
        console_parser.rx_w_idx  = len - temp;
    }


    while (console_parser.rx_count)
    {
        if (console_parser.rx_buffer[console_parser.rx_process_offset] == CMD_SYNC_BYTE)
        {
            temp_index = console_parser.rx_process_offset;
            //sync_byte(1) and Seqn(1) and length(2) and cmd id(2) and checksum(1)
            if (console_parser.rx_count >= 7)
            {
                temp_index++;
                if (temp_index >= CONSOLE_PARSER_RX_BUFFER_SIZE)
                {
                    temp_index = 0;
                }
                //rx_seqn = console_parser.rx_buffer[temp_index];
                temp_index++;
                if (temp_index >= CONSOLE_PARSER_RX_BUFFER_SIZE)
                {
                    temp_index = 0;
                }
                cmd_len = console_parser.rx_buffer[temp_index];
                temp_index++;
                if (temp_index >= CONSOLE_PARSER_RX_BUFFER_SIZE)
                {
                    temp_index = 0;
                }
                cmd_len |= (console_parser.rx_buffer[temp_index] << 8);
                //only process when received whole command
                if (console_parser.rx_count >= (cmd_len + 5))
                {
                    uint8_t     *temp_ptr;
                    uint8_t     check_sum;

                    temp_ptr = os_mem_alloc(RAM_TYPE_DATA_ON, (cmd_len + 5));
                    if (temp_ptr != NULL)
                    {
                        if ((console_parser.rx_process_offset + cmd_len + 5) > CONSOLE_PARSER_RX_BUFFER_SIZE)
                        {
                            uint16_t    temp_len;

                            temp_len = CONSOLE_PARSER_RX_BUFFER_SIZE - console_parser.rx_process_offset;
                            memcpy(temp_ptr, &console_parser.rx_buffer[console_parser.rx_process_offset], temp_len);
                            temp_index = temp_len;
                            temp_len = (cmd_len + 5) - (CONSOLE_PARSER_RX_BUFFER_SIZE - console_parser.rx_process_offset);
                            memcpy(&temp_ptr[temp_index], &console_parser.rx_buffer[0], temp_len);
                        }
                        else
                        {
                            memcpy(temp_ptr, &console_parser.rx_buffer[console_parser.rx_process_offset],
                                   (cmd_len + 5));
                        }
                        check_sum = app_util_calc_checksum(&temp_ptr[1], cmd_len + 3);//from seqn
                        if (check_sum == temp_ptr[cmd_len + 4])
                        {
                            app_console_send_msg(IO_MSG_CONSOLE_BINARY_RX, temp_ptr);
                        }
                        else
                        {
                            APP_PRINT_ERROR2("app_console_cmd_set_parser: checksum error calc check_sum 0x%x, rx check_sum 0x%x",
                                             check_sum, temp_ptr[cmd_len + 4]);
                            os_mem_free(temp_ptr);
                        }
                    }
                    console_parser.rx_count -= (cmd_len + 5);
                    console_parser.rx_process_offset += (cmd_len + 5);
                    if (console_parser.rx_process_offset >= CONSOLE_PARSER_RX_BUFFER_SIZE)
                    {
                        console_parser.rx_process_offset -= CONSOLE_PARSER_RX_BUFFER_SIZE;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            console_parser.rx_count--;
            console_parser.rx_process_offset++;
            if (console_parser.rx_process_offset >= CONSOLE_PARSER_RX_BUFFER_SIZE)
            {
                console_parser.rx_process_offset = 0;
            }
        }
    }
    return true;
}

bool app_console_parser_register(void)
{
    return console_register_parser(&app_console_cmd_set_parser);
}

void app_console_parser_init(void)
{
    console_parser.rx_buffer = os_mem_alloc(RAM_TYPE_DATA_ON, CONSOLE_PARSER_RX_BUFFER_SIZE);
    console_parser.rx_buf_size = CONSOLE_PARSER_RX_BUFFER_SIZE;
    console_parser.rx_count = 0;
    console_parser.rx_process_offset = 0;
    console_parser.rx_w_idx = 0;
}

void app_console_init(void)
{
#if F_APP_CONSOLE_SUPPORT
    app_console_uart_init();

#if F_APP_ONE_WIRE_UART_SUPPORT
    if (app_cfg_const.one_wire_uart_support)
    {
        app_one_wire_uart_init();
    }
#endif

#if F_APP_LEA_SUPPORT
    le_audio_cmd_register();
#endif

#if F_APP_BQB_CLI_SUPPORT
    bqb_cmd_register();
#endif

    power_cmd_register();

    app_console_parser_init();
#if F_APP_TUYA_SUPPORT
    if (extend_app_cfg_const.tuya_support)
    {
        tuya_cmd_register();
        console_set_mode(CONSOLE_MODE_BINARY);
    }
#else
    app_console_parser_register();
#endif

#if F_APP_CLI_CFG_SUPPORT
    cfg_cmd_register();
#endif

#if F_APP_CLI_STRING_MP_SUPPORT
    mp_cmd_str_register();
#endif

#if ISOC_TEST_SUPPORT
    isoc_cmd_register();
#endif

#if F_APP_SS_SUPPORT
    app_ss_cmd_register();
#endif
#endif
}
