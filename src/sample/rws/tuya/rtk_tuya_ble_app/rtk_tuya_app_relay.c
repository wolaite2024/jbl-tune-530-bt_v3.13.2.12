#if F_APP_TUYA_SUPPORT
#include "app_relay.h"
#include "trace.h"
#include "app_cfg.h"
#include "rtk_tuya_app_relay.h"
#include "string.h"
#include "rtk_tuya_ble_config.h"
#include "ftl.h"
#include "os_mem.h"
#include "tuya_ble_internal_config.h"
#include "tuya_ble_api.h"

void app_tuya_relay_all(void)
{
    APP_PRINT_INFO0("app_tuya_relay_all");
    app_relay_async_single(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_AUTH_INFO);
    app_relay_async_single(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_AUTH_INFO_BACKUP);
    app_relay_async_single(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_SYS_INFO);
    app_relay_async_single(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_SYS_INFO_BACKUP);
}

tuya_ble_status_t app_tuya_info_sync_to_remote(uint32_t addr, uint8_t *p_data, uint32_t size)
{
    //sync tuya info to secondary
    tuya_ble_status_t ret = TUYA_BLE_SUCCESS;

    uint8_t *p_tuya_info = os_mem_zalloc(RAM_TYPE_DATA_ON, size);
    if (p_tuya_info == NULL)
    {
        return TUYA_BLE_ERR_NO_MEM;
    }
    memcpy(p_tuya_info, p_data, size);

    if (addr == TUYA_BLE_AUTH_FLASH_ADDR)
    {
        if (size != sizeof(tuya_ble_auth_settings_t))
        {
            ret = TUYA_BLE_ERR_DATA_SIZE;
            goto err;
        }

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_AUTH_INFO,
                                            p_tuya_info, size);
    }
    else if (addr == TUYA_BLE_AUTH_FLASH_BACKUP_ADDR)
    {
        if (size != sizeof(tuya_ble_auth_settings_t))
        {
            ret = TUYA_BLE_ERR_DATA_SIZE;
            goto err;
        }

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_AUTH_INFO_BACKUP,
                                            p_tuya_info, size);
    }
    else if (addr == TUYA_BLE_SYS_FLASH_ADDR)
    {
        if (size != sizeof(tuya_ble_sys_settings_t))
        {
            ret = TUYA_BLE_ERR_DATA_SIZE;
            goto err;
        }

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_SYS_INFO,
                                            p_tuya_info, size);
    }
    else if (addr == TUYA_BLE_SYS_FLASH_BACKUP_ADDR)
    {
        if (size != sizeof(tuya_ble_sys_settings_t))
        {
            ret = TUYA_BLE_ERR_DATA_SIZE;
            goto err;
        }

        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_SYS_INFO_BACKUP,
                                            p_tuya_info, size);
    }
    else
    {
        ret = TUYA_BLE_ERR_INVALID_ADDR;
        goto err;
    }

    if (p_tuya_info)
    {
        os_mem_free(p_tuya_info);
        p_tuya_info = NULL;
    }
    APP_PRINT_INFO2("app_tuya_info_sync_to_remote: success addr 0x%x,size %d", addr, size);
    return TUYA_BLE_SUCCESS;

err:
    if (p_tuya_info)
    {
        os_mem_free(p_tuya_info);
        p_tuya_info = NULL;
    }
    APP_PRINT_ERROR2("app_tuya_info_sync_to_remote: err %d,addr 0x%x", ret, addr);
    return ret;
}

void app_tuya_remote_handle_info_sync(uint32_t addr, uint8_t *info_data, uint8_t info_len)
{
    if (info_data == NULL)
    {
        APP_PRINT_ERROR0("app_tuya_remote_handle_info_sync: err");
        return;
    }

    if (app_tuya_ext_save_ftl_data((void *)info_data, addr, info_len) == 0)
    {
        APP_PRINT_INFO2("app_tuya_remote_handle_info_sync: success addr 0x%x,size %d", addr, info_len);
    }
}


