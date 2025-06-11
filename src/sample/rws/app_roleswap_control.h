#ifndef __APP_ROLESWAP_CONTROL_H__
#define __APP_ROLESWAP_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    APP_ROLESWAP_CTRL_EVENT_LOCAL_IN_CASE,                  //0x00
    APP_ROLESWAP_CTRL_EVENT_REMOTE_IN_CASE,                 //0x01
    APP_ROLESWAP_CTRL_EVENT_LOCAL_OUT_CASE,                 //0x02
    APP_ROLESWAP_CTRL_EVENT_REMOTE_OUT_CASE,                //0x03
    APP_ROLESWAP_CTRL_EVENT_LOCAL_IN_EAR,                   //0x04
    APP_ROLESWAP_CTRL_EVENT_REMOTE_IN_EAR,                  //0x05
    APP_ROLESWAP_CTRL_EVENT_LOCAL_OUT_EAR,                  //0x06
    APP_ROLESWAP_CTRL_EVENT_REMOTE_OUT_EAR,                 //0x07
    APP_ROLESWAP_CTRL_EVENT_SCO_CONN_CMPL,                  //0x08
    APP_ROLESWAP_CTRL_EVENT_ROLESWAP_SUCCESS,               //0x09
    APP_ROLESWAP_CTRL_EVENT_BLE_DISC,                       //0x0A
    APP_ROLESWAP_CTRL_EVENT_B2B_CONNECTED,                  //0x0B
    APP_ROLESWAP_CTRL_EVENT_BATTERY,                        //0x0C
    APP_ROLESWAP_CTRL_EVENT_ACL_ROLE,                       //0x0D
    APP_ROLESWAP_CTRL_EVENT_SNIFFING_STATE,                 //0x0E
    APP_ROLESWAP_CTRL_EVENT_RETRY,                          //0x0F
    APP_ROLESWAP_CTRL_EVENT_SINGLE_BUD_TO_POWER_OFF,        //0x10
    APP_ROLESWAP_CTRL_EVENT_CLOSE_CASE_POWER_OFF,           //0x11
    APP_ROLESWAP_CTRL_EVENT_OPEN_CASE,                      //0x12
    APP_ROLESWAP_CTRL_EVENT_IN_CASE_TIMEOUT_TO_POWER_OFF,   //0x13
    APP_ROLESWAP_CTRL_EVENT_REJCT_BY_PEER,                  //0x14
    APP_ROLESWAP_CTRL_EVENT_ALL_PROFILE_CONNECTED,          //0x15
    APP_ROLESWAP_CTRL_EVENT_BUFFER_LEVEL_CHANGED,           //0x16
    APP_ROLESWAP_CTRL_EVENT_ACL_DISC,                       //0x17
    APP_ROLESWAP_CTRL_EVENT_DECODE_EMPTY,                   //0x18
    APP_ROLESWAP_CTRL_EVENT_CALL_STATUS_CHAGNED,            //0x19
    APP_ROLESWAP_CTRL_EVENT_DURIAN_MIC_CHANGED,             //0x1A
    APP_ROLESWAP_CTRL_EVENT_ROLESWAP_TERMINATED,            //0x1B
    APP_ROLESWAP_CTRL_EVENT_ABS_VOL_CHECK_TIMEOUT,          //0x1C
    APP_ROLESWAP_CTRL_EVENT_MMI_TRIGGER_ROLESWAP,           //0x1D
    APP_ROLESWAP_CTRL_EVENT_ROLESWAP_FAILED_RETRY,          //0x1E
    APP_ROLESWAP_CTRL_EVENT_LEA_2ND_ROLESWAP,               //0x1F
    APP_ROLESWAP_CTRL_EVENT_LEA_CONNECT                     //0x20
} T_APP_ROLESWAP_CTRL_EVENT;

typedef enum
{
    POWER_OFF_REASON_NON                = 0x00,
    POWER_OFF_REASON_IN_CASE            = 0x01,
    POWER_OFF_REASON_SINGLE_POWER_OFF   = 0x02,
    POWER_OFF_REASON_CLOSE_CASE         = 0x04,
} T_APP_POWER_OFF_REASON_AFTER_ROLESWAP;

typedef enum
{
    APP_ROLESWAP_STATUS_IDLE,
    APP_ROLESWAP_STATUS_BUSY,
} T_APP_ROLESWAP_STATUS;

T_APP_ROLESWAP_STATUS app_roleswap_ctrl_get_status(void);

/* @brief  roleswap control check
*
* @param  event : roleswap check event
* @return true: roleswap is triggered or ongoing
*/
bool app_roleswap_ctrl_check(T_APP_ROLESWAP_CTRL_EVENT event);

/* @brief  roleswap module init
*
* @param  void
* @return none
*/
void app_roleswap_ctrl_init(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
