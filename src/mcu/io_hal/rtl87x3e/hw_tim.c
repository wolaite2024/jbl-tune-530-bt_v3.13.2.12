/*
 * Copyright (c) 2021, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "rtl876x_tim.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_nvic.h"
#include "rtl876x_gdma.h"
#include "hw_tim.h"
#include "hw_tim_internal.h"
#include "os_sync.h"
#include "trace.h"
#include "section.h"

#define PWM_DEADZONE_MASK              (BIT2 | BIT3)          /* TIM2/TIM3 support PWM deadzone */
#define HW_TIMER_DMA_SUPPORT_MASK      (0xFC)                /* TIM0~TIM7 support DMA handshake */

#define hw_timer_is_one_shot(TIMx)              ((TIMx->ControlReg & BIT1) ? false : true)
#define hw_timer_id_valid(id)                   ((id < HW_TIMER_ID_MAX) && (id > HW_TIMER_ID_1))
#define hw_timer_support_deadzone(id)           (BIT(id) & PWM_DEADZONE_MASK)

typedef struct _T_HW_TIMER
{
    union
    {
        struct
        {
            uint32_t timer_id:               5;
            uint32_t is_used:                1;
            uint32_t is_running:             1;
            uint32_t pwm_deadzone_enable:    1;
            uint32_t pwm_pin_index:          8;
            uint32_t pwm_p_pin_index:        8;
            uint32_t pwm_n_pin_index:        8;
        };
        uint32_t flag;
    };

    const char *user_name;
    P_HW_TIMER_CBACK cback;
    void *context;
} T_HW_TIMER;

typedef struct _T_HW_TIMER_TABLE
{
    T_HW_TIMER item[HW_TIMER_ID_MAX];
} T_HW_TIMER_TABLE;

T_HW_TIMER_TABLE hw_timer_pool =
{
    .item[0]     =  {HW_TIMER_ID_EMPTY,   },        /*TIM0/TIM1 reserved for lowstack*/
    .item[1]     =  {HW_TIMER_ID_EMPTY,   },        /*TIM0/TIM1 reserved for lowstack*/
    .item[2]     =  {HW_TIMER_ID_2,       },
    .item[3]     =  {HW_TIMER_ID_3,       },
    .item[4]     =  {HW_TIMER_ID_4,       },
    .item[5]     =  {HW_TIMER_ID_5,       },
    .item[6]     =  {HW_TIMER_ID_6,       },
    .item[7]     =  {HW_TIMER_ID_7,       },
#if (TOTAL_HW_TIMER_NUM == 16)
    .item[8]     =  {HW_TIMER_ID_8,       },
    .item[9]     =  {HW_TIMER_ID_9,       },
    .item[10]    =  {HW_TIMER_ID_10,      },
    .item[11]    =  {HW_TIMER_ID_11,      },
    .item[12]    =  {HW_TIMER_ID_12,      },
    .item[13]    =  {HW_TIMER_ID_13,      },
    .item[14]    =  {HW_TIMER_ID_14,      },
    .item[15]    =  {HW_TIMER_ID_15,      }
#endif
};

uint16_t g_hw_tim_mask = HW_TIMER_MASK;

T_HW_TIMER_HANDLE hw_timer_pool_alloc(bool deadzone_support, bool dma_support)
{
    uint32_t s;
    uint8_t timer_id = 0;
    uint16_t temp_mask = 0;
    T_HW_TIMER_HANDLE handle = NULL;

    s = os_lock();

    uint16_t curr_mask = g_hw_tim_mask & HW_TIMER_DYNAMIC_MASK;

    if (dma_support)
    {
        temp_mask = curr_mask & HW_TIMER_DMA_SUPPORT_MASK;
    }
    else
    {
        temp_mask = curr_mask & (~HW_TIMER_DMA_SUPPORT_MASK);
        if (!temp_mask)
        {
            temp_mask = curr_mask;
        }
    }

    curr_mask = temp_mask;

    if (deadzone_support)
    {
        temp_mask = curr_mask & PWM_DEADZONE_MASK;
    }
    else
    {
        temp_mask = curr_mask & (~PWM_DEADZONE_MASK);
        if (!temp_mask)
        {
            temp_mask = curr_mask;
        }
    }

    if (temp_mask)
    {
        timer_id = 31 - __clz(temp_mask);
    }
    else
    {
        timer_id = HW_TIMER_ID_EMPTY;
    }

    if ((timer_id != HW_TIMER_ID_EMPTY) && (timer_id < HW_TIMER_ID_MAX))
    {
        handle = &hw_timer_pool.item[timer_id];
        g_hw_tim_mask &= ~BIT(timer_id);
    }

    os_unlock(s);

    return handle;
}

