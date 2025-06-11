/*
 * Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_MSG_H_
#define _APP_MSG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup APP_MSG APP Message
 * @brief message definition for user application task.
 * @{
 */

/**
 * @defgroup APP_MSG_Exported_Types APP Message Exported Types
 * @{
 */

/**
 * @brief     Event group definitions.
 * @details   event code is 1 byte code to define messages type exchanging from/to different layers.
 *            first half byte is for group define, and second harf byte is for code define
 *            group code:
 *            0x00: from stack layer
 *            0x01: from peripheral layer
 *            0x02: from framework
 *            0x03: from app service
 *            0x0A: from instructment
 */
typedef enum
{
    EVENT_GROUP_STACK = 0x00,       /**< message group from BT layer */
    EVENT_GROUP_IO = 0x01,          /**< message group from IO layer */
    EVENT_GROUP_FRAMEWORK = 0x02,   /**< message group from Framework layer */
    EVENT_GROUP_APP = 0x03,         /**< message group from APP layer */
    EVENT_GROUP_INSTRUMENT = 0x0A,  /**< message group from instructment layer */
} T_EVENT_GROUP;

/**  @brief     Event type definitions.
*/
typedef enum
{
    //EVENT_GROUP_STACK
    EVENT_GAP_MSG = 0x01,           /**< message from gap layer for stack */
    EVENT_GAP_TIMER = 0x02,         /**< message from gap layer for timer */
    EVENT_HCI_MSG = 0x03,           /**< message from HCI layer for test */

    //EVENT_GROUP_IO
    EVENT_IO_TO_APP = 0x11,         /**< message from IO to user application */

    //EVENT_GROUP_FRAMEWORK
    EVENT_SYS_MSG           = 0x21, /**< message from SYS timer to user application */
    EVENT_DSP_MSG           = 0x22, /**< message from DSP to user application */
    EVENT_CODEC_MSG         = 0x23, /**< message from CODEC to user application */
    EVENT_ANC_MSG           = 0x24, /**< message from ANC to user application */
    EVENT_NOTIFICATION_MSG  = 0x25, /**< message from Notification to user application */
    EVENT_BIN_LOADER_MSG    = 0x26, /**< message from bin load to user application */
    EVENT_AUDIO_PLUGIN_MSG  = 0x27, /**< message from external device to Audio Plugin */
    EVENT_AUDIO_PATH_MSG    = 0x28, /**< message from Audio Plugin to Audio Path */
    EVENT_ENGINE_MSG        = 0x29,

    //EVENT_GROUP_APP
    EVENT_APP_TIMER_MSG = 0x30,     /**< message from APP timer to user application */

    //EVENT_GROUP_INSTRUMENT
    EVENT_CMD_FROM_8852B = 0xA1,    /**< message from 8852B to user application */
} T_EVENT_TYPE;

/** @brief get event code group definition */
#define EVENT_GROUP(event_code) (event_code >> 4)

/**  @brief IO type definitions for IO message, may extend as requested */
typedef enum
{
    IO_MSG_TYPE_BT_STATUS,  /**< BT status change with subtype @ref GAP_MSG_TYPE */
    IO_MSG_TYPE_KEYSCAN,    /**< Key scan message with subtype @ref T_IO_MSG_KEYSCAN */
    IO_MSG_TYPE_QDECODE,    /**< subtype to be defined */
    IO_MSG_TYPE_UART,       /**< Uart message with subtype @ref T_IO_MSG_UART */
    IO_MSG_TYPE_KEYPAD,     /**< subtype to be defined */
    IO_MSG_TYPE_IR,         /**< subtype to be defined */
    IO_MSG_TYPE_GDMA,       /**< subtype to be defined */
    IO_MSG_TYPE_ADC,        /**< subtype to be defined */
    IO_MSG_TYPE_D3DG,       /**< subtype to be defined */
    IO_MSG_TYPE_SPI,        /**< subtype to be defined */
    IO_MSG_TYPE_MOUSE_BUTTON,   /**< subtype to be defined */
    IO_MSG_TYPE_GPIO,       /**< Gpio message with subtype @ref T_IO_MSG_GPIO*/
    IO_MSG_TYPE_MOUSE_SENSOR,   /**< subtype to be defined */
    IO_MSG_TYPE_TIMER,      /**< App timer message with subtype @ref T_IO_MSG_TIMER */
    IO_MSG_TYPE_WRISTBNAD,  /**< wristband message with subtype @ref T_IO_MSG_WRISTBAND */
    IO_MSG_TYPE_MESH_STATUS,    /**< subtype to be defined */
    IO_MSG_TYPE_KEYBOARD_BUTTON, /**< subtype to be defined */
    IO_MSG_TYPE_ANCS,            /**< ANCS message*/
    IO_MSG_TYPE_LE_AMS,
    IO_MSG_TYPE_CONSOLE,    /**< Console message with subtype @ref T_IO_CONSOLE */
    IO_MSG_TYPE_LE_MGR,
    IO_MSG_TYPE_CAP_TOUCH,/**< Cap Touch message with subtype @ref T_IO_MSG_CAP_TOUCH */
    IO_MSG_TYPE_ECC,
    IO_MSG_TYPE_ADP_VOLTAGE,
    IO_MSG_TYPE_BISTO,
    IO_MSG_TYPE_USB_DEV,
    IO_MSG_TYPE_USB_UAC,
    IO_MSG_TYPE_USB_HID,
    IO_MSG_TYPE_LE_AUDIO,
    IO_MSG_TYPE_A2DP_SRC,
    IO_MSG_TYPE_TUYA,
    IO_MSG_TYPE_TUYA_OTA,
    IO_MSG_TYPE_SS,
    IO_MSG_TYPE_UART_LINE_STATUS,      /**< UART LINE STATUS event */
    IO_MSG_TYPE_FINDMY,
    IO_MSG_TYPE_AUDIO_TRANS_SPI,
    IO_MSG_TYPE_GPIO_DLPS,
    IO_MSG_TYPE_LEA_SRC,
    IO_MSG_TYPE_LEA_SNK,
#if (F_APP_FWK_PIPE_DEMO_SUPPORT == 1)
    IO_MSG_TYPE_PIPE,
#endif
} T_IO_MSG_TYPE;

