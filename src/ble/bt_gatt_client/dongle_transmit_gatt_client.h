/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     dongle_transfer_client.h
  * @brief    Head file for using dongle_transfer BLE Client.
  * @details  dongle_transfer data structs and external functions declaration.
  * @author   ken
  * @date     2017-12-04
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _DONGLE_TRANSMIT_CLIENT_H_
#define _DONGLE_TRANSMIT_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "bt_gatt_client.h"
//#include "profile_client_ext.h"

/** @defgroup DONGLE_TRANSMIT_Client dongle_transfer service client
  * @brief dongle_transfer service client
  * @details
     dongle_transfer Profile is a customized BLE-based Profile. dongle_transfer ble service please refer to @ref DONGLE_TRANSMIT_Service .
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup DONGLE_TRANSMIT_Client_Exported_Macros DONGLE_TRANSMIT Client Exported Macros
  * @brief
  * @{
  */

/** @brief  Define links number. range: 0-4 */
#define DONGLE_TRANSMIT_MAX_LINKS  2
/** End of DONGLE_TRANSMIT_Client_Exported_Macros
  * @}
  */

typedef T_APP_RESULT(*P_FUN_CLIENT_GENERAL_APP_CB)(uint16_t conn_handle, uint8_t *p_srv_uuid,
                                                   void *p_data);

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup DONGLE_TRANSMIT_Client_Exported_Types DONGLE_TRANSMIT Client Exported Types
  * @brief
  * @{
  */
/** @brief DONGLE_TRANSMIT client discovery done*/
typedef struct
{
    uint16_t conn_handle;
    bool is_found;
    bool load_from_ftl;
} T_DONGLE_TRANSMIT_DISC_DONE;

/** @brief DONGLE_TRANSMIT client write type*/
typedef enum
{
    DONGLE_TRANSMIT_WRITE_DATA,
} T_DONGLE_TRANSMIT_WRTIE_TYPE;

/** @brief DONGLE_TRANSMIT client write result*/
typedef struct
{
    T_DONGLE_TRANSMIT_WRTIE_TYPE type;
    uint16_t cause;

} T_DONGLE_TRANSMIT_WRITE_RESULT;

/** @brief DONGLE_TRANSMIT client cccd cfg*/
typedef struct
{
    uint16_t conn_handle;
    uint16_t cause;
} T_DONGLE_TRANSMIT_CCCD_CFG;

/** @brief DONGLE_TRANSMIT client notify indication*/
typedef struct
{
    uint16_t conn_handle;
    uint16_t value_size;
    uint8_t *p_value;
} T_DONGLE_TRANSMIT_NOTIFY_IND;

/** @brief DONGLE_TRANSMIT client callback type*/
typedef enum
{
    DONGLE_TRANSMIT_CLIENT_CB_TYPE_DISC_DONE,          //!< Discovery procedure state, done or pending.
    DONGLE_TRANSMIT_CLIENT_CB_TYPE_WRITE_RESULT,        //!< Write request result, success or fail.
    DONGLE_TRANSMIT_CLIENT_CB_TYPE_CCCD_CFG,        //!< Write request result, success or fail.
    DONGLE_TRANSMIT_CLIENT_CB_TYPE_NOTIFY_IND,        //!< Write request result, success or fail.
    DONGLE_TRANSMIT_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} T_DONGLE_TRANSMIT_CLIENT_CB_TYPE;

/** @brief DONGLE_TRANSMIT client callback content*/
typedef union
{
    T_DONGLE_TRANSMIT_DISC_DONE      disc_done;
    T_DONGLE_TRANSMIT_WRITE_RESULT    write_result;
    T_DONGLE_TRANSMIT_CCCD_CFG        cccd_cfg;
    T_DONGLE_TRANSMIT_NOTIFY_IND      notify_ind;
} T_DONGLE_TRANSMIT_CLIENT_CB_CONTENT;

/** @brief DONGLE_TRANSMIT client callback data*/
typedef struct
{
    T_DONGLE_TRANSMIT_CLIENT_CB_TYPE     cb_type;
    T_DONGLE_TRANSMIT_CLIENT_CB_CONTENT  cb_content;
} T_DONGLE_TRANSMIT_CLT_CB_DATA;


/** End of DONGLE_TRANSMIT_Client_Exported_Types * @} */

/** @defgroup DONGLE_TRANSMIT_Client_Exported_Functions DONGLE_TRANSMIT Client Exported Functions
  * @{
  */

/**
  * @brief           Add dongle_transfer service client to application.
  * @param[in]       app_cb pointer of app callback function to handle specific client module data.
  * @return true     add client to upper stack success
  * @return false    add client to upper stack fail
  */
bool dongle_transmit_client_init(P_FUN_CLIENT_GENERAL_APP_CB app_cb);

/**
  * @brief  Used by application, to write char to dongle_transfer server.
  * @param conn_handle     connection handle.
  * @param p_dongle_transmit_data the pointer of data for writing to server.
  * @param data_len        the len of data for writing to server
  * @retval true           send data success.
  * @retval false          send data failed.
  */
bool dongle_transmit_data_write(uint16_t conn_handle, uint8_t *p_dongle_transmit_data,
                                uint16_t data_len);

/**
  * @brief  Used by application, to write char to dongle_transmit server.
  * @param conn_handle     connection handle.
  * @param p_dongle_transmit_data the pointer of data for writing to server.
  * @param data_len        the len of data for writing to server
  * @retval true           send data success.
  * @retval false          send data failed.
  */
bool dongle_transmit_dev_info_write(uint16_t conn_handle, uint8_t *p_dongle_transmit_data,
                                    uint16_t data_len);

/** @} End of DONGLE_TRANSMIT_Client_Exported_Functions */

/** @} End of DONGLE_TRANSMIT_Client */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _DONGLE_TRANSMIT_CLIENT_H_ */
