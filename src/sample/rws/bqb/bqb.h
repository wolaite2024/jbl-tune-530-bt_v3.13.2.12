/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _BQB_H_
#define _BQB_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @defgroup APP_RWS_BQB App bqb
 * @brief Provides BQB interfaces for RWS BQB test.
 * @{
 */


/** @brief BQB command */
typedef enum
{
    BQB_CMD_NULL            = 0x0000,
    BQB_CMD_ENTER           = 0x0001,
    BQB_CMD_EXIT            = 0x0002,
    BQB_CMD_RESET           = 0x0003,
    BQB_CMD_POWER           = 0x0004,
    BQB_CMD_PAIR            = 0x0005,
    BQB_CMD_SDP             = 0x0006,
    BQB_CMD_AVDTP           = 0x0007,
    BQB_CMD_AVRCP           = 0x0008,
    BQB_CMD_RFCOMM          = 0x0009,
    BQB_CMD_HFHS            = 0x000A,
    BQB_CMD_PBAP            = 0x000B,
    BQB_CMD_HFHS_AG         = 0x000C,
    BQB_CMD_MAP             = 0x000D,
    BQB_CMD_MCP             = 0x0010,
    BQB_CMD_CCP             = 0x0011,
    BQB_CMD_VCS             = 0x0012,
    BQB_CMD_PBP             = 0x0013,
    BQB_CMD_TMAP            = 0x0014,
    BQB_CMD_HAP             = 0x0015,
} T_BQB_CMD;


#define BQB_ACTION_POWER_ON                 0x01
#define BQB_ACTION_POWER_OFF                0x02

#define BQB_ACTION_PAIR_START               0x01
#define BQB_ACTION_PAIR_STOP                0x02

#define BQB_ACTION_SDP_SEARCH               0x01

#define BQB_ACTION_AVDTP_DISCOVER           0x01
#define BQB_ACTION_AVDTP_GET_CPBS           0x02
#define BQB_ACTION_AVDTP_SET_CFG            0x03
#define BQB_ACTION_AVDTP_GET_CFG            0x04
#define BQB_ACTION_AVDTP_RECFG              0x05
#define BQB_ACTION_AVDTP_OPEN               0x06
#define BQB_ACTION_AVDTP_START              0x07
#define BQB_ACTION_AVDTP_CLOSE              0x08
#define BQB_ACTION_AVDTP_SUSPEND            0x09
#define BQB_ACTION_AVDTP_ABORT              0x0A
#define BQB_ACTION_AVDTP_SECURITY_CONTROL   0x0B
#define BQB_ACTION_AVDTP_GET_ALL_CPBS       0x0C
#define BQB_ACTION_AVDTP_DELAY_REPORT       0x0D
#define BQB_ACTION_AVDTP_CONNECT_SIGNAL     0x20
#define BQB_ACTION_AVDTP_CONNECT_STREAM     0x21
#define BQB_ACTION_AVDTP_DISCONNECT         0x22

#define BQB_ACTION_AVRCP_CONNECT            0x01
#define BQB_ACTION_AVRCP_CONNECT_CONTROLLER 0x02
#define BQB_ACTION_AVRCP_CONNECT_TARGET     0x03
#define BQB_ACTION_AVRCP_GET_PLAY_STATUS    0x04
#define BQB_ACTION_AVRCP_GET_ELEMENT_ATTR   0x05
#define BQB_ACTION_AVRCP_PLAY               0x06
#define BQB_ACTION_AVRCP_PAUSE              0x07
#define BQB_ACTION_AVRCP_STOP               0x08
#define BQB_ACTION_AVRCP_REWIND             0x09
#define BQB_ACTION_AVRCP_FASTFORWARD        0x0A
#define BQB_ACTION_AVRCP_FORWARD            0x0B
#define BQB_ACTION_AVRCP_BACKWARD           0x0C
#define BQB_ACTION_AVRCP_NOTIFY_VOLUME      0x0D
#define BQB_ACTION_AVRCP_DISCONNECT         0x0E

#define BQB_ACTION_RFCOMM_CONNECT_SPP       0x01
#define BQB_ACTION_RFCOMM_CONNECT_HFP       0x02
#define BQB_ACTION_RFCOMM_CONNECT_HSP       0x03
#define BQB_ACTION_RFCOMM_CONNECT_PBAP      0x04
#define BQB_ACTION_RFCOMM_DISCONNECT_SPP    0x05
#define BQB_ACTION_RFCOMM_DISCONNECT_HFP    0x06
#define BQB_ACTION_RFCOMM_DISCONNECT_HSP    0x07
#define BQB_ACTION_RFCOMM_DISCONNECT_PBAP   0x08
#define BQB_ACTION_RFCOMM_CONNECT_HFP_AG    0x09
#define BQB_ACTION_RFCOMM_CONNECT_HSP_AG    0x0A
#define BQB_ACTION_RFCOMM_DISCONNECT_HFP_AG 0x0B
#define BQB_ACTION_RFCOMM_DISCONNECT_HSP_AG 0x0C
#define BQB_ACTION_RFCOMM_DISCONNECT_ALL    0x0D

#define BQB_ACTION_HFHS_CONNECT_SCO                   0x01
#define BQB_ACTION_HFHS_DISCONNECT_SCO                0x02
#define BQB_ACTION_HFHS_CALL_ANSWER                   0x03
#define BQB_ACTION_HFHS_CALL_ACTIVE                   0x04
#define BQB_ACTION_HFHS_CALL_END                      0x05
#define BQB_ACTION_HFHS_CALL_REJECT                   0x06
#define BQB_ACTION_HFHS_CALL_REDIAL                   0x07
#define BQB_ACTION_HFHS_SPK_GAIN_LEVEL_REPORT         0x08
#define BQB_ACTION_HFHS_MIC_GAIN_LEVEL_REPORT         0x09
#define BQB_ACTION_HFHS_VOICE_RECOGNITION_ACTIVATE    0x0A
#define BQB_ACTION_HFHS_VOICE_RECOGNITION_DEACTIVATE  0x0B
#define BQB_ACTION_HFHS_SCO_CONN_REQ                  0x0C

#define BQB_ACTION_HFHS_AG_CONNECT_SCO                0x01
#define BQB_ACTION_HFHS_AG_DISCONNECT_SCO             0x02
#define BQB_ACTION_HFHS_AG_CALL_INCOMING              0x03
#define BQB_ACTION_HFHS_AG_CALL_ANSWER                0x04
#define BQB_ACTION_HFHS_AG_CALL_TERMINATE             0x05
#define BQB_ACTION_HFHS_AG_CALL_DIAL                  0x06
#define BQB_ACTION_HFHS_AG_CALL_ALERTING              0x07
#define BQB_ACTION_HFHS_AG_CALL_WAITING               0x08
#define BQB_ACTION_HFHS_AG_MIC_GAIN_LEVEL_REPORT      0x09
#define BQB_ACTION_HFHS_AG_SPEAKER_GAIN_LEVEL_REPORT  0x0A
#define BQB_ACTION_HFHS_AG_RING_INTERVAL_SET          0x0B
#define BQB_ACTION_HFHS_AG_INBAND_RINGING_SET         0x0C
#define BQB_ACTION_HFHS_AG_CURRENT_CALLS_LIST_SEND    0x0D
#define BQB_ACTION_HFHS_AG_SEVICE_STATUS_SEND         0x0E
#define BQB_ACTION_HFHS_AG_CALL_STATUS_SEND           0x0F
#define BQB_ACTION_HFHS_AG_CALL_SETUP_STATUS_SEND     0x10
#define BQB_ACTION_HFHS_AG_CALL_HELD_STATUS_SEND      0x11
#define BQB_ACTION_HFHS_AG_SIGNAL_STRENGTH_SEND       0x12
#define BQB_ACTION_HFHS_AG_ROAMING_STATUS_SEND        0x13
#define BQB_ACTION_HFHS_AG_BATTERY_CHANGE_SEND        0x14
#define BQB_ACTION_HFHS_AG_OK_SEND                    0x15
#define BQB_ACTION_HFHS_AG_ERROR_SEND                 0x16
#define BQB_ACTION_HFHS_AG_NETWORK_OPERATOR_SEND      0x17
#define BQB_ACTION_HFHS_AG_SUBSCRIBER_NUMBER_SEND     0x18
#define BQB_ACTION_HFHS_AG_INDICATORS_STATUS_SEND     0x19
#define BQB_ACTION_HFHS_AG_VENDOR_CMD_SEND            0x1A
#define BQB_ACTION_HFHS_AG_SCO_CONN_REQ               0x1B

#define BQB_ACTION_PBAP_VCARD_SRM               0x01
#define BQB_ACTION_PBAP_VCARD_NOSRM             0x02
#define BQB_ACTION_PBAP_VCARD_ENTRY             0x03

#define BQB_ACTION_MAP_CONNECT_MAS                    0x01
#define BQB_ACTION_MAP_DISCONNECT_MAS                 0x02
#define BQB_ACTION_MAP_MNS_ON                         0x03
#define BQB_ACTION_MAP_MNS_OFF                        0x04
#define BQB_ACTION_MAP_FOLDER_LISTING                 0x05
#define BQB_ACTION_MAP_MSG_LISTING                    0x06
#define BQB_ACTION_MAP_GET_MSG                        0x07
#define BQB_ACTION_MAP_UPLOAD                         0x08
#define BQB_ACTION_MAP_UPLOAD_SRM                     0x09
#define BQB_ACTION_MAP_SET_FOLDER_INBOX               0x0a
#define BQB_ACTION_MAP_SET_FOLDER_OUTBOX              0x0b

#define BQB_ACTION_MCP_PLAY                     0x01
#define BQB_ACTION_MCP_PAUSE                    0x02
#define BQB_ACTION_MCP_STOP                     0x03
#define BQB_ACTION_MCP_FASTFORWARD              0x04
#define BQB_ACTION_MCP_FASTFORWARD_STOP         0x05
#define BQB_ACTION_MCP_REWIND                   0x06
#define BQB_ACTION_MCP_REWIND_STOP              0x07
#define BQB_ACTION_MCP_FORWARD                  0x08
#define BQB_ACTION_MCP_BACKWARD                 0x09

#define BQB_ACTION_CAP_ACCEPT                   0x01
#define BQB_ACTION_CAP_TERMINATE                0x02
#define BQB_ACTION_CAP_LOCAL_HOLD               0x03
#define BQB_ACTION_CAP_LOCAL_RETRIEVE           0x04

#define BQB_ACTION_VCS_UP                       0x01
#define BQB_ACTION_VCS_DOWN                     0x02

#define BQB_ACTION_PBP_BMR_START                0x01
#define BQB_ACTION_PBP_BMR_STOP                 0x02
#define BQB_ACTION_PBP_BROADCAST_CODE           0x03

#define BQB_ACTION_TMAP_ADV_START               0x01
#define BQB_ACTION_TMAP_ADV_STOP                0x02
#define BQB_ACTION_TMAP_GAMING_MODE             0x03
#define BQB_ACTION_TMAP_ORIGINATE_CALL          0x04
#define BQB_ACTION_TMAP_TERMINATE_CALL          0x05

#define BQB_ACTION_HAP_FEATURE                  0x01
#define BQB_ACTION_HAP_ACTIVE_INDEX             0x02
#define BQB_ACTION_HAP_PRESET_NAME              0x03
#define BQB_ACTION_HAP_PRESET_AVAILABLE         0x04
#define BQB_ACTION_HAP_PRESET_UNAVAILABLE       0x05
#define BQB_ACTION_HAP_PRESET_ADD               0x06
#define BQB_ACTION_HAP_PRESET_DELETE            0x07
#define BQB_ACTION_HAP_RTK_ADV_START            0x08

/**
  * @brief  Register bqb command.
  * @param  None
  * @return The status of registering command.
  * @retval true    Command has been registered successfully.
  * @retval false   Command was fail to register.
  */
bool bqb_cmd_register(void);

/** End of APP_RWS_BQB
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BQB_H_ */