/**  @brief IO subtype definitions for @ref T_IO_CONSOLE type */
typedef enum
{
    IO_MSG_CONSOLE_STRING_RX    = 0x01, /**< Console CLI RX event */
    IO_MSG_CONSOLE_STRING_TX    = 0x02, /**< Console CLI TX event */
    IO_MSG_CONSOLE_BINARY_RX    = 0x03, /**< Console protocol RX event */
    IO_MSG_CONSOLE_BINARY_TX    = 0x04, /**< Console protocol TX event */
} T_IO_CONSOLE;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_KEYSCAN type */
typedef enum
{
    IO_MSG_KEYSCAN_RX_PKT        = 1, /**< Keyscan RX data event */
    IO_MSG_KEYSCAN_MAX           = 2, /**<  */
    IO_MSG_KEYSCAN_ALLKEYRELEASE = 3, /**< All keys are released event */
    IO_MSG_KEYSCAN_STUCK         = 4, /**<  key stuck message */
} T_IO_MSG_KEYSCAN;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_UART type */
typedef enum
{
    IO_MSG_UART_RX              = 0x10, /**< UART RX event */

    IO_MSG_UART_TX              = 0x20, /**< UART TX event */
} T_IO_MSG_UART;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_GPIO type */
typedef enum
{
    IO_MSG_GPIO_KEY,               /**< KEY GPIO event */
    IO_MSG_GPIO_UART_WAKE_UP,      /**< UART WAKE UP event */
    IO_MSG_GPIO_CHARGER,           /**< CHARGER event */
    IO_MSG_GPIO_NFC,               /**< NFC event */
    IO_MSG_GPIO_ADAPTOR_PLUG,         /**< ADAPTOR PLUG IN event */
    IO_MSG_GPIO_ADAPTOR_UNPLUG,       /**< ADAPTOR PLUG OUT event */
    IO_MSG_GPIO_CHARGERBOX_DETECT,     /**< CHARGERBOX DETECT event*/
    IO_MSG_GPIO_CASE_DETECT,                   /**< CASE DETECT event*/
    IO_MSG_GPIO_GSENSOR,            /**< Gsensor detect event*/
    IO_MSG_GPIO_SENSOR_LD,            /**< sensor light detect event*/
    IO_MSG_GPIO_SENSOR_LD_IO_DETECT,  /**< sensor light io detect event*/
    IO_MSG_GPIO_ADAPTOR_DAT,      /**< ADAPTOR DATA event*/
    IO_MSG_GPIO_XM_ADP_WAKE_UP,    /**< XM ADP WAKE UP event*/
    IO_MSG_GPIO_LINE_IN,    /**< line in detect event*/
    IO_MSG_GPIO_PSENSOR,             /**< Psensor INT event */
    IO_MSG_GPIO_PLAYBACK_TRANS_FILE_ACK,    /**< PLAYBACK TRANS file event */
    IO_MSG_GPIO_DLPS_ADAPTOR_DETECT,
    IO_MSG_GPIO_SLIDE_SWITCH_0,
    IO_MSG_GPIO_SLIDE_SWITCH_1,
    IO_MSG_GPIO_QDEC,
    IO_MSG_GPIO_KEY0,            /**< Key0 INT event */
    IO_MSG_GPIO_SMARTBOX_COMMAND_PROTECT,   /**< cmd protect */
    IO_MSG_GPIO_ADP_INT,      /**< Adapter INT event */
    IO_MSG_GPIO_ADP_HW_TIMER_HANDLER,
    IO_MSG_MEMS_ACC_DATA_INT,           /**< MEMS ACC dat received event*/
    IO_MSG_MEMS_ACC_DATA,               /**< MEMS ACC dat event*/
    IO_MSG_GPIO_BUD_LOC_CHANGE,     /**< BUD LOCATION DETECT event*/
    IO_MSG_GPIO_UART_ENABLE_DLPS,
    IO_MSG_GPIO_UART_DISABLE_DLPS,
    IO_MSG_GPIO_EXT_CHARGER_STATE,
    IO_MSG_GPIO_EXT_CHARGER_ADC_VALUE,
    IO_MSG_GPIO_USB_CONNECTOR_ADC_VALUE,
} T_IO_MSG_GPIO;

