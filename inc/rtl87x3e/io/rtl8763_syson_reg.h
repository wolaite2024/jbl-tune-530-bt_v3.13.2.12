#ifndef _RTL8763_SYSON_REG_H
#define _RTL8763_SYSON_REG_H

/* BTAON_reg from Chinwen */
#define BTAON_REG_0x0               0x0
#define BTAON_REG_0x4               0x4
#define BTAON_REG_0x8               0x8
#define BTAON_REG_0xC               0xC
#define BTAON_REG_0x10              0x10
#define BTAON_REG_0x14              0x14
#define BTAON_REG_0x18              0x18
#define BTAON_REG_0x1C              0x1C

typedef union _BTAON_REG_0x0_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t POW_BIAS: 1;                  // 00       rw  1'b0
        uint32_t POW_LDO_PA: 1;                // 01       rw  1'b0
        uint32_t rsvd0: 1;                     // 02       rw  1'b0
        uint32_t POW_LDO533: 1;                // 03       rw  1'b0
        uint32_t POW_LDO_VREF: 1;              // 04       rw  1'b0
        uint32_t POW_LDO_AUX_VREF: 1;          // 05       rw  1'b0
        uint32_t rsvd1: 2;                     // 07:06    rw
        uint32_t POW_LDODIG_PC: 1;             // 08       rw  1'b0
        uint32_t POW_LDODIG: 1;                // 09       rw  1'b0
        uint32_t POW_VCORE_PC: 1;              // 10       rw  1'b0
        uint32_t POW_LDO_AUX_HQ: 1;            // 11       rw  1'b0
        uint32_t POW_LDO_RET: 1;               // 12       rw  1'b0
        uint32_t POW_LDO318: 1;                // 13       rw  1'b0
        uint32_t SWRCORE_POW_HV_SW: 1;         // 14       rw  1'b0
        uint32_t POW_LDODIG_VREF: 1;           // 15       rw  1'b0
        uint32_t rsvd2: 1;                     // 16       rw
        uint32_t POW_VAUDIO_DET: 1;            // 17       rw  1'b0
        uint32_t POW_VAUX_DET: 1;              // 18       rw  1'b0
        uint32_t POW_VDDCORE_DET: 1;           // 19       rw  1'b0
        uint32_t POW_HV_DET: 1;                // 20       rw  1'b0
        uint32_t POW_VBAT_DET: 1;              // 21       rw  1'b0
        uint32_t POW_ADP_DET: 1;               // 22       rw  1'b0
        uint32_t rsvd3: 1;                     // 23       rw
        uint32_t POW_VCORE_PC_VG2: 1;          // 24       rw  1'b0
        uint32_t POW_VCORE_PC_VG1: 1;          // 25       rw  1'b0
        uint32_t rsvd4: 6;                     // 31:26    rw
    };
} BTAON_REG_0x0_TYPE;

typedef union _BTAON_REG_0x4_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t POW_M2: 1;                    // 00       rw  1'b0
        uint32_t POW_M1: 1;                    // 01       rw  1'b0
        uint32_t POW_DVDET_M1: 1;              // 02       rw  1'b0
        uint32_t POW_DVDET_M2: 1;              // 03       rw  1'b0
        uint32_t rsvd0: 12;                    // 15:04    rw
        uint32_t reg_POW_32KXTAL: 1;           // 16       rw  1'b1
        uint32_t reg_POW_32KOSC: 1;            // 17       rw  1'b1
        uint32_t rsvd1: 14;                    // 31:18    rw
    };
} BTAON_REG_0x4_TYPE;

typedef union _BTAON_REG_0x8_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t VOUT_TUNE_H: 7;               // 06:00    rw  7'b0010000
        uint32_t rsvd0: 9;                     // 15:07    rw
        uint32_t LDO_DIG_TUNE : 5;             // 20:16    rw  5'b10110
        uint32_t rsvd1: 11;                    // 31:21    rw
    };
} BTAON_REG_0x8_TYPE;

/* 0xc
    07:00    rw
    08       rw  ENLDO=1, ENSWR=1 open SWR, ENLDO=0 ENSWR=1 open SWR                                                                                            1'b0
    09       rw  ENLDO=1, ENSWR=0 open LDO                                                                                                                      1'b0
    10       rw  if FPWMDIG=1 and FPWMCTL<0>=0, SWR only PWM mode                                                                                               1'b0
    11       rw  1: enable aon clock, 0: clock gated                                                                                                            1'b1
    12       rw  00: Normal mode, XTAL_LPMODE=0, TCXO_MODE=0, 01: Low power mode, XTAL_LPMODE=1, TCXO_MODE=0, 10: buffer mode, , XTAL_LPMODE=0, TCXO_MODE=1     1'b0
    13       rw  00: Normal mode, XTAL_LPMODE=0, TCXO_MODE=0, 01: Low power mode, XTAL_LPMODE=1, TCXO_MODE=0, 10: buffer mode, , XTAL_LPMODE=0, TCXO_MODE=1     1'b0
    15:14    rw
    16       rw  Power on the crystal oscillator 1: on, 0: off                                                                                                  1'b0
    17       rw
    18       rw  Power on the 40M ring oscillator 1: on, 0: off                                                                                                 1'b0
    22:19    rw
    26:23    rw  tune PFM VOUT voltage ,PFM_VCORE
    31:27    rw
*/
typedef union _BTAON_REG_0xC_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t rsvd0: 8;                     // 07:00
        uint32_t ENSWR_AONH: 1;                // 08
        uint32_t ENLDO_AONH: 1;                // 09
        uint32_t FPWMDIG_H: 1;                 // 10
        uint32_t AON_GATED_EN: 1;              // 11
        uint32_t XTAL_MODE_0: 1;               // 12
        uint32_t XTAL_MODE_1: 1;               // 13
        uint32_t rsvd1: 2;                     // 15:14
        uint32_t BT_anapar_pow_xtal: 1;        // 16
        uint32_t rsvd2: 1;                     // 17
        uint32_t POW_OSC40: 1;                 // 18
        uint32_t rsvd3: 4;                     // 22:19
        uint32_t VREFPFM_H: 4;                 // 26:23
        uint32_t rsvd4: 5;                     // 31:27
    };
} BTAON_REG_0xC_TYPE;

/* 0x10
    00       rw
    01       rw  ISO from VCORE power domain to other power domain. 1: ISO, not in scan chain for avoid combinational loop                                      1'b1
    02       rw  ISO from AFE_PLL power domain to other power domain. 1: ISO                                                                                    1'b1
    03       rw  1: iso xtal to digital. 0: iso release                                                                                                         1'b1
    14:04    rw
    15       rw  reset the all RET domain flip-flops 0: reset                                                                                                   1'b1
    16       rw  reset the PON power domain flip-flops 0: reset                                                                                                 1'b0
    17       rw  reset the CORE power domain flip-flops 0: reset                                                                                                1'b0
    31:18    rw
*/
typedef union _BTAON_REG_0x10_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t ISO_BT_PON: 1;                // 00
        uint32_t ISO_BT_CORE: 1;               // 01
        uint32_t ISO_AFE_PLL: 1;               // 02
        uint32_t ISO_XTAL: 1;                  // 03
        uint32_t rsvd0: 11;                    // 14:04
        uint32_t BT_RET_RSTB: 1;               // 15
        uint32_t BT_PON_RSTB: 1;               // 16
        uint32_t BT_CORE_RSTB: 1;              // 17
        uint32_t rsvd1: 14;                    // 31:18
    };
} BTAON_REG_0x10_TYPE;

/* 0x14
    0:26     rw
    27       rw  1: enable PLL 0: disable PLL                                                                                                                   1'b0
    28       rw                                                                                                                                                 1'b0
    29       rw                                                                                                                                                 1'b0
    30       rw  110: LDO mode, 001: power cut mode. 000: off                                                                                                   1'b0
    31       rw
*/
typedef union _BTAON_REG_0x14_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t rsvd0: 27;                    // 0:26
        uint32_t POW_PLL: 1;                   // 27
        uint32_t PLL_SW_LDO2PWRCUT: 1;         // 28
        uint32_t PLL_POW_LDO_VREF: 1;          // 29
        uint32_t PLL_POW_LDO_OP: 1;            // 30
        uint32_t rsvd1: 1;                     // 31
    };
} BTAON_REG_0x14_TYPE;

/* 0x18
    0:14     rw
    15       rw  Trigger the MODEM data path retention FF restore                                                                                               1'b0
    16       rw  Trigger the MODEM data path retention FF store                                                                                                 1'b0
    17       rw  Trigger the Platform retention FF restore                                                                                                      1'b0
    18       rw  Trigger the Platform retention FF store                                                                                                        1'b0
    19       rw  Trigger the BLE retention FF restore                                                                                                           1'b0
    20       rw  Trigger the BLE retention FF store                                                                                                             1'b0
    21       rw  Trigger the Bluewiz retention FF restore                                                                                                       1'b0
    22       rw  Trigger the Bluewiz retention FF store                                                                                                         1'b0
    23       rw  Trigger the MODEM retention FF restore                                                                                                         1'b0
    24       rw  Trigger the MODEM retention FF store                                                                                                           1'b0
    25       rw  Trigger the RFC retention FF restore                                                                                                           1'b0
    26       rw  Trigger the RFC retention FF store                                                                                                             1'b0
    31:27    rw
*/
typedef union _BTAON_REG_0x18_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t rsvd0: 15;                    // 0:14
        uint32_t DP_MODEM_RESTORE: 1;          // 15
        uint32_t DP_MODEM_STORE: 1;            // 16
        uint32_t PF_RESTORE: 1;                // 17
        uint32_t PF_STORE: 1;                  // 18
        uint32_t BLE_RESTORE: 1;               // 19
        uint32_t BLE_STORE: 1;                 // 20
        uint32_t BZ_RESTORE: 1;                // 21
        uint32_t BZ_STORE: 1;                  // 22
        uint32_t MODEM_RESTORE: 1;             // 23
        uint32_t MODEM_STORE: 1;               // 24
        uint32_t RFC_RESTORE: 1;               // 25
        uint32_t RFC_STORE: 1;                 // 26
        uint32_t rsvd1: 5;                     // 31:27
    };
} BTAON_REG_0x18_TYPE;

/* 0x1c
    00      rw  enter low power state                                                                                                                           1'b0
    01      rw  re-start the power on sequence.                                                                                                                 1'b0
    02      rw  enter power off mode                                                                                                                            1'b0
    03      r                                                                                                                                                   1'b0
    12:04   rw  DPD_R[8:0] is the register set for PMU                                                                                                          9'b111111101
    13      rw  CLK for DPD_R[8:0].     When DPD_RCK is from low to high, the rising edge is used to latch DPD_R[8:0] for PMU.                                  1'b0
    31:14   r                                                                                                                                                   1'b0
*/

typedef union _BTAON_REG_0x1C_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t FW_enter_lps: 1;              // 00
        uint32_t FW_PON_SEQ_RST_N: 1;          // 01
        uint32_t true_power_off: 1;            // 02
        uint32_t rsvd0: 1;                     // 03
        uint32_t DPD_R_0: 1;                   // 04
        uint32_t DPD_R_1: 1;                   // 05
        uint32_t DPD_R_2: 1;                   // 06
        uint32_t DPD_R_3: 1;                   // 07
        uint32_t DPD_R_4: 1;                   // 08
        uint32_t DPD_R_5: 1;                   // 09
        uint32_t DPD_R_6: 1;                   // 10
        uint32_t DPD_R_7: 1;                   // 11
        uint32_t DPD_R_8: 1;                   // 12
        uint32_t DPD_RCK: 1;                   // 13
        uint32_t rsvd1: 18;                    // 31:14
    };
} BTAON_REG_0x1C_TYPE;

