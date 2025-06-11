/**
************************************************************************************************************
*            Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file      app_cap_touch.c
* @brief     app Cap Touch Demonstration.
* @author
* @date      2020-12-08
* @version   v1.0
*************************************************************************************************************
*/
#if F_APP_CAP_TOUCH_SUPPORT

#include <stdlib.h>
#include "app_device.h"
#include "app_msg.h"
#include "app_main.h"
#include "app_cap_touch.h"
#include "app_key_process.h"
#include "app_cfg.h"
#include "app_mmi.h"
#include "app_timer.h"
#include "app_sensor.h"
#include "app_audio_policy.h"
#include "app_dlps.h"
#include "app_report.h"
#include "app_cmd.h"
#include "app_transfer.h"
#include "app_io_msg.h"
#include "board.h"
#include "ftl.h"
#include "pmu_api.h"
#include "trace.h"
#include "vector_table.h"
#include "rtl876x_captouch.h"
#include "sysm.h"
#include "os_msg.h"
#include "os_sched.h"
#include "os_mem.h"
#include "system_status_api.h"
#include "dlps_util.h"

#define APP_CAP_TOUCH_DBG 0
#define APP_CAP_TOUCH_MAX_CHANNEL_NUM   4
#define APP_CAP_TOUCH_MBIAS_TAP_MAX 63
#define APP_CAP_TOUCH_MAX_BASELINE 4095
#define APP_CAP_TOUCH_MAX_MBIAS_LEVEL 64
#define APP_CAP_TOUCH_MAX_THRESHOLD 4095

#define APP_CAP_TOUCH_FIRST_REPORT_DELAY_TIME   500
#define APP_CAP_TOUCH_RECORD_DATA_NUM   256
#define APP_CAP_TOUCH_REPORT_INTERVAL   (500)
#define APP_CAP_TOUCH_RECORD_INTERVAL   (fast_mode_scan_interval)

#define APP_CAP_TOUCH_CALIBRATION_INTERVAL 200
#define APP_CAP_TOUCH_CALIBRATION_DIFFERENCE 300
#define APP_CAP_TOUCH_CALIBRATION_TARGET 3300
#define APP_CAP_TOUCH_CALIBRATION_NUM  11
#define APP_CAP_TOUCH_CALIBRATION_TAP_POSITION 5
#define APP_CAP_TOUCH_CH0       0x1
#define APP_CAP_TOUCH_CH1       0x2
#define APP_CAP_TOUCH_CH2       0x4
#define APP_CAP_TOUCH_CH3       0x8
#define APP_CAP_TOUCH_CH_ALL    0x0F

#define APP_CAP_TOUCH_PM_WAKEUP_CTC  0x8000

typedef struct t_app_cap_spp_cmd
{
    uint16_t baseline;
    uint16_t threshold;
    uint16_t adc_value;
    uint8_t mbias;
    uint8_t calibr_st;
} T_APP_CAP_SPP_CFG_SET;

typedef struct t_app_cap_calibration
{
    T_APP_CAP_SPP_CFG_SET cap_param[4];
} T_APP_CAP_CALIBRATION;

typedef enum
{
    APP_CAP_TOUCH_CAL_SUCCESS,              //0x00
    APP_CAP_TOUCH_CAL_OUT_OF_RANGE,         //0x01
    APP_CAP_TOUCH_CAL_CH_DISABLE,           //0x02

} T_APP_CAP_TOUCH_CAL_RST;

typedef enum
{
    APP_TIMER_CAP_TOUCH_DLPS_CHECK,
    APP_TIMER_CAP_TOUCH_CHANNEL_ADC,
    APP_TIMER_CAP_TOUCH_REPORT_ADC_VALUE,
    APP_TIMER_CAP_TOUCH_REPORT_ADC_RECORD,
    APP_TIMER_CAP_TOUCH_CALIBRATION,
    APP_TIMER_CAP_TOUCH_EN,
} T_APP_CAP_TOUCH_TIMER;

typedef enum
{
    CAP_TOUCH_CMD_GET_STATUS,               //0x00
    CAP_TOUCH_CMD_SET_BASELINE,             //0x01
    CAP_TOUCH_CMD_SET_MBIAS,                //0x02
    CAP_TOUCH_CMD_SET_THRESHOLD,            //0x03
    CAP_TOUCH_CMD_SET_TEST_MODE_STATUS,     //0x04
    CAP_TOUCH_CMD_GET_TEST_MODE_STATUS,     //0x05
    CAP_TOUCH_CMD_SET_CURRENT_ADC_REPORT,   //0x06
    CAP_TOUCH_CMD_SET_PERIOD_ADC_REPORT,    //0x07
    CAP_TOUCH_CMD_SET_CALIBRATION,          //0x08
    CAP_TOUCH_CMD_SAVE_PARAM_TO_FTL,        //0x09
} T_APP_CAP_TOUCH_SUB_CMD;

typedef enum
{
    CAP_TOUCH_EVENT_REPORT_STATUS,
    CAP_TOUCH_EVENT_REPORT_TEST_MODE_STATUS,
    CAP_TOUCH_EVENT_REPORT_CURRENT_ADC,
    CAP_TOUCH_EVENT_REPORT_PERIOD_ADC,
    CAP_TOUCH_EVENT_REPORT_CALIBRATION,
} T_APP_CAP_TOUCH_SUB_EVENT;

typedef enum
{
    CAP_TOUCH_LD_TYPE_NONE,
    CAP_TOUCH_LD_TYPE_ONE_CH,
    CAP_TOUCH_LD_TYPE_TWO_CH,
} T_CAP_TOUCH_LD_TYPE;

// mapping with config setting
typedef enum
{
    CHANNEL_TYPE_CTC_NONE,
    CHANNEL_TYPE_TOUCH_DET_ISR,
    CHANNEL_TYPE_LEFT_DET_ISR,
    CHANNEL_TYPE_RIGHT_DET_ISR,
    CHANNEL_TYPE_IN_EAR_DET_CH1_ISR,
    CHANNEL_TYPE_IN_EAR_DET_CH2_ISR,

    CHANNEL_TYPE_CTC_TPYE_TOTAL = 0xF,
} T_CAP_TOUCH_CHANNEL_TYPE;

typedef enum
{
    TOUCH_ACTION_CTC_PRESS,
    TOUCH_ACTION_CTC_RELEASE,
    TOUCH_ACTION_CTC_FALSE_TRIGGER,
} T_CAP_TOUCH_ACTION;

typedef enum
{
    CAP_TOUCH_ADC_REPORT_TYPE_OFF,
    CAP_TOUCH_ADC_REPORT_TYPE_REALTIME,
    CAP_TOUCH_ADC_REPORT_TYPE_RECORD,
} T_CAP_TOUCH_ADC_REPORT_TYPE;

static uint16_t slow_mode_scan_interval = 0x3B;//59
static uint16_t fast_mode_scan_interval = 0x1D;//29
static uint8_t over_p_en_wait =
    80;  // 38 ms = [ round up (Fast_scan_interval(0x1D)/1.024 +2*channel_num(4) +1  ) ]
static uint16_t slow_mode_scan_interval_pw_down = 0x107;

static bool ld_det_ch0 = false;
static bool ld_det_ch1 = false;

static bool ctc_test_mode_enable = false;
static bool ctc_ld_set_enable = false;
static bool ctc_check_channel_enable = false;
static uint8_t orignal_dt_resend_num = 3;
static uint8_t *ctc_adc_report_value = NULL;
static uint8_t record_data_num = 0;
static uint8_t report_cmd_path;
static uint8_t report_cmd_idx;
static uint8_t ctc_channel_lock = 0;
static uint8_t  *adc_report_ptr;
static FunctionalState captouch_param_auto_det = DISABLE;
static T_APP_CAP_SPP_CFG_SET *cap_touch_param_info          = NULL;
static T_APP_CAP_SPP_CFG_SET *cap_touch_calibration_data    = NULL;
static T_CAP_TOUCH_ADC_REPORT_TYPE ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_OFF;

// cap touch timer
static uint8_t timer_idx_cap_touch_dlps_check = 0;
static uint8_t timer_idx_cap_touch_channel_adc = 0;
static uint8_t timer_idx_cap_touch_report_adc_value = 0;
static uint8_t timer_idx_cap_touch_report_adc_record = 0;
static uint8_t timer_idx_cap_touch_calibration = 0;
static uint8_t timer_idx_cap_touch_en_overp = 0;
static uint8_t cap_touch_timer_id = 0;
////

struct T_CTC_ISR_CB
{
    void (*func)(uint8_t);
};

