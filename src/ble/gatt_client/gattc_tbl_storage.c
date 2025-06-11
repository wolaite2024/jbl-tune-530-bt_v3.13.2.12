#include "trace.h"
#include "ftl.h"
#include "crc16btx.h"
#include "bt_gatt_client.h"
#include "gattc_tbl_storage.h"

#if GATTC_TBL_STORAGE_SUPPORT
#include <string.h>
#if F_APP_BOND_MGR_SUPPORT
#include "bt_bond_le.h"
#else
#include "gap_bond_le.h"
#endif

#define GATTC_FTL_MGR_VERSION 0x02
#define GATTC_FTL_SYNC_WORD 0x55335533

#define GATTC_FTL_MAX_BLOCK_LEN BT_EXT_FTL_BLOCK_LEN
#define GATTC_FTL_BLOCK_MAX_NUM  ((BT_EXT_FTL_SIZE+GATTC_FTL_MAX_BLOCK_LEN-1)/GATTC_FTL_MAX_BLOCK_LEN)
#define GATTC_FTL_BITMAP_MAX_NUM  ((GATTC_FTL_BLOCK_MAX_NUM + 7)/8)

#define GATTC_STORAGE_DEBUG 0

typedef struct
{
    uint8_t  used: 1;
    uint8_t  reserved: 7;
    uint8_t  remote_bd_type;
    uint8_t  remote_bd[6];
    T_GATT_SERVICE_DATA gatt_data;
    uint8_t  cccd_block_num;
    uint8_t  svc_block_idx;
    uint8_t  block_used_num;
    uint16_t cccd_data_len;
    uint16_t cccd_crc16;
    uint16_t svc_total_len;
    uint16_t svc_crc16;
    uint8_t  block_used_bitmap[GATTC_FTL_BITMAP_MAX_NUM];
} T_GATTC_FTL_DEV;

typedef struct
{
    uint8_t version;
    uint8_t block_unused_num;
    uint8_t block_unused_bitmap[GATTC_FTL_BITMAP_MAX_NUM];
    T_GATTC_FTL_DEV dev_tbl[GATTC_FTL_MAX_DEV_NUM];
    uint32_t sync_word;
} T_GATTC_FTL_CB;

#define GATTC_FTL_CB_LEN  ((sizeof(T_GATTC_FTL_CB)+GATTC_FTL_MAX_BLOCK_LEN-1)/GATTC_FTL_MAX_BLOCK_LEN)*GATTC_FTL_MAX_BLOCK_LEN

typedef union
{
    uint8_t data[GATTC_FTL_CB_LEN];
    T_GATTC_FTL_CB ftl_cb;
} T_GATTC_FTL_HEADER;

bool gattc_tbl_get_identity_addr(uint8_t *bd_addr, uint8_t bd_type,
                                 uint8_t *identity_addr, uint8_t *identity_addr_type)
{
    uint8_t err_idx = 0;
#if F_APP_BOND_MGR_SUPPORT
    T_LE_BOND_ENTRY *p_key_entry = NULL;
    if (bd_type == GAP_REMOTE_ADDR_LE_RANDOM)
    {
        if ((bd_addr[5] & RANDOM_ADDR_MASK) == RANDOM_ADDR_MASK_RESOLVABLE)
        {
            if (bt_le_resolve_random_address(bd_addr, identity_addr, identity_addr_type) == false)
            {
                err_idx = 1;
                goto failed;
            }
            else
            {
                return true;
            }
        }
        else if ((bd_addr[5] & RANDOM_ADDR_MASK) == RANDOM_ADDR_MASK_NON_RESOLVABLE)
        {
            err_idx = 2;
            goto failed;
        }
    }

    p_key_entry = bt_le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type, NULL,
                                       LE_BOND_LOCAL_ADDRESS_ANY);
    if (p_key_entry == NULL)
    {
        err_idx = 3;
        goto failed;
    }
    if ((bd_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY)
        || (bd_type == GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY))
    {
        bd_type -= 2;
    }
    *identity_addr_type = bd_type;
    memcpy(identity_addr, bd_addr, 6);
#else
    T_LE_KEY_ENTRY *p_key_entry = NULL;
    if (bd_type == GAP_REMOTE_ADDR_LE_RANDOM)
    {
        if ((bd_addr[5] & RANDOM_ADDR_MASK) == RANDOM_ADDR_MASK_RESOLVABLE)
        {
            if (le_resolve_random_address(bd_addr, identity_addr, identity_addr_type) == false)
            {
                err_idx = 1;
                goto failed;
            }
            else
            {
                return true;
            }
        }
        else if ((bd_addr[5] & RANDOM_ADDR_MASK) == RANDOM_ADDR_MASK_NON_RESOLVABLE)
        {
            err_idx = 2;
            goto failed;
        }
    }

    p_key_entry = le_find_key_entry(bd_addr, (T_GAP_REMOTE_ADDR_TYPE)bd_type);
    if (p_key_entry == NULL)
    {
        err_idx = 3;
        goto failed;
    }
    if ((bd_type == GAP_REMOTE_ADDR_LE_PUBLIC_IDENTITY)
        || (bd_type == GAP_REMOTE_ADDR_LE_RANDOM_IDENTITY))
    {
        bd_type -= 2;
    }
    *identity_addr_type = bd_type;
    memcpy(identity_addr, bd_addr, 6);
#endif
    return true;