/* BTAON_fast_reg from Chinwen */
#define BTAON_FAST_BOOT                 0x0
#define BTAON_FAST_01                   0x1
#define BTAON_FAST_REBOOT_SW_INFO0      0x2
#define BTAON_FAST_REBOOT_SW_INFO1      0x3
#define BTAON_FAST_BOOT_RETRY_CNT       0x4
#define BTAON_FAST_DEBUG_PASSWORD       0x5
#define BTAON_FAST_REF_RESISTANCE_LSB   0x16
#define BTAON_FAST_REF_RESISTANCE_MSB   0x17
#define BTAON_FAST_1A                   0x1a
#define BTAON_FAST_20                   0x20
#define BTAON_FAST_RG0X_AUXADC          0x22
#define BTAON_FAST_RG1X_AUXADC          0x24
#define BTAON_FAST_RG2X_AUXADC          0x26
#define BTAON_FAST_RG3X_AUXADC          0x28
#define BTAON_FAST_RG4X_AUXADC          0x2a
#define BTAON_FAST_RG5X_AUXADC          0x2c
#define BTAON_FAST_RG6X_AUXADC          0x2e
#define BTAON_FAST_RG0X_MBIAS           0x30
#define BTAON_FAST_RG1X_MBIAS           0x32
#define BTAON_FAST_RG2X_MBIAS           0x34
#define BTAON_FAST_RG3X_MBIAS           0x36
#define BTAON_FAST_RG4X_MBIAS           0x38
#define BTAON_FAST_RG5X_MBIAS           0x3a
#define BTAON_FAST_RG6X_MBIAS           0x3c
#define BTAON_FAST_RG7X_MBIAS           0x3e
#define BTAON_FAST_RG8X_MBIAS           0x40
#define BTAON_FAST_FLAG0X_MBIAS         0x44
#define BTAON_FAST_RG0X_CHG             0x46
#define BTAON_FAST_RG1X_CHG             0x48
#define BTAON_FAST_RG2X_CHG             0x4a
#define BTAON_FAST_RG3X_CHG             0x4c
#define BTAON_FAST_FLAG0X_CHG           0x4e
#define BTAON_FAST_RG0X_SWR_CORE        0x50
#define BTAON_FAST_RG2X_SWR_CORE        0x54
#define BTAON_FAST_RG1X_SWR_CORE        0x56
#define BTAON_FAST_RG3X_SWR_CORE        0x58
#define BTAON_FAST_RG9X_SWR_CORE        0x5a
#define BTAON_FAST_RG4X_SWR_CORE        0x5c
#define BTAON_FAST_RG5X_SWR_CORE        0x5e
#define BTAON_FAST_RG10X_SWR_CORE       0x60
#define BTAON_FAST_RG6X_SWR_CORE        0x62
#define BTAON_FAST_RG7X_SWR_CORE        0x64
#define BTAON_FAST_RG8X_SWR_CORE        0x66
#define BTAON_FAST_FLAG0X_SWR_CORE      0x68
#define BTAON_FAST_FLAG1X_SWR_CORE      0x6a
#define BTAON_FAST_FLAG2X_SWR_CORE      0x6c
#define BTAON_FAST_CKOUT0X_SWR_CORE     0x6e
#define BTAON_FAST_RG0X_SWR_AUDIO       0x70
#define BTAON_FAST_RG1X_SWR_AUDIO       0x78
#define BTAON_FAST_RG5X_SWR_AUDIO       0x7e
#define BTAON_FAST_RG2X_SWR_AUDIO       0x80
#define BTAON_FAST_RG3X_SWR_AUDIO       0x82
#define BTAON_FAST_RG4X_SWR_AUDIO       0x86
#define BTAON_FAST_FLAG0X_SWR_AUDIO     0x88
#define BTAON_FAST_FLAG1X_SWR_AUDIO     0x8a
#define BTAON_FAST_FLAG2X_SWR_AUDIO     0x8c
#define BTAON_FAST_CKOUT0X_SWR_AUDIO    0x8e
#define BTAON_FAST_RG0X_LDO1            0x90
#define BTAON_FAST_RG1X_LDO1            0x92
#define BTAON_FAST_RG2X_LDO1            0x94
#define BTAON_FAST_A4                   0xa4
#define BTAON_FAST_RG3X_LDO2            0xa8
#define BTAON_FAST_BT_ANAPAR_LDO_ADDA   0xaa
#define BTAON_FAST_AC                   0xac
#define BTAON_FAST_AE                   0xae
#define BTAON_FAST_B0                   0xb0
#define BTAON_FAST_B2                   0xb2
#define BTAON_FAST_B4                   0xb4
#define BTAON_FAST_B6                   0xb6
#define BTAON_FAST_B8                   0xb8
#define BTAON_FAST_BA                   0xba
#define BTAON_FAST_BC                   0xbc
#define BTAON_FAST_BE                   0xbe
#define BTAON_FAST_C0                   0xc0
#define BTAON_FAST_C2                   0xc2
#define BTAON_FAST_D0                   0xd0
#define BTAON_FAST_D2                   0xd2
#define BTAON_FAST_D4                   0xd4
#define BTAON_FAST_D6                   0xd6
#define BTAON_FAST_D8                   0xd8
#define BTAON_FAST_DA                   0xda
#define BTAON_FAST_DC                   0xdc
#define BTAON_FAST_DE                   0xde
#define BTAON_FAST_E0                   0xe0
#define BTAON_FAST_E2                   0xe2
#define BTAON_FAST_E4                   0xe4
#define BTAON_FAST_E6                   0xe6
#define BTAON_FAST_E8                   0xe8
#define BTAON_FAST_F2                   0xf2
#define BTAON_FAST_F4                   0xf4
#define BTAON_FAST_F6                   0xf6
#define BTAON_FAST_F8                   0xf8
#define BTAON_FAST_FA                   0xfa
#define BTAON_FAST_FC                   0xfc
#define BTAON_FAST_FE                   0xfe
#define BTAON_FAST_100                  0x100
#define BTAON_FAST_10A                  0x10a
#define BTAON_FAST_112                  0x112
#define BTAON_FAST_120                  0x120
#define BTAON_FAST_122                  0x122
#define BTAON_FAST_BT_PAD_SPI_FLASH     0x124
#define BTAON_FAST_BT_PWDPAD            0x12a
#define BTAON_FAST_WK_STATUS            0x12c
#define BTAON_FAST_12E                  0x12e
#define BTAON_FAST_130                  0x130
#define BTAON_FAST_132                  0x132
#define BTAON_FAST_13A                  0x13a
#define BTAON_FAST_13C                  0x13c
#define BTAON_FAST_140                  0x140
#define BTAON_FAST_142                  0x142
#define BTAON_FAST_154                  0x154
#define BTAON_FAST_156                  0x156
#define BTAON_FAST_158                  0x158
#define BTAON_FAST_15E                  0x15e
#define BTAON_FAST_160                  0x160
#define BTAON_FAST_162                  0x162
#define BTAON_FAST_164                  0x164
#define BTAON_FAST_RG0X_LDODIG          0x168
#define BTAON_FAST_172                  0x172
#define BTAON_FAST_174                  0x174
#define BTAON_FAST_176                  0x176
#define BTAON_FAST_178                  0x178
#define BTAON_FAST_17E                  0x17e
#define BTAON_FAST_RTC_AON_WDT          0x190

/* Bit definition of AON FAST register */
/* 0x00 (BOOT) */
typedef union _BTAON_FAST_BOOT_TYPE
{
    uint8_t d8;
    struct
    {
        uint8_t bootup_before:              1;  /* bit[0]: 0: first boot up                     */
        uint8_t need_restore:               1;  /* bit[1]: need restore flow                    */
        uint8_t rsvd1:                      2;  /* bit[3:2]: reserved */
        uint8_t BOOT_STATUS:                4;  /* bit[7:4]: bit mask to see boot flow          */
    };
} BTAON_FAST_BOOT_TYPE;

/* 0x02 (BTAON_FAST_REBOOT_SW_INFO0) */
typedef union _BTAON_FAST_REBOOT_SW_INFO0_TYPE
{
    uint8_t d8;
    struct
    {
        uint8_t ota_valid: 1;               /* bit[0]: ota valid */
        uint8_t is_rom_code_patch: 1;       /* bit[1]: is rom code patch ? */
        uint8_t is_fw_trig_wdg_to: 1;       /* bit[2]: does fw trigger watchdog timeout ? */
        uint8_t is_airplane_mode: 1;        /* bit[3]: does h5 link reset ? */
        uint8_t is_send_patch_end_evt: 1;   /* bit[4]: does we send patch end event ? */
        uint8_t ota_mode: 1;                /* bit[5]: ota mode */
        uint8_t is_hci_mode: 1;             /* bit[6]: switch to hci mode? */
        uint8_t is_single_tone_mode: 1;                   /* bit[7]: reserved */
    };
} BTAON_FAST_REBOOT_SW_INFO0_TYPE;

/* 0x04 (BOOT_RETRY_CNT) */
typedef union _BTAON_FAST_BOOT_RETRY_CNT_TYPE
{
    uint8_t d8;
    struct
    {
        uint8_t efuse_invalid_cnt:          4;  /* bit[3:0]: efuse invalid retry fail count */
        uint8_t flash_invalid_cnt:          4;  /* bit[7:4]: flash invalid retry fail count */
    };
} BTAON_FAST_BOOT_RETRY_CNT_TYPE;

/* 0x1a ( RG1X_32K )
    05:00    rw  adjust 32k osc freq                                                                        6'b100000
    06       rw  -                                                                                          1'b0
    07       rw  bootup_before                                                                              1'b0
    10:08    rw  -                                                                                          3'b000
    15:11    w1o -                                                                                          5'b00000
*/
typedef union _BTAON_FAST_RG1X_32K_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t RCAL_5_0: 6;                  // 05:00
        uint16_t rsvd1: 1;                     // 06
        uint16_t bootup_before: 1;             // 07
        uint16_t rsvd3: 3;                     // 11:08
        uint16_t w1o_rsvd5: 5;                 // 15:12
    };
} BTAON_FAST_RG1X_32K_TYPE;

/* 0x1a
    10:00    rw  -
    11       w1o reg_aon_flash_clk_dis                                                                      1'b0
    12       w1o is_efuse_invalid                                                                           1'b0
    13       w1o is_enable_efuse_read_protect                                                               1'b0
    14       w1o is_enable_efuse_write_protect                                                              1'b0
    15       w1o -
*/
typedef union _BTAON_FAST_0x1A_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t rsvd11: 11;                            // 10:00
        uint16_t reg_aon_flash_clk_dis: 1;              // 11
        uint16_t is_efuse_invalid: 1;                   // 12
        uint16_t is_enable_efuse_read_protect: 1;       // 13
        uint16_t is_enable_efuse_write_protect: 1;      // 14
        uint16_t w1o_rsvd: 1;                           // 15
    };
} BTAON_FAST_0x1A_TYPE;

/* 0x20
    01:00    rw  -
    02       w1o reg_aon_dsp_flash_port_dis                                                                 1'b0
    03       w1o reg_aon_hwspi_en_wp                                                                        1'b0
    04       rw  reg_aon_hwspi_en                                                                           1'b0
    05       w1o reg_aon_debug_port_wp                                                                      1'b0
    06       rw  reg_aon_dbg_port                                                                           1'b0
    07       w1o reg_aon_dbg_boot_dis                                                                       1'b0
    08       w1o is_disable_hci_ram_patch                                                                   1'b0
    09       w1o is_disable_hci_flash_access                                                                1'b0
    10       w1o is_disable_hci_system_access                                                               1'b0
    11       w1o is_disable_hci_efuse_access                                                                1'b0
    12       w1o is_disable_hci_bt_test                                                                     1'b0
    13       w1o is_disable_usb_function                                                                    1'b0
    14       w1o is_disable_sdio_function                                                                   1'b0
    15       w1o is_debug_password_invalid                                                                  1'b0
*/
typedef union _BTAON_FAST_0x20_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t rsvd2: 2;                              // 01:00
        uint16_t reg_aon_dsp_flash_port_dis: 1;         // 02
        uint16_t reg_aon_hwspi_en_wp: 1;                // 03
        uint16_t reg_aon_hwspi_en: 1;                   // 04
        uint16_t reg_aon_debug_port_wp: 1;              // 05
        uint16_t reg_aon_dbg_port: 1;                   // 06
        uint16_t reg_aon_dbg_boot_dis: 1;               // 07
        uint16_t is_disable_hci_ram_patch: 1;           // 08
        uint16_t is_disable_hci_flash_access: 1;        // 09
        uint16_t is_disable_hci_system_access: 1;       // 10
        uint16_t is_disable_hci_efuse_access: 1;        // 11
        uint16_t is_disable_hci_bt_test: 1;             // 12
        uint16_t is_disable_usb_function: 1;            // 13
        uint16_t is_disable_sdio_function: 1;           // 14
        uint16_t is_debug_password_invalid: 1;          // 15
    };
} BTAON_FAST_0x20_TYPE;

/* 0x22 ( RG0X_AUXADC )
    00       rw  pow adc                                                                                    1'b0
    01       rw  pow adc refgen                                                                             1'b0
    02       rw  1: clk from external                                                                       1'b0
    03       rw  clk delay                                                                                  1'b0
    05:04    rw  00:0.9V 01:0.95V 10:1V 11:1.05V                                                            2'b10
    06       rw  1: rever ckdata                                                                            1'b0
    08:07    rw  meta detect time 00: short                                                                 2'b00
    09       rw  1: enable meta detect                                                                      1'b0
    15:10    rw  -                                                                                          6'b000000
*/
typedef union _BTAON_FAST_RG0X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_pow: 1;                // 00
        uint16_t AUXADC_pow_ref: 1;            // 01
        uint16_t AUXADC_reg_ck: 1;             // 02
        uint16_t AUXADC_reg_delay: 1;          // 03
        uint16_t AUXADC_reg_vref_sel: 2;       // 05:04
        uint16_t AUXADC_reg_revck: 1;          // 06
        uint16_t AUXADC_reg_cmpdec: 2;         // 08:07
        uint16_t AUXADC_reg_meta: 1;           // 09
        uint16_t AUXADC_Reserved0: 6;          // 15:10
    };
} BTAON_FAST_RG0X_AUXADC_TYPE;

/* 0x24 ( RG1X_AUXADC )
    07:00    rw  input bypass resistor                                                                      8'b00000000
    10:08    rw  ldo voltage 000:0.9V 001:0.95V 010:1.0V 011:1.05V 100:1.1V 101:1.15V 110:1.2V 111:1.25V    3'b100
    11       rw                                                                                             1'b0
    12       rw  1:sel ldo 0:sel power cut                                                                  1'b0
    13       rw  1: double OP current                                                                       1'b0
    15:14    rw  -                                                                                          2'b00
*/
typedef union _BTAON_FAST_RG1X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_bypassen: 8;           // 07:00
        uint16_t AUXADC_LDO_SW: 3;             // 10:08
        uint16_t AUXADC_LDO_VPULSE: 1;         // 11
        uint16_t AUXADC_SEL_LDO: 1;            // 12
        uint16_t AUXADC_DOUBLE_OP_I: 1;        // 13
        uint16_t AUXADC_Reserved1_2: 2;        // 15:14
    };
} BTAON_FAST_RG1X_AUXADC_TYPE;

