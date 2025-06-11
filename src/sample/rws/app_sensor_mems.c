#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "trace.h"
#include "board.h"
#include "platform_utils.h"
#include "sysm.h"
#include "pm.h"
#include "bt_hfp.h"
#include "app_cfg.h"
#include "app_hfp.h"
#include "app_a2dp.h"
#include "app_dlps.h"
#include "hal_gpio.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "app_msg.h"
#include "app_timer.h"
#include "app_main.h"
#include "app_sensor_mems.h"
#include "app_io_msg.h"
#include "section.h"
#include "app_relay.h"
#include "app_sniff_mode.h"
#include "app_bud_loc.h"
#include "app_sniff_mode.h"
#include "app_multilink.h"
#include "app_cmd.h"

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
#include "os_ext.h"
#include "app_timer.h"
#include "inv_imu_transport.h"
#include "inv_imu_driver.h"
#include "inv_imu_defs.h"
#include "InvError.h"
#endif

#if F_APP_DUAL_AUDIO_EFFECT
#include "app_dual_audio_effect.h"
#include <math.h>
#include "pm.h"
#endif

static uint8_t app_sensor_mems_timer_id = 0;
static uint8_t app_sensor_mems_freq_handle = 0;

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
#define ATTI_REPORT_MS      20
#define pdFALSE ( ( BaseType_t ) 0 )
#define pdPASS  ( ( BaseType_t ) 1 )
#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
static uint8_t timer_idx_qst_report = 0;
#define QST_REPORT_MS      20
#endif

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
static uint8_t timer_idx_inv_report = 0;
#if F_APP_DUAL_AUDIO_EFFECT
static uint8_t timer_idx_stop_sensor = 0;
#endif

#if (SENSOR_25HZ == 1)
#define INV_REPORT_MS      40
#else
#define INV_REPORT_MS      20
#endif

static uint8_t fifo_index = 0;
static bool init_suc = false;
#endif

typedef enum
{
    APP_TIMER_INV_REPORT,
    APP_TIMER_QST_REPORT,
    APP_TIMER_STOP_SENSOR,
} T_APP_SENSOR_MEMS_TIMER_ID;

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
#include "qmi8658.h"
#include "imualgo_axis9.h"
#endif

#if (F_APP_CWM_ALGO_SUPPORT == 1)
#include "cwm_lib.h"
#include "os_api.h"
#include "os_sched.h"
#include "app_report.h"
#endif

#if (F_APP_CWM_ALGO_SUPPORT == 1)
static float cwm_accRaw[3] = {0.0f};
static float cwm_gyrRaw[3] = {0.0f};
static uint8_t drop_data;
static int32_t cwm_atti[14] = {0};
static bool cwm_atti_need_set = false;
#endif

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
static struct inv_imu_serif icm_serif;
static struct inv_imu_device icm_driver;
#endif

static int cmw_debug_func(const char *dbg_log)
{
    IO_PRINT_INFO1("cmw_dbg: %s", TRACE_STRING(dbg_log));
    return -1;
}

static int CWM_OS_dbgOutput(const char *format)
{
    cmw_debug_func(format);
    return 0;
}

static char tBuffer[150];
static int CWM_OS_dbgPrintf(const char *format, ...)
{
    va_list argList;
    int size = 0;

    va_start(argList, format);
    size = vsnprintf(tBuffer, sizeof(tBuffer), format, argList);
    va_end(argList);
    CWM_OS_dbgOutput(tBuffer);
    return size;
}

static bool app_sensor_mems_need_report(uint8_t *app_idx)
{
    bool need_report = false;
    uint8_t num_mems = app_link_get_misc_num(APP_LINK_MEMS);
#if F_APP_DURIAN_SUPPORT
    uint8_t num_stream = app_link_get_misc_num(APP_LINK_STREAM),
            num_avp = app_link_get_misc_num(APP_LINK_AVP);
#endif
    uint8_t sniffing_state = 0, a2dp_active_idx = 0;
    T_APP_CALL_STATUS call_status = app_hfp_get_call_status();

    if (call_status != APP_CALL_IDLE)
    {
        goto mems_report;
    }

    if (num_mems >= 2)
    {
        goto mems_report;
    }

#if F_APP_DURIAN_SUPPORT
    if ((num_avp >= 2) && (num_stream >= 2))
    {
        goto mems_report;
    }
#endif

    a2dp_active_idx = app_a2dp_get_active_idx();

    if (!app_db.br_link[a2dp_active_idx].mems_is_start)
    {
        goto mems_report;
    }

    sniffing_state = app_db.br_link[a2dp_active_idx].bt_sniffing_state;

    if ((sniffing_state != APP_BT_SNIFFING_STATE_IDLE) &&
        (sniffing_state != APP_BT_SNIFFING_STATE_SNIFFING) &&
        (sniffing_state != APP_BT_SNIFFING_STATE_READY))
    {
        goto mems_report;
    }

    need_report = true;
    *app_idx = a2dp_active_idx;

mems_report:
#if F_APP_DURIAN_SUPPORT
    IO_PRINT_TRACE7("app_sensor_mems_need_report: call_status %d num_avp %d num_mems %d num_stream %d sniffing_state %d app_idx %d need_report %d",
                    call_status, num_avp, num_mems, num_stream, sniffing_state, a2dp_active_idx, need_report);
#else
    IO_PRINT_TRACE5("app_sensor_mems_need_report: call_status %d num_mems %d sniffing_state %d app_idx %d need_report %d",
                    call_status, num_mems, sniffing_state, a2dp_active_idx, need_report);
#endif
    return need_report;
}

void app_sensor_mems_atti_flag_set(void)
{
#if (F_APP_CWM_ALGO_SUPPORT == 1)
    cwm_atti_need_set = true;
#endif
}

void app_sensor_mems_atti_save(int32_t *dat)
{
#if (F_APP_CWM_ALGO_SUPPORT == 1)
    memcpy((uint8_t *)&cwm_atti[0], (uint8_t *)dat, sizeof(cwm_atti));
#endif
}

void app_sensor_mems_atti_get(int32_t *dat)
{
#if (F_APP_CWM_ALGO_SUPPORT == 1)
    uint8_t num_mems = app_link_get_misc_num(APP_LINK_MEMS);
    SettingControl_t scl = {0};

    scl.iData[0] = 1;
    scl.iData[1] = 1;
    CWM_SettingControl(SCL_HS_ORIEN_CTRL_EXC, &scl);

    if (num_mems > 0)
    {
        memcpy((uint8_t *)dat, (uint8_t *)&scl.iData[2], sizeof(scl) - sizeof(scl.iData[0]) * 2);
    }
#endif
}

#if (F_APP_CWM_ALGO_SUPPORT == 1)
#define BIAS_EN
#define CWM_LOG_EN

#if TARGET_RTL8753GFE
#undef APP_CWM_LIB_SERIAL
#define APP_CWM_LIB_SERIAL               2
#endif

#if TARGET_RTL8753EFE_VS|TARGET_RTL8753EFE_VS_AVP|TARGET_RTL8773EFE_AVP|TARGET_RTL8763ESE_AVP|TARGET_RTL8763ESE
#undef APP_CWM_LIB_SERIAL
#define APP_CWM_LIB_SERIAL               19010100
#endif

static int cwm_init = 0;
static int m_req_sensor_state;

#define IDX_ALGO_HS_FUSION  100
#define IDX_HS_ORIG_ADV     106
#define ACC_COORDINATE_IDX   0
#define GYR_COORDINATE_IDX   0

#define SENSOR_ODR_25HZ     25
#define SENSOR_ODR_50HZ     50
#define SENSOR_ODR_100HZ    100
#define SENSOR_ODR_200HZ    200

#if (SENSOR_25HZ == 1)
#define SENSOR_ODR          SENSOR_ODR_25HZ
#else
#define SENSOR_ODR          SENSOR_ODR_50HZ
#endif

#define SENSOR_DT_US        (1000000/SENSOR_ODR)