bool hw_timer_pool_free(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;
    uint8_t timer_id = p_hw_timer->timer_id;
    bool ret = false;

    uint32_t s = os_lock();
    if (!(g_hw_tim_mask & BIT(timer_id)))
    {
        g_hw_tim_mask |= BIT(timer_id);
        ret = true;
    }
    else
    {
        IO_PRINT_ERROR2("free hw timer error, timer not alloc, id %x, name %s",
                        p_hw_timer->timer_id, TRACE_STRING(p_hw_timer->user_name));
    }
    os_unlock(s);

    return ret;
}

static bool hw_timer_update_isr(VECTORn_Type v_num, IRQ_Fun isr_handler, bool is_xip)
{
    if (is_xip)
    {
        return RamVectorTableUpdate_for_xip_isr(v_num, isr_handler);
    }
    else
    {
        return RamVectorTableUpdate_rom(v_num, isr_handler);
    }
}

RAM_TEXT_SECTION void hw_timer_handler_imp(T_HW_TIMER_ID id)
{
    TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id(id);
    TIM_ClearINT(TIMx);

    if (hw_timer_is_one_shot(TIMx))
    {
        TIM_Cmd(TIMx, DISABLE);
    }

    if (hw_timer_pool.item[id].cback)
    {
        hw_timer_pool.item[id].cback(hw_timer_pool.item[id].context);
    }
}

extern bool hw_timer_callback_xip_check(uint32_t callback);
static void hw_timer_driver_init_int(T_HW_TIMER_ID id, uint32_t period_us, bool reload,
                                     uint32_t callback)
{
    uint32_t period_min = 1;
    TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id(id);

    TIM_TimeBaseInitTypeDef time_param;

    hw_timer_rcc_clock_cmd(id);

    if (callback)
    {
        period_min = 5;
    }

    if (period_us < period_min)
    {
        /* Workaround for hw not accept 0 */
        period_us = period_min;
    }

    TIM_StructInit(&time_param);
    time_param.TIM_Mode = reload ? TIM_Mode_UserDefine : TIM_Mode_FreeRun;
    time_param.ClockDepend = false;
    time_param.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_40;
    time_param.TIM_PWM_En = PWM_DISABLE;
    time_param.TIM_Period = (period_us - 1);
    time_param.PWMDeadZone_En = DEADZONE_DISABLE;
    TIM_TimeBaseInit(TIMx, &time_param);

    if (callback)
    {
        hw_timer_update_isr(hw_timer_get_vectorn_from_id(id), hw_timer_get_isr(id),
                            hw_timer_callback_xip_check((uint32_t)callback));
        NVIC_InitTypeDef nvic_param;

        nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(id);
        nvic_param.NVIC_IRQChannelPriority = 3;
        nvic_param.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_param);

        TIM_ClearINT(TIMx);
        TIM_INTConfig(TIMx, ENABLE);
    }
}

T_HW_TIMER_HANDLE hw_timer_create_int(const char *name, uint32_t period_us, bool reload,
                                      P_HW_TIMER_CBACK cback, bool dma_support)
{
    T_HW_TIMER *p_hw_timer;

    p_hw_timer = hw_timer_pool_alloc(false, dma_support);
    if (p_hw_timer != NULL)
    {
        p_hw_timer->user_name = name;
        p_hw_timer->cback = cback;
        p_hw_timer->context = p_hw_timer;

        hw_timer_driver_init_int((T_HW_TIMER_ID)(p_hw_timer->timer_id), period_us, reload, (uint32_t)cback);
    }

    return (T_HW_TIMER_HANDLE)p_hw_timer;
}

