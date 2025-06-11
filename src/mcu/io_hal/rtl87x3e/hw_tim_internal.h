/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      hw_tim_internal.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _HW_TIM_INTERNAL_
#define _HW_TIM_INTERNAL_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"
#include "vector_table.h"
#include "rtl876x.h"

/** @addtogroup 87x3e_HW_TIM_INTERNAL HW TIM INTERNAL
  * @brief HW TIM driver module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup HW_TIM_INTERNAL_Exported_Types HW TIM INTERNAL Exported Types
  * @{
  */
#define HW_TIMER_DYNAMIC_MASK          (BIT6 | BIT5 | BIT4 | BIT3 | BIT2)   /* Not used in ROM */
#define HW_TIMER_MASK                  (0xFC)                /* TIM2~TIM11, TIM0/TIM1 reserved for lowstack, not handled in driver */

#define TOTAL_HW_TIMER_NUM              (8)

#define hw_timer_get_reg_base_from_id(id)       (TIM_TypeDef *)(TIM0_REG_BASE + 0x14 * id)
#define hw_timer_get_pwm_func(id)               (TIMER_PWM0 + id)

typedef enum
{
    HW_TIMER_ID_EMPTY,
    HW_TIMER_ID_1,
    HW_TIMER_ID_2,
    HW_TIMER_ID_3,
    HW_TIMER_ID_4,
    HW_TIMER_ID_5,
    HW_TIMER_ID_6,
    HW_TIMER_ID_7,
    HW_TIMER_ID_MAX,
} T_HW_TIMER_ID;

#define  hw_timer_get_isr(id)  hal_timer_handler

#define count_1bits(x) (uint8_t)__builtin_popcount(x)

/** End of group HW_TIM_INTERNAL_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup HW_TIM_INTERNAL_Exported_Functions HW TIM INTERNAL Exported Functions
  * @{
  */
extern void hw_timer_handler_imp(T_HW_TIMER_ID id);

extern void hal_timer_handler(void);

extern bool RamVectorTableUpdate_for_xip_isr(VECTORn_Type v_num, IRQ_Fun isr_handler);
extern bool RamVectorTableUpdate_rom(VECTORn_Type v_num, IRQ_Fun isr_handler);
extern void hw_timer_rcc_clock_cmd(T_HW_TIMER_ID id);
extern VECTORn_Type hw_timer_get_vectorn_from_id(T_HW_TIMER_ID id);
extern IRQn_Type hw_timer_get_irqn_from_id(T_HW_TIMER_ID id);
extern VECTORn_Type hw_timer_get_vectorn_from_id(T_HW_TIMER_ID id);
extern IRQn_Type hw_timer_get_irqn_from_id(T_HW_TIMER_ID id);
extern uint8_t pwm_src_div_mapping(T_PWM_CLOCK_SOURCE source);

#ifdef __cplusplus
}
#endif

#endif /* _HW_TIM_INTERNAL_ */

/** @} */ /* End of group HW_TIM_INTERNAL_Exported_Functions */
/** @} */ /* End of group 87x3e_HW_TIM_INTERNAL */

/******************* (C) COPYRIGHT 2021 Realtek Semiconductor Corporation *****END OF FILE****/