// at 2G condition: 1 LSB = 1 / 16384 g = (1/16384)*1000 mg
#ifndef GRAVITY_EARTH
#define GRAVITY_EARTH       9.8066f
#endif
#define ACC_RANGE_2G_CONVERT    (GRAVITY_EARTH*(1.0f/16384.0f)) //unit: m/s^2
#define ACC_RANGE_4G_CONVERT    (GRAVITY_EARTH*(1.0f/8192.0f))
#define ACC_RANGE_8G_CONVERT    (GRAVITY_EARTH*(1.0f/4096.0f))
#define ACC_RANGE_16G_CONVERT   (GRAVITY_EARTH*(1.0f/2048.0f))
#define ACC_RANGE_CONVERT       ACC_RANGE_4G_CONVERT

// at 2000 dps condition: 1 LSB = 1 / 16.4 dps = (1/16.4)*1000 mdps
//                to rps: dps * (PI/180.0f)
#ifndef M_PI
#define M_PI                3.1415926f
#endif
#define GYR_RANGE_2000DPS_CONVERT   ((1.0f/16.4f) *(M_PI/180.0f)) //unit: rps
#define GYR_RANGE_1000DPS_CONVERT   ((1.0f/32.8f) *(M_PI/180.0f))
#define GYR_RANGE_500DPS_CONVERT    ((1.0f/65.6f) *(M_PI/180.0f))
#define GYR_RANGE_250DPS_CONVERT    ((1.0f/131.2f)*(M_PI/180.0f))
#define GYR_RANGE_125DPS_CONVERT    ((1.0f/262.4f)*(M_PI/180.0f))
#define GYR_RANGE_CONVERT           GYR_RANGE_2000DPS_CONVERT

#define IS_HIGH_RES_MODE  0
#define IS_LOW_NOISE_MODE 1
#define INV_42607_SLAVE_ADDR              0x68

#if (SENSOR_25HZ == 1)
#define GYRO_FREQ            GYRO_CONFIG0_ODR_25_HZ
#define ACCEL_FREQ           ACCEL_CONFIG0_ODR_25_HZ
#else
#define GYRO_FREQ            GYRO_CONFIG0_ODR_50_HZ
#define ACCEL_FREQ           ACCEL_CONFIG0_ODR_50_HZ
#endif

/* Full Scale Range */
#if IS_HIGH_RES_MODE
#if defined(ICM43686)
static const int32_t acc_fsr = 32;   /* +/- 32g */
static const int32_t gyr_fsr = 4000; /* +/- 4000dps */
#else
static const int32_t acc_fsr = 16;   /* +/- 16g */
static const int32_t gyr_fsr = 2000; /* +/- 2000dps */
#endif
#else
static const int32_t acc_fsr = 4;        /* +/- 4g */
static const int32_t gyr_fsr = 2000;     /* +/- 2000dps */
#endif

static struct inv_imu_serif icm_serif;
static struct inv_imu_device icm_driver;

#define ONE_LEN_RTK        36
#define RTK_REPORT_COUNT   10
#define REPORT_LENTH_RTK   (ONE_LEN_RTK*RTK_REPORT_COUNT)

#define CYWEE_REPORT_COUNT   1
#define ONE_LEN_CYWEE_IMU        36
#define REPORT_LENTH_CYWEE_IMU   (ONE_LEN_CYWEE_IMU*CYWEE_REPORT_COUNT + 1)
#define ONE_LEN_CYWEE_ADV        28
#define REPORT_LENTH_CYWEE_ADV   (ONE_LEN_CYWEE_ADV*CYWEE_REPORT_COUNT + 1)

static uint8_t *sensor_report_buf;
static uint8_t *sensor_report_buf_adv;
void app_sensor_mems_spp_report_init(void)
{
    if (app_db.report == REPORT_RTK)
    {
        sensor_report_buf = malloc(REPORT_LENTH_RTK);
    }
    else if (app_db.report == REPORT_CYWEE)
    {
        sensor_report_buf = malloc(REPORT_LENTH_CYWEE_IMU);
        sensor_report_buf_adv = malloc(REPORT_LENTH_CYWEE_ADV);
    }
}

void app_sensor_mems_spp_report_de_init(void)
{
    free(sensor_report_buf);
    if (app_db.report == REPORT_CYWEE)
    {
        free(sensor_report_buf_adv);
    }
}

#if F_APP_DUAL_AUDIO_EFFECT
void app_sensor_mems_stop_sensor(void)
{
    if (app_cfg_const.mems_support)
    {
        app_db.spatial_off_anchor =  os_sys_time_get();
        app_start_timer(&timer_idx_stop_sensor, "stop_sensor",
                        app_sensor_mems_timer_id, APP_TIMER_STOP_SENSOR, 0, false,
                        200);
    }
}
#endif

static uint8_t app_sensor_mems_report_idx(void)
{
    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & A2DP_PROFILE_MASK)
        {
            return i;
        }
    }

    for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].connected_profile & SPP_PROFILE_MASK)
        {
            return i;
        }
    }
    return MAX_BR_LINK_NUM;
}

static void app_sensor_mems_spp_report_data(float *data)
{
    static uint8_t idx = 0;
    uint8_t buf[ONE_LEN_RTK];
    memcpy(&buf[0], &cwm_accRaw[0], 4);
    memcpy(&buf[4], &cwm_accRaw[1], 4);
    memcpy(&buf[8], &cwm_accRaw[2], 4);
    memcpy(&buf[12], &cwm_gyrRaw[0], 4);
    memcpy(&buf[16], &cwm_gyrRaw[1], 4);
    memcpy(&buf[20], &cwm_gyrRaw[2], 4);
    for (uint8_t i = 0; i < 3; i++)
    {
        memcpy(&buf[24 + 4 * i], &data[i], 4);
    }
    memcpy(&sensor_report_buf[idx * ONE_LEN_RTK], &buf[0], sizeof(buf));
    idx++;
    if (idx == RTK_REPORT_COUNT)
    {
        idx = 0;
        app_report_event(CMD_PATH_SPP, EVENT_REPORT_SPACE_SENSOR_DATA, app_sensor_mems_report_idx(),
                         sensor_report_buf, REPORT_LENTH_RTK);
    }
}

static const char *REQ_SENS_NAME[] = {"acc", "gyro", "mag", "", "", "", "", "", "", "", ""};

struct
{
    uint32_t sys_time_ori;
    uint32_t sys_time_cur;
    uint16_t int_counts;
    uint32_t sensor_delta;
    bool delta_caled;
} cwm_delta;

static void  cwm_mems_delta_handle(void)
{
    if (cwm_delta.delta_caled)
    {
        return;
    }
    cwm_delta.sys_time_cur = os_sys_time_get();

    if (cwm_delta.sys_time_ori == 0)
    {
        cwm_delta.sys_time_ori = cwm_delta.sys_time_cur;
    }
    else
    {
        uint32_t delta = cwm_delta.sys_time_cur - cwm_delta.sys_time_ori;
        if (delta > 120000)
        {
            cwm_delta.delta_caled = true;
            cwm_delta.sensor_delta = (delta * 1000) / (cwm_delta.int_counts * FIFO_DEPTH);
            CWM_OS_dbgPrintf("sensor_delta %d \n", cwm_delta.sensor_delta);
        }
    }
    cwm_delta.int_counts++;
}

static void cwm_mems_delta_init(void)
{
    cwm_delta.sys_time_ori = 0;
    cwm_delta.sys_time_cur = 0;
    cwm_delta.int_counts = 0;
}

static int getReqSensor(int index)
{
    if (m_req_sensor_state & (1 << index))
    {
        return 1;
    }
    return 0;
}

