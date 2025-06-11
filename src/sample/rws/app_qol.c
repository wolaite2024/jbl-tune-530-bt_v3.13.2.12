#if F_APP_QOL_MONITOR_SUPPORT
#include <string.h>
#include "stdlib.h"
#include "os_mem.h"
#include "os_timer.h"
#include "os_sync.h"
#include "btm.h"
#include "gap.h"
#include "gap_br.h"
#include "app_timer.h"
#include "trace.h"
#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_asp_device.h"
#endif
#include "app_audio_policy.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_main.h"
#include "app_multilink.h"
#include "app_qol.h"
#include "app_relay.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"

/*for qol*/
#define RSSI_LINK_MONITOR_TIMEOUT          200
#define BUFFER_LEVEL_REPORT_TIMEOUT        100

typedef enum
{
    APP_TIMER_B2B_LINK_MONITOR = 0x00,
    APP_TIMER_B2S_LINK_MONITOR = 0x01,
    APP_TIMER_BUFFER_LEVEL_REPORT = 0x02,
} T_APP_QOL_TIMER;

typedef struct t_alist
{
    int8_t front;
    int8_t rear;
    uint8_t size;
    int8_t data[64];
} T_ALIST;

static uint8_t link_monitor_timer_id = 0;
static uint8_t timer_idx_b2b_link_monitor = 0;
static uint8_t timer_idx_b2s_link_monitor = 0;
static uint8_t timer_idx_buffer_level_report = 0;
static T_APP_QOL_DATA qol_data, b2b_qol_data;
static T_ALIST rssi_q, per_q, b2b_rssi_q;
static bool b2s_qol_enable;
static bool b2b_qol_enable;

static void app_qol_init_q(void)
{
    memset((void *)&rssi_q, 0, sizeof(T_ALIST));
    memset((void *)&per_q, 0, sizeof(T_ALIST));
    memset((void *)&b2b_rssi_q, 0, sizeof(T_ALIST));
    rssi_q.front = per_q.front = b2b_rssi_q.front = -1;
    rssi_q.rear = per_q.rear = b2b_rssi_q.rear = -1;
    per_q.size = rssi_q.size = b2b_rssi_q.size = 16;
}

#if (REMOVE_MAX_RSSI == 1)
static int8_t app_qol_max_q(T_ALIST *qlist)
{
    int count = 1;
    int8_t max = qlist->data[qlist->front];
    int8_t pq = qlist->front;
    do
    {
        pq = (pq + 1) % (qlist->size - 1);
        if (max < (int8_t)qlist->data[pq])
        {
            max = qlist->data[pq];
        }
        count++;
    }
    while (pq != qlist->rear);

    APP_PRINT_TRACE2("app_qol_max_q: count %d, max mssi %d", count, max);
    return max;
}
#endif

static bool app_qol_enqueue(T_ALIST *qlist, int8_t value)
{
    // APP_PRINT_TRACE3("app_qol_enqueue:%d, rear:%d, size:%d", qlist->front, qlist->rear,
    //                    qlist->size);
    if ((qlist->front == 0 && qlist->rear == qlist->size - 1) ||
        (qlist->rear == (qlist->front - 1) % (qlist->size - 1)))
    {
        return false;
    }
    else if (qlist->front == -1)
    {
        //first element
        qlist->front = qlist->rear = 0;
        qlist->data[qlist->rear] = value;

    }
    else if (qlist->rear == qlist->size - 1 && qlist->front != 0)
    {
        qlist->rear = 0;
        qlist->data[qlist->rear] = value;
    }
    else
    {
        qlist->rear++;
        qlist->data[qlist->rear] = value;
    }
    // APP_PRINT_TRACE3("app_qol_enqueue:%d, rear:%d, size:%d", qlist->front, qlist->rear,
    //                    qlist->size);
    return true;
}

