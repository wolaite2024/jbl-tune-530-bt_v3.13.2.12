/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_ADV_STOP_CAUSE_H_
#define _APP_ADV_STOP_CAUSE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define APP_STOP_ADV_CAUSE_POWER_OFF                (0x01)
#define APP_STOP_ADV_CAUSE_ROLE_SECONDARY           (0x02)
#define APP_STOP_ADV_CAUSE_ROLE_SWAP                (0x03)
#define APP_STOP_ADV_CAUSE_RECOVERY_LINK_SET_UP     (0x04)
#define APP_STOP_ADV_CAUSE_DUT_TEST                 (0x05)
#define APP_STOP_ADV_CAUSE_AVP                      (0x06)
#define APP_STOP_ADV_CAUSE_ENGAGE                   (0x07)
#define APP_STOP_ADV_CAUSE_INVALID_DATA             (0x08)
#define APP_STOP_ADV_CAUSE_IBEACON_FOR_LE_CONN      (0x09)
#define APP_STOP_ADV_CAUSE_IBEACON_FOR_SPP_CONN     (0x0A)
#define APP_STOP_ADV_CAUSE_IBEACON_FOR_DEV_DISC     (0x0B)
#define APP_STOP_ADV_CAUSE_COMMON_FOR_SPP_CONN      (0x0C)
#define APP_STOP_ADV_CAUSE_COMMON_FOR_DEV_DISC      (0x0D)
#define APP_STOP_ADV_CAUSE_OTA                      (0x0E)
#define APP_STOP_ADV_CAUSE_RTK_FAST_PAIR            (0x0F)
#define APP_STOP_ADV_CAUSE_XM_ADP_TIMEOUT           (0x10)
#define APP_STOP_ADV_CAUSE_GFPS_ACTION_IDLE         (0x11)
#define APP_STOP_ADV_CAUSE_EXIT_PAIRING_MODE        (0x12)
#define APP_STOP_ADV_CAUSE_TUYA                     (0x13)
#define APP_STOP_ADV_CAUSE_AIRPLANE                 (0x14)
#define APP_STOP_ADV_CAUSE_TEAMS_BR_LINK_DISCONN    (0x15)
#define APP_STOP_ADV_CAUSE_FINDMY                   (0x16)
#define APP_STOP_ADV_CAUSE_USB_SUSPEND              (0x17)
#define APP_STOP_ADV_CAUSE_MIX_MULTI_STREAM         (0x18)
#define APP_STOP_ADV_CAUSE_SWIFT_PAIR_IDLE          (0x19)
#define APP_STOP_ADV_CAUSE_GFPS_LINK_FULL           (0x20)
#define APP_STOP_ADV_CAUSE_GFPS_FINDER              (0x21)
#define APP_STOP_ADV_CAUSE_LEGACY_GAMING_STREAMING  (0x22)
#define APP_STOP_ADV_CAUSE_DULT_PAUSE               (0x23)
#define APP_STOP_ADV_CAUSE_GFPS_FINDER_KEY_PRESS    (0x24)
#define APP_STOP_ADV_CAUSE_CONNECTED                (0x25)
#define APP_STOP_ADV_CAUSE_GFPS_FINDER_UPDATE_EIK   (0x26)

// you can add new cause in here
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_GAP_H_ */
