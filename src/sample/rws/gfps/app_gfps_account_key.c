/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include "trace.h"
#include "remote.h"
#include "gfps.h"
#include "ftl.h"
#include "stdlib.h"
#include "app_gfps_account_key.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_relay.h"
#include "app_link_util.h"
#if GFPS_FEATURE_SUPPORT
#include "errno.h"
#include "app_gfps_sync.h"
#include <string.h>
#include "bt_gfps.h"
static T_ACCOUNT_KEY *account_key          = NULL;
static uint8_t gfps_account_key_num        = 5;  //sum of account key
static uint8_t gfps_account_key_table_size = 0;
/*Fast pair initialize*/
bool app_gfps_account_key_init(uint8_t key_num)
{
    if (key_num > GFPS_ACCOUNT_KEY_MAX)
    {
        key_num = GFPS_ACCOUNT_KEY_MAX;
    }
    gfps_account_key_num        = key_num;
    gfps_account_key_table_size = 4 + gfps_account_key_num * (GFPS_ACCOUNT_KEY_LENGTH +
                                                              GFPS_BD_ADDR_LENGTH);
    account_key = calloc(1, gfps_account_key_table_size);

    if (account_key)
    {
        uint32_t read_result = ftl_load_from_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET,
                                                     gfps_account_key_table_size);
        if (read_result == ENOF)
        {
            memset(account_key, 0, gfps_account_key_table_size);

#if GFPS_FINDER_SUPPORT
            /*the Owner Account Key is defined as the first Account Key introduced to the Provider
            The Owner Account Key must not be removed when the Provider runs out of free Account Key slots.*/
            if (extend_app_cfg_const.gfps_finder_support)
            {
                account_key->del = 1;
            }
#endif

            ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
        }
        else
        {
            for (uint8_t i = 0; i < account_key->num; i++)
            {
                APP_PRINT_INFO4("app_gfps_account_key_init: idx %d, key %b, bd_addr %b, del %d", i,
                                TRACE_BINARY(16, account_key->account_info[i].key),
                                TRACE_BDADDR(account_key->account_info[i].addr), account_key->del);
            }
        }
    }
    else
    {
        APP_PRINT_ERROR0("app_gfps_account_key_init: alloc account key memory fail");
        return false;
    }

    gfps_account_key_init(account_key, gfps_account_key_num);
    return true;
}

uint8_t app_gfps_account_key_get_table_size(void)
{
    return gfps_account_key_table_size;
}

T_ACCOUNT_KEY *app_gfps_account_key_get_table(void)
{
    return account_key;
}

/*Account key store */
bool app_gfps_account_key_store(uint8_t key[16], uint8_t *bd_addr)
{
    for (uint8_t i = 0; i < account_key->num; i++)
    {
        if (memcmp(bd_addr, account_key->account_info[i].addr, 6) == 0)
        {
            if (memcmp(key, account_key->account_info[i].key, 16) == 0)
            {
                /*bd_addr and key has already in account key table*/
                APP_PRINT_INFO1("app_gfps_account_key_store: key exist i %d", i);
                return false;
            }
            else
            {
                /*bd_addr has already stored in flash, just update key
                 if finder support, shall not update owner key*/
                APP_PRINT_INFO1("app_gfps_account_key_store: key update i %d", i);
#if GFPS_FINDER_SUPPORT
                if (extend_app_cfg_const.gfps_finder_support)
                {
                    if (i != 0)
                    {
                        memcpy(account_key->account_info[i].key, key, 16);
                        ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
                        return true;
                    }
                }
                else
#endif
                {
                    memcpy(account_key->account_info[i].key, key, 16);
                    ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
                    return true;
                }
            }
        }
    }

    if (account_key->num < gfps_account_key_num)
    {
        memcpy(account_key->account_info[account_key->num].key, key, 16);
        memcpy(account_key->account_info[account_key->num].addr, bd_addr, 6);
        account_key->num++;
    }
    else if (account_key->num == gfps_account_key_num)
    {
        memcpy(account_key->account_info[account_key->del].key, key, 16);
        memcpy(account_key->account_info[account_key->del].addr, bd_addr, 6);
        account_key->del++;
        if (account_key->del == gfps_account_key_num)
        {
#if GFPS_FINDER_SUPPORT
            /*the Owner Account Key is defined as the first Account Key introduced to the Provider
            The Owner Account Key must not be removed when the Provider runs out of free Account Key slots.*/
            if (extend_app_cfg_const.gfps_finder_support)
            {
                account_key->del = 1;
            }
            else
#endif
            {
                account_key->del = 0;
            }
        }
    }

    ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
    return true;
}

