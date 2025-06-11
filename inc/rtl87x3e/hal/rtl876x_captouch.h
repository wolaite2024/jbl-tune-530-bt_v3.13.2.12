/**
************************************************************************************************************
*            Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file      rtl876x_captouch.h
* @brief     Cap Touch implementation header file.
* @author    js.lin
* @date      2020-11-24
* @version   v1.0
*************************************************************************************************************
*/
#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

typedef enum
{
    CTC_CH0 = 0,
    CTC_CH1 = 1,
    CTC_CH2 = 2,
    CTC_CH3 = 3
} CTC_CH_TYPE;

typedef enum
{
    CTC_TOUCH_PRESS_INT     = (1UL << 0),
    CTC_TOUCH_RELEASE_INT   = (1UL << 8),
    CTC_FALSE_TOUCH_INT     = (1UL << 19)
} CTC_CH_INT_TYPE;

typedef enum
{
    CTC_OVER_P_NOISE_INT    = (1UL << 16),
    CTC_OVER_N_NOISE_INT    = (1UL << 18)
} CTC_NOISE_INT_TYPE;

typedef enum
{
    CTC_SLOW_MODE = 0,
    CTC_FAST_MODE = 1
} CTC_MODE_TYPE;

/**
  * @brief  Reset CapTouch system and clear all setting.
  * @retval None
  */
void CapTouch_SysReset(void);

/**
  * @brief  Enables or disables the specified CapTouch interrupts of specified channel.
  * @param  channel: specified channel
  * @param  INT_to_config: specifies the CapTouch interrupt sources to be enabled or masked.
  *         This parameter can be one or combinations of the following values:
  *            @arg CTC_TOUCH_PRESS_INT:   CapTouch press interrupt
  *            @arg CTC_TOUCH_RELEASE_INT: CapTouch release interrupt
  *            @arg CTC_FALSE_TOUCH_INT  CapTouch false touch interrupt
  * @param  new_state: new state of the specified CapTouch interrupts mask.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_ChINTConfig(CTC_CH_TYPE channel, CTC_CH_INT_TYPE INT_to_config,
                          FunctionalState new_state);

/**
  * @brief  Enables or disables the specified CapTouch noise interrupts.
  * @param  INT_to_config: specifies the CapTouch interrupt sources to be enabled or masked.
  *         This parameter can be one or combinations of the following values:
  *            @arg CTC_OVER_P_NOISE_INT: CapTouch press interrupt
  *            @arg CTC_OVER_N_NOISE_INT: CapTouch release interrupt
  * @param  new_state: new state of the specified CapTouch interrupts mask.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_NoiseINTConfig(CTC_NOISE_INT_TYPE INT_to_config, FunctionalState new_state);

/**
  * @brief  Check whether the specific interrupt of specific channel is triggered
  * @param  int_status: interrupt status
  * @param  channel: specified channel
  * @param  INT_source: specifies the CapTouch interrupts to be check.
  *         This parameter can be one or combinations of the following values:
  *            @arg CTC_TOUCH_PRESS_INT:   CapTouch press interrupt
  *            @arg CTC_TOUCH_RELEASE_INT: CapTouch release interrupt
  *            @arg CTC_FALSE_TOUCH_INT  CapTouch false touch interrupt
  * @retval Interrupt status; 1: Triggered, 0: Not triggered
  */
bool CapTouch_IsChINTTriggered(uint32_t int_status, CTC_CH_TYPE channel,
                               CTC_CH_INT_TYPE INT_source);

/**
  * @brief  Check whether the specific noise interrupt is triggered
  * @param  int_status: interrupt status
  * @param  INT_source: specifies the CapTouch interrupts to be check.
  *         This parameter can be one or combinations of the following values:
  *            @arg CTC_OVER_P_NOISE_INT: CapTouch press interrupt
  *            @arg CTC_OVER_N_NOISE_INT: CapTouch release interrupt
  * @retval Interrupt status; 1: Triggered, 0: Not triggered
  */
bool CapTouch_IsNoiseINTTriggered(uint32_t int_status, CTC_NOISE_INT_TYPE INT_source);

/**
  * @brief  Clears the specified CapTouch interrupt pending bits of specified channel.
  * @param  channel: specified channel
  * @param  INT_to_clear: specifies the CapTouch interrupts to be cleared.
  *         This parameter can be one or combinations of the following values:
  *            @arg CTC_TOUCH_PRESS_INT:   CapTouch press interrupt
  *            @arg CTC_TOUCH_RELEASE_INT: CapTouch release interrupt
  *            @arg CTC_FALSE_TOUCH_INT  CapTouch false touch interrupt
  * @retval None
  */
void CapTouch_ChINTClearPendingBit(CTC_CH_TYPE channel, CTC_CH_INT_TYPE INT_to_clear);

/**
  * @brief  Clears the specified CapTouch noise interrupt pending bits.
  * @param  INT_to_clear: specifies the CapTouch interrupts to be cleared.
  *         This parameter can be one or combinations of the following values:
  *            @arg CTC_OVER_P_NOISE_INT: CapTouch press interrupt
  *            @arg CTC_OVER_N_NOISE_INT: CapTouch release interrupt
  * @retval None
  */
void CapTouch_NoiseINTClearPendingBit(CTC_NOISE_INT_TYPE INT_to_clear);

/**
  * @brief  Clears all CapTouch interrupt pending bits.
  * @retval None
  */
void CapTouch_INTClearAllPendingBit(void);

