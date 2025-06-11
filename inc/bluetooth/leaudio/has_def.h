/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     has_def.h
  * @brief    Head file for Hearing Aid Profile.
  * @details  This file defines Hearing Aid Profile related definition.
  * @author
  * @date
  * @version
  * *************************************************************************************
  */

#ifndef _HAS_DEF_H_
#define _HAS_DEF_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>

/**
 * \defgroup    LEA_USE_CASE_HAP_Def Hearing Aid Profile Definition
 *
 * \brief   Hearing Aid Profile related definition.
 */

/**
 * \defgroup HAP_Def_Exported_Macros Hearing Aid Profile Definition Exported Macros
 *
 * \ingroup LEA_USE_CASE_HAP_Def
 * \{
 */

/**
 * has_def.h
 *
 * \defgroup  HAS_UUID Hearing Aid Service UUID
 *
 * \brief  Define Hearing Aid Service UUID.
 *
 * \ingroup HAP_Def_Exported_Macros
 * \{
 */
#define GATT_UUID_HAS                              0x1854    /**< Hearing Aid Service. */

#define HAS_UUID_CHAR_HA_FEATURES                  0x2BDA    /**< Hearing Aid Features. */
#define HAS_UUID_CHAR_HA_PRESET_CP                 0x2BDB    /**< Hearing Aid Preset Control Point. */
#define HAS_UUID_CHAR_ACTIVE_PRESET_IDX            0x2BDC    /**< Active Preset Index. */
/**
 * End of HAS_UUID
 * \}
 */

/**
 * has_def.h
 *
 * \defgroup  HAS_ERR_CODE HAS Error Code
 *
 * \brief  Define Hearing Aid Service related error code.
 *
 * \ingroup HAP_Def_Exported_Macros
 * \{
 */
#define ATT_ERR_HAS_OP_INVALID_OPCODE                   (ATT_ERR | 0x80)  /**< Invalid Opcode. */
#define ATT_ERR_HAS_OP_WRITE_NAME_NOT_ALLOWED           (ATT_ERR | 0x81)  /**< Write Name Not Allowed. */
#define ATT_ERR_HAS_OP_PRESET_SYNC_NOT_SUPPORT          (ATT_ERR | 0x82)  /**< Preset Synchronization Not Supported. */
#define ATT_ERR_HAS_OP_PRESET_OPERATION_NOT_POSSIBLE    (ATT_ERR | 0x83)  /**< Preset Operation Not Possible. */
#define ATT_ERR_HAS_OP_PRESET_NAME_TOO_LONG             (ATT_ERR | 0x84)  /**< Invalid Parameters Length. */
/**
 * End of HAS_ERR_CODE
 * \}
 */

/**
 * has_def.h
 *
 * \defgroup  HA_TYPE Hearing Aid Types
 *
 * \brief  Define Hearing Aid Types bit mask.
 *
 * \ingroup HAP_Def_Exported_Macros
 * \{
 */
#define HA_FEATURES_BINAURAL_HA_MASK                    0x00
#define HA_FEATURES_MONAURAL_HA_MASK                    0x01
#define HA_FEATURES_BANDED_HA_MASK                      0x02
#define HA_FEATURES_HA_TYPE_RFU_MASK                    0x03
/**
 * End of HA_TYPE
 * \}
 */

/**
 * has_def.h
 *
 * \defgroup  PRESET_RECORD_PROP Preset Record Properties
 *
 * \brief  Define preset record properties bit mask.
 *
 * \ingroup HAP_Def_Exported_Macros
 * \{
 */
#define HAS_PRESET_PROPERTIES_WRITABLE                  ((uint8_t)0x01)
#define HAS_PRESET_PROPERTIES_AVAILABLE                 ((uint8_t)0x02)
/**
 * End of PRESET_RECORD_PROP
 * \}
 */

/**
 * has_def.h
 *
 * \defgroup  PRESET_NAME_LEN Preset Name Length Range
 *
 * \brief  Define preset record name length range.
 *
 * \ingroup HAP_Def_Exported_Macros
 * \{
 */
#define HAS_MIN_PRESET_NAME_LENGTH          1
#define HAS_MAX_PRESET_NAME_LENGTH          40
/**
 * End of PRESET_NAME_LEN
 * \}
 */

/**
 * has_def.h
 *
 * \brief  Define maximum preset record index.
 *
 * \ingroup HAP_Def_Exported_Macros
 */
#define HAS_MAX_PRESET_IDX                  0xFF
/**
 * End of HAP_Def_Exported_Macros
 * \}
 */

/**
 * \defgroup HAP_Def_Exported_Types Hearing Aid Profile Definition Exported Types
 *
 * \ingroup LEA_USE_CASE_HAP_Def
 * \{
 */

/**
 * has_def.h
 *
 * \brief  HAS Control Point Opcode.
 *
 * \ingroup HAP_Def_Exported_Types
 */
typedef enum
{
    HAS_CP_OP_RESERVED                       = 0x00,  /**< RFU. */
    HAS_CP_OP_READ_PRESETS_REQ               = 0x01,  /**< Read Presets Request. */
    HAS_CP_OP_READ_PRESET_RSP                = 0x02,  /**< Read Preset Response. */
    HAS_CP_OP_PRESET_CHANGED                 = 0x03,  /**< Preset Changed. */
    HAS_CP_OP_WRITE_PRESET_NAME              = 0x04,  /**< Write Preset Name. */
    HAS_CP_OP_SET_ACTIVE_PRESET              = 0x05,  /**< Set Active Preset. */
    HAS_CP_OP_SET_NEXT_PRESET                = 0x06,  /**< Set Next Preset. */
    HAS_CP_OP_SET_PREVIOUS_PRESET            = 0x07,  /**< Set Previous Preset. */
    HAS_CP_OP_SET_ACTIVE_PRESET_SYNC_LOCAL   = 0x08,  /**< Set Active Preset Synchronized Locally. */
    HAS_CP_OP_SET_NEXT_PRESET_SYNC_LOCAL     = 0x09,  /**< Set Next Preset Synchronized Locally. */
    HAS_CP_OP_SET_PREVIOUS_PRESET_SYNC_LOCAL = 0x0A,  /**< Set Previous Preset Synchronized Locally. */
} T_HAS_CP_OP;

/**
 * has_def.h
 *
 * \brief  Format of Preset Record.
 *
 * \ingroup HAP_Def_Exported_Types
 */
typedef struct
{
    uint8_t index;
    uint8_t properties;
    char *p_name;
} T_HAS_PRESET_FORMAT;

/**
 * has_def.h
 *
 * \brief  Preset Record Change ID.
 *
 * \ingroup HAP_Def_Exported_Types
 */
typedef enum
{
    GENERIC_UPDATE = 0x00,
    PRESET_RECORD_DELETED = 0x01,
    PRESET_RECORD_AVAILABLE = 0x02,
    PRESET_RECORD_UNAVAILABLE = 0x03
} T_HAS_PRESET_CHANGE_ID;

/**
 * has_def.h
 *
 * \brief  Hearing Aid Features.
 *
 * \ingroup HAP_Def_Exported_Types
 */
typedef struct
{
    uint8_t hearing_aid_type: 2;
    uint8_t preset_sync_support: 1;
    uint8_t independent_preset: 1;
    uint8_t dynamic_preset: 1;
    uint8_t writable_preset_support: 1;
    uint8_t rfu: 2;
} T_HAS_HA_FEATURES;
/**
 * End of HAP_Def_Exported_Types
 * \}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
