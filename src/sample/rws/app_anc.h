#ifndef _APP_ANC_H_
#define _APP_ANC_H_

#include <stdint.h>
#include <stdbool.h>
#include "anc_tuning.h"
#include "anc.h"
#include "gap.h"
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/** @defgroup APP_ANC tool ANC param report
  * @brief App ANC param report
  * @{
  */

#if F_APP_ANC_SUPPORT

#define ANC_EVENT_TO_STATE(event)    (T_ANC_APT_STATE)(ANC_ON_SCENARIO_1_APT_OFF + event - EVENT_ANC_ON_SCENARIO_1)
#define ANC_STATE_TO_EVENT(state)    (T_ANC_APT_EVENT)(EVENT_ANC_ON_SCENARIO_1   + state - ANC_ON_SCENARIO_1_APT_OFF)

#if F_APP_SUPPORT_ANC_APT_COEXIST
#define ANC_APT_EVENT_TO_STATE(event)    (T_ANC_APT_STATE)(ANC_ON_SCENARIO_1_APT_ON + event - EVENT_ANC_ON_SCENARIO_1_APT_ON)
#define ANC_APT_STATE_TO_EVENT(state)    (T_ANC_APT_EVENT)(EVENT_ANC_ON_SCENARIO_1_APT_ON + state - ANC_ON_SCENARIO_1_APT_ON)
#define ANC_TO_ANC_APT_STATE(anc_state)  (T_ANC_APT_STATE)(ANC_ON_SCENARIO_1_APT_ON + anc_state - ANC_ON_SCENARIO_1_APT_OFF)
#define ANC_APT_TO_ANC_STATE(anc_apt_state)  (T_ANC_APT_STATE)(ANC_ON_SCENARIO_1_APT_OFF + anc_apt_state - ANC_ON_SCENARIO_1_APT_ON)
#endif

#define FLASH_SECTION_SIZE    (1024 * 4)
#define ANC_SCENARIO_MAX_NUM    5
#define REPORT_VALUE_MAX_LENGTH 64

typedef enum t_anc_state
{
    ANC_STOPPED,
    ANC_STARTING,
    ANC_STARTED,
    ANC_STOPPING,
} T_ANC_SM_STATE;

typedef enum
{
    ANC_SWITCH_SCENARIO_SUCCESS,
    ANC_SWITCH_SCENARIO_NO_DEFINE,
    ANC_SWITCH_SCENARIO_MAX,
} T_ANC_SWITCH_SCENARIO;

typedef enum
{
    ANC_TEST_MODE_EXIT,
    ANC_TEST_MODE_ENTER_ANC,
    ANC_TEST_MODE_ENTER_LLAPT,
} T_ANC_TEST_MODE;

typedef enum
{
    MP_TYPE_ONE_DEVICE   = 1,      /* the selection type from anc mp tool (stereo/single RWS) */
    MP_TYPE_TWO_DEVICE   = 2,      /* the selection type from anc mp tool (RWS) */
} T_ANC_MP_TYPE;

typedef enum
{
    MP_LEN_READ         = 0x01,     /* param len of read gain(stereo/RWS not engaged) = 0x01 */
    MP_LEN_ANC_BURN_WRITE   = 0x14,     /* param len of burn/write gain(stereo/RWS not engaged) = 0x14 */
    MP_LEN_LLAPT_BURN_WRITE  = 0x1c,
    MP_LEN_SUBOPCODE    = 0x03,
} T_ANC_MP_PARAM_LEN;

typedef enum t_anc_gain_src_sel
{
    ANC_GAIN_SRC_EXT_MIC = 0,
    ANC_GAIN_SRC_INT_MIC = 2,
    // ANC_GAIN_SRC_SPK = 4,
} T_ANC_GAIN_SRC_SEL;

typedef enum
{
    APP_REMOTE_MSG_EXIT_TEST_MODE                       = 0x00,
    APP_REMOTE_MSG_ANC_TOOL_MEASURE_MODE                = 0x01,
    APP_REMOTE_MSG_ANC_LLAPT_FEATURE_CONTROL            = 0x02,
    APP_REMOTE_MSG_ANC_LLAPT_FEATURE_CONTROL_FINISH     = 0x03,
    APP_REMOTE_MSG_RELAY_ANC_CMD                        = 0x04,
    APP_REMOTE_MSG_RELAY_ANC_EVENT                      = 0x05,
    APP_REMOTE_MSG_EXIT_ANC_CHOOSE_MODE                 = 0x06,

    APP_REMOTE_MSG_ANC_TOTAL,
} T_ANC_REMOTE_MSG;

typedef enum t_anc_llapt_feature_ctrl
{
    ANC_LLAPT_FEATURE_WNS_SWITCH = 0x01,
    ANC_LLAPT_FEATURE_MAX,
} T_ANC_LLAPT_FEATURE_CTRL;

typedef enum
{
    APP_ANC_MODE_TYPE_NONE,     //0x00
    APP_ANC_MODE_TYPE_HIGH,     //0x01
    APP_ANC_MODE_TYPE_LOW,      //0x02
    APP_ANC_MODE_TYPE_FAMILY,   //0x03
    APP_ANC_MODE_TYPE_LIBRARY,  //0x04
    APP_ANC_MODE_TYPE_AIRPLANE, //0x05
    APP_ANC_MODE_TYPE_SUBWAY,   //0x06
    APP_ANC_MODE_TYPE_OUTDOOR,  //0x07
    APP_ANC_MODE_TYPE_RUNNING,  //0x08
    APP_ANC_MODE_TYPE_LINE_IN,  //0x09

    APP_ANC_MODE_TYPE_UNKNOWN = 0xFF,
} T_ANC_MODE_TYPE;

