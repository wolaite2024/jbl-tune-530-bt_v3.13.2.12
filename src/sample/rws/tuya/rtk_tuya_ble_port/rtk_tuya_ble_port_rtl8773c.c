#if F_APP_TUYA_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tuya_ble_port.h"
#include "tuya_ble_type.h"
#include "os_sched.h"
#include "os_sync.h"
#include "rtl876x_wdg.h"
#include "os_mem.h"
#include "os_msg.h"
#include "os_task.h"
#include "aes_api.h"
#include "md5.h"
#include "hmac.h"
#include "trace.h"
#include "ftl.h"
#include "errno.h"
#include "os_timer.h"
#include "tuya_ble_internal_config.h"
#include "rtk_tuya_ble_port_rtl8773c.h"
#include "rtk_tuya_ble_adv.h"
#include "rtk_tuya_app_ext_ftl.h"
#include "ble_ext_adv.h"
#include "app_main.h"
#include "app_ble_gap.h"
#include "rtk_tuya_ble_service.h"
#include "platform_utils.h"
#include "wdg.h"
#include "app_msg.h"
#include "aes_cbc_api.h"
#include "console.h"
#include "app_relay.h"
#include "rtk_tuya_app_relay.h"
#include "app_cfg.h"
tuya_ble_status_t tuya_ble_gap_advertising_adv_data_update(uint8_t const *p_ad_data, uint8_t ad_len)
{
    ble_ext_adv_mgr_set_adv_data(le_tuya_adv_get_adv_handle(), ad_len, (uint8_t *)p_ad_data);
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_advertising_scan_rsp_data_update(uint8_t const *p_sr_data,
                                                                uint8_t sr_len)
{
    ble_ext_adv_mgr_set_scan_response_data(le_tuya_adv_get_adv_handle(), sr_len, (uint8_t *)p_sr_data);
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_gap_disconnect(void)
{
    T_APP_LE_LINK *p_link;
    p_link = app_link_find_le_link_by_conn_id(le_tuya_adv_get_conn_id());

    if (p_link != NULL)
    {
        app_ble_gap_disconnect(p_link, LE_LOCAL_DISC_CAUSE_TUYA);
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_NOT_FOUND;
    }
}

tuya_ble_status_t tuya_ble_gatt_send_data(const uint8_t *p_data, uint16_t len)
{
    APP_PRINT_INFO1("tuya_ble_gatt_send_data: data %b", TRACE_BINARY(len, p_data));
    uint8_t data_len = len;
    if (data_len > TUYA_BLE_DATA_MTU_MAX)
    {
        data_len = TUYA_BLE_DATA_MTU_MAX;
    }

    if (app_tuya_ble_service_tx_send_notify(le_tuya_adv_get_conn_id(), (uint8_t *)p_data, data_len))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}

tuya_ble_status_t tuya_ble_device_info_characteristic_value_update(uint8_t const *p_data,
                                                                   uint8_t data_len)
{
    app_tuya_ble_set_read_value((uint8_t *)p_data, data_len);
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_common_uart_init(void)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_common_uart_send_data(const uint8_t *p_data, uint16_t len)
{
    console_write((uint8_t *)p_data, len);
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_timer_create(void **p_timer_id, uint32_t timeout_value_ms,
                                        tuya_ble_timer_mode mode, tuya_ble_timer_handler_t timeout_handler)
{
    bool reload = (mode == TUYA_BLE_TIMER_SINGLE_SHOT) ? false : true;

    if (os_timer_create(p_timer_id, "tuya_ble_sdk_timer", 1, timeout_value_ms, reload, timeout_handler))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}

tuya_ble_status_t tuya_ble_timer_delete(void *timer_id)
{
    if (os_timer_delete(&timer_id))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}

tuya_ble_status_t tuya_ble_timer_start(void *timer_id)
{

    if (os_timer_start(&timer_id))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}

tuya_ble_status_t tuya_ble_timer_stop(void *timer_id)
{
    if (os_timer_stop(&timer_id))
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_COMMON;
    }
}


void tuya_ble_device_delay_ms(uint32_t ms)
{
    os_delay(ms);
}

void tuya_ble_device_delay_us(uint32_t us)
{
    platform_delay_us(us);
}

tuya_ble_status_t tuya_ble_rand_generator(uint8_t *p_buf, uint8_t len)
{
    uint32_t cnt = len / 4;
    uint8_t  remain = len % 4;
    int32_t temp;
    for (uint32_t i = 0; i < cnt; i++)
    {
        temp = platform_random(0xFFFFFFFF);
        memcpy(p_buf, (uint8_t *)&temp, 4);
        p_buf += 4;
    }
    temp = platform_random(0xFFFFFFFF);
    memcpy(p_buf, (uint8_t *)&temp, remain);
    APP_PRINT_INFO1("tuya_ble_rand_generator: rand %b,len %d", TRACE_BINARY(len, p_buf));

    return TUYA_BLE_SUCCESS;
}

/*
 *@brief
 *@param
 *
 *@note
 *
 * */
tuya_ble_status_t tuya_ble_device_reset(void)
{
    chip_reset(RESET_ALL);
    return TUYA_BLE_SUCCESS;
}


tuya_ble_status_t tuya_ble_gap_addr_get(tuya_ble_gap_addr_t *p_addr)
{
    if (GAP_CAUSE_SUCCESS == gap_get_param(GAP_PARAM_BD_ADDR, p_addr->addr))
    {
        p_addr->addr_type = TUYA_BLE_ADDRESS_TYPE_PUBLIC;
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        APP_PRINT_ERROR0("tuya_ble_gap_addr_get: err");
        return TUYA_BLE_ERR_INTERNAL;
    }
}


tuya_ble_status_t tuya_ble_gap_addr_set(tuya_ble_gap_addr_t *p_addr)
{
    APP_PRINT_INFO1("tuya_ble_gap_addr_set: type %d", p_addr->addr_type);
    if (p_addr->addr_type == TUYA_BLE_ADDRESS_TYPE_PUBLIC)
    {
        gap_set_bd_addr(p_addr->addr);
        le_cfg_local_identity_address(NULL, GAP_IDENT_ADDR_PUBLIC);
        return TUYA_BLE_SUCCESS;
    }
    else if (p_addr->addr_type == TUYA_BLE_ADDRESS_TYPE_RANDOM)
    {
        le_set_rand_addr(p_addr->addr);
        le_cfg_local_identity_address(p_addr->addr, GAP_IDENT_ADDR_RAND);
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        APP_PRINT_ERROR0("tuya_ble_gap_addr_set:err");
        return TUYA_BLE_ERR_INTERNAL;
    }
}

static uint32_t int_mask;

void tuya_ble_device_enter_critical(void)
{
    int_mask = os_lock();
}

void tuya_ble_device_exit_critical(void)
{
    os_unlock(int_mask);
}

tuya_ble_status_t tuya_ble_rtc_get_timestamp(uint32_t *timestamp, int32_t *timezone)
{
    *timestamp = 0;
    *timezone = 0;
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_rtc_set_timestamp(uint32_t timestamp, int32_t timezone)
{

    return TUYA_BLE_SUCCESS;
}

#if (TUYA_BLE_DEVICE_REGISTER_FROM_BLE)

tuya_ble_status_t tuya_ble_nv_init(void)
{
    uint8_t *p_tuya_nv_data = NULL;

    p_tuya_nv_data = os_mem_zalloc(RAM_TYPE_DATA_ON, TUYA_NV_AREA_SIZE);

    if (p_tuya_nv_data)
    {
        uint32_t read_result = app_tuya_ext_load_ftl_data(p_tuya_nv_data, TUYA_NV_START_ADDR,
                                                          TUYA_NV_AREA_SIZE);
        if (read_result == ENOF)
        {
            APP_PRINT_INFO0("tuya_ble_nv_init: initialize");
            memset(p_tuya_nv_data, 0, TUYA_NV_AREA_SIZE);
            app_tuya_ext_save_ftl_data(p_tuya_nv_data, TUYA_NV_START_ADDR, TUYA_NV_AREA_SIZE);
        }

        os_mem_free(p_tuya_nv_data);
        p_tuya_nv_data = NULL;
    }

    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_erase(uint32_t addr, uint32_t size)
{
    return TUYA_BLE_SUCCESS;
}

tuya_ble_status_t tuya_ble_nv_write(uint32_t addr, const uint8_t *p_data, uint32_t size)
{
    APP_PRINT_INFO3("tuya_ble_nv_write: addr 0x%x, p_data 0x%x, size %d", addr, p_data, size);

    if (app_tuya_ext_save_ftl_data((void *)p_data, addr, size) == 0)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_tuya_info_sync_to_remote(addr, (uint8_t *)p_data, size);
        }

        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}

tuya_ble_status_t tuya_ble_nv_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
    APP_PRINT_INFO3("tuya_ble_nv_read: addr 0x%x, p_data 0x%x, size %d", addr, p_data, size);

    if (app_tuya_ext_load_ftl_data((uint8_t *)p_data, addr, size) == 0)
    {
        return TUYA_BLE_SUCCESS;
    }
    else
    {
        return TUYA_BLE_ERR_BUSY;
    }
}

#endif

bool tuya_ble_os_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
                             void *p_param, uint16_t stack_size, uint16_t priority)
{
    return os_task_create(pp_handle, p_name, p_routine, p_param, stack_size, priority);
}

bool tuya_ble_os_task_delete(void *p_handle)
{
    return os_task_delete(p_handle);
}

bool tuya_ble_os_task_suspend(void *p_handle)
{
    return os_task_suspend(p_handle);
}

bool tuya_ble_os_task_resume(void *p_handle)
{
    return os_task_resume(p_handle);
}

bool tuya_ble_os_msg_queue_create(void **pp_handle, uint32_t msg_num, uint32_t msg_size)
{
    return os_msg_queue_create(pp_handle, "tuya_os_msg", msg_num, msg_size);
}

bool tuya_ble_os_msg_queue_delete(void *p_handle)
{
    return os_msg_queue_delete(p_handle);
}

bool tuya_ble_os_msg_queue_peek(void *p_handle, uint32_t *p_msg_num)
{
    return os_msg_queue_peek(p_handle, p_msg_num);
}

bool tuya_ble_os_msg_queue_send(void *p_handle, void *p_msg, uint32_t wait_ms)
{
    return os_msg_send(p_handle, p_msg, wait_ms);
}

bool tuya_ble_os_msg_queue_recv(void *p_handle, void *p_msg, uint32_t wait_ms)
{
    return os_msg_recv(p_handle, p_msg, wait_ms);
}

#if !TUYA_BLE_SELF_BUILT_TASK
static bool rtk_tuya_send_event_msg(tuya_ble_evt_param_t *evt, uint32_t wait_ms)
{
    tuya_ble_evt_param_t *p_evt = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(tuya_ble_evt_param_t));
    T_IO_MSG tuya_io_msg;
    unsigned char app_event = EVENT_IO_TO_APP;
    tuya_io_msg.type = IO_MSG_TYPE_TUYA;

    if (p_evt)
    {
        memcpy(p_evt, evt, sizeof(tuya_ble_evt_param_t));
        tuya_io_msg.u.buf = p_evt;

        if (os_msg_send(audio_io_queue_handle, &tuya_io_msg, wait_ms))
        {
            if (os_msg_send(audio_evt_queue_handle, &app_event, wait_ms))
            {
                APP_PRINT_INFO2("rtk_tuya_send_event_msg: success, evt_addr 0x%x, evt %d", tuya_io_msg.u.buf,
                                evt->hdr.event);
                return true;
            }
            else
            {
                os_mem_free(p_evt);
                p_evt = NULL;
                return false;
            }
        }
        else
        {
            os_mem_free(p_evt);
            p_evt = NULL;
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool tuya_ble_event_queue_send_port(tuya_ble_evt_param_t *evt, uint32_t wait_ms)
{
    return rtk_tuya_send_event_msg(evt, wait_ms);
}
#endif

bool tuya_ble_aes128_ecb_encrypt(uint8_t *key, uint8_t *input, uint16_t input_len, uint8_t *output)
{
    uint16_t length;
    if (input_len % 16)
    {
        return false;
    }
    length = input_len;
    while (length > 0)
    {
        aes128_ecb_encrypt(input, key, output);
        input  += 16;
        output += 16;
        length -= 16;
    }

    return true;
}

bool tuya_ble_aes128_ecb_decrypt(uint8_t *key, uint8_t *input, uint16_t input_len, uint8_t *output)
{
    uint16_t length;
    if (input_len % 16)
    {
        return false;
    }
    length = input_len;
    while (length > 0)
    {
        aes128_ecb_decrypt(input, key, output);
        input  += 16;
        output += 16;
        length -= 16;
    }
    return true;
}

bool tuya_ble_aes128_cbc_encrypt(uint8_t *key, uint8_t *iv, uint8_t *input, uint16_t input_len,
                                 uint8_t *output)
{
    if (input_len % 16)
    {
        return false;
    }

//    uint32_t IV[4] = {0};
//    IV[0] = (iv[12] << 24) | (iv[13] << 16) | (iv[14] << 8) | iv[15];
//    IV[1] = (iv[8] << 24) | (iv[9] << 16) | (iv[10] << 8) | iv[11];
//    IV[2] = (iv[4] << 24) | (iv[5] << 16) | (iv[6] << 8) | iv[7];
//    IV[3] = (iv[0] << 24) | (iv[1] << 16) | (iv[2] << 8) | iv[3];

    uint32_t len;
    len = input_len / 4;

    aes128_cbc_encrypt(input, key, output, (uint32_t *)iv, len); //danni
    APP_PRINT_INFO4("tuya_ble_aes128_cbc_encrypt: key %b\n,IV %b\n,input %b\n,output %b",
                    TRACE_BINARY(16, key), TRACE_BINARY(16, iv), TRACE_BINARY(16, input), TRACE_BINARY(16, output));
    return true;
}

bool tuya_ble_aes128_cbc_decrypt(uint8_t *key, uint8_t *iv, uint8_t *input, uint16_t input_len,
                                 uint8_t *output)
{
    if (input_len % 16)
    {
        return false;
    }

//    uint32_t IV[4] = {0};
//    IV[0] = (iv[12] << 24) | (iv[13] << 16) | (iv[14] << 8) | iv[15];
//    IV[1] = (iv[8] << 24) | (iv[9] << 16) | (iv[10] << 8) | iv[11];
//    IV[2] = (iv[4] << 24) | (iv[5] << 16) | (iv[6] << 8) | iv[7];
//    IV[3] = (iv[0] << 24) | (iv[1] << 16) | (iv[2] << 8) | iv[3];

    uint32_t len;
    len = input_len / 4;

    aes128_cbc_decrypt(input, key, output, (uint32_t *)iv, len); //danni
    APP_PRINT_INFO4("tuya_ble_aes128_cbc_decrypt: key %b\n,IV %b\n,input %b\n,output %b",
                    TRACE_BINARY(16, key), TRACE_BINARY(16, iv), TRACE_BINARY(16, input), TRACE_BINARY(16, output));
    return true;
}

bool tuya_ble_md5_crypt(uint8_t *input, uint16_t input_len, uint8_t *output)
{
    mbedtls_md5_context md5_ctx;
    mbedtls_md5_init(&md5_ctx);
    mbedtls_md5_starts(&md5_ctx);
    mbedtls_md5_update(&md5_ctx, input, input_len);
    mbedtls_md5_finish(&md5_ctx, output);
    mbedtls_md5_free(&md5_ctx);

    return true;
}

bool tuya_ble_hmac_sha1_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input,
                              uint32_t input_len, uint8_t *output)
{
    hmac_sha1_crypt(key, key_len, input, input_len, output);
    return true;
}

bool tuya_ble_hmac_sha256_crypt(const uint8_t *key, uint32_t key_len, const uint8_t *input,
                                uint32_t input_len, uint8_t *output)
{
    hmac_sha256_crypt(key, key_len, input, input_len, output);
    return true;
}

#if TUYA_BLE_LOG_ENABLE
uint8_t tuya_ble_log_get_args(const char *format)
{
    uint8_t arg_no = 0;
    if (!format)
    {
        return 0;
    }
    for (; * (format + 1) != '\0'; format++)
    {
        if ((*format == '%') && (*(format + 1) != '%'))
        {
            arg_no++;
        }
    }
    if (arg_no > 8)
    {
        arg_no = 8;
    }
    return arg_no;
}

void tuya_ble_log_hexdump(const char *name, uint8_t width, uint8_t *buf, uint16_t size)
{
    uint16_t total_line;
    uint16_t line = size / width;
    uint16_t line_remain = size % width;
    if (line_remain > 0)
    {
        total_line = line + 1;
    }
    else
    {
        total_line = line;
    }

//    if (line > 3)
//    {
//        line = 3;
//    }
    //APP_PRINT_INFO1("HEX:%s\n", TRACE_STRING(name));
    uint8_t i = 0;
    for (i = 0; i < line; i++)
    {
        APP_PRINT_INFO3("tuya_ble_log_hexdump[%d-%d]: %b\n", total_line, i + 1, TRACE_BINARY(width,
                        buf + i * width));
    }
    if (line_remain > 0)
    {
        APP_PRINT_INFO3("tuya_ble_log_hexdump[%d-%d]: %b\n", total_line, total_line,
                        TRACE_BINARY(line_remain, buf + i * width));
    }
}
#endif


#if (TUYA_BLE_USE_PLATFORM_MEMORY_HEAP==1)

void *tuya_ble_port_malloc(uint32_t size)
{
    return malloc(size);
}

void tuya_ble_port_free(void *pv)
{
    if (pv != NULL)
    {
        free(pv);
    }
    pv = NULL;
}


#endif

#endif
