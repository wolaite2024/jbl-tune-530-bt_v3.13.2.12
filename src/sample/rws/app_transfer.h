/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TRANSFER_H_
#define _APP_TRANSFER_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_msg.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_TRANSFER App Transfer
  * @brief App Transfer
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_TRANSFER_Exported_Functions App Transfer Functions
    * @{
    */
/**
    * @brief  Ble Service module data transfer
    * @param  le_link_id: le_link id
    * @param  data_len    the len of data for rws transfer
    * @return p_data      the pointer of data for rws transfer
    */
bool app_transfer_start_for_le(uint8_t le_link_id, uint16_t data_len, uint8_t *p_data);

/**
    * @brief  Ble Service module data transfer
    * @param  le_link_id: le_link id
    * @param  data_len    the len of data for rws transfer
    * @return p_data      the pointer of data for rws transfer
    */
bool app_transfer_start_for_le(uint8_t le_link_id, uint16_t data_len, uint8_t *p_data);
/**
    * @brief  App will put the data,which needs transfer to the other side, in the transfer queue.
    *         The transfer queue can play a buffer role and achieve retransmission function.
    * @param  cmd_path distinguish legacy or le path
    * @param  link_idx app link channel index
    * @param  *p_data need to transfer data
    * @param  data_len transfer data length
    * @return bool
    */
bool app_transfer_push_data_queue(uint8_t cmd_path, uint8_t *data, uint16_t data_len,
                                  uint8_t extra_param);

/**
    * @brief  App will get the data,which needs transfer to the other side, in the transfer queue.
    *         The transfer queue can play a buffer role and achieve retransmission function.
    *         App use iap,or spp,or le channel send data according to active flag
    * @param  next_flag when next_flag is true, app will discard this rx index data
    * @return void
    */
void app_transfer_pop_data_queue(uint8_t cmd_path, bool next_flag);

/**
    * @brief  check current transfer state is active or not
    *
    * @param  void
    * @return check result
    */
bool app_transfer_check_active(uint8_t cmd_path);

/**
    * @brief  handle io msg in app transfer module
    *
    * @param  void
    * @return none
    */
void app_transfer_handle_msg(T_IO_MSG *io_driver_msg_recv);

/**
    * @brief  app transfer module init.
    *
    * @param  void
    * @return none
    */
void app_transfer_init(void);

/**
    * @brief  app transfer queue reset.
    *
    * @param  cmd_path distinguish legacy, le path or UART
    * @return none
    */
void app_transfer_queue_reset(uint8_t cmd_path);

/**
    * @brief  app transfer queue check ack event id.
    *
    * @param  cmd_path distinguish legacy, le path
    * @return none
    */
void app_transfer_queue_recv_ack_check(uint16_t event_id, uint8_t cmd_path);

/**
    * @brief  app transfer cmd handler
    *
    * @param  cmd_ptr command data
    * @param  cmd_len command length
    * @param  cmd_path command path is used to distinguish from uart, le, spp or iap channel.
    * @param  app_idx received rx command device index
    * @param  ack_pkt ACK data
    * @return none
    */
void app_transfer_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                             uint8_t *ack_pkt);

/** @} */ /* End of group APP_TRANSFER_Exported_Functions */

/** End of APP_TRANSFER
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TRANSFER_H_ */