failed:
    APP_PRINT_ERROR3("gattc_tbl_get_identity_addr: bd_type %d, bd_addr %s, err_idx %d", bd_type,
                     TRACE_BDADDR(bd_addr), err_idx);
    return false;
}
#if GATTC_STORAGE_DEBUG
void gattc_tbl_storage_print(void)
{
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    p_ftl_header = calloc(1, sizeof(T_GATTC_FTL_HEADER));

    if (p_ftl_header)
    {
#if (FTL_POOL_ENABLE == 1)
        if (ftl_load_from_module(BT_EXT_FTL_PARTITION_NAME, p_ftl_header, 0,
                                 sizeof(T_GATTC_FTL_HEADER)) != 0)
#else
        if (ftl_load_from_storage(p_ftl_header, bt_ext_ftl_start_offset,
                                  sizeof(T_GATTC_FTL_HEADER)) != 0)
#endif
        {
            APP_PRINT_INFO0("gattc_tbl_storage_print: load header failed");
        }
        else
        {
            if (p_ftl_header->ftl_cb.sync_word == GATTC_FTL_SYNC_WORD)
            {
                APP_PRINT_INFO3("gattc_tbl_storage_print: version 0x%x, block_unused_num %d, block_unused_bitmap %b",
                                p_ftl_header->ftl_cb.version, p_ftl_header->ftl_cb.block_unused_num,
                                TRACE_BINARY(GATTC_FTL_BITMAP_MAX_NUM, p_ftl_header->ftl_cb.block_unused_bitmap));
                for (uint8_t i = 0; i < GATTC_FTL_MAX_DEV_NUM; i++)
                {
                    if (p_ftl_header->ftl_cb.dev_tbl[i].used)
                    {
                        APP_PRINT_INFO5("dev_tbl[%d]: remote_bd_type %d, remote_bd %s, block_used_num %d, block_used_bitmap %b",
                                        i,
                                        p_ftl_header->ftl_cb.dev_tbl[i].remote_bd_type,
                                        TRACE_BDADDR(p_ftl_header->ftl_cb.dev_tbl[i].remote_bd),
                                        p_ftl_header->ftl_cb.dev_tbl[i].block_used_num,
                                        TRACE_BINARY(GATTC_FTL_BITMAP_MAX_NUM, p_ftl_header->ftl_cb.dev_tbl[i].block_used_bitmap));
                        APP_PRINT_INFO6("\t cccd_block_num %d, cccd_data_len %d, cccd_crc16 0x%x, svc_block_idx %d, svc_total_len %d, svc_crc16 0x%x",
                                        p_ftl_header->ftl_cb.dev_tbl[i].cccd_block_num,
                                        p_ftl_header->ftl_cb.dev_tbl[i].cccd_data_len,
                                        p_ftl_header->ftl_cb.dev_tbl[i].cccd_crc16,
                                        p_ftl_header->ftl_cb.dev_tbl[i].svc_block_idx,
                                        p_ftl_header->ftl_cb.dev_tbl[i].svc_total_len,
                                        p_ftl_header->ftl_cb.dev_tbl[i].svc_crc16);
                    }
                    else
                    {
                        APP_PRINT_INFO1("dev_tbl[%d]: not used", i);
                    }
                }
            }
            else
            {
                APP_PRINT_INFO0("gattc_tbl_storage_print: no info");
            }
        }
    }
    if (p_ftl_header)
    {
        free(p_ftl_header);
    }
}
#endif

void gattc_tbl_bitmap_init(T_GATTC_FTL_CB *p_ftl_cb)
{
    uint16_t block_unused_num;
    p_ftl_cb->block_unused_num = ((BT_EXT_FTL_SIZE - sizeof(T_GATTC_FTL_HEADER) +
                                   GATTC_FTL_MAX_BLOCK_LEN - 1) / GATTC_FTL_MAX_BLOCK_LEN);
    block_unused_num = p_ftl_cb->block_unused_num;
    for (uint8_t i = 0; i < GATTC_FTL_BITMAP_MAX_NUM; i++)
    {
        if (block_unused_num >= 8)
        {
            p_ftl_cb->block_unused_bitmap[i] = 0xFF;
            block_unused_num -= 8;
        }
        else
        {
            p_ftl_cb->block_unused_bitmap[i] = (1 << block_unused_num) - 1;
            break;
        }
    }
#if GATTC_STORAGE_DEBUG
    APP_PRINT_INFO2("gattc_tbl_bitmap_init: p_ftl_cb->block_unused_num %d, block_unused_bitmap %b",
                    p_ftl_cb->block_unused_num,
                    TRACE_BINARY(GATTC_FTL_BITMAP_MAX_NUM, p_ftl_cb->block_unused_bitmap));
#endif
}

bool gattc_tbl_bitmap_free(T_GATTC_FTL_CB *p_ftl_cb, T_GATTC_FTL_DEV *p_ftl_dev)
{
    for (uint8_t i = 0; i < GATTC_FTL_BITMAP_MAX_NUM; i++)
    {
        if ((p_ftl_cb->block_unused_bitmap[i] & p_ftl_dev->block_used_bitmap[i]) == 0)
        {
            p_ftl_cb->block_unused_bitmap[i] |= p_ftl_dev->block_used_bitmap[i];
        }
        else
        {
            APP_PRINT_ERROR0("gattc_tbl_bitmap_free: no matching");
            return false;
        }
    }
    p_ftl_cb->block_unused_num += p_ftl_dev->block_used_num;
    return true;
}

