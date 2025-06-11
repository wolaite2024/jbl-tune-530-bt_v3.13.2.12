/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     ias_client.h
  * @brief    Head file for using Ias BLE Client.
  * @details  Ias data structs and external functions declaration.
  * @author   ken
  * @date     2017-12-04
  * @version  v0.1
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _IAS_CLIENT_H_
#define _IAS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include <bt_gatt_client.h>
//#include "profile_client_ext.h"


/** @defgroup IAS_Client Ias service client
  * @brief ias service client
  * @details
     Ias Profile is a customized BLE-based Profile. Ias ble service please refer to @ref IAS_Service .
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup IAS_Client_Exported_Macros IAS Client Exported Macros
  * @brief
  * @{
  */
/** @defgroup IAS UUIDs
  * @brief Ias BLE Profile UUID definitions
  * @{
  */

#define GATT_UUID_IMMEDIATE_ALERT_SERVICE       0x1802
#define GATT_UUID_CHAR_ALERT_LEVEL              0x2A06

/** @} End of SIMP_UUIDs */

/** @brief  Define links number. range: 0-4 */
#define IAS_MAX_LINKS  2
/** End of IAS_Client_Exported_Macros
  * @}
  */

typedef T_APP_RESULT(*P_FUN_CLIENT_GENERAL_APP_CB)(uint16_t conn_handle, uint16_t srv_uuid,
                                                   void *p_data);

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup IAS_Client_Exported_Types IAS Client Exported Types
  * @brief
  * @{
  */
/** @brief IAS client discovery state*/
typedef enum
{
    DISC_IAS_DONE,
    DISC_IAS_FAILED
} T_IAS_DISC_STATE;


/** @brief IAS client write type*/
typedef enum
{
    IAS_WRITE_ALERT,
} T_IAS_WRTIE_TYPE;

/** @brief IAS client write result*/
typedef struct
{
    T_IAS_WRTIE_TYPE type;
    uint16_t cause;
} T_IAS_WRITE_RESULT;

/** @brief IAS client callback type*/
typedef enum
{
    IAS_CLIENT_CB_TYPE_DISC_STATE,          //!< Discovery procedure state, done or pending.
    IAS_CLIENT_CB_TYPE_WRITE_RESULT,        //!< Write request result, success or fail.
    IAS_CLIENT_CB_TYPE_INVALID              //!< Invalid callback type, no practical usage.
} T_IAS_CLIENT_CB_TYPE;

/** @brief IAS client callback content*/
typedef union
{
    T_IAS_DISC_STATE      disc_state;
    T_IAS_WRITE_RESULT    write_result;
} T_IAS_CLIENT_CB_CONTENT;

/** @brief IAS client callback data*/
typedef struct
{
    T_IAS_CLIENT_CB_TYPE     cb_type;
    T_IAS_CLIENT_CB_CONTENT  cb_content;
} T_IAS_CLT_CB_DATA;

/** End of IAS_Client_Exported_Types * @} */

/** @defgroup IAS_Client_Exported_Functions IAS Client Exported Functions
  * @{
  */

/**
  * @brief      Add ias service client to application.
  * @param[in]  app_cb pointer of app callback function to handle specific client module data.
  * @param[in]  link_num initialize link num.
  * @return Client ID of the specific client module.
  * @retval 0xff failed.
  * @retval other success.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_profile_init(void)
    {

    }
  * \endcode
  */
bool ias_add_client(P_FUN_CLIENT_GENERAL_APP_CB app_cb);

/**
  * @brief  Used by application, to start the discovery procedure of ias server.
  * @param[in]  conn_id connection ID.
  * @retval true send request to upper stack success.
  * @retval false send request to upper stack failed.
  */
bool ias_client_write_char(uint16_t conn_handle, uint16_t length, uint8_t *p_value);

/** @} End of IAS_Client_Exported_Functions */

/** @} End of IAS_Client */


#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _IAS_CLIENT_H_ */
