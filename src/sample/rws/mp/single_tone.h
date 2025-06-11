/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _SINGLETONE_H_
#define _SINGLETONE_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_RWS_MP App mp
  * @brief App mp
  * @{
  */

/** @brief single tone event */
typedef enum
{
    SINGLE_TONE_EVT_OPENED,     /* single tone open completed */
    SINGLE_TONE_EVT_CLOSED,     /* single tone close completed */
    SINGLE_TONE_EVT_DATA_IND,   /* single tone rx data indicated */
    SINGLE_TONE_EVT_DATA_XMIT,  /* single tone tx data transmitted */
    SINGLE_TONE_EVT_ERROR,      /* single tone error occurred */
} T_SINGLE_TONE_EVT;

/** @brief single tone command param */
#pragma pack(1)
typedef struct
{
    uint8_t pkt_type;
    uint16_t opcode;
    uint8_t length;
    uint8_t start;   // start -->1 , stop -->0
    uint8_t channel; //0-79
    uint8_t tx_gain; //0-255, 0 - Max Tx power of legacy in Config tool RF TX page
    uint8_t rsvd;
} T_SINGLE_TONE_VEND_CMD_PARAMS;
#pragma pack()

/** @brief P_SINGLE_TONE_CALLBACK */
typedef bool (*P_SINGLE_TONE_CALLBACK)(T_SINGLE_TONE_EVT evt, bool status, uint8_t *p_buf,
                                       uint32_t len);
/**
  * @brief  hci open
  * @param  p_callback P_SINGLE_TONE_CALLBACK
  * @retval true   Success
  * @retval false  Failed
  */
extern bool hci_if_open(P_SINGLE_TONE_CALLBACK p_callback);

/**
  * @brief  hci close
  * @retval true   Success
  * @retval false  Failed
  */
extern bool hci_if_close(void);

/**
  * @brief  hci write
  * @param  pbuf   buffer for data
  * @param  len    buffer length
  * @retval true   Success
  * @retval false  Failed
  */
extern bool hci_if_write(uint8_t *p_buf, uint32_t len);

/**
  * @brief  hci confirm
  * @param  pbuf   buffer
  * @retval true   Success
  * @retval false  Failed
  */
extern bool hci_if_confirm(uint8_t *p_buf);

typedef enum
{
    MP_HCI_TEST_DUT_MODE    = 0,
    MP_HCI_TEST_SINGLE_TONE = 1
} T_MP_HCI_TEST_ITEM;

/**
  * @brief  single tone initial
  */
bool mp_hci_test_mode_is_running(void);
void mp_hci_test_mmi_handle_action(uint8_t action);
void mp_hci_test_deinit(void);
void mp_hci_test_init(T_MP_HCI_TEST_ITEM mp_hci_test_item);
void mp_hci_test_auto_enter_dut_init(void);

/**
  * @brief  set hci test mode
  * @param  mp_hci_test_mode_running status
  * @retval none
  */
void mp_hci_test_set_mode(bool mp_hci_test_mode_running_status);

#if F_APP_DUT_MODE_AUTO_POWER_OFF
void dut_test_start_auto_power_off_timer(void);
#endif
/** End of APP_RWS_MP
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* _SINGLETONE_H_ */

