#ifndef _APP_AUDIO_PASSTHROUGH_BRIGHTNESS_H_
#define _APP_AUDIO_PASSTHROUGH_BRIGHTNESS_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_listening_mode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_AUDIO_PASSTHROUGH_BRIGHTNESS
  * @brief
  * @{
  */

#if F_APP_BRIGHTNESS_SUPPORT

#define LLAPT_MAIN_BRIGHTNESS_LEVEL_MAX  brightness_level_max
#define LLAPT_SUB_BRIGHTNESS_LEVEL_MAX   400
#define LLAPT_SUB_BRIGHTNESS_LEVEL_MIN   0

#define RWS_SYNC_LLAPT_BRIGHTESS         (app_cfg_nv.rws_disallow_sync_llapt_brightness ^ 1)

#define L_CH_BRIGHTNESS_MAIN_LEVEL      (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? app_cfg_nv.main_brightness_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_main_brightness_level :\
    INVALID_LEVEL_VALUE

#define L_CH_BRIGHTNESS_SUB_LEVEL      (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? app_cfg_nv.sub_brightness_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_sub_brightness_level :\
    INVALID_LEVEL_VALUE

#define R_CH_BRIGHTNESS_MAIN_LEVEL      (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT) ? app_cfg_nv.main_brightness_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_main_brightness_level :\
    INVALID_LEVEL_VALUE

#define R_CH_BRIGHTNESS_SUB_LEVEL      (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT) ? app_cfg_nv.sub_brightness_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_sub_brightness_level :\
    INVALID_LEVEL_VALUE

extern bool    dsp_config_support_brightness;
extern uint8_t brightness_level_max;
extern uint8_t brightness_level_min;
extern uint8_t brightness_level_default;
extern uint16_t brightness_gain_table[16];

typedef struct t_apt_birghtness_relay_msg
{
    uint8_t report_phone;
    uint8_t first_sync;
    uint8_t brightness_type;
    uint8_t main_brightness_level;
    uint16_t sub_brightness_level;
    uint8_t rws_disallow_sync_llapt_brightness;
} T_APT_BRIGHTNESS_RELAY_MSG;

typedef enum
{
    APP_REMOTE_MSG_APT_BRIGHTNESS_SYNC          = 0x00,
    APP_REMOTE_MSG_RELAY_APT_BRIGHTNESS_CMD     = 0x01,
    APP_REMOTE_MSG_RELAY_APT_BRIGHTNESS_EVENT   = 0x02,

    APP_REMOTE_MSG_APT_BRIGHTNESS_TOTAL         = 0x03,
} T_APT_BRIGHTNESS_REMOTE_MSG;

void app_apt_brightness_cmd_pre_handle(uint16_t brightness_cmd, uint8_t *param_ptr,
                                       uint16_t param_len, uint8_t path, uint8_t app_idx, uint8_t *ack_pkt);
void app_apt_brightness_cmd_handle(uint16_t brightness_cmd, uint8_t *param_ptr, uint16_t param_len,
                                   uint8_t path, uint8_t app_idx);
bool app_apt_brightness_relay(bool first_time_sync, bool primary_report_phone);
void app_apt_brightness_relay_report_handle(void);
void app_apt_brightness_sync_handle(T_APT_BRIGHTNESS_RELAY_MSG *rev_msg);
void app_apt_brightness_set_remote_level(uint8_t main_level, uint16_t sub_level);
void app_apt_brightness_update_sub_level(void);
void app_apt_brightness_main_level_set(uint8_t level);
void app_apt_brightness_sub_level_set(uint16_t level);
bool app_apt_brightness_llapt_scenario_support(T_ANC_APT_STATE state);
uint32_t app_apt_brightness_get_support_bitmap(void);
void app_apt_brightness_init(void);
void app_apt_brightness_report(uint16_t brightness_report_event, uint8_t *event_data,
                               uint16_t event_len);

#endif
/** End of APP_AUDIO_PASSTHROUGH_BRIGHTNESS
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AUDIO_PASSTHROUGH_BRIGHTNESS_H_ */
