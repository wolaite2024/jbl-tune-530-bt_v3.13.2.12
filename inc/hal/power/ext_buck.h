/*******************************************************************************
* Copyright (C) 2021 Realtek Semiconductor Corporation. All Rights Reserved.
*/

/**
 * @file
 *
 */

#ifndef EXT_BUCK_H
#define EXT_BUCK_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _EXT_BUCK_T
{
    bool (*ext_buck_hw_init)(void);
    bool (*ext_buck_set_voltage)(uint32_t volt_h_uv);
    bool (*ext_buck_enable)(void);
    bool (*ext_buck_disable)(void);
} EXT_BUCK_T;

void ext_buck_init(EXT_BUCK_T *ext_buck);
bool ext_buck_hw_init(void);
bool ext_buck_vcore2_enable(void);
bool ext_buck_vcore2_disable(void);

#endif /* EXT_BUCK_H */
