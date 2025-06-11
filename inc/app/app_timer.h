/*
 * Copyright (c) 2017, Realtek Semiconductor Corporation. All rights reserved.
 */
#ifndef __SW_TIM_H
#define __SW_TIM_H

#include <stdint.h>
#include <stdbool.h>
#include "errno.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    APP_TIMER  APP Timer
 *
 * \brief   Register timeout callback functions, start and stop software timers.
 * \details The APP Timer Module allows to register timeout callback functions
 *          which will be called when timeout, also provides start/stop timer
 *          functions. APP Timer Module provides a method for APP to handle timeout
 *          events in its own task.\n
 */

/**
 * app_timer.h
 *
 * \brief   Prototype of callback functions other modules registered to handle timeout events.
 * \details Timer event and param are the same as those used in \ref app_start_timer.\n
 *
 * \ingroup  APP_TIMER
 */
typedef void (* P_TIMEOUT_CB)(uint8_t timer_evt, uint16_t param);

/**
 * app_timer.h
 *
 * \brief   Register a timeout callback function in APP Timer Module. A module ID is
 *          returned which will be used when start timer by calling \ref app_start_timer.
 *          When timeout event occurs, APP Timer Module will find the exact
 *          timeout function by module ID and execute it.
 *
 * \param[in]   callback         Timeout callback function. Callback functions
 *                               must have the prototype defined as \ref P_TIMEOUT_CB.
 *
 * \param[out]  p_module_id       Used to pass back a module ID that will be used when start timer.
 *
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 * \ingroup  APP_TIMER
 */
int32_t app_timer_reg_cb(P_TIMEOUT_CB callback, uint8_t *p_module_id);
/**
 * app_timer.h
 *
 * \brief   Create a new APP timer and start it. This allocates the storage required by
 *          the new timer, starts this timer if create successfully, and returns a handle
 *          by which the new timer can be referenced. <b>APP timer is always a one-shot timer</b>
 *
 * \param[out]  p_idx            Used to pass back an index by which the created timer
 *                               can be referenced.
 *
 * \param[in]   p_name           A descriptive name for the timer.
 *
 * \param[in]   module_id        Timer module ID assigned by APP Timer Module when register
 *                               timer callback function using \ref app_reg_timer_cb.
 *                               module ID would be used to identify which timeout function will be execute.
 *
 * \param[in]   timer_evt        Timer event assigned by module which starts this timer.
 *
 * \param[in]   param            Paramer assigned by module which start this timer.
 *                               Typically timer event and paramter would be used in the
 *                               timer callback function to identify which timer expired when
 *                               the same callback function is assigned to more than one timer.
 *
 * \param[in]   reload           Used to set the timer as a periodic or one-shot timer.
 *
 * \param[in]   timeout_ms       Timeout value in milliseconds.
 *
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 *
 * \ingroup  APP_TIMER
 */
int32_t app_start_timer(uint8_t *p_idx, const char *p_name,
                        uint8_t module_id, uint8_t timer_evt, uint16_t param, bool reload,  uint32_t timeout_ms);

/**
 * app_timer.h
 *
 * \brief   Stop a timer that was previously started using \ref app_start_timer.
 *
 * \param[in] p_idx  Pointer to the index of timer being stopped.
 *
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 *
 * \ingroup  APP_TIMER
 */
int32_t app_stop_timer(uint8_t *p_idx);


/**
 * app_timer.h
 *
 * \brief   Init APP Timer Module.
 *
 * \param[in]   evt_handle              Event queue handle which represents event queue created by APP task.
 * \param[in]   timer_module_max_num    Number of modules which will be create by APP timer Module.
 *
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 * \ingroup  APP_TIMER
 */
int32_t app_init_timer(void *evt_handle, uint8_t timer_module_num);

/**
 * app_timer.h
 *
 * \brief   Handle APP Timer event.
 *
 * \param[in]   event              Event received from event queue, which handle event EVENT_APP_TIMER_MSG.
 *
 * \return      None.
 *
 * \ingroup  APP_TIMER
 */
void app_timer_handle_msg(uint8_t event);

/**
 * app_timer.h
 *
 * \brief   Register timer for power manager not to check when try to enter low power mode.
 *
 * \param[in]   p_idx       Used to pass back an index by which the created timer
 *                          can be referenced.
 *
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 * \ingroup  APP_TIMER
 */
int32_t app_timer_register_pm_excluded(uint8_t *p_idx);

/**
 * app_timer.h
 *
 * \brief   Unregister timer for power manager not to check when try to enter low power mode.
 *
 * \param[in]   p_idx       Used to pass back an index by which the created timer
 *                          can be referenced.
 *
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 * \ingroup  APP_TIMER
 */
int32_t app_timer_unregister_pm_excluded(uint8_t *p_idx);

/**
 * app_timer.h
 *
 * \brief   get app timer state.
 *
 * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
 *
 * \param[in]   p_idx       Used to pass back an index by which the created timer
 *                          can be referenced.
 * \param[in]   p_timer_state    timer state.
 * @return      Operation result @ref error.h.
 * @retval      0 Operation success.
 * @retval      Others Operation failure.
 *
 * \ingroup  APP_TIMER
 */
int32_t app_timer_state_get(uint8_t *p_idx, uint32_t *p_timer_state);
#endif

