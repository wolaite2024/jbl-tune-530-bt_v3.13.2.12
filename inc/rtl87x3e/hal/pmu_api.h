/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     pmu_api.c
* @brief    This file provides pmu api wrapper for sdk customer.
* @details
* @author   mj_mengjie_han
* @date     2021-04-07
* @version  v1.0
* *************************************************************************************/
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __PMU_API_H_
#define __PMU_API_H_


/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif
void pmu_set_clk_32k_power_in_powerdown(bool para);

#ifdef __cplusplus
}
#endif

#endif
