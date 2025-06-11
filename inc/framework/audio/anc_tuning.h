/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _ANC_TUNING_H_
#define _ANC_TUNING_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_ANC_TUNING
 *
 * \brief   Provide ANC control tools.
 */

/**  \brief  ANC response measurement mode */
typedef enum t_anc_resp_meas_mode
{
    ANC_RESP_MEAS_MODE_NONE,
    ANC_RESP_MEAS_MODE_ENTER,
    ANC_RESP_MEAS_MODE_EXIT,
} T_ANC_RESP_MEAS_MODE;

/**
 * anc_tuning.h
 *
 * \brief   Enable active noise cancellation.
 *
 * \parameter  image_sub_type   ANC image sub-type, only available for LL-APT image.
 * \parameter  scenario_id      Scenario id for ANC image type.
 *
 * \return          The status of load ANC scenario image.
 * \retval  always true
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_load_scenario(uint8_t image_sub_type, uint8_t scenario_id);

/**
 * anc_tuning.h
 *
 * \brief   Set ANC feature map for ANC control.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  feature_map      ANC control option feature map.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_set_feature_map(uint32_t feature_map);

/**
 * anc_tuning.h
 *
 * \brief   Get ANC feature map for ANC control.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \return
 * \retval  any     ANC feature map.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
uint32_t anc_tool_get_feature_map(void);

/**
 * anc_tuning.h
 *
 * \brief   Apply enable ANC setting directly.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_enable(void);

/**
 * anc_tuning.h
 *
 * \brief   Apply disable ANC setting directly.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_disable(void);

/**
 * anc_tuning.h
 *
 * \brief   Set ANC register or PALU register.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  anc_cmd_ptr      ANC command which follow HCI cmd format.
 *                              This command will indicate register offset and content,
 *                              and parse by anc driver.
 *
 * \return
 * \retval  true    ANC/PALU register write success.
 * \retval  false   ANC/PALU register write fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
uint8_t anc_tool_set_para(void *anc_cmd_ptr);

/**
 * anc_tuning.h
 *
 * \brief   Get ANC/PALU register content.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  reg_addr         Target register address by word.
 *
 * \return
 * \retval  0xDEADBEEF          Cannot get content by target reg addr.
 * \retval  others              Content by target reg addr.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
uint32_t anc_tool_read_reg(uint32_t reg_addr);

/**
 * anc_tuning.h
 *
 * \brief   Get data log length from ANC reg.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \return
 * \retval  any     Data log length by word.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
uint32_t anc_tool_get_data_log_length(void);

/**
 * anc_tuning.h
 *
 * \brief   Enable MCU play tone by specific frequencies and amp_ratio
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  enable           Enable/Disable MCU play tone.
 * \parameter  ch_sel           L/R channel select for measurement.
 * \parameter  tx_freq          User specific frequencies.
 * \parameter  freq_num         Number of frequencies.
 * \parameter  amp_ratio        Tone Amplitude, must be 0 ~ 128.
 *
 * \return
 * \retval  true    MCU play tone success.
 * \retval  false   MCU play tone fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_response_measure_enable(uint8_t enable, uint8_t ch_sel, uint32_t *tx_freq,
                                      uint8_t freq_num, uint8_t amp_ratio);

/**
 * anc_tuning.h
 *
 * \brief   Set ANC data log configuration and dump log.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  src0_sel         Log source 0 selection.
 * \parameter  src1_sel         Log source 1 selection.
 * \parameter  log_len          Log length(by word).
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_config_data_log(uint8_t src0_sel, uint8_t src1_sel, uint16_t log_len);

/**
 * anc_tuning.h
 *
 * \brief   Unlock ram setting to read log directly.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_load_data_log(void);

/**
 * anc_tuning.h
 *
 * \brief   convert data log dest addr.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \parameter  log_dest_addr    Destination address of ANC data log.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_convert_data_log_addr(uint32_t *log_dest_addr);
/**
 * anc_tuning.h
 *
 * \brief   Get current response measurement mode.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \return
 * \retval  any     Response measurement mode.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
T_ANC_RESP_MEAS_MODE anc_tool_check_resp_meas_mode(void);

/**
 * anc_tuning.h
 *
 * \brief   Enter specific response measurement mode.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  resp_meas_mode   Response measurement mode.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_set_resp_meas_mode(T_ANC_RESP_MEAS_MODE resp_meas_mode);

/**
 * anc_tuning.h
 *
 * \brief   Set gain mismatch to ram.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  gain_src   Gain source selection, 0: external mic, 2: internal mic.
 * \parameter  l_gain     Gain value for Lch.
 * \parameter  r_gain     Gain value for Rch.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_set_gain_mismatch(uint8_t gain_src, uint32_t l_gain, uint32_t r_gain);

/**
 * anc_tuning.h
 *
 * \brief   Read gain mismatch from ram or flash.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  gain_src   Gain source selection, 0: external mic, 1: internal mic.
 * \parameter  read_flash Read from flash or not, 0: from ram, 1: from flash.
 * \parameter  l_gain     Buffer to store Lch gain value.
 * \parameter  r_gain     Buffer to store Rch gain value.
 *
 * \return
 * \retval  true    Read gain mismatch success.
 * \retval  false   Read gain mismatch fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_read_gain_mismatch(uint8_t gain_src, uint8_t read_flash,
                                 uint32_t *l_gain, uint32_t *r_gain);

/**
 * anc_tuning.h
 *
 * \brief   Read MP extend data from flash.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  mp_ext_data   Buffer to store MP extend data.
 *
 * \return
 * \retval  true    Read MP extend data success.
 * \retval  false   Read MP extend data fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_read_mp_ext_data(uint32_t *mp_ext_data);

/**
 * anc_tuning.h
 *
 * \brief   Read LLAPT extend data from flash.
 *
 * \parameter  llapt_ext_data   Buffer to store LLAPT extend data.
 *
 * \return
 * \retval  true    Read LLAPT extend data success.
 * \retval  false   Read LLAPT extend data fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_read_llapt_ext_data(uint32_t *llapt_ext_data);

/**
 * anc_tuning.h
 *
 * \brief   Burn gain mismatch and MP extend data from ram to flash.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  mp_ext_data   MP extend data.
 *
 * \return
 * \retval  true    Burn MP extend data success.
 * \retval  false   Burn MP extend data fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */

