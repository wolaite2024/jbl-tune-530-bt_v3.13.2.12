/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef SHM_API_H
#define SHM_API_H
#include <stdint.h>
#include <stdbool.h>

#define PKT_TYPE_SCO                0
#define PKT_TYPE_A2DP               1
#define PKT_TYPE_VOICE_PROMPT       2
#define PKT_TYPE_LOST_PACKET        3
#define PKT_TYPE_IOT_DATA           4
#define PKT_TYPE_RAW_AUDIO_DATA     5
#define PKT_TYPE_ZERO_PACKET        6
#define PKT_TYPE_DUMMY              0xFF

#define AAC_TYPE_LATM_NORMAL        0x00
#define AAC_TYPE_LATM_SIMPLE        0x01
#define AAC_TYPE_ADTS               0x02
#define AAC_TYPE_ADIF               0x03
#define AAC_TYPE_RAW                0x04

#define STREAM_CHANNEL_OUTPUT_MONO          0
#define STREAM_CHANNEL_OUTPUT_STEREO        1

typedef enum
{
    SHM_STATE_RESET,
    SHM_STATE_SHARE_MEM_READY,
    SHM_STATE_INIT_FINISH,

    SHN_STATE_TOTAL
} T_SHM_STATE;

typedef enum
{
    SHM_IOCTL_VERSION,
    SHM_IOCTL_CALLBACK_CMD_EVENT,
    SHM_IOCTL_CALLBACK_RX_REQ,
    SHM_IOCTL_CALLBACK_TX_ACK,
    SHM_IOCTL_CALLBACK_MAILBOX,
    SHM_IOCTL_QUERY_STATE,
    SHM_IOCTL_H2D_CMD_UNPROCESSED_LENGTH,
    SHM_IOCTL_D2H_CMD_UNPROCESSED_LENGTH,
    SHM_IOCTL_H2D_DATA_UNPROCESSED_LENGTH,
    SHM_IOCTL_D2H_DATA_UNPROCESSED_LENGTH,

    SHN_IOCTL_TOTAL
} T_SHM_IO_CTL;

typedef enum
{
    CLK_DIV_NONE,       //0x00
    CLK_DIV_BY_7_8,     //0x01
    CLK_DIV_BY_3_4,     //0x02
    CLK_DIV_BY_2_3,     //0x03
    CLK_DIV_BY_1_2,     //0x04
    CLK_DIV_BY_1_3,     //0x05
    CLK_DIV_BY_1_4,     //0x06
    CLK_DIV_BY_1_5,     //0x07
    CLK_DIV_BY_1_6,     //0x08
    CLK_DIV_BY_1_7,     //0x09
    CLK_DIV_BY_1_8,     //0x0A
    CLK_DIV_BY_1_10,    //0x0B
    CLK_DIV_BY_1_12,    //0x0C
    CLK_DIV_BY_1_16,    //0x0D
    CLK_DIV_BY_1_32,    //0x0E
    CLK_DIV_BY_1_64,    //0x0F

    CLK_DIV_TOTAL
} T_DSP_CLK_DIV;

typedef enum
{
    DSP_ONLY,
    DSP_SHARE_8K,
    DSP_SHARE_16K,
    DSP_SHARE_24K,
    DSP_SHARE_32K,
    DSP_SHARE_40K,
    DSP_SHARE_48K,
    DSP_SHARE_56K,
    DSP_SHARE_64K,
    DSP_SHARE_72K,
    DSP_SHARE_80K,
    DSP_SHARE_88K,
    DSP_SHARE_96K,
    DSP_SHARE_104K,
    DSP_SHARE_112K,
    DSP_SHARE_120K,
    DSP_SHARE_128K,
    DSP_SHARE_136K,
    DSP_SHARE_144K,
    DSP_SHARE_152K,
    DSP_SHARE_160K,
    DSP_SHARE_168K,
    DSP_SHARE_176K,
    DSP_SHARE_184K,
    DSP_SHARE_192K,
    DSP_SHARE_200K,
    DSP_SHARE_208K,
    DSP_SHARE_216K,
    DSP_SHARE_224K,
    DSP_SHARE_232K,
    DSP_SHARE_240K,

    DSP_SHARE_TOTAL,
} T_DSP_SHARE_RAM;

typedef struct t_exp_wr_header
{
    union
    {
        uint16_t raw16;
        struct
        {
            uint16_t sequence           : 8;
            uint16_t length_minus_ofs   : 2;
            uint16_t rsvd               : 6;
        } cmd;

        struct
        {
            uint16_t sequence           : 8;
            uint16_t length_minus_ofs   : 2;
            uint16_t rsvd               : 6;
        } data;
    };
} T_EXP_WR_HEADER;

typedef struct t_exp_wr_iteam
{
    T_EXP_WR_HEADER header;
    uint16_t length;
    uint8_t *p_buffer;
} T_EXP_WR_ITEAM;

void shm_init(void);
uint32_t shm_enable(uint32_t max_timeout);
void shm_reset(uint8_t func_only, uint32_t max_timeout);
void shm_disable(void);
void shm_deinit(void);
uint32_t shm_cmd_recv(uint8_t *buffer, uint16_t length);
void shm_clear_wdg_timer(void);
void shm_set_dsp_clock(void);
void shm_cfg_wdg(uint8_t enable, uint8_t fun_only);
void shm_cfg_asrc(uint8_t enable, uint8_t fun_only);
void shm_cfg_core(uint8_t enable, uint8_t fun_only);
void shm_cfg_h2d_d2h(uint8_t enable, uint8_t fun_only);
void shm_cfg_dma(uint8_t enable, uint8_t fun_only);
void shm_cfg_mem(uint8_t enable, uint8_t fun_only);
void shm_cfg_run(uint8_t run);
void shm_cfg_dsp_clk(uint8_t enable);
uint32_t shm_io_ctl(uint32_t ioctl, uint32_t para);
uint8_t shm_switch_clk(uint8_t type, uint8_t require_mips);
uint32_t shm_state_get(void);
void shm_init_share_queue(void);

// Raven.note
// For HW performance reason, make sure "buffer" is four bytes alignment
// example for keil,
// uint8_t buffer[128] __attribute__ ((aligned (4)));
bool h2d_cmd_send(uint8_t *buffer, uint16_t length, bool flush);
// return 1 : ok
uint32_t d2h_cmd_recv(uint8_t *buffer, uint16_t length);
// return "received length"
bool h2d_data_send(uint8_t type, uint8_t *buffer, uint16_t length, bool flush, uint8_t seq);
// return 1 : ok
uint32_t d2h_data_recv(uint8_t *p_type, uint8_t *buffer, uint16_t length);
// return "received length"
uint32_t d2h_data_length_peek(void);
uint32_t d2h_data_flush(void);

uint32_t h2d_data_send2(void *addr, uint8_t type, uint8_t *buffer, uint16_t length,
                        bool flush, uint8_t seq);
uint32_t d2h_data_recv2(void *addr, uint8_t *p_type, uint8_t *buffer, uint16_t length);

#endif // #ifndef SHM_API_H

