/**
****************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
****************************************************************************************************
* @file     test_mode.h
* @brief    test mode definition.
* @author   Serval Li
* @date     2017-08-29
* @version  v1.0
****************************************************************************************************
*/

#ifndef _TEST_MODE_H_
#define _TEST_MODE_H_

#include <stdbool.h>
#include "wdg.h"

#ifdef __cplusplus
extern "C" {
#endif


bool is_single_tone_test_mode(void);

void switch_into_single_tone_test_mode(void);

void reset_single_tone_test_mode(void);

void set_hci_mode_flag(bool enable);

bool check_hci_mode_flag(void);

void switch_into_hci_mode(void);


#ifdef __cplusplus
}
#endif

#endif /* _TEST_MODE_H_ */

