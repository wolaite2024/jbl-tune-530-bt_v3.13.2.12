#ifndef _APP_LEA_MGR_H_
#define _APP_LEA_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */
#include "os_queue.h"
#include "bt_direct_msg.h"
#include "syncclk_driver.h"
#include "audio.h"
#include "audio_type.h"
#include "audio_track.h"
#include "codec_def.h"
#include "ble_audio.h"
#include "multitopology_ctrl.h"
#include "app_lea_scan.h"
#include "app_types.h"

#define LEA_SYNC_CLK_REF SYNCCLK_ID4

#define AUDIO_CHANNEL_L  (AUDIO_CHANNEL_LOCATION_FL|AUDIO_CHANNEL_LOCATION_BL|AUDIO_CHANNEL_LOCATION_FLC|AUDIO_CHANNEL_LOCATION_SL \
                          |AUDIO_CHANNEL_LOCATION_TFL|AUDIO_CHANNEL_LOCATION_TBL|AUDIO_CHANNEL_LOCATION_TSL|AUDIO_CHANNEL_LOCATION_BFL \
                          |AUDIO_CHANNEL_LOCATION_FLW|AUDIO_CHANNEL_LOCATION_LS) \

#define AUDIO_CHANNEL_R  (AUDIO_CHANNEL_LOCATION_FR|AUDIO_CHANNEL_LOCATION_BR|AUDIO_CHANNEL_LOCATION_FRC|AUDIO_CHANNEL_LOCATION_SR \
                          |AUDIO_CHANNEL_LOCATION_TFR|AUDIO_CHANNEL_LOCATION_TBR|AUDIO_CHANNEL_LOCATION_TSR|AUDIO_CHANNEL_LOCATION_BFR \
                          |AUDIO_CHANNEL_LOCATION_FRW|AUDIO_CHANNEL_LOCATION_RS) \

//Setting 1 due to DSP doesn't support LC3 multi-frame*/
#define FIXED_FRAME_NUM           1

#define PLAYBACK_RESUME_CNT 400

extern uint8_t lea_bas_id;

typedef enum
{
    LEA_ADV_TIMEOUT_CIS               = 0x00,
    LEA_ADV_TIMEOUT_DELEGATOR         = 0x01,
} T_LEA_ADV_TIMEOUT;

typedef enum
{
    LEA_ADV_MODE_PAIRING              = 0x00,
    LEA_ADV_MODE_LINK_LOSS_LINK_BACK  = 0x01,
} T_LEA_ADV_MODE;

typedef enum
{
    LEA_LINK_IDLE                = 0x00,
    LEA_LINK_CONNECTED           = 0x01,
    LEA_LINK_STREAMING           = 0x02,
} T_LEA_LINK_STATE;

typedef enum
{
    LEA_CONNECT                  = 0x20,
    LEA_DISCONNECT               = 0x21,
    LEA_CODEC_CONFIGURED         = 0x22,
    LEA_PREFER_QOS               = 0x23,
    LEA_ENABLE                   = 0x25,
    LEA_ENABLEING                = 0x26,
    LEA_SETUP_DATA_PATH          = 0x27,
    LEA_REMOVE_DATA_PATH         = 0x28,
    LEA_STREAMING                = 0x29,
    LEA_PAUSE                    = 0x2A,
    LEA_A2DP_START               = 0x2B,
    LEA_A2DP_SUSPEND             = 0x2C,
    LEA_AVRCP_PLAYING            = 0x2D,
    LEA_AVRCP_PAUSE              = 0x2E,
    LEA_MCP_STATE                = 0x2F,
    LEA_HFP_CALL_STATE           = 0x30,
    LEA_CCP_CALL_STATE           = 0x31,
    LEA_CCP_READ_RESULT          = 0x32,
    LEA_CCP_TERM_NOTIFY          = 0x33,
    LEA_SNIFFING_STOP            = 0x34,
    LEA_PAUSE_LOCAL              = 0x35,
    LEA_DISCON_LOCAL             = 0x36,
    LEA_CONTEXTS_CHECK           = 0x37,
    LEA_METADATA_UPDATE          = 0x38,
    LEA_RELEASING                = 0x39,
    LEA_MCP_PAUSE                = 0x3A,
} T_LEA_LINK_EVENT;

