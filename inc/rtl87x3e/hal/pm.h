/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    pm.h
  * @brief   This file provides apis for power manager.
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
#ifndef __PM_H_
#define __PM_H_


/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup HAL_87x3e_PM
  * @brief
  * @{
  */

/*============================================================================*
 *                              Variables
*============================================================================*/
/** @defgroup HAL_87x3e_PM_Exported_Variables PM Exported Variables
  * @{
  */

typedef enum
{
    BTPOWER_DEEP_SLEEP        = 0,   /**< Deep sleep */
    BTPOWER_ACTIVE            = 1,   /**< Active     */
} BtPowerMode;


/** @brief POWER STAGE struct */
typedef enum
{
    POWER_STAGE_STORE           = 0,
    POWER_STAGE_RESTORE         = 1,
} POWERStage;

/** @brief LPSMode struct */
typedef enum
{
    POWER_SHIP_MODE    = 0,
    POWER_POWERDOWN_MODE   = 1,   /**< Power down */
    POWER_DLPS_MODE        = 2,   /**< DLPS       */
    POWER_LPS_MODE         = 4,   /**< LPS        */
    POWER_ACTIVE_MODE      = 5,    /**< Active     */
    POWER_MODE_MAX         = 6
} POWERMode;

typedef enum
{
    POWER_DLPS_PFM_MODE = 0,    /**< default one */
    POWER_DLPS_RET_MODE = 1,
} POWERDlpsMode;

/* BB2 using*/
typedef enum
{
    PM_ERROR_UNKNOWN               = 0x0,
    PM_ERROR_POWER_MODE            = 0x1,
    PM_ERROR_DISABLE_DLPS_TIME     = 0x2,
    PM_ERROR_32K_CHECK_LOCK        = 0x3,
    PM_ERROR_LOG_DMA_NOT_IDLE      = 0x4,
    PM_ERROR_CALLBACK_CHECK        = 0x5,
    PM_ERROR_INTERRUPT_OCCURRED    = 0x6,
    PM_ERROR_WAKEUP_TIME           = 0x7,
    PM_ERROR_FREE_BLOCK_STORE      = 0x8,
    PM_ERROR_BUFFER_PRE_ALLOCATE   = 0x9,
} PowerModeErrorCode;

typedef enum
{
    PM_EXCLUDED_TIMER,
    PM_EXCLUDED_TASK,
    PM_EXCLUDED_TYPE_MAX,
} PowerModeExcludedHandleType;

#define POWER_POWEROFF_MODE            POWER_SHIP_MODE      /* for compatible */

typedef struct t_pm_cpu_handle
{
    uint32_t cpu_freq;
} T_PM_CPU_HANDLE;

/** @brief This CB is used for any module which needs to be checked before entering DLPS */
typedef bool (*POWERCheckFunc)();

/** @brief This CB is used for any module which needs to control the hw before entering or after exiting from DLPS */
typedef void (*POWERStageFunc)();

typedef bool (*PMDVFSCheckFunc)(void);
/** @} */ /* End of group HAL_87x3e_PM_Exported_Variables */

/*============================================================================*
 *                              Functions
*============================================================================*/
/** @defgroup HAL_87x3e_PM_Exported_Functions PM Exported Functions
  * @{
  */

/**
 * @brief set up bt power mode
 * @param  mode  BTPowerMode.
 * @return  none.
*/
void bt_power_mode_set(BtPowerMode mode);

/**
 * @brief getbt power mode
 * @param  mode  BTPowerMode.
 * @return  mode of bt power.
*/

BtPowerMode bt_power_mode_get(void);

/**
 * @brief Register Check CB to Power module which will call it before entering Dlps.
 * @param  func  DLPSEnterCheckFunc.
 * @return  Status of Operation.
*/
int32_t power_check_cb_register(POWERCheckFunc func);


/**
 * @brief Register HW Control CB to Power module which will call it before entering power mode or after exiting from power mode (according to POWERStage) .
 * @param  func  POWERStageFunc.
 * @param  stage  tell the Power module the CB should be called when @ref POWER_STAGE_ENTER or POWER_STAGE_EXIT.
 * @return  Status of Operation.
*/
int32_t power_stage_cb_register(POWERStageFunc func, POWERStage stage);

/**
 * @brief  POWERMode Set .
 * @param  mode   POWERMode.
 * @return  none
*/
int32_t power_mode_set(POWERMode mode);

/**
 * @brief  POWERMode Get .
 * @param  none
 * @return  POWERMode
*/
POWERMode power_mode_get(void);

/**
 * @brief  POWERDLPSMode Set .
 * @param  mode   DLPSMODE,default value is DLPS_PFM.
 * @return  none
 * @note    It only takes effect when bt_power_mode_set(POWER_DLPS_MODE).Please call this function before power_mode_set(POWER_DLPS_MODE)
*/
int32_t power_dlps_mode_select(POWERDlpsMode mode);

/**
 * @brief  POWERDlpsMode Get.
 * @param  mode   none.
 * @return  POWERDlpsMode
*/
POWERDlpsMode power_dlps_mode_get(void);

/**
 * @brief  POWERMode Pause .
 * @param  none
 * @return  void
*/
int32_t power_mode_pause(void);

/**
 * @brief  POWERMode Resume .
 * @param  none
 * @return  void
*/
int32_t power_mode_resume(void);

/**
 * @brief  POWERErrorCode get .
 * @param  none
 * @return  POWERErrorCode
*/
PowerModeErrorCode power_get_error_code(void);

/**
 * @brief  stop timer which are not using after POWERDlpsMode
 * @param  none
 * @return  none
*/
void power_stop_all_non_excluded_timer(void);

/**
 * @brief  register task or timer handle in excluded pool.
 * @param  handle: task or timer handle
 * @param  type: PM_EXCLUDED_TIMER or PM_EXCLUDED_TASK
 * @return  the error code of power manager
*/
bool power_register_excluded_handle(void **handle, PowerModeExcludedHandleType type);

/**
 * @brief  get statistics of power manager .
 * @param  wakeup_count: amount of wakeup
 * @param  total_wakeup_time: time of total wakeup
 * @param  total_sleep_time: time of total sleep
 * @return  none
*/
void power_get_statistics(uint32_t *wakeup_count, uint32_t *total_wakeup_time,
                          uint32_t *total_sleep_time);


typedef enum
{
    PM_SUCCESS                         = 0x0,
    PM_DVFS_BUSY                       = -0x1,
    PM_DVFS_VOLTAGE_FAIL               = -0x2,
    PM_DVFS_CONDITION_FAIL             = -0x4,
    PM_DVFS_SRAM_FAIL                  = -0x8,
    PM_DVFS_NOT_SUPPORT                = -0x10,
} PMErrorCode;

/**
 * @brief Config cpu clock freq.
 * @param  require_mhz  require cpu freqency .
 * @param  real_mhz  the freqency of current cpu.
 * @return  Status of Operation.
*/
int32_t pm_cpu_freq_set(uint32_t required_mhz, uint32_t *actual_mhz);
int32_t pm_cpu_slow_freq_set(uint32_t required_mhz);
/**
 * @brief Config dsp1 clock freq.
 * @param  require_mhz  require dsp1 freqency .
 * @param  real_mhz  the freqency of current dsp1.
 * @return  Status of Operation.
*/
int32_t pm_dsp1_freq_set(uint32_t required_mhz, uint32_t *actual_mhz);

/**
 * @brief Get cpu clock freq.
 * @return  mhz.
*/
uint32_t pm_cpu_freq_get(void);

/**
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 * @brief Get cpu default clock freq.
 * @return  mhz.
*/
uint32_t pm_cpu_default_freq_get(void);

/**
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 * @brief Get cpu max clock freq.
 * @return  mhz.
*/
uint32_t pm_cpu_max_freq_get(void);

/**
    * @brief  init cpu freq handle
    * @return Status of Operation
*/
int32_t pm_cpu_freq_init(void);

/**
    * @brief  CPU freq adjustment for different application scenarios.
              The actual CPU freq will be set according current scanarios
    * @note   pm_cpu_freq_clear and pm_cpu_freq_req must match
    * @param  handle:the cpu freq session handle.
    * @param  required_mhz:require cpu freqency .
    * @param  actual_mhz:the freqency of current cpu.
    * @return Status of Operation
*/
int32_t pm_cpu_freq_req(uint8_t *handle, uint32_t required_mhz, uint32_t *actual_mhz);

/**
    * @brief  Used for recovery cpu freq.
    * @note   pm_cpu_freq_clear and pm_cpu_freq_req must match
    * @param  handle:the cpu freq session handle.
    * @param  actual_mhz:the freqency of current cpu.
    * @return Status of Operation
*/
int32_t pm_cpu_freq_clear(uint8_t *handle, uint32_t *actual_mhz);


/**
 * @brief Get dsp1 clock freq.
 * @return  mhz.
*/
uint32_t pm_dsp1_freq_get(void);

/**
 * @brief Set dvfs to the supreme mode.
 * @return  Status of Operation.
*/
int32_t pm_dvfs_set_supreme(void);

/**
 * @brief Set dvfs mode refs to current freqency of clock.
 * @return  Status of Operation.
*/
int32_t pm_dvfs_set_by_clock(void);
/**
 * @brief Register Check CB to dvfs module which will call it before dvfs mode changed.
 * @param  func  PMDVFSCheckFunc.
 * @return  Status of Operation.
*/
int32_t pm_dvfs_register_check_func(PMDVFSCheckFunc func);

/**
 * @brief whether the cpu enable auto slow
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * @param is_auto_slow true: enable
 *                     false: disable
 */
void pm_cpu_set_auto_slow_enable(bool is_auto_slow);

/** @} */ /* End of group PM_Exported_Functions */

/** @} */ /* End of group PM */

/** @} End of HAL_87x3e_PM */

#ifdef __cplusplus
}
#endif

#endif
