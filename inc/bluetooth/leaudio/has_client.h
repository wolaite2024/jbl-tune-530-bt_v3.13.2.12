/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     has_client.h
  * @brief    Head file for Hearing Aid Client.
  * @details  This file defines Hearing Aid Client related API.
  * @author
  * @date
  * @version
  * *************************************************************************************
  */

#ifndef _HAS_CLIENT_H_
#define _HAS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdbool.h>
#include "has_def.h"

/**
 * \defgroup    LEA_USE_CASE_HAP_Client Hearing Aid Client
 *
 * \brief   Control and interact with hearing aid
 */

/**
 * \defgroup HAP_Client_Exported_Types Hearing Aid Client Exported Types
 *
 * \ingroup LEA_USE_CASE_HAP_Client
 * \{
 */

/**
 * has_client.h
 *
 * \brief  HAS CCCD Configuration Opcode.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef enum
{
    HAS_OP_ENABLE_ALL = 0x00
} T_HAS_CCCD_OP_TYPE;

/**
 * has_client.h
 *
 * \brief  HAS Client Discover HAS Service Result.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_DIS_DONE.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    bool     is_found;       /**< Whether to find service. */
    bool     load_from_ftl;  /**< Whether the service table is loaded from FTL. */
    bool     is_ha_cp_exist;
    bool     is_ha_cp_notify;
    bool     is_ha_features_notify_support;
} T_HAS_CLIENT_DIS_DONE;

/**
 * has_client.h
 *
 * \brief  HAS Client Read Hearing Aid Features Result.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_READ_HA_FEATURES_RESULT.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cause;
    T_HAS_HA_FEATURES has_feature;
} T_HAS_CLIENT_READ_HA_FEATURES_RESULT;

/**
 * has_client.h
 *
 * \brief  HAS Client Read Active Preset Index Result.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_READ_ACTIVE_PRESET_IDX_RESULT.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cause;
    uint8_t active_preset_idx;
} T_HAS_CLIENT_READ_ACTIVE_PRESET_IDX_RESULT;

/**
 * has_client.h
 *
 * \brief  HAS Client Write Control Point Result.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_CP_RESULT.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cause;
} T_HAS_CLIENT_CP_RESULT;

/**
 * has_client.h
 *
 * \brief  HAS Client Receive Hearing Aid Features Notify Data.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_HA_FEATURES_NOTIFY.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    T_HAS_HA_FEATURES has_feature;
} T_HAS_CLIENT_HA_FEATURES_NOTIFY;

/**
 * has_client.h
 *
 * \brief  HAS Client Receive Active Preset Index Notify Data.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_ACTIVE_PRESET_IDX_NOTIFY.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t active_preset_idx;
} T_HAS_CLIENT_ACTIVE_PRESET_IDX_NOTIFY;

/**
 * has_client.h
 *
 * \brief  Control Point Notify or Indication Data.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    T_HAS_CP_OP cp_op;
    uint8_t change_id;
    uint8_t is_last;
    uint8_t pre_idx;
    uint8_t name_length;
    T_HAS_PRESET_FORMAT preset;
} T_HAS_CP_NOTIFY_IND_DATA;

/**
 * has_client.h
 *
 * \brief  HAS Client Receive Control Point Notify or Indication Data.
 *         The message data for LE_AUDIO_MSG_HAS_CLIENT_CP_NOTIFY_IND_DATA.
 *
 * \ingroup HAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    bool notify;
    T_HAS_CP_NOTIFY_IND_DATA cp_data;
} T_HAS_CLIENT_CP_NOTIFY_IND_DATA;
/**
 * End of HAP_Client_Exported_Types
 * \}
 */

/**
 * \defgroup HAP_Client_Exported_Functions Hearing Aid Client Exported Functions
 *
 * \ingroup LEA_USE_CASE_HAP_Client
 * \{
 */