bool gattc_tbl_bitmap_assign(T_GATTC_FTL_CB *p_ftl_cb, T_GATTC_FTL_DEV *p_ftl_dev,
                             uint8_t block_num)
{
    uint8_t block_idx = 0;
    uint8_t block_assign_num = 0;
    uint8_t block_unused_bitmap;
    if (p_ftl_cb->block_unused_num < block_num)
    {
        return false;
    }
    memset(p_ftl_dev->block_used_bitmap, 0, sizeof(uint8_t)*GATTC_FTL_BITMAP_MAX_NUM);
    for (uint8_t i = 0; i < GATTC_FTL_BITMAP_MAX_NUM; i++)
    {
        block_idx = 0;
        block_unused_bitmap = p_ftl_cb->block_unused_bitmap[i];
        while (block_unused_bitmap)
        {
            if (block_unused_bitmap & 0x01)
            {
                p_ftl_dev->block_used_bitmap[i] |= (1 << block_idx);
                p_ftl_cb->block_unused_bitmap[i] &= ~(1 << block_idx);
                block_assign_num++;
                if (block_assign_num == block_num)
                {
                    break;
                }
            }
            block_unused_bitmap >>= 1;
            block_idx++;
        }
        if (block_assign_num == block_num)
        {
            break;
        }
    }
    if (block_assign_num == block_num)
    {
        p_ftl_dev->block_used_num = block_num;
        p_ftl_cb->block_unused_num -= p_ftl_dev->block_used_num;
#if GATTC_STORAGE_DEBUG
        APP_PRINT_INFO4("gattc_tbl_bitmap_assign: block_unused_bitmap %b, block_unused_num %d, block_used_bitmap %b, block_used_num %d",
                        TRACE_BINARY(GATTC_FTL_BITMAP_MAX_NUM, p_ftl_cb->block_unused_bitmap),
                        p_ftl_cb->block_unused_num,
                        TRACE_BINARY(GATTC_FTL_BITMAP_MAX_NUM, p_ftl_dev->block_used_bitmap),
                        p_ftl_dev->block_used_num);
#endif
        return true;
    }
    else
    {
        return false;
    }
}

bool gattc_tbl_write_data(T_GATTC_FTL_DEV *p_ftl_dev, uint8_t block_start_idx,
                          uint8_t  *p_data, uint16_t write_len)
{
    uint16_t write_idx = 0;
    uint8_t error_idx = 1;
    uint8_t block_idx = 0;
    uint8_t used_block_idx = 0;
    uint8_t block_used_bitmap;
    for (uint8_t i = 0; i < GATTC_FTL_BITMAP_MAX_NUM; i++)
    {
        block_used_bitmap = p_ftl_dev->block_used_bitmap[i];
        while (block_used_bitmap)
        {
            if (block_used_bitmap & 0x01)
            {
                if (used_block_idx < block_start_idx)
                {
                    block_used_bitmap >>= 1;
                    block_idx++;
                    used_block_idx++;
                    continue;
                }

#if (FTL_POOL_ENABLE == 1)
                if (ftl_save_to_module(BT_EXT_FTL_PARTITION_NAME, p_data + write_idx,
                                       GATTC_FTL_CB_LEN + GATTC_FTL_MAX_BLOCK_LEN * block_idx,
                                       GATTC_FTL_MAX_BLOCK_LEN) != 0)
#else
                if (ftl_save_to_storage(p_data + write_idx,
                                        bt_ext_ftl_start_offset + GATTC_FTL_CB_LEN + GATTC_FTL_MAX_BLOCK_LEN * block_idx,
                                        GATTC_FTL_MAX_BLOCK_LEN) != 0)
#endif
                {
                    error_idx = 2;
                    goto failed;
                }
                else
                {
                    write_idx += GATTC_FTL_MAX_BLOCK_LEN;
                    if (write_idx >= write_len)
                    {
                        goto success;
                    }
                }
            }
            block_used_bitmap >>= 1;
            block_idx++;
        }
        if (write_idx >= write_len)
        {
            goto success;
        }
    }
failed:
    APP_PRINT_ERROR1("gattc_tbl_write_data: failed, error_idx %d", error_idx);
    return false;
success:
    return true;
}