/* 0x26 ( RG2X_AUXADC:output )
    00       rw  1: enable 5V power cut                                                                     1'b0
    02:01    rw  ldo voltage 00:1.62V 01:1.71V 10:1.8V 11:1.89V                                             2'b01
    03       rw  1: enable 1V power cut                                                                     1'b0
    04       r   pow low power cmp                                                                          1'b1
    05       r   cmp output edge                                                                            1'b0
    08:06    r   000: ch1 111::ch8                                                                          3'b000
    09       rw                                                                                             1'b0
    15:10    rw  -                                                                                          6'b000000
*/
typedef union _BTAON_FAST_RG2X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_en_pc_5V: 1;           // 00
        uint16_t AUXADC_ldo_sel: 2;            // 02:01
        uint16_t AUXADC_en_pc_1V: 1;           // 03
        uint16_t AUXADC_POW_SD: 1;             // 04
        uint16_t AUXADC_SD_POSEDGE: 1;         // 05
        uint16_t AUXADC_ch_num_sd: 3;          // 08:06
        uint16_t AUXADC_PWDPAD1: 1;            // 09
        uint16_t AUXADC_Reserved2: 6;          // 15:10
    };
} BTAON_FAST_RG2X_AUXADC_TYPE;

/* 0x28 ( RG3X_AUXADC:output )
    07:00    r   E[07:00]                                                                                   8'b00000000
    15:08    r   E2[07:00]                                                                                  8'b11111111
*/
typedef union _BTAON_FAST_RG3X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_E: 8;                  // 07:00
        uint16_t AUXADC_E2: 8;                 // 15:08
    };
} BTAON_FAST_RG3X_AUXADC_TYPE;

/* 0x2a ( RG4X_AUXADC:output )
    07:00    r   PD[07:00]                                                                                  8'b00000000
    15:08    r   PU[07:00]                                                                                  8'b00000000
*/
typedef union _BTAON_FAST_RG4X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_PD: 8;                 // 07:00
        uint16_t AUXADC_PU: 8;                 // 15:08
    };
} BTAON_FAST_RG4X_AUXADC_TYPE;

/* 0x2c ( RG5X_AUXADC:output )
    07:00    r                                                                                              8'b00000000
    15:08    r                                                                                              8'b00000000
*/
typedef union _BTAON_FAST_RG5X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_SHDN: 8;               // 07:00
        uint16_t AUXADC_SMT: 8;                // 15:08
    };
} BTAON_FAST_RG5X_AUXADC_TYPE;

/* 0x2e ( RG6X_AUXADC:output )
     r   I[7:0]                                                                                             8'b00000000
     r   -                                                                                                  8'b00000000
*/
typedef union _BTAON_FAST_RG6X_AUXADC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AUXADC_I: 8;                  // 07:00
        uint16_t AUXADC_PUPDC: 8;              // 15:08
    };
} BTAON_FAST_RG6X_AUXADC_TYPE;

/* 0x30 ( RG0X_MBIAS )
    00       r   1: enable LDO_PA                                                                           1'b0
    01       rw  1: connect LDO733LQ and LDO533HQ                                                           1'b0
                 0: disconnect LDO733LQ and LDO533HQ
                 enable this power cut after LDO533HQ is enable
    02       r   1: enable LDO533_HQ, default 3.3V                                                          1'b0
    03       r   1: enable VREF of LDO533_HQ and LDOPA                                                      1'b0
    04       r   1: enable BIAS to supply IB for SWR/CHG/LDO                                                1'b0
    06:05    rw  00: ADPSO slowest 11: ADPSO fastest                                                        2'b00
    07       rw  control HV33 Power Cut PAD  (VG2 is larger Power Cut PMOS)                                 1'b1
    08       rw  control HV33 Power Cut PAD  (VG1 is smaller Power Cut PMOS)                                1'b1
    09       rw  control HV33 Power Cut PAD                                                                 1'b1
    10       rw  1: enable hysteresis of DPD Detetor                                                        1'b1
    11       rw  register for codec                                                                         1'b0
    12       r   1: enable POWERCUT PMOS                                                                    1'b0
                 0: enable POWERCUT NMOS
    13       r   1: supply 1.8V for RF (HV18, loading < 100uA)                                              1'b0
    14       rw  1: enable AON 32kHz clock                                                                  1'b1
    15       r   1: enable BIAS to supply IB for USB/PLL/CODEC/AUXADC                                       1'b1
*/
typedef union _BTAON_FAST_RG0X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POW_LDO_PA: 1;                // 00
        uint16_t PCUT_LDO733_LQ: 1;            // 01
        uint16_t POW_LDO533_HQ: 1;             // 02
        uint16_t POW_LDO533_HQ_VREF: 1;        // 03
        uint16_t POW_BIAS_500nA: 1;            // 04
        uint16_t ADPSS: 2;                     // 06:05
        uint16_t PCUT_MBIAS_VG2_33: 1;         // 07
        uint16_t PCUT_MBIAS_VG1_33: 1;         // 08
        uint16_t PCUT_MBIAS_ESDCTRL: 1;        // 09
        uint16_t EN_DPD_COMP_HYS: 1;           // 10
        uint16_t REG_FOR_CODEC: 1;             // 11
        uint16_t EN_EFUSE_POWERCUT: 1;         // 12
        uint16_t POW_LDO318: 1;                // 13
        uint16_t EN_CLK_32K_AON: 1;            // 14
        uint16_t POW_BIAS_IP: 1;               // 15
    };
} BTAON_FAST_RG0X_MBIAS_TYPE;

/* 0x32 ( RG1X_MBIAS )
    02:00    rw  set 000 to 111 to make output voltage of LDO_PA_VREF from 0V to 0.5V                       3'b000
    05:03    rw  set 000 to 111 to make output voltage of LDO_HV33_HQ_VREF from 0V to 0.5V                  3'b000
    09:06    rw  AUXADC REF 800mV:50mV:50mV def<1000>=450mV                                                 4'b1000
    15:10    rw  output voltage of LDO733_LQ (default = 3.0V (smaller than LDO533_HQ))                      6'b101101
*/
typedef union _BTAON_FAST_RG1X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t VR_SEL_LDO_PA: 3;             // 02:00
        uint16_t VR_SEL_LDO533_HQ: 3;          // 05:03
        uint16_t VR_SEL_AUXADC: 4;             // 09:06
        uint16_t LDO733_LQ_TUNE: 6;            // 15:10
    };
} BTAON_FAST_RG1X_MBIAS_TYPE;

/* 0x34 ( RG2X_MBIAS )
    04:00    r   DPD R0-R4 DFF is reset by DVDD_DET                                                         5'b11101
    08:05    r   DPD R8-R5 DFF is reset by POR733DPD&HW_RST_N                                               4'b0111
    09       r   CK signal of DPD R0-R3 DFF                                                                 1'b0
                 set from 0 to 1 to change DPD_R0-R3, set from 1 to 0 to enable lock
    11:10    rw  output voltage of LDO_AUX_LQ VREF                                                          2'b10
    14:12    rw  LPBG VREF 1.2V fine tune                                                                   3'b100
    15       rw  enable HV33 domain DET (Note: useless currently)                                           1'b0
*/
typedef union _BTAON_FAST_RG2X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t DPD_R_0: 1;                   // 00
        uint16_t DPD_R_1: 1;                   // 01
        uint16_t DPD_R_2: 1;                   // 02
        uint16_t DPD_R_3: 1;                   // 03
        uint16_t DPD_R_4: 1;                   // 04
        uint16_t DPD_R_5: 1;                   // 05
        uint16_t DPD_R_6: 1;                   // 06
        uint16_t DPD_R_7: 1;                   // 07
        uint16_t DPD_R_8: 1;                   // 08
        uint16_t DPD_RCK: 1;                   // 09
        uint16_t VR_SEL_LDOAUXLQ: 2;           // 11:10
        uint16_t BG_VREF_tune: 3;              // 14:12
        uint16_t POE_DET: 1;                   // 15
    };
} BTAON_FAST_RG2X_MBIAS_TYPE;

/* 0x36 ( RG3X_MBIAS )
    01:00    rw  as defined in POWER_DET sheet                                                              2'b10
    03:02    rw  as defined in POWER_DET sheet                                                              2'b10
    05:04    rw  as defined in POWER_DET sheet                                                              2'b10
    07:06    rw  as defined in POWER_DET sheet                                                              2'b10
    09:08    rw  as defined in POWER_DET sheet                                                              2'b10
    11:10    rw  as defined in POWER_DET sheet                                                              2'b10
    13:12    rw  as defined in POWER_DET sheet                                                              2'b10
    15:14    rw  as defined in POWER_DET sheet                                                              2'b10
*/
typedef union _BTAON_FAST_RG3X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t SEL_VAUX_VR_L: 2;             // 01:00
        uint16_t SEL_VAUX_VR_H: 2;             // 03:02
        uint16_t SEL_HV_VR_L: 2;               // 05:04
        uint16_t SEL_HV_VR_H: 2;               // 07:06
        uint16_t SEL_VBAT_VR_L: 2;             // 09:08
        uint16_t SEL_VBAT_VR_H: 2;             // 11:10
        uint16_t SEL_ADPIN_VR_L: 2;            // 13:12
        uint16_t SEL_ADPIN_VR_H: 2;            // 15:14
    };
} BTAON_FAST_RG3X_MBIAS_TYPE;

/* 0x38 ( RG4X_MBIAS )
    01:00    rw  as defined in POWER_DET sheet                                                              2'b10
    03:02    rw  as defined in POWER_DET sheet                                                              2'b10
    05:04    rw  as defined in POWER_DET sheet                                                              2'b10
    07:06    rw  as defined in POWER_DET sheet                                                              2'b10
    09:08    rw  as defined in POWER_DET sheet                                                              2'b10
    11:10    rw  as defined in POWER_DET sheet                                                              2'b10
    13:12    rw  as defined in POWER_DET sheet                                                              2'b10
    15:14    rw  as defined in POWER_DET sheet                                                              2'b10
*/
typedef union _BTAON_FAST_RG4X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t SEL_VAUDIO_VR_L: 2;           // 01:00
        uint16_t SEL_VAUDIO_VR_H: 2;           // 03:02
        uint16_t SEL_VDDCORE_VR_L: 2;          // 05:04
        uint16_t SEL_VDDCORE_VR_H: 2;          // 07:06
        uint16_t SEL_DVDD_VR_L: 2;             // 09:08
        uint16_t SEL_DVDD_VR_H: 2;             // 11:10
        uint16_t SEL_HV33_VR_L: 2;             // 13:12
        uint16_t SEL_HV33_VR_H: 2;             // 15:14
    };
} BTAON_FAST_RG4X_MBIAS_TYPE;

/* 0x3a ( RG5X_MBIAS )
    00       rw  0: enable LDO_PA softstart of PowerMOS                                                     1'b0
                 1: disable LDO_PA softstart of PowerMOS
    01       rw  0: enable LDO533_HQ softstart of PowerMOS                                                  1'b0
                 1: disable LDO533_HQ softstart of PowerMOS
    02       rw  1: enable dummy load, 0: disable dummy load                                                1'b0
    03       rw  1: enable dummy load, 0: disable dummy load                                                1'b0
    05:04    rw  output voltage of LDO_RET VREF                                                             2'b10
    06       rw  0: enable dummy load, 1: disable dummy load (enable dummy load by default, set 1 for DLPS) 1'b0
    07       rw  0: enable dummy load, 1: disable dummy load (disable dummy load by default for DPD)        1'b1
    08       rw  0: enable dummy load, 1: disable dummy load (enable dummy load by default, set 1 for DLPS) 1'b0
    09       rw  0: enable dummy load, 1: disable dummy load (enable dummy load by default, set 1 for DLPS) 1'b0
    10       r                                                                                              1'b0
    11       r                                                                                              1'b0
    12       r                                                                                              1'b0
    13       r                                                                                              1'b0
    14       r                                                                                              1'b0
    15       r                                                                                              1'b0
*/
typedef union _BTAON_FAST_RG5X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ENB_LDO_PA_SS: 1;             // 00
        uint16_t ENB_LDO533_HQ_SS: 1;          // 01
        uint16_t EN_LDO_PA_DL: 1;              // 02
        uint16_t EN_LDO533_HQ_DL: 1;           // 03
        uint16_t VR_SEL_LDORET: 2;             // 05:04
        uint16_t ENB_LDO733_LQ_DL: 1;          // 06
        uint16_t ENB_LDO733DPD_DL: 1;          // 07
        uint16_t ENB_LDO318_DL: 1;             // 08
        uint16_t ENB_LDO311_DL: 1;             // 09
        uint16_t POW_VAUDIO_DET: 1;            // 10
        uint16_t POW_VDDCORE_DET: 1;           // 11
        uint16_t POW_VAUX_DET: 1;              // 12
        uint16_t POW_HV_DET: 1;                // 13
        uint16_t POW_VBAT_DET: 1;              // 14
        uint16_t POW_ADP_DET: 1;               // 15
    };
} BTAON_FAST_RG5X_MBIAS_TYPE;

