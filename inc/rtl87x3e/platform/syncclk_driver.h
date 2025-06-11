/**
 * @file rtk_timestamp_reg.h
 * @author
 * @brief
 *
 * @version 0.0.1
 * @date 2022-01-06
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef _SYNCCLK_DRIVER_H_
#define _SYNCCLK_DRIVER_H_
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

typedef enum t_syncclk_id
{
    SYNCCLK_ID_EMPTY,
    SYNCCLK_ID1,
    SYNCCLK_ID2,
    SYNCCLK_ID3,
    SYNCCLK_ID4,
    SYNCCLK_ID5,
    SYNCCLK_ID6,
    SYNCCLK_ID7,
    SYNCCLK_ID8,
    SYNCCLK_ID9,
    SYNCCLK_ID10,
    SYNCCLK_ID_MAX,
} T_SYNCCLK_ID;

#define MASTER                              0
#define SLAVE                               1

#define CONN_HANDLE_TYPE_BREDR              (0x00)
#define CONN_HANDLE_TYPE_LE                 (0x01)
#define CONN_HANDLE_TYPE_FREERUN_CLOCK      (0xFE)
#define CONN_HANDLE_TYPE_UNDEFINE           (0xFF)

#define H2D_REG_S_SYNC_CLK_LATCH_PN_SRC_SEL_PICONET_CLOCK  (0x00)
#define H2D_REG_S_SYNC_CLK_LATCH_PN_SRC_SEL_NATIVE_CLOCK   (0x01)

typedef union t_syncclk_latch_info
{
    uint32_t d32[6];
    struct
    {
        uint8_t  conn_role;
        uint8_t  conn_type;
        uint8_t  role;
        uint8_t  net_id;
        uint32_t exp_sync_clock;
    };
} T_SYNCCLK_LATCH_INFO_TypeDef;

bool syncclk_drv_timer_start(T_SYNCCLK_ID timer_id, uint8_t conn_type, uint8_t role,
                             uint8_t net_id);

void syncclk_drv_timer_stop(T_SYNCCLK_ID timer_id);

void syncclk_drv_init(void);

T_SYNCCLK_LATCH_INFO_TypeDef *synclk_drv_time_get(T_SYNCCLK_ID id);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _SYNCCLK_DRIVER_H_ */