bool anc_tool_burn_gain_mismatch(uint32_t mp_ext_data);

/**
* anc_tuning.h
*
* \brief   get ANC scenario info
*
* \parameter  scenario_mode    Get ANC scenario mode.
*
* \return
* \retval  any     ANC scenario num
*
* \ingroup AUDIO_ANC_TUNING
*/
uint8_t anc_tool_get_anc_scenario_info(uint8_t *scenario_mode);

/**
* anc_tuning.h
*
* \brief   get LLAPT scenario info
*
* \parameter  scenario_mode    Get LLAPT scenario mode.
*
* \return
* \retval  any     LLAPT scenario num
*
* \ingroup AUDIO_ANC_TUNING
*/
uint8_t anc_tool_get_llapt_scenario_info(uint8_t *scenario_mode, uint8_t *scenario_apt_effect);

/**
 * anc_tuning.h
 *
 * \brief   Set LLAPT gain mismatch to ram.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  l_gain     Gain value for Lch.
 * \parameter  r_gain     Gain value for Rch.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_set_llapt_gain_mismatch(uint32_t l_gain, uint32_t r_gain);

/**
 * anc_tuning.h
 *
 * \brief   Read LLAPT gain mismatch from ram or flash.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  read_flash Read from flash or not, 0: from ram, 1: from flash.
 * \parameter  l_gain     Buffer to store Lch gain value.
 * \parameter  r_gain     Buffer to store Rch gain value.
 *
 * \return
 * \retval  true    Read LLAPT gain mismatch success.
 * \retval  false   Read LLAPT gain mismatch fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_read_llapt_gain_mismatch(uint8_t read_flash, uint32_t *l_gain, uint32_t *r_gain);

/**
 * anc_tuning.h
 *
 * \brief   Burn LLAPT gain mismatch from ram to flash.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  llapt_ext_data   LLAPT extend data.
 *
 * \return
 * \retval  true    Burn LLAPT gain mismatch success.
 * \retval  false   Burn LLAPT gain mismatch fail.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
bool anc_tool_burn_llapt_gain_mismatch(uint32_t llapt_ext_data);

/**
 * anc_tuning.h
 *
 * \brief   Switch ANC/LLAPT limiter WNS on/off depend on feature map.
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_limiter_wns_switch(void);

/**
 * anc_tuning.h
 *
 * \brief   set anc MP backup buffer
 *
 * \parameter  param1, param2 control parameters.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_anc_mp_bak_buf_config(uint32_t param1, uint32_t param2);

/**
 * anc_tuning.h
 *
 * \brief   ramp data write for ramp tool
 *
 * \parameter  wdata, data to be written.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_ramp_data_write(uint32_t wdata);

/**
 * anc_tuning.h
 *
 * \brief   get ramp scenario address for ramp tool
 *
 * \parameter  param, command buffer address send to driver.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_get_scenario_addr(uint32_t param);

/**
 * anc_tuning.h
 *
 * \brief   Set scenario img to anc module
 *
 * \parameter sub image scenario.
 *
 * \ingroup AUDIO_ANC_TUNING
 */
