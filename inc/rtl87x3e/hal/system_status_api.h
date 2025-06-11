/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    system_status_api.h
  * @brief   This file provides api wrapper for bbpro compatibility..
  * @author  sandy_jiang
  * @date    2018-11-29
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __SYSTEM_STATUS_API_H_
#define __SYSTEM_STATUS_API_H_


/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup HAL_87x3e_SYSTEM_STATUS_API System Status Api
  * @{
  */

/*============================================================================*
 *                              Variables
*============================================================================*/
/** @defgroup HAL_87x3e_SYSTEM_STATUS_API_Exported_Variables HAL SYSTEM STATUS Exported Variables
  * @{
  */


/** @} */ /* End of group HAL_87x3e_SYSTEM_STATUS_API_Exported_Variables */

/*============================================================================*
 *                              Functions
*============================================================================*/

/** @defgroup HAL_87x3e_SYSTEM_STATUS_API_Exported_Functions HAL SYSTEM STATUS Exported Variables
  * @{
  */
/**
    * @brief  get reset status to tell apart whether the mcu reboot from software reset or hardware reset
    * @param  none
    * @return true: reboot from software reset
    * @return false: reboot from hardware reset
    */
bool sys_hall_get_reset_status(void);
void sys_hall_get_power_down_info(void);
bool sys_hall_adp_read_adp_level(void);
void sys_hall_set_dsp_share_memory_80k(bool is_off_ram);
void sys_hall_btaon_fast_read_safe(uint16_t *input_info, uint16_t *output_info);


/**
    * @brief  store register value of aon register safely
    * @param  offset: offerset of aon register
    * @param  input_info: the value store to aon register
    * @return none
    */
void sys_hall_btaon_fast_write_safe(uint16_t offset, uint16_t *input_info);

/**
    * @brief  get package id of ic
    * @note   temporarily unavailable
    * @param  enable: enable or disable charger
    * @return none
    */
void sys_hall_charger_auto_enable(bool enable);
uint8_t sys_hall_read_package_id(void);
uint8_t sys_hall_read_chip_id(void);
uint8_t sys_hall_read_rom_version(void);
uint8_t *sys_hall_get_ic_euid(void);

/**
    * @brief  set rglx level of auxadc
    * @note   temporarily unavailable
    * @param  input_pin: the pin of auxadc to set rglx
    * @return none
    */
void sys_hall_set_rglx_auxadc(uint8_t input_pin);

/**
    * @brief  set hci download mode
    * @note   temporarily unavailable
    * @param  the pin of auxadc to set rglx
    * @return none
    */
void sys_hall_set_hci_download_mode(bool mode);

void sys_hall_upperstack_ini(uint8_t *upperstack_compile_stamp);

/**
    * @brief  enable or disable auto sleep in idle task
    * @note   temporarily unavailable
    * @param  flag: true or false to enable or disable
    * @return none
    */
void sys_hall_auto_sleep_in_idle(bool flag);

/**
    * @brief  get IC secure state
    * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Added API"
    * @param  void
    * @return true: secure enabled; false: secure disabled
    */
bool sys_hall_get_secure_state(void);
/** @} */ /* End of group HAL_87x3e_SYSTEM_STATUS_API_Exported_Functions */
/** End of HAL_87x3e_SYSTEM_STATUS_API
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
