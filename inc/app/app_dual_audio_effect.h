/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DUAL_AUDIO_EFFECT_H_
#define _APP_DUAL_AUDIO_EFFECT_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_AUDIO_EFFECT App auido effect
  * @brief App auido effect
  * @{
  */

/*dual audio effect:
**NO_EFFECT , MUSIC_MODE, GAMING_MODE, MOVIE_MODE, PARTY_MODE
*/

#define SITRONIX_SECP_INDEX            0x06
#define SPP_UPADTE_DUAL_AUDIO_EFFECT   0x0D
#define SPP_UPDATE_SECP_DATA           0x0F
#define SITRONIX_SECP_NUM              0x10
#define SITRONIX_SECP_SET_APP_VALUE    0x11
#define SITRONIX_SECP_GET_APP_VALUE    0x12
#define SITRONIX_SECP_SPP_RESTART      0x13
#define SITRONIX_SECP_SPP_RESTART_ACK  0x14
#define SITRONIX_SECP_SPP_GET_INFO     0x15

#define MALLEUS_MUSIC                  1
#define MALLEUS_GAMING                 2
#define MALLEUS_MOVIE                  4
#define MALLEUS_PARTY                  8

#define MALLEUS_FULL_CYCLE             (MALLEUS_MUSIC | MALLEUS_GAMING | MALLEUS_MOVIE | MALLEUS_PARTY)

#define dual_audio_mode_sel \
    app_db.gaming_mode ? app_cfg_nv.audio_effect_gaming_type : app_cfg_nv.audio_effect_normal_type

/*single mode effect  is auto selected if rws disconn */
typedef enum
{
    VENDOR_MUSIC_MODE    = 0,
    VENDOR_GAMING_MODE   = 1,
    VENDOR_MOVIE_MODE    = 2,
    VENDOR_PARTY_MODE    = 3,
    VENDOR_SINGLE_MODE   = 4,
} DUAL_EFFECT_TYPE;

typedef struct t_dual_audio_pos
{
    int16_t horz;
    int16_t vert;
    uint16_t factor;
} T_DUAL_AUDIO_POS;

typedef enum
{
    DUAL_EFFECT_CUR_NON   = 0,
    DUAL_EFFECT_CUR_AUDIO = 1,
} T_DUAL_EFFECT_CUR;

typedef enum
{
    DUAL_AUDIO_MSG_NO                     = 0x0000,
    DUAL_AUDIO_MSG_SYNC_PLAY              = 0x0001,
    DUAL_AUDIO_MSG_SEC_REQ                = 0x0002,
    DUAL_AUDIO_MSG_PRI_SYNC               = 0x0003,
    DUAL_AUDIO_MSG_SYNC_SECP_INIT         = 0x0004,
    DUAL_AUDIO_MSG_SYNC_SPP_DATA          = 0x0005,
    DUAL_AUDIO_MSG_SYNC_44_OFFSET         = 0x0006,
    DUAL_AUDIO_MSG_SYNC_DOWNSTREAM        = 0x0007,
    DUAL_AUDIO_MSG_SIGNAL_CTS             = 0x0008,
    DUAL_AUDIO_MSG_SYNC_APP_KEY_VAL       = 0x0009,
    DUAL_AUDIO_MSG_SYNC_SPP_RESTART       = 0x000C,
    DUAL_AUDIO_MSG_RELAY_POS_DATA         = 0x000D,
    DUAL_AUDIO_MSG_SYNC_SPP_RELOAD        = 0x000E,
    DUAL_AUDIO_MSG_SYNC_ALL_APP_KEY_VAL   = 0x000F,
    DUAL_AUDIO_MSG_MAX
} T_DUAL_AUDIO_REMOTE_MSG;


typedef enum
{
    MODE_INFO_L_R_DIV2        = 0,
    MODE_INFO_L               = 1,
    MODE_INFO_R               = 2,
    MODE_INFO_LINE_IN         = 3,
} T_DUAL_EFFECT_MODE_INFO;
/*============================================================================*
 *                              Variables
 *============================================================================*/

/**
 * @brief  audio effect mode reset.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_effect_reset(void);

/**
 * @brief  sync audio effect mode.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_sync_info(void);

/**
 * @brief  get a2dp ignore flag.
 *
 * @param  void
 * @return true if set ignore, false if not ignore.
*/
bool app_dual_audio_get_a2dp_ignore(void);

/**
 * @brief  update secp table data.
 *
 * @param  buf buffer.
 * @return void
*/
void app_dual_audio_spp_update_data(uint8_t *buf);

/**
 * @brief  secp offset init.
 *
 * @param  offset_44K offset.
 * @param  app_idx index of AG.
 * @return void
*/
void app_dual_audio_effect_spp_init(uint16_t offset_44K, uint8_t app_idx);

/**
 * @brief  config secp left right channel.
 *
 * @param  offset_44K offset.
 * @param  parameter left right audio channel.
 * @param  action ture is execute at once,false is execute after audio handle is connect.
 * @param  para_chanel_out speak channel.
 * @return void
*/
void app_dual_audio_lr_info(T_DUAL_EFFECT_MODE_INFO parameter, bool action,
                            uint8_t para_chanel_out);

/**
 * @brief  change to single effect.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_single_effect(void);

/**
 * @brief  sync key value.
 *
 * @param  data pointer of data.
 * @param  to_dsp ture is send to dsp.
 * @return void
*/
void app_dual_audio_sync_app_key_val(uint8_t *data, bool to_dsp);

/**
 * @brief  send app key to dsp.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_app_key_to_dsp(void);

/**
 * @brief  sync app key to dsp.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_sync_all_app_key_val(void);

/**
 * @brief  get audio effect num.
 *
 * @param  void
 * @return number of audio effect.
*/
uint8_t app_dual_audio_get_effect_num(void);

/**
 * @brief  set key val.
 *
 * @param  index  index to set.
 * @param  val    key value.
 * @return void
*/
void app_dual_audio_set_app_key_val(uint8_t index, uint16_t val);

/**
 * @brief  save key val in ftl.
 *
 * @param  void
 * @return save resutl,0 status successful,otherwise fail.
*/
uint32_t app_dual_audio_save_app_key_val(void);

/**
 * @brief  config report path and index.
 *
 * @param  app_idx index.
 * @param  path    report path,@ref T_CMD_PATH.
 * @return save resutl,0 status successful,otherwise fail.
*/
void app_dual_audio_set_report_para(uint8_t app_idx, uint8_t path);

/**
 * @brief  report audio effect.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_report_effect(void);

/**
 * @brief  report dual audio key.
 *
 * @param  idx index.
 * @return void
*/
void app_dual_audio_report_app_key(uint16_t idx);

/**
 * @brief  restart audio effect ack.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_effect_restart_ack(void);

/**
 * @brief  update pos for spatial audio.
 *
 * @param  pos  position.
 * @param  real true update pos.
 * @return void
*/
void app_dual_audio_update_pos(T_DUAL_AUDIO_POS pos, bool real);

/**
 * @brief  relay position.
 *
 * @param  dat  position
 * @return void
*/
void app_dual_audio_relay_pos_data(T_DUAL_AUDIO_POS data);

/**
 * @brief  set get_data flag.
 *
 * @param  flag  get_data flag.
 * @return void
*/
void app_dual_audio_set_get_flag(bool flag);

/**
 * @brief  pos remap for spatial audio.
 *
 * @param  accs  gaccs.
 * @param  gyros gyros.
 * @return void
*/
void app_dual_audio_data_map(int16_t accs[3], int16_t gyros[3]);

/**
 * @brief  report audio effect version.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_effect_report_version_info(void);

/**
 * @brief  get cycle num.
 *
 * @param  void
 * @return number of cycle
*/
uint8_t app_dual_audio_get_cycle_num(void);

/**
 * @brief  switch to next audio effect base on cycle.
 *
 * @param  void
 * @return void
*/
void app_dual_audio_effect_switch(void);

/**
 * @brief  force switch to the specify effect mode.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @param  mode  effect mode.
 * @return void
*/
void app_dual_audio_effect_set_force(uint8_t mode);

/**
 * @brief  audio effect set.
 *
 * @param  mode  @ref DUAL_EFFECT_TYPE
 * @return void
*/
void app_dual_audio_effect_set(uint8_t mode);

/**
 * @brief  audio effect init.
 *
 * @param  normal_cycle normal mode cycle.
 * @param  gaming_cycle gaming mode cycle.
 * @return void
*/
void app_dual_audio_effect_init(uint8_t normal_cycle, uint8_t gaming_cycle);

/** End of APP_AUDIO_EFFECT
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