static int8_t app_qol_dequeue(T_ALIST *qlist)
{
    // APP_PRINT_TRACE3("app_qol_dequeue:%d, rear:%d, size:%d", qlist->front, qlist->rear,
    //                    qlist->size);
    if (qlist->front == -1)
    {
        return 0;
    }

    int8_t data = qlist->data[qlist->front];

    if (qlist->front == qlist->rear)
    {
        qlist->front = -1;
        qlist->rear = -1;
    }
    else if (qlist->front == qlist->size - 1)
    {
        qlist->front = 0;
    }
    else
    {
        qlist->front++;
    }

    // APP_PRINT_TRACE4("app_qol_dequeue:%d, rear:%d, size:%d data:%d", qlist->front,
    //                    qlist->rear, qlist->size, data);
    return data;
}

static int16_t app_qol_get_skip_aggregate(T_ALIST *qlist, uint8_t skipcount)
{
    int16_t value = 0;
    while (skipcount)
    {
        value += (int16_t)(app_qol_dequeue(qlist));
        skipcount--;
    }
    return value;
}

static void app_qol_update_rssi(bool is_b2b, int8_t rssi)
{
    if (rssi != 0)
    {
        if (is_b2b)
        {
            if (app_qol_enqueue(&b2b_rssi_q, rssi))
            {
                b2b_qol_data.rssi.aggregate_rssi += rssi;
                b2b_qol_data.rssi.samples_cnt++;
                APP_PRINT_TRACE4("app_qol_update_rssi: %d, n: %d, rssi_sum: %d, is_b2b_link %d", rssi,
                                 b2b_qol_data.rssi.samples_cnt, b2b_qol_data.rssi.aggregate_rssi, is_b2b);
            }
        }
        else
        {
            if (app_qol_enqueue(&rssi_q, rssi))
            {
                qol_data.rssi.aggregate_rssi += rssi;
                qol_data.rssi.samples_cnt++;
                APP_PRINT_TRACE4("app_qol_update_rssi: %d, n: %d, rssi_sum: %d, is_b2b_link %d", rssi,
                                 qol_data.rssi.samples_cnt, qol_data.rssi.aggregate_rssi, is_b2b);
            }
        }
    }
}

static void app_qol_update_per(int8_t per)
{
    if (app_qol_enqueue(&per_q, (int8_t)per))
    {
        qol_data.per.aggregate_per += per;
        qol_data.per.samples_cnt++;
        APP_PRINT_TRACE3("app_qol_update_per: %d, n: %d, per_sum: %d", per, qol_data.per.samples_cnt,
                         qol_data.per.aggregate_per);
    }
}

bool app_qol_get_aggregate_rssi(bool is_b2b, int8_t *rssi)
{
    int16_t skip_aggregate_rssi = 0, rssi_sum = 0;
    uint8_t rssicount = 0;
    int8_t rssi_average = 0;

    if (is_b2b)
    {
        if (b2b_qol_enable == false)
        {
            return false;
        }

        APP_PRINT_TRACE1("app_qol_get_aggregate_rssi: n_b2b_rssi: %d", b2b_qol_data.rssi.samples_cnt);

        if (b2b_qol_data.rssi.samples_cnt > b2b_qol_data.skipsamples)
        {
            skip_aggregate_rssi = app_qol_get_skip_aggregate(&b2b_rssi_q,
                                                             b2b_qol_data.rssi.samples_cnt - b2b_qol_data.skipsamples);
            b2b_qol_data.rssi.aggregate_rssi -= skip_aggregate_rssi;
            b2b_qol_data.rssi.samples_cnt -= b2b_qol_data.rssi.samples_cnt - b2b_qol_data.skipsamples;
        }
        rssicount = b2b_qol_data.rssi.samples_cnt;
        rssi_sum = b2b_qol_data.rssi.aggregate_rssi;

#if (REMOVE_MAX_RSSI == 1)
        {
            rssi_sum -= app_qol_max_q(&b2b_rssi_q);
            rssicount -= 1;
        }
#endif
    }
    else
    {
        if (b2s_qol_enable == false)
        {
            return false;
        }

        APP_PRINT_TRACE1("app_qol_get_aggregate_rssi: n_b2s_rssi: %d", qol_data.rssi.samples_cnt);

        if (qol_data.rssi.samples_cnt > qol_data.skipsamples)
        {
            skip_aggregate_rssi = app_qol_get_skip_aggregate(&rssi_q,
                                                             qol_data.rssi.samples_cnt - qol_data.skipsamples);
            qol_data.rssi.aggregate_rssi -= skip_aggregate_rssi;
            qol_data.rssi.samples_cnt -= qol_data.rssi.samples_cnt - qol_data.skipsamples;
        }
        rssicount = qol_data.rssi.samples_cnt;
        rssi_sum = qol_data.rssi.aggregate_rssi;

#if (REMOVE_MAX_RSSI == 1)
        {
            rssi_sum -= app_qol_max_q(&rssi_q);
            rssicount -= 1;
        }
#endif
    }

    if (rssicount)
    {
        rssi_average = rssi_sum / rssicount;
    }
    APP_PRINT_TRACE1("app_qol_get_aggregate_rssi: rssi_average: %d", rssi_average);

    *rssi = rssi_average;
    return true;
}

