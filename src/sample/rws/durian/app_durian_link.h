/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_LINK_H_
#define _APP_DURIAN_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool app_durian_link_disconn_req(uint8_t *bd_addr);
void app_durian_link_bud_disconn(void);
void app_durian_link_src_connect(void);
void app_durian_link_profile_conn(void);
void app_durian_link_back(void);
void app_durian_link_src_disconn(void);
void app_durian_link_init(void);
void app_durian_link_src_connect_delay(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
