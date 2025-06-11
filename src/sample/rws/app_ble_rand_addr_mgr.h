#ifndef __APP_BLE_RAND_ADDR_MGR_H
#define __APP_BLE_RAND_ADDR_MGR_H

#include <stdint.h>


typedef enum
{
    RANDOM_ADDR_TWS_ADDR_UPD,
    RANDOM_ADDR_TWS_ADDR_RECVED,
} RANDOM_ADDR_MGR_EVT;


typedef struct
{
    RANDOM_ADDR_MGR_EVT evt;

    union
    {
        struct
        {
            uint8_t addr[6];
        } upd_tws_addr;

        struct
        {
            uint8_t addr[6];
        } recv_tws_addr;
    };
} RANDOM_ADDR_MGR_EVT_DATA;


typedef void (*RANDOM_ADDR_MGR_CB)(RANDOM_ADDR_MGR_EVT_DATA *data);


void app_ble_rand_addr_init(void);
void app_ble_rand_addr_cb_reg(RANDOM_ADDR_MGR_CB cb);
void app_ble_rand_addr_get(uint8_t random_addr[6]);
void app_ble_rand_addr_handle_role_decieded(void);
void app_ble_rand_addr_handle_remote_conn_cmpl(void);
#endif