#if 0
static void app_qol_sliding_interval_hdlr(void)
{
    //calculate and notify the average value
    int16_t skip_aggregate_rssi = 0, rssi_sum = 0;
    uint16_t skip_aggregate_per = 0, per_sum = 0;
    uint8_t rssicount = 0, percount = 0;
    int8_t rssi_average = 0, per_average = 0;
    int8_t rssi_average_remote = 0, per_average_remote = 0;

    APP_PRINT_TRACE4("app_qol_sliding_interval_hdlr: n_rssi: %d, n_per: %d, n_rssi_remote: %d, n_per_remote: %d",
                     qol_data.rssi.samples_cnt, qol_data.per.samples_cnt,
                     qol_data.rssi_remote.samples_cnt, qol_data.per_remote.samples_cnt);

    // Local
    {
        if (qol_data.rssi.samples_cnt > qol_data.skipsamples)
        {
            skip_aggregate_rssi = app_qol_get_skip_aggregate(&rssi_q, qol_data.skipsamples);
            qol_data.rssi.aggregate_rssi -= skip_aggregate_rssi;
            qol_data.rssi.samples_cnt -= qol_data.skipsamples;
        }
        rssicount = qol_data.rssi.samples_cnt;
        rssi_sum = qol_data.rssi.aggregate_rssi;
#if (REMOVE_MAX_RSSI == 1)
        {
            rssi_sum -= app_qol_max_q(&rssi_q);
            rssicount -= 1;
        }
#endif

        if (qol_data.per.samples_cnt > qol_data.skipsamples)
        {
            skip_aggregate_per = (uint16_t)app_qol_get_skip_aggregate(&per_q, qol_data.skipsamples);
            qol_data.per.aggregate_per -= skip_aggregate_per;
            qol_data.per.samples_cnt -= qol_data.skipsamples;
        }
        percount = qol_data.per.samples_cnt;
        per_sum = qol_data.per.aggregate_per;


        if (rssicount)
        {
            rssi_average = rssi_sum / rssicount;
        }
        if (percount)
        {
            per_average = per_sum / percount;
        }
    }

    // Remote
    {
        if (qol_data.rssi_remote.samples_cnt > qol_data.skipsamples)
        {
            skip_aggregate_rssi = app_qol_get_skip_aggregate(&rssi_q_remote, qol_data.skipsamples);
            qol_data.rssi_remote.aggregate_rssi -= skip_aggregate_rssi;
            qol_data.rssi_remote.samples_cnt -= qol_data.skipsamples;
        }
        rssicount = qol_data.rssi_remote.samples_cnt;
        rssi_sum = qol_data.rssi_remote.aggregate_rssi;
#if (REMOVE_MAX_RSSI == 1)
        {
            rssi_sum -= app_qol_max_q(&rssi_q_remote);
            rssicount -= 1;
        }
#endif

        if (qol_data.per_remote.samples_cnt > qol_data.skipsamples)
        {
            skip_aggregate_per = (uint16_t)app_qol_get_skip_aggregate(&per_q_remote, qol_data.skipsamples);
            qol_data.per_remote.aggregate_per -= skip_aggregate_per;
            qol_data.per_remote.samples_cnt -= qol_data.skipsamples;
        }
        percount = qol_data.per_remote.samples_cnt;
        per_sum = qol_data.per_remote.aggregate_per;

        if (rssicount)
        {
            rssi_average_remote = rssi_sum / rssicount;
        }
        if (percount)
        {
            per_average_remote = per_sum / percount;
        }
    }

    APP_PRINT_TRACE4("app_qol_sliding_interval_hdlr: "
                     "rssi_average: %d, per_average: %d, "
                     "rssi_average_remote: %d, per_average_remote: %d",
                     rssi_average, per_average,
                     rssi_average_remote, per_average_remote);


    // TODO: Check PER and RSSI to do handover
    // Check water level
    // skip time for quick handover
#if 0
    if (rssi_average + app_cfg_const.roleswap_rssi_threshold < rssi_average_remote)
    {
        T_ROLESWAP_CHECK_RESULT check_result = ROLESWAP_DISALLOW;
        app_stop_timer(&timer_idx_b2s_link_monitor);
        check_result = app_roleswap_check();
        app_roleswap_process_after_check(check_result, APP_RS_TRIGGER_CHECK_EVENT_LINK_MONITOR);
    }
#endif

}
#endif


