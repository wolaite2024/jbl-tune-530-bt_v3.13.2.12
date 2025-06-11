/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     aes_cbc_api.c
* @brief    This file provides aes cbc api.
* @details
* @author   jane_zhang
* @date     2021-9-24
* @version  v1.0
*********************************************************************************************************
*/
#if F_APP_TUYA_SUPPORT
#include <string.h>
#include "aes_api.h"
#include "rtk_aes_cbc_api.h"
#include "rtl876x_hw_aes.h"
#include "hw_aes.h"
#include "hw_aes_int.h"

/**
    * @brief  byte swap
    * @param  src          source address
    * @param  dst          destination address
    * @param  len          swap size
*/
static void swap_buf(const uint8_t *src, uint8_t *dst, uint16_t len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        dst[len - 1 - i] = src[i];
    }
}
// CBC mode for 128-bit aes key
bool aes128_cbc_encrypt(uint8_t *plaintext, const uint8_t key[16], uint8_t *encrypted,
                        uint32_t *p_iv, uint32_t data_word_len)
{
    bool ret;

    uint8_t *in = plaintext;
    uint8_t *out = encrypted;
    uint32_t iv[4] = {0, 0, 0, 0};

    if (p_iv != 0)
    {
        memcpy((uint8_t *)iv, (uint8_t *)p_iv, 16);
    }

    if (data_word_len % 4)
    {
        return false;
    }

    for (int i = 0; i < data_word_len; i += 4)
    {
        ret = hw_aes_encrypt128((uint32_t *) in, (uint32_t *) out, 4, (uint32_t *)key, iv,
                                AES_MODE_CBC);

        memcpy((uint8_t *)iv, out, 16);

        in += 16;
        out += 16;

        if (!ret)
        {
            break;
        }
    }

    return ret;
}

bool aes128_cbc_decrypt(uint8_t *input, const uint8_t key[16], uint8_t *output, uint32_t *p_iv,
                        uint32_t data_word_len)
{
    bool ret;

    uint8_t *in = input;
    uint8_t *out = output;
    uint32_t iv[4] = {0, 0, 0, 0};

    if (p_iv != 0)
    {
        memcpy((uint8_t *)iv, (uint8_t *)p_iv, 16);
    }

    if (data_word_len % 4)
    {
        return false;
    }

    for (int i = 0; i < data_word_len; i += 4)
    {
        ret = hw_aes_decrypt128((uint32_t *) in, (uint32_t *) out, 4, (uint32_t *)key, iv,
                                AES_MODE_CBC);

        memcpy((uint8_t *)iv, in, 16);

        in += 16;
        out += 16;

        if (!ret)
        {
            break;
        }
    }

    return ret;
}


bool aes128_cbc_encrypt_msb2lsb(uint8_t plaintext[16], const uint8_t key[16], uint8_t *encrypted,
                                uint32_t *p_iv, uint32_t data_word_len)
{
    bool ret;

    uint32_t iv[4] = {0, 0, 0, 0};
    uint8_t key_tmp[16] = {0};
    uint8_t *in = plaintext;
    uint8_t *out = encrypted;

    /* The most significant octet of key corresponds to key[0] */
    swap_buf(key, key_tmp, 16);

    if (p_iv != 0)
    {
        memcpy((uint8_t *)iv, (uint8_t *)p_iv, 16);
    }

    if (data_word_len % 4)
    {
        return false;
    }

    for (int i = 0; i < data_word_len; i += 4)
    {

        uint8_t in_tmp[16] = {0};
        uint8_t out_tmp[16] = {0};

        swap_buf(in, in_tmp, 16);

        ret = hw_aes_encrypt128((uint32_t *) in_tmp, (uint32_t *) out_tmp, 4, (uint32_t *)key_tmp, iv,
                                AES_MODE_CBC);

        memcpy(iv, out_tmp, 16);

        swap_buf(out_tmp, out, 16);

        in += 16;
        out += 16;

        if (!ret)
        {
            break;
        }
    }

    return ret;
}

bool aes128_cbc_decrypt_msb2lsb(uint8_t *input, const uint8_t key[16], uint8_t *output,
                                uint32_t *p_iv, uint32_t data_word_len)
{
    bool ret;

    uint32_t iv[4] = {0, 0, 0, 0};
    uint8_t *in = input;
    uint8_t *out = output;
    uint8_t key_tmp[32] = {0};

    swap_buf(key, key_tmp, 32);

    if (p_iv != 0)
    {
        memcpy((uint8_t *)iv, (uint8_t *)p_iv, 16);
    }

    if (data_word_len % 4)
    {
        return false;
    }

    for (int i = 0; i < data_word_len; i += 4)
    {
        uint8_t in_tmp[16] = {0};
        uint8_t out_tmp[16] = {0};

        swap_buf(in, in_tmp, 16);

        ret = hw_aes_decrypt256((uint32_t *) in_tmp, (uint32_t *) out_tmp, 4, (uint32_t *)key_tmp, iv,
                                AES_MODE_CBC);

        memcpy(iv, in_tmp, 16);

        swap_buf(out_tmp, out, 16);

        in += 16;
        out += 16;

        if (!ret)
        {
            break;
        }
    }

    return ret;
}

// CBC mode for 256-bit aes key
bool aes256_cbc_encrypt(uint8_t *plaintext, const uint8_t key[32], uint8_t *encrypted,
                        uint32_t *p_iv, uint32_t data_word_len)
{
    bool ret;
    uint32_t iv[4] = {0, 0, 0, 0};
    uint8_t *in = plaintext;
    uint8_t *out = encrypted;

    if (p_iv != 0)
    {
        memcpy((uint8_t *)iv, (uint8_t *)p_iv, 16);
    }

    if (data_word_len % 4)
    {
        return false;
    }

    for (int i = 0; i < data_word_len; i += 4)
    {

        ret = hw_aes_encrypt256((uint32_t *) in, (uint32_t *) out, 4, (uint32_t *)key, iv,
                                AES_MODE_CBC);

        memcpy(iv, out, 16);
        in += 16;
        out += 16;

        if (!ret)
        {
            break;
        }
    }

    return ret;
}

bool aes256_cbc_decrypt(uint8_t *input, const uint8_t key[32], uint8_t *output, uint32_t *p_iv,
                        uint32_t data_word_len)
{
    bool ret;

    uint8_t *in = input;
    uint8_t *out = output;
    uint32_t iv[4] = {0, 0, 0, 0};

    if (p_iv != 0)
    {
        memcpy((uint8_t *)iv, (uint8_t *)p_iv, 16);
    }

    if (data_word_len % 4)
    {
        return false;
    }

    for (int i = 0; i < data_word_len; i += 4)
    {
        ret = hw_aes_decrypt256((uint32_t *) in, (uint32_t *) out, 4, (uint32_t *)key, iv,
                                AES_MODE_CBC);

        memcpy(iv, in, 16);

        in += 16;
        out += 16;

        if (!ret)
        {
            break;
        }
    }

    return ret;
}
#endif
