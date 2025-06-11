/*******************************************************************************
* Copyright (C) 2021 Realtek Semiconductor Corporation. All Rights Reserved.
*/

#if F_APP_ALC1017_SUPPORT
#include "app_alc1017.h"
#include "app_sensor_i2c.h"
#include "platform_utils.h"
#include "trace.h"

#define ALC1017_DEVICE_ADDR (0x90 >> 1)

struct alc1017_init_reg
{
    uint32_t reg;
    uint8_t val;
    uint16_t delay;
};

// 48k 24bit
static struct alc1017_init_reg init_list[] =
{

    {0x0000C000, 0xFF, 0},  // SW RST
    {0x0000D101, 0x10, 0},  //LR Configuration
    {0x0000C001, 0x02, 0},
    {0x0000C003, 0x22, 0},
    {0x0000C004, 0x44, 0},
    {0x0000C005, 0x44, 0},
    {0x0000C007, 0x21, 0},
    {0x0000C020, 0x08, 0},  // Enable BCLK detection
// PLL Setting x16
    {0x0000C720, 0x00, 0},  //PLL Settings
    {0x0000C721, 0x00, 0},
    {0x0000C722, 0x3E, 0},
    {0x0000C104, 0xAA, 0},  // EN_CKGEN_DAC
    {0x0000C110, 0x59, 0},  // EN_CKGEN_ADC
    {0x0000C112, 0xC0, 0},  // I2S TX  RX enable
//WrL1 0000C11A 01 // TDM Ch
    {0x0000C340, 0x80, 0},  // SDB
    {0x0000D017, 0x08, 0},  //adcdat sel
    {0x0000D01A, 0xC0, 0},  // pow_clk_ad
    {0x0000D01B, 0xCF, 0},  // pow_clk_ad, fifo, dre
    {0x0000D01C, 0x0C, 0},  // pow_clk_da, spkvol, gain_syn
    {0x0000D20C, 0x14, 0},  // mu_pwm_dac_offset=0, mute_pwm_offset=0
    {0x0000C908, 0xFF, 0},  // da_gain = 0 dB (5W / 8 Ohm)
// =========================== Battery Protect ===========================
    {0x0000DB14, 0x03, 0},  // vbat_code_target_dn_hold
    {0x0000DB18, 0x00, 0},
    {0x0000DB19, 0x40, 0},
    {0x0000DB1A, 0x00, 0},
    {0x0000DB1C, 0x00, 0},
    {0x0000DB1D, 0x40, 0},
    {0x0000DB1E, 0x00, 0},
    {0x0000CB00, 0x33, 0},
//WrL1 0000CB00 31 // pvdd_det_en, bvdd_det_en ; bvdd_limit_mute
//===========================Analog Power Control===========================
    {0x0000C318, 0x44, 0},  // pow_temp_reg = 1
    {0x0000C325, 0xCE, 0},  // POW_LDO2 ; POW_DAC ; POW_ISENSE_SPK ; POW_PORAVDD ; POW_VSENSE_SPK
    {0x0000C326, 0x13, 0},  // EN_PLL2F ; POW_BG ; POW_BG_MBIAS_LV
    {0x0000C327, 0x5F, 0},  // POW_DET_VBAT; POW_MBIAS_LV ; POW_ADC_I ; POW_ADC_V ; POW_ADC_T ; POWD_ADC_T
    {0x0000C328, 0xF3, 0},  // POW_MIX_I ; POW_MIX_V ; POW_SUM_I ; POW_SUM_V
//===========================DC-DC Boost ================================
//WrL1 0000C315 22 // 44.1k based
    {0x0000C350, 0x81, 0},  // Fixed boost mode
    {0x0000C351, 0x77, 0},  // 9.3V
    {0x0000C330, 0x10, 0},  // EN_SWR
    {0x0000C331, 0xF2, 0},  // EN_OTP_SWR
// =========================== HPF (D-A) ==================================
// =========================== BK Gain ==================================
    {0x0000C551, 0x0F, 0},  // bk_gain
    {0x0000C552, 0xFF, 0},  // bk_gain
    {0x0000C651, 0xC0, 0},  // bk_gain enable
    {0x0000C550, 0xD0, 0},  // Enable post_gain proc
// =========================== TBDRC ==================================
// HB
    {0x0000C612, 0x00, 0},
// MB
    {0x0000C622, 0x00, 0},
// LB
    {0x0000C632, 0x00, 0},
// POS
    {0x0000C642, 0x00, 0},
    {0x0000C602, 0xF0, 0},  // All Crossover Filter enable, All paths unmute
    {0x0000C600, 0xD0, 0},  // enable TBDRC
// =========================== Protection Gain ==============================
    {0x0000CC02, 0x78, 0},  // Enable Crossover Filter
    {0x0000CC00, 0x90, 0},  // Enable Protection Gain
    {0x0000D5BA, 0xEF, 0},  //Basic Mode Gain
//===========================Class D ===================================
    {0x0000D370, 0x10, 0},  // pwm_freq_ctrl_src_sel = From reg
    {0x0000D300, 0x10, 0},  // DRIVER_READY_SPK = 1
//===========================ADC Path & Gain =============================
    {0x0000C300, 0x3F, 0},  //
    {0x0000C301, 0x1D, 0},  //
    {0x0000C10B, 0x2E, 0},  // ad_gain_l
    {0x0000C10C, 0x36, 0},  // ad_gain_r
//===========================DC Calibration =============================
    {0x0000D208, 0xCC, 0},  // cal_pwm_dac_intg_coef_sel = 2^-16
    {0x0000D20A, 0xCC, 0},  // cal_pwm_dac_intg_coef_sel = 2^-16
    {0x0000D20C, 0x24, 1000},  // cal_pwm_dac_timing_sel = 10 ms
    {0x0000D200, 0xB9, 0},  // K ADC & DAC
// =========================== STP ====================================
// R0 Max = 2.4 ohm
    {0x0000D5A5, 0x00, 0},
    {0x0000D5A6, 0x6A, 0},
    {0x0000D5A7, 0xAA, 0},
    {0x0000D5A8, 0xAA, 0},
// R0 Min = 11.2 ohm
    {0x0000D5AA, 0x00, 0},
    {0x0000D5AB, 0x16, 0},
    {0x0000D5AC, 0xDB, 0},
    {0x0000D5AD, 0x6D, 0},
// SPK alpha = 0.0033
    {0x0000D545, 0x09, 0},
    {0x0000D546, 0x78, 0},
    {0x0000D547, 0x3E, 0},
    {0x0000D548, 0x0F, 0},
    {0x0000D500, 0x20, 0},  // sin_gen_en
    {0x0000C504, 0x3F, 0},  // stp_delay_nts
    {0x0000C540, 0x00, 0},  // iv_hpf_band_en = 0
    {0x0000C541, 0x0A, 0},  // voltage_sep_sel = adfilter output ; current_sep_sel = adfilter output
    {0x0000C542, 0x1A, 0},  // dcr_sep_sel=1 ; STP VI from adfilter output
//WrL1 0000C512 40 // stp_calib_rs_temp_sel = From REG
    {0x0000C512, 0x00, 0},  // stp_calib_rs_temp_sel = From Calibration
    {0x0000C520, 0x40, 0},  // Tdc from start value
    {0x0000C51B, 0x7F, 0},  // stp_r0_delay
    {0x0000C51D, 0x0F, 0},  // stp_t0_delay
    {0x0000C500, 0x40, 0},  // stp_rs_clb_en
    {0x0000C502, 0x79, 0},
    {0x0000C507, 0x05, 0},  // stp_thres_max_sel = 70C
    {0x0000C509, 0x05, 0},
    {0x0000C510, 0x40, 0},  // R0 from start value
    {0x0000C518, 0xC0, 0},  // stp_r0_en ; t0_en
    {0x0000C500, 0xC0, 0},  // stp_en ; stp_rs_clb_en
// =========================== SEP ====================================
    {0x0000DA0C, 0x00, 0},
    {0x0000DA0D, 0x19, 0},
    {0x0000DA0E, 0xE7, 0},
    {0x0000DA0F, 0x9E, 0},
    {0x0000DA04, 0x00, 0},
    {0x0000DA05, 0x51, 0},
    {0x0000DA06, 0xEB, 0},
    {0x0000DA07, 0x85, 0},
    {0x0000CA16, 0x0F, 0},  //Target Gain
    {0x0000CA17, 0x00, 0},
    {0x0000DA09, 0x5D, 0},  // nlms_sample : ~2s
    {0x0000DA0A, 0xC0, 0},
    {0x0000DA11, 0x26, 0},
    {0x0000DA12, 0x66, 0},
    {0x0000DA13, 0x66, 0},
    {0x0000DA14, 0x66, 0},
    {0x0000DA16, 0x79, 0},
    {0x0000DA17, 0x99, 0},
    {0x0000DA18, 0x99, 0},
    {0x0000DA19, 0x99, 0},
    {0x0000CA09, 0x00, 0},
    {0x0000CA0A, 0x09, 0},
    {0x0000CA0B, 0x02, 0},
    {0x0000CA0C, 0xDE, 0},
    {0x0000CA0E, 0x00, 0},
    {0x0000CA0F, 0x04, 0},
    {0x0000CA10, 0x6C, 0},
    {0x0000CA11, 0x76, 0},
    {0x0000DAB6, 0x00, 0},
    {0x0000DAB7, 0x03, 0},
    {0x0000DAB8, 0x00, 0},
    {0x0000DAB9, 0x03, 0},
    {0x0000DABA, 0x00, 0},
    {0x0000DABB, 0x03, 0},
    {0x0000D017, 0x08, 0},  // dout = xpre / sep_external_gain
    {0x0000CA00, 0xCD, 0},  // pre-emf recovery debounce time= 341 ms ; enable sep (nlms,premf)
//===== ALC1017 Init Done =====


//===Overview===//
    {0x0000C022, 0x04, 0},  //Silence Detection
//Boost Mode
    {0x0000C315, 0x20, 0},
    {0x0000C350, 0x81, 0},
    {0x0000C351, 0x77, 0},
    {0x0000C330, 0x10, 0},
//Boost Current Limit
    {0x0000C336, 0x7E, 0},
    {0x0000C339, 0x00, 0},
    {0x0000C33A, 0xA9, 0},
//PLL+SRC For SampleRate+DataLen

//===DRC Settings===//
//DRC CrossOver(hlpf1)
//DRC CrossOver(hlpf2)
    {0x0000C602, 0xF0, 0},
//Pos Band
    {0x0000D640, 0xF1, 0},
    {0x0000C646, 0x51, 0},
    {0x0000C64E, 0x14, 0},
//High Band
    {0x0000D610, 0xE1, 0},
    {0x0000C616, 0x51, 0},
    {0x0000C61E, 0x14, 0},
//Middle Band
    {0x0000D620, 0xF1, 0},
    {0x0000C626, 0x51, 0},
    {0x0000C62E, 0x14, 0},
//Low Band
    {0x0000D630, 0xF1, 0},
    {0x0000C636, 0x51, 0},
    {0x0000C63E, 0x14, 0},
    {0x0000C600, 0xD0, 0},

//===All EQ Settings===//
//BQ
//Pre/Pos Gain
    {0x0000C400, 0x0C, 0},
    {0x0000C402, 0x04, 0},
//Acoustic
//Pre/Pos Gain
    {0x0000C800, 0x0C, 0},
    {0x0000C802, 0x04, 0},

//===VDEQ===//
    {0x0000C550, 0xC0, 0},  //battery protection gain
    {0x0000C90A, 0x08, 0},  //VDEQ En

//===STP===//
    {0x0000C500, 0xC0, 0},  //STP On or Off
    {0x0000C507, 0x05, 0},  //Mute
    {0x0000D500, 0x20, 0},  //SineGen
//STP R0 Max
//STP R0 Min

//===SEP===//
    {0x0000CC00, 0x90, 0},  //SEP On or off
    {0x0000CA00, 0xCD, 0},
//SEP Attack Gain
//SEP Recovery Gain
    {0x0000CC02, 0x08, 0},
    {0x0000CC00, 0x10, 0},
//SEP CrossOver(hlpf1)
//SEP CrossOver(hlpf2)
    {0x0000CC00, 0x90, 0},
    {0x0000CC02, 0x78, 0},

//===All Battery Protection Settings===//
    {0x0000CB00, 0x33, 0},  //Battery Protection
};