static void app_sensor_mems_report_imu_info(float *data, uint64_t time)
{
    static uint8_t idx = 0;
    uint8_t buf[ONE_LEN_CYWEE_IMU];

    for (uint8_t i = 0; i < 7; i++)
    {
        memcpy(&buf[4 * i], &data[i], 4);
    }
    memcpy(&buf[28], &time, 8);
    memcpy(&sensor_report_buf[idx * ONE_LEN_CYWEE_IMU + 1], &buf[0], sizeof(buf));
    idx++;
    if (idx == CYWEE_REPORT_COUNT)
    {
        idx = 0;
        sensor_report_buf[0] = 0;
        app_report_event(CMD_PATH_SPP, EVENT_REPORT_SPACE_SENSOR_DATA, app_sensor_mems_report_idx(),
                         sensor_report_buf, REPORT_LENTH_CYWEE_IMU);
    }
}

static void app_sensor_mems_report_adv_info(float *data)
{
    static uint8_t idx = 0;
    uint8_t buf[ONE_LEN_CYWEE_ADV];

    for (uint8_t i = 0; i < 7; i++)
    {
        memcpy(&buf[4 * i], &data[i], 4);
    }
    memcpy(&sensor_report_buf_adv[idx * ONE_LEN_CYWEE_ADV + 1], &buf[0], sizeof(buf));
    idx++;
    if (idx == CYWEE_REPORT_COUNT)
    {
        idx = 0;
        sensor_report_buf_adv[0] = 1;
        app_report_event(CMD_PATH_SPP, EVENT_REPORT_SPACE_SENSOR_DATA, app_sensor_mems_report_idx(),
                         sensor_report_buf_adv, REPORT_LENTH_CYWEE_ADV);
    }
}

static void app_sensor_mems_atti_new_data(pSensorEVT_t sensorEVT)
{
#if F_APP_DUAL_AUDIO_EFFECT
    float horz, vert;
    vert = sensorEVT->fData[1];
#if SITRON_LIB_b4_1098
#if YAW_360 == 0
    if (sensorEVT->fData[0] <= 90 && sensorEVT->fData[0] >= -90)  // -90 ~ 90
    {
        horz = sensorEVT->fData[0];//- 180.0;
    }
    else if (sensorEVT->fData[0] > 90) //90 ~ 180
    {
        horz = 180 - sensorEVT->fData[0];//- 180.0;
    }
    else if (sensorEVT->fData[0] < -90) // -90 ~ -180
    {
        horz = - 180 - sensorEVT->fData[0];//- 180.0;
    }
    else  //180.1 ~ 269
    {
        CWM_OS_dbgPrintf("CWM SHOULD NOT HAPPENED %.5f", sensorEVT->fData[0]);
        //horz =  180 - sensorEVT->fData[0];//- 180.0;
    }
#else
    if (sensorEVT->fData[0] <= 90)  // 0 ~ 90
    {
        horz = sensorEVT->fData[0];//- 180.0;
    }
    else if (sensorEVT->fData[0] >= 270) // 270 ~ 360  -90
    {
        horz = sensorEVT->fData[0] - 360;//- 180.0;
    }
    else if (sensorEVT->fData[0] <= 180 && sensorEVT->fData[0] > 90) // 90.1 ~ 180
    {
        horz = 180 - sensorEVT->fData[0];//- 180.0;
    }
    else  //180.1 ~ 269
    {
        horz =  180 - sensorEVT->fData[0];//- 180.0;
    }
#endif
#if YAW_360 == 0
    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
#else
    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
#endif
    {
        horz *= -1;
    }

    if (horz < -90)
    {
        horz = -90;
    }
    else if (horz > 90)
    {
        horz = 90;
    }

    if (vert < -90)
    {
        vert = -90;
    }
    else if (vert > 90)
    {
        vert = 90;
    }

    T_DUAL_AUDIO_POS temp;
    temp.horz = horz * 360;
    temp.vert = vert * 360;
    temp.factor = 0xfae0;
#else
    horz = (short)roundf(sensorEVT->fData[0]);
    vert = (short)roundf(sensorEVT->fData[1]);

    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
    {
        horz *= -1;
    }

    T_DUAL_AUDIO_POS temp;
    temp.horz = horz;
    temp.vert = vert;
    temp.factor = 0xfae0;
#endif
#endif

#if F_APP_DUAL_AUDIO_EFFECT
    if (app_db.report == REPORT_RTK)
    {
        app_sensor_mems_spp_report_data(&sensorEVT->fData[0]);
    }
    else if (app_db.report == REPORT_CYWEE)
    {
        app_sensor_mems_report_imu_info(&sensorEVT->fData[0], sensorEVT->timestamp_ns);
    }
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO
    app_dual_audio_relay_pos_data(temp);
#else
    app_dual_audio_update_pos(temp, true);
#endif
#endif
}

