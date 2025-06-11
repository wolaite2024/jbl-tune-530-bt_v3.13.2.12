/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      sdcard_fs_app.h
* @brief     header file of sdcard file system application driver.
* @details
* @author   elliot chen
* @date     2019-1-14
* @version   v1.0
* *********************************************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _APP_PLAYBACK_H_
#define _APP_PLAYBACK_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "rtl876x.h"
#include "remote.h"

#define PLAYBACK_PLAYLIST_PAUSE_EN  1

#define PLAYBACK_PLAYLIST_NUM   8
#define FILE_NAME_LEN   (PATH_LEN * sizeof(TCHAR))
#define PLAYBACK_TRACK_STATE_CLEAR        0xFF

#define PLAYBACK_PUT_DATA_PKT_NUM           4
#define PLAYBACK_PUT_DATA_TIME_MS           (PLAYBACK_PUT_DATA_PKT_NUM * 20)
#define PLAYBACK_SINGLE_PREQ_PKTS(sf)        sf * (PLAYBACK_LATENCY + PLAYBACK_LOW_LEVEL) / 1000 / 1024 + 3
#define PLAYBACK_COUPLE_PREQ_PKTS(sf)        sf * PLAYBACK_LATENCY / 1000 / 1024 - 1
#define PLAYBACK_INTERVAL_TIMER_MS(pkt_num, sf)    pkt_num * 1000 * 1024 / sf

typedef struct
{
    uint8_t                     switch_mode;
    uint8_t                     play_state;
    uint32_t                    file_offset;
    uint16_t                    file_crc;
    uint16_t                    name_len;
    uint8_t                     playlist_idx;
    uint16_t                    volume;

} T_PLAYBACK_INFO;

typedef struct
{
    uint16_t                    file_idx;
    uint32_t                    file_offset;
    uint16_t                    file_crc;
    uint16_t                    name_len;
    uint8_t                     playlist_idx;
    uint16_t                    volume;

} T_PLAYBACK_INFO_TYPE2;

typedef struct
{
    uint8_t                     pkt_num;
    uint16_t                    seq_num;
    uint32_t                    file_offset;
} T_PLAYBACK_PUT_DATA_INFO;
/** Little Endian array to uint16 */
#define PLAYBACK_LE_ARRAY_TO_UINT16(u16, a)  {          \
        u16 = ((uint16_t)(*(a + 0)) << 0) +             \
              ((uint16_t)(*(a + 1)) << 8);              \
    }

/** Little Endian array to uint32 */
#define PLAYBACK_LE_ARRAY_TO_UINT32(u32, a) {           \
        u32 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16) +            \
              ((uint32_t)(*(a + 3)) << 24);             \
    }


/** Little Endian uint16 to array */
#define PLAYBACK_LE_UINT16_TO_ARRAY(a, u16)  {          \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 0);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 8);  \
    }

/** Little Endian uint32 to array */
#define PLAYBACK_LE_UINT32_TO_ARRAY(a, u32) {           \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >> 24); \
    }

typedef enum
{
    PLAYBACK_SUCCESS            = 0x00,
    PLAYBACK_READ_ERROR         = 0x01,
    PLAYBACK_IDX_ERROR          = 0x02,
    PLAYBACK_OPEN_FILE_ERROR    = 0x03,
    PLAYBACK_CRC_ERROR          = 0x04,
    PLAYBACK_CLOSE_FILE_ERROR   = 0x05,
    PLAYBACK_PLAYLIST_ERROR     = 0x06,
    PLAYBACK_WRITE_ERROR        = 0x07,
} T_PLAYBACK_ERROR;

typedef enum
{
//    PLAYBACK_STATE_INIT,
    PLAYBACK_STATE_PAUSE    = 0x00,
    PLAYBACK_STATE_PLAY,
    PLAYBACK_STATE_TOTAL,
} T_PLAYBACK_STATE;
typedef enum
{
    PLAYBACK_QUIT       = 0x00,
    PLAYBACK_ENTER      = 0x01,
    PLAYBACK_UPDATA     = 0x02,
} T_PLAYBACK_MODE;

typedef enum
{
    PLAYBACK_BUF_NORMAL         = 0x00,
    PLAYBACK_BUF_LOW            = 0x01,
    PLAYBACK_BUF_HIGH           = 0x02,
} T_PLAYBACK_BUF_STATE;

typedef struct
{
    T_PLAYBACK_STATE            sd_play_state;
    uint32_t                    sample_rate;
    uint8_t                     buffer_state;
    uint8_t                     op_next_action;
    uint8_t                     sec_track_state;
    uint8_t                     local_track_state;
    uint8_t                     volume;
    uint8_t                     playlist_idx;
    uint16_t                    file_idx;
    uint16_t                    seq_num;

} T_PLAYBACK_DATA;

extern T_PLAYBACK_DATA playback_db;
extern T_AUDIO_FS_HANDLE playback_audio_fs_handle;

/** @defgroup APP_PLAYBACK App playback
  * @brief App playback
  * @{
  */

