#ifndef _APP_LEA_CCP_DEF_H_
#define _APP_LEA_CCP_DEF_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#define CCP_CALL_STATE_CHARA_LEN 3
#define CCP_CALL_ENTRY_NUM 2 /** max CCP call entry num */

typedef struct t_lea_call_entry
{
    bool used;
    uint8_t call_index;
    uint8_t call_state;
    uint8_t *call_uri;
} T_LEA_CALL_ENTRY;

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
