/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     application.c
* @brief
* @details
* @author   hunter_shuai
* @date     23-June-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#include "rtl876x.h"
#include "application.h"
#include "dlps_platform.h"
#include "SimpleBLEPeripheral.h"
#include "SimpleBLEPheripheral_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "blueapi.h"
#include "peripheral.h"
#include "timers.h"

#include "profileApi.h"
#include "hids_mouse.h"

#include "rtl_delay.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"

#include "iqs773_873.h"


extern void AppHandleIODriverMessage(BEE_IO_MSG io_driver_msg_recv);
extern void Driver_Init(void);

extern uint8_t _touch_flag;

extern uint8_t gHIDSServiceId;

extern const uint8_t MouseTestDataMoveUp[4];
extern const uint8_t MouseTestDataMoveDown[4];
extern const uint8_t MouseTestDataMoveRight[4];
extern const uint8_t MouseTestDataMoveLeft[4];
extern uint8_t MouseData[4];


/****************************************************************************/
/* Events                                                                   */
/****************************************************************************/
#define BLUEAPI_MSG_EVENT               0x01
#define EVENT_IODRIVER_TO_APP           0x05

#define BROADCASTER_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)   /* Task priorities. */
#define BROADCASTER_TASK_STACK_SIZE     1024*2

#define MAX_NUMBER_OF_RX_EVENT          0x20
#define MAX_NUMBER_OF_MESSAGE           0x20
#define MAX_HEARTRATE_TASK_COUNT        0x20

#define IQS_MSG_RDY_EVENT               0x01

//#define USE_POLLING                       1
#define SENSOR_POLLING_INTERVAL         50
#define SENSOR_IQS_TIMER_ID             3552

xTaskHandle  hOTAAppTaskHandle;
xTaskHandle  hIqsAppTaskHandle;

xQueueHandle hEventQueueHandle;
xQueueHandle hMessageQueueHandle;
xQueueHandle hIoQueueHandle;
xQueueHandle hIqsQueueHandle = NULL;
xTimerHandle hIQS_Timer = NULL;

uint8_t Moudle_Type;

uint8_t iqs_data_buf[64];

uint8_t iqs_sys_flag;
uint8_t iqs_event_flag;
uint8_t iqs_touch_flag;
uint8_t iqs_hall_flag;
uint8_t iqs_gesture_flag;
uint8_t iqs_temp_flag;

uint8_t iqs_global_set;
uint8_t iqs_touch_set;
uint8_t iqs_force_set;
uint8_t iqs_wear_set;
uint8_t iqs_hold_set;
uint8_t iqs_slider_set;
uint8_t iqs_hall_set;
uint8_t iqs_temp_set;

int16_t iqs_touch_cnt;
int16_t iqs_force_cnt;
int16_t iqs_force_lta;
int16_t iqs_wear_cnt;
int16_t iqs_hall_cnt;
int16_t iqs_temp_cnt;


void peripheralBlueAPIMessageCB(PBlueAPI_UsMessage pMsg)
{
    uint8_t Event = BLUEAPI_MSG_EVENT;

    if (xQueueSend(hMessageQueueHandle, &pMsg, 0) == errQUEUE_FULL)
    {
        blueAPI_BufferRelease(pMsg);
    }
    else if (xQueueSend(hEventQueueHandle, &Event, 0) == errQUEUE_FULL)
    {

    }
}

void bee_task_app(void *pvParameters);
void iqs_task_app(void *pvParameters);

void application_task_init()
{
    /* Create APP Task. */
    xTaskCreate(bee_task_app,               /* Pointer to the function that implements the task. */
                "APPTask",                  /* Text name for the task.  This is to facilitate debugging only. */
                256,                        /* Stack depth in words. 1KB*/
                NULL,                       /* We are not using the task parameter. */
                1,                          /* This task will run at priority 1. */
                &hOTAAppTaskHandle);        /* the task handle. */

    /* Create APP Task. */
    xTaskCreate(iqs_task_app,               /* Pointer to the function that implements the task. */
                "IQS_APPTask",              /* Text name for the task.  This is to facilitate debugging only. */
                256,                        /* Stack depth in words. 1KB*/
                NULL,                       /* We are not using the task parameter. */
                1,                          /* This task will run at priority 1. */
                &hIqsAppTaskHandle);        /* the task handle. */
}


