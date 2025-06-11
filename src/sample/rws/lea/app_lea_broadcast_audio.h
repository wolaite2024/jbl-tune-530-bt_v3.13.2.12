#ifndef _APP_LEA_BROADCAST_AUDIO_H_
#define _APP_LEA_BROADCAST_AUDIO_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */
#include "bt_direct_msg.h"
#include "app_lea_scan.h"

#define APP_LEA_BCA_GET                     0xFF
#define APP_LEA_BCA_CLR                     0x00
#define APP_BIS_BS_FIND_RECORD              0x01
#define APP_BIS_BS_FIND_RESET_RECORD        0xFE
#define APP_BIS_BS_FIND_ALIGN               0x04
#define APP_BIS_BS_FIND_RESET_ALIGN         0xFB
#define APP_BIS_BS_FIND_HIT                 0x10
#define APP_BIS_BS_FIND_RESET_HIT           0xEF
#define APP_BIS_BS_FIND_RECORD_ALIGN        (APP_BIS_BS_FIND_RECORD|APP_BIS_BS_FIND_ALIGN)
#define APP_BIS_BIS_CTRL_ACTIVE             0x01
#define APP_BIS_BIS_CTRL_SD_ACTIVE          0x02

#define APP_BIS_BIS_CTRL_RESET_ACTIVE       0xFE
#define APP_BIS_BIS_CTRL_RESET_SD_ACTIVE    0xFD

//#define APP_BIS_BIS_CTRL_INACTIVE         0x10
//#define APP_BIS_BIS_CTRL_RESET_INACTIVE   0xEF
#define APP_BIS_BIS_MAX_COUNT               3
#define APP_BIS_SEC_NUM                     2

typedef enum
{
    LEA_BCA_TMR_SCAN,
    LEA_BCA_TMR_RESYNC,
    LEA_BCA_TMR_MAX,
} T_LEA_BCA_TMR;

typedef enum
{
    LEA_BCA_BIS_SCAN_TO,
    LEA_BCA_BIS_RESYNC_TO,
    LEA_BCA_TO_MAX,
} T_LEA_BCA_TO;

typedef enum
{
    LEA_BCA_MMI              = 0x01,
    LEA_BCA_BIS_SYNC         = 0x02,
    LEA_BCA_BIS_TERMINATE    = 0x03,
    LEA_BCA_SCAN_TIMEOUT     = 0x04,
} T_LEA_BCA_EVENT;

typedef enum
{
    LEA_BCA_STATE_IDLE             = 0x00,
    LEA_BCA_STATE_BASS_ADD_SOURCE  = 0x01,
    LEA_BCA_STATE_PRE_SCAN         = 0x02,
    LEA_BCA_STATE_SCAN             = 0x03,
    LEA_BCA_STATE_STREAMING        = 0x04,
    LEA_BCA_STATE_WAIT_TERM        = 0x05,
    LEA_BCA_STATE_WAIT_RETRY       = 0x06,
} T_APP_LEA_BCA_STATE;

bool app_lea_bca_poweroff_check(T_LEA_BCA_TO event);
void app_lea_bca_scan_start(uint16_t timeout);
void app_lea_bca_scan_stop(void);
uint8_t app_lea_bca_count_num(void);
bool app_lea_bca_bis_exist(void);
bool app_lea_bca_bs_update_device_info(T_LEA_BRS_INFO *src_info);
bool app_lea_bca_pa_sync(uint8_t dev_idx);
bool app_lea_bca_get_pa_info(uint8_t *sets, uint8_t *active_index, uint8_t *para);
bool app_lea_bca_bs_set_active(uint8_t dev_idx);
void app_lea_bca_bs_handle_pa_info_sync(T_LEA_BRS_INFO *src_info);
bool app_lea_bca_bs_pa_terminate(uint8_t dev_idx);
bool app_lea_bca_bs_big_establish_dev_idex(uint8_t dev_idx);
bool app_lea_bca_big_terminate(uint8_t dev_idx);
bool app_lea_bca_terminate(void);
bool app_lea_bca_sink_pa_terminate(uint8_t source_id);
bool app_lea_bca_reset_active(uint16_t *conn_handle);
bool app_lea_bca_sm(uint8_t event, void *p_data);
void app_lea_bca_tgt_addr(bool get, uint8_t *para);
void app_lea_bca_handle_iso_data(T_BT_DIRECT_CB_DATA *p_data);
void app_lea_bca_used(void);
void *app_lea_bca_get_track_handle(void);
uint8_t app_lea_bca_tgt(uint8_t flag);
void app_lea_bca_state_change(T_APP_LEA_BCA_STATE state);
T_APP_LEA_BCA_STATE app_lea_bca_state(void);
void app_lea_bca_scan_info(T_LEA_BRS_INFO *src_info);
void app_lea_bca_bmr_terminate(void);
void app_lea_bca_init(void);
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