/* 0x3c ( RG6X_MBIAS )
    02:00    rw  protected by lock                                                                          3'b110
    06:03    rw  output voltage of LDO318 (default 1.8V)                                                    4'b0100
    11:07    rw  output voltage of LDO311 (default 1.1V)                                                    5'b10000
    13:12    rw  as defined in POWER_DET sheet                                                              2'b10
    15:14    rw  as defined in POWER_DET sheet                                                              2'b10
*/
typedef union _BTAON_FAST_RG6X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LDO733DPD_TUNE: 3;            // 02:00
        uint16_t LDO318_TUNE: 4;               // 06:03
        uint16_t LDO311_TUNE: 5;               // 11:07
        uint16_t SEL_LDO733_LQ_2D5_VR_L: 2;    // 13:12
        uint16_t SEL_LDO733_LQ_2D5_VR_H: 2;    // 15:14
    };
} BTAON_FAST_RG6X_MBIAS_TYPE;

/* 0x3e ( RG7X_MBIAS )
    05:00    rw  output voltage of LDO_PA (default 3.3V)                                                    6'b110100
    07:06    rw  00:10uA, 01:20uA, 10:40uA, 11:80uA                                                         2'b10
    09:08    rw  00:10uA, 01:20uA, 10:40uA, 11:80uA                                                         2'b10
    15:10    r   output voltage of LDO533_HQ (default 3.3V)                                                 6'b110100
*/
typedef union _BTAON_FAST_RG7X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LDO_PA_TUNE: 6;               // 05:00
        uint16_t LDO_PA_DL: 2;                 // 07:06
        uint16_t LDO533_HQ_DL: 2;              // 09:08
        uint16_t LDO533_HQ_TUNE: 6;            // 15:10
    };
} BTAON_FAST_RG7X_MBIAS_TYPE;

/* 0x40 ( RG8X_MBIAS )
    02:00    rw  adjust thermal curve of BG                                                                 3'b100
    05:03    rw  adjust thermal curve of BG                                                                 3'b100
    07:06    rw  adjust voltage divider resistance of ADPIN                                                 2'b00
    09:08    rw  adjust MFB R                                                                               2'b11
    12:10    rw  adjust DPD ADPIN DET Vth                                                                   3'b010
    15:13    rw  adjust DPD ADPIN DET Vth                                                                   3'b010
*/
typedef union _BTAON_FAST_RG8X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t BG_R2_tune: 3;                // 02:00
        uint16_t BG_R1_tune: 3;                // 05:03
        uint16_t DPD_ADPIN_DR_L: 2;            // 07:06
        uint16_t DPD_TUNE_MFB: 2;              // 09:08
        uint16_t DPD_ADPIN_DET_L: 3;           // 12:10
        uint16_t DPD_ADPIN_DET_H: 3;           // 15:13
    };
} BTAON_FAST_RG8X_MBIAS_TYPE;

/* 0x44 ( FLAG0X_MBIAS )
    00      r   Adapter Power detect                                                                        1'b0
    01      r   VBAT Power detect                                                                           1'b0
    02      r   HW_RST_N                                                                                    1'b0
    03      r   HV Power on reset(5V Power detect)                                                          1'b0
    04      r   LDO_AUX Power on reset                                                                      1'b0
    05      r   Force power on reset low; PMU weakly pull low for digital                                   1'b0
    06      r   LDO_HV33_LQ 2.5V Power on reset                                                             1'b0
    07      r   LDO_HV33_LQ Power on reset                                                                  1'b0
    08      r   LDO311 Power on reset                                                                       1'b0
    09      r   Bandgap OK                                                                                  1'b0
    10      r   VAUDIO(AVCCDRV) Power detect                                                                1'b0
    11      r   VDDCORE Power detect                                                                        1'b0
    12      r   LDO733DPD Power detect                                                                      1'b0
    13      r   MFB Power detect                                                                            1'b0
    14      r   VBAT Power detect in DPD                                                                    1'b0
    15      r   ADPIN Power detect in DPD                                                                   1'b0
*/
typedef union _BTAON_FAST_FLAG0X_MBIAS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ADP_DET_L: 1;                 // 00
        uint16_t VBAT_DET_L: 1;                // 01
        uint16_t HW_RST_N: 1;                  // 02
        uint16_t HV_DET_L: 1;                  // 03
        uint16_t VAUX_DET_L: 1;                // 04
        uint16_t FPOR_L: 1;                    // 05
        uint16_t LDD733_LQ_2D5_DET_L: 1;       // 06
        uint16_t LDO733_LQ_DET_L: 1;           // 07
        uint16_t DVDD_DET_L: 1;                // 08
        uint16_t BGOK_L: 1;                    // 09
        uint16_t VAUDIO_DET_L: 1;              // 10
        uint16_t VDDCORE_DET_L: 1;             // 11
        uint16_t LDO733DPD_DET_L: 1;           // 12
        uint16_t MFB1_DET_L: 1;                // 13
        uint16_t VBAT_DPD_DET_L: 1;            // 14
        uint16_t ADPIN_DPD_DET_L: 1;           // 15
    };
} BTAON_FAST_FLAG0X_MBIAS_TYPE;

/* 0x46 ( RG0X_CHG )
    00       r   POW M2                                                                                     1'b0
    01       r   POW M1                                                                                     1'b0
    03:02    rw                                                                                             2'b11
    04       rw  OP offset K mode                                                                           1'b0
    09:05    rw  M2 CC current sense offset K code                                                          5'b10000
                 Trim range:~ -19mV to +15mV
    10       r   enable dvdet_M2 (compare HV and VBAT)                                                      1'b0
    11       r   enable dvdet_M1 (compare ADP and VBAT)                                                     1'b0
    12       rw  Current sense offset K mode                                                                1'b0
    13       rw  enable M2 VC disable bias                                                                  1'b1
    14       rw  enable M1 VC disable bias                                                                  1'b1
    15       rw                                                                                             1'b0
*/
typedef union _BTAON_FAST_RG0X_CHG_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POW_M2: 1;                    // 00
        uint16_t POW_M1: 1;                    // 01
        uint16_t CHG_dummy0: 2;                // 03:02
        uint16_t KMODOP: 1;                    // 04
        uint16_t CSOFSKM2: 5;                  // 09:05
        uint16_t POW_DVDET_M2: 1;              // 10
        uint16_t POW_DVDET_M1: 1;              // 11
        uint16_t KMODCS: 1;                    // 12
        uint16_t ENDISBIAS_M2: 1;              // 13
        uint16_t ENDISBIAS_M1: 1;              // 14
        uint16_t CHG_dummy1: 1;                // 15
    };
} BTAON_FAST_RG0X_CHG_TYPE;

/* 0x48 ( RG1X_CHG )
    06:00    rw  M2 CV Gain error K,Trim range: +/-20%, 1lsb=~0.3%                                          7'b1000000
    11:07    rw  M2 CV Select, Vout=3.7+0.025*M2CVSEL, Detail in CHG_table                                  5'b10100
    15:12    rw  M1 CV Select, Vout= 3.72+0.12*M1CVSEL, Detail in CHG_table                                 4'b1011
*/
typedef union _BTAON_FAST_RG1X_CHG_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t M2CVK: 7;                     // 06:00
        uint16_t M2CVSEL: 5;                   // 11:07
        uint16_t M1CVSEL: 4;                   // 15:12
    };
} BTAON_FAST_RG1X_CHG_TYPE;

/* 0x4a ( RG2X_CHG )
    00       rw  1: Enable BAT short detector                                                               1'b0
    01       rw  1: M2 fully on 1K mode                                                                     1'b0
    03:02    rw  M2 discharge CC Select, 00: SD, 01: 50mA, 10: 100mA, 11: 500mA                             2'b11
    07:04    rw  M2 CC Select, Detail in CHG_table                                                          4'b0000
    09:08    rw  M2 CC FB select, 00: SD, 01: 25X, 10: 5X, 11: 1X, Detail in CHG_table                      2'b00
    13:10    rw  M1 CC Select, Detail in CHG_table                                                          4'b0010
    15:14    rw  M1 CC FB select, 00: SD, 01: 25X, 10: 5X, 11: 1X, Detail in CHG_table                      2'b11
*/
typedef union _BTAON_FAST_RG2X_CHG_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ENBATSHORTDET: 1;             // 00
        uint16_t M2FON1K: 1;                   // 01
        uint16_t M2CCDFBSEL: 2;                // 03:02
        uint16_t M2CCSEL: 4;                   // 07:04
        uint16_t M2CCFBSEL: 2;                 // 09:08
        uint16_t M1CCSEL: 4;                   // 13:10
        uint16_t M1CCFBSEL: 2;                 // 15:14
    };
} BTAON_FAST_RG2X_CHG_TYPE;

/* 0x4c ( RG3X_CHG )
    04:00   rw  M2 CC EA Offset K,Trim target: offset < 1mV                                                 5'b10000
    09:05   rw  M2 CV EA Offset K, Trim target: offset < 1mV                                                5'b10000
    15:10   rw  M2 CC Gain error K, Trim range: +/-34%, 1lsb=~1.06%                                         6'b100000
*/
typedef union _BTAON_FAST_RG3X_CHG_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t M2CCOPOFS: 5;                 // 04:00
        uint16_t M2CVOPOFS: 5;                 // 09:05
        uint16_t M2CCK: 6;                     // 15:10
    };
} BTAON_FAST_RG3X_CHG_TYPE;

/* 0x4e ( FLAG0X_CHG )
    02:00    r                                                                                              1'b0
    03       r                                                                                              1'b0
    04       r                                                                                              1'b0
    05       r                                                                                              1'b0
    06       r                                                                                              1'b0
    07       r                                                                                              1'b0
*/
typedef union _BTAON_FAST_FLAG0X_CHG_TYPE
{
    uint8_t d8;
    struct
    {
        uint8_t CHG_dummy: 3;                  // 00
        uint8_t M2CSOPK_FLG: 1;                // 03
        uint8_t M2CCOPK_FLG: 1;                // 04
        uint8_t M2CVOPK_FLG: 1;                // 05
        uint8_t BATSUP: 1;                     // 06
        uint8_t ADPHIBAT: 1;                   // 07
    };
} BTAON_FAST_FLAG0X_CHG_TYPE;

/* 0x78 ( RG1X_SWR_AUDIO )
    00      rw  1: ENABLE HV SW(if EN SWR AUDIO)                                                            1'b0
    04:01   rw  tune PFM VOUT voltage ,PFM_AUDIO                                                            4'b0111
                VR<564>=2.825V to VR<494>=2.48V, VR<10>=50mV/step
                VR<444>=2.225V to VR<374>=1.875V, VR<10>=50mV/step
    07:05   rw  For PWM (no use)                                                                            3'b100
    08      rw  1: PWM_VREF Softstart, 0: PWM_VREF=0.48V                                                    1'b1
    11:09   rw  VR<134>,VR<124>,VR<114>,VR<104>,VR<29>,VR<19>,VR<9>,0"                                      3'b100
    12      rw  0:LX Reset 1:PGATE Reset                                                                    1'b1
    13      rw                                                                                              1'b1
    14      rw                                                                                              1'b1
    15      rw                                                                                              1'b1
*/
typedef union _BTAON_FAST_RG1X_SWR_AUDIO_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POW_HV_SW: 1;                 // 00
        uint16_t VREFPFM_H: 4;                 // 04:01
        uint16_t VREFOCP_H: 3;                 // 07:05
        uint16_t ENVREFSS_H: 1;                // 08
        uint16_t VREFOCP_PFM: 3;               // 11:09
        uint16_t ZCDRSNEWB: 1;                 // 12
        uint16_t ENZCDPFMPS_H: 1;              // 13
        uint16_t ENZCDPFM_H: 1;                // 14
        uint16_t ENZCDPWMPS_H: 1;              // 15
    };
} BTAON_FAST_RG1X_SWR_AUDIO_TYPE;

/* 0x86 ( RG4X_SWR_AUDIO )
    02:00   rw  tune PMOS feedback delay                                                                    3'b100
    09:03   rw  <9>:dummy <8:3>: 000000=3.21V                                                               7'b0101111
                                 000001=3.18V
                                 000010=3.15V
                                 ??
                                 101111=1.8V
                                 ??
                                 111111=1.32V
    10      rw  1: SWR OFF, PWDN Current = 10uA, 0: PWDN Current = 10nA                                     1'b0
    11      rw  1: enable bypass REFOK 0: disable bypass REFOK                                              1'b0
    12      rw  1: enable PFMCOT 0:disable PFMCOT                                                           1'b1
    15:13   rw  off time of pfm                                                                             3'b100
*/
typedef union _BTAON_FAST_RG4X_SWR_AUDIO_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t SWRAUD_SDZP_H: 3;             // 02:00
        uint16_t VOUT_TUNE_H: 7;               // 09:03
        uint16_t EN_POWDN_CURRENT: 1;          // 10
        uint16_t BYPASS_REFOK: 1;              // 11
        uint16_t PFM_COT_EN: 1;                // 12
        uint16_t OFFT_H: 3;                    // 15:13
    };
} BTAON_FAST_RG4X_SWR_AUDIO_TYPE;

