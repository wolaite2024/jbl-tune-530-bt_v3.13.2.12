#include <stdint.h>
#include <stdbool.h>
#include "audio_track.h"
#include "audio_type.h"

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
 * \brief   Attach Noise Reduction & Echo Cancelation(NREC) instance.
 *
 * \param[in]  Audio_track_handle   audio track handle
 * \param[in]  Enable               NREC enable
 *
 * \return  Created Voice nrec instance.
 */
T_AUDIO_EFFECT_INSTANCE app_nrec_attach(T_AUDIO_TRACK_HANDLE handle, bool enable);

/**
 * \brief   Detach Noise Reduction & Echo Cancelation(NREC) instance.
 *
 * \param[in]  Audio_track_handle   Audio track handle
 * \param[in]  NREC instance        NREC instance
 */
void app_nrec_detach(T_AUDIO_TRACK_HANDLE handle, T_AUDIO_EFFECT_INSTANCE nrec_instance);

/**
 * \brief   Roleswap NREC effect
 *
 * \param[in]  NREC instance        NREC instance
 * \param[in]  sec_mic_enable       Secondary mic enable
 */
void app_nrec_roleswap(T_AUDIO_EFFECT_INSTANCE nrec_instance, bool sec_mic_enable);


