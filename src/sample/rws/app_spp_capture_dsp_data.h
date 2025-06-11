#ifndef _APP_SPP_CAPTURE_DSP_DATA_H_
#define _APP_SPP_CAPTURE_DSP_DATA_H_

#if (F_APP_SPP_CAPTURE_DSP_DATA_1 == 1)
#define SPP_CAPTURE_DATA_START_MASK                 0x01
#define SPP_CAPTURE_DATA_SWAP_TO_MASTER             0x02
#define SPP_CAPTURE_DATA_ENTER_SCO_MODE_MASK        0x04
#define SPP_CAPTURE_DATA_CHANGE_MODE_TO_SCO_MASK    0x08
#define SPP_CAPTURE_RAW_DATA_EXECUTING              0x10
#define SPP_CAPTURE_DATA_LOG_EXECUTING              0x20

uint8_t app_spp_capture_data_state(void);
bool app_spp_capture_executing_check(void);
bool app_spp_capture_audio_dsp_ctrl_send_handler(uint8_t *cmd_ptr, uint16_t cmd_len,
                                                 uint8_t cmd_path, uint8_t app_idx, uint8_t *ack_pkt, bool send_flag);
void app_spp_capture_init(void);

#endif
#endif
