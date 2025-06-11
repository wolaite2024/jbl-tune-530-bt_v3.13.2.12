/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "string.h"
#include "rtl876x_nvic.h"
#include "rtl876x_qdec.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "app_timer.h"
#include "section.h"
#include "app_dlps.h"
#include "app_msg.h"
#include "app_mmi.h"
#include "app_cfg.h"
#include "hal_gpio.h"
#include "app_io_msg.h"
#include "trace.h"
#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#if F_APP_USB_HID_SUPPORT
#include "app_usb_mmi.h"
#endif
#endif

#if (F_APP_QDECODE_SUPPORT == 1)

#define QDEC_Y_PHA_PIN app_cfg_const.qdec_y_pha_pinmux
#define QDEC_Y_PHB_PIN app_cfg_const.qdec_y_phb_pinmux

#define QDEC_WAKEUP_MAGIC_NUM   0xABAB

#define QDEC_BLOCK_DLPS_TIMER_MS    4000

typedef struct
{
    int16_t pre_ct;     //previous counter value
    int16_t cur_ct;     //current counter value
    uint16_t dir;        //1--up; 0-- down
} T_APP_QDEC;

T_APP_QDEC qdec_ctx;

static uint8_t app_qdec_timer_id = 0;
static uint8_t timer_idx_qdec_block_dlps = 0;

static uint8_t qdecoder_a_status = 0;
static uint8_t qdecoder_b_status = 0;

static uint8_t pre_a_status = 0;
static uint8_t pre_b_status = 0;

static bool wakeup_2phase = false;

static void app_qdec_ctx_clear(void)
{
    memset(&qdec_ctx, 0, sizeof(T_APP_QDEC));
}

static void app_qdec_pinmux_config(void)
{
    Pinmux_Config(QDEC_Y_PHA_PIN, QDEC_PHASE_A_Y);
    Pinmux_Config(QDEC_Y_PHB_PIN, QDEC_PHASE_B_Y);
}

static void app_qdec_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    app_stop_timer(&timer_idx_qdec_block_dlps);
    app_dlps_enable(APP_DLPS_ENTER_CHECK_QDEC);
    wakeup_2phase = false;
}

void app_qdec_pad_config(void)
{
    Pad_Config(QDEC_Y_PHA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(QDEC_Y_PHB_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP,
               PAD_OUT_DISABLE,
               PAD_OUT_LOW);
}

void app_qdec_init_status_read(void)
{
    hal_gpio_init_pin(QDEC_Y_PHA_PIN, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    hal_gpio_init_pin(QDEC_Y_PHB_PIN, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_UP);
    qdecoder_a_status = hal_gpio_get_input_level(QDEC_Y_PHA_PIN);
    qdecoder_b_status = hal_gpio_get_input_level(QDEC_Y_PHB_PIN);
    app_qdec_pinmux_config();
}

void app_qdec_driver_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, ENABLE);

    QDEC_InitTypeDef qdecInitStruct;
    QDEC_StructInit(&qdecInitStruct);
    qdecInitStruct.axisConfigY       = ENABLE;
    qdecInitStruct.debounceEnableY   = Debounce_Enable;
    qdecInitStruct.initPhaseY = (qdecoder_a_status << 1) | qdecoder_b_status;
    qdecInitStruct.counterScaleY = counterScaleDisable;
    QDEC_Init(QDEC, &qdecInitStruct);
    QDEC_INTConfig(QDEC, QDEC_Y_INT_NEW_DATA, ENABLE);
    QDEC_INTMask(QDEC, QDEC_Y_ILLEAGE_INT_MASK, ENABLE);
    QDEC_Cmd(QDEC, QDEC_AXIS_Y, ENABLE);

    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = QDEC_IRQn;
    nvic_init_struct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&nvic_init_struct);

    app_qdec_ctx_clear();
}

void app_qdec_init(void)
{
    app_timer_reg_cb(app_qdec_timeout_cb, &app_qdec_timer_id);
}