T_HW_TIMER_HANDLE hw_timer_create(const char *name, uint32_t period_us, bool reload,
                                  P_HW_TIMER_CBACK cback)
{
    return hw_timer_create_int(name, period_us, reload, cback, false);
}

T_HW_TIMER_HANDLE hw_timer_create_dma_mode(const char *name, uint32_t period_us, bool reload,
                                           P_HW_TIMER_CBACK cback)
{
    return hw_timer_create_int(name, period_us, reload, cback, true);
}

bool hw_timer_delete(T_HW_TIMER_HANDLE handle)
{
    bool ret = false;

    if (handle != NULL)
    {
        hw_timer_stop(handle);
        ret = hw_timer_pool_free(handle);
    }

    return ret;
}

uint8_t hw_timer_get_dma_handshake(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        return p_hw_timer->timer_id + GDMA_Handshake_TIM1 - 1;
    }

    return 0;
}

const char *hw_timer_get_name(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        return p_hw_timer->user_name;
    }

    return NULL;
}

uint8_t hw_timer_get_id(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        return p_hw_timer->timer_id;
    }

    return 0;
}

RAM_TEXT_SECTION bool hw_timer_start(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));
        bool int_en = (p_hw_timer->cback) ? true : false;

        TIM_Cmd(TIMx, ENABLE);

        if (int_en)
        {
            TIM_INTConfig(TIMx, ENABLE);
        }

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool hw_timer_stop(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

        /* work around for hw timer isr miss-triggered */
        TIM_Cmd(TIMx, DISABLE);
        TIM_ClearINT(TIMx);
        TIM_INTConfig(TIMx, DISABLE);
        TIM_ClearINT(TIMx);

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool hw_timer_restart(T_HW_TIMER_HANDLE handle, uint32_t period_us)
{
    uint32_t period_min = 1;
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));
        bool int_en = (p_hw_timer->cback) ? true : false;

        if (int_en)
        {
            period_min = 5;
        }

        if (period_us < period_min)
        {
            /* Workaround for hw not accept 0 */
            period_us = period_min;
        }

        TIM_Cmd(TIMx, DISABLE);

        if (int_en)
        {
            TIM_INTConfig(TIMx, DISABLE);
            TIM_ClearINT(TIMx);
            TIM_ChangePeriod(TIMx, period_us - 1);
            TIM_INTConfig(TIMx, ENABLE);
        }
        else
        {
            TIM_ChangePeriod(TIMx, period_us - 1);
        }

        TIM_Cmd(TIMx, ENABLE);

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool hw_timer_get_current_count(T_HW_TIMER_HANDLE handle,
                                                 uint32_t *count)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

        *count = TIM_GetCurrentValue(TIMx);
        return true;
    }

    *count = 0;
    return false;
}

bool hw_timer_get_elapsed_time(T_HW_TIMER_HANDLE handle, uint32_t *time)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

        *time = TIM_GetElapsedValue(TIMx);
        return true;
    }

    *time = 0;
    return false;
}

bool hw_timer_get_period(T_HW_TIMER_HANDLE handle, uint32_t *period_us)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

        *period_us = TIMx->LoadCount;
        return true;
    }

    *period_us = 0;
    return false;
}

bool hw_timer_is_active(T_HW_TIMER_HANDLE handle, bool *state)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

        *state = TIM_GetStatus(TIMx);
        return true;
    }

    *state = false;
    return false;
}