/* 0x90 ( RG0X_LDO1 )
    00       rw  1: power on LDO_AUX_HQ, 0: power off LDO_AUX_HQ                                            1'b0
    01       rw  1: power on LDO_AUX_HQ VREF, 0: power off LDO_AUX_HQ VREF                                  1'b0
    04:02    rw  REF = 62.5mV * REG<2:0> (REG<2:0> = 111, REF = 500mV)                                      3'b000
    05       rw  1: enable dummy load, 0: disable dummy load                                                1'b0
    06       rw  0: enable LDO_AUX_HQ softstart of POWER MOS softstart                                      1'b0
                 1: disable LDO_AUX_HQ softstart of POWER MOS softstart
    12:07    rw  output voltage of AUX_HQ (default 1.8V)                                                    6'b001110
    13       rw  1: enable LDO_AUX_LQ driving                                                               1'b0
    14       rw  1: enable LDO_AUDIO_LQ driving                                                             1'b0
    15       rw                                                                                             1'b0
*/
typedef union _BTAON_FAST_RG0X_LDO1_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POW_LDO_AUX_HQ: 1;            // 00
        uint16_t POW_LDO_AUX_HQ_VREF: 1;       // 01
        uint16_t VR_SEL_LDO_AUX_HQ: 3;         // 04:02
        uint16_t EN_LDO_AUX_HQ_DL: 1;          // 05
        uint16_t ENB_LDO_AUX_HQ_SS: 1;         // 06
        uint16_t LDO_AUX_HQ_TUNE: 6;           // 12:07
        uint16_t EN_LDO_AUX_LQ_DRV: 1;         // 13
        uint16_t EN_LDO_AUDIO_LQ_DRV: 1;       // 14
        uint16_t LDO1_dummy: 1;                // 15
    };
} BTAON_FAST_RG0X_LDO1_TYPE;

/* 0x92 ( RG1X_LDO1 )
    00       rw  1: power on LDO_AUX_LQ, 0: power off LDO_AUX_LQ                                            1'b0
    01       rw  0: enable dummy load, 1: disable dummy load (enable dummy load by default, set 1 for DLPS) 1'b0
    07:02    rw  output voltage of AUX_LQ (default 1.7V)                                                    6'b001100
    08       rw  1: power on LDO_AUDIO_LQ, 0: power off LDO_AUDIO_LQ                                        1'b0
    09       rw  0: enable dummy load, 1: disable dummy load (enable dummy load by default, set 1 for DLPS) 1'b0
    10       rw                                                                                             1'b0
    15:11    rw  output voltage of AUDIO_LQ (default 1.8V)                                                  5'b00100
*/
typedef union _BTAON_FAST_RG1X_LDO1_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POW_LDO_AUX_LQ: 1;            // 00
        uint16_t ENB_LDO_AUX_LQ_DL: 1;         // 01
        uint16_t LDO_AUX_LQ_TUNE: 6;           // 07:02
        uint16_t POW_LDO_AUDIO_LQ: 1;          // 08
        uint16_t ENB_LDO_AUDIO_LQ_DL: 1;       // 09
        uint16_t LDO1_dummy: 1;                // 10
        uint16_t LDO_AUDIO_LQ_TUNE: 5;         // 15:11
    };
} BTAON_FAST_RG1X_LDO1_TYPE;

/* 0x94 ( RG2X_LDO1 )
    00       rw  0: VOUT_AVCC_DRV floating, 1: VOUT_AVCC_DRV = VOUT_AUDIO_LQ                                1'b0
    01       rw  0: VOUT_AVCC floating, 1: VOUT_AVCC = VOUT_AUDIO_LQ                                        1'b0
    03:02    rw  00:10uA, 01:20uA, 10:40uA, 11:80uA                                                         2'b00
    06:04    rw                                                                                             3'b000
    07       rw  control AUX Power Cut PAD  (VG2 is larger Power Cut PMOS)                                  1'b1
    08       rw  control AUX Power Cut PAD  (VG1 is smaller Power Cut PMOS)                                 1'b1
    09       rw  control AUX Power Cut PAD                                                                  1'b1
    15:10    rw                                                                                             6'b000000
*/
typedef union _BTAON_FAST_RG2X_LDO1_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t VOUT_AVCC_DRV: 1;             // 00
        uint16_t VOUT_AVCC: 1;                 // 01
        uint16_t LDO_AUX_HQ_DL: 2;             // 03:02
        uint16_t LDO1_dummy1: 3;               // 06:04
        uint16_t PCUT_AUX_VG2_33: 1;           // 07
        uint16_t PCUT_AUX_VG1_33: 1;           // 08
        uint16_t PCUT_AUX_ESDCTRL: 1;          // 09
        uint16_t LDO1_dummy0: 6;               // 15:10
    };
} BTAON_FAST_RG2X_LDO1_TYPE;

/* 0xa4
    00       rw  1: power on LDO PA, 0: power off                                                           1'b0
    01       r   1: power on LDO PA, 0: power off                                                           1'b0
    15:02    rw  dummy
*/
typedef union _BTAON_FAST_0xA4_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LDO_POW_LDO_PA_src: 1;        // 00
        uint16_t LDO_POW_LDO_PA: 1;            // 01
        uint16_t LDO_rsvd: 14;                 // 15:02
    };
} BTAON_FAST_0xA4_TYPE;

/* 0xa8
    02:00    rw  0: 16Mhz, 1: 24Mhz, 2: 26Mhz, 3: 32Mhz, 4: 38.4Mhz, 5: 40Mhz                               3'b5
    03       rw  0: from AON, 1: from core domain                                                           1'b0
    09:04    rw  AAC_GM value to XTAL. valid if AAC_SEL = 0, for RF mode                                    6'h1f
    15:10    rw  AAC_GM value to XTAL. valid if AAC_SEL = 0, for IDLE mode                                  6'h1f
*/
typedef union _BTAON_FAST_RG3X_LDO2_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t XTAL_FREQ_SEL: 3;             // 02:00
        uint16_t AAC_SEL: 1;                   // 03
        uint16_t AAC_GM1: 6;                   // 09:04
        uint16_t AAC_GM: 6;                    // 15:10
    };
} BTAON_FAST_RG3X_LDO2_TYPE;

/* 0xaa
    00       rw                                                                                             1'b0
    01       rw                                                                                             1'b0
    02       rw  ADDA_POW_LDO_OP    110: LDO mode, 001: power cut mode. 000: off                            1'b0
    03       rw                                                                                             1'b0
    06:04    rw  000:0.90 001:0.95 010:1.00 011:1.05 100:1.10 101:1.15 110:1.20                             3'b010
    07       rw                                                                                             1'b0
    08       rw                                                                                             1'b0
    09       rw  Power cut USB1.1                                                                           1'b0
    10       rw  Power cut BC1.2                                                                            1'b0
    11       rw  Power on the OP in the LDO                                                                 1'b0
    12       rw  Power on mbias                                                                             1'b0
    13       rw  Power on BC1.2                                                                             1'b0
    14       rw  Isolation APHY to DPHY signal                                                              1'b1
    15       rw  0: from AON, 1: from core domain                                                           1'b0
*/
typedef union _BTAON_FAST_0xAA_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ADDA_SW_LDO2PWRCUT: 1;        // 00
        uint16_t ADDA_POW_LDO_VREF: 1;         // 01
        uint16_t ADDA_POW_LDO_OP: 1;           // 02
        uint16_t ADDA_LDO_VPULSE: 1;           // 03
        uint16_t ADDA_LDO_SW: 3;               // 06:04
        uint16_t ADDA_DOUBLE_OP_I: 1;          // 07
        uint16_t rsvd: 1;                      // 08
        uint16_t U11_POW_BC_V33: 1;            // 09
        uint16_t U11_POW_USB_V33: 1;           // 10
        uint16_t U11_en_pc_mode: 1;            // 11
        uint16_t U11_POW_BG: 1;                // 12
        uint16_t U11_POW_BC_L: 1;              // 13
        uint16_t ISO_UA2USB: 1;                // 14
        uint16_t RCAL_32K_SEL: 1;              // 15
    };
} BTAON_FAST_0xAA_TYPE;

/* 0xac
    00       r                                                                                              1'b0
    01       r                                                                                              1'b0
    02       r   110: LDO mode, 001: power cut mode. 000: off                                               1'b0
    03       rw                                                                                             1'b0
    06:04    rw  000:0.90 001:0.95 010:1.00 011:1.05 100:1.10 101:1.15 110:1.20                             3'b010
    07       rw                                                                                             1'b0
    08       rw  CPU 1st stage CKMUX0 select, 0x0 : clock from xtal, 0x1 : clock from FNPLL                 1'b0
    09       rw  CPU 2ed stage CKMUX1 select, 0x0 : clock from OSC40, 0x1 : clock from CKMUX0 out           1'b0
    15:10    rw
*/
typedef union _BTAON_FAST_0xAC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t PLL_SW_LDO2PWRCUT: 1;         // 00
        uint16_t PLL_POW_LDO_VREF: 1;          // 01
        uint16_t PLL_POW_LDO_OP: 1;            // 02
        uint16_t PLL_LDO_VPULSE: 1;            // 03
        uint16_t PLL_LDO_SW: 3;                // 06:04
        uint16_t PLL_DOUBLE_OP_I: 1;           // 07
        uint16_t cpu_clk_src_sel_0: 1;         // 08
        uint16_t cpu_clk_src_sel_1: 1;         // 09
        uint16_t rsvd: 6;                      // 15:10
    };
} BTAON_FAST_0xAC_TYPE;

/* 0xd0 ( RG11X_MBIAS )
    00      rw  power on seq POW_VAUX_DET reg setting                                                       1'b1
    01      rw  power on seq POW_VDDCORE_DET reg setting                                                    1'b1
    02      rw  power on seq POW_HV_DET reg setting                                                         1'b1
    03      rw  power on seq POW_VBAT_DET reg setting                                                       1'b1
    04      rw  power on seq POW_ADP_DET reg setting                                                        1'b1
    05      rw  power on seq POW_BIAS reg setting                                                           1'b1
    06      rw  power on seq POW_LDO_PA reg setting                                                         1'b0
    07      rw
    08      rw  power on seq POW_LDO533 reg setting                                                         1'b1
    09      rw  power on seq POW_LDO_VREF reg setting                                                       1'b1
    10      rw  power on seq POW_LDODIG_PC reg setting                                                      1'b0
    11      rw  power on seq POW_LDODIG reg setting                                                         1'b1
    12      rw  power on seq POW_VCORE_PC reg setting                                                       1'b0
    13      rw  power on seq POW_LDO_VCORE reg setting                                                      1'b1
    14      rw  power on seq POW_LDO_RET reg setting                                                        1'b0
    15      rw  power on seq POW_LDO318 reg setting                                                         1'b1
*/
typedef union _BTAON_FAST_0xD0_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_POW_VAUX_DET: 1;      // 00
        uint16_t LOP_PON_POW_VDDCORE_DET: 1;   // 01
        uint16_t LOP_PON_POW_HV_DET: 1;        // 02
        uint16_t LOP_PON_POW_VBAT_DET: 1;      // 03
        uint16_t LOP_PON_POW_ADP_DET: 1;       // 04
        uint16_t LOP_PON_POW_BIAS: 1;          // 05
        uint16_t LOP_PON_POW_LDO_PA: 1;        // 06
        uint16_t LOP_rsvd: 1;                  // 07
        uint16_t LOP_PON_POW_LDO533: 1;        // 08
        uint16_t LOP_PON_POW_LDO_VREF: 1;      // 09
        uint16_t LOP_PON_POW_LDODIG_PC: 1;     // 10
        uint16_t LOP_PON_POW_LDODIG: 1;        // 11
        uint16_t LOP_PON_POW_VCORE_PC: 1;      // 12
        uint16_t LOP_PON_POW_LDO_AUX_HQ: 1;    // 13
        uint16_t LOP_PON_POW_LDO_RET: 1;       // 14
        uint16_t LOP_PON_POW_LDO318: 1;        // 15
    };
} BTAON_FAST_0xD0_TYPE;

/* 0xd2 ( RG11X_MBIAS )
    00      rw  power on seq POW_DVDET_M2 reg setting                                                       1'b1
    01      rw  power on seq POW_DVDET_M1 reg setting                                                       1'b1
    02      rw  power on seq POW_SWRCORE_HV_SW reg setting                                                  1'b0
    03      rw  power on seq POW_LDODIG_VREF reg setting                                                    1'b0
    06:04   rw
    07      rw  power on seq SWR mode reg setting                                                           1'b0
    08      rw  power on seq LDO mode reg setting                                                           1'b1
    09      rw  power on seq PFM mode reg setting                                                           1'b0
    10      rw  power on seq POW_POW_32KXTAL reg setting                                                    1'b0
    11      rw  power on seq POW_POW_32KOSC reg setting                                                     1'b1
    12      rw  power on seq POW_LDO_AUX_VREF reg setting                                                   1'b1
    13      rw  power on seq POW_VAUDIO_DET reg setting                                                     1'b1
    14      rw  power on seq POW_M2 reg setting                                                             1'b1
    15      rw  power on seq POW_M1 reg setting                                                             1'b1
*/
typedef union _BTAON_FAST_0xD2_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_POW_DVDET_M2: 1;      // 00
        uint16_t LOP_PON_POW_DVDET_M1: 1;      // 01
        uint16_t LOP_PON_SWRCORE_POW_HV_SW: 1; // 02
        uint16_t LOP_PON_POW_LDODIG_VREF: 1;   // 03
        uint16_t LOP_PON_rsvd: 3;              // 06:04
        uint16_t LOP_PON_ENSWR_AONH: 1;        // 07
        uint16_t LOP_PON_ENLDO_AONH: 1;        // 08
        uint16_t LOP_PON_PFM: 1;               // 09
        uint16_t LOP_PON_POW_32KXTAL: 1;       // 10
        uint16_t LOP_PON_POW_32KOSC: 1;        // 11
        uint16_t LOP_PON_POW_LDO_AUX_VREF: 1;  // 12
        uint16_t LOP_PON_POW_VAUDIO_DET: 1;    // 13
        uint16_t LOP_PON_POW_M2: 1;            // 14
        uint16_t LOP_PON_POW_M1: 1;            // 15
    };
} BTAON_FAST_0xD2_TYPE;