/**
  * @brief  Enables or disables the specified CapTouch peripheral.
  * @param  new_state: new state of the CapTouch peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @param  baseline_init: enable or disable baseline init.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_Cmd(FunctionalState new_state, FunctionalState baseline_init);

/**
  * @brief  Enable or disable specified channel.
  * @param  channel: specified channel
  * @param  new_state: new state of the specified channel.
  *   this parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_ChCmd(CTC_CH_TYPE channel, FunctionalState new_state);

/**
  * @brief  Enable or disable specified channel wakeup function.
  * @param  channel: specified channel
  * @param  new_state: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_ChWakeupCmd(CTC_CH_TYPE channel, FunctionalState new_state);

/**
  * @brief  Set CapTouch Scan interval for slow mode or fast mode.
  * @param  interval: scan interval in units of 1s/1.024KHz (= 0.9765625 ms)
  *         Configurable range: 0x0~0xFFF (0~4095)
  *         Note that slow mode interval should not be lower then fast mode interval,
  *         Fast mode interval should be greater then total guard time for auxADC
  * @param  mode: CTC_SLOW_MODE or CTC_FAST_MODE.
  * @retval ture: success, false: interval is out of range
  */
bool CapTouch_SetScanInterval(uint16_t interval, CTC_MODE_TYPE mode);

/**
  * @brief  Set baseline to specified channel.
  * @param  channel: specified channel
  * @param  baseline: baseline value
  * @retval None
  */
void CapTouch_SetChBaseline(CTC_CH_TYPE channel, uint16_t baseline);

/**
  * @brief  Set relative touch threshold for related channel.
  * @param  channel: specified channel
  * @param  threshold: related threshold value.
  * @retval None
  */
void CapTouch_SetChDiffThres(CTC_CH_TYPE channel, uint16_t threshold);

/**
  * @brief  Set P noise threshold for related channel.
  * @param  channel: specified channel
  * @param  p_noise_threshold: Pnoise Threshold value.
  * @retval None
  */
void CapTouch_SetChPNoiseThres(CTC_CH_TYPE channel, uint8_t p_noise_threshold);

/**
  * @brief  Set N noise threshold for related channel.
  * @param  channel: specified channel
  * @param  n_noise_threshold: Nnoise Threshold value.
  * @retval None
  */
void CapTouch_SetChNNoiseThres(CTC_CH_TYPE channel, uint8_t n_noise_threshold);

/**
  * @brief  Set Mbias current for specified channel.
  * @param  channel: specified channel
  * @param  mbias,mbias value, relate current = 0.25*mbias.
  * @retval None
  */
void CapTouch_SetChMbias(CTC_CH_TYPE channel, uint8_t mbias);

/**
  * @brief  Get CapTouch interrupt status.
  * @retval interrupt status
  */
uint32_t CapTouch_GetINTStatus(void);

/**
  * @brief  Get status from specified channel.
  * @param  channel: specified channel
  * @retval channel enable status.
  */
bool CapTouch_GetChStatus(CTC_CH_TYPE channel);

/**
  * @brief  Read Baseline data from specified channel.
  * @param  channel: specified channel
  * @retval Baseline data
  */
uint16_t CapTouch_GetChBaseline(CTC_CH_TYPE channel);

/**
  * @brief  Read Ave data from specified channel.
  * @param  channel: specified channel
  * @retval Average data
  */
uint16_t CapTouch_GetChAveData(CTC_CH_TYPE channel);

/**
  * @brief  Get relative touch threshold of specified channel.
  * @param  channel: specified channel
  * @retval related threshold value.
  */
uint16_t CapTouch_GetChDiffThres(CTC_CH_TYPE channel);

/**
  * @brief  Get Mbias current setting from specified channel.
  * @param  channel: specified channel
  * @retval mbias value, unit 0.25uA
  */
uint8_t CapTouch_GetChMbias(CTC_CH_TYPE channel);

/**
  * @brief  Read touch active count from specified channel.
  * @param  channel: specified channel
  * @retval touch active count
  */
uint16_t CapTouch_GetChTouchCnt(CTC_CH_TYPE channel);

/**
  * @brief  Read CTC touch status under fast mode
  * @retval CTC touch status of all channel; bit4~bit0 each bit represent one channel, 0: un-tounch, 1: tounch
  */
uint8_t CapTouch_GetChTouchStatus(void);

/**
  * @brief  Get number of enabled channels
  * @retval number of enabled channels
  */
uint8_t CapTouch_GetNumOfEnabledChs(void);

/**
  * @brief  Read CTC mode status
  * @retval CTC mode status; 1: Fast mode, 0: Slow mode
  */
bool CapTouch_IsFastMode(void);

/**
  * @brief  Check legality of specified channel
  * @param  channel: specified channel
  * @retval 1: Allowed, 0: Not allowed
  */
bool CapTouch_IsChAllowed(CTC_CH_TYPE channel);

/**
  * @brief  Read CTC_en status
  * @retval CTC_en status; 1: Running, 0: Non-runnung
  */
bool CapTouch_IsRunning(void);

/**
  * @brief  Read system enable bit
  * @retval System enable status; 1: enable, 0:disable
  */
bool CapTouch_IsSystemEnable(void);

/**
  * @brief Set system enable bit
  * @param System enable status; true: enable, false :disable
  * @retval none
  */
void CapTouch_SystemEnable(bool status);

/**
  * @brief  Get max channel of ctc.
  * @retval max channel of ctc.
  */
uint8_t CapTouch_GetMaxChannel(void);

/**
  * @brief  Get 32k clock power Domain. CTC wakup from powerdown 32K clock power domain should be AON_DOMAIN  or ALWAYS_ACTIVE
  * @retval
   0  : ALWAYS_ACTIVE  ,
   1  : AON_DOMAIN     ,
   2  : PON_DOMAIN     ,
   4  : CORE_DOMAIN    ,
   6  : ALWAYS_INACTIVE,
  */
uint32_t Captouch_Get32KClkPowerDomain(void);
