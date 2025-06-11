/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_IPC_H_
#define _APP_IPC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *T_APP_IPC_HANDLE;

typedef void (*P_APP_IPC_CBACK)(uint32_t event, void *msg);

bool app_ipc_init(void);

bool app_ipc_publish(const char *topic, uint32_t event, void *msg);

T_APP_IPC_HANDLE app_ipc_subscribe(const char *topic, P_APP_IPC_CBACK cback);

bool app_ipc_unsubscribe(T_APP_IPC_HANDLE handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_IPC_H_ */
