/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_TUYA_SUPPORT
#include "string.h"
#include "trace.h"
#include "app_cfg.h"
#include "remote.h"
#include "app_bt_policy_api.h"
#include "app_device.h"
#include "app_main.h"
#include "app_ble_gap.h"
#include "rtk_tuya_ble_adv.h"
#include "ble_conn.h"
#include "rtk_tuya_ble_device.h"
#include "tuya_ble_api.h"
#include "console.h"
#include "rtk_tuya_app_relay.h"
#include "app_relay.h"
#include "os_mem.h"
#include "os_msg.h"
#include "ftl.h"
#include "tuya_ble_secure.h"
#include "rtk_tuya_ble_ota.h"
void app_tuya_handle_power_off(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_tuya_adv_stop(APP_STOP_ADV_CAUSE_TUYA);
    }
}

void app_tuya_handle_power_on()
{
#if RTK_TUYA_AES_TEST_DEMO
    void rtk_tuya_aes_test(void);
    rtk_tuya_aes_test();
#endif

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
    else
    {
        //rws mode, wait role decided info, handle in API app_tuya_handle_engage_role_decided
    }
}

void app_tuya_handle_radio_mode(T_BT_DEVICE_MODE radio_mode)
{

}

void app_tuya_handle_enter_pair_mode(void)
{
    APP_PRINT_INFO0("app_tuya_handle_enter_pair_mode");

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
}

void app_tuya_handle_engage_role_decided(void)
{
    APP_PRINT_INFO0("app_tuya_handle_engage_role_decided");
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_tuya_adv_stop(APP_STOP_ADV_CAUSE_TUYA);

        uint8_t conn_id = le_tuya_adv_get_conn_id();
        if (conn_id != 0xFF)
        {
            T_APP_LE_LINK *p_link;
            p_link = app_link_find_le_link_by_conn_id(conn_id);

            if (p_link != NULL)
            {
                app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_SECONDARY);
            }
        }
    }
}

void app_tuya_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
    else if (device_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        le_tuya_adv_stop(APP_STOP_ADV_CAUSE_TUYA);

        uint8_t conn_id = le_tuya_adv_get_conn_id();
        if (conn_id != 0xFF)
        {
            T_APP_LE_LINK *p_link;
            p_link = app_link_find_le_link_by_conn_id(conn_id);

            if (p_link != NULL)
            {
                app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_SECONDARY);
            }
        }
    }
}

void app_tuya_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role)
{
    if (device_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
}

void app_tuya_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("app_tuya_handle_remote_conn_cmpl:pri sync all info to sec");
        app_tuya_relay_all();
        // T_APP_RELAY_MSG_LIST_HANDLE relay_msg_handle = app_relay_async_build();
        // app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_AUTH_INFO);
        // app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_AUTH_INFO_BACKUP);
        // app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_SYS_INFO);
        // app_relay_async_add(relay_msg_handle, APP_MODULE_TYPE_TUYA, APP_REMOTE_MSG_TUYA_SYS_INFO_BACKUP);
        // app_relay_async_send(relay_msg_handle);
    }
}

void app_tuya_handle_remote_disconn_cmpl(void)
{

}

void app_tuya_handle_b2s_bt_disconnected(uint8_t *bd_addr)
{
    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
}

void app_tuya_handle_b2s_ble_connected(uint8_t conn_id)
{
    APP_PRINT_INFO0("app_tuya_handle_b2s_ble_connected");
    tuya_ble_connected_handler();
    ble_set_prefer_conn_param(conn_id, 12, 12, 0, 200);
}

void app_tuya_handle_b2s_ble_disconnected(uint8_t *bd_addr, uint8_t conn_id,
                                          uint8_t local_disc_cause, uint16_t disc_cause)
{
    APP_PRINT_INFO0("app_tuya_handle_b2s_ble_disconnected");
    tuya_ble_disconnected_handler();
    if (tuya_ota_is_busy())
    {
        tuya_ota_exit();
    }

    if (conn_id == le_tuya_adv_get_conn_id())
    {
        le_tuya_adv_reset_conn_id();
    }

    if (local_disc_cause != LE_LOCAL_DISC_CAUSE_POWER_OFF &&
        local_disc_cause != LE_LOCAL_DISC_CAUSE_ROLESWAP)
    {
        le_tuya_adv_start(extend_app_cfg_const.tuya_adv_timeout);
    }
}
/*clear tuya info stored in ftl */
void app_tuya_handle_factory_reset(void)
{
    APP_PRINT_WARN0("app_tuya_handle_factory_reset");
    uint8_t *p_tuya_nv_data = NULL;
    p_tuya_nv_data = os_mem_zalloc(RAM_TYPE_DATA_ON, TUYA_NV_AREA_SIZE);

    if (p_tuya_nv_data)
    {
        memset(p_tuya_nv_data, 0, TUYA_NV_AREA_SIZE);
        app_tuya_ext_save_ftl_data(p_tuya_nv_data, TUYA_NV_START_ADDR, TUYA_NV_AREA_SIZE);

        os_mem_free(p_tuya_nv_data);
        p_tuya_nv_data = NULL;
    }
}
bool rtk_tuya_uart_receive_data(uint8_t *buf, uint32_t len)
{
    APP_PRINT_INFO1("rtk_tuya_uart_receive_data: buf %b", TRACE_BINARY(len, buf));
    tuya_ble_common_uart_receive_data(buf, len);
    tuya_ble_common_uart_send_data(buf, len); ///danni for test
    return true;
}

bool tuya_cmd_register(void)
{
    return console_register_parser(&rtk_tuya_uart_receive_data);
}

void rtk_tuya_handle_event_msg(T_IO_MSG io_msg)
{
    if (io_msg.u.buf)
    {
        APP_PRINT_INFO2("rtk_tuya_handle_event_msg: addr 0x%x,evt %d", io_msg.u.buf,
                        (*((tuya_ble_evt_param_t *)io_msg.u.buf)).hdr.event);
        tuya_ble_event_process((tuya_ble_evt_param_t *)io_msg.u.buf);
    }
    else
    {
        APP_PRINT_ERROR0("rtk_tuya_handle_event_msg: fail");
    }

    if (io_msg.u.buf)
    {
        os_mem_free(io_msg.u.buf);
        io_msg.u.buf = NULL;
    }
}
#endif
