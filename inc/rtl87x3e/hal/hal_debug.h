
#ifndef HAL_DEBUG_H
#define HAL_DEBUG_H
/** @defgroup  HAL_DEBUG    hal debug
    * @brief hal debug api
    * @{
    */
/** @defgroup HAL_DEBUG_Exported_Macros HAL Debug Exported Macros
    * @brief
    * @{
    */
#define F_HAL_DEBUG_TASK_SCHEDULING         1
#define F_HAL_DEBUG_HIT_RATE_PRINT          1
#define F_HAL_DEBUG_HEAP_USAGE_INFO         1
#define F_HAL_DEBUG_PC_SAMPLING             1
#define F_HAL_DEBUG_QUEUE_USAGE             1
#define F_HAL_DEBUG_TASK_TIME_PROPORTION    0
#define F_HAL_DEBUG_HW_TIMER_IRQ            1
#define F_HAL_DEBUG_MALLOC_DEBUG_INFO       0
#define F_HAL_DEBUG_DBG_MONITOR_CB_ENABLE   0
/** End of HAL_DEBUG_Exported_Macros
    * @}
    */
/** @defgroup HAL_Debug_Exported_Functions Hal Debug Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/** @brief This CB is used for registering cb in debug monitor isr */
typedef void (* HAL_DEBUG_DEBUG_MONITOR_CB)(void);
/**
    * @brief  register app debug timer callback function
    * @note   app_init_timer should be called before hal_debug_init.
    * @param  void
    * @return void
    */
void hal_debug_init(void);
/**
    * @brief  allocate task information record buffer
    * @param  task_num specify the max task count to record task information
    * @return void
    */
void hal_debug_task_schedule_init(uint32_t task_num);
/**
    * @brief  print task name and task context switch out time
    * @param  void
    * @return void
    */
void hal_debug_print_task_info(void);
/**
    * @brief init cache hit and debug timer
    * @param  period_ms specify cache hit rate period
    * @return void
    */
void hal_debug_cache_hit_count_init(uint32_t period_ms);
/**
    * @brief  trigger raw memory dump
    * @param  void
    * @return void
    */
void hal_debug_memory_dump(void);
/**
    * @brief  set queue handler to monitor and init debug timer
    * @param  period_ms specify queue usage statistic period
    * @param  queue_handle1 specify the monitor message queue handle 1
    * @param  queue_handle2 specify the monitor message queue handle 2
    * @param  queue_handle3 specify the monitor message queue handle 3
    * @return void
    */
void hal_debug_msg_queue_usage_monitor(uint32_t period_ms, void *queue_handle1, void *queue_handle2,
                                       void *queue_handle3);
/**
    * @brief  init hal debug task time statistic proportion environment
    * @param  period_ms specify task statistic proportion period
    * @return void
    */
void hal_debug_task_time_proportion_init(uint32_t period_ms);
/**
    * @brief  print pc/lr sampling record data
    * @param  void
    * @return void
    */
void hal_debug_print_pc_sampling(void);
/**
    * @brief  init hal debug pc sampling environment
    * @param  num specify the pc sampling count
    * @param  period_ms specify pc sampling period
    * @return void
    */
void hal_debug_pc_sampling_init(uint8_t num, uint32_t period_ms);
/**
    * @brief  debug the hw timer timeout time
    * @param  void
    * @return void
    */
void hal_debug_hw_timer_irq_init(void);
/**
    * @brief  register cb in debug monitor isr
    * @param  cb register in debug monitor isr
    * @return void
    */
void hal_debug_debug_monitor_cb_register(HAL_DEBUG_DEBUG_MONITOR_CB cb);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_Debug_Exported_Functions */
/** @} */ /* End of group HAL_DEBUG */
#endif