static void CWM_AP_SensorListen(pSensorEVT_t sensorEVT)
{
    switch (sensorEVT->sensorType)
    {
    case IDX_REQUEST_SENSOR:
        if (sensorEVT->fData[2] != 0)
        {
            m_req_sensor_state |= (1 << (int)sensorEVT->fData[1]);
        }
        else
        {
            m_req_sensor_state &= ~(1 << (int)sensorEVT->fData[1]);
        }

        CWM_OS_dbgPrintf("req_sensor input_sensor: %s(%d), switch: %d\n",
                         REQ_SENS_NAME[(int)sensorEVT->fData[1]], (int)sensorEVT->fData[1], (int)sensorEVT->fData[2]);
        CWM_OS_dbgPrintf("input_sensor now: acc=%d gyro=%d mag=%d\n",
                         getReqSensor(0), getReqSensor(1), getReqSensor(2));
        break;

    case IDX_ALGO_HS_FUSION:
        {
            uint8_t app_idx = 0;
#if 1
            CWM_OS_dbgPrintf("cwm_atti: yaw:%.5f pitch:%.5f roll:%.5f quater x:%.5f quater y:%.5f quater z:%.5f quater w:%.5f",
                             sensorEVT->fData[0], sensorEVT->fData[1], sensorEVT->fData[2], sensorEVT->fData[3],
                             sensorEVT->fData[4], sensorEVT->fData[5], sensorEVT->fData[6]);
#endif

            if (app_sensor_mems_need_report(&app_idx))
            {
#if F_APP_DURIAN_SUPPORT
                if (app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK)
                {
                    durian_atti_new_data(app_db.br_link[app_idx].bd_addr, &(sensorEVT->fData[3]));
                }
                else
                {
                    app_sensor_mems_atti_new_data(sensorEVT);
                }
#else
                app_sensor_mems_atti_new_data(sensorEVT);
#endif
            }
        }
        break;

#ifdef BIAS_EN
    case IDX_ALGO_SENS_CALIBRATION:
        CWM_OS_dbgPrintf("acc_calibration_status:%f gyro_calibration_status:%f mag_calibration_status:%f\n",
                         sensorEVT->fData[1],
                         sensorEVT->fData[2],
                         sensorEVT->fData[3]);

        if (sensorEVT->fData[2] == 1)
        {
            SettingControl_t scl;
            memset(&scl, 0, sizeof(scl));
            scl.iData[0] = 1;
            scl.iData[1] = 1;
            CWM_SettingControl(SCL_SENS_CALI_CTRL_A, &scl);

            app_cfg_nv.acc_bias[0] = scl.iData[2];
            app_cfg_nv.acc_bias[1] = scl.iData[3];
            app_cfg_nv.acc_bias[2] = scl.iData[4];

            app_cfg_nv.gyro_bias[0] = scl.iData[5];
            app_cfg_nv.gyro_bias[1] = scl.iData[6];
            app_cfg_nv.gyro_bias[2] = scl.iData[7];

            app_cfg_nv.spatial_auto_cal = 1;

            CWM_Sensor_Disable(IDX_ALGO_SENS_CALIBRATION);
            CWM_OS_dbgPrintf("acc bias save %d %d %d gyro bias save %d %d %d\n", app_cfg_nv.acc_bias[0],
                             app_cfg_nv.acc_bias[1], app_cfg_nv.acc_bias[2], app_cfg_nv.gyro_bias[0], app_cfg_nv.gyro_bias[1],
                             app_cfg_nv.gyro_bias[2]);
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
            if (app_link_get_misc_num(APP_LINK_MEMS) == 0)
            {
                hal_gpio_irq_disable(app_cfg_const.mems_int_pinmux);
#if F_APP_DUAL_AUDIO_TWS_SPATIAL_AUDIO
                uint32_t cpu_freq;
                pm_cpu_freq_clear(&app_sensor_mems_freq_handle, &cpu_freq);
#endif
            }
#endif
        }
        break;
#endif
    case IDX_HS_ORIG_ADV:
        {
#if F_APP_DURIAN_SUPPORT
            durian_atti_imu_data(&sensorEVT->fData[3]);
#endif
            bool need_report = true;
            T_APP_BR_LINK *p_link;
#if 1
            CWM_OS_dbgPrintf("cwm_atti: acc x:%.5f acc y:%.5f acc z:%.5f rotation x:%.5f rotation y:%.5f rotation z:%.5f ear side w:%.5f",
                             sensorEVT->fData[3], sensorEVT->fData[4], sensorEVT->fData[5], sensorEVT->fData[6],
                             sensorEVT->fData[7], sensorEVT->fData[8], sensorEVT->fData[9]);
#endif
            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_link_check_b2s_link_by_id(i))
                {
                    p_link = &app_db.br_link[i];

                    if (p_link)
                    {
                        if ((p_link->bt_sniffing_state != APP_BT_SNIFFING_STATE_IDLE) &&
                            (p_link->bt_sniffing_state != APP_BT_SNIFFING_STATE_SNIFFING) &&
                            (p_link->bt_sniffing_state != APP_BT_SNIFFING_STATE_READY))
                        {
                            need_report = false;
                        }
                    }
                }
            }

            if (need_report)
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    if (app_db.report == REPORT_CYWEE)
                    {
                        app_sensor_mems_report_adv_info(&sensorEVT->fData[3]);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

static void app_sensor_mems_atti_set(void)
{
#if (F_APP_CWM_ALGO_SUPPORT == 1)
    SettingControl_t scl = {0};

    scl.iData[0] = 1;
    scl.iData[1] = 2;
    memcpy((uint8_t *)&scl.iData[2], (uint8_t *)&cwm_atti[0], sizeof(scl) - sizeof(scl.iData[0]) * 2);
    CWM_SettingControl(SCL_HS_ORIEN_CTRL_EXC, &scl);
#endif
}

static void cwm_sensor_origin_config(uint8_t app_idx)
{
    SettingControl_t scl;

    memset(&scl, 0, sizeof(scl));
#if F_APP_DURIAN_SUPPORT
    bool is_avp = ((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) != 0) ? true : false;

    if (is_avp)
    {
        scl.iData[0] = 1;
        scl.iData[4] = 3;
        scl.iData[1] = -1;
        scl.iData[12] = 1;
    }
    else
    {
#if TARGET_RTL8753EFE_VS_AVP&DURIAN_FOUR
        scl.iData[0] = 1;
        scl.iData[1] = 7000;
        scl.iData[3] = 20;
        scl.iData[4] = 3;      //yaw_output_mode3
        scl.iData[11] = 1;
        scl.iData[12] = 2;
        if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
        {
            scl.iData[6] = 2;      //ear_side 1:Left
            scl.iData[7] = 50;    //init_pitch
            scl.iData[8] = 6;      //init_roll
        }
        else
        {
            scl.iData[6] = 1;      //ear_side 1:right
            scl.iData[7] = 49;    //init_pitch
            scl.iData[8] = 9;      //init_roll
        }
#endif

#if (TARGET_RTL8763ESE_AVP&DURIAN_PRO)|(TARGET_RTL8763ESE_AVP&DURIAN_PRO2)
        scl.iData[0] = 1;
        scl.iData[1] = 7000;
        scl.iData[3] = 20;
        scl.iData[4] = 3;
        scl.iData[12] = 2;
        scl.iData[7] = 40;    //pitch
        scl.iData[8] = 25;    //roll
        if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
        {
            scl.iData[6] = 2;
        }
        else
        {
            scl.iData[6] = 1;
        }
#endif
    }
    CWM_SettingControl(SCL_HS_ORIEN_CONFIG, &scl);

#if (TARGET_RTL8763ESE_AVP&DURIAN_PRO)|(TARGET_RTL8763ESE_AVP&DURIAN_PRO2)
    if (is_avp)
    {
        scl.iData[0] = 1;
        scl.iData[1] = 1;
    }
    else
    {
        scl.iData[0] = 1;
        if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
        {
            scl.iData[1] = 23;
        }
        else
        {
            scl.iData[1] = 13;
        }
    }
    CWM_SettingControl(SCL_INPUT_SENSOR_CONFIG, &scl);
#endif
#else
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = -1;   //disable reset yaw
    scl.iData[4] = 3;    //yaw_output_mode3
    //ODR@25Hz/50Hz
    CWM_SettingControl(SCL_HS_ORIEN_CONFIG, &scl);
#endif
}

static void cwm_sensor_enable(uint8_t app_idx)
{
    cwm_sensor_origin_config(app_idx);
    CWM_Sensor_Enable(IDX_REQUEST_SENSOR);
    CWM_Sensor_Enable(IDX_ALGO_HS_FUSION);
    CWM_Sensor_Enable(IDX_HS_ORIG_ADV);
#ifdef BIAS_EN
    if (app_cfg_nv.spatial_auto_cal == 0)
    {
        CWM_Sensor_Enable(IDX_ALGO_SENS_CALIBRATION);
    }
#endif

    if (cwm_atti_need_set)
    {
        cwm_atti_need_set = false;
        app_sensor_mems_atti_set();
    }
}

static void cwm_sensor_disable(void)
{
    CWM_Sensor_Disable(IDX_REQUEST_SENSOR);
    CWM_Sensor_Disable(IDX_ALGO_HS_FUSION);
    CWM_Sensor_Disable(IDX_HS_ORIG_ADV);
#ifdef BIAS_EN
    if (app_cfg_nv.spatial_auto_cal == 1)
    {
        CWM_Sensor_Disable(IDX_ALGO_SENS_CALIBRATION);
    }
#endif
}

static CustomSensorData cwm_csd;
static void algolib_in(float *Acc, float *Gyr, int dt_us)
{
    int ret;

    memset(&cwm_csd, 0, sizeof(CustomSensorData));
    cwm_csd.sensorType = CUSTOM_ACC;
    memcpy(cwm_csd.fData, Acc, sizeof(float) * 3);
    //cwm_csd.fData[2] = 9.80665;
    CWM_CustomSensorInput(&cwm_csd);

    memset(&cwm_csd, 0, sizeof(CustomSensorData));
    cwm_csd.sensorType = CUSTOM_GYRO;
    memcpy(cwm_csd.fData, Gyr, sizeof(float) * 3);
    //cwm_csd.fData[2] = 0.7854;
    CWM_CustomSensorInput(&cwm_csd);

    ret = CWM_process2(dt_us);
    if (ret < 0)
    {
        CWM_OS_dbgPrintf("FATAL ERROR! ret=%d\n", ret);
    }
}

static void algolib_init(void)
{
    SettingControl_t scl;

    //get lib version information
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    CWM_SettingControl(SCL_GET_LIB_INFO, &scl);
    CWM_OS_dbgPrintf("version:%d.%d.%d.%d product:%d\n", scl.iData[1], scl.iData[2], scl.iData[3],
                     scl.iData[4], scl.iData[5]);


    os_api api = {0};
    api.dbgOutput    = CWM_OS_dbgOutput;

    CWM_LibPreInit(&api);

    //set mcu_chip information, must call this before CWM_LibPostInit()
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = APP_CWM_LIB_SERIAL;
    CWM_SettingControl(SCL_CHIP_VENDOR_CONFIG, &scl);

    CWM_LibPostInit(CWM_AP_SensorListen);

    //get chip information
    char chipInfo[64];

    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = 1;
    scl.iData[2] = (int)chipInfo;
    scl.iData[3] = sizeof(chipInfo);
    scl.iData[4] = 0;
    scl.iData[5] = 0;
    scl.iData[6] = 0;
    CWM_SettingControl(SCL_GET_CHIP_INFO, &scl);
    CWM_OS_dbgPrintf("have_security = %d.%d ret_buff_size = %d  chipInfo = %s\n", scl.iData[5],
                     scl.iData[6], scl.iData[4], chipInfo);
    CWM_OS_dbgPrintf("chip_settings = %d, %d, %d\n", scl.iData[9], scl.iData[10], scl.iData[11]);

    // log control
    // memset(&scl, 0, sizeof(scl));
    // scl.iData[0] = 1;
    // scl.iData[3] = 7 + 8;
    // scl.iData[4] = 64 + 5;
    // scl.iData[5] = 3;
    // scl.iData[6] = -1 - 1 - 2 - 4 - 8 - 16 - 32;
    // // scl.iData[8] = 70000;
    // CWM_SettingControl(SCL_LOG, &scl);

    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[3] = 7 + 8;
    scl.iData[4] = 64 + 5 + 10;
    scl.iData[5] = 3;
    scl.iData[6] = -1 - 1 - 2 - 4 - 8 - 16 - 32;
    // scl.iData[8] = 70000;
    CWM_SettingControl(SCL_LOG, &scl);

    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = -1;   //disable reset yaw
    scl.iData[4] = 3;    //yaw_output_mode3
    scl.iData[12] = 1;
    CWM_SettingControl(SCL_HS_ORIEN_CONFIG, &scl);

    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
#if SENSOR_25HZ
    scl.iData[1] = 25;
#else
    scl.iData[1] = 50;
#endif
    scl.iData[2] = 0;
    scl.iData[3] = 0;
    scl.iData[4] = 0;     //yaw_output_mode3

    CWM_SettingControl(SCL_ALGO_PROC_CONFIG, &scl);

#ifdef BIAS_EN
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = 2;
    scl.iData[2] = app_cfg_nv.acc_bias[0];
    scl.iData[3] = app_cfg_nv.acc_bias[1];
    scl.iData[4] = app_cfg_nv.acc_bias[2];
    scl.iData[5] = app_cfg_nv.gyro_bias[0];
    scl.iData[6] = app_cfg_nv.gyro_bias[1];
    scl.iData[7] = app_cfg_nv.gyro_bias[2];
    CWM_SettingControl(SCL_SENS_CALI_CTRL_A, &scl);

    CWM_OS_dbgPrintf("acc bias restore %d %d %d gyro bias restore %d %d %d\n", app_cfg_nv.acc_bias[0],
                     app_cfg_nv.acc_bias[1], app_cfg_nv.acc_bias[2], app_cfg_nv.gyro_bias[0], app_cfg_nv.gyro_bias[1],
                     app_cfg_nv.gyro_bias[2]);
#endif
#if 0
    // headset orient config
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[2] = 4;
    CWM_SettingControl(SCL_HS_ORIEN_CONFIG, &scl);
#endif
    cwm_mems_delta_init();
    cwm_delta.delta_caled = false;//only calibration one time
}
#endif

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
inv_imu_sensor_data gyro_acc_sensor_data[FIFO_DEPTH] = {0};
inv_imv_report_data report_gyro_acc_sensor_data = {0};

#if !IS_HIGH_RES_MODE
static int inv42607_gyro_fsr_dps_to_bitfield(int32_t fsr)
{
    switch (fsr)
    {
#if !defined(ICM43686)
    case 250:  return GYRO_CONFIG0_FS_SEL_250dps;
#endif
    case 500:  return GYRO_CONFIG0_FS_SEL_500dps;
    case 1000:  return GYRO_CONFIG0_FS_SEL_1000dps;
    case 2000:  return GYRO_CONFIG0_FS_SEL_2000dps;
#if defined(ICM43686)
    case 4000: return GYRO_CONFIG0_FS_SEL_4000dps;
#endif
    default:             return -1;
    }
}

void inv42607_get_imu_data(void)
{
    /*
     * Extract packets from FIFO. Callback defined at init time (i.e.
     * imu_cb) will be called for each valid packet extracted from
     * FIFO.
     */
    int packets = inv_imu_get_data_from_fifo_wo_int_check(&icm_driver);
    IO_PRINT_INFO1("inv42607_get_imu_data: packets %d", packets);
}

static int inv42607_accel_fsr_g_to_bitfield(int32_t fsr)
{
    switch (fsr)
    {
    case 2:  return ACCEL_CONFIG0_FS_SEL_2g;
    case 4:  return ACCEL_CONFIG0_FS_SEL_4g;
    case 8:  return ACCEL_CONFIG0_FS_SEL_8g;
    case 16: return ACCEL_CONFIG0_FS_SEL_16g;
#if defined(ICM43686)
    case 32: return ACCEL_CONFIG0_FS_SEL_32g;
#endif
    default:   return -1;
    }
}
#endif

static void app_sensor_mems_inv42607_cb(inv_imu_sensor_event_t *event)
{
    IO_PRINT_INFO0("app_sensor_mems_inv42607_cb");
}

static T_MEM_INIT_STATUS app_sensor_mems_inv42607_setup(struct inv_imu_serif *icm_serif)
{
    uint8_t id = 0;
    T_MEM_INIT_STATUS status = MEM_INIT_SUCC;
    if (inv_imu_init(&icm_driver, icm_serif, app_sensor_mems_inv42607_cb) == INV_ERROR_SUCCESS)
    {
        inv_imu_get_who_am_i(&icm_driver, &id);
    }
    IO_PRINT_INFO2("setup_imu_device: id 0x%x  expect 0x%x", id, ICM_WHOAMI);
    if (id != ICM_WHOAMI)
    {
        status = MEM_INIT_FAIL;
    }
    return status;
}

static int app_sensor_mems_inv42607_read(struct inv_imu_serif *serif, uint8_t reg, uint8_t *rbuffer,
                                         uint32_t rlen)
{
    switch (serif->serif_type)
    {
    case UI_I2C:
        {
            app_sensor_i2c_read(MEMS_I2C_INV42607_SLAVE_ADDR, reg, rbuffer, rlen);
        }
        return 0;
    default:
        return -1;
    }
}

static int app_sensor_mems_inv42607_write(struct inv_imu_serif *serif, uint8_t reg,
                                          const uint8_t *wbuffer,
                                          uint32_t wlen)
{
    switch (serif->serif_type)
    {
    case UI_I2C:
        {
            for (uint32_t index = 0; index < wlen; index++)
            {
                app_sensor_i2c_write_8(MEMS_I2C_INV42607_SLAVE_ADDR, (reg + index),
                                       wbuffer[index]);
            }
        }
        return 0;
    default:
        return -1;
    }
}

static void app_sensor_mems_inv42607_enable(void)
{
    int status = 0;

#if MEM_DBUG_EN
    IO_PRINT_INFO0("app_sensor_mems_inv42607_enable_int");
#endif

#if IS_LOW_NOISE_MODE
    status |= inv_imu_enable_accel_low_noise_mode(&icm_driver);
#else
    status |= inv_imu_enable_accel_low_power_mode(&icm_driver);
#endif

    status |= inv_imu_enable_gyro_low_noise_mode(&icm_driver);
    hal_gpio_irq_enable(app_cfg_const.mems_int_pinmux);
    APP_PRINT_INFO1("app_sensor_mems_inv42607_enable: status 0x%x", status);
}

static void app_sensor_mems_inv42607_disable(void)
{
    int status = 0;
    hal_gpio_irq_disable(app_cfg_const.mems_int_pinmux);
    status |= inv_imu_disable_accel(&icm_driver);
    status |= inv_imu_disable_gyro(&icm_driver);
    APP_PRINT_INFO1("app_sensor_mems_inv42607_disable: status 0x%x", status);
}

static T_MEM_INIT_STATUS app_sensor_mems_inv42607_init_device(void)
{
    T_MEM_INIT_STATUS status;
#if MEM_DBUG_EN
    IO_PRINT_INFO0("app_sensor_mems_inv42607_init_device");
#endif
    icm_serif.context   = 0;        /* no need */
    icm_serif.read_reg  = app_sensor_mems_inv42607_read;
    icm_serif.write_reg = app_sensor_mems_inv42607_write;
    icm_serif.max_read  = 1024 * 32; /* maximum number of bytes allowed per serial read */
    icm_serif.max_write = 1024 * 32; /* maximum number of bytes allowed per serial write */
    icm_serif.serif_type = UI_I2C;
    status =  app_sensor_mems_inv42607_setup(&icm_serif);
    return status;
}

static void app_sensor_mems_inv42607_cfg_int(void)
{
    app_sensor_int_gpio_init(app_cfg_const.mems_int_pinmux,
                             (P_GPIO_CBACK)app_sensor_mems_int_gpio_intr_handler);
}

static void app_sensor_mems_inv42607_cfg(void)
{
    int status = 0;

#if IS_HIGH_RES_MODE
    status |= inv_imu_enable_high_resolution_fifo(&icm_driver);
#else
    status |= inv_imu_set_accel_fsr(&icm_driver,
                                    (ACCEL_CONFIG0_FS_SEL_t)inv42607_accel_fsr_g_to_bitfield(acc_fsr));
    status |= inv_imu_set_gyro_fsr(&icm_driver,
                                   (GYRO_CONFIG0_FS_SEL_t) inv42607_gyro_fsr_dps_to_bitfield(gyr_fsr));
#endif

    status |= inv_imu_set_accel_frequency(&icm_driver, ACCEL_FREQ);
    status |= inv_imu_set_gyro_frequency(&icm_driver,  GYRO_FREQ);

    IO_PRINT_INFO1("app_sensor_mems_inv42607_cfg: status 0x%x", status);
}

void app_sensor_mems_inv42607_read_int_status(void)
{
    uint8_t int_status = 0x0;

    inv_imu_read_reg(&icm_driver, INT_STATUS_DRDY, 1, &int_status);
    inv_imu_read_reg(&icm_driver, INT_STATUS, 1, &int_status);
    inv_imu_read_reg(&icm_driver, INT_STATUS2, 1, &int_status);
    inv_imu_read_reg(&icm_driver, INT_STATUS3, 1, &int_status);
    inv_imu_read_reg(&icm_driver, INT_CONFIG0_MREG_TOP1, 1, &int_status);
    IO_PRINT_INFO1("app_sensor_mems_inv42607_read_int_status: int_status 0x%x", int_status);
}

#if F_APP_DURIAN_SUPPORT
static void dual_audio_unify_data_map(int16_t accs[3], int16_t gyros[3])
{
    float   temp = 0;
    if (app_cfg_const.bud_side == DEVICE_ROLE_LEFT)
    {
        /*
        left:
        X= Raw_Y
        Y=-Raw_X
        Z= Raw_Z
        */
        temp = accs[1];

        accs[1] = 0 - accs[0];
        accs[2] = accs[2];
        accs[0] = temp;

        temp = gyros[1];

        gyros[1] = 0 - gyros[0];
        gyros[2] = gyros[2];
        gyros[0] = temp;
    }

    if (app_cfg_const.bud_side == DEVICE_ROLE_RIGHT)
    {
        /*
        right:
        X=-Raw_X
        Y=-Raw_Y
        Z= Raw_Z
        */
        temp = 0 - accs[1];

        accs[0] = 0 - accs[0];
        accs[2] = accs[2];
        accs[1] = temp;

        temp = 0 - gyros[1];

        gyros[0] = 0 - gyros[0];
        gyros[2] = gyros[2];
        gyros[1] = temp;
    }
}
#endif

#if F_APP_DURIAN_SUPPORT
static void dual_audio_non_unify_data_map(uint8_t app_idx, int16_t accs[3], int16_t gyros[3])
{
    if ((app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK) != 0)
    {
        durian_atti_data_map(accs, gyros);
    }
    else
    {
#if F_APP_DUAL_AUDIO_EFFECT
        app_dual_audio_data_map(accs, gyros);
#endif
    }
}
#endif

static void app_sensor_mems_inv42607_handle_fifo(void)
{
#if (F_APP_CWM_ALGO_SUPPORT == 1)
    int16_t *accel = report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].accel_data;
    int16_t *gyro = report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].gyro_data;

    if (fifo_index >= report_gyro_acc_sensor_data.packet_count)
    {
        return;
    }

#if F_APP_DURIAN_SUPPORT
    bool unify_data_map = false;
    uint8_t a2dp_active_idx = app_a2dp_get_active_idx();

#if TARGET_RTL8753EFE_VS_AVP&DURIAN_FOUR
    unify_data_map = true;
#endif

    if (unify_data_map)
    {
        dual_audio_unify_data_map(accel, gyro);
    }
    else
    {
        dual_audio_non_unify_data_map(a2dp_active_idx, accel, gyro);
    }
#else
    /*handle for pure spatial audio*/
#if F_APP_DUAL_AUDIO_EFFECT
    app_dual_audio_data_map(accel, gyro);
#endif
#endif

    cwm_accRaw[0] = (float)report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].accel_data[0] *
                    ACC_RANGE_CONVERT;
    cwm_accRaw[1] = (float)report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].accel_data[1] *
                    ACC_RANGE_CONVERT;
    cwm_accRaw[2] = (float)report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].accel_data[2] *
                    ACC_RANGE_CONVERT;

    cwm_gyrRaw[0] = (float)report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].gyro_data[0] *
                    GYR_RANGE_CONVERT;
    cwm_gyrRaw[1] = (float)report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].gyro_data[1] *
                    GYR_RANGE_CONVERT;
    cwm_gyrRaw[2] = (float)report_gyro_acc_sensor_data.gyro_acc_sensor_data[fifo_index].gyro_data[2] *
                    GYR_RANGE_CONVERT;

    fifo_index++;

