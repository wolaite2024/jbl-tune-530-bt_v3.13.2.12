/**
 * \file rtk_tuya_ble_port_rtl8773c.h
 *
 * \brief
 */
#ifndef TUYA_BLE_PORT_RTL8773c_H__
#define TUYA_BLE_PORT_RTL8773c_H__

#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_config.h"

#if TUYA_BLE_LOG_ENABLE
#include "trace.h"

uint8_t tuya_ble_log_get_args(const char *format);
void tuya_ble_log_hexdump(const char *name, uint8_t width, uint8_t *buf, uint16_t size);

#define  TUYA_BLE_PRINTF(fmt,...) \
    { \
        uint8_t param_num = tuya_ble_log_get_args(fmt); \
        DBG_BUFFER_INTERNAL(LOG_TYPE, SUBTYPE_FORMAT, MODULE_APP, LEVEL_INFO, fmt, param_num, ##__VA_ARGS__); \
    }

#define TUYA_BLE_HEXDUMP(...)           tuya_ble_log_hexdump("", 8, __VA_ARGS__)

#else

#define TUYA_BLE_PRINTF(...)
#define TUYA_BLE_HEXDUMP(...)

#endif
void *rtk_tuya_msg_queue_handle_get(void);
void tuya_ble_device_enter_critical(void);
void tuya_ble_device_exit_critical(void);//danni
#endif
