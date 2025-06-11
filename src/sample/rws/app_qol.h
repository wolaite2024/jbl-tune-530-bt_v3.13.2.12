
#ifndef _APP_QOL_H_
#define _APP_QOL_H_

#include <stdint.h>
#include <stdbool.h>
#include "os_queue.h"

#define REMOVE_MAX_RSSI 0
#define RSSI_MINIMUM_THRESHOLD_TO_RECEIVE_PACKET_BAD    -70
#define RSSI_MAXIMUM_THRESHOLD_TO_RECEIVE_PACKET_WELL   -60
#define RSSI_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP  -85
#define RSSI_MAXIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP  -60
#define BUFFER_LEVEL_MINIMUM_THRESHOLD_TO_ENABLE_RSSI_ROLESWAP   180
#define B2B_RSSI_THRESHOLD_START_SNIFFING               -75
#define B2B_RSSI_CHECK_INTERVAL                         1000

typedef struct
{
    int16_t aggregate_rssi;
    uint8_t samples_cnt;
    int16_t skip_aggregate_rssi;
} T_QOL_RSSI_DATA;

typedef struct
{
    uint16_t aggregate_per;
    uint8_t samples_cnt;
    uint16_t skip_aggregate_per;
} T_QOL_PER_DATA;

typedef struct
{
    T_QOL_RSSI_DATA rssi;
    T_QOL_PER_DATA per;
    uint16_t interval;
    uint16_t sliding_int;
    uint8_t skipsamples;
} T_APP_QOL_DATA;

typedef enum
{
    APP_REMOTE_MSG_SEC_GOING_AWAY               = 0x00, //sec goes away from src and pri
    APP_REMOTE_MSG_SEC_GOING_NEAR               = 0x01, //pri goes near src and pri
    APP_REMOTE_MSG_PRI_DECODE_EMPTY             = 0x02, //dsp decode empty happen to pri
    APP_REMOTE_MSG_DSP_DECODE_EMPTY             = 0x03, //dsp decode empty happen to pri or sec
    APP_REMOTE_MSG_PERIODIC_UPDATE              = 0x04, //sec notice rssi and buffer level to pri periodicly
    APP_REMOTE_MSG_SYNC_MULTI_SRC_AWAY          = 0X05, // src goes away from buds in multilink scene

    APP_REMOTE_MSG_QOL_TOTAL
} T_QOL_REMOTE_MSG;

bool app_qol_get_aggregate_rssi(bool is_b2b, int8_t *rssi);
void app_qol_link_monitor(uint8_t *bd_addr, bool enable);
void app_qol_init(void);
void app_qol_disable_link_monitor_timer(void);
void app_qol_disable_buffer_level_report_timer(void);
void app_qol_handle_remote_rssi(bool caused_by_decode_empty);

#endif
