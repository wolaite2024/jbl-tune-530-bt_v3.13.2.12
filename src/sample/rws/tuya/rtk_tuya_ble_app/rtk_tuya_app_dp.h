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
#ifndef __RTK_TUYA_APP_DP_H_
#define __RTK_TUYA_APP_DP_H_

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "tuya_ble_mutli_tsf_protocol.h"
/*============================================================================*
 *                              Variables
*============================================================================*/
typedef enum
{
    TUYA_DP_ID_BROADCAST_MODE = 0x01,
    TUYA_DP_ID_BATTERY_PERCENTAGE = 0x02,
    TUYA_DP_ID_LEFT_BATTERY_PERCENTAGE = 0x03,
    TUYA_DP_ID_RIGHT_BATTERY_PERCENTAGE = 0x04,
    TUYA_DP_ID_VOLUME_SET = 0x05,
    TUYA_DP_ID_CHANGE_CONTROL = 0x06,
    TUYA_DP_ID_PLAY = 0x07,
    TUYA_DP_ID_NOISE_MODE = 0x08,
    TUYA_DP_ID_NOISE_SCENE = 0x09,
    TUYA_DP_ID_TRANSPARENCY_SCENES = 0x0A,
    TUYA_DP_ID_NOISE_SET = 0x0B,
    TUYA_DP_ID_DEVICE_FIND = 0x0C,
    TUYA_DP_ID_BREAK_REMINDER = 0x0D,
    TUYA_DP_ID_RECONNECT_REMINDER = 0x0E,
} T_TUYA_DP_ID;

typedef enum
{
    TUYA_DP_CHANGE_CONTROL_LAST = 0x00,
    TUYA_DP_CHANGE_CONTROL_NEXT = 0x01,
} T_TUYA_DP_CHANGE_CONTROL;

/*============================================================================*
 *                              Functions
*============================================================================*/
void app_tuya_dp_data_receive(uint8_t *dp_data, uint16_t dp_data_len);

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif
