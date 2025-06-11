/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    section.h
  * @brief
  * @author
  * @date    2017.6.7
  * @version v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/
#ifndef SECTION_H
#define SECTION_H


/** @defgroup 87x3e_SECTION
  * @brief memory section definition for user application.
  * @{
  */

/*============================================================================*
 *                              Macro
*============================================================================*/
/** @defgroup 87x3e_SECTION_Exported_Macros Section Sets Exported Macros
    * @{
    */
#define FLASH_HEADER     __attribute__((section(".flash.header")))     __attribute__((used))
#define FLASH_HEADER_EXT __attribute__((section(".flash.header_ext"))) __attribute__((used))
#define RAM_TEXT_SECTION __attribute__((section(".ram_text")))
#define SHM_DATA_SECTION __attribute__((section(".shm.data")))
#define ISR_TEXT_SECTION __attribute__((section(".isr.text"))) /*not very urgent isr*/
#define BUFFER_RAM_DATA_SECTION __attribute__((section(".buffer_ram_data")))

#define RAM_DATAON_RANDOM_SECTION     __attribute__((section(".ram.dataon.random")))
/** @} */
/** @} */ /* End of group 87x3e_SECTION */



#endif // SECTION_H
