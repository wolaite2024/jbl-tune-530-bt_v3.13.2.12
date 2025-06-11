#include <stdint.h>
#include <stdbool.h>
#include "audio_track.h"
#include "audio_type.h"
#include "app_dsp_cfg.h"

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
 * \brief   Attach sidetone instance.
 *
 * \param[in]  audio_track_handle   audio track handle
 * \param[in]  sidetone_cfg         sidetone config
 *
 * \return  Created Sidetone instance.
 */
T_AUDIO_EFFECT_INSTANCE app_sidetone_attach(T_AUDIO_TRACK_HANDLE handle,
                                            T_APP_DSP_CFG_SIDETONE sidetone_cfg);

/**
 * \brief   detach sidetone instance.
 *
 * \param[in]  audio_track_handle   audio track handle
 * \param[in]  sidetone instance    sidetone instance
 */
void app_sidetone_detach(T_AUDIO_TRACK_HANDLE handle, T_AUDIO_EFFECT_INSTANCE sidetone_instance);

