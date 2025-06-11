/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_PBAP_H_
#define _APP_PBAP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_PBAP App Pbap
  * @brief App Pbap
  * @{
  */

#if F_APP_PBAP_CMD_SUPPORT
void app_pbap_cmd_pbap_download(uint8_t *bd_addr, uint16_t pb_size);
void app_pbap_cmd_pbap_download_check(uint8_t *bd_addr);
bool app_pbap_get_auto_pbap_download_continue_flag(void);
void app_pbap_pull_continue_timer_start(void);
void app_pbap_split_pbap_data(uint8_t *p_data, uint16_t data_len);
#endif

/**
    * @brief  pbap related command handler
    * @param  cmd_ptr: pbap command
    * @param  cmd_len: length of command
    * @param  cmd_path: command path
    * @param  app_idx: link index
    * @param  ack_pkt: ack status
    * @return void
    */
void app_pbap_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                         uint8_t *ack_pkt);

/**
    * @brief  init pbap module.
    * @param  void
    * @return void
    */
void app_pbap_init(void);

/** End of APP_PBAP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_PBAP_H_ */
