/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PROBE_H_
#define _AUDIO_PROBE_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_PROBE Audio Probe
 *
 * \brief   Test, analyze and make profilings on the Audio subsystem.
 *
 * \details Audio Probe provides the testing or profiling interfaces, which applications can analyze
 *          the Audio subsystem.
 *
 * \note    Probe interfaces are used only for testing and profiling. Applications shall not use Probe
 *          interfaces in any formal product scenarios.
 */

/**
 * audio_probe.h
 *
 * \brief   Define probe event from DSP.
 *
 * \note    Only provide PROBE event for APP using.
 *
 * \ingroup AUDIO_PROBE
 */

typedef enum
{
    PROBE_SCENARIO_STATE             = 0x00,
    PROBE_EAR_FIT_RESULT             = 0x01,
    PROBE_SDK_GENERAL_CMD            = 0x02,
    PROBE_SDK_BOOT_DONE              = 0x03,
    PROBE_HA_VER_INFO                = 0x04,
    PROBE_SEG_SEND_REQ_DATA          = 0x05,
    PROBE_SEG_SEND_ERROR             = 0x06,
    PROBE_SYNC_REF_REQUEST           = 0x07,
    PROBE_REPORT_ALGO_STATUS         = 0x08,
} T_AUDIO_PROBE_EVENT;

typedef enum
{
    AUDIO_PROBE_DSP_EVT_INIT_FINISH                     = 0x00,
    AUDIO_PROBE_DSP_EVT_POWER_OFF                       = 0x01,
    AUDIO_PROBE_DSP_EVT_POWER_ON                        = 0x02,
    AUDIO_PROBE_DSP_EVT_EXCEPTION                       = 0x03,
    AUDIO_PROBE_DSP_EVT_REQ                             = 0x04,
    AUDIO_PROBE_DSP_EVT_READY                           = 0x05,
    AUDIO_PROBE_DSP_EVT_REQUEST_EFFECT                  = 0x06,
    AUDIO_PROBE_DSP_EVT_FADE_OUT_FINISH                 = 0x07,
    AUDIO_PROBE_DSP_EVT_SPORT0_READY                    = 0x08,
    AUDIO_PROBE_DSP_EVT_SPORT0_STOP                     = 0x09,
    AUDIO_PROBE_DSP_EVT_SPORT1_READY                    = 0x0a,
    AUDIO_PROBE_DSP_EVT_SPORT1_STOP                     = 0x0b,
    AUDIO_PROBE_DSP_EVT_SPORT_STOP_FAKE                 = 0x0c,
    AUDIO_PROBE_DSP_EVT_SPORT_START_FAKE                = 0x0d,
    AUDIO_PROBE_DSP_EVT_CODEC_STATE                     = 0x0e,
    AUDIO_PROBE_DSP_MGR_EVT_FW_READY                    = 0x0f,
    AUDIO_PROBE_DSP_MGR_EVT_FW_STOP                     = 0x10,
    AUDIO_PROBE_DSP_EVT_MAILBOX_DSP_DATA                = 0x11,
    AUDIO_PROBE_DSP_MGR_EVT_NOTIFICATION_FINISH         = 0x12,
    AUDIO_PROBE_DSP_MGR_EVT_OPEN_AIR_AVC                = 0x21,
    AUDIO_PROBE_DSP_MGR_EVT_HARMAN_DEFAULT_EQ           = 0x22,
} T_AUDIO_PROBE_DSP_EVENT;

typedef enum
{
    PROBE_ALGO_PROPRIETARY_VOICE,    //00
    PROBE_ALGO_G711_ALAW,            //01
    PROBE_ALGO_CVSD,                 //02
    PROBE_ALGO_MSBC,                 //03
    PROBE_ALGO_OPUS_VOICE,           //04
    PROBE_ALGO_UHQ_VOICE,            //05
    PROBE_ALGO_USB_SPEECH,           //06
    PROBE_ALGO_SBC,                  //07
    PROBE_ALGO_AAC,                  //08
    PROBE_ALGO_PROPRIETARY_AUDIO1,   //09
    PROBE_ALGO_PROPRIETARY_AUDIO2,   //10
    PROBE_ALGO_MP3,                  //11
    PROBE_ALGO_USB_AUDIO,            //12
    PROBE_ALGO_SUB_WOOFER,           //13
    PROBE_ALGO_LDAC,                 //14 SONY
    PROBE_ALGO_UHQ,                  //15 Samsung
    PROBE_ALGO_LHDC,                 //16 Savitech
    PROBE_ALGO_OPUS_AUDIO,           //17
    PROBE_ALGO_PROPRIETARY_AUDIO3,   //18
    PROBE_ALGO_PURE_STREAM,          //19, ALGORITHM_PROPRIETARY_AUDIO4
    PROBE_ALGO_LINE_IN,              //20, MUST before ALGORITHM_END
    PROBE_ALGO_END                   //21
} T_ALGO_TYPE;

