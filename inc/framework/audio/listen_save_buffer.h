/*
 * Copyright (c) 2020, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _LISTEN_SAVE_BUFFER_H_
#define _LISTEN_SAVE_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct t_media_listen_save_header
{
    struct t_media_listen_save_header *p_next;
    uint16_t                      seq;
    uint16_t                      len;
    uint16_t                      frame_num;
    uint16_t                      rsvd[2];
    uint8_t                       data[0];
} T_MEDIA_LISTEN_SAVE_HEADER;

typedef struct
{
    uint8_t     frame_num;
    uint16_t    seq_num;
    uint16_t    len;
} T_LISTEN_SAVE_SYNC_INFO;

typedef struct
{
    uint8_t                     pkt_num     : 4;
    uint8_t                     rsv_bits    : 3;
    uint8_t                     end_flag    : 1;
    T_LISTEN_SAVE_SYNC_INFO     pkt_info[3];
} T_LISTEN_SAVE_PRI_PKT_INFO;

void *listen_save_buffer_peek(int index, uint16_t *len, uint32_t *timestamp);

bool listen_save_buffer_write(void *buf, uint16_t len, void *param);

bool listen_save_buffer_flush(uint16_t cnt, void *param);

void listen_save_buffer_reset(void);

void listen_save_media_buffer_init(void);

void listen_save_media_buffer_deinit(void);

uint16_t listen_save_get_pkt_count(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LISTEN_SAVE_BUFFER_H_ */
