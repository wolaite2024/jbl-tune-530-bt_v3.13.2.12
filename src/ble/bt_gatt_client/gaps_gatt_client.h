/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gaps_gatt_client.h
  * @brief    Head file for using GAP GATT service client.
  * @details  GAP GATT service client data structs and external functions declaration
  *           developed based on bt_gatt_client.h.
  * @author   carrie
  * @date     2024-05-17
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _GAPS_GATT_CLIENT_H_
#define _GAPS_GATT_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/* Add Includes here */
#include "bt_gatt_client.h"

/** @defgroup GAPS_GATT_Client GAP Service GATT Client
  * @brief GAP service GATT client
  * @details
     Application shall register gaps client when initialization through @ref gaps_client_init function.

     Application can read GAP service referenced values through @ref gaps_client_read_char function.

     Application can write device name characteristic values if write is supported through
     @ref gaps_client_write_device_name function.

     Application can write appearance characteristic values if write is supported through
     @ref gaps_client_write_appearance function.

     Application shall handle callback function registered by gaps_client_init.
  * \code{.c}
    T_APP_RESULT app_gaps_client_callback(uint16_t conn_handle, uint8_t type, void *p_data)
    {
        T_APP_RESULT  result = APP_RESULT_SUCCESS;
        switch(type)
        {
            case GATT_MSG_BAS_CLIENT_DIS_DONE:
                {
                    T_GAPS_CLIENT_DIS_DONE *p_dis_done = (T_GAPS_CLIENT_DIS_DONE *)p_data;
                }
                break;
            ......
        }
    }
  * \endcode
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup GAPS_GATT_Client_Exported_Macros GAPS GATT Client Exported Macros
  * @brief
  * @{
  */
/** @defgroup GAPS_CLT_CB_MSG
 * @brief GAPS client callback messages
 * @{
 */
