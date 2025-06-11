/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    log_api.h
* @brief   This file provides log api wrapper for sdk customer.
* @author  Sandy
* @date    2021-05-20
* @version v1.0
* *************************************************************************************
*/

#ifndef __LOG_API_H_
#define __LOG_API_H_
#include "stdbool.h"
#include "stdint.h"
/** @defgroup  HAL_87x3e_LOG_API    log control api
    * @brief log control api
    * @{
    */
/** @defgroup HAL_87x3e_Log_Control_Exported_Functions log control Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  get the log enable status
    * @param void
    * @return the log enable status
    * @retval true   log enable
    * @retval false  log disable
    */
bool log_enable_get(void);
/**
    * @brief  set the log enable status
    * @param enable  log enable status, false: log disable, true: log enable
    * @return void
    */
void log_enable_set(bool enable);

/**
    * @brief  set the trace string log enable status
    * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
    * @warning This api is supported in the RTL87x3E and RTL87x3D.
    *          It is NOT supported in the RTL87x3G.
    * @param enable  whether to output trace string, false: don't output, true: output
    * @return void
    */
void log_enable_trace_string(bool enable);

/**
    * @brief  get the trace string log enable status
    * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
    * @warning This api is supported in the RTL87x3E and RTL87x3D.
    *          It is NOT supported in the RTL87x3G.
    * @param   void
    * @return  the status whether to output trace string, false: don't output, true: output
    */
bool log_enable_trace_string_get(void);
/**
    * @brief  enable LEVEL_CRITICAL log
    * @note   LEVEL_CRITICAL log is disabled by default. If LEVEL_CRITICAL log
    *         is enabled, it will be output ad LEVEL_INFO.
    *\xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
    * @warning This api is supported in the RTL87x3E and RTL87x3D.
    *          It is NOT supported in the RTL87x3G.
    * @param enable  whether to output LEVEL_CRITICAL log, false: don't output, true: output
    * @return void
    */
void log_enable_critical_level(bool enable);

/**
    * @brief  get the LEVEL_CRITICAL log enable status
    *\xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
    * @warning This api is supported in the RTL87x3E and RTL87x3D.
    *          It is NOT supported in the RTL87x3G.
    * @param void
    * @return the output status for LEVEL_CRITICAL log, true: output, false: don't output
    */
bool log_enable_critical_level_get(void);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_87x3e_Log_Control_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_LOG_API */
#endif

