#ifndef _GATTC_TBL_STORAGE_H_
#define _GATTC_TBL_STORAGE_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "bt_ext_ftl.h"
#include "gap_callback_le.h"

#if GATTC_TBL_STORAGE_SUPPORT

#define GATTC_FTL_MAX_DEV_NUM   2

void gattc_tbl_storage_init(void);
void gattc_tbl_storage_handle_bond_modify(T_LE_BOND_MODIFY_INFO *p_info);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
