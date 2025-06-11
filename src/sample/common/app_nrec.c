#include <string.h>
#include "app_nrec.h"
#include "app_cfg.h"
#include "nrec.h"
#include "remote.h"

#if F_APP_VOICE_NREC_SUPPORT
T_AUDIO_EFFECT_INSTANCE app_nrec_attach(T_AUDIO_TRACK_HANDLE handle, bool enable)
{
    T_AUDIO_EFFECT_INSTANCE nrec_instance;

    nrec_instance = nrec_create(NREC_CONTENT_TYPE_VOICE, NREC_MODE_HIGH_SOUND_QUALITY, 0);

    if (nrec_instance != NULL)
    {
        if (enable)
        {
            nrec_enable(nrec_instance);
        }
        else
        {
            nrec_disable(nrec_instance);
        }

        audio_track_effect_attach(handle, nrec_instance);

        return nrec_instance;
    }

    return NULL;
}

void app_nrec_detach(T_AUDIO_TRACK_HANDLE handle, T_AUDIO_EFFECT_INSTANCE nrec_instance)
{
    if (nrec_instance != NULL)
    {
        audio_track_effect_detach(handle, nrec_instance);
        nrec_disable(nrec_instance);
        nrec_release(nrec_instance);
    }

    nrec_instance = NULL;
}

void app_nrec_roleswap(T_AUDIO_EFFECT_INSTANCE nrec_instance, bool sec_mic_enable)
{
    if (nrec_instance != NULL)
    {
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY || sec_mic_enable)
        {
            nrec_enable(nrec_instance);
        }
        else
        {
            nrec_disable(nrec_instance);
        }
    }
}

#else
T_AUDIO_EFFECT_INSTANCE app_nrec_attach(T_AUDIO_TRACK_HANDLE handle, bool enable)
{
    return NULL;
}

void app_nrec_detach(T_AUDIO_TRACK_HANDLE handle, T_AUDIO_EFFECT_INSTANCE nrec_instance)
{
    return;
}

void app_nrec_roleswap(T_AUDIO_EFFECT_INSTANCE nrec_instance, bool sec_mic_enable)
{
    return;
}
#endif