/* 0xd4 ( RG11X_MBIAS )
    00      rw  power off seq POW_VAUX_DET reg setting                                                      1'b1
    01      rw  power off seq POW_VDDCORE_DET reg setting                                                   1'b1
    02      rw  power off seq POW_HV_DET reg setting                                                        1'b1
    03      rw  power off seq POW_VBAT_DET reg setting                                                      1'b1
    04      rw  power off seq POW_ADP_DET reg setting                                                       1'b1
    05      rw  power off seq POW_BIAS reg setting                                                          1'b0
    06      rw  power off seq POW_LDO_PA reg setting                                                        1'b0
    07      rw
    08      rw  power off seq POW_LDO533 reg setting                                                        1'b0
    09      rw  power off seq POW_LDO_VREF reg setting                                                      1'b0
    10      rw  power off seq POW_LDODIG_PC reg setting                                                     1'b0
    11      rw  power off seq POW_LDODIG reg setting                                                        1'b0
    12      rw  power off seq POW_VCORE_PC reg setting                                                      1'b0
    13      rw  power off seq POW_LDO_AUX_HQ reg setting                                                    1'b0
    14      rw  power off seq POW_LDO_RET reg setting                                                       1'b0
    15      rw  power off seq POW_LDO318 reg setting                                                        1'b0
*/
typedef union _BTAON_FAST_0xD4_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_POW_VAUX_DET: 1;      // 00
        uint16_t LOP_POF_POW_VDDCORE_DET: 1;   // 01
        uint16_t LOP_POF_POW_HV_DET: 1;        // 02
        uint16_t LOP_POF_POW_VBAT_DET: 1;      // 03
        uint16_t LOP_POF_POW_ADP_DET: 1;       // 04
        uint16_t LOP_POF_POW_BIAS: 1;          // 05
        uint16_t LOP_POF_POW_LDO_PA: 1;        // 06
        uint16_t LOP_rsvd: 1;                  // 07
        uint16_t LOP_POF_POW_LDO533: 1;        // 08
        uint16_t LOP_POF_POW_LDO_VREF: 1;      // 09
        uint16_t LOP_POF_POW_LDODIG_PC: 1;     // 10
        uint16_t LOP_POF_POW_LDODIG: 1;        // 11
        uint16_t LOP_POF_POW_VCORE_PC: 1;      // 12
        uint16_t LOP_POF_POW_LDO_AUX_HQ: 1;    // 13
        uint16_t LOP_POF_POW_LDO_RET: 1;       // 14
        uint16_t LOP_POF_POW_LDO318: 1;        // 15
    };
} BTAON_FAST_0xD4_TYPE;

/* 0xd6 ( RG11X_MBIAS )
    00      rw  power off seq POW_DVDET_M2 reg setting                                                      1'b0
    01      rw  power off seq POW_DVDET_M1 reg setting                                                      1'b0
    02      rw  power off seq POW_SWRCORE_HV_SW reg setting                                                 1'b0
    03      rw  power off seq POW_LDODIG_VREF reg setting                                                   1'b0
    07      rw  power off seq SWR mode reg setting                                                          1'b0
    08      rw  power off seq LDO mode reg setting                                                          1'b0
    09      rw  power off seq PFM mode reg setting                                                          1'b0
    10      rw  power off seq POW_POW_32KXTAL reg setting                                                   1'b0
    11      rw  power off seq POW_POW_32KOSC reg setting                                                    1'b1
    12      rw  power on seq POW_LDO_AUX_VREF reg setting                                                   1'b1
    13      rw  power off seq POW_VAUDIO_DET reg setting                                                    1'b1
    14      rw  power off seq POW_M2 reg setting                                                            1'b0
    15      rw  power off seq POW_M1 reg setting                                                            1'b0
*/
typedef union _BTAON_FAST_0xD6_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_POW_DVDET_M2: 1;      // 00
        uint16_t LOP_POF_POW_DVDET_M1: 1;      // 01
        uint16_t LOP_POF_SWRCORE_POW_HV_SW: 1; // 02
        uint16_t LOP_POF_POW_LDODIG_VREF: 1;   // 03
        uint16_t LOP_POF_rsvd: 3;              // 06:04
        uint16_t LOP_POF_ENSWR_AONH: 1;        // 07
        uint16_t LOP_POF_ENLDO_AONH: 1;        // 08
        uint16_t LOP_POF_PFM: 1;               // 09
        uint16_t LOP_POF_POW_32KXTAL: 1;       // 10
        uint16_t LOP_POF_POW_32KOSC: 1;        // 11
        uint16_t LOP_POF_POW_LDO_AUX_VREF: 1;  // 12
        uint16_t LOP_POF_POW_VAUDIO_DET: 1;    // 13
        uint16_t LOP_POF_POW_M2: 1;            // 14
        uint16_t LOP_POF_POW_M1: 1;            // 15
    };
} BTAON_FAST_0xD6_TYPE;

/* 0xd8 ( RG11X_MBIAS )
    07:00   rw  M1M2 delay if FW_PMU_SEQ is 1.                         unit : 32k period * 2                8'h30
    15:08   rw  Power on BIAS delay setting, if FW_PMU_SEQ is set 1.                                        8'h30
                all SWR/LDO delay if FW_PMU_SEQ is set 0.              unit : 32k period * 2
*/
typedef union _BTAON_FAST_0xD8_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_M1M2_delay: 8;        // 07:00
        uint16_t LOP_PON_BIAS_delay: 8;        // 15:08
    };
} BTAON_FAST_0xD8_TYPE;

/* 0xda ( RG11X_MBIAS )
    07:00   rw  VDDCORE delay if FW_PMU_SEQ is 1.                      unit : 32k period * 2                8'h30
    15:08   rw  VCORE/VPON delay if FW_PMU_SEQ is 1.                   unit : 32k period * 2                8'h30
*/
typedef union _BTAON_FAST_0xDA_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_SWR_delay: 8;         // 07:00
        uint16_t LOP_PON_LDO_delay: 8;         // 15:08
    };
} BTAON_FAST_0xDA_TYPE;

/* 0xdc ( RG11X_MBIAS )
    06:00   rw  MSB=0, 0.68+(0.01*i) --> default Vout=1 --> 0100000 MSB=1, 1.02+0.015*i                     7'b0010000
    07      rw
    12:08   rw                                                                                              5'b01100
    13
    15:14   rw                                                                                              2'b00
*/
typedef union _BTAON_FAST_0xDC_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_VOUT_TUNE_H: 7;       // 06:00
        uint16_t LOP_rsvd0: 1;                 // 07
        uint16_t LOP_PON_VREFPFM_H: 5;         // 12:08
        uint16_t LOP_rsvd1: 1;                 // 13
        uint16_t LOP_PON_FPWMCTL_H: 2;         // 15:14
    };
} BTAON_FAST_0xDC_TYPE;

/* 0xde ( RG11X_MBIAS )
    04:00   rw  output voltage of LDODIG, default 1.1V                                                      5'b10110
    07:05   rw
    15:08   rw  default: 3 ms                                                                               8'h30
*/
typedef union _BTAON_FAST_0xDE_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_LDO_DIG_TUNE: 5;      // 04:00
        uint16_t rsvd: 3;                      // 07:05
        uint16_t LOP_PON_AUX_HQ_delay: 8;      // 15:08
    };
} BTAON_FAST_0xDE_TYPE;

/* 0xe0 ( RG11X_MBIAS )
    06:00   rw  MSB=0, 0.68+(0.01*i) --> default Vout=1 --> 0100000 MSB=1, 1.02+0.015*i                     7'b0010000
    07      rw
    12:08   rw                                                                                              5'b01100
    13
    15:14   rw                                                                                              2'b00
*/
typedef union _BTAON_FAST_0xE0_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_VOUT_TUNE_H: 7;       // 06:00
        uint16_t LOP_rsvd0: 1;                 // 07
        uint16_t LOP_POF_VREFPFM_H: 5;         // 12:08
        uint16_t LOP_rsvd1: 1;                 // 13
        uint16_t LOP_POF_FPWMCTL_H: 2;         // 15:14
    };
} BTAON_FAST_0xE0_TYPE;

/* 0xe2 ( RG11X_MBIAS )
    04:00   rw  output voltage of LDODIG, default 1.1V                                                      5'b10110
    05      rw  POW_BIAS for USB/PLL/CODEC/AUXADC                                                           1'b1
    15:06   rw
*/
typedef union _BTAON_FAST_0xE2_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_LDO_DIG_TUNE: 5;      // 04:00
        uint16_t LOP_POF_POW_BIAS_IP: 1;       // 05
        uint16_t rsvd: 10;                     // 15:06
    };
} BTAON_FAST_0xE2_TYPE;

/* 0xe4 ( RG11X_MBIAS )
    00      rw  0:gated the AON clock as in low power state.                                                1'b0
    01      rw  1:turn on xtal when power off.                                                              1'b0
    02      rw  1:turn on PLL when power on. 0:turn on osc40 when power on                                  1'b0
    03      rw  PLL power source LDO/PC mode. 0:LDO. 1:power cut mode                                       1'b0
    04      rw  1:bypass PLL and xtal clock directly output.                                                1'b0
    05      rw  bluewiz wakeup BTAON enable.                                                                1'b0
    07:06   rw
    15:08   rw  XTAL or OSC40 settling time                                                                 8'h30
*/
typedef union _BTAON_FAST_0xE4_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_AON_GATED_EN: 1;      // 00
        uint16_t LOP_POF_XTAL: 1;              // 01
        uint16_t LOP_PON_PLL: 1;               // 02
        uint16_t LOP_PON_PLL_TYPE: 1;          // 03
        uint16_t LOP_PON_BP_PLL: 1;            // 04
        uint16_t LOP_LPS_TIMER_WAKEUP: 1;      // 05
        uint16_t rsvd0: 2;                     // 07:06
        uint16_t LOP_PON_XTAL_delay: 8;        // 15:08
    };
} BTAON_FAST_0xE4_TYPE;

/* 0xe6 ( RG11X_MBIAS )
    07:00   rw  PLL settling time.                                                                          8'h30
    15:08   rw
*/
typedef union _BTAON_FAST_0xE6_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_PON_PLL_delay: 8;         // 07:00
        uint16_t rsvd0: 8;                     // 15:08
    };
} BTAON_FAST_0xE6_TYPE;

/* 0xe8 ( RG11X_MBIAS )
    05:00   rw
    07:06   rw  00: Normal mode     XTAL_LPMODE=0, TCXO_MODE=0                                              2'b00
                01: Low power mode  XTAL_LPMODE=1, TCXO_MODE=0
                10: buffer mode     XTAL_LPMODE=0, TCXO_MODE=1
    09:08   rw  00: Normal mode     XTAL_LPMODE=0, TCXO_MODE=0                                              2'b00
                01: Low power mode  XTAL_LPMODE=1, TCXO_MODE=0
                10: buffer mode     XTAL_LPMODE=0, TCXO_MODE=1
    10      rw  1: Retent the MODEM data path Retention FF when power state from Power down to active       1'b0
    11      rw  1: Retent the Platform Retention FF when power state from Power down to active              1'b0
    12      rw  1: Retent the BLE Retention FF when power state from Power down to active                   1'b0
    13      rw  1: Retent the Bluewiz Retention FF when power state from Power down to active               1'b0
    14      rw  1: Retent the MODEM Retention FF when power state from Power down to active                 1'b0
    15      rw  1: Retent the RFC Retention FF when power state from Power down to active                   1'b0
*/
typedef union _BTAON_FAST_0xE8_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t rsvd: 6;                      // 05:00
        uint16_t LOP_POF_XTAL_MODE: 2;         // 07:06
        uint16_t LOP_PON_XTAL_MODE: 2;         // 09:08
        uint16_t LOP_DP_MODEM_RET: 1;          // 10
        uint16_t LOP_PF_RET: 1;                // 11
        uint16_t LOP_BLE_RET: 1;               // 12
        uint16_t LOP_BZ_RET: 1;                // 13
        uint16_t LOP_MODEM_RET: 1;             // 14
        uint16_t LOP_RFC_RET: 1;               // 15
    };
} BTAON_FAST_0xE8_TYPE;