static T_CAP_TOUCH_ACTION app_cap_touch_get_curr_action(uint8_t event);
static void app_cap_touch_enable(bool enable);
static void app_cap_touch_handle(uint8_t event);
static void app_cap_touch_in_ear_detect_ch1(uint8_t event);
static void app_cap_touch_in_ear_detect_ch2(uint8_t event);
static void app_cap_touch_slide_left_handle(uint8_t event);
static void app_cap_touch_slide_right_handle(uint8_t event);
static void app_cap_touch_set_parameter(T_APP_CAP_SPP_CFG_SET *p_data, bool check_calibration);
static bool app_cap_touch_calibration_check_param(uint8_t *calibration_cnt);
static bool app_cap_touch_check_channel_type_enable(CTC_CH_TYPE channel_num);
static void app_cap_touch_in_ear_handle(uint8_t event, T_CAP_TOUCH_CHANNEL_TYPE LD_channel);

static void app_cap_touch_ld_check(void);
static void app_cap_touch_cfg_init(void);
static void app_cap_touch_reg_timer(void);
static void app_cap_touch_load_param(void);
static void app_cap_touch_parameter_init(void);
static void app_cap_touch_reset_parameter(void);
static void app_cap_touch_reset_all_setting(void);
static void app_cap_touch_calibraion_read_value(void);

void (*cap_touch_channel0)(uint8_t);
void (*cap_touch_channel1)(uint8_t);
void (*cap_touch_channel2)(uint8_t);
void (*cap_touch_channel3)(uint8_t);

const static struct T_CTC_ISR_CB ctc_trigger_event[CHANNEL_TYPE_CTC_TPYE_TOTAL] =
{
    NULL,
    app_cap_touch_handle,
    app_cap_touch_slide_left_handle,
    app_cap_touch_slide_right_handle,
    app_cap_touch_in_ear_detect_ch1,
    app_cap_touch_in_ear_detect_ch2,
};

static void app_cap_touch_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
    case SYS_EVENT_POWER_OFF:
        {
            app_cap_touch_reset_parameter();
            app_cap_touch_calibraion_read_value();
            /* Cap Touch start */
            app_cap_touch_enable(true);
#if APP_CAP_TOUCH_DBG
            if (event_type == SYS_EVENT_POWER_OFF)
            {
                app_stop_timer(&timer_idx_cap_touch_channel_adc);
            }
#endif
        }
        break;
    }
}

void app_cap_touch_calibration_start(void)
{
#if APP_CAP_TOUCH_DBG
    app_stop_timer(&timer_idx_cap_touch_channel_adc);
#endif
    ctc_test_mode_enable = true;

    app_cap_touch_load_param();

    memset(cap_touch_calibration_data, 0x0, APP_RW_CAPTOUCH_CALIBRATION_SIZE);
    app_cap_touch_reset_parameter();

    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
    {
        if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)ch))
        {
            cap_touch_param_info[ch].mbias = (cap_touch_param_info[ch].mbias <
                                              APP_CAP_TOUCH_CALIBRATION_TAP_POSITION) ?
                                             0 : cap_touch_param_info[ch].mbias - APP_CAP_TOUCH_CALIBRATION_TAP_POSITION ;
        }
    }
    app_cap_touch_set_parameter(cap_touch_param_info, false);
    app_cap_touch_enable(true);
    app_start_timer(&timer_idx_cap_touch_calibration, "cap_touch_calibration",
                    cap_touch_timer_id, APP_TIMER_CAP_TOUCH_CALIBRATION, 0, false,
                    APP_CAP_TOUCH_CALIBRATION_INTERVAL);
}

static void app_cap_touch_calibration_report(T_APP_CAP_SPP_CFG_SET *p_data)
{
    uint8_t *buf;
    uint8_t ch_idx = 0;
    buf = calloc(1, APP_RW_CAPTOUCH_CALIBRATION_SIZE + 1);

    buf[ch_idx++] = CAP_TOUCH_CMD_SET_CALIBRATION;

    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
    {
        buf[ch_idx++]   = ch;
        memcpy(&buf[ch_idx], &p_data[ch].baseline, sizeof(p_data[ch].baseline));
        ch_idx += 2;
        memcpy(&buf[ch_idx], &p_data[ch].mbias, sizeof(p_data[ch].mbias));
        ch_idx += 2;
        memcpy(&buf[ch_idx], &p_data[ch].threshold, sizeof(p_data[ch].threshold));
        ch_idx += 2;
        memcpy(&buf[ch_idx], &p_data[ch].calibr_st, sizeof(p_data[ch].calibr_st));
        ch_idx ++;
    }

    CTC_PRINT_TRACE4("app_cap_touch_calibration_report path %d idx %d data[%d] %b",
                     report_cmd_path,
                     report_cmd_idx,
                     ch_idx,
                     TRACE_BINARY(ch_idx, buf));

    app_report_event(report_cmd_path, EVENT_CAP_TOUCH_CTL, report_cmd_idx, buf, ch_idx);
    free(buf);
}

#if APP_CAP_TOUCH_DBG
static void app_cap_touch_calibration_dump(T_APP_CAP_SPP_CFG_SET *p_data)
{
    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
    {
        CTC_PRINT_TRACE6("app_cap_touch_calibration_dump CH %d baseline %d mbias %d threshold %d adc_value %d calibr_st %d",
                         ch,
                         p_data[ch].baseline,
                         p_data[ch].mbias,
                         p_data[ch].threshold,
                         p_data[ch].adc_value,
                         p_data[ch].calibr_st);
    }
}
#endif

static uint32_t app_cap_touch_calibration_load_data(T_APP_CAP_SPP_CFG_SET *p_data)
{
    uint32_t ret;

    ret =  ftl_load_from_storage(p_data, APP_RW_CAPTOUCH_CALIBRATION_ADDR,
                                 APP_RW_CAPTOUCH_CALIBRATION_SIZE);
#if APP_CAP_TOUCH_DBG
    CTC_PRINT_TRACE1("app_cap_touch_calibration_load_data ctc_channel_lock %d", ctc_channel_lock);
#endif

    if (ret)
    {
        ctc_channel_lock = APP_CAP_TOUCH_CH_ALL;
        CTC_PRINT_TRACE1("app_cap_touch_calibration_load_data fail %d", ret);
    }
    else
    {
        T_APP_CAP_SPP_CFG_SET *calibration_zero_data    = NULL;
        calibration_zero_data = calloc(1, APP_RW_CAPTOUCH_CALIBRATION_SIZE);

        if (memcmp(p_data, calibration_zero_data, APP_RW_CAPTOUCH_CALIBRATION_SIZE))
        {
            for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
            {
#if APP_CAP_TOUCH_DBG
                CTC_PRINT_TRACE3("app_cap_touch_calibration_load_data ch %d calibr_st %d ctc_channel_lock %d",
                                 ch,
                                 p_data->calibr_st,
                                 ctc_channel_lock);
#endif
                if (p_data->calibr_st != APP_CAP_TOUCH_CAL_SUCCESS)
                {
                    ctc_channel_lock |= (APP_CAP_TOUCH_CH0 << ch);
                }
            }
        }
        free(calibration_zero_data);
    }
#if APP_CAP_TOUCH_DBG
    app_cap_touch_calibration_dump(p_data);
#endif
    return ret;
}

static uint32_t app_cap_touch_calibration_save_data(void *p_data)
{
    uint32_t ret;

    ret = ftl_save_to_storage(p_data, APP_RW_CAPTOUCH_CALIBRATION_ADDR,
                              APP_RW_CAPTOUCH_CALIBRATION_SIZE);
    if (ret)
    {
        CTC_PRINT_TRACE1("app_cap_touch_calibration_save_data fail %d", ret);
    }
#if APP_CAP_TOUCH_DBG
    app_cap_touch_calibration_dump(p_data);
#endif
    return ret;
}

static void app_cap_touch_calibraion_read_value(void)
{
    T_APP_CAP_SPP_CFG_SET *calibration_zero_data    = NULL;
    calibration_zero_data = calloc(1, APP_RW_CAPTOUCH_CALIBRATION_SIZE);

    if (memcmp(cap_touch_calibration_data, calibration_zero_data, APP_RW_CAPTOUCH_CALIBRATION_SIZE))
    {
        CTC_PRINT_TRACE0("app_cap_touch_calibraion_read_value from FTL");
        app_cap_touch_set_parameter(cap_touch_calibration_data, true);
    }
    free(calibration_zero_data);
}