uint8_t *gattc_tbl_read_data(T_GATTC_FTL_DEV *p_ftl_dev, uint8_t block_start_idx,
                             uint16_t svc_total_len)
{
    uint8_t *p_temp_buf = NULL;
    uint16_t read_len = svc_total_len;
    uint16_t read_idx = 0;
    uint8_t error_idx = 1;
    uint8_t block_idx = 0;
    uint8_t used_block_idx = 0;
    uint8_t block_used_bitmap;

    if ((read_len % GATTC_FTL_MAX_BLOCK_LEN) != 0)
    {
        read_len = (read_len / GATTC_FTL_MAX_BLOCK_LEN + 1) * GATTC_FTL_MAX_BLOCK_LEN;
    }
    p_temp_buf = calloc(1, read_len);
    if (p_temp_buf == NULL)
    {
        error_idx = 1;
        goto failed;
    }
    for (uint8_t i = 0; i < GATTC_FTL_BITMAP_MAX_NUM; i++)
    {
        block_used_bitmap = p_ftl_dev->block_used_bitmap[i];
        while (block_used_bitmap)
        {
            if (block_used_bitmap & 0x01)
            {
                if (used_block_idx < block_start_idx)
                {
                    block_used_bitmap >>= 1;
                    block_idx++;
                    used_block_idx++;
                    continue;
                }
#if (FTL_POOL_ENABLE == 1)
                if (ftl_load_from_module(BT_EXT_FTL_PARTITION_NAME, p_temp_buf + read_idx,
                                         GATTC_FTL_CB_LEN + GATTC_FTL_MAX_BLOCK_LEN * block_idx,
                                         GATTC_FTL_MAX_BLOCK_LEN) != 0)
#else
                if (ftl_load_from_storage(p_temp_buf + read_idx,
                                          bt_ext_ftl_start_offset + GATTC_FTL_CB_LEN + GATTC_FTL_MAX_BLOCK_LEN * block_idx,
                                          GATTC_FTL_MAX_BLOCK_LEN) != 0)
#endif
                {
                    error_idx = 2;
                    goto failed;
                }
                else
                {
                    read_idx += GATTC_FTL_MAX_BLOCK_LEN;
                    if (read_idx > read_len)
                    {
                        error_idx = 3;
                        goto failed;
                    }
                    else if (read_idx == read_len)
                    {
                        goto success;
                    }
                }
            }
            block_used_bitmap >>= 1;
            block_idx++;
        }
        if (read_idx > read_len)
        {
            error_idx = 4;
            goto failed;
        }
        else if (read_idx == read_len)
        {
            goto success;
        }
    }
failed:
    APP_PRINT_ERROR1("gattc_tbl_read_data: failed, error_idx %d", error_idx);
    if (p_temp_buf)
    {
        free(p_temp_buf);
    }
    return NULL;
success:
    return p_temp_buf;
}

T_GATTC_FTL_DEV *gattc_tbl_storage_find_dev(T_GATTC_FTL_CB *p_ftl_cb,
                                            uint8_t *identity_addr, uint8_t identity_addr_type)
{
    if (p_ftl_cb->sync_word != GATTC_FTL_SYNC_WORD)
    {
        return NULL;
    }
    for (uint8_t i = 0; i < GATTC_FTL_MAX_DEV_NUM; i++)
    {
        if (p_ftl_cb->dev_tbl[i].used && p_ftl_cb->dev_tbl[i].remote_bd_type == identity_addr_type &&
            memcmp(p_ftl_cb->dev_tbl[i].remote_bd, identity_addr, 6) == 0)
        {
            return &p_ftl_cb->dev_tbl[i];
        }
    }
    return NULL;
}

T_GATTC_FTL_DEV *gattc_tbl_storage_find_unused_dev(T_GATTC_FTL_CB *p_ftl_cb)
{
    if (p_ftl_cb->sync_word != GATTC_FTL_SYNC_WORD)
    {
        return NULL;
    }
    for (uint8_t i = 0; i < GATTC_FTL_MAX_DEV_NUM; i++)
    {
        if (p_ftl_cb->dev_tbl[i].used == false)
        {
            return &p_ftl_cb->dev_tbl[i];
        }
    }
    return NULL;
}

bool gattc_tbl_storage_save_header(T_GATTC_FTL_HEADER *p_ftl_header)
{
#if (FTL_POOL_ENABLE == 1)
    if (ftl_save_to_module(BT_EXT_FTL_PARTITION_NAME, p_ftl_header, 0, sizeof(T_GATTC_FTL_HEADER)) == 0)
#else
    if (ftl_save_to_storage(p_ftl_header, bt_ext_ftl_start_offset, sizeof(T_GATTC_FTL_HEADER)) == 0)
#endif
    {
        return true;
    }
    return false;
}

bool gattc_tbl_storage_clear(void)
{
    T_GATTC_FTL_HEADER ftl_header;
    memset(&ftl_header, 0, sizeof(ftl_header));

    APP_PRINT_INFO0("gattc_tbl_storage_clear");
    return gattc_tbl_storage_save_header(&ftl_header);
}

bool gattc_tbl_storage_remove_dev(T_GATTC_FTL_HEADER *p_ftl_header, T_GATTC_FTL_DEV *p_ftl_dev)
{
    if (gattc_tbl_bitmap_free(&p_ftl_header->ftl_cb, p_ftl_dev))
    {
        memset(p_ftl_dev, 0, sizeof(T_GATTC_FTL_DEV));
        return gattc_tbl_storage_save_header(p_ftl_header);
    }
    return false;
}

bool gattc_tbl_storage_remove(uint8_t *identity_addr, uint8_t identity_addr_type)
{
    T_GATTC_FTL_HEADER ftl_header;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
#if (FTL_POOL_ENABLE == 1)
    if (ftl_load_from_module(BT_EXT_FTL_PARTITION_NAME, &ftl_header, 0, sizeof(ftl_header)) != 0)
#else
    if (ftl_load_from_storage(&ftl_header, bt_ext_ftl_start_offset, sizeof(ftl_header)) != 0)
#endif
    {
        goto failed;
    }
    p_ftl_dev = gattc_tbl_storage_find_dev(&ftl_header.ftl_cb, identity_addr, identity_addr_type);
    if (p_ftl_dev)
    {
        if (gattc_tbl_storage_remove_dev(&ftl_header, p_ftl_dev) == false)
        {
            goto failed;
        }
    }
    else
    {
        goto failed;
    }
    APP_PRINT_INFO2("gattc_tbl_storage_remove: identity_addr_type %d, identity_addr %s",
                    identity_addr_type,
                    TRACE_BDADDR(identity_addr));
    return true;
failed:
    return false;
}

