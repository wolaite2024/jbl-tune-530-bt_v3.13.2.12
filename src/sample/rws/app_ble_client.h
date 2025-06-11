/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BLE_CLIENT_H_
#define _APP_BLE_CLIENT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_BLE_CLIENT App Ble Client
  * @brief App Ble Client
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_BLE_CLIENT_Exported_Macros App Ble Client Macros
   * @{
   */
/** max ANCS attribute length*/
#define ANCS_MAX_ATTR_LEN           256
/** End of APP_BLE_CLIENT_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_BLE_CLIENT_Exported_Types App Ble Client Types
    * @{
    */
/**  @brief  ANCS event id for the peer device notification */
typedef enum
{
    NS_EVENT_ID_NOTIFICATION_ADD = 0,
    NS_EVENT_ID_NOTIFICATION_MODIFIED = 1,
    NS_EVENT_ID_NOTIFICATION_REMOVED = 2,
    NS_EVENT_ID_NOTIFICATION_RESERVED = 0xff
} T_NS_EVENT_ID;

/**  @brief  ANCS event flag */
typedef enum
{
    NS_EVENT_FLAG_SILENT = (1 << 0),
    NS_EVENT_FLAG_IMPORTANT = (1 << 1),
    NS_EVENT_FLAG_PRE_EXISTING = (1 << 2),
    NS_EVENT_FLAG_POSITIVE_ACTION = (1 << 3),
    NS_EVENT_FLAG_NEGATIVE_ACTION = (1 << 4),
    NS_EVENT_FLAG_RESERVED = (1 << 7)
} T_NS_EVENT_FLAG;

/**  @brief  ANCS category id */
typedef enum
{
    NS_CATEGORY_ID_OTHER = 0,
    NS_CATEGORY_ID_INCOMING_CALL = 1,
    NS_CATEGORY_ID_MISSED_CALL = 2,
    NS_CATEGORY_ID_VOICE_MAIL = 3,
    NS_CATEGORY_ID_SOCIAL = 4,
    NS_CATEGORY_ID_SCHEDULE = 5,
    NS_CATEGORY_ID_EMAIL = 6,
    NS_CATEGORY_ID_NEWS = 7,
    NS_CATEGORY_ID_HEALTH_AND_FITNESS = 8,
    NS_CATEGORY_ID_BUSINESS_ADN_FINANCE = 9,
    NS_CATEGORY_ID_LOCATION = 10,
    NS_CATEGORY_ID_ENTERTAINMENT = 11,
    NS_CATEGORY_ID_RESERVED = 255
} T_NS_CATEGORY_ID;

/**  @brief  ANCS notification attribute id */
typedef enum
{
    DS_NOTIFICATION_ATTR_ID_APP_IDENTIFIER = 0,
    DS_NOTIFICATION_ATTR_ID_TITLE = 1,// (Needs to be followed by a 2-bytes max length parameter)
    DS_NOTIFICATION_ATTR_ID_SUB_TITLE = 2, //(Needs to be followed by a 2-bytes max length parameter)
    DS_NOTIFICATION_ATTR_ID_MESSAGE = 3, //(Needs to be followed by a 2-bytes max length parameter)
    DS_NOTIFICATION_ATTR_ID_MESSAGE_SIZE = 4,
    DS_NOTIFICATION_ATTR_ID_DATE = 5,
    DS_NOTIFICATION_ATTR_ID_POSITIVE_ACTION_LABEL = 6,
    DS_NOTIFICATION_ATTR_ID_NEGATIVE_ACTION_LABEL = 7,
    DS_NOTIFICATION_ATTR_ID_RESERVED = 255
} T_DS_NOTIFICATION_ATTR_ID;

/**  @brief  App parse ANCS notification attribute state */
typedef enum
{
    DS_PARSE_NOT_START = 0x00,
    DS_PARSE_GET_NOTIFICATION_COMMAND_ID = 0x01,
    DS_PARSE_UID1,
    DS_PARSE_UID2,
    DS_PARSE_UID3,
    DS_PARSE_UID4,
    DS_PARSE_ATTRIBUTE_ID,
    DS_PARSE_ATTRIBUTE_LEN1,
    DS_PARSE_ATTRIBUTE_LEN2,
    DS_PARSE_ATTRIBUTE_READY
} T_DS_NOTIFICATION_ATTR_PARSE_STATE;

/**  @brief  Smart phone App attribute id */
typedef enum
{
    DS_APP_ATTR_ID_DISPLAY_NAME = 0,
    DS_APP_ATTR_ID_RESERVED = 255
} T_DS_APP_ATTR_ID;

/**  @brief  App parse ANCS attribute state */
typedef enum
{
    DS_PARSE_GET_APP_COMMAND_ID = 0x10,
    DS_PARSE_APP_IDENTIFIER_START,
    DS_PARSE_APP_IDENTIFIER_END,
    DS_PARSE_APP_ATTRIBUTE_ID,
    DS_PARSE_APP_ATTRIBUTE_LEN1,
    DS_PARSE_APP_ATTRIBUTE_LEN2,
    DS_PARSE_APP_ATTRIBUTE_READY
} T_DS_APP_ATTR_PARSE_STATE;

/**  @brief  ANCS notification actions id state */
typedef enum
{
    /**A positive action exists and is associated with this iOS notification.*/
    CP_ACTION_ID_POSITIVE = 0,
    /**A negative action exists and is associated with this iOS notification.*/
    CP_ACTION_ID_NEGATIVE = 1,
    /**Action of reserved.*/
    CP_ACTION_ID_RESERVED  = 255
} T_CP_ACTION_ID_VALUES;

/**  @brief  Define notification source data for record ANCS notification parameters */
typedef struct
{
    /**This field informs the accessory whether the given iOS notification was added,
           modified, or removed*/
    uint8_t event_id;
    /**A bitmask whose set bits inform an NC of specificities with the iOS notification.*/
    uint8_t event_flags;
    /**A numerical value providing a category in which the iOS notification can be classified.*/
    uint8_t category_id;
    /**The current number of active iOS notifications in the given category.*/
    uint8_t category_count;
    /**The unique identifier (UID) for the iOS notification.*/
    uint32_t notification_uid;
} T_NS_DATA, *P_NS_DATA;

/**  @brief  Define notification attribute details data */
/**          App can acquire details information by attribute id */
typedef struct
{
    /** Should be set to 0*/
    uint8_t    command_id;
    /**
    The 32-bit numerical value representing the UID of the iOS notification for
    which the client wants information*/
    uint8_t    notification_uid[4];
    /** The attribute the NC wants to retrieve.*/
    uint8_t    attribute_id;
    /** The length of attribute.
          Some attributes may need to be followed by a 16-bit length parameter
          that specifies the maximum number of bytes of the attribute
          the NC wants to retrieve.*/
    uint16_t   attribute_len;
    /** The data of attribute */
    uint8_t    data[ANCS_MAX_ATTR_LEN];
} T_DS_NOTIFICATION_ATTR;

/**  @brief  Local app record notification attribute information */
typedef struct
{
    /**Should be set to 1*/
    uint8_t    command_id;
    /**An attribute is always a string whose length in bytes is provided
          in the tuple but that is not NULL-terminated. If a requested attribute
          is empty or missing for the app, its length is set to 0.
          The tuples are always in the same order as the AttributeIDs
          of the Get App Attributes command*/
    uint8_t    attribute_id;
    uint16_t   attribute_len;
    /**The data of app attribute*/
    uint8_t    data[ANCS_MAX_ATTR_LEN];
} T_DS_APP_ATTR;
/** End of APP_BLE_CLIENT_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_BLE_CLIENT_Exported_Functions App Ble Client Functions
    * @{
    */
/**
    * @brief  Ble Client module init
    * @param  void
    * @return void
    */
void app_ble_client_init(void);
/** @} */ /* End of group APP_BLE_CLIENT_Exported_Functions */


/** End of APP_BLE_CLIENT
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_BLE_CLIENT_H_ */