void anc_tool_set_scenario(void *image_buf);

/**
* anc_tuning.h
*
* \brief   Get scenario img
*
* \parameter  Sub_type      ANC or LLAPT type.
* \parameter  Scenario_id   scenario index .
* \parameter  Length        ANC or LLAPT length .
* \return
* \retval anc img buffer ptr.
*
* \ingroup AUDIO_ANC_TUNING
*/
void *anc_tool_get_scenario_img(uint8_t sub_type, uint8_t scenario_id, uint32_t *length);

/**
* anc_tuning.h
*
* \brief   Get PALU filter info
*
* \parameter  Length        filter info length .
* \return
* \retval  anc img buffer ptr.
*
* \ingroup AUDIO_ANC_TUNING
*/
void *anc_tool_get_filter_info_img(uint32_t *length);

/**
* anc_tuning.h
*
* \brief   Encrypt img
*
* \parameter  scenario_img      scenario image
* \parameter  length            scenario length
* \return
* \retval  anc img buffer ptr.
*
* \ingroup AUDIO_ANC_TUNING
*/
void anc_tool_para_aes_encrypt(uint32_t scenario_img, uint32_t length);

/**
 * anc_tuning.h
 *
 * \brief   query ADSP para
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  mtu_size: mtu size information
 *
 * \return
 * \retval  true    config success.
 * \retval  false   config fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_query_para(uint8_t *adsp_info);

/**
 * anc_tuning.h
 *
 * \brief   config ADSP para transfer parameters
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  crc_check: CRC16 checksum for all ADSP para payload
 *             adsp_para_len: length of adsp para payload
 *             seqment_total: total number of segment, each segment has mtusize as set in
 *                            CMD_ANC_GET_ADSP_INFO
 *
 * \return
 * \retval  true    config success.
 * \retval  false   config fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_config_adsp_para(uint16_t crc_check, uint32_t adsp_para_len,
                                  uint8_t segment_total);

/**
 * anc_tuning.h
 *
 * \brief   config turn on ADSP
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  enable: 1: enable ADSP 0: disable ADSP
 *
 * \return
 * \retval  true    config success.
 * \retval  false   config fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_turn_on_adsp(uint8_t enable);

/**
 * anc_tuning.h
 *
 * \brief   Set ADSP para
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  para_ptr: ADSP para array
 *             segment index: maximun is adsp_para_len/mtusize + 1
 *
 * \return
 * \retval  true    ADSP para register write success.
 * \retval  false   ADSP para register write fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_load_adsp_para(uint8_t *para_ptr, uint8_t segment_index);

/**
 * anc_tuning.h
 *
 * \brief   Set adaptive ANC enable or disable
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  enable 1:enable 0: disable
 *
 * \return
 * \retval  true    ANC/PALU register write success.
 * \retval  false   ANC/PALU register write fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_enable_adaptive_anc(uint8_t enable, uint8_t grp_idx);

/**
 * anc_tuning.h
 *
 * \brief   load adsp image
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  encode from tool to indicate which image should be loaded
 *
 * \return
 * \retval  true    success.
 * \retval  false   fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_load_adsp_image(uint32_t encode);

/**
 * anc_tuning.h
 *
 * \brief   get adsp image load done
 *          Only provided for tool usage, cannot invoke in normal uasge.
 *
 * \parameter  encode from tool to indicate which image should be loaded
 *
 * \return
 * \retval  true    success.
 * \retval  false   fail.
 *
 * \ingroup AUDIO_ANC
 */
uint8_t anc_tool_get_adsp_image_load_done(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ANC_TUNING_H_ */
