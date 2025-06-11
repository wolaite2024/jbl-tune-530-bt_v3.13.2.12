/*
 * Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TYPES_H_
#define _APP_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RTK_COMPANY_ID    (0x005D)
#define VENDOR_ID_SOURCE  (0x0001)
#define PRODUCT_ID        (0x223B)
#define PRODUCT_VERSION   (0x0100)

#define VENDOR_DATA_TYPE_GAMING_MODE       0x01

/**
 * @defgroup APP_TYPES APP common type
 * @brief Common type declaration for app modules.
 * @{
 */

/**
 * @defgroup APP_TYPES_Exported_Types APP TYPES Exported Types
 * @{
 */

/**  @brief Translated call status for APP modules */
typedef enum t_app_call_status
{
    APP_CALL_IDLE                               = 0x00,
    APP_VOICE_ACTIVATION_ONGOING                = 0x01,
    APP_CALL_INCOMING                           = 0x02,
    APP_CALL_OUTGOING                           = 0x03,
    APP_CALL_ACTIVE                             = 0x04,
    APP_CALL_ACTIVE_WITH_CALL_WAITING           = 0x05,
    APP_CALL_ACTIVE_WITH_CALL_HELD              = 0x06,
    APP_MULTILINK_CALL_ACTIVE_WITH_CALL_WAIT    = 0x07,
    APP_MULTILINK_CALL_ACTIVE_WITH_CALL_HELD    = 0x08,
    APP_CALL_HELD                               = 0x09,
} T_APP_CALL_STATUS;

typedef enum t_app_low_latency_level
{
    LOW_LATENCY_LEVEL1      = 0x00,
    LOW_LATENCY_LEVEL2      = 0x01,
    LOW_LATENCY_LEVEL3      = 0x02,
    LOW_LATENCY_LEVEL4      = 0x03,
    LOW_LATENCY_LEVEL5      = 0x04,
    LOW_LATENCY_LEVEL_MAX   = 0x05,
} T_APP_LOW_LATENCY_LEVEL;

typedef enum t_app_key_index
{
    KEY0 = 0x00,
    KEY1 = 0x01,
    KEY2 = 0x02,
    KEY3 = 0x03,
    KEY4 = 0x04,
    KEY5 = 0x05,
    KEY6 = 0x06,
    KEY7 = 0x07
} T_APP_KEY_INDEX;

typedef enum
{
    WAKE_UP_RTC                        = 0x0001,
    WAKE_UP_PAD                        = 0x0002,
    WAKE_UP_MFB                        = 0x0004,
    WAKE_UP_ADP                        = 0x0008,
    WAKE_UP_CTC                        = 0x0010,
    WAKE_UP_KEY0                       = 0x0020,
    WAKE_UP_COMBINE_KEY_POWER_ONOFF    = 0x0040,
    WAKE_UP_3PIN                       = 0x0080,
} T_WAKE_UP_REASON;

/** @} */ /* End of group APP_TYPES_Exported_Types */

/** @} */ /* End of group APP_TYPES */

#ifdef __cplusplus
}
#endif

#endif /* _APP_TYPES_H_ */
