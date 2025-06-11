#ifndef OS_HEAP_H
#define OS_HEAP_H

void heap_shm_set(uint32_t shm_addr, uint32_t shm_size, bool is_connect);
void heap_shm_init(uint32_t addr, uint32_t size);
void add_shm_to_continuous_tail(void);
#endif