static void app_qol_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_qol_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);
    switch (timer_evt)
    {
    case APP_TIMER_B2S_LINK_MONITOR:
        {
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                //If device is off ing or off , disable starting another timer.
                app_stop_timer(&timer_idx_b2s_link_monitor);
            }
            else
            {
                int8_t rssi = 0;

                app_qol_get_aggregate_rssi(false, &rssi);
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_PERIODIC_UPDATE);

                    if ((rssi >= RSSI_MAXIMUM_THRESHOLD_TO_RECEIVE_PACKET_WELL) && app_db.sec_going_away)
                    {
                        app_db.sec_going_away = false;
                        app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_SEC_GOING_NEAR);
                    }
                }
            }
        }
        break;

    case APP_TIMER_BUFFER_LEVEL_REPORT:
        {
            if (app_db.device_state != APP_DEVICE_STATE_ON)
            {
                //If device is off ing or off , disable starting another timer.
                app_stop_timer(&timer_idx_buffer_level_report);
            }
            else
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_bt_sniffing_handle_periodic_evt(APP_BT_SNIFFING_PERIODIC_EVENT_LEVEL_UPDATE);
                }
            }
        }
        break;

    case APP_TIMER_B2B_LINK_MONITOR:
        {
            int8_t b2b_rssi = 0;

            if (app_qol_get_aggregate_rssi(true, &b2b_rssi) && (b2b_rssi > B2B_RSSI_THRESHOLD_START_SNIFFING))
            {
                app_bt_sniffing_process(app_cfg_nv.bud_peer_addr);
            }
        }
        break;

    default:
        break;
    }
}

static void app_qol_event_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_LINK_PER_INFO:
        {
#if F_APP_TEAMS_FEATURE_SUPPORT
            app_asp_device_handle_per(param->link_per_info.bd_addr,
                                      ((param->link_per_info.err_pkts * 100) / param->link_per_info.total_pkts));
#endif
            app_qol_update_per(param->link_per_info.err_pkts / param->link_per_info.total_pkts);
        }
        break;

    case BT_EVENT_LINK_RSSI_INFO:
        {
            if (app_link_check_b2b_link(param->link_rssi_info.bd_addr))
            {
                app_qol_update_rssi(true, param->link_rssi_info.rssi);
            }
            else
            {
                app_qol_update_rssi(false, param->link_rssi_info.rssi);
            }
        }
        break;
    }
}

