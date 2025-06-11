#ifndef _APP_AUDIO_PASSTHROUGH_H_
#define _APP_AUDIO_PASSTHROUGH_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_listening_mode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_AUDIO_PASSTHROUGH
  * @brief
  * @{
  */

#define LLAPT_EVENT_TO_STATE(event)    (T_ANC_APT_STATE)(ANC_OFF_LLAPT_ON_SCENARIO_1 + event - EVENT_LLAPT_ON_SCENARIO_1)
#define LLAPT_STATE_TO_EVENT(state)    (T_ANC_APT_EVENT)(EVENT_LLAPT_ON_SCENARIO_1   + state - ANC_OFF_LLAPT_ON_SCENARIO_1)

#define LLAPT_SCENARIO_MAX_NUM    5

typedef struct t_apt_relay_msg
{
    uint8_t anc_apt_state;
    uint8_t volume_level;
    uint8_t apt_eq_idx;
} T_APT_RELAY_MSG;

//cmd set 1.5+ support
#define APT_SUB_VOLUME_LEVEL_SUPPORT     1

#define APT_MAIN_VOLUME_LEVEL_MAX        app_dsp_cfg_vol.apt_volume_out_max
#define APT_SUB_VOLUME_LEVEL_MAX         100
#define APT_SUB_VOLUME_LEVEL_MIN         0

#define INVALID_LEVEL_VALUE              0xFFFF

#define L_CH_LLAPT_MAIN_VOLUME         (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? app_cfg_nv.apt_volume_out_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_apt_volume_out_level :\
    INVALID_LEVEL_VALUE

#define L_CH_LLAPT_SUB_VOLUME          (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT) ? app_cfg_nv.apt_sub_volume_out_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_apt_sub_volume_out_level :\
    INVALID_LEVEL_VALUE

#define R_CH_LLAPT_MAIN_VOLUME         (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT) ? app_cfg_nv.apt_volume_out_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_apt_volume_out_level :\
    INVALID_LEVEL_VALUE

#define R_CH_LLAPT_SUB_VOLUME          (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT) ? app_cfg_nv.apt_sub_volume_out_level :\
    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_apt_sub_volume_out_level :\
    INVALID_LEVEL_VALUE

#define MAIN_TYPE_LEVEL         0
#define SUB_TYPE_LEVEL          1

#define RWS_SYNC_APT_VOLUME     (app_cfg_nv.rws_disallow_sync_apt_volume ^ 1)

typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t grp_info: 12;           /* bit[11:0] */
        uint32_t rsvd2: 20;              /* bit[31:12], reserved */
    } data;
} T_LLAPT_EXT_DATA;

typedef struct
{
    uint8_t total_scenario_num;
    uint16_t llapt_activated_list;
    uint8_t *mode;
    uint8_t *apt_effect;
} T_LLAPT_SCENARIO_INFO;

typedef struct t_apt_volume_relay_msg
{
    uint8_t report_phone;
    uint8_t first_sync;
    uint8_t volume_type;
    uint8_t main_volume_level;
    uint16_t sub_volume_level;
    uint8_t rws_disallow_sync_apt_volume;
} T_APT_VOLUME_RELAY_MSG;

typedef enum t_apt_state
{
    APT_STOPPED,
    APT_STARTING,
    APT_STARTED,
    APT_STOPPING,
} T_APT_SM_STATE;

typedef enum
{
    LLAPT_SWITCH_SCENARIO_SUCCESS,
    LLAPT_SWITCH_SCENARIO_NO_DEFINE,
    LLAPT_SWITCH_SCENARIO_MAX,
} T_LLAPT_SWITCH_SCENARIO;

typedef enum
{
    APP_REMOTE_MSG_APT_VOLUME_OUT_LEVEL             = 0x00,
    APP_REMOTE_MSG_APT_EQ_INDEX_SYNC                = 0x01,
    APP_REMOTE_MSG_APT_VOLUME_SYNC                  = 0x02,
    APP_REMOTE_MSG_RELAY_APT_CMD                    = 0x03,
    APP_REMOTE_MSG_RELAY_APT_EVENT                  = 0x04,
    APP_REMOTE_MSG_RELAY_APT_NR_STATE               = 0x05,
    APP_REMOTE_MSG_EXIT_LLAPT_CHOOSE_MODE           = 0x06,
    APP_REMOTE_MSG_APT_EQ_DEFAULT_INDEX_SYNC        = 0x07,
    APP_REMOTE_MSG_APT_SUPORT_TYPE                  = 0x08,

    APP_REMOTE_MSG_APT_TOTAL,
} T_APT_REMOTE_MSG;

