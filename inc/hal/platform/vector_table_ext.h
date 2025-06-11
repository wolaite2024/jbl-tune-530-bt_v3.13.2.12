/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     vector_table_ext.h
* @brief    Vector table extend implementaion header file
* @details
* @author   yuhungweng
* @date     2021-9-02
* @version  v1.0
*********************************************************************************************************
*/

#include "stdbool.h"
#include "vector_table.h"

/** @defgroup  HAL_VectorTable    Vector Table Ext
    * @brief simple implementation of VectorTable
    * @{
    */
/**
    * @brief    Update vector table in Ram
    * @param    v_num  vector type
    * @param    isr_handler isr execution function
    * @retval   true: successfully
    * @retval   otherwise fail
    * @note     Update isr execution function
    */
bool RamVectorTableUpdate(VECTORn_Type v_num, IRQ_Fun isr_handler);

/** @} */ /* End of group HAL_VectorTable */
