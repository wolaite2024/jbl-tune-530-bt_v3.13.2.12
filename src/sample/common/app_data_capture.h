#ifndef _APP_DATA_CAPTURE_H_
#define _APP_DATA_CAPTURE_H_

#if F_APP_SPP_CAPTURE_DSP_DATA_2
#define DATA_CAPTURE_DATA_START_MASK                 0x0001
#define DATA_CAPTURE_DATA_SWAP_TO_MASTER             0x0002
#define DATA_CAPTURE_DATA_START_SCO_MODE             0x0004
#define DATA_CAPTURE_DATA_CHANGE_TO_SCO_MODE         0x0008
#define DATA_CAPTURE_RAW_DATA_EXECUTING              0x0010
#define DATA_CAPTURE_DATA_LOG_EXECUTING              0x0020
#define DATA_CAPTURE_DATA_SAIYAN_EXECUTING           0x0040
#define DATA_CAPTURE_DATA_DSP2_MODE                  0x0080
#define DATA_CAPTURE_DATA_ENTER_START                0x0100

typedef struct
{
    uint8_t     bandwith;
    uint8_t     tpoll;
    uint16_t    flush_tout;
    uint8_t     type_num;
    uint8_t     capture_enable;
} T_CAPTURE_HEADER;

typedef struct
{
    uint8_t le_op_code;
    uint8_t saiyan_enable;
    uint16_t rsv;
} T_SAIYAN_MODE;

extern T_SAIYAN_MODE data_capture_saiyan;

uint16_t app_data_capture_get_state(void);

void app_data_capture_init(void);

void app_data_capture_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                 uint8_t app_idx,
                                 uint8_t *ack_pkt);
void app_data_capture_start_process(T_CAPTURE_HEADER *param, uint8_t cmd_path, uint8_t app_idx);
void app_data_capture_stop_process(uint8_t cmd_path, uint8_t app_idx);
bool app_data_capture_executing_check(void);
void app_data_capture_send_gain(void);
void app_data_capture_mode_ctl(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt);

#if F_APP_SAIYAN_MODE
void app_data_capture_saiyan_mode_ctl(uint8_t start, uint8_t op_code);
void app_data_capture_gain_ctl(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt);
#endif

#endif

#endif
