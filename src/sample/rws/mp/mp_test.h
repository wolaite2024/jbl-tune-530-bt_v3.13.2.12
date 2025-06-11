/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _MP_TEST_H_
#define _MP_TEST_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_MP App mp
  * @brief App mp
  * @{
  */


/** @brief  mp test result */
typedef enum
{
    MP_TEST_RESULT_PASS = 0,
    MP_TEST_RESULT_FAIL = 1,
    MP_TEST_RESULT_FAIL_NO_MEMORY = 2,
    MP_TEST_RESULT_FAIL_FLASH_OP_FAIL = 3,
    MP_TEST_RESULT_FAIL_INVALID_LEN = 4,
    MP_TEST_RESULT_FAIL_NOT_IN_TEST_MODE = 5,
    MP_TEST_RESULT_FAIL_NOT_SUPPORTED_OPCODE     = 6,
    MP_TEST_RESULT_FAIL_NOT_SUPPORTED_SUBOPCODE  = 7,
} MP_TEST_RESULT;

/** @brief mp test opcode */
typedef enum
{
    MP_TEST_OPCODE_RTK = 0,
    MP_TEST_OPCODE_VENDOR_BINARY = 1,
    MP_TEST_OPCODE_VENDOR_AT = 2,
} MP_TEST_OP_CODE;

/** @brief mp test rtk sub opcode*/
typedef enum
{
    MP_TEST_RTK_SUB_OPCODE_ENTER_TEST_MODE      = 0x00,
    MP_TEST_RTK_SUB_OPCODE_EXIT_TEST_MODE       = 0x01,
    MP_TEST_RTK_SUB_OPCODE_SWITCH_TO_HCI_MODE   = 0x02,
    MP_TEST_RTK_SUB_OPCODE_SWITCH_TO_SINGLE_TONE_MODE     = 0x03,
    MP_TEST_RTK_SUB_OPCODE_FACTORY_RESET        = 0x04,
    MP_TEST_RTK_SUB_OPCODE_CHANGE_BAUD_RATE     = 0x05,


    MP_TEST_RTK_SUB_OPCODE_FLASH_READ_BUFFER    = 0x10,
    MP_TEST_RTK_SUB_OPCODE_FLASH_WRITE_BUFFER   = 0x11,
    MP_TEST_RTK_SUB_OPCODE_FLASH_ERASE_SECTOR   = 0x12,
    MP_TEST_RTK_SUB_OPCODE_FLASH_IOCTL          = 0x13,

    MP_TEST_RTK_SUB_OPCODE_AUDIO_TEST           = 0x20,

    MP_TEST_RTK_SUB_OPCODE_VENDOR_CMD           = 0x30,

    MP_TEST_RTK_SUB_OPCODE_CHANGE_DEVICE_NAME   = 0x40,
} MP_TEST_RTK_SUB_OPCODE;

typedef struct
{

} T_MP_TEST_FLASH_WRITE;

typedef struct
{

} T_MP_TEST_FLASH_READ;

/**
  * @brief  mp test handle command
  * @param  app_idx     app index
  * @param  cmd_path    cammand path
  * @param  opcode      opcode
  * @param  sub_opcode  sub opcode
  * @param  p_param     parameters
  * @param  param_len   length of parameters
  * @return true   Success
  * @return false  Failed
  */
bool mp_test_handle_cmd(uint8_t app_idx, uint8_t cmd_path, uint8_t opcode, uint8_t sub_opcode,
                        uint8_t *p_param, uint16_t param_len);

/**
  * @brief  mp test handle command
  * @param  app_idx     app index
  * @param  cmd_path    cammand path
  * @param  result      result
  * @param  opcode      opcode
  * @param  sub_opcode  sub opcode
  * @param  p_param     parameters
  * @param  param_len   length of parameters
  */
void mp_test_send_test_result(uint8_t app_idx, uint8_t cmd_path, uint8_t result,
                              uint8_t opcode, uint8_t sub_opcode, uint8_t *p_param, uint16_t param_len);

/**
  * @brief  mp test send vendor command completion event
  * @param  p_gap_vnd_cmd_cmpl_evt_rsp   vendor command complrtion event response
  */
void mp_test_send_vnd_cmd_cmpl_evt(void *p_gap_vnd_cmd_cmpl_evt_rsp);

bool mp_test_get_mode_flag(void);

void mp_test_set_mode_flag(bool flag);
/** End of APP_RWS_MP
* @}
*/



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _MP_CMD_H_ */
