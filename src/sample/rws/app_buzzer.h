/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_BUZZER_H_
#define _APP_BUZZER_H_

#include <stdint.h>

/*============================================================================*
 *                              Macros
 *============================================================================*/
#define BUZZER_MODE_NONE            0

#define INCOMING_CALL_BUZZER        app_cfg_const.buzzer_mode_incoming_call
#define POWER_ON_BUZZER             app_cfg_const.buzzer_mode_power_on
#define POWER_OFF_BUZZER            app_cfg_const.buzzer_mode_power_off
#define ENTER_PAIRING_BUZZER        app_cfg_const.buzzer_mode_enter_pairing
#define PAIRING_COMPLETE_BUZZER     app_cfg_const.buzzer_mode_pairing_complete
#define LINK_LOSS_BUZZER            app_cfg_const.buzzer_mode_link_loss
#define ALARM_BUZZER                app_cfg_const.buzzer_mode_alarm

/*============================================================================*
 *                              Types
 *============================================================================*/
/**  @brief  PWM frequence for 1k, 2k, 3k*/
typedef enum t_pwm_freq
{
    BUZZER_PWM_FREQ_1K,
    BUZZER_PWM_FREQ_2K,
    BUZZER_PWM_FREQ_3K,
} T_BUZZER_PWM_FREQ;

/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
    * @brief  According to different scenario,set buzzer mode that include incoming call, power on,
    *         power off, enter pairing etc.
    * @param  mode buzzer mode
    * @return void
    */
void buzzer_set_mode(uint8_t mode);

#endif
