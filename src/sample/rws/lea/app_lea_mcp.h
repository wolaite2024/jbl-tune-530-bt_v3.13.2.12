#ifndef _APP_MCP_H_
#define _APP_MCP_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_audio.h"
#include "mcp_def.h"

/** @defgroup APP_LEA_MCP App LE Audio MCP
  * @brief this file handle MCP profile related process
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LEA_MCP_Exported_Functions App LE Audio MCP Functions
    * @{
    */

/**
    * @brief  mcp module init. Register ble audio callback
    *         to handle mcp message.
    * @param  void
    * @return void
    */
void app_lea_mcp_init(void);

/**
    * @brief  Get mcp active link connection handle
    * @param  void
    * @return LE link connection handle
    * @retval uint16_t
    */
uint16_t app_lea_mcp_get_active_conn_handle(void);

/**
    * @brief  Set mcp active link connection handle
    * @param  conn_handle LE link connection handle
    * @return The status of set connection handle
    * @retval true  success
    * @retval false fail
    */
bool app_lea_mcp_set_active_conn_handle(uint16_t conn_handle);

/**
    * @brief  Reset mcp active link connection handle
    * @param  void
    * @return void
    */
void app_lea_mcp_reset_active_conn_handle(void);
void app_lea_mcp_play_pause(uint16_t conn_handle, bool play);
/** @} */ /* End of group APP_LEA_MCP_Exported_Functions */
/** End of APP_LEA_MCP
* @}
*/

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