typedef enum
{
    IO_MSG_LEA_ISO_SYNC_DOWNSTREAM = 0x01,
} T_IO_MSG_LEA;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_CAP_TOUCH type */
typedef enum
{
    IO_MSG_CAP_CH0_PRESS = 0,
    IO_MSG_CAP_CH0_RELEASE,             /**< 0x1 */
    IO_MSG_CAP_CH0_FALSE_TRIGGER,       /**< 0x2 */
    IO_MSG_CAP_CH1_PRESS,               /**< 0x3 */
    IO_MSG_CAP_CH1_RELEASE,             /**< 0x4 */
    IO_MSG_CAP_CH1_FALSE_TRIGGER,       /**< 0x5 */
    IO_MSG_CAP_CH2_PRESS,               /**< 0x6 */
    IO_MSG_CAP_CH2_RELEASE,             /**< 0x7 */
    IO_MSG_CAP_CH2_FALSE_TRIGGER,       /**< 0x8 */
    IO_MSG_CAP_CH3_PRESS,               /**< 0x9 */
    IO_MSG_CAP_CH3_RELEASE,             /**< 0xA */
    IO_MSG_CAP_CH3_FALSE_TRIGGER,       /**< 0xB */
    IO_MSG_CAP_TOUCH_OVER_N_NOISE,      /**< 0xC ,N_NOISE event */
    IO_MSG_CAP_TOUCH_OVER_P_NOISE,      /**< 0xD ,P_NOISE event */

    IO_MSG_CAP_TOTAL,
} T_IO_MSG_CAP_TOUCH;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_TIMER type */
typedef enum
{
    IO_MSG_TIMER_ALARM,
    IO_MSG_TIMER_RWS
} T_IO_MSG_TIMER;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_WRISTBNAD type */
typedef enum
{
    IO_MSG_BWPS_TX_VALUE,
    IO_MSG_UPDATE_CONPARA,
    IO_MSG_ANCS_DISCOVERY,
    IO_MSG_TYPE_AMS,
    IO_MSG_INQUIRY_START,
    IO_MSG_INQUIRY_STOP,
    IO_MSG_CONNECT_BREDR_DEVICE,
    IO_MSG_MMI,
    IO_MSG_SET_PLAY_MODE,
    IO_MSG_PLAY_BY_NAME,
    IO_MSG_PLAYBACK_TRANS_FILE_ACK,    /**< PLAYBACK TRANS file event */
    IO_MSG_PREPARE_USB_ENVIRONMENT,
    IO_MSG_HANDLE_USB_PLUG_OUT,
    IO_MSG_PLAYBACK_TRANS_FILE_END,
} T_IO_MSG_WRISTBAND;

/**  @brief IO subtype definitions for @ref IO_MSG_TYPE_FINDMY type */
typedef enum
{
    IO_MSG_FINDMY_KEYROLL,
    IO_MSG_FINDMY_EVENT,
} T_IO_MSG_TYPE_FINDMY;

/**  @brief IO message definition for communications between tasks*/
typedef struct
{
    uint16_t type;
    uint16_t subtype;
    union
    {
        uint32_t  param;
        void     *buf;
    } u;
} T_IO_MSG;


/** @} */ /* End of group APP_MSG_Exported_Types */

/** @} */ /* End of group APP_MSG */

#ifdef __cplusplus
}
#endif

#endif /* _APP_MSG_H_ */
