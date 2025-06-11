/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef __BT_SYSCALL_H
#define __BT_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/** @defgroup BT_SYSCALL BT Syscall
  * @brief BT Syscall
  * @{
  */

/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup BT_SYSCALL_Exported_Types BT Syscall Types
    * @{
    */

/**
 * @brief The SDU Synchronization Reference Point Information Requirement of LE ISO
 */
typedef struct
{
    uint32_t valid: 1;    /**< bit[0]: 1 is valid and 0 is invalid */
    uint32_t type: 1;     /**< bit[1]: 1 is the ISO and 0 is the ACL (always 1) */
    uint32_t role: 1;     /**< bit[2]: 1 is the master and 0 is the slave */
    uint32_t dir: 1;      /**< bit[3]: 1 is Tx, 0 is Rx. Input parameter */
    uint32_t cis: 1;      /**< bit[4]: 1 is the CIS, 0 is the BIS */
    uint32_t rsvd0: 3;    /**< bit[7:5]: reserved */
    uint32_t status: 8;   /**< bit[15:8]: status */
    uint32_t rsvd1: 16;   /**< bit[31:16]: reserved */

    uint16_t conn_handle;         /**< iso (cis or bis) connection handle, Input parameter */
    uint16_t sdu_seq_num;         /**< sdu sequence number */
    uint32_t sdu_interval;        /**< sdu interval (us) */
    uint32_t cur_sync_ref_point;  /**< current sdu sync ref point (us) */
    uint32_t pre_sync_ref_point;  /**< previous sdu sync ref point (us) */
    uint32_t accumulate_sw_timer; /**< accumulate sw timer (us) */
    uint32_t iso_interval_us;     /**< iso interval (us)*/
    uint32_t group_anchor_point;  /**< cig or big anchor point */
} T_BT_LE_ISO_SYNC_REF_AP_INFO;

/** End of BT_SYSCALL_Exported_Types
    * @}
    */

/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup BT_SYSCALL_Exported_Functions BT Syscall Functions
    * @{
    */

/**
*@brief    Get the application information of the le isochronous synchronization reference.
*@param[in]  p_info  The pointer of the data structure includes the SDU sync ref point information.
*@return   bool
*@retval   true  Success
*@retval   false  Failed
*
* <b>Example usage</b>
* \code{.c}

    void app_lea_probe_cback(T_AUDIO_PROBE_EVENT event, void *buf)
    {
        uint32_t *event_buff = (uint32_t *)buf;
        T_BT_LE_ISO_SYNC_REF_AP_INFO temp_info, info;
        APP_PRINT_TRACE2("app_lea_probe_cback: %d, 0x%08x", event, event_buff[0]);
        if (event != PROBE_SYNC_REF_REQUEST)
        {
            return;
        }
        memset(&info, 0, sizeof(T_BT_LE_ISO_SYNC_REF_AP_INFO));
        memset(&temp_info, 0, sizeof(T_BT_LE_ISO_SYNC_REF_AP_INFO));
        //FIXME: what to do if get multi active conn_handle
        T_ISO_CHNL_INFO_CB *iso_elem = NULL;
        info.group_anchor_point = 0;
        for (uint8_t i = 0; i < p_audio_co_cb->iso_input_queue.count; i++)
        {
            iso_elem = (T_ISO_CHNL_INFO_CB *)os_queue_peek(&p_audio_co_cb->iso_input_queue, i);
            if (iso_elem->path_direction == DATA_PATH_INPUT_FLAG)
            {
                APP_PRINT_TRACE2("app_lea_probe_cback: iso %d, 0x%08x", i, iso_elem->iso_conn_handle);
                temp_info.dir = 1;
                temp_info.conn_handle = iso_elem->iso_conn_handle;
                bt_get_le_iso_sync_ref_ap_info(&temp_info);
                if ((info.group_anchor_point == 0) || (temp_info.group_anchor_point > info.group_anchor_point))
                {
                    info.dir = temp_info.dir;
                    info.conn_handle = temp_info.conn_handle;
                    info.sdu_seq_num = temp_info.sdu_seq_num;
                    info.sdu_interval = temp_info.sdu_interval;
                    info.cur_sync_ref_point = temp_info.cur_sync_ref_point;
                    info.pre_sync_ref_point = temp_info.pre_sync_ref_point;
                    info.accumulate_sw_timer = temp_info.accumulate_sw_timer;
                    info.iso_interval_us = temp_info.iso_interval_us;
                    info.group_anchor_point   = temp_info.group_anchor_point;
                    info.valid = temp_info.valid;
                }
            }
        }
        APP_PRINT_INFO8("ref_ap_info conn_handle:%x sdu_seq_num:%x sdu_interval:%x cur_sync_ref_point:%x pre_sync_ref_point:%x accumulate_sw_timer:%x iso_interval_us:%x group_anchor_point:%x ",
                        info.conn_handle,
                        info.sdu_seq_num,
                        info.sdu_interval,
                        info.cur_sync_ref_point,
                        info.pre_sync_ref_point,
                        info.accumulate_sw_timer,
                        info.iso_interval_us,
                        info.group_anchor_point);
        T_SYNCCLK_LATCH_INFO_TypeDef *p_latch_info  = synclk_drv_time_get(LEA_SYNC_CLK_REF);
        syncclk_set_anchor_point = p_latch_info->exp_sync_clock;
        anchor_point = info.group_anchor_point;
        iso_interval = info.iso_interval_us;
        APP_PRINT_INFO1("conn_handle refclk %x", syncclk_set_anchor_point);
        app_lea_send_ref(&info, REF_GUARD_TIME_US);
    }
* \endcode
*/
bool bt_get_le_iso_sync_ref_ap_info(T_BT_LE_ISO_SYNC_REF_AP_INFO *p_info);

#ifdef __cplusplus
}
#endif

#endif

