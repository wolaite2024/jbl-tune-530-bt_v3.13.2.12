#ifndef _APP_BLE_BOND_H_
#define _APP_BLE_BOND_H_


#include <stdint.h>
#include <stdbool.h>

void app_ble_bond_add_send_to_sec(uint8_t *bd_addr);


void app_ble_bond_sync_all_info(void);


void app_ble_bond_init(void);


void app_ble_bond_sync_recovery(void);
#endif