typedef struct
{
    uint8_t algo;
    uint8_t category;
} T_AUDIO_PROBE_DSP_EVENT_MSG;

/**
 * audio_probe.h
 *
 * \brief   Define probe cmd id to DSP.
 *
 * \note    Should be referenced to h2d_cmd.
 *
 * \ingroup AUDIO_PROBE
 */
typedef enum
{
    AUDIO_PROBE_TEST_MODE       = 0x02,
    AUDIO_PROBE_UPLINK_SYNCREF  = 0x5E,
    AUDIO_PROBE_SEG_SEND        = 0x63,
    AUDIO_PROBE_HA_PARA         = 0x70,
} T_AUDIO_PROBE_DSP_CMD;

/**
 * audio_probe.h
 *
 * \brief Define DSP probe callback prototype.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in]   event   event for dsp \ref T_AUDIO_PROBE_EVENT.
 * \param[in]   buf     The buffer that holds the DSP probe data.
 *
 * \ingroup AUDIO_PROBE
 */
typedef void (*P_AUDIO_PROBE_DSP_CABCK)(T_AUDIO_PROBE_EVENT event, void *buf);

/**
 * audio_probe.h
 *
 * \brief Define DSP ststus probe callback prototype.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in]   event   event for dsp \ref T_AUDIO_PROBE_DSP_EVENT_MSG.
 * \param[in]   msg     The msg that holds the DSP status.
 *
 * \ingroup AUDIO_PROBE
 */
typedef void (*P_AUDIO_PROBE_DSP_EVENT_CABCK)(uint32_t event, void *msg);

typedef bool (*P_SYS_IPC_CBACK)(uint32_t id, void *msg);

typedef void *T_SYS_IPC_HANDLE;

/**
 * audio_probe.h
 *
 * \brief   Register DSP probe callback.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The DSP probe callback \ref P_AUDIO_PROBE_DSP_CABCK.
 *
 * \return          The status of registering DSP probe callback.
 * \retval true     DSP probe data was registered successfully.
 * \retval false    DSP probe data was failed to register.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_cback_register(P_AUDIO_PROBE_DSP_CABCK cback);

/**
 * audio_probe.h
 *
 * \brief   Unregister DSP probe callback.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The DSP probe callback \ref P_AUDIO_PROBE_DSP_CABCK.
 *
 * \return          The status of unregistering DSP probe callback.
 * \retval true     DSP probe data was unregistered successfully.
 * \retval false    DSP probe data was failed to unregister.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_cback_unregister(P_AUDIO_PROBE_DSP_CABCK cback);

/**
 * audio_probe.h
 *
 * \brief   Send probe data to DSP.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] buf   The probing data buffer address.
 * \param[in] len   The probing data buffer length.
 *
 * \return          The status of sending DSP probe data.
 * \retval true     DSP probe data was sent successfully.
 * \retval false    DSP probe data was failed to send.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_send(uint8_t *buf, uint16_t len);

/**
 * audio_probe.h
 *
 * \brief   Set codec hardware EQ.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] eq_type   Hardware EQ type.
 * \param[in] eq_chann  Hardware EQ channel.
 * \param[in] buf       Hardware EQ band parameter buffer.
 * \param[in] len       Hardware EQ band parameter length.
 *
 * \return          The status of setting codec hardware EQ.
 * \retval true     Codec hardware EQ was setting successfully.
 * \retval false    Codec hardware EQ was failed to set.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_codec_hw_eq_set(uint8_t eq_type, uint8_t eq_chann, uint8_t *buf, uint16_t len);

bool audio_probe_send_dsp_sdk_data(uint8_t *p_data, uint16_t data_len);

/**
 * audio_probe.h
 *
 * \brief   Set voice primary mic.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] mic_sel   mic selection(0: Dmic 1, 1: Dmic 2, 2: Amic 1, 3: Amic 2, 4: Amic 3, 5: Amic 4, 6: Dmic 3, 7: Mic sel disable).
 * \param[in] mic_type  mic type(0: single-end AMIC/falling DMIC, 1: differential AMIC/raising DMIC).
 *
 * \ingroup AUDIO_PROBE
 */