static void app_cap_touch_calibration_update(void)
{
    uint16_t calibration_diff; //new diff
    uint16_t lst_diff;

    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
    {
        lst_diff = (cap_touch_calibration_data[ch].adc_value >
                    APP_CAP_TOUCH_CALIBRATION_TARGET) ?
                   (cap_touch_calibration_data[ch].adc_value - APP_CAP_TOUCH_CALIBRATION_TARGET) :
                   (APP_CAP_TOUCH_CALIBRATION_TARGET - cap_touch_calibration_data[ch].adc_value);

        if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)ch))
        {
            calibration_diff = (cap_touch_param_info[ch].adc_value >
                                APP_CAP_TOUCH_CALIBRATION_TARGET) ?
                               (cap_touch_param_info[ch].adc_value - APP_CAP_TOUCH_CALIBRATION_TARGET) :
                               (APP_CAP_TOUCH_CALIBRATION_TARGET - cap_touch_param_info[ch].adc_value);

            if (calibration_diff <= lst_diff)
            {
                cap_touch_calibration_data[ch].adc_value    = cap_touch_param_info[ch].adc_value;
                cap_touch_calibration_data[ch].baseline     = cap_touch_param_info[ch].adc_value;
                cap_touch_calibration_data[ch].mbias        = cap_touch_param_info[ch].mbias;
                cap_touch_calibration_data[ch].threshold    = cap_touch_param_info[ch].threshold;
            }
#if APP_CAP_TOUCH_DBG
            CTC_PRINT_TRACE7("app_cap_touch_calibration_update CH[%d] cur_adv_val %d cur_mbias %d K_diff %d lst_diff %d K_mbias %d k_baseline %d",
                             ch,
                             cap_touch_param_info[ch].adc_value,
                             cap_touch_param_info[ch].mbias,
                             calibration_diff,
                             lst_diff,
                             cap_touch_calibration_data[ch].mbias,
                             cap_touch_calibration_data[ch].baseline);
#endif
        }
    }
}

static void app_cap_touch_calibration_check_result(void)
{
    uint16_t calibration_diff; //new diff
    uint16_t lst_diff;
    float adc_value;
    float default_baseline;
    float threshold;

    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
    {
        lst_diff = (cap_touch_calibration_data[ch].adc_value >
                    APP_CAP_TOUCH_CALIBRATION_TARGET) ?
                   (cap_touch_calibration_data[ch].adc_value - APP_CAP_TOUCH_CALIBRATION_TARGET) :
                   (APP_CAP_TOUCH_CALIBRATION_TARGET - cap_touch_calibration_data[ch].adc_value);

        if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)ch))
        {
            if (lst_diff <= APP_CAP_TOUCH_CALIBRATION_DIFFERENCE)
            {
                cap_touch_calibration_data[ch].calibr_st = APP_CAP_TOUCH_CAL_SUCCESS;

                if ((CTC_CH_TYPE)ch == CTC_CH0)
                {
                    default_baseline    = app_cfg_const.ctc_chan_0_default_baseline;
                    threshold           = app_cfg_const.ctc_chan_0_threshold;
                }
                else if ((CTC_CH_TYPE)ch == CTC_CH1)
                {
                    default_baseline    = app_cfg_const.ctc_chan_1_default_baseline;
                    threshold           = app_cfg_const.ctc_chan_1_threshold;
                }
                else if ((CTC_CH_TYPE)ch == CTC_CH2)
                {
                    default_baseline    = app_cfg_const.ctc_chan_2_default_baseline;
                    threshold           = app_cfg_const.ctc_chan_2_threshold_upper << 8 |
                                          app_cfg_const.ctc_chan_2_threshold_lower;;
                }
                else if ((CTC_CH_TYPE)ch == CTC_CH3)
                {
                    default_baseline    = app_cfg_const.ctc_chan_3_default_baseline;
                    threshold           = app_cfg_const.ctc_chan_3_threshold;
                }

                adc_value = (float)cap_touch_calibration_data[ch].adc_value;

                float cal_thres = adc_value / default_baseline;
                cap_touch_calibration_data[ch].threshold = (uint32_t)(cal_thres * threshold);

                bool cal_fail = false;
                if (cap_touch_calibration_data[ch].threshold > 4095)
                {
                    cal_fail = true;
                    cap_touch_calibration_data[ch].threshold = 4095;
                }
                if (cap_touch_calibration_data[ch].mbias > 63)
                {
                    cal_fail = true;
                    cap_touch_calibration_data[ch].mbias = 63;
                }
                if (cap_touch_calibration_data[ch].baseline > 4095)
                {
                    cal_fail = true;
                    cap_touch_calibration_data[ch].baseline = 4095;
                }
                if (cal_fail)
                {
                    cap_touch_calibration_data[ch].calibr_st = APP_CAP_TOUCH_CAL_OUT_OF_RANGE;
                }
            }
            else
            {
                cap_touch_calibration_data[ch].calibr_st = APP_CAP_TOUCH_CAL_OUT_OF_RANGE;
            }
        }
        else
        {
            cap_touch_calibration_data[ch].calibr_st = APP_CAP_TOUCH_CAL_CH_DISABLE;
        }

        CTC_PRINT_TRACE6("app_cap_touch_calibration_check_result CH %d baseline %d mbias %d  thres %d  rst %d lst_diff %d",
                         ch,
                         cap_touch_calibration_data[ch].baseline,
                         cap_touch_calibration_data[ch].mbias,
                         cap_touch_calibration_data[ch].threshold,
                         cap_touch_calibration_data[ch].calibr_st,
                         lst_diff);
    }

    app_cap_touch_calibration_save_data(cap_touch_calibration_data);
    app_cap_touch_calibration_report(cap_touch_calibration_data);
}

static bool app_cap_touch_calibration_check_param(uint8_t *calibration_cnt)
{
    bool calibration_done = false;

    app_cap_touch_calibration_update();

    if (*calibration_cnt)
    {

#if APP_CAP_TOUCH_DBG
        app_cap_touch_calibration_dump(&cap_touch_param_info[0]);
#endif

        if (*calibration_cnt <= APP_CAP_TOUCH_CALIBRATION_NUM)
        {
            for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
            {
                if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)ch))
                {
                    cap_touch_param_info[ch].mbias = (cap_touch_param_info[ch].mbias >= APP_CAP_TOUCH_MBIAS_TAP_MAX) ?
                                                     APP_CAP_TOUCH_MBIAS_TAP_MAX : cap_touch_param_info[ch].mbias + 1;
                }
            }
        }
    }

    app_cap_touch_reset_parameter();

    if (*calibration_cnt > APP_CAP_TOUCH_CALIBRATION_NUM)
    {
        //check calibration result
        app_cap_touch_calibration_check_result();
        app_cap_touch_set_parameter(cap_touch_calibration_data, false);
        calibration_done = true;
    }
    else
    {
        app_cap_touch_set_parameter(cap_touch_param_info, false);
        (*calibration_cnt)++;
    }
    app_cap_touch_enable(true);

#if APP_CAP_TOUCH_DBG
    if (calibration_done)
    {
        app_start_timer(&timer_idx_cap_touch_channel_adc, "cap_touch_channel_adc",
                        cap_touch_timer_id, APP_TIMER_CAP_TOUCH_CHANNEL_ADC, 0, false,
                        500);
    }
#endif
    CTC_PRINT_TRACE2("app_cap_touch_calibration_check_param %d cnt %d", calibration_done,
                     *calibration_cnt);
    return calibration_done;
}

static void app_cap_touch_slide_left_handle(uint8_t event)
{
// to do
}

static void app_cap_touch_slide_right_handle(uint8_t event)
{
// to do
}

static void app_cap_touch_load_param_channel(uint8_t channel)
{
    if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)channel))
    {
        cap_touch_param_info[channel].baseline  = CapTouch_GetChBaseline((CTC_CH_TYPE) channel);
        cap_touch_param_info[channel].mbias     = CapTouch_GetChMbias((CTC_CH_TYPE) channel);
        cap_touch_param_info[channel].threshold = CapTouch_GetChDiffThres((CTC_CH_TYPE) channel);
        cap_touch_param_info[channel].adc_value = CapTouch_GetChAveData((CTC_CH_TYPE) channel);
#if APP_CAP_TOUCH_DBG
        CTC_PRINT_TRACE5("app_cap_touch_load_param_channel CH %d baseline %d mbias %d threshold %d adv_value %d",
                         channel,
                         cap_touch_param_info[channel].baseline,
                         cap_touch_param_info[channel].mbias,
                         cap_touch_param_info[channel].threshold,
                         cap_touch_param_info[channel].adc_value);
#endif
    }
}

static void app_cap_touch_load_param(void)
{
    if (cap_touch_param_info)
    {
        free(cap_touch_param_info);
        cap_touch_param_info = NULL;
    }
    cap_touch_param_info = calloc(1, APP_RW_CAPTOUCH_CALIBRATION_SIZE);

    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM; ch++)
    {
        app_cap_touch_load_param_channel(ch);
    }
}

static void app_cap_touch_reset_parameter(void)
{
    app_cap_touch_enable(false);
    CapTouch_SysReset();
    app_cap_touch_parameter_init();
}

static void app_cap_touch_reset_all_setting(void)
{
    app_cap_touch_reset_parameter();
    app_cap_touch_calibraion_read_value();
    app_cap_touch_enable(true);
}