T_GATTC_FTL_HEADER *gattc_tbl_storage_get_header(bool is_new)
{
    uint8_t error_idx = 0;
    int32_t ret;
    T_GATTC_FTL_HEADER *p_ftl_header = calloc(1, sizeof(T_GATTC_FTL_HEADER));

    if (p_ftl_header == NULL)
    {
        error_idx = 1;
        goto failed;
    }
#if (FTL_POOL_ENABLE == 1)
    ret = ftl_load_from_module(BT_EXT_FTL_PARTITION_NAME, p_ftl_header, 0,
                               sizeof(T_GATTC_FTL_HEADER));
#else
    ret = ftl_load_from_storage(p_ftl_header, bt_ext_ftl_start_offset,
                                sizeof(T_GATTC_FTL_HEADER));
#endif
    if (ret == 0)
    {
        if (p_ftl_header->ftl_cb.sync_word != GATTC_FTL_SYNC_WORD ||
            p_ftl_header->ftl_cb.version != GATTC_FTL_MGR_VERSION)
        {
            if (p_ftl_header->ftl_cb.sync_word != 0 &&
                p_ftl_header->ftl_cb.version != 0)
            {
                gattc_tbl_storage_clear();
            }
            if (is_new)
            {
                p_ftl_header->ftl_cb.sync_word = 0;
            }
            else
            {
                error_idx = 2;
                goto failed;
            }
        }
    }
    else
    {
        if (is_new)
        {
            p_ftl_header->ftl_cb.sync_word = 0;
        }
        else
        {
            error_idx = 3;
            goto failed;
        }
    }
    return p_ftl_header;
failed:
    if (p_ftl_header)
    {
        free(p_ftl_header);
    }
    APP_PRINT_ERROR2("gattc_tbl_storage_get_header:failed, is_new %d, error_idx %d", is_new,
                     error_idx);
    return NULL;
}

bool gattc_tbl_storage_get_dev(T_GATT_STORAGE_SVC_TBL_GET_IND *p_get_ind)
{
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
    uint8_t error_idx = 0;
    uint8_t *p_temp_buf = NULL;
    uint16_t crc16 = 0;
    uint8_t  identity_addr[6];
    uint8_t  identity_addr_type;
    if (gattc_tbl_get_identity_addr(p_get_ind->addr, p_get_ind->remote_bd_type,
                                    identity_addr, &identity_addr_type) == false)
    {
        error_idx = 1;
        goto failed;
    }
    p_ftl_header = gattc_tbl_storage_get_header(false);

    if (p_ftl_header == NULL)
    {
        error_idx = 2;
        goto failed;
    }
    p_ftl_dev = gattc_tbl_storage_find_dev(&p_ftl_header->ftl_cb, identity_addr, identity_addr_type);
    if (p_ftl_dev == NULL)
    {
        error_idx = 4;
        goto failed;
    }

    p_temp_buf = gattc_tbl_read_data(p_ftl_dev, p_ftl_dev->svc_block_idx, p_ftl_dev->svc_total_len);

    if (p_temp_buf == NULL)
    {
        error_idx = 5;
        goto failed;
    }
    crc16 = btxfcs(crc16, p_temp_buf, p_ftl_dev->svc_total_len);

    if (p_ftl_dev->svc_crc16 != crc16)
    {
        gattc_tbl_storage_remove_dev(p_ftl_header, p_ftl_dev);
        error_idx = 9;
        goto failed;
    }
    p_get_ind->data_len = p_ftl_dev->svc_total_len;
    p_get_ind->p_data = p_temp_buf;
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return true;
failed:
    APP_PRINT_ERROR1("gattc_tbl_storage_get_dev:failed, error_idx %d", error_idx);
    if (p_temp_buf != NULL)
    {
        free(p_temp_buf);
    }
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return false;
}