static void hw_timer_driver_init_pwm_int(T_HW_TIMER_ID id, uint32_t period_us_high,
                                         uint32_t period_us_low, bool deadzone)
{
    TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id(id);

    hw_timer_rcc_clock_cmd(id);

    TIM_TimeBaseInitTypeDef time_param;

    TIM_StructInit(&time_param);
    time_param.ClockDepend = false;
    time_param.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_40;
    time_param.TIM_PWM_En = PWM_ENABLE;
    time_param.TIM_PWM_High_Count = period_us_high > 1 ? (period_us_high - 1) : 1;
    time_param.TIM_PWM_Low_Count = period_us_low > 1 ? (period_us_low - 1) : 1;
    time_param.PWMDeadZone_En = deadzone ? DEADZONE_ENABLE : DEADZONE_DISABLE;
    time_param.PWM_Deazone_Size = 0;
    TIM_TimeBaseInit(TIMx, &time_param);
}

T_PWM_HANDLE pwm_create(const char *name,
                        uint32_t high_period_us, uint32_t low_period_us, bool deadzone_enable)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)hw_timer_pool_alloc(deadzone_enable, false);

    if (p_hw_timer != NULL)
    {
        p_hw_timer->user_name = name;
        p_hw_timer->cback = NULL;
        p_hw_timer->context = p_hw_timer;

        hw_timer_driver_init_pwm_int((T_HW_TIMER_ID)(p_hw_timer->timer_id), high_period_us, low_period_us,
                                     deadzone_enable);
        p_hw_timer->pwm_deadzone_enable = deadzone_enable;
    }

    return (T_PWM_HANDLE)p_hw_timer;
}

static bool pwm_deinit(T_PWM_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        if (p_hw_timer->pwm_deadzone_enable == false)
        {
            Pinmux_Deinit(p_hw_timer->pwm_pin_index);
            Pad_Config(p_hw_timer->pwm_pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            return true;
        }
        else if (p_hw_timer->pwm_deadzone_enable == true)
        {
            Pinmux_Deinit(p_hw_timer->pwm_p_pin_index);
            Pinmux_Deinit(p_hw_timer->pwm_n_pin_index);
            Pad_Config(p_hw_timer->pwm_p_pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(p_hw_timer->pwm_n_pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            return true;
        }
    }

    return false;
}

bool pwm_delete(T_PWM_HANDLE handle)
{
    return (hw_timer_delete(handle) && pwm_deinit(handle));
}

RAM_TEXT_SECTION bool pwm_start(T_PWM_HANDLE handle)
{
    return hw_timer_start(handle);
}

RAM_TEXT_SECTION bool pwm_stop(T_PWM_HANDLE handle)
{
    return hw_timer_stop(handle);
}

bool pwm_register_timeout_callback(T_PWM_HANDLE handle, P_HW_TIMER_CBACK cback)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

        p_hw_timer->cback = cback;

        hw_timer_update_isr(hw_timer_get_vectorn_from_id(id), hw_timer_get_isr(id),
                            hw_timer_callback_xip_check((uint32_t)cback));

        NVIC_InitTypeDef nvic_param;
        nvic_param.NVIC_IRQChannel = hw_timer_get_irqn_from_id(id);
        nvic_param.NVIC_IRQChannelPriority = 3;
        nvic_param.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic_param);

        TIM_ClearINT(TIMx);
        TIM_INTConfig(TIMx, ENABLE);
        return true;
    }

    return false;
}

bool pwm_is_active(T_PWM_HANDLE handle, bool *state)
{
    return hw_timer_is_active(handle, state);
}

bool pwm_config(T_PWM_HANDLE handle, T_PWM_CONFIG *config_struct)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID id = (T_HW_TIMER_ID)(p_hw_timer->timer_id);
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id(id);

        uint32_t pwm_high_count = config_struct->pwm_high_count ? config_struct->pwm_high_count : 1;
        uint32_t pwm_low_count = config_struct->pwm_low_count ? config_struct->pwm_low_count : 1;

        TIM_TimeBaseInitTypeDef time_param;

        TIM_StructInit(&time_param);
        time_param.PWMDeadZone_En = DEADZONE_DISABLE;
        time_param.PWM_Deazone_Size = 0;
        time_param.ClockDepend = false;
        time_param.TIM_SOURCE_DIV = pwm_src_div_mapping(config_struct->clock_source);
        time_param.TIM_PWM_En = PWM_ENABLE;
        time_param.TIM_PWM_High_Count = pwm_high_count - 1;
        time_param.TIM_PWM_Low_Count = pwm_low_count - 1;

        if (config_struct->pwm_deadzone_enable && hw_timer_support_deadzone(id))
        {
            time_param.PWMDeadZone_En = ENABLE;
            time_param.PWM_Deazone_Size = config_struct->pwm_deadzone_size;
            time_param.PWM_Stop_State_N = config_struct->pwm_n_stop_state;
            time_param.PWM_Stop_State_P = config_struct->pwm_p_stop_state;
        }

        TIM_TimeBaseInit(TIMx, &time_param);
        return true;
    }

    return false;
}

