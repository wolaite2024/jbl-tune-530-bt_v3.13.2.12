/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      io_assert.h
* @brief
* @details
* @author    elliot chen
* @date      2016-11-29
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __IO_ASSERT_H__
#define __IO_ASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "rtl876x.h"

/**
* @brief  IO Parameter check fail.
* @param  file __FILE__
* @param  line __LINE__
                    call as follow io_assert_failed((uint8_t *)__FILE__, __LINE__).
* @return  void
*/
void io_assert_failed(uint8_t *file, uint32_t line);

/**
* @brief  IO Parameter check report.
* @param  file __FILE__
* @param  line __LINE__
                    call as follow io_assert_report((uint8_t *)__FILE__, __LINE__).
* @return  void
*/
void io_assert_report(uint8_t *file, uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /*__IO_ASSERT_H__*/

/** @} */ /* End of group LPC_Exported_Functions */
/** @} */ /* End of group LPC */