/**
* @brief
*
*
* @param   pvParameters
* @return  void
*/
void bee_task_app(void *pvParameters)
{
    uint8_t Event;

    hMessageQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE,
                                       sizeof(PBlueAPI_UsMessage));

    hIoQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE,
                                  sizeof(BEE_IO_MSG));

    hEventQueueHandle = xQueueCreate(MAX_NUMBER_OF_MESSAGE + MAX_NUMBER_OF_RX_EVENT,
                                     sizeof(unsigned char));

    peripheral_StartBtStack();

    Driver_Init();

    while (true)
    {
        if (xQueueReceive(hEventQueueHandle, &Event, portMAX_DELAY) == pdPASS)
        {
            if (Event == BLUEAPI_MSG_EVENT) /* BlueAPI */
            {
                PBlueAPI_UsMessage pMsg;

                while (xQueueReceive(hMessageQueueHandle, &pMsg, 0) == pdPASS)
                {
                    peripheral_HandleBlueAPIMessage(pMsg);
                }
            }
            else if (Event == EVENT_NEWIODRIVER_TO_APP)
            {
                BEE_IO_MSG io_driver_msg_recv;

                //while(xQueueReceive(hIoQueueHandle, &io_driver_msg_recv, 0) == pdPASS)
                if (xQueueReceive(hIoQueueHandle, &io_driver_msg_recv, 0) == pdPASS)
                {
                    AppHandleIODriverMessage(io_driver_msg_recv);
                }
            }
        }
    }
}



