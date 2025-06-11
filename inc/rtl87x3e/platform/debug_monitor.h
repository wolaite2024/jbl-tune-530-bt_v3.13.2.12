/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    debug_monitor.h
  * @brief   Debug Monitor
  * @author  Henry_Huang
  * @date    2020-09-18
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __DEBUG_MONITOR_H
#define __DEBUG_MONITOR_H

/*============================================================================*
 *                               Header Files
*============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DWT Config*/
typedef enum
{
    BYTE  = 0,
    HALFWORD,
    WORD,
    WATCH_SIZE_MAX
} DWT_WATCH_SIZE;

typedef enum
{
    TRIGGER_ONLY = 0,
    DEBUG_EVENT,
    ACTION_MAX
} DWT_ACTION_TYPE;


typedef enum
{
    FUNCTION_DISABLED = 0,
    FUNCTION_INSTR_ADDR = 2,
    FUNCTION_INSTR_ADDR_LIM,
    FUNCTION_DADDR_RW,
    FUNCTION_DADDR_W,
    FUNCTION_DADDR_R,
    FUNCTION_DADDR_LIM,
    FUNCTION_DVAL_RW,
    FUNCTION_DVAL_W,
    FUNCTION_DVAL_R,
    FUNCTION_DVAL_LINK,
    FUNCTION_MAX
} DWT_FUNCTION_TYPE;

void enable_debug_monitor(void);

void watch_point_0_setting(uint32_t watch_address,
                           DWT_WATCH_SIZE watch_size,
                           DWT_ACTION_TYPE action_type,
                           DWT_FUNCTION_TYPE read_write_func);

void watch_point_1_setting(uint32_t watch_address,
                           DWT_WATCH_SIZE watch_size,
                           DWT_ACTION_TYPE action_type,
                           DWT_FUNCTION_TYPE read_write_func);


#endif  /* __DEBUG_MONITOR_H */