void audio_probe_set_voice_primary_mic(uint8_t mic_sel, uint8_t mic_type);

/**
 * audio_probe.h
 *
 * \brief   Set voice secondary mic.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] mic_sel   mic selection(0: Dmic 1, 1: Dmic 2, 2: Amic 1, 3: Amic 2, 4: Amic 3, 5: Amic 4, 6: Dmic 3, 7: Mic sel disable).
 * \param[in] mic_type  mic type(0: single-end AMIC/falling DMIC, 1: differential AMIC/raising DMIC).
 *
 * \ingroup AUDIO_PROBE
 */
void auido_probe_set_voice_secondary_mic(uint8_t mic_sel, uint8_t mic_type);

/**
 * audio_probe.h
 *
 * \brief   Get voice primary mic selection.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \ingroup AUDIO_PROBE
 */
uint8_t audio_probe_get_voice_primary_mic_sel(void);

/**
 * audio_probe.h
 *
 * \brief   Get voice primary mic type.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \ingroup AUDIO_PROBE
 */
uint8_t audio_probe_get_voice_primary_mic_type(void);

/**
 * audio_probe.h
 *
 * \brief   Get voice secondary mic selection.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \ingroup AUDIO_PROBE
 */
uint8_t audio_probe_get_voice_secondary_mic_sel(void);

/**
 * audio_probe.h
 *
 * \brief   Get voice secondary mic type.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \ingroup AUDIO_PROBE
 */
uint8_t audio_probe_get_voice_secondary_mic_type(void);

/**
 * audio_probe.h
 *
 * \brief   Register DSP status probe callback.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The DSP probe callback \ref P_AUDIO_PROBE_DSP_EVENT_CABCK.
 *
 * \return          The status of registering DSP probe callback.
 * \retval true     DSP probe data was registered successfully.
 * \retval false    DSP probe data was failed to register.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_evt_cback_register(P_AUDIO_PROBE_DSP_EVENT_CABCK cback);


/**
 * audio_probe.h
 *
 * \brief   Unregister DSP status callback.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The DSP probe callback \ref P_AUDIO_PROBE_DSP_EVENT_CABCK.
 *
 * \return          The status of unregistering DSP probe callback.
 * \retval true     DSP probe data was unregistered successfully.
 * \retval false    DSP probe data was failed to unregister.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_evt_cback_unregister(P_AUDIO_PROBE_DSP_EVENT_CABCK cback);

bool audio_probe_dsp_ipc_send_ha_param(uint8_t *payload_data, uint16_t payload_len);
/**
 * audio_probe.h
 *
 * \brief   malloc ram from playback buffer.
 *
 * \note
 *
 * \ingroup AUDIO_PROBE
 */
void *audio_probe_media_buffer_malloc(uint16_t buf_size);

/**
 * audio_probe.h
 *
 * \brief   free ram to playback buffer.
 *
 * \note
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_media_buffer_free(void *p_buf);

bool audio_probe_dsp_ipc_send_call_status(bool enable);

bool audio_probe_send_sepc_info(uint8_t parameter, bool action, uint8_t para_chanel_out);

void audio_probe_set_sepc_info(void);

/**
 * audio_probe.h
 *
 * \brief   Control loading DSP test bin.
 *
 * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
 *
 * \param[in] enable    Enable/disable loading DSP test bin.
 *
 * \ingroup AUDIO_PROBE
 */
void audio_probe_dsp_test_bin_set(bool enable);

/**
 * audio_probe.h
 *
 * \brief   disable dsp power down.
 *
 * \note
 *
 * \return
 * \retval  zero
 * \retval  non-zero
 *
 * \ingroup AUDIO_PROBE
 */
void audio_probe_disable_dsp_powerdown(void);

/**
 * audio_probe.h
 *
 * \brief   start the engine communication w/ DSP2.
 *
 * \note
 *
 * \return              engine instance ID
 * \retval  zero        engine start fail
 * \retval  non-zero    engine start success
 *
 * \ingroup AUDIO_PROBE
 */
uint32_t engine_start(void);

/**
 * audio_probe.h
 *
 * \brief   stop the engine communication w/ DSP2.
 *
 * \note
 *
 * \return
 * \retval  zero        engine stop success
 * \retval  non-zero    engine stop fail, the retval is error code
 *
 * \ingroup AUDIO_PROBE
 */
int32_t engine_stop(uint32_t instance);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PROBE_H_ */
