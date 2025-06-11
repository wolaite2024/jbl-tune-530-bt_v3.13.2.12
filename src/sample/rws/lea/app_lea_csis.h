#ifndef _APP_CSIS_H_
#define _APP_CSIS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "csis_def.h"
#include "cap.h"
/** @defgroup APP_LEA_CSIS App LE Audio CSIS
  * @brief this file handle CSIS profile related process
  * @{
  */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_LEA_CSIS_Exported_Functions App CSIS Functions
    * @{
    */

/**
    * @brief  Update SIRK(Set Identity Resolving Key)
     * @param No parameter.
     * @return void
    */
void app_lea_csis_update_sirk(void);

/**
    * @brief  Generate Identity Resolving Key(SIRK)
     * @param No parameter.
     * @return void
    */
void app_lea_csis_gen_sirk(void);

/**
     * @brief csis init Paramer assigned by lea profile init.
     * @param p_param \ref T_CAP_INIT_PARAMS
     * @return  void
     */
void app_lea_csis_init(T_CAP_INIT_PARAMS *p_param);

/** @} */ /* End of group APP_LEA_CSIS_Exported_Functions */
/** End of APP_LEA_CSIS
* @}
*/
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
