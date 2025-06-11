/*
 * Copyright (c) 2015, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _OS_TIMER_H_
#define _OS_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    OS_87x3e_Timer   Timer Management
 *
 * \brief   Create and control software timer and timer callback functions.
 * \details The Timer Management function group allows to create, delete, and control software
 *          timers in the system. The software timers can be configured as one-short or periodic
 *          timers. When a timer expires, a callback function associated with the timer is
 *          executed.\n
 * Timers can be in the following two states:\n
 * \arg <b>Dormant</b> The timer first enters <b>Dormant</b> or <b>Inactive</b> state when created.
 *                     If a one-short timer expires but not restarts yet, or is stopped the timer
 *                     will be tranformed from <b>Active</b> state into <b>Dormant</b> state.
 * \arg <b>Active</b> The timer enters <b>Active</b> state If the timer starts or restarts. When
 *                    expired, the callback function associcated with the timer will be executed.
 *
 * \image html OS-timer-state-transition.jpg "Software Timer State Transition" width=617px height=321px
 *
 */


/**
 * os_timer.h
 *
 * \brief    Get the ID assigned to the timer when created.
 *
 * \param[in]   pp_handle   Pointer to the created timer handle.
 *
 * \param[out]  p_timer_id  Used to pass back the ID assigned to the timer.
 *
 * \return           The status of the timer id getting.
 * \retval true      Timer ID was got successfully.
 * \retval false     Timer ID was failed to get.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Delete the specified timer.
 *          os_timer_delete(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_id_get)(void **pp_handle, uint32_t *p_timer_id);

/**
 * os_timer.h
 *
 * \brief   Create a new software timer instance. This allocates the storage required by
 *          the new timer, initializes the new timers internal state, and returns a handle
 *          by which the new timer can be referenced.
 *
 * \param[out]  pp_handle        Used to pass back a handle by which the created timer
 *                               can be referenced.
 *
 * \param[in]   p_timer_name     A descriptive name for the timer.
 *
 * \param[in]   timer_id         An identifier that is assigned to the timer being created.
 *                               Typically this would be used in the timer callback function to
 *                               identify which timer expired when the same callback function is
 *                               assigned to more than one timer.
 *
 * \param[in]   interval_ms      The timer period in milliseconds.
 *
 * \param[in]   reload           Used to set the timer as a periodic or one-shot timer.
 * \arg \c      true             Create a periodic timer.
 * \arg \c      false            Create a one-shot timer.
 *
 * \param[in]   p_timer_callback The function to call when the timer expires. Callback functions
 *                               must have the prototype defined as 'void callback(void *)'.
 *
 * \return           The status of the timer creation.
 * \retval true      Timer was created successfully.
 * \retval false     Timer was failed to create.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Delete the specified timer.
 *          os_timer_delete(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_create)(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                               uint32_t interval_ms, bool reload, void (*p_timer_callback)(void *));

/**
 * os_timer.h
 *
 * \brief    Start a timer that was previously created using the os_timer_create() API
 *           function. If the timer had already been started and was in the active state,
 *           then os_timer_create() has equivalent functionality to the os_timer_create()
 *           API function.
 *
 * \param[in] pp_handle Pointer to the created timer handle.
 *
 * \return           The status of the timer starting.
 * \retval true      Timer was started successfully.
 * \retval false     Timer was failed to start.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Delete the specified timer.
 *          os_timer_delete(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_start)(void **pp_handle);

/**
 * os_timer.h
 *
 * \brief     Restart a timer that was previously created using the os_timer_create() API
 *            function. If the timer had already been started and was already in the active
 *            state, then os_timer_start() will cause the timer to re-evaluate its expiry
 *            time so that it is relative to when os_timer_start() was called. If the timer
 *            was in the dormant state then os_timer_start() has equivalent functionality to
 *            the os_timer_start() API function.
 *
 * \param[in] pp_handle     Pointer to the created timer handle.
 *
 * \param[in] interval_ms   The timer period in milliseconds.
 *
 * \return           The status of the timer restarting.
 * \retval true      Timer was restarted successfully.
 * \retval false     Timer was failed to restart.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Restart the specified timer.
 *          os_timer_restart(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_restart)(void **pp_handle, uint32_t interval_ms);

/**
 * os_timer.h
 *
 * \brief     Stop a timer that was previously started using either of the os_timer_start(),
 *            os_timer_restart() API functions. Stopping a timer ensures the timer is not in
 *            the active state.
 *
 * \param[in] pp_handle Pointer to the handle of timer being stopped.
 *
 * \return           The status of the timer stopping.
 * \retval true      Timer was stopped successfully.
 * \retval false     Timer was failed to stop.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Delete the specified timer.
 *          os_timer_delete(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created and stopped.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     // Stop the timer before expiration.
 *     os_timer_stop(&p_handle);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_stop)(void **pp_handle);

/**
 * os_timer.h
 *
 * \brief      Delete a timer that was previously created using the os_timer_create()
 *             API function.
 *
 * \param[in] pp_handle Pointer to the handle of timer being deleted.
 *
 * \return           The status of the timer deletion.
 * \retval true      Timer was deleted successfully.
 * \retval false     Timer was failed to delete.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Delete the specified timer.
 *          os_timer_delete(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created and deleted.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_delete)(void **pp_handle);

/**
 * os_timer.h
 *
 * \brief      Dump all current used timers in system.
 *
 * \param None
 *
 * \return           The status of the timer dump.
 * \retval true      Timer was dumped successfully.
 * \retval false     Timer was failed to dump.
 *
 * <b>Example usage</b>
 * \code{.c}
 * // Timer callback function.
 * void timer_callback(void *p_handle)
 * {
 *     uint32_t timer_id;
 *
 *     // Which timer expired?
 *     os_timer_id_get(&p_handle, &timer_id);
 *
 *     if (timer_id == TIMER_ID)
 *     {
 *          // Delete the specified timer.
 *          os_timer_delete(&p_handle);
 *     }
 * }
 *
 * #define TIMER_ID 1
 *
 * // Timer to be created and dumped.
 * int test(void)
 * {
 *     void *p_handle = NULL;
 *
 *     if (os_timer_create(&p_handle, "timer", TIMER_ID,
 *                        INTERVAL_MS, false, timer_callback) == true)
 *     {
 *         // Timer created successfully, start the timer.
 *         os_timer_start(&p_handle);
 *     }
 *     else
 *     {
 *         // Timer failed to create.
 *         return -1;
 *     }
 *
 *     // Dump all used timers.
 *     os_timer_dump();
 *
 *     return 0;
 * }
 * \endcode
 *
 * \ingroup  OS_87x3e_Timer
 */