static void app_cap_touch_reset_test_mode_setting(void)
{
    ctc_test_mode_enable = false;
    ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_OFF;
    app_cfg_const.dt_resend_num = orignal_dt_resend_num;
    orignal_dt_resend_num = 0;
    app_stop_timer(&timer_idx_cap_touch_report_adc_value);
    app_stop_timer(&timer_idx_cap_touch_report_adc_record);
    if (ctc_adc_report_value != NULL)
    {
        free(ctc_adc_report_value);
        ctc_adc_report_value = NULL;
    }
    if (cap_touch_param_info != NULL)
    {
        free(cap_touch_param_info);
        cap_touch_param_info = NULL;
    }
}

static void app_cap_touch_handle(uint8_t event)
{
    T_CAP_TOUCH_ACTION action = app_cap_touch_get_curr_action(event);
    CTC_PRINT_TRACE3("app_cap_touch_handle act %d do_act %d, test mode %d", event, action,
                     ctc_test_mode_enable);

    if ((action == TOUCH_ACTION_CTC_PRESS) || (action == TOUCH_ACTION_CTC_RELEASE))
    {
        app_key_cap_touch_key_process((action == TOUCH_ACTION_CTC_PRESS) ? KEY_PRESS : KEY_RELEASE);
    }
}

static void app_cap_touch_in_ear_handle(uint8_t event, T_CAP_TOUCH_CHANNEL_TYPE LD_channel)
{
    if (event < IO_MSG_CAP_TOUCH_OVER_N_NOISE)
    {
        T_CAP_TOUCH_ACTION action = app_cap_touch_get_curr_action(event);

        CTC_PRINT_TRACE3("app_cap_touch_in_ear_handle ch %d act %d do_act %d", LD_channel, event, action);

        if (action != TOUCH_ACTION_CTC_FALSE_TRIGGER)
        {
            if (LD_channel == CHANNEL_TYPE_IN_EAR_DET_CH1_ISR)
            {
                ld_det_ch0 = (action == TOUCH_ACTION_CTC_RELEASE) ? false : true;
            }
            else if (LD_channel == CHANNEL_TYPE_IN_EAR_DET_CH2_ISR)
            {
                ld_det_ch1 = (action == TOUCH_ACTION_CTC_RELEASE) ? false : true;
            }
            app_cap_touch_ld_check();
        }
    }
}

static void app_cap_touch_in_ear_detect_ch1(uint8_t event)
{
    app_cap_touch_in_ear_handle(event, CHANNEL_TYPE_IN_EAR_DET_CH1_ISR);
}

static void app_cap_touch_in_ear_detect_ch2(uint8_t event)
{
    app_cap_touch_in_ear_handle(event, CHANNEL_TYPE_IN_EAR_DET_CH2_ISR);
}

static bool app_cap_touch_send_msg(T_IO_MSG_CAP_TOUCH subtype, void *param_buf)
{
    uint8_t  event;
    T_IO_MSG msg;

    event = EVENT_IO_TO_APP;

    msg.type    = IO_MSG_TYPE_CAP_TOUCH;
    msg.subtype = subtype;
    msg.u.buf   = param_buf;
    msg.subtype = subtype;

    if (os_msg_send(audio_evt_queue_handle, &event, 0) == true)
    {
        return os_msg_send(audio_io_queue_handle, &msg, 0);
    }

    return false;
}

/**
  * @brief  CapTouch Interrupt handler.
  * @retval None
  */
ISR_TEXT_SECTION
static void app_cap_touch_Handler(void)
{
    uint32_t int_status = 0;
    int_status = CapTouch_GetINTStatus();
    T_IO_MSG_CAP_TOUCH msg_type = IO_MSG_CAP_TOTAL;

    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

    if (cap_touch_channel0)
    {
        /* Channel 0 interrupts */
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH0, CTC_TOUCH_PRESS_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH0, CTC_TOUCH_PRESS_INT);
            msg_type = IO_MSG_CAP_CH0_PRESS;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH0, CTC_TOUCH_RELEASE_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH0, CTC_TOUCH_RELEASE_INT);
            msg_type = IO_MSG_CAP_CH0_RELEASE;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH0, CTC_FALSE_TOUCH_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH0, CTC_FALSE_TOUCH_INT);
            msg_type = IO_MSG_CAP_CH0_FALSE_TRIGGER;
        }
    }

    if (cap_touch_channel1)
    {
        /* Channel 1 interrupts */
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH1, CTC_TOUCH_PRESS_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH1, CTC_TOUCH_PRESS_INT);
            msg_type = IO_MSG_CAP_CH1_PRESS;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH1, CTC_TOUCH_RELEASE_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH1, CTC_TOUCH_RELEASE_INT);
            msg_type = IO_MSG_CAP_CH1_RELEASE;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH1, CTC_FALSE_TOUCH_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH1, CTC_FALSE_TOUCH_INT);
            msg_type = IO_MSG_CAP_CH1_FALSE_TRIGGER;
        }
    }

    if (cap_touch_channel2)
    {
        /* Channel 2 interrupts */
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH2, CTC_TOUCH_PRESS_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH2, CTC_TOUCH_PRESS_INT);
            msg_type = IO_MSG_CAP_CH2_PRESS;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH2, CTC_TOUCH_RELEASE_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH2, CTC_TOUCH_RELEASE_INT);
            msg_type = IO_MSG_CAP_CH2_RELEASE;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH2, CTC_FALSE_TOUCH_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH2, CTC_FALSE_TOUCH_INT);
            msg_type = IO_MSG_CAP_CH2_FALSE_TRIGGER;
        }
    }

    if (cap_touch_channel3)
    {
        /* Channel 3 interrupts */
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH3, CTC_TOUCH_PRESS_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH3, CTC_TOUCH_PRESS_INT);
            msg_type = IO_MSG_CAP_CH3_PRESS;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH3, CTC_TOUCH_RELEASE_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH3, CTC_TOUCH_RELEASE_INT);
            msg_type = IO_MSG_CAP_CH3_RELEASE;
        }
        if (CapTouch_IsChINTTriggered(int_status, CTC_CH3, CTC_FALSE_TOUCH_INT))
        {
            /* do something */
            CapTouch_ChINTClearPendingBit(CTC_CH3, CTC_FALSE_TOUCH_INT);
            msg_type = IO_MSG_CAP_CH3_FALSE_TRIGGER;
        }
    }
    /* Noise Interrupt */
    if (CapTouch_IsNoiseINTTriggered(int_status, CTC_OVER_N_NOISE_INT))
    {
        /* do something */
        CapTouch_NoiseINTClearPendingBit(CTC_OVER_N_NOISE_INT);
        msg_type = IO_MSG_CAP_TOUCH_OVER_N_NOISE;
    }
    if (CapTouch_IsNoiseINTTriggered(int_status, CTC_OVER_P_NOISE_INT))
    {
#if APP_CAP_TOUCH_DBG
        if (cap_touch_channel0)
        {
            CTC_PRINT_TRACE4("OVER_P_ch0_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                             CapTouch_GetChBaseline(CTC_CH0),
                             CapTouch_GetChMbias(CTC_CH0),
                             CapTouch_GetChDiffThres(CTC_CH0),
                             CapTouch_GetChAveData(CTC_CH0));
        }
        if (cap_touch_channel1)
        {
            CTC_PRINT_TRACE4("OVER_P_ch1_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                             CapTouch_GetChBaseline(CTC_CH1),
                             CapTouch_GetChMbias(CTC_CH1),
                             CapTouch_GetChDiffThres(CTC_CH1),
                             CapTouch_GetChAveData(CTC_CH1));
        }
        if (cap_touch_channel2)
        {
            CTC_PRINT_TRACE4("OVER_P_ch2_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                             CapTouch_GetChBaseline(CTC_CH2),
                             CapTouch_GetChMbias(CTC_CH2),
                             CapTouch_GetChDiffThres(CTC_CH2),
                             CapTouch_GetChAveData(CTC_CH2));
        }
        if (cap_touch_channel3)
        {
            CTC_PRINT_TRACE4("OVER_P_ch3_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                             CapTouch_GetChBaseline(CTC_CH3),
                             CapTouch_GetChMbias(CTC_CH3),
                             CapTouch_GetChDiffThres(CTC_CH3),
                             CapTouch_GetChAveData(CTC_CH3));
        }
#endif

        msg_type = IO_MSG_CAP_TOUCH_OVER_P_NOISE;
        CapTouch_NoiseINTConfig(CTC_OVER_P_NOISE_INT, DISABLE);
        CapTouch_NoiseINTClearPendingBit(CTC_OVER_P_NOISE_INT);
    }
    CTC_PRINT_TRACE1("app_cap_touch_Handler %d", msg_type);

    if (!ctc_test_mode_enable)
    {
        app_cap_touch_send_msg(msg_type, NULL);
    }
}