RAM_TEXT_SECTION
bool pwm_pin_config(T_PWM_HANDLE handle, uint8_t pin_index, T_PWM_PIN_FUNC func)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID id = (T_HW_TIMER_ID)p_hw_timer->timer_id;

        if (hw_timer_id_valid(id) == false)
        {
            return false;
        }

        if (func == PWM_FUNC)
        {
            Pinmux_Config(pin_index, hw_timer_get_pwm_func(id));
            Pad_Config(pin_index,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
            p_hw_timer->pwm_pin_index = pin_index;
        }
        else if ((func == PWM_FUNC_P) || (func == PWM_FUNC_N))
        {
            if (!hw_timer_support_deadzone(id))
            {
                IO_PRINT_ERROR1("pwm %d not support deadzone", id);
                return false;
            }

            uint8_t pwm_func_off = (id == 2) ? 0 : 2;
            uint8_t pwm_func_p = PWM2_P + pwm_func_off;
            uint8_t pwm_func_n = PWM2_N + pwm_func_off;

            Pad_Config(pin_index,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

            if (func == PWM_FUNC_P)
            {
                Pinmux_Config(pin_index, pwm_func_p);
                p_hw_timer->pwm_p_pin_index = pin_index;
            }
            else
            {
                Pinmux_Config(pin_index, pwm_func_n);
                p_hw_timer->pwm_n_pin_index = pin_index;
            }
        }
        else
        {
            Pinmux_Deinit(pin_index);
            Pad_Config(pin_index,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }

        return true;
    }

    return false;
}

RAM_TEXT_SECTION bool pwm_change_duty_and_frequency(T_PWM_HANDLE handle, uint32_t high_count,
                                                    uint32_t low_count)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;

    if (p_hw_timer != NULL)
    {
        T_HW_TIMER_ID id = (T_HW_TIMER_ID)p_hw_timer->timer_id;
        TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id(id);
        TIM_PWMChangeFreqAndDuty(TIMx, high_count, low_count);

        return true;
    }

    return false;
}

uint8_t hw_timer_get_free_timer_number(void)
{
    return count_1bits(g_hw_tim_mask & HW_TIMER_DYNAMIC_MASK);
}

void hw_timer_print_all_users(void)
{
    uint16_t used_mask = (~g_hw_tim_mask) & HW_TIMER_DYNAMIC_MASK;

    for (uint8_t i = 0; i < TOTAL_HW_TIMER_NUM; i++)
    {
        if (used_mask & BIT(i))
        {
            IO_PRINT_INFO2("dump hw timer user id %d: %s",
                           i, TRACE_STRING(hw_timer_pool.item[i].user_name));
        }
    }
}

void hw_timer_print_debug_info(T_HW_TIMER_HANDLE handle)
{
    T_HW_TIMER *p_hw_timer = (T_HW_TIMER *)handle;
    TIM_TypeDef *TIMx = hw_timer_get_reg_base_from_id((T_HW_TIMER_ID)(p_hw_timer->timer_id));

    IO_PRINT_TRACE6("Debug hw timer id %d LoadCount 0x%x, CurrentValue 0x%x, ControlReg 0x%x, EOI 0x%x, IntStatus 0x%x",
                    p_hw_timer->timer_id, TIMx->LoadCount,
                    TIMx->CurrentValue, TIMx->ControlReg, TIMx->EOI, TIMx->IntStatus);
}
