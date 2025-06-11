#ifndef FTL_EXT_H
#define FTL_EXT_H

#include <stdint.h>

#define FTL_DEPRECATED      __attribute__((deprecated))

FTL_DEPRECATED uint32_t ftl_partition_init(uint32_t u32PageStartAddr, uint8_t pagenum,
                                           uint32_t value_size,
                                           uint16_t *offset_min, uint16_t *offset_max);
#endif
