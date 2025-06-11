/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    aon_wdg_ext.h
  * @brief   This file provides api wrapper for bbpro compatibility..
  * @author  huan_yang
  * @date    2023-4-23
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __AON_WDG_EXT_H_
#define __AON_WDG_EXT_H_


/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
  * @brief AON Watch Dog (RTC)
  */

/* 0x190 & 0x19c(AON_WDT_CRT)
    01:00    rw  00/01/11: turn on AON Watchdog 10: turn off AON Watchdog                                   2'b10
    02       rw  1: WDG countine count in DLPS 0:WDG stop count in DLPS                                     1'b0
    03       rw  1: reset whole chip            0: reset whole chip - (AON register and RTC)                1'b0
    04       rw  when  reg_aon_wdt_cnt_ctl == 0                                                             1'b0
                 1:relaod counter when exit DLPS 0:not reload counter when exit DLPS
    07:04    rw
    25:08    rw  Set the period of AON watchdog (unit:1/450Hz ~ 1/2.3KHz)                                   8'hff
    31:26    rw
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_aon_wdt_en: 2;         // 01:00
        uint32_t reg_aon_wdt_cnt_ctl: 1;    // 02
        uint32_t reg_aon_wdt_rst_lv_sel: 1; // 03
        uint32_t reg_aon_wdt_cnt_reload: 1; // 04
        uint32_t rsvd0: 3;                  // 07:05
        uint32_t reg_aon_wdt_comp: 18;      // 25:08
        uint32_t rsvd1: 6;                  // 31:26
    };
} RTC_AON_WDT_CRT_TYPE;

/* 0x198 & 0x1a4(AON_WDT_CRT)
    00    rw  write_protect 1:enable 0:disable               1'b0
    01    rw  wdt_disable                                    1'b0                                 8'hff
    31:02    rw
*/
typedef struct
{
    uint32_t write_protect : 1;
    uint32_t wdt_disable : 1;
    uint32_t reserved: 30;
} BTAON_FAST_RTC_AON_WDT_CONTROL;

typedef struct
{
    RTC_AON_WDT_CRT_TYPE aon_wdg_crt;
    uint32_t aon_wdg_clr;
    BTAON_FAST_RTC_AON_WDT_CONTROL aon_wdg_wp;
} BTAON_FAST_RTC_AON_WDT;

//#define AON_WDG_TIMEROUT_MAX_MS 43690

#define AON_WDG1_REG   ((BTAON_FAST_RTC_AON_WDT *)AON_WDG_REG_BASE)
#define AON_WDG2_REG   ((BTAON_FAST_RTC_AON_WDT *)(AON_WDG_REG_BASE + 0x0c))

#define AON_WDG1_REG_OFFSET   ((uint32_t)AON_WDG1_REG - RTC_REG_BASE)
#define AON_WDG2_REG_OFFSET   ((uint32_t)AON_WDG2_REG - RTC_REG_BASE)
/*AON_WDG1 is the same as charger aon wdg, and can not be used for the project with using charger*/
#define AON_WDG1 (BTAON_FAST_RTC_AON_WDT *)AON_WDG1_REG_OFFSET
#define AON_WDG2 (BTAON_FAST_RTC_AON_WDT *)AON_WDG2_REG_OFFSET

/**
    * @brief  start aon wdg
    * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
    * @param  cnt_ctl: true: aon wdg continues counting in DLPS. false: aon wdg stops counting in DLPS
    * @param  cnt_reload: when cnt_ctl is false
    *                     true: relaod counter when exit DLPS. false: not reload counter when exit DLPS
    * @return none
    */
void aon_wdg_set(bool cnt_ctl, bool cnt_reload);

/**
    * @brief  start aon wdg
    * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @param  count: timeout value and its maximum value is 0x3FFFF. aon wdg clock range is 0.29 to 6khz
    * @return true: success
    * @return false: fail
    */
bool aon_wdg_start(BTAON_FAST_RTC_AON_WDT *wdg, uint32_t count);

/**
    * @brief  disable aon wdg
    * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @return none
    */
void aon_wdg_disable(BTAON_FAST_RTC_AON_WDT *wdg);

/**
    * @brief  restart aon wdg
    * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @return none
    */
void aon_wdg_restart(BTAON_FAST_RTC_AON_WDT *wdg);

#ifdef __cplusplus
}
#endif

#endif