/*Clear Account key store */
void app_gfps_account_key_clear(void)
{
    APP_PRINT_WARN0("app_gfps_account_key_clear");
    memset(account_key, 0, gfps_account_key_table_size);

#if GFPS_FINDER_SUPPORT
    /*the Owner Account Key is defined as the first Account Key introduced to the Provider
    The Owner Account Key must not be removed when the Provider runs out of free Account Key slots.*/
    if (extend_app_cfg_const.gfps_finder_support)
    {
        account_key->del = 1;
    }
#endif

    ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
}

bool app_gfps_account_key_remote_add(uint8_t key[16], uint8_t *bd_addr)
{
    APP_PRINT_INFO0("app_gfps_account_key_remote_add");
    uint8_t buffer[22] = {0};//22=(16bytes account_key + 6 bytes bd_addr)
    memcpy(buffer, key, 16);//first 16 bytes is account_key info
    memcpy(buffer + 16, bd_addr, 6);// remain 6 bytes is bd addr
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS, APP_REMOTE_MSG_GFPS_ACCOUNT_KEY_ADD,
                                        buffer, 22);
    return true;
}

bool app_gfps_account_key_remote_sync(void)
{
    APP_PRINT_INFO0("app_gfps_account_key_remote_sync");
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_GFPS, APP_REMOTE_MSG_GFPS_ACCOUNT_KEY,
                                        (uint8_t *)account_key, gfps_account_key_table_size);
    return true;
}

/**
 * @brief print all account key info
 */
void app_gfps_account_key_table_print(void)
{
    for (uint8_t i = 0; i < account_key->num;  i++)
    {
        APP_PRINT_INFO3("app_gfps_account_key_table_print: idx %d, key %b, bd_addr %b", i,
                        TRACE_BINARY(16, account_key->account_info[i].key),
                        TRACE_BDADDR(account_key->account_info[i].addr));
    }
}

#if GFPS_SASS_SUPPORT
bool app_gfps_account_key_find_index_by_account_key(uint8_t *index, uint8_t *target_account_key)
{
    bool ret = false;
    for (uint8_t i = 0; i < account_key->num; i++)
    {
        if (!memcmp(target_account_key, account_key->account_info[i].key, 16))
        {
            *index = i;
            ret = true;
            break;
        }
    }
    APP_PRINT_TRACE3("app_gfps_account_key_find_index_by_account_key ret %d, accountkey %b, index %d",
                     ret,
                     TRACE_BINARY(16, account_key), *index);
    return ret;
}

bool app_gfps_account_key_verify_mac(uint8_t conn_id, uint8_t *bd_addr, uint8_t *msg,
                                     uint16_t data_len,
                                     uint8_t *msg_nonce, uint8_t *mac)
{
    uint8_t inuse_account_key_index = gfps_get_inuse_account_key_index();
    bool ret = false;
    uint8_t tmp_mac[8] = {0};
    uint8_t gfps_session_nonce[8];

    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
        if (p_link == NULL)
        {
            return false;
        }
        inuse_account_key_index = p_link->gfps_inuse_account_key;
        memcpy(gfps_session_nonce, p_link->gfps_session_nonce, 8);
    }
#if GFPS_SASS_LEA_SUPPORT
    else
    {
        T_APP_LE_LINK *p_le_link = app_link_find_le_link_by_conn_id(conn_id);
        if (p_le_link == NULL)
        {
            return false;
        }
        inuse_account_key_index = p_le_link->gfps_inuse_account_key;
        memcpy(gfps_session_nonce, p_le_link->gfps_session_nonce, 8);
    }
