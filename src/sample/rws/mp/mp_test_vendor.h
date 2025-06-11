/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _MP_TEST_VENDOR_H_
#define _MP_TEST_VENDOR_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_MP App mp
  * @brief APP_RWS_MP App mp
  * @{
  */

/** @brief mp test vendor binary sub opcode */
typedef enum
{
    MP_TEST_VENDOR_SUB_OPCODE_LED = 0,
    MP_TEST_VENDOR_SUB_OPCODE_KEY = 1,
    MP_TEST_VENDOR_SUB_OPCODE_ENTER_NORMAL_APT  = 2,
    MP_TEST_VENDOR_SUB_OPCODE_EXIT_NORMAL_APT   = 3,
} MP_TEST_VENDOR_BINARY_SUB_OPCODE;

/**
  * @brief  mp test handle vendor binary command
  * @param  app_idx     app index
  * @param  cmd_path    cammand path
  * @param  sub_opcode  sub opcode
  * @param  p_param     parameters
  * @param  param_len   length of parameters
  * @retval true   Success
  * @retval false  Failed
  */
bool mp_test_handle_vendor_binary_cmd(uint8_t app_idx, uint8_t cmd_path, uint8_t sub_opcode,
                                      uint8_t *p_param, uint16_t param_len);
/**
  * @brief  mp test vendor send key event
  * @param  key_index     key index
  */
void mp_test_vendor_send_key_event(uint8_t key_index);

bool app_mp_test_enter_normal_apt_flag_get(void);

void app_mp_test_enter_normal_apt_success(void);

/** End of APP_RWS_MP
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MP_CMD_H_ */