/**
 * has_client.h
 *
 * \brief  Initialize HAS client.
 *
 * \return         The result of initialize HAS client.
 * \retval true    Initialize HAS client success.
 * \retval false   Initialize HAS client failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_client_init(void);

/**
 * has_client.h
 *
 * \brief  Configure HAS CCCD.
 *
 * \param[in]  cccd_type      HAS cccd operation type: @ref T_HAS_CCCD_OP_TYPE.
 * \param[in]  conn_handle    Connection handle.
 *
 * \return         The result of configure HAS CCCD.
 * \retval true    Configure HAS CCCD success.
 * \retval false   Configure HAS CCCD failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
void has_cfg_cccd(T_HAS_CCCD_OP_TYPE cccd_type, uint16_t conn_handle);

/**
 * has_client.h
 *
 * \brief  Read hearing aid features.
 *
 * \param[in]  conn_handle    Connection handle.
 *
 * \return         The result of read hearing aid features.
 * \retval true    Read hearing aid features success.
 * \retval false   Read hearing aid features failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_read_ha_features(uint16_t conn_handle);

/**
 * has_client.h
 *
 * \brief  Read active preset index.
 *
 * \param[in]  conn_handle     Connection handle.
 *
 * \return         The result of read active preset index.
 * \retval true    Read active preset index success.
 * \retval false   Read active preset index failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_read_active_preset_idx(uint16_t conn_handle);

/**
 * has_client.h
 *
 * \brief  Write read presets control point.
 *
 * \param[in]  conn_handle        Connection handle.
 * \param[in]  start_preset_idx   The start preset index to read.
 * \param[in]  preset_num         The preset number to read.
 *
 * \return         The result of write read presets control point.
 * \retval true    Write read presets control point success.
 * \retval false   Write read presets control point failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_cp_read_presets(uint16_t conn_handle, uint8_t start_preset_idx, uint8_t preset_num);

/**
 * has_client.h
 *
 * \brief  Send write preset name control point.
 *
 * \param[in]  conn_handle     Connection handle.
 * \param[in]  preset_idx      Preset index.
 * \param[in]  name_len        Preset name length.
 * \param[in]  p_name          Pointer to preset name.
 *
 * \return         The result of send write preset name control point.
 * \retval true    Send write preset name control point success.
 * \retval false   Send write preset name control point failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_cp_write_preset_name(uint16_t conn_handle, uint8_t preset_idx,
                              uint8_t name_len, char *p_name);

/**
 * has_client.h
 *
 * \brief  Write set active preset control point.
 *
 * \param[in]  conn_handle     Connection handle.
 * \param[in]  preset_idx      Preset index.
 * \param[in]  is_sync_local   Wether the opcode is sync locally.
 * \arg    true  : The control point opcode is @ref HAS_CP_OP_SET_ACTIVE_PRESET_SYNC_LOCAL.
 * \arg    false : The control point opcode is @ref HAS_CP_OP_SET_ACTIVE_PRESET.
 *
 * \return         The result of write set active preset control point.
 * \retval true    Write set active preset control point success.
 * \retval false   Write set active preset control point failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_cp_set_active_preset(uint16_t conn_handle, uint8_t preset_idx, bool is_sync_local);

/**
 * has_client.h
 *
 * \brief  Write set next active preset control point.
 *
 * \param[in]  conn_handle     Connection handle.
 * \param[in]  is_sync_local   Wether the opcode is sync locally.
 * \arg    true  : The control point opcode is @ref HAS_CP_OP_SET_NEXT_PRESET_SYNC_LOCAL.
 * \arg    false : The control point opcode is @ref HAS_CP_OP_SET_NEXT_PRESET.
 *
 * \return         The result of write set next active preset control point.
 * \retval true    Write set next active preset control point success.
 * \retval false   Write set next active preset control point failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_cp_set_next_preset(uint16_t conn_handle, bool is_sync_local);

/**
 * has_client.h
 *
 * \brief  Write set previous active preset control point.
 *
 * \param[in]  conn_handle     Connection handle.
 * \param[in]  is_sync_local   Wether the opcode is sync locally.
 * \arg    true  : The control point opcode is @ref HAS_CP_OP_SET_PREVIOUS_PRESET_SYNC_LOCAL.
 * \arg    false : The control point opcode is @ref HAS_CP_OP_SET_PREVIOUS_PRESET.
 *
 * \return         The result of write set previous active preset control point.
 * \retval true    Write set previous active preset control point success.
 * \retval false   Write set previous active preset control point failed.
 *
 * \ingroup HAP_Client_Exported_Functions
 */
bool has_cp_set_previous_preset(uint16_t conn_handle, bool is_sync_local);
/**
 * End of HAP_Client_Exported_Functions
 * \}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
