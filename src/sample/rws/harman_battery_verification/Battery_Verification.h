#ifndef __BATTERY_VERIFY_H__
#define __BATTERY_VERIFY_H__

//Modify/add any necessary headers here
//************************************************************
//#include "appHead.h"
#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "Battery_Verification.h"
#include "hal_gpio.h"
#include "hal_gpio_int.h"
#include "platform_utils.h"
#include "trace.h"
#include "hw_tim.h"
#include "section.h"
#include "rtl876x.h"
//************************************************************

#define STAMP_SIZE 65
#define CONFIG_VOL_PER_RECORD                       0   //config with battery voltage percentage record

typedef struct TimeRecordStruct
{
    unsigned char sendData[4];
    unsigned char recvData[4];
    unsigned char timePointer;
    unsigned char stampPointer;
    unsigned char timestamp[STAMP_SIZE];
    unsigned char timegap[32];
} TimeRdST;
extern TimeRdST timeRdSt;


/**
 * DETECT THE TYPES OF BATTERIES, FOR DIFFERENT CHARGING PROFILE
 * @Enter a parameter with ADC value, must be random 16-bit value
 * @Return UNSIGNED CHAR value below:
 * @= 0     Fake/Unauthorised batteries
 * @= 1     Productive batteries
 * @= 2     Authorised batteries
 */
unsigned char Battery_ID_Verification(unsigned int random_seed);


/**
 * KEYBOARD INTERRUPT CALLBACK FUNCTION, RECORD TIME GAPS
 * @No enter parameters
 * @No returns
 */
void Time_Record_Callback(void);

/**
 * COMPARE BATTERY VOLTAGE PERCENTAGE BETWEEN LAST TIME SAVE AND NEW INSERT
 * USE IT ONCE WHEN INITIALIZATIOON
 * @Enter battery voltage percentage parameter
 * @= 0     Fake/Unauthorised batteries
 * @= 1     Productive batteries
 */

unsigned char Com_signal_generate(unsigned int random_seed);
unsigned char verifyCode(unsigned char *in, unsigned char *recv);
void WriteByte(unsigned char data);
void Inital_signal_generate(void);
void TB0W(void);
void TB1W(void);
unsigned int create_16bit_random(unsigned int random_seed_16bit);
void GetTimeGap(void);
void timer_delay(unsigned char us);
//void Bytes_generate(unsigned char h8, unsigned char l8);

void TimerEnable(void);
void TimerDisable(void);
void TIMER_Config(void);
void delay_1us(unsigned int us);
void delay_1ms(unsigned char ms);


void BAT_ID_INPUT_INT_Init(void);
void BAT_ID_INT_Disable(void);
void BAT_ID_INT_Enable(void);
void BAT_ID_INPUT(void);
void BAT_ID_OUTPUT(void);
void BAT_ID_OUTPUT_LOW(void);
void BAT_ID_OUTPUT_HIGH(void);
void BAT_ID_PULL_UP(void);
void BAT_ID_PULL_DOWN(void);

uint32_t Get_Timer_Value(void);
unsigned char Get_BAT_ID_Value(void);

#endif
