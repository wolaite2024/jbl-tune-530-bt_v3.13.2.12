/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_wdg.h
* @brief     header file of watch dog driver.
* @details
* @author    Lory_xu
* @date      2016-06-12
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876X_WDG_H_
#define _RTL876X_WDG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "wdg.h"

/** @addtogroup 87x3e_WATCH_DOG Watch Dog
  * @brief Watch dog module
  * @{
  */


/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup 87x3e_WATCH_DOG_Types Watch Dog Exported Types
  * @{
  */

/**
  * @brief   Watch Dog Mode structure definition
  */

typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t div_factor  : 16; //[15:0]  R/W Dividing factor. (at least set 1)
        uint32_t en_byte     : 8; //[23:16] R/WSet 0xA5 to enable watch dog timer
        uint32_t clear       : 1; //[24]    W     Write 1 to clear timer
        uint32_t cnt_limit   : 4; //[28:25] R/W   0: 0x001 1: 0x003 .... 10: 0x7FF 11~15: 0xFFF
        uint32_t wdg_mode    : 2; //[29:30] R/W
        uint32_t timeout     : 1; //[31]    R/W1C Watch dog timer timeout. 1 cycle pulse
    } b;
} T_WATCH_DOG_TIMER_REG;

typedef enum
{
    WDG_TYPE_DISABLE,
    WDG_TYPE_ENABLE,
    WDG_TYPE_RESTART,
} T_WDG_TYPE;

/** End of group 87x3e_WATCH_DOG_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup 87x3e_WATCH_DOG_Exported_Functions Watch Dog Exported Functions
  * @{
  */
/**
   * @brief  Watch Dog Clock Enable.
   */
void WDG_ClockEnable(void);
void WDG_Config(uint16_t div_factor, uint8_t  cnt_limit, T_WDG_MODE  wdg_mode);
bool WDG_Calculate_Config(uint32_t timeout_ms, uint16_t *div_factor, uint8_t *cnt_limit);
void WDG_Set_Core(T_WDG_TYPE type);
#define WDG_Enable()  WDG_Set_Core(WDG_TYPE_ENABLE)
bool WDG_Start_Core(uint32_t ms, T_WDG_MODE  wdg_mode);
void WDG_Disable_Core(void);
void WDG_Kick_Core(void);
/**
   * @brief  Watch Dog System Reset.
   * @param  wdg_mode @ref T_WDG_MODE
   */
void WDG_SystemReset(T_WDG_MODE wdg_mode);
void WDG_SystemReset_Dump(T_WDG_MODE wdg_mode, uint32_t dump_size);

/** @} */ /* End of group 87x3e_WATCH_DOG_Exported_Functions */
/** @} */ /* End of group 87x3e_WATCH_DOG */

#ifdef __cplusplus
}
#endif

#endif //_RTL876X_WDG_H_
