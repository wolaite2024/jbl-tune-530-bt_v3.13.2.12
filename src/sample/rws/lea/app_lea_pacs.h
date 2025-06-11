#ifndef _APP_LEA_PACS_H_
#define _APP_LEA_PACS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "codec_def.h"
#include "codec_qos.h"

#if F_APP_LC3_PLUS_SUPPORT
// vendor id(1) + RTK id(2) + LTV(3)
#define LEA_VENDOR_PARA_LEN          6

#define LEA_BIT_DEPTH                1

#define VENDOR_SPECIFIC_LC3_PLUS_ID 0x0001

#define FRAUNHOFER_COMPANY_ID   0xA908
#define FRAUNHOFER_HR_VBR_ID    0x0100
#define FRAUNHOFER_HR_CBR_ID    0x0200

#define FRAUNHOFER_SUPPORTED_FD            0xF1
#define FRAUNHOFER_SUPPORTED_OCTETS_10_MS  0xF2
#define FRAUNHOFER_SUPPORTED_OCTETS_7_5_MS 0xF3
#define FRAUNHOFER_SUPPORTED_OCTETS_5_MS   0xF4
#define FRAUNHOFER_SUPPORTED_OCTETS_2_5_MS 0xF5

#define FRAUNHOFER_FD_10_MS_BIT         0x0001
#define FRAUNHOFER_FD_7_5_MS_BIT        0x0002
#define FRAUNHOFER_FD_5_MS_BIT          0x0004
#define FRAUNHOFER_FD_2_5_MS_BIT        0x0008

#define FRAUNHOFER_FD_PREFER_10_MS_BIT  0x0100
#define FRAUNHOFER_FD_PREFER_7_5_MS_BIT 0x0200
#define FRAUNHOFER_FD_PREFER_5_MS_BIT   0x0400
#define FRAUNHOFER_FD_PREFER_2_5_MS_BIT 0x0800

#define FRAUNHOFER_CFG_FD               0xF1

#define FRAUNHOFER_CFG_FD_10_MS         0x01
#define FRAUNHOFER_CFG_FD_7_5_MS        0x02
#define FRAUNHOFER_CFG_FD_5_MS          0x03
#define FRAUNHOFER_CFG_FD_2_5_MS        0x04

#endif

/**
 * @brief  Initialize Audio Capabilities parameter include PAC record,
 *         Audio Location, Context Type,...etc. Register ble audio
 *         callback to handle pacs message.
 * @param  No parameter.
 * @return void
 */
void app_lea_pacs_init(void);

/**@brief Get Sink audio context.
 *
 * @param   No parameter.
 * @return  uint16_t available contexts of sink.
 */
uint16_t app_lea_pacs_get_sink_available_contexts(void);

/**@brief Get Source audio context.
 *
 * @param   No parameter.
 * @return  uint16_t available contexts of source.
 */
uint16_t app_lea_pacs_get_source_available_contexts(void);

/**@brief A set of parameter values that denote low latency is using or not.
 *
 * @param[in] low_latency true:low latency, false:high reliability
 * @return    void.
 */
void app_lea_pacs_update_low_latency(bool low_latency);

/**@brief Initialize available context.
 *
 * @param   conn_handle ble connection handle.
 * \retval  true  success.
 * \retval  false fail.
 */
bool app_lea_pacs_init_available_context(uint16_t conn_handle);

/**@brief Require changing sink audio context.
 *
 * @param   conn_handle ble connection handle.
 * @param   sink_contexts using sink audio context.
 * @param   enable true: enable available context; false: disable available context.
 * \retval  true  success.
 * \retval  false fail.
 */
bool app_lea_pacs_set_sink_available_contexts(uint16_t conn_handle, uint16_t sink_contexts,
                                              bool enable);

/**@brief Require changing source audio context.
 *
 * @param   conn_handle ble connection handle.
 * @param   source_contexts using source audio context.
 * @param   enable true: enable available context; false: disable available context.
 * \retval  true  success.
 * \retval  false fail.
 */
bool app_lea_pacs_set_source_available_contexts(uint16_t conn_handle, uint16_t source_contexts,
                                                bool enable);

#if F_APP_LC3_PLUS_SUPPORT
bool app_lea_pacs_qos_preferred_cfg_get_by_codec(T_CODEC_CFG *p_cfg, uint8_t target_latency,
                                                 T_QOS_CFG_PREFERRED *p_qos, uint16_t conn_handle, uint8_t ase_id);
T_AUDIO_FORMAT_TYPE app_lea_pacs_check_codec_type(uint8_t codec_id[CODEC_ID_LEN]);
uint8_t app_lea_pacs_check_bit_depth(T_AUDIO_FORMAT_TYPE codec_type, uint8_t metadata_length,
                                     uint8_t *p_metadata);
T_AUDIO_LC3PLUS_MODE app_lea_pacs_get_mode(uint32_t sample_rate,
                                           T_AUDIO_LC3PLUS_FRAME_DURATION duration, uint16_t octets);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif

