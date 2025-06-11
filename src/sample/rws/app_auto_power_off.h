/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AUTO_POWER_OFF_H_
#define _APP_AUTO_POWER_OFF_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    POWER_OFF_CAUSE_UNKNOWN                 = 0x00,
    POWER_OFF_CAUSE_HYBRID_KEY              = 0x01,
    POWER_OFF_CAUSE_LONG_KEY                = 0x02,
    POWER_OFF_CAUSE_SHORT_KEY               = 0x03,
    POWER_OFF_CAUSE_SLIDE_SWITCH            = 0x04,
    POWER_OFF_CAUSE_CMD_SET                 = 0x05,
    POWER_OFF_CAUSE_ADP_CMD                 = 0x06,
    POWER_OFF_CAUSE_FACTORY_RESET           = 0x07,
    POWER_OFF_CAUSE_LOW_VOLTAGE             = 0x08,
    POWER_OFF_CAUSE_SEC_LINKLOST_TIMEOUT    = 0x09,
    POWER_OFF_CAUSE_EXIT_PAIRING_MODE       = 0x10,
    POWER_OFF_CAUSE_AUTO_POWER_OFF          = 0x11,
    POWER_OFF_CAUSE_ANC_TOOL                = 0x12,
    POWER_OFF_CAUSE_OTA_TOOL                = 0x13,
    POWER_OFF_CAUSE_NORMAL_INBOX            = 0x20,
    POWER_OFF_CAUSE_ADP_CLOSE_CASE_TIMEOUT  = 0x21,
    POWER_OFF_CAUSE_ADP_IN_CASE_TIMEOUT     = 0x22,
    POWER_OFF_CAUSE_RELAY                   = 0x30,
} T_POWER_OFF_CAUSE;

typedef enum
{
    AUTO_POWER_OFF_MASK_POWER_ON                            = 0x00000001,
    AUTO_POWER_OFF_MASK_SOURCE_LINK                         = 0x00000002,
    AUTO_POWER_OFF_MASK_IN_BOX                              = 0x00000004,
    AUTO_POWER_OFF_MASK_BUD_COUPLING                        = 0x00000008,
    AUTO_POWER_OFF_MASK_KEY                                 = 0x00000010,
    AUTO_POWER_OFF_MASK_ALREADY_POWER_OFF                   = 0x00000020,
    AUTO_POWER_OFF_MASK_PAIRING_MODE                        = 0x00000040,
    AUTO_POWER_OFF_MASK_ANC_APT_MODE                        = 0x00000080,
    AUTO_POWER_OFF_MASK_BLE_LINK_EXIST                      = 0x00000100,
    AUTO_POWER_OFF_MASK_PLAYBACK_MODE                       = 0x00000200,
    AUTO_POWER_OFF_MASK_LINE_IN                             = 0x00000400,
    AUTO_POWER_OFF_MASK_ANC_APT_MODE_WITH_PHONE_CONNECTED   = 0x00000800,
    AUTO_POWER_OFF_MASK_AIRPLANE_MODE                       = 0x00001000,
    AUTO_POWER_OFF_MASK_LINKBACK                            = 0x00002000,
    AUTO_POWER_OFF_MASK_DUT_MODE                            = 0x00004000,
    AUTO_POWER_OFF_MASK_USB_AUDIO_MODE                      = 0x00008000,
    AUTO_POWER_OFF_MASK_XIAOAI_OTA                          = 0x00010000,
    AUTO_POWER_OFF_MASK_OTA_TOOLING                         = 0x00020000,
    AUTO_POWER_OFF_MASK_BLE_AUDIO                           = 0x00040000,
    AUTO_POWER_OFF_MASK_TUYA_OTA                            = 0x00080000,
    AUTO_POWER_OFF_MASK_CONSOLE_CMD                         = 0x00100000,
    AUTO_POWER_OFF_MASK_VOICE                               = 0x00200000,
    AUTO_POWER_OFF_MASK_AUDIO                               = 0x00400000,
    AUTO_POWER_OFF_MASK_DATA_CAPTURE                        = 0x00800000,
    AUTO_POWER_OFF_MASK_LE_CMD                              = 0x01000000,
} T_AUTO_POWER_OFF_MASK;

void app_auto_power_off_enable(uint32_t flag, uint16_t timeout);

void app_auto_power_off_disable(uint32_t flag);

void app_auto_power_off_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AUTO_POWER_OFF_H_ */