bool gattc_tbl_storage_add_dev(T_GATT_STORAGE_SVC_TBL_SET_IND *p_set_ind)
{
    uint8_t error_idx = 0;
    uint16_t crc16 = 0;
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
    uint8_t block_num;
    uint8_t  identity_addr[6];
    uint8_t  identity_addr_type;
    if (gattc_tbl_get_identity_addr(p_set_ind->addr, p_set_ind->remote_bd_type,
                                    identity_addr, &identity_addr_type) == false)
    {
        error_idx = 1;
        goto failed;
    }

    p_ftl_header = gattc_tbl_storage_get_header(true);

    if (p_ftl_header == NULL)
    {
        error_idx = 2;
        goto failed;
    }
    crc16 = btxfcs(crc16, p_set_ind->p_data, p_set_ind->data_len);

    if (p_ftl_header->ftl_cb.sync_word != GATTC_FTL_SYNC_WORD)
    {
        memset(p_ftl_header, 0, sizeof(T_GATTC_FTL_HEADER));
        p_ftl_header->ftl_cb.sync_word = GATTC_FTL_SYNC_WORD;
        p_ftl_header->ftl_cb.version = GATTC_FTL_MGR_VERSION;
        gattc_tbl_bitmap_init(&p_ftl_header->ftl_cb);
    }
    else
    {
        p_ftl_dev = gattc_tbl_storage_find_dev(&p_ftl_header->ftl_cb, identity_addr, identity_addr_type);
        if (p_ftl_dev != NULL)
        {
            if (p_ftl_dev->svc_crc16 == crc16 && p_ftl_dev->svc_total_len == p_set_ind->data_len)
            {
                APP_PRINT_INFO0("gattc_tbl_storage_add_dev:already exist");
                free(p_ftl_header);
                return true;
            }
            else
            {
                APP_PRINT_INFO0("gattc_tbl_storage_add_dev: table changed");
                if (gattc_tbl_bitmap_free(&p_ftl_header->ftl_cb, p_ftl_dev))
                {
                    memset(p_ftl_dev, 0, sizeof(T_GATTC_FTL_DEV));
                }
                else
                {
                    error_idx = 10;
                    goto failed;
                }
            }
        }
    }
    p_ftl_dev = gattc_tbl_storage_find_unused_dev(&p_ftl_header->ftl_cb);
    if (p_ftl_dev == NULL)
    {
        error_idx = 3;
        goto failed;
    }
    p_ftl_dev->used = true;
    p_ftl_dev->remote_bd_type = identity_addr_type;
    memcpy(p_ftl_dev->remote_bd, identity_addr, 6);
    p_ftl_dev->svc_total_len = p_set_ind->data_len;
    p_ftl_dev->svc_crc16 = crc16;
    p_ftl_dev->cccd_data_len = p_set_ind->cccd_data_len;
    p_ftl_dev->cccd_block_num = (p_ftl_dev->cccd_data_len + GATTC_FTL_MAX_BLOCK_LEN - 1) /
                                GATTC_FTL_MAX_BLOCK_LEN;
    p_ftl_dev->cccd_crc16 = 0;

    if ((p_set_ind->data_len % GATTC_FTL_MAX_BLOCK_LEN) != 0)
    {
        block_num = p_set_ind->data_len / GATTC_FTL_MAX_BLOCK_LEN + 1;
    }
    else
    {
        block_num = p_set_ind->data_len / GATTC_FTL_MAX_BLOCK_LEN;
    }
    p_ftl_dev->svc_block_idx = p_ftl_dev->cccd_block_num;
    block_num += p_ftl_dev->cccd_block_num;

    if (gattc_tbl_bitmap_assign(&p_ftl_header->ftl_cb, p_ftl_dev, block_num) == false)
    {
        error_idx = 4;
        goto failed;
    }
    if (gattc_tbl_write_data(p_ftl_dev, p_ftl_dev->svc_block_idx, p_set_ind->p_data,
                             p_set_ind->data_len) == false)
    {
        error_idx = 7;
        goto failed;
    }
    if (gattc_tbl_storage_save_header(p_ftl_header) == false)
    {
        error_idx = 8;
        goto failed;
    }
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return true;
failed:
    APP_PRINT_ERROR1("gattc_tbl_storage_add_dev:failed, error_idx %d", error_idx);
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return false;
}

bool gattc_tbl_storage_get_cccd(T_GATT_STORAGE_SVC_CCCD_GET_IND *p_get_ind)
{
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
    uint8_t *p_cccd = NULL;
    uint8_t  error_idx = 0;
    uint8_t  identity_addr[6];
    uint8_t  identity_addr_type;
    uint16_t crc16 = 0;
    if (gattc_tbl_get_identity_addr(p_get_ind->addr, p_get_ind->remote_bd_type,
                                    identity_addr, &identity_addr_type) == false)
    {
        error_idx = 1;
        goto failed;
    }

    p_ftl_header = gattc_tbl_storage_get_header(false);

    if (p_ftl_header == NULL)
    {
        error_idx = 2;
        goto failed;
    }

    p_ftl_dev = gattc_tbl_storage_find_dev(&p_ftl_header->ftl_cb, identity_addr, identity_addr_type);
    if (p_ftl_dev == NULL)
    {
        error_idx = 4;
        goto failed;
    }
    if (p_ftl_dev->cccd_data_len && p_ftl_dev->cccd_crc16 != 0)
    {
        p_cccd = gattc_tbl_read_data(p_ftl_dev, 0, p_ftl_dev->cccd_data_len);
    }
    if (p_cccd)
    {
        crc16 = btxfcs(crc16, p_cccd, p_ftl_dev->cccd_data_len);
        if (p_ftl_dev->cccd_crc16 != crc16)
        {
            error_idx = 5;
            goto failed;
        }
        p_get_ind->data_len = p_ftl_dev->cccd_data_len;
        p_get_ind->p_data = p_cccd;
    }
    else
    {
        p_get_ind->data_len = 0;
        p_get_ind->p_data = NULL;
    }

    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return true;
failed:
    APP_PRINT_ERROR1("gattc_tbl_storage_get_cccd:failed, error_idx %d", error_idx);
    if (p_cccd)
    {
        free(p_cccd);
    }
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return false;
}