static void app_cap_touch_parameter_init(void)
{
    uint16_t slow_mode_interval;
    ctc_check_channel_enable = false;

    RamVectorTableUpdate(TOUCH_VECTORn, (IRQ_Fun)app_cap_touch_Handler);

    CTC_PRINT_TRACE1("app_cap_touch_parameter_init channel_lock %d", ctc_channel_lock);
    if (cap_touch_channel0)
    {
        if ((app_db.device_state == APP_DEVICE_STATE_ON) || ((app_db.device_state != APP_DEVICE_STATE_ON)
                                                             && (app_cfg_const.ctc_chan_0_type == CHANNEL_TYPE_TOUCH_DET_ISR)))
        {
            CTC_PRINT_TRACE0("app_cap_touch_parameter_init channel_0");
            CapTouch_ChCmd(CTC_CH0, ENABLE);
            ctc_check_channel_enable = true;

            if ((ctc_channel_lock & APP_CAP_TOUCH_CH0) == 0)
            {
                CapTouch_ChINTConfig(CTC_CH0, (CTC_CH_INT_TYPE)(CTC_TOUCH_PRESS_INT | CTC_TOUCH_RELEASE_INT |
                                                                CTC_FALSE_TOUCH_INT), ENABLE);
            }
            CapTouch_SetChBaseline(CTC_CH0, app_cfg_const.ctc_chan_0_default_baseline);
            CapTouch_SetChMbias(CTC_CH0, app_cfg_const.ctc_chan_0_mbias);
            CapTouch_SetChDiffThres(CTC_CH0, app_cfg_const.ctc_chan_0_threshold);
            CapTouch_SetChPNoiseThres(CTC_CH0, app_cfg_const.ctc_chan_0_threshold / 2);
            CapTouch_SetChNNoiseThres(CTC_CH0, app_cfg_const.ctc_chan_0_threshold / 2);
            CapTouch_ChWakeupCmd(CTC_CH0, (FunctionalState)ENABLE);
        }
    }

    if (cap_touch_channel1)
    {
        if ((app_db.device_state == APP_DEVICE_STATE_ON) || ((app_db.device_state != APP_DEVICE_STATE_ON)
                                                             && (app_cfg_const.ctc_chan_1_type == CHANNEL_TYPE_TOUCH_DET_ISR)))
        {
            CTC_PRINT_TRACE0("app_cap_touch_parameter_init channel_1");
            CapTouch_ChCmd(CTC_CH1, ENABLE);
            ctc_check_channel_enable = true;

            if ((ctc_channel_lock & APP_CAP_TOUCH_CH1) == 0)
            {
                CapTouch_ChINTConfig(CTC_CH1, (CTC_CH_INT_TYPE)(CTC_TOUCH_PRESS_INT | CTC_TOUCH_RELEASE_INT |
                                                                CTC_FALSE_TOUCH_INT), ENABLE);
            }
            CapTouch_SetChBaseline(CTC_CH1, app_cfg_const.ctc_chan_1_default_baseline);
            CapTouch_SetChMbias(CTC_CH1, app_cfg_const.ctc_chan_1_mbias);
            CapTouch_SetChDiffThres(CTC_CH1, app_cfg_const.ctc_chan_1_threshold);
            CapTouch_SetChPNoiseThres(CTC_CH1, app_cfg_const.ctc_chan_1_threshold / 2);
            CapTouch_SetChNNoiseThres(CTC_CH1, app_cfg_const.ctc_chan_1_threshold / 2);
            CapTouch_ChWakeupCmd(CTC_CH1, (FunctionalState)ENABLE);
        }
    }

    if (cap_touch_channel2)
    {
        if ((app_db.device_state == APP_DEVICE_STATE_ON) || ((app_db.device_state != APP_DEVICE_STATE_ON)
                                                             && (app_cfg_const.ctc_chan_2_type == CHANNEL_TYPE_TOUCH_DET_ISR)))
        {
            CTC_PRINT_TRACE0("app_cap_touch_parameter_init channel_2");
            uint16_t ctc_chan_2_threshold = app_cfg_const.ctc_chan_2_threshold_upper << 8 |
                                            app_cfg_const.ctc_chan_2_threshold_lower;
            CapTouch_ChCmd(CTC_CH2, ENABLE);
            ctc_check_channel_enable = true;

            if ((ctc_channel_lock & APP_CAP_TOUCH_CH2) == 0)
            {
                CapTouch_ChINTConfig(CTC_CH2, (CTC_CH_INT_TYPE)(CTC_TOUCH_PRESS_INT | CTC_TOUCH_RELEASE_INT |
                                                                CTC_FALSE_TOUCH_INT), ENABLE);
            }
            CapTouch_SetChBaseline(CTC_CH2, app_cfg_const.ctc_chan_2_default_baseline);
            CapTouch_SetChMbias(CTC_CH2, app_cfg_const.ctc_chan_2_mbias);
            CapTouch_SetChDiffThres(CTC_CH2, ctc_chan_2_threshold);
            CapTouch_SetChPNoiseThres(CTC_CH2, ctc_chan_2_threshold / 2);
            CapTouch_SetChNNoiseThres(CTC_CH2, ctc_chan_2_threshold / 2);
            CapTouch_ChWakeupCmd(CTC_CH2, (FunctionalState)ENABLE);
        }
    }

    if (cap_touch_channel3)
    {
        if ((app_db.device_state == APP_DEVICE_STATE_ON) || ((app_db.device_state != APP_DEVICE_STATE_ON)
                                                             && (app_cfg_const.ctc_chan_3_type == CHANNEL_TYPE_TOUCH_DET_ISR)))
        {
            CTC_PRINT_TRACE0("app_cap_touch_parameter_init channel_3");
            CapTouch_ChCmd(CTC_CH3, ENABLE);
            ctc_check_channel_enable = true;

            if ((ctc_channel_lock & APP_CAP_TOUCH_CH3) == 0)
            {
                CapTouch_ChINTConfig(CTC_CH3, (CTC_CH_INT_TYPE)(CTC_TOUCH_PRESS_INT | CTC_TOUCH_RELEASE_INT |
                                                                CTC_FALSE_TOUCH_INT), ENABLE);
            }
            CapTouch_SetChBaseline(CTC_CH3, app_cfg_const.ctc_chan_3_default_baseline);
            CapTouch_SetChMbias(CTC_CH3, app_cfg_const.ctc_chan_3_mbias);
            CapTouch_SetChDiffThres(CTC_CH3, app_cfg_const.ctc_chan_3_threshold);
            CapTouch_SetChPNoiseThres(CTC_CH3, app_cfg_const.ctc_chan_3_threshold / 2);
            CapTouch_SetChNNoiseThres(CTC_CH3, app_cfg_const.ctc_chan_3_threshold / 2);
            CapTouch_ChWakeupCmd(CTC_CH3, (FunctionalState)ENABLE);
        }
    }

    if (ctc_test_mode_enable == false)
    {
        app_start_timer(&timer_idx_cap_touch_en_overp, "cap_touch_en_overp",
                        cap_touch_timer_id, APP_TIMER_CAP_TOUCH_EN, 0, false,
                        over_p_en_wait);
        //CapTouch_NoiseINTConfig(CTC_OVER_P_NOISE_INT, ENABLE);
    }
    slow_mode_interval = (app_db.device_state == APP_DEVICE_STATE_ON) ? slow_mode_scan_interval :
                         slow_mode_scan_interval_pw_down;

    /* Set scan interval */
    if (!CapTouch_SetScanInterval(slow_mode_interval, CTC_SLOW_MODE))
    {
        CTC_PRINT_WARN0("Slow mode scan interval overange!");
    }
    if (!CapTouch_SetScanInterval(fast_mode_scan_interval, CTC_FAST_MODE))
    {
        CTC_PRINT_WARN0("Fast mode scan interval overange!");
    }

    pmu_set_clk_32k_power_in_powerdown(true); // keep 32k clk when enter power down
}

static void app_cap_touch_enable(bool enable)
{
    if (enable && !CapTouch_IsRunning() && ctc_check_channel_enable)
    {
        CapTouch_Cmd(ENABLE, captouch_param_auto_det);
    }
    else if (!enable)
    {
        CapTouch_Cmd(DISABLE, captouch_param_auto_det);
    }
    CTC_PRINT_INFO3("app_cap_touch_enable: %d, %d %d", enable, CapTouch_IsRunning(),
                    ctc_check_channel_enable);
}

static T_CAP_TOUCH_ACTION app_cap_touch_get_curr_action(uint8_t event)
{
    return (T_CAP_TOUCH_ACTION)(event % 3);
}