void app_qol_link_monitor(uint8_t *bd_addr, bool enable)
{
    if (app_link_check_b2b_link(bd_addr))
    {
        b2b_qol_enable = enable;

        if (enable)
        {
            bt_link_rssi_report(bd_addr, true, RSSI_LINK_MONITOR_TIMEOUT);

            app_start_timer(&timer_idx_b2b_link_monitor, "b2b_link_monitor",
                            link_monitor_timer_id, APP_TIMER_B2B_LINK_MONITOR, 0, true,
                            b2b_qol_data.interval);
        }
        else
        {
            app_stop_timer(&timer_idx_b2b_link_monitor);
            bt_link_rssi_report(bd_addr, false, 0);
        }
    }
    else
    {
        b2s_qol_enable = enable;

        if (enable)
        {
            bt_link_rssi_report(bd_addr, true, RSSI_LINK_MONITOR_TIMEOUT);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                app_start_timer(&timer_idx_b2s_link_monitor, "b2s_link_monitor",
                                link_monitor_timer_id, APP_TIMER_B2S_LINK_MONITOR, 0, true,
                                qol_data.interval);
            }
            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                //start buffer level periodic report to adjust param
                app_start_timer(&timer_idx_buffer_level_report, "buffer_level_report",
                                link_monitor_timer_id, APP_TIMER_BUFFER_LEVEL_REPORT, 0, true,
                                BUFFER_LEVEL_REPORT_TIMEOUT);
            }
        }
        else
        {
            bt_link_rssi_report(bd_addr, false, 0);
            app_stop_timer(&timer_idx_b2s_link_monitor);
            app_stop_timer(&timer_idx_buffer_level_report);
        }
    }
}

void app_qol_disable_link_monitor_timer(void)
{
    app_stop_timer(&timer_idx_b2s_link_monitor);
}

void app_qol_disable_buffer_level_report_timer(void)
{
    app_stop_timer(&timer_idx_buffer_level_report);
}

uint16_t app_qol_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;

    int8_t rssi = 0;
    uint16_t buffer_level = 0;

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SEC_GOING_AWAY:
    case APP_REMOTE_MSG_SYNC_MULTI_SRC_AWAY:
    case APP_REMOTE_MSG_DSP_DECODE_EMPTY:
        payload_len = 1;
        break;

    case APP_REMOTE_MSG_PERIODIC_UPDATE:
        payload_len = 3;
        break;

    default:
        break;
    }

    if (payload_len != 0)
    {
        msg_ptr = (uint8_t *)malloc(payload_len);

        if (msg_ptr == NULL)
        {
            return 0;
        }
    }

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SEC_GOING_AWAY:
        {
            app_qol_get_aggregate_rssi(false, &rssi);
            memcpy(msg_ptr, (uint8_t *)&rssi, 1);
        }
        break;

    case APP_REMOTE_MSG_SYNC_MULTI_SRC_AWAY:
        {
            memcpy(msg_ptr, &app_db.src_going_away_multilink_enabled, 1);
        }
        break;

    case APP_REMOTE_MSG_PERIODIC_UPDATE:
        {
            app_qol_get_aggregate_rssi(false, &rssi);
            app_audio_buffer_level_get(&buffer_level);
            memcpy(msg_ptr, (uint8_t *)&rssi, 1);
            memcpy(&msg_ptr[1], &buffer_level, 2);
        }
        break;

    case APP_REMOTE_MSG_DSP_DECODE_EMPTY:
        {
            app_qol_get_aggregate_rssi(false, &rssi);
            memcpy(msg_ptr, (uint8_t *)&rssi, 1);
        }
        break;

    default:
        break;
    }

    uint16_t msg_len;
    msg_len = app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_QOL, payload_len, msg_ptr, skip, total);

    free(msg_ptr);

    return msg_len;
}

