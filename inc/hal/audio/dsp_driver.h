/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _DSP_HAL_H_
#define _DSP_HAL_H_


#include <stdint.h>
#include "sys_ipc.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    DSP_MSG_INTR_D2H_CMD,
    DSP_MSG_INTR_UP_STREAM,
    DSP_MSG_INTR_DOWN_STREAM_ACK,
    DSP_MSG_INTR_MAILBOX,
    DSP_MSG_CHECK_LOAD_IMAGE_FINISH,
    DSP_MSG_LOAD_IMAGE_FIHISH,
    DSP_MSG_ADJUST_DAC_GAIN_DOWN,
    DSP_MSG_ADJUST_DAC_GAIN_UP,
    DSP_MSG_CODEC_STATE,
    DSP_MSG_FADE_OUT_FINISH,
    DSP_MSG_SPORT_STOP,
    DSP_MSG_SPORT_START,
    DSP_MSG_PREPARE_READY,
    DSP_MSG_FAKE_ACTION,
} T_DSP_MSG;

typedef enum t_dsp_hal_priority
{
    DSP_HAL_PRIORITY_LOW,
    DSP_HAL_PRIORITY_HIGH,
} T_DSP_HAL_PRIORITY;

typedef enum t_dsp_hal_evt
{
    DSP_HAL_EVT_NONE                = 0x0,
    DSP_HAL_EVT_D2H                 = 0x1,
    DSP_HAL_EVT_DATA_IND            = 0x2,
    DSP_HAL_EVT_DATA_ACK            = 0x3,
    DSP_HAL_EVT_MAILBOX             = 0x4,
    DSP_HAL_EVT_DSP_LOAD_PART       = 0x5,
    DSP_HAL_EVT_DSP_LOAD_FINISH     = 0x6,
    DSP_HAL_EVT_CODEC_STATE         = 0x7,
    DSP_HAL_EVT_FADE_OUT_FINISH     = 0x8,
    DSP_HAL_EVT_SPORT_STOP          = 0x9,
    DSP_HAL_EVT_SPORT_START         = 0xA,
    DSP_HAL_EVT_PREPARE_READY       = 0xB,
    DSP_HAL_EVT_FAKE_ACTION         = 0xC,
} T_DSP_HAL_EVT;

typedef struct
{
    uint8_t     type;
    uint8_t     subtype;
    uint16_t    data_len;
    void       *p_data;
} T_DSP_SCHED_MSG;

#define READY_BIT_DSP   (1<<1)
#define READY_BIT_DSP2  (1<<2)
#define READY_BIT_ALL   (READY_BIT_DSP|READY_BIT_DSP2)

bool dsp_hal_init(void);
void dsp_hal_deinit(void);
T_SYS_IPC_HANDLE dsp_hal_register_cback(T_DSP_HAL_PRIORITY priority, P_SYS_IPC_CBACK cback);
bool dsp_hal_unregister_cback(T_SYS_IPC_HANDLE handle);
void dsp_hal_boot(void);
void dsp_hal_reset(void);
void dsp_hal_shut_down(void);
void dsp_shm_cb_cmd_event(void);
void dsp_shm_cb_rx_req(void);
void dsp_shm_cb_tx_ack(void);
void dsp_shm_cb_mailbox(uint32_t group, uint32_t value);
void dsp_send_msg(uint8_t type, uint8_t subtype, void *p_data, uint16_t len);

// TODO: integrate to other place with dsp_shut_down()
uint32_t dsp_hal_boot_ref_cnt_get(void);

//#if (TARGET_RTL8773DO == 1)/*https://jira.realtek.com/browse/BBPRO3RD-762*/
void dsp_hal_ready_set(uint8_t val);
uint8_t dsp_hal_ready_get(void);
void dsp_hal_enable_dsp2(void);
//#endif

void dsp_hal_retrieve_malleus_security_value(void);
bool dsp_hal_is_fpga_mode(void);

// TODO: B-Cut ???
extern void (*dsp_log_output)(void);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _DSP_HAL_H_ */
