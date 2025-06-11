/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_EQ_H_
#define _APP_EQ_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"
#include "eq.h"
#include "eq_utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \defgroup APP_EQ App EQ
 *  \brief App EQ
 *  \{
 */

/*============================================================================*
 *                              Macros
 *============================================================================*/

#define EQ_START_FRAME_HEADER_LEN       0x10
#define EQ_SINGLE_FRAME_HEADER_LEN      0x08
#define EQ_CONTINUE_FRAME_HEADER_LEN    0x08
#define EQ_END_FRAME_HEADER_LEN         0x08
#define EQ_ABORT_FRAME_HEADER_LEN       0x08

#define EQ_INIT_SEQ                     0x00
#define EQ_SDK_HEADER_SIZE              0x04
#define EQ_HEADER_SIZE                  0x04
#define EQ_INFO_HEADER_SIZE             0x04

/*============================================================================*
 *                              Types
 *============================================================================*/

typedef struct t_audio_eq_info
{
    uint8_t eq_idx;
    uint8_t eq_mode;
    uint8_t eq_seq;
    uint8_t sw_eq_type : 4;
    uint8_t adjust_side : 4;

    uint16_t eq_data_offset;
    uint16_t eq_data_len;

    uint8_t *eq_data_buf;
} T_AUDIO_EQ_INFO;

typedef struct t_audio_eq_report_data
{
    T_AUDIO_EQ_INFO *eq_info;

    uint8_t id;
    uint8_t cmd_path;
    uint16_t max_frame_len;
} T_AUDIO_EQ_REPORT_DATA;

typedef struct t_eq_enable_info
{
    T_AUDIO_EFFECT_INSTANCE  instance;
    bool                     is_enable;
    uint8_t                  idx;
} T_EQ_ENABLE_INFO;

typedef enum
{
    EQ_INDEX_REPORT_BY_PLAYING,
    EQ_INDEX_REPORT_BY_CHANGE_MODE,
    EQ_INDEX_REPORT_BY_SWITCH_EQ_INDEX,
    EQ_INDEX_REPORT_BY_GET_EQ_INDEX,
    EQ_INDEX_REPORT_BY_UPDATE_EQ_INDEX,
    EQ_INDEX_REPORT_BY_GET_UNSAVED_EQ,
} T_EQ_DATA_UPDATE_EVENT;

#if F_APP_USER_EQ_SUPPORT

typedef struct t_eq_user_eq_header
{
    uint16_t supported_sample_rate;
    uint8_t spk_user_eq_num;
    uint8_t mic_user_eq_num;
    uint8_t user_eq_version;
    uint8_t rsv[3];
} T_EQ_USER_EQ_HEADER;

typedef struct t_eq_user_eq_info
{
    uint8_t eq_info_len;
    uint8_t eq_type;
    uint8_t eq_idx;
    uint8_t eq_info[54];
    uint8_t rsv[3];
} T_EQ_USER_EQ_INFO;

typedef struct t_eq_user_eq_data
{
    uint8_t eq_data_len;
    uint8_t sample_rate;
    uint8_t eq_data[208];
    uint8_t rsv[2];
} T_EQ_USER_EQ_DATA;

typedef enum
{
    EQ_SYNC_USER_EQ,
    EQ_SYNC_USER_EQ_WHEN_B2B_CONNECTED,
    EQ_SYNC_RESET_EQ,
    EQ_SYNC_GET_SECONDARY_EQ_INFO,
    EQ_SYNC_REPORT_SECONDARY_EQ_INFO,
} T_EQ_SYNC_ACTION;
#endif

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
 * \brief   Eq param get.
 */

uint16_t app_eq_dsp_param_get(T_EQ_TYPE eq_type, T_EQ_STREAM_TYPE stream_type,
                              uint8_t eq_mode, uint8_t index, uint8_t *data);

/**
 * \brief  Init APP EQ module.
 */
void app_eq_init(void);

/**
 * \brief  Deinit APP EQ module.
 */
void app_eq_deinit(void);

/**
 * \brief   Set the specific EQ index.
 *
 * \param[in] eq_type   EQ type
 * \param[in] mode      EQ mode
 * \param[in] index     EQ index
 *
 * \return  The status of setting the EQ index.
 * \retval  true        EQ index was set successfully.
 * \retval  false       EQ index was failed to set.
 */
bool app_eq_index_set(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index);

/**
 * \brief   Set the specific EQ index parameter.
 *
 * \param[in] index     EQ index.
 * \param[in] data      EQ parameter buffer.
 * \param[in] len       EQ parameter length.
 *
 * \return  The status of setting the EQ parameter.
 * \retval  true        EQ parameter was set successfully.
 * \retval  false       EQ parameter was failed to set.
 */
bool app_eq_param_set(uint8_t eq_mode, uint8_t index, void *data, uint16_t len);

/**
 * \brief   Get the specific EQ enable info.
 *
 * \param[in] eq_mode      EQ mode.
 * \param[in] enable_info  EQ enable info.
 */
void app_eq_enable_info_get(uint8_t eq_mode, T_EQ_ENABLE_INFO *enable_info);

/**
 * \brief Report EQ parameter
 *
 * \param[in] cmd_path  Command path is used to distinguish from uart, le, spp or iap channel.
 * \param[in] app_idx   Device index
 */
void app_eq_report_eq_param(uint8_t cmd_path, uint8_t app_idx);

/**
 * \brief Terminate report EQ parameter
 *
 * \param[in] cmd_path  Command path is used to distinguish from uart, le, spp or iap channel.
 * \param[in] app_idx   Device index
 */
void app_eq_report_terminate_param_report(uint8_t cmd_path, uint8_t app_idx);

/**
 * \brief  Get current sample rate.
 *
 * \return Current sample rate.
 */
uint32_t app_eq_sample_rate_get(void);

/**
 * \brief  Play audio EQ tone.
 */
void app_eq_play_audio_eq_tone(void);

/**
 * \brief  Play APT EQ tone.
 */
void app_eq_play_apt_eq_tone(void);


/**
 * \brief  Check idx accord EQ mode
 */
void app_eq_idx_check_accord_mode(void);

/**
 * \brief  Check EQ mode then sync idx
 *
 * \param[in] eq_mode   EQ mode
 * \param[in] index     EQ index
*/
void app_eq_sync_idx_accord_eq_mode(uint8_t eq_mode, uint8_t index);

/**
 * \brief   Create EQ instance.
 *
 * \param[in]  eq_content_type  EQ content type
 * \param[in]  stream_type      EQ stream type
 * \param[in]  eq_type          EQ type
 * \param[in]  eq_mode          EQ mode
 * \param[in]  eq_index         EQ index
 *
 * \return  Created EQ instance.
 */
T_AUDIO_EFFECT_INSTANCE app_eq_create(T_EQ_CONTENT_TYPE eq_content_type,
                                      T_EQ_STREAM_TYPE stream_type, T_EQ_TYPE eq_type, uint8_t eq_mode, uint8_t eq_index);

/**
 * \brief  EQ related command handler.
 *
 * \param[in]  cmd_ptr   cmd
 * \param[in]  cmd_len   cmd len
 * \param[in]  cmd_path  command path is used to distinguish from uart, le, spp or iap channel
 * \param[in]  app_idx   device index
 * \param[out] ack_pkt   ack
 */
void app_eq_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt);