#ifdef CWM_LOG_EN
    CWM_OS_dbgPrintf("A3(%.3f,%.3f,%.3f)G3(%.3f,%.3f,%.3f)(index %d)\n",
                     cwm_accRaw[0], cwm_accRaw[1], cwm_accRaw[2],
                     cwm_gyrRaw[0], cwm_gyrRaw[1], cwm_gyrRaw[2], fifo_index);
#endif

    if (drop_data == 0)
    {
        algolib_in(cwm_accRaw, cwm_gyrRaw, cwm_delta.sensor_delta);
    }
    else
    {
        drop_data--;
    }
#endif
}

static void app_sensor_mems_inv42607_handle_data(void)
{
    fifo_index = 0;

    if (app_link_get_misc_num(APP_LINK_MEMS) > 0)
    {
#if (FIFO_DEPTH == 1)
        app_sensor_mems_inv42607_handle_fifo();
#else
        app_start_timer(&timer_idx_inv_report, "inv_report",
                        app_sensor_mems_timer_id, APP_TIMER_INV_REPORT, 0, true,
                        INV_REPORT_MS);
        app_sensor_mems_inv42607_handle_fifo();
#endif
    }
}

static void app_sensor_mems_inv42607_read_fifo(void *user_fun, uint32_t para)
{
#if MEM_DBUG_EN
    IO_PRINT_INFO1("app_sensor_mems_inv42607_read_fifo %d", app_link_get_misc_num(APP_LINK_MEMS));
#endif
    if (app_link_get_misc_num(APP_LINK_MEMS) > 0)
    {
        if (app_cfg_const.mems_vendor == MEMS_VENDOR_INV42607)
        {
            if (inv_imu_is_required_get_fifo_data(&icm_driver) == 0)
            {
                inv42607_get_imu_data();
            }
        }
    }
}
#endif

