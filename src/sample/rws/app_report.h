/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_REPORT_H_
#define _APP_REPORT_H_

#include <stdint.h>
#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif

#include "app_eq.h"
#include "app_link_util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_REPORT App Report
  * @brief App Report
  * @{
  */
//for EVENT_VOLUME_SYNC
#define SYNC_TYPE_HF_GAIN_LEVEL             0x00
#define SYNC_TYPE_AVRCP_UP_DN               0x01
#define SYNC_TYPE_AVRCP_ABSOLUTE_VOL        0x02

//for EVENT_CALLER_ID
#define CALLER_ID_TYPE_NUMBER               0x00
#define CALLER_ID_TYPE_NAME                 0x01

#define RTK_HEADER_LEN                      6
#define ATT_HEADER_LEN                      3

typedef enum
{
    CMD_PATH_NONE = 0x00,
    CMD_PATH_UART = 0x01,
    CMD_PATH_LE   = 0x02,
    CMD_PATH_SPP  = 0x03,
    CMD_PATH_IAP  = 0x04,

    CMD_PATH_RWS_ASYNC = 0x10,
    CMD_PATH_RWS_SYNC  = 0x11,
} T_CMD_PATH;

typedef enum
{
    EVENT_ACK                               = 0x0000,
    EVENT_REPLY_PAIRED_RECORD               = 0x0001,
    EVENT_LEGACY_DATA_TRANSFER              = 0x0002,
    EVENT_CONNECT_STATUS                    = 0x0003,
    EVENT_DISCONNECT_STATUS                 = 0x0004,
    EVENT_CALLER_ID                         = 0x0005,
    EVENT_IAP_STATUS                        = 0x0006,
    EVENT_DEVICE_STATE                      = 0x0007,
    EVENT_REPLY_LINK_INFO                   = 0x0008,
    EVENT_REPLY_REMOTE_NAME                 = 0x0009,
    EVENT_CALL_STATUS                       = 0x000A,
    EVENT_PLAYER_STATUS                     = 0x000B,
    EVENT_FEATURE_REPORT                    = 0x000C,
    EVENT_REPORT_SPP_INFO                   = 0x000D,
    EVENT_TTS                               = 0x000E,
    EVENT_VENDOR_AT_RESULT                  = 0x0010,
    EVENT_INFO_RSP                          = 0x0011,
    EVENT_RESUME_DATA_TRANSFER              = 0x0012,
    EVENT_VOLUME_SYNC                       = 0x0013,
    EVENT_LINK_STATUS                       = 0x0014,
    EVENT_CODEC_SETTING                     = 0x0015,
    EVENT_INITIATE_CONN_DONE                = 0x0016,
    EVENT_LANGUAGE_REPORT                   = 0x0017,
    EVENT_REPORT_CFG_TYPE                   = 0x0018,
    EVENT_REPORT_STATUS                     = 0x0019,  // used to report part of bud info
    EVENT_GET_BD_ADDR                       = 0x001A,
    EVENT_REPORT_DLPS_STATUS                = 0x001C,
    EVENT_GAMING_MODE_INFO                  = 0x001D,

    EVENT_AUDIO_VOL_CHANGE                  = 0x0020,
    EVENT_TRACK_CHANGED                     = 0x0021,
    EVENT_REPORT_BUD_INFO                   = 0x0022,  // used to report complete bud info

    EVENT_BT_READY                          = 0x0023,
    EVENT_REPORT_PIN_CODE                   = 0x0024,
    EVENT_INQUIRY_STATE                     = 0x0025,
    EVENT_SERVICES_SEARCH_STATE             = 0x0026,
    EVENT_PAIRING_REQUEST                   = 0x0027,
    EVENT_REPORT_SSP_NUMERIC_VALUE          = 0x0028,
    EVENT_LEGACY_RSSI                       = 0x0029,
    EVENT_REPORT_CONNECTED_DEV_ID           = 0x002A,
    EVENT_INQUIRY_RESULT                    = 0x002B,
    EVENT_REPORT_REMOTE_DEV_ATTR_INFO       = 0x002C,

    EVENT_DEDICATE_BUD_COUPLING             = 0x002D,

    EVENT_PROFILE_CONNECT_FAIL_STATUS       = 0x002F,

    EVENT_LE_CONNECTED                      = 0x0100,
    EVENT_LE_DISCONNECTED                   = 0x0101,
    EVENT_LE_PAIR_STATUS                    = 0x0102,
    EVENT_LE_DATA_TRANSFER                  = 0x0103,
    EVENT_LE_PUBLIC_ADDR                    = 0x0104,
    EVENT_LE_SCAN_INFO                      = 0x0105,
    EVENT_LE_SET_DATA_LEN                   = 0x0106,
    EVENT_LE_PHY_UPD                        = 0x0107,
    EVENT_LE_REMOTE_FEATURES                = 0x0108,
    EVENT_LE_BONDED_DEV                     = 0x0109,
    EVENT_LE_IGNORE_SLAVE_LATENCY           = 0x010A,
    EVENT_LE_AUDIO_SCAN_INFO                = 0x010B,

    EVENT_ANCS_REGISTER_COMPLETE            = 0x0110,
    EVENT_ANCS_NOTIFICATION                 = 0x0111,
    EVENT_ANCS_ACTION_RESULT                = 0x0112,

    EVENT_AMS_REGISTER_COMPLETE             = 0x0120,
    EVENT_AMS_ENTITY_UPD_NOTIFY             = 0x0121,
    EVENT_AMS_ENTITY_UPD_NOTIFY_EN          = 0x0122,
    EVENT_AMS_REMOTE_CMD_NOTIFY             = 0x0123,
    EVENT_AMS_REMOTE_CMD_NOTIFY_EN          = 0x0124,
    EVENT_AMS_ENTITY_UPD_RESULT             = 0x0125,

    EVENT_AUDIO_EQ_REPLY                    = 0x0200,
    EVENT_AUDIO_EQ_PARAM_REPORT             = 0x0201,
    EVENT_AUDIO_EQ_INDEX_REPORT             = 0x0202,

    EVENT_DSP_READ_PARAM                    = 0x0203,
    EVENT_DSP_REPORT_AUDIO_EQ               = 0x0204,
#if F_APP_APT_SUPPORT
    EVENT_APT_VOLUME_OUT_LEVEL              = 0x0205,
#endif
    EVENT_APT_EQ_INDEX_INFO                 = 0x0206,

    EVENT_AUDIO_DSP_CTRL_INFO               = 0x0207,
    // for equalize page
    EVENT_AUDIO_EQ_REPLY_PARAM              = 0x0208,
    EVENT_NOTIFICATION_VOL_STATUS           = 0x0209,
    EVENT_NOTIFICATION_VOL_LEVEL_SET        = 0x020A,
    EVENT_DSP_TOOL_FUNCTION_ADJUSTMENT      = 0x0213,

#if F_APP_SPP_CAPTURE_DSP_DATA_2
    EVENT_DSP_CAPTURE_V2_START_STOP_RESULT      = 0x0220,
    EVENT_DSP_CAPTURE_V2_DATA                   = 0x0221,
    EVT_DSP_CAPTURE_SCENARIO_ENTER_EXIT_RESULT  = 0x0222,
#endif

    EVENT_REPORT_NUM_OF_CONNECTION                  = 0x0315,
    EVENT_REG_ACCESS                                = 0x0316,
    EVENT_REPORT_IMAGE_INFO                         = 0x0319,
    EVENT_GET_MAX_TRANSMIT_SIZE                     = 0x031A,
    EVENT_DUMP_FLASH_DATA                           = 0x031B,

    EVENT_FLASH_BUFFER_CHECK                        = 0x031D,
    EVENT_DUMP_FLASH                                = 0x031E,
    EVENT_REPORT_DUMP_STATE                         = 0x031F,

    EVENT_FLASH_STATE_REPORT                        = 0x0322,
    EVENT_FEATURE_STATE_GET_RESULT                  = 0x0323,
    EVENT_RF_XTAL_K                                 = 0x032A,
    EVENT_RF_XTAL_K_GET_RESULT                      = 0x032B,

    EVENT_HFP_AG_CONN_IND                           = 0x0380,
    EVENT_HFP_AG_CONN_CMPL                          = 0x0381,
    EVENT_HFP_AG_DISCONN_CMPL                       = 0x0382,
    EVENT_HFP_AG_MIC_VOL_CHANGED                    = 0x0383,
    EVENT_HFP_AG_SPK_VOL_CHANGED                    = 0x0384,
    EVENT_HFP_AG_CALL_STATUS_CHANGED                = 0x0385,
    EVENT_HFP_AG_INDI_STATUS_REQ                    = 0x0386,
    EVENT_HFP_AG_BATT_LEVEL                         = 0x0387,
    EVENT_HFP_AG_HF_CODEC_TYPE_SELECTED             = 0x0388,
    EVENT_HFP_AG_SUPPORTED_FEATURES                 = 0x0389,
    EVENT_HFP_AG_INBAND_RINGING_REQ                 = 0x038A,
    EVENT_HFP_AG_CALL_ANSWER_REQ                    = 0x038B,
    EVENT_HFP_AG_CALL_TERM_REQ                      = 0x038C,
    EVENT_HFP_AG_CURR_CALLS_LIST_QUERY              = 0x038D,
    EVENT_HFP_AG_DTMF_CODE                          = 0x038E,
    EVENT_HFP_AG_DIAL_WITH_NUMBER                   = 0x038F,
    EVENT_HFP_AG_DIAL_LAST_NUMBER                   = 0x0390,
    EVENT_HFP_AG_3WAY_HELD_CALL_RELEASED            = 0x0391,
    EVENT_HFP_AG_3WAY_ACTIVE_CALL_RELEASED          = 0x0392,
    EVENT_HFP_AG_3WAY_SWITCHED                      = 0x0393,
    EVENT_HFP_AG_3WAY_MERGED                        = 0x0394,
    EVENT_HFP_AG_SUBSCRIBER_NUMBER_QUERY            = 0x0395,
    EVENT_HFP_AG_NETWORK_NAME_FORMAT_SET            = 0x0396,
    EVENT_HFP_AG_CURR_OPERATOR_QUERY                = 0x0397,
    EVENT_HFP_AG_ENHANCED_SAFETY_STATUS             = 0x0398,


    //0x0400 ~ 0x04FF reserved for profile
#if F_APP_HFP_CMD_SUPPORT
    EVENT_SCO_STATE                         = 0x0400,
    EVENT_CALL_WAITING                      = 0x0401,
    EVENT_HFP_SIGNAL                        = 0x0402,
    EVENT_HFP_ROAM                          = 0x0403,
    EVENT_HFP_SERVICE                       = 0x0404,
    EVENT_NETWORK_OPERATOR                  = 0x0405,
    EVENT_SUBSCRIBER_NUMBER                 = 0x0406,
    EVENT_CURRENT_CALLS                     = 0x0407,
    EVENT_DTMF                              = 0x0408,
    EVENT_REPORT_HFP_BATTERY                = 0x0409,
    EVENT_REPORT_AG_BRSF                    = 0x040A,
    EVENT_REPORT_INBAND_RINGTONE            = 0x040B,
#endif
    EVENT_RING_IND                          = 0x040C,


#if F_APP_AVRCP_CMD_SUPPORT
    EVENT_AVRCP_REPORT_LIST_SETTING_ATTR    = 0x0410,
    EVENT_AVRCP_REPORT_LIST_SETTING_VALUE   = 0x0411,
    EVENT_AVRCP_REPORT_CURRENT_VALUE        = 0x0412,
    EVENT_AVRCP_REPORT_SETTING_CHANGED      = 0x0413,
    EVENT_AVRCP_REPORT_ELEMENT_ATTR         = 0x0414,
    EVENT_AVRCP_REPORT_PLAYER_STATUS        = 0x0415,
#endif

#if F_APP_PBAP_CMD_SUPPORT
    EVENT_PBAP_REPORT_DATA                  = 0x0420,
    EVENT_PBAP_REPORT_SESSION_STATUS        = 0x0421,
    EVENT_PBAP_DOWNLOAD_START               = 0x0422,
    EVENT_PBAP_DOWNLOAD_CMPL                = 0x0423,
    EVENT_PBAP_REPORT_SIZE                  = 0x0424,
    EVENT_PBAP_CONNECT_FAIL                 = 0x0425,
#endif

    EVENT_OTA_DEV_INFO                      = 0x0600,
    EVENT_OTA_ACTIVE_BANK_VER               = 0x0601,
    EVENT_OTA_START                         = 0x0602,
    EVENT_OTA_PACKET                        = 0x0603,
    EVENT_OTA_VALID                         = 0x0604,
    EVENT_OTA_BUFFER_CHECK_ENABLE           = 0x0605,
    EVENT_OTA_BUFFER_CHECK                  = 0x0606,
    EVENT_OTA_IMG_INFO                      = 0x0607,
    EVENT_OTA_SECTION_SIZE                  = 0x0608,
    EVENT_OTA_DEV_EXTRA_INFO                = 0x0609,
    EVENT_OTA_PROTOCOL_TYPE                 = 0x060A,
    EVENT_OTA_ACTIVE_ACK                    = 0x060B,
    EVENT_OTA_GET_RELEASE_VER               = 0x060C,
    EVENT_OTA_INACTIVE_BANK_VER             = 0x060D,
    EVENT_OTA_COPY_IMG                      = 0x060E,
    EVENT_OTA_CHECK_SHA256                  = 0x060F,
    EVENT_OTA_ROLESWAP                      = 0x0610,

    EVENT_PLAYBACK_QUERY_INFO               = 0x0680,
    EVENT_PLAYBACK_GET_LIST_DATA            = 0x0681,
    EVENT_PLAYBACK_TRANS_START              = 0x0682,
    EVENT_PLAYBACK_REPORT_BUFFER_CHECK      = 0x0683,
    EVENT_PLAYBACK_VALID_SONG               = 0x0684,
    EVENT_PLAYBACK_TRIGGER_ROLE_SWAP        = 0x0685,
    EVENT_PLAYBACK_TRANS_CANCEL             = 0x0686,
    EVENT_PLAYBACK_EXIT_TRANS               = 0x0687,
    EVENT_PLAYBACK_PERMANENT_DELETE_SONG    = 0x0688,
    EVENT_PLAYBACK_PLAYLIST_ADD_SONG        = 0x0689,
    EVENT_PLAYBACK_PLAYLIST_DELETE_SONG     = 0x068A,
    EVENT_PLAYBACK_PERMANENT_DELETE_ALL_SONG = 0x068B,

    EVENT_REPORT_SUPPORTED_MMI_LIST         = 0x0700,
    EVENT_REPORT_SUPPORTED_CLICK_TYPE       = 0x0701,
    EVENT_REPORT_SUPPORTED_CALL_STATE       = 0x0702,
    EVENT_REPORT_KEY_MMI_MAP                = 0x0703,

    EVENT_REPORT_USER_BIN_VER               = 0x0705,
    EVENT_REPORT_PAD_VOLTAGE                = 0x0706,
    EVENT_PX318J_CALIBRATION_REPORT         = 0x0707,
    EVENT_REPORT_RWS_KEY_MMI_MAP            = 0x0708,
    EVENT_IQS773_STREAM_REPORT              = 0x0709,

    EVENT_VENDOR_SEPC                       = 0x8800, //It has been reserved for vendor customer A, please dont't use this value.

    EVENT_LED_TEST                          = 0x0900,
    EVENT_KEY_TEST                          = 0x0901,
    EVENT_CLEAR_MP_DATA                     = 0x0902,
    EVENT_LOCAL_ADDR                        = 0x0903,
    EVENT_BT_BOND_INFO_CLEAR                = 0x0904,
    EVENT_DEVICE_ROLE                       = 0x0905,
    EVENT_CRYSTAL_TRIM                      = 0x0906,
    EVENT_LINK_KEY                          = 0x0907,
    EVENT_COUNTRY_CODE                      = 0x0908,
    EVENT_FW_VERSION                        = 0x0909,
    EVENT_GET_FLASH_BKP_DATA                = 0x090A,
    EVENT_ADC_VALUE_1                       = 0x090B,
    EVENT_ADC_VALUE_2                       = 0x090C,
    EVENT_GET_UNSIZE_RAM                    = 0x090D,
    EVENT_REPORT_FLASH_DATA                 = 0x090E,
    EVENT_MIC_SWITCH                        = 0x090F,
    EVENT_FACTORY_RESET_DONE                = 0x0910,
    EVENT_REPORT_PACKAGE_ID                 = 0x0911,

    //for customize
    EVENT_JSA_CAL_RESULT                    = 0x0A02,
    EVENT_SITRON_CUSTOMIZED_FEATURE_REPORT  = 0x0A06,
    EVENT_MIC_MP_VERIFY_BY_HFP              = 0x0A07,
    EVENT_GET_DSP_CONFIG_GAIN               = 0x0A08,
    EVENT_CUSTOMIZED_TOZO_FEATURE           = 0x0A09,
    EVENT_RSV                               = 0x0A0A,
    EVENT_IO_PIN_PULL_HIGH                  = 0x0A0B,
    EVENT_ENTER_BAT_OFF_MODE                = 0x0A0C,
    EVENT_REPORT_SASS_FEATURE               = 0x0A0D,

    //for HCI command
    EVENT_ANC_VENDOR_REPORT                 = 0x0B00,
    EVENT_WDG_RESET                         = 0x0B01,
    EVENT_DUAL_MIC_MP_VERIFY                = 0x0B02,

    EVENT_SOUND_PRESS_CALIBRATION           = 0x0B10,
    EVENT_CAP_TOUCH_CTL                     = 0x0B11,
    EVENT_REPORT_SPACE_SENSOR_DATA          = 0x0B12,

    EVENT_SAVE_DATA_TO_FLASH                = 0x0B30,

    // ANC
    EVENT_ANC_TEST_MODE                     = 0x0C00,
    EVENT_ANC_WRITE_GAIN,
    EVENT_ANC_READ_GAIN,
    EVENT_ANC_BURN_GAIN,
    EVENT_ANC_COMPARE,
    EVENT_ANC_GEN_TONE,
    EVENT_ANC_CONFIG_DATA_LOG,
    EVENT_ANC_READ_DATA_LOG,
    EVENT_ANC_READ_MIC_CONFIG,
    EVENT_ANC_READ_SPEAKER_CHANNEL,
    EVENT_ANC_READ_REGISTER,                //0x0C0A
    EVENT_ANC_WRITE_REGISTER,
    EVENT_ANC_LLAPT_WRITE_GAIN              = 0x0C0C,
    EVENT_ANC_LLAPT_READ_GAIN               = 0x0C0D,
    EVENT_ANC_LLAPT_BURN_GAIN               = 0x0C0E,
    EVENT_ANC_LLAPT_FEATURE_CONTROL         = 0x0C0F,

    EVENT_ANC_GEN_TONE_CHANNEL_SET          = 0x0C10,
    EVENT_ANC_CONFIG_MEASURE_MIC            = 0x0C11,

    EVENT_ANC_QUERY                         = 0x0C20,
    EVENT_ANC_ENABLE_DISABLE                = 0x0C21,
    EVENT_LLAPT_QUERY                       = 0x0C22,
    EVENT_LLAPT_ENABLE_DISABLE              = 0x0C23,
    EVENT_SPECIFY_ANC_QUERY                 = 0x0C24,

    EVENT_RAMP_GET_INFO                     = 0x0C26,
    EVENT_RAMP_BURN_PARA_START              = 0x0C27,
    EVENT_RAMP_BURN_PARA_CONTINUE           = 0x0C28,
    EVENT_RAMP_BURN_GRP_INFO                = 0x0C29,
    EVENT_RAMP_MULTI_DEVICE_APPLY           = 0x0C2A,

    EVENT_LISTENING_MODE_CYCLE_SET          = 0x0C2B,
    EVENT_LISTENING_MODE_CYCLE_GET          = 0x0C2C,
    EVENT_CUSTOMIZE_SENSOR_ACK              = 0x0C2D,

    EVENT_APT_VOLUME_INFO                   = 0x0C2E,
    EVENT_APT_VOLUME_SET                    = 0x0C2F,
    EVENT_APT_VOLUME_STATUS                 = 0x0C30,
    EVENT_LLAPT_BRIGHTNESS_INFO             = 0x0C31,
    EVENT_LLAPT_BRIGHTNESS_SET              = 0x0C32,
    EVENT_LLAPT_BRIGHTNESS_STATUS           = 0x0C33,
    EVENT_LLAPT_SCENARIO_CHOOSE_INFO        = 0x0C36,
    EVENT_LLAPT_SCENARIO_CHOOSE_TRY         = 0x0C37,
    EVENT_LLAPT_SCENARIO_CHOOSE_RESULT      = 0x0C38,
    EVENT_APT_POWER_ON_DELAY_TIME           = 0x0C39,
    EVENT_LISTENING_STATE_SET               = 0x0C3B,
    EVENT_LISTENING_STATE_STATUS            = 0x0C3C,
    EVENT_APT_TYPE_QUERY                    = 0x0C3D,
    EVENT_ASSIGN_APT_TYPE                   = 0x0C3E,

    // ADSP PARA
    EVENT_ANC_GET_ADSP_INFO                 = 0x0C40,
    EVENT_ANC_SEND_ADSP_PARA_START          = 0x0C41,
    EVENT_ANC_SEND_ADSP_PARA_CONTINUE       = 0x0C42,
    EVENT_ANC_ENABLE_DISABLE_ADAPTIVE_ANC   = 0x0C43,

    EVENT_ANC_SCENARIO_CHOOSE_INFO          = 0x0C44,
    EVENT_ANC_SCENARIO_CHOOSE_TRY           = 0x0C45,
    EVENT_ANC_SCENARIO_CHOOSE_RESULT        = 0x0C46,

    EVENT_APT_VOLUME_MUTE_SET               = 0X0C47,
    EVENT_APT_VOLUME_MUTE_STATUS            = 0X0C48,

    EVENT_ANC_GET_SCENARIO_IMAGE            = 0x0C49,
    EVENT_ANC_GET_FILTER_INFO               = 0x0C4A,

    // OTA Tooling
    EVENT_OTA_TOOLING_PARKING               = 0x0D00,

    EVENT_LOW_LATENCY_STATUS = 0x0E00,      //0x0E00
    EVENT_EAR_DETECTION_STATUS = 0x0E01,    //0x0E01
    EVENT_LOW_LATENCY_LEVEL_SET             = 0x0E02,

    // one wire uart
    EVENT_MP_TEST_RESULT                    = 0x0F00,
    EVENT_FORCE_ENGAGE                      = 0x0F01,
    EVENT_AGING_TEST_CONTROL                = 0x0F02,

#if F_APP_SAIYAN_EQ_FITTING
    EVENT_HW_EQ_TEST_MODE                       = 0x1200,
    EVENT_HW_EQ_START_SET                       = 0x1201,
    EVENT_HW_EQ_CONTINUE_SET                    = 0x1202,
    EVENT_HW_EQ_GET_CALIBRATE_FLAG              = 0x1203,
    EVENT_HW_EQ_CLEAR_CALIBRATE_FLAG            = 0x1204,
    EVENT_HW_EQ_SET_TEST_MODE_TMP_EQ            = 0x1205,
    EVENT_HW_EQ_SET_TEST_MODE_TMP_EQ_ADVANCED   = 0x1206,
#endif

#if F_APP_HEARABLE_SUPPORT
    EVENT_HA_PROGRAM_INFO                   = 0x2000,
    EVENT_HA_SPK_RESPONSE_INFO              = 0x2001,
    EVENT_HA_DSP_CFG_GAIN_INFO              = 0x2002,
    EVENT_HA_AUDIO_VOLUME_INFO              = 0x2003,
    EVENT_HA_ADJUST_VOLUME_INFO             = 0x2004,
#endif

#if F_APP_DURIAN_SUPPORT
    EVENT_AVP_BUD_LOCAL                     = 0x2100,
    EVENT_AVP_ANC_SET                       = 0x2101,
    EVENT_AVP_PROFILE_CONNECT               = 0x2102,
    EVENT_AVP_NAME_SET                      = 0x2103,
    EVENT_AVP_CONTROL_SET                   = 0x2104,
    EVENT_AVP_CLICK_SET                     = 0x2105,
    EVENT_AVP_ANC_APT_CYCLE                 = 0x2106,
#endif

#if F_APP_SS_SUPPORT
    EVENT_SS_RESP                           = 0x2200,
#endif

#if F_APP_LEA_REALCAST_SUPPORT
    EVENT_LEA_SYNC_NOTIFY                   = 0x3000,
    EVENT_LEA_SYNC_CANCEL_NOTIFY            = 0x3001,
    EVENT_LEA_SCAN_NOTIFY                   = 0x3002,
    EVENT_LEA_SCAN_STOP_NOTIFY              = 0x3003,
    EVENT_LEA_PA_NOTIFY                     = 0x3004,
    EVENT_LEA_PA_STOP_NOTIFY                = 0x3005,
    EVENT_LEA_DEVICE_STATE_NOTIFY           = 0x3006,
#endif

    EVENT_GCSS_REG                  = 0x3100,
    EVENT_GCSS_DATA_TRANSFER        = 0x3101,
    EVENT_GCSS_CON_NOTIFY           = 0x3102,
    EVENT_GCSS_INFO                 = 0x3103,
    EVENT_GCSS_READ_REQ             = 0x3104,
    EVENT_GCSS_IND_ACK              = 0x3105,
    EVENT_GCSS_SERV_CHG             = 0x3106,
    EVENT_GCSS_NOTI_CREDIT_UPD      = 0x3107,

    EVENT_GCSC_DISCOVER_STATE                = 0x3140,
    EVENT_GCSC_DISCOVER_RES                  = 0x3141,
    EVENT_GCSC_READ_RES                      = 0x3142,
    EVENT_GCSC_WRITE_RES                     = 0x3143,
    EVENT_GCSC_NOTI_IND                      = 0x3144,
    EVENT_GCSC_DISC                          = 0x3145,
    EVENT_GCSC_DISCOVER_ALL_END              = 0x3146,

    //for MAP test
    EVENT_MAP_SDP_REQUEST                   = 0x3800,
    EVENT_MAP_CONNECT                       = 0x3801,
    EVENT_MAP_GET_FOLDER_LISTING            = 0x3802,
    EVENT_MAP_FOLDER_LISTING_DATA_CONTINUE  = 0x3803,
    EVENT_MAP_FOLDER_LISTING_DATA_END       = 0x3804,
    EVENT_MAP_GET_MESSAGE_LISTING           = 0x3805,
    EVENT_MAP_MSG_LISTING_DATA_CONTINUE     = 0x3806,
    EVENT_MAP_MSG_LISTING_DATA_END          = 0x3807,
    EVENT_MAP_SET_FOLDER                    = 0x3808,
    EVENT_MAP_REG_MSG_NOTIFICATION          = 0x3809,
    EVENT_MAP_GET_MESSAGE                   = 0x380A,
    EVENT_MAP_MSG_DATA_CONTINUE             = 0x380B,
    EVENT_MAP_MSG_DATA_END                  = 0x380C,

    EVENT_XM_SPP_DATA_TRANSFER              = 0x8000,
    EVENT_XM_SPP_CONNECT_STATUS             = 0x8001,
    EVENT_XM_SPP_DISCONNECT_STATUS          = 0x8002,
    EVENT_XM_BR_ATT_CONNECTED               = 0x8003,
    EVENT_XM_BR_ATT_DISCONNECTED            = 0x8004,
    EVENT_XM_GATT_DATA_TRANSFER             = 0x8005,
    EVENT_XM_GATT_CREDIT_UPDATE             = 0x8006,
#if F_APP_CUSTOMER_CONN_MANAGER_SUPPORT
    EVENT_XM_ALL_CONNECT_INFO               = 0x8007,
#endif
    EVENT_XM_BT_SDP_RECORD_IND              = 0x8008,
    EVENT_XM_BT_SDP_RECORD_CMPL             = 0x8009,
    EVENT_XM_BT_SDP_RECORD_DISCONN          = 0x800A,
    EVENT_XM_BT_SDP_RECORD_DATA_IND         = 0x800B,
    EVENT_XM_USER_CONFIRMATION_REQ          = 0x800C,
    EVENT_XM_BT_ACL_SNIFF_STATUS            = 0x800D,

    EVENT_XM_LE_ADV_STATE                   = 0x8040,
    EVENT_XM_LE_ATT_MTU                     = 0x8041,
    EVENT_XM_LE_ADDR                        = 0x8042,
    EVENT_XM_LE_CON_STATE                   = 0x8043,
    EVENT_XM_LE_CON_PARAM                   = 0x8044,
    EVENT_XM_GATT_CON_NOTIFY                = 0x8045,
    EVENT_XM_LE_USER_CONFIRMATION_REQ       = 0x8046,
    EVENT_XM_LE_LINK_KEY                    = 0x8047,
    EVENT_XM_LE_ADV_CREATED                 = 0x8048,
    EVENT_XM_GET_PHY                        = 0x8049,

    EVENT_XM_MUSIC_REQ_FRAME                = 0x8080,
    EVENT_XM_TTS_REQ_FRAME                  = 0x8081,
    EVENT_XM_RECORDING_DATA                 = 0x8082,
    EVENT_XM_REOCRD_SP_STOP                 = 0x8083,
    EVENT_XM_RECORD_PLAY_REQ_FRAME          = 0x8084,
    EVENT_XM_AUDIO_PLAY_TIME                = 0x8085,
    EVENT_XM_AUDIO_TEST_PA_ID               = 0x8086,
    EVENT_XM_BT_AVRCP_STATUS                = 0x8087,
    EVENT_XM_AVRCP_ELEM_ATTRS_GET           = 0x8088,
    EVENT_XM_BT_AVRCP_GET_ABS_SUPPORTED     = 0x8089,

    EVENT_XM_FACTORY_RESET_STATUS           = 0x80C0,

    EVENT_TOTAL
} T_EVENT_ID;

//for EVENT_CALLER_ID
#define CALLER_ID_TYPE_NUMBER           0x00
#define CALLER_ID_TYPE_NAME             0x01

/**
    * @brief  report app event by spp/le/uart
    * @param  cmd_path br/le/uart
    * @param  event_id event id
    * @param  app_index BR/LE link index
    * @param  data event data
    * @param  len  event len
    * @return void
    */
void app_report_event(uint8_t cmd_path, uint16_t event_id, uint8_t app_index, uint8_t *data,
                      uint16_t len);

/**
    * @brief  broadcast event to all path and all devices: UART, br link and le link.
    * @param  event_id @ref T_EVENT_ID
    * @param  buf content in the event packet
    * @param  len buf length.
    * @return void
    */
void app_report_event_broadcast(uint16_t event_id, uint8_t *buf, uint16_t len);

/**
    * @brief  report raw data by spp/le/uart
    * @param  cmd_path br/le/uart
    * @param  app_index BR/LE link index
    * @param  data event data
    * @param  len  event len
    * @return void
    */
void app_report_raw_data(uint8_t cmd_path, uint8_t app_index, uint8_t *data,
                         uint16_t len);

/**
    * @brief  report eq_index to phone
    * @param  eq_data_update_event
    * @return void
    */
void app_report_eq_idx(T_EQ_DATA_UPDATE_EVENT eq_data_update_event);

/**
    * @brief  report rws state to phone
    * @param  none
    * @return void
    */
void app_report_rws_state(void);

/**
    * @brief  report rws bud side to phone
    * @param  none
    * @return void
    */
void app_report_rws_bud_side(void);

#if F_APP_APT_SUPPORT
/**
    * @brief  report apt eq index to phone
    * @param  apt_eq_data_update_event
    * @return void
    */
void app_report_apt_eq_idx(T_APT_EQ_DATA_UPDATE_EVENT apt_eq_data_update_event);
#endif

/**
    * @brief  report rws bud info to phone
    * @param  none
    * @return void
    */
void app_report_rws_bud_info(void);

/**
    * @brief  Get bud info
    * @param  data bud info data
    * @return void
    */
void app_report_get_bud_info(uint8_t *data);

/**
    * @brief  report gaming mode info
    * @param  none
    * @return void
    */
void app_report_gaming_mode_info(void);

/** End of APP_REPORT
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_REPORT_H_ */
