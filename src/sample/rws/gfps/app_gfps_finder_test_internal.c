/*Note:
 *Only used by realtek to internally test the correctness of the algorithm.
 */
#if GFPS_FINDER_TEST_SUPPORT
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "trace.h"
#include "gfps_find_my_device.h"

bool gfps_finder_encrypted_eik(uint8_t *p_accountkey, uint8_t *p_input, uint8_t *p_output);
bool gfps_finder_decrypted_eik(uint8_t *p_accountkey, uint8_t *p_input, uint8_t *p_output);
void app_gfps_finder_generate_adv_ei_and_hash(uint8_t *p_adv_ei, uint8_t *p_eik,
                                              uint32_t counter_value, uint8_t *hash);

/**
 * @brief app_gfps_finder_verify_encrypted_beacon_data
 * online verify AES256encrypt:
 * Key:00000000FF00FF00FF00000000000000ff000000FF00FF000000000000000000
 * Input Data:ffffffffffffffffffffff0a0000000000000000000000000000000a00000000
 * Output Data:2db5b96ebe2b527bb8df20befecbe46ebbc398d6be7f70904aba4a03bd8350f3
 * http://www.cryptogrium.com/aes-encryption-online-ecb.html
 * rtk verify AES256encrypt:
 * Key : @ref EIK
 * Input Data: @ref beacon_data
 * Output Data: @ref result
 * @return true
 * @return false
 */
bool app_gfps_finder_verify_encrypted_beacon_data(void)
{
    uint8_t EIK[32]         = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
                               0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0xff, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                              };
    uint8_t beacon_data[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0xff, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00
                              };
    uint8_t result[32]      = {0x2D, 0xB5, 0xB9, 0x6E, 0xBE, 0x2B, 0x52, 0x7B,
                               0xB8, 0xDF, 0x20, 0xBE, 0xFE, 0xCB, 0xE4, 0x6E,
                               0xBB, 0xC3, 0x98, 0xD6, 0xBE, 0x7F, 0x70, 0x90,
                               0x4A, 0xBA, 0x4A, 0x03, 0xBD, 0x83, 0x50, 0xF3
                              };
    uint8_t r[32]           = {0};

    gfps_finder_encrypted_beacon_data(EIK, beacon_data, r);
    if (memcmp(result, r, 32) != 0)
    {
        APP_PRINT_ERROR0("app_gfps_finder_verify_encrypted_beacon_data: fail");
        return false;
    }
    return true;
}

/**
 * @brief app_gfps_finder_verify_encrypted_eik
 * online verify AES256encrypt:
 * Key:00000000FF00FF00FF00000000000000
 * Input Data:ffffffffffffffffffffff0a0000000000000000000000000000000a00000000
 * Output Data:e5c7f12ff044d8e653ea27b71422074596687e4c9ef2a0ed392384ca1404931a
 * http://www.cryptogrium.com/aes-encryption-online-ecb.html
 * rtk verify AES256encrypt:
 * Key : @ref EIK
 * Input Data: @ref beacon_data
 * Output Data: @ref result
 * @return true
 * @return false
 */
bool app_gfps_finder_verify_encrypted_eik(void)
{
    uint8_t account_key[16] = {0x04, 0x39, 0xB8, 0x37, 0x36, 0x6B, 0xE6, 0xE3,
                               0x5C, 0xA2, 0x3E, 0xDB, 0xB7, 0xCF, 0x7B, 0xF1,
                              };
    uint8_t result[32]      = {0x70, 0xc2, 0x08, 0xe2, 0xa0, 0xd2, 0x85, 0x38,
                               0x30, 0x2f, 0x2f, 0x89, 0x9c, 0xfb, 0xeb, 0x4e,
                               0x01, 0x67, 0x00, 0xac, 0x8e, 0x99, 0xbc, 0x88,
                               0x3b, 0xe8, 0x47, 0x1e, 0x2e, 0xd1, 0xd4, 0xb6,
                              };
    uint8_t eik_data[32]    = {0x2E, 0xF6, 0xCE, 0xDB, 0x85, 0xB5, 0x8F, 0x6D,
                               0x79, 0x5A, 0x3E, 0xBD, 0x44, 0xDD, 0x33, 0xC6,
                               0x6A, 0xC9, 0x01, 0x88, 0xC5, 0x00, 0x7B, 0xEA,
                               0x93, 0x8A, 0x1F, 0xEF, 0x75, 0x7E, 0x69, 0x2E,
                              };
    uint8_t r[32]           = {0};

    gfps_finder_encrypted_eik(account_key, eik_data, r);
    if (memcmp(result, r, 32) != 0)
    {
        APP_PRINT_ERROR0("app_gfps_finder_verify_encrypted_eik: fail");
        return false;
    }
    return true;
}