static void app_cap_touch_ld_check(void)
{
    CTC_PRINT_TRACE3("app_cap_touch_ld_check ld_det %d %d %d", app_cap_touch_get_ld_det(),
                     ld_det_ch0,
                     ld_det_ch1);
    if ((((app_cfg_const.ctc_light_det_pin_num == CAP_TOUCH_LD_TYPE_TWO_CH) &&
          (ld_det_ch0 == ld_det_ch1)) ||
         (app_cfg_const.ctc_light_det_pin_num == CAP_TOUCH_LD_TYPE_ONE_CH)) &&
        app_cap_touch_get_ld_det())
    {
        T_IO_MSG gpio_msg;   //in-out ear

        gpio_msg.type = IO_MSG_TYPE_GPIO;
        gpio_msg.subtype = IO_MSG_GPIO_SENSOR_LD;
        gpio_msg.u.param = (ld_det_ch0) ? SENSOR_LD_IN_EAR : SENSOR_LD_OUT_EAR;

        app_io_msg_send(&gpio_msg);
    }
}

static void app_cap_touch_cfg_init(void)
{
#if APP_CAP_TOUCH_DBG
    CTC_PRINT_TRACE4("app_cap_touch_cfg_init CH0 %x CH1 %x CH2 %x CH3 %x",
                     app_cfg_const.ctc_chan_0_type,
                     app_cfg_const.ctc_chan_1_type,
                     app_cfg_const.ctc_chan_2_type,
                     app_cfg_const.ctc_chan_3_type);
#endif

    cap_touch_channel0 =
        ctc_trigger_event[app_cfg_const.ctc_chan_0_type].func;
    cap_touch_channel1 =
        ctc_trigger_event[app_cfg_const.ctc_chan_1_type].func;
    cap_touch_channel2 =
        ctc_trigger_event[app_cfg_const.ctc_chan_2_type].func;
    cap_touch_channel3 =
        ctc_trigger_event[app_cfg_const.ctc_chan_3_type].func;
}

static void app_cap_touch_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    CTC_PRINT_TRACE2("app_cap_touch_timeout_cb: timer_evt 0x%02x, param %d", timer_evt, param);

    switch (timer_evt)
    {
    case APP_TIMER_CAP_TOUCH_DLPS_CHECK:
        {
            app_stop_timer(&timer_idx_cap_touch_dlps_check);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
        }
        break;

    case APP_TIMER_CAP_TOUCH_EN:
        {
            app_stop_timer(&timer_idx_cap_touch_en_overp);
            CapTouch_NoiseINTClearPendingBit(CTC_OVER_P_NOISE_INT);
            CapTouch_NoiseINTConfig(CTC_OVER_P_NOISE_INT, ENABLE);
        }
        break;

#if APP_CAP_TOUCH_DBG
    case APP_TIMER_CAP_TOUCH_CHANNEL_ADC:
        {
            app_stop_timer(&timer_idx_cap_touch_channel_adc);
            app_start_timer(&timer_idx_cap_touch_channel_adc, "cap_touch_channel_adc",
                            cap_touch_timer_id, APP_TIMER_CAP_TOUCH_CHANNEL_ADC, 0, false,
                            500);
            //captouch_get_reg(0x108) : cap touch register addres

            if (cap_touch_channel0)
            {
                CTC_PRINT_TRACE4("ch0_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                                 CapTouch_GetChBaseline(CTC_CH0),
                                 CapTouch_GetChMbias(CTC_CH0),
                                 CapTouch_GetChDiffThres(CTC_CH0),
                                 CapTouch_GetChAveData(CTC_CH0));
            }
            if (cap_touch_channel1)
            {
                CTC_PRINT_TRACE4("ch1_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                                 CapTouch_GetChBaseline(CTC_CH1),
                                 CapTouch_GetChMbias(CTC_CH1),
                                 CapTouch_GetChDiffThres(CTC_CH1),
                                 CapTouch_GetChAveData(CTC_CH1));
            }
            if (cap_touch_channel2)
            {
                CTC_PRINT_TRACE4("ch2_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                                 CapTouch_GetChBaseline(CTC_CH2),
                                 CapTouch_GetChMbias(CTC_CH2),
                                 CapTouch_GetChDiffThres(CTC_CH2),
                                 CapTouch_GetChAveData(CTC_CH2));
            }
            if (cap_touch_channel3)
            {
                CTC_PRINT_TRACE4("ch3_baseline %d curr_mbias %d curr_thres %d curr_avg %d",
                                 CapTouch_GetChBaseline(CTC_CH3),
                                 CapTouch_GetChMbias(CTC_CH3),
                                 CapTouch_GetChDiffThres(CTC_CH3),
                                 CapTouch_GetChAveData(CTC_CH3));
            }
        }
        break;
#endif
    case APP_TIMER_CAP_TOUCH_REPORT_ADC_VALUE:
        {
            uint8_t buf[4] = {0};
            uint16_t average_data;
            uint8_t channel = param;
            if (ctc_adc_report_type == CAP_TOUCH_ADC_REPORT_TYPE_REALTIME)
            {
                average_data = CapTouch_GetChAveData((CTC_CH_TYPE)channel);

                buf[0] = CAP_TOUCH_EVENT_REPORT_CURRENT_ADC; //status_index
                buf[1] = channel;
                memcpy(&buf[2], &average_data, sizeof(average_data));
                app_report_event_broadcast(EVENT_CAP_TOUCH_CTL, buf, 4);
                app_transfer_pop_data_queue(CMD_PATH_SPP, true);
                app_start_timer(&timer_idx_cap_touch_report_adc_value, "cap_touch_report_adc_value",
                                cap_touch_timer_id, APP_TIMER_CAP_TOUCH_REPORT_ADC_VALUE, channel, false,
                                APP_CAP_TOUCH_REPORT_INTERVAL);
            }
        }
        break;
    case APP_TIMER_CAP_TOUCH_REPORT_ADC_RECORD:
        {
            uint8_t channel = param;
            app_stop_timer(&timer_idx_cap_touch_report_adc_record);
            if (ctc_adc_report_type == CAP_TOUCH_ADC_REPORT_TYPE_RECORD)
            {
                uint16_t avg_data = 0;
                if (record_data_num < APP_CAP_TOUCH_RECORD_DATA_NUM - 1)
                {
                    avg_data = CapTouch_GetChAveData((CTC_CH_TYPE)channel);
                    memcpy(&ctc_adc_report_value[record_data_num * 2 + 2], &avg_data, 2);
                    record_data_num++;
                    app_start_timer(&timer_idx_cap_touch_report_adc_record, "cap_touch_report_adc_record",
                                    cap_touch_timer_id, APP_TIMER_CAP_TOUCH_REPORT_ADC_RECORD, channel, false,
                                    APP_CAP_TOUCH_RECORD_INTERVAL);
                }
                else
                {
                    avg_data = CapTouch_GetChAveData((CTC_CH_TYPE)channel);
                    memcpy(&ctc_adc_report_value[record_data_num * 2 + 2], &avg_data, 2);
                    app_report_event_broadcast(EVENT_CAP_TOUCH_CTL, ctc_adc_report_value,
                                               APP_CAP_TOUCH_RECORD_DATA_NUM * 2 + 2);
                    free(ctc_adc_report_value);
                    ctc_adc_report_value = NULL;
                    ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_OFF;
                    record_data_num = 0;
                }
            }
            else
            {
                if (ctc_adc_report_value)
                {
                    free(ctc_adc_report_value);
                    ctc_adc_report_value = NULL;
                }
            }
        }
        break;

    case APP_TIMER_CAP_TOUCH_CALIBRATION:
        {
            uint8_t calibration_cnt = param;
            app_stop_timer(&timer_idx_cap_touch_calibration);
            app_cap_touch_load_param();

            if (app_cap_touch_calibration_check_param(&calibration_cnt))
            {
                app_cap_touch_reset_test_mode_setting();
            }
            else
            {
                app_start_timer(&timer_idx_cap_touch_calibration, "cap_touch_calibration",
                                cap_touch_timer_id, APP_TIMER_CAP_TOUCH_CALIBRATION, calibration_cnt, false,
                                APP_CAP_TOUCH_CALIBRATION_INTERVAL);
            }
        }
        break;

    default:
        break;
    }
}