/**
 * \brief   Enable the EQ effect.
 *
 * \param[in]      eq_instance         EQ instance
 * \param[in,out]  audio_eq_enabled    this EQ instance enabled or not
 */
void app_eq_audio_eq_enable(T_AUDIO_EFFECT_INSTANCE *eq_instance, bool *audio_eq_enabled);

/**
 * \brief   Change audio speaker EQ mode.
 *
 * \param[in] report_when_eq_mode_change  is reporting EQ index when audio EQ mode changed
 */
void app_eq_change_audio_eq_mode(bool report_when_eq_mode_change);

/**
 * \brief   Judge audio speaker EQ mode.
 *
 * \return  The new audio speaker EQ mode.
 * \retval  NORMAL_MODE   normal mode
 * \retval  GAMING_MODE   gaming mode
 * \retval  ANC_MODE      ANC mode
 * \retval  LINE_IN_MODE  line-in mode
 */
uint8_t app_eq_judge_audio_eq_mode(void);

/**
 * \brief   Get EQ mode.
 * \param[in] eq_type       EQ type
 * \param[in] stream_type   EQ stream type
 * \return   EQ mode
 */
uint8_t app_eq_mode_get(uint8_t eq_type, uint8_t stream_type);

/**
 * \brief   Enable EQ effect.
 * \param[in] eq_mode   EQ mode
 * \param[in] eq_len    EQ len
 */
void app_eq_enable_effect(uint8_t eq_mode, uint16_t eq_len);

/**
 * \brief   Set EQ effect.
 * \param[in] eq_instance   EQ instance
 * \param[in] eq_buf        EQ data
 * \param[in] eq_len        EQ len
 */
void app_eq_media_eq_set(T_AUDIO_EFFECT_INSTANCE eq_instance, uint8_t *eq_buf,
                         uint16_t eq_len);

/**
 * \brief   Operate EQ cmd sended by tool.
 * \param[in] eq_mode          EQ mode
 * \param[in] eq_adjust_side   the site of the user EQ
 * \param[in] is_play_eq_tone  is play eq tone
 * \param[in] eq_idx           EQ idx
 * \param[in] eq_len_to_dsp    EQ len
 * \param[in] cmd_ptr          EQ data
 *
 * \return  The status of operate EQ cmd.
 * \retval  true        successfully
 * \retval  false       failed
 */
