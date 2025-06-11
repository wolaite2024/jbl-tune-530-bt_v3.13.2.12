/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PLUGIN_H_
#define _AUDIO_PLUGIN_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_PLUGIN Audio Plugin
 *
 * \brief   Customize the Audio Plugin configurations.
 */

/**
 * audio_plugin.h
 *
 * \brief Define the Audio Plugin instance handle, created by \ref audio_plugin_create.
 *
 * \ingroup AUDIO_PLUGIN
 */
typedef void *T_AUDIO_PLUGIN_HANDLE;

/**
 * audio_plugin.h
 *
 * \brief Define the Audio Plugin parameter type, used in \ref P_AUDIO_PLUGIN_ACTION_HANDLER.
 *
 * \ingroup AUDIO_PLUGIN
 */
typedef struct t_audio_plugin_param
{
    uint32_t dac_sample_rate;
    uint32_t adc_sample_rate;
} T_AUDIO_PLUGIN_PARAM;

/**
 * audio_plugin.h
 *
 * \brief Define Audio Plugin action handler prototype.
 *
 * \details Each Audio Plugin can do some user-implemented action at occasions defined by \ref T_AUDIO_PLUGIN_OCCASION.
 *          The prototype of action handler is defined by \ref P_AUDIO_PLUGIN_ACTION_HANDLER. The action
 *          handler is registered to Audio Plugin within \ref T_AUDIO_PLUGIN_POLICY when creating an Audio
 *          Plugin instance by \ref audio_plugin_create.
 *
 * \note    The Audio Plugin action is ALWAYS treated as async within audio plugin, no matter whether its
 *          implementation deals with timer or not. This means the Audio Plugin will not know when the action is done
 *          until the same context is sent back by user application through interface \ref audio_plugin_msg_send.
 *
 * \param[in]     handle       The Audio Plugin instance handle \ref T_AUDIO_PLUGIN_HANDLE.
 * \param[in]     param        The parameter for the Audio Plugin action handler.
 * \param[in]     context      The context for the Audio Plugin instance to use in \ref audio_plugin_msg_send.
 *
 *
 * \ingroup AUDIO_PLUGIN
 */
typedef void (*P_AUDIO_PLUGIN_ACTION_HANDLER)(T_AUDIO_PLUGIN_HANDLE handle,
                                              T_AUDIO_PLUGIN_PARAM  param,
                                              void                 *context);

/**
 * audio_plugin.h
 *
 * \brief Define Audio Plugin occasions.
 *
 * \ingroup AUDIO_PLUGIN
 */
typedef enum t_audio_plugin_occasion
{
    AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_ON,
    AUDIO_PLUGIN_OCCASION_ANALOG_DOMAIN_OFF,
    AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_ON,
    AUDIO_PLUGIN_OCCASION_DIGITAL_DOMAIN_OFF,
    AUDIO_PLUGIN_OCCASION_NUM,
} T_AUDIO_PLUGIN_OCCASION;

/**
 * audio_plugin.h
 *
 * \brief Define Audio Plugin policies.
 *
 * \ingroup AUDIO_PLUGIN
 */
typedef struct t_audio_plugin_policy
{
    T_AUDIO_CATEGORY              category;
    T_AUDIO_PLUGIN_OCCASION       occasion;
    P_AUDIO_PLUGIN_ACTION_HANDLER handler;
} T_AUDIO_PLUGIN_POLICY;

/**
 * audio_plugin.h
 *
 * \brief   Create an Audio Plugin instance handle by registering operations and usage policies.
 *
 * \param[in] policies    Array of Audio Plugin instance usage policies \ref T_AUDIO_PLUGIN_POLICY.
 * \param[in] count       Number of elements in array policies.
 * \return  The instance handle of Audio Plugin instance. If returned handle is NULL, the Audio Plugin
 *          instance was failed to create.
 *
 * \ingroup AUDIO_PLUGIN
 */
T_AUDIO_PLUGIN_HANDLE audio_plugin_create(const T_AUDIO_PLUGIN_POLICY *policies,
                                          uint32_t                     count);

/**
 * audio_plugin.h
 *
 * \brief   Release the Audio Plugin instance.
 *
 * \param[in] handle       Audio Plugin instance handle returned by \ref audio_plugin_create.
 * \return                 The result of releasing the Audio Plugin instance.
 * \retval    true         Audio Plugin instance was released successfully.
 * \retval    false        Audio Plugin instance was failed to release.
 *
 * \ingroup AUDIO_PLUGIN
 */
bool audio_plugin_release(T_AUDIO_PLUGIN_HANDLE handle);

/**
 * audio_plugin.h
 *
 * \brief   User application sends message to Audio Plugin to indicate the action has been executed.
 *
 * \param[in] handle        Audio Plugin handle \ref T_AUDIO_PLUGIN_HANDLE.
 * \param[in] context       The context parameter in \ref P_AUDIO_PLUGIN_ACTION_HANDLER.
 *
 * \ingroup AUDIO_PLUGIN
 */
void audio_plugin_msg_send(T_AUDIO_PLUGIN_HANDLE   handle,
                           void                   *context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PLUGIN_H_ */
