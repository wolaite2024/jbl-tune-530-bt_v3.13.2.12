/**
*****************************************************************************************
*     Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2021 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                      Define to prevent recursive inclusion
 *============================================================================*/

#ifndef _AUDIO_PAD_H_
#define _AUDIO_PAD_H_

#ifdef __cplusplus
extern "C" {
#endif

bool audio_pad_init(void);
void audio_pad_deinit(void);
void audio_pad_bias_enable(void);
void audio_pad_bias_set(bool enable);
void audio_pad_mic_set(uint8_t mic_sel, uint8_t mic_src, uint8_t mic_type);
void audio_pad_spk_set(uint8_t spk_sel, uint8_t spk_type);
void audio_pad_aux_set(uint8_t aux_sel);
uint8_t audio_pad_check_bias_gpio_mode(uint8_t avcc_drv_sel, uint8_t micbias_voltage_sel);
void audio_pad_dmic_pad_ctrl(uint32_t dmic_pad_ctrl);

#ifdef __cplusplus
}
#endif

#endif  // _AUDIO_PAD_H_

