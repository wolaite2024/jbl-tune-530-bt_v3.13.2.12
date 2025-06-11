/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_GFPS_SASS_CONN_STATUS_H_
#define _APP_GFPS_SASS_CONN_STATUS_H_

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "gfps.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_GFPS_SASS_ADV App Gfps
  * @brief App Gfps
  * @{
  */
/**
 * @brief connection status type
 * 0bLLLL0101: type = TYPE_CONNECTION_STATUS(0b0101)
 */
#define SASS_TYPE_CONN_STATUS               0x05
#define SASS_TYPE_ENCRYPT_CONN_STATUS       0x06
/**
 * @brief 0bH = on head detection
 * SASS_NOT_ON_HEAD: not on head or there is no OHD sensor.
 * SASS_ON_HEAD: on head now
 */
typedef enum
{
    SASS_NOT_ON_HEAD = 0x00,
    SASS_ON_HEAD     = 0x01,
} T_SASS_ON_HEAD_DETECTION;

/**
 * @brief 0bA = connection availability
 * SASS_CONN_FULL: there is no available connection
 * SASS_CONN_NOT_FULL: there is an available connection
 */
typedef enum
{
    SASS_CONN_FULL     = 0x00,
    SASS_CONN_NOT_FULL = 0x01,
} T_SASS_CONN_AVAILABLE;

/**
 * @brief 0bF = focus mode
 * SASS_NOT_IN_FOCUS: not in focused mode now
 * SASS_IN_FOCUS: in focused mode now,connection switching is not allowed for media usage
 */
typedef enum
{
    SASS_NOT_IN_FOCUS = 0x00,
    SASS_IN_FOCUS     = 0x01,
} T_SASS_FOCUS_MODE;

/**
 * @brief 0bR = auto-reconnected
 * SASS_AUTO_RECONN_DISABLE: the current connection is not auto-reconnected by the provider.
 * SASS_AUTO_RECONN_ENABLE: the current connection is auto-reconnected by the provider.
 */
typedef enum
{
    SASS_AUTO_RECONN_DISABLE = 0x00,
    SASS_AUTO_RECONN_ENABLE  = 0x01,
} T_SASS_AUTO_RECONN;

/**
 * @brief Connection state
 * SASS_NO_CONN: no connection
 * SASS_PAGING: paging
 * SASS_CONN_NO_DATA_TRANS: connected but no data transferring
 * SASS_NON_AUDIO_DATA_TRANS: Non-audio data transferring (e.g. firmware image)
 * SASS_A2DP_ONLY: A2DP streaming only
 * SASS_A2DP_WITH_AVRCP:A2DP streaming with AVRCP
 * SASS_HFP: HFP (phone/voip call) streaming, including inband and non-inband ringtone
 * SASS_LEA_MEDIA_WITHOUT_CONTROL:LE audio - media streaming without control
 * SASS_LEA_MEDIA_WITH_CONTROL: LE audio - media streaming with control
 * SASS_LEA_CALL: LE audio - call streaming
 * SASS_LEA_BIS: LE audio - BIS
 * SASS_DISABLE_CONN_SWITCH: Disable connection switch
 *
 */
typedef enum
{
    SASS_NO_CONN                   = 0x00,
    SASS_PAGING                    = 0x01,
    SASS_CONN_NO_DATA_TRANS        = 0x02,
    SASS_NON_AUDIO_DATA_TRANS      = 0x03,
    SASS_A2DP_ONLY                 = 0x04,
    SASS_A2DP_WITH_AVRCP           = 0x05,
    SASS_HFP                       = 0x06,
    SASS_LEA_MEDIA_WITHOUT_CONTROL = 0x07,
    SASS_LEA_MEDIA_WITH_CONTROL    = 0x08,
    SASS_LEA_CALL                  = 0x09,
    SASS_LEA_BIS                   = 0x0A,

    SASS_DISABLE_CONN_SWITCH       = 0x0F,
} T_SASS_CONN_STATE;

/**
 * @brief uint8 Connection state 0bHAFRSSSS
 * conn_state: @ref T_SASS_CONN_STATE
 * auto_reconn: @ref T_SASS_AUTO_RECONN
 * focus_mode: @ref T_SASS_FOCUS_MODE
 * conn_availability: @ref T_SASS_CONN_AVAILABLE
 * on_head_detection: @ref T_SASS_ON_HEAD_DETECTION
 */
typedef struct
{
    uint8_t conn_state        : 4;
    uint8_t auto_reconn       : 1;
    uint8_t focus_mode        : 1;
    uint8_t conn_availability : 1;
    uint8_t on_head_detection : 1;
} T_SASS_CONN_STATUS_INFO;

/**
 * @brief Provider shall include its current connection status in the advertisement
 * length_type:  for example 0bLLLL0101 length(L): varies, type = SASS_TYPE_CONN_STATUS(0b0101).
 * conn_status_info: 0bHAFRSSSS @ref T_SASS_CONN_STATUS_INFO
 * custom_data: The Seeker will send it to the Provider,0 if the current active streaming is not from Seeker.
 * conn_dev_bitmap:
 */
typedef struct t_sass_adv_connection_status
{
    uint8_t length_type;
    T_SASS_CONN_STATUS_INFO conn_status_info;
    uint8_t custom_data;
    uint8_t conn_dev_bitmap;
} T_SASS_CONN_STATUS_FIELD;

/**
 * @brief get current conn status used in adv data @ref T_SASS_CONN_STATUS_FIELD
 *
 * @param[out] p_sass_conn_status return current conn status used in adv data
 * @return true success
 * @return false fail
 */
bool gfps_sass_get_conn_status(T_SASS_CONN_STATUS_FIELD *p_sass_conn_status);

/**
 * @brief set current conn status
 * when conn status changed, update conn status
 * when conn status no changed, do not need update conn status
 *
 * @param[in] p_sass_conn_status current conn status
 * @return true: new connection status set sucess
 * @return false: new connection status is equal to old connection status, do not need update
 */
bool gfps_sass_set_conn_status(T_SASS_CONN_STATUS_FIELD *p_sass_conn_status);

/**
 * @brief init sass conn status field. set default value for conn status field.
 *
 * <b>Example usage</b>
 * \code{.c}
    //google Fast pair initialize
    void app_gfps_init(void)
    {
        uint8_t sec_req_enable = false;
        if (app_gfps_account_key_init(app_cfg_const.gfps_account_key_num) == false)
        {
            goto error;
        }
        gfps_sass_conn_status_init();
    ......
    }
 * \endcode
 */
void gfps_sass_conn_status_init(void);
/** End of APP_GFPS_SASS_ADV
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
