#ifndef _APP_LEA_REALCAST_H_
#define _APP_LEA_REALCAST_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <string.h>

#define REALCAST_BS_NAME_LEN 40
#define REALCAST_BS_CODE_LEN 16
#define REALCAST_DEVICE_LEN  6
#define REALCAST_VENDOR_LEN  2

#define ADV_FILTER_NAME      1
#define ADV_FILTER_REALCAST  2

typedef struct
{
    uint8_t bs_name[REALCAST_BS_NAME_LEN];
    uint8_t bs_code[REALCAST_BS_CODE_LEN];
    uint8_t bd_addr[REALCAST_DEVICE_LEN];
    uint8_t app_idx;
    uint8_t cmd_path;
    uint16_t cmd_id;
    uint8_t subgroup;
} T_REALCAST_DEVICE_INFO;

typedef enum
{
    REALCAST_ENCRYPTION_NO          = 0x00,
    REALCAST_ENCRYPTION_YES         = 0x01,
    REALCAST_BIG_INFO_GET           = 0x02,
} T_REALCAST_ENCRYPTION;

typedef enum
{
    REALCAST_REMOTE_DEVICE_SYNC     = 0x01,
    REALCAST_REMOTE_PA_SYNC         = 0x02,
    REALCAST_REMOTE_MSG_TOTAL
} T_REALCAST_REMOTE_MSG;

typedef struct
{
    uint8_t report_data_len;
    uint8_t *p_report_data;
    uint8_t filter;
} T_REALCAST_SCAN_ADV_INFO;

typedef enum t_realcast_cmd_status
{
    CMD_STATUS_SUCCESS      = 0x00,
    CMD_STATUS_NO_RESOURCE  = 0x01,
} T_REALCAST_CMD_STATUS;

typedef enum t_realcast_ltv_type
{
    BROADCAST_NAME  = 0x00,
    BROADCAST_CODE  = 0x01,
    REALCAST_DEVICE = 0x02,
} T_REALCAST_LTV_TYPE;

typedef enum t_realcast_sync_state
{
    SYNC_STATE_IDLE      = 0x00,
    SYNC_STATE_SEARCHING = 0x01,
    SYNC_STATE_SYNCING   = 0x02,
    SYNC_STATE_SYNCED    = 0x03,
} T_REALCAST_SYNC_STATE;

typedef enum t_realcast_ea_filter
{
    EA_FILTER_NO              = 0x00,
    EA_FILTER_REALCAST_DEVICE = 0x01,
} T_REALCAST_EA_FILTER;

typedef enum t_realcast_scan_state
{
    SCAN_STATE_IDLE        = 0x00,
    SCAN_STATE_SEARCHING   = 0x01,
    SCAN_STATE_CMPLT       = 0x02,
} T_REALCAST_SCAN_STATE;

typedef enum t_realcast_packet_type
{
    PKT_TYPE_COMPLETE       = 0x00,
    PKT_TYPE_MORE           = 0x01,
    PKT_TYPE_TRUNCATED      = 0x02,
} T_REALCAST_PACKET_TYPE;

typedef enum t_realcast_pa_mode
{
    PA_MODE_MAINTAIN_SYNCED  = 0x00,
    PA_MODE_LAUNCH_TERMINATE = 0x01,
} T_REALCAST_PA_MODE;

typedef enum t_realcast_adv_addr_type
{
    ADV_ADDR_TYPE_PUBLIC              = 0x00,
    ADV_ADDR_TYPE_RANDOM              = 0x01,
    ADV_ADDR_TYPE_RPA_PUBLIC_IDENTITY = 0x02,
    ADV_ADDR_TYPE_RPA_RANDOM_IDENTITY = 0x03,
} T_REALCAST_ADV_ADDR_TYPE;

typedef enum t_realcast_pa_state
{
    PA_STATE_IDLE        = 0x00,
    PA_STATE_SYNCING     = 0x01,
    PA_STATE_SYNC_CMPLT  = 0x02,
} T_REALCAST_PA_STATE;

typedef enum t_realcast_vol_state
{
    VOL_MUTE        = 0x00,
    VOL_UNMUTE      = 0x01,
    VOL_UP          = 0x02,
    VOL_DOWN        = 0x03,
} T_REALCAST_VOL_STATE;

typedef enum t_realcast_device_state
{
    DEVICE_STREAM_STATE = 0x00,
    VOL_MUTE_STATE      = 0x01,
    VOL_VALUE_STATE     = 0x02,
} T_REALCAST_DEVICE_STATE;

uint8_t app_lea_realcast_get_subgroup(void);
uint16_t app_lea_realcast_get_cmd_id(void);
bool app_lea_realcast_get_data(T_REALCAST_LTV_TYPE type_para, uint8_t **p_para, uint8_t *para_len);
bool app_lea_realcast_relay_device_info(void);
void app_lea_realcast_set_big_encryption(uint8_t state);
void app_lea_realcast_stop_scan(void);
void app_lea_realcast_state_notify(uint16_t cmd_id, uint8_t status, uint8_t state,
                                   uint8_t data_type, uint8_t *p_data, uint16_t data_len);
bool app_lea_realcast_filter_service_data(T_REALCAST_SCAN_ADV_INFO adv_info,
                                          uint8_t *p_service_data, uint16_t *p_data_len);
void app_lea_realcast_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                 uint8_t app_idx, uint8_t *ack_pkt);
void app_lea_realcast_init(void);
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
