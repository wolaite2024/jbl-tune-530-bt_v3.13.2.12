/**
 * @copyright Copyright (C) 2022 Realtek Semiconductor Corporation.
 *
 * @file ipc.h
 * @version 1.0
 * @brief IPC-related definitions
 *
 * @note:
 */
#ifndef __IPC_H__
#define __IPC_H__
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DSP2_IPC_TOPIC                 "DSP2"

typedef enum
{
    IPC_EVT_DSP2_READY,
    IPC_EVT_D2H_CMD,
    IPC_EVT_DATA_IND,
    IPC_EVT_DATA_ACK,
} T_IPC_EVT;

typedef enum
{
    DSP2_H2D_SPP_CAPTURE = 0x7F,
} T_IPC_MCU_TO_DSP2_CMD;

/**
 * @brief task to process ipc-related evt
 *
 * @param pvParameters
 */
void ipc_task(void *pvParameters);

/**
 * @brief receive dsp2 data
 *
 * \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 *
 * @param p_type data type.Typical usage is to indicate data type in \buffer
 * @param buffer pointer to receive dsp2 data
 * @param length length to receive dsp2 data
 * @param recv_queue_id Queue_id of receiving dsp2 data
 * @return uint32_t actual data length
 */
uint32_t ipc_dsp2_data_recv(uint8_t *p_type, uint8_t *buffer, uint16_t length,
                            uint32_t recv_queue_id);

/**
 * @brief send data to dsp2
 *
 * \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 *
 * @param type data type.Typical usage is to indicate data type in \buffer
 * @param buffer point to send data to dsp2
 * @param length length to send data to dsp2
 * @param flush If data has been sent after the function returns. \n
 *              true: command has been put into data queue and inform DSP2 about the data. \n
 *              false: command has just put into data queue
 * @param seq user-defined parameter.Typical usage is to indicate data sequence.
 * @param send_queue_id queue_id of sending dsp2 data
 * @return true data is sent successfully
 * @return false data is sent unsuccessfully
 */
bool ipc_dsp2_data_send(uint8_t type, uint8_t *buffer, uint16_t length, bool flush, uint8_t seq,
                        uint32_t send_queue_id);

/**
 * @brief send cmd to dsp2
 *
 * Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 *
 * @param buffer point to send cmd to dsp2
 * @param length length to send cmd to dsp2
 * @param flush If command has been sent after the function returns. \n
 *              true: command has been put into cmd queue and inform DSP2 about the cmd. \n
 *              false: command has just put into shm queue
 * @return true command is sent successfully
 * @return false command is sent unsuccessfully
 */
bool ipc_dsp2_cmd_send(uint8_t *buffer, uint16_t length, bool flush);

/**
 * @brief receive dsp2 cmd
 *
 * \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 *
 * @param buffer point to receive dsp2 cmd
 * @param length length to receive dsp2 cmd
 * @return uint32_t actual cmd length
 */
uint32_t ipc_dsp2_cmd_recv(uint8_t *buffer, uint16_t length);

/**
 * @brief receive dsp2 data length
 *
 * \xrefitem Added_API_2_12_1_0 "Added Since 2.12.1.0" "Added API"
 *
 * @param queue_id Queue_id of receiving dsp2 data
 * @return uint32_t actual data length
 */
uint32_t ipc_dsp2_data_length_peek(uint32_t queue_id);

#endif //__IPC_H__
