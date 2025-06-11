/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_TEST_H_
#define _APP_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "app_report.h"

/** @defgroup APP_TEST App test
  * @brief App test
  * @{
  */

#if F_APP_DEVICE_CMD_SUPPORT
uint8_t *app_test_get_acl_conn_ind_bd_addr(void);
uint8_t *app_test_get_user_confirmation_bd_addr(void);
uint8_t app_test_get_sco_state(void);
#endif

#if F_APP_HFP_CMD_SUPPORT
typedef struct
{
    uint8_t     call_idx;
    uint8_t     dir_incoming;
    uint8_t     status;
    uint8_t     mode;
    uint8_t     mpty;
    uint8_t     type;
    uint8_t     num_len;
    const char *number;
} T_HFP_CURRENT_CALL_LIST;
#endif

typedef enum
{
    VOL_CHANGE_NONE                  = 0x00,
    VOL_CHANGE_UP                    = 0x01,
    VOL_CHANGE_DOWN                  = 0x02,
    VOL_CHANGE_MAX                   = 0x03,
    VOL_CHANGE_MIN                   = 0x04,
    VOL_CHANGE_MUTE                  = 0x05,
    VOL_CHANGE_UNMUTE                = 0x06,
} T_VOL_CHANGE;

typedef struct
{
    uint8_t             *spp_ptr;
    uint16_t            w_idx;
    uint16_t            r_idx;
    uint16_t            total_len;
    uint8_t             vendor_action_index;
} T_VENDOR_SPP_DATA;

void app_test_dsp_capture_data_set_param(uint8_t idx);
void app_test_report_event(uint8_t *bd_addr, uint16_t event_id, uint8_t *data, uint16_t len);
/**
  * @brief  Initiate app test module.
  * @param  None
  * @return None
  */
void app_test_init(void);

/** End of APP_TEST
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEST_H_ */