typedef enum
{
    EQ_INDEX_REPORT_BY_APT_ENABLE,
    EQ_INDEX_REPORT_BY_RWS_CONNECTED,
    EQ_INDEX_REPORT_BY_SWITCH_APT_EQ_INDEX,
    EQ_INDEX_REPORT_BY_GET_APT_EQ_INDEX,
    EQ_INDEX_REPORT_BY_UPDATE_APT_EQ_INDEX,
    EQ_INDEX_REPORT_BY_GET_UNSAVED_APT_EQ,
} T_APT_EQ_DATA_UPDATE_EVENT;

typedef enum
{
    APT_SUPPORT_TYPE_NORMAL_APT = 0,
    APT_SUPPORT_TYPE_LLAPT = 1,
    APT_SUPPORT_TYPE_NONE,
} T_APT_SUPPORT_TYPE;

bool app_apt_set_first_llapt_scenario(T_ANC_APT_STATE *state);
void app_apt_cmd_pre_handle(uint16_t apt_cmd, uint8_t *param_ptr, uint16_t param_len, uint8_t path,
                            uint8_t app_idx, uint8_t *ack_pkt);
void app_apt_cmd_handle(uint16_t apt_cmd, uint8_t *param_ptr, uint16_t param_len, uint8_t path,
                        uint8_t app_idx);
bool app_apt_open_condition_check(void);
bool app_apt_is_llapt_on_state(T_ANC_APT_STATE state);
T_LLAPT_SWITCH_SCENARIO app_apt_llapt_switch_next_scenario(T_ANC_APT_STATE llapt_current_scenario,
                                                           T_ANC_APT_STATE *llapt_next_scenario);
bool app_normal_apt_related_event(T_ANC_APT_EVENT event);
bool app_llapt_related_event(T_ANC_APT_EVENT event);
bool app_apt_is_apt_on_state(T_ANC_APT_STATE state);
bool app_apt_is_normal_apt_on_state(T_ANC_APT_STATE state);
void app_apt_play_apt_volume_tone(void);
void app_apt_init(void);
uint8_t app_apt_llapt_get_coeff_idx(uint8_t scenario_id);
bool app_apt_volume_relay(bool first_time_sync, bool primary_report_phone);
void app_apt_volume_relay_report_handle(void);
void app_apt_volume_sync_handle(T_APT_VOLUME_RELAY_MSG *rev_msg);
void app_apt_volume_set_remote_level(uint8_t main_level, uint16_t sub_level);
void app_apt_volume_update_sub_level(void);
void app_apt_main_volume_set(uint8_t level);
void app_apt_sub_volume_set(uint16_t level);
uint8_t app_apt_get_llapt_selected_scenario_cnt(void);
uint8_t app_apt_get_llapt_activated_scenario_cnt(void);
uint8_t app_apt_get_llapt_total_scenario_cnt(void);
void app_apt_report(uint16_t apt_report_event, uint8_t *event_data, uint16_t event_len);
bool app_apt_llapt_scenario_is_busy(void);
void app_apt_exit_llapt_scenario_choose_mode(void);
void app_apt_state_set(T_APT_SM_STATE state);
bool app_apt_is_busy(void);
bool app_apt_normal_apt_enable(void);
bool app_apt_llapt_enable(uint8_t llapt_scenario_id);
bool app_apt_disable(void);
T_APT_SM_STATE app_apt_state_get(void);
bool app_apt_is_normal_apt_started(void);
bool app_apt_is_llapt_started(void);
T_APT_SUPPORT_TYPE app_apt_get_apt_support_type(void);
void app_apt_setting_state_update(void);

/** End of APP_AUDIO_PASSTHROUGH
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_AUDIO_PASSTHROUGH_H_ */