static void app_qol_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    uint8_t app_idx = app_a2dp_get_active_idx();

    switch (msg_type)
    {
    case APP_REMOTE_MSG_SEC_GOING_AWAY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                int8_t remote_rssi;
                int8_t local_rssi = 0;

                app_qol_get_aggregate_rssi(false, &local_rssi);
                remote_rssi = *((int8_t *)buf);
                T_APP_BR_LINK *p_link;
                p_link = &app_db.br_link[app_idx];

                if (p_link)
                {
                    if ((local_rssi + app_cfg_const.roleswap_rssi_threshold >= remote_rssi) &&
                        (app_roleswap_ctrl_get_status() == APP_ROLESWAP_STATUS_IDLE) && (!app_db.sec_going_away) &&
                        (local_rssi >= RSSI_MAXIMUM_THRESHOLD_TO_RECEIVE_PACKET_WELL))
                    {
                        app_db.sec_going_away = true;
                        app_audio_remote_join_set(true);
                        bt_sniffing_set_a2dp_dup_num(true, 0, true);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SEC_GOING_NEAR:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    T_APP_BR_LINK *p_link;
                    p_link = &app_db.br_link[app_idx];

                    if (p_link && app_db.sec_going_away)
                    {
                        app_db.sec_going_away = false;
                        app_bt_sniffing_set_nack_flush_param(p_link->bd_addr);
                        app_audio_remote_join_set(false);
                    }
                }
                else
                {
                    app_db.sec_going_away = false;
                }
            }
        }
        break;

    case APP_REMOTE_MSG_SYNC_MULTI_SRC_AWAY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_db.src_going_away_multilink_enabled = *((uint8_t *)buf);
            }
        }
        break;

    case APP_REMOTE_MSG_PERIODIC_UPDATE:
        {
            uint8_t *p_info = (uint8_t *)buf;

            if ((status == REMOTE_RELAY_STATUS_ASYNC_RCVD) &&
                (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY))
            {
                //pri
                app_db.rssi_remote = (int8_t)p_info[0];
                app_qol_get_aggregate_rssi(false, &app_db.rssi_local);
                app_db.buffer_level_remote = *((uint16_t *)(p_info + 1));
                app_audio_buffer_level_get(&app_db.buffer_level_local);

                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_BUFFER_LEVEL_CHANGED);

                app_bt_sniffing_handle_periodic_evt(APP_BT_SNIFFING_PERIODIC_EVENT_RSSI_UPDATE);
            }
        }
        break;

    case APP_REMOTE_MSG_PRI_DECODE_EMPTY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                //sec notice rssi to pri when dsp decode empty happen to pri
                app_relay_async_single(APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_DSP_DECODE_EMPTY);
            }
        }
        break;

    case APP_REMOTE_MSG_DSP_DECODE_EMPTY:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                //pri
                app_db.rssi_remote = *((int8_t *)buf);
                app_qol_get_aggregate_rssi(false, &app_db.rssi_local);

                app_roleswap_ctrl_check(APP_ROLESWAP_CTRL_EVENT_DECODE_EMPTY);
            }
        }
        break;

    default:
        break;
    }
}

void app_qol_init(void)
{
    b2b_qol_enable = false;
    b2s_qol_enable = false;
    bt_mgr_cback_register(app_qol_event_cback);
    memset((void *)&qol_data, 0, sizeof(qol_data));
    app_qol_init_q();

    qol_data.interval = app_cfg_const.rssi_roleswap_judge_timeout * 1000;
    qol_data.sliding_int = RSSI_LINK_MONITOR_TIMEOUT;
    if (qol_data.interval)
    {
        qol_data.skipsamples = qol_data.interval / qol_data.sliding_int;
    }

    b2b_qol_data.interval = B2B_RSSI_CHECK_INTERVAL;
    b2b_qol_data.sliding_int = RSSI_LINK_MONITOR_TIMEOUT;
    if (b2b_qol_data.interval)
    {
        b2b_qol_data.skipsamples = b2b_qol_data.interval / b2b_qol_data.sliding_int;
    }

    app_timer_reg_cb(app_qol_timeout_cb, &link_monitor_timer_id);
    app_relay_cback_register(app_qol_relay_cback, app_qol_parse_cback,
                             APP_MODULE_TYPE_QOL, APP_REMOTE_MSG_QOL_TOTAL);

    APP_PRINT_TRACE3("app_qol_init: interval %d, sliding_int %d, skipsamples %d",
                     qol_data.interval, qol_data.sliding_int, qol_data.skipsamples);
}

#endif

