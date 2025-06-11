#ifndef _HA_EXT_FTL_H_
#define _HA_EXT_FTL_H_

#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define HA_SIZE    (0x1000)

void ha_ext_ftl_storage_init(void);
uint32_t ha_ext_ftl_load_data(void *data, uint32_t offset, uint32_t len);
uint32_t ha_ext_ftl_save_data(void *data, uint32_t offset, uint32_t len);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