static void app_alc1017_i2c_write(uint8_t slave_addr, uint32_t reg_addr, uint8_t reg_value)
{
    I2C_Status status;

    status = app_sensor_i2c_32_write_8(slave_addr, reg_addr, reg_value);
    if (status != I2C_Success)
    {
        APP_PRINT_INFO1("app_alc1017_i2c_write err: status %d", status);
    }
}

static void app_alc1017_i2c_read(uint8_t slave_addr, uint32_t reg_addr)
{
    uint8_t read_buf[1] = {0};
    I2C_Status status;

    status = app_sensor_i2c_32_read_8(slave_addr, reg_addr, read_buf);
    if (status == I2C_Success)
    {
        APP_PRINT_INFO2("app_alc1017_i2c_read pass: reg 0x%08x, val 0x%02x", reg_addr, read_buf[0]);
    }
    else
    {
        APP_PRINT_ERROR1("app_alc1017_i2c_read err: status %d", status);
    }
}

/***************************************************************************
 * Description: for ALC1017 init.
 *
 **************************************************************************/
void app_alc1017_init(void)
{
    int i;
    int init_list_len = sizeof(init_list) / sizeof(init_list[0]);

    for (i = 0; i < init_list_len; i++)
    {
        app_alc1017_i2c_write(ALC1017_DEVICE_ADDR, init_list[i].reg, init_list[i].val);
        platform_delay_ms(init_list[i].delay);
        // app_alc1017_i2c_read(ALC1017_DEVICE_ADDR, init_list[i].reg);
    }
    APP_PRINT_INFO0("APP ALC1017 INIT SUCCESS");
}
#endif

