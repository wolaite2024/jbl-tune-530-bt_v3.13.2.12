#if F_APP_TUYA_SUPPORT
#include "rtk_tuya_ble_config.h"
#if RTK_TUYA_AES_TEST_DEMO
#include "stdlib.h"
#include "stdint.h"
#include "tuya_ble_api.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_log.h"
#include "rtk_tuya_aes_test.h"

void rtk_tuya_aes_test(void)//for test
{
    uint8_t iv[16] = {0x6a, 0xf3, 0x3b, 0xd3, 0x1b, 0x84, 0x0f, 0x36, 0x44, 0x42, 0x5c, 0x23, 0x2c, 0x4b, 0xbc, 0x97};
    uint8_t in_put[16] = {0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0xaf, 0xa3, 0x01};
    static uint8_t output_demo_result[16] = {0xFF, 0xF8, 0x18, 0xFC, 0xBD, 0x7F, 0x2A, 0x77, 0x4E, 0xB0, 0x59, 0xFE, 0x58, 0x87, 0x3F, 0x14};

    static uint8_t out_put[16];
    uint32_t out_len;

    static tuya_ble_parameters_settings_t test_para;
    static uint8_t test_login_key[6] = {0x34, 0x65, 0x30, 0x33, 0x32, 0x39};
    static uint8_t test_uuid[16] = {0x74, 0x75, 0x79, 0x61, 0x37, 0x33, 0x38, 0x38, 0x33, 0x32, 0x38, 0x36, 0x33, 0x38, 0x36, 0x31};
    static uint8_t test_authkey[] = "tsCPAPyTxG1BBzm8qn0gGfl721DzANLF";
    static uint8_t test_rand[6] = {0xc7, 0xe6, 0xf6, 0x13, 0x7a, 0x5e};

    memcpy(test_para.sys_settings.login_key, test_login_key, 6);
    memcpy(test_para.auth_settings.device_id, test_uuid, 16);
    memcpy(test_para.auth_settings.auth_key, test_authkey, 32);

    TUYA_APP_LOG_HEXDUMP_DEBUG("rtk_tuya_aes_test uuid:", test_para.auth_settings.device_id, 16);
    TUYA_APP_LOG_HEXDUMP_DEBUG("rtk_tuya_aes_test auth key:", test_para.auth_settings.auth_key, 32);
    TUYA_APP_LOG_HEXDUMP_DEBUG("rtk_tuya_aes_test srand:", test_rand, 6);

    tuya_ble_device_delay_ms(1000);

    if (tuya_ble_encryption(0x0303, 5, iv, in_put, sizeof(in_put), &out_len,
                            out_put, &test_para, test_rand) == 0)
    {
        TUYA_APP_LOG_HEXDUMP_DEBUG("rtk_tuya_aes_test encryption output:", out_put, out_len);
        if (memcmp(out_put, output_demo_result, out_len) == 0)
        {
            APP_PRINT_INFO0("rtk_tuya_aes_test: encryption success");
        }
        else
        {
            APP_PRINT_INFO0("rtk_tuya_aes_test: error");
        }
    }
    else
    {
        TUYA_APP_LOG_ERROR("rtk_tuya_aes_test:encryption failed");
    }

    uint8_t rtk_key[16] = {0xb3, 0x87, 0xbf, 0xd4, 0xbb, 0x6c, 0xe8, 0xe1, 0x36, 0x56, 0x99, 0x8d, 0x24, 0xfc, 0x25, 0x49};
    uint8_t rtk_iv[16] = {0x45, 0xc6, 0x90, 0xd8, 0x16, 0xcd, 0xea, 0xa3, 0x1d, 0xac, 0x8e, 0x84, 0xe5, 0xf1, 0x7f, 0x84};
    uint8_t rtk_input[16] = {0xac, 0xb9, 0x7c, 0x1a, 0xe2, 0x5f, 0x08, 0xb5, 0xf0, 0xb5, 0x21, 0xd5, 0xcb, 0x10, 0x00, 0x81};
    uint8_t rtk_out_put[16];
    uint8_t rtk_out_demo[16] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xf3, 0x78, 0x4e};
    tuya_ble_aes128_cbc_decrypt(rtk_key, rtk_iv, rtk_input, 16,
                                rtk_out_put);
    TUYA_APP_LOG_HEXDUMP_DEBUG("rtk_tuya_aes_test decrypt output:", rtk_out_put, out_len);
    if (memcmp(rtk_out_put, rtk_out_demo, out_len) == 0)
    {
        APP_PRINT_INFO0("rtk_tuya_aes_test: decrypt success");
    }
    else
    {
        APP_PRINT_INFO0("rtk_tuya_aes_test: decrypt error");
    }
}
#endif
#endif