typedef enum t_anc_resp_meas_mic_sel
{
    ANC_RESP_MEAS_LEFT_FEED_FORWARD_MIC,
    ANC_RESP_MEAS_VOICE_PRIMARY_MIC,
    ANC_RESP_MEAS_VOICE_SECONDARY_MIC,
    ANC_RESP_MEAS_APT_PRIMARY_MIC,
    ANC_RESP_MEAS_APT_SECONDARY_MIC,
} T_ANC_RESP_MEAS_MIC_SEL;

typedef enum
{
    APP_ANC_RAMP_BURN_MODE,
    APP_ANC_RAMP_APPLY_MODE,
} T_ANC_RAMP_MODE;

typedef enum
{
    APP_ANC_RAMP_ENCRYPTED,
    APP_ANC_RAMP_DECRYPTED,
} T_ANC_RAMP_ENCRYPTED;

typedef union t_anc_feature_map
{
    uint32_t d32;
    struct
    {
        uint32_t set_regs: 1;           /* bit[0], 0: bypass ANC register setting script in driver */
        uint32_t enable_fading: 1;      /* bit[1], 0: bypass ANC fade in/out */
        uint32_t user_mode: 1;          /* bit[2], 0: set tool mode to block ANC user mode operation */
        uint32_t mic_path: 2;           /* bit[4:3], 0: none, 1: ANC mic path, 2: APT mic path */
        uint32_t enable_dehowling: 1;   /* bit[5], 0: bypass llapt dehowling */
        uint32_t enable_wns: 1;         /* bit[6], 0: disable WNS limiter */
        uint32_t rsvd: 25;              /* bit[31:7] */
    };
} T_ANC_FEATURE_MAP;

typedef struct
{
    uint32_t ext_l_gain;
    uint32_t ext_r_gain;
    uint32_t int_l_gain;
    uint32_t int_r_gain;
    uint32_t llapt_ext_l_gain;
    uint32_t llapt_ext_r_gain;
    uint32_t grp_index;
} T_ANC_MP_OPERATE_DATA;

typedef struct
{
    uint32_t            cmd_grp_index;     /**< record group index from ANC MP tool*/
    uint8_t             tool_enable;       /**< enable anc from ANC design tool */
    uint8_t
    report_value[REPORT_VALUE_MAX_LENGTH];  /**< record the read gain value of spk1 and spk2 */
    uint16_t            report_event;      /**< anc command event id */
    uint8_t             burn_start;
} T_ANC_MP_TOOL_DATA;

typedef struct
{
    uint8_t                 ramp_burn_start;
    uint8_t                 *buffer_ptr;
    uint16_t                buffer_size;
    uint16_t                data_receive_length;
    uint8_t                 data_segement_count;
    uint8_t                 parameter_type;
    uint8_t                 parameter_group_index;
    T_ANC_RAMP_MODE         mode;
    T_ANC_RAMP_ENCRYPTED    encrypted;
} T_ANC_RAMP_TOOL_DATA;

typedef struct
{
    uint16_t        anc_activated_list;    //indicate activated/inactivated information
    bool            enter_anc_select_mode;
} T_ANC_GROUP_DATA;

extern T_ANC_MP_TOOL_DATA anc_mp_tool_data;
//extern T_ANC_CMD_POSTPONE_DATA anc_cmd_postpone_data;

bool app_anc_set_first_anc_sceanrio(T_ANC_APT_STATE *state);
void app_anc_cmd_pre_handle(uint16_t anc_cmd, uint8_t *param_ptr, uint16_t param_len, uint8_t path,
                            uint8_t app_idx, uint8_t *ack_pkt);
void app_anc_cmd_handle(uint16_t anc_cmd, uint8_t *param_ptr, uint16_t param_len, uint8_t path,
                        uint8_t app_idx);

bool app_anc_is_anc_on_state(T_ANC_APT_STATE state);
bool app_anc_open_condition_check(void);

void app_anc_handle_vendor_cmd(uint8_t *cmd_ptr);
void app_anc_wait_wdg_reset(uint8_t mode);
void app_anc_wdg_reset(void);
uint8_t app_anc_get_measure_mode(void);
T_ANC_SWITCH_SCENARIO app_anc_switch_next_scenario(T_ANC_APT_STATE anc_current_scenario,
                                                   T_ANC_APT_STATE *anc_next_scenario);
void app_anc_init(void);
uint8_t app_anc_get_coeff_idx(uint8_t scenario_id);
uint8_t app_anc_scenario_mode_get(uint8_t *mode);
uint8_t app_anc_tool_burn_llapt_gain_mismatch(uint32_t ext_data);
void app_anc_enter_test_mode(uint8_t mode);
void app_anc_exit_test_mode(uint8_t mode);
uint8_t app_anc_get_selected_scenario_cnt(void);
uint8_t app_anc_get_activated_scenario_cnt(void);
bool app_anc_related_event(T_ANC_APT_EVENT event);
void app_anc_enter_measure_mode(uint8_t resp_meas_mode);
uint8_t app_anc_ramp_tool_is_busy(void);
uint8_t app_anc_tool_burn_gain_is_busy(void);
bool app_anc_tool_burn_gain_prepare(void);
void app_anc_tool_burn_gain_finish(void);
uint8_t app_anc_scenario_select_is_busy(void);
void app_anc_exit_scenario_select_mode(void);
T_ANC_SM_STATE app_anc_state_get(void);
bool app_anc_is_busy(void);
bool app_anc_enable(uint8_t scenario_id);
bool app_anc_disable(void);

#endif
/** End of APP_ANC
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_ANC_H_ */
