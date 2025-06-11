/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TTS_H_
#define _APP_TTS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_TTS App BLE TTS
  * @brief App BLE TTS
  * @{
  */

/**  @brief  phone send this cmd to soc.*/
typedef enum
{
    TTS_SESSION_OPEN = 0x00,
    TTS_SESSION_PAUSE = 0x01,
    TTS_SESSION_RESUME = 0x02,
    TTS_SESSION_ABORT = 0x03,
    TTS_SESSION_CLOSE = 0x04,
    TTS_SESSION_SEND_SINGLE_FRAME = 0x05,
    TTS_SESSION_SEND_START_FRAME = 0x06,
    TTS_SESSION_SEND_CONTINUE_FRAME = 0x07,
    TTS_SESSION_SEND_END_FRAME = 0x08
} T_TTS_SESSION_CMD_TYPE;

/**  @brief  soc send this event to phone.*/
typedef enum
{
    TTS_SESSION_OPEN_REQ = 0x00,
    TTS_SESSION_PAUSE_REQ = 0x01,
    TTS_SESSION_RESUME_REQ = 0x02,
    TTS_SESSION_ABORT_REQ = 0x03,
    TTS_SESSION_CLOSE_REQ = 0x04,
    TTS_SESSION_SEND_ENCODE_DATA = 0x05
} T_TTS_SESSION_EVENT_TYPE;

typedef struct t_tts_info
{
    void        *tts_handle;
    uint8_t     tts_state;
    uint8_t     tts_seq;
    uint16_t    tts_data_offset;
    uint8_t     *tts_frame_buf;
    uint16_t    tts_frame_len;
} T_TTS_INFO;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_TTS_Exported_Functions App Ble TTS Functions
    * @{
    */
/**
    * @brief  Ble TTS module init
    * @param  void
    * @return void
    */
void app_tts_init(void);
void app_tts_handle_power_off(void);
void app_tts_ble_disconnect_cb(uint8_t conn_id, uint8_t local_disc_cause, uint16_t disc_cause);
void app_tts_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                        uint8_t *ack_pkt);

/** @} */ /* End of group APP_BLE_TTS_Exported_Functions */

/** End of APP_BLE_DEVICE
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TTS_H_ */
