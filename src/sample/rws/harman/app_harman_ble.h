#ifndef _APP_HARMAN_BLE_H_
#define _APP_HARMAN_BLE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_app_harman_ble_entry_event
{
    HARMAN_BLE_UPDATE_ADV_CRC = 0x01,
    HARMAN_BLE_TX_WHEN_BLE_CONN = 0x02,

} T_APP_HARMAN_BLE_ENTRY_EVENT;

uint8_t *app_harman_ble_data_get(void);
uint8_t app_harman_ble_data_len_get(void);
void app_harman_remote_device_name_crc_set(uint8_t conn_id, uint8_t *bd_addr, uint8_t action);
uint8_t *app_harman_ble_bt_mac_crc_get(void);
void app_harman_le_common_adv_update(void);
uint16_t harman_crc16_ibm(uint8_t *buf, uint32_t size, uint8_t *bd_addr);
void harman_le_common_disc(void);
void app_harman_ble_adv_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