#endif

    if (inuse_account_key_index >= gfps_account_key_num)//invalid
    {
        if (gfps_get_highest_priority_device(&inuse_account_key_index))
        {
            bt_gfps_generate_mac(account_key->account_info[inuse_account_key_index].key,

                                 gfps_session_nonce, msg_nonce,

                                 msg, data_len, tmp_mac);

        }
    }
    else
    {
        bt_gfps_generate_mac(account_key->account_info[inuse_account_key_index].key,
                             gfps_session_nonce, msg_nonce,
                             msg, data_len, tmp_mac);


    }
    if (!memcmp(tmp_mac, mac, 8))
    {
        ret = true;
    }
    APP_PRINT_TRACE5("app_gfps_account_key_verify_mac id %d, addr %s, ret %d, account %d, %b", conn_id,
                     TRACE_BDADDR(bd_addr), ret, inuse_account_key_index,
                     TRACE_BINARY(8, gfps_session_nonce));
    return ret;
}
bool app_gfps_account_key_find_inuse_account_key(uint8_t *inuse_accout_idx, uint8_t conn_id,
                                                 uint8_t *bd_addr,
                                                 uint8_t *msg, uint16_t data_len, uint8_t *msg_nonce, uint8_t *mac)
{
    uint8_t tmp_mac[8] = {0};
    uint8_t gfps_session_nonce[8];
    bool ret = false;
    uint8_t addr[6] = {0};

    if (conn_id == 0xFF)
    {
        T_APP_BR_LINK *p_link = app_link_find_br_link(bd_addr);
        if (p_link == NULL)
        {
            return false;
        }
        memcpy(gfps_session_nonce, p_link->gfps_session_nonce, 8);
        memcpy(addr, p_link->bd_addr, 6);
    }
#if GFPS_SASS_LEA_SUPPORT
    else
    {
        T_APP_LE_LINK *p_le_link = app_link_find_le_link_by_conn_id(conn_id);
        if (p_le_link == NULL)
        {
            return false;
        }
        memcpy(gfps_session_nonce, p_le_link->gfps_session_nonce, 8);
        memcpy(addr, p_le_link->bd_addr, 6);
    }
#endif

    for (uint8_t i = 0; i < account_key->num; i++)
    {
        bt_gfps_generate_mac(account_key->account_info[i].key, gfps_session_nonce, msg_nonce,
                             msg, data_len, tmp_mac);

        APP_PRINT_TRACE2("app_gfps_account_key_find_inuse_account_key: addr: %b, account_addr: %b",
                         TRACE_BINARY(6, bd_addr), TRACE_BINARY(6, account_key->account_info[i].addr));
        if (!memcmp(tmp_mac, mac, 8) && !memcmp(account_key->account_info[i].addr, addr, 6))
        {
            *inuse_accout_idx = i;
            ret = true;
        }
    }
    APP_PRINT_TRACE5("app_gfps_account_key_find_inuse_account_key id %d, addr %s, ret %d index %d, account key %b",
                     conn_id, TRACE_BDADDR(bd_addr), ret,
                     *inuse_accout_idx, TRACE_BINARY(16, account_key->account_info[*inuse_accout_idx].key));
    return ret;
}
#endif

void app_gfps_account_key_remote_handle_add(uint8_t key[16], uint8_t *bd_addr)
{
    APP_PRINT_INFO0("app_gfps_account_key_remote_handle_add: sec store account key");
    app_gfps_account_key_store(key, bd_addr);
}

#if GFPS_FINDER_SUPPORT
bool app_gfps_account_key_update_owner_key(uint8_t key[16], T_ACCOUNT_KEY *p_table)
{
    memcpy(p_table->account_info[0].key, key, 16);
    if (p_table->num == 0)
    {
        p_table->num++;
    }
    return true;
}
#endif
bool app_gfps_account_key_add(uint8_t key[16], uint8_t *bd_addr, T_ACCOUNT_KEY *p_table)
{
    for (uint8_t i = 0; i < p_table->num; i++)
    {
        if (memcmp(bd_addr, p_table->account_info[i].addr, 6) == 0)
        {
            if (memcmp(key, p_table->account_info[i].key, 16) == 0)
            {
                /*bd_addr and key has already in p_table*/
                return false;
            }
            else
            {
                /*bd_addr has already in p_table, just update key
                 if finder support, shall not update owner key*/
                APP_PRINT_INFO1("app_gfps_account_key_add: key update i %d", i);
#if GFPS_FINDER_SUPPORT
                if (extend_app_cfg_const.gfps_finder_support)
                {
                    if (i != 0)
                    {
                        memcpy(p_table->account_info[i].key, key, 16);
                        return true;
                    }
                }
                else
#endif
                {
                    memcpy(p_table->account_info[i].key, key, 16);
                    return true;
                }
            }
        }
    }

    if (p_table->num < gfps_account_key_num)
    {
        memcpy(p_table->account_info[p_table->num].key, key, 16);
        memcpy(p_table->account_info[p_table->num].addr, bd_addr, 6);
        p_table->num++;
    }
    else if (p_table->num == gfps_account_key_num)
    {
        memcpy(p_table->account_info[p_table->del].key, key, 16);
        memcpy(p_table->account_info[p_table->del].addr, bd_addr, 6);
        p_table->del++;
        if (p_table->del == gfps_account_key_num)
        {
#if GFPS_FINDER_SUPPORT
            /*the Owner Account Key is defined as the first Account Key introduced to the Provider
            The Owner Account Key must not be removed when the Provider runs out of free Account Key slots.*/
            if (extend_app_cfg_const.gfps_finder_support)
            {
                p_table->del = 1;
            }
            else
#endif
            {
                p_table->del = 0;
            }
        }
    }

    return true;
}