/**
 * @brief app_gfps_finder_verify_decrypted_eik
 * online verify AES256decrypt:
 * Account Key:00000000FF00FF00FF00000000000000
 * Input Data:e5c7f12ff044d8e653ea27b71422074596687e4c9ef2a0ed392384ca1404931a
 * Output Data:ffffffffffffffffffffff0a0000000000000000000000000000000a00000000
 * http://www.cryptogrium.com/aes-encryption-online-ecb.html
 * rtk verify AES256decrypt:
 * Key : @ref EIK
 * Input Data: @ref beacon_data
 * Output Data: @ref result
 * @return true
 * @return false
 */
bool app_gfps_finder_verify_decrypted_eik(void)
{
    uint8_t account_key[16] = {0x04, 0x39, 0xB8, 0x37, 0x36, 0x6B, 0xE6, 0xE3,
                               0x5C, 0xA2, 0x3E, 0xDB, 0xB7, 0xCF, 0x7B, 0xF1,
                              };
    uint8_t result[32]      = {0xd6, 0x2d, 0x11, 0xdb, 0x35, 0x46, 0x6d, 0xf3,
                               0x00, 0xf9, 0xf4, 0x52, 0x0f, 0x34, 0xf9, 0x0c,
                               0x98, 0x83, 0x4f, 0x7b, 0xd1, 0x7c, 0x84, 0x4f,
                               0x89, 0xf9, 0xc6, 0x4e, 0x12, 0x23, 0x9d, 0x83,
                              };
    uint8_t eik_data[32]    = {0x2E, 0xF6, 0xCE, 0xDB, 0x85, 0xB5, 0x8F, 0x6D,
                               0x79, 0x5A, 0x3E, 0xBD, 0x44, 0xDD, 0x33, 0xC6,
                               0x6A, 0xC9, 0x01, 0x88, 0xC5, 0x00, 0x7B, 0xEA,
                               0x93, 0x8A, 0x1F, 0xEF, 0x75, 0x7E, 0x69, 0x2E,
                              };
    uint8_t r[32]           = {0};

    gfps_finder_decrypted_eik(account_key, eik_data, r);
    if (memcmp(result, r, 32) != 0)
    {
        APP_PRINT_ERROR0("app_gfps_finder_verify_decrypted_eik: fail");
        return false;
    }
    return true;
}

/**
 * @brief
 * Key:00000000FF00FF00FF00000000000000ff000000FF00FF000000000000000000
 * K:10
 * Clock_value: 0
 * @return true
 * @return false
 */
bool app_gfps_finder_verify_ei(void)
{
    uint8_t EIK[32] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
                       0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0xff, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                      };

    uint8_t sample_ei[20] = {0x54, 0x73, 0x86, 0xa7, 0x39, 0xc0, 0x0e, 0x3f, 0xba, 0xbd,
                             0x29, 0xe0, 0x17, 0xef, 0x08, 0x5d, 0xaa, 0xaf, 0x6a, 0x99
                            };
    uint8_t ei[20]        = {0};
    uint32_t counter_value  = 0;
    uint8_t hash = 0;

    app_gfps_finder_generate_adv_ei_and_hash(ei, EIK, counter_value, &hash);
    if (memcmp(ei, sample_ei, 20) != 0)
    {
        APP_PRINT_ERROR0("app_gfps_finder_verify_ei: fail");
        return false;
    }
    return true;
}

bool app_gfps_finder_test(void)
{
    uint8_t ret = 0;
    if (app_gfps_finder_verify_encrypted_beacon_data() == false)
    {
        ret = 1;
        goto err;
    };
    if (app_gfps_finder_verify_encrypted_eik() == false)
    {
        ret = 2;
        goto err;
    };
    if (app_gfps_finder_verify_decrypted_eik() == false)
    {
        ret = 3;
        goto err;
    };
    if (app_gfps_finder_verify_ei() == false)
    {
        ret = 4;
        goto err;
    };

    APP_PRINT_TRACE0("app_gfps_finder_test: success");
    return true;

err:
    APP_PRINT_ERROR1("app_gfps_finder_test: err ret %d", ret);
    return false;
}
#endif
