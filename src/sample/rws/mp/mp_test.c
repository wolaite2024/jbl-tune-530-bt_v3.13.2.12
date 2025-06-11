/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CLI_BINARY_MP_SUPPORT
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "os_mem.h"
#include "os_msg.h"
#include "os_sched.h"
#include "rtl876x_uart.h"
#include "trace.h"
#include "gap.h"
#include "gap_vendor.h"
#include "gap_br.h"
#include "app_mmi.h"
#include "mp_test.h"
#include "mp_test_vendor.h"
#include "test_mode.h"
#include "fmc_api.h"
#include "app_cmd.h"
#include "app_report.h"
#include "app_transfer.h"
#include "app_main.h"
#include "app_cfg.h"

extern bool h2d_cmd_send(uint8_t *buffer, uint16_t length, bool flush);

static bool g_mp_test_mode = false;
uint8_t mp_app_index = 0;
uint8_t mp_cmd_path = 0;

typedef enum _IOCTL_RESULT
{
    RESULT_FAILURE = 0,
    RESULT_SUCCESS = 1
} IOCTL_RESULT;

bool mp_test_get_mode_flag(void)
{
    return g_mp_test_mode;
}

void mp_test_set_mode_flag(bool flag)
{
    g_mp_test_mode = flag;
}

void mp_test_send_vnd_cmd_cmpl_evt(void *p_gap_vnd_cmd_cmpl_evt_rsp)
{
    uint8_t *p_head = NULL;

    uint8_t result      = MP_TEST_RESULT_PASS;
    uint8_t opcode      = MP_TEST_OPCODE_RTK;
    uint8_t sub_opcode  = MP_TEST_RTK_SUB_OPCODE_VENDOR_CMD;
    uint8_t *p_param    = NULL;
    uint16_t param_len  = 0;

    T_GAP_VND_CMD_CMPL_EVT_RSP *p_vnd_cmd_cmpl_rsp = (T_GAP_VND_CMD_CMPL_EVT_RSP *)
                                                     p_gap_vnd_cmd_cmpl_evt_rsp;

    param_len = sizeof(p_vnd_cmd_cmpl_rsp->command) + sizeof(p_vnd_cmd_cmpl_rsp->cause) +
                sizeof(p_vnd_cmd_cmpl_rsp->is_cmpl_evt) + sizeof(p_vnd_cmd_cmpl_rsp->param_len) +
                p_vnd_cmd_cmpl_rsp->param_len;

    p_head = os_mem_alloc(RAM_TYPE_DATA_ON,  param_len);
    if (p_head != NULL)
    {
        p_param = p_head;
        LE_UINT16_TO_STREAM(p_param, p_vnd_cmd_cmpl_rsp->command);
        LE_UINT16_TO_STREAM(p_param, p_vnd_cmd_cmpl_rsp->cause);
        LE_UINT8_TO_STREAM(p_param, p_vnd_cmd_cmpl_rsp->is_cmpl_evt);
        LE_UINT8_TO_STREAM(p_param, p_vnd_cmd_cmpl_rsp->param_len);
        memcpy(p_param, p_vnd_cmd_cmpl_rsp->p_param, p_vnd_cmd_cmpl_rsp->param_len);
    }
    else
    {
        result = MP_TEST_RESULT_FAIL_NO_MEMORY;
    }

    mp_test_send_test_result(mp_app_index, mp_cmd_path, result, opcode, sub_opcode, p_head, param_len);

    if (p_head)
    {
        os_mem_free(p_head);
    }
}