static void app_sensor_mems_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    IO_PRINT_TRACE2("app_sensor_mems_timeout_cb: timer_id 0x%02x, timer_chann %d", timer_evt,
                    param);

    switch (timer_evt)
    {
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    case APP_TIMER_INV_REPORT:
        {
            if (app_link_get_misc_num(APP_LINK_MEMS) > 0)
            {
                if (fifo_index >= (report_gyro_acc_sensor_data.packet_count - 1))
                {
                    app_stop_timer(&timer_idx_inv_report);
                }
                app_sensor_mems_inv42607_handle_fifo();
            }
            else
            {
                app_stop_timer(&timer_idx_inv_report);
                fifo_index = 0;
            }
        }
        break;
#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
    case APP_TIMER_QST_REPORT:
        {
            if (app_cfg_const.mems_vendor == MEMS_VENDOR_QMI8658)
            {
                if (app_link_get_misc_num(APP_LINK_MEMS) > 0)
                {
                    app_sensor_mems_handle_data(NULL);
                }
                else
                {
                    app_stop_timer(&timer_idx_qst_report);
                }
            }
        }
        break;
#endif
#if F_APP_DUAL_AUDIO_EFFECT
    case APP_TIMER_STOP_SENSOR:
        {
            app_stop_timer(&timer_idx_stop_sensor);
            for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
            {
                if (app_link_check_b2s_link_by_id(app_idx))
                {
                    if (app_db.br_link[app_idx].mems_is_start)
                    {
                        app_sensor_mems_spatial_stop(app_idx);
                    }
                }
            }
        }
        break;
#endif

    default:
        break;
    }
}

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
static qmi_imu_sensor_data gyro_acc_sensor_data;
static qmi_imu_algo_data algo_data;

