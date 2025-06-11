/**
*****************************************************************************************
*     Copyright (C) 2021 Realtek Semiconductor Corporation.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
  * @attention
  ***************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/

#ifndef _APP_DONGLE_CTRL_H_
#define _APP_DONGLE_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

/*============================================================================*
 *                         Macros
 *============================================================================*/

#if F_APP_A2DP_CODEC_LC3_SUPPORT
#define STEREO_GAMING_LC3_CHANNEL   2
#define STEREO_GAMING_1_LC3_SIZE    90 // 48_3 setting
#define STEREO_GAMING_PKT_SIZE      (STEREO_GAMING_LC3_CHANNEL * STEREO_GAMING_1_LC3_SIZE)
#else
#define STEREO_GAMING_SBC_NUM       2
#define STEREO_GAMING_1_SBC_SIZE    87 // = (13 + bitpool * 2), bitpool setting is 37.
#define STEREO_GAMING_PKT_SIZE      (STEREO_GAMING_SBC_NUM * STEREO_GAMING_1_SBC_SIZE)
#endif

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct ctrl_header
{
    uint8_t seq;
    uint8_t is_ack : 1;
    uint8_t chk_seq : 7; /* debug use */
    uint16_t size;
} T_APP_CTRL_HEADER;

/* common control info between headset and dongle */
typedef struct ctrl_queue
{
    struct ctrl_queue *next;
    T_APP_CTRL_HEADER header;
    uint8_t *data;
} T_APP_CTRL_QUEUE;

/*============================================================================*
 *                         Functions
 *============================================================================*/

uint16_t app_dongle_get_append_ctrl_data_len(void);
void app_dongle_append_ctrl_data_to_voice_spp(uint8_t *start);
void app_dongle_dispatch_ctrl_pkt(uint8_t *a2dp_payload, uint16_t *len);
bool app_dongle_send_ctrl_data(uint8_t *data, uint16_t size);
void app_dongle_stop_send_ack_timer(void);
void app_dongle_rcv_ctrl_data(uint8_t *data, uint16_t size);
void app_dongle_ctrl_init(void);


#ifdef __cplusplus
}
#endif

#endif  // _POLICY_CMD_H_

