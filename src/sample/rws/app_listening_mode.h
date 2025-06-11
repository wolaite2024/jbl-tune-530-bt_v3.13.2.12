#ifndef _APP_LISTENING_MODE_H_
#define _APP_LISTENING_MODE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_LISTENING_MODE param report
  * @brief App LISTENING_MODE param report
  * @{
  */

#define LISTENING_EVENT_NUM                  11

#define APP_LISTENING_EVENT_SCO              0    /*  0x0001  */
#define APP_LISTENING_EVENT_B2B_CONNECT      1    /*  0x0002  */
#define APP_LISTENING_EVENT_AIRPLANE         2    /*  0x0004  */
#define APP_LISTENING_EVENT_BOX              3    /*  0x0008  */
#define APP_LISTENING_EVENT_XIAOAI           4    /*  0x0010  */
#define APP_LISTENING_EVENT_OUT_EAR          5    /*  0x0020  */
#define APP_LISTENING_EVENT_B2S_CONNECT      6    /*  0x0040  */
#define APP_LISTENING_EVENT_GAMING_MODE      7    /*  0x0080  */
#define APP_LISTENING_EVENT_A2DP             8    /*  0x0100  */
#define APP_LISTENING_EVENT_DIRECT_APT_ON    9    /*  0x0200  */
#define APP_LISTENING_EVENT_USB_SUSPEND      10   /*  0x0400  */

#define UINT16_SET_LSB_BITS(bits_num)        (0xFFFF >> (16 - bits_num))

typedef struct t_listening_mode_relay_msg
{
    uint8_t anc_one_setting;
    uint8_t anc_both_setting;
    uint8_t anc_apt_state;
    uint8_t apt_eq_idx;
    uint8_t apt_support_type;
    bool is_need_tone;
} T_LISTENING_MODE_RELAY_MSG;

typedef enum
{
    APP_REMOTE_MSG_LISTENING_ENGAGE_SYNC            = 0x00,
    APP_REMOTE_MSG_LISTENING_SOURCE_CONTROL         = 0x01,
    APP_REMOTE_MSG_LISTENING_MODE_CYCLE_SYNC        = 0x02,
    APP_REMOTE_MSG_SECONDARY_LISTENING_STATUS       = 0x03,
    APP_REMOTE_MSG_SECONDARY_LISTENING_STATUS_GET   = 0x04,

    APP_REMOTE_MSG_LISTENING_MODE_TOTAL             = 0x05,
} T_LISTENING_MODE_REMOTE_MSG;

typedef enum
{
    ANC_OFF_APT_OFF,
    ANC_OFF_NORMAL_APT_ON,
    ANC_ON_SCENARIO_1_APT_OFF,
    ANC_ON_SCENARIO_2_APT_OFF,
    ANC_ON_SCENARIO_3_APT_OFF,
    ANC_ON_SCENARIO_4_APT_OFF,
    ANC_ON_SCENARIO_5_APT_OFF,
    ANC_OFF_LLAPT_ON_SCENARIO_1,
    ANC_OFF_LLAPT_ON_SCENARIO_2,
    ANC_OFF_LLAPT_ON_SCENARIO_3,
    ANC_OFF_LLAPT_ON_SCENARIO_4,
    ANC_OFF_LLAPT_ON_SCENARIO_5,
    ANC_ON_SCENARIO_1_APT_ON,
    ANC_ON_SCENARIO_2_APT_ON,
    ANC_ON_SCENARIO_3_APT_ON,
    ANC_ON_SCENARIO_4_APT_ON,
    ANC_ON_SCENARIO_5_APT_ON,
    ANC_APT_STATE_TOTAL,
} T_ANC_APT_STATE;

typedef enum
{
    EVENT_NORMAL_APT_ON,                //0x00
    EVENT_APT_OFF,                      //0x01
    EVENT_ANC_ON_SCENARIO_1,            //0x02
    EVENT_ANC_ON_SCENARIO_2,            //0x03
    EVENT_ANC_ON_SCENARIO_3,            //0x04
    EVENT_ANC_ON_SCENARIO_4,            //0x05
    EVENT_ANC_ON_SCENARIO_5,            //0x06
    EVENT_ANC_OFF,                      //0x07
    EVENT_LISTENING_MODE_CYCLE,         //0x08
    EVENT_ANC_CYCLE,                    //0x09
    EVENT_DELAY_APPLY_LISTENING_MODE,   //0x0A
    EVENT_ALL_OFF,                      //0x0B
    EVENT_APPLY_PENDING_STATE,          //0x0C
    EVENT_APPLY_FINAL_STATE,            //0x0D
    EVENT_APPLY_BLOCKED_STATE,          //0x0E
    EVENT_LLAPT_ON_SCENARIO_1,          //0x0F
    EVENT_LLAPT_ON_SCENARIO_2,          //0x10
    EVENT_LLAPT_ON_SCENARIO_3,          //0x11
    EVENT_LLAPT_ON_SCENARIO_4,          //0x12
    EVENT_LLAPT_ON_SCENARIO_5,          //0x13
    EVENT_LLAPT_CYCLE,                  //0x14
    EVENT_ANC_ON_SCENARIO_1_APT_ON,     //0x15
    EVENT_ANC_ON_SCENARIO_2_APT_ON,     //0x16
    EVENT_ANC_ON_SCENARIO_3_APT_ON,     //0x17
    EVENT_ANC_ON_SCENARIO_4_APT_ON,     //0x18
    EVENT_ANC_ON_SCENARIO_5_APT_ON,     //0x19
    EVENT_ANC_APT_OFF,                  //0x1A
    EVENT_DEFAULT_LISTENING_MODE_CYCLE, //0x1B
} T_ANC_APT_EVENT;

typedef enum
{
    POWER_ON_LISTENING_MODE_FOLLOW_LAST = 0,
    POWER_ON_LISTENING_MODE_APT_ON,
    POWER_ON_LISTENING_MODE_ANC_ON,
    POWER_ON_LISTENING_MODE_ALL_OFF,
    POWER_ON_LISTENING_MODE_ANC_APT_ON,
} T_POWER_ON_LISTENING_MODE;

typedef enum t_listening_state_report_type
{
    LISTENING_STATE_ALL_OFF,
    LISTENING_STATE_NORMAL_APT,
    LISTENING_STATE_ANC,
    LISTENING_STATE_LLAPT,
    LISTENING_STATE_ANC_APT,
} T_LISTENING_STATE_REPORT_TYPE;

typedef enum
{
    APPLY_LISTENING_MODE_SRC_CONNECTED            = 0x00,
    APPLY_LISTENING_MODE_SRC_DISCONNECTED         = 0x01,
    APPLY_LISTENING_MODE_BUD_CONNECTED            = 0x02,
    APPLY_LISTENING_MODE_BUD_DISCONNECTED         = 0x03,
    APPLY_LISTENING_MODE_CALL_NOT_IDLE            = 0x04,
    APPLY_LISTENING_MODE_CALL_IDLE                = 0x05,
    APPLY_LISTENING_MODE_VOICE_TRACE_RELEASE      = 0x06,
    APPLY_LISTENING_MODE_AVRCP_PLAY_STATUS_CHANGE = 0x07,
} T_APPLY_LISTENING_MODE_EVENT;

typedef enum t_listening_special_event
{
    //event start
    LISTENING_MODE_SPECIAL_EVENT_SCO              = 0x00,
    LISTENING_MODE_SPECIAL_EVENT_B2B_CONNECT      = 0x01,
    LISTENING_MODE_SPECIAL_EVENT_AIRPLANE         = 0x02,
    LISTENING_MODE_SPECIAL_EVENT_IN_BOX           = 0x03,
    LISTENING_MODE_SPECIAL_EVENT_XIAOAI           = 0x04,
    LISTENING_MODE_SPECIAL_EVENT_OUT_EAR          = 0x05,
    LISTENING_MODE_SPECIAL_EVENT_B2S_CONNECT      = 0x06,
    LISTENING_MODE_SPECIAL_EVENT_GAMING_MODE      = 0x07,
    LISTENING_MODE_SPECIAL_EVENT_A2DP             = 0x08,
    LISTENING_MODE_SPECIAL_EVENT_DIRECT_APT_ON    = 0x09,
    LISTENING_MODE_SPECIAL_EVENT_USB_SUSPEND      = 0x0A,

    //event end
    LISTENING_MODE_SPECIAL_EVENT_SCO_END          = 0x80,
    LISTENING_MODE_SPECIAL_EVENT_B2B_CONNECT_END  = 0x81,
    LISTENING_MODE_SPECIAL_EVENT_AIRPLANE_END     = 0x82,
    LISTENING_MODE_SPECIAL_EVENT_OUT_BOX          = 0x83,
    LISTENING_MODE_SPECIAL_EVENT_XIAOAI_END       = 0x84,
    LISTENING_MODE_SPECIAL_EVENT_IN_EAR           = 0x85,
    LISTENING_MODE_SPECIAL_EVENT_B2S_CONNECT_END  = 0x86,
    LISTENING_MODE_SPECIAL_EVENT_GAMING_MODE_END  = 0x87,
    LISTENING_MODE_SPECIAL_EVENT_A2DP_END         = 0x88,
    LISTENING_MODE_SPECIAL_EVENT_DIRECT_APT_ON_END    = 0x89,
    LISTENING_MODE_SPECIAL_EVENT_USB_SUSPEND_END  = 0x8A,
} T_LISTENING_SPECIAL_EVENT;

typedef struct
{
    uint16_t event;
    T_ANC_APT_STATE state;
} T_LISTENING_QUEUE_ELEMENT;

typedef struct
{
    T_LISTENING_QUEUE_ELEMENT listening_queue[LISTENING_EVENT_NUM];
    uint8_t totol_element;
} T_LISTENING_QUEUE;

typedef enum
{
    ANC_APT_CMD_POSTPONE_NONE                    = 0x00,
    ANC_APT_CMD_POSTPONE_ANC_WAIT_MEDIA_BUFFER   = 0x01,
    ANC_APT_CMD_POSTPONE_LLAPT_WAIT_MEDIA_BUFFER = 0x02,
    ANC_APT_CMD_POSTPONE_WAIT_USER_MODE_CLOSE    = 0x03,
} T_ANC_APT_CMD_POSTPONE_REASON;

typedef struct
{
    T_ANC_APT_CMD_POSTPONE_REASON reason;
    uint16_t pos_anc_apt_cmd;
    uint8_t *pos_param_ptr;
    uint16_t pos_param_len;
    uint8_t  pos_path;
    uint8_t  pos_app_idx;
} T_ANC_APT_CMD_POSTPONE_DATA;

/** End of APP_LISTENING_MODE
* @}
*/

void app_listening_state_machine(T_ANC_APT_EVENT event, bool is_need_push_tone, bool update_final);
void app_listening_apply_when_power_on(void);
void app_listening_anc_apt_cfg_pointer_change(uint8_t *ptr);
void app_listening_apply_when_power_off(void);
void app_listening_set_disallow_trigger_again(void);
uint8_t app_listening_set_delay_apply_time(T_ANC_APT_STATE new_state, T_ANC_APT_STATE prev_state);
void app_listening_rtk_in_case(void);
void app_listening_rtk_out_case(void);
void app_listening_rtk_in_ear(void);
void app_listening_rtk_out_ear(void);
void app_listening_anc_apt_relay(bool need_tone);
void app_listening_cmd_pre_handle(uint16_t listening_cmd, uint8_t *param_ptr, uint16_t param_len,
                                  uint8_t path, uint8_t app_idx, uint8_t *ack_pkt);
void app_listening_cmd_handle(uint16_t listening_cmd, uint8_t *param_ptr, uint16_t param_len,
                              uint8_t path, uint8_t app_idx);
void app_listening_set_disallow_trigger_again(void);
uint8_t app_listening_set_delay_apply_time(T_ANC_APT_STATE new_state, T_ANC_APT_STATE prev_state);
void app_listening_judge_conn_disc_evnet(T_APPLY_LISTENING_MODE_EVENT event);
void app_listening_judge_sco_event(T_APPLY_LISTENING_MODE_EVENT event);
void app_listening_judge_a2dp_event(T_APPLY_LISTENING_MODE_EVENT event);
void app_listening_mode_init(void);
bool app_listening_b2b_connected(void);
bool app_listening_report_secondary_state_condition(void);
uint8_t app_listening_count_1bits(uint32_t x);
T_ANC_APT_CMD_POSTPONE_REASON app_listening_cmd_postpone_reason_get(void);
void app_listening_cmd_postpone_stash(uint16_t anc_apt_cmd, uint8_t *param_ptr,
                                      uint16_t param_len, uint8_t path, uint8_t app_idx, T_ANC_APT_CMD_POSTPONE_REASON postpone_reason);
void app_listening_cmd_postpone_pop(void);
void app_listening_report_status_change(uint8_t state);
bool app_listening_mode_mmi(uint8_t action);
T_ANC_APT_STATE app_listening_get_temp_state(void);
T_ANC_APT_STATE app_listening_get_blocked_state(void);
void app_listening_special_event_trigger(T_LISTENING_SPECIAL_EVENT enter_special_event);
void app_listening_special_event_bitmap_clear(uint8_t event_idx);
bool app_listening_get_is_special_event_ongoing(void);
bool app_listening_final_state_valid(void);
bool app_listening_is_allow_all_off_condition_check(void);
void app_listening_handle_remote_state_sync(void *msg);
void app_listening_direct_apt_on_off_handling(void);
bool app_listening_is_anc_apt_on_state(T_ANC_APT_STATE state);
bool app_listening_anc_apt_on_related_event(T_ANC_APT_EVENT event);
void app_listening_mode_call_status_update(void);
void app_listening_mode_check_remote_loc(uint8_t remote_loc);
bool app_listening_anc_apt_open_condition_check(void);
void app_listening_assign_specific_state(T_ANC_APT_STATE start_state, T_ANC_APT_STATE des_state,
                                         bool tone, bool update_final);

/** End of APP_LISTENING_MODE
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LISTENING_MODE */