static uint8_t app_sensor_mems_qmi8658_read(uint8_t reg, uint8_t *rbuffer, uint32_t rlen)
{
    I2C_Status status;
    status = app_sensor_i2c_read(MEMS_I2C_QMI8658_SLAVE_ADDR, reg, rbuffer, rlen);
    return status;
}

static uint8_t app_sensor_mems_qmi8658_write(uint8_t reg, uint8_t value)
{
    I2C_Status status;
    status = app_sensor_i2c_write_8(MEMS_I2C_QMI8658_SLAVE_ADDR, reg, value);
    return status;
}

static void app_sensor_mems_qmi8658_init_device(void)
{
    IO_PRINT_INFO0("app_sensor_mems_qmi8658_init_device");
    Qmi8658_cb_init(app_sensor_mems_qmi8658_read, app_sensor_mems_qmi8658_write);
    Qmi8658_init();
}
#endif

void app_sensor_mems_cfg_pad(void)
{
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    hal_gpio_set_pull_value(app_cfg_const.mems_int_pinmux, GPIO_PULL_UP);
#endif
}

void app_sensor_mems_post_data(void *acc_gyro_data)
{
    if (app_cfg_const.mems_support)
    {
        T_IO_MSG io_msg;
        io_msg.type = IO_MSG_TYPE_GPIO;
        io_msg.subtype = IO_MSG_MEMS_ACC_DATA;
        io_msg.u.buf = acc_gyro_data;
        app_io_msg_send(&io_msg);
    }
}

void app_sensor_mems_handle_data(void *acc_gyro_data)
{
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_INV42607)
    {
        app_stop_timer(&timer_idx_inv_report);
        app_sensor_mems_inv42607_handle_data();
    }
#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_QMI8658)
    {
        float fusion_dt = 0.04;
        Qmi8658_read_xyz(&gyro_acc_sensor_data);
#ifdef CWM_LOG_EN
        CWM_OS_dbgPrintf("A3(%.3f,%.3f,%.3f)G3(%.3f,%.3f,%.3f)STAMP(%d)\n",
                         gyro_acc_sensor_data.acc[0],
                         gyro_acc_sensor_data.acc[1],
                         gyro_acc_sensor_data.acc[2],
                         gyro_acc_sensor_data.gyro[0],
                         gyro_acc_sensor_data.gyro[1],
                         gyro_acc_sensor_data.gyro[2],
                         gyro_acc_sensor_data.timestamp);
#endif

        qst_fusion_update(gyro_acc_sensor_data.acc, gyro_acc_sensor_data.gyro, &fusion_dt,
                          &algo_data.rpy[0], &algo_data.quaternion[0], &algo_data.line_accel[0]);
#ifdef CWM_LOG_EN
        CWM_OS_dbgPrintf("rpy(%.3f,%.3f,%.3f) quaternion((%.3f,%.3f,%.3f,%.3f)) line_accel(%.3f,%.3f,%.3f)\n",
                         algo_data.rpy[0],
                         algo_data.rpy[1],
                         algo_data.rpy[2],
                         algo_data.quaternion[0],
                         algo_data.quaternion[1],
                         algo_data.quaternion[2],
                         algo_data.quaternion[3],
                         algo_data.line_accel[0],
                         algo_data.line_accel[1],
                         algo_data.line_accel[2]
                        );
#endif

        uint8_t app_idx = 0;
        if (app_sensor_mems_need_report(&app_idx))
        {
#if F_APP_DURIAN_SUPPORT
            durian_atti_new_data(app_db.br_link[app_idx].bd_addr, &algo_data.quaternion[0]);
#endif
        }
    }
#endif
}

// void app_sensor_mems_dat_int(void)
// {
// #if (F_APP_SENSOR_INV42607_SUPPORT == 1)
//     if (mems_is_start)
//     {
//         if (app_cfg_const.mems_vendor == MEMS_VENDOR_INV42607)
//         {
//             if (inv_imu_is_required_get_fifo_data(&icm_driver) == 0)
//             {
//                 inv42607_get_imu_data();
//             }
//         }
//     }
// #endif
// }

ISR_TEXT_SECTION void app_sensor_mems_int_gpio_intr_handler(void)
{
#if MEM_DBUG_EN
    IO_PRINT_INFO0("app_sensor_mems_int_gpio_intr_handler");
#endif
#if (F_APP_CWM_ALGO_SUPPORT == 1)
    cwm_mems_delta_handle();
#endif
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    if (!init_suc)
    {
        IO_PRINT_WARN0("app_sensor_mems_int_gpio_intr_handler init not suc");
        return;
    }
    /* C ontrol of entering DLPS */
    app_dlps_disable(APP_DLPS_ENTER_CHECK_GPIO);
    hal_gpio_irq_disable(app_cfg_const.mems_int_pinmux);

    static BaseType_t high_pri_task_woken = pdFALSE;

    xTimerPendFunctionCallFromISR(app_sensor_mems_inv42607_read_fifo, NULL, 0, &high_pri_task_woken);


    os_portyield_from_isr(high_pri_task_woken);

    app_dlps_enable(APP_DLPS_ENTER_CHECK_GPIO);
    hal_gpio_irq_enable(app_cfg_const.mems_int_pinmux);
#endif
}

static void app_sensor_mems_disable(void)
{
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    inv_imu_enter_full_sleep(&icm_driver);
#endif
}

static void app_sensor_mems_cfg(void)
{
#if MEM_DBUG_EN
    IO_PRINT_INFO0("app_sensor_mems_cfg");
#endif
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_INV42607)
    {
        T_MEM_INIT_STATUS status = app_sensor_mems_inv42607_init_device();
        if (status != MEM_INIT_SUCC)
        {
            init_suc = false;
            IO_PRINT_ERROR0("app_sensor_mems_cfg: init failed");
            return ;
        }

        init_suc = true;
        app_sensor_mems_inv42607_cfg();
        app_sensor_mems_inv42607_read_int_status();
        app_sensor_mems_inv42607_cfg_int();
        app_sensor_mems_inv42607_read_int_status();
        app_sensor_mems_inv42607_read_int_status();
    }
#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_QMI8658)
    {
        app_sensor_mems_qmi8658_init_device();
    }
#endif

#if (F_APP_CWM_ALGO_SUPPORT == 1)
    drop_data = FIFO_DEPTH;
    cwm_delta.sensor_delta = SENSOR_DT_US;
#endif

    //app_sensor_mems_start(NULL);//for test
}