bool gattc_tbl_storage_set_cccd(T_GATT_STORAGE_SVC_CCCD_SET_IND *p_set_ind)
{
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
    uint16_t crc16 = 0;
    uint8_t  error_idx = 0;
    uint8_t  identity_addr[6];
    uint8_t  identity_addr_type;
    if (gattc_tbl_get_identity_addr(p_set_ind->addr, p_set_ind->remote_bd_type,
                                    identity_addr, &identity_addr_type) == false)
    {
        error_idx = 1;
        goto failed;
    }
    p_ftl_header = gattc_tbl_storage_get_header(false);

    if (p_ftl_header == NULL)
    {
        error_idx = 2;
        goto failed;
    }

    p_ftl_dev = gattc_tbl_storage_find_dev(&p_ftl_header->ftl_cb, identity_addr, identity_addr_type);
    if (p_ftl_dev == NULL)
    {
        error_idx = 3;
        goto failed;
    }

    if ((p_ftl_dev->cccd_block_num * GATTC_FTL_MAX_BLOCK_LEN) < p_set_ind->data_len ||
        p_set_ind->data_len == 0)
    {
        error_idx = 4;
        goto failed;
    }
    crc16 = btxfcs(crc16, p_set_ind->p_data, p_set_ind->data_len);
    if (gattc_tbl_write_data(p_ftl_dev, 0, p_set_ind->p_data, p_set_ind->data_len) == false)
    {
        error_idx = 5;
        goto failed;
    }
    p_ftl_dev->cccd_data_len = p_set_ind->data_len;
    p_ftl_dev->cccd_crc16 = crc16;
    if (gattc_tbl_storage_save_header(p_ftl_header) == false)
    {
        error_idx = 8;
        goto failed;
    }
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return true;
failed:
    APP_PRINT_ERROR1("gattc_tbl_storage_set_cccd:failed, error_idx %d", error_idx);
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return false;
}

bool gattc_tbl_storage_get_gatt_data(T_GATT_STORAGE_SVC_GATT_GET_IND *p_get_ind)
{
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
    uint8_t  error_idx = 0;
    uint8_t  identity_addr[6];
    uint8_t  identity_addr_type;

    if (gattc_tbl_get_identity_addr(p_get_ind->addr, p_get_ind->remote_bd_type,
                                    identity_addr, &identity_addr_type) == false)
    {
        error_idx = 1;
        goto failed;
    }

    p_ftl_header = gattc_tbl_storage_get_header(false);

    if (p_ftl_header == NULL)
    {
        error_idx = 2;
        goto failed;
    }

    p_ftl_dev = gattc_tbl_storage_find_dev(&p_ftl_header->ftl_cb, identity_addr, identity_addr_type);
    if (p_ftl_dev == NULL)
    {
        error_idx = 4;
        goto failed;
    }
    memcpy(p_get_ind->p_gatt_data, &p_ftl_dev->gatt_data, sizeof(T_GATT_SERVICE_DATA));

    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return true;
failed:
    APP_PRINT_ERROR1("gattc_tbl_storage_get_gatt_data:failed, error_idx %d", error_idx);
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return false;
}

bool gattc_tbl_storage_set_gatt_data(T_GATT_STORAGE_SVC_GATT_SET_IND *p_set_ind)
{
    T_GATTC_FTL_HEADER *p_ftl_header = NULL;
    T_GATTC_FTL_DEV *p_ftl_dev = NULL;
    uint8_t  error_idx = 0;
    uint8_t  identity_addr[6];
    uint8_t  identity_addr_type;

    if (gattc_tbl_get_identity_addr(p_set_ind->addr, p_set_ind->remote_bd_type,
                                    identity_addr, &identity_addr_type) == false)
    {
        error_idx = 1;
        goto failed;
    }
    p_ftl_header = gattc_tbl_storage_get_header(false);

    if (p_ftl_header == NULL)
    {
        error_idx = 2;
        goto failed;
    }

    p_ftl_dev = gattc_tbl_storage_find_dev(&p_ftl_header->ftl_cb, identity_addr, identity_addr_type);
    if (p_ftl_dev == NULL)
    {
        error_idx = 3;
        goto failed;
    }

    if (memcmp(p_set_ind->p_gatt_data, &p_ftl_dev->gatt_data, sizeof(T_GATT_SERVICE_DATA)) != 0)
    {
        memcpy(&p_ftl_dev->gatt_data, p_set_ind->p_gatt_data, sizeof(T_GATT_SERVICE_DATA));
        if (gattc_tbl_storage_save_header(p_ftl_header) == false)
        {
            error_idx = 8;
            goto failed;
        }
    }

    if (gattc_tbl_storage_save_header(p_ftl_header) == false)
    {
        error_idx = 8;
        goto failed;
    }

    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return true;
failed:
    APP_PRINT_ERROR1("gattc_tbl_storage_set_gatt_data:failed, error_idx %d", error_idx);
    if (p_ftl_header != NULL)
    {
        free(p_ftl_header);
    }
    return false;
}

