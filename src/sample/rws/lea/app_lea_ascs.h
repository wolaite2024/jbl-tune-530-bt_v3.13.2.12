#ifndef _APP_ASCS_H_
#define _APP_ASCS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_isoch_def.h"
#include "codec_def.h"
#include "app_link_util.h"
#include "app_lea_ascs_def.h"

/** @defgroup APP_LEA_ASCS App LE Audio ASCS
  * @brief this file handle ASCS profile related process
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LEA_ASCS_Exported_Functions App ASCS Functions
    * @{
    */

/**
    * @brief  ascs module init. Register ble audio callback
    *         to handle ascs message.
    * @param  void
    * @return void
    */
void app_lea_ascs_init(void);

/**
    * @brief  Allocate ase entry by LE entry connection handle and update ase id, codec parameters
    * @param  conn_handle LE entry connection handle
    * @param  ase_id    audio stream endpoint identifier
    * @param  codec_id  codec identifier
    * @param  codec_cfg codec parameters
    * @return The status of allocate ase
    * @retval true  success
    * @retval false fail
    */
bool app_lea_ascs_alloc_ase_entry(uint16_t conn_handle, uint8_t ase_id,
                                  uint8_t codec_id[CODEC_ID_LEN], T_CODEC_CFG codec_cfg);

/**
    * @brief  Free ase entry
    * @param  p_ase_entry ase entry
    * @return The status of free ase
    * @retval true  success
    * @retval false fail
    */
bool app_lea_ascs_free_ase_entry(T_LEA_ASE_ENTRY *p_ase_entry);

/**
    * @brief  Get audio context from metadata
    * @param  p_data metadata buffer
    * @param  len    metadata length
    * @return audio context
    * @retval uint16_t
    */
uint16_t app_lea_ascs_get_context(uint8_t *p_data, uint16_t len);

/**
    * @brief  Find ase entry by path direction
    * @param  p_link the LE link
    * @param  path_direction the direction of path
    * @return ase entry
    * @retval \ref T_LEA_ASE_ENTRY
    */
T_LEA_ASE_ENTRY *app_lea_ascs_find_ase_entry_by_direction(T_APP_LE_LINK *p_link,
                                                          uint8_t path_direction);

/**
    * @brief  Find ase entry by ase type \ref T_LEA_ASE_TYPE when has LE audio entry
    * @param  para ase type
    * @param  conn_handle LE entry connection handle
    * @param  data
    * @return ase entry
    * @retval \ref T_LEA_ASE_ENTRY
    */
T_LEA_ASE_ENTRY *app_lea_ascs_find_ase_entry(T_LEA_ASE_TYPE para, uint16_t conn_handle, void *data);

/**
    * @brief  Find ase entry by ase type \ref T_LEA_ASE_TYPE when has not LE audio entry
    * @param  para ase type
    * @param  data
    * @param  idx LE entry index
    * @return ase entry
    * @retval \ref T_LEA_ASE_ENTRY
    */
T_LEA_ASE_ENTRY *app_lea_ascs_find_ase_entry_non_conn(T_LEA_ASE_TYPE para, void *data,
                                                      uint8_t *idx);

uint8_t app_lea_ascs_get_enable_ase_num(T_APP_LE_LINK *p_link, uint8_t direction, bool cis_exist);
/** @} */ /* End of group APP_LEA_ASCS_Exported_Functions */
/** End of APP_LEA_ASCS
* @}
*/

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
