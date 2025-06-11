/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_KEY_GPIO_H_
#define _APP_KEY_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_KEY_GPIO App Key GPIO
  * @brief App Key GPIO
  * @{
  */

/**  @brief  Define key GPIO data for saving key press or release and key status information */
typedef struct
{
    uint8_t key_press[MAX_KEY_NUM];         /**< key press or key release */
    uint8_t key_status[MAX_KEY_NUM];            /**< key GPIO get value of  specified input port pin */
} T_KEY_GPIO_DATA;

/**
    * @brief  Key GPIO initial.
    *         Include APB peripheral clock config, key GPIO parameter config and
    *         key specific GPIO interrupt mark config. Enable key GPIO interrupt.
    * @param  void
    * @return void
    */
void key_gpio_initial(void);

/**
    * @brief  Key1~Key5 GPIO interrupt will be handle in this function. And they are all edge trigger.
    *         First disable app enter dlps and read current specific key GPIO input data bit.
    *         Disable key GPIO interrupt and send IO_MSG_TYPE_GPIO message to app task.
    *         Then enable key GPIO interrupt.
    * @param  key_mask key GPIO mask
    * @param  gpio_index  key GPIO pinmux
    * @param  key_index key number, such as key1, key2 etc.
    * @return void
    */
void key_gpio_intr_cb(uint32_t key_index);

/**
    * @brief  Key0 GPIO interrupt will be handle in this function. And it is level trigger.
    *         Key0 GPIO can wake up the system from powerdown mode.
    *         First disable app enter dlps mode and read current key0 GPIO input data bit.
    *         Then disable key0 GPIO interrupt and start HW timer to read the key0 GPIO status.
    * @param  void
    * @return void
    */
void key_gpio_key0_intr_handler(void);

/**
    * @brief  mfb GPIO interrupt will be handle in this function. And it is level trigger.
    *         mfb GPIO can wake up the system from powerdown mode.
    *         First disable app enter dlps mode and read current mfb GPIO input data bit.
    *         Then disable mfb GPIO interrupt and start HW timer to read the mfb GPIO status.
    * @param  void
    * @return void
    */
void key_gpio_mfb_intr_handler(void);

/**
    * @brief  mfb key init
    * @param  none
    * @return none
        */
void key_gpio_mfb_init(void);

bool key_gpio_get_press_state(uint8_t key_index);

/**
    * @brief app_key_check_power_button_pressed
    * @return ture pressed
    */
bool key_gpio_check_power_button_pressed(void);

void key_gpio_key0_debounce_start(void);

/** End of APP_KEY_GPIO
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_KEY_GPIO_H_ */
