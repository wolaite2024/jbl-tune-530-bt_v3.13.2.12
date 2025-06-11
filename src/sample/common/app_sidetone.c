#include <string.h>
#include "app_sidetone.h"
#include "sidetone.h"

#if F_APP_SIDETONE_SUPPORT
T_AUDIO_EFFECT_INSTANCE app_sidetone_attach(T_AUDIO_TRACK_HANDLE handle,
                                            T_APP_DSP_CFG_SIDETONE sidetone_cfg)
{
    T_AUDIO_EFFECT_INSTANCE sidetone_instance;

    if (sidetone_cfg.hw_enable)
    {
        sidetone_instance = sidetone_create(SIDETONE_TYPE_HW, sidetone_cfg.gain, sidetone_cfg.hpf_level);
        if (sidetone_instance != NULL)
        {
            sidetone_enable(sidetone_instance);
            audio_track_effect_attach(handle, sidetone_instance);

            return sidetone_instance;
        }
    }

    return NULL;
}

void app_sidetone_detach(T_AUDIO_TRACK_HANDLE handle, T_AUDIO_EFFECT_INSTANCE sidetone_instance)
{
    if (sidetone_instance != NULL)
    {
        audio_track_effect_detach(handle, sidetone_instance);
        sidetone_disable(sidetone_instance);
        sidetone_release(sidetone_instance);
    }

    sidetone_instance = NULL;
}

#else
T_AUDIO_EFFECT_INSTANCE app_sidetone_attach(T_AUDIO_TRACK_HANDLE handle,
                                            T_APP_DSP_CFG_SIDETONE sidetone_cfg)
{
    return NULL;
}

void app_sidetone_detach(T_AUDIO_TRACK_HANDLE handle, T_AUDIO_EFFECT_INSTANCE sidetone_instance)
{
    return;
}
#endif
