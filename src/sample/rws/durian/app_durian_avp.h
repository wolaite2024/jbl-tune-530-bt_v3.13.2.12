/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_DURIAN_AVP_H_
#define _APP_DURIAN_AVP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    AVP_VIOCE_RECOGNITION_IDLE               = 0x00,
    AVP_VIOCE_RECOGNITION_CREATE             = 0x01,
    AVP_VIOCE_RECOGNITION_START              = 0x02,
    AVP_VIOCE_RECOGNITION_ENCODE_START       = 0x03,
    AVP_VIOCE_RECOGNITION_ENCODE_STOP        = 0x04,
    AVP_VIOCE_RECOGNITION_STOP               = 0x05,
} T_AVP_VIOCE_RECOGNITION_STATUS;

typedef enum
{
    AVP_EARFIT_RESULT_UNKOWN,
    AVP_EARFIT_RESULT_GOOD,
    AVP_EARFIT_RESULT_BAD,
} T_AVP_EARFIT_RESULT;

typedef enum
{
    AVP_COMPACTNESS_IDLE,
    AVP_COMPACTNESS_START,
    AVP_COMPACTNESS_STARTING,
    AVP_COMPACTNESS_END,
} T_AVP_COMPACTNESS_STATE;

typedef struct
{
    T_AVP_EARFIT_RESULT compactness_result;
    T_AVP_COMPACTNESS_STATE state;
} T_AVP_COMPACTNESS_DB;

typedef enum
{
    AVP_MULTILINK_STATUS_OFF           = 0X00,
    AVP_MULTILINK_STATUS_ON            = 0X01,
} T_AVP_MULTILINK_STATUS;

typedef enum
{
    AVP_LD_ENABLE       = 0X00,
    AVP_LD_DISABLE      = 0X01,
} T_AVP_LD_STATUS;

#if (F_APP_SENSOR_MEMS_SUPPORT == 1)
void app_durian_avp_atti_start(uint8_t app_idx);
#endif

T_AVP_VIOCE_RECOGNITION_STATUS app_durian_avp_get_opus_status(void);
void app_durian_avp_voice_recognition_check(void);
void app_durian_avp_voice_start(uint8_t app_idx);
void app_durian_avp_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