/* 0xf4 ( RG11X_MBIAS )
    00      rw  clock select 32K XTAL for AON                                                               1'b0
    01      rw  clock select 32K from LP XTAL for AON                                                       1'b0
    02      rw  1: enable clock gating as brown detected                                                    1'b0
    03      rw  1: enable core and pon domain reset as brown out detected.                                  1'b0
    04      rw  1: WAIT HV_DET in PON_M1M2 state. 0: only wait timeout in                                   1'b1
    05      rw  1: enable VIO2 detection reset the system.                                                  1'b1
    06      rw  1: enable VIO1 detection reset the system.                                                  1'b1
    07      rw  1: SWR controlled by core domain. 0: SWR controlled by AON.                                 1'b0
                VREFPFM, VREFOCP, OCPRSTSEL, OCPSEL, EHDP, EHDN, MPSZ, MNSZ. SAW_FREQ, VOUT_TUNE_H
    08      rw  disable brown out no wakeup function.                                                       1'b1
                1: disable. 0: as brown out occur, do not wakeup from LPS.
    09      rw  enable PMU sequently 0: turn on all the Power simultaneously                                1'b1
    10      rw
    15:11   rw  debug port selection valid when aon selected to aon and AON_DBG_SEL[4] = 1.                 5'b0
                output to {P1[7:0],AUXADC[7:0]}
*/
typedef union _BTAON_FAST_0xF4_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t SEL_32K_XTAL: 1;              // 00
        uint16_t SEL_32K_LP: 1;                // 01
        uint16_t BRWN_OUT_GATING: 1;           // 02
        uint16_t BRWN_OUT_RESET: 1;            // 03
        uint16_t WAIT_HV_DET: 1;               // 04
        uint16_t VIO2_RST_EN: 1;               // 05
        uint16_t VIO1_RST_EN: 1;               // 06
        uint16_t SWR_BY_CORE: 1;               // 07
        uint16_t LOP_DIS_BR_NWAKE: 1;          // 08
        uint16_t FW_PMU_SEQ: 1;                // 09
        uint16_t rsvd: 1;                      // 10
        uint16_t AON_DBG_SEL: 5;               // 15:11
    };
} BTAON_FAST_0xF4_TYPE;

/* 0x124 ( RG11X_MBIAS )
    02:00   rw
    03      rw  0: shutdown of FLASH PADs.                                                                  1'b0
    04      rw  0: select AON control, 1: core domain control.                                              1'b1
    05      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    06      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    07      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    08      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    09      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    10      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    11      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    12      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    13      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    14      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
    15      rw  0: 150K weakly pull. 1: 15K pull                                                            1'b0
*/
typedef union _BTAON_FAST_BT_PAD_SPI_FLASH_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t PAD_rsvd: 3;                  // 02:00
        uint16_t PAD_SPI_FLASH_SHDN: 1;        // 03
        uint16_t AON_PAD_SPI_FLASH_S: 1;       // 04
        uint16_t PAD_P3_PDPUC_5: 1;            // 05
        uint16_t PAD_P3_PDPUC_4: 1;            // 06
        uint16_t PAD_P4_PDPUC_1: 1;            // 07
        uint16_t PAD_P4_PDPUC_0: 1;            // 08
        uint16_t PAD_P3_PDPUC_3: 1;            // 09
        uint16_t PAD_P3_PDPUC_2: 1;            // 10
        uint16_t PAD_P3_PDPUC_1: 1;            // 11
        uint16_t PAD_P3_PDPUC_0: 1;            // 12
        uint16_t PAD_P2_PDPUC_7: 1;            // 13
        uint16_t PAD_P2_PDPUC_6: 1;            // 14
        uint16_t PAD_P2_PDPUC_5: 1;            // 15
    };
} BTAON_FAST_BT_PAD_SPI_FLASH_TYPE;

/* 0x12a ( RG11X_MBIAS )
    00      rw  for SHDN control of HW_RSTN I/O cell                                                        1'b1
    01      rw  to power down I/O cells for Serial Flash                                                    1'b1
    02      rw  to power down I/O cells at TOP side                                                         1'b0
    03      rw  to power down I/O cells at BOTTOM side                                                      1'b0
    04      rw  to power down I/O cells at LEFT side                                                        1'b0
    05      rw  power down AUXLDO2 power cell at TOP side                                                   1'b0
    06      rw  power down AUXLDO1 power cell at BOTTOM side                                                1'b0
    07      rw  power down PALDO power cell at BOTTOM side                                                  1'b0
    08      rw  power down USB power cell at TOP side                                                       1'b0
    11:09   rw
    12      rw  battery plug in wake up ploarity 0: high level wakeup 1: low level wakeup                   1'b0
    13      rw  battery plug in wake up enable                                                              1'b0
    14      rw  Adaptor plug in wake up ploarity 0: high level wakeup 1: low level wakeup                   1'b0
    15      rw  Adaptor plug in wake up enable                                                              1'b0
*/
typedef union _BTAON_FAST_BT_PWDPAD_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POWPAD1: 1;                   // 00
        uint16_t PWDPAD1_FLASH: 1;             // 01
        uint16_t PWDPAD1_TOP: 1;               // 02
        uint16_t PWDPAD1_BOT: 1;               // 03
        uint16_t PWDPAD1_LEFT: 1;              // 04
        uint16_t PWDPAD1_AUXLDO2: 1;           // 05
        uint16_t PWDPAD1_AUXLDO1: 1;           // 06
        uint16_t PWDPAD1_PALDO: 1;             // 07
        uint16_t PWDPAD1_USB: 1;               // 08
        uint16_t rsvd: 1;                      // 09
        uint16_t MFB_WKPOL: 1;                 // 10
        uint16_t MFB_WKEN: 1;                  // 11
        uint16_t BAT_WKPOL: 1;                 // 12
        uint16_t BAT_WKEN: 1;                  // 13
        uint16_t ADP_WKPOL: 1;                 // 14
        uint16_t ADP_WKEN: 1;                  // 15
    };
} BTAON_FAST_BT_PWDPAD_TYPE;

/* 0x12c
    05:00   rw
    06      rw  1: enable brown detection interrupt.                                                        1'b0
    07      rw  brwon out detection interrupt polarity. 0:low level trigger. 1: high level trigger          1'b1
    08      w1c brown out detection pending interrupt status.                                               1'b1
    09      rw  brown out block wakeup indicator, set as try to wakeup in LPS but in brwon out state        1'b1
    10      r   power on trap                                                                               1'b0
    11      r   bondign option from PAD.                                                                    1'b0
    12      r   the result of GPIO_WKUP_STS & gpio_wkup_int_en which is issued to CPU ISR                   1'b0
    13      r   1: enable gpio wakeup interrupt as GPIO_WKUP_STS = 1                                        1'b0
    14      w1c wakeup status                                                                               1'b1
    15      rw
*/
typedef union _BTAON_FAST_WK_STATUS_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t rsvd0: 6;                     // 05:00
        uint16_t brwn_int_en: 1;               // 06
        uint16_t brwn_int_pol: 1;              // 07
        uint16_t brwn_det_intr: 1;             // 08
        uint16_t wakeup_blk_ind: 1;            // 09
        uint16_t PAD_P2_0_I: 1;                // 10
        uint16_t PAD_BOOT_FROM_FLASH_I: 1;     // 11
        uint16_t gpio_wkup_int: 1;             // 12
        uint16_t gpio_wkup_int_en: 1;          // 13
        uint16_t GPIO_WKUP_STS: 1;             // 14
        uint16_t rsvd1: 1;                     // 15
    };
} BTAON_FAST_WK_STATUS_TYPE;

/* 0x13a ( RG11X_MBIAS )
    00      rw  driving current control of the PAD                                                          1'b0
    01      rw  driving current control of the PAD                                                          1'b0
    02      rw  driving current control of the PAD                                                          1'b0
    03      rw  driving current control of the PAD                                                          1'b0
    04      rw  driving current control of the PAD                                                          1'b0
    05      rw  driving current control of the PAD                                                          1'b0
    06      rw  driving current control of the PAD                                                          1'b0
    07      rw  driving current control of the PAD                                                          1'b0
    08      rw  driving current control of the SPI FLASH PAD                                                1'b0
    09      rw  driving current control of the SPI FLASH PAD                                                1'b0
    10      rw  driving current control of the SPI FLASH PAD                                                1'b0
    11      rw  driving current control of the SPI FLASH PAD                                                1'b0
    12      rw  driving current control of the SPI FLASH PAD                                                1'b0
    13      rw  driving current control of the SPI FLASH PAD                                                1'b0
    14      rw  driving current control of the SPI FLASH PAD                                                1'b0
    15      rw  driving current control of the SPI FLASH PAD                                                1'b0
*/
typedef union _BTAON_FAST_0x13A_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t PAD_P3_E3_5: 1;               // 00
        uint16_t PAD_P3_E3_3: 1;               // 01
        uint16_t PAD_P3_E3_4: 1;               // 02
        uint16_t PAD_P3_E3_2: 1;               // 03
        uint16_t PAD_P3_E2_5: 1;               // 04
        uint16_t PAD_P3_E2_4: 1;               // 05
        uint16_t PAD_P3_E2_3: 1;               // 06
        uint16_t PAD_P3_E2_2: 1;               // 07
        uint16_t PAD_SPI_FLASH_E5N: 1;         // 08
        uint16_t PAD_SPI_FLASH_E4N: 1;         // 09
        uint16_t PAD_SPI_FLASH_E3N: 1;         // 10
        uint16_t PAD_SPI_FLASH_E2N: 1;         // 11
        uint16_t PAD_SPI_FLASH_E5P: 1;         // 12
        uint16_t PAD_SPI_FLASH_E4P: 1;         // 13
        uint16_t PAD_SPI_FLASH_E3P: 1;         // 14
        uint16_t PAD_SPI_FLASH_E2P: 1;         // 15
    };
} BTAON_FAST_0x13A_TYPE;

/* 0x13c ( RG11X_MBIAS )
    02:00   rw  0: AON GPIO, 1: LED0, 2: LED1, 3: LED2, 4: CLK_REQ. valid as PAD_P2_S[0] eq. 0.                     3'b0
    05:03   rw  0: AON GPIO, 1: LED0, 2: LED1, 3: LED2, 4: CLK_REQ, 5: xtal clock. valid as PAD_P2_S[1] eq. 0.      3'b0
    08:06   rw  0: AON GPIO, 1: LED0, 2: LED1, 3: LED2, 4: CLK_REQ, 5: RTC_CLK. valid as PAD_P2_S[2] eq. 0.         3'b0
    11:09   rw  0: AON GPIO, 1: LED0, 2: LED1, 3: LED2. valid as AON_PAD_ADC_S[0] eq 0                              3'b0
    14:12   rw  0: AON GPIO, 1: LED0, 2: LED1, 3: LED2, 4: CLK_REQ. valid as AON_PAD_ADC_S[1] eq 0                  3'b0
    15      rw                                                                                                      1'b0
*/
typedef union _BTAON_FAST_0x13C_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t PAD_P2_0_CFG: 3;              // 02:00
        uint16_t PAD_P2_1_CFG: 3;              // 05:03
        uint16_t PAD_P2_2_CFG: 3;              // 08:06
        uint16_t PAD_ADC_0_CFG: 3;             // 11:09
        uint16_t PAD_ADC_1_CFG: 3;             // 14:12
        uint16_t rsvd0: 1;                     // 15
    };
} BTAON_FAST_0x13C_TYPE;

/* 0x140 ( RG11X_MBIAS )
    00      rw  PAD output enable. valid only if AON_PAD_SPI_FLASH_S = 0                                    1'b0
    01      rw  1: PAD pull down, 0: PAD pull up. vaild if pull enable                                      1'b0
    02      rw  AON PAD output value. valid only if AON_PAD_SPI_FLASH_S = 0 and OE = 1                      1'b0
    03      rw  AON PAD pull enable                                                                         1'b0
    04      rw  AON PAD output value. valid only if AON_PAD_SPI_FLASH_S = 0 and OE = 1                      1'b0
    05      rw  PAD output enable. valid only if AON_PAD_SPI_FLASH_S = 0                                    1'b0
    06      rw  1: PAD pull down, 0: PAD pull up. vaild if pull enable                                      1'b0
    07      rw  AON PAD pull enable                                                                         1'b0
    08      rw  AON PAD output value. valid only if AON_PAD_SPI_FLASH_S = 0 and OE = 1                      1'b0
    09      rw  PAD output enable. valid only if AON_PAD_SPI_FLASH_S = 0                                    1'b0
    10      rw  1: PAD pull down, 0: PAD pull up. vaild if pull enable                                      1'b0
    11      rw  AON PAD pull enable                                                                         1'b0
    12      rw  AON PAD output value. valid only if AON_PAD_SPI_FLASH_S = 0 and OE = 1                      1'b0
    13      rw  PAD output enable. valid only if AON_PAD_SPI_FLASH_S = 0                                    1'b0
    14      rw  1: PAD pull down, 0: PAD pull up. vaild if pull enable                                      1'b0
    15      rw  AON PAD pull enable                                                                         1'b0
*/
typedef union _BTAON_FAST_0x140_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t AON_PAD_SPI_SIO1_O: 1;        // 00
        uint16_t AON_PAD_SPI_SIO1_OE: 1;       // 01
        uint16_t AON_PAD_SPI_SIO1_PD: 1;       // 02
        uint16_t AON_PAD_SPI_SIO1_PU_EN: 1;    // 03
        uint16_t AON_PAD_SPI_SIO0_O: 1;        // 04
        uint16_t AON_PAD_SPI_SIO0_OE: 1;       // 05
        uint16_t AON_PAD_SPI_SIO0_PD: 1;       // 06
        uint16_t AON_PAD_SPI_SIO0_PU_EN: 1;    // 07
        uint16_t AON_PAD_SPI_SCK_O: 1;         // 08
        uint16_t AON_PAD_SPI_SCK_OE: 1;        // 09
        uint16_t AON_PAD_SPI_SCK_PD: 1;        // 10
        uint16_t AON_PAD_SPI_SCK_PU_EN: 1;     // 11
        uint16_t AON_PAD_SPI_CSN_O: 1;         // 12
        uint16_t AON_PAD_SPI_CSN_OE: 1;        // 13
        uint16_t AON_PAD_SPI_CSN_PD: 1;        // 14
        uint16_t AON_PAD_SPI_CSN_PU_EN: 1;     // 15
    };
} BTAON_FAST_0x140_TYPE;

