/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#if F_APP_SS_SUPPORT

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "os_sched.h"
#include "trace.h"
#include "console.h"
#include "cli.h"
#include "anc.h"

#include "app_msg.h"
#include "app_io_msg.h"
#include "app_ss_cmd.h"
#include "app_cmd.h"
#include "app_dlps.h"
#include "app_cfg.h"
#include "app_auto_power_off.h"

#define WILDCARD        ("\"*\"")

#define CMD_NONE        (0x0000)
#define CMD_ID_LENGTH   (0x04)
#define CMD_LENGTH_MAX  (0x40)

typedef struct
{
    const char *name;
    uint16_t cmd_id;
    const char *param;
    const char *resp_format;
    void (*cb)(const char *param, const char *format);
} T_SS_CLI_CMD;

static const char *ss_prefix = "\r\n\r\nroot@BBPRO:ss# ";
static const T_SS_CLI_CMD ss_cli_cmd_list[];
static const T_CLI_CMD ss_cmd_help;

static char cmd_buf[CMD_LENGTH_MAX];
static uint32_t cmd_buf_len = CMD_LENGTH_MAX;

static uint8_t cur_cmd_path;
static uint8_t cur_app_idx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*version*/

static void cmd_sw_version_check(const char *param, const char *format)
{
    snprintf(cmd_buf, cmd_buf_len, format, 1, "FV00_R180CXTB3");
}

static void cmd_model_name_check(const char *param, const char *format)
{
    snprintf(cmd_buf, cmd_buf_len, format, 1, "SM-R180");
}

//------------------------------------------------------------------------------------------------------------------

/*system*/

static void cmd_factory_reset(const char *param, const char *format)
{
}

static void cmd_reboot(const char *param, const char *format)
{
}

//------------------------------------------------------------------------------------------------------------------

static void cmd_adaptive_anc_on(const char *param, const char *format)
{
    anc_adaptive_filter_start();

    snprintf(cmd_buf, cmd_buf_len, format, 1, "OK");
}

static void cmd_adaptive_anc_off(const char *param, const char *format)
{
    anc_adaptive_filter_stop();

    snprintf(cmd_buf, cmd_buf_len, format, 1, "OK");
}

//------------------------------------------------------------------------------------------------------------------

/*help*/

static void cmd_conflict_item(const char *param, const char *format)
{
    uint32_t i, j;

    if (cur_cmd_path == CMD_PATH_NONE)
    {
        strcpy(cmd_buf, "\r\n");
        console_write((uint8_t *)cmd_buf, strlen(cmd_buf));
    }

    i = 0;
    while (ss_cli_cmd_list[i].cmd_id != CMD_NONE)
    {
        for (j = 0; j < i; j++)
        {
            if (ss_cli_cmd_list[i].cmd_id == ss_cli_cmd_list[j].cmd_id)
            {
                snprintf(cmd_buf, cmd_buf_len, "conflict: %04X [%s] \r\n", ss_cli_cmd_list[i].cmd_id,
                         ss_cli_cmd_list[i].name);

                if (cur_cmd_path == CMD_PATH_NONE)
                {
                    console_write((uint8_t *)cmd_buf, strlen(cmd_buf));

                    os_delay(5);
                }
                else
                {
                    app_report_event(cur_cmd_path, EVENT_SS_RESP, cur_app_idx, (uint8_t *)cmd_buf, strlen(cmd_buf));
                }
            }
        }

        i++;
    }

    if (cur_cmd_path == CMD_PATH_NONE)
    {
        console_write((uint8_t *)ss_prefix, strlen(ss_prefix));
    }

    memset(cmd_buf, 0, cmd_buf_len);
}

