/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _MULTIPRO_CTRLOR_H_
#define _MULTIPRO_CTRLOR_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "remote.h"
#include "multitopology_if.h"
/*============================================================================*
 *                              Constants
 *============================================================================*/
typedef enum
{
    LE_AUDIO_NO    = 0x00,
    LE_AUDIO_CIS   = 0x01,
    LE_AUDIO_BIS   = 0x02,
    LE_AUDIO_ALL   = 0x03,
} T_MTC_AUDIO_MODE;

typedef enum t_mtc_pro_beep
{
    MTC_PRO_BEEP_NONE,
    MTC_PRO_BEEP_PROMPTLY,
    MTC_PRO_BEEP_DELAY,
} T_MTC_PRO_BEEP;

typedef enum t_mtc_pro_key
{
    MULTI_PRO_BT_BREDR,
    MULTI_PRO_BT_CIS,
    MULTI_PRO_BT_BIS,
} T_MTC_BT_MODE;

typedef enum t_multi_pro_sniffing_ststus
{
    MULTI_PRO_SNIFI_NOINVO = 0x0,
    MULTI_PRO_SNIFI_INVO,
} T_MTC_SNIFI_STATUS;

typedef enum
{
    MTC_BUD_STATUS_NONE      = 0x00,
    MTC_BUD_STATUS_SNIFF      = 0x01,
    MTC_BUD_STATUS_ACTIVE      = 0x02,

    MTC_BUD_STATUS_TOTAL,
} T_MTC_BUD_STATUS;

typedef enum
{
    MTC_TOPO_EVENT_A2DP_START      = 0x00,
    MTC_TOPO_EVENT_CIS_ENABLE      = 0x01,
    MTC_TOPO_EVENT_HFP_CALL        = 0x02,
    MTC_TOPO_EVENT_CCP_CALL        = 0x03,
    MTC_TOPO_EVENT_LEA_CONN        = 0x04,
    MTC_TOPO_EVENT_LEA_DISCONN     = 0x05,
    MTC_TOPO_EVENT_LEGACY_CONN     = 0x06,
    MTC_TOPO_EVENT_LEGACY_DISCONN  = 0x07,
    MTC_TOPO_EVENT_BIS_START       = 0x08,
    MTC_TOPO_EVENT_BIS_STOP        = 0x09,
    MTC_TOPO_EVENT_LEA_ADV_STOP    = 0x0A,
    MTC_TOPO_EVENT_BR_ROLE_SWAP    = 0x0B,
    MTC_TOPO_EVENT_CIS_STREAMING   = 0x0C,
    MTC_TOPO_EVENT_SET_BR_MODE     = 0x0D,
    MTC_TOPO_EVENT_CIS_TERMINATE   = 0x0E,
    MTC_TOPO_EVENT_BIS_STREAMING   = 0x0F,
    MTC_TOPO_EVENT_MCP_PLAYING     = 0x10,
    MTC_TOPO_EVENT_MMI             = 0x11,
    MTC_TOPO_EVENT_TOTAL,
} T_MTC_TOPO_EVENT;

/*============================================================================*
 *                              Functions
 *============================================================================*/
T_MTC_BUD_STATUS mtc_get_b2d_sniff_status(void);
T_MTC_BUD_STATUS mtc_get_b2s_sniff_status(void);
void mtc_set_pending(T_MTC_AUDIO_MODE action);
bool mtc_stream_switch(bool iscall);
void mtc_set_beep(T_MTC_PRO_BEEP para);
T_MTC_PRO_BEEP mtc_get_beep(void);
void mtc_set_btmode(T_MTC_BT_MODE para);
void mtc_set_active_bt_mode(T_MTC_BT_MODE para);
T_MTC_BT_MODE mtc_get_btmode(void);
T_MTC_SNIFI_STATUS mtc_get_sniffing(void);
void  mtc_set_sniffing(T_MTC_SNIFI_STATUS para);
bool mtc_term_sd_sniffing(void);
bool mtc_check_call_sate(void);
void mtc_check_reopen_mic(void);
void mtc_legacy_update_call_status(uint8_t *call_status_old);
bool mtc_is_lea_cis_stream(uint8_t *set_mcp);
bool mtc_topology_dm(uint8_t event);
bool mtc_ase_release(void);
void mtc_cis_audio_conext_change(bool enable);
uint8_t mtc_get_active_idx(uint8_t *bt_mode);
void mtc_audio_policy_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                            T_REMOTE_RELAY_STATUS status);
void mtc_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