void app_tuya_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                          T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_tuya_parse_cback: msg_type 0x%04x, status %d", msg_type, status);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_TUYA_AUTH_INFO:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.tuya_support && buf != NULL && len != 0)
            {
                uint32_t addr = TUYA_BLE_AUTH_FLASH_ADDR;
                app_tuya_remote_handle_info_sync(addr, buf, len);
            }
        }
        break;
    case APP_REMOTE_MSG_TUYA_AUTH_INFO_BACKUP:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.tuya_support && buf != NULL && len != 0)
            {
                uint32_t addr = TUYA_BLE_AUTH_FLASH_BACKUP_ADDR;
                app_tuya_remote_handle_info_sync(addr, buf, len);
            }
        }
        break;
    case APP_REMOTE_MSG_TUYA_SYS_INFO:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.tuya_support && buf != NULL && len != 0)
            {
                uint32_t addr = TUYA_BLE_SYS_FLASH_ADDR;
                app_tuya_remote_handle_info_sync(addr, buf, len);
            }
        }
        break;
    case APP_REMOTE_MSG_TUYA_SYS_INFO_BACKUP:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.tuya_support && buf != NULL && len != 0)
            {
                uint32_t addr = TUYA_BLE_SYS_FLASH_BACKUP_ADDR;
                app_tuya_remote_handle_info_sync(addr, buf, len);
            }
        }
        break;

    default:
        break;
    }
}

uint16_t app_tuya_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    APP_PRINT_INFO1("app_tuya_relay_cback: msg_type %d", msg_type);
    uint8_t *p_tuya_info = os_mem_zalloc(RAM_TYPE_DATA_ON, TUYA_NV_ERASE_MIN_SIZE);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_TUYA_AUTH_INFO:
        {
            payload_len = (uint8_t)sizeof(tuya_ble_auth_settings_t);

            if (p_tuya_info &&
                app_tuya_ext_load_ftl_data((uint8_t *)p_tuya_info, TUYA_BLE_AUTH_FLASH_ADDR, payload_len) == 0)
            {
                msg_ptr = (uint8_t *)p_tuya_info;
            }
        }
        break;
    case APP_REMOTE_MSG_TUYA_AUTH_INFO_BACKUP:
        {
            payload_len = (uint8_t)sizeof(tuya_ble_auth_settings_t);

            if (p_tuya_info &&
                app_tuya_ext_load_ftl_data((uint8_t *)p_tuya_info, TUYA_BLE_AUTH_FLASH_BACKUP_ADDR,
                                           payload_len) == 0)
            {
                msg_ptr = (uint8_t *)p_tuya_info;
            }
        }
        break;
    case APP_REMOTE_MSG_TUYA_SYS_INFO:
        {
            payload_len = (uint8_t)sizeof(tuya_ble_sys_settings_t);

            if (p_tuya_info &&
                app_tuya_ext_load_ftl_data((uint8_t *)p_tuya_info, TUYA_BLE_SYS_FLASH_ADDR, payload_len) == 0)
            {
                msg_ptr = (uint8_t *)p_tuya_info;
            }
        }
        break;
    case APP_REMOTE_MSG_TUYA_SYS_INFO_BACKUP:
        {
            payload_len = (uint8_t)sizeof(tuya_ble_sys_settings_t);

            if (p_tuya_info &&
                app_tuya_ext_load_ftl_data((uint8_t *)p_tuya_info, TUYA_BLE_SYS_FLASH_BACKUP_ADDR,
                                           payload_len) == 0)
            {
                msg_ptr = (uint8_t *)p_tuya_info;
            }
        }
        break;

    default:
        break;
    }

    uint16_t msg_len;
    msg_len = app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_TUYA, payload_len, msg_ptr, skip,
                                 total);
    if (p_tuya_info != NULL)
    {
        os_mem_free(p_tuya_info);
        p_tuya_info = NULL;
    }
    return msg_len;
}

bool app_tuya_relay_init(void)
{
    bool ret = app_relay_cback_register(app_tuya_relay_cback, app_tuya_parse_cback,
                                        APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_MAX_MSG_NUM);

    return ret;
}
#endif
