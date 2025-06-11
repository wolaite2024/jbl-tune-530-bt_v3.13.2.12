
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_CLI_BINARY_MP_SUPPORT
#include <stdint.h>
#include <stdbool.h>

#include "os_mem.h"
#include "os_msg.h"
#include "trace.h"
#include "bt_types.h"
#include "mp_test.h"
#include "mp_test_vendor.h"
#include "app_led.h"

#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#include "app_cfg.h"
#include "app_main.h"
#endif

extern uint8_t mp_app_index;
extern uint8_t mp_cmd_path;

#if F_APP_APT_SUPPORT
static bool mp_mode_enter_normal_apt = false;
static uint8_t old_apt_support_type = APT_SUPPORT_TYPE_NORMAL_APT;
#endif

/**
#define KEY0_MASK 0x01
#define KEY1_MASK 0x02
#define KEY2_MASK 0x04
#define KEY3_MASK 0x08
#define KEY4_MASK 0x10
#define KEY5_MASK 0x20
#define KEY6_MASK 0x40
#define KEY7_MASK 0x80
*/
void mp_test_vendor_send_key_event(uint8_t key_index)
{
    uint8_t result      = MP_TEST_RESULT_PASS;
    uint8_t opcode      = MP_TEST_OPCODE_VENDOR_BINARY;
    uint8_t sub_opcode  = MP_TEST_VENDOR_SUB_OPCODE_KEY;
    uint16_t param_len  = sizeof(key_index);

    mp_test_send_test_result(mp_app_index, mp_cmd_path, result, opcode, sub_opcode, &key_index,
                             param_len);
}

#if F_APP_APT_SUPPORT
static void app_mp_test_enter_normal_apt_flag_set(bool enable)
{
    mp_mode_enter_normal_apt = enable;
}

bool app_mp_test_enter_normal_apt_flag_get(void)
{
    return mp_mode_enter_normal_apt;
}

void app_mp_test_enter_normal_apt_success(void)
{
    uint8_t result      = MP_TEST_RESULT_PASS;
    uint8_t opcode      = MP_TEST_OPCODE_VENDOR_BINARY;
    uint8_t sub_opcode  = MP_TEST_VENDOR_SUB_OPCODE_ENTER_NORMAL_APT;
    mp_test_send_test_result(mp_app_index, mp_cmd_path, result, opcode, sub_opcode, NULL, 0);
}
#endif

bool mp_test_handle_vendor_binary_cmd(uint8_t app_idx, uint8_t cmd_path, uint8_t sub_opcode,
                                      uint8_t *p_param, uint16_t param_len)
{
    bool handle = true;
    uint8_t result = MP_TEST_RESULT_PASS;
    uint16_t param_total_len = 0;

    APP_PRINT_INFO2("mp_test_handle_vendor_binary_cmd: sub_opcode %d, param_len %d", sub_opcode,
                    param_len);
    switch (sub_opcode)
    {
    case MP_TEST_VENDOR_SUB_OPCODE_LED:
        {
            uint8_t led_index = 0;
            uint8_t  on_off_flag = 0;

            if (param_len != sizeof(led_index) + sizeof(on_off_flag))
            {
                result = MP_TEST_RESULT_FAIL_INVALID_LEN;
                break;
            }

            LE_STREAM_TO_UINT8(led_index, p_param);
            LE_STREAM_TO_UINT8(on_off_flag, p_param);

            APP_PRINT_INFO2("mp_test_handle_rtk_cmd: led_index 0x%d, on_off_flag %d", led_index,
                            on_off_flag);

            app_led_change_mode((T_LED_MODE)on_off_flag, true, false);
        }
        break;

    case MP_TEST_VENDOR_SUB_OPCODE_KEY:
        {
            handle = false;
        }
        break;

#if F_APP_APT_SUPPORT
    case MP_TEST_VENDOR_SUB_OPCODE_ENTER_NORMAL_APT:
        {
            handle = false;

            app_mp_test_enter_normal_apt_flag_set(true);

            if (app_db.current_listening_state == ANC_OFF_NORMAL_APT_ON)
            {
                if (app_apt_state_get() == APT_STARTED)
                {
                    handle = true;
                }
                else
                {
                    //wait apt ready
                }
            }
            else
            {
                old_apt_support_type = app_cfg_nv.apt_support_type;
                app_cfg_nv.apt_support_type = APT_SUPPORT_TYPE_NORMAL_APT;
                app_listening_state_machine(EVENT_NORMAL_APT_ON, false, false);
            }
        }
        break;

    case MP_TEST_VENDOR_SUB_OPCODE_EXIT_NORMAL_APT:
        {
            handle = true;
            app_mp_test_enter_normal_apt_flag_set(false);
            app_cfg_nv.apt_support_type = old_apt_support_type;
            app_listening_state_machine(EVENT_APPLY_FINAL_STATE, false, false);
        }
        break;
#endif

    default:
        {
            result = MP_TEST_RESULT_FAIL_NOT_SUPPORTED_SUBOPCODE;
        }
        break;

    }

    if (handle)
    {
        mp_test_send_test_result(app_idx, cmd_path, result, MP_TEST_OPCODE_VENDOR_BINARY,
                                 sub_opcode, NULL, param_total_len);
    }

    return true;
}
#endif