void mp_test_send_test_result(uint8_t app_idx, uint8_t cmd_path, uint8_t result,
                              uint8_t opcode, uint8_t sub_opcode, uint8_t *p_param, uint16_t param_len)
{
    uint16_t len_event_buf = sizeof(result) + sizeof(opcode) + sizeof(sub_opcode) + param_len;
    uint8_t *event_buff = os_mem_alloc(RAM_TYPE_DATA_ON,  len_event_buf);

    APP_PRINT_INFO5("mp_test_send_test_result: result %d, event_buff %p, cmd_path %d, opcode 0x%x, sub_opcode 0x%x",
                    result, event_buff, cmd_path, opcode, sub_opcode);

    if (event_buff == NULL)
    {
        return;
    }

    event_buff[0] = result;
    event_buff[1] = opcode;
    event_buff[2] = sub_opcode;
    if (p_param != NULL)
    {
        memcpy(&event_buff[3], p_param, param_len);
    }

    app_report_event(cmd_path, EVENT_MP_TEST_RESULT, app_idx, event_buff, len_event_buf);

    os_mem_free(event_buff);
}

static bool mp_test_handle_rtk_cmd(uint8_t app_idx, uint8_t cmd_path, uint8_t sub_opcode,
                                   uint8_t *p_param, uint16_t param_len)
{
    bool handle = true;
    uint8_t result = MP_TEST_RESULT_PASS;
    uint8_t *p_head = NULL;
    uint16_t param_total_len = 0;

    APP_PRINT_INFO2("mp_test_handle_rtk_cmd: sub_opcode 0x%x, param_len %d", sub_opcode,
                    param_len);

    if (sub_opcode == MP_TEST_RTK_SUB_OPCODE_ENTER_TEST_MODE)
    {
        if (param_len != 0)
        {
            result = MP_TEST_RESULT_FAIL_INVALID_LEN;
        }
        else
        {
            mp_test_set_mode_flag(true);
        }
    }
    else if (sub_opcode == MP_TEST_RTK_SUB_OPCODE_CHANGE_BAUD_RATE)
    {
        uint8_t baud_rate = 0;
        if (param_len != sizeof(baud_rate))
        {
            result = MP_TEST_RESULT_FAIL_INVALID_LEN;
        }
        else
        {
            LE_STREAM_TO_UINT8(baud_rate, p_param);

            mp_test_send_test_result(app_idx, cmd_path, result, MP_TEST_OPCODE_RTK,
                                     sub_opcode, p_head, param_total_len);
            os_delay(10);
            UART_SetBaudRate(UART0, (UartBaudRate_TypeDef)baud_rate);
            handle = false;
        }

    }
    else
    {
        if (mp_test_get_mode_flag())
        {
            switch (sub_opcode)
            {
            case MP_TEST_RTK_SUB_OPCODE_EXIT_TEST_MODE:
                {
                    if (param_len != 0)
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    mp_test_send_test_result(app_idx, cmd_path, MP_TEST_RESULT_PASS, MP_TEST_OPCODE_RTK,
                                             sub_opcode, NULL, 0);
                    os_delay(500);
                    gap_br_send_acl_disconn_req(app_db.br_link[app_idx].bd_addr);
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_SWITCH_TO_HCI_MODE:
                {
                    if (param_len != 0)
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }
                    switch_into_hci_mode();
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_SWITCH_TO_SINGLE_TONE_MODE:
                {
                    uint32_t timeout_val = 0;
                    uint8_t channel = 0;
                    uint8_t tx_gain  = 0;

                    if (param_len != sizeof(timeout_val) + sizeof(channel) + sizeof(tx_gain))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    LE_STREAM_TO_UINT32(timeout_val, p_param);
                    LE_STREAM_TO_UINT8(channel, p_param);
                    LE_STREAM_TO_UINT8(tx_gain, p_param);

                    if (timeout_val == 0)
                    {
                        result = MP_TEST_RESULT_FAIL;
                        break;
                    }

                    app_cfg_nv.single_tone_timeout_val = timeout_val;
                    app_cfg_nv.single_tone_channel_num = channel;
                    app_cfg_nv.single_tone_tx_gain = tx_gain;
                    app_cfg_store(&app_cfg_nv.single_tone_timeout_val, 8);

                    mp_test_send_test_result(app_idx, cmd_path, MP_TEST_RESULT_PASS, MP_TEST_OPCODE_RTK,
                                             sub_opcode, NULL, 0);
                    os_delay(10);
                    switch_into_single_tone_test_mode();

                }
                break;


            case MP_TEST_RTK_SUB_OPCODE_FACTORY_RESET:
                {
                    if (param_len != 0)
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }
                    app_mmi_handle_action(MMI_DEV_FACTORY_RESET);
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_FLASH_READ_BUFFER:
                {
                    bool ret = false;
                    uint32_t start_addr = 0;
                    uint32_t data_len = 0;
                    uint8_t *p_buf = NULL;

                    if (param_len != sizeof(start_addr) + sizeof(data_len))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    LE_STREAM_TO_UINT32(start_addr, p_param);
                    LE_STREAM_TO_UINT32(data_len, p_param);

                    APP_PRINT_INFO2("mp_test_handle_rtk_cmd: flash read start_addr 0x%x, data_len %d", start_addr,
                                    data_len);

                    param_total_len = sizeof(data_len) + data_len;
                    p_buf = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(data_len) + data_len);
                    if (p_buf != NULL)
                    {
                        p_head = p_buf;

                        LE_UINT32_TO_STREAM(p_buf, data_len);

                        ret = fmc_flash_nor_read(start_addr, p_buf, data_len);
                        if (ret == false)
                        {
                            result = MP_TEST_RESULT_FAIL_FLASH_OP_FAIL;
                        }
                    }
                    else
                    {
                        result = MP_TEST_RESULT_FAIL_NO_MEMORY;
                    }
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_FLASH_WRITE_BUFFER:
                {
                    bool ret = false;
                    uint32_t start_addr;
                    uint32_t data_len;
                    uint8_t *data;

                    if (param_len < sizeof(start_addr) + sizeof(data_len))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    LE_STREAM_TO_UINT32(start_addr, p_param);
                    LE_STREAM_TO_UINT32(data_len, p_param);

                    APP_PRINT_INFO2("mp_test_handle_rtk_cmd: flash write start_addr 0x%x, data_len %d", start_addr,
                                    data_len);

                    if (param_len != (data_len + sizeof(start_addr) + sizeof(data_len)))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    data = p_param;

                    ret = fmc_flash_nor_write(start_addr, data, data_len);
                    if (ret == false)
                    {
                        result = MP_TEST_RESULT_FAIL_FLASH_OP_FAIL;
                    }
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_FLASH_ERASE_SECTOR:
                {
                    bool ret = false;
                    uint32_t start_addr = 0;

                    if (param_len != sizeof(start_addr))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    LE_STREAM_TO_UINT32(start_addr, p_param);

                    APP_PRINT_INFO1("mp_test_handle_rtk_cmd: flash write start_addr 0x%x", start_addr);

                    ret = fmc_flash_nor_erase(start_addr, FMC_FLASH_NOR_ERASE_SECTOR);
                    if (ret == false)
                    {
                        result = MP_TEST_RESULT_FAIL_FLASH_OP_FAIL;
                    }
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_FLASH_IOCTL:
                {
                    uint32_t cmd = 0;
                    uint32_t param_1 = 0;
                    uint32_t param_2 = 0;

                    if (param_len != sizeof(cmd) + sizeof(param_1) + sizeof(param_2))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    LE_STREAM_TO_UINT32(cmd, p_param);
                    LE_STREAM_TO_UINT32(param_1, p_param);
                    LE_STREAM_TO_UINT32(param_2, p_param);

                    APP_PRINT_INFO3("mp_test_handle_rtk_cmd: cmd 0x%x, param_1 %d, param_2 %d", cmd,
                                    param_1, param_2);
#ifdef TODO
                    //TODO: add flash api in bbpro2
                    uint32_t ioctl_result = RESULT_SUCCESS;
                    flash_lock(FLASH_LOCK_USER_MODE_WRITE);
                    ioctl_result = flash_ioctl(cmd, param_1, param_2);
                    flash_unlock(FLASH_LOCK_USER_MODE_WRITE);
                    if (ioctl_result != RESULT_SUCCESS)
                    {
                        result = MP_TEST_RESULT_FAIL_FLASH_OP_FAIL;
                    }
#endif
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_AUDIO_TEST:
                {
                    if (h2d_cmd_send(p_param, param_len, true) == true)
                    {
                        APP_PRINT_INFO2("MP_TEST_RTK_SUB_OPCODE_AUDIO_TEST: param_len %d, data %b", param_len,
                                        TRACE_BINARY(param_len, p_param));
                    }
                    else
                    {
                        APP_PRINT_ERROR0("MP_TEST_RTK_SUB_OPCODE_AUDIO_TEST: h2d_cmd_send fail");
                    }
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_VENDOR_CMD:
                {
                    uint16_t op = 0;
                    uint8_t len = 0;

                    if (param_len != sizeof(op) + sizeof(len) + len)
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    LE_STREAM_TO_UINT16(op, p_param);
                    LE_STREAM_TO_UINT8(len, p_param);

                    APP_PRINT_INFO2("mp_test_handle_rtk_cmd: op 0x%x, len %d", op, len);

                    if (gap_vendor_cmd_req(op, len, p_param))
                    {
                        result = MP_TEST_RESULT_PASS;
                        handle = false;
                    }
                    else
                    {
                        return MP_TEST_RESULT_FAIL;
                    }
                }
                break;

            case MP_TEST_RTK_SUB_OPCODE_CHANGE_DEVICE_NAME:
                {
                    if (param_len > sizeof(app_cfg_nv.device_name_legacy))
                    {
                        result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                        break;
                    }

                    APP_PRINT_INFO2("MP_TEST_RTK_SUB_OPCODE_CHANGE_DEVICE_NAME: param_len %d, name %s",
                                    param_len, TRACE_STRING(p_param));
                    memcpy(app_cfg_nv.device_name_legacy, p_param, param_len);
                    memcpy(app_cfg_nv.device_name_le, p_param, param_len);
                    if (app_cfg_nv.factory_reset_done)
                    {
                        app_cfg_store(app_cfg_nv.device_name_legacy, 80);
                    }
                }
                break;

            default:
                {
                    result = MP_TEST_RESULT_FAIL_NOT_SUPPORTED_SUBOPCODE;
                }
                break;
            }
        }
        else
        {
            result = MP_TEST_RESULT_FAIL_NOT_IN_TEST_MODE;
        }
    }

    if (handle)
    {
        mp_test_send_test_result(app_idx, cmd_path, result, MP_TEST_OPCODE_RTK,
                                 sub_opcode, p_head, param_total_len);
    }

    if (p_head != NULL)
    {
        os_mem_free(p_head);
    }

    return true;
}

bool mp_test_handle_cmd(uint8_t app_idx, uint8_t cmd_path, uint8_t opcode, uint8_t sub_opcode,
                        uint8_t *p_param, uint16_t param_len)
{
    mp_app_index = app_idx;
    mp_cmd_path = cmd_path;

    switch (opcode)
    {
    case MP_TEST_OPCODE_RTK:
        mp_test_handle_rtk_cmd(app_idx, cmd_path, sub_opcode, p_param, param_len);
        break;

    case MP_TEST_OPCODE_VENDOR_BINARY:
        mp_test_handle_vendor_binary_cmd(app_idx, cmd_path, sub_opcode, p_param, param_len);
        break;

    default:
        mp_test_send_test_result(app_idx, cmd_path, MP_TEST_RESULT_FAIL_NOT_SUPPORTED_OPCODE,
                                 MP_TEST_OPCODE_VENDOR_BINARY,
                                 sub_opcode, NULL, 0);
        break;
    }

    return true;
}
#endif