#define GATT_MSG_GAPS_CLIENT_DIS_DONE                   0x00
#define GATT_MSG_GAPS_CLIENT_READ_RESULT                0x01
#define GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT   0x02
#define GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT    0x03
/** @} End of GAPS_CLT_CB_MSG */
/** @} End of GAPS_GATT_Client_Exported_Macros */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAPS_GATT_Client_Exported_Types GAPS GATT Client Exported Types
  * @brief
  * @{
  */
/**
 * @brief P_FUN_GAPS_CLIENT_APP_CB GAPS Client Callback Function Point Definition Function
 *        pointer used in GAPS client module, to send events to specific client module.
 *        The events @ref GAPS_CLT_CB_MSG.
 */
typedef T_APP_RESULT(*P_FUN_GAPS_CLIENT_APP_CB)(uint16_t conn_handle, uint8_t type, void *p_data);

/** @brief GAPS client discovery result
 *         The message data for GATT_MSG_GAPS_CLIENT_DIS_DONE.
 */
typedef struct
{
    bool     is_found;
    bool     load_from_ftl;
    bool     device_name_write_support;
    bool     appearance_write_support;
    bool     central_address_resolution_exist;
    bool     resolvable_private_addr_only_exist;
} T_GAPS_CLIENT_DIS_DONE;

/** @brief GAPS client prefer connect parameter data.*/
typedef struct
{
    uint16_t conn_interval_min;
    uint16_t conn_interval_max;
    uint16_t slave_latency;
    uint16_t supervision_timeout;
} T_GAPS_CLIENT_PRE_CONN_PARAM;

/** @brief GAPS client read result
 *         The message data for GATT_MSG_GAPS_CLIENT_READ_RESULT.
 */
typedef struct
{
    uint16_t char_uuid;       /**< @ref GATT_CHARACTERISTICS_UUID. */
    uint16_t cause;
    uint16_t value_len;
    union
    {
        uint8_t *p_device_name;                         /**< When char_uuid is GATT_UUID_CHAR_DEVICE_NAME and
                                                             the cause is success, the data is p_device_name. */
        uint16_t device_appearance;                     /**< When char_uuid is GATT_UUID_CHAR_APPEARANCE and
                                                             the cause is success, the data is device_appearance. */
        T_GAPS_CLIENT_PRE_CONN_PARAM
        prefer_conn_param; /**< When char_uuid is GATT_UUID_CHAR_PER_PREF_CONN_PARAM and
                                                             the cause is success, the data is prefer_conn_param. */
        uint8_t  central_addr_resolution;               /**< When char_uuid is GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION and
                                                             the cause is success, the data is central_addr_resolution. */
        uint8_t  rpa_only;                              /**< When char_uuid is GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY and
                                                             the cause is success, the data is rpa_only. */
    } data;
} T_GAPS_CLIENT_READ_RESULT;

/** @brief GAPS client write device name result
 *         The message data for GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT.
 */
typedef struct
{
    uint16_t cause;
} T_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT;

/** @brief GAPS client write device appearance result
 *         The message data for GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT.
 */
typedef struct
{
    uint16_t cause;
} T_GAPS_CLIENT_WRITE_APPEARANCE_RESULT;

/** End of GAPS_GATT_Client_Exported_Types * @} */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup GPAS_GATT_Client_Exported_Functions GAPS GATT Client Exported Functions
  * @brief
  * @{
  */
/**
  * @brief  Add gap service client to application.
  * @param[in]  app_cb  pointer of app callback function to handle specific client module data.
  * @return Result of add the specific client module.
  * @retval true  Add client module success.
  * @retval false Add client module failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void app_le_profile_init(void)
    {
        if(!gaps_client_init(app_gaps_client_callback))
        {
            APP_PRINT_ERROR0("app_le_profile_init: add gaps client fail!");
        }
        gaps_client_init(app_gaps_gatt_client_callback);
    }

    T_APP_RESULT app_gaps_gatt_client_callback(uint16_t conn_handle, uint8_t type, void *p_data)
    {
        if (type == GATT_MSG_GAPS_CLIENT_DIS_DONE)
        {
            T_GAPS_CLIENT_DIS_DONE *p_dis = (T_GAPS_CLIENT_DIS_DONE *)p_data;
            APP_PRINT_INFO3("GATT_MSG_GAPS_CLIENT_DIS_DONE: conn_handle 0x%x, is_found %d, load_from_ftl %d",
                            conn_handle, p_dis->is_found, p_dis->load_from_ftl);
        }
        else if (type == GATT_MSG_GAPS_CLIENT_READ_RESULT)
        {
            ......
        }
        ......

        return APP_RESULT_SUCCESS;
    }
  * \endcode
  */
bool gaps_client_init(P_FUN_GAPS_CLIENT_APP_CB app_cb);

/**
  * @brief  Used by application, to read data from server by using uuid.
  * @param[in]  conn_handle  Connection handle of the ACL link.
  * @param[in]  char_uuid    Characteristic UUID : @ref GATT_CHARACTERISTICS_UUID.
  * @retval true send read request to upper stack success.
  * @retval false send read request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(uint16_t conn_handle, uint16_t char_uuid)
    {
        if(!gaps_client_read_char(conn_handle, char_uuid))
        {
            APP_PRINT_ERROR0("write alert level fail!");
        }
    }

    T_APP_RESULT app_gaps_gatt_client_callback(uint16_t conn_handle, uint8_t type, void *p_data)
    {
        if (type == GATT_MSG_GAPS_CLIENT_READ_RESULT)
        {
            T_GAPS_CLIENT_READ_RESULT *p_read = (T_GAPS_CLIENT_READ_RESULT *)p_data;
            APP_PRINT_INFO3("GATT_MSG_GAPS_CLIENT_READ_RESULT: conn_handle 0x%x, cause 0x%x, char_uuid 0x%x",
                            conn_handle, p_read->cause, p_read->char_uuid);

            if (p_read->cause == APP_RESULT_SUCCESS)
            {
                switch (p_read->char_uuid)
                {
                case GATT_UUID_CHAR_DEVICE_NAME:
                    {
                        APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: device_name %s",
                                        TRACE_STRING(p_read->data.p_device_name));
                    }
                    break;

                case GATT_UUID_CHAR_APPEARANCE:
                    {
                        APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: device_appearance 0x%x",
                                        p_read->data.device_appearance);
                    }
                    break;

                case GATT_UUID_CHAR_PER_PREF_CONN_PARAM:
                    {
                        APP_PRINT_INFO0("GATT_MSG_GAPS_CLIENT_READ_RESULT: prefer conn parameter");
                        APP_PRINT_INFO0("conn interval min 0x%x, conn interval max 0x%x, slave latecy 0x%x, sup timeout 0x%x",
                                        p_read->data.prefer_conn_param.conn_interval_min,
                                        p_read->data.prefer_conn_param.conn_interval_max,
                                        p_read->data.prefer_conn_param.slave_latency,
                                        p_read->data.prefer_conn_param.supervision_timeout);
                    }
                    break;

                case GATT_UUID_CHAR_CENTRAL_ADDRESS_RESOLUTION:
                    {
                        APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: central addr resolution 0x%x",
                                        p_read->data.central_addr_resolution);
                    }
                    break;

                case GATT_UUID_CHAR_RESOLVABLE_PRIVATE_ADDRESS_ONLY:
                    {
                        APP_PRINT_INFO1("GATT_MSG_GAPS_CLIENT_READ_RESULT: central addr resolution 0x%x",
                                        p_read->data.rpa_only);
                    }
                    break;

                default:
                    break;
                }
            }
        }
        ......

        return APP_RESULT_SUCCESS;
    }
  * \endcode
  */
bool gaps_client_read_char(uint16_t conn_handle, uint16_t char_uuid);

/**
  * @brief  Used by application, to write device name from server.
  * @param[in]  conn_handle  Connection handle of the ACL link.
  * @param[in]  name_len     Device name value length.
  * @param[in]  p_name       Device name value.
  * @retval true send write request to upper stack success.
  * @retval false send write request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(uint16_t conn_handle, uint8_t name_len, uint8_t *p_name)
    {
        if(!gaps_client_write_device_name(conn_handle, name_len, p_name))
        {
            APP_PRINT_ERROR0("write device name value fail!");
        }
    }

    T_APP_RESULT app_gaps_gatt_client_callback(uint16_t conn_handle, uint8_t type, void *p_data)
    {
        if (type == GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT)
        {
            T_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT *p_write = (T_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT *)p_data;
            APP_PRINT_INFO2("GATT_MSG_GAPS_CLIENT_WRITE_DEVICE_NAME_RESULT: conn_handle 0x%x, cause 0x%x",
                            conn_handle, p_write->cause);
        }
        ......

        return APP_RESULT_SUCCESS;
    }
  * \endcode
  */
bool gaps_client_write_device_name(uint16_t conn_handle, uint16_t name_len, uint8_t *p_name);

/**
  * @brief  Used by application, to write device appearance from server.
  * @param[in]  conn_handle  Connection handle of the ACL link.
  * @param[in]  appearance   Device appearance value.
  * @retval true send write request to upper stack success.
  * @retval false send write request to upper stack failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(uint16_t conn_handle, uint16_t appearance)
    {
        if(!gaps_client_write_appearance(conn_handle, appearance))
        {
            APP_PRINT_ERROR0("write device appearance value fail!");
        }
    }

    T_APP_RESULT app_gaps_gatt_client_callback(uint16_t conn_handle, uint8_t type, void *p_data)
    {
        if (type == GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT)
        {
            T_GAPS_CLIENT_WRITE_APPEARANCE_RESULT *p_write = (T_GAPS_CLIENT_WRITE_APPEARANCE_RESULT *)p_data;

            APP_PRINT_INFO2("GATT_MSG_GAPS_CLIENT_WRITE_APPEARANCE_RESULT: conn_handle 0x%x, cause 0x%x",
                            conn_handle, p_write->cause);
        }

        return APP_RESULT_SUCCESS;
    }
  * \endcode
  */
bool gaps_client_write_appearance(uint16_t conn_handle, uint16_t appearance);
/** @} End of GAPS_GATT_Client_Exported_Functions */
/** @} End of GAPS_GATT_Client */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _GAPS_GATT_CLIENT_H_ */
