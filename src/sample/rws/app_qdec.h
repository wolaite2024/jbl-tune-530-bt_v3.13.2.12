#ifndef _APP_QDEC_H_
#define _APP_QDEC_H_

#include "app_msg.h"

void app_qdec_pad_config(void);
void app_qdec_init_status_read(void);
void app_qdec_driver_init(void);
void app_qdec_init(void);
void app_qdec_enter_power_down_cfg(void);
void app_qdec_msg_handler(T_IO_MSG *io_driver_msg_recv);
void app_qdec_pad_enter_dlps_config(void);
void app_qdec_pad_exit_dlps_config(void);
void app_qdec_wakeup_handle(void);

#endif