/***************************************************************************

***************************************************************************/
void iqs_task_app(void *pvParameters)
{
    uint8_t Event;

    hIqsQueueHandle = xQueueCreate(0x40, sizeof(uint8_t));

    //Moudle_Type = i2c_iqs_check_device();

    Moudle_Type = IQS873_MODULE;

    if (Moudle_Type == IQS773_MODULE)
    {
        // Initial IQS773
        i2c_iqs773_setup();
    }
    else if (Moudle_Type == IQS873_MODULE)
    {
        // Initial IQS873
        i2c_iqs873_setup();
    }
    else
    {
        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Wrong Device\r\n", 0);
    }

    // Enable Interrupt
    GPIO_ClearINTPendingBit(GPIO_GetPin(P2_1));
    GPIO_INTConfig(GPIO_GetPin(P2_1), ENABLE);
    GPIO_MaskINTConfig(GPIO_GetPin(P2_1), DISABLE);

    while (true)
    {
        if (xQueueReceive(hIqsQueueHandle, &Event, portMAX_DELAY) == pdPASS)
        {
            if (Event == IQS_MSG_RDY_EVENT)
            {
                if (Moudle_Type == IQS773_MODULE)
                {
                    i2c_iqs773_i2c_stop_ctrl(DISABLE_I2C_STOP);

                    i2c_iqs_burst_read_no_rdy(IQS773_SYSTEM_FLAGS, &iqs_data_buf[0], 0x0C);

                    i2c_iqs_burst_read_no_rdy(IQS773_CH_COUNTS, &iqs_data_buf[16], 0x02);

                    i2c_iqs_burst_read_no_rdy(IQS773_CH_LTA, &iqs_data_buf[18], 0x02);

                    i2c_iqs773_i2c_stop_ctrl(ENABLE_I2C_STOP);

                    // System flags
                    iqs_sys_flag = iqs_data_buf[0];

                    // Global events
                    iqs_event_flag = iqs_data_buf[1];

                    // Touch UI flags
                    iqs_touch_flag = iqs_data_buf[2];

                    // Hall UI Output
                    iqs_hall_flag = iqs_data_buf[6];

                    //Temp UI Output
                    iqs_temp_flag = iqs_data_buf[9];

                    // Reset Indicator
                    if ((iqs_sys_flag & 0x80) == 0x80)
                    {
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IQS773 Reset\r\n", 0);

                        // Disable Interrupt
                        GPIO_ClearINTPendingBit(GPIO_GetPin(P2_1));
                        GPIO_INTConfig(GPIO_GetPin(P2_1), DISABLE);

                        i2c_iqs773_setup();

                        // Enable Interrupt
                        GPIO_ClearINTPendingBit(GPIO_GetPin(P2_1));
                        GPIO_INTConfig(GPIO_GetPin(P2_1), ENABLE);
                    }
                    else
                    {
                        // Prox UI Event
                        if (iqs_event_flag & 0x01)
                        {
                            iqs_force_cnt = ((iqs_data_buf[17] << 8) + iqs_data_buf[16]);

                            iqs_force_lta = ((iqs_data_buf[19] << 8) + iqs_data_buf[18]);

                            iqs_force_cnt = iqs_force_cnt - iqs_force_lta;

                            if ((iqs_touch_flag & 0x10) == 0x10)
                            {
                                if (!iqs_force_set)
                                {
                                    iqs_force_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Force Touch 1  Delta:%d\n", 1, iqs_force_cnt);
                                }
                            }
                            else if ((iqs_touch_flag & 0x00) == 0x00)
                            {
                                if (iqs_force_set)
                                {
                                    iqs_force_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Force Touch 0  Delta:%d\n", 1, iqs_force_cnt);
                                }
                            }
                        }

                        // Hall UI Event
                        if (iqs_event_flag & 0x04)
                        {
                            iqs_hall_cnt = ((iqs_data_buf[8] << 8) + iqs_data_buf[7]);

                            if ((iqs_hall_flag & 0x04) == 0x04)
                            {
                                if (!iqs_hall_set)
                                {
                                    iqs_hall_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Hall Touch 1  Delta:%d\n", 1, iqs_hall_cnt);
                                }
                            }
                            else if ((iqs_hall_flag & 0x00) == 0x00)
                            {
                                if (iqs_hall_set)
                                {
                                    iqs_hall_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Hall Touch 0  Delta:%d\n", 1, iqs_hall_cnt);
                                }
                            }
                        }

                        // Temp UI Event
                        if (iqs_event_flag & 0x10)
                        {
                            iqs_temp_cnt = ((iqs_data_buf[11] << 8) + iqs_data_buf[10]);

                            if ((iqs_temp_flag & 0x80) == 0x80)
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Temp Trip  Delta:%d\n", 1, iqs_temp_cnt);
                            }
                        }
                    }
                }
                else if (Moudle_Type == IQS873_MODULE)
                {
                    i2c_iqs873_i2c_stop_ctrl(DISABLE_I2C_STOP);

                    i2c_iqs_burst_read_no_rdy(IQS873_MM_SYSTEM_FLAGS, &iqs_data_buf[0], 0x08);

                    i2c_iqs_burst_read_no_rdy(IQS873_MM_CH_DELTAS, &iqs_data_buf[10], 0x10);

                    i2c_iqs873_i2c_stop_ctrl(ENABLE_I2C_STOP);

                    // System flags
                    iqs_sys_flag = iqs_data_buf[0];

                    // Global events
                    iqs_event_flag = iqs_data_buf[1];

                    // Gesture events
                    iqs_gesture_flag = iqs_data_buf[2];

                    // Touch UI flags
                    iqs_touch_flag = iqs_data_buf[6];

                    // Reset Indicator
                    if ((iqs_sys_flag & 0x80) == 0x80)
                    {
                        DBG_BUFFER(MODULE_APP, LEVEL_INFO, "IQS873 Reset\r\n", 0);

                        // Disable Interrupt
                        GPIO_ClearINTPendingBit(GPIO_GetPin(P2_1));
                        GPIO_INTConfig(GPIO_GetPin(P2_1), DISABLE);

                        i2c_iqs873_setup();

                        // Enable Interrupt
                        GPIO_ClearINTPendingBit(GPIO_GetPin(P2_1));
                        GPIO_INTConfig(GPIO_GetPin(P2_1), ENABLE);
                    }
                    else
                    {
                        // Touch UI Event
                        if ((iqs_event_flag & 0x02) == 0x02)
                        {
                            iqs_force_cnt = ((iqs_data_buf[13] << 8) + iqs_data_buf[12]);

                            iqs_wear_cnt = ((iqs_data_buf[21] << 8) + iqs_data_buf[20]);

                            iqs_hall_cnt = ((iqs_data_buf[25] << 8) + iqs_data_buf[24]);

#if 0
                            // Global Prox
                            if ((iqs_touch_flag & 0x01) == 0x01)
                            {
                                if (!iqs_global_set)
                                {
                                    iqs_global_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Global Touch 1", 0);
                                }
                            }
                            else if ((iqs_touch_flag & 0x01) == 0x00)
                            {
                                if (iqs_global_set)
                                {
                                    iqs_global_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Global Touch 0", 0);
                                }
                            }
#endif

                            // Force Touch
                            if ((iqs_touch_flag & 0x02) == 0x02)
                            {
                                if (!iqs_force_set)
                                {
                                    iqs_force_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Force Touch 1  Delta:%d\n", 1, iqs_force_cnt);
                                }
                            }
                            else if ((iqs_touch_flag & 0x02) == 0x00)
                            {
                                if (iqs_force_set)
                                {
                                    iqs_force_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Force Touch 0  Delta:%d\n", 1, iqs_force_cnt);
                                }
                            }

#if 0
                            // Touch on slider
                            if ((iqs_touch_flag & 0x1C))
                            {
                                if (!iqs_slider_set)
                                {
                                    iqs_slider_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Slider Touch 1", 0);
                                }
                            }
                            else
                            {
                                if (iqs_slider_set)
                                {
                                    iqs_slider_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Slider Touch 0", 0);
                                }
                            }
#endif

                            //Wear Detect
                            if ((iqs_touch_flag & 0x20) == 0x20)
                            {
                                if (!iqs_wear_set)
                                {
                                    iqs_wear_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Wear Detect 1  Delta:%d\n", 1, iqs_wear_cnt);
                                }
                            }
                            else if ((iqs_touch_flag & 0x20) == 0x00)
                            {
                                if (iqs_wear_set)
                                {
                                    iqs_wear_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Wear Detect 0  Delta:%d\n", 1, iqs_wear_cnt);
                                }
                            }

                            //Hall Touch
                            if ((iqs_touch_flag & 0x80) == 0x80)
                            {
                                if (!iqs_hall_set)
                                {
                                    iqs_hall_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Hall Touch 1  Delta:%d\n", 1, iqs_hall_cnt);
                                }
                            }
                            else if ((iqs_touch_flag & 0x80) == 0x00)
                            {
                                if (iqs_hall_set)
                                {
                                    iqs_hall_set = 0;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Hall Touch 0  Delta:%d\n", 1, iqs_hall_cnt);
                                }
                            }
                        }

#if 0
                        // Gesture UI Event
                        if ((iqs_event_flag & 0x08) == 0x08)
                        {
                            // Flick Negative
                            if ((iqs_gesture_flag & 0x08) == 0x08)
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Flick Negative", 0);
                            }
                            // Flick Positive
                            else if ((iqs_gesture_flag & 0x04) == 0x04)
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Flick Positive", 0);
                            }
                            // Hold
                            else if ((iqs_gesture_flag & 0x02) == 0x02)
                            {
                                if (!iqs_hold_set)
                                {
                                    iqs_hold_set = 1;

                                    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Hold", 0);
                                }
                            }
                            // Tap
                            else if ((iqs_gesture_flag & 0x01) == 0x01)
                            {
                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Tap", 0);
                            }

                            //Double tap you can look at the tap bit and implement a timer. Not on chip
                        }

                        // Release Hold
                        if ((iqs_touch_flag & 0x9E) == 0x00)
                        {
                            if (iqs_hold_set)
                            {
                                iqs_hold_set = 0;

                                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Release Hold", 0);
                            }
                        }
#endif
                    }
                }
            }
        }
    }
}


// IQS RDY pin interrupt
void Gpio17IntrHandler(void)
{
    uint8_t iqs_rdy_event;

    GPIO_ClearINTPendingBit(GPIO_GetPin(P2_1));

    iqs_rdy_event = IQS_MSG_RDY_EVENT;

    // Send Task
    xQueueSend(hIqsQueueHandle, &iqs_rdy_event, 0);
}