void app_gfps_account_key_remote_handle_sync(uint8_t *info_data, uint8_t info_len)
{
    APP_PRINT_INFO0("app_gfps_account_key_remote_handle_sync: sec store account key");
    T_ACCOUNT_KEY *account_key_remote = (T_ACCOUNT_KEY *)info_data;
    if (gfps_account_key_table_size != info_len ||
        account_key_remote->num > gfps_account_key_num)
    {
        return;
    }
    if (memcmp(info_data, account_key, gfps_account_key_table_size))
    {
        T_ACCOUNT_KEY *account_key_temp = calloc(1, gfps_account_key_table_size);
        T_ACCOUNT_KEY *account_key1 = account_key;
        T_ACCOUNT_KEY *account_key2 = account_key_remote;
        uint8_t account_key1_idx;
        uint8_t account_key2_idx;
        uint8_t account_key2_num = 0;
        uint8_t i;
        if (account_key_temp == NULL)
        {
            return;
        }
        if (account_key_remote->num > account_key->num)
        {
            account_key1 = account_key_remote;
            account_key2 = account_key;
        }
        else if (account_key_remote->num < account_key->num)
        {
            account_key1 = account_key;
            account_key2 = account_key_remote;
        }
        else if (account_key_remote->num == account_key->num)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                account_key1 = account_key;
                account_key2 = account_key_remote;
            }
            else
            {
                account_key1 = account_key_remote;
                account_key2 = account_key;
            }
        }
        account_key1_idx = account_key1->del;
        account_key2_idx = account_key2->del;
#if GFPS_FINDER_SUPPORT
        if (extend_app_cfg_const.gfps_finder_support)
        {
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_gfps_account_key_update_owner_key(account_key->account_info[0].key, account_key_temp);
            }
            else
            {
                app_gfps_account_key_update_owner_key(account_key_remote->account_info[0].key, account_key_temp);
            }
        }
#endif
        for (i = account_key1->num; i > 0; i--)
        {
            app_gfps_account_key_add(account_key1->account_info[account_key1_idx].key,
                                     account_key1->account_info[account_key1_idx].addr, account_key_temp);
            account_key1_idx++;
            if (account_key1_idx == gfps_account_key_num)
            {
                account_key1_idx = 0;
            }
            if (account_key2_num < account_key2->num)
            {
                app_gfps_account_key_add(account_key2->account_info[account_key2_idx].key,
                                         account_key2->account_info[account_key2_idx].addr, account_key_temp);
                account_key2_idx++;
                account_key2_num++;
                if (account_key2_idx == gfps_account_key_num)
                {
                    account_key2_idx = 0;
                }
            }
        }

        if (memcmp(account_key_temp, account_key, gfps_account_key_table_size))
        {
            memcpy(account_key, account_key_temp, gfps_account_key_table_size);
            ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
        }
        free(account_key_temp);
    }
}

#if GFPS_FINDER_SUPPORT
void app_gfps_account_key_save_ownerkey_valid(void)
{
    APP_PRINT_INFO1("app_gfps_account_key_save_ownerkey_valid: valid %d", account_key->owner_key_valid);
    ftl_save_to_storage(account_key, ACCOUNT_KEY_FLASH_OFFSET, gfps_account_key_table_size);
}
#endif
#endif
