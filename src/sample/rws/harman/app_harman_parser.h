#ifndef _APP_HARMAN_PARSER_H_
#define _APP_HARMAN_PARSER_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_audio_policy.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_app_harman_parser_entry_event
{
    HARMAN_PARSER_WRITE_CMD = 0x01,
    HARMAN_REFINE_REPORT_CMD = 0x02,

} T_APP_HARMAN_PARSER_ENTRY_EVENT;

uint8_t app_harman_parser_process(uint8_t entry_point, void *msg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
