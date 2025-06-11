#ifndef _EQ_EXT_FTL_H_
#define _EQ_EXT_FTL_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "flash_map.h"

#define EQ_EXT_FTL_PARTITION_NAME "EQ_FTL"

#define VOCIE_SPK_EQ_SIZE 0x800

#if (FLASH_SIZE >= 0x00400000)
#define AUDIO_AND_APT_EQ_SIZE 0x2800
#else
#define AUDIO_AND_APT_EQ_SIZE 0xC00
#endif

void eq_ext_ftl_storage_init(void);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
