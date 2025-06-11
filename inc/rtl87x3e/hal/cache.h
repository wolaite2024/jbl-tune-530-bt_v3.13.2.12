#ifndef __CACHE_H
#define __CACHE_H

#include <stdint.h>

void cache_enable(void);
void cache_disable(void);
void cache_flush_by_addr(uint32_t *addr, uint32_t length);
/**
 * @brief flash cache init.
 * @param do_cache_flush not used just for 3in1
 * @return
*/
void cache_hit_init(bool do_cache_flush);

/**
 * @brief get cache hit rate *100.
 *
 * @return cache hit rate *100
*/
uint32_t cache_hit_get(void);
#endif