extern bool (*os_timer_pend_function_call)(void (*p_pend_function)(void *, uint32_t),
                                           void *pvParameter1, uint32_t ulParameter2);
extern bool (*os_timer_get_timer_number)(void **pp_handle, uint8_t *p_timer_number);
extern bool (*os_timer_is_timer_active)(void **pp_handle);
extern bool (*os_timer_get_auto_reload)(void **pp_handle, long *p_autoreload);
extern bool (*os_timer_get_next_timeout_item)(void **pxListItem, unsigned long *puxListNum,
                                              uint8_t xListType);

extern bool (*os_timer_dump)(void);


bool os_timer_id_get_rom(void **pp_handle, uint32_t *p_timer_id);
bool os_timer_create_rom(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                         uint32_t interval_ms, bool reload, void (*p_timer_callback)(void *));
bool os_timer_start_rom(void **pp_handle);
bool os_timer_restart_rom(void **pp_handle, uint32_t interval_ms);
bool os_timer_stop_rom(void **pp_handle);
bool os_timer_delete_rom(void **pp_handle);
bool os_timer_pend_function_call_rom(void (*p_pend_function)(void *, uint32_t),
                                     void *pvParameter1, uint32_t ulParameter2);
bool os_timer_get_timer_number_rom(void **pp_handle, uint8_t *p_timer_number);
bool os_timer_is_timer_active_rom(void **pp_handle);
bool os_timer_dump_rom(void);
bool os_timer_get_auto_reload_rom(void **pp_handle, long *p_autoreload);
bool os_timer_get_next_timeout_item_rom(void **pxListItem, unsigned long *puxListNum,
                                        uint8_t xListType);

#ifdef __cplusplus
}
#endif

#endif /* _OS_TIMER_H_ */

