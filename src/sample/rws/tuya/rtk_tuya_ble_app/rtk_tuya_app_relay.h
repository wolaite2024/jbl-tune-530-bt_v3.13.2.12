/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtk_tuya_app_dp.h
* @brief    This file provides aes cbc api wrapper for sdk customer.
* @details
* @author
* @date     2021-9-24
* @version  v1.0
*********************************************************************************************************
*/
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __RTK_TUYA_APP_RELAY_H_
#define __RTK_TUYA_APP_RELAY_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "tuya_ble_api.h"
/*============================================================================*
 *                              Variables
*============================================================================*/



/*============================================================================*
 *                              Functions
*============================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    APP_REMOTE_MSG_TUYA_AUTH_INFO                         = 0x00,
    APP_REMOTE_MSG_TUYA_AUTH_INFO_BACKUP                  = 0x01,
    APP_REMOTE_MSG_TUYA_SYS_INFO                          = 0x02,
    APP_REMOTE_MSG_TUYA_SYS_INFO_BACKUP                   = 0x03,
    APP_REMOTE_MSG_TUYA_MAX_MSG_NUM                       = 0x04,
} T_APP_TUYA_REMOTE_MSG;

tuya_ble_status_t app_tuya_info_sync_to_remote(uint32_t addr, uint8_t *p_data, uint32_t size);
void app_tuya_relay_all(void);
bool app_tuya_relay_init(void);
#ifdef __cplusplus
}
#endif

#endif
