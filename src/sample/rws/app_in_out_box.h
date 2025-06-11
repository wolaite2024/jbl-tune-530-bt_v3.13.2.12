/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_IN_OUT_BOX_H_
#define _APP_IN_OUT_BOX_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    IN_CASE,
    OUT_CASE
} T_CASE_LOCATION_STATUS;

void app_in_out_box_init(void);
void app_in_out_box_handle(T_CASE_LOCATION_STATUS local);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_IN_OUT_BOX_H_ */
