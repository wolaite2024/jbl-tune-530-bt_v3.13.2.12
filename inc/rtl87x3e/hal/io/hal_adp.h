/*
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _HAL_ADP_
#define _HAL_ADP_

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x_pinmux.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup 87x3e_HAL_ADP HAL ADP
 * @{
 */

/** @defgroup T_ADP_DETECT adp detect type
 * @{
 */

typedef enum
{
    ADP_DETECT_5V,
    ADP_DETECT_IO,
    ADP_DETECT_MAX,
} T_ADP_DETECT;

/** End of group T_ADP_DETECT
  * @}
  */

/** @defgroup T_ADP_PLUG_EVENT adp plug event
 * @{
 */

typedef enum
{
    ADP_EVENT_PLUG_IN,
    ADP_EVENT_PLUG_OUT,
    ADP_EVENT_MAX,
} T_ADP_PLUG_EVENT;

/** End of group T_ADP_PLUG_EVENT
  * @}
  */

/** @defgroup T_ADP_LEVEL adp level
 * @{
 */

typedef enum
{
    ADP_LEVEL_LOW,
    ADP_LEVEL_HIGH,
    ADP_LEVEL_UNKNOWN,
} T_ADP_LEVEL;

/** End of group T_ADP_LEVEL
  * @}
  */

/** @defgroup T_ADP_STATE adp state
 * @{
 */

typedef enum
{
    ADP_STATE_DETECTING,
    ADP_STATE_IN,
    ADP_STATE_OUT,
    ADP_STATE_UNKNOWN,
} T_ADP_STATE;

/** End of group T_ADP_STATE
  * @}
  */


/** @defgroup T_ADP_IO_GPIO_STATE adp state
 * @{
 */

typedef enum
{
    ADP_IO_GPIO_CLOSE,
    ADP_IO_GPIO_OPEN,
} T_ADP_IO_GPIO_STATE;

/** End of group T_ADP_IO_GPIO_STATE
  * @}
  */

/** @defgroup T_ADP_IO_DEBOUNCE_TIME adp io debounce time
 * @{
 */
typedef enum
{
    IO_DEBOUNCE_TIME_0MS,       /*!< ADP IO debounce time 0ms.   */
    IO_DEBOUNCE_TIME_10MS,      /*!< ADP IO debounce time 10ms.  */
    IO_DEBOUNCE_TIME_20MS,      /*!< ADP IO debounce time 20ms.  */
    IO_DEBOUNCE_TIME_40MS,      /*!< ADP IO debounce time 40ms.  */
    IO_DEBOUNCE_TIME_60MS,      /*!< ADP IO debounce time 60ms.  */
    IO_DEBOUNCE_TIME_80MS,      /*!< ADP IO debounce time 80ms.  */
    IO_DEBOUNCE_TIME_100MS,     /*!< ADP IO debounce time 100ms. */
    IO_DEBOUNCE_TIME_200MS,     /*!< ADP IO debounce time 200ms. */
    IO_DEBOUNCE_TIME_300MS,     /*!< ADP IO debounce time 300ms. */
    IO_DEBOUNCE_TIME_400MS,     /*!< ADP IO debounce time 400ms. */
    IO_DEBOUNCE_TIME_500MS,     /*!< ADP IO debounce time 500ms. */
    IO_DEBOUNCE_TIME_600MS,     /*!< ADP IO debounce time 600ms. */
    IO_DEBOUNCE_TIME_700MS,     /*!< ADP IO debounce time 700ms. */
    IO_DEBOUNCE_TIME_800MS,     /*!< ADP IO debounce time 800ms. */
    IO_DEBOUNCE_TIME_900MS,     /*!< ADP IO debounce time 900ms. */
    IO_DEBOUNCE_TIME_1000MS,    /*!< ADP IO debounce time 1000ms.*/
    IO_DEBOUNCE_TIME_MAX,
} T_ADP_IO_DEBOUNCE_TIME;

/** End of group T_ADP_IO_DEBOUNCE_TIME
  * @}
  */

/**
  * @brief  Adp interrupt callback function type
  * @param  None
  * @retval None
  */
typedef void (*P_ADP_ISR_CBACK)(void);

/**
  * @brief  Adp plug callback function type
  * @param  event: adp plug event
  *     This parameter can be one of the following values:
  *     @arg ADP_EVENT_PLUG_IN: adp plug in.
  *     @arg ADP_EVENT_PLUG_OUT: adp plug out.
  * @param  user_data: user data
  * @retval None
  */
typedef void (*P_ADP_PLUG_CBACK)(T_ADP_PLUG_EVENT event, void *user_data);

