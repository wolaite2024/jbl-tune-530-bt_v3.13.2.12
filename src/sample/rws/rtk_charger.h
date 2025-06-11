#ifndef __RTK_CHARGER_H
#define __RTK_CHARGER_H

#include "charger_api.h"
#include "app_charger.h"


typedef void (*RTK_CHARGER_STATE_CB)(T_CHARGER_STATE charge_state);
typedef void (*RTK_CHARGER_SOC_CB)(uint8_t soc);


T_CHARGER_STATE rtk_charger_get_charge_state(void);

uint8_t rtk_charger_get_soc(void);

void rtk_charger_init(RTK_CHARGER_STATE_CB app_state_cb, RTK_CHARGER_SOC_CB app_soc_cb);
uint32_t rtk_charger_get_bat_vol(void);

int32_t rtk_charger_get_bat_curr(void);
#endif