bool app_eq_cmd_operate(uint8_t eq_mode, uint8_t eq_adjust_side, uint8_t is_play_eq_tone,
                        uint8_t eq_idx, uint8_t eq_len_to_dsp, uint8_t *cmd_ptr);

#if F_APP_USER_EQ_SUPPORT
/**
 * \brief   Save specific user EQ to FTL.
 *
 * \param[in] eq_type           EQ type
 * \param[in] mode              EQ mode
 * \param[in] index             EQ index
 * \param[in] sample_rate       the sample rate
 * \param[in] eq_adjust_side    the site of the user EQ
 * \param[in] p_data            EQ data
 * \param[in] eq_data_len       EQ len
 * \param[in] eq_info           EQ info
 * \param[in] eq_info_len       EQ info len
 *
 * \return  The status of save user eq.
 * \retval  true        successfully
 * \retval  false       failed
 */
bool app_eq_save_user_eq_to_ftl(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index, uint8_t sample_rate,
                                uint8_t eq_adjust_side, uint8_t *p_data, uint16_t eq_data_len, uint8_t *eq_info,
                                uint16_t eq_info_len);

/**
 * \brief   Check the specific EQ is valid user EQ or not.
 *
 * \param[in] eq_type   EQ type.
 * \param[in] mode      EQ mode.
 * \param[in] index     EQ index.
 *
 * \return  The EQ is valid user EQ or not.
 * \retval  true        valid user EQ
 * \retval  false       invalid user EQ
 */
bool app_eq_is_valid_user_eq_index(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index);

/**
 * \brief   Reset specific user EQ.
 *
 * \param[in] eq_type   EQ type
 * \param[in] mode      EQ mode
 * \param[in] index     EQ index
 *
 * \return  The status of reset user eq.
 * \retval  true        successfully
 * \retval  false       failed
 */
bool app_eq_reset_user_eq(T_EQ_TYPE eq_type, uint8_t mode, uint8_t index);

/**
 * \brief   Reset all user EQ.
 */
void app_eq_reset_all_user_eq(void);

/**
 * \brief   Reset unsaved user EQ.
 */
void app_eq_reset_unsaved_user_eq(void);

#if F_APP_ERWS_SUPPORT
/**
 * \brief   Sync user EQ when bud connected.
 */
void app_eq_sync_user_eq_when_connected(void);

/**
 * \brief   Continue sync user EQ when bud connected.
 *
 * \param[in] is_first_time  is the first time to calling this function after bud connected
 * \param[in] last_eq_type   last time synchronized EQ type
 * \param[in] last_eq_idx    last time synchronized EQ index
 * \param[in] last_offset    last time synchronized offset of EQ data
 * \param[in] last_len       last time synchronized EQ len
 */
void app_eq_continue_sync_user_eq_when_connected(bool is_first_time, T_EQ_TYPE last_eq_type,
                                                 uint8_t last_eq_idx, uint16_t last_offset, uint16_t last_len);

/**
 * \brief    Process sync user EQ message when bud connected.
 *
 * \param[in] eq_type  EQ type
 * \param[in] eq_idx   EQ index
 * \param[in] offset   The offset of eq_data
 * \param[in] eq_data  EQ data
 * \param[in] eq_len   EQ len
 */
void app_eq_process_sync_user_eq_when_b2b_connected(T_EQ_TYPE eq_type, uint8_t eq_idx,
                                                    uint16_t offset, uint8_t *eq_data, uint16_t eq_len);

#if F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT
/**
 * \brief   Report the EQ of secondary to primary.
 *
 * \param[in] eq_type   EQ type
 * \param[in] eq_mode   EQ mode
 * \param[in] eq_idx    EQ index
 * \param[in] cmd_path  Command path is used to distinguish from uart, le, spp or iap channel
 * \param[in] app_idx   Device index
 */
void app_eq_report_sec_eq_to_pri(uint8_t eq_type, uint8_t eq_mode, uint8_t eq_idx, uint8_t cmd_path,
                                 uint8_t app_idx);

/**
 * \brief   Report the EQ of secondary to source.
 *
 * \param[in] cmd_path   Command path is used to distinguish from uart, le, spp or iap channel
 * \param[in] app_idx    Device index
 * \param[in] eq_len     EQ len
 * \param[in] eq_data    EQ data
 */
void app_eq_report_sec_eq_to_src(uint8_t cmd_path, uint8_t app_idx, uint16_t eq_len,
                                 uint8_t *eq_data);

#endif  /* F_APP_SEPARATE_ADJUST_APT_EQ_SUPPORT */
#endif  /* F_APP_ERWS_SUPPORT */
#endif  /* F_APP_USER_EQ_SUPPORT */

/** End of APP_EQ
* \}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_EQ_H_ */
