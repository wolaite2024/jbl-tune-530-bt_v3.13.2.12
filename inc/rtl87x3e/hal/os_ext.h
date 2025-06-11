#ifndef OS_EXT_H
#define OS_EXT_H

typedef long BaseType_t;
typedef void (*PendedFunction_t)(void *, uint32_t);

void monitor_memory_and_timer(uint16_t heap_and_timer_monitor_timer_timeout_s);
void *os_timer_handle_get(uint8_t id);
uint8_t os_timer_index_get(void *timer);
bool os_timer_state_get(void **pp_handle, uint32_t *p_timer_state);
void os_portyield_from_isr(BaseType_t x);
BaseType_t xTimerPendFunctionCallFromISR(PendedFunction_t xFunctionToPend, void *pvParameter1,
                                         uint32_t ulParameter2, BaseType_t *pxHigherPriorityTaskWoken);
#endif