static bool app_cap_touch_enable_adc_report(uint8_t channel, bool enable)
{
    uint8_t error_code = 0;
    if (enable)
    {
        if (ctc_adc_report_type != CAP_TOUCH_ADC_REPORT_TYPE_OFF)
        {
            error_code = 1;
            goto ENABLE_ADC_REPORT_ERROR;
        }
        else if (!CapTouch_GetChStatus((CTC_CH_TYPE)channel))
        {
            error_code = 2;
            goto ENABLE_ADC_REPORT_ERROR;
        }
        else
        {
            ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_REALTIME;
            app_stop_timer(&timer_idx_cap_touch_report_adc_value);
            app_start_timer(&timer_idx_cap_touch_report_adc_value, "cap_touch_report_adc_value",
                            cap_touch_timer_id, APP_TIMER_CAP_TOUCH_REPORT_ADC_VALUE, channel, false,
                            APP_CAP_TOUCH_FIRST_REPORT_DELAY_TIME);
            return true;
        }
    }
    else
    {
        if (ctc_adc_report_type != CAP_TOUCH_ADC_REPORT_TYPE_REALTIME)
        {
            error_code = 3;
            goto ENABLE_ADC_REPORT_ERROR;
        }
        else
        {
            ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_OFF;
            app_stop_timer(&timer_idx_cap_touch_report_adc_value);
            app_transfer_queue_reset(CMD_PATH_SPP);
            return true;
        }
    }
ENABLE_ADC_REPORT_ERROR:
    CTC_PRINT_ERROR1("app_cap_touch_enable_adc_report error code %d", error_code);
    return false;
}

static bool app_cap_touch_enable_adc_record(uint8_t channel, bool enable)
{
    uint8_t error_code = 0;
    if (enable)
    {
        if (ctc_adc_report_type != CAP_TOUCH_ADC_REPORT_TYPE_OFF)
        {
            error_code = 1;
            goto ENABLE_ADC_RECORD_ERROR;
        }
        else if (!CapTouch_GetChStatus((CTC_CH_TYPE)channel))
        {
            error_code = 2;
            goto ENABLE_ADC_RECORD_ERROR;
        }
        else
        {
#if APP_CAP_TOUCH_DBG
            CTC_PRINT_WARN1("unused memory: %d", mem_peek());
#endif
            ctc_adc_report_value = malloc(APP_CAP_TOUCH_RECORD_DATA_NUM * 2 + 2);
            if (ctc_adc_report_value != NULL)
            {
                ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_RECORD;
                record_data_num = 0;
                memset(ctc_adc_report_value, 0xff, APP_CAP_TOUCH_RECORD_DATA_NUM * 2 + 2);
                ctc_adc_report_value[0] = CAP_TOUCH_EVENT_REPORT_PERIOD_ADC;
                ctc_adc_report_value[1] = channel;
                app_stop_timer(&timer_idx_cap_touch_report_adc_record);
                app_start_timer(&timer_idx_cap_touch_report_adc_record, "cap_touch_report_adc_record",
                                cap_touch_timer_id, APP_TIMER_CAP_TOUCH_REPORT_ADC_RECORD, channel, false,
                                APP_CAP_TOUCH_FIRST_REPORT_DELAY_TIME);
                return true;
            }
            else
            {
                error_code = 3;
                goto ENABLE_ADC_RECORD_ERROR;
            }
        }
    }
    else
    {
        if (ctc_adc_report_type != CAP_TOUCH_ADC_REPORT_TYPE_RECORD)
        {
            error_code = 4;
            goto ENABLE_ADC_RECORD_ERROR;
        }
        else
        {
            ctc_adc_report_type = CAP_TOUCH_ADC_REPORT_TYPE_OFF;
            app_stop_timer(&timer_idx_cap_touch_report_adc_value);
            if (ctc_adc_report_value != NULL)
            {
                app_report_event_broadcast(EVENT_CAP_TOUCH_CTL, ctc_adc_report_value,
                                           APP_CAP_TOUCH_RECORD_DATA_NUM * 2 + 2);
                free(ctc_adc_report_value);
                ctc_adc_report_value = NULL;
                record_data_num = 0;
            }
            return true;
        }
    }
ENABLE_ADC_RECORD_ERROR:
    CTC_PRINT_ERROR1("app_cap_touch_enable_adc_record error code %d", error_code);
    return false;
}

static void app_cap_touch_set_parameter(T_APP_CAP_SPP_CFG_SET *p_data, bool check_calibration)
{
    for (uint8_t ch = 0; ch < APP_CAP_TOUCH_MAX_CHANNEL_NUM ; ch++)
    {
        if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)ch) &&
            ((check_calibration == false) || ((check_calibration == true) &&
                                              (p_data[ch].calibr_st == APP_CAP_TOUCH_CAL_SUCCESS))))
        {
            CTC_PRINT_TRACE4("app_cap_touch_set_parameter CH %d baseline %d mbias %d thres %d",
                             ch,
                             p_data[ch].baseline,
                             p_data[ch].mbias,
                             p_data[ch].threshold);

            CapTouch_SetChBaseline((CTC_CH_TYPE)ch, p_data[ch].baseline);
            CapTouch_SetChMbias((CTC_CH_TYPE)ch, p_data[ch].mbias);
            CapTouch_SetChDiffThres((CTC_CH_TYPE)ch, p_data[ch].threshold);
            CapTouch_SetChPNoiseThres((CTC_CH_TYPE)ch, p_data[ch].threshold / 2);
            CapTouch_SetChNNoiseThres((CTC_CH_TYPE)ch, p_data[ch].threshold / 2);
        }
    }
}

static uint8_t app_cap_touch_set_cfg(uint8_t cmd_id, uint8_t *cmd_ptr)
{
    uint8_t ret = CMD_SET_STATUS_PROCESS_FAIL;
    uint16_t baseline;
    uint16_t mbias;
    uint16_t threshold;
    uint16_t average_data;
    uint8_t channel_num = cmd_ptr[0];
    if (!ctc_test_mode_enable)
    {
        ret = CMD_SET_STATUS_DISALLOW;
        return ret;
    }

    app_cap_touch_reset_parameter();

    switch (cmd_id)
    {
    case CAP_TOUCH_CMD_SET_BASELINE:
        {
            baseline = (uint16_t)(cmd_ptr[1] | (cmd_ptr[2] << 8));
            if (baseline <= APP_CAP_TOUCH_MAX_BASELINE)
            {
                cap_touch_param_info[channel_num].baseline = baseline;
                ret = CMD_SET_STATUS_COMPLETE;
            }
        }
        break;
    case CAP_TOUCH_CMD_SET_MBIAS:
        {
            mbias = cmd_ptr[1];
            if (mbias < APP_CAP_TOUCH_MAX_MBIAS_LEVEL)
            {
                cap_touch_param_info[channel_num].mbias = mbias;
                ret = CMD_SET_STATUS_COMPLETE;
            }
        }
        break;
    case CAP_TOUCH_CMD_SET_THRESHOLD:
        {
            threshold = (uint16_t)(cmd_ptr[1] | (cmd_ptr[2] << 8));
            if (threshold <= APP_CAP_TOUCH_MAX_THRESHOLD)
            {
                cap_touch_param_info[channel_num].threshold = threshold;
                ret = CMD_SET_STATUS_COMPLETE;
            }
        }
        break;
    }
    app_cap_touch_set_parameter(cap_touch_param_info, false);

    app_cap_touch_enable(true);
    return ret;
}