typedef enum
{
    LEA_REMOTE_MMI_SWITCH_SYNC        = 0x01,
    LEA_REMOTE_PA_INFO_SYNC           = 0x02,
    LEA_REMOTE_SEC_SYNC_CIS_STREAMING = 0x03,
    LEA_REMOTE_MEDIA_SUSPEND_SYNC     = 0x04,
    LEA_REMOTE_SYNC_SIRK              = 0x05,
    LEA_REMOTE_SYNC_PRI_MIC_STATE     = 0x06,
    LEA_REMOTE_SYNC_LINK_NUM          = 0x07,
    LEA_REMOTE_MSG_TOTAL
} T_LEA_REMOTE_MSG;

typedef enum
{
    LEA_DEV_CTRL_SET_IDLE             = 0x00,
    LEA_DEV_CTRL_DISCOVERABLE         = 0x01,
    LEA_DEV_CTRL_GET_LEGACY           = 0x02,
    LEA_DEV_CTRL_SET_RADIO            = 0x03,
    LEA_DEV_CTRL_GET_CIS_POLICY       = 0x04,
} T_LEA_BUD_DEV_CTRL;

typedef enum
{
    LEA_BIS_POLICY_RANDOM             = 0x00,
    LEA_BIS_POLICY_SPECIFIC           = 0x01,
    LEA_BIS_POLICY_SPECIFIC_AND_LAST  = 0x02,
} T_LEA_BIS_POLICY;

typedef enum
{
    LEA_BROADCAST_NONE                = 0x00,
    LEA_BROADCAST_DELEGATOR           = 0x01,
    LEA_BROADCAST_SINK                = 0x02,
} T_LEA_BROADCAST_ROLE;

typedef enum
{
    LEA_LOC_ACTION_AUDIO_NOTHING = 0x00,
    LEA_LOC_ACTION_AUDIO_PAUSE   = 0x01,
    LEA_LOC_ACTION_AUDIO_RESUME  = 0x02,
} T_LEA_LOC_ACTION;

typedef struct t_lea_link_info
{
    uint8_t bd_addr[6];
    uint8_t link_state;
} T_LEA_LINK_INFO;

typedef struct t_iso_data_ind
{
    struct t_iso_data_ind *p_next;
    uint16_t conn_handle;
    uint8_t pkt_status_flag;
    uint32_t time_stamp;
    uint16_t iso_sdu_len;
    uint16_t pkt_seq_num;
    uint8_t *p_buf;
} T_ISO_DATA_IND;

void app_lea_mgr_init(void);
void app_lea_mgr_set_media_state(uint8_t para);
void app_lea_mgr_mmi_handle(uint8_t action);
void app_lea_mgr_update_sniff_mask(void);
bool app_lea_mgr_dev_ctrl(uint8_t para, uint8_t *data);
bool app_lea_mgr_is_media_streaming(void);
bool app_lea_mgr_is_downstreaming(void);
bool app_lea_mgr_is_upstreaming(void);
void app_lea_mgr_update_current_mcp_state(void *p_link_info, uint8_t mcp_last_state);
void app_lea_mgr_clear_iso_queue(T_OS_QUEUE *p_queue);
void app_lea_mgr_enqueue_iso_data(T_OS_QUEUE *p_queue, T_BT_DIRECT_ISO_DATA_IND *p_iso_data,
                                  uint16_t output_handle);
T_ISO_DATA_IND *app_lea_mgr_find_iso_elem(T_OS_QUEUE *p_queue,
                                          T_BT_DIRECT_ISO_DATA_IND *p_direct_iso);
bool app_lea_mgr_find_call_device_by_priority(void);
bool app_lea_mgr_sync_link_num(void);
bool app_lea_mgr_get_media_suspend_by_out_ear(void);
void app_lea_mgr_update_media_suspend_by_out_ear(bool out_ear);
void app_lea_mgr_disc_inactive_le_link(uint8_t id, uint8_t link_type);
uint8_t app_lea_mgr_lea_bond_num(void);
void app_lea_bas_batt_level_report(uint16_t conn_handle);
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
