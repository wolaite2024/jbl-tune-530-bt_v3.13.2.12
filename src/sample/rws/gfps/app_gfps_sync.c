
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FEATURE_SUPPORT
#include "app_relay.h"
#include <string.h>
#include "trace.h"
#include "app_cfg.h"
#include "app_gfps.h"
#include "app_gfps_sync.h"
#include "app_gfps_account_key.h"
#include "app_gfps_personalized_name.h"
#include "app_gfps_msg.h"
#include "app_main.h"
#if GFPS_LE_DEVICE_SUPPORT
#include "app_lea_adv.h"
#include "app_lea_mgr.h"
#include "gap_bond_le.h"
#endif

#if F_APP_ERWS_SUPPORT
void app_gfps_sync_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                               T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE2("app_gfps_sync_parse_cback: msg_type 0x%04x, status %d", msg_type, status);
    switch (msg_type)
    {

    case APP_REMOTE_MSG_GFPS_ACCOUNT_KEY:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.gfps_support && buf != NULL && len != 0)
            {
                app_gfps_account_key_remote_handle_sync(buf, len);

#if GFPS_LE_DEVICE_SUPPORT
                if (extend_app_cfg_const.gfps_le_device_support)
                {
                    T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

                    if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
                         gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
                        && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
                    }
                }
#endif
            }
        }
        break;

    case APP_REMOTE_MSG_GFPS_ACCOUNT_KEY_ADD:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.gfps_support)
            {
                if (len == 22 && buf != NULL)
                {
                    uint8_t key[16] = {0};//16 bytes account key
                    uint8_t  bd_addr[6] = {0};//6 bytes address

                    memcpy(key, buf, 16);
                    memcpy(bd_addr, buf + 16, 6);
                    app_gfps_account_key_remote_handle_add(key, bd_addr);

#if GFPS_LE_DEVICE_SUPPORT
                    if (extend_app_cfg_const.gfps_le_device_support)
                    {
                        T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

                        if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
                             gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
                            && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                        {
                            app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
                        }
                    }
#endif
                }
            }
        }
        break;

#if GFPS_PERSONALIZED_NAME_SUPPORT
    case APP_REMOTE_MSG_GFPS_PERSONALIZED_NAME:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (extend_app_cfg_const.gfps_support && buf != NULL)
            {
                app_gfps_personalized_name_store(buf, len);
            }
        }
        break;
#endif

#if GFPS_LE_DEVICE_SUPPORT
    case APP_REMOTE_MSG_GFPS_SUBPAIR_SEC_ADV_ENABLE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (extend_app_cfg_const.gfps_support && extend_app_cfg_const.gfps_le_device_support)
                {
                    T_GFPS_LE_DEVICE_MODE gfps_le_device_mode = gfps_le_get_device_mode();

                    if ((gfps_le_device_mode == GFPS_LE_DEVICE_MODE_LE_MODE_WITH_LEA ||
                         gfps_le_device_mode == GFPS_LE_DEVICE_MODE_DUAL_MODE_WITH_LEA)
                        && app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        app_gfps_le_device_adv_start((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
                    }
                }
            }
        }
        break;
#endif

#if GFPS_SASS_SUPPORT
    case GFPS_RFC_REMOTE_MSG_SYNC_CUSTOM_DATA:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_gfps_msg_update_custom_data(*buf);
            }
        }
        break;
    case GFPS_RFC_REMOTE_MSG_SYNC_TARGET_DROP_DEVICE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_gfps_msg_update_target_drop_device(0xFF, buf);
            }
        }
        break;
#endif

#if GFPS_LE_DEVICE_SUPPORT
    case APP_REMOTE_MSG_GFPS_ADDITIONAL_BOND_INFO:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD && extend_app_cfg_const.gfps_le_device_support)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_gfps_sec_handle_addition_bond_info(buf);
                }
                else
                {
                    app_gfps_pri_handle_additional_bond_info(buf);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_GFPS_ADDITIONAL_IO_CAP:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                bool change_io_cap = buf[0];
                app_gfps_sec_handle_additional_io_cap(change_io_cap);
            }
        }
        break;
#endif

    default:
        break;
    }
}

uint16_t app_gfps_sync_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    uint8_t name[GFPS_PERSONALIZED_NAME_MAX_LEN];
    uint8_t name_len;
#if GFPS_SASS_SUPPORT
    T_SASS_CONN_STATUS_FIELD conn_status;
    uint8_t tmp[20] = {0};
#endif

    APP_PRINT_INFO1("app_gfps_sync_relay_cback: msg_type %d", msg_type);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_GFPS_ACCOUNT_KEY:
        {
            uint8_t gfps_account_key_table_size = app_gfps_account_key_get_table_size();
            T_ACCOUNT_KEY *account_key         = app_gfps_account_key_get_table();

            payload_len = gfps_account_key_table_size;
            msg_ptr = (uint8_t *)account_key;
            skip = false;
        }
        break;

    case APP_REMOTE_MSG_GFPS_PERSONALIZED_NAME:
        {
            if (app_gfps_personalized_name_read(name, &name_len) == APP_GFPS_PERSONALIZED_NAME_SUCCESS)
            {
                payload_len = name_len;
                msg_ptr = name;
                skip = false;
            }
            else
            {
                payload_len = 0;
                msg_ptr = NULL;
                skip = true;
            }
        }
        break;

#if GFPS_SASS_SUPPORT
    case GFPS_RFC_REMOTE_MSG_SYNC_CUSTOM_DATA:
        {
            gfps_sass_get_conn_status(&conn_status);
            skip = false;
            payload_len = 1 ;
            msg_ptr = &conn_status.custom_data;
        }
        break;
    case GFPS_RFC_REMOTE_MSG_SYNC_TARGET_DROP_DEVICE:
        {
            skip = false;
            payload_len = 6;
            //TODO: Get target drop device
            memcpy(tmp, app_db.sass_target_drop_device, 6);
            msg_ptr = tmp;
        }
        break;
#endif

    default:
        break;
    }

    uint16_t msg_len = payload_len + 4;
    if ((total == true) && (skip == true))
    {
        msg_len = 0;
    }

    if (buf != NULL)
    {
        if (((total == true) && (skip == false)) || (total == false))
        {
            buf[0] = (uint8_t)(msg_len & 0xFF);
            buf[1] = (uint8_t)(msg_len >> 8);
            buf[2] = APP_MODULE_TYPE_GFPS;
            buf[3] = msg_type;
            if (payload_len != 0 && msg_ptr != NULL)
            {
                memcpy(&buf[4], msg_ptr, payload_len);
            }
        }
        APP_PRINT_INFO1("app_gfps_sync_relay_cback: buf %b", TRACE_BINARY(msg_len, buf));
    }
    return msg_len;
}

bool app_gfps_sync_relay_init(void)
{
    bool ret = app_relay_cback_register(app_gfps_sync_relay_cback, app_gfps_sync_parse_cback,
                                        APP_MODULE_TYPE_GFPS, APP_REMOTE_MSG_GFPS_MAX_MSG_NUM);
    return ret;
}
#endif
#endif
