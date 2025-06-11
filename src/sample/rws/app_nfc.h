/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_NFC_H_
#define _APP_NFC_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_NFC App Nfc
  * @brief NFC module implementation for rws sample project
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup NFC_PROCESS_Exported_Macros App Nfc Macros
  * @{
  */

/**NFC profile*/
#define NFC_MULTI_LINK_SWITCH_A2DP               0x01
#define NFC_MULTI_LINK_SWITCH_HF                 0x02

/** End of NFC_PROCESS_Exported_Macros
  * @}
  */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup NFC_PROCESS_Exported_Types App Nfc Types
    * @{
    */

/**  @brief  Define NFC data for record NFC detect status and stable count */
typedef struct
{
    uint8_t nfc_detected;           /**< NFC is detected*/
    uint8_t nfc_stable_count;       /**< record count of NFC stable */
    uint8_t nfc_multi_link_switch;  /**< NFC handover flag*/
} T_NFC_DATA;

extern T_NFC_DATA nfc_data;

/** End of NFC_PROCESS_Exported_Types
    * @}
    */

void app_nfc_board_init(void);
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup NFC_PROCESS_Exported_Functions App Nfc Functions
    * @{
    */

/**
    * @brief  NFC GPIO initial.
    *         Include APB peripheral clock config, NFC GPIO parameter config and
    *         NFC GPIO interrupt mark config. Enable NFC GPIO interrupt.
    * @param  void
    * @return void
    */
void app_nfc_init(void);

/**
    * @brief  NFC GPIO interrupt will be handle in this function.
    *         First disable app enter dlps mode and read current NFC GPIO input data bit.
    *         Disable NFC GPIO interrupt and send IO_MSG_TYPE_GPIO message to app task.
    *         Then enable NFC GPIO interrupt.
    * @param  void
    * @return void
    */
void app_nfc_intr_handler(void);

/**
    * @brief  NFC GPIO mmi msg will be handle in this function.
    *         First recorder NFC GPIO input status and turn on stable timer.
    * @param  void
    * @return void
    */
void app_nfc_handle_msg(T_IO_MSG *io_driver_msg_recv);

/**
    * @brief  NFC GPIO roleswap ttigger if enable_nfc_roleswap is enable.
    *         Delay for AG connected,then enter into roleswap.
    * @param  void
    * @return void
    */
void app_nfc_multi_link_switch_trigger(void);
/** @} */ /* End of group NFC_PROCESS_Exported_Functions */
/** @} */ /* End of group NFC_PROCESS */
#ifdef __cplusplus
}
#endif
#endif

