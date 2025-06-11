#ifndef _APP_DULT_SVC_H_
#define _APP_DULT_SVC_H_

#include "stdint.h"
#include "stdbool.h"
#include "bt_gatt_svc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup DULT DULT
  * @brief
  * @{
  */

/** @defgroup APP_DULT DULT
  * @brief Detect Unwanted Location Tracker
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup DULT_Exported_Macros DULT Macros
    * @{
    */
#define DULT_PROTOCAL_VERSION                  0x00010000

#define DULT_CHAR_NONE_OWNER_INDEX        0x02
#define DULT_CHAR_NONE_OWNER_CCCD_INDEX   (DULT_CHAR_NONE_OWNER_INDEX + 1)

#define DULT_SRV_UUID                     0xFCB2

typedef enum
{
    DULT_SEPERATED,
    DULT_NEARING_OWNER,
} T_DULT_NEAR_OWNER_STATE;

typedef enum
{
    DULT_OPCODE_INFO_GET_PRODUCT_DATA           = 0x0003,
    DULT_OPCODE_INFO_GET_PRODUCT_DATA_RSP       = 0x0803,

    DULT_OPCODE_INFO_GET_MANUFT_NAME            = 0x0004,
    DULT_OPCODE_INFO_GET_MANUFT_NAME_RSP        = 0x0804,

    DULT_OPCODE_INFO_GET_MODEL_NAME             = 0x0005,
    DULT_OPCODE_INFO_GET_MODEL_NAME_RSP         = 0x0805,

    DULT_OPCODE_INFO_GET_ACCESSORY_CATEGORY     = 0x0006,
    DULT_OPCODE_INFO_GET_ACCESSORY_CATEGORY_RSP = 0x0806,

    DULT_OPCODE_INFO_GET_PROTOCAL_VER           = 0x0007,
    DULT_OPCODE_INFO_GET_PROTOCAL_VER_RSP       = 0x0807,

    DULT_OPCODE_INFO_GET_ACCESSORY_CAP          = 0x0008,
    DULT_OPCODE_INFO_GET_ACCESSORY_CAP_RSP      = 0x0808,

    DULT_OPCODE_INFO_GET_NETWORK_ID             = 0x0009,
    DULT_OPCODE_INFO_GET_NETWORK_ID_RSP         = 0x0809,

    DULT_OPCODE_INFO_GET_FIRMWARE_VER           = 0x000A,
    DULT_OPCODE_INFO_GET_FIRMWARE_VER_RSP       = 0x080A,

    DULT_OPCODE_INFO_GET_BATT_TYPE              = 0x000B,
    DULT_OPCODE_INFO_GET_BATT_TYPE_RSP          = 0x080B,

    DULT_OPCODE_INFO_GET_BATT_LEVEL             = 0x000C,
    DULT_OPCODE_INFO_GET_BATT_LEVEL_RSP         = 0x080C,

    DULT_OPCODE_INFO_MAX                        = 0x005F,
} T_DULT_OPCODE_INFO;

typedef enum
{
    DULT_OPCODE_CTRL_SOUND_START                = 0x0300,
    DULT_OPCODE_CTRL_SOUND_STOP                 = 0x0301,
    DULT_OPCODE_CTRL_CMD_RSP                    = 0x0302,
    DULT_OPCODE_CTRL_SOUND_COMPLETE             = 0x0303,
    DULT_OPCODE_CTRL_MAX                        = 0x035F,

    DULT_OPCODE_CTRL_GET_IDENTIFIER             = 0x0404,
    DULT_OPCODE_CTRL_GET_IDENTIFIER_RSP         = 0x0405,
} T_DULT_OPCODE_CTRL;

typedef enum
{
    DULT_CMD_RSP_STATE_SUCCESS                  = 0x0000,
    DULT_CMD_RSP_STATE_INVALID_STATE            = 0x0001,
    DULT_CMD_RSP_STATE_INVALID_CONFIG           = 0x0002,
    DULT_CMD_RSP_STATE_INVALID_LENGTH           = 0x0003,
    DULT_CMD_RSP_STATE_INVALID_PARAM            = 0x0004,
    DULT_CMD_RSP_STATE_INVALID_CMD              = 0xFFFF,
} T_DULT_CMD_RSP_STATE;

/** End of DULT_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup DULT_Exported_Types DULT Types
    * @{
    */

typedef struct
{
    uint16_t opcode;
    uint8_t  operands_len;
    uint8_t  *p_operands;
} T_DULT_UPSTREAM_MSG_DATA;

typedef struct
{
    uint8_t                     conn_handle;
    uint16_t                    cid;
    T_SERVICE_CALLBACK_TYPE     msg_type;
    T_DULT_UPSTREAM_MSG_DATA    msg_data;
} T_DULT_CALLBACK_DATA;

/** End of DULT_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup DULT_Exported_Functions Ble Adv Functions
    * @{
    */
T_SERVER_ID app_dult_add_service(void *p_func);

bool app_dult_send_indication(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint8_t *p_data, uint16_t data_len);

bool app_dult_report_cmd_rsp(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                             uint16_t opcode, T_DULT_CMD_RSP_STATE state);

/** @} */ /* End of group DULT_Exported_Functions */
/** End of APP_DULT
* @}
*/

/** @} End of DULT */

#ifdef __cplusplus
}
#endif

#endif
