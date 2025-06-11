/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_IO_OUTPUT_H_
#define _APP_IO_OUTPUT_H_

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_IO_OUTPUT App IO Output
  * @brief App IO Output
  * @{
  */

/**
    * @brief  app_io_output_ctrl_ext_mcu_pin
    *         enable or disable  pin output 10hz pulse.
    *         .
    * @param  enable
    * @return void
    */
void app_io_output_ctrl_ext_mcu_pin(uint8_t enable);

/**
    * @brief  app_io_output_init
    *         initial  io output.
    *         .
    * @param  void
    * @return void
    */
void app_io_output_init(void);

/**
    * @brief  app_io_output_power_supply
    *         enable or disable io power supply.
    *         .
    * @param  enable
    *         true is enable false disable
    * @return void
    */
void app_io_output_power_supply(bool enable);

/**
    * @brief  out_indication_pinmux init
    * @param  void
    * @return void
    */
void app_io_output_out_indication_pinmux_init(void);
/** End of APP_IO_OUTPUT
* @}
*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_IO_OUTPUT_H_ */
