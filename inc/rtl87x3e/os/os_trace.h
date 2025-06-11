/*
 * Copyright (c) 2015, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _OS_TRACE_H_
#define _OS_TRACE_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup    OS_87x3e_Trace ISR trace Management
 *
 * \brief   Add isr trace information for traceAlyzer
 * \details traceAlyzer can not trace isr automatically. os_trace_isr_create() give a name to
 *          the isr. os_trace_isr_begin() & os_trace_isr_end() can determine
 *          the trace scope.
 *
 */


/**
 * os_trace.h
 *
 * \brief   Stores a name and priority level for an Interrupt Service Routine, to allow
 * for better visualization. Returns a handle used by os_trace_isr_begin.
 *
 * \param[out]  pp_handle  Used to pass back a handle by which the isr can be traced.
 *
 * \param[in]   p_name     A descriptive name for the isr.
 *
 * \param[in]   priority   the hardware priority of the isr.
 *
 * \return           The status of the isr trace creation.
 * \retval true      ISR trace handle was created successfully.
 * \retval false     ISR trace handle was failed to create.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define PRIO_ISR_TIMER1 3
 * void *p_handle = NULL;
 *
 * int test(void)
 * {
 *
 *     // Create a isr trace
 *     if (os_trace_isr_create(&p_handle, "isr name", PRIO_ISR_TIMER1) == true)
 *     {
 *         // isr_trace created successfully.
 *     }
 *     else
 *     {
 *         // isr_trace failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 *
 *  void ISR_handler()
 *   {
 *       os_trace_isr_begin(p_handle);
 *       ...
 *       os_trace_isr_end();
 *   }
 * \endcode
 *
 * \ingroup  OS_87x3e_Trace
 */
extern bool (*os_trace_isr_create)(void **pp_handle, const char *p_name, uint32_t priority);


/**
 * os_trace.h
 *
 * \brief   Registers the beginning of an Interrupt Service Routine, using a handle
 * provided by os_trace_isr_create.
 *
 * \param[out]  pp_handle  The handle of the isr to be traced.
 *
 * \return           The status of the isr trace creation.
 * \retval true      ISR trace started successfully.
 * \retval false     ISR trace started failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define PRIO_ISR_TIMER1 3
 * void *p_handle = NULL;
 *
 * int test(void)
 * {
 *
 *     // Create a isr trace
 *     if (os_trace_isr_create(&p_handle, "isr name", PRIO_ISR_TIMER1) == true)
 *     {
 *         // isr_trace created successfully.
 *     }
 *     else
 *     {
 *         // isr_trace failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 *
 *  void ISR_handler()
 *   {
 *
 *       os_trace_isr_begin(p_handle);
 *       ...
 *       os_trace_isr_end();
 *   }
 * \endcode
 *
 * \ingroup  OS_87x3e_Trace
 */
extern bool (*os_trace_isr_begin)(void *pp_handle);

/**
 * os_trace.h
 *
 * \brief   Registers the end of an Interrupt Service Routine.
 *
 * \return           The status of the isr trace creation.
 * \retval true      ISR trace ended successfully.
 * \retval false     ISR trace ended failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define PRIO_ISR_TIMER1 3
 * void *p_handle = NULL;
 *
 * int test(void)
 * {
 *
 *     // Create a isr trace
 *     if (os_trace_isr_create(&p_handle, "isr name", PRIO_ISR_TIMER1) == true)
 *     {
 *         // isr_trace created successfully.
 *     }
 *     else
 *     {
 *         // isr_trace failed to create.
 *         return -1;
 *     }
 *
 *     return 0;
 * }
 *
 *  void ISR_handler()
 *   {
 *
 *       os_trace_isr_begin(p_handle);
 *       ...
 *       os_trace_isr_end();
 *   }
 * \endcode
 *
 * \ingroup  OS_87x3e_Trace
 */
extern bool (*os_trace_isr_end)(void);

bool os_trace_isr_create_rom(void **pp_handle, const char *p_name, uint32_t priority);
bool os_trace_isr_begin_rom(void *pp_handle);
bool os_trace_isr_end_rom(void);

#ifdef __cplusplus
}
#endif

#endif /* _OS_TRACE_H_ */
