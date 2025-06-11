/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    mem_config.h
  * @brief   Memory Configuration
  * @date    2017.6.6
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __MEM_CONFIG__
#define __MEM_CONFIG__

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup MEM_CONFIG Memory Configure
  * @brief Memory configuration for user application
  * @{
  */

/*============================================================================*
 *                        Constants
 *============================================================================*/
/** @defgroup MEM_CONFIG_Exported_Constant Memory Configure Exported Constants
  * @{
  */
/* --------------------CONFIG BY USER------------------------- */

/** @brief  following is for application define
    @note  SUM OF APP_GLOBAL_SIZE PLUS HEAP_DATA_OFF_SIZE SHOULD BE SMALLER THAN DTCM1_SIZE.
    APP_RAM_TEXT_SIZE SHOULD BE SMALLER THAN (ITCM1_END_ADDR - APP_RAM_TEXT_ADDR).
    */
#define APP_RAM_TEXT_SIZE           (4*1024)
#define APP_GLOBAL_SIZE             (12*1024)
#define HEAP_DATA_OFF_SIZE          (16 * 1024)

/* --------------------CONFIG BY USER -------------------------*/

/* --------------------The following macros should not be modified!------------------------- */
#define DATA_RAM_SIZE               (60*1024) // DTCM1_SIZE
#define CODE_RAM_SIZE               (116*1024) // ITCM1_SIZE

#define APP_TRACE_ADR               0x08800000
#define APP_TRACE_SIZE              (4*1024*1024)

/** @brief  data ram layout */
#define HEAP_DATA_ON_SIZE           (DATA_RAM_SIZE - APP_GLOBAL_SIZE - HEAP_DATA_OFF_SIZE)


#define APP_GLOBAL_ADDR             (0x2C0000) // DTCM1_ADDR
#define HEAP_DATA_ON_ADDR           (APP_GLOBAL_ADDR + APP_GLOBAL_SIZE)
#define HEAP_DATA_OFF_ADDR          (HEAP_DATA_ON_ADDR + HEAP_DATA_ON_SIZE)

/* Instruction RAM(ITCM1):    32K, Patch used only
example:
    1) patch code:            32K
*/

//(ITCM1) ram
#define PATCH_BUFFER_ADDR           (0x200000) // ITCM1_ADDR
#define PATCH_BUFFER_SIZE           (24*1024)
/** @brief  buffer ram start address */

#define APP_RAM_TEXT_ADDR           (PATCH_BUFFER_ADDR + PATCH_BUFFER_SIZE)

#define HEAP_ITCM1_ADDR             (APP_RAM_TEXT_ADDR + APP_RAM_TEXT_SIZE)

/** @brief  boundary check */

#if(( APP_GLOBAL_SIZE + HEAP_DATA_OFF_SIZE) > DATA_RAM_SIZE)
#error " SUM OF APP_GLOBAL_SIZE PLUSHEAP_DATA_OFF_SIZE SHOULD BE SMALLER THAN DTCM1_SIZE"
#endif

#if(( APP_RAM_TEXT_SIZE) > (PATCH_BUFFER_ADDR + CODE_RAM_SIZE - APP_RAM_TEXT_ADDR))
#error "APP_RAM_TEXT_SIZE SHOULD BE SMALLER THAN (ITCM1_END_ADDR - APP_RAM_TEXT_ADDR)"
#endif
/** @} */ /* End of group MEM_CONFIG_Exported_Constents */

#define DSP_SHM_GLOBAL_ADDR    0x536000
#define DSP_SHM_TOTAl_SIZE     80 * 1024
#define DSP_SHM_GLOBAL_SIZE    10 * 1024
#define DSP_SHM_HEAP_ADDR      DSP_SHM_GLOBAL_ADDR + DSP_SHM_GLOBAL_SIZE
#define DSP_SHM_HEAP_SIZE      (DSP_SHM_TOTAl_SIZE - DSP_SHM_GLOBAL_SIZE)


#ifdef __cplusplus
}
#endif


/** @} */ /* End of group MEM_CONFIG */

#endif