/**  @brief app remote msg*/
typedef enum
{
    PLAYBACK_REMOTE_MSG_START_SYNC                      = 0x00,
    PLAYBACK_REMOTE_MSG_SWITCH_SYNC                     = 0x01,
    PLAYBACK_REMOTE_MSG_PUT_DATA_SYNC                   = 0x02,
    PLAYBACK_REMOTE_MSG_PRI_STARTED_SYNC                = 0x03,
    PLAYBACK_REMOTE_MSG_SEC_STARTED_SYNC                = 0x04,
    PLAYBACK_REMOTE_MSG_CHECK_FILE_SYNC                 = 0x05,
    PLAYBACK_REMOTE_MSG_CHECK_RESULT_SYNC               = 0x06,
    PLAYBACK_REMOTE_MSG_SOLO_COUPLE_PRI_SYNC            = 0x07,
    PLAYBACK_REMOTE_MSG_SOLO_COUPLE_SEC_SYNC            = 0x08,
    PLAYBACK_REMOTE_MSG_STOP_SYNC                       = 0x09,
    PLAYBACK_REMOTE_MSG_RESUM_SYNC                      = 0x0A,
    PLAYBACK_REMOTE_MSG_PLAYLIST_SYNC                   = 0x0B,
    PLAYBACK_REMOTE_MSG_TRACK_STOP_SYNC                 = 0x0C,
    PLAYBACK_REMOTE_MSG_SEC_TRACK_STOPPED_SYNC          = 0x0D,

    PLAYBACK_REMOTE_MSG_MAX
} T_PLAYBACK_REMOTE_MSG;

/**  @brief app playback stop audio_track_stop-> audio_track_start need some time*/
typedef enum
{
    PLAYBACK_STOPPED_IDLE_ACTION                        = 0x00,
    PLAYBACK_STOPPED_SWITCH_BWD_ACTION                  = 0x01,
    PLAYBACK_STOPPED_SWITCH_FWD_ACTION                  = 0x02,
    PLAYBACK_STOPPED_FILE_END_TO_NEXT_ACTION            = 0x03,

} T_PLAYBACK_STOPPED_NEXT_ACTION;

typedef enum
{
    SINGLE_FILE = 0x00,
    NEXT_FILE,
    PREVERSE_FILE,
    RANDOM_FILE,
    RETURN_IDX,
} T_PLAYBACK_FILE;
typedef enum
{
    PLAYBACK_STOP = 0x00,
    PLAYBACK_START,
} T_PLAYBACK_PAUSE;


/**  @brief app sd card power down check bit map*/
#define APP_SD_POWER_DOWN_ENTER_CHECK_IDLE          0x0000
#define APP_SD_POWER_DOWN_ENTER_CHECK_PLAYBACK      0x0001
#define APP_SD_POWER_DOWN_ENTER_CHECK_TRANS_FILE    0x0002
#define APP_SD_POWER_DOWN_ENTER_CHECK_USB_MSC       0x0004
#define APP_SD_POWER_DOWN_ENTER_CHECK_LISTEN_SAVE   0x0008

/**  @brief app sd card power down check bit map*/
#define APP_SD_DLPS_ENTER_CHECK_IDLE          0x0000
#define APP_SD_DLPS_ENTER_CHECK_PLAYBACK      0x0001
#define APP_SD_DLPS_ENTER_CHECK_TRANS_FILE    0x0002

/**
  * @brief Initialize.
  * @param   No parameter.
  * @return  execution status
  */
uint8_t app_playback_mode_init(void);
void app_playback_volume_up(void);
void app_playback_volume_down(void);
void app_playback_volume_set(void);
uint8_t app_playback_mode_deinit(void);

void app_playback_get_audio_track_state_handle(void);
void app_playback_track_state_changed_handle(void);
void app_playback_buffer_low_handle(void);
void app_playback_buffer_high_handle(void);
void app_playback_solo_to_couple_sync_handle(T_PLAYBACK_PAUSE state);
//void app_playback_stop_entery_hfp(void);
//void app_playback_resume_quit_hfp(uint8_t play_state);
void app_playback_resume_timer_start(uint8_t play_state_old);
void app_playback_solo_to_couple_sync_timer_start(void);
void app_playback_put_data_start_timer(uint16_t time_ms);
void app_playback_put_data_stop_timer(uint8_t pkt_num);

void app_playback_stopped_start_next_action(uint8_t next_action);
void app_playback_mmi_handle_action(uint8_t action);
void app_playback_init(void);
void app_playback_power_off_handle(void);

void app_sd_card_power_down_clear(void);
void app_sd_card_power_down_enable(uint16_t bit);
void app_sd_card_power_down_disable(uint16_t bit);
bool app_sd_card_power_down_check_idle(void);
void app_sd_card_power_on(void);
void app_sd_card_power_down(void);

void app_sd_card_dlps_clear(void);
void app_sd_card_dlps_enable(uint16_t bit);
void app_sd_card_dlps_disable(uint16_t bit);
bool app_sd_card_dlps_check_idle(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_PLAYBACK_H_ */

///@endcond

/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/