void app_qdec_enter_power_down_cfg(void)
{
    RCC_PeriphClockCmd(APBPeriph_QDEC, APBPeriph_QDEC_CLOCK, DISABLE);
    Pad_Config(QDEC_Y_PHA_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(QDEC_Y_PHB_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

void app_qdec_pad_enter_dlps_config(void)
{
    app_qdec_init_status_read();

    pre_a_status = qdecoder_a_status;
    pre_b_status = qdecoder_b_status;

    Pad_Config(QDEC_Y_PHA_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(QDEC_Y_PHB_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    if (qdecoder_a_status)
    {
        app_dlps_set_pad_wake_up(QDEC_Y_PHA_PIN, PAD_WAKEUP_POL_LOW);
    }
    else
    {
        app_dlps_set_pad_wake_up(QDEC_Y_PHA_PIN, PAD_WAKEUP_POL_HIGH);
    }

    if (qdecoder_b_status)
    {
        app_dlps_set_pad_wake_up(QDEC_Y_PHB_PIN, PAD_WAKEUP_POL_LOW);
    }
    else
    {
        app_dlps_set_pad_wake_up(QDEC_Y_PHB_PIN, PAD_WAKEUP_POL_HIGH);
    }

    //DBG_DIRECT("qdecoder_a_status %d  qdecoder_b_status %d  ", qdecoder_a_status,
    //           qdecoder_b_status);
}

void app_qdec_pad_exit_dlps_config(void)
{
    app_qdec_init_status_read();
    app_qdec_driver_init();
}

void app_qdec_wakeup_handle(void)
{
    app_qdec_init_status_read();

    if (pre_a_status != qdecoder_a_status ||
        pre_b_status != qdecoder_b_status)
    {
        if (pre_a_status != qdecoder_a_status &&
            pre_b_status != qdecoder_b_status)
        {
            wakeup_2phase = true;
            T_IO_MSG qdec_msg;
            qdec_msg.type = IO_MSG_TYPE_GPIO;
            qdec_msg.subtype = IO_MSG_GPIO_QDEC;
            qdec_msg.u.param = (qdec_ctx.dir << 16) | 1;
            app_io_msg_send(&qdec_msg);
        }
        else
        {
            //send msg to start qdec timer outside
            T_IO_MSG qdec_msg;
            qdec_msg.type = IO_MSG_TYPE_GPIO;
            qdec_msg.subtype = IO_MSG_GPIO_QDEC;
            qdec_msg.u.param = QDEC_WAKEUP_MAGIC_NUM;
            app_io_msg_send(&qdec_msg);
        }
    }
}

void app_qdec_msg_handler(T_IO_MSG *io_driver_msg_recv)
{
    uint16_t direction = io_driver_msg_recv->u.param >> 16;
    uint16_t delta = (uint16_t)(io_driver_msg_recv->u.param & 0xFFFF);
    bool vol_is_up = false;
    uint8_t action = MMI_TOTAL;

    APP_PRINT_INFO2("app_qdec_msg_handler: delta 0x%x , direction %d", delta, direction);

#if F_APP_DURIAN_SUPPORT
    vol_is_up = app_durian_key_vol_is_up(direction);
#else
    vol_is_up = (direction != 0) ? true : false;
#endif


    if (delta == QDEC_WAKEUP_MAGIC_NUM)
    {
        //only need start timer
    }
    else
    {
        if (vol_is_up)
        {
            action = MMI_DEV_SPK_VOL_UP;
        }
        else
        {
            action = MMI_DEV_SPK_VOL_DOWN;
        }

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI
        uint8_t multilink_active_link = app_multilink_customer_get_active_index();

        APP_PRINT_INFO2("app_qdec_msg_handler: CUSTOMER_MULTILINK_SCENERIO_2_MMI action = %x, active_link = %x",
                        action, multilink_active_link);

        if (multilink_active_link == multilink_usb_idx)
        {
#if F_APP_USB_HID_SUPPORT
            if (app_usb_mmi_handle_action(action) == -ESRCH)
            {
                app_mmi_handle_action(action);
            }
#endif
        }
        else
#endif
        {
            app_mmi_handle_action(action);
        }
    }
    app_dlps_disable(APP_DLPS_ENTER_CHECK_QDEC);
    app_start_timer(&timer_idx_qdec_block_dlps, "qdec_block_dlps",
                    app_qdec_timer_id, 0, 0, false,
                    QDEC_BLOCK_DLPS_TIMER_MS);
}

RAM_TEXT_SECTION void QDEC_Handler(void)
{
    if (QDEC_GetFlagState(QDEC, QDEC_FLAG_NEW_CT_STATUS_Y) == SET)
    {
        /* Mask qdec interrupt */
        QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, ENABLE);

        /* Read direction & count */
        qdec_ctx.dir = QDEC_GetAxisDirection(QDEC, QDEC_AXIS_Y);
        qdec_ctx.cur_ct = QDEC_GetAxisCount(QDEC, QDEC_AXIS_Y);

        T_IO_MSG qdec_msg;

        qdec_msg.type = IO_MSG_TYPE_GPIO;
        qdec_msg.subtype = IO_MSG_GPIO_QDEC;
        qdec_msg.u.param = (qdec_ctx.dir << 16) | 1;

        APP_PRINT_INFO3("QDEC_Handler: pre_ct %d , cur_ct %d  wakeup_2phase %d", qdec_ctx.pre_ct,
                        qdec_ctx.cur_ct, wakeup_2phase);

        if (qdec_ctx.pre_ct == 0)
        {
            if (wakeup_2phase == false)
            {
                app_io_msg_send(&qdec_msg);
                qdec_ctx.pre_ct = qdec_ctx.cur_ct;
            }
        }
        else if ((qdec_ctx.cur_ct - qdec_ctx.pre_ct >= 2) || (qdec_ctx.cur_ct - qdec_ctx.pre_ct <= -2))
        {
            app_io_msg_send(&qdec_msg);
            qdec_ctx.pre_ct = qdec_ctx.cur_ct;
        }

        /* clear qdec interrupt flags */
        QDEC_ClearFlags(QDEC, QDEC_CLR_NEW_CT_Y);
        /* Unmask qdec interrupt */
        QDEC_INTMask(QDEC, QDEC_Y_CT_INT_MASK, DISABLE);

    }
    wakeup_2phase = false;
}
#endif