/* 0x142 ( RG11X_MBIAS )
    07:00   rw
    08      rw  AON PAD output value. valid only if AON_PAD_SPI_FLASH_S = 0 and OE = 1                      1'b0
    09      rw  PAD output enable. valid only if AON_PAD_SPI_FLASH_S = 0                                    1'b0
    10      rw  1: PAD pull down, 0: PAD pull up. vaild if pull enable                                      1'b0
    11      rw  AON PAD pull enable                                                                         1'b0
    12      rw  AON PAD output value. valid only if AON_PAD_SPI_FLASH_S = 0 and OE = 1                      1'b0
    13      rw  PAD output enable. valid only if AON_PAD_SPI_FLASH_S = 0                                    1'b0
    14      rw  1: PAD pull down, 0: PAD pull up. vaild if pull enable                                      1'b0
    15      rw  AON PAD pull enable                                                                         1'b0
*/
typedef union _BTAON_FAST_0x142_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t rsvd: 8;                      // 07:00
        uint16_t AON_PAD_SPI_SIO3_O: 1;        // 08
        uint16_t AON_PAD_SPI_SIO3_OE: 1;       // 09
        uint16_t AON_PAD_SPI_SIO3_PD: 1;       // 10
        uint16_t AON_PAD_SPI_SIO3_PU_EN: 1;    // 11
        uint16_t AON_PAD_SPI_SIO2_O: 1;        // 12
        uint16_t AON_PAD_SPI_SIO2_OE: 1;       // 13
        uint16_t AON_PAD_SPI_SIO2_PD: 1;       // 14
        uint16_t AON_PAD_SPI_SIO2_PU_EN: 1;    // 15
    };
} BTAON_FAST_0x142_TYPE;

/* 0x156 ( RG11X_MBIAS )
    05:00   rw  fractional part compensat . LSB : 1/2^13                                                    6'b0
    06      rw  1: LP_XTAL fractional part add lp_xtal_ppm_err, 0: minus lp_xtal_ppm_err                    1'b0
    07      rw                                                                                              1'b0
    15:08   rw  option register reserved for RFC LDO                                                        8'b0
*/
typedef union _BTAON_FAST_0x156_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t lp_xtal_ppm_err: 6;           // 05:00
        uint16_t lp_xtal_ppm_plus: 1;          // 06
        uint16_t rsvd: 1;                      // 07
        uint16_t LDO_RFC_OPT: 8;               // 15:08
    };
} BTAON_FAST_0x156_TYPE;

/* 0x158 ( RG11X_MBIAS )
    00      rw  1: RG2X_CHG[3:2] =2'b01@ST_PON_M1M2                                                         1'b1
                   RG2X_CHG[3:2] =2'b11@ST_PON_LDO533
*/
typedef union _BTAON_FAST_0x158_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t CHG_M2CCDFBSEL_eco_en: 1;              // 06:00
        uint16_t rsvd: 15;                  // 07
    };
} BTAON_FAST_0x158_TYPE;

/* 0x15e
    06:00   rw  the destination retention ldo voltage setting.                                              7'b101101
    15:08   rw  retention register step down delay for each step. unit 32k period (default: 2ms)            8'h40
*/
typedef union _BTAON_FAST_0x15E_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ldoret_tune6: 7;              // 06:00
        uint16_t LOP_rsvd: 1;                  // 07
        uint16_t LOP_POF_LDODIG_delay: 8;      // 15:08
    };
} BTAON_FAST_0x15E_TYPE;

/* 0x160
    01:00   rw  2'b00: 31.25us, 2'b01: 62.5us, 2'b10: 125us, 2'b11: 250us                                   2'b00
    03:02   rw  2'b00: 31.25us, 2'b01: 62.5us, 2'b10: 125us, 2'b11: 250us                                   2'b11
    05:04   rw  2'b00: 31.25us, 2'b01: 62.5us, 2'b10: 125us, 2'b11: 250us                                   2'b11
    07:06   rw  2'b00: 31.25us, 2'b01: 62.5us, 2'b10: 125us, 2'b11: 250us                                   2'b11
    08      rw  1'b1: external XTAL 32.768K for Bluewiz                                                     1'b0
    09      rw  1'b1: internal low power 32K for Bluewiz                                                    1'b0
    10      rw  1'b1: external XTAL 32.768K for RTC                                                         1'b0
    11      rw  1'b1: internal low power 32K for RTC                                                        1'b0
    12      rw  1'b1: pass external XTAL 32.768K through P2_1                                               1'b0
    13      rw                                                                                              1'b0
    14      rw                                                                                              1'b0
    15      rw
*/
typedef union _BTAON_FAST_0x160_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ldopa_step_ctrl: 2;           // 01:00
        uint16_t ldo533_step_ctrl: 2;          // 03:02
        uint16_t ldoaux_step_ctrl: 2;          // 05:04
        uint16_t ldodig_step_ctrl: 2;          // 07:06
        uint16_t SEL_32K_XTAL_bt_systick: 1;   // 08
        uint16_t SEL_32K_LP_bt_systick: 1;     // 09
        uint16_t SEL_32K_XTAL_rtc: 1;          // 10
        uint16_t SEL_32K_LP_rtc: 1;            // 11
        uint16_t en_p21_ext_32k: 1;            // 12
        uint16_t USB_WKPOL: 1;                 // 13
        uint16_t USB_WKEN: 1;                  // 14
        uint16_t rsvd: 1;                      // 15
    };
} BTAON_FAST_0x160_TYPE;

/* 0x168 (RG0X_LDODIG)
    00      r   1: power on LDODIG, power on LDODIG after generate LDODIG_VREF with soft start              1'b0
    01      r   1: enable LDODIG Power Cut mode if POW_LDODIG = 1                                           1'b0
    02      rw                                                                                              1'b0
    07:03   r   output voltage of LDODIG (default 1.1V)                                                     5'b10110
    08      r   1: power on LDODIG_VREF                                                                     1'b0
    09      r   1: power on LDORET                                                                          1'b0
    10      rw  0: enable LDORET dummy load                                                                 1'b0
    15:11   rw  output voltage of LDORET (default 1.1V)                                                     5'b11000
*/
typedef union _BTAON_FAST_RG0X_LDODIG_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t POW_LDODIG: 1;                // 00
        uint16_t POW_LDODIG_PC: 1;             // 01
        uint16_t ENB_LDODIG_FB_LOCAL: 1;       // 02
        uint16_t TUNE_LDODIG_VOUT: 5;          // 07:03
        uint16_t POW_LDODIG_VREF: 1;           // 08
        uint16_t POW_LDORET: 1;                // 09
        uint16_t ENB_LDORET_DL: 1;             // 10
        uint16_t TUNE_LDORET_VOUT: 5;          // 15:11
    };
} BTAON_FAST_RG0X_LDODIG_TYPE;

/* 0x172
    00      rw  1: LDO_PA is controlled by H/W, 0: LDO_PA is controlled by F/W                              1'b1
    01      rw  1: 1MHz as system tick, 0: 32KHz as system tick                                             1'b0
    02      rw  1: to disable the step control of LDO_AUX/LDO533/LDODIG when re_power_on                    1'b0
    03      r                                                                                               1'b0
    04      rw  1: power on LDORET (F/W controlled)                                                         1'b0
    05      rw  1: power on LDO318 (F/W controlled)                                                         1'b0
    06      r                                                                                               1'b0
    07      rw  dummy                                                                                       1'b1
    08      rw  1: soft off is enabled for LDODIG                                                           1'b1
    09      rw  1: soft off is enabled for AUX HQ                                                           1'b1
    10      rw  1: soft off is enabled for LDO PA                                                           1'b1
    11      rw  1: soft off is enabled for LDO533                                                           1'b1
    12      rw  1: soft start is enabled for LDODIG (Note: It cannot be set to 1'b0 currently)              1'b1
    13      rw  1: soft start is enabled for AUX HQ                                                         1'b1
    14      rw  1: soft start is enabled for LDO PA                                                         1'b1
    15      rw  1: soft start is enabled for LDO533 (Note: It cannot be set to 1'b0 currently)              1'b1
*/
typedef union _BTAON_FAST_0x172_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t ldopa_step_ctrl_by_hw: 1;     // 00
        uint16_t r_clk_cpu_tick_sel: 1;        // 01
        uint16_t re_power_on: 1;               // 02
        uint16_t rsvd0: 1;                     // 03
        uint16_t LDODIG_POW_LDORET: 1;         // 04
        uint16_t MBIAS_POW_LDO318: 1;          // 05
        uint16_t rsvd1: 1;                     // 06
        uint16_t ldoret_step_on: 1;            // 07
        uint16_t soft_off_ldodig: 1;           // 08
        uint16_t soft_off_auxhq: 1;            // 09
        uint16_t soft_off_ldopa: 1;            // 10
        uint16_t soft_off_ldo533: 1;           // 11
        uint16_t soft_start_ldodig: 1;         // 12
        uint16_t soft_start_auxhq: 1;          // 13
        uint16_t soft_start_ldopa: 1;          // 14
        uint16_t soft_start_ldo533: 1;         // 15
    };
} BTAON_FAST_0x172_TYPE;

/* 0x174
    07:00   rw
    15:08   rw  default: 3 ms                                                                               8'h30
*/
typedef union _BTAON_FAST_0x174_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_rsvd: 8;                  // 07:00
        uint16_t LOP_PON_LDO533_delay: 8;      // 15:08
    };
} BTAON_FAST_0x174_TYPE;

/* 0x176
    07:00   rw  default: 2 ms                                                                               8'h40
    15:08   rw  default: 2 ms                                                                               8'h40
*/
typedef union _BTAON_FAST_0x176_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_M1M2_delay: 8;        // 07:00
        uint16_t LOP_POF_LDO533_delay: 8;      // 15:08
    };
} BTAON_FAST_0x176_TYPE;

/* 0x178
    07:00   rw  default: 2 ms                                                                               8'h40
    08      rw  reserved by H/W                                                                             1'b1
    09      rw  reserved by H/W                                                                             1'b0
    10      rw  reserved by H/W                                                                             1'b0
    11      rw  reserved by H/W                                                                             1'b0
    15:12   rw
*/
typedef union _BTAON_FAST_0x178_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t LOP_POF_SWR_delay: 8;         // 07:00
        uint16_t auto_vg1_vg2: 1;              // 08
        uint16_t vg1_vg2_by_fw: 1;             // 09
        uint16_t vcore_pc_vg1_fw: 1;           // 10
        uint16_t vcore_pc_vg2_fw: 1;           // 11
        uint16_t rsvd: 4;                      // 15:12
    };
} BTAON_FAST_0x178_TYPE;

/* 0x17e
    00      rw  reg_anc_ff_en_wp    For ANC, enable the type of "Feed-Forward"                              1'b0
    01      rw  reg_anc_fb_en_wp    For ANC, enable the type of "Feed-Back"                                 1'b0
    02      rw  reg_anc_fbpn_en_wp  For ANC, enable the type of "FBPN"                                      1'b0
    03      rw  reg_anc_lms_en_wp   For ANC, enable the type of "LMS"                                       1'b0
    07:04   r   dummy                                                                                       4'h0
    09:08   rw  pfm2pwm_step_ctrl   2'b00: 31.25us, 2'b01: 62.5us, 2'b10: 125us, 2'b11: 250us               2'h0
    10      r   dummy                                                                                       1'b0
    11      rw  pfm2pwm_soft_start  1'b1: pfm2pwm soft start is enabled                                     1'b0
    15:12   r   dummy                                                                                       4'h0
*/
typedef union _BTAON_FAST_0x17E_TYPE
{
    uint8_t d8[2];
    struct
    {
        uint16_t reg_anc_ff_en_wp: 1;          // 00
        uint16_t reg_anc_fb_en_wp: 1;          // 01
        uint16_t reg_anc_fbpn_en_wp: 1;        // 02
        uint16_t reg_anc_lms_en_wp: 1;         // 03
        uint16_t rsvd0: 4;                     // 07:04
        uint16_t pfm2pwm_step_ctrl: 2;         // 09:08
        uint16_t rsvd1: 1;                     // 10
        uint16_t pfm2pwm_soft_start: 1;        // 11
        uint16_t rsvd2: 4;                     // 15:12
    };
} BTAON_FAST_0x17E_TYPE;

#define BTAON_FAST_183                  0x183
#define BTAON_FAST_RTC_AON_WDT          0x190

#endif /* _RTL8763_SYSON_REG_H */