static bool app_cap_touch_check_channel_type_enable(CTC_CH_TYPE channel_num)
{
    if (((app_cfg_const.ctc_chan_0_type) && (channel_num == CTC_CH0)) ||
        ((app_cfg_const.ctc_chan_1_type) && (channel_num == CTC_CH1)) ||
        ((app_cfg_const.ctc_chan_2_type) && (channel_num == CTC_CH2)) ||
        ((app_cfg_const.ctc_chan_3_type) && (channel_num == CTC_CH3)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void app_cap_touch_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    bool handle = true;

    switch (event_type)
    {
    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            if (ctc_test_mode_enable)
            {
                app_cap_touch_reset_all_setting();
                app_cap_touch_reset_test_mode_setting();
            }
        }
        break;
    default:
        handle = false;
        break;
    }
    if (handle)
    {
        CTC_PRINT_TRACE1("app_cap_touch_bt_cback type 0x%x", event_type);
    }
}

static void app_cap_touch_reg_timer(void)
{
    app_timer_reg_cb(app_cap_touch_timeout_cb, &cap_touch_timer_id);
}

void app_cap_touch_set_ld_det(bool enable)
{
    ctc_ld_set_enable = enable;
}

bool app_cap_touch_get_ld_det(void)
{
    return ctc_ld_set_enable;
}

void app_cap_touch_msg_handle(T_IO_MSG_CAP_TOUCH subtype)
{
    CTC_PRINT_TRACE1("app_cap_touch_msg_handle 0x%x", subtype);

    if (timer_idx_cap_touch_dlps_check)
    {
        app_stop_timer(&timer_idx_cap_touch_dlps_check);
    }

    switch (subtype)
    {
    case IO_MSG_CAP_TOUCH_OVER_P_NOISE: // press ISR
        {
            app_cap_touch_reset_all_setting();
        }
        break;

    case IO_MSG_CAP_CH0_PRESS:
    case IO_MSG_CAP_CH0_RELEASE:
    case IO_MSG_CAP_CH0_FALSE_TRIGGER:
        {
            if (cap_touch_channel0)
            {
                cap_touch_channel0(subtype);
            }
        }
        break;

    case IO_MSG_CAP_CH1_PRESS:
    case IO_MSG_CAP_CH1_RELEASE:
    case IO_MSG_CAP_CH1_FALSE_TRIGGER:
        {
            if (cap_touch_channel1)
            {
                cap_touch_channel1(subtype);
            }
        }
        break;

    case IO_MSG_CAP_CH2_PRESS:
    case IO_MSG_CAP_CH2_RELEASE:
    case IO_MSG_CAP_CH2_FALSE_TRIGGER:
        {
            if (cap_touch_channel2)
            {
                cap_touch_channel2(subtype);
            }
        }
        break;

    case IO_MSG_CAP_CH3_PRESS:
    case IO_MSG_CAP_CH3_RELEASE:
    case IO_MSG_CAP_CH3_FALSE_TRIGGER:
        {
            if (cap_touch_channel3)
            {
                cap_touch_channel3(subtype);
            }
        }
        break;

    default:
        break;
    }

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
}

void app_cap_touch_cmd_param_handle(uint8_t cmd_path, uint8_t *cmd_ptr, uint8_t app_idx)
{
    uint8_t sub_id = cmd_ptr[2];
    uint8_t i;
    uint8_t temp = 0;
    uint8_t  ack_pkt[3];
    uint16_t baseline;
    uint16_t mbias;
    uint16_t threshold;
    uint16_t average_data;
    ack_pkt[0] = cmd_ptr[0];
    ack_pkt[1] = cmd_ptr[1];
    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;

    CTC_PRINT_TRACE1("app_cap_touch_cmd_param_handle sub_id %d", sub_id);
    switch (sub_id)
    {
    case CAP_TOUCH_CMD_GET_STATUS:
        {
            uint8_t *buf = NULL;//sub event_id 1 + test_mode status 1 + channel data 36
            buf = malloc(APP_CAP_TOUCH_MAX_CHANNEL_NUM * 9 + 2);
            memset(buf, 0x00, APP_CAP_TOUCH_MAX_CHANNEL_NUM * 9 + 2);
            buf[0] = CAP_TOUCH_EVENT_REPORT_STATUS;
            buf[1] = ctc_test_mode_enable;
            for (i = 0; i < APP_CAP_TOUCH_MAX_CHANNEL_NUM; i++)
            {
                temp = i * 9 + 2;
                buf[temp++] = i;
                if (app_cap_touch_check_channel_type_enable((CTC_CH_TYPE)i))
                {
                    baseline = CapTouch_GetChBaseline((CTC_CH_TYPE)i);
                    memcpy(&buf[temp], &baseline, sizeof(baseline));
                    temp += 2;
                    mbias = CapTouch_GetChMbias((CTC_CH_TYPE)i);
                    memcpy(&buf[temp], &mbias, sizeof(mbias));
                    temp += 2;
                    threshold = CapTouch_GetChDiffThres((CTC_CH_TYPE)i);
                    memcpy(&buf[temp], &threshold, sizeof(threshold));
                    temp += 2;
                    average_data = CapTouch_GetChAveData((CTC_CH_TYPE)i);
                    memcpy(&buf[temp], &average_data, sizeof(average_data));
                }
            }
            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_CAP_TOUCH_CTL, app_idx, buf,
                             APP_CAP_TOUCH_MAX_CHANNEL_NUM * 9 + 2);
            free(buf);
        }
        break;
    case CAP_TOUCH_CMD_SET_BASELINE:
    case CAP_TOUCH_CMD_SET_MBIAS:
    case CAP_TOUCH_CMD_SET_THRESHOLD:
        {
            ack_pkt[2] = app_cap_touch_set_cfg(sub_id, &cmd_ptr[3]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    case CAP_TOUCH_CMD_SET_TEST_MODE_STATUS:
        {
            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;
            bool enable = cmd_ptr[3];
            app_cfg_const.dt_resend_num = 0;

            if (enable)
            {
                app_cap_touch_load_param();
            }
            else
            {
                app_cap_touch_reset_all_setting();
                app_cap_touch_reset_test_mode_setting();
            }
            ctc_test_mode_enable = enable;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    case CAP_TOUCH_CMD_GET_TEST_MODE_STATUS:
        {
            uint8_t buf[2] = {0};
            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;
            buf[0] = CAP_TOUCH_EVENT_REPORT_TEST_MODE_STATUS;
            buf[1] = ctc_test_mode_enable;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(cmd_path, EVENT_CAP_TOUCH_CTL, app_idx, buf,
                             sizeof(buf));
        }
        break;
    case CAP_TOUCH_CMD_SET_CURRENT_ADC_REPORT:
    case CAP_TOUCH_CMD_SET_PERIOD_ADC_REPORT:
        {
            uint8_t channel = cmd_ptr[3];
            bool enable = (bool)cmd_ptr[4];

            if (ctc_test_mode_enable)
            {
                if (!CapTouch_IsChAllowed((CTC_CH_TYPE)channel))
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                }
                else
                {
                    if (sub_id == CAP_TOUCH_CMD_SET_CURRENT_ADC_REPORT)
                    {
                        if (!app_cap_touch_enable_adc_report(channel, enable))
                        {
                            ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        }
                        else
                        {
                            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;
                        }
                    }
                    else
                    {
                        if (!app_cap_touch_enable_adc_record(channel, enable))
                        {
                            ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        }
                        else
                        {
                            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;
                        }
                    }
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CAP_TOUCH_CMD_SET_CALIBRATION:
        {
            report_cmd_path = cmd_path;
            report_cmd_idx = app_idx;
            app_cap_touch_calibration_start();
        }
        break;
    case CAP_TOUCH_CMD_SAVE_PARAM_TO_FTL:
        {
            uint8_t channel = cmd_ptr[3];
            app_cap_touch_load_param();
            memcpy(&cap_touch_calibration_data[channel], &cap_touch_param_info[channel],
                   sizeof(T_APP_CAP_SPP_CFG_SET));
            app_cap_touch_calibration_save_data(cap_touch_calibration_data);
            ack_pkt[2] = CMD_SET_STATUS_COMPLETE;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    }
}

void app_cap_touch_send_event_check(void)
{
//to do
}

void app_cap_touch_init(void)
{
    bt_mgr_cback_register(app_cap_touch_bt_cback);
    sys_mgr_cback_register(app_cap_touch_dm_cback);
}

void app_cap_touch_driver_init(bool first_power_on)
{
#if APP_CAP_TOUCH_DBG
    uint32_t reg_data = 0;
    CTC_PRINT_TRACE1("app_cap_touch_driver_init start reg_data %d", reg_data);
    reg_data = HAL_READ32(0x40007000UL, (0x0));
    CTC_PRINT_TRACE1("app_cap_touch_driver_init get reg_data %d", reg_data);
#endif

    /*need cap touch mcu configure setting
    1.using channel
    2.isr
    3.scan interval:CTC_SLOW_MODE & CTC_FAST_MODE*/
    if (first_power_on)
    {
        cap_touch_calibration_data = calloc(1, APP_RW_CAPTOUCH_CALIBRATION_SIZE);
        app_cap_touch_calibration_load_data(cap_touch_calibration_data);
        app_cap_touch_cfg_init();
        app_cap_touch_reg_timer();
    }
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);

    app_cap_touch_parameter_init();
    app_cap_touch_calibraion_read_value();

    /* Cap Touch start */
    app_cap_touch_enable(true);
#if APP_CAP_TOUCH_DBG
    CTC_PRINT_TRACE4("MBIAS %d,%d,%d,%d", app_cfg_const.ctc_chan_0_mbias,
                     app_cfg_const.ctc_chan_1_mbias, app_cfg_const.ctc_chan_2_mbias, app_cfg_const.ctc_chan_3_mbias);
    CTC_PRINT_TRACE4("DiffThreshold %d,%d,%d,%d", app_cfg_const.ctc_chan_0_threshold,
                     app_cfg_const.ctc_chan_1_threshold,
                     app_cfg_const.ctc_chan_2_threshold_upper << 8 | app_cfg_const.ctc_chan_2_threshold_lower,
                     app_cfg_const.ctc_chan_3_threshold);
#endif
    CTC_PRINT_TRACE0("app_cap_touch_driver_init");

    app_start_timer(&timer_idx_cap_touch_dlps_check, "cap_touch_dlps_check",
                    cap_touch_timer_id, APP_TIMER_CAP_TOUCH_DLPS_CHECK, 0, false,
                    1000);
#if APP_CAP_TOUCH_DBG
    app_start_timer(&timer_idx_cap_touch_channel_adc, "cap_touch_channel_adc",
                    cap_touch_timer_id, APP_TIMER_CAP_TOUCH_CHANNEL_ADC, 0, false,
                    100);
#endif
}

#endif