static void cmd_help(const char *param, const char *format)
{
    uint32_t i;

    if (cur_cmd_path == CMD_PATH_NONE)
    {
        strcpy(cmd_buf, "\r\n");
        console_write((uint8_t *)cmd_buf, strlen(cmd_buf));
    }

    i = 0;
    while (ss_cli_cmd_list[i].cmd_id != CMD_NONE)
    {
        if (ss_cli_cmd_list[i].param != NULL)
        {
            snprintf(cmd_buf, cmd_buf_len, "%04X%s : %s\r\n", ss_cli_cmd_list[i].cmd_id,
                     ss_cli_cmd_list[i].param,
                     ss_cli_cmd_list[i].name);
        }
        else
        {
            snprintf(cmd_buf, cmd_buf_len, "%04X    : %s\r\n", ss_cli_cmd_list[i].cmd_id,
                     ss_cli_cmd_list[i].name);
        }

        if (cur_cmd_path == CMD_PATH_NONE)
        {
            console_write((uint8_t *)cmd_buf, strlen(cmd_buf));

            os_delay(5);
        }
        else
        {
            app_report_event(cur_cmd_path, EVENT_SS_RESP, cur_app_idx, (uint8_t *)cmd_buf, strlen(cmd_buf));
        }

        i++;
    }

    if (cur_cmd_path == CMD_PATH_NONE)
    {
        console_write((uint8_t *)ss_prefix, strlen(ss_prefix));
    }

    memset(cmd_buf, 0, cmd_buf_len);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void cmd_response_plan_item(const char *param, const char *format)
{
    snprintf(cmd_buf, cmd_buf_len, "yellow: planning to implement CLI");
}

static void cmd_response_later_item(const char *param, const char *format)
{
    snprintf(cmd_buf, cmd_buf_len, "blue: means RTK will implement later");
}

static void cmd_response_not_item(const char *param, const char *format)
{
    snprintf(cmd_buf, cmd_buf_len, "red: means RTK maybe need not implement?");
}

static const T_SS_CLI_CMD ss_cli_cmd_list[] =
{
    /*Version*/
    {"H/W Version Write",                 0x0002,   WILDCARD,  "+VERSNAME:2,OK",                   cmd_response_later_item  },
    {"H/W Version Check",                 0x0003,   NULL,      "+VERSNAME:1,REV0.3",               cmd_response_later_item  },
    {"HW ID Check",                       0x007F,   NULL,      "+HWINDICK:1,0",                    cmd_response_later_item  },
    {"SW Version Check",                  0x0006,   NULL,      "+VERSNAME:%d,%s",                  cmd_sw_version_check     },
    {"Model name Check",                  0x0004,   NULL,      "+VERSNAME:%d,%s",                  cmd_model_name_check     },
    {"Header info Check",                 0x004C,   NULL,      "+HEADINFO:1,EC69A408DF174521",     cmd_response_later_item  },
    {"Set serial number",                 0x000B,   WILDCARD,  "L+SERIALNO:2,OK",                  cmd_response_later_item  },
    {"Get serial number",                 0x000C,   NULL,      "L+SERIALNO:1,R3AN400VN1L",         cmd_response_later_item  },
    {"Set SKU code",                      0x007D,   WILDCARD,  "L+SKUCODEC:2,OK",                  cmd_response_later_item  },
    {"Get SKU code",                      0x007E,   NULL,      "L+SKUCODEC:1,SM-R177NZWAXAR",      cmd_response_later_item  },
    {"Set Device ID",                     0x00AD,   NULL,      "",                                 cmd_response_not_item    },
    {"Get Device ID",                     0x00AE,   NULL,      "",                                 cmd_response_not_item    },
    {"HW Approval Ver",                   0xF000,   NULL,      "",                                 cmd_response_not_item    },
    {"SW Approval Ver",                   0xF001,   NULL,      "",                                 cmd_response_not_item    },
    {"Write date",                        0x0026,   NULL,      "",                                 cmd_response_not_item    },
    {"Read date",                         0x0027,   NULL,      "",                                 cmd_response_not_item    },

    /*System*/
    {"factory reset",                     0x0055,   NULL,      "",                                 cmd_factory_reset        },
    {"Reboot",                            0x9999,   NULL,      "L+SWREBOOT:0,OK",                  cmd_reboot               },
    {"Binary mode check",                 0x005E,   NULL,      "",                                 cmd_response_not_item    },
    {"Finish factory_uart",               0xAAAA,   NULL,      "",                                 cmd_response_not_item    },

    /*power manager*/
    {"Enter sleep mode",                  0x004A,   NULL,      "L+SYSSLEEP:0,OK",                  cmd_response_plan_item   },
    {"Power off check",                   0x005B,   NULL,      "",                                 cmd_response_plan_item   },
    {"Enter ship mode",                   0x005C,   NULL,      "L+SHIPMODE:0,OK",                  cmd_response_later_item  },
    {"Set PowerOFF_Time",                 0x00CB,   WILDCARD,  "L+SHIPMODE:0,OK",                  cmd_response_later_item  },
    {"power manager debug",               0xFFDE,   NULL,      "",                                 cmd_response_not_item    },

    /*OTA*/
    {"reset file/fota state",             0x99F5,   NULL,      "",                                 cmd_response_not_item    },

    /*RWS*/
    {"get coupling status",               0x0103,   NULL,      "",                                 cmd_response_not_item    },
    {"bt id write",                       0x0030,   NULL,      "",                                 cmd_response_not_item    },
    {"bt id read",                        0x0031,   NULL,      "L+BTIDTEST:1,5CCB9906D30E",        cmd_response_plan_item   },
    {"peer bt id write",                  0x0047,   NULL,      "",                                 cmd_response_not_item    },
    {"peer bt id read",                   0x0048,   NULL,      "",                                 cmd_response_not_item    },
    {"write simple(local/peer) bt id",    0x9930,   NULL,      "",                                 cmd_response_not_item    },
    {"get br/le key",                     0x99F3,   NULL,      "",                                 cmd_response_plan_item   },
    {"get local/peer bt addr",            0x99F4,   NULL,      "",                                 cmd_response_not_item    },
    {"print address change history",      0x00FF,   NULL,      "",                                 cmd_response_not_item    },
    {"Check EarPhone Left/Right",         0x0065,   NULL,      "+DIRECCHK:1,LEFT",                 cmd_response_plan_item   },

    /*Cycle*/
    {"Reset SOC Cycle",                   0xBB01,   NULL,      "",                                 cmd_response_not_item    },
    {"Read SOC Cycle",                    0xBB02,   NULL,      "",                                 cmd_response_not_item    },
    {"Read SOC Cycle From Manger",        0xBB03,   NULL,      "",                                 cmd_response_not_item    },

    /*NV*/
    {"read All NV",                       0xE001,   NULL,      "",                                 cmd_response_not_item    },
    {"Set NV",                            0xEB01,   NULL,      "+SETTESTNV:01,E",                  cmd_response_plan_item   },
    {"Check NV",                          0xED01,   NULL,      "+GETTESTNV:01,E",                  cmd_response_plan_item   },
    {"All NVLog read",                    0xE003,   NULL,      "L+GETFULLHISTNV:13E13P08E27P08P",  cmd_response_later_item  },
    {"NV Log full erase",                 0xE004,   NULL,      "L+LOGERASE:",                      cmd_response_later_item  },
    {"NV,SN,HWver erase",                 0xE009,   NULL,      "",                                 cmd_response_not_item    },
    {"NV single Read/Write",              0xEABC,   NULL,      "",                                 cmd_response_not_item    },
    {"Get trace log",                     0x0074,   NULL,      "",                                 cmd_response_not_item    },

    /*GPIO*/
    {"Pin check",                         0xDDDD,   NULL,      "",                                 cmd_response_plan_item   },

    /*Battery*/
    {"Read Thermistor",                   0x004D,   NULL,      "+TEMPTEST:1,25.11",                cmd_response_plan_item   },
    {"Battery Voltage Read",              0x0050,   NULL,      "L+BATTTEST:1,4.13",                cmd_response_plan_item   },
    {"BatteryChargeCurrent",              0x0077,   NULL,      "L+BATTTEST:1,-112",                cmd_response_plan_item   },
    {"In Battery Battery Voltage Read",   0x0085,   NULL,      "",                                 cmd_response_not_item    },
    {"Fuel Gauge IC Current Read",        0x0086,   NULL,      "",                                 cmd_response_not_item    },
    {"In Battery Battery Voltage Read",   0x0185,   NULL,      "",                                 cmd_response_not_item    },
    {"BattID_Write",                      0x00C2,   WILDCARD,  "L+BATTTEST:2,OK",                  cmd_response_later_item  },
    {"BattID_Check",                      0x00C3,   NULL,      "L+BATTTEST:1,SD",                  cmd_response_later_item  },
    {"set high batt at moment",           0x990F,   NULL,      "",                                 cmd_response_not_item    },
    {"pmic fuelgauge debug register",     0xFF85,   NULL,      "",                                 cmd_response_not_item    },

    /*Charging*/
    {"Connection Detect check",           0x00BB,   NULL,      "",                                 cmd_response_not_item    },
    {"Write charging step current",       0xBB04,   NULL,      "",                                 cmd_response_not_item    },
    {"Read charging step current",        0xBB05,   NULL,      "",                                 cmd_response_not_item    },
    {"Read Full Charged Check Time",      0xBB06,   NULL,      "",                                 cmd_response_not_item    },
    {"Write Full Charged Check Time",     0xBB07,   NULL,      "",                                 cmd_response_not_item    },

    /*LED*/
    {"LED ON",                            0x0070,   NULL,      "",                                 cmd_response_not_item    },
    {"LED OFF",                           0x0071,   NULL,      "",                                 cmd_response_not_item    },
    {"LED Control",                       0xFFFE,   NULL,      "",                                 cmd_response_not_item    },

    /*Data Uart*/
    {"Disable user uart",                 0xF010,   NULL,      "",                                 cmd_response_plan_item   },
    {"Enable user uart",                  0xF011,   NULL,      "",                                 cmd_response_plan_item   },
    {"Get user_uart_enable",              0xF012,   NULL,      "",                                 cmd_response_plan_item   },

    /*Location*/
    {"post placement. in_eaer",           0x9910,   NULL,      "",                                 cmd_response_not_item    },
    {"post placement. outside",           0x9911,   NULL,      "",                                 cmd_response_not_item    },
    {"post placement. in_close_case",     0x9912,   NULL,      "",                                 cmd_response_not_item    },
    {"post placement. in_open_case",      0x9913,   NULL,      "",                                 cmd_response_not_item    },

    /*RF TEST*/

    /*BT TEST*/

    /*Codec*/
    {"codec on",                          0x00B8,   NULL,      "",                                 cmd_response_plan_item   },
    {"codec off",                         0x00B9,   NULL,      "",                                 cmd_response_plan_item   },

    /*A2DP*/
    {"rcv 1khz start",                    0x0057,   NULL,      "",                                 cmd_response_plan_item   },
    {"rcv stop",                          0x005A,   NULL,      "",                                 cmd_response_plan_item   },

    /*Sensor log*/
    {"enable vpu logging",                0x9908,   NULL,      "",                                 cmd_response_not_item    },
    {"disable vpu logging",               0x9909,   NULL,      "",                                 cmd_response_not_item    },
    {"enable sensor display",             0x990A,   NULL,      "",                                 cmd_response_not_item    },
    {"disable sensor display",            0x990B,   NULL,      "",                                 cmd_response_not_item    },
    {"enable SensorLogging",              0x990D,   NULL,      "",                                 cmd_response_not_item    },
    {"disable SensorLogging",             0x990E,   NULL,      "",                                 cmd_response_not_item    },

    /*TSP Sensor*/
    {"read tsp ic fw ver",                0x0007,   NULL,      "",                                 cmd_response_not_item    },
    {"read tsp bin fw ver",               0x0008,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp abs",                           0x0009,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp delta",                         0x000A,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp grip abs - pin 1",              0x008B,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp grip delta - pin 1",            0x008C,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp grip abs - pin 2",              0x008D,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp grip delta - pin 2",            0x008E,   NULL,      "",                                 cmd_response_not_item    },
    {"tsp INT test",                      0x00AF,   NULL,      "",                                 cmd_response_not_item    },

    /*Hall Sensor*/
    {"Hall IC Test SMD",                  0x0078,   NULL,      "+HAICTEST:1,OFF",                  cmd_response_not_item    },

    /*Speed Sensor*/
    {"Acc On",                            0x000D,   NULL,      "",                                 cmd_response_not_item    },
    {"Acc XYZ Read",                      0x000E,   NULL,      "",                                 cmd_response_not_item    },
    {"Acc Self test",                     0x0012,   NULL,      "",                                 cmd_response_not_item    },
    {"Acc Off",                           0x0013,   NULL,      "",                                 cmd_response_not_item    },
    {"Acc vector sum",                    0x0033,   NULL,      "",                                 cmd_response_not_item    },
    {"Acc oneshot",                       0x00B3,   NULL,      "+ACSENSOR:0,OK,%s",                cmd_response_later_item  },

    /*Gyro Sensor*/
    {"Gyro On",                           0x001A,   NULL,      "",                                 cmd_response_not_item    },
    {"Gyro Temp Read",                    0x001B,   NULL,      "",                                 cmd_response_not_item    },
    {"Gyro Selftest",                     0x001C,   NULL,      "+GYROSCOP:1,OK,%s",                cmd_response_later_item  },
    {"Gyro Off",                          0x001D,   NULL,      "",                                 cmd_response_not_item    },

    /*AMP*/
    {"set AMPCHECK",                      0x0062,   NULL,      "",                                 cmd_response_not_item    },
    {"get AMPCHECK",                      0x0063,   NULL,      "",                                 cmd_response_not_item    },

    /*Speaker*/
    {"change spk volume",                 0x00B0,   NULL,      "",                                 cmd_response_plan_item   },
    {"spk test start",                    0x00B1,   NULL,      "",                                 cmd_response_plan_item   },
    {"revoke spk volume",                 0x00B2,   NULL,      "",                                 cmd_response_plan_item   },

    /*MIC TEST*/
    {"mic_loopback_off",                  0x005F,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1_gain_read",                    0x002C,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic2_gain_read",                    0x002D,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic3_gain_read",                    0x008F,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1_loopback_test",                0x0083,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic2_loopback_test",                0x0084,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic3_loopback_test",                0x009A,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1_rcv1khz_loopback_test",        0x0087,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic2_rcv1khz_loopback_test",        0x0088,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic3_rcv1khz_loopback_test",        0x009B,   NULL,      "",                                 cmd_response_plan_item   },
    {"get loopback test gain",            0x0089,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1_loopback_test(Close-save)",    0x009C,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1_loopback_test(open-save)",     0x009D,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic2_loopback_test(Close-save)",    0x009E,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic2_loopback_test(open-save)",     0x009F,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1,mic2_loopback_(close-read)",   0x00A4,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1,mic2_loopback_(open-read)",    0x00A5,   NULL,      "",                                 cmd_response_plan_item   },
    {"read the FB Mic Sensitivity",       0xAC02,   NULL,      "",                                 cmd_response_plan_item   },
    {"read the FF Mic Sensitivity",       0xAC03,   NULL,      "",                                 cmd_response_plan_item   },
    {"Change the FB Mic gain",            0xAC04,   NULL,      "",                                 cmd_response_plan_item   },
    {"Change the FF Mic gain",            0xAC05,   NULL,      "",                                 cmd_response_plan_item   },
    {"Write the best FB Mic gain",        0xAC06,   NULL,      "",                                 cmd_response_plan_item   },
    {"Write the best FF Mic gain",        0xAC07,   NULL,      "",                                 cmd_response_plan_item   },
    {"Read the best FB Mic gain",         0xAC08,   NULL,      "",                                 cmd_response_plan_item   },
    {"Read the best FF Mic gain",         0xAC09,   NULL,      "",                                 cmd_response_plan_item   },
    {"FF Mic On",                         0xAC0A,   NULL,      "",                                 cmd_response_plan_item   },
    {"FF Mic Off",                        0xAC0B,   NULL,      "",                                 cmd_response_plan_item   },
    {"FB Mic On",                         0xAC0C,   NULL,      "",                                 cmd_response_plan_item   },
    {"FB Mic Off",                        0xAC0D,   NULL,      "",                                 cmd_response_plan_item   },
    {"Write volume level to NV",          0xAC0E,   NULL,      "",                                 cmd_response_plan_item   },
    {"Read volume level from NV",         0xAC0F,   NULL,      "",                                 cmd_response_plan_item   },
    {"10hz calibration",                  0xAC11,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic_adc_read_start",                0x9920,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic_adc_value_set",                 0x9921,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic_adc_value_read",                0x9922,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic_adc_read_end",                  0x9923,   NULL,      "",                                 cmd_response_plan_item   },
    {"output_dac_set",                    0x9924,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic1_loopback",                     0x992C,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic2_loopback",                     0x992D,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic3_loopback",                     0x992E,   NULL,      "",                                 cmd_response_plan_item   },
    {"mic_off",                           0x992F,   NULL,      "",                                 cmd_response_plan_item   },

    /*TOUCH TEST*/
    {"Proximity 1st Cal Sensor On",       0x001E,   NULL,      "L+PROXIMIT:0,OK",                  cmd_response_later_item  },
    {"Proximity Check",                   0x001F,   NULL,      "",                                 cmd_response_not_item    },
    {"Proximity 1st ADC Check",           0x0024,   NULL,      "L+PROXIMIT:1,2176,2179,2177",      cmd_response_later_item  },
    {"Proximity Off",                     0x0025,   NULL,      "",                                 cmd_response_not_item    },
    {"Proximity set Threshold",           0x1122,   NULL,      "",                                 cmd_response_not_item    },
    {"Proximity get Threshold",           0x1123,   NULL,      "",                                 cmd_response_not_item    },
    {"Proximity 1st Sensor Cal",          0x1024,   NULL,      "L+PROXIMIT:1,2177",                cmd_response_later_item  },
    {"Proximity 1st Cal Sensor Off",      0x0025,   NULL,      "L+PROXIMIT:0,OK",                  cmd_response_later_item  },
    {"Proximity 1st Cal Sensor On",       0x001E,   NULL,      "L+PROXIMIT:0,OK",                  cmd_response_later_item  },
    {"Proximity 1st Working ADC Check",   0x0024,   NULL,      "L+PROXIMIT:1,8722,8727,8724",      cmd_response_later_item  },
    {"Proximity 1st Working Working",     0x001F,   NULL,      "L+PROXIMIT:1,FOUND",               cmd_response_later_item  },
    {"Proximity 1st Release ADC Check",   0x0024,   NULL,      "L+PROXIMIT:1,2177,2179,2178",      cmd_response_later_item  },
    {"Proximity 1st Release",             0x001F,   NULL,      "L+PROXIMIT:1,NOT,FOUND",           cmd_response_later_item  },
    {"Proximity 1st Release Sensor Off",  0x0025,   NULL,      "L+PROXIMIT:0,OK",                  cmd_response_later_item  },

    /*BONE CON*/
    {"VPU Off",                           0x00B4,   NULL,      "",                                 cmd_response_not_item    },
    {"VPU On",                            0x00B5,   NULL,      "",                                 cmd_response_not_item    },
    {"Bone Conduction Sensor Test",       0x00B6,   NULL,      "",                                 cmd_response_later_item  },
    {"VPU vibration",                     0x00B7,   NULL,      "L+BCSENSOR:0,OK",                  cmd_response_not_item    },
    {"BoneConductionSensor OneShot -90",  0x00BA,   NULL,      "L+BCSENSOR:0,OK,8117,-263,117",    cmd_response_later_item  },

    /*ANC*/
    {"READ PN(product No), Right info",   0x00C5,   NULL,      "R+HEADINFO:1,%s",                  cmd_response_later_item  },
    {"SET_VOL_LEVEL_CHECK_CODEC",         0x00B2,   NULL,      "R+LOOPTEST:0,OK",                  cmd_response_later_item  },
    {"FeedBack_Passive_FF_MIC_OF",        0xAC0B,   NULL,      "R+ANCCHECK:0,OK",                  cmd_response_later_item  },
    {"FeedBack_Passive_FB_MIC_OF",        0xAC0D,   NULL,      "R+ANCCHECK:0,OK",                  cmd_response_later_item  },
    {"FeedBack_Passive_ANC_OFF",          0xAC00,   NULL,      "R+ANCCHECK:0,OK",                  cmd_response_later_item  },
    {"FeedBack_ANC_ON",                   0xAC01,   NULL,      "R+ANCCHECK:0,OK",                  cmd_response_plan_item   },
    {"FeedBack_FF_MIC_OFF",               0xAC0B,   NULL,      "R+ANCCHECK:0,OK",                  cmd_response_plan_item   },
    {"FB_SET_GAIN",                       0xAC04,   WILDCARD,  "R+ANCCHECK:2,OK",                  cmd_response_plan_item   },
    {"FB_FIND_BEST_GAIN",                 0xAC06,   WILDCARD,  "R+ANCCHECK:2,OK",                  cmd_response_plan_item   },
    {"Hybrid_FF_SET_GAIN",                0xAC05,   WILDCARD,  "R+ANCCHECK:2,OK",                  cmd_response_plan_item   },
    {"Hybird_FF_STORE_BEST_GAIN",         0xAC07,   WILDCARD,  "R+ANCCHECK:2,OK",                  cmd_response_plan_item   },
    {"SWEEP_ON",                          0xAC10,   NULL,      "",                                 cmd_response_not_item    },
    {"AUDIO_CHECK_ON",                    0xAC12,   NULL,      "",                                 cmd_response_not_item    },
    {"adpt_anc_switch",                   0xF015,   NULL,      "",                                 cmd_response_not_item    },
    {"Adaptive ANC On",                   0xAC21,   NULL,      "+AD_ANC_ON:%d,%s",                 cmd_adaptive_anc_on      },
    {"Adaptive ANC Off",                  0xAC22,   NULL,      "+AD_ANC_OFF:%d,%s",                cmd_adaptive_anc_off     },

    /*Unknown*/
    {"Outgoing_over_temp_sound",          0xF013,   NULL,      "",                                 cmd_response_not_item    },
    {"getting_leakage_level",             0xF014,   NULL,      "",                                 cmd_response_not_item    },

    /*Help*/
    {"Print Conflict Item",               0x1111,   NULL,      "",                                 cmd_conflict_item        },
    {"Print Help menu",                   0xFFFF,   NULL,      "",                                 cmd_help                 },

    {NULL,                                CMD_NONE, NULL,      NULL,                               NULL                     },
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void app_ss_cmd_handle_msg(T_IO_MSG *msg)
{
    uint16_t i = msg->subtype;
    void *param = msg->u.buf;
    uint32_t len;

    memset(cmd_buf, 0, cmd_buf_len);

    ss_cli_cmd_list[i].cb(param, ss_cli_cmd_list[i].resp_format);
    len = strlen(cmd_buf);
    if (len != 0)
    {
        if (cur_cmd_path == CMD_PATH_NONE)
        {
            console_write((uint8_t *)cmd_buf, len);
            console_write((uint8_t *)ss_prefix, strlen(ss_prefix));
        }
        else
        {
            app_report_event(cur_cmd_path, EVENT_SS_RESP, cur_app_idx, (uint8_t *)cmd_buf, len);
        }
    }

    if (param != NULL)
    {
        free(param);
    }
}

static void app_ss_cmd_ss_send_msg(uint16_t cmd_id, void *param)
{
    T_IO_MSG msg;

    msg.type    = IO_MSG_TYPE_SS;
    msg.subtype = cmd_id;
    msg.u.buf   = param;

    app_io_msg_send(&msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool app_ss_cmd_ss_common(const char *cmd_str, char *buf, size_t buf_len)
{
    uint16_t cmd_len;
    char cmd_id_tmp[CMD_ID_LENGTH + 1];
    char param_tmp[CMD_LENGTH_MAX];
    uint16_t cmd_id;
    char *param_mark;
    char *param_left;
    char *param_send;
    uint16_t param_left_len;
    bool is_found;
    bool is_exec;
    bool have_param;
    uint32_t i;

    is_exec = false;

    memset(buf, 0x00, buf_len);

    cmd_len = strlen(cmd_str);

    if (cmd_len > CMD_LENGTH_MAX)
    {
        snprintf(buf, buf_len, "err: cmd length is too long, > %d!", CMD_LENGTH_MAX);
        goto exit;
    }

    if (cmd_len > CMD_ID_LENGTH)
    {
        for (i = CMD_ID_LENGTH; i < cmd_len; i++)
        {
            if (cmd_str[i] == ('\"'))
            {
                break;
            }
            else if (cmd_str[i] != ' ')
            {
                snprintf(buf, buf_len, "err: after cmd id %u byte meet unknow char '%c'!", i - (CMD_ID_LENGTH - 1),
                         cmd_str[i]);
                goto exit;
            }
        }
    }

    memset(cmd_id_tmp, 0, sizeof(cmd_id_tmp));
    strncpy(cmd_id_tmp, cmd_str, 4);
    cmd_id = strtol(cmd_id_tmp, NULL, 16);

    have_param = false;
    param_mark = strchr(cmd_str, ('\"'));
    if (param_mark != NULL)
    {
        strncpy(param_tmp, param_mark + 1, CMD_LENGTH_MAX - 1);
        param_mark = strchr(param_tmp, ('\"'));
        if (param_mark == NULL)
        {
            snprintf(buf, buf_len, "err: param lack right mark!");
            goto exit;
        }
        else
        {
            param_left = param_mark + 1;
            param_left_len = strlen(param_left);

            for (i = 0; i < param_left_len; i++)
            {
                if (param_left[i] != ' ')
                {
                    snprintf(buf, buf_len, "err: after param %u byte meet unknow char '%c'!", i + 1, param_left[i]);
                    goto exit;
                }
            }

            *param_mark = 0;

            if (strlen(param_tmp) == 0)
            {
                snprintf(buf, buf_len, "err: param len is 0!");
                goto exit;
            }
            else
            {
                have_param = true;
            }
        }
    }

    is_found = false;

    i = 0;
    while (ss_cli_cmd_list[i].cmd_id != CMD_NONE)
    {
        if (ss_cli_cmd_list[i].cmd_id == cmd_id)
        {
            is_found = true;
            if (ss_cli_cmd_list[i].cb != NULL)
            {
                if (ss_cli_cmd_list[i].resp_format != NULL)
                {
                    if (ss_cli_cmd_list[i].param != NULL)
                    {
                        if (have_param)
                        {
                            param_send = malloc(strlen(param_tmp) + 1);
                            if (param_send != NULL)
                            {
                                strcpy(param_send, param_tmp);
                                app_ss_cmd_ss_send_msg(i, param_send);
                                is_exec = true;
                            }
                        }
                        else
                        {
                            snprintf(buf, buf_len, "err: please input param!");
                        }
                    }
                    else
                    {
                        if (have_param)
                        {
                            snprintf(buf, buf_len, "err: cmd not need param!");
                        }
                        else
                        {
                            app_ss_cmd_ss_send_msg(i, NULL);
                            is_exec = true;
                        }
                    }
                }
                else
                {
                    snprintf(buf, buf_len, "err: cmd no resp format!");
                }
            }
            else
            {
                snprintf(buf, buf_len, "err: cmd no callback!");
            }
            break;
        }
        i++;
    }

    if (!is_found)
    {
        snprintf(buf, buf_len, "err: cmd not found!");
    }

exit:
    strcat(buf, "\r\n");

    return is_exec;
}

static bool app_ss_cmd_ss_all(const char *cmd_str, char *buf, size_t buf_len)
{
    cur_cmd_path = CMD_PATH_NONE;

    app_ss_cmd_ss_common(cmd_str, buf, buf_len);

    return false;
}

static bool app_ss_cmd_ss_exit(const char *cmd_str, char *buf, size_t buf_len)
{
    snprintf(buf, buf_len, "Exit from ss mode...\r\n");

    app_dlps_enable(APP_DLPS_ENTER_CHECK_CMD);

    app_auto_power_off_enable(AUTO_POWER_OFF_MASK_CONSOLE_CMD, app_cfg_const.timer_auto_power_off);

    return false;
}

static bool app_ss_cmd_ss_help(const char *cmd_str, char *buf, size_t buf_len)
{
    return cli_help_set(&ss_cmd_help, cmd_str, buf, buf_len);
}

static bool app_ss_cmd_ss_mode(const char *cmd_str, char *buf, size_t buf_len)
{
    snprintf(buf, buf_len, "Enter into ss mode...\r\n");

    app_dlps_disable(APP_DLPS_ENTER_CHECK_CMD);

    app_auto_power_off_disable(AUTO_POWER_OFF_MASK_CONSOLE_CMD);

    return false;
}

static bool app_ss_cmd_match(const char *cmd_str, const char *p_cmd, size_t cmd_len)
{
    bool ret = false;

    if (cmd_len >= CMD_ID_LENGTH)
    {
        if (isxdigit(cmd_str[0]) && isxdigit(cmd_str[1]) && isxdigit(cmd_str[2]) && isxdigit(cmd_str[3]))
        {
            ret = true;
        }
    }
    return ret;
}

static const T_CLI_CMD ss_cmd_all =
{
    NULL,              /* Next command pointed to "reset". */
    NULL,              /* Next subcommand. */
    "****",            /* The command string to type. */
    "\r\nHHHH:\r\n 4 HEX ss cmd(\"FFFF\" is cmd help)\r\n",
    app_ss_cmd_ss_all, /* The function to run. */
    -1,                /* Zero or more parameter is expected. */
    app_ss_cmd_match   /* 4 HEX command match pattern. */
};

static const T_CLI_CMD ss_cmd_exit =
{
    &ss_cmd_all,         /* Next command pointed to "reset". */
    NULL,                /* Next subcommand. */
    "exit",              /* The command string to type. */
    "\r\nexit:\r\n Exits from ss mode\r\n",
    app_ss_cmd_ss_exit,  /* The function to run. */
    0,                   /* Zero or One parameter is expected. */
    NULL                 /* Default command match pattern. */
};

static const T_CLI_CMD ss_cmd_help =
{
    &ss_cmd_exit,       /* Next command pointed to "exit" command. */
    NULL,               /* Next subcommand. */
    "help",             /* The command string to type. */
    "\r\nhelp:\r\n Lists all the registered commands\r\n",
    app_ss_cmd_ss_help, /* The function to run. */
    0,                  /* Zero or One parameter is expected. */
    NULL                /* Default command match pattern. */
};

static T_CLI_CMD ss_cmd_entry  =
{
    NULL,               /* Next command will be determined when registered into file system. */
    &ss_cmd_help,       /* Next subcommand pointer to "help" subcommand. */
    "ss",               /* The command string to type. */
    "\r\nss:\r\n Enters ss cmd mode\r\n",
    app_ss_cmd_ss_mode, /* The function to run. */
    0,                  /* Zero or One parameter is expected. */
    NULL                /* Default command match pattern. */
};

bool app_ss_cmd_register(void)
{
    return cli_cmd_register(&ss_cmd_entry);
}

void app_ss_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt)
{
    uint16_t cmd_set_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    bool is_exec;

    switch (cmd_set_id)
    {
    case CMD_SS_REQ:
        {
            cmd_ptr[cmd_len] = 0;

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            cur_cmd_path = cmd_path;
            cur_app_idx = app_idx;

            is_exec = app_ss_cmd_ss_common((const char *)&cmd_ptr[2], cmd_buf, cmd_buf_len);
            if (!is_exec)
            {
                app_report_event(cmd_path, EVENT_SS_RESP, app_idx, (uint8_t *)cmd_buf, strlen(cmd_buf));
            }
        }
        break;

    default:
        {
            ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;
    }
}

#endif
