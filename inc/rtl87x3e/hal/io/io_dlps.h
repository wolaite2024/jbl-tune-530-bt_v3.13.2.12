/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      io_dlps.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _IO_DLPS_H_
#define _IO_DLPS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler_abstraction.h"
#include "rtl876x.h"

extern void (*set_io_power_in_lps_mode)(bool);

/** @addtogroup 87x3e_IO_DLPS IO DLPS
  * @brief IO DLPS dirver module
  * @{
  */

/** @defgroup IO_DLPS_Exported_Types IO DLPS Exported Types
  * @{
  */

typedef void (*P_IO_DLPS_CALLBACK)(void);

/** End of group IO_DLPS_Exported_Types
  * @}
  */

/** @defgroup IO_DLPS_Exported_Constants IO DLPS Exported Constants
  * @{
  */

/** End of group IO_DLPS_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup IO_DLPS_Exported_Functions IO DLPS Exported Functions
  * @{
  */

/**
  * @brief  Register io restore function in dlps mode
  * @param  None
  * @retval None
  */
extern void io_dlps_register(void);

/**
  * @brief  Set IO power domain ON/OFF in DLPS mode
  * @note   IO power domain would change to _LQ, and certain IO(Sleep LED) would possible need more
            power which is not available under _LQ mode, and this API is used to keep the _HQ power
            even when under DLPS mode to make these IOs works as expected
  * @param  on: true to keep power on, false to let power off
  * @param  mode: DLPS mode, @ref LPSMode
  * @retval None
  */
static inline void io_dlps_set_vio_power(bool on)
{
    set_io_power_in_lps_mode(on);
}

/**
  * @brief  Rrgister user-defined exit dlps callback function
  * @param  func: user-defined callback functon.
  * @retval None
  */
extern void io_dlps_register_exit_cb(P_IO_DLPS_CALLBACK func);

/**
  * @brief  Rrgister user-defined enter dlps callback function
  * @param  func: user-defined callback functon.
  * @retval None
  */
extern void io_dlps_register_enter_cb(P_IO_DLPS_CALLBACK func);

#define DLPS_IORegister             io_dlps_register
#define DLPS_IORegUserDlpsExitCb    io_dlps_register_exit_cb
#define DLPS_IORegUserDlpsEnterCb   io_dlps_register_enter_cb
#define DLPS_IOSetPower             io_dlps_set_vio_power

#ifdef __cplusplus
}
#endif

#endif /* _IO_DLPS_H_ */

/** @} */ /* End of group IO_DLPS_Exported_Functions */
/** @} */ /* End of group 87x3e_IO_DLPS */


/******************* (C) COPYRIGHT 2022 Realtek Semiconductor *****END OF FILE****/

