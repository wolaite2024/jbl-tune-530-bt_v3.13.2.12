/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      power_debug.h
* @brief
* @details
* @author
* @date      2022-7-29
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _POWER_DEBUG_
#define _POWER_DEBUG_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    POWER_DEBUG_STAGE_BOOT,
    POWER_DEBUG_STAGE_A2DP_START,
    POWER_DEBUG_STAGE_A2DP_STOP,
    POWER_DEBUG_STAGE_HFP_START,
    POWER_DEBUG_STAGE_HFP_STOP,
} T_POWER_DEBUG_STAGE;

/**
 * @brief  Dump register for power test.
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 * @param  none
 * @retval none
*/
void power_test_dump_register(T_POWER_DEBUG_STAGE stage);

#ifdef __cplusplus
}
#endif

#endif /* _POWER_DEBUG_ */

/** @} */ /* End of group POWER_DEBUG */


/******************* (C) COPYRIGHT 2021 Realtek Semiconductor *****END OF FILE****/

