#ifndef _BT_EXT_FTL_H_
#define _BT_EXT_FTL_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#define BT_EXT_FTL_BLOCK_LEN     128
#define BT_EXT_FTL_PARTITION_NAME "BT_FTL"

#define BT_EXT_FTL_SIZE 0x2000

extern uint16_t bt_ext_ftl_start_offset;
extern uint16_t bt_ext_ftl_max_offset;

bool bt_ext_ftl_init(void);
bool bt_ext_ftl_deinit(void);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