/**
  * @brief  Update adp io/5V interrupt callback
  * @param  adp_type: use adp 5v or adp io
  *     This parameter can be one of the following values:
  *     @arg ADP_DETECT_5V: use adp 5v.
  *     @arg ADP_DETECT_IO: use adp io.
  * @param  cback: adp interrupt callback function
  * @retval true: Update adp interrupt callback success
  * @retval false: Update adp interrupt callback failed
  */
bool adp_update_isr_cb(T_ADP_DETECT adp_type, P_ADP_ISR_CBACK cback);

/**
  * @brief  Get adp io/5V current level
  * @param  adp_type: use adp 5v or adp io
  *     This parameter can be one of the following values:
  *     @arg ADP_DETECT_5V: use adp 5v.
  *     @arg ADP_DETECT_IO: use adp io.
  * @retval return adp current level
  */
T_ADP_LEVEL adp_get_level(T_ADP_DETECT adp_type);

/**
  * @brief  Register adp io/5V callback
  * @param  adp_type: use adp 5v or adp io
  *     This parameter can be one of the following values:
  *     @arg ADP_DETECT_5V: use adp 5v.
  *     @arg ADP_DETECT_IO: use adp io.
  * @param  cback: adp callback function
  * @param  user_data: user data
  * @retval true: Register adp callback success
  * @retval false: Register adp callback failed
  */
bool adp_register_state_change_cb(T_ADP_DETECT adp_type, P_ADP_PLUG_CBACK cback, void *user_data);

/**
  * @brief  Unregister adp io/5V callback
  * @param  adp_type: use adp 5v or adp io
  *     This parameter can be one of the following values:
  *     @arg ADP_DETECT_5V: use adp 5v.
  *     @arg ADP_DETECT_IO: use adp io.
  * @param  cback: adp callback function
  * @retval true: Unregister adp callback success
  * @retval false: Unregister adp callback failed
  */
bool adp_unregister_state_change_cb(T_ADP_DETECT adp_type, P_ADP_PLUG_CBACK cback);

/**
  * @brief  Get adp io/5V current state
  * @param  adp_type: use adp 5v or adp io
  *     This parameter can be one of the following values:
  *     @arg ADP_DETECT_5V: use adp 5v.
  *     @arg ADP_DETECT_IO: use adp io.
  * @retval return adp current state
  */
T_ADP_STATE adp_get_current_state(T_ADP_DETECT adp_type);

/**
  * @brief  Get adp io/5V function debounce time
  * @param  adp_type: use adp 5v or adp io
  *     This parameter can be one of the following values:
  *     @arg ADP_DETECT_5V: use adp 5v.
  *     @arg ADP_DETECT_IO: use adp io.
  * @param  io_in_debounce_time: adp in debounce time
  * @param  io_out_debounce_time: adp out debounce time
  * @retval true: get adp io debounce time success
  * @retval false: get adp io debounce time failed
  */
bool adp_get_debounce_time(T_ADP_DETECT adp_type, uint32_t *p_in_debounce_ms,
                           uint32_t *p_out_debounce_ms);

/**
  * @brief  Only can set adp io function debounce time
  * @param  adp_type: only can be set ADP_DETECT_IO
  * @param  io_in_debounce_time: adp io in debounce time
  * @param  io_out_debounce_time: adp io out debounce time
  * @retval true: set adp io debounce success
  * @retval false: adp type error or debounce time overflow
  */
bool adp_set_debounce_time(T_ADP_DETECT adp_type, T_ADP_IO_DEBOUNCE_TIME io_in_debounce_time,
                           T_ADP_IO_DEBOUNCE_TIME io_out_debounce_time);

/**
 * @brief  Close adp function
 * @param  adp_type: only can be set ADP_DETECT_IO
 * @retval true: close adp io function success
 * @retval false: adp type error
 */
bool adp_close(T_ADP_DETECT adp_type);

/**
  * @brief  Open adp function
  * @param  adp_type: only can be set ADP_DETECT_IO
  * @retval true: open adp io function success
  * @retval false: adp type error
  */
bool adp_open(T_ADP_DETECT adp_type);

/**
  * @brief  adapter wake up function enable
  * @param  mode: config power off or none power off
  *     This parameter value is WAKE_UP_POWER_OFF or WAKE_UP_GENERAL.
  * @param  pol: polarity to wake up
  *     This parameter WAKEUP_POL POL_HIGH means high level POL_LOW means low level to wakeup.
  * @retval true: enable success
  * @retval false: enable failed
  */
bool adp_wake_up_enable(T_WAKE_UP_MODE mode, WAKEUP_POL pol);

/**
  * @brief  adapter wake up function disable
  * @param  mode: config power off or none power off
  *     This parameter value is WAKE_UP_POWER_OFF or WAKE_UP_GENERAL.
  * @retval none
  */
void adp_wake_up_disable(T_WAKE_UP_MODE mode);

/** End of group 87x3e_HAL_ADP
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HAL_ADP_ */

