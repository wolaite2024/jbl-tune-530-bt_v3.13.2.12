/**
*********************************************************************************************************
*               Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file         platform_rtc.h
* @brief        Platfrom RTC implementation head file.
* @details
* @author       Po Yu Chen
* @date         2020-12-15
* @version      v0.1
*********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __PLATFORM_RTC_H
#define __PLATFORM_RTC_H

/*============================================================================*
 *                               Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup  87x3e_PLATFORM_RTC  Platform RTC
    * @brief
    * @{
    */

#define PF_RTC_BIT_NUM              BIT64(24)

/* base address = RTC_REG_BASE */
/* PF_RTC offset*/
#define PF_RTC_INT_CLR_REG          0x4
#define PF_RTC_INT_SR_REG           0x8
#define PF_RTC_CTRL_REG             0x40
#define PF_RTC_COMP0_REG            0x44
#define PF_RTC_COMP1_REG            0x48
#define PF_RTC_COMP0_GT_REG         0x4C
#define PF_RTC_COMP1_GT_REG         0x50
#define PF_RTC_BACKUP_REG           0x54
#define PF_RTC_CNT_REG              0x58
/* RTC register RW */
#define RTC_FAST_WDATA              0xF0
#define RTC_FAST_ADDR               0xF4
#define RTC_WR_STROBE               0xF8

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup 87x3e_PLATFORM_RTC_Exported_Types Platform RTC Exported Types
  * @{
  */

enum
{
    PF_RTC_COMP0,
    PF_RTC_COMP1,
    PF_RTC_COMP0_GT,
    PF_RTC_COMP1_GT,
};

/* 0x104 RTC_PF_INTERRUPT_CLEAR_REGISTER */
typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t rsvd0:             16;
        uint32_t cmp_gt_clr:        2;
        uint32_t cmp_nv_clr:        2;
        uint32_t cmp_wk_clr:        2;
        uint32_t rsvd:              10;
    };
} PF_RTC_INT_CLR_TYPE;

/* 0x108 RTC_PF_INTERRUPT_STATUS_REGISTER */
typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t rsvd0:             16;
        uint32_t cmp_gt_sr:         2;
        uint32_t cmp_nv_sr:         2;
        uint32_t cmp_wk_sr:         2;
        uint32_t rsvd:              10;
    };
} PF_RTC_INT_SR_TYPE;

/* 0x140 RTC_PF_CONTROL_REGISTER */
typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t cnt_start:         1;
        uint32_t cnt_rst:           1;
        uint32_t rsvd0:             10;
        uint32_t cmp_gt_ie:         2;
        uint32_t cmp_nv_ie:         2;
        uint32_t cmp_wk_ie:         2;
        uint32_t rsvd1:             11;
        uint32_t wk_ie:             1;
        uint32_t nv_ie:             1;
        uint32_t rsvd2:             1;
    };
} PF_RTC_CTRL_TYPE;

/* 0x154 RTC_PF_BACKUP_REGISTER */
typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t rsvd:              30;
        uint32_t intr:              2;
    };
} PF_RTC_BACKUP_TYPE;

/** @} */ /* End of group 87x3e_PLATFORM_RTC_Exported_Types */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup 87x3e_PLATFORM_RTC_Exported_Functions Platform RTC Exported Functions
  * @{
  */
void platform_rtc_run_cmd(FunctionalState new_state);

void platform_rtc_system_wakeup_config(FunctionalState new_state);

void platform_rtc_nmi_enable(FunctionalState new_state);

void platform_rtc_comp_config(uint32_t intr_bitmap, uint32_t wkup_bitmap, FunctionalState NewState);

void platform_rtc_set_comp(uint8_t index, uint32_t value);

void platform_rtc_initiate_interrupt(uint32_t initiate_bitmap);

uint32_t platform_rtc_get_counter(void);

uint8_t platform_rtc_get_interrupt_status(void);

void platform_rtc_clear_status(uint32_t initiate_bitmap, uint32_t intr_bitmap,
                               uint32_t wkup_bitmap);

/** @} */ /* End of group 87x3e_PLATFORM_RTC_Exported_Functions */

/** @} */ /* End of group 87x3e_PLATFORM_RTC */

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_RTC_H */