T_APP_RESULT gattc_tbl_storage_cb(T_GATT_STORAGE_EVENT type, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_REJECT;
#if GATTC_STORAGE_DEBUG
    gattc_tbl_storage_print();
    APP_PRINT_ERROR1("gattc_tbl_storage_cb:type %d", type);
#endif
    switch (type)
    {
    case GATT_STORAGE_EVENT_SVC_TBL_GET_IND:
        {
            T_GATT_STORAGE_SVC_TBL_GET_IND *p_get_ind = (T_GATT_STORAGE_SVC_TBL_GET_IND *)p_data;
            if (gattc_tbl_storage_get_dev(p_get_ind))
            {
                app_result = APP_RESULT_SUCCESS;
            }
        }
        break;

    case GATT_STORAGE_EVENT_SVC_TBL_SET_IND:
        {
            T_GATT_STORAGE_SVC_TBL_SET_IND *p_set_ind = (T_GATT_STORAGE_SVC_TBL_SET_IND *)p_data;
            if (gattc_tbl_storage_add_dev(p_set_ind))
            {
                app_result = APP_RESULT_SUCCESS;
            }
        }
        break;

    case GATT_STORAGE_EVENT_SVC_CCCD_GET_IND:
        {
            T_GATT_STORAGE_SVC_CCCD_GET_IND *p_get_ind = (T_GATT_STORAGE_SVC_CCCD_GET_IND *)p_data;
            if (gattc_tbl_storage_get_cccd(p_get_ind))
            {
                app_result = APP_RESULT_SUCCESS;
            }
        }
        break;

    case GATT_STORAGE_EVENT_SVC_CCCD_SET_IND:
        {
            T_GATT_STORAGE_SVC_CCCD_SET_IND *p_set_ind = (T_GATT_STORAGE_SVC_CCCD_SET_IND *)p_data;
            if (gattc_tbl_storage_set_cccd(p_set_ind))
            {
                app_result = APP_RESULT_SUCCESS;
            }
        }
        break;

    case GATT_STORAGE_EVENT_SVC_GATT_GET_IND:
        {
            T_GATT_STORAGE_SVC_GATT_GET_IND *p_get_ind = (T_GATT_STORAGE_SVC_GATT_GET_IND *)p_data;
            if (gattc_tbl_storage_get_gatt_data(p_get_ind))
            {
                app_result = APP_RESULT_SUCCESS;
            }
        }
        break;

    case GATT_STORAGE_EVENT_SVC_GATT_SET_IND:
        {
            T_GATT_STORAGE_SVC_GATT_SET_IND *p_set_ind = (T_GATT_STORAGE_SVC_GATT_SET_IND *)p_data;
            if (gattc_tbl_storage_set_gatt_data(p_set_ind))
            {
                app_result = APP_RESULT_SUCCESS;
            }
        }
        break;
    default:
        break;
    }
#if GATTC_STORAGE_DEBUG
    gattc_tbl_storage_print();
#endif
    return app_result;
}

#if F_APP_BOND_MGR_SUPPORT
void gattc_tbl_storage_bond_cb(uint8_t cb_type, void *p_cb_data)
{
    T_BT_LE_BOND_CB_DATA cb_data;
    T_LE_BOND_ENTRY *p_entry = NULL;
    memcpy(&cb_data, p_cb_data, sizeof(T_BT_LE_BOND_CB_DATA));
    APP_PRINT_INFO1("gattc_tbl_storage_bond_cb: cb_type = %d", cb_type);
    switch (cb_type)
    {
    case BT_BOND_MSG_LE_BOND_UPDATE:
        {
            if (cb_data.p_le_bond_update->p_entry &&
                (cb_data.p_le_bond_update->modify_flags & LE_BOND_MODIFY_REPAIR))
            {
                p_entry = cb_data.p_le_bond_update->p_entry;
                gattc_tbl_storage_remove(p_entry->remote_identity_addr,
                                         p_entry->remote_identity_addr_type);
            }
        }
        break;

    case BT_BOND_MSG_LE_BOND_REMOVE:
        {
            if (cb_data.p_le_bond_remove->p_entry)
            {
                p_entry = cb_data.p_le_bond_remove->p_entry;
                if (bt_le_find_key_entry_num(p_entry->remote_identity_addr,
                                             p_entry->remote_identity_addr_type) == 1)
                {
                    gattc_tbl_storage_remove(p_entry->remote_identity_addr,
                                             p_entry->remote_identity_addr_type);
                }
            }
        }
        break;

    case BT_BOND_MSG_LE_BOND_CLEAR:
        {
            gattc_tbl_storage_clear();
        }
        break;

    default:
        break;
    }
    return;
}
#endif

void gattc_tbl_storage_init(void)
{
    bt_ext_ftl_init();
#ifndef FTL_POOL_ENABLE
    if (bt_ext_ftl_start_offset != 0)
#endif
    {
        gatt_client_storage_register(gattc_tbl_storage_cb);
    }
#if F_APP_BOND_MGR_SUPPORT
    bt_bond_register_app_cb(gattc_tbl_storage_bond_cb);
#endif
}
#if !F_APP_BOND_MGR_SUPPORT
void gattc_tbl_storage_handle_bond_modify(T_LE_BOND_MODIFY_INFO *p_info)
{
#ifndef FTL_POOL_ENABLE
    if (bt_ext_ftl_start_offset == 0)
    {
        return;
    }
#endif

    if (p_info->type == LE_BOND_CLEAR)
    {
        gattc_tbl_storage_clear();
    }
    else if (p_info->type == LE_BOND_DELETE ||
             p_info->type == LE_BOND_ADD)
    {
        if (p_info->p_entry)
        {
            uint8_t  identity_addr[6];
            uint8_t  identity_addr_type;
            if (gattc_tbl_get_identity_addr(p_info->p_entry->remote_bd.addr,
                                            p_info->p_entry->remote_bd.remote_bd_type,
                                            identity_addr, &identity_addr_type) == false)
            {
                return;
            }
            gattc_tbl_storage_remove(identity_addr,
                                     identity_addr_type);
        }
    }
}
#endif

void gattc_tbl_storage_deinit(void)
{
    bt_ext_ftl_deinit();
    gatt_client_storage_register(NULL);
#if F_APP_BOND_MGR_SUPPORT
    bt_bond_unregister_app_cb(gattc_tbl_storage_bond_cb);
#endif
}
#endif
