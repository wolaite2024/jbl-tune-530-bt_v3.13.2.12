/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     app_cfu_transfer.h
  * @brief
  * @details
  * @author   huan_yang
  * @date     2023-1-6
  * @version  v1.0
  * *************************************************************************************
  */

#ifndef _APP_CFU_TRANSFER_H_
#define _APP_CFU_TRANSFER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_CFU_TRANSFER APP CFU TRANSFER
    * @brief app cfu transfer api
    * @{
    */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup APP_CFU_TRANSFER_Exported_Macros APP CFU TRANSFER Exported Macros
    * @brief
    * @{
    */
#define REPORT_ID_CFU_FEATURE               0x2A
#define REPORT_ID_CFU_FEATURE_EX            0x2B
#define REPORT_ID_CFU_OFFER                 0x2D
#define REPORT_ID_CFU_CONTENT               0x2C
#define REPORT_ID_CFU_TEST                  0x90


#define SPP_PKT_HEADER_LEN (6)    //< length for header
#define SPP_PKT_SYNC_FIELD_LEN (3)    //< length for sync field
#define SPP_PKT_LENGTH_LEN (2)    //< length for length field
#define SPP_PKT_REPORT_ID_LEN (1)    //< length for report id

/** End of APP_CFU_TRANSFER_Exported_Macros
    * @}
    */
typedef enum t_app_cfu_report_action_type
{
    APP_CFU_REPORT_ACTION_NONE,
    APP_CFU_REPORT_ACTION_GET,
    APP_CFU_REPORT_ACTION_SET,
} T_APP_CFU_REPORT_ACTION_TYPE;

typedef enum t_app_cfu_report_source
{
    APP_CFU_REPORT_SOURCE_USB,
    APP_CFU_REPORT_SOURCE_BLE,
    APP_CFU_REPORT_SOURCE_SPP,
    APP_CFU_REPORT_SOURCE_CLASSIC_HID,
} T_APP_CFU_REPORT_SOURCE;

typedef struct t_app_cfu_usb_data
{
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_CFU_USB_DATA;

typedef struct t_app_cfu_ble_data
{
    uint8_t conn_id;
    uint8_t report_type;
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_CFU_BLE_DATA;

typedef struct t_app_cfu_spp_data
{
    uint8_t bd_addr[6];
    uint8_t report_id;
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_CFU_SPP_DATA;

typedef struct t_app_cfu_classic_data
{
    uint8_t bd_addr[6];
    uint8_t report_id;
    uint8_t report_type;
    uint16_t report_size;
    T_APP_CFU_REPORT_ACTION_TYPE report_action;
    uint16_t data_len;
    uint8_t *p_data;
} T_APP_CFU_CLASSIC_DATA;

typedef union t_app_cfu_data
{
    T_APP_CFU_USB_DATA usb_data;
    T_APP_CFU_BLE_DATA ble_data;
    T_APP_CFU_SPP_DATA spp_data;
    T_APP_CFU_CLASSIC_DATA classic_hid_data;
} T_APP_CFU_DATA;

/*============================================================================*
 *                              Functions
 *============================================================================*/

/** @defgroup APP_CFU_TRANSFER_Exported_Functions APP CFU TRANSFER Exported Functions
    * @brief
    * @{
    */

/**
    * @brief parse cfu spp header.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param data_len data length
    * @param p_data spp data
    * @param cfu_data cfu data
    * @return True: success; False: fail
    */
bool app_cfu_spp_header_parse(uint16_t data_len, uint8_t *p_data, T_APP_CFU_DATA *cfu_data);

/**
    * @brief parse cfu spp header.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param cfu_data cfu data
    * @return True: success; False: fail
    */
bool app_cfu_ble_header_parse(T_APP_CFU_DATA *cfu_data);

/**
    * @brief handle cfu report.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param report_id report id
    * @param source_type source type
    * @param p_cfu_data data
    * @return void
    */
void app_cfu_handle_report(uint8_t report_id, T_APP_CFU_REPORT_SOURCE source_type,
                           T_APP_CFU_DATA *p_cfu_data);

/**
    * @brief usb get report.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param report_id report id
    * @param data response data
    * @param len data length
    * @return True: success; False: fail
    */
bool app_cfu_get_report(uint8_t report_id, uint8_t *data, uint16_t *len);

/**
    * @brief usb set report.
    *\xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param data data
    * @param len data length
    * @return void
    */
void app_cfu_set_report(uint8_t *data, uint16_t len);

/** @} */ /* End of group APP_CFU_TRANSFER_Exported_Functions */
/** @} */ /* End of group APP_CFU_TRANSFER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_CFU_TRANSFER_H_ */