void app_sensor_mems_spatial_start(uint8_t app_idx)
{
    IO_PRINT_TRACE4("app_sensor_mems_spatial_start: app_idx %d mems_is_start %d mems_support %d dual_audio_space %d",
                    app_idx, app_db.br_link[app_idx].mems_is_start, app_cfg_const.mems_support,
                    app_cfg_nv.dual_audio_space);

    if (app_cfg_const.mems_support == 0)
    {
        return;
    }

    if (app_db.br_link[app_idx].mems_is_start)
    {
        return;
    }

#if F_APP_DURIAN_SUPPORT
    if (app_db.br_link[app_idx].connected_profile & AVP_PROFILE_MASK)
    {
        app_durian_avp_atti_start(app_idx);
    }
    else
    {
#if F_APP_DUAL_AUDIO_EFFECT
        app_dual_audio_set_get_flag(false);
#endif
    }
#else
#if F_APP_DUAL_AUDIO_EFFECT
    app_dual_audio_set_get_flag(false);
#endif
#endif

    app_db.br_link[app_idx].mems_is_start = true;
    app_sniff_mode_b2s_disable(app_db.br_link[app_idx].bd_addr, SNIFF_DISABLE_MASK_A2DP);

#if (F_APP_CWM_ALGO_SUPPORT == 1)
    drop_data = FIFO_DEPTH;
    cwm_sensor_disable();
    cwm_sensor_enable(app_idx);
#endif

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_INV42607)
    {
#if F_APP_POWER_TEST
#else
        uint32_t cpu_freq;
        pm_cpu_freq_req(&app_sensor_mems_freq_handle, 60, &cpu_freq);
#endif
        app_sensor_mems_inv42607_enable();
    }
#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_QMI8658)
    {
        app_start_timer(&timer_idx_qst_report, "qst_report",
                        app_sensor_mems_timer_id, APP_TIMER_QST_REPORT, 0, true,
                        QST_REPORT_MS);
    }
#endif
}

void app_sensor_mems_spatial_stop(uint8_t app_idx)
{
    IO_PRINT_TRACE2("app_sensor_mems_spatial_stop: app_idx %d mems_is_start %d", app_idx,
                    app_db.br_link[app_idx].mems_is_start);

    if (app_cfg_const.mems_support == 0)
    {
        return;
    }

    if (!app_db.br_link[app_idx].mems_is_start)
    {
        return;
    }

    app_db.br_link[app_idx].mems_is_start = false;

    if (app_link_get_misc_num(APP_LINK_MEMS) > 0)
    {
        return;
    }

#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    if (app_cfg_nv.spatial_auto_cal == 1)
    {
        app_sensor_mems_inv42607_disable();
#if F_APP_POWER_TEST
#else
        uint32_t cpu_freq;
        pm_cpu_freq_clear(&app_sensor_mems_freq_handle, &cpu_freq);
#endif
    }

#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_QMI8658)
    {
        app_stop_timer(&timer_idx_qst_report);
    }
#elif (F_APP_SENSOR_INV42607_SUPPORT == 1)
    if (app_cfg_const.mems_vendor == MEMS_VENDOR_INV42607)
    {
        app_stop_timer(&timer_idx_inv_report);
    }
#endif

    app_sniff_mode_b2s_enable(app_db.br_link[app_idx].bd_addr, SNIFF_DISABLE_MASK_A2DP);

#if (F_APP_CWM_ALGO_SUPPORT == 1)
    cwm_mems_delta_init();
    cwm_sensor_disable();
#endif
}

void app_sensor_mems_stop(uint8_t app_idx)
{
    if ((app_cfg_const.mems_support != 0) && (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        ((app_cfg_nv.dual_audio_space == SPATIAL_AUDIO_OPEN)))
    {
#if F_APP_DURIAN_SUPPORT
        app_durian_audio_spatial_stop(app_idx);
#else
        app_sensor_mems_spatial_stop(app_idx);
#endif
    }
}

void app_sensor_mems_start(uint8_t app_idx)
{
    if ((app_cfg_const.mems_support != 0) && (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        ((app_cfg_nv.dual_audio_space == SPATIAL_AUDIO_OPEN)))
    {
#if F_APP_DURIAN_SUPPORT
        app_durian_audio_spatial_start(app_idx);
#else
        app_sensor_mems_spatial_start(app_idx);
#endif
    }
}

void app_sensor_mems_dlps_enter(void)
{
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    app_dlps_set_pad_wake_up(app_cfg_const.mems_int_pinmux, PAD_WAKEUP_POL_HIGH);
#endif
}

void app_sensor_mems_dlps_exit(void)
{
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    app_dlps_restore_pad(app_cfg_const.mems_int_pinmux);
#endif
}

static void app_sensor_mems_mode_switch(uint8_t mode)
{
    uint8_t spatial_audio_mode = mode;

    switch (spatial_audio_mode)
    {
    case SPATIAL_AUDIO_CLOSE:
        {
            if (app_cfg_nv.dual_audio_space == SPATIAL_AUDIO_OPEN)
            {
                app_mmi_handle_action(MMI_AUDIO_DUAL_EFFECT_SPACE_SWITCH);
            }
        }
        break;

    case SPATIAL_AUDIO_OPEN:
        {
            if (app_cfg_nv.dual_audio_space == SPATIAL_AUDIO_CLOSE)
            {
                app_mmi_handle_action(MMI_AUDIO_DUAL_EFFECT_SPACE_SWITCH);
            }
        }
        break;

    default:
        break;
    }

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_DEVICE, APP_REMOTE_MSG_SPATIAL_AUDIO,
                                        (uint8_t *)&spatial_audio_mode, 1);
    APP_PRINT_INFO2("app_sensor_mems_mode_switch: mode 0x%x dual_audio_space %d", mode,
                    app_cfg_nv.dual_audio_space);
}

void app_sensor_mems_mode_sync(uint8_t mode)
{
    app_cfg_nv.dual_audio_space = mode;
}

void app_sensor_mems_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                uint8_t app_idx,
                                uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    if (app_cfg_const.mems_support == 0)
    {
        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        return;
    }

    switch (cmd_id)
    {
    case CMD_SET_SPATIAL_AUDIO:
        {
            app_sensor_mems_mode_switch(cmd_ptr[2]);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    default:
        break;
    }
}

static void app_sensor_mems_dm_cback(T_SYS_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case SYS_EVENT_POWER_ON:
        {
            if (app_cfg_const.mems_support)
            {
                app_sensor_mems_cfg();
            }
        }
        break;

    case SYS_EVENT_POWER_OFF:
        {
            if (app_cfg_const.mems_support)
            {
                app_sensor_mems_disable();
            }
        }
        break;

    default:
        break;
    }
}

void app_sensor_mems_init(void)
{
    sys_mgr_cback_register(app_sensor_mems_dm_cback);
    app_timer_reg_cb(app_sensor_mems_timeout_cb, &app_sensor_mems_timer_id);
#if (F_APP_SENSOR_INV42607_SUPPORT == 1)
    app_sensor_i2c_init(MEMS_I2C_INV42607_SLAVE_ADDR, MEMS_I2C_CLK_SPD, true);
#endif

#if (F_APP_SENSOR_QMI8658_SUPPORT == 1)
    app_sensor_i2c_init(MEMS_I2C_QMI8658_SLAVE_ADDR, MEMS_I2C_CLK_SPD, true);
#endif

#if (F_APP_CWM_ALGO_SUPPORT == 1)
    if (cwm_init == 0)
    {
        cwm_init = 1;
        algolib_init();
    }
#endif
}

#if F_APP_DUAL_AUDIO_EFFECT
bool app_sensor_mems_dual_audio_spatial_off(void)
{
#if F_APP_SENSOR_MEMS_SUPPORT
    if (os_sys_time_get() - app_db.spatial_off_anchor >= 100)
    {
        for (uint8_t app_idx = 0; app_idx < MAX_BR_LINK_NUM; app_idx++)
        {
            if (app_link_check_b2s_link_by_id(app_idx))
            {
                if (app_db.br_link[app_idx].mems_is_start)
                {
                    app_sensor_mems_spatial_stop(app_idx);
                }
            }
        }
        return true;
    }
    return false;
#else
    return true;
#endif
}
#endif
#endif
