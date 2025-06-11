#ifndef _RTL876X_AON_REG_H
#define _RTL876X_AON_REG_H

#include <stdint.h>

/* Auto gen based on RL6736A_REG_20210722.xlsx */

#define AON_RG0X        0x0
#define AON_RG1X        0x4
#define AON_RG2X        0x8
#define AON_RG3X        0xC
#define AON_RG4X        0x10

/* 0x0      0x4000_0000
    0       R/W AON_SWR_CORE_POW_SAW_IB         1'b0        BTAON_reg_wr
    1       R/W AON_SWR_CORE_POW_IMIR           1'b0        BTAON_reg_wr
    2       R/W AON_LDOAUX1_POW_LDO533HQ        1'b0        BTAON_reg_wr
    3       R/W AON_LDOAUX1_EN_POS              1'b0        BTAON_reg_wr
    4       R/W AON_LDOSYS_POW_HQLQ533_PC       1'b0        BTAON_reg_wr
    5       R/W AON_LDOSYS_POW_HQLQVCORE533_PC  1'b0        BTAON_reg_wr
    6       R/W AON_LDOAUX1_POS_RST_B           1'b0        BTAON_reg_wr
    7       R/W AON_LDOAUX1_POW_VREF            1'b0        BTAON_reg_wr
    8       R/W AON_LDOSYS_POW_LDO533HQ         1'b0        BTAON_reg_wr
    9       R/W AON_LDOSYS_EN_POS               1'b0        BTAON_reg_wr
    10      R/W AON_LDOSYS_POW_LDO733LQ_VCORE   1'b0        BTAON_reg_wr
    12:11   R/W AON_CHG_SEL_M2CCDFB             2'b01       BTAON_reg_wr
    13      R/W AON_LDOSYS_POS_RST_B            1'b0        BTAON_reg_wr
    14      R/W AON_LDOSYS_POW_LDOVREF          1'b0        BTAON_reg_wr
    15      R/W AON_CHG_POW_M2                  1'b0        BTAON_reg_wr
    16      R/W AON_CHG_POW_M1                  1'b0        BTAON_reg_wr
    17      R/W AON_CHG_POW_M2_DVDET            1'b0        BTAON_reg_wr
    18      R/W AON_CHG_POW_M1_DVDET            1'b0        BTAON_reg_wr
    19      R/W AON_CHG_EN_M1FON_LDO733         1'b0        BTAON_reg_wr
    20      R/W AON_CHG_EN_M2FONBUF             1'b0        BTAON_reg_wr
    21      R/W AON_CHG_EN_M2FON1K              1'b0        BTAON_reg_wr
    22      R/W AON_POW32K_32KXTAL              1'b0        BTAON_reg_wr
    23      R/W AON_POW32K_32KOSC               1'b1        BTAON_reg_wr
    24      R/W AON_MBIAS_POW_VAUDIO_DET        1'b0        BTAON_reg_wr
    25      R/W AON_MBIAS_POW_VDDCORE_DET       1'b0        BTAON_reg_wr
    26      R/W AON_MBIAS_POW_VAUX_DET          1'b0        BTAON_reg_wr
    27      R/W AON_MBIAS_POW_HV_DET            1'b0        BTAON_reg_wr
    28      R/W AON_MBIAS_POW_VBAT_DET          1'b0        BTAON_reg_wr
    29      R/W AON_MBIAS_POW_ADP_DET           1'b0        BTAON_reg_wr
    30      R/W AON_MBIAS_POW_BIAS_500nA        1'b0        BTAON_reg_wr
    31      R/W AON_MBIAS_POW_BIAS              1'b1        BTAON_reg_wr
 */
typedef union _AON_RG0X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_SWR_CORE_POW_SAW_IB: 1;
        uint32_t AON_SWR_CORE_POW_IMIR: 1;
        uint32_t AON_LDOAUX1_POW_LDO533HQ: 1;
        uint32_t AON_LDOAUX1_EN_POS: 1;
        uint32_t AON_LDOSYS_POW_HQLQ533_PC: 1;
        uint32_t AON_LDOSYS_POW_HQLQVCORE533_PC: 1;
        uint32_t AON_LDOAUX1_POS_RST_B: 1;
        uint32_t AON_LDOAUX1_POW_VREF: 1;
        uint32_t AON_LDOSYS_POW_LDO533HQ: 1;
        uint32_t AON_LDOSYS_EN_POS: 1;
        uint32_t AON_LDOSYS_POW_LDO733LQ_VCORE: 1;
        uint32_t AON_CHG_SEL_M2CCDFB: 2;
        uint32_t AON_LDOSYS_POS_RST_B: 1;
        uint32_t AON_LDOSYS_POW_LDOVREF: 1;
        uint32_t AON_CHG_POW_M2: 1;
        uint32_t AON_CHG_POW_M1: 1;
        uint32_t AON_CHG_POW_M2_DVDET: 1;
        uint32_t AON_CHG_POW_M1_DVDET: 1;
        uint32_t AON_CHG_EN_M1FON_LDO733: 1;
        uint32_t AON_CHG_EN_M2FONBUF: 1;
        uint32_t AON_CHG_EN_M2FON1K: 1;
        uint32_t AON_POW32K_32KXTAL: 1;
        uint32_t AON_POW32K_32KOSC: 1;
        uint32_t AON_MBIAS_POW_VAUDIO_DET: 1;
        uint32_t AON_MBIAS_POW_VDDCORE_DET: 1;
        uint32_t AON_MBIAS_POW_VAUX_DET: 1;
        uint32_t AON_MBIAS_POW_HV_DET: 1;
        uint32_t AON_MBIAS_POW_VBAT_DET: 1;
        uint32_t AON_MBIAS_POW_ADP_DET: 1;
        uint32_t AON_MBIAS_POW_BIAS_500nA: 1;
        uint32_t AON_MBIAS_POW_BIAS: 1;
    };
} AON_RG0X_TYPE;

/* 0x4      0x4000_0004
    0       R/W AON_REG1X_DUMMY1                1'h0        BTAON_reg_wr
    1       R/W AON_SWR_CORE_POW_BNYCNT_2       1'b0        BTAON_reg_wr
    2       R/W AON_LDO_DIG_POW_LDODIG          1'b0        BTAON_reg_wr
    3       R/W AON_LDO_DIG_EN_POS              1'b0        BTAON_reg_wr
    4       R/W AON_LDO_DIG_EN_LDODIG_PC        1'b0        BTAON_reg_wr
    6:5     R/W AON_XTAL_LPS_CAP_STEP           2'b01       BTAON_reg_wr
    8:7     R/W AON_XTAL_LPS_CAP_CYC            2'b00       BTAON_reg_wr
    9       R/W AON_LDO_DIG_POS_RST_B           1'b0        BTAON_reg_wr
    14:10   R/W AON_LDO_DIG_TUNE_LDODIG_VOUT    5'b10110    BTAON_reg_wr
    15      R/W AON_LDO_DIG_POW_LDODIG_VREF     1'b0        BTAON_reg_wr
    16      R/W AON_SWR_CORE_POW_ZCD_COMP_LOWIQ 1'b0        BTAON_reg_wr
    22:17   R/W AON_SWR_CORE_TUNE_BNYCNT_INI    6'b000000   BTAON_reg_wr
    23      R/W AON_SWR_CORE_POW_BNYCNT_1       1'b0        BTAON_reg_wr
    24      R/W AON_SWR_CORE_FPWM_1             1'b0        BTAON_reg_wr
    25      R/W AON_SWR_CORE_POW_OCP            1'b0        BTAON_reg_wr
    26      R/W AON_SWR_CORE_POW_ZCD            1'b0        BTAON_reg_wr
    27      R/W AON_SWR_CORE_POW_PFM            1'b0        BTAON_reg_wr
    28      R/W AON_SWR_CORE_POW_PWM            1'b0        BTAON_reg_wr
    29      R/W AON_SWR_CORE_POW_VDIV           1'b0        BTAON_reg_wr
    30      R/W AON_SWR_CORE_POW_REF            1'b0        BTAON_reg_wr
    31      R/W AON_SWR_CORE_POW_SAW            1'b0        BTAON_reg_wr
 */
typedef union _AON_RG1X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG1X_DUMMY1: 1;
        uint32_t AON_SWR_CORE_POW_BNYCNT_2: 1;
        uint32_t AON_LDO_DIG_POW_LDODIG: 1;
        uint32_t AON_LDO_DIG_EN_POS: 1;
        uint32_t AON_LDO_DIG_EN_LDODIG_PC: 1;
        uint32_t AON_XTAL_LPS_CAP_STEP: 2;
        uint32_t AON_XTAL_LPS_CAP_CYC: 2;
        uint32_t AON_LDO_DIG_POS_RST_B: 1;
        uint32_t AON_LDO_DIG_TUNE_LDODIG_VOUT: 5;
        uint32_t AON_LDO_DIG_POW_LDODIG_VREF: 1;
        uint32_t AON_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint32_t AON_SWR_CORE_TUNE_BNYCNT_INI: 6;
        uint32_t AON_SWR_CORE_POW_BNYCNT_1: 1;
        uint32_t AON_SWR_CORE_FPWM_1: 1;
        uint32_t AON_SWR_CORE_POW_OCP: 1;
        uint32_t AON_SWR_CORE_POW_ZCD: 1;
        uint32_t AON_SWR_CORE_POW_PFM: 1;
        uint32_t AON_SWR_CORE_POW_PWM: 1;
        uint32_t AON_SWR_CORE_POW_VDIV: 1;
        uint32_t AON_SWR_CORE_POW_REF: 1;
        uint32_t AON_SWR_CORE_POW_SAW: 1;
    };
} AON_RG1X_TYPE;

/* 0x8      0x4000_0008
    3:0     R/W AON_REG2X_DUMMY1                4'h0        BTAON_reg_wr
    4       R/W AON_BLE_RESTORE                 1'b0        BTAON_reg_wr
    5       R/W AON_VCORE_PC_POW_VCORE_PC_VG2   1'b1        BTAON_reg_wr
    6       R/W AON_VCORE_PC_POW_VCORE_PC_VG1   1'b1        BTAON_reg_wr
    7       R/W AON_LDO_DIG_POW_LDORET          1'b0        BTAON_reg_wr
    8       R/W AON_SWR_CORE_POW_SWR            1'b0        BTAON_reg_wr
    9       R/W AON_SWR_CORE_POW_LDO            1'b0        BTAON_reg_wr
    10      R/W AON_SWR_CORE_SEL_POS_VREFLPPFM  1'b0        BTAON_reg_wr
    11      R/W AON_SWR_CORE_FPWM_2             1'b0        BTAON_reg_wr
    19:12   R/W AON_SWR_CORE_TUNE_VDIV          8'b10001010 BTAON_reg_wr
    27:20   R/W AON_SWR_CORE_TUNE_POS_VREFPFM   8'b01101110 BTAON_reg_wr
    31:28   R/W AON_SWR_CORE_TUNE_REF_VREFLPPFM 4'b0110     BTAON_reg_wr
 */
typedef union _AON_RG2X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG2X_DUMMY1: 4;
        uint32_t AON_BLE_RESTORE: 1;
        uint32_t AON_VCORE_PC_POW_VCORE_PC_VG2: 1;
        uint32_t AON_VCORE_PC_POW_VCORE_PC_VG1: 1;
        uint32_t AON_LDO_DIG_POW_LDORET: 1;
        uint32_t AON_SWR_CORE_POW_SWR: 1;
        uint32_t AON_SWR_CORE_POW_LDO: 1;
        uint32_t AON_SWR_CORE_SEL_POS_VREFLPPFM: 1;
        uint32_t AON_SWR_CORE_FPWM_2: 1;
        uint32_t AON_SWR_CORE_TUNE_VDIV: 8;
        uint32_t AON_SWR_CORE_TUNE_POS_VREFPFM: 8;
        uint32_t AON_SWR_CORE_TUNE_REF_VREFLPPFM: 4;
    };
} AON_RG2X_TYPE;

/* 0xC      0x4000_000c
    1:0     R/W AON_REG3X_DUMMY1                2'h0        BTAON_reg_wr
    2       R/W AON_RFC_STORE                   1'b0        BTAON_reg_wr
    3       R/W AON_PF_STORE                    1'b0        BTAON_reg_wr
    4       R/W AON_MODEM_STORE                 1'b0        BTAON_reg_wr
    5       R/W AON_DP_MODEM_STORE              1'b0        BTAON_reg_wr
    6       R/W AON_BZ_STORE                    1'b0        BTAON_reg_wr
    7       R/W AON_BLE_STORE                   1'b0        BTAON_reg_wr
    8       R/W AON_BT_CORE_RSTB                1'b0        BTAON_reg_wr
    9       R/W AON_BT_PON_RSTB                 1'b0        BTAON_reg_wr
    10      R/W AON_ISO_BT_PON                  1'b1        BTAON_reg_wr
    11      R/W AON_ISO_BT_CORE                 1'b1        BTAON_reg_wr
    12      R/W AON_ISO_PLL2                    1'b1        BTAON_reg_wr
    13      R/W AON_ISO_PLL                     1'b1        BTAON_reg_wr
    14      R/W AON_BT_PLL_LP_PLL_pow_cpop      1'b0        BTAON_reg_wr
    15      R/W AON_BT_PLL_LP_PLL_pow_pll       1'b0        BTAON_reg_wr
    16      R/W AON_BT_PLL_pow_pll              1'b0        BTAON_reg_wr
    17      R/W AON_BT_PLL_LDO_pow_LDO          1'b0        BTAON_reg_wr
    18      R/W AON_BT_PLL_LDO_ERC_V12A_BTPLL   1'b0        BTAON_reg_wr
    19      R/W AON_BT_PLL_LDO_SW_LDO2PORCUT    1'b0        BTAON_reg_wr
    20      R/W AON_ISO_XTAL                    1'b1        BTAON_reg_wr
    21      R/W AON_OSC40M_POW_OSC              1'b1        BTAON_reg_wr
    24:22   R/W AON_XTAL_MODE                   3'b100      BTAON_reg_wr
    25      R/W AON_XTAL_POW_XTAL               1'b0        BTAON_reg_wr
    26      R/W AON_BT_RET_RSTB                 1'b1        BTAON_reg_wr
    27      R/W AON_RFC_RESTORE                 1'b0        BTAON_reg_wr
    28      R/W AON_PF_RESTORE                  1'b0        BTAON_reg_wr
    29      R/W AON_MODEM_RESTORE               1'b0        BTAON_reg_wr
    30      R/W AON_DP_MODEM_RESTORE            1'b0        BTAON_reg_wr
    31      R/W AON_BZ_RESTORE                  1'b0        BTAON_reg_wr
 */
typedef union _AON_RG3X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG3X_DUMMY1: 2;
        uint32_t AON_RFC_STORE: 1;
        uint32_t AON_PF_STORE: 1;
        uint32_t AON_MODEM_STORE: 1;
        uint32_t AON_DP_MODEM_STORE: 1;
        uint32_t AON_BZ_STORE: 1;
        uint32_t AON_BLE_STORE: 1;
        uint32_t AON_BT_CORE_RSTB: 1;
        uint32_t AON_BT_PON_RSTB: 1;
        uint32_t AON_ISO_BT_PON: 1;
        uint32_t AON_ISO_BT_CORE: 1;
        uint32_t AON_ISO_PLL2: 1;
        uint32_t AON_ISO_PLL: 1;
        uint32_t AON_BT_PLL_LP_PLL_pow_cpop: 1;
        uint32_t AON_BT_PLL_LP_PLL_pow_pll: 1;
        uint32_t AON_BT_PLL_pow_pll: 1;
        uint32_t AON_BT_PLL_LDO_pow_LDO: 1;
        uint32_t AON_BT_PLL_LDO_ERC_V12A_BTPLL: 1;
        uint32_t AON_BT_PLL_LDO_SW_LDO2PORCUT: 1;
        uint32_t AON_ISO_XTAL: 1;
        uint32_t AON_OSC40M_POW_OSC: 1;
        uint32_t AON_XTAL_MODE: 3;
        uint32_t AON_XTAL_POW_XTAL: 1;
        uint32_t AON_BT_RET_RSTB: 1;
        uint32_t AON_RFC_RESTORE: 1;
        uint32_t AON_PF_RESTORE: 1;
        uint32_t AON_MODEM_RESTORE: 1;
        uint32_t AON_DP_MODEM_RESTORE: 1;
        uint32_t AON_BZ_RESTORE: 1;
    };
} AON_RG3X_TYPE;

/* 0x10     0x4000_0010
    0       R/W FW_enter_lps                    1'b0        BTAON_reg_wr
    1       R/W FW_PON_SEQ_RST_N                1'b0        BTAON_reg_wr
    2       R/W true_power_off                  1'b0        BTAON_reg_wr
    3       R/W AON_RG4X_DUMMY3                 1'b0        BTAON_reg_wr
    4       R/W DPD_R[0]                        1'b1        BTAON_reg_wr
    5       R/W DPD_R[1]                        1'b0        BTAON_reg_wr
    6       R/W DPD_R[2]                        1'b1        BTAON_reg_wr
    7       R/W DPD_R[3]                        1'b1        BTAON_reg_wr
    8       R/W DPD_R[4]                        1'b1        BTAON_reg_wr
    9       R/W DPD_R[5]                        1'b1        BTAON_reg_wr
    10      R/W DPD_R[6]                        1'b1        BTAON_reg_wr
    11      R/W DPD_R[7]                        1'b1        BTAON_reg_wr
    12      R/W DPD_R[8]                        1'b1        BTAON_reg_wr
    13      R/W DPD_RCK                         1'b0        BTAON_reg_wr
    14      R/W AON_GATED_EN                    1'b1        BTAON_reg_wr
    15      R/W AON_RG4X_DUMMY15                1'b0        BTAON_reg_wr
    16      R/W AON_RG4X_DUMMY16                1'b0        BTAON_reg_wr
    17      R/W AON_RG4X_DUMMY17                1'b0        BTAON_reg_wr
    18      R/W AON_RG4X_DUMMY18                1'b0        BTAON_reg_wr
    19      R/W AON_RG4X_DUMMY19                1'b0        BTAON_reg_wr
    20      R/W AON_RG4X_DUMMY20                1'b0        BTAON_reg_wr
    21      R/W AON_RG4X_DUMMY21                1'b0        BTAON_reg_wr
    22      R/W AON_RG4X_DUMMY22                1'b0        BTAON_reg_wr
    23      R/W AON_RG4X_DUMMY23                1'b0        BTAON_reg_wr
    24      R/W AON_RG4X_DUMMY24                1'b0        BTAON_reg_wr
    25      R/W AON_RG4X_DUMMY25                1'b0        BTAON_reg_wr
    26      R/W AON_RG4X_DUMMY26                1'b0        BTAON_reg_wr
    27      R/W AON_RG4X_DUMMY27                1'b0        BTAON_reg_wr
    28      R/W AON_RG4X_DUMMY28                1'b0        BTAON_reg_wr
    29      R/W AON_RG4X_DUMMY29                1'b0        BTAON_reg_wr
    30      R/W AON_RG4X_DUMMY30                1'b0        BTAON_reg_wr
    31      R/W ISO_VDDON_XTAL                  1'b1        BTAON_reg_wr
 */
typedef union _AON_RG4X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t FW_enter_lps: 1;
        uint32_t FW_PON_SEQ_RST_N: 1;
        uint32_t true_power_off: 1;
        uint32_t AON_RG4X_DUMMY3: 1;
        uint32_t DPD_R_0: 1;
        uint32_t DPD_R_1: 1;
        uint32_t DPD_R_2: 1;
        uint32_t DPD_R_3: 1;
        uint32_t DPD_R_4: 1;
        uint32_t DPD_R_5: 1;
        uint32_t DPD_R_6: 1;
        uint32_t DPD_R_7: 1;
        uint32_t DPD_R_8: 1;
        uint32_t DPD_RCK: 1;
        uint32_t AON_GATED_EN: 1;
        uint32_t AON_RG4X_DUMMY15: 1;
        uint32_t AON_RG4X_DUMMY16: 1;
        uint32_t AON_RG4X_DUMMY17: 1;
        uint32_t AON_RG4X_DUMMY18: 1;
        uint32_t AON_RG4X_DUMMY19: 1;
        uint32_t AON_RG4X_DUMMY20: 1;
        uint32_t AON_RG4X_DUMMY21: 1;
        uint32_t AON_RG4X_DUMMY22: 1;
        uint32_t AON_RG4X_DUMMY23: 1;
        uint32_t AON_RG4X_DUMMY24: 1;
        uint32_t AON_RG4X_DUMMY25: 1;
        uint32_t AON_RG4X_DUMMY26: 1;
        uint32_t AON_RG4X_DUMMY27: 1;
        uint32_t AON_RG4X_DUMMY28: 1;
        uint32_t AON_RG4X_DUMMY29: 1;
        uint32_t AON_RG4X_DUMMY30: 1;
        uint32_t ISO_VDDON_XTAL: 1;
    };
} AON_RG4X_TYPE;

#define AON_FAST_0x0                        0x0
#define AON_FAST_REBOOT_SW_INFO0            0x2
#define AON_FAST_REBOOT_SW_INFO1            0x3
#define AON_FAST_DEBUG_PASSWORD             0x4
#define AON_FAST_0x6                        0x6
#define AON_FAST_0x8                        0x8
#define AON_FAST_0xA                        0xA
#define AON_FAST_0xC                        0xC
#define AON_FAST_0xE                        0xE
#define AON_FAST_0x10                       0x10
#define AON_FAST_0x12                       0x12
#define AON_FAST_0x14                       0x14
#define AON_FAST_0x16                       0x16
#define AON_FAST_0x1A                       0x1A
#define AON_FAST_0x20                       0x20
#define AON_FAST_0xA8                       0xA8
#define AON_FAST_0xAA                       0xAA
#define AON_FAST_0xC4                       0xC4
#define AON_FAST_0xF4                       0xF4
#define AON_FAST_0x120                      0x120
#define AON_FAST_0x122                      0x122
#define AON_FAST_WK_STATUS                  0x12C
#define AON_FAST_0x154                      0x154
#define AON_FAST_0x156                      0x156
#define AON_FAST_0x15E                      0x15E
#define AON_FAST_0x164                      0x164
#define AON_FAST_0x166                      0x166
#define AON_FAST_0x172                      0x172
#define AON_FAST_0x17A                      0x17A
#define AON_FAST_0x17C                      0x17C
#define AON_FAST_0x17E                      0x17E
#define AON_FAST_0x182                      0x182
#define AON_FAST_0x184                      0x184
#define AON_FAST_0x186                      0x186
#define AON_FAST_0x188                      0x188
#define AON_FAST_0x18A                      0x18A
#define AON_FAST_0x18C                      0x18C
#define AON_FAST_0x18E                      0x18E
#define AON_FAST_0x190                      0x190
#define AON_FAST_0x192                      0x192
#define AON_FAST_0x194                      0x194
#define AON_FAST_BOOT                       0x400
#define AON_FAST_REG1X_FW_GENERAL           0x402
#define AON_FAST_REG2X_FW_GENERAL           0x404
#define AON_FAST_REG3X_FW_GENERAL           0x406
#define AON_FAST_REG4X_FW_GENERAL           0x408
#define AON_FAST_REG5X_FW_GENERAL           0x40A
#define AON_FAST_REG6X_FW_GENERAL           0x40C
#define AON_FAST_REG7X_FW_GENERAL           0x40E
#define AON_FAST_REG8X_FW_GENERAL           0x410
#define AON_FAST_REG9X_FW_GENERAL           0x412
#define AON_FAST_REG10X_FW_GENERAL          0x414
#define AON_FAST_REF_RESISTANCE             0x416
#define AON_FAST_REG12X_FW_GENERAL          0x418
#define AON_FAST_REG13X_FW_GENERAL          0x41A
#define AON_FAST_REG14X_FW_GENERAL          0x41C
#define AON_FAST_REG15X_FW_GENERAL          0x41E
#define AON_FAST_REG16X_FW_GENERAL          0x420
#define AON_FAST_REG17X_FW_GENERAL          0x422
#define AON_FAST_RG0X_MEMORY                0x580
#define AON_FAST_RG1X_MEMORY                0x582
#define AON_FAST_REG_RAMROM_VSEL            0x584
#define AON_FAST_REG_ROM_MCU                0x586
#define AON_FAST_REG_ROM_DSP1_HV_LV         0x588
#define AON_FAST_REG_ROM_RFC                0x58A
#define AON_FAST_REG_RAM_RX2_HV             0x58C
#define AON_FAST_REG_RAM_RX2_LV             0x58E
#define AON_FAST_REG_RAM_RX2_CACHE_HV       0x590
#define AON_FAST_REG_RAM_RX2_CACHE_LV       0x592
#define AON_FAST_REG_RAM_BUF_HV             0x594
#define AON_FAST_REG_RAM_BUF_LV             0x596
#define AON_FAST_REG_RAM_SDIO               0x598
#define AON_FAST_REG_RAM_USB                0x59A
#define AON_FAST_REG_RAM_MAC_HV             0x59C
#define AON_FAST_REG_RAM_MAC_LV             0x59E
#define AON_FAST_REG_RAM_MDM_HV             0x5A0
#define AON_FAST_REG_RAM_MDM_LV             0x5A2
#define AON_FAST_REG_RAM_DSP1_HV            0x5A4
#define AON_FAST_REG_RAM_DSP1_LV            0x5A6
#define AON_FAST_REG_RAM_DATA_HV            0x5A8
#define AON_FAST_REG_RAM_DATA_LV            0x5AA
#define AON_FAST_REG_RAM_RFC_HV             0x5AC
#define AON_FAST_REG_RAM_RFC_LV             0x5AE
#define AON_FAST_SRAM_DS_ISO0_1             0x5B0
#define AON_FAST_SRAM_DS_ISO0_2             0x5B2
#define AON_FAST_SRAM_DS_ISO0_3             0x5B4
#define AON_FAST_SRAM_DS_ISO0_4             0x5B6
#define AON_FAST_SRAM_DS_ISO0_5             0x5B8
#define AON_FAST_SRAM_DS_ISO0_6             0x5BA
#define AON_FAST_SRAM_DS_ISO0_7             0x5BC
#define AON_FAST_SRAM_DS_ISO0_8             0x5BE
#define AON_FAST_SRAM_DS_ISO0_9             0x5C0
#define AON_FAST_SRAM_DS_ISO0_10            0x5C2
#define AON_FAST_SRAM_DS_ISO1_1             0x5C4
#define AON_FAST_SRAM_DS_ISO1_2             0x5C6
#define AON_FAST_SRAM_DS_ISO1_3             0x5C8
#define AON_FAST_SRAM_DS_ISO1_4             0x5CA
#define AON_FAST_SRAM_DS_ISO1_5             0x5CC
#define AON_FAST_SRAM_DS_ISO1_6             0x5CE
#define AON_FAST_SRAM_DS_ISO1_7             0x5D0
#define AON_FAST_SRAM_DS_ISO1_8             0x5D2
#define AON_FAST_SRAM_DS_ISO1_9             0x5D4
#define AON_FAST_SRAM_DS_ISO1_10            0x5D6
#define AON_FAST_SRAM_SD_ISO0_1             0x5D8
#define AON_FAST_SRAM_SD_ISO0_2             0x5DA
#define AON_FAST_SRAM_SD_ISO0_3             0x5DC
#define AON_FAST_SRAM_SD_ISO0_4             0x5DE
#define AON_FAST_SRAM_SD_ISO0_5             0x5E0
#define AON_FAST_SRAM_SD_ISO0_6             0x5E2
#define AON_FAST_SRAM_SD_ISO0_7             0x5E4
#define AON_FAST_SRAM_SD_ISO0_8             0x5E6
#define AON_FAST_SRAM_SD_ISO0_9             0x5E8
#define AON_FAST_SRAM_SD_ISO0_10            0x5EA
#define AON_FAST_SRAM_SD_ISO1_1             0x5EC
#define AON_FAST_SRAM_SD_ISO1_2             0x5EE
#define AON_FAST_SRAM_SD_ISO1_3             0x5F0
#define AON_FAST_SRAM_SD_ISO1_4             0x5F2
#define AON_FAST_SRAM_SD_ISO1_5             0x5F4
#define AON_FAST_SRAM_SD_ISO1_6             0x5F6
#define AON_FAST_SRAM_SD_ISO1_7             0x5F8
#define AON_FAST_SRAM_SD_ISO1_8             0x5FA
#define AON_FAST_SRAM_SD_ISO1_9             0x5FC
#define AON_FAST_SRAM_SD_ISO1_10            0x5FE
#define AON_FAST_ROM_LS_VCORE_1             0x600
#define AON_FAST_SRAM_LS_VCORE_1            0x602
#define AON_FAST_SRAM_RM3_ISO0_1            0x604
#define AON_FAST_SRAM_RM3_ISO0_2            0x606
#define AON_FAST_SRAM_RM3_ISO0_3            0x608
#define AON_FAST_SRAM_RM3_ISO0_4            0x60A
#define AON_FAST_SRAM_RM3_ISO0_5            0x60C
#define AON_FAST_SRAM_RM3_ISO0_6            0x60E
#define AON_FAST_SRAM_RM3_ISO0_7            0x610
#define AON_FAST_SRAM_RM3_ISO0_8            0x612
#define AON_FAST_SRAM_RM3_ISO0_9            0x614
#define AON_FAST_SRAM_RM3_ISO0_10           0x616
#define AON_FAST_SRAM_RM3_ISO1_1            0x618
#define AON_FAST_SRAM_RM3_ISO1_2            0x61A
#define AON_FAST_SRAM_RM3_ISO1_3            0x61C
#define AON_FAST_SRAM_RM3_ISO1_4            0x61E
#define AON_FAST_SRAM_RM3_ISO1_5            0x620
#define AON_FAST_SRAM_RM3_ISO1_6            0x622
#define AON_FAST_SRAM_RM3_ISO1_7            0x624
#define AON_FAST_SRAM_RM3_ISO1_8            0x626
#define AON_FAST_SRAM_RM3_ISO1_9            0x628
#define AON_FAST_REG0X_BTPLL_SYS            0x6F0
#define AON_FAST_REG1X_BTPLL_SYS            0x6F2
#define AON_FAST_REG2X_BTPLL_SYS            0x6F4
#define AON_FAST_REG3X_BTPLL_SYS            0x6F6
#define AON_FAST_REG0X_XTAL_OSC_SYS         0x6F8
#define AON_FAST_REG0X_RET_SYS              0x6FA
#define AON_FAST_REG0X_CORE_SYS             0x6FC
#define AON_FAST_REG1X_CORE_SYS             0x6FE
#define AON_FAST_REG2X_CORE_SYS             0x700
#define AON_FAST_REG3X_CORE_SYS             0x702
#define AON_FAST_REG4X_CORE_SYS             0x704
#define AON_FAST_REG0X_REG_MASK             0x706
#define AON_FAST_REG1X_REG_MASK             0x708
#define AON_FAST_REG0X_PMU_POS_CLK_MUX      0x70A
#define AON_FAST_REG0X_PMU_POS_SEL          0x70C
#define AON_FAST_REG1X_PMU_POS_SEL          0x70E
#define AON_FAST_REG2X_PMU_POS_SEL          0x710
#define AON_FAST_REG3X_PMU_POS_SEL          0x712
#define AON_FAST_REG4X_PMU_POS_SEL          0x714
#define AON_FAST_REG5X_PMU_POS_SEL          0x716
#define AON_FAST_REG6X_PMU_POS_SEL          0x718
#define AON_FAST_REG7X_PMU_POS_SEL          0x71A
#define AON_FAST_REG8X_PMU_POS_SEL          0x71C
#define AON_FAST_REG9X_PMU_POS_SEL          0x71E
#define AON_FAST_REG10X_PMU_POS_SEL         0x720
#define AON_FAST_REG11X_PMU_POS_SEL         0x722
#define AON_FAST_REG12X_PMU_POS_SEL         0x724
#define AON_FAST_REG0X_REG_FSM              0x726
#define AON_FAST_REG0X_CLK_SEL              0x728
#define AON_FAST_REG0X_REG_AON_LOADER       0x72A
#define AON_FAST_REG0X_REG_AON_WDT          0x72C
#define AON_FAST_REG0X_PAD_H3L1             0x742
#define AON_FAST_REG0X_PAD_CFG              0x744
#define AON_FAST_REG1X_PAD_CFG              0x746
#define AON_FAST_REG2X_PAD_CFG              0x748
#define AON_FAST_REG0X_PAD_STS              0x7EC
#define AON_FAST_REG1X_PAD_STS              0x7EE
#define AON_FAST_REG2X_PAD_STS              0x7F0
#define AON_FAST_REG3X_PAD_STS              0x7F2
#define AON_FAST_REG4X_PAD_STS              0x7F4
#define AON_FAST_REG5X_PAD_STS              0x7F6
#define AON_FAST_REG6X_PAD_STS              0x7F8
#define AON_FAST_AON_REG_LOP_PON_RG0X       0x800
#define AON_FAST_AON_REG_LOP_PON_RG1X       0x802
#define AON_FAST_AON_REG_LOP_PON_RG2X       0x804
#define AON_FAST_AON_REG_LOP_PON_RG3X       0x806
#define AON_FAST_AON_REG_LOP_PON_RG4X       0x808
#define AON_FAST_AON_REG_LOP_PON_RG5X       0x80A
#define AON_FAST_AON_REG_LOP_PON_RG6X       0x80C
#define AON_FAST_AON_REG_LOP_PON_RG7X       0x80E
#define AON_FAST_AON_REG_LOP_PON_RG8X       0x810
#define AON_FAST_AON_REG_LOP_PON_RG9X       0x812
#define AON_FAST_AON_REG_LOP_PON_RG10X      0x814
#define AON_FAST_AON_REG_LOP_PON_RG11X      0x816
#define AON_FAST_AON_REG_LOP_PON_RG12X      0x818
#define AON_FAST_AON_REG_LOP_PON_RG13X      0x81A
#define AON_FAST_AON_REG_LOP_PON_RG14X      0x81C
#define AON_FAST_AON_REG_LOP_PON_RG15X      0x81E
#define AON_FAST_AON_REG_LOP_PON_RG16X      0x820
#define AON_FAST_AON_REG_LOP_PON_RG17X      0x822
#define AON_FAST_AON_REG_LOP_PON_RG18X      0x824
#define AON_FAST_AON_REG_LOP_PON_RG19X      0x826
#define AON_FAST_AON_REG_LOP_PON_RG20X      0x828
#define AON_FAST_AON_REG_LOP_PON_RG21X      0x82A
#define AON_FAST_AON_REG_LOP_PON_RG22X      0x82C
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG0X 0x82E
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG1X 0x830
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG2X 0x832
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG3X 0x834
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG4X 0x836
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG5X 0x838
#define AON_FAST_AON_REG_LOP_PON_DELAY_RG6X 0x83A
#define AON_FAST_AON_REG_LOP_PON_ECO_RG0X   0x83C
#define AON_FAST_AON_REG_LOP_POF_RG0X       0x850
#define AON_FAST_AON_REG_LOP_POF_RG1X       0x852
#define AON_FAST_AON_REG_LOP_POF_RG2X       0x854
#define AON_FAST_AON_REG_LOP_POF_RG3X       0x856
#define AON_FAST_AON_REG_LOP_POF_RG4X       0x858
#define AON_FAST_AON_REG_LOP_POF_RG5X       0x85A
#define AON_FAST_AON_REG_LOP_POF_RG6X       0x85C
#define AON_FAST_AON_REG_LOP_POF_RG7X       0x85E
#define AON_FAST_AON_REG_LOP_POF_RG8X       0x860
#define AON_FAST_AON_REG_LOP_POF_RG9X       0x862
#define AON_FAST_AON_REG_LOP_POF_RG10X      0x864
#define AON_FAST_AON_REG_LOP_POF_RG11X      0x866
#define AON_FAST_AON_REG_LOP_POF_RG12X      0x868
#define AON_FAST_AON_REG_LOP_POF_RG13X      0x86A
#define AON_FAST_AON_REG_LOP_POF_RG14X      0x86C
#define AON_FAST_AON_REG_LOP_POF_RG15X      0x86E
#define AON_FAST_AON_REG_LOP_POF_RG16X      0x870
#define AON_FAST_AON_REG_LOP_POF_RG17X      0x872
#define AON_FAST_AON_REG_LOP_POF_RG18X      0x874
#define AON_FAST_AON_REG_LOP_POF_RG19X      0x876
#define AON_FAST_AON_REG_LOP_POF_RG20X      0x878
#define AON_FAST_AON_REG_LOP_POF_RG21X      0x87A
#define AON_FAST_AON_REG_LOP_POF_RG22X      0x87C
#define AON_FAST_AON_REG_LOP_POF_DELAY_RG0X 0x87E
#define AON_FAST_AON_REG_LOP_POF_DELAY_RG1X 0x880
#define AON_FAST_AON_REG_LOP_POF_DELAY_RG2X 0x882
#define AON_FAST_AON_REG_LOP_POF_DELAY_RG3X 0x884
#define AON_FAST_AON_REG_LOP_POF_DELAY_RG4X 0x886
#define AON_FAST_AON_REG_LOP_POF_DELAY_RG5X 0x888
#define AON_FAST_AON_REG_LOP_POF_MISC       0x88A
#define AON_FAST_AON_REG_LOP_POF_ECO_RG0X   0x88C
#define AON_FAST_REG0X_WAIT_READY           0x8A0
#define AON_FAST_REG1X_WAIT_READY           0x8A2
#define AON_FAST_REG0X_WAIT_AON_CNT         0x8A4
#define AON_FAST_REG1X_WAIT_AON_CNT         0x8A6
#define AON_FAST_REG2X_WAIT_AON_CNT         0x8A8
#define AON_FAST_REG3X_WAIT_AON_CNT         0x8AA
#define AON_FAST_REG4X_WAIT_AON_CNT         0x8AC
#define AON_FAST_REG5X_WAIT_AON_CNT         0x8AE
#define AON_FAST_REG6X_WAIT_AON_CNT         0x8B0
#define AON_FAST_REG7X_WAIT_AON_CNT         0x8B2
#define AON_FAST_REG8X_WAIT_AON_CNT         0x8B4
#define AON_FAST_REG9X_WAIT_AON_CNT         0x8B6
#define AON_FAST_REG10X_WAIT_AON_CNT        0x8B8
#define AON_FAST_REG11X_WAIT_AON_CNT        0x8BA
#define AON_FAST_REG12X_WAIT_AON_CNT        0x8BC
#define AON_FAST_REG13X_WAIT_AON_CNT        0x8BE
#define AON_FAST_REG14X_WAIT_AON_CNT        0x8C0
#define AON_FAST_REG15X_WAIT_AON_CNT        0x8C2
#define AON_FAST_REG16X_WAIT_AON_CNT        0x8C4
#define AON_FAST_REG17X_WAIT_AON_CNT        0x8C6
#define AON_FAST_REG18X_WAIT_AON_CNT        0x8C8
#define AON_FAST_REG19X_WAIT_AON_CNT        0x8CA
#define AON_FAST_SET_WKEN_MISC              0x8CC
#define AON_FAST_SET_WKPOL_MISC             0x8CE
#define AON_FAST_RO_WK_REG0X                0x8D0
#define AON_FAST_SET_SHIP_MODE              0x8D2
#define AON_FAST_SET_PMU_DBG_MODE           0x8D4
#define AON_FAST_REG0X_32k                  0x8D6
#define AON_FAST_REG1X_32k                  0x8D8
#define AON_FAST_REG2X_32k                  0x8DA
#define AON_FAST_REG0X_300k                 0x8DC
#define AON_FAST_REG1X_300k                 0x8DE
#define AON_FAST_REG2X_300k                 0x8E0
#define AON_FAST_REG3X_300k                 0x8E2
#define AON_FAST_REG0X_ZCD                  0x8E4
#define AON_FAST_REG0X_ZCD_AUDIO            0x8E6
#define AON_FAST_AON_REG0X_MUX_SEL          0x940
#define AON_FAST_AON_REG1X_MUX_SEL          0x942
#define AON_FAST_AON_REG2X_MUX_SEL          0x944
#define AON_FAST_AON_REG3X_MUX_SEL          0x946
#define AON_FAST_AON_REG4X_MUX_SEL          0x948
#define AON_FAST_AON_REG5X_MUX_SEL          0x94A
#define AON_FAST_AON_REG6X_MUX_SEL          0x94C
#define AON_FAST_AON_REG7X_MUX_SEL          0x94E
#define AON_FAST_AON_REG8X_MUX_SEL          0x950
#define AON_FAST_AON_REG9X_MUX_SEL          0x952
#define AON_FAST_AON_REG10X_MUX_SEL         0x954
#define AON_FAST_AON_REG11X_MUX_SEL         0x956
#define AON_FAST_AON_REG12X_MUX_SEL         0x958
#define AON_FAST_CORE_MODULE_REG0X_MUX_SEL  0x95A
#define AON_FAST_FAON_READ_RG0X             0x970
#define AON_FAST_FAON_READ_RG1X             0x972
#define AON_FAST_FAON_READ_RG2X             0x974
#define AON_FAST_FAON_READ_RG3X             0x976
#define AON_FAST_FAON_READ_RG4X             0x978
#define AON_FAST_FAON_READ_RG5X             0x97A
#define AON_FAST_FAON_READ_RG6X             0x97C
#define AON_FAST_FAON_READ_RG7X             0x97E
#define AON_FAST_FAON_READ_RG8X             0x980
#define AON_FAST_FAON_READ_RG9X             0x982
#define AON_FAST_FAON_READ_RG10X            0x984
#define AON_FAST_FAON_READ_RG11X            0x986
#define AON_FAST_FAON_READ_RG12X            0x988
#define AON_FAST_FAON_READ_RG13X            0x98A
#define AON_FAST_FAON_READ_RG14X            0x98C
#define AON_FAST_FAON_READ_RG15X            0x98E
#define AON_FAST_FAON_READ_RG16X            0x990
#define AON_FAST_FAON_READ_RG17X            0x992
#define AON_FAST_FAON_READ_RG18X            0x994
#define AON_FAST_FAON_READ_RG19X            0x996
#define AON_FAST_FAON_READ_RG20X            0x998
#define AON_FAST_FAON_READ_RG21X            0x99A
#define AON_FAST_FAON_READ_RG22X            0x99C
#define AON_FAST_REG0X_MBIAS                0xC00
#define AON_FAST_REG1X_MBIAS                0xC02
#define AON_FAST_REG2X_MBIAS                0xC04
#define AON_FAST_REG3X_MBIAS                0xC06
#define AON_FAST_REG4X_MBIAS                0xC08
#define AON_FAST_REG5X_MBIAS                0xC0A
#define AON_FAST_REG6X_MBIAS                0xC0C
#define AON_FAST_REG7X_MBIAS                0xC0E
#define AON_FAST_REG8X_MBIAS                0xC10
#define AON_FAST_REG9X_MBIAS                0xC12
#define AON_FAST_REG10X_MBIAS               0xC14
#define AON_FAST_FLAG0X_MBIAS               0xC16
#define AON_FAST_FLAG1X_MBIAS               0xC18
#define AON_FAST_FLAG2X_MBIAS               0xC1A
#define AON_FAST_REG0X_LDOSYS               0xC80
#define AON_FAST_REG1X_LDOSYS               0xC82
#define AON_FAST_REG2X_LDOSYS               0xC84
#define AON_FAST_REG3X_LDOSYS               0xC86
#define AON_FAST_REG4X_LDOSYS               0xC88
#define AON_FAST_FLAG0X_LDOSYS              0xC8A
#define AON_FAST_REG0X_LDOAUX1              0xD50
#define AON_FAST_REG1X_LDOAUX1              0xD52
#define AON_FAST_REG2X_LDOAUX1              0xD54
#define AON_FAST_REG0X_LDO_DIG              0xE60
#define AON_FAST_REG1X_LDO_DIG              0xE62
#define AON_FAST_REG0X_VCORE_PC             0xEB0
#define AON_FAST_REG0X_CHG                  0x1000
#define AON_FAST_REG1X_CHG                  0x1002
#define AON_FAST_REG2X_CHG                  0x1004
#define AON_FAST_REG3X_CHG                  0x1006
#define AON_FAST_REG4X_CHG                  0x1008
#define AON_FAST_FLAG0X_CHG                 0x100A
#define AON_FAST_REG0X_SWR_CORE             0x1040
#define AON_FAST_REG1X_SWR_CORE             0x1042
#define AON_FAST_REG2X_SWR_CORE             0x1044
#define AON_FAST_REG3X_SWR_CORE             0x1046
#define AON_FAST_REG4X_SWR_CORE             0x1048
#define AON_FAST_REG5X_SWR_CORE             0x104A
#define AON_FAST_REG6X_SWR_CORE             0x104C
#define AON_FAST_REG7X_SWR_CORE             0x104E
#define AON_FAST_REG8X_SWR_CORE             0x1050
#define AON_FAST_REG9X_SWR_CORE             0x1052
#define AON_FAST_REG10X_SWR_CORE            0x1054
#define AON_FAST_FLAG0X_SWR_CORE            0x1056
#define AON_FAST_C_KOUT0X_SWR_CORE          0x1058
#define AON_FAST_C_KOUT1X_SWR_CORE          0x105A
#define AON_FAST_C_KOUT2X_SWR_CORE          0x105C
#define AON_FAST_REG0X_SWR_AUDIO            0x11C0
#define AON_FAST_REG1X_SWR_AUDIO            0x11C2
#define AON_FAST_REG2X_SWR_AUDIO            0x11C4
#define AON_FAST_REG3X_SWR_AUDIO            0x11C6
#define AON_FAST_REG4X_SWR_AUDIO            0x11C8
#define AON_FAST_REG5X_SWR_AUDIO            0x11CA
#define AON_FAST_REG6X_SWR_AUDIO            0x11CC
#define AON_FAST_REG7X_SWR_AUDIO            0x11CE
#define AON_FAST_REG8X_SWR_AUDIO            0x11D0
#define AON_FAST_REG9X_SWR_AUDIO            0x11D2
#define AON_FAST_REG10X_SWR_AUDIO           0x11D4
#define AON_FAST_FLAG0X_SWR_AUDIO           0x11D6
#define AON_FAST_C_KOUT0X_SWR_AUDIO         0x11D8
#define AON_FAST_C_KOUT1X_SWR_AUDIO         0x11DA
#define AON_FAST_C_KOUT2X_SWR_AUDIO         0x11DC
#define AON_FAST_REG_BT_ANAPAR_PLL0         0x1400
#define AON_FAST_REG_BT_ANAPAR_PLL1         0x1402
#define AON_FAST_REG_BT_ANAPAR_PLL2         0x1404
#define AON_FAST_REG_BT_ANAPAR_PLL3         0x1406
#define AON_FAST_REG_BT_ANAPAR_PLL4         0x1408
#define AON_FAST_REG_BT_ANAPAR_PLL5         0x140A
#define AON_FAST_REG_BT_ANAPAR_LDO_PLL0     0x140C
#define AON_FAST_RG0X_32KXTAL               0x1470
#define AON_FAST_RG1X_32KXTAL               0x1472
#define AON_FAST_RG2X_32KXTAL               0x1474
#define AON_FAST_RG0X_32KOSC                0x1476
#define AON_FAST_RG0X_POW_32K               0x1478
#define AON_FAST_BT_ANAPAR_XTAL_mode        0x1490
#define AON_FAST_BT_ANAPAR_XTAL0            0x1492
#define AON_FAST_BT_ANAPAR_XTAL1            0x1494
#define AON_FAST_BT_ANAPAR_XTAL2            0x1496
#define AON_FAST_BT_ANAPAR_XTAL3            0x1498
#define AON_FAST_BT_ANAPAR_XTAL4            0x149A
#define AON_FAST_BT_ANAPAR_XTAL5            0x149C
#define AON_FAST_BT_ANAPAR_XTAL6            0x149E
#define AON_FAST_BT_ANAPAR_XTAL7            0x14A0
#define AON_FAST_BT_ANAPAR_XTAL8            0x14A2
#define AON_FAST_BT_ANAPAR_XTAL9            0x14A4
#define AON_FAST_BT_ANAPAR_XTAL10           0x14A6
#define AON_FAST_BT_ANAPAR_XTAL11           0x14A8
#define AON_FAST_BT_ANAPAR_XTAL12           0x14AA
#define AON_FAST_BT_ANAPAR_XTAL13           0x14AC
#define AON_FAST_BT_ANAPAR_XTAL14           0x14AE
#define AON_FAST_BT_ANAPAR_XTAL15           0x14B0
#define AON_FAST_BT_ANAPAR_XTAL16           0x14B2
#define AON_FAST_BT_ANAPAR_XTAL17           0x14B4
#define AON_FAST_BT_ANAPAR_OSC40M           0x14F0
#define AON_FAST_REG0X_AUXADC               0x1510
#define AON_FAST_REG1X_AUXADC               0x1512
#define AON_FAST_REG2X_AUXADC               0x1514
#define AON_FAST_REG3X_AUXADC               0x1516
#define AON_FAST_REG4X_AUXADC               0x1518
#define AON_FAST_REG5X_AUXADC               0x151A
#define AON_FAST_REG6X_AUXADC               0x151C
#define AON_FAST_RG0X_CODEC_LDO             0x1530
#define AON_FAST_REG0X_BTADDA_LDO           0x1540
#define AON_FAST_REG0X_USB                  0x1550
#define AON_FAST_REG0X_PAD_ADC_0            0x1560
#define AON_FAST_REG1X_PAD_ADC_1            0x1562
#define AON_FAST_REG2X_PAD_ADC_2            0x1564
#define AON_FAST_REG3X_PAD_ADC_3            0x1566
#define AON_FAST_REG0X_PAD_P1_0             0x1568
#define AON_FAST_REG1X_PAD_P1_1             0x156A
#define AON_FAST_REG2X_PAD_P1_2             0x156C
#define AON_FAST_REG3X_PAD_P1_3             0x156E
#define AON_FAST_REG4X_PAD_P1_4             0x1570
#define AON_FAST_REG5X_PAD_P1_5             0x1572
#define AON_FAST_REG6X_PAD_P1_6             0x1574
#define AON_FAST_REG7X_PAD_P1_7             0x1576
#define AON_FAST_REG0X_PAD_P2_0             0x1578
#define AON_FAST_REG1X_PAD_P2_1             0x157A
#define AON_FAST_REG2X_PAD_P2_2             0x157C
#define AON_FAST_REG3X_PAD_P2_3             0x157E
#define AON_FAST_REG4X_PAD_P2_4             0x1580
#define AON_FAST_REG5X_PAD_P2_5             0x1582
#define AON_FAST_REG6X_PAD_P2_6             0x1584
#define AON_FAST_REG7X_PAD_P2_7             0x1586
#define AON_FAST_REG0X_PAD_P3_0             0x1588
#define AON_FAST_REG1X_PAD_P3_1             0x158A
#define AON_FAST_REG2X_PAD_P3_2             0x158C
#define AON_FAST_REG3X_PAD_P3_3             0x158E
#define AON_FAST_REG4X_PAD_P3_4             0x1590
#define AON_FAST_REG5X_PAD_P3_5             0x1592
#define AON_FAST_REG0X_PAD_P4_0             0x1594
#define AON_FAST_REG1X_PAD_P4_1             0x1596
#define AON_FAST_REG2X_PAD_P4_2             0x1598
#define AON_FAST_REG3X_PAD_P4_3             0x159A
#define AON_FAST_REG4X_PAD_P4_4             0x159C
#define AON_FAST_REG5X_PAD_P4_5             0x159E
#define AON_FAST_REG6X_PAD_P4_6             0x15A0
#define AON_FAST_REG7X_PAD_P4_7             0x15A2
#define AON_FAST_REG0X_PAD_P5_0             0x15A4
#define AON_FAST_REG1X_PAD_P5_1             0x15A6
#define AON_FAST_REG2X_PAD_P5_2             0x15A8
#define AON_FAST_REG3X_PAD_P5_3             0x15AA
#define AON_FAST_REG4X_PAD_P5_4             0x15AC
#define AON_FAST_REG5X_PAD_P5_5             0x15AE
#define AON_FAST_REG6X_PAD_P5_6             0x15B0
#define AON_FAST_REG7X_PAD_P5_7             0x15B2
#define AON_FAST_REG0X_PAD_P6_0             0x15B4
#define AON_FAST_REG1X_PAD_P6_1             0x15B6
#define AON_FAST_REG2X_PAD_P6_2             0x15B8
#define AON_FAST_REG3X_PAD_P6_3             0x15BA
#define AON_FAST_REG4X_PAD_P6_4             0x15BC
#define AON_FAST_REG5X_PAD_P6_5             0x15BE
#define AON_FAST_REG6X_PAD_P6_6             0x15C0
#define AON_FAST_REG0X_PAD_P7_0             0x15C2
#define AON_FAST_REG1X_PAD_P7_1             0x15C4
#define AON_FAST_REG2X_PAD_P7_2             0x15C6
#define AON_FAST_REG3X_PAD_P7_3             0x15C8
#define AON_FAST_REG4X_PAD_P7_4             0x15CA
#define AON_FAST_REG5X_PAD_P7_5             0x15CC
#define AON_FAST_REG6X_PAD_P7_6             0x15CE
#define AON_FAST_REG0X_PAD_P8_0             0x15D0
#define AON_FAST_REG1X_PAD_P8_1             0x15D2
#define AON_FAST_REG2X_PAD_P8_2             0x15D4
#define AON_FAST_REG3X_PAD_P8_3             0x15D6
#define AON_FAST_REG4X_PAD_P8_4             0x15D8
#define AON_FAST_REG5X_PAD_P8_5             0x15DA
#define AON_FAST_REG0X_PAD_SPIC0_CSN        0x15DC
#define AON_FAST_REG1X_PAD_SPIC0_SCK        0x15DE
#define AON_FAST_REG2X_PAD_SPIC0_SIO0       0x15E0
#define AON_FAST_REG3X_PAD_SPIC0_SIO1       0x15E2
#define AON_FAST_REG4X_PAD_SPIC0_SIO2       0x15E4
#define AON_FAST_REG5X_PAD_SPIC0_SIO3       0x15E6
#define AON_FAST_REG0X_PAD_SPIC1_CSN        0x15E8
#define AON_FAST_REG1X_PAD_SPIC1_SCK        0x15EA
#define AON_FAST_REG2X_PAD_SPIC1_SIO0       0x15EC
#define AON_FAST_REG3X_PAD_SPIC1_SIO1       0x15EE
#define AON_FAST_REG4X_PAD_SPIC1_SIO2       0x15F0
#define AON_FAST_REG5X_PAD_SPIC1_SIO3       0x15F2
#define AON_FAST_REG0X_PAD_SPIC2_CSN        0x15F4
#define AON_FAST_REG1X_PAD_SPIC2_SCK        0x15F6
#define AON_FAST_REG2X_PAD_SPIC2_SIO0       0x15F8
#define AON_FAST_REG3X_PAD_SPIC2_SIO1       0x15FA
#define AON_FAST_REG4X_PAD_SPIC2_SIO2       0x15FC
#define AON_FAST_REG5X_PAD_SPIC2_SIO3       0x15FE
#define AON_FAST_REG0X_PAD_MIC1_P           0x1600
#define AON_FAST_REG1X_PAD_MIC1_N           0x1602
#define AON_FAST_REG2X_PAD_MIC2_P           0x1604
#define AON_FAST_REG3X_PAD_MIC2_N           0x1606
#define AON_FAST_REG4X_PAD_MIC3_P           0x1608
#define AON_FAST_REG5X_PAD_MIC3_N           0x160A
#define AON_FAST_REG6X_PAD_LOUT_P           0x160C
#define AON_FAST_REG7X_PAD_LOUT_N           0x160E
#define AON_FAST_REG8X_PAD_ROUT_P           0x1610
#define AON_FAST_REG9X_PAD_ROUT_N           0x1612
#define AON_FAST_REG10X_PAD_AUX_L           0x1614
#define AON_FAST_REG11X_PAD_AUX_R           0x1616
#define AON_FAST_REG12X_PAD_MICBIAS         0x1618
#define AON_FAST_REG0X_PAD_P10_0            0x161A

/* 0x0
    15:00   R/W freg0                                            16'b0
 */
typedef union _AON_FAST_0x0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg0: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x0_TYPE;

/* 0x02 (AON_FAST_REBOOT_SW_INFO0, AON_FAST_REBOOT_SW_INFO1) */
typedef union _AON_FAST_REBOOT_SW_INFO0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ota_valid: 1;                  /* bit[0]: ota valid */
        uint16_t is_rom_code_patch: 1;          /* bit[1]: is rom code patch ? */
        uint16_t is_fw_trig_wdg_to: 1;          /* bit[2]: does fw trigger watchdog timeout ? */
        uint16_t is_airplane_mode: 1;           /* bit[3]: does h5 link reset ? */
        uint16_t is_send_patch_end_evt: 1;      /* bit[4]: does we send patch end event ? */
        uint16_t ota_mode: 1;                   /* bit[5]: ota mode */
        uint16_t is_hci_mode: 1;                /* bit[6]: switch to hci mode? */
        uint16_t is_single_tone_mode: 1;        /* bit[7]: reserved */
        uint16_t REBOOT_SW_INFO1: 8;            /* bit[15:08] for AON_FAST_REBOOT_SW_INFO1 */
    };
} AON_FAST_REBOOT_SW_INFO0_TYPE;

/* 0x4
    15:00   R/W freg2                                            16'b0
 */
typedef union _AON_FAST_0x4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg2: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x4_TYPE;

/* 0x6
    15:00   R/W freg3                                            16'h0
 */
typedef union _AON_FAST_0x6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg3: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x6_TYPE;

/* 0x8
    15:00   R/W freg4                                            16'h0
 */
typedef union _AON_FAST_0x8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg4: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x8_TYPE;

/* 0xA
    15:00   R/W freg5                                            16'h0
 */
typedef union _AON_FAST_0xA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg5: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0xA_TYPE;

/* 0xC
    15:00   R/W freg6                                            16'h0
 */
typedef union _AON_FAST_0xC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg6: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0xC_TYPE;

/* 0xE
    15:00   R/W freg7                                            16'h0
 */
typedef union _AON_FAST_0xE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg7: 16;                 /* FW reserved registers. */
    };
} AON_FAST_0x0E_TYPE;

/* 0x10
    15:00   R/W freg8                                            16'h0
 */
typedef union _AON_FAST_0x10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg8: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x10_TYPE;

/* 0x12
    15:00   R/W freg9                                            16'h0
 */
typedef union _AON_FAST_0x12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg9: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x12_TYPE;

/* 0x14
    15:00   R/W freg10                                            16'h0
 */
typedef union _AON_FAST_0x14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg10: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x14_TYPE;

/* 0x16
    15:00   R/W freg11                                            16'h0
 */
typedef union _AON_FAST_0x16_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg11: 16;                   /* FW reserved registers. */
    };
} AON_FAST_0x16_TYPE;

/* 0x1A
    05:00   R   RSVD
    10:06   R/W ECO_D_023_REVERSE_4_0
    11      R/W reg_aon_w1o_clk_flash_dis
    15:12   R/W XTAL32K_Reserved15_9_6
 */
typedef union _AON_FAST_0x1A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RCAL_5_0: 6;                           // 05:00
        uint16_t rsvd0: 5;                              // 10:06
        uint16_t reg_aon_flash_clk_dis: 1;              // 11
        uint16_t is_efuse_invalid: 1;                   // 12
        uint16_t is_enable_efuse_read_protect: 1;       // 13
        uint16_t is_enable_efuse_write_protect: 1;      // 14
        uint16_t is_hw_aes_dma_mode: 1;                 // 15
    };
} AON_FAST_0x1A_TYPE;

/* 0x20
    0       R   POW_32KXTAL
    1       R   POW_32KOSC
    2       R/W reg_dsp_flash_prot
    3       R/W reg_aon_hwspi_en_wp
    4       R/W reg_aon_hwspi_en
    5       R/W reg_aon_debug_port_wp
    6       R/W reg_aon_debug_port
    7       R/W reg_aon_dbg_boot_dis
    15:08   R/W XTAL32K_Reserved16_13_6
 */
typedef union _AON_FAST_0x20_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t POW_32KXTAL: 1;
        uint16_t POW_32KOSC: 1;
        uint16_t reg_dsp_flash_prot: 1;
        uint16_t reg_aon_hwspi_en_wp: 1;
        uint16_t reg_aon_hwspi_en: 1;
        uint16_t reg_aon_debug_port_wp: 1;
        uint16_t reg_aon_debug_port: 1;
        uint16_t reg_aon_dbg_boot_dis: 1;
        uint16_t is_disable_hci_ram_patch: 1;           // 08
        uint16_t is_disable_hci_flash_access: 1;        // 09
        uint16_t is_disable_hci_system_access: 1;       // 10
        uint16_t is_disable_hci_efuse_access: 1;        // 11
        uint16_t is_disable_hci_bt_test: 1;             // 12
        uint16_t is_disable_usb_function: 1;            // 13
        uint16_t is_disable_sdio_function: 1;           // 14
        uint16_t is_debug_password_invalid: 1;          // 15
    };
} AON_FAST_0x20_TYPE;

/* 0xA8
*/
typedef union _AON_FAST_0xA8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_FREQ_SEL: 3;             // 02:00
        uint16_t AAC_SEL: 1;                   // 03
        uint16_t AAC_GM1: 6;                   // 09:04
        uint16_t AAC_GM: 6;                    // 15:10
    };
} AON_FAST_0xA8_TYPE;

/* 0xAA
*/
typedef union _AON_FAST_0xAA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 15;                     // 14:00
        uint16_t RCAL_32K_SEL: 1;              // 15
    };
} AON_FAST_0xAA_TYPE;

/* 0xC4
    07:00   rw  RSVD                            8'h0
    15:08   r   XTAL_IS_FINEO                   8'h0
 */
typedef union _AON_FAST_0xC4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD: 8;
        uint16_t XTAL_IS_FINEO: 8;
    };
} AON_FAST_0xC4_TYPE;

/* 0xF4
*/
typedef union _AON_FAST_0xF4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 1;                      // 00
        uint16_t rsvd1: 1;                     // 01
        uint16_t BRWN_OUT_GATING: 1;           // 02
        uint16_t BRWN_OUT_RESET: 1;            // 03
        uint16_t rsvd2: 7;                     // 10:04
        uint16_t AON_DBG_SEL: 5;               // 15:11
    };
} AON_FAST_0xF4_TYPE;


/* 0x120
*/
typedef union _AON_FAST_0x120_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ROUT_N_ANA_MODE: 1;             // 00
        uint16_t PAD_ROUT_P_ANA_MODE: 1;             // 01
        uint16_t PAD_LOUT_N_ANA_MODE: 1;             // 02
        uint16_t PAD_LOUT_P_ANA_MODE: 1;             // 03
        uint16_t PAD_MIC2_N_ANA_MODE: 1;             // 04
        uint16_t PAD_MIC2_P_ANA_MODE: 1;             // 05
        uint16_t PAD_MIC1_N_ANA_MODE: 1;             // 06
        uint16_t PAD_MIC1_P_ANA_MODE: 1;             // 07
        uint16_t rsvd: 8;                            // 15:08
    };
} AON_FAST_0x120_TYPE;

/* 0x122
*/
typedef union _AON_FAST_0x122_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 13;                          // 12:00
        uint16_t PAD_MICBIAS_ANA_MODE: 1;           // 13
        uint16_t PAD_AUX_R_ANA_MODE: 1;             // 14
        uint16_t PAD_AUX_L_ANA_MODE: 1;             // 15
    };
} AON_FAST_0x122_TYPE;


/* 0x12C
*/
typedef union _AON_FAST_WK_STATUS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 6;                      // 05:00
        uint16_t brwn_int_en: 1;               // 06
        uint16_t brwn_int_pol: 1;              // 07
        uint16_t brwn_det_intr: 1;             // 08
        uint16_t wakeup_blk_ind: 1;            // 09
        uint16_t PAD_P2_0_I: 1;                // 10
        uint16_t PAD_BOOT_FROM_FLASH_I: 1;     // 11
        uint16_t rsvd1: 4;                     // 15:12
    };
} AON_FAST_WK_STATUS_TYPE;

/* 0x154
*/
typedef union _AON_FAST_0x154_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 8;                 // 07:00
        uint16_t lp_xtal_sel: 2;          // 09:08
        uint16_t swr_ss_lpf_out: 6;       // 15:10
    };
} AON_FAST_0x154_TYPE;

/* 0x156
*/
typedef union _AON_FAST_0x156_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t lp_xtal_ppm_err: 6;           // 05:00
        uint16_t lp_xtal_ppm_plus: 1;          // 06
        uint16_t rsvd: 1;                      // 07
        uint16_t aon_dbgo_8_for_aon_clk: 1;    // 08
        uint16_t rsvd1: 6;                     // 14:09
        uint16_t en_ldo_pa_switch: 1;          // 15
    };
} AON_FAST_0x156_TYPE;

/* 0x15E
*/
typedef union _AON_FAST_0x15E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 3;                     // 02:00
        uint16_t MAC_TX_ON_PA_DL: 1;          // 03
        uint16_t quick_wakeup: 1;             // 04
        uint16_t rsvd1: 3;                    // 15:05
    };
} AON_FAST_0x15E_TYPE;

/* 0x164
*/
typedef union _AON_FAST_0x164_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 1;                       // 00
        uint16_t rsvd1: 1;                      // 01
        uint16_t r_PMUX_SCAN_FT_2_EN: 1;        // 02
        uint16_t r_PMUX_SCAN_FT_EN: 1;          // 03
        uint16_t r_PMUX_SCAN_FT_2_EN_src: 1;    // 04
        uint16_t r_PMUX_SCAN_FT_EN_src: 1;      // 05
        uint16_t r_PMUX_SCAN_MODE_EN: 1;        // 06
        uint16_t rsvd2: 1;                      // 07
        uint16_t rsvd3: 1;                      // 08
        uint16_t rsvd4: 5;                      // 13:09
        uint16_t PAD_MIC3_N_ANA_MODE: 1;        // 14
        uint16_t PAD_MIC3_P_ANA_MODE: 1;        // 15
    };
} AON_FAST_0x164_TYPE;

/* 0x166
*/
typedef union _AON_FAST_0x166_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XPDCK_VREF_OUT: 5;             // 04:00
        uint16_t XPDCK_READY: 1;                // 05
        uint16_t XPDCK_BUSY: 1;                 // 06
        uint16_t dummy: 1;                      // 07
        uint16_t XTAL_PDCK_LP: 1;               // 12:08
        uint16_t XTAL_PDCK_OK: 1;               // 13
        uint16_t XTAL_PDCK_BUSY: 1;             // 14
        uint16_t xtal_pdck_rslt_latch: 1;       // 15
    };
} AON_FAST_0x166_TYPE;

/* 0x172
*/
typedef union _AON_FAST_0x172_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rsvd: 1;                   // 00
        uint16_t r_clk_cpu_tick_sel: 1;     // 01
        uint16_t rsvd1: 14;                 // 15:02
    };
} AON_FAST_0x172_TYPE;


/* 0x17A
*/
typedef union _AON_FAST_0x17A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EFUSE_RP_LOCK: 2;              // 01:00
        uint16_t EFUSE_WP_LOCK: 2;              // 03:02
        uint16_t EFUSE_RP_EN: 2;                // 05:04
        uint16_t EFUSE_WP_EN: 2;                // 07:06
        uint16_t reg_aon_w1o_gpr_0: 8;          // 15:08
    };
} AON_FAST_0x17A_TYPE;

/* 0x17C
*/
typedef union _AON_FAST_0x17C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_1: 16;         // 15:00
    };
} AON_FAST_0x17C_TYPE;

/* 0x17E
*/
typedef union _AON_FAST_0x17E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_anc_ff_en_wp: 1;               // 00
        uint16_t reg_anc_fb_en_wp: 1;               // 01
        uint16_t reg_anc_fbpn_en_wp: 1;             // 02
        uint16_t reg_anc_lms_en_wp: 1;              // 03
        uint16_t sel_swr_ss_top: 1;                 // 04
        uint16_t rsvd: 11;                          // 15:05
    };
} AON_FAST_0x17E_TYPE;

/* 0x184
*/
typedef union _AON_FAST_0x184_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG_194_w1o_B7toB0: 8;                 // 07:00
        uint16_t reg_aon_w1o_bt4_func_dis: 1;           // 08
        uint16_t reg_aon_w1o_bt2_func_dis: 1;           // 09
        uint16_t reg_aon_w1o_handover_func_dis: 1;      // 10
        uint16_t reg_aon_w1o_sniff2_func_dis: 1;        // 11
        uint16_t reg_aon_w1o_sniff1_func_dis: 1;        // 12
        uint16_t reg_aon_w1o_sniff_half_slot_dis: 1;    // 13
        uint16_t reg_aon_w1o_fir_dis: 1;                // 14
        uint16_t rsvd: 1;                               // 15
    };
} AON_FAST_0x184_TYPE;

/* 0x186
*/
typedef union _AON_FAST_0x186_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_anc_io_en_wp: 8;         // 07:00
        uint16_t reg_anc_inst_rp: 1;          // 08
        uint16_t reg_anc_para_rp: 1;          // 09
        uint16_t reg_anc_w1o_spk1_dis: 1;     // 10
        uint16_t reg_anc_w1o_spk2_dis: 1;     // 11
        uint16_t reg_anc_w1o_mic1_dis: 1;     // 12
        uint16_t reg_anc_w1o_mic2_dis: 1;     // 13
        uint16_t reg_anc_w1o_mic3_dis: 1;     // 14
        uint16_t reg_anc_w1o_mic4_dis: 1;     // 15
    };
} AON_FAST_0x186_TYPE;

/* 0x188
*/
typedef union _AON_FAST_0x188_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t cpu_pc_15_0: 16;         // 15:00
    };
} AON_FAST_0x188_TYPE;

/* 0x18A
*/
typedef union _AON_FAST_0x18A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t cpu_pc_31_16: 16;         // 15:00
    };
} AON_FAST_0x18A_TYPE;

/* 0x18C
*/
typedef union _AON_FAST_0x18C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t cpu_lr_15_0: 16;         // 15:00
    };
} AON_FAST_0x18C_TYPE;


/* 0x18E
*/
typedef union _AON_FAST_0x18E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t cpu_lr_31_16: 16;         // 15:00
    };
} AON_FAST_0x18E_TYPE;


/* 0x190
*/
typedef union _AON_FAST_0x190_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t cpu_psr_15_0: 16;         // 15:00
    };
} AON_FAST_0x190_TYPE;

/* 0x192
*/
typedef union _AON_FAST_0x192_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t cpu_psr_31_16: 16;         // 15:00
    };
} AON_FAST_0x192_TYPE;

/* 0x194
*/
typedef union _AON_FAST_0x194_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t TX_LDO_PA_TUNE_RL6736: 8;                  // 07:00
        uint16_t TX_LDO_PA_TUNE_RL6736_reserved: 8;         // 15:08
    };
} AON_FAST_0x194_TYPE;


/* 0x400
    00      R/W bootup_before                               1'b0
    01      R/W need_restore                                1'b0
    02      R/W dvfs_normal_vdd_mode                        1'b0
    15:03   R/W reserved                                    13'b0
 */
typedef union _AON_FAST_BOOT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t bootup_before:         1;  /* bit[0]: 0: first boot up                     */
        uint16_t need_restore:          1;  /* bit[1]: need restore flow                    */
        uint16_t dvfs_normal_vdd_mode:  1;  /* bit[2]: reference to DVFSVDDMode             */
        uint16_t rsvd:                  13; /* bit[15:3]: reserved                          */
    };
} AON_FAST_BOOT_TYPE;

/* 0x402
    15:0    R/W FW_GENERAL_REG1X                            16'h0
 */
typedef union _AON_FAST_REG1X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG1X;
    };
} AON_FAST_REG1X_FW_GENERAL_TYPE;

/* 0x404
    15:0    R/W FW_GENERAL_REG2X                            16'h0
 */
typedef union _AON_FAST_REG2X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG2X;
    };
} AON_FAST_REG2X_FW_GENERAL_TYPE;

/* 0x406
    15:0    R/W FW_GENERAL_REG3X                            16'h0
 */
typedef union _AON_FAST_REG3X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG3X;
    };
} AON_FAST_REG3X_FW_GENERAL_TYPE;

/* 0x408
    15:0    R/W FW_GENERAL_REG4X                            16'h0
 */
typedef union _AON_FAST_REG4X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG4X;
    };
} AON_FAST_REG4X_FW_GENERAL_TYPE;

/* 0x40A
    15:0    R/W FW_GENERAL_REG5X                            16'h0
 */
typedef union _AON_FAST_REG5X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG5X;
    };
} AON_FAST_REG5X_FW_GENERAL_TYPE;

/* 0x40C
    15:0    R/W FW_GENERAL_REG6X                            16'h0
 */
typedef union _AON_FAST_REG6X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG6X;
    };
} AON_FAST_REG6X_FW_GENERAL_TYPE;

/* 0x40E
    15:0    R/W FW_GENERAL_REG7X                            16'h0
 */
typedef union _AON_FAST_REG7X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG7X;
    };
} AON_FAST_REG7X_FW_GENERAL_TYPE;

/* 0x410
    15:0    R/W FW_GENERAL_REG8X                            16'h0
 */
typedef union _AON_FAST_REG8X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG8X;
    };
} AON_FAST_REG8X_FW_GENERAL_TYPE;

/* 0x412
    15:0    R/W FW_GENERAL_REG9X                            16'h0
 */
typedef union _AON_FAST_REG9X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG9X;
    };
} AON_FAST_REG9X_FW_GENERAL_TYPE;

/* 0x414
    15:0    R/W FW_GENERAL_REG10X                           16'h0
 */
typedef union _AON_FAST_REG10X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG10X;
    };
} AON_FAST_REG10X_FW_GENERAL_TYPE;

/* 0x416
    15:0    R/W FW_GENERAL_REG11X                           16'h0
 */
typedef union _AON_FAST_REG11X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG11X;
    };
} AON_FAST_REG11X_FW_GENERAL_TYPE;

/* 0x418
    15:0    R/W FW_GENERAL_REG12X                           16'h0
 */
typedef union _AON_FAST_REG12X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG12X;
    };
} AON_FAST_REG12X_FW_GENERAL_TYPE;

/* 0x41A
    15:0    R/W FW_GENERAL_REG13X                           16'h0
 */
typedef union _AON_FAST_REG13X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG13X;
    };
} AON_FAST_REG13X_FW_GENERAL_TYPE;

/* 0x41C
    15:0    R/W FW_GENERAL_REG14X                           16'h0
 */
typedef union _AON_FAST_REG14X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG14X;
    };
} AON_FAST_REG14X_FW_GENERAL_TYPE;

/* 0x41E
    15:0    R/W FW_GENERAL_REG15X                           16'h0
 */
typedef union _AON_FAST_REG15X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG15X;
    };
} AON_FAST_REG15X_FW_GENERAL_TYPE;

/* 0x420
    15:0    R/W FW_GENERAL_REG16X                           16'h0
 */
typedef union _AON_FAST_REG16X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG16X;
    };
} AON_FAST_REG16X_FW_GENERAL_TYPE;

/* 0x422
    15:0    R/W FW_GENERAL_REG17X                           16'h0
 */
typedef union _AON_FAST_REG17X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG17X;
    };
} AON_FAST_REG17X_FW_GENERAL_TYPE;

/* 0x580
    0       R/W reg_ic_data_to_itcm1_en                     1'h0
    15:1    R/W MEMORY_REG0X_DUMMY1                         15'h0
 */
typedef union _AON_FAST_RG0X_MEMORY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ic_data_to_itcm1_en: 1;
        uint16_t MEMORY_REG0X_DUMMY1: 15;
    };
} AON_FAST_RG0X_MEMORY_TYPE;

/* 0x582
    0       R/W dummy                        1'h0
    1       R/W dummy1                       1'h0
    15:2    R/W MEMORY_REG1X_DUMMY1          14'h0
 */
typedef union _AON_FAST_RG1X_MEMORY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t dummy: 1;
        uint16_t dummy1: 1;
        uint16_t MEMORY_REG1X_DUMMY1: 14;
    };
} AON_FAST_RG1X_MEMORY_TYPE;

/* 0x584
    0       R/W ram_rx2_vsel                                1'b1
    1       R/W ram_rx2_cache_vsel                          1'b1
    2       R/W ram_buf_vsel                                1'b1
    3       R/W ram_mac_vsel                                1'b1
    4       R/W ram_mdm_vsel                                1'b1
    5       R/W rom_rx2_vsel                                1'b1
    6       R/W rom_dsp1_vsel                               1'b1
    7       R/W ram_dsp1_vsel                               1'b1
    8       R/W ram_data_vsel                               1'b1
    9       R/W ram_rfc_vsel                                1'b1
    10      R/W vsel_dummy0                                 1'b1
    11      R/W vsel_dummy1                                 1'b1
    12      R/W vsel_dummy2                                 1'b1
    13      R/W vsel_dummy3                                 1'b1
    14      R/W reg_data_dspram_0_en                        1'b1
    15      R/W reg_data_dspram_1_en                        1'b1
 */
typedef union _AON_FAST_REG_RAMROM_VSEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ram_rx2_vsel: 1;
        uint16_t ram_rx2_cache_vsel: 1;
        uint16_t ram_buf_vsel: 1;
        uint16_t ram_mac_vsel: 1;
        uint16_t ram_mdm_vsel: 1;
        uint16_t rom_rx2_vsel: 1;
        uint16_t rom_dsp1_vsel: 1;
        uint16_t ram_dsp1_vsel: 1;
        uint16_t ram_data_vsel: 1;
        uint16_t ram_rfc_vsel: 1;
        uint16_t vsel_dummy0: 1;
        uint16_t vsel_dummy1: 1;
        uint16_t vsel_dummy2: 1;
        uint16_t vsel_dummy3: 1;
        uint16_t reg_data_dspram_0_en: 1;
        uint16_t reg_data_dspram_1_en: 1;
    };
} AON_FAST_REG_RAMROM_VSEL_TYPE;

/* 0x586
    5:0     R/W reg_rom_mcu_hv                              6'b010011
    11:6    R/W reg_rom_mcu_lv                              6'b010001
    15:12   R/W REG_ROM_MCU_DUMMY1                          4'h0
 */
typedef union _AON_FAST_REG_ROM_MCU_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_mcu_hv: 6;
        uint16_t reg_rom_mcu_lv: 6;
        uint16_t REG_ROM_MCU_DUMMY1: 4;
    };
} AON_FAST_REG_ROM_MCU_TYPE;

/* 0x588
    5:0     R/W reg_rom_dsp1_hv                             6'b010011
    11:6    R/W reg_rom_dsp1_lv                             6'b010001
    15:12   R/W REG_ROM_DSP1_HV_LV_DUMMY1                   4'h0
 */
typedef union _AON_FAST_REG_ROM_DSP1_HV_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_dsp1_hv: 6;
        uint16_t reg_rom_dsp1_lv: 6;
        uint16_t REG_ROM_DSP1_HV_LV_DUMMY1: 4;
    };
} AON_FAST_REG_ROM_DSP1_HV_LV_TYPE;

/* 0x58A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG_ROM_RFC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG_ROM_RFC_TYPE;

/* 0x58C
    13:0    R/W reg_ram_rx2_hv                              14'b00001000010011
    15:14   R/W REG_RAM_RX2_HV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_RX2_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rx2_hv: 14;
        uint16_t REG_RAM_RX2_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_RX2_HV_TYPE;

/* 0x58E
    13:0    R/W reg_ram_rx2_lv                              14'b00001100110000
    15:14   R/W REG_RAM_RX2_LV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_RX2_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rx2_lv: 14;
        uint16_t REG_RAM_RX2_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_RX2_LV_TYPE;

/* 0x590
    13:0    R/W reg_ram_rx2_cache_hv                        14'b00001000010011
    15:14   R/W REG_RAM_RX2_CACHE_HV_DUMMY1                 2'h0
 */
typedef union _AON_FAST_REG_RAM_RX2_CACHE_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rx2_cache_hv: 14;
        uint16_t REG_RAM_RX2_CACHE_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_RX2_CACHE_HV_TYPE;

/* 0x592
    13:0    R/W reg_ram_rx2_cache_lv                        14'b00001100110000
    15:14   R/W REG_RAM_RX2_CACHE_LV_DUMMY1                 2'h0
 */
typedef union _AON_FAST_REG_RAM_RX2_CACHE_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rx2_cache_lv: 14;
        uint16_t REG_RAM_RX2_CACHE_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_RX2_CACHE_LV_TYPE;

/* 0x594
    13:0    R/W reg_ram_buf_hv                              14'b00001000010011
    15:14   R/W REG_RAM_BUF_HV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_BUF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_buf_hv: 14;
        uint16_t REG_RAM_BUF_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_BUF_HV_TYPE;

/* 0x596
    13:0    R/W reg_ram_buf_lv                              14'b00001100110000
    15:14   R/W REG_RAM_BUF_LV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_BUF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_buf_lv: 14;
        uint16_t REG_RAM_BUF_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_BUF_LV_TYPE;

/* 0x598
    13:0    R/W reg_ram_sdio                                14'b00001000010011
    15:14   R/W REG_RAM_SDIO_DUMMY1                         2'h0
 */
typedef union _AON_FAST_REG_RAM_SDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_sdio: 14;
        uint16_t REG_RAM_SDIO_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_SDIO_TYPE;

/* 0x59A
    13:0    R/W reg_ram_usb                                 14'b00001000010011
    15:14   R/W REG_RAM_USB_DUMMY1                          2'h0
 */
typedef union _AON_FAST_REG_RAM_USB_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_usb: 14;
        uint16_t REG_RAM_USB_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_USB_TYPE;

/* 0x59C
    13:0    R/W reg_ram_mac_fifo_hv                         14'b00001000010011
    15:14   R/W REG_RAM_MAC_HV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_MAC_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mac_fifo_hv: 14;
        uint16_t REG_RAM_MAC_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_MAC_HV_TYPE;

/* 0x59E
    13:0    R/W reg_ram_mac_fifo_lv                         14'b00001100110000
    15:14   R/W REG_RAM_MAC_LV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_MAC_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mac_fifo_lv: 14;
        uint16_t REG_RAM_MAC_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_MAC_LV_TYPE;

/* 0x5A0
    13:0    R/W reg_ram_mdm_hv                              14'b00001000010011
    15:14   R/W REG_RAM_MDM_HV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_MDM_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mdm_hv: 14;
        uint16_t REG_RAM_MDM_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_MDM_HV_TYPE;

/* 0x5A2
    13:0    R/W reg_ram_mdm_lv                              14'b00001100110000
    15:14   R/W REG_RAM_MDM_LV_DUMMY1                       2'h0
 */
typedef union _AON_FAST_REG_RAM_MDM_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mdm_lv: 14;
        uint16_t REG_RAM_MDM_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_MDM_LV_TYPE;

/* 0x5A4
    13:0    R/W reg_ram_dsp1_hv                             14'b00001000010011
    15:14   R/W REG_RAM_DSP1_HV_DUMMY1                      2'h0
 */
typedef union _AON_FAST_REG_RAM_DSP1_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_hv: 14;
        uint16_t REG_RAM_DSP1_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_DSP1_HV_TYPE;

/* 0x5A6
    13:0    R/W reg_ram_dsp1_lv                             14'b00001100110000
    15:14   R/W REG_RAM_DSP1_LV_DUMMY1                      2'h0
 */
typedef union _AON_FAST_REG_RAM_DSP1_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_lv: 14;
        uint16_t REG_RAM_DSP1_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_DSP1_LV_TYPE;

/* 0x5A8
    13:0    R/W reg_ram_data_hv                             14'b00001000010011
    15:14   R/W REG_RAM_DATA_HV_DUMMY1                      2'h0
 */
typedef union _AON_FAST_REG_RAM_DATA_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_data_hv: 14;
        uint16_t REG_RAM_DATA_HV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_DATA_HV_TYPE;

/* 0x5AA
    13:0    R/W reg_ram_data_lv                             14'b00001100110000
    15:14   R/W REG_RAM_DATA_LV_DUMMY1                      2'h0
 */
typedef union _AON_FAST_REG_RAM_DATA_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_data_lv: 14;
        uint16_t REG_RAM_DATA_LV_DUMMY1: 2;
    };
} AON_FAST_REG_RAM_DATA_LV_TYPE;

/* 0x5AC
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG_RAM_RFC_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG_RAM_RFC_HV_TYPE;

/* 0x5AE
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG_RAM_RFC_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG_RAM_RFC_LV_TYPE;

/* 0x5B0
    15:0    R/W r_DS_rx2_itcm1_iso0[15:0]                   16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_itcm1_iso0_15_0;
    };
} AON_FAST_SRAM_DS_ISO0_1_TYPE;

/* 0x5B2
    13:0    R/W r_DS_rx2_itcm1_iso0[29:16]                  14'h0
    15:14   R/W SRAM_DS_ISO0_2_DUMMY1                       2'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_itcm1_iso0_29_16: 14;
        uint16_t SRAM_DS_ISO0_2_DUMMY1: 2;
    };
} AON_FAST_SRAM_DS_ISO0_2_TYPE;

/* 0x5B4
    12:0    R/W r_DS_rx2_dtcm0_iso0                         13'h0
    15:13   R/W SRAM_DS_ISO0_3_DUMMY1                       3'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_dtcm0_iso0: 13;
        uint16_t SRAM_DS_ISO0_3_DUMMY1: 3;
    };
} AON_FAST_SRAM_DS_ISO0_3_TYPE;

/* 0x5B6
    15:0    R/W r_DS_rx2_dtcm1_iso0                         16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_dtcm1_iso0;
    };
} AON_FAST_SRAM_DS_ISO0_4_TYPE;

/* 0x5B8
    6:0     R/W r_DS_rx2_cache_iso0                         7'h0
    15:7    R/W SRAM_DS_ISO0_5_DUMMY1                       9'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_cache_iso0: 7;
        uint16_t SRAM_DS_ISO0_5_DUMMY1: 9;
    };
} AON_FAST_SRAM_DS_ISO0_5_TYPE;

/* 0x5BA
    7:0     R/W r_DS_buf_iso0                               8'h0
    11:8    R/W SRAM_DS_ISO0_6_DUMMY1                       4'h0
    12      R/W r_DS_master_cam_iso0                        1'h0
    13      R/W r_DS_lut_cam_iso0                           1'h0
    14      R/W r_DS_slave_cam_iso0                         1'h0
    15      R/W r_DS_pa_cam_iso0                            1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_buf_iso0: 8;
        uint16_t SRAM_DS_ISO0_6_DUMMY1: 4;
        uint16_t r_DS_master_cam_iso0: 1;
        uint16_t r_DS_lut_cam_iso0: 1;
        uint16_t r_DS_slave_cam_iso0: 1;
        uint16_t r_DS_pa_cam_iso0: 1;
    };
} AON_FAST_SRAM_DS_ISO0_6_TYPE;

/* 0x5BC
    15:0    R/W r_DS_dsp1_ram_iso0[15:0]                    16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso0_15_0;
    };
} AON_FAST_SRAM_DS_ISO0_7_TYPE;

/* 0x5BE
    15:0    R/W r_DS_dsp1_ram_iso0[31:16]                   16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso0_31_16;
    };
} AON_FAST_SRAM_DS_ISO0_8_TYPE;

/* 0x5C0
    7:0     R/W r_DS_dsp1_ram_iso0[39:32]                   8'h0
    15:8    R/W SRAM_DS_ISO0_9_DUMMY1                       8'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso0_39_32: 8;
        uint16_t SRAM_DS_ISO0_9_DUMMY1: 8;
    };
} AON_FAST_SRAM_DS_ISO0_9_TYPE;

/* 0x5C2
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_SRAM_DS_ISO0_10_TYPE;

/* 0x5C4
    15:0    R/W r_DS_rx2_itcm1_iso1[15:0]                   16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_itcm1_iso1_15_0;
    };
} AON_FAST_SRAM_DS_ISO1_1_TYPE;

/* 0x5C6
    13:0    R/W r_DS_rx2_itcm1_iso1[29:16]                  14'h0
    15:14   R/W SRAM_DS_ISO1_2_DUMMY1                       2'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_itcm1_iso1_29_16: 14;
        uint16_t SRAM_DS_ISO1_2_DUMMY1: 2;
    };
} AON_FAST_SRAM_DS_ISO1_2_TYPE;

/* 0x5C8
    12:0    R/W r_DS_rx2_dtcm0_iso1                         13'h0
    15:13   R/W SRAM_DS_ISO1_3_DUMMY1                       3'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_dtcm0_iso1: 13;
        uint16_t SRAM_DS_ISO1_3_DUMMY1: 3;
    };
} AON_FAST_SRAM_DS_ISO1_3_TYPE;

/* 0x5CA
    15:0    R/W r_DS_rx2_dtcm1_iso1                         16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_dtcm1_iso1;
    };
} AON_FAST_SRAM_DS_ISO1_4_TYPE;

/* 0x5CC
    6:0     R/W r_DS_rx2_cache_iso1                         7'h0
    15:7    R/W SRAM_DS_ISO1_5_DUMMY1                       9'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_rx2_cache_iso1: 7;
        uint16_t SRAM_DS_ISO1_5_DUMMY1: 9;
    };
} AON_FAST_SRAM_DS_ISO1_5_TYPE;

/* 0x5CE
    7:0     R/W r_DS_buf_iso1                               8'h0
    11:8    R/W SRAM_DS_ISO1_6_DUMMY1                       4'h0
    12      R/W r_DS_master_cam_iso1                        1'h0
    13      R/W r_DS_lut_cam_iso1                           1'h0
    14      R/W r_DS_slave_cam_iso1                         1'h0
    15      R/W r_DS_pa_cam_iso1                            1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_buf_iso1: 8;
        uint16_t SRAM_DS_ISO1_6_DUMMY1: 4;
        uint16_t r_DS_master_cam_iso1: 1;
        uint16_t r_DS_lut_cam_iso1: 1;
        uint16_t r_DS_slave_cam_iso1: 1;
        uint16_t r_DS_pa_cam_iso1: 1;
    };
} AON_FAST_SRAM_DS_ISO1_6_TYPE;

/* 0x5D0
    15:0    R/W r_DS_dsp1_ram_iso1[15:0]                    16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso1_15_0;
    };
} AON_FAST_SRAM_DS_ISO1_7_TYPE;

/* 0x5D2
    15:0    R/W r_DS_dsp1_ram_iso1[31:16]                   16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso1_31_16;
    };
} AON_FAST_SRAM_DS_ISO1_8_TYPE;

/* 0x5D4
    7:0     R/W r_DS_dsp1_ram_iso1[39:32]                   8'h0
    15:8    R/W SRAM_DS_ISO1_9_DUMMY1                       8'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso1_39_32: 8;
        uint16_t SRAM_DS_ISO1_9_DUMMY1: 8;
    };
} AON_FAST_SRAM_DS_ISO1_9_TYPE;

/* 0x5D6
    15:0    R/W RSVD                                        16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_SRAM_DS_ISO1_10_TYPE;

/* 0x5D8
    15:0    R/W r_SD_rx2_itcm1_iso0[15:0]                   16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_itcm1_iso0_15_0;
    };
} AON_FAST_SRAM_SD_ISO0_1_TYPE;

/* 0x5DA
    13:0    R/W r_SD_rx2_itcm1_iso0[29:16]                  14'h0
    15:14   R/W SRAM_SD_ISO0_2_DUMMY1                       2'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_itcm1_iso0_29_16: 14;
        uint16_t SRAM_SD_ISO0_2_DUMMY1: 2;
    };
} AON_FAST_SRAM_SD_ISO0_2_TYPE;

/* 0x5DC
    12:0    R/W r_SD_rx2_dtcm0_iso0                         13'h0
    15:13   R/W SRAM_SD_ISO0_3_DUMMY1                       3'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_dtcm0_iso0: 13;
        uint16_t SRAM_SD_ISO0_3_DUMMY1: 3;
    };
} AON_FAST_SRAM_SD_ISO0_3_TYPE;

/* 0x5DE
    15:0    R/W r_SD_rx2_dtcm1_iso0                         16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_dtcm1_iso0;
    };
} AON_FAST_SRAM_SD_ISO0_4_TYPE;

/* 0x5E0
    6:0     R/W r_SD_rx2_cache_iso0                         7'h0
    15:7    R/W SRAM_SD_ISO0_5_DUMMY1                       9'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_cache_iso0: 7;
        uint16_t SRAM_SD_ISO0_5_DUMMY1: 9;
    };
} AON_FAST_SRAM_SD_ISO0_5_TYPE;

/* 0x5E2
    7:0     R/W r_SD_buf_iso0                               8'h0
    11:8    R/W SRAM_SD_ISO0_6_DUMMY1                       4'h0
    12      R/W r_SD_master_cam_iso0                        1'h0
    13      R/W r_SD_lut_cam_iso0                           1'h0
    14      R/W r_SD_slave_cam_iso0                         1'h0
    15      R/W r_SD_pa_cam_iso0                            1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_buf_iso0: 8;
        uint16_t SRAM_SD_ISO0_6_DUMMY1: 4;
        uint16_t r_SD_master_cam_iso0: 1;
        uint16_t r_SD_lut_cam_iso0: 1;
        uint16_t r_SD_slave_cam_iso0: 1;
        uint16_t r_SD_pa_cam_iso0: 1;
    };
} AON_FAST_SRAM_SD_ISO0_6_TYPE;

/* 0x5E4
    15:0    R/W r_SD_dsp1_ram_iso0[15:0]                    16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso0_15_0;
    };
} AON_FAST_SRAM_SD_ISO0_7_TYPE;

/* 0x5E6
    15:0    R/W r_SD_dsp1_ram_iso0[31:16]                   16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso0_31_16;
    };
} AON_FAST_SRAM_SD_ISO0_8_TYPE;

/* 0x5E8
    7:0     R/W r_SD_dsp1_ram_iso0[39:32]                   8'h0
    15:8    R/W SRAM_SD_ISO0_9_DUMMY1                       8'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso0_39_32: 8;
        uint16_t SRAM_SD_ISO0_9_DUMMY1: 8;
    };
} AON_FAST_SRAM_SD_ISO0_9_TYPE;

/* 0x5EA
    15:0    R/W RSVD                                        16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_SRAM_SD_ISO0_10_TYPE;

/* 0x5EC
    15:0    R/W r_SD_rx2_itcm1_iso1[15:0]                   16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_itcm1_iso1_15_0;
    };
} AON_FAST_SRAM_SD_ISO1_1_TYPE;

/* 0x5EE
    13:0    R/W r_SD_rx2_itcm1_iso1[29:16]                  14'h0
    15:14   R/W SRAM_SD_ISO1_2_DUMMY1                       2'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_itcm1_iso1_29_16: 14;
        uint16_t SRAM_SD_ISO1_2_DUMMY1: 2;
    };
} AON_FAST_SRAM_SD_ISO1_2_TYPE;

/* 0x5F0
    12:0    R/W r_SD_rx2_dtcm0_iso1                         13'h0
    15:13   R/W SRAM_SD_ISO1_3_DUMMY1                       3'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_dtcm0_iso1: 13;
        uint16_t SRAM_SD_ISO1_3_DUMMY1: 3;
    };
} AON_FAST_SRAM_SD_ISO1_3_TYPE;

/* 0x5F2
    15:0    R/W r_SD_rx2_dtcm1_iso1                         16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_dtcm1_iso1;
    };
} AON_FAST_SRAM_SD_ISO1_4_TYPE;

/* 0x5F4
    6:0     R/W r_SD_rx2_cache_iso1                         7'h0
    15:7    R/W SRAM_SD_ISO1_5_DUMMY1                       9'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rx2_cache_iso1: 7;
        uint16_t SRAM_SD_ISO1_5_DUMMY1: 9;
    };
} AON_FAST_SRAM_SD_ISO1_5_TYPE;

/* 0x5F6
    7:0     R/W r_SD_buf_iso1                               8'h0
    11:8    R/W SRAM_SD_ISO1_6_DUMMY1                       4'h0
    12      R/W r_SD_master_cam_iso1                        1'h0
    13      R/W r_SD_lut_cam_iso1                           1'h0
    14      R/W r_SD_slave_cam_iso1                         1'h0
    15      R/W r_SD_pa_cam_iso1                            1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_buf_iso1: 8;
        uint16_t SRAM_SD_ISO1_6_DUMMY1: 4;
        uint16_t r_SD_master_cam_iso1: 1;
        uint16_t r_SD_lut_cam_iso1: 1;
        uint16_t r_SD_slave_cam_iso1: 1;
        uint16_t r_SD_pa_cam_iso1: 1;
    };
} AON_FAST_SRAM_SD_ISO1_6_TYPE;

/* 0x5F8
    15:0    R/W r_SD_dsp1_ram_iso1[15:0]                    16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso1_15_0;
    };
} AON_FAST_SRAM_SD_ISO1_7_TYPE;

/* 0x5FA
    15:0    R/W r_SD_dsp1_ram_iso1[31:16]                   16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso1_31_16;
    };
} AON_FAST_SRAM_SD_ISO1_8_TYPE;

/* 0x5FC
    7:0     R/W r_SD_dsp1_ram_iso1[39:32]                   8'h0
    15:8    R/W SRAM_SD_ISO1_9_DUMMY1                       8'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso1_39_32: 8;
        uint16_t SRAM_SD_ISO1_9_DUMMY1: 8;
    };
} AON_FAST_SRAM_SD_ISO1_9_TYPE;

/* 0x5FE
    15:0    R/W RSVD                                        16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_SRAM_SD_ISO1_10_TYPE;

/* 0x600
    0       R/W r_LS_mcu_rom                                1'h0
    1       R/W r_LS_dsp1_rom                               1'h0
    13:2    R/W ROM_LS_VCORE_1_DUMMY1                       12'h0
    14      R/W r_LS_usb                                    1'h0
    15      R/W r_LS_sdh                                    1'h0
 */
typedef union _AON_FAST_ROM_LS_VCORE_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_LS_mcu_rom: 1;
        uint16_t r_LS_dsp1_rom: 1;
        uint16_t ROM_LS_VCORE_1_DUMMY1: 12;
        uint16_t r_LS_usb: 1;
        uint16_t r_LS_sdh: 1;
    };
} AON_FAST_ROM_LS_VCORE_1_TYPE;

/* 0x602
    15:0    R/W SRAM_LS_VCORE_1_DUMMY1                      16'h0
 */
typedef union _AON_FAST_SRAM_LS_VCORE_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SRAM_LS_VCORE_1_DUMMY1;
    };
} AON_FAST_SRAM_LS_VCORE_1_TYPE;

/* 0x604
    15:0    R/W r_RM3_rx2_itcm1_iso0[15:0]                  16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_itcm1_iso0_15_0;
    };
} AON_FAST_SRAM_RM3_ISO0_1_TYPE;

/* 0x606
    13:0    R/W r_RM3_rx2_itcm1_iso0[29:16]                 14'h0
    15:14   R/W SRAM_RM3_ISO0_2_DUMMY1                      2'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_itcm1_iso0_29_16: 14;
        uint16_t SRAM_RM3_ISO0_2_DUMMY1: 2;
    };
} AON_FAST_SRAM_RM3_ISO0_2_TYPE;

/* 0x608
    12:0    R/W r_RM3_rx2_dtcm0_iso0                        13'h0
    15:13   R/W SRAM_RM3_ISO0_3_DUMMY1                      3'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_dtcm0_iso0: 13;
        uint16_t SRAM_RM3_ISO0_3_DUMMY1: 3;
    };
} AON_FAST_SRAM_RM3_ISO0_3_TYPE;

/* 0x60A
    15:0    R/W r_RM3_rx2_dtcm1_iso0                        16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_dtcm1_iso0;
    };
} AON_FAST_SRAM_RM3_ISO0_4_TYPE;

/* 0x60C
    6:0     R/W r_RM3_rx2_cache_iso0                        7'h0
    15:7    R/W SRAM_RM3_ISO0_5_DUMMY1                      9'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_cache_iso0: 7;
        uint16_t SRAM_RM3_ISO0_5_DUMMY1: 9;
    };
} AON_FAST_SRAM_RM3_ISO0_5_TYPE;

/* 0x60E
    7:0     R/W r_RM3_buf_iso0                              8'h0
    11:8    R/W SRAM_RM3_ISO0_6_DUMMY1                      4'h0
    12      R/W r_RM3_master_cam_iso0                       1'h0
    13      R/W r_RM3_lut_cam_iso0                          1'h0
    14      R/W r_RM3_slave_cam_iso0                        1'h0
    15      R/W r_RM3_pa_cam_iso0                           1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_buf_iso0: 8;
        uint16_t SRAM_RM3_ISO0_6_DUMMY1: 4;
        uint16_t r_RM3_master_cam_iso0: 1;
        uint16_t r_RM3_lut_cam_iso0: 1;
        uint16_t r_RM3_slave_cam_iso0: 1;
        uint16_t r_RM3_pa_cam_iso0: 1;
    };
} AON_FAST_SRAM_RM3_ISO0_6_TYPE;

/* 0x610
    15:0    R/W r_RM3_dsp1_ram_iso0[15:0]                   16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso0_15_0;
    };
} AON_FAST_SRAM_RM3_ISO0_7_TYPE;

/* 0x612
    15:0    R/W r_RM3_dsp1_ram_iso0[31:16]                  16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso0_31_16;
    };
} AON_FAST_SRAM_RM3_ISO0_8_TYPE;

/* 0x614
    7:0     R/W r_RM3_dsp1_ram_iso0[39:32]                  8'h0
    15:8    R/W SRAM_RM3_ISO0_9_DUMMY1                      8'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso0_39_32: 8;
        uint16_t SRAM_RM3_ISO0_9_DUMMY1: 8;
    };
} AON_FAST_SRAM_RM3_ISO0_9_TYPE;

/* 0x616
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_SRAM_RM3_ISO0_10_TYPE;

/* 0x618
    15:0    R/W r_RM3_rx2_itcm1_iso1[15:0]                  16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_itcm1_iso1_15_0;
    };
} AON_FAST_SRAM_RM3_ISO1_1_TYPE;

/* 0x61A
    13:0    R/W r_RM3_rx2_itcm1_iso1[29:16]                 14'h0
    15:14   R/W SRAM_RM3_ISO1_2_DUMMY1                      2'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_itcm1_iso1_29_16: 14;
        uint16_t SRAM_RM3_ISO1_2_DUMMY1: 2;
    };
} AON_FAST_SRAM_RM3_ISO1_2_TYPE;

/* 0x61C
    12:0    R/W r_RM3_rx2_dtcm0_iso1                        13'h0
    15:13   R/W SRAM_RM3_ISO1_3_DUMMY1                      3'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_dtcm0_iso1: 13;
        uint16_t SRAM_RM3_ISO1_3_DUMMY1: 3;
    };
} AON_FAST_SRAM_RM3_ISO1_3_TYPE;

/* 0x61E
    15:0    R/W r_RM3_rx2_dtcm1_iso1                        16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_dtcm1_iso1;
    };
} AON_FAST_SRAM_RM3_ISO1_4_TYPE;

/* 0x620
    6:0     R/W r_RM3_rx2_cache_iso1                        7'h0
    15:7    R/W SRAM_RM3_ISO1_5_DUMMY1                      9'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_rx2_cache_iso1: 7;
        uint16_t SRAM_RM3_ISO1_5_DUMMY1: 9;
    };
} AON_FAST_SRAM_RM3_ISO1_5_TYPE;

/* 0x622
    7:0     R/W r_RM3_buf_iso1                              8'h0
    11:8    R/W SRAM_RM3_ISO1_6_DUMMY1                      4'h0
    12      R/W r_RM3_master_cam_iso1                       1'h0
    13      R/W r_RM3_lut_cam_iso1                          1'h0
    14      R/W r_RM3_slave_cam_iso1                        1'h0
    15      R/W r_RM3_pa_cam_iso1                           1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_buf_iso1: 8;
        uint16_t SRAM_RM3_ISO1_6_DUMMY1: 4;
        uint16_t r_RM3_master_cam_iso1: 1;
        uint16_t r_RM3_lut_cam_iso1: 1;
        uint16_t r_RM3_slave_cam_iso1: 1;
        uint16_t r_RM3_pa_cam_iso1: 1;
    };
} AON_FAST_SRAM_RM3_ISO1_6_TYPE;

/* 0x624
    15:0    R/W r_RM3_dsp1_ram_iso1[15:0]                   16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso1_15_0;
    };
} AON_FAST_SRAM_RM3_ISO1_7_TYPE;

/* 0x626
    15:0    R/W r_RM3_dsp1_ram_iso1[31:16]                  16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso1_31_16;
    };
} AON_FAST_SRAM_RM3_ISO1_8_TYPE;

/* 0x628
    7:0     R/W r_RM3_dsp1_ram_iso1[39:32]                  8'h0
    15:8    R/W SRAM_RM3_ISO1_9_DUMMY1                      8'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso1_39_32: 8;
        uint16_t SRAM_RM3_ISO1_9_DUMMY1: 8;
    };
} AON_FAST_SRAM_RM3_ISO1_9_TYPE;

/* 0x6F0
    1:0     R/W BTPLL_SYS_REG0X_DUMMY1                      2'b00
    2       R/W BTPLL_SYS_REG0X_DUMMY2                      1'b1
    3       R/W BTPLL_SYS_REG0X_DUMMY3                      1'b1
    4       R/W ISO_PLL2                                    1'b1
    5       R/W ISO_PLL                                     1'b1
    6       R/W BTPLL_SYS_REG0X_DUMMY6                      1'b0
    7       R/W BTPLL_SYS_REG0X_DUMMY7                      1'b0
    8       R/W BTPLL_SYS_REG0X_DUMMY8                      1'b0
    9       R/W BTPLL_SYS_REG0X_DUMMY9                      1'b0
    10      R/W BTPLL_SYS_REG0X_DUMMY10                     1'b0
    11      R/W BTPLL_SYS_REG0X_DUMMY11                     1'b0
    12      R/W BTPLL_SYS_REG0X_DUMMY12                     1'b0
    13      R/W BTPLL_SYS_REG0X_DUMMY13                     1'b0
    14      R/W BTPLL_SYS_REG0X_DUMMY14                     1'b0
    15      R/W BTPLL_SYS_REG0X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_REG0X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_SYS_REG0X_DUMMY1: 2;
        uint16_t BTPLL_SYS_REG0X_DUMMY2: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY3: 1;
        uint16_t ISO_PLL2: 1;
        uint16_t ISO_PLL: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY6: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY7: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY8: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY9: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY10: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY11: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY12: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY13: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY14: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_BTPLL_SYS_TYPE;

/* 0x6F2
    7:0     R/W BTPLL_SYS_REG1X_DUMMY0                      8'h0
    15:8    R/W BTPLL_SYS_REG1X_DUMMY8                      8'h0
 */
typedef union _AON_FAST_REG1X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_SYS_REG1X_DUMMY0: 8;
        uint16_t BTPLL_SYS_REG1X_DUMMY8: 8;
    };
} AON_FAST_REG1X_BTPLL_SYS_TYPE;

/* 0x6F4
    7:0     R/W BTPLL_SYS_REG2X_DUMMY0                      8'h0
    15:8    R/W BTPLL_SYS_REG2X_DUMMY8                      8'h0
 */
typedef union _AON_FAST_REG2X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_SYS_REG2X_DUMMY0: 8;
        uint16_t BTPLL_SYS_REG2X_DUMMY8: 8;
    };
} AON_FAST_REG2X_BTPLL_SYS_TYPE;

/* 0x6F6
    7:0     R/W BTPLL_SYS_REG3X_DUMMY0                      8'h0
    15:8    R/W BTPLL_SYS_REG3X_DUMMY8                      8'h0
 */
typedef union _AON_FAST_REG3X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_SYS_REG3X_DUMMY0: 8;
        uint16_t BTPLL_SYS_REG3X_DUMMY8: 8;
    };
} AON_FAST_REG3X_BTPLL_SYS_TYPE;

/* 0x6F8
    13:0    R/W XTAL_SYS_REG0X_DUMMY1                       14'h0
    14      R/W ISO_XTAL                                    1'b1
    15      R/W XTAL_POW_XTAL                               1'b0
 */
typedef union _AON_FAST_REG0X_XTAL_OSC_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SYS_REG0X_DUMMY1: 14;
        uint16_t ISO_XTAL: 1;
        uint16_t XTAL_POW_XTAL: 1;
    };
} AON_FAST_REG0X_XTAL_OSC_SYS_TYPE;

/* 0x6FA
    2:0     R/W RET_SYS_REG0X_DUMMY1                        3'h0
    3       R/W RFC_STORE                                   1'b0
    4       R/W PF_STORE                                    1'b0
    5       R/W MODEM_STORE                                 1'b0
    6       R/W DP_MODEM_STORE                              1'b0
    7       R/W BZ_STORE                                    1'b0
    8       R/W BLE_STORE                                   1'b0
    9       R/W RFC_RESTORE                                 1'b0
    10      R/W PF_RESTORE                                  1'b0
    11      R/W MODEM_RESTORE                               1'b0
    12      R/W DP_MODEM_RESTORE                            1'b0
    13      R/W BZ_RESTORE                                  1'b0
    14      R/W BLE_RESTORE                                 1'b0
    15      R/W BT_RET_RSTB                                 1'b1
 */
typedef union _AON_FAST_REG0X_RET_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RET_SYS_REG0X_DUMMY1: 3;
        uint16_t RFC_STORE: 1;
        uint16_t PF_STORE: 1;
        uint16_t MODEM_STORE: 1;
        uint16_t DP_MODEM_STORE: 1;
        uint16_t BZ_STORE: 1;
        uint16_t BLE_STORE: 1;
        uint16_t RFC_RESTORE: 1;
        uint16_t PF_RESTORE: 1;
        uint16_t MODEM_RESTORE: 1;
        uint16_t DP_MODEM_RESTORE: 1;
        uint16_t BZ_RESTORE: 1;
        uint16_t BLE_RESTORE: 1;
        uint16_t BT_RET_RSTB: 1;
    };
} AON_FAST_REG0X_RET_SYS_TYPE;

/* 0x6FC
    0       R/W CORE_SYS_REG0X_DUMMY0                       1'b1
    1       R/W ISO_BT_PON                                  1'b1
    2       R/W CORE_SYS_REG0X_DUMMY2                       1'b1
    3       R/W CORE_SYS_REG0X_DUMMY3                       1'b1
    4       R/W CORE_SYS_REG0X_DUMMY4                       1'b1
    5       R/W CORE_SYS_REG0X_DUMMY5                       1'b1
    6       R/W CORE_SYS_REG0X_DUMMY6                       1'b1
    7       R/W ISO_BT_CORE                                 1'b1
    8       R/W CORE_SYS_REG0X_DUMMY8                       1'b0
    9       R/W CORE_SYS_REG0X_DUMMY9                       1'b0
    10      R/W CORE_SYS_REG0X_DUMMY10                      1'b0
    11      R/W CORE_SYS_REG0X_DUMMY11                      1'b0
    12      R/W CORE_SYS_REG0X_DUMMY12                      1'b0
    13      R/W BT_CORE_RSTB                                1'b0
    14      R/W CORE_SYS_REG0X_DUMMY14                      1'b0
    15      R/W BT_PON_RSTB                                 1'b0
 */
typedef union _AON_FAST_REG0X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_SYS_REG0X_DUMMY0: 1;
        uint16_t ISO_BT_PON: 1;
        uint16_t CORE_SYS_REG0X_DUMMY2: 1;
        uint16_t CORE_SYS_REG0X_DUMMY3: 1;
        uint16_t CORE_SYS_REG0X_DUMMY4: 1;
        uint16_t CORE_SYS_REG0X_DUMMY5: 1;
        uint16_t CORE_SYS_REG0X_DUMMY6: 1;
        uint16_t ISO_BT_CORE: 1;
        uint16_t CORE_SYS_REG0X_DUMMY8: 1;
        uint16_t CORE_SYS_REG0X_DUMMY9: 1;
        uint16_t CORE_SYS_REG0X_DUMMY10: 1;
        uint16_t CORE_SYS_REG0X_DUMMY11: 1;
        uint16_t CORE_SYS_REG0X_DUMMY12: 1;
        uint16_t BT_CORE_RSTB: 1;
        uint16_t CORE_SYS_REG0X_DUMMY14: 1;
        uint16_t BT_PON_RSTB: 1;
    };
} AON_FAST_REG0X_CORE_SYS_TYPE;

/* 0x6FE
    3:0     R/W CORE_SYS_REG1X_DUMMY0                       4'h0
    4       R/W CORE_SYS_REG1X_DUMMY4                       1'b0
    5       R/W CORE_SYS_REG1X_DUMMY5                       1'b0
    6       R/W CORE_SYS_REG1X_DUMMY6                       1'b0
    7       R/W CORE_SYS_REG1X_DUMMY7                       1'b0
    8       R/W CORE_SYS_REG1X_DUMMY8                       1'b0
    9       R/W CORE_SYS_REG1X_DUMMY9                       1'b0
    10      R/W CORE_SYS_REG1X_DUMMY10                      1'b0
    11      R/W CORE_SYS_REG1X_DUMMY11                      1'b0
    12      R/W CORE_SYS_REG1X_DUMMY12                      1'b0
    13      R/W r_cpu_clk_src_pll_sel                       1'b0
    14      R/W r_cpu_clk_src_sel_0                         1'b0
    15      R/W r_cpu_clk_src_sel_1                         1'b0
 */
typedef union _AON_FAST_REG1X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_SYS_REG1X_DUMMY0: 4;
        uint16_t CORE_SYS_REG1X_DUMMY4: 1;
        uint16_t CORE_SYS_REG1X_DUMMY5: 1;
        uint16_t CORE_SYS_REG1X_DUMMY6: 1;
        uint16_t CORE_SYS_REG1X_DUMMY7: 1;
        uint16_t CORE_SYS_REG1X_DUMMY8: 1;
        uint16_t CORE_SYS_REG1X_DUMMY9: 1;
        uint16_t CORE_SYS_REG1X_DUMMY10: 1;
        uint16_t CORE_SYS_REG1X_DUMMY11: 1;
        uint16_t CORE_SYS_REG1X_DUMMY12: 1;
        uint16_t r_cpu_clk_src_pll_sel: 1;
        uint16_t r_cpu_clk_src_sel_0: 1;
        uint16_t r_cpu_clk_src_sel_1: 1;
    };
} AON_FAST_REG1X_CORE_SYS_TYPE;

/* 0x700
    0       R/W ISO_OTP_PDOUT                               1'b0
    3:1     R/W CORE_SYS_REG2X_DUMMY1                       3'b000
    6:4     R/W CORE_SYS_REG2X_DUMMY4                       3'b000
    9:7     R/W CORE_SYS_REG2X_DUMMY7                       3'b000
    12:10   R/W CORE_SYS_REG2X_DUMMY10                      3'b000
    15:13   R/W CORE_SYS_REG2X_DUMMY13                      3'b000
 */
typedef union _AON_FAST_REG2X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ISO_OTP_PDOUT: 1;
        uint16_t CORE_SYS_REG2X_DUMMY1: 3;
        uint16_t CORE_SYS_REG2X_DUMMY4: 3;
        uint16_t CORE_SYS_REG2X_DUMMY7: 3;
        uint16_t CORE_SYS_REG2X_DUMMY10: 3;
        uint16_t CORE_SYS_REG2X_DUMMY13: 3;
    };
} AON_FAST_REG2X_CORE_SYS_TYPE;

/* 0x702
    0       R/W CORE_SYS_REG3X_DUMMY0                       1'b1
    1       R/W CORE_SYS_REG3X_DUMMY1                       1'b0
    2       R/W CORE_SYS_REG3X_DUMMY2                       1'b0
    3       R/W CORE_SYS_REG3X_DUMMY3                       1'b0
    4       R/W CORE_SYS_REG3X_DUMMY4                       1'b0
    5       R/W CORE_SYS_REG3X_DUMMY5                       1'b0
    6       R/W CORE_SYS_REG3X_DUMMY6                       1'b0
    7       R/W CORE_SYS_REG3X_DUMMY7                       1'b0
    8       R/W CORE_SYS_REG3X_DUMMY8                       1'b0
    9       R/W CORE_SYS_REG3X_DUMMY9                       1'b0
    10      R/W CORE_SYS_REG3X_DUMMY10                      1'b0
    11      R/W CORE_SYS_REG3X_DUMMY11                      1'b0
    12      R/W CORE_SYS_REG3X_DUMMY12                      1'b0
    13      R/W CORE_SYS_REG3X_DUMMY13                      1'b0
    14      R/W CORE_SYS_REG3X_DUMMY14                      1'b0
    15      R/W CORE_SYS_REG3X_DUMMY15                      1'b0
 */
typedef union _AON_FAST_REG3X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_SYS_REG3X_DUMMY0: 1;
        uint16_t CORE_SYS_REG3X_DUMMY1: 1;
        uint16_t CORE_SYS_REG3X_DUMMY2: 1;
        uint16_t CORE_SYS_REG3X_DUMMY3: 1;
        uint16_t CORE_SYS_REG3X_DUMMY4: 1;
        uint16_t CORE_SYS_REG3X_DUMMY5: 1;
        uint16_t CORE_SYS_REG3X_DUMMY6: 1;
        uint16_t CORE_SYS_REG3X_DUMMY7: 1;
        uint16_t CORE_SYS_REG3X_DUMMY8: 1;
        uint16_t CORE_SYS_REG3X_DUMMY9: 1;
        uint16_t CORE_SYS_REG3X_DUMMY10: 1;
        uint16_t CORE_SYS_REG3X_DUMMY11: 1;
        uint16_t CORE_SYS_REG3X_DUMMY12: 1;
        uint16_t CORE_SYS_REG3X_DUMMY13: 1;
        uint16_t CORE_SYS_REG3X_DUMMY14: 1;
        uint16_t CORE_SYS_REG3X_DUMMY15: 1;
    };
} AON_FAST_REG3X_CORE_SYS_TYPE;

/* 0x704
    8:0     R/W CORE_SYS_REG4X_DUMMY0                       9'h0
    9       R/W CORE_SYS_REG4X_DUMMY9                       1'b0
    10      R/W CORE_SYS_REG4X_DUMMY10                      1'b0
    11      R/W CORE_SYS_REG4X_DUMMY11                      1'b0
    12      R/W CORE_SYS_REG4X_DUMMY12                      1'b0
    13      R/W CORE_SYS_REG4X_DUMMY13                      1'b0
    14      R/W CORE_SYS_REG4X_DUMMY14                      1'b0
    15      R/W CORE_SYS_REG4X_DUMMY15                      1'b0
 */
typedef union _AON_FAST_REG4X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_SYS_REG4X_DUMMY0: 9;
        uint16_t CORE_SYS_REG4X_DUMMY9: 1;
        uint16_t CORE_SYS_REG4X_DUMMY10: 1;
        uint16_t CORE_SYS_REG4X_DUMMY11: 1;
        uint16_t CORE_SYS_REG4X_DUMMY12: 1;
        uint16_t CORE_SYS_REG4X_DUMMY13: 1;
        uint16_t CORE_SYS_REG4X_DUMMY14: 1;
        uint16_t CORE_SYS_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_CORE_SYS_TYPE;

/* 0x706
    0       R/W MASK_REG0X_DUMMY0                           1'b0
    1       R/W MASK_REG0X_DUMMY1                           1'b0
    2       R/W MASK_REG0X_DUMMY2                           1'b0
    3       R/W MASK_REG0X_DUMMY3                           1'b0
    4       R/W MASK_REG0X_DUMMY4                           1'b0
    5       R/W CHG_REG_MASK                                1'b1
    6       R/W MASK_REG0X_DUMMY6                           1'b0
    7       R/W MASK_REG0X_DUMMY7                           1'b0
    8       R/W MASK_REG0X_DUMMY8                           1'b0
    9       R/W MASK_REG0X_DUMMY9                           1'b0
    10      R/W MASK_REG0X_DUMMY10                          1'b0
    11      R/W MASK_REG0X_DUMMY11                          1'b0
    12      R/W MASK_REG0X_DUMMY12                          1'b0
    13      R/W MASK_REG0X_DUMMY13                          1'b0
    14      R/W MASK_REG0X_DUMMY14                          1'b0
    15      R/W MASK_REG0X_DUMMY15                          1'b0
 */
typedef union _AON_FAST_REG0X_REG_MASK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MASK_REG0X_DUMMY0: 1;
        uint16_t MASK_REG0X_DUMMY1: 1;
        uint16_t MASK_REG0X_DUMMY2: 1;
        uint16_t MASK_REG0X_DUMMY3: 1;
        uint16_t MASK_REG0X_DUMMY4: 1;
        uint16_t CHG_REG_MASK: 1;
        uint16_t MASK_REG0X_DUMMY6: 1;
        uint16_t MASK_REG0X_DUMMY7: 1;
        uint16_t MASK_REG0X_DUMMY8: 1;
        uint16_t MASK_REG0X_DUMMY9: 1;
        uint16_t MASK_REG0X_DUMMY10: 1;
        uint16_t MASK_REG0X_DUMMY11: 1;
        uint16_t MASK_REG0X_DUMMY12: 1;
        uint16_t MASK_REG0X_DUMMY13: 1;
        uint16_t MASK_REG0X_DUMMY14: 1;
        uint16_t MASK_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_REG_MASK_TYPE;

/* 0x708
    13:0    R/W REG_MASK_REG1X_DUMMY1                       14'h0
    14      R/W REG_MASK_REG1X_DUMMY2                       1'b0
    15      R/W REG_MASK_REG1X_DUMMY3                       1'b0
 */
typedef union _AON_FAST_REG1X_REG_MASK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG_MASK_REG1X_DUMMY1: 14;
        uint16_t REG_MASK_REG1X_DUMMY2: 1;
        uint16_t REG_MASK_REG1X_DUMMY3: 1;
    };
} AON_FAST_REG1X_REG_MASK_TYPE;

/* 0x70A
    7:0     R/W PMU_POS_CLK_REG0X_DUMMY1                    8'h0
    8       R/W PMU_POS_CLK_REG0X_DUMMY8                    1'b1
    9       R/W PMU_POS_CLK_REG0X_DUMMY9                    1'b1
    11:10   R/W PMU_POS_CLK_REG0X_DUMMY10                   2'b10
    13:12   R/W PMU_POS_CLK_REG0X_DUMMY12                   2'b10
    14      R/W PMU_POS_CLK_REG0X_DUMMY14                   1'b0
    15      R/W PMU_POS_CLK_REG0X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG0X_PMU_POS_CLK_MUX_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMU_POS_CLK_REG0X_DUMMY1: 8;
        uint16_t PMU_POS_CLK_REG0X_DUMMY8: 1;
        uint16_t PMU_POS_CLK_REG0X_DUMMY9: 1;
        uint16_t PMU_POS_CLK_REG0X_DUMMY10: 2;
        uint16_t PMU_POS_CLK_REG0X_DUMMY12: 2;
        uint16_t PMU_POS_CLK_REG0X_DUMMY14: 1;
        uint16_t PMU_POS_CLK_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_PMU_POS_CLK_MUX_TYPE;

/* 0x70C
    0       R/W LDO_DIG_EN_POS                              1'b0
    1       R/W LDO_DIG_POS_RST_B                           1'b0
    2       R/W PMU_POS_SEL_REG0X_DUMMY2                    1'b0
    3       R/W PMU_POS_SEL_REG0X_DUMMY3                    1'b0
    4       R/W LDOSYS_EN_POS                               1'b0
    5       R/W LDOSYS_POS_RST_B                            1'b0
    6       R/W PMU_POS_SEL_REG0X_DUMMY6                    1'b0
    7       R/W PMU_POS_SEL_REG0X_DUMMY7                    1'b0
    8       R/W LDOAUX1_EN_POS                              1'b0
    9       R/W LDOAUX1_POS_RST_B                           1'b0
    10      R/W PMU_POS_SEL_REG0X_DUMMY10                   1'b0
    11      R/W PMU_POS_SEL_REG0X_DUMMY11                   1'b0
    12      R/W LDOAUX2_EN_POS                              1'b0
    13      R/W LDOAUX2_POS_RST_B                           1'b0
    14      R/W PMU_POS_SEL_REG0X_DUMMY14                   1'b0
    15      R/W PMU_POS_SEL_REG0X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG0X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_DIG_EN_POS: 1;
        uint16_t LDO_DIG_POS_RST_B: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY2: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY3: 1;
        uint16_t LDOSYS_EN_POS: 1;
        uint16_t LDOSYS_POS_RST_B: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY6: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY7: 1;
        uint16_t LDOAUX1_EN_POS: 1;
        uint16_t LDOAUX1_POS_RST_B: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY10: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY11: 1;
        uint16_t LDOAUX2_EN_POS: 1;
        uint16_t LDOAUX2_POS_RST_B: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY14: 1;
        uint16_t PMU_POS_SEL_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_PMU_POS_SEL_TYPE;

/* 0x70E
    2:0     R/W LDOAUX2_PON_WAIT[2:0]                       3'b000
    3       R/W PMU_POS_SEL_REG1X_DUMMY3                    1'b0
    6:4     R/W LDOAUX2_PON_OVER[2:0]                       3'b111
    7       R/W PMU_POS_SEL_REG1X_DUMMY7                    1'b0
    10:8    R/W LDOAUX2_PON_START[2:0]                      3'b000
    11      R/W PMU_POS_SEL_REG1X_DUMMY11                   1'b0
    14:12   R/W LDOAUX2_PON_STEP[2:0]                       3'b001
    15      R/W PMU_POS_SEL_REG1X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG1X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX2_PON_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG1X_DUMMY3: 1;
        uint16_t LDOAUX2_PON_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG1X_DUMMY7: 1;
        uint16_t LDOAUX2_PON_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG1X_DUMMY11: 1;
        uint16_t LDOAUX2_PON_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_PMU_POS_SEL_TYPE;

/* 0x710
    2:0     R/W LDOAUX2_POF_WAIT[2:0]                       3'b000
    3       R/W PMU_POS_SEL_REG2X_DUMMY3                    1'b0
    6:4     R/W LDOAUX2_POF_OVER[2:0]                       3'b000
    7       R/W PMU_POS_SEL_REG2X_DUMMY7                    1'b0
    10:8    R/W LDOAUX2_POF_START[2:0]                      3'b111
    11      R/W PMU_POS_SEL_REG2X_DUMMY11                   1'b0
    14:12   R/W LDOAUX2_POF_STEP[2:0]                       3'b001
    15      R/W PMU_POS_SEL_REG2X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG2X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX2_POF_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG2X_DUMMY3: 1;
        uint16_t LDOAUX2_POF_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG2X_DUMMY7: 1;
        uint16_t LDOAUX2_POF_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG2X_DUMMY11: 1;
        uint16_t LDOAUX2_POF_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG2X_DUMMY15: 1;
    };
} AON_FAST_REG2X_PMU_POS_SEL_TYPE;

/* 0x712
    2:0     R/W LDOAUX2_SET_POF_FLAG2[2:0]                  3'b111
    3       R/W PMU_POS_SEL_REG3X_DUMMY3                    1'b0
    6:4     R/W LDOAUX2_SET_POF_FLAG1[2:0]                  3'b000
    7       R/W PMU_POS_SEL_REG3X_DUMMY7                    1'b0
    10:8    R/W LDOAUX2_SET_PON_FLAG2[2:0]                  3'b111
    11      R/W PMU_POS_SEL_REG3X_DUMMY11                   1'b0
    14:12   R/W LDOAUX2_SET_PON_FLAG1[2:0]                  3'b000
    15      R/W PMU_POS_SEL_REG3X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG3X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX2_SET_POF_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG3X_DUMMY3: 1;
        uint16_t LDOAUX2_SET_POF_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG3X_DUMMY7: 1;
        uint16_t LDOAUX2_SET_PON_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG3X_DUMMY11: 1;
        uint16_t LDOAUX2_SET_PON_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG3X_DUMMY15: 1;
    };
} AON_FAST_REG3X_PMU_POS_SEL_TYPE;

/* 0x714
    2:0     R/W LDOAUX1_PON_WAIT[2:0]                       3'b001
    3       R/W PMU_POS_SEL_REG4X_DUMMY3                    1'b0
    6:4     R/W LDOAUX1_PON_OVER[2:0]                       3'b111
    7       R/W PMU_POS_SEL_REG4X_DUMMY7                    1'b0
    10:8    R/W LDOAUX1_PON_START[2:0]                      3'b000
    11      R/W PMU_POS_SEL_REG4X_DUMMY11                   1'b0
    14:12   R/W LDOAUX1_PON_STEP[2:0]                       3'b001
    15      R/W PMU_POS_SEL_REG4X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG4X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_PON_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG4X_DUMMY3: 1;
        uint16_t LDOAUX1_PON_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG4X_DUMMY7: 1;
        uint16_t LDOAUX1_PON_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG4X_DUMMY11: 1;
        uint16_t LDOAUX1_PON_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_PMU_POS_SEL_TYPE;

/* 0x716
    2:0     R/W LDOAUX1_POF_WAIT[2:0]                       3'b000
    3       R/W PMU_POS_SEL_REG5X_DUMMY3                    1'b0
    6:4     R/W LDOAUX1_POF_OVER[2:0]                       3'b000
    7       R/W PMU_POS_SEL_REG5X_DUMMY7                    1'b0
    10:8    R/W LDOAUX1_POF_START[2:0]                      3'b111
    11      R/W PMU_POS_SEL_REG5X_DUMMY11                   1'b0
    14:12   R/W LDOAUX1_POF_STEP[2:0]                       3'b001
    15      R/W PMU_POS_SEL_REG5X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG5X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_POF_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG5X_DUMMY3: 1;
        uint16_t LDOAUX1_POF_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG5X_DUMMY7: 1;
        uint16_t LDOAUX1_POF_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG5X_DUMMY11: 1;
        uint16_t LDOAUX1_POF_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG5X_DUMMY15: 1;
    };
} AON_FAST_REG5X_PMU_POS_SEL_TYPE;

/* 0x718
    2:0     R/W LDOAUX1_SET_POF_FLAG2[2:0]                  3'b111
    3       R/W PMU_POS_SEL_REG6X_DUMMY3                    1'b0
    6:4     R/W LDOAUX1_SET_POF_FLAG1[2:0]                  3'b000
    7       R/W PMU_POS_SEL_REG6X_DUMMY7                    1'b0
    10:8    R/W LDOAUX1_SET_PON_FLAG2[2:0]                  3'b111
    11      R/W PMU_POS_SEL_REG6X_DUMMY11                   1'b0
    14:12   R/W LDOAUX1_SET_PON_FLAG1[2:0]                  3'b000
    15      R/W PMU_POS_SEL_REG6X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG6X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_SET_POF_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG6X_DUMMY3: 1;
        uint16_t LDOAUX1_SET_POF_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG6X_DUMMY7: 1;
        uint16_t LDOAUX1_SET_PON_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG6X_DUMMY11: 1;
        uint16_t LDOAUX1_SET_PON_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG6X_DUMMY15: 1;
    };
} AON_FAST_REG6X_PMU_POS_SEL_TYPE;

/* 0x71A
    2:0     R/W LDOSYS_PON_WAIT[2:0]                        3'b001
    3       R/W PMU_POS_SEL_REG7X_DUMMY3                    1'b0
    6:4     R/W LDOSYS_PON_OVER[2:0]                        3'b111
    7       R/W PMU_POS_SEL_REG7X_DUMMY7                    1'b0
    10:8    R/W LDOSYS_PON_START[2:0]                       3'b000
    11      R/W PMU_POS_SEL_REG7X_DUMMY11                   1'b0
    14:12   R/W LDOSYS_PON_STEP[2:0]                        3'b001
    15      R/W PMU_POS_SEL_REG7X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG7X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_PON_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG7X_DUMMY3: 1;
        uint16_t LDOSYS_PON_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG7X_DUMMY7: 1;
        uint16_t LDOSYS_PON_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG7X_DUMMY11: 1;
        uint16_t LDOSYS_PON_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG7X_DUMMY15: 1;
    };
} AON_FAST_REG7X_PMU_POS_SEL_TYPE;

/* 0x71C
    2:0     R/W LDOSYS_POF_WAIT[2:0]                        3'b000
    3       R/W PMU_POS_SEL_REG8X_DUMMY3                    1'b0
    6:4     R/W LDOSYS_POF_OVER[2:0]                        3'b000
    7       R/W PMU_POS_SEL_REG8X_DUMMY7                    1'b0
    10:8    R/W LDOSYS_POF_START[2:0]                       3'b111
    11      R/W PMU_POS_SEL_REG8X_DUMMY11                   1'b0
    14:12   R/W LDOSYS_POF_STEP[2:0]                        3'b001
    15      R/W PMU_POS_SEL_REG8X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG8X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POF_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG8X_DUMMY3: 1;
        uint16_t LDOSYS_POF_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG8X_DUMMY7: 1;
        uint16_t LDOSYS_POF_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG8X_DUMMY11: 1;
        uint16_t LDOSYS_POF_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG8X_DUMMY15: 1;
    };
} AON_FAST_REG8X_PMU_POS_SEL_TYPE;

/* 0x71E
    2:0     R/W LDOSYS_SET_POF_FLAG2[2:0]                   3'b111
    3       R/W PMU_POS_SEL_REG9X_DUMMY3                    1'b0
    6:4     R/W LDOSYS_SET_POF_FLAG1[2:0]                   3'b000
    7       R/W PMU_POS_SEL_REG9X_DUMMY7                    1'b0
    10:8    R/W LDOSYS_SET_PON_FLAG2[2:0]                   3'b111
    11      R/W PMU_POS_SEL_REG9X_DUMMY11                   1'b0
    14:12   R/W LDOSYS_SET_PON_FLAG1[2:0]                   3'b000
    15      R/W PMU_POS_SEL_REG9X_DUMMY15                   1'b0
 */
typedef union _AON_FAST_REG9X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_SET_POF_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG9X_DUMMY3: 1;
        uint16_t LDOSYS_SET_POF_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG9X_DUMMY7: 1;
        uint16_t LDOSYS_SET_PON_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG9X_DUMMY11: 1;
        uint16_t LDOSYS_SET_PON_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG9X_DUMMY15: 1;
    };
} AON_FAST_REG9X_PMU_POS_SEL_TYPE;

/* 0x720
    2:0     R/W LDO_DIG_PON_WAIT[2:0]                       3'b000
    3       R/W PMU_POS_SEL_REG10X_DUMMY3                   1'b0
    6:4     R/W LDO_DIG_PON_OVER[2:0]                       3'b111
    7       R/W PMU_POS_SEL_REG10X_DUMMY7                   1'b0
    10:8    R/W LDO_DIG_PON_START[2:0]                      3'b000
    11      R/W PMU_POS_SEL_REG10X_DUMMY11                  1'b0
    14:12   R/W LDO_DIG_PON_STEP[2:0]                       3'b001
    15      R/W PMU_POS_SEL_REG10X_DUMMY15                  1'b0
 */
typedef union _AON_FAST_REG10X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_DIG_PON_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG10X_DUMMY3: 1;
        uint16_t LDO_DIG_PON_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG10X_DUMMY7: 1;
        uint16_t LDO_DIG_PON_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG10X_DUMMY11: 1;
        uint16_t LDO_DIG_PON_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG10X_DUMMY15: 1;
    };
} AON_FAST_REG10X_PMU_POS_SEL_TYPE;

/* 0x722
    2:0     R/W LDO_DIG_POF_WAIT[2:0]                       3'b000
    3       R/W PMU_POS_SEL_REG11X_DUMMY3                   1'b0
    6:4     R/W LDO_DIG_POF_OVER[2:0]                       3'b000
    7       R/W PMU_POS_SEL_REG11X_DUMMY7                   1'b0
    10:8    R/W LDO_DIG_POF_START[2:0]                      3'b111
    11      R/W PMU_POS_SEL_REG11X_DUMMY11                  1'b0
    14:12   R/W LDO_DIG_POF_STEP[2:0]                       3'b001
    15      R/W PMU_POS_SEL_REG11X_DUMMY15                  1'b0
 */
typedef union _AON_FAST_REG11X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_DIG_POF_WAIT_2_0: 3;
        uint16_t PMU_POS_SEL_REG11X_DUMMY3: 1;
        uint16_t LDO_DIG_POF_OVER_2_0: 3;
        uint16_t PMU_POS_SEL_REG11X_DUMMY7: 1;
        uint16_t LDO_DIG_POF_START_2_0: 3;
        uint16_t PMU_POS_SEL_REG11X_DUMMY11: 1;
        uint16_t LDO_DIG_POF_STEP_2_0: 3;
        uint16_t PMU_POS_SEL_REG11X_DUMMY15: 1;
    };
} AON_FAST_REG11X_PMU_POS_SEL_TYPE;

/* 0x724
    2:0     R/W LDO_DIG_SET_POF_FLAG2[2:0]                  3'b111
    3       R/W PMU_POS_SEL_REG12X_DUMMY3                   1'b0
    6:4     R/W LDO_DIG_SET_POF_FLAG1[2:0]                  3'b000
    7       R/W PMU_POS_SEL_REG12X_DUMMY7                   1'b0
    10:8    R/W LDO_DIG_SET_PON_FLAG2[2:0]                  3'b111
    11      R/W PMU_POS_SEL_REG12X_DUMMY11                  1'b0
    14:12   R/W LDO_DIG_SET_PON_FLAG1[2:0]                  3'b000
    15      R/W PMU_POS_SEL_REG12X_DUMMY15                  1'b0
 */
typedef union _AON_FAST_REG12X_PMU_POS_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_DIG_SET_POF_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG12X_DUMMY3: 1;
        uint16_t LDO_DIG_SET_POF_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG12X_DUMMY7: 1;
        uint16_t LDO_DIG_SET_PON_FLAG2_2_0: 3;
        uint16_t PMU_POS_SEL_REG12X_DUMMY11: 1;
        uint16_t LDO_DIG_SET_PON_FLAG1_2_0: 3;
        uint16_t PMU_POS_SEL_REG12X_DUMMY15: 1;
    };
} AON_FAST_REG12X_PMU_POS_SEL_TYPE;

/* 0x726
    13:0    R/W REG0X_REG_FSM_DUMMY1                        14'h0
    14      R/W REG0X_REG_FSM_DUMMY2                        1'b0
    15      R/W LOP_POF_CAL                                 1'b0
 */
typedef union _AON_FAST_REG0X_REG_FSM_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_REG_FSM_DUMMY1: 14;
        uint16_t REG0X_REG_FSM_DUMMY2: 1;
        uint16_t LOP_POF_CAL: 1;
    };
} AON_FAST_REG0X_REG_FSM_TYPE;

/* 0x728
    0       R/W SEL_32K_XTAL                                1'b0
    1       R/W SEL_32K_LP                                  1'b0
    2       R/W SEL_32K_SDM                                 1'b0
    3       R/W SEL_32K_OSC                                 1'b0
    4       R/W SEL_32K_XTAL_rtc                            1'b0
    5       R/W SEL_32K_LP_rtc                              1'b0
    6       R/W SEL_32K_SDM_rtc                             1'b0
    7       R/W SEL_32K_OSC_rtc                             1'b0
    8       R/W SEL_32K_XTAL_bt                             1'b0
    9       R/W SEL_32K_LP_bt                               1'b0
    10      R/W SEL_32K_SDM_bt                              1'b0
    11      R/W SEL_32K_OSC_bt                              1'b0
    12      R/W en_p21_ext_32k                              1'b0
    13      R/W AON_GATED_EN_128K                           1'b0
    14      R/W REG0X_CLK_SEL_DUMMY14                       1'b0
    15      R/W BTAON_REG_CLK_SEL                           1'b0
 */
typedef union _AON_FAST_REG0X_CLK_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SEL_32K_XTAL: 1;
        uint16_t SEL_32K_LP: 1;
        uint16_t SEL_32K_SDM: 1;
        uint16_t SEL_32K_OSC: 1;
        uint16_t SEL_32K_XTAL_rtc: 1;
        uint16_t SEL_32K_LP_rtc: 1;
        uint16_t SEL_32K_SDM_rtc: 1;
        uint16_t SEL_32K_OSC_rtc: 1;
        uint16_t SEL_32K_XTAL_bt: 1;
        uint16_t SEL_32K_LP_bt: 1;
        uint16_t SEL_32K_SDM_bt: 1;
        uint16_t SEL_32K_OSC_bt: 1;
        uint16_t en_p21_ext_32k: 1;
        uint16_t AON_GATED_EN_128K: 1;
        uint16_t REG0X_CLK_SEL_DUMMY14: 1;
        uint16_t BTAON_REG_CLK_SEL: 1;
    };
} AON_FAST_REG0X_CLK_SEL_TYPE;

/* 0x72A
    10:0    R/W REG0X_REG_AON_LOADER_DUMMY0                 11'h0
    11      R   AON_loader_load_over_512_byte               1'b0
    12      R   AON_loader_forbidden_addr                   1'b0
    13      R   AON_loader_incorrect_key                    1'b0
    14      R/W AON_loader_clk_en                           1'b1
    15      R/W AON_loader_efuse_ctrl_sel                   1'b0
 */
typedef union _AON_FAST_REG0X_REG_AON_LOADER_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_REG_AON_LOADER_DUMMY0: 11;
        uint16_t AON_loader_load_over_512_byte: 1;
        uint16_t AON_loader_forbidden_addr: 1;
        uint16_t AON_loader_incorrect_key: 1;
        uint16_t AON_loader_clk_en: 1;
        uint16_t AON_loader_efuse_ctrl_sel: 1;
    };
} AON_FAST_REG0X_REG_AON_LOADER_TYPE;

/* 0x72C
    0       R/W reg_aon_1k_wdt_reset                        1'b0
    15:1    R/W REG0X_REG_AON_WDT_DUMMY1                    15'h0
 */
typedef union _AON_FAST_REG0X_REG_AON_WDT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_1k_wdt_reset: 1;
        uint16_t REG0X_REG_AON_WDT_DUMMY1: 15;
    };
} AON_FAST_REG0X_REG_AON_WDT_TYPE;

/* 0x742
    0       R/W PAD_P2_P4_SPI2_ADC_H3L1                     1'b1
    1       R/W PAD_P7_P8_SPI1_H3L1                         1'b0
    2       R/W REG0X_PAD_H3L1_DUMMY2                       1'b0
    3       R/W REG0X_PAD_H3L1_DUMMY3                       1'b0
    4       R/W REG0X_PAD_H3L1_DUMMY4                       1'b0
    5       R/W REG0X_PAD_H3L1_DUMMY5                       1'b0
    6       R/W REG0X_PAD_H3L1_DUMMY6                       1'b0
    7       R/W REG0X_PAD_H3L1_DUMMY7                       1'b0
    8       R/W REG0X_PAD_H3L1_DUMMY8                       1'b0
    9       R/W REG0X_PAD_H3L1_DUMMY9                       1'b0
    10      R/W REG0X_PAD_H3L1_DUMMY10                      1'b0
    11      R/W REG0X_PAD_H3L1_DUMMY11                      1'b0
    12      R/W REG0X_PAD_H3L1_DUMMY12                      1'b0
    13      R/W REG0X_PAD_H3L1_DUMMY13                      1'b0
    14      R/W REG0X_PAD_H3L1_DUMMY14                      1'b0
    15      R/W REG0X_PAD_H3L1_DUMMY15                      1'b0
 */
typedef union _AON_FAST_REG0X_PAD_H3L1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_P4_SPI2_ADC_H3L1: 1;
        uint16_t PAD_P7_P8_SPI1_H3L1: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY2: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY3: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY4: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY5: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY6: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY7: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY8: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY9: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY10: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY11: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY12: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY13: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY14: 1;
        uint16_t REG0X_PAD_H3L1_DUMMY15: 1;
    };
} AON_FAST_REG0X_PAD_H3L1_TYPE;

/* 0x744
    2:0     R/W PAD_P1_2_CFG                                3'b000
    5:3     R/W PAD_P1_1_CFG                                3'b000
    8:6     R/W PAD_P1_0_CFG                                3'b000
    11:9    R/W PAD_ADC_1_CFG                               3'b000
    14:12   R/W PAD_ADC_0_CFG                               3'b000
    15      R/W REG0X_PAD_CFG_DUMMY15                       1'b0
 */
typedef union _AON_FAST_REG0X_PAD_CFG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P1_2_CFG: 3;
        uint16_t PAD_P1_1_CFG: 3;
        uint16_t PAD_P1_0_CFG: 3;
        uint16_t PAD_ADC_1_CFG: 3;
        uint16_t PAD_ADC_0_CFG: 3;
        uint16_t REG0X_PAD_CFG_DUMMY15: 1;
    };
} AON_FAST_REG0X_PAD_CFG_TYPE;

/* 0x746
    2:0     R/W PAD_P2_4_CFG                                3'b000
    5:3     R/W PAD_P2_3_CFG                                3'b000
    8:6     R/W PAD_P2_2_CFG                                3'b000
    11:9    R/W PAD_P2_1_CFG                                3'b000
    14:12   R/W PAD_P2_0_CFG                                3'b000
    15      R/W REG1X_PAD_CFG_DUMMY15                       1'b0
 */
typedef union _AON_FAST_REG1X_PAD_CFG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_4_CFG: 3;
        uint16_t PAD_P2_3_CFG: 3;
        uint16_t PAD_P2_2_CFG: 3;
        uint16_t PAD_P2_1_CFG: 3;
        uint16_t PAD_P2_0_CFG: 3;
        uint16_t REG1X_PAD_CFG_DUMMY15: 1;
    };
} AON_FAST_REG1X_PAD_CFG_TYPE;

/* 0x748
    2:0     R/W REG2X_PAD_CFG_DUMMY0                        3'b000
    5:3     R/W REG2X_PAD_CFG_DUMMY3                        3'b000
    8:6     R/W REG2X_PAD_CFG_DUMMY6                        3'b000
    11:9    R/W PAD_P3_1_CFG                                3'b000
    14:12   R/W PAD_P3_0_CFG                                3'b000
    15      R/W REG2X_PAD_CFG_DUMMY15                       1'b0
 */
typedef union _AON_FAST_REG2X_PAD_CFG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG2X_PAD_CFG_DUMMY0: 3;
        uint16_t REG2X_PAD_CFG_DUMMY3: 3;
        uint16_t REG2X_PAD_CFG_DUMMY6: 3;
        uint16_t PAD_P3_1_CFG: 3;
        uint16_t PAD_P3_0_CFG: 3;
        uint16_t REG2X_PAD_CFG_DUMMY15: 1;
    };
} AON_FAST_REG2X_PAD_CFG_TYPE;

/* 0x7EC
    0       W1C PAD_ADC_STS[0]                              1'b1
    1       W1C PAD_ADC_STS[1]                              1'b1
    2       W1C PAD_ADC_STS[2]                              1'b1
    3       W1C PAD_ADC_STS[3]                              1'b1
    4       R/W REG0X_PAD_STS_DUMMY4                        1'b1
    5       R/W REG0X_PAD_STS_DUMMY5                        1'b1
    6       R/W REG0X_PAD_STS_DUMMY6                        1'b1
    7       R/W REG0X_PAD_STS_DUMMY7                        1'b1
    8       W1C PAD_P1_STS[0]                               1'b1
    9       W1C PAD_P1_STS[1]                               1'b1
    10      W1C PAD_P1_STS[2]                               1'b1
    11      W1C PAD_P1_STS[3]                               1'b1
    12      W1C PAD_P1_STS[4]                               1'b1
    13      W1C PAD_P1_STS[5]                               1'b1
    14      W1C PAD_P1_STS[6]                               1'b1
    15      W1C PAD_P1_STS[7]                               1'b1
 */
typedef union _AON_FAST_REG0X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_STS_0: 1;
        uint16_t PAD_ADC_STS_1: 1;
        uint16_t PAD_ADC_STS_2: 1;
        uint16_t PAD_ADC_STS_3: 1;
        uint16_t REG0X_PAD_STS_DUMMY4: 1;
        uint16_t REG0X_PAD_STS_DUMMY5: 1;
        uint16_t REG0X_PAD_STS_DUMMY6: 1;
        uint16_t REG0X_PAD_STS_DUMMY7: 1;
        uint16_t PAD_P1_STS_0: 1;
        uint16_t PAD_P1_STS_1: 1;
        uint16_t PAD_P1_STS_2: 1;
        uint16_t PAD_P1_STS_3: 1;
        uint16_t PAD_P1_STS_4: 1;
        uint16_t PAD_P1_STS_5: 1;
        uint16_t PAD_P1_STS_6: 1;
        uint16_t PAD_P1_STS_7: 1;
    };
} AON_FAST_REG0X_PAD_STS_TYPE;

/* 0x7EE
    0       W1C PAD_P2_STS[0]                               1'b1
    1       W1C PAD_P2_STS[1]                               1'b1
    2       W1C PAD_P2_STS[2]                               1'b1
    3       W1C PAD_P2_STS[3]                               1'b1
    4       W1C PAD_P2_STS[4]                               1'b1
    5       W1C PAD_P2_STS[5]                               1'b1
    6       W1C PAD_P2_STS[6]                               1'b1
    7       W1C PAD_P2_STS[7]                               1'b1
    8       W1C PAD_P3_STS[0]                               1'b1
    9       W1C PAD_P3_STS[1]                               1'b1
    10      W1C PAD_P3_STS[2]                               1'b1
    11      W1C PAD_P3_STS[3]                               1'b1
    12      W1C PAD_P3_STS[4]                               1'b1
    13      W1C PAD_P3_STS[5]                               1'b1
    14      R/W REG1X_PAD_STS_DUMMY14                       1'b1
    15      R/W REG1X_PAD_STS_DUMMY15                       1'b1
 */
typedef union _AON_FAST_REG1X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_STS_0: 1;
        uint16_t PAD_P2_STS_1: 1;
        uint16_t PAD_P2_STS_2: 1;
        uint16_t PAD_P2_STS_3: 1;
        uint16_t PAD_P2_STS_4: 1;
        uint16_t PAD_P2_STS_5: 1;
        uint16_t PAD_P2_STS_6: 1;
        uint16_t PAD_P2_STS_7: 1;
        uint16_t PAD_P3_STS_0: 1;
        uint16_t PAD_P3_STS_1: 1;
        uint16_t PAD_P3_STS_2: 1;
        uint16_t PAD_P3_STS_3: 1;
        uint16_t PAD_P3_STS_4: 1;
        uint16_t PAD_P3_STS_5: 1;
        uint16_t REG1X_PAD_STS_DUMMY14: 1;
        uint16_t REG1X_PAD_STS_DUMMY15: 1;
    };
} AON_FAST_REG1X_PAD_STS_TYPE;

/* 0x7F0
    0       W1C PAD_P4_STS[0]                               1'b1
    1       W1C PAD_P4_STS[1]                               1'b1
    2       W1C PAD_P4_STS[2]                               1'b1
    3       W1C PAD_P4_STS[3]                               1'b1
    4       W1C PAD_P4_STS[4]                               1'b1
    5       W1C PAD_P4_STS[5]                               1'b1
    6       W1C PAD_P4_STS[6]                               1'b1
    7       W1C PAD_P4_STS[7]                               1'b1
    8       W1C PAD_P5_STS[0]                               1'b1
    9       W1C PAD_P5_STS[1]                               1'b1
    10      W1C PAD_P5_STS[2]                               1'b1
    11      W1C PAD_P5_STS[3]                               1'b1
    12      W1C PAD_P5_STS[4]                               1'b1
    13      W1C PAD_P5_STS[5]                               1'b1
    14      W1C PAD_P5_STS[6]                               1'b1
    15      W1C PAD_P5_STS[7]                               1'b1
 */
typedef union _AON_FAST_REG2X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_STS_0: 1;
        uint16_t PAD_P4_STS_1: 1;
        uint16_t PAD_P4_STS_2: 1;
        uint16_t PAD_P4_STS_3: 1;
        uint16_t PAD_P4_STS_4: 1;
        uint16_t PAD_P4_STS_5: 1;
        uint16_t PAD_P4_STS_6: 1;
        uint16_t PAD_P4_STS_7: 1;
        uint16_t PAD_P5_STS_0: 1;
        uint16_t PAD_P5_STS_1: 1;
        uint16_t PAD_P5_STS_2: 1;
        uint16_t PAD_P5_STS_3: 1;
        uint16_t PAD_P5_STS_4: 1;
        uint16_t PAD_P5_STS_5: 1;
        uint16_t PAD_P5_STS_6: 1;
        uint16_t PAD_P5_STS_7: 1;
    };
} AON_FAST_REG2X_PAD_STS_TYPE;

/* 0x7F2
    0       W1C PAD_P7_STS[0]                               1'b1
    1       W1C PAD_P7_STS[1]                               1'b1
    2       W1C PAD_P7_STS[2]                               1'b1
    3       W1C PAD_P7_STS[3]                               1'b1
    4       W1C PAD_P7_STS[4]                               1'b1
    5       W1C PAD_P7_STS[5]                               1'b1
    6       W1C PAD_P7_STS[6]                               1'b1
    7       R/W REG3X_PAD_STS_DUMMY7                        1'b1
    8       W1C PAD_P6_STS[0]                               1'b1
    9       W1C PAD_P6_STS[1]                               1'b1
    10      W1C PAD_P6_STS[2]                               1'b1
    11      W1C PAD_P6_STS[3]                               1'b1
    12      W1C PAD_P6_STS[4]                               1'b1
    13      W1C PAD_P6_STS[5]                               1'b1
    14      W1C PAD_P6_STS[6]                               1'b1
    15      R/W REG3X_PAD_STS_DUMMY15                       1'b1
 */
typedef union _AON_FAST_REG3X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_STS_0: 1;
        uint16_t PAD_P7_STS_1: 1;
        uint16_t PAD_P7_STS_2: 1;
        uint16_t PAD_P7_STS_3: 1;
        uint16_t PAD_P7_STS_4: 1;
        uint16_t PAD_P7_STS_5: 1;
        uint16_t PAD_P7_STS_6: 1;
        uint16_t REG3X_PAD_STS_DUMMY7: 1;
        uint16_t PAD_P6_STS_0: 1;
        uint16_t PAD_P6_STS_1: 1;
        uint16_t PAD_P6_STS_2: 1;
        uint16_t PAD_P6_STS_3: 1;
        uint16_t PAD_P6_STS_4: 1;
        uint16_t PAD_P6_STS_5: 1;
        uint16_t PAD_P6_STS_6: 1;
        uint16_t REG3X_PAD_STS_DUMMY15: 1;
    };
} AON_FAST_REG3X_PAD_STS_TYPE;

/* 0x7F4
    0       W1C PAD_P8_STS[0]                               1'b1
    1       W1C PAD_P8_STS[1]                               1'b1
    2       W1C PAD_P8_STS[2]                               1'b1
    3       W1C PAD_P8_STS[3]                               1'b1
    4       W1C PAD_P8_STS[4]                               1'b1
    5       W1C PAD_P8_STS[5]                               1'b1
    6       R/W REG4X_PAD_STS_DUMMY6                        1'b1
    7       W1C REG4X_PAD_STS_DUMMY7                        1'b1
    8       W1C PAD_P9_STS[0]                               1'b1
    9       W1C PAD_P9_STS[1]                               1'b1
    10      W1C PAD_P9_STS[2]                               1'b1
    11      W1C PAD_P9_STS[3]                               1'b1
    12      W1C PAD_P9_STS[4]                               1'b1
    13      R/W PAD_P9_STS[5]                               1'b1
    14      R/W REG4X_PAD_STS_DUMMY14                       1'b1
    15      R/W REG4X_PAD_STS_DUMMY15                       1'b1
 */
typedef union _AON_FAST_REG4X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P8_STS_0: 1;
        uint16_t PAD_P8_STS_1: 1;
        uint16_t PAD_P8_STS_2: 1;
        uint16_t PAD_P8_STS_3: 1;
        uint16_t PAD_P8_STS_4: 1;
        uint16_t PAD_P8_STS_5: 1;
        uint16_t REG4X_PAD_STS_DUMMY6: 1;
        uint16_t REG4X_PAD_STS_DUMMY7: 1;
        uint16_t PAD_P9_STS_0: 1;
        uint16_t PAD_P9_STS_1: 1;
        uint16_t PAD_P9_STS_2: 1;
        uint16_t PAD_P9_STS_3: 1;
        uint16_t PAD_P9_STS_4: 1;
        uint16_t PAD_P9_STS_5: 1;
        uint16_t REG4X_PAD_STS_DUMMY14: 1;
        uint16_t REG4X_PAD_STS_DUMMY15: 1;
    };
} AON_FAST_REG4X_PAD_STS_TYPE;

/* 0x7F6
    0       W1C PAD_MIC1_P_STS                              1'b1
    1       W1C PAD_MIC1_N_STS                              1'b1
    2       W1C PAD_MIC2_P_STS                              1'b1
    3       W1C PAD_MIC2_N_STS                              1'b1
    4       W1C PAD_MIC3_P_STS                              1'b1
    5       W1C PAD_MIC3_N_STS                              1'b1
    6       W1C PAD_LOUT_P_STS                              1'b1
    7       W1C PAD_LOUT_N_STS                              1'b1
    8       W1C PAD_ROUT_P_STS                              1'b1
    9       W1C PAD_ROUT_N_STS                              1'b1
    10      W1C PAD_AUX_L_STS                               1'b1
    11      W1C PAD_AUX_R_STS                               1'b1
    12      W1C PAD_MICBIAS_STS                             1'b1
    13      R/W REG5X_PAD_STS_DUMMY13                       1'b1
    14      R/W REG5X_PAD_STS_DUMMY14                       1'b1
    15      R/W REG5X_PAD_STS_DUMMY15                       1'b1
 */
typedef union _AON_FAST_REG5X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC1_P_STS: 1;
        uint16_t PAD_MIC1_N_STS: 1;
        uint16_t PAD_MIC2_P_STS: 1;
        uint16_t PAD_MIC2_N_STS: 1;
        uint16_t PAD_MIC3_P_STS: 1;
        uint16_t PAD_MIC3_N_STS: 1;
        uint16_t PAD_LOUT_P_STS: 1;
        uint16_t PAD_LOUT_N_STS: 1;
        uint16_t PAD_ROUT_P_STS: 1;
        uint16_t PAD_ROUT_N_STS: 1;
        uint16_t PAD_AUX_L_STS: 1;
        uint16_t PAD_AUX_R_STS: 1;
        uint16_t PAD_MICBIAS_STS: 1;
        uint16_t REG5X_PAD_STS_DUMMY13: 1;
        uint16_t REG5X_PAD_STS_DUMMY14: 1;
        uint16_t REG5X_PAD_STS_DUMMY15: 1;
    };
} AON_FAST_REG5X_PAD_STS_TYPE;

/* 0x7F8
    0       W1C PAD_P10_STS[0]                              1'b1
    1       W1C REG6X_PAD_STS_DUMMY1                        1'b1
    2       W1C REG6X_PAD_STS_DUMMY2                        1'b1
    3       W1C REG6X_PAD_STS_DUMMY3                        1'b1
    4       W1C REG6X_PAD_STS_DUMMY4                        1'b1
    5       W1C REG6X_PAD_STS_DUMMY5                        1'b1
    6       W1C REG6X_PAD_STS_DUMMY6                        1'b1
    7       W1C REG6X_PAD_STS_DUMMY7                        1'b1
    8       W1C REG6X_PAD_STS_DUMMY8                        1'b1
    9       W1C REG6X_PAD_STS_DUMMY9                        1'b1
    10      W1C REG6X_PAD_STS_DUMMY10                       1'b1
    11      W1C REG6X_PAD_STS_DUMMY11                       1'b1
    12      W1C REG6X_PAD_STS_DUMMY12                       1'b1
    13      W1C REG6X_PAD_STS_DUMMY13                       1'b1
    14      W1C REG6X_PAD_STS_DUMMY14                       1'b1
    15      W1C REG6X_PAD_STS_DUMMY15                       1'b1
 */
typedef union _AON_FAST_REG6X_PAD_STS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P10_STS_0: 1;
        uint16_t REG6X_PAD_STS_DUMMY1: 1;
        uint16_t REG6X_PAD_STS_DUMMY2: 1;
        uint16_t REG6X_PAD_STS_DUMMY3: 1;
        uint16_t REG6X_PAD_STS_DUMMY4: 1;
        uint16_t REG6X_PAD_STS_DUMMY5: 1;
        uint16_t REG6X_PAD_STS_DUMMY6: 1;
        uint16_t REG6X_PAD_STS_DUMMY7: 1;
        uint16_t REG6X_PAD_STS_DUMMY8: 1;
        uint16_t REG6X_PAD_STS_DUMMY9: 1;
        uint16_t REG6X_PAD_STS_DUMMY10: 1;
        uint16_t REG6X_PAD_STS_DUMMY11: 1;
        uint16_t REG6X_PAD_STS_DUMMY12: 1;
        uint16_t REG6X_PAD_STS_DUMMY13: 1;
        uint16_t REG6X_PAD_STS_DUMMY14: 1;
        uint16_t REG6X_PAD_STS_DUMMY15: 1;
    };
} AON_FAST_REG6X_PAD_STS_TYPE;

/* 0x800
    0       R/W AON_REG_LOP_PON_CHG_POW_M1                  1'b1
    1       R/W AON_REG_LOP_PON_CHG_POW_M2_DVDET            1'b1
    2       R/W AON_REG_LOP_PON_CHG_POW_M1_DVDET            1'b1
    3       R/W AON_REG_LOP_PON_CHG_EN_M1FON_LDO733         1'b0
    4       R/W AON_REG_LOP_PON_CHG_EN_M2FONBUF             1'b0
    5       R/W AON_REG_LOP_PON_CHG_EN_M2FON1K              1'b0
    6       R/W AON_REG_LOP_PON_POW32K_32KXTAL              1'b0
    7       R/W AON_REG_LOP_PON_POW32K_32KOSC               1'b1
    8       R/W AON_REG_LOP_PON_MBIAS_POW_VAUDIO_DET        1'b1
    9       R/W AON_REG_LOP_PON_MBIAS_POW_VDDCORE_DET       1'b1
    10      R/W AON_REG_LOP_PON_MBIAS_POW_VAUX_DET          1'b1
    11      R/W AON_REG_LOP_PON_MBIAS_POW_HV_DET            1'b1
    12      R/W AON_REG_LOP_PON_MBIAS_POW_VBAT_DET          1'b1
    13      R/W AON_REG_LOP_PON_MBIAS_POW_ADP_DET           1'b1
    14      R/W AON_REG_LOP_PON_MBIAS_POW_BIAS_500nA        1'b1
    15      R/W AON_REG_LOP_PON_MBIAS_POW_BIAS              1'b1
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_CHG_POW_M1: 1;
        uint16_t AON_REG_LOP_PON_CHG_POW_M2_DVDET: 1;
        uint16_t AON_REG_LOP_PON_CHG_POW_M1_DVDET: 1;
        uint16_t AON_REG_LOP_PON_CHG_EN_M1FON_LDO733: 1;
        uint16_t AON_REG_LOP_PON_CHG_EN_M2FONBUF: 1;
        uint16_t AON_REG_LOP_PON_CHG_EN_M2FON1K: 1;
        uint16_t AON_REG_LOP_PON_POW32K_32KXTAL: 1;
        uint16_t AON_REG_LOP_PON_POW32K_32KOSC: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_VAUDIO_DET: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_VDDCORE_DET: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_VAUX_DET: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_HV_DET: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_VBAT_DET: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_ADP_DET: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_BIAS_500nA: 1;
        uint16_t AON_REG_LOP_PON_MBIAS_POW_BIAS: 1;
    };
} AON_FAST_AON_REG_LOP_PON_RG0X_TYPE;

/* 0x802
    0       R/W AON_REG_LOP_PON_SWR_CORE_POW_SAW_IB         1'b1
    1       R/W AON_REG_LOP_PON_SWR_CORE_POW_IMIR           1'b1
    2       R/W AON_REG_LOP_PON_LDOAUX1_POW_LDO533HQ        1'b1
    3       R/W AON_REG_LOP_PON_LDOAUX1_EN_POS              1'b1
    4       R/W AON_REG_LOP_PON_LDOSYS_POW_HQLQ533_PC       1'b1
    5       R/W AON_REG_LOP_PON_LDOSYS_POW_HQLQVCORE533_PC  1'b1
    6       R/W AON_REG_LOP_PON_LDOAUX1_POS_RST_B           1'b1
    7       R/W AON_REG_LOP_PON_LDOAUX1_POW_VREF            1'b1
    8       R/W AON_REG_LOP_PON_LDOSYS_POW_LDO533HQ         1'b1
    9       R/W AON_REG_LOP_PON_LDOSYS_EN_POS               1'b1
    10      R/W AON_REG_LOP_PON_LDOSYS_POW_LDO733LQ_VCORE   1'b1
    12:11   R/W AON_REG_LOP_PON_CHG_SEL_M2CCDFB             2'b11
    13      R/W AON_REG_LOP_PON_LDOSYS_POS_RST_B            1'b1
    14      R/W AON_REG_LOP_PON_LDOSYS_POW_LDOVREF          1'b1
    15      R/W AON_REG_LOP_PON_CHG_POW_M2                  1'b1
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_SAW_IB: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_IMIR: 1;
        uint16_t AON_REG_LOP_PON_LDOAUX1_POW_LDO533HQ: 1;
        uint16_t AON_REG_LOP_PON_LDOAUX1_EN_POS: 1;
        uint16_t AON_REG_LOP_PON_LDOSYS_POW_HQLQ533_PC: 1;
        uint16_t AON_REG_LOP_PON_LDOSYS_POW_HQLQVCORE533_PC: 1;
        uint16_t AON_REG_LOP_PON_LDOAUX1_POS_RST_B: 1;
        uint16_t AON_REG_LOP_PON_LDOAUX1_POW_VREF: 1;
        uint16_t AON_REG_LOP_PON_LDOSYS_POW_LDO533HQ: 1;
        uint16_t AON_REG_LOP_PON_LDOSYS_EN_POS: 1;
        uint16_t AON_REG_LOP_PON_LDOSYS_POW_LDO733LQ_VCORE: 1;
        uint16_t AON_REG_LOP_PON_CHG_SEL_M2CCDFB: 2;
        uint16_t AON_REG_LOP_PON_LDOSYS_POS_RST_B: 1;
        uint16_t AON_REG_LOP_PON_LDOSYS_POW_LDOVREF: 1;
        uint16_t AON_REG_LOP_PON_CHG_POW_M2: 1;
    };
} AON_FAST_AON_REG_LOP_PON_RG1X_TYPE;

/* 0x804
    0       R/W AON_REG_LOP_PON_SWR_CORE_POW_ZCD_COMP_LOWIQ 1'b0
    6:1     R/W AON_REG_LOP_PON_SWR_CORE_TUNE_BNYCNT_INI    6'b000000
    7       R/W AON_REG_LOP_PON_SWR_CORE_POW_BNYCNT_1       1'b0
    8       R/W AON_REG_LOP_PON_SWR_CORE_FPWM_1             1'b1
    9       R/W AON_REG_LOP_PON_SWR_CORE_POW_OCP            1'b0
    10      R/W AON_REG_LOP_PON_SWR_CORE_POW_ZCD            1'b0
    11      R/W AON_REG_LOP_PON_SWR_CORE_POW_PFM            1'b0
    12      R/W AON_REG_LOP_PON_SWR_CORE_POW_PWM            1'b0
    13      R/W AON_REG_LOP_PON_SWR_CORE_POW_VDIV           1'b1
    14      R/W AON_REG_LOP_PON_SWR_CORE_POW_REF            1'b1
    15      R/W AON_REG_LOP_PON_SWR_CORE_POW_SAW            1'b1
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_TUNE_BNYCNT_INI: 6;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_BNYCNT_1: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_FPWM_1: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_OCP: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_ZCD: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_PFM: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_PWM: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_VDIV: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_REF: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_SAW: 1;
    };
} AON_FAST_AON_REG_LOP_PON_RG2X_TYPE;

/* 0x806
    0       R/W AON_REG_LOP_PON_RG3X_DUMMY1                 1'h0
    1       R/W AON_REG_LOP_PON_SWR_CORE_POW_BNYCNT_2       1'b1
    2       R/W AON_REG_LOP_PON_LDO_DIG_POW_LDODIG          1'b1
    3       R/W AON_REG_LOP_PON_LDO_DIG_EN_POS              1'b1
    4       R/W AON_REG_LOP_PON_LDO_DIG_EN_LDODIG_PC        1'b0
    6:5     R/W AON_REG_LOP_PON_XTAL_LPS_CAP_STEP           2'b01
    8:7     R/W AON_REG_LOP_PON_XTAL_LPS_CAP_CYC            2'b00
    9       R/W AON_REG_LOP_PON_LDO_DIG_POS_RST_B           1'b1
    14:10   R/W AON_REG_LOP_PON_LDO_DIG_TUNE_LDODIG_VOUT    5'b10110
    15      R/W AON_REG_LOP_PON_LDO_DIG_POW_LDODIG_VREF     1'b1
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_RG3X_DUMMY1: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_BNYCNT_2: 1;
        uint16_t AON_REG_LOP_PON_LDO_DIG_POW_LDODIG: 1;
        uint16_t AON_REG_LOP_PON_LDO_DIG_EN_POS: 1;
        uint16_t AON_REG_LOP_PON_LDO_DIG_EN_LDODIG_PC: 1;
        uint16_t AON_REG_LOP_PON_XTAL_LPS_CAP_STEP: 2;
        uint16_t AON_REG_LOP_PON_XTAL_LPS_CAP_CYC: 2;
        uint16_t AON_REG_LOP_PON_LDO_DIG_POS_RST_B: 1;
        uint16_t AON_REG_LOP_PON_LDO_DIG_TUNE_LDODIG_VOUT: 5;
        uint16_t AON_REG_LOP_PON_LDO_DIG_POW_LDODIG_VREF: 1;
    };
} AON_FAST_AON_REG_LOP_PON_RG3X_TYPE;

/* 0x808
    3:0     R/W AON_REG_LOP_PON_RG4X_DUMMY1                 4'h0
    11:4    R/W AON_REG_LOP_PON_SWR_CORE_TUNE_POS_VREFPFM   8'b01101110
    15:12   R/W AON_REG_LOP_PON_SWR_CORE_TUNE_REF_VREFLPPFM 4'b0110
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_RG4X_DUMMY1: 4;
        uint16_t AON_REG_LOP_PON_SWR_CORE_TUNE_POS_VREFPFM: 8;
        uint16_t AON_REG_LOP_PON_SWR_CORE_TUNE_REF_VREFLPPFM: 4;
    };
} AON_FAST_AON_REG_LOP_PON_RG4X_TYPE;

/* 0x80A
    0       R/W AON_REG_LOP_PON_BLE_RESTORE                 1'b0
    1       R/W AON_REG_LOP_PON_VCORE_PC_POW_VCORE_PC_VG2   1'b0
    2       R/W AON_REG_LOP_PON_VCORE_PC_POW_VCORE_PC_VG1   1'b0
    3       R/W AON_REG_LOP_PON_LDO_DIG_POW_LDORET          1'b0
    4       R/W AON_REG_LOP_PON_SWR_CORE_POW_SWR            1'b0
    5       R/W AON_REG_LOP_PON_SWR_CORE_POW_LDO            1'b1
    6       R/W AON_REG_LOP_PON_SWR_CORE_SEL_POS_VREFLPPFM  1'b0
    7       R/W AON_REG_LOP_PON_SWR_CORE_FPWM_2             1'b1
    15:8    R/W AON_REG_LOP_PON_SWR_CORE_TUNE_VDIV          8'b10001010
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_BLE_RESTORE: 1;
        uint16_t AON_REG_LOP_PON_VCORE_PC_POW_VCORE_PC_VG2: 1;
        uint16_t AON_REG_LOP_PON_VCORE_PC_POW_VCORE_PC_VG1: 1;
        uint16_t AON_REG_LOP_PON_LDO_DIG_POW_LDORET: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_SWR: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_POW_LDO: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_SEL_POS_VREFLPPFM: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_FPWM_2: 1;
        uint16_t AON_REG_LOP_PON_SWR_CORE_TUNE_VDIV: 8;
    };
} AON_FAST_AON_REG_LOP_PON_RG5X_TYPE;

/* 0x80C
    0       R/W AON_REG_LOP_PON_BT_PLL_pow_pll              1'b0
    1       R/W AON_REG_LOP_PON_BT_PLL_LDO_pow_LDO          1'b0
    2       R/W AON_REG_LOP_PON_BT_PLL_LDO_ERC_V12A_BTPLL   1'b0
    3       R/W AON_REG_LOP_PON_BT_PLL_LDO_SW_LDO2PORCUT    1'b0
    4       R/W AON_REG_LOP_PON_ISO_XTAL                    1'b0
    5       R/W AON_REG_LOP_PON_OSC40M_POW_OSC              1'b1
    8:6     R/W AON_REG_LOP_PON_XTAL_MODE                   3'b100
    9       R/W AON_REG_LOP_PON_XTAL_POW_XTAL               1'b1
    10      R/W AON_REG_LOP_PON_BT_RET_RSTB                 1'b1
    11      R/W AON_REG_LOP_PON_RFC_RESTORE                 1'b0
    12      R/W AON_REG_LOP_PON_PF_RESTORE                  1'b0
    13      R/W AON_REG_LOP_PON_MODEM_RESTORE               1'b0
    14      R/W AON_REG_LOP_PON_DP_MODEM_RESTORE            1'b0
    15      R/W AON_REG_LOP_PON_BZ_RESTORE                  1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_BT_PLL_pow_pll: 1;
        uint16_t AON_REG_LOP_PON_BT_PLL_LDO_pow_LDO: 1;
        uint16_t AON_REG_LOP_PON_BT_PLL_LDO_ERC_V12A_BTPLL: 1;
        uint16_t AON_REG_LOP_PON_BT_PLL_LDO_SW_LDO2PORCUT: 1;
        uint16_t AON_REG_LOP_PON_ISO_XTAL: 1;
        uint16_t AON_REG_LOP_PON_OSC40M_POW_OSC: 1;
        uint16_t AON_REG_LOP_PON_XTAL_MODE: 3;
        uint16_t AON_REG_LOP_PON_XTAL_POW_XTAL: 1;
        uint16_t AON_REG_LOP_PON_BT_RET_RSTB: 1;
        uint16_t AON_REG_LOP_PON_RFC_RESTORE: 1;
        uint16_t AON_REG_LOP_PON_PF_RESTORE: 1;
        uint16_t AON_REG_LOP_PON_MODEM_RESTORE: 1;
        uint16_t AON_REG_LOP_PON_DP_MODEM_RESTORE: 1;
        uint16_t AON_REG_LOP_PON_BZ_RESTORE: 1;
    };
} AON_FAST_AON_REG_LOP_PON_RG6X_TYPE;

/* 0x80E
    7:0     R/W AON_REG_LOP_PON_RG7X_DUMMY1                 8'h0
    8       R/W AON_REG_LOP_PON_BT_CORE_RSTB                1'b1
    9       R/W AON_REG_LOP_PON_BT_PON_RSTB                 1'b1
    10      R/W AON_REG_LOP_PON_ISO_BT_PON                  1'b0
    11      R/W AON_REG_LOP_PON_ISO_BT_CORE                 1'b0
    12      R/W AON_REG_LOP_PON_ISO_PLL2                    1'b1
    13      R/W AON_REG_LOP_PON_ISO_PLL                     1'b1
    14      R/W AON_REG_LOP_PON_BT_PLL_LP_PLL_pow_cpop      1'b0
    15      R/W AON_REG_LOP_PON_BT_PLL_LP_PLL_pow_pll       1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG7X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_RG7X_DUMMY1: 8;
        uint16_t AON_REG_LOP_PON_BT_CORE_RSTB: 1;
        uint16_t AON_REG_LOP_PON_BT_PON_RSTB: 1;
        uint16_t AON_REG_LOP_PON_ISO_BT_PON: 1;
        uint16_t AON_REG_LOP_PON_ISO_BT_CORE: 1;
        uint16_t AON_REG_LOP_PON_ISO_PLL2: 1;
        uint16_t AON_REG_LOP_PON_ISO_PLL: 1;
        uint16_t AON_REG_LOP_PON_BT_PLL_LP_PLL_pow_cpop: 1;
        uint16_t AON_REG_LOP_PON_BT_PLL_LP_PLL_pow_pll: 1;
    };
} AON_FAST_AON_REG_LOP_PON_RG7X_TYPE;

/* 0x810
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG8X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG8X_TYPE;

/* 0x812
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG9X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG9X_TYPE;

/* 0x814
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG10X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG10X_TYPE;

/* 0x816
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG11X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG11X_TYPE;

/* 0x818
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG12X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG12X_TYPE;

/* 0x81A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG13X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG13X_TYPE;

/* 0x81C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG14X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG14X_TYPE;

/* 0x81E
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG15X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG15X_TYPE;

/* 0x820
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG16X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG16X_TYPE;

/* 0x822
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG17X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG17X_TYPE;

/* 0x824
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG18X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG18X_TYPE;

/* 0x826
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG19X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG19X_TYPE;

/* 0x828
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG20X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG20X_TYPE;

/* 0x82A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG21X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG21X_TYPE;

/* 0x82C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_RG22X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_RG22X_TYPE;

/* 0x82E
    7:0     R/W LOP_PON_M1M2_DELAY                          8'ha0
    15:8    R/W LOP_PON_BIAS_DELAY                          8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_M1M2_DELAY: 8;
        uint16_t LOP_PON_BIAS_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG0X_TYPE;

/* 0x830
    7:0     R/W LOP_PON_LDOHQ_DELAY                         8'ha0
    15:8    R/W LOP_PON_SYS_DELAY                           8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOHQ_DELAY: 8;
        uint16_t LOP_PON_SYS_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG1X_TYPE;

/* 0x832
    7:0     R/W LOP_PON_SWR_DELAY                           8'ha0
    15:8    R/W LOP_PON_SWR_BIAS_DELAY                      8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_SWR_DELAY: 8;
        uint16_t LOP_PON_SWR_BIAS_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG2X_TYPE;

/* 0x834
    7:0     R/W LOP_PON_VCORE2_DELAY                        8'ha0
    15:8    R/W LOP_PON_VCORE1_DELAY                        8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_VCORE2_DELAY: 8;
        uint16_t LOP_PON_VCORE1_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG3X_TYPE;

/* 0x836
    7:0     R/W LOP_PON_RST_DELAY                           8'ha0
    15:8    R/W LOP_PON_RESTORE_DELAY                       8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RST_DELAY: 8;
        uint16_t LOP_PON_RESTORE_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG4X_TYPE;

/* 0x838
    7:0     R/W LOP_PON_PLL_DELAY                           8'ha0
    15:8    R/W LOP_PON_XTAL_DELAY                          8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_PLL_DELAY: 8;
        uint16_t LOP_PON_XTAL_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG5X_TYPE;

/* 0x83A
    7:0     R/W AON_REG_LOP_PON_DELAY_RG4X_DUMMY0           8'ha0
    15:8    R/W LOP_PON_ISO_DELAY                           8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_DELAY_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_PON_DELAY_RG4X_DUMMY0: 8;
        uint16_t LOP_PON_ISO_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_PON_DELAY_RG6X_TYPE;

/* 0x83C
    15:0    R/W RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_PON_ECO_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_PON_ECO_RG0X_TYPE;

/* 0x850
    0       R/W AON_REG_LOP_POF_CHG_POW_M1                  1'b0
    1       R/W AON_REG_LOP_POF_CHG_POW_M2_DVDET            1'b0
    2       R/W AON_REG_LOP_POF_CHG_POW_M1_DVDET            1'b0
    3       R/W AON_REG_LOP_POF_CHG_EN_M1FON_LDO733         1'b0
    4       R/W AON_REG_LOP_POF_CHG_EN_M2FONBUF             1'b0
    5       R/W AON_REG_LOP_POF_CHG_EN_M2FON1K              1'b0
    6       R/W AON_REG_LOP_POF_POW32K_32KXTAL              1'b0
    7       R/W AON_REG_LOP_POF_POW32K_32KOSC               1'b1
    8       R/W AON_REG_LOP_POF_MBIAS_POW_VAUDIO_DET        1'b0
    9       R/W AON_REG_LOP_POF_MBIAS_POW_VDDCORE_DET       1'b0
    10      R/W AON_REG_LOP_POF_MBIAS_POW_VAUX_DET          1'b0
    11      R/W AON_REG_LOP_POF_MBIAS_POW_HV_DET            1'b0
    12      R/W AON_REG_LOP_POF_MBIAS_POW_VBAT_DET          1'b0
    13      R/W AON_REG_LOP_POF_MBIAS_POW_ADP_DET           1'b0
    14      R/W AON_REG_LOP_POF_MBIAS_POW_BIAS_500nA        1'b0
    15      R/W AON_REG_LOP_POF_MBIAS_POW_BIAS              1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_CHG_POW_M1: 1;
        uint16_t AON_REG_LOP_POF_CHG_POW_M2_DVDET: 1;
        uint16_t AON_REG_LOP_POF_CHG_POW_M1_DVDET: 1;
        uint16_t AON_REG_LOP_POF_CHG_EN_M1FON_LDO733: 1;
        uint16_t AON_REG_LOP_POF_CHG_EN_M2FONBUF: 1;
        uint16_t AON_REG_LOP_POF_CHG_EN_M2FON1K: 1;
        uint16_t AON_REG_LOP_POF_POW32K_32KXTAL: 1;
        uint16_t AON_REG_LOP_POF_POW32K_32KOSC: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_VAUDIO_DET: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_VDDCORE_DET: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_VAUX_DET: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_HV_DET: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_VBAT_DET: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_ADP_DET: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_BIAS_500nA: 1;
        uint16_t AON_REG_LOP_POF_MBIAS_POW_BIAS: 1;
    };
} AON_FAST_AON_REG_LOP_POF_RG0X_TYPE;

/* 0x852
    0       R/W AON_REG_LOP_POF_SWR_CORE_POW_SAW_IB         1'b0
    1       R/W AON_REG_LOP_POF_SWR_CORE_POW_IMIR           1'b0
    2       R/W AON_REG_LOP_POF_LDOAUX1_POW_LDO533HQ        1'b0
    3       R/W AON_REG_LOP_POF_LDOAUX1_EN_POS              1'b0
    4       R/W AON_REG_LOP_POF_LDOSYS_POW_HQLQ533_PC       1'b1
    5       R/W AON_REG_LOP_POF_LDOSYS_POW_HQLQVCORE533_PC  1'b0
    6       R/W AON_REG_LOP_POF_LDOAUX1_POS_RST_B           1'b0
    7       R/W AON_REG_LOP_POF_LDOAUX1_POW_VREF            1'b0
    8       R/W AON_REG_LOP_POF_LDOSYS_POW_LDO533HQ         1'b0
    9       R/W AON_REG_LOP_POF_LDOSYS_EN_POS               1'b0
    10      R/W AON_REG_LOP_POF_LDOSYS_POW_LDO733LQ_VCORE   1'b0
    12:11   R/W AON_REG_LOP_POF_CHG_SEL_M2CCDFB             2'b11
    13      R/W AON_REG_LOP_POF_LDOSYS_POS_RST_B            1'b0
    14      R/W AON_REG_LOP_POF_LDOSYS_POW_LDOVREF          1'b0
    15      R/W AON_REG_LOP_POF_CHG_POW_M2                  1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_SAW_IB: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_IMIR: 1;
        uint16_t AON_REG_LOP_POF_LDOAUX1_POW_LDO533HQ: 1;
        uint16_t AON_REG_LOP_POF_LDOAUX1_EN_POS: 1;
        uint16_t AON_REG_LOP_POF_LDOSYS_POW_HQLQ533_PC: 1;
        uint16_t AON_REG_LOP_POF_LDOSYS_POW_HQLQVCORE533_PC: 1;
        uint16_t AON_REG_LOP_POF_LDOAUX1_POS_RST_B: 1;
        uint16_t AON_REG_LOP_POF_LDOAUX1_POW_VREF: 1;
        uint16_t AON_REG_LOP_POF_LDOSYS_POW_LDO533HQ: 1;
        uint16_t AON_REG_LOP_POF_LDOSYS_EN_POS: 1;
        uint16_t AON_REG_LOP_POF_LDOSYS_POW_LDO733LQ_VCORE: 1;
        uint16_t AON_REG_LOP_POF_CHG_SEL_M2CCDFB: 2;
        uint16_t AON_REG_LOP_POF_LDOSYS_POS_RST_B: 1;
        uint16_t AON_REG_LOP_POF_LDOSYS_POW_LDOVREF: 1;
        uint16_t AON_REG_LOP_POF_CHG_POW_M2: 1;
    };
} AON_FAST_AON_REG_LOP_POF_RG1X_TYPE;

/* 0x854
    0       R/W AON_REG_LOP_POF_SWR_CORE_POW_ZCD_COMP_LOWIQ 1'b0
    6:1     R/W AON_REG_LOP_POF_SWR_CORE_TUNE_BNYCNT_INI    6'b000000
    7       R/W AON_REG_LOP_POF_SWR_CORE_POW_BNYCNT_1       1'b0
    8       R/W AON_REG_LOP_POF_SWR_CORE_FPWM_1             1'b0
    9       R/W AON_REG_LOP_POF_SWR_CORE_POW_OCP            1'b0
    10      R/W AON_REG_LOP_POF_SWR_CORE_POW_ZCD            1'b0
    11      R/W AON_REG_LOP_POF_SWR_CORE_POW_PFM            1'b0
    12      R/W AON_REG_LOP_POF_SWR_CORE_POW_PWM            1'b0
    13      R/W AON_REG_LOP_POF_SWR_CORE_POW_VDIV           1'b0
    14      R/W AON_REG_LOP_POF_SWR_CORE_POW_REF            1'b0
    15      R/W AON_REG_LOP_POF_SWR_CORE_POW_SAW            1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_TUNE_BNYCNT_INI: 6;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_BNYCNT_1: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_FPWM_1: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_OCP: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_ZCD: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_PFM: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_PWM: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_VDIV: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_REF: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_SAW: 1;
    };
} AON_FAST_AON_REG_LOP_POF_RG2X_TYPE;

/* 0x856
    0       R/W AON_REG_LOP_POF_RG3X_DUMMY1                 1'h0
    1       R/W AON_REG_LOP_POF_SWR_CORE_POW_BNYCNT_2       1'b0
    2       R/W AON_REG_LOP_POF_LDO_DIG_POW_LDODIG          1'b0
    3       R/W AON_REG_LOP_POF_LDO_DIG_EN_POS              1'b0
    4       R/W AON_REG_LOP_POF_LDO_DIG_EN_LDODIG_PC        1'b0
    6:5     R/W AON_REG_LOP_POF_XTAL_LPS_CAP_STEP           2'b01
    8:7     R/W AON_REG_LOP_POF_XTAL_LPS_CAP_CYC            2'b00
    9       R/W AON_REG_LOP_POF_LDO_DIG_POS_RST_B           1'b0
    14:10   R/W AON_REG_LOP_POF_LDO_DIG_TUNE_LDODIG_VOUT    5'b10110
    15      R/W AON_REG_LOP_POF_LDO_DIG_POW_LDODIG_VREF     1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_RG3X_DUMMY1: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_BNYCNT_2: 1;
        uint16_t AON_REG_LOP_POF_LDO_DIG_POW_LDODIG: 1;
        uint16_t AON_REG_LOP_POF_LDO_DIG_EN_POS: 1;
        uint16_t AON_REG_LOP_POF_LDO_DIG_EN_LDODIG_PC: 1;
        uint16_t AON_REG_LOP_POF_XTAL_LPS_CAP_STEP: 2;
        uint16_t AON_REG_LOP_POF_XTAL_LPS_CAP_CYC: 2;
        uint16_t AON_REG_LOP_POF_LDO_DIG_POS_RST_B: 1;
        uint16_t AON_REG_LOP_POF_LDO_DIG_TUNE_LDODIG_VOUT: 5;
        uint16_t AON_REG_LOP_POF_LDO_DIG_POW_LDODIG_VREF: 1;
    };
} AON_FAST_AON_REG_LOP_POF_RG3X_TYPE;

/* 0x858
    3:0     R/W AON_REG_LOP_POF_RG4X_DUMMY1                 4'h0
    11:4    R/W AON_REG_LOP_POF_SWR_CORE_TUNE_POS_VREFPFM   8'b01101110
    15:12   R/W AON_REG_LOP_POF_SWR_CORE_TUNE_REF_VREFLPPFM 4'b0110
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_RG4X_DUMMY1: 4;
        uint16_t AON_REG_LOP_POF_SWR_CORE_TUNE_POS_VREFPFM: 8;
        uint16_t AON_REG_LOP_POF_SWR_CORE_TUNE_REF_VREFLPPFM: 4;
    };
} AON_FAST_AON_REG_LOP_POF_RG4X_TYPE;

/* 0x85A
    0       R/W AON_REG_LOP_POF_BT_RET_RSTB                 1'b1
    1       R/W AON_REG_LOP_POF_VCORE_PC_POW_VCORE_PC_VG2   1'b1
    2       R/W AON_REG_LOP_POF_VCORE_PC_POW_VCORE_PC_VG1   1'b1
    3       R/W AON_REG_LOP_POF_LDO_DIG_POW_LDORET          1'b0
    4       R/W AON_REG_LOP_POF_SWR_CORE_POW_SWR            1'b0
    5       R/W AON_REG_LOP_POF_SWR_CORE_POW_LDO            1'b0
    6       R/W AON_REG_LOP_POF_SWR_CORE_SEL_POS_VREFLPPFM  1'b0
    7       R/W AON_REG_LOP_POF_SWR_CORE_FPWM_2             1'b0
    15:8    R/W AON_REG_LOP_POF_SWR_CORE_TUNE_VDIV          8'b10001010
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_BT_RET_RSTB: 1;
        uint16_t AON_REG_LOP_POF_VCORE_PC_POW_VCORE_PC_VG2: 1;
        uint16_t AON_REG_LOP_POF_VCORE_PC_POW_VCORE_PC_VG1: 1;
        uint16_t AON_REG_LOP_POF_LDO_DIG_POW_LDORET: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_SWR: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_POW_LDO: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_SEL_POS_VREFLPPFM: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_FPWM_2: 1;
        uint16_t AON_REG_LOP_POF_SWR_CORE_TUNE_VDIV: 8;
    };
} AON_FAST_AON_REG_LOP_POF_RG5X_TYPE;

/* 0x85C
    0       R/W AON_REG_LOP_POF_ISO_BT_PON                  1'b1
    1       R/W AON_REG_LOP_POF_ISO_BT_CORE                 1'b1
    2       R/W AON_REG_LOP_POF_ISO_PLL2                    1'b1
    3       R/W AON_REG_LOP_POF_ISO_PLL                     1'b1
    4       R/W AON_REG_LOP_POF_BT_PLL_LP_PLL_pow_cpop      1'b0
    5       R/W AON_REG_LOP_POF_BT_PLL_LP_PLL_pow_pll       1'b0
    6       R/W AON_REG_LOP_POF_BT_PLL_pow_pll              1'b0
    7       R/W AON_REG_LOP_POF_BT_PLL_LDO_pow_LDO          1'b0
    8       R/W AON_REG_LOP_POF_BT_PLL_LDO_ERC_V12A_BTPLL   1'b0
    9       R/W AON_REG_LOP_POF_BT_PLL_LDO_SW_LDO2PORCUT    1'b0
    10      R/W AON_REG_LOP_POF_ISO_XTAL                    1'b1
    11      R/W AON_REG_LOP_POF_OSC40M_POW_OSC              1'b0
    14:12   R/W AON_REG_LOP_POF_XTAL_MODE                   3'b100
    15      R/W AON_REG_LOP_POF_XTAL_POW_XTAL               1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_ISO_BT_PON: 1;
        uint16_t AON_REG_LOP_POF_ISO_BT_CORE: 1;
        uint16_t AON_REG_LOP_POF_ISO_PLL2: 1;
        uint16_t AON_REG_LOP_POF_ISO_PLL: 1;
        uint16_t AON_REG_LOP_POF_BT_PLL_LP_PLL_pow_cpop: 1;
        uint16_t AON_REG_LOP_POF_BT_PLL_LP_PLL_pow_pll: 1;
        uint16_t AON_REG_LOP_POF_BT_PLL_pow_pll: 1;
        uint16_t AON_REG_LOP_POF_BT_PLL_LDO_pow_LDO: 1;
        uint16_t AON_REG_LOP_POF_BT_PLL_LDO_ERC_V12A_BTPLL: 1;
        uint16_t AON_REG_LOP_POF_BT_PLL_LDO_SW_LDO2PORCUT: 1;
        uint16_t AON_REG_LOP_POF_ISO_XTAL: 1;
        uint16_t AON_REG_LOP_POF_OSC40M_POW_OSC: 1;
        uint16_t AON_REG_LOP_POF_XTAL_MODE: 3;
        uint16_t AON_REG_LOP_POF_XTAL_POW_XTAL: 1;
    };
} AON_FAST_AON_REG_LOP_POF_RG6X_TYPE;

/* 0x85E
    7:0     R/W AON_REG_LOP_POF_RG7X_DUMMY1                 8'h0
    8       R/W AON_REG_LOP_POF_RFC_STORE                   1'b0
    9       R/W AON_REG_LOP_POF_PF_STORE                    1'b0
    10      R/W AON_REG_LOP_POF_MODEM_STORE                 1'b0
    11      R/W AON_REG_LOP_POF_DP_MODEM_STORE              1'b0
    12      R/W AON_REG_LOP_POF_BZ_STORE                    1'b0
    13      R/W AON_REG_LOP_POF_BLE_STORE                   1'b0
    14      R/W AON_REG_LOP_POF_BT_CORE_RSTB                1'b0
    15      R/W AON_REG_LOP_POF_BT_PON_RSTB                 1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG7X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_RG7X_DUMMY1: 8;
        uint16_t AON_REG_LOP_POF_RFC_STORE: 1;
        uint16_t AON_REG_LOP_POF_PF_STORE: 1;
        uint16_t AON_REG_LOP_POF_MODEM_STORE: 1;
        uint16_t AON_REG_LOP_POF_DP_MODEM_STORE: 1;
        uint16_t AON_REG_LOP_POF_BZ_STORE: 1;
        uint16_t AON_REG_LOP_POF_BLE_STORE: 1;
        uint16_t AON_REG_LOP_POF_BT_CORE_RSTB: 1;
        uint16_t AON_REG_LOP_POF_BT_PON_RSTB: 1;
    };
} AON_FAST_AON_REG_LOP_POF_RG7X_TYPE;

/* 0x860
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG8X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG8X_TYPE;

/* 0x862
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG9X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG9X_TYPE;

/* 0x864
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG10X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG10X_TYPE;

/* 0x866
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG11X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG11X_TYPE;

/* 0x868
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG12X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG12X_TYPE;

/* 0x86A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG13X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG13X_TYPE;

/* 0x86C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG14X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG14X_TYPE;

/* 0x86E
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG15X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG15X_TYPE;

/* 0x870
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG16X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG16X_TYPE;

/* 0x872
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG17X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG17X_TYPE;

/* 0x874
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG18X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG18X_TYPE;

/* 0x876
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG19X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG19X_TYPE;

/* 0x878
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG20X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG20X_TYPE;

/* 0x87A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG21X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG21X_TYPE;

/* 0x87C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_RG22X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_RG22X_TYPE;

/* 0x87E
    7:0     R/W LOP_POF_M1M2_DELAY                          8'ha0
    15:8    R/W LOP_POF_BIAS_DELAY                          8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_DELAY_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_M1M2_DELAY: 8;
        uint16_t LOP_POF_BIAS_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_POF_DELAY_RG0X_TYPE;

/* 0x880
    7:0     R/W LOP_POF_SWR_BIAS_DELAY                      8'ha0
    15:8    R/W LOP_POF_LDOHQ_DELAY                         8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_DELAY_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_SWR_BIAS_DELAY: 8;
        uint16_t LOP_POF_LDOHQ_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_POF_DELAY_RG1X_TYPE;

/* 0x882
    7:0     R/W LOP_POF_VCORE_DELAY                         8'ha0
    15:8    R/W LOP_POF_SWR_DELAY                           8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_DELAY_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_VCORE_DELAY: 8;
        uint16_t LOP_POF_SWR_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_POF_DELAY_RG2X_TYPE;

/* 0x884
    7:0     R/W LOP_POF_RST_DELAY                           8'ha0
    15:8    R/W LOP_POF_STORE_DELAY                         8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_DELAY_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RST_DELAY: 8;
        uint16_t LOP_POF_STORE_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_POF_DELAY_RG3X_TYPE;

/* 0x886
    7:0     R/W LOP_POF_PLL_DELAY                           8'ha0
    15:8    R/W LOP_POF_XTAL_DELAY                          8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_DELAY_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_PLL_DELAY: 8;
        uint16_t LOP_POF_XTAL_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_POF_DELAY_RG4X_TYPE;

/* 0x888
    7:0     R/W AON_REG_LOP_POF_DELAY_RG5X_DUMMY0           8'ha0
    15:8    R/W LOP_POF_ISO_DELAY                           8'ha0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_DELAY_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG_LOP_POF_DELAY_RG5X_DUMMY0: 8;
        uint16_t LOP_POF_ISO_DELAY: 8;
    };
} AON_FAST_AON_REG_LOP_POF_DELAY_RG5X_TYPE;

/* 0x88A
    14:0    R/W LOP_POF_MISC_DUMMY1                         15'h0
    15      R/W LOP_POF_AON_GATED_EN                        1'b0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_MISC_DUMMY1: 15;
        uint16_t LOP_POF_AON_GATED_EN: 1;
    };
} AON_FAST_AON_REG_LOP_POF_MISC_TYPE;

/* 0x88C
    15:0    R/W RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG_LOP_POF_ECO_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG_LOP_POF_ECO_RG0X_TYPE;

/* 0x8A0
    8:0     R/W REG0X_WAIT_READY_DUMMY0                     8'h0
    9       R/W WAIT_XTAL_CLK_OK                            1'b0
    10      R/W WAIT_SWR_CORE_POR_DET                       1'b0
    11      R/W REG0X_WAIT_READY_DUMMY11                    1'b0
    12      R/W REG0X_WAIT_READY_DUMMY12                    1'b0
    13      R/W REG0X_WAIT_READY_DUMMY13                    1'b0
    14      R/W WAIT_HV_DET                                 1'b0
    15      R/W WAIT_HV33_2D5_DET                           1'b0
 */
typedef union _AON_FAST_REG0X_WAIT_READY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_WAIT_READY_DUMMY0: 9;
        uint16_t WAIT_XTAL_CLK_OK: 1;
        uint16_t WAIT_SWR_CORE_POR_DET: 1;
        uint16_t REG0X_WAIT_READY_DUMMY11: 1;
        uint16_t REG0X_WAIT_READY_DUMMY12: 1;
        uint16_t REG0X_WAIT_READY_DUMMY13: 1;
        uint16_t WAIT_HV_DET: 1;
        uint16_t WAIT_HV33_2D5_DET: 1;
    };
} AON_FAST_REG0X_WAIT_READY_TYPE;

/* 0x8A2
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG1X_WAIT_READY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG1X_WAIT_READY_TYPE;

/* 0x8A4
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_BIAS             16'h0
 */
typedef union _AON_FAST_REG0X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_BIAS;
    };
} AON_FAST_REG0X_WAIT_AON_CNT_TYPE;

/* 0x8A6
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_M1M2             16'h0
 */
typedef union _AON_FAST_REG1X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_M1M2;
    };
} AON_FAST_REG1X_WAIT_AON_CNT_TYPE;

/* 0x8A8
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG2X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG2X_WAIT_AON_CNT_TYPE;

/* 0x8AA
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG3X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG3X_WAIT_AON_CNT_TYPE;

/* 0x8AC
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG4X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG4X_WAIT_AON_CNT_TYPE;

/* 0x8AE
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_SWR              16'h0
 */
typedef union _AON_FAST_REG5X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_SWR;
    };
} AON_FAST_REG5X_WAIT_AON_CNT_TYPE;

/* 0x8B0
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG6X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG6X_WAIT_AON_CNT_TYPE;

/* 0x8B2
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG7X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG7X_WAIT_AON_CNT_TYPE;

/* 0x8B4
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG8X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG8X_WAIT_AON_CNT_TYPE;

/* 0x8B6
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG9X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG9X_WAIT_AON_CNT_TYPE;

/* 0x8B8
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_XTAL             16'h0
 */
typedef union _AON_FAST_REG10X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_XTAL;
    };
} AON_FAST_REG10X_WAIT_AON_CNT_TYPE;

/* 0x8BA
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG11X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG11X_WAIT_AON_CNT_TYPE;

/* 0x8BC
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG12X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG12X_WAIT_AON_CNT_TYPE;

/* 0x8BE
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG13X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG13X_WAIT_AON_CNT_TYPE;

/* 0x8C0
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG14X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG14X_WAIT_AON_CNT_TYPE;

/* 0x8C2
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG15X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG15X_WAIT_AON_CNT_TYPE;

/* 0x8C4
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG16X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG16X_WAIT_AON_CNT_TYPE;

/* 0x8C6
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG17X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG17X_WAIT_AON_CNT_TYPE;

/* 0x8C8
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG18X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG18X_WAIT_AON_CNT_TYPE;

/* 0x8CA
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_REG19X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_REG19X_WAIT_AON_CNT_TYPE;

/* 0x8CC
    11:0    R/W SET_WKEN_REG0X_DUMMY1                       12'h0
    12      R/W USB_WKEN                                    1'b0
    13      R/W MFB_WKEN                                    1'b0
    14      R/W BAT_WKEN                                    1'b0
    15      R/W ADP_WKEN                                    1'b0
 */
typedef union _AON_FAST_SET_WKEN_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SET_WKEN_REG0X_DUMMY1: 12;
        uint16_t USB_WKEN: 1;
        uint16_t MFB_WKEN: 1;
        uint16_t BAT_WKEN: 1;
        uint16_t ADP_WKEN: 1;
    };
} AON_FAST_SET_WKEN_MISC_TYPE;

/* 0x8CE
    11:0    R/W SET_WKPOL_REG0X_DUMMY1                      12'h0
    12      R/W USB_WKPOL                                   1'b0
    13      R/W MFB_WKPOL                                   1'b0
    14      R/W BAT_WKPOL                                   1'b0
    15      R/W ADP_WKPOL                                   1'b0
 */
typedef union _AON_FAST_SET_WKPOL_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SET_WKPOL_REG0X_DUMMY1: 12;
        uint16_t USB_WKPOL: 1;
        uint16_t MFB_WKPOL: 1;
        uint16_t BAT_WKPOL: 1;
        uint16_t ADP_WKPOL: 1;
    };
} AON_FAST_SET_WKPOL_MISC_TYPE;

/* 0x8D0
    7:0     R   RO_WK_REG0X_DUMMY1                          1'b0
    15:8    R   RO_WK_REASON                                1'b0
 */
typedef union _AON_FAST_RO_WK_REG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_WK_REG0X_DUMMY1: 8;
        uint16_t RO_WK_REASON: 8;
    };
} AON_FAST_RO_WK_REG0X_TYPE;

/* 0x8D2
    15:0    R/W SET_SHIP_MODE_DUMMY0                        15'h0
 */
typedef union _AON_FAST_SET_SHIP_MODE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SET_SHIP_MODE_DUMMY0;
    };
} AON_FAST_SET_SHIP_MODE_TYPE;

/* 0x8D4
    3:0     R/W SWR_DBG_MODE                                4'b0000
    4       R/W SWR_DBG_MODE_EN                             1'b0
    15:5    R/W SET_PMU_DBG_MODE_DUMMY4                     11'h0
 */
typedef union _AON_FAST_SET_PMU_DBG_MODE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_DBG_MODE: 4;
        uint16_t SWR_DBG_MODE_EN: 1;
        uint16_t SET_PMU_DBG_MODE_DUMMY4: 11;
    };
} AON_FAST_SET_PMU_DBG_MODE_TYPE;

/* 0x8D6
    1:0     R/W REG0X_32k_DUMMY0                            2'b00
    2       R/W D32K_REG_MANU_MODE_CCOT                     1'b0
    7:3     R/W D32K_REG_MANU_CCOT                          5'b00100
    13:8    R/W D32K_PFM_TARGET                             6'b001111
    14      R/W D32K_EN_32K_AUDIO                           1'b0
    15      R/W D32K_RSTB                                   1'b0
 */
typedef union _AON_FAST_REG0X_32k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_32k_DUMMY0: 2;
        uint16_t D32K_REG_MANU_MODE_CCOT: 1;
        uint16_t D32K_REG_MANU_CCOT: 5;
        uint16_t D32K_PFM_TARGET: 6;
        uint16_t D32K_EN_32K_AUDIO: 1;
        uint16_t D32K_RSTB: 1;
    };
} AON_FAST_REG0X_32k_TYPE;

/* 0x8D8
    4:0     R/W D32K_CCOUT_ST4                              5'b01010
    9:5     R/W D32K_CCOUT_ST3                              5'b01000
    14:10   R/W D32K_CCOUT_ST2                              5'b00010
    15      R/W REG1X_32k_DUMMY15                           1'b0
 */
typedef union _AON_FAST_REG1X_32k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t D32K_CCOUT_ST4: 5;
        uint16_t D32K_CCOUT_ST3: 5;
        uint16_t D32K_CCOUT_ST2: 5;
        uint16_t REG1X_32k_DUMMY15: 1;
    };
} AON_FAST_REG1X_32k_TYPE;

/* 0x8DA
    2:0     R   D32K_pfm_state                              3'b000
    8:3     R   REG2X_32k_DUMMY3                            6'h0
    9       R   D32K_en_audk                                1'b0
    15:10   R   D32K_count                                  6'h0
 */
typedef union _AON_FAST_REG2X_32k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t D32K_pfm_state: 3;
        uint16_t REG2X_32k_DUMMY3: 6;
        uint16_t D32K_en_audk: 1;
        uint16_t D32K_count: 6;
    };
} AON_FAST_REG2X_32k_TYPE;

/* 0x8DC
    1:0     R/W REG0X_300k_DUMMY0                           2'b00
    11:2    R/W D300k_PFM_LOWER_BND                         10'h0
    12      R/W D300k_EN_VDROP_DET                          1'b0
    13      R/W D300k_EN_32K_AUDIO                          1'b0
    14      R/W D300k_RSTB                                  1'b0
    15      R/W D300k_SEL_NI_ON                             1'b0
 */
typedef union _AON_FAST_REG0X_300k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_300k_DUMMY0: 2;
        uint16_t D300k_PFM_LOWER_BND: 10;
        uint16_t D300k_EN_VDROP_DET: 1;
        uint16_t D300k_EN_32K_AUDIO: 1;
        uint16_t D300k_RSTB: 1;
        uint16_t D300k_SEL_NI_ON: 1;
    };
} AON_FAST_REG0X_300k_TYPE;

/* 0x8DE
    0       R/W D300k_REG_MANU_MODE_CCOT                    1'b0
    5:1     R/W D300k_REG_MANU_CCOT                         5'b00100
    15:6    R/W D300k_PFM_UPPER_BND                         10'h0
 */
typedef union _AON_FAST_REG1X_300k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t D300k_REG_MANU_MODE_CCOT: 1;
        uint16_t D300k_REG_MANU_CCOT: 5;
        uint16_t D300k_PFM_UPPER_BND: 10;
    };
} AON_FAST_REG1X_300k_TYPE;

/* 0x8E0
    1:0     R/W REG2X_300k_DUMMY0                           2'b00
    3:2     R/W D300k_PFM_COMP_SAMPLE_CYC                   2'b00
    5:4     R/W D300k_SAMPLE_CYC                            2'b00
    10:6    R/W D300k_CCOT_FORCE                            5'b00100
    15:11   R/W D300k_CCOT_INIT                             5'b00100
 */
typedef union _AON_FAST_REG2X_300k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG2X_300k_DUMMY0: 2;
        uint16_t D300k_PFM_COMP_SAMPLE_CYC: 2;
        uint16_t D300k_SAMPLE_CYC: 2;
        uint16_t D300k_CCOT_FORCE: 5;
        uint16_t D300k_CCOT_INIT: 5;
    };
} AON_FAST_REG2X_300k_TYPE;

/* 0x8E2
    3:0     R   REG3X_300k_DUMMY0                           4'b0000
    4       R   D300k_SEL_FORCE                             1'b0
    5       R   D300k_en_audk                               1'b0
    15:6    R   D300k_pfm_count                             10'h0
 */
typedef union _AON_FAST_REG3X_300k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG3X_300k_DUMMY0: 4;
        uint16_t D300k_SEL_FORCE: 1;
        uint16_t D300k_en_audk: 1;
        uint16_t D300k_pfm_count: 10;
    };
} AON_FAST_REG3X_300k_TYPE;

/* 0x8E4
    2:0     R/W REG0X_ZCD_DUMMY0                            3'b000
    3       R/W ZCD_reg_UD_bypass                           1'b0
    4       R/W ZCD_reg_UD                                  1'b0
    5       R/W ZCD_ZCDQ_RSTB                               1'b0
    6       R/W ZCD_STICKY_CODE1                            1'b0
    7       R/W ZCD_POW_UD_DIG                              1'b0
    8       R/W ZCD_FORCE_CODE1                             1'b0
    15:9    R/W ZCD_FORCE1                                  7'b0110000
 */
typedef union _AON_FAST_REG0X_ZCD_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_ZCD_DUMMY0: 3;
        uint16_t ZCD_reg_UD_bypass: 1;
        uint16_t ZCD_reg_UD: 1;
        uint16_t ZCD_ZCDQ_RSTB: 1;
        uint16_t ZCD_STICKY_CODE1: 1;
        uint16_t ZCD_POW_UD_DIG: 1;
        uint16_t ZCD_FORCE_CODE1: 1;
        uint16_t ZCD_FORCE1: 7;
    };
} AON_FAST_REG0X_ZCD_TYPE;

/* 0x8E6
    2:0     R/W REG1X_ZCD_AUDIO_DUMMY0                      3'b000
    3       R/W ZCD_reg_UD_bypass_AUDIO                     1'b0
    4       R/W ZCD_reg_UD_AUDIO                            1'b0
    5       R/W ZCD_ZCDQ_RSTB_AUDIO                         1'b0
    6       R/W ZCD_STICKY_CODE1_AUDIO                      1'b0
    7       R/W ZCD_POW_UD_DIG_AUDIO                        1'b0
    8       R/W ZCD_FORCE_CODE1_AUDIO                       1'b0
    15:9    R/W ZCD_FORCE1_AUDIO                            7'b0110000
 */
typedef union _AON_FAST_REG0X_ZCD_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG1X_ZCD_AUDIO_DUMMY0: 3;
        uint16_t ZCD_reg_UD_bypass_AUDIO: 1;
        uint16_t ZCD_reg_UD_AUDIO: 1;
        uint16_t ZCD_ZCDQ_RSTB_AUDIO: 1;
        uint16_t ZCD_STICKY_CODE1_AUDIO: 1;
        uint16_t ZCD_POW_UD_DIG_AUDIO: 1;
        uint16_t ZCD_FORCE_CODE1_AUDIO: 1;
        uint16_t ZCD_FORCE1_AUDIO: 7;
    };
} AON_FAST_REG0X_ZCD_AUDIO_TYPE;

/* 0x940
    0       R/W EN_CHG_POW_M1_CTRL_AON                      1'b1
    1       R/W EN_CHG_POW_M2_DVDET_CTRL_AON                1'b1
    2       R/W EN_CHG_POW_M1_DVDET_CTRL_AON                1'b1
    3       R/W EN_CHG_EN_M1FON_LDO733_CTRL_AON             1'b1
    4       R/W EN_CHG_EN_M2FONBUF_CTRL_AON                 1'b1
    5       R/W EN_CHG_EN_M2FON1K_CTRL_AON                  1'b1
    6       R/W EN_POW32K_32KXTAL_CTRL_AON                  1'b1
    7       R/W EN_POW32K_32KOSC_CTRL_AON                   1'b1
    8       R/W EN_MBIAS_POW_VAUDIO_DET_CTRL_AON            1'b1
    9       R/W EN_MBIAS_POW_VDDCORE_DET_CTRL_AON           1'b1
    10      R/W EN_MBIAS_POW_VAUX_DET_CTRL_AON              1'b1
    11      R/W EN_MBIAS_POW_HV_DET_CTRL_AON                1'b1
    12      R/W EN_MBIAS_POW_VBAT_DET_CTRL_AON              1'b1
    13      R/W EN_MBIAS_POW_ADP_DET_CTRL_AON               1'b1
    14      R/W EN_MBIAS_POW_BIAS_500nA_CTRL_AON            1'b1
    15      R/W EN_MBIAS_POW_BIAS_CTRL_AON                  1'b1
 */
typedef union _AON_FAST_AON_REG0X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EN_CHG_POW_M1_CTRL_AON: 1;
        uint16_t EN_CHG_POW_M2_DVDET_CTRL_AON: 1;
        uint16_t EN_CHG_POW_M1_DVDET_CTRL_AON: 1;
        uint16_t EN_CHG_EN_M1FON_LDO733_CTRL_AON: 1;
        uint16_t EN_CHG_EN_M2FONBUF_CTRL_AON: 1;
        uint16_t EN_CHG_EN_M2FON1K_CTRL_AON: 1;
        uint16_t EN_POW32K_32KXTAL_CTRL_AON: 1;
        uint16_t EN_POW32K_32KOSC_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_VAUDIO_DET_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_VDDCORE_DET_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_VAUX_DET_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_HV_DET_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_VBAT_DET_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_ADP_DET_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_BIAS_500nA_CTRL_AON: 1;
        uint16_t EN_MBIAS_POW_BIAS_CTRL_AON: 1;
    };
} AON_FAST_AON_REG0X_MUX_SEL_TYPE;

/* 0x942
    0       R/W EN_SWR_CORE_POW_SAW_CTRL_AON                1'b1
    1       R/W EN_SWR_CORE_POW_SAW_IB_CTRL_AON             1'b1
    2       R/W EN_SWR_CORE_POW_IMIR_CTRL_AON               1'b1
    3       R/W EN_LDOAUX1_POW_LDO533HQ_CTRL_AON            1'b1
    4       R/W EN_LDOAUX1_EN_POS_CTRL_AON                  1'b1
    5       R/W EN_LDOSYS_POW_HQLQ533_PC_CTRL_AON           1'b1
    6       R/W EN_LDOSYS_POW_HQLQVCORE533_PC_CTRL_AON      1'b1
    7       R/W EN_LDOAUX1_POS_RST_B_CTRL_AON               1'b1
    8       R/W EN_LDOAUX1_POW_VREF_CTRL_AON                1'b1
    9       R/W EN_LDOSYS_POW_LDO533HQ_CTRL_AON             1'b1
    10      R/W EN_LDOSYS_EN_POS_CTRL_AON                   1'b1
    11      R/W EN_LDOSYS_POW_LDO733LQ_VCORE_CTRL_AON       1'b1
    12      R/W EN_CHG_SEL_M2CCDFB_CTRL_AON                 1'b1
    13      R/W EN_LDOSYS_POS_RST_B_CTRL_AON                1'b1
    14      R/W EN_LDOSYS_POW_LDOVREF_CTRL_AON              1'b1
    15      R/W EN_CHG_POW_M2_CTRL_AON                      1'b1
 */
typedef union _AON_FAST_AON_REG1X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EN_SWR_CORE_POW_SAW_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_SAW_IB_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_IMIR_CTRL_AON: 1;
        uint16_t EN_LDOAUX1_POW_LDO533HQ_CTRL_AON: 1;
        uint16_t EN_LDOAUX1_EN_POS_CTRL_AON: 1;
        uint16_t EN_LDOSYS_POW_HQLQ533_PC_CTRL_AON: 1;
        uint16_t EN_LDOSYS_POW_HQLQVCORE533_PC_CTRL_AON: 1;
        uint16_t EN_LDOAUX1_POS_RST_B_CTRL_AON: 1;
        uint16_t EN_LDOAUX1_POW_VREF_CTRL_AON: 1;
        uint16_t EN_LDOSYS_POW_LDO533HQ_CTRL_AON: 1;
        uint16_t EN_LDOSYS_EN_POS_CTRL_AON: 1;
        uint16_t EN_LDOSYS_POW_LDO733LQ_VCORE_CTRL_AON: 1;
        uint16_t EN_CHG_SEL_M2CCDFB_CTRL_AON: 1;
        uint16_t EN_LDOSYS_POS_RST_B_CTRL_AON: 1;
        uint16_t EN_LDOSYS_POW_LDOVREF_CTRL_AON: 1;
        uint16_t EN_CHG_POW_M2_CTRL_AON: 1;
    };
} AON_FAST_AON_REG1X_MUX_SEL_TYPE;

/* 0x944
    0       R/W EN_LDO_DIG_EN_LDODIG_PC_CTRL_AON            1'b1
    1       R/W EN_XTAL_LPS_CAP_STEP_CTRL_AON               1'b1
    2       R/W EN_XTAL_LPS_CAP_CYC_CTRL_AON                1'b1
    3       R/W EN_LDO_DIG_POS_RST_B_CTRL_AON               1'b1
    4       R/W EN_LDO_DIG_TUNE_LDODIG_VOUT_CTRL_AON        1'b1
    5       R/W EN_LDO_DIG_POW_LDODIG_VREF_CTRL_AON         1'b1
    6       R/W EN_SWR_CORE_POW_ZCD_COMP_LOWIQ_CTRL_AON     1'b1
    7       R/W EN_SWR_CORE_TUNE_BNYCNT_INI_CTRL_AON        1'b1
    8       R/W EN_SWR_CORE_POW_BNYCNT_1_CTRL_AON           1'b1
    9       R/W EN_SWR_CORE_FPWM_1_CTRL_AON                 1'b1
    10      R/W EN_SWR_CORE_POW_OCP_CTRL_AON                1'b1
    11      R/W EN_SWR_CORE_POW_ZCD_CTRL_AON                1'b1
    12      R/W EN_SWR_CORE_POW_PFM_CTRL_AON                1'b1
    13      R/W EN_SWR_CORE_POW_PWM_CTRL_AON                1'b1
    14      R/W EN_SWR_CORE_POW_VDIV_CTRL_AON               1'b1
    15      R/W EN_SWR_CORE_POW_REF_CTRL_AON                1'b1
 */
typedef union _AON_FAST_AON_REG2X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EN_LDO_DIG_EN_LDODIG_PC_CTRL_AON: 1;
        uint16_t EN_XTAL_LPS_CAP_STEP_CTRL_AON: 1;
        uint16_t EN_XTAL_LPS_CAP_CYC_CTRL_AON: 1;
        uint16_t EN_LDO_DIG_POS_RST_B_CTRL_AON: 1;
        uint16_t EN_LDO_DIG_TUNE_LDODIG_VOUT_CTRL_AON: 1;
        uint16_t EN_LDO_DIG_POW_LDODIG_VREF_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_ZCD_COMP_LOWIQ_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_TUNE_BNYCNT_INI_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_BNYCNT_1_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_FPWM_1_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_OCP_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_ZCD_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_PFM_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_PWM_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_VDIV_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_REF_CTRL_AON: 1;
    };
} AON_FAST_AON_REG2X_MUX_SEL_TYPE;

/* 0x946
    0       R/W EN_DP_MODEM_RESTORE_CTRL_AON                1'b1
    1       R/W EN_BZ_RESTORE_CTRL_AON                      1'b1
    2       R/W EN_BLE_RESTORE_CTRL_AON                     1'b1
    3       R/W EN_VCORE_PC_POW_VCORE_PC_VG2_CTRL_AON       1'b1
    4       R/W EN_VCORE_PC_POW_VCORE_PC_VG1_CTRL_AON       1'b1
    5       R/W EN_LDO_DIG_POW_LDORET_CTRL_AON              1'b1
    6       R/W EN_SWR_CORE_POW_SWR_CTRL_AON                1'b1
    7       R/W EN_SWR_CORE_POW_LDO_CTRL_AON                1'b1
    8       R/W EN_SWR_CORE_SEL_POS_VREFLPPFM_CTRL_AON      1'b1
    9       R/W EN_SWR_CORE_FPWM_2_CTRL_AON                 1'b1
    10      R/W EN_SWR_CORE_TUNE_VDIV_CTRL_AON              1'b1
    11      R/W EN_SWR_CORE_TUNE_POS_VREFPFM_CTRL_AON       1'b1
    12      R/W EN_SWR_CORE_TUNE_REF_VREFLPPFM_CTRL_AON     1'b1
    13      R/W EN_SWR_CORE_POW_BNYCNT_2_CTRL_AON           1'b1
    14      R/W EN_LDO_DIG_POW_LDODIG_CTRL_AON              1'b1
    15      R/W EN_LDO_DIG_EN_POS_CTRL_AON                  1'b1
 */
typedef union _AON_FAST_AON_REG3X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EN_DP_MODEM_RESTORE_CTRL_AON: 1;
        uint16_t EN_BZ_RESTORE_CTRL_AON: 1;
        uint16_t EN_BLE_RESTORE_CTRL_AON: 1;
        uint16_t EN_VCORE_PC_POW_VCORE_PC_VG2_CTRL_AON: 1;
        uint16_t EN_VCORE_PC_POW_VCORE_PC_VG1_CTRL_AON: 1;
        uint16_t EN_LDO_DIG_POW_LDORET_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_SWR_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_LDO_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_SEL_POS_VREFLPPFM_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_FPWM_2_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_TUNE_VDIV_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_TUNE_POS_VREFPFM_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_TUNE_REF_VREFLPPFM_CTRL_AON: 1;
        uint16_t EN_SWR_CORE_POW_BNYCNT_2_CTRL_AON: 1;
        uint16_t EN_LDO_DIG_POW_LDODIG_CTRL_AON: 1;
        uint16_t EN_LDO_DIG_EN_POS_CTRL_AON: 1;
    };
} AON_FAST_AON_REG3X_MUX_SEL_TYPE;

/* 0x948
    0       R/W EN_ISO_PLL2_CTRL_AON                        1'b1
    1       R/W EN_ISO_PLL_CTRL_AON                         1'b1
    2       R/W EN_BT_PLL_LP_PLL_pow_cpop_CTRL_AON          1'b1
    3       R/W EN_BT_PLL_LP_PLL_pow_pll_CTRL_AON           1'b1
    4       R/W EN_BT_PLL_pow_pll_CTRL_AON                  1'b1
    5       R/W EN_BT_PLL_LDO_pow_LDO_CTRL_AON              1'b1
    6       R/W EN_BT_PLL_LDO_ERC_V12A_BTPLL_CTRL_AON       1'b1
    7       R/W EN_BT_PLL_LDO_SW_LDO2PORCUT_CTRL_AON        1'b1
    8       R/W EN_ISO_XTAL_CTRL_AON                        1'b1
    9       R/W EN_OSC40M_POW_OSC_CTRL_AON                  1'b1
    10      R/W EN_XTAL_MODE_CTRL_AON                       1'b1
    11      R/W EN_XTAL_POW_XTAL_CTRL_AON                   1'b1
    12      R/W EN_BT_RET_RSTB_CTRL_AON                     1'b1
    13      R/W EN_RFC_RESTORE_CTRL_AON                     1'b1
    14      R/W EN_PF_RESTORE_CTRL_AON                      1'b1
    15      R/W EN_MODEM_RESTORE_CTRL_AON                   1'b1
 */
typedef union _AON_FAST_AON_REG4X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EN_ISO_PLL2_CTRL_AON: 1;
        uint16_t EN_ISO_PLL_CTRL_AON: 1;
        uint16_t EN_BT_PLL_LP_PLL_pow_cpop_CTRL_AON: 1;
        uint16_t EN_BT_PLL_LP_PLL_pow_pll_CTRL_AON: 1;
        uint16_t EN_BT_PLL_pow_pll_CTRL_AON: 1;
        uint16_t EN_BT_PLL_LDO_pow_LDO_CTRL_AON: 1;
        uint16_t EN_BT_PLL_LDO_ERC_V12A_BTPLL_CTRL_AON: 1;
        uint16_t EN_BT_PLL_LDO_SW_LDO2PORCUT_CTRL_AON: 1;
        uint16_t EN_ISO_XTAL_CTRL_AON: 1;
        uint16_t EN_OSC40M_POW_OSC_CTRL_AON: 1;
        uint16_t EN_XTAL_MODE_CTRL_AON: 1;
        uint16_t EN_XTAL_POW_XTAL_CTRL_AON: 1;
        uint16_t EN_BT_RET_RSTB_CTRL_AON: 1;
        uint16_t EN_RFC_RESTORE_CTRL_AON: 1;
        uint16_t EN_PF_RESTORE_CTRL_AON: 1;
        uint16_t EN_MODEM_RESTORE_CTRL_AON: 1;
    };
} AON_FAST_AON_REG4X_MUX_SEL_TYPE;

/* 0x94A
    5:0     R/W AON_REG5X_MUX_SEL_DUMMY5                    6'b0
    6       R/W EN_RFC_STORE_CTRL_AON                       1'b1
    7       R/W EN_PF_STORE_CTRL_AON                        1'b1
    8       R/W EN_MODEM_STORE_CTRL_AON                     1'b1
    9       R/W EN_DP_MODEM_STORE_CTRL_AON                  1'b1
    10      R/W EN_BZ_STORE_CTRL_AON                        1'b1
    11      R/W EN_BLE_STORE_CTRL_AON                       1'b1
    12      R/W EN_BT_CORE_RSTB_CTRL_AON                    1'b1
    13      R/W EN_BT_PON_RSTB_CTRL_AON                     1'b1
    14      R/W EN_ISO_BT_PON_CTRL_AON                      1'b1
    15      R/W EN_ISO_BT_CORE_CTRL_AON                     1'b1
 */
typedef union _AON_FAST_AON_REG5X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_REG5X_MUX_SEL_DUMMY5: 6;
        uint16_t EN_RFC_STORE_CTRL_AON: 1;
        uint16_t EN_PF_STORE_CTRL_AON: 1;
        uint16_t EN_MODEM_STORE_CTRL_AON: 1;
        uint16_t EN_DP_MODEM_STORE_CTRL_AON: 1;
        uint16_t EN_BZ_STORE_CTRL_AON: 1;
        uint16_t EN_BLE_STORE_CTRL_AON: 1;
        uint16_t EN_BT_CORE_RSTB_CTRL_AON: 1;
        uint16_t EN_BT_PON_RSTB_CTRL_AON: 1;
        uint16_t EN_ISO_BT_PON_CTRL_AON: 1;
        uint16_t EN_ISO_BT_CORE_CTRL_AON: 1;
    };
} AON_FAST_AON_REG5X_MUX_SEL_TYPE;

/* 0x94C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG6X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG6X_MUX_SEL_TYPE;

/* 0x94E
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG7X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG7X_MUX_SEL_TYPE;

/* 0x950
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG8X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG8X_MUX_SEL_TYPE;

/* 0x952
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG9X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG9X_MUX_SEL_TYPE;

/* 0x954
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG10X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG10X_MUX_SEL_TYPE;

/* 0x956
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG11X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG11X_MUX_SEL_TYPE;

/* 0x958
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_AON_REG12X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_AON_REG12X_MUX_SEL_TYPE;

/* 0x95A
    13:0    R/W CORE_MODULE_REG0X_DUMMY1                    14'h0
    14      R/W sel_swr_ss_top                              1'b0
    15      R/W SWR_BY_CORE                                 1'b0
 */
typedef union _AON_FAST_CORE_MODULE_REG0X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_MODULE_REG0X_DUMMY1: 14;
        uint16_t sel_swr_ss_top: 1;
        uint16_t SWR_BY_CORE: 1;
    };
} AON_FAST_CORE_MODULE_REG0X_MUX_SEL_TYPE;

/* 0x970
    0       R   FAON_READ_CHG_POW_M1                        1'b1
    1       R   FAON_READ_CHG_POW_M2_DVDET                  1'b1
    2       R   FAON_READ_CHG_POW_M1_DVDET                  1'b1
    3       R   FAON_READ_CHG_EN_M1FON_LDO733               1'b0
    4       R   FAON_READ_CHG_EN_M2FONBUF                   1'b0
    5       R   FAON_READ_CHG_EN_M2FON1K                    1'b0
    6       R   FAON_READ_POW32K_32KXTAL                    1'b0
    7       R   FAON_READ_POW32K_32KOSC                     1'b1
    8       R   FAON_READ_MBIAS_POW_VAUDIO_DET              1'b1
    9       R   FAON_READ_MBIAS_POW_VDDCORE_DET             1'b1
    10      R   FAON_READ_MBIAS_POW_VAUX_DET                1'b1
    11      R   FAON_READ_MBIAS_POW_HV_DET                  1'b1
    12      R   FAON_READ_MBIAS_POW_VBAT_DET                1'b1
    13      R   FAON_READ_MBIAS_POW_ADP_DET                 1'b1
    14      R   FAON_READ_MBIAS_POW_BIAS_500nA              1'b1
    15      R   FAON_READ_MBIAS_POW_BIAS                    1'b1
 */
typedef union _AON_FAST_FAON_READ_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_CHG_POW_M1: 1;
        uint16_t FAON_READ_CHG_POW_M2_DVDET: 1;
        uint16_t FAON_READ_CHG_POW_M1_DVDET: 1;
        uint16_t FAON_READ_CHG_EN_M1FON_LDO733: 1;
        uint16_t FAON_READ_CHG_EN_M2FONBUF: 1;
        uint16_t FAON_READ_CHG_EN_M2FON1K: 1;
        uint16_t FAON_READ_POW32K_32KXTAL: 1;
        uint16_t FAON_READ_POW32K_32KOSC: 1;
        uint16_t FAON_READ_MBIAS_POW_VAUDIO_DET: 1;
        uint16_t FAON_READ_MBIAS_POW_VDDCORE_DET: 1;
        uint16_t FAON_READ_MBIAS_POW_VAUX_DET: 1;
        uint16_t FAON_READ_MBIAS_POW_HV_DET: 1;
        uint16_t FAON_READ_MBIAS_POW_VBAT_DET: 1;
        uint16_t FAON_READ_MBIAS_POW_ADP_DET: 1;
        uint16_t FAON_READ_MBIAS_POW_BIAS_500nA: 1;
        uint16_t FAON_READ_MBIAS_POW_BIAS: 1;
    };
} AON_FAST_FAON_READ_RG0X_TYPE;

/* 0x972
    0       R   FAON_READ_SWR_CORE_POW_SAW_IB               1'b1
    1       R   FAON_READ_SWR_CORE_POW_IMIR                 1'b1
    2       R   FAON_READ_LDOAUX1_POW_LDO533HQ              1'b1
    3       R   FAON_READ_LDOAUX1_EN_POS                    1'b1
    4       R   FAON_READ_LDOSYS_POW_HQLQ533_PC             1'b1
    5       R   FAON_READ_LDOSYS_POW_HQLQVCORE533_PC        1'b1
    6       R   FAON_READ_LDOAUX1_POS_RST_B                 1'b1
    7       R   FAON_READ_LDOAUX1_POW_VREF                  1'b1
    8       R   FAON_READ_LDOSYS_POW_LDO533HQ               1'b1
    9       R   FAON_READ_LDOSYS_EN_POS                     1'b1
    10      R   FAON_READ_LDOSYS_POW_LDO733LQ_VCORE         1'b1
    12:11   R   FAON_READ_CHG_SEL_M2CCDFB                   2'b11
    13      R   FAON_READ_LDOSYS_POS_RST_B                  1'b1
    14      R   FAON_READ_LDOSYS_POW_LDOVREF                1'b1
    15      R   FAON_READ_CHG_POW_M2                        1'b1
 */
typedef union _AON_FAST_FAON_READ_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_SWR_CORE_POW_SAW_IB: 1;
        uint16_t FAON_READ_SWR_CORE_POW_IMIR: 1;
        uint16_t FAON_READ_LDOAUX1_POW_LDO533HQ: 1;
        uint16_t FAON_READ_LDOAUX1_EN_POS: 1;
        uint16_t FAON_READ_LDOSYS_POW_HQLQ533_PC: 1;
        uint16_t FAON_READ_LDOSYS_POW_HQLQVCORE533_PC: 1;
        uint16_t FAON_READ_LDOAUX1_POS_RST_B: 1;
        uint16_t FAON_READ_LDOAUX1_POW_VREF: 1;
        uint16_t FAON_READ_LDOSYS_POW_LDO533HQ: 1;
        uint16_t FAON_READ_LDOSYS_EN_POS: 1;
        uint16_t FAON_READ_LDOSYS_POW_LDO733LQ_VCORE: 1;
        uint16_t FAON_READ_CHG_SEL_M2CCDFB: 2;
        uint16_t FAON_READ_LDOSYS_POS_RST_B: 1;
        uint16_t FAON_READ_LDOSYS_POW_LDOVREF: 1;
        uint16_t FAON_READ_CHG_POW_M2: 1;
    };
} AON_FAST_FAON_READ_RG1X_TYPE;

/* 0x974
    0       R   FAON_READ_SWR_CORE_POW_ZCD_COMP_LOWIQ       1'b0
    6:1     R   FAON_READ_SWR_CORE_TUNE_BNYCNT_INI          6'b000000
    7       R   FAON_READ_SWR_CORE_POW_BNYCNT_1             1'b0
    8       R   FAON_READ_SWR_CORE_FPWM_1                   1'b1
    9       R   FAON_READ_SWR_CORE_POW_OCP                  1'b0
    10      R   FAON_READ_SWR_CORE_POW_ZCD                  1'b0
    11      R   FAON_READ_SWR_CORE_POW_PFM                  1'b0
    12      R   FAON_READ_SWR_CORE_POW_PWM                  1'b0
    13      R   FAON_READ_SWR_CORE_POW_VDIV                 1'b1
    14      R   FAON_READ_SWR_CORE_POW_REF                  1'b1
    15      R   FAON_READ_SWR_CORE_POW_SAW                  1'b1
 */
typedef union _AON_FAST_FAON_READ_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t FAON_READ_SWR_CORE_TUNE_BNYCNT_INI: 6;
        uint16_t FAON_READ_SWR_CORE_POW_BNYCNT_1: 1;
        uint16_t FAON_READ_SWR_CORE_FPWM_1: 1;
        uint16_t FAON_READ_SWR_CORE_POW_OCP: 1;
        uint16_t FAON_READ_SWR_CORE_POW_ZCD: 1;
        uint16_t FAON_READ_SWR_CORE_POW_PFM: 1;
        uint16_t FAON_READ_SWR_CORE_POW_PWM: 1;
        uint16_t FAON_READ_SWR_CORE_POW_VDIV: 1;
        uint16_t FAON_READ_SWR_CORE_POW_REF: 1;
        uint16_t FAON_READ_SWR_CORE_POW_SAW: 1;
    };
} AON_FAST_FAON_READ_RG2X_TYPE;

/* 0x976
    0       R   FAON_READ_RG3X_DUMMY1                       1'h0
    1       R   FAON_READ_SWR_CORE_POW_BNYCNT_2             1'b1
    2       R   FAON_READ_LDO_DIG_POW_LDODIG                1'b1
    3       R   FAON_READ_LDO_DIG_EN_POS                    1'b1
    4       R   FAON_READ_LDO_DIG_EN_LDODIG_PC              1'b0
    6:5     R   FAON_READ_XTAL_LPS_CAP_STEP                 2'b01
    8:7     R   FAON_READ_XTAL_LPS_CAP_CYC                  2'b00
    9       R   FAON_READ_LDO_DIG_POS_RST_B                 1'b1
    14:10   R   FAON_READ_LDO_DIG_TUNE_LDODIG_VOUT          5'b10110
    15      R   FAON_READ_LDO_DIG_POW_LDODIG_VREF           1'b1
 */
typedef union _AON_FAST_FAON_READ_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_RG3X_DUMMY1: 1;
        uint16_t FAON_READ_SWR_CORE_POW_BNYCNT_2: 1;
        uint16_t FAON_READ_LDO_DIG_POW_LDODIG: 1;
        uint16_t FAON_READ_LDO_DIG_EN_POS: 1;
        uint16_t FAON_READ_LDO_DIG_EN_LDODIG_PC: 1;
        uint16_t FAON_READ_XTAL_LPS_CAP_STEP: 2;
        uint16_t FAON_READ_XTAL_LPS_CAP_CYC: 2;
        uint16_t FAON_READ_LDO_DIG_POS_RST_B: 1;
        uint16_t FAON_READ_LDO_DIG_TUNE_LDODIG_VOUT: 5;
        uint16_t FAON_READ_LDO_DIG_POW_LDODIG_VREF: 1;
    };
} AON_FAST_FAON_READ_RG3X_TYPE;

/* 0x978
    3:0     R   FAON_READ_RG4X_DUMMY1                       4'h0
    11:4    R   FAON_READ_SWR_CORE_TUNE_POS_VREFPFM         8'b01101110
    15:12   R   FAON_READ_SWR_CORE_TUNE_REF_VREFLPPFM       4'b0110
 */
typedef union _AON_FAST_FAON_READ_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_RG4X_DUMMY1: 4;
        uint16_t FAON_READ_SWR_CORE_TUNE_POS_VREFPFM: 8;
        uint16_t FAON_READ_SWR_CORE_TUNE_REF_VREFLPPFM: 4;
    };
} AON_FAST_FAON_READ_RG4X_TYPE;

/* 0x97A
    0       R   FAON_READ_BLE_RESTORE                       1'b0
    1       R   FAON_READ_VCORE_PC_POW_VCORE_PC_VG2         1'b0
    2       R   FAON_READ_VCORE_PC_POW_VCORE_PC_VG1         1'b0
    3       R   FAON_READ_LDO_DIG_POW_LDORET                1'b0
    4       R   FAON_READ_SWR_CORE_POW_SWR                  1'b0
    5       R   FAON_READ_SWR_CORE_POW_LDO                  1'b1
    6       R   FAON_READ_SWR_CORE_SEL_POS_VREFLPPFM        1'b0
    7       R   FAON_READ_SWR_CORE_FPWM_2                   1'b1
    15:8    R   FAON_READ_SWR_CORE_TUNE_VDIV                8'b10001010
 */
typedef union _AON_FAST_FAON_READ_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_BLE_RESTORE: 1;
        uint16_t FAON_READ_VCORE_PC_POW_VCORE_PC_VG2: 1;
        uint16_t FAON_READ_VCORE_PC_POW_VCORE_PC_VG1: 1;
        uint16_t FAON_READ_LDO_DIG_POW_LDORET: 1;
        uint16_t FAON_READ_SWR_CORE_POW_SWR: 1;
        uint16_t FAON_READ_SWR_CORE_POW_LDO: 1;
        uint16_t FAON_READ_SWR_CORE_SEL_POS_VREFLPPFM: 1;
        uint16_t FAON_READ_SWR_CORE_FPWM_2: 1;
        uint16_t FAON_READ_SWR_CORE_TUNE_VDIV: 8;
    };
} AON_FAST_FAON_READ_RG5X_TYPE;

/* 0x97C
    0       R   FAON_READ_BT_PLL_pow_pll                    1'b0
    1       R   FAON_READ_BT_PLL_LDO_pow_LDO                1'b0
    2       R   FAON_READ_BT_PLL_LDO_ERC_V12A_BTPLL         1'b0
    3       R   FAON_READ_BT_PLL_LDO_SW_LDO2PORCUT          1'b0
    4       R   FAON_READ_ISO_XTAL                          1'b0
    5       R   FAON_READ_OSC40M_POW_OSC                    1'b1
    8:6     R   FAON_READ_XTAL_MODE                         3'b100
    9       R   FAON_READ_XTAL_POW_XTAL                     1'b1
    10      R   FAON_READ_BT_RET_RSTB                       1'b1
    11      R   FAON_READ_RFC_RESTORE                       1'b0
    12      R   FAON_READ_PF_RESTORE                        1'b0
    13      R   FAON_READ_MODEM_RESTORE                     1'b0
    14      R   FAON_READ_DP_MODEM_RESTORE                  1'b0
    15      R   FAON_READ_BZ_RESTORE                        1'b0
 */
typedef union _AON_FAST_FAON_READ_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_BT_PLL_pow_pll: 1;
        uint16_t FAON_READ_BT_PLL_LDO_pow_LDO: 1;
        uint16_t FAON_READ_BT_PLL_LDO_ERC_V12A_BTPLL: 1;
        uint16_t FAON_READ_BT_PLL_LDO_SW_LDO2PORCUT: 1;
        uint16_t FAON_READ_ISO_XTAL: 1;
        uint16_t FAON_READ_OSC40M_POW_OSC: 1;
        uint16_t FAON_READ_XTAL_MODE: 3;
        uint16_t FAON_READ_XTAL_POW_XTAL: 1;
        uint16_t FAON_READ_BT_RET_RSTB: 1;
        uint16_t FAON_READ_RFC_RESTORE: 1;
        uint16_t FAON_READ_PF_RESTORE: 1;
        uint16_t FAON_READ_MODEM_RESTORE: 1;
        uint16_t FAON_READ_DP_MODEM_RESTORE: 1;
        uint16_t FAON_READ_BZ_RESTORE: 1;
    };
} AON_FAST_FAON_READ_RG6X_TYPE;

/* 0x97E
    1:0     R   FAON_READ_RG7X_DUMMY1                       2'h0
    2       R   FAON_READ_RFC_STORE                         1'b0
    3       R   FAON_READ_PF_STORE                          1'b0
    4       R   FAON_READ_MODEM_STORE                       1'b0
    5       R   FAON_READ_DP_MODEM_STORE                    1'b0
    6       R   FAON_READ_BZ_STORE                          1'b0
    7       R   FAON_READ_BLE_STORE                         1'b0
    8       R   FAON_READ_BT_CORE_RSTB                      1'b1
    9       R   FAON_READ_BT_PON_RSTB                       1'b1
    10      R   FAON_READ_ISO_BT_PON                        1'b0
    11      R   FAON_READ_ISO_BT_CORE                       1'b0
    12      R   FAON_READ_ISO_PLL2                          1'b1
    13      R   FAON_READ_ISO_PLL                           1'b1
    14      R   FAON_READ_BT_PLL_LP_PLL_pow_cpop            1'b0
    15      R   FAON_READ_BT_PLL_LP_PLL_pow_pll             1'b0
 */
typedef union _AON_FAST_FAON_READ_RG7X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FAON_READ_RG7X_DUMMY1: 2;
        uint16_t FAON_READ_RFC_STORE: 1;
        uint16_t FAON_READ_PF_STORE: 1;
        uint16_t FAON_READ_MODEM_STORE: 1;
        uint16_t FAON_READ_DP_MODEM_STORE: 1;
        uint16_t FAON_READ_BZ_STORE: 1;
        uint16_t FAON_READ_BLE_STORE: 1;
        uint16_t FAON_READ_BT_CORE_RSTB: 1;
        uint16_t FAON_READ_BT_PON_RSTB: 1;
        uint16_t FAON_READ_ISO_BT_PON: 1;
        uint16_t FAON_READ_ISO_BT_CORE: 1;
        uint16_t FAON_READ_ISO_PLL2: 1;
        uint16_t FAON_READ_ISO_PLL: 1;
        uint16_t FAON_READ_BT_PLL_LP_PLL_pow_cpop: 1;
        uint16_t FAON_READ_BT_PLL_LP_PLL_pow_pll: 1;
    };
} AON_FAST_FAON_READ_RG7X_TYPE;

/* 0x980
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG8X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG8X_TYPE;

/* 0x982
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG9X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG9X_TYPE;

/* 0x984
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG10X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG10X_TYPE;

/* 0x986
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG11X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG11X_TYPE;

/* 0x988
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG12X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG12X_TYPE;

/* 0x98A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG13X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG13X_TYPE;

/* 0x98C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG14X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG14X_TYPE;

/* 0x98E
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG15X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG15X_TYPE;

/* 0x990
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG16X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG16X_TYPE;

/* 0x992
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG17X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG17X_TYPE;

/* 0x994
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG18X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG18X_TYPE;

/* 0x996
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG19X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG19X_TYPE;

/* 0x998
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG20X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG20X_TYPE;

/* 0x99A
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG21X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG21X_TYPE;

/* 0x99C
    15:0    R   RSVD                                        16'h0
 */
typedef union _AON_FAST_FAON_READ_RG22X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_FAON_READ_RG22X_TYPE;

/* 0xC00
    0       R/W MBIAS_POW_HV_DET                            1'b0
    1       R/W MBIAS_POW_VBAT_DET                          1'b0
    2       R/W MBIAS_POW_ADP_DET                           1'b0
    3       R/W MBIAS_LDO318_EN_LDO318_IB20NA               1'b0
    7:4     R/W MBIAS_LDO318_TUNE_LDO318                    4'b0100
    10:8    R/W MBIAS_TUNE_500NA                            3'b100
    11      R/W MBIAS_REG_FOR_CODEC                         1'b0
    12      R/W MBIAS_LDO318_EN_LDO318_DL_B                 1'b0
    13      R/W MBIAS_LDO318_POW_LDO318                     1'b0
    14      R/W MBIAS_POW_BIAS_500nA                        1'b0
    15      R/W MBIAS_POW_BIAS                              1'b1
 */
typedef union _AON_FAST_REG0X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_POW_HV_DET: 1;
        uint16_t MBIAS_POW_VBAT_DET: 1;
        uint16_t MBIAS_POW_ADP_DET: 1;
        uint16_t MBIAS_LDO318_EN_LDO318_IB20NA: 1;
        uint16_t MBIAS_LDO318_TUNE_LDO318: 4;
        uint16_t MBIAS_TUNE_500NA: 3;
        uint16_t MBIAS_REG_FOR_CODEC: 1;
        uint16_t MBIAS_LDO318_EN_LDO318_DL_B: 1;
        uint16_t MBIAS_LDO318_POW_LDO318: 1;
        uint16_t MBIAS_POW_BIAS_500nA: 1;
        uint16_t MBIAS_POW_BIAS: 1;
    };
} AON_FAST_REG0X_MBIAS_TYPE;

/* 0xC02
    1:0     R/W MBIAS_SEL_LDO733LQ_2D5_VR_L                 2'b10
    3:2     R/W MBIAS_SEL_LDO733LQ_2D5_VR_H                 2'b10
    5:4     R/W MBIAS_SEL_DVDD_VR_L                         2'b10
    7:6     R/W MBIAS_SEL_DVDD_VR_H                         2'b10
    9:8     R/W MBIAS_SEL_ADPIN_VR_L                        2'b10
    11:10   R/W MBIAS_SEL_ADPIN_VR_H                        2'b10
    12      R/W MBIAS_POW_HV33_SWR_DET                      1'b0
    13      R/W MBIAS_POW_VAUDIO_DET                        1'b0
    14      R/W MBIAS_POW_VDDCORE_DET                       1'b0
    15      R/W MBIAS_POW_VAUX_DET                          1'b0
 */
typedef union _AON_FAST_REG1X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_LDO733LQ_2D5_VR_L: 2;
        uint16_t MBIAS_SEL_LDO733LQ_2D5_VR_H: 2;
        uint16_t MBIAS_SEL_DVDD_VR_L: 2;
        uint16_t MBIAS_SEL_DVDD_VR_H: 2;
        uint16_t MBIAS_SEL_ADPIN_VR_L: 2;
        uint16_t MBIAS_SEL_ADPIN_VR_H: 2;
        uint16_t MBIAS_POW_HV33_SWR_DET: 1;
        uint16_t MBIAS_POW_VAUDIO_DET: 1;
        uint16_t MBIAS_POW_VDDCORE_DET: 1;
        uint16_t MBIAS_POW_VAUX_DET: 1;
    };
} AON_FAST_REG1X_MBIAS_TYPE;

/* 0xC04
    1:0     R/W MBIAS_SEL_VAUDIO_VR_L                       2'b10
    3:2     R/W MBIAS_SEL_VAUDIO_VR_H                       2'b10
    5:4     R/W MBIAS_SEL_HV_VR_L                           2'b10
    7:6     R/W MBIAS_SEL_HV_VR_H                           2'b10
    9:8     R/W MBIAS_SEL_HV33_VR_L                         2'b10
    11:10   R/W MBIAS_SEL_HV33_VR_H                         2'b10
    13:12   R/W MBIAS_SEL_HV33_SWR_VR_L                     2'b10
    15:14   R/W MBIAS_SEL_HV33_SWR_VR_H                     2'b10
 */
typedef union _AON_FAST_REG2X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_VAUDIO_VR_L: 2;
        uint16_t MBIAS_SEL_VAUDIO_VR_H: 2;
        uint16_t MBIAS_SEL_HV_VR_L: 2;
        uint16_t MBIAS_SEL_HV_VR_H: 2;
        uint16_t MBIAS_SEL_HV33_VR_L: 2;
        uint16_t MBIAS_SEL_HV33_VR_H: 2;
        uint16_t MBIAS_SEL_HV33_SWR_VR_L: 2;
        uint16_t MBIAS_SEL_HV33_SWR_VR_H: 2;
    };
} AON_FAST_REG2X_MBIAS_TYPE;

/* 0xC06
    1:0     R/W MBIAS_AUX311_SEL_DL_LDO311_AUXADC_B         2'b00
    2       R/W MBIAS_AUX311_POW_PCUT_LDO311_AUXADC         1'b1
    3       R/W MBIAS_AUX311_POW_LDO311_AUXADC              1'b0
    5:4     R/W MBIAS_SEL_VDDCORE_VR_L                      2'b10
    7:6     R/W MBIAS_SEL_VDDCORE_VR_H                      1'b10
    9:8     R/W MBIAS_SEL_VBAT_VR_L                         2'b10
    11:10   R/W MBIAS_SEL_VBAT_VR_H                         2'b10
    13:12   R/W MBIAS_SEL_VAUX_VR_L                         2'b10
    15:14   R/W MBIAS_SEL_VAUX_VR_H                         2'b10
 */
typedef union _AON_FAST_REG3X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_AUX311_SEL_DL_LDO311_AUXADC_B: 2;
        uint16_t MBIAS_AUX311_POW_PCUT_LDO311_AUXADC: 1;
        uint16_t MBIAS_AUX311_POW_LDO311_AUXADC: 1;
        uint16_t MBIAS_SEL_VDDCORE_VR_L: 2;
        uint16_t MBIAS_SEL_VDDCORE_VR_H: 2;
        uint16_t MBIAS_SEL_VBAT_VR_L: 2;
        uint16_t MBIAS_SEL_VBAT_VR_H: 2;
        uint16_t MBIAS_SEL_VAUX_VR_L: 2;
        uint16_t MBIAS_SEL_VAUX_VR_H: 2;
    };
} AON_FAST_REG3X_MBIAS_TYPE;

/* 0xC08
    0       R/W MBIAS_REG4X_DUMMY0                          1'b0
    1       R/W MBIAS_REG4X_DUMMY1                          1'b0
    2       R/W MBIAS_REG4X_DUMMY2                          1'b0
    3       R/W MBIAS_REG4X_DUMMY3                          1'b0
    4       R/W MBIAS_AUX311_EN_DL_LDO311_AUXADC            1'b0
    5       R/W MBIAS_CODEC_LDO_PREC                        1'b0
    6       R/W MBIAS_POW_LVSFT                             1'b1
    7       R/W MBIAS_PCUT_VG2                              1'b1
    8       R/W MBIAS_PCUT_VG1                              1'b1
    9       R/W MBIAS_PCUT_ESDCTRL                          1'b1
    10      R/W MBIAS_AUX311_EN_LDO311_AUXADC_IB20NA        1'b0
    15:11   R/W MBIAS_AUX311_TUNE_LDO311_AUXADC             5'b10100
 */
typedef union _AON_FAST_REG4X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_REG4X_DUMMY0: 1;
        uint16_t MBIAS_REG4X_DUMMY1: 1;
        uint16_t MBIAS_REG4X_DUMMY2: 1;
        uint16_t MBIAS_REG4X_DUMMY3: 1;
        uint16_t MBIAS_AUX311_EN_DL_LDO311_AUXADC: 1;
        uint16_t MBIAS_CODEC_LDO_PREC: 1;
        uint16_t MBIAS_POW_LVSFT: 1;
        uint16_t MBIAS_PCUT_VG2: 1;
        uint16_t MBIAS_PCUT_VG1: 1;
        uint16_t MBIAS_PCUT_ESDCTRL: 1;
        uint16_t MBIAS_AUX311_EN_LDO311_AUXADC_IB20NA: 1;
        uint16_t MBIAS_AUX311_TUNE_LDO311_AUXADC: 5;
    };
} AON_FAST_REG4X_MBIAS_TYPE;

/* 0xC0A
    0       R/W MBIAS_REG5X_DUMMY0                          1'b0
    1       R/W MBIAS_REG5X_DUMMY1                          1'b0
    4:2     R/W MBIAS_TUNE_BG_R2                            3'b100
    7:5     R/W MBIAS_TUNE_BG_R1                            3'b100
    8       R/W MBIAS_EN_RC_LPBG_B                          1'b0
    9       R/W MBIAS_EN_RC_HV50NA_B                        1'b0
    13:10   R/W MBIAS_SEL_VR_AUXADC                         4'b1000
    15:14   R/W MBIAS_SEL_VR_CHG                            2'b10
 */
typedef union _AON_FAST_REG5X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_REG5X_DUMMY0: 1;
        uint16_t MBIAS_REG5X_DUMMY1: 1;
        uint16_t MBIAS_TUNE_BG_R2: 3;
        uint16_t MBIAS_TUNE_BG_R1: 3;
        uint16_t MBIAS_EN_RC_LPBG_B: 1;
        uint16_t MBIAS_EN_RC_HV50NA_B: 1;
        uint16_t MBIAS_SEL_VR_AUXADC: 4;
        uint16_t MBIAS_SEL_VR_CHG: 2;
    };
} AON_FAST_REG5X_MBIAS_TYPE;

/* 0xC0C
    5:0     R/W MBIAS_LDOAUX2_TUNE_LDO533HQ                 6'b001110
    6       R/W MBIAS_LDOAUX2_POW_VREF                      1'b0
    7       R/W MBIAS_LDOAUX2_POW_LDO733LQ                  1'b0
    8       R/W MBIAS_LDOAUX2_POW_LDO533HQ                  1'b0
    9       R/W MBIAS_FORCE_VBATSW_OFF                      1'b0
    12:10   R/W MBIAS_SEL_50NAIQ                            3'b000
    15:13   R/W MBIAS_TUNE_BG_VREF                          3'b100
 */
typedef union _AON_FAST_REG6X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_LDOAUX2_TUNE_LDO533HQ: 6;
        uint16_t MBIAS_LDOAUX2_POW_VREF: 1;
        uint16_t MBIAS_LDOAUX2_POW_LDO733LQ: 1;
        uint16_t MBIAS_LDOAUX2_POW_LDO533HQ: 1;
        uint16_t MBIAS_FORCE_VBATSW_OFF: 1;
        uint16_t MBIAS_SEL_50NAIQ: 3;
        uint16_t MBIAS_TUNE_BG_VREF: 3;
    };
} AON_FAST_REG6X_MBIAS_TYPE;

/* 0xC0E
    0       R/W MBIAS_REG7X_DUMMY0                          1'b0
    1       R/W MBIAS_REG7X_DUMMY1                          1'b0
    2       R/W MBIAS_LDOAUX2_EN_NOSS_LDO533HQ              1'b0
    3       R/W MBIAS_LDOAUX2_EN_DL_LDO533HQ                1'b0
    4       R/W MBIAS_LDOAUX2_ENB_DL_LDO733LQ_B             1'b0
    7:5     R/W MBIAS_LDOAUX2_SEL_VR_LDO533HQ               3'b000
    9:8     R/W MBIAS_LDOAUX2_TUNE_LDO533HQ_DL              2'b00
    15:10   R/W MBIAS_LDOAUX2_TUNE_LDO733LQ                 6'b001110
 */
typedef union _AON_FAST_REG7X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_REG7X_DUMMY0: 1;
        uint16_t MBIAS_REG7X_DUMMY1: 1;
        uint16_t MBIAS_LDOAUX2_EN_NOSS_LDO533HQ: 1;
        uint16_t MBIAS_LDOAUX2_EN_DL_LDO533HQ: 1;
        uint16_t MBIAS_LDOAUX2_ENB_DL_LDO733LQ_B: 1;
        uint16_t MBIAS_LDOAUX2_SEL_VR_LDO533HQ: 3;
        uint16_t MBIAS_LDOAUX2_TUNE_LDO533HQ_DL: 2;
        uint16_t MBIAS_LDOAUX2_TUNE_LDO733LQ: 6;
    };
} AON_FAST_REG7X_MBIAS_TYPE;

/* 0xC10
    1:0     R/W MBIAS_SEL_DPD_VBAT_DR_L                     2'b00
    4:2     R/W MBIAS_SEL_DPD_VBAT_DET_L                    3'b100
    7:5     R/W MBIAS_SEL_DPD_VBAT_DET_H                    3'b100
    9:8     R/W MBIAS_SEL_DPD_ADPIN_DR_L                    2'b00
    12:10   R/W MBIAS_SEL_DPD_ADPIN_DET_L                   3'b010
    15:13   R/W MBIAS_SEL_DPD_ADPIN_DET_H                   3'b010
 */
typedef union _AON_FAST_REG8X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_DPD_VBAT_DR_L: 2;
        uint16_t MBIAS_SEL_DPD_VBAT_DET_L: 3;
        uint16_t MBIAS_SEL_DPD_VBAT_DET_H: 3;
        uint16_t MBIAS_SEL_DPD_ADPIN_DR_L: 2;
        uint16_t MBIAS_SEL_DPD_ADPIN_DET_L: 3;
        uint16_t MBIAS_SEL_DPD_ADPIN_DET_H: 3;
    };
} AON_FAST_REG8X_MBIAS_TYPE;

/* 0xC12
    0       R/W MBIAS_DPD_R[0]                              1'b1
    1       R/W MBIAS_DPD_R[1]                              1'b0
    2       R/W MBIAS_DPD_R[2]                              1'b1
    3       R/W MBIAS_DPD_R[3]                              1'b1
    4       R/W MBIAS_DPD_R[4]                              1'b1
    5       R/W MBIAS_DPD_R[5]                              1'b1
    6       R/W MBIAS_DPD_R[6]                              1'b1
    7       R/W MBIAS_DPD_R[7]                              1'b1
    8       R/W MBIAS_DPD_R[8]                              1'b1
    11:9    R/W MBIAS_TUNE_LDO733DPD_TUNE                   3'b110
    12      R/W MBIAS_EN_DPD_COMP_HYS                       1'b0
    14:13   R/W MBIAS_SEL_DPD_MFB                           2'b11
    15      R/W MBIAS_DPD_RCK                               1'b0
 */
typedef union _AON_FAST_REG9X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_DPD_R_0: 1;
        uint16_t MBIAS_DPD_R_1: 1;
        uint16_t MBIAS_DPD_R_2: 1;
        uint16_t MBIAS_DPD_R_3: 1;
        uint16_t MBIAS_DPD_R_4: 1;
        uint16_t MBIAS_DPD_R_5: 1;
        uint16_t MBIAS_DPD_R_6: 1;
        uint16_t MBIAS_DPD_R_7: 1;
        uint16_t MBIAS_DPD_R_8: 1;
        uint16_t MBIAS_TUNE_LDO733DPD_TUNE: 3;
        uint16_t MBIAS_EN_DPD_COMP_HYS: 1;
        uint16_t MBIAS_SEL_DPD_MFB: 2;
        uint16_t MBIAS_DPD_RCK: 1;
    };
} AON_FAST_REG9X_MBIAS_TYPE;

/* 0xC14
    0       R/W MBIAS_REG10X_DUMMY0                         1'b0
    1       R/W MBIAS_REG10X_DUMMY1                         1'b0
    2       R/W MBIAS_REG10X_DUMMY2                         1'b0
    3       R/W MBIAS_REG10X_DUMMY3                         1'b0
    4       R/W MBIAS_REG10X_DUMMY4                         1'b0
    5       R/W MBIAS_REG10X_DUMMY5                         1'b0
    6       R/W MBIAS_REG10X_DUMMY6                         1'b0
    7       R/W MBIAS_REG10X_DUMMY7                         1'b0
    8       R/W MBIAS_REG10X_DUMMY8                         1'b0
    9       R/W MBIAS_REG10X_DUMMY9                         1'b0
    10      R/W MBIAS_REG10X_DUMMY10                        1'b0
    11      R/W MBIAS_REG10X_DUMMY11                        1'b0
    12      R/W MBIAS_REG10X_DUMMY12                        1'b0
    13      R/W MBIAS_REG10X_DUMMY13                        1'b0
    14      R/W MBIAS_REG10X_DUMMY14                        1'b0
    15      R/W MBIAS_EN_CLK_1K_AON                         1'b1
 */
typedef union _AON_FAST_REG10X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_REG10X_DUMMY0: 1;
        uint16_t MBIAS_REG10X_DUMMY1: 1;
        uint16_t MBIAS_REG10X_DUMMY2: 1;
        uint16_t MBIAS_REG10X_DUMMY3: 1;
        uint16_t MBIAS_REG10X_DUMMY4: 1;
        uint16_t MBIAS_REG10X_DUMMY5: 1;
        uint16_t MBIAS_REG10X_DUMMY6: 1;
        uint16_t MBIAS_REG10X_DUMMY7: 1;
        uint16_t MBIAS_REG10X_DUMMY8: 1;
        uint16_t MBIAS_REG10X_DUMMY9: 1;
        uint16_t MBIAS_REG10X_DUMMY10: 1;
        uint16_t MBIAS_REG10X_DUMMY11: 1;
        uint16_t MBIAS_REG10X_DUMMY12: 1;
        uint16_t MBIAS_REG10X_DUMMY13: 1;
        uint16_t MBIAS_REG10X_DUMMY14: 1;
        uint16_t MBIAS_EN_CLK_1K_AON: 1;
    };
} AON_FAST_REG10X_MBIAS_TYPE;

/* 0xC16
    0       R   MBIAS_FLAG_ADPIN_DET_L                      1'b0
    1       R   MBIAS_FLAG_VBAT_DET_L                       1'b0
    2       R   MBIAS_FLAG_HW_RST_N_L                       1'b0
    3       R   MBIAS_FLAG_HV_DET_L                         1'b0
    4       R   MBIAS_FLAG_VAUX_DET_L                       1'b0
    5       R   MBIAS_FLAG_FPOR_L                           1'b0
    6       R   MBIAS_FLAG_LDO733_LQ_2D5_DET_L              1'b0
    7       R   MBIAS_FLAG_LDO733_LQ_DET_L                  1'b0
    8       R   MBIAS_FLAG_DVDD_DET_L                       1'b0
    9       R   MBIAS_FLAG_BGOK_L                           1'b0
    10      R   MBIAS_FLAG_VAUDIO_DET_L                     1'b0
    11      R   MBIAS_FLAG_VDDCORE_DET_L                    1'b0
    12      R   MBIAS_FLAG_LDO733DPD_DET_L                  1'b0
    13      R   MBIAS_FLAG_MFB_DET_L                        1'b0
    14      R   MBIAS_FLAG_VBAT_DPD_DET_L                   1'b0
    15      R   MBIAS_FLAG_ADPIN_DPD_DET_L                  1'b0
 */
typedef union _AON_FAST_FLAG0X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_ADPIN_DET_L: 1;
        uint16_t MBIAS_FLAG_VBAT_DET_L: 1;
        uint16_t MBIAS_FLAG_HW_RST_N_L: 1;
        uint16_t MBIAS_FLAG_HV_DET_L: 1;
        uint16_t MBIAS_FLAG_VAUX_DET_L: 1;
        uint16_t MBIAS_FLAG_FPOR_L: 1;
        uint16_t MBIAS_FLAG_LDO733_LQ_2D5_DET_L: 1;
        uint16_t MBIAS_FLAG_LDO733_LQ_DET_L: 1;
        uint16_t MBIAS_FLAG_DVDD_DET_L: 1;
        uint16_t MBIAS_FLAG_BGOK_L: 1;
        uint16_t MBIAS_FLAG_VAUDIO_DET_L: 1;
        uint16_t MBIAS_FLAG_VDDCORE_DET_L: 1;
        uint16_t MBIAS_FLAG_LDO733DPD_DET_L: 1;
        uint16_t MBIAS_FLAG_MFB_DET_L: 1;
        uint16_t MBIAS_FLAG_VBAT_DPD_DET_L: 1;
        uint16_t MBIAS_FLAG_ADPIN_DPD_DET_L: 1;
    };
} AON_FAST_FLAG0X_MBIAS_TYPE;

/* 0xC18
    2:0     R   MBIAS_FLAG_LDO733DPD_TUNE                   3'b000
    5:3     R   MBIAS_FLAG_DPD_ADIPN_DET_L                  3'b000
    8:6     R   MBIAS_FLAG_DPD_ADPIN_DET_H                  3'b000
    9       R   MBIAS_FLAG_DPD_COMP_HYS                     1'b0
    14:10   R   MBIAS_FLAG_DPD_EN_4_0                       5'b00000
    15      R   MBIAS_FLAG_HV33_SWR_DET_L                   1'b0
 */
typedef union _AON_FAST_FLAG1X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_LDO733DPD_TUNE: 3;
        uint16_t MBIAS_FLAG_DPD_ADIPN_DET_L: 3;
        uint16_t MBIAS_FLAG_DPD_ADPIN_DET_H: 3;
        uint16_t MBIAS_FLAG_DPD_COMP_HYS: 1;
        uint16_t MBIAS_FLAG_DPD_EN_4_0: 5;
        uint16_t MBIAS_FLAG_HV33_SWR_DET_L: 1;
    };
} AON_FAST_FLAG1X_MBIAS_TYPE;

/* 0xC1A
    4:0     R   MBIAS_FLAG_DPD_R_4_0                        5'b00000
    8:5     R   MBIAS_FLAG_DPD_R_8_5                        4'b0000
    10:9    R   MBIAS_FLAG_DPD_ADPIN_DR_L                   2'b00
    12:11   R   MBIAS_FLAG_DPD_VBAT_DR_L                    2'b00
    14:13   R   MBIAS_FLAG_DPD_TUNE_MFB                     2'b00
    15      R   MBIAS_FLAG_DPD_EN_CLK_1K                    1'b0
 */
typedef union _AON_FAST_FLAG2X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_DPD_R_4_0: 5;
        uint16_t MBIAS_FLAG_DPD_R_8_5: 4;
        uint16_t MBIAS_FLAG_DPD_ADPIN_DR_L: 2;
        uint16_t MBIAS_FLAG_DPD_VBAT_DR_L: 2;
        uint16_t MBIAS_FLAG_DPD_TUNE_MFB: 2;
        uint16_t MBIAS_FLAG_DPD_EN_CLK_1K: 1;
    };
} AON_FAST_FLAG2X_MBIAS_TYPE;

/* 0xC80
    0       R/W LDOSYS_POW_LDOVREF                          1'b0
    1       R/W LDOSYS_POW_HQLQ533_PC                       1'b0
    2       R/W LDOSYS_POW_LDO533HQ                         1'b0
    3       R/W LDOSYS_EN_NOSS                              1'b0
    4       R/W LDOSYS_EN_LDOSYS_HQ_OFF_IB50nA              1'b0
    7:5     R/W LDOSYS_VR_SEL_LDO533HQ                      3'b000
    9:8     R/W LDOSYS_TUNE_DL_LDO533HQ                     2'b10
    15:10   R/W LDOSYS_TUNE_LDO533HQ                        6'b000000
 */
typedef union _AON_FAST_REG0X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POW_LDOVREF: 1;
        uint16_t LDOSYS_POW_HQLQ533_PC: 1;
        uint16_t LDOSYS_POW_LDO533HQ: 1;
        uint16_t LDOSYS_EN_NOSS: 1;
        uint16_t LDOSYS_EN_LDOSYS_HQ_OFF_IB50nA: 1;
        uint16_t LDOSYS_VR_SEL_LDO533HQ: 3;
        uint16_t LDOSYS_TUNE_DL_LDO533HQ: 2;
        uint16_t LDOSYS_TUNE_LDO533HQ: 6;
    };
} AON_FAST_REG0X_LDOSYS_TYPE;

/* 0xC82
    0       R/W LDOSYS_ENB_DL_LDO733LQ                      1'b0
    6:1     R/W LDOSYS_TUNE_LDO733LQ                        6'b110011
    7       R/W LDOSYS_EN_LDO311_IB20nA                     1'b0
    9:8     R/W LDOSYS_ENB_DL_LDO311                        2'b00
    14:10   R/W LDOSYS_TUNE_LDO311                          5'b01111
    15      R/W LDOSYS_EN_LDOSYS_LQVCORE_OFF_IB_50A         1'b0
 */
typedef union _AON_FAST_REG1X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_ENB_DL_LDO733LQ: 1;
        uint16_t LDOSYS_TUNE_LDO733LQ: 6;
        uint16_t LDOSYS_EN_LDO311_IB20nA: 1;
        uint16_t LDOSYS_ENB_DL_LDO311: 2;
        uint16_t LDOSYS_TUNE_LDO311: 5;
        uint16_t LDOSYS_EN_LDOSYS_LQVCORE_OFF_IB_50A: 1;
    };
} AON_FAST_REG1X_LDOSYS_TYPE;

/* 0xC84
    5:0     R/W LDOSYS_VCORELDO733LQ_TUNE                   6'b110011
    6       R/W LDOSYS_EN_EFUSE_PCUT                        1'b0
    7       R/W LDOSYS_POW_HQLQVCORE533_PC                  1'b0
    8       R/W LDOSYS_POW_EHVT_PCUT_VCORE                  1'b0
    9       R/W LDOSYS_POW_EHVT_PCUT_DVDD                   1'b1
    10      R/W LDOSYS_POW_LDO733LQ_VCORE                   1'b0
    11      R/W LDOSYS_EN_DL_311                            1'b1
    12      R/W LDOSYS_REG2X_DUMMY12                        1'b1
    13      R/W LDOSYS__EN_LDOSYS_LQ_OFF_IB_50A             1'b0
    14      R/W LDOSYS_ENB_DL_VCORELDOLQ                    1'b1
    15      R/W LDOSYS_EN_DL_LDO533HQ                       1'b1
 */
typedef union _AON_FAST_REG2X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_VCORELDO733LQ_TUNE: 6;
        uint16_t LDOSYS_EN_EFUSE_PCUT: 1;
        uint16_t LDOSYS_POW_HQLQVCORE533_PC: 1;
        uint16_t LDOSYS_POW_EHVT_PCUT_VCORE: 1;
        uint16_t LDOSYS_POW_EHVT_PCUT_DVDD: 1;
        uint16_t LDOSYS_POW_LDO733LQ_VCORE: 1;
        uint16_t LDOSYS_EN_DL_311: 1;
        uint16_t LDOSYS_REG2X_DUMMY12: 1;
        uint16_t LDOSYS__EN_LDOSYS_LQ_OFF_IB_50A: 1;
        uint16_t LDOSYS_ENB_DL_VCORELDOLQ: 1;
        uint16_t LDOSYS_EN_DL_LDO533HQ: 1;
    };
} AON_FAST_REG2X_LDOSYS_TYPE;

/* 0xC86
    7:0     R/W LDOSYS_LDOPA_TUNE_LDO_VOUT                  8'b10111101
    8       R/W LDOSYS_REG3X_DUMMY10                        1'b0
    9       R/W LDOSYS_REG3X_DUMMY9                         1'b0
    10      R/W LDOSYS_LDOPA_EN_LO_IQ2                      1'b0
    12:11   R/W LDOSYS_LDOPA_SEL_BIAS                       2'b00
    13      R/W LDOSYS_LDOPA_POW_BIAS                       1'b0
    14      R/W LDOSYS_LDOPA_EN_HI_IQ                       1'b0
    15      R/W LDOSYS_LDOPA_POW_LDO                        1'b0
 */
typedef union _AON_FAST_REG3X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_LDOPA_TUNE_LDO_VOUT: 8;
        uint16_t LDOSYS_REG3X_DUMMY10: 1;
        uint16_t LDOSYS_REG3X_DUMMY9: 1;
        uint16_t LDOSYS_LDOPA_EN_LO_IQ2: 1;
        uint16_t LDOSYS_LDOPA_SEL_BIAS: 2;
        uint16_t LDOSYS_LDOPA_POW_BIAS: 1;
        uint16_t LDOSYS_LDOPA_EN_HI_IQ: 1;
        uint16_t LDOSYS_LDOPA_POW_LDO: 1;
    };
} AON_FAST_REG3X_LDOSYS_TYPE;

/* 0xC88
    2:0     R/W LDOSYS_LDOPA_SEL_DL                         3'b000
    3       R/W LDOSYS_REG1X_DUMMY3                         1'b1
    4       R/W LDOSYS_REG4X_DUMMY4                         1'b0
    5       R/W LDOSYS_REG4X_DUMMY5                         1'b0
    6       R/W LDOSYS_REG4X_DUMMY6                         1'b0
    7       R/W LDOSYS_REG4X_DUMMY7                         1'b0
    8       R/W LDOSYS_REG4X_DUMMY8                         1'b0
    9       R/W LDOSYS_REG4X_DUMMY9                         1'b1
    10      R/W LDOSYS_REG4X_DUMMY10                        1'b0
    11      R/W LDOSYS_REG4X_DUMMY11                        1'b0
    12      R/W LDOSYS_REG4X_DUMMY12                        1'b0
    13      R/W LDOSYS_REG4X_DUMMY13                        1'b0
    14      R/W LDOSYS_REG4X_DUMMY14                        1'b0
    15      R/W LDOSYS_REG4X_DUMMY15                        1'b1
 */
typedef union _AON_FAST_REG4X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_LDOPA_SEL_DL: 3;
        uint16_t LDOSYS_REG1X_DUMMY3: 1;
        uint16_t LDOSYS_REG4X_DUMMY4: 1;
        uint16_t LDOSYS_REG4X_DUMMY5: 1;
        uint16_t LDOSYS_REG4X_DUMMY6: 1;
        uint16_t LDOSYS_REG4X_DUMMY7: 1;
        uint16_t LDOSYS_REG4X_DUMMY8: 1;
        uint16_t LDOSYS_REG4X_DUMMY9: 1;
        uint16_t LDOSYS_REG4X_DUMMY10: 1;
        uint16_t LDOSYS_REG4X_DUMMY11: 1;
        uint16_t LDOSYS_REG4X_DUMMY12: 1;
        uint16_t LDOSYS_REG4X_DUMMY13: 1;
        uint16_t LDOSYS_REG4X_DUMMY14: 1;
        uint16_t LDOSYS_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_LDOSYS_TYPE;

/* 0xC8A
    0       R   LDOEXTRF_FLAG0X_DUMMY0                      1'b0
    1       R   LDOEXTRF_FLAG0X_DUMMY1                      1'b0
    2       R   LDOEXTRF_FLAG0X_DUMMY2                      1'b0
    3       R   LDOEXTRF_FLAG0X_DUMMY3                      1'b0
    4       R   LDOEXTRF_FLAG0X_DUMMY4                      1'b0
    5       R   LDOEXTRF_FLAG0X_DUMMY5                      1'b0
    6       R   LDOEXTRF_FLAG0X_DUMMY6                      1'b0
    7       R   LDOEXTRF_FLAG0X_DUMMY7                      1'b0
    8       R   LDOEXTRF_FLAG0X_DUMMY8                      1'b0
    9       R   LDOEXTRF_FLAG0X_DUMMY9                      1'b0
    10      R   LDOEXTRF_FLAG0X_DUMMY10                     1'b0
    11      R   LDOEXTRF_FLAG0X_DUMMY11                     1'b0
    12      R   LDOEXTRF_FLAG0X_DUMMY12                     1'b0
    13      R   LDOEXTRF_FLAG0X_DUMMY13                     1'b0
    14      R   LDOEXTRF_FLAG0X_DUMMY14                     1'b0
    15      R   LDOEXTRF_FLAG0X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_FLAG0X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXTRF_FLAG0X_DUMMY0: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY1: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY2: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY3: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY4: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY5: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY6: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY7: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY8: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY9: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY10: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY11: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY12: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY13: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY14: 1;
        uint16_t LDOEXTRF_FLAG0X_DUMMY15: 1;
    };
} AON_FAST_FLAG0X_LDOSYS_TYPE;

/* 0xD50
    0       R/W LDOAUX1_POW_LDO533HQ                        1'b0
    1       R/W LDOAUX1_POW_VREF                            1'b0
    4:2     R/W LDOAUX1_VR_SEL_LDO533HQ                     3'b000
    5       R/W LDOAUX1_EN_LDO533HQ_DL                      1'b0
    6       R/W LDOAUX1_ENB_LDO533HQ_SS                     1'b0
    12:7    R/W LDOAUX1_TUNE_LDO533HQ                       6'b001110
    13      R/W LDOAUX1_AUDIOLQ_EN_AUDIOLQ_IB20A            1'b0
    14      R/W LDOAUX1_AUDIOLQ_ENB_LDOAUDIO_LQ_DL1         1'b0
    15      R/W LDOAUX1_EN_LDO533HQ_IOFF_50nA               1'b0
 */
typedef union _AON_FAST_REG0X_LDOAUX1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_POW_LDO533HQ: 1;
        uint16_t LDOAUX1_POW_VREF: 1;
        uint16_t LDOAUX1_VR_SEL_LDO533HQ: 3;
        uint16_t LDOAUX1_EN_LDO533HQ_DL: 1;
        uint16_t LDOAUX1_ENB_LDO533HQ_SS: 1;
        uint16_t LDOAUX1_TUNE_LDO533HQ: 6;
        uint16_t LDOAUX1_AUDIOLQ_EN_AUDIOLQ_IB20A: 1;
        uint16_t LDOAUX1_AUDIOLQ_ENB_LDOAUDIO_LQ_DL1: 1;
        uint16_t LDOAUX1_EN_LDO533HQ_IOFF_50nA: 1;
    };
} AON_FAST_REG0X_LDOAUX1_TYPE;

/* 0xD52
    0       R/W LDOAUX1_POW_LDO733LQ                        1'b0
    1       R/W LDOAUX1_ENB_LDO733LQ_DL                     1'b0
    7:2     R/W LDOAUX1_TUNE_LDO733_LQ                      6'b001110
    8       R/W LDOAUX1_AUDIOLQ_POW_LDOAUDIO_LQ             1'b0
    9       R/W LDOAUX1_AUDIOLQ_ENB_LDOAUDIO_LQ_DL0         1'b0
    10      R/W LDOAUX1_EN_LDO733_LQ_IOFF_50nA              1'b0
    15:11   R/W LDOAUX1_AUDIOLQ_TUNE_LDOAUDIO_LQ            5'b00101
 */
typedef union _AON_FAST_REG1X_LDOAUX1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_POW_LDO733LQ: 1;
        uint16_t LDOAUX1_ENB_LDO733LQ_DL: 1;
        uint16_t LDOAUX1_TUNE_LDO733_LQ: 6;
        uint16_t LDOAUX1_AUDIOLQ_POW_LDOAUDIO_LQ: 1;
        uint16_t LDOAUX1_AUDIOLQ_ENB_LDOAUDIO_LQ_DL0: 1;
        uint16_t LDOAUX1_EN_LDO733_LQ_IOFF_50nA: 1;
        uint16_t LDOAUX1_AUDIOLQ_TUNE_LDOAUDIO_LQ: 5;
    };
} AON_FAST_REG1X_LDOAUX1_TYPE;

/* 0xD54
    0       R/W LDOAUX1_VOUT_AVCC_DRV                       1'b0
    1       R/W LDOAUX1_VOUT_AVCC                           1'b0
    3:2     R/W LDOAUX1_LDO533HQ_DL                         2'b00
    4       R/W LDOAUX1_REG2X_DUMMY4                        1'b0
    5       R/W LDOAUX1_REG2X_DUMMY5                        1'b0
    6       R/W LDOAUX1_REG2X_DUMMY6                        1'b0
    7       R/W LDOAUX1_PCUT_AUX_VG2_33                     1'b1
    8       R/W LDOAUX1_PCUT_AUX_VG1_33                     1'b1
    9       R/W LDOAUX1_PCUT_AUX_ESDCTRL                    1'b1
    10      R/W LDOAUX1_REG2X_DUMMY10                       1'b0
    11      R/W LDOAUX1_REG2X_DUMMY11                       1'b0
    12      R/W LDOAUX1_REG2X_DUMMY12                       1'b0
    13      R/W LDOAUX1_REG2X_DUMMY13                       1'b0
    14      R/W LDOAUX1_REG2X_DUMMY14                       1'b0
    15      R/W LDOAUX1_REG2X_DUMMY15                       1'b0
 */
typedef union _AON_FAST_REG2X_LDOAUX1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_VOUT_AVCC_DRV: 1;
        uint16_t LDOAUX1_VOUT_AVCC: 1;
        uint16_t LDOAUX1_LDO533HQ_DL: 2;
        uint16_t LDOAUX1_REG2X_DUMMY4: 1;
        uint16_t LDOAUX1_REG2X_DUMMY5: 1;
        uint16_t LDOAUX1_REG2X_DUMMY6: 1;
        uint16_t LDOAUX1_PCUT_AUX_VG2_33: 1;
        uint16_t LDOAUX1_PCUT_AUX_VG1_33: 1;
        uint16_t LDOAUX1_PCUT_AUX_ESDCTRL: 1;
        uint16_t LDOAUX1_REG2X_DUMMY10: 1;
        uint16_t LDOAUX1_REG2X_DUMMY11: 1;
        uint16_t LDOAUX1_REG2X_DUMMY12: 1;
        uint16_t LDOAUX1_REG2X_DUMMY13: 1;
        uint16_t LDOAUX1_REG2X_DUMMY14: 1;
        uint16_t LDOAUX1_REG2X_DUMMY15: 1;
    };
} AON_FAST_REG2X_LDOAUX1_TYPE;

/* 0xE60
    0       R/W LDO_DIG_POW_LDODIG                          1'b0
    1       R/W LDO_DIG_EN_LDODIG_PC                        1'b0
    2       R/W LDO_DIG_EN_LDODIG_FB_LOCAL_B                1'b0
    7:3     R/W LDO_DIG_TUNE_LDODIG_VOUT                    5'b10110
    8       R/W LDO_DIG_POW_LDODIG_VREF                     1'b0
    9       R/W LDO_DIG_POW_LDORET                          1'b0
    10      R/W LDO_DIG_EN_LDORET_DL_B                      1'b0
    15:11   R/W LDO_DIG_TUNE_LDORET_VOUT                    5'b11000
 */
typedef union _AON_FAST_REG0X_LDO_DIG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_DIG_POW_LDODIG: 1;
        uint16_t LDO_DIG_EN_LDODIG_PC: 1;
        uint16_t LDO_DIG_EN_LDODIG_FB_LOCAL_B: 1;
        uint16_t LDO_DIG_TUNE_LDODIG_VOUT: 5;
        uint16_t LDO_DIG_POW_LDODIG_VREF: 1;
        uint16_t LDO_DIG_POW_LDORET: 1;
        uint16_t LDO_DIG_EN_LDORET_DL_B: 1;
        uint16_t LDO_DIG_TUNE_LDORET_VOUT: 5;
    };
} AON_FAST_REG0X_LDO_DIG_TYPE;

/* 0xE62
    0       R/W LDO_DIG_POW_PCUT_VPON_DVDD_LQ               1'b0
    3:1     R/W LDO_DIG_SEL_VR_LDODIG                       3'b000
    4       R/W LDO_DIG_REG1X_DUMMY4                        1'b0
    5       R/W LDO_DIG_REG1X_DUMMY5                        1'b0
    6       R/W LDO_DIG_REG1X_DUMMY6                        1'b0
    7       R/W LDO_DIG_REG1X_DUMMY7                        1'b0
    8       R/W LDO_DIG_REG1X_DUMMY8                        1'b0
    9       R/W LDO_DIG_REG1X_DUMMY9                        1'b0
    10      R/W LDO_DIG_REG1X_DUMMY10                       1'b0
    11      R/W LDO_DIG_REG1X_DUMMY11                       1'b0
    12      R/W LDO_DIG_REG1X_DUMMY12                       1'b0
    13      R/W LDO_DIG_REG1X_DUMMY13                       1'b0
    14      R/W LDO_DIG_REG1X_DUMMY14                       1'b0
    15      R/W LDO_DIG_REG1X_DUMMY15                       1'b0
 */
typedef union _AON_FAST_REG1X_LDO_DIG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_DIG_POW_PCUT_VPON_DVDD_LQ: 1;
        uint16_t LDO_DIG_SEL_VR_LDODIG: 3;
        uint16_t LDO_DIG_REG1X_DUMMY4: 1;
        uint16_t LDO_DIG_REG1X_DUMMY5: 1;
        uint16_t LDO_DIG_REG1X_DUMMY6: 1;
        uint16_t LDO_DIG_REG1X_DUMMY7: 1;
        uint16_t LDO_DIG_REG1X_DUMMY8: 1;
        uint16_t LDO_DIG_REG1X_DUMMY9: 1;
        uint16_t LDO_DIG_REG1X_DUMMY10: 1;
        uint16_t LDO_DIG_REG1X_DUMMY11: 1;
        uint16_t LDO_DIG_REG1X_DUMMY12: 1;
        uint16_t LDO_DIG_REG1X_DUMMY13: 1;
        uint16_t LDO_DIG_REG1X_DUMMY14: 1;
        uint16_t LDO_DIG_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_LDO_DIG_TYPE;

/* 0xEB0
    1:0     R/W VCORE_PC_REG0X_DUMMY1                       2'b00
    2       R/W VCORE_PC_REG0X_DUMMY2                       1'b1
    3       R/W VCORE_PC_REG0X_DUMMY3                       1'b1
    4       R/W VCORE_PC_REG0X_DUMMY4                       1'b1
    5       R/W VCORE_PC_REG0X_DUMMY5                       1'b1
    6       R/W VCORE_PC_REG0X_DUMMY6                       1'b1
    7       R/W VCORE_PC_REG0X_DUMMY7                       1'b1
    8       R/W VCORE_PC_REG0X_DUMMY8                       1'b1
    9       R/W VCORE_PC_REG0X_DUMMY9                       1'b1
    10      R/W VCORE_PC_REG0X_DUMMY10                      1'b1
    11      R/W VCORE_PC_REG0X_DUMMY11                      1'b1
    12      R/W VCORE_PC_POW_VCORE_PC_VG2                   1'b1
    13      R/W VCORE_PC_POW_VCORE_PC_VG1                   1'b1
    14      R/W VCORE_PC_REG0X_DUMMY14                      1'b1
    15      R/W VCORE_PC_REG0X_DUMMY15                      1'b1
 */
typedef union _AON_FAST_REG0X_VCORE_PC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t VCORE_PC_REG0X_DUMMY1: 2;
        uint16_t VCORE_PC_REG0X_DUMMY2: 1;
        uint16_t VCORE_PC_REG0X_DUMMY3: 1;
        uint16_t VCORE_PC_REG0X_DUMMY4: 1;
        uint16_t VCORE_PC_REG0X_DUMMY5: 1;
        uint16_t VCORE_PC_REG0X_DUMMY6: 1;
        uint16_t VCORE_PC_REG0X_DUMMY7: 1;
        uint16_t VCORE_PC_REG0X_DUMMY8: 1;
        uint16_t VCORE_PC_REG0X_DUMMY9: 1;
        uint16_t VCORE_PC_REG0X_DUMMY10: 1;
        uint16_t VCORE_PC_REG0X_DUMMY11: 1;
        uint16_t VCORE_PC_POW_VCORE_PC_VG2: 1;
        uint16_t VCORE_PC_POW_VCORE_PC_VG1: 1;
        uint16_t VCORE_PC_REG0X_DUMMY14: 1;
        uint16_t VCORE_PC_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_VCORE_PC_TYPE;

/* 0x1000
    0       R/W CHG_POW_M2                                  1'b0                    CHG_REG_MASK
    1       R/W CHG_POW_M1                                  1'b0                    CHG_REG_MASK
    2       R/W CHG_SEL_M2CCD                               1'b1                    CHG_REG_MASK
    3       R/W CHG_ENB_IB6X                                1'b1                    CHG_REG_MASK
    4       R/W CHG_EN_OP_OFS_KMOD                          1'b0                    CHG_REG_MASK
    9:5     R/W CHG_TUNE_M2_CS_OFSK                         5'b10000                CHG_REG_MASK
    10      R/W CHG_POW_M2_DVDET                            1'b0                    CHG_REG_MASK
    11      R/W CHG_POW_M1_DVDET                            1'b0                    CHG_REG_MASK
    12      R/W CHG_EN_CS_KMOD                              1'b0                    CHG_REG_MASK
    13      R/W CHG_EN_M2_DISBIAS                           1'b1                    CHG_REG_MASK
    14      R/W CHG_EN_M1_DISBIAS                           1'b1                    CHG_REG_MASK
    15      R/W CHG_EN_BS_M2_OFS                            1'b0                    CHG_REG_MASK
 */
typedef union _AON_FAST_REG0X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_POW_M2: 1;
        uint16_t CHG_POW_M1: 1;
        uint16_t CHG_SEL_M2CCD: 1;
        uint16_t CHG_ENB_IB6X: 1;
        uint16_t CHG_EN_OP_OFS_KMOD: 1;
        uint16_t CHG_TUNE_M2_CS_OFSK: 5;
        uint16_t CHG_POW_M2_DVDET: 1;
        uint16_t CHG_POW_M1_DVDET: 1;
        uint16_t CHG_EN_CS_KMOD: 1;
        uint16_t CHG_EN_M2_DISBIAS: 1;
        uint16_t CHG_EN_M1_DISBIAS: 1;
        uint16_t CHG_EN_BS_M2_OFS: 1;
    };
} AON_FAST_REG0X_CHG_TYPE;

/* 0x1002
    6:0     R/W CHG_TUNE_M2_CVK                             7'b1000000              CHG_REG_MASK
    11:7    R/W CHG_SEL_M2CV                                5'b10100                CHG_REG_MASK
    15:12   R/W CHG_SEL_M1CV                                4'b1000                 CHG_REG_MASK
 */
typedef union _AON_FAST_REG1X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_TUNE_M2_CVK: 7;
        uint16_t CHG_SEL_M2CV: 5;
        uint16_t CHG_SEL_M1CV: 4;
    };
} AON_FAST_REG1X_CHG_TYPE;

/* 0x1004
    0       R/W CHG_EN_BATSHORTDET                          1'b0                    CHG_REG_MASK
    1       R/W CHG_EN_M2FON1K                              1'b0                    CHG_REG_MASK
    3:2     R/W CHG_SEL_M2CCDFB                             2'b01                   CHG_REG_MASK
    7:4     R/W CHG_SEL_M2CC                                4'b0000                 CHG_REG_MASK
    9:8     R/W CHG_SEL_M2CCFB                              2'b00                   CHG_REG_MASK
    13:10   R/W CHG_SEL_M1CC                                4'b0010                 CHG_REG_MASK
    15:14   R/W CHG_SEL_M1CCFB                              2'b11                   CHG_REG_MASK
 */
typedef union _AON_FAST_REG2X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_EN_BATSHORTDET: 1;
        uint16_t CHG_EN_M2FON1K: 1;
        uint16_t CHG_SEL_M2CCDFB: 2;
        uint16_t CHG_SEL_M2CC: 4;
        uint16_t CHG_SEL_M2CCFB: 2;
        uint16_t CHG_SEL_M1CC: 4;
        uint16_t CHG_SEL_M1CCFB: 2;
    };
} AON_FAST_REG2X_CHG_TYPE;

/* 0x1006
    4:0     R/W CHG_TUNE_M2_CC_OFSK                         5'b10000                CHG_REG_MASK
    9:5     R/W CHG_TUNE_M2_CV_OFSK                         5'b10000                CHG_REG_MASK
    15:10   R/W CHG_TUNE_M2_CCK                             6'b100000               CHG_REG_MASK
 */
typedef union _AON_FAST_REG3X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_TUNE_M2_CC_OFSK: 5;
        uint16_t CHG_TUNE_M2_CV_OFSK: 5;
        uint16_t CHG_TUNE_M2_CCK: 6;
    };
} AON_FAST_REG3X_CHG_TYPE;

/* 0x1008
    0       R/W CHG_REG4X_DUMMY0                            1'b0                    CHG_REG_MASK
    1       R/W CHG_REG4X_DUMMY1                            1'b0                    CHG_REG_MASK
    2       R/W CHG_REG4X_DUMMY2                            1'b0                    CHG_REG_MASK
    3       R/W CHG_REG4X_DUMMY3                            1'b0                    CHG_REG_MASK
    4       R/W CHG_REG4X_DUMMY4                            1'b0                    CHG_REG_MASK
    5       R/W CHG_REG4X_DUMMY5                            1'b0                    CHG_REG_MASK
    6       R/W CHG_REG4X_DUMMY6                            1'b0                    CHG_REG_MASK
    7       R/W CHG_REG4X_DUMMY7                            1'b0                    CHG_REG_MASK
    8       R/W CHG_REG4X_DUMMY8                            1'b0                    CHG_REG_MASK
    9       R/W CHG_REG4X_DUMMY9                            1'b0                    CHG_REG_MASK
    10      R/W CHG_EN_M1FON_cut_leak                       1'b0                    CHG_REG_MASK
    11      R/W CHG_EN_M2FON_cut_leak                       1'b0                    CHG_REG_MASK
    12      R/W CHG_EN_IBSN_MCCD                            1'b0                    CHG_REG_MASK
    13      R/W CHG_EN_M2FONBUF                             1'b0                    CHG_REG_MASK
    14      R/W CHG_EN_M1FON_LDO733                         1'b0                    CHG_REG_MASK
    15      R/W CHG_EN_M2_VCM2                              1'b0                    CHG_REG_MASK
 */
typedef union _AON_FAST_REG4X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_REG4X_DUMMY0: 1;
        uint16_t CHG_REG4X_DUMMY1: 1;
        uint16_t CHG_REG4X_DUMMY2: 1;
        uint16_t CHG_REG4X_DUMMY3: 1;
        uint16_t CHG_REG4X_DUMMY4: 1;
        uint16_t CHG_REG4X_DUMMY5: 1;
        uint16_t CHG_REG4X_DUMMY6: 1;
        uint16_t CHG_REG4X_DUMMY7: 1;
        uint16_t CHG_REG4X_DUMMY8: 1;
        uint16_t CHG_REG4X_DUMMY9: 1;
        uint16_t CHG_EN_M1FON_cut_leak: 1;
        uint16_t CHG_EN_M2FON_cut_leak: 1;
        uint16_t CHG_EN_IBSN_MCCD: 1;
        uint16_t CHG_EN_M2FONBUF: 1;
        uint16_t CHG_EN_M1FON_LDO733: 1;
        uint16_t CHG_EN_M2_VCM2: 1;
    };
} AON_FAST_REG4X_CHG_TYPE;

/* 0x100A
    0       R   CHG_FLAG0X_CHG_DUMM0                        1'b0
    1       R   CHG_FLAG0X_CHG_DUMMY1                       1'b0
    2       R   CHG_FLAG0X_CHG_DUMMY2                       1'b0
    3       R   CHG_M2CSOPK_FLG                             1'b0
    4       R   CHG_M2CCOPK_FLG                             1'b0
    5       R   CHG_M2CVOPK_FLG                             1'b0
    6       R   CHG_BATSUP                                  1'b0
    7       R   CHG_ADPHIBAT                                1'b0
    8       R   CHG_FL0X_DUMMY_8                            1'b0
    9       R   CHG_FL0X_DUMMY_9                            1'b0
    10      R   CHG_FL0X_DUMMY_10                           1'b0
    11      R   CHG_FL0X_DUMMY_11                           1'b0
    12      R   CHG_FL0X_DUMMY_12                           1'b0
    13      R   CHG_FL0X_DUMMY_13                           1'b0
    14      R   CHG_FL0X_DUMMY_14                           1'b0
    15      R   CHG_FL0X_DUMMY_15                           1'b0
 */
typedef union _AON_FAST_FLAG0X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_FLAG0X_CHG_DUMM0: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY1: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY2: 1;
        uint16_t CHG_M2CSOPK_FLG: 1;
        uint16_t CHG_M2CCOPK_FLG: 1;
        uint16_t CHG_M2CVOPK_FLG: 1;
        uint16_t CHG_BATSUP: 1;
        uint16_t CHG_ADPHIBAT: 1;
        uint16_t CHG_FL0X_DUMMY_8: 1;
        uint16_t CHG_FL0X_DUMMY_9: 1;
        uint16_t CHG_FL0X_DUMMY_10: 1;
        uint16_t CHG_FL0X_DUMMY_11: 1;
        uint16_t CHG_FL0X_DUMMY_12: 1;
        uint16_t CHG_FL0X_DUMMY_13: 1;
        uint16_t CHG_FL0X_DUMMY_14: 1;
        uint16_t CHG_FL0X_DUMMY_15: 1;
    };
} AON_FAST_FLAG0X_CHG_TYPE;

/* 0x1040
    0       R/W SWR_CORE_SEL_CCMCOT                         1'b0
    1       R/W SWR_CORE_POW_REF_LPPFM                      1'b0
    2       R/W SWR_CORE_FPWM                               1'b0
    3       R/W SWR_CORE_POW_SWR                            1'b0
    4       R/W SWR_CORE_POW_LDO                            1'b0
    5       R/W SWR_CORE_POW_BNYCNT                         1'b0
    6       R/W SWR_CORE_POW_CCMCOT                         1'b0
    7       R/W SWR_CORE_POW_OCP                            1'b0
    8       R/W SWR_CORE_POW_ZCD                            1'b0
    9       R/W SWR_CORE_POW_PFM                            1'b0
    10      R/W SWR_CORE_POW_PWM                            1'b0
    11      R/W SWR_CORE_POW_VDIV                           1'b0
    12      R/W SWR_CORE_POW_REF                            1'b0
    13      R/W SWR_CORE_POW_SAW                            1'b0
    14      R/W SWR_CORE_POW_SAW_IB                         1'b0
    15      R/W SWR_CORE_POW_IMIR                           1'b0
 */
typedef union _AON_FAST_REG0X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_SEL_CCMCOT: 1;
        uint16_t SWR_CORE_POW_REF_LPPFM: 1;
        uint16_t SWR_CORE_FPWM: 1;
        uint16_t SWR_CORE_POW_SWR: 1;
        uint16_t SWR_CORE_POW_LDO: 1;
        uint16_t SWR_CORE_POW_BNYCNT: 1;
        uint16_t SWR_CORE_POW_CCMCOT: 1;
        uint16_t SWR_CORE_POW_OCP: 1;
        uint16_t SWR_CORE_POW_ZCD: 1;
        uint16_t SWR_CORE_POW_PFM: 1;
        uint16_t SWR_CORE_POW_PWM: 1;
        uint16_t SWR_CORE_POW_VDIV: 1;
        uint16_t SWR_CORE_POW_REF: 1;
        uint16_t SWR_CORE_POW_SAW: 1;
        uint16_t SWR_CORE_POW_SAW_IB: 1;
        uint16_t SWR_CORE_POW_IMIR: 1;
    };
} AON_FAST_REG0X_SWR_CORE_TYPE;

/* 0x1042
    5:0     R/W SWR_CORE_TUNE_BNYCNT_INI                    6'b000000
    6       R/W SWR_CORE_BYPASS_SAW_RAMPONDELAY             1'b0
    8:7     R/W SWR_CORE_TUNE_SAW_CCLK                      2'b10
    14:9    R/W SWR_CORE_TUNE_SAW_ICLK                      6'b010000
    15      R/W SWR_CORE_EN_SAW_RAMPON                      1'b1
 */
typedef union _AON_FAST_REG1X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_BNYCNT_INI: 6;
        uint16_t SWR_CORE_BYPASS_SAW_RAMPONDELAY: 1;
        uint16_t SWR_CORE_TUNE_SAW_CCLK: 2;
        uint16_t SWR_CORE_TUNE_SAW_ICLK: 6;
        uint16_t SWR_CORE_EN_SAW_RAMPON: 1;
    };
} AON_FAST_REG1X_SWR_CORE_TYPE;

/* 0x1044
    1:0     R/W SWR_CORE_TUNE_BNYCNT_CLKDIV                 2'b11
    2       R/W SWR_CORE_REG2X_DUMMY2                       1'b0
    3       R/W SWR_CORE_REG2X_DUMMY3                       1'b0
    4       R/W SWR_CORE_SEL_POS_VREFLPPFM                  1'b0
    7:5     R/W SWR_CORE_TUNE_POS_VREFOCPPFM                3'b100
    10:8    R/W SWR_CORE_TUNE_POS_VREFOCP                   3'b111
    13:11   R/W SWR_CORE_TUNE_POS_VREFPWM                   3'b011
    14      R/W SWR_CORE_SEL_POS_VREFSS                     1'b1
    15      R/W SWR_CORE_SEL_POS_VREFOCP                    1'b1
 */
typedef union _AON_FAST_REG2X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_BNYCNT_CLKDIV: 2;
        uint16_t SWR_CORE_REG2X_DUMMY2: 1;
        uint16_t SWR_CORE_REG2X_DUMMY3: 1;
        uint16_t SWR_CORE_SEL_POS_VREFLPPFM: 1;
        uint16_t SWR_CORE_TUNE_POS_VREFOCPPFM: 3;
        uint16_t SWR_CORE_TUNE_POS_VREFOCP: 3;
        uint16_t SWR_CORE_TUNE_POS_VREFPWM: 3;
        uint16_t SWR_CORE_SEL_POS_VREFSS: 1;
        uint16_t SWR_CORE_SEL_POS_VREFOCP: 1;
    };
} AON_FAST_REG2X_SWR_CORE_TYPE;

/* 0x1046
    7:0     R/W SWR_CORE_TUNE_VDIV                          8'b10001010
    15:8    R/W SWR_CORE_TUNE_POS_VREFPFM                   8'b01101110
 */
typedef union _AON_FAST_REG3X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_VDIV: 8;
        uint16_t SWR_CORE_TUNE_POS_VREFPFM: 8;
    };
} AON_FAST_REG3X_SWR_CORE_TYPE;

/* 0x1048
    2:0     R/W SWR_CORE_TUNE_PWM_C2                        3'b000
    3       R/W SWR_CORE_BYPASS_PWM_RoughSS                 1'b0
    5:4     R/W SWR_CORE_TUNE_PWM_RoughSS                   2'b11
    8:6     R/W SWR_CORE_TUNE_PWM_VCL                       3'b001
    11:9    R/W SWR_CORE_TUNE_PWM_VCH                       3'b110
    12      R/W SWR_CORE_BYPASS_PWM_SSR                     1'b1
    13      R/W SWR_CORE_X4_PWM_COMP_IB                     1'b0
    14      R/W SWR_CORE_REG4X_DUMMY14                      1'b0
    15      R/W SWR_CORE_POW_PWM_CLP                        1'b0
 */
typedef union _AON_FAST_REG4X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_PWM_C2: 3;
        uint16_t SWR_CORE_BYPASS_PWM_RoughSS: 1;
        uint16_t SWR_CORE_TUNE_PWM_RoughSS: 2;
        uint16_t SWR_CORE_TUNE_PWM_VCL: 3;
        uint16_t SWR_CORE_TUNE_PWM_VCH: 3;
        uint16_t SWR_CORE_BYPASS_PWM_SSR: 1;
        uint16_t SWR_CORE_X4_PWM_COMP_IB: 1;
        uint16_t SWR_CORE_REG4X_DUMMY14: 1;
        uint16_t SWR_CORE_POW_PWM_CLP: 1;
    };
} AON_FAST_REG4X_SWR_CORE_TYPE;

/* 0x104A
    0       R/W SWR_CORE_REG5X_DUMMY0                       1'b0
    3:1     R/W SWR_CORE_TUNE_PWM_R3                        3'b000
    6:4     R/W SWR_CORE_TUNE_PWM_R2                        3'b010
    9:7     R/W SWR_CORE_TUNE_PWM_R1                        3'b111
    12:10   R/W SWR_CORE_TUNE_PWM_C3                        3'b101
    15:13   R/W SWR_CORE_TUNE_PWM_C1                        3'b100
 */
typedef union _AON_FAST_REG5X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG5X_DUMMY0: 1;
        uint16_t SWR_CORE_TUNE_PWM_R3: 3;
        uint16_t SWR_CORE_TUNE_PWM_R2: 3;
        uint16_t SWR_CORE_TUNE_PWM_R1: 3;
        uint16_t SWR_CORE_TUNE_PWM_C3: 3;
        uint16_t SWR_CORE_TUNE_PWM_C1: 3;
    };
} AON_FAST_REG5X_SWR_CORE_TYPE;

/* 0x104C
    0       R/W SWR_CORE_X4_PFM_COMP_IB                     1'b0
    2:1     R/W SWR_CORE_TUNE_PFM_ICOT                      2'b01
    7:3     R/W SWR_CORE_TUNE_PFM_CCOT                      5'b00111
    8       R/W SWR_CORE_EN_PFM_COT                         1'b1
    9       R/W SWR_CORE_EN_PFM_ForceOFFtoZCD               1'b1
    11:10   R/W SWR_CORE_SEL_PFM_VCL                        2'b10
    12      R/W SWR_CORE_EN_PFM_FollowSTOP                  1'b1
    14:13   R/W SWR_CORE_TUNE_PWM_IMINON                    2'b10
    15      R/W SWR_CORE_POW_PWM_MINON                      1'b1
 */
typedef union _AON_FAST_REG6X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_X4_PFM_COMP_IB: 1;
        uint16_t SWR_CORE_TUNE_PFM_ICOT: 2;
        uint16_t SWR_CORE_TUNE_PFM_CCOT: 5;
        uint16_t SWR_CORE_EN_PFM_COT: 1;
        uint16_t SWR_CORE_EN_PFM_ForceOFFtoZCD: 1;
        uint16_t SWR_CORE_SEL_PFM_VCL: 2;
        uint16_t SWR_CORE_EN_PFM_FollowSTOP: 1;
        uint16_t SWR_CORE_TUNE_PWM_IMINON: 2;
        uint16_t SWR_CORE_POW_PWM_MINON: 1;
    };
} AON_FAST_REG6X_SWR_CORE_TYPE;

/* 0x104E
    0       R/W SWR_CORE_RST_B_CKOUT0X                      1'b0
    1       R/W SWR_CORE_POW_HVD17_SHORT                    1'b0
    2       R/W SWR_CORE_EN_HVD17_LOWIQ                     1'b1
    3       R/W SWR_CORE_EN_HVD17_POWDN_CURRENT             1'b1
    6:4     R/W SWR_CORE_TUNE_HVD17_POR17_VREFL             3'b011
    9:7     R/W SWR_CORE_TUNE_HVD17_POR17_VREFH             3'b011
    13:10   R/W SWR_CORE_TUNE_HVD17_IB                      4'b1000
    14      R/W SWR_CORE_FORCE_HVD17_POR                    1'b0
    15      R/W SWR_CORE_EN_HVD17_POR17                     1'b1
 */
typedef union _AON_FAST_REG7X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_RST_B_CKOUT0X: 1;
        uint16_t SWR_CORE_POW_HVD17_SHORT: 1;
        uint16_t SWR_CORE_EN_HVD17_LOWIQ: 1;
        uint16_t SWR_CORE_EN_HVD17_POWDN_CURRENT: 1;
        uint16_t SWR_CORE_TUNE_HVD17_POR17_VREFL: 3;
        uint16_t SWR_CORE_TUNE_HVD17_POR17_VREFH: 3;
        uint16_t SWR_CORE_TUNE_HVD17_IB: 4;
        uint16_t SWR_CORE_FORCE_HVD17_POR: 1;
        uint16_t SWR_CORE_EN_HVD17_POR17: 1;
    };
} AON_FAST_REG7X_SWR_CORE_TYPE;

/* 0x1050
    6:0     R/W SWR_CORE_TUNE_ZCD_FORCECODE                 7'b0110000
    7       R/W SWR_CORE_RST_B_CKOUT1X                      1'b0
    8       R/W SWR_CORE_STICKY_ZCD_CODE                    1'b0
    9       R/W SWR_CORE_FORCE_ZCD_CODE                     1'b0
    10      R/W SWR_CORE_RST_B_CKOUT2X                      1'b0
    12:11   R/W SWR_CORE_TUNE_OCP_RES                       2'b10
    13      R/W SWR_CORE_SEL_OCP_RST                        1'b0
    14      R/W SWR_CORE_SEL_OCP_SET                        1'b1
    15      R/W SWR_CORE_SEL_OCP_TABLE                      1'b0
 */
typedef union _AON_FAST_REG8X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_ZCD_FORCECODE: 7;
        uint16_t SWR_CORE_RST_B_CKOUT1X: 1;
        uint16_t SWR_CORE_STICKY_ZCD_CODE: 1;
        uint16_t SWR_CORE_FORCE_ZCD_CODE: 1;
        uint16_t SWR_CORE_RST_B_CKOUT2X: 1;
        uint16_t SWR_CORE_TUNE_OCP_RES: 2;
        uint16_t SWR_CORE_SEL_OCP_RST: 1;
        uint16_t SWR_CORE_SEL_OCP_SET: 1;
        uint16_t SWR_CORE_SEL_OCP_TABLE: 1;
    };
} AON_FAST_REG8X_SWR_CORE_TYPE;

/* 0x1052
    0       R/W SWR_CORE_EN_NONOVERLAP_BYPASSLX             1'b0
    1       R/W SWR_CORE_EN_NONOVERLAP_OCPMUX               1'b1
    2       R/W SWR_CORE_EN_POWERMOS_DR8X                   1'b1
    3       R/W SWR_CORE_SEL_NONOVERLAP_PGATEFB             1'b0
    4       R/W SWR_CORE_REG9X_DUMMY4                       1'b0
    6:5     R/W SWR_CORE_TUNE_OCP_OFFTIME                   2'b00
    7       R/W SWR_CORE_POW_ZCD_COMP_CLAMPLX               1'b1
    8       R/W SWR_CORE_POW_ZCD_COMP_LOWIQ                 1'b0
    10:9    R/W SWR_CORE_TUNE_ZCD_SDZ2d                     2'b00
    13:11   R/W SWR_CORE_TUNE_ZCD_SDZ2                      3'b001
    15:14   R/W SWR_CORE_TUNE_ZCD_SDZ1                      2'b00
 */
typedef union _AON_FAST_REG9X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_EN_NONOVERLAP_BYPASSLX: 1;
        uint16_t SWR_CORE_EN_NONOVERLAP_OCPMUX: 1;
        uint16_t SWR_CORE_EN_POWERMOS_DR8X: 1;
        uint16_t SWR_CORE_SEL_NONOVERLAP_PGATEFB: 1;
        uint16_t SWR_CORE_REG9X_DUMMY4: 1;
        uint16_t SWR_CORE_TUNE_OCP_OFFTIME: 2;
        uint16_t SWR_CORE_POW_ZCD_COMP_CLAMPLX: 1;
        uint16_t SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t SWR_CORE_TUNE_ZCD_SDZ2d: 2;
        uint16_t SWR_CORE_TUNE_ZCD_SDZ2: 3;
        uint16_t SWR_CORE_TUNE_ZCD_SDZ1: 2;
    };
} AON_FAST_REG9X_SWR_CORE_TYPE;

/* 0x1054
    3:0     R/W SWR_CORE_TUNE_REF_VREFLPPFM                 4'b0110
    4       R/W SWR_CORE_REG10X_DUMMY4                      1'b1
    5       R/W SWR_CORE_BYPASS_CCMCOT_RoughSS              1'b0
    7:6     R/W SWR_CORE_TUNE_CCMCOT_RoughSS                2'b00
    9:8     R/W SWR_CORE_TUNE_CCMCOT_LXLPF_C                2'b01
    11:10   R/W SWR_CORE_TUNE_CCMCOT_LXLPF_R                2'b01
    13:12   R/W SWR_CORE_TUNE_CCMCOT_EA_C                   2'b10
    15:14   R/W SWR_CORE_TUNE_CCMCOT_EA_R                   2'b10
 */
typedef union _AON_FAST_REG10X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_REF_VREFLPPFM: 4;
        uint16_t SWR_CORE_REG10X_DUMMY4: 1;
        uint16_t SWR_CORE_BYPASS_CCMCOT_RoughSS: 1;
        uint16_t SWR_CORE_TUNE_CCMCOT_RoughSS: 2;
        uint16_t SWR_CORE_TUNE_CCMCOT_LXLPF_C: 2;
        uint16_t SWR_CORE_TUNE_CCMCOT_LXLPF_R: 2;
        uint16_t SWR_CORE_TUNE_CCMCOT_EA_C: 2;
        uint16_t SWR_CORE_TUNE_CCMCOT_EA_R: 2;
    };
} AON_FAST_REG10X_SWR_CORE_TYPE;

/* 0x1056
    0       R   SWR_CORE_HVD17_POR                          1'b0
    1       R   SWR_CORE_FLAG0X_DUMMY1                      1'b0
    2       R   SWR_CORE_FLAG0X_DUMMY2                      1'b0
    3       R   SWR_CORE_FLAG0X_DUMMY3                      1'b0
    4       R   SWR_CORE_FLAG0X_DUMMY4                      1'b0
    5       R   SWR_CORE_FLAG0X_DUMMY5                      1'b0
    6       R   SWR_CORE_FLAG0X_DUMMY6                      1'b0
    7       R   SWR_CORE_FLAG0X_DUMMY7                      1'b0
    8       R   SWR_CORE_FLAG0X_DUMMY8                      1'b0
    9       R   SWR_CORE_FLAG0X_DUMMY9                      1'b0
    10      R   SWR_CORE_FLAG0X_DUMMY10                     1'b0
    11      R   SWR_CORE_FLAG0X_DUMMY11                     1'b0
    12      R   SWR_CORE_FLAG0X_DUMMY12                     1'b0
    13      R   SWR_CORE_FLAG0X_DUMMY13                     1'b0
    14      R   SWR_CORE_FLAG0X_DUMMY14                     1'b0
    15      R   SWR_CORE_FLAG0X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_FLAG0X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_HVD17_POR: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY1: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY2: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY3: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY4: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY5: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY6: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY7: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY8: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY9: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY10: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY11: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY12: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY13: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY14: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY15: 1;
    };
} AON_FAST_FLAG0X_SWR_CORE_TYPE;

/* 0x1058
    0       R   SWR_CORE_ONTIME_OVER                        1'b0
    1       R   SWR_CORE_ONTIME_SET                         1'b0
    2       R   SWR_CORE_VCOMP                              1'b0
    3       R   SWR_CORE_PFM_CTRL                           1'b0
    4       R   SWR_CORE_PWM_CTRL                           1'b0
    5       R   SWR_CORE_OCP                                1'b0
    6       R   SWR_CORE_ZCD                                1'b0
    7       R   SWR_CORE_LX_FALL_DET                        1'b0
    9:8     R   SWR_CORE_S                                  2'b00
    10      R   SWR_CORE_CK_CTRL                            1'b0
    11      R   SWR_CORE_NI                                 1'b0
    12      R   SWR_CORE_PI                                 1'b0
    13      R   SWR_CORE_PGATEd                             1'b0
    14      R   SWR_CORE_NGATE_HV                           1'b0
    15      R   SWR_CORE_PGATE_HV                           1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_ONTIME_OVER: 1;
        uint16_t SWR_CORE_ONTIME_SET: 1;
        uint16_t SWR_CORE_VCOMP: 1;
        uint16_t SWR_CORE_PFM_CTRL: 1;
        uint16_t SWR_CORE_PWM_CTRL: 1;
        uint16_t SWR_CORE_OCP: 1;
        uint16_t SWR_CORE_ZCD: 1;
        uint16_t SWR_CORE_LX_FALL_DET: 1;
        uint16_t SWR_CORE_S: 2;
        uint16_t SWR_CORE_CK_CTRL: 1;
        uint16_t SWR_CORE_NI: 1;
        uint16_t SWR_CORE_PI: 1;
        uint16_t SWR_CORE_PGATEd: 1;
        uint16_t SWR_CORE_NGATE_HV: 1;
        uint16_t SWR_CORE_PGATE_HV: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_TYPE;

/* 0x105A
    6:0     R   SWR_CORE_ZCDQ                               7'b0000000
    7       R   SWR_CORE_CKOUT1X_DUMMY7                     1'b0
    8       R   SWR_CORE_CKOUT1X_DUMMY8                     1'b0
    9       R   SWR_CORE_UPDATE_CK                          1'b0
    10      R   SWR_CORE_EN_UPDATE                          1'b0
    11      R   SWR_CORE_UD                                 1'b0
    12      R   SWR_CORE_SampleOver                         1'b0
    13      R   SWR_CORE_LX_DET                             1'b0
    14      R   SWR_CORE_CKOUT1X_DUMMY14                    1'b0
    15      R   SWR_CORE_ZCD_SET                            1'b0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_ZCDQ: 7;
        uint16_t SWR_CORE_CKOUT1X_DUMMY7: 1;
        uint16_t SWR_CORE_CKOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_UPDATE_CK: 1;
        uint16_t SWR_CORE_EN_UPDATE: 1;
        uint16_t SWR_CORE_UD: 1;
        uint16_t SWR_CORE_SampleOver: 1;
        uint16_t SWR_CORE_LX_DET: 1;
        uint16_t SWR_CORE_CKOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_ZCD_SET: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_TYPE;

/* 0x105C
    0       R   SWR_CORE_X_RAMPON                           1'b0
    1       R   SWR_CORE_X_CLK                              1'b0
    2       R   SWR_CORE_SOFTSTART_OVER                     1'b0
    3       R   SWR_CORE_VREFSS_OVER                        1'b0
    4       R   SWR_CORE_STOP                               1'b0
    5       R   SWR_CORE_VREFSS_START                       1'b0
    10:6    R   SWR_CORE_SSCODE                             5'b00000
    11      R   SWR_CORE_ENLVS_O                            1'b0
    12      R   SWR_CORE_LDO17_POR                          1'b0
    13      R   SWR_CORE_ENHV17_O                           1'b0
    14      R   SWR_CORE_OCP_OUT                            1'b0
    15      R   SWR_CORE_TIE_RDY                            1'b0
 */
typedef union _AON_FAST_C_KOUT2X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_X_RAMPON: 1;
        uint16_t SWR_CORE_X_CLK: 1;
        uint16_t SWR_CORE_SOFTSTART_OVER: 1;
        uint16_t SWR_CORE_VREFSS_OVER: 1;
        uint16_t SWR_CORE_STOP: 1;
        uint16_t SWR_CORE_VREFSS_START: 1;
        uint16_t SWR_CORE_SSCODE: 5;
        uint16_t SWR_CORE_ENLVS_O: 1;
        uint16_t SWR_CORE_LDO17_POR: 1;
        uint16_t SWR_CORE_ENHV17_O: 1;
        uint16_t SWR_CORE_OCP_OUT: 1;
        uint16_t SWR_CORE_TIE_RDY: 1;
    };
} AON_FAST_C_KOUT2X_SWR_CORE_TYPE;

/* 0x11C0
    0       R/W SWR_AUDIO_SEL_CCMCOT                        1'b0
    1       R/W SWR_AUDIO_REG0X_DUMMY1                      1'b0
    2       R/W SWR_AUDIO_FPWM                              1'b0
    3       R/W SWR_AUDIO_POW_SWR                           1'b0
    4       R/W SWR_AUDIO_POW_LDO                           1'b0
    5       R/W SWR_AUDIO_POW_BNYCNT                        1'b0
    6       R/W SWR_AUDIO_POW_CCMCOT                        1'b0
    7       R/W SWR_AUDIO_POW_OCP                           1'b0
    8       R/W SWR_AUDIO_POW_ZCD                           1'b0
    9       R/W SWR_AUDIO_POW_PFM                           1'b0
    10      R/W SWR_AUDIO_REG0X_DUMMY10                     1'b0
    11      R/W SWR_AUDIO_POW_VDIV                          1'b0
    12      R/W SWR_AUDIO_POW_REF                           1'b0
    13      R/W SWR_AUDIO_REG0X_DUMMY13                     1'b0
    14      R/W SWR_AUDIO_REG0X_DUMMY14                     1'b0
    15      R/W SWR_AUDIO_POW_IMIR                          1'b0
 */
typedef union _AON_FAST_REG0X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_SEL_CCMCOT: 1;
        uint16_t SWR_AUDIO_REG0X_DUMMY1: 1;
        uint16_t SWR_AUDIO_FPWM: 1;
        uint16_t SWR_AUDIO_POW_SWR: 1;
        uint16_t SWR_AUDIO_POW_LDO: 1;
        uint16_t SWR_AUDIO_POW_BNYCNT: 1;
        uint16_t SWR_AUDIO_POW_CCMCOT: 1;
        uint16_t SWR_AUDIO_POW_OCP: 1;
        uint16_t SWR_AUDIO_POW_ZCD: 1;
        uint16_t SWR_AUDIO_POW_PFM: 1;
        uint16_t SWR_AUDIO_REG0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_POW_VDIV: 1;
        uint16_t SWR_AUDIO_POW_REF: 1;
        uint16_t SWR_AUDIO_REG0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_REG0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_POW_IMIR: 1;
    };
} AON_FAST_REG0X_SWR_AUDIO_TYPE;

/* 0x11C2
    5:0     R/W SWR_AUDIO_TUNE_BNYCNT_INI                   6'b000000
    6       R/W SWR_AUDIO_REG1X_DUMMY6                      1'b0
    7       R/W SWR_AUDIO_REG1X_DUMMY7                      1'b0
    8       R/W SWR_AUDIO_REG1X_DUMMY8                      1'b0
    9       R/W SWR_AUDIO_REG1X_DUMMY9                      1'b0
    10      R/W SWR_AUDIO_REG1X_DUMMY10                     1'b0
    11      R/W SWR_AUDIO_REG1X_DUMMY11                     1'b0
    12      R/W SWR_AUDIO_REG1X_DUMMY12                     1'b0
    13      R/W SWR_AUDIO_REG1X_DUMMY13                     1'b0
    14      R/W SWR_AUDIO_REG1X_DUMMY14                     1'b0
    15      R/W SWR_AUDIO_REG1X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_REG1X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_BNYCNT_INI: 6;
        uint16_t SWR_AUDIO_REG1X_DUMMY6: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY7: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY8: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY9: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY10: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY11: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY12: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY13: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_AUDIO_TYPE;

/* 0x11C4
    1:0     R/W SWR_AUDIO_TUNE_BNYCNT_CLKDIV                2'b11
    2       R/W SWR_AUDIO_REG2X_DUMMY2                      1'b0
    3       R/W SWR_AUDIO_REG2X_DUMMY3                      1'b0
    4       R/W SWR_AUDIO_RST_B_CKOUT2X                     1'b0
    7:5     R/W SWR_AUDIO_TUNE_POS_VREFOCPPFM               3'b100
    10:8    R/W SWR_AUDIO_TUNE_POS_VREFOCP                  3'b111
    13:11   R/W SWR_AUDIO_TUNE_POS_VREFPWM                  3'b011
    14      R/W SWR_AUDIO_SEL_POS_VREFSS                    1'b1
    15      R/W SWR_AUDIO_SEL_POS_VREFOCP                   1'b1
 */
typedef union _AON_FAST_REG2X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_BNYCNT_CLKDIV: 2;
        uint16_t SWR_AUDIO_REG2X_DUMMY2: 1;
        uint16_t SWR_AUDIO_REG2X_DUMMY3: 1;
        uint16_t SWR_AUDIO_RST_B_CKOUT2X: 1;
        uint16_t SWR_AUDIO_TUNE_POS_VREFOCPPFM: 3;
        uint16_t SWR_AUDIO_TUNE_POS_VREFOCP: 3;
        uint16_t SWR_AUDIO_TUNE_POS_VREFPWM: 3;
        uint16_t SWR_AUDIO_SEL_POS_VREFSS: 1;
        uint16_t SWR_AUDIO_SEL_POS_VREFOCP: 1;
    };
} AON_FAST_REG2X_SWR_AUDIO_TYPE;

/* 0x11C6
    7:0     R/W SWR_AUDIO_TUNE_VDIV                         8'b10011011
    15:8    R/W SWR_AUDIO_TUNE_POS_VREFPFM                  8'b10101111
 */
typedef union _AON_FAST_REG3X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_VDIV: 8;
        uint16_t SWR_AUDIO_TUNE_POS_VREFPFM: 8;
    };
} AON_FAST_REG3X_SWR_AUDIO_TYPE;

/* 0x11C8
    0       R/W SWR_AUDIO_REG4X_DUMMY0                      1'b0
    1       R/W SWR_AUDIO_REG4X_DUMMY1                      1'b0
    2       R/W SWR_AUDIO_REG4X_DUMMY2                      1'b0
    3       R/W SWR_AUDIO_REG4X_DUMMY3                      1'b0
    4       R/W SWR_AUDIO_REG4X_DUMMY4                      1'b0
    5       R/W SWR_AUDIO_REG4X_DUMMY5                      1'b0
    6       R/W SWR_AUDIO_REG4X_DUMMY6                      1'b0
    7       R/W SWR_AUDIO_REG4X_DUMMY7                      1'b0
    8       R/W SWR_AUDIO_REG4X_DUMMY8                      1'b0
    9       R/W SWR_AUDIO_REG4X_DUMMY9                      1'b0
    10      R/W SWR_AUDIO_REG4X_DUMMY10                     1'b0
    11      R/W SWR_AUDIO_REG4X_DUMMY11                     1'b0
    12      R/W SWR_AUDIO_REG4X_DUMMY12                     1'b0
    13      R/W SWR_AUDIO_REG4X_DUMMY13                     1'b0
    14      R/W SWR_AUDIO_REG4X_DUMMY14                     1'b0
    15      R/W SWR_AUDIO_REG4X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_REG4X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG4X_DUMMY0: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY1: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY2: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY3: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY4: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY5: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY6: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY7: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY8: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY9: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY10: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY11: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY12: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY13: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY14: 1;
        uint16_t SWR_AUDIO_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_SWR_AUDIO_TYPE;

/* 0x11CA
    0       R/W SWR_AUDIO_REG5X_DUMMY0                      1'b0
    1       R/W SWR_AUDIO_REG5X_DUMMY1                      1'b0
    2       R/W SWR_AUDIO_REG5X_DUMMY2                      1'b0
    3       R/W SWR_AUDIO_REG5X_DUMMY3                      1'b0
    4       R/W SWR_AUDIO_REG5X_DUMMY4                      1'b0
    5       R/W SWR_AUDIO_REG5X_DUMMY5                      1'b0
    6       R/W SWR_AUDIO_REG5X_DUMMY6                      1'b0
    7       R/W SWR_AUDIO_REG5X_DUMMY7                      1'b0
    8       R/W SWR_AUDIO_REG5X_DUMMY8                      1'b0
    9       R/W SWR_AUDIO_REG5X_DUMMY9                      1'b0
    10      R/W SWR_AUDIO_REG5X_DUMMY10                     1'b0
    11      R/W SWR_AUDIO_REG5X_DUMMY11                     1'b0
    12      R/W SWR_AUDIO_REG5X_DUMMY12                     1'b0
    13      R/W SWR_AUDIO_REG5X_DUMMY13                     1'b0
    14      R/W SWR_AUDIO_REG5X_DUMMY14                     1'b0
    15      R/W SWR_AUDIO_REG5X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_REG5X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG5X_DUMMY0: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY1: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY2: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY3: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY4: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY5: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY6: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY7: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY8: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY9: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY10: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY11: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY12: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY13: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY14: 1;
        uint16_t SWR_AUDIO_REG5X_DUMMY15: 1;
    };
} AON_FAST_REG5X_SWR_AUDIO_TYPE;

/* 0x11CC
    0       R/W SWR_AUDIO_X4_PFM_COMP_IB                    1'b0
    2:1     R/W SWR_AUDIO_TUNE_PFM_ICOT                     2'b01
    7:3     R/W SWR_AUDIO_TUNE_PFM_CCOT                     5'b01100
    8       R/W SWR_AUDIO_EN_PFM_COT                        1'b1
    9       R/W SWR_AUDIO_EN_PFM_ForceOFFtoZCD              1'b1
    11:10   R/W SWR_AUDIO_SEL_PFM_VCL                       2'b10
    12      R/W SWR_AUDIO_EN_PFM_FollowSTOP                 1'b1
    13      R/W SWR_AUDIO_REG6X_DUMMY13                     1'b0
    14      R/W SWR_AUDIO_REG6X_DUMMY14                     1'b1
    15      R/W SWR_AUDIO_REG6X_DUMMY15                     1'b1
 */
typedef union _AON_FAST_REG6X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_X4_PFM_COMP_IB: 1;
        uint16_t SWR_AUDIO_TUNE_PFM_ICOT: 2;
        uint16_t SWR_AUDIO_TUNE_PFM_CCOT: 5;
        uint16_t SWR_AUDIO_EN_PFM_COT: 1;
        uint16_t SWR_AUDIO_EN_PFM_ForceOFFtoZCD: 1;
        uint16_t SWR_AUDIO_SEL_PFM_VCL: 2;
        uint16_t SWR_AUDIO_EN_PFM_FollowSTOP: 1;
        uint16_t SWR_AUDIO_REG6X_DUMMY13: 1;
        uint16_t SWR_AUDIO_REG6X_DUMMY14: 1;
        uint16_t SWR_AUDIO_REG6X_DUMMY15: 1;
    };
} AON_FAST_REG6X_SWR_AUDIO_TYPE;

/* 0x11CE
    0       R/W SWR_AUDIO_RST_B_CKOUT0X                     1'b0
    1       R/W SWR_AUDIO_POW_HVD17_SHORT                   1'b0
    2       R/W SWR_AUDIO_EN_HVD17_LOWIQ                    1'b1
    3       R/W SWR_AUDIO_EN_HVD17_POWDN_CURRENT            1'b1
    6:4     R/W SWR_AUDIO_TUNE_HVD17_POR17_VREFL            3'b011
    9:7     R/W SWR_AUDIO_TUNE_HVD17_POR17_VREFH            3'b011
    13:10   R/W SWR_AUDIO_TUNE_HVD17_IB                     4'b1000
    14      R/W SWR_AUDIO_FORCE_HVD17_POR                   1'b0
    15      R/W SWR_AUDIO_EN_HVD17_POR17                    1'b1
 */
typedef union _AON_FAST_REG7X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_RST_B_CKOUT0X: 1;
        uint16_t SWR_AUDIO_POW_HVD17_SHORT: 1;
        uint16_t SWR_AUDIO_EN_HVD17_LOWIQ: 1;
        uint16_t SWR_AUDIO_EN_HVD17_POWDN_CURRENT: 1;
        uint16_t SWR_AUDIO_TUNE_HVD17_POR17_VREFL: 3;
        uint16_t SWR_AUDIO_TUNE_HVD17_POR17_VREFH: 3;
        uint16_t SWR_AUDIO_TUNE_HVD17_IB: 4;
        uint16_t SWR_AUDIO_FORCE_HVD17_POR: 1;
        uint16_t SWR_AUDIO_EN_HVD17_POR17: 1;
    };
} AON_FAST_REG7X_SWR_AUDIO_TYPE;

/* 0x11D0
    6:0     R/W SWR_AUDIO_TUNE_ZCD_FORCECODE                7'b0110000
    7       R/W SWR_AUDIO_RST_B_CKOUT1X                     1'b0
    8       R/W SWR_AUDIO_STICKY_ZCD_CODE                   1'b0
    9       R/W SWR_AUDIO_FORCE_ZCD_CODE                    1'b0
    10      R/W SWR_AUDIO_EN_SCP                            1'b0
    12:11   R/W SWR_AUDIO_TUNE_OCP_RES                      2'b10
    13      R/W SWR_AUDIO_SEL_OCP_RST                       1'b0
    14      R/W SWR_AUDIO_SEL_OCP_SET                       1'b1
    15      R/W SWR_AUDIO_SEL_OCP_TABLE                     1'b0
 */
typedef union _AON_FAST_REG8X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_ZCD_FORCECODE: 7;
        uint16_t SWR_AUDIO_RST_B_CKOUT1X: 1;
        uint16_t SWR_AUDIO_STICKY_ZCD_CODE: 1;
        uint16_t SWR_AUDIO_FORCE_ZCD_CODE: 1;
        uint16_t SWR_AUDIO_EN_SCP: 1;
        uint16_t SWR_AUDIO_TUNE_OCP_RES: 2;
        uint16_t SWR_AUDIO_SEL_OCP_RST: 1;
        uint16_t SWR_AUDIO_SEL_OCP_SET: 1;
        uint16_t SWR_AUDIO_SEL_OCP_TABLE: 1;
    };
} AON_FAST_REG8X_SWR_AUDIO_TYPE;

/* 0x11D2
    0       R/W SWR_AUDIO_EN_NONOVERLAP_BYPASSLX            1'b0
    1       R/W SWR_AUDIO_EN_NONOVERLAP_OCPMUX              1'b1
    2       R/W SWR_AUDIO_EN_POWERMOS_DR8X                  1'b1
    3       R/W SWR_AUDIO_SEL_NONOVERLAP_PGATEFB            1'b0
    4       R/W SWR_AUDIO_REG9X_DUMMY4                      1'b0
    6:5     R/W SWR_AUDIO_TUNE_OCP_OFFTIME                  2'b00
    7       R/W SWR_AUDIO_POW_ZCD_COMP_CLAMPLX              1'b1
    8       R/W SWR_AUDIO_POW_ZCD_COMP_LOWIQ                1'b0
    10:9    R/W SWR_AUDIO_TUNE_ZCD_SDZ2d                    2'b00
    13:11   R/W SWR_AUDIO_TUNE_ZCD_SDZ2                     3'b001
    15:14   R/W SWR_AUDIO_TUNE_ZCD_SDZ1                     2'b00
 */
typedef union _AON_FAST_REG9X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_EN_NONOVERLAP_BYPASSLX: 1;
        uint16_t SWR_AUDIO_EN_NONOVERLAP_OCPMUX: 1;
        uint16_t SWR_AUDIO_EN_POWERMOS_DR8X: 1;
        uint16_t SWR_AUDIO_SEL_NONOVERLAP_PGATEFB: 1;
        uint16_t SWR_AUDIO_REG9X_DUMMY4: 1;
        uint16_t SWR_AUDIO_TUNE_OCP_OFFTIME: 2;
        uint16_t SWR_AUDIO_POW_ZCD_COMP_CLAMPLX: 1;
        uint16_t SWR_AUDIO_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t SWR_AUDIO_TUNE_ZCD_SDZ2d: 2;
        uint16_t SWR_AUDIO_TUNE_ZCD_SDZ2: 3;
        uint16_t SWR_AUDIO_TUNE_ZCD_SDZ1: 2;
    };
} AON_FAST_REG9X_SWR_AUDIO_TYPE;

/* 0x11D4
    0       R/W SWR_AUDIO_REG10X_DUMMY0                     1'b0
    1       R/W SWR_AUDIO_REG10X_DUMMY1                     1'b0
    2       R/W SWR_AUDIO_REG10X_DUMMY2                     1'b0
    3       R/W SWR_AUDIO_REG10X_DUMMY3                     1'b0
    4       R/W SWR_AUDIO_REG10X_DUMMY4                     1'b1
    5       R/W SWR_AUDIO_BYPASS_CCMCOT_RoughSS             1'b0
    7:6     R/W SWR_AUDIO_TUNE_CCMCOT_RoughSS               2'b00
    9:8     R/W SWR_AUDIO_TUNE_CCMCOT_LXLPF_C               2'b01
    11:10   R/W SWR_AUDIO_TUNE_CCMCOT_LXLPF_R               2'b01
    13:12   R/W SWR_AUDIO_TUNE_CCMCOT_EA_C                  2'b10
    15:14   R/W SWR_AUDIO_TUNE_CCMCOT_EA_R                  2'b10
 */
typedef union _AON_FAST_REG10X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG10X_DUMMY0: 1;
        uint16_t SWR_AUDIO_REG10X_DUMMY1: 1;
        uint16_t SWR_AUDIO_REG10X_DUMMY2: 1;
        uint16_t SWR_AUDIO_REG10X_DUMMY3: 1;
        uint16_t SWR_AUDIO_REG10X_DUMMY4: 1;
        uint16_t SWR_AUDIO_BYPASS_CCMCOT_RoughSS: 1;
        uint16_t SWR_AUDIO_TUNE_CCMCOT_RoughSS: 2;
        uint16_t SWR_AUDIO_TUNE_CCMCOT_LXLPF_C: 2;
        uint16_t SWR_AUDIO_TUNE_CCMCOT_LXLPF_R: 2;
        uint16_t SWR_AUDIO_TUNE_CCMCOT_EA_C: 2;
        uint16_t SWR_AUDIO_TUNE_CCMCOT_EA_R: 2;
    };
} AON_FAST_REG10X_SWR_AUDIO_TYPE;

/* 0x11D6
    0       R   SWR_AUDIO_HVD17_POR                         1'b0
    1       R   SWR_AUDIO_FLAG0X_DUMMY1                     1'b0
    2       R   SWR_AUDIO_FLAG0X_DUMMY2                     1'b0
    3       R   SWR_AUDIO_FLAG0X_DUMMY3                     1'b0
    4       R   SWR_AUDIO_FLAG0X_DUMMY4                     1'b0
    5       R   SWR_AUDIO_FLAG0X_DUMMY5                     1'b0
    6       R   SWR_AUDIO_FLAG0X_DUMMY6                     1'b0
    7       R   SWR_AUDIO_FLAG0X_DUMMY7                     1'b0
    8       R   SWR_AUDIO_FLAG0X_DUMMY8                     1'b0
    9       R   SWR_AUDIO_FLAG0X_DUMMY9                     1'b0
    10      R   SWR_AUDIO_FLAG0X_DUMMY10                    1'b0
    11      R   SWR_AUDIO_FLAG0X_DUMMY11                    1'b0
    12      R   SWR_AUDIO_FLAG0X_DUMMY12                    1'b0
    13      R   SWR_AUDIO_FLAG0X_DUMMY13                    1'b0
    14      R   SWR_AUDIO_FLAG0X_DUMMY14                    1'b0
    15      R   SWR_AUDIO_FLAG0X_DUMMY15                    1'b0
 */
typedef union _AON_FAST_FLAG0X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_HVD17_POR: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY1: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY2: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY3: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY4: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY5: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY6: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY7: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY8: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY9: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY15: 1;
    };
} AON_FAST_FLAG0X_SWR_AUDIO_TYPE;

/* 0x11D8
    0       R   SWR_AUDIO_ONTIME_OVER                       1'b0
    1       R   SWR_AUDIO_ONTIME_SET                        1'b0
    2       R   SWR_AUDIO_CKOUT0X_DUMMY2                    1'b0
    3       R   SWR_AUDIO_PFM_CTRL                          1'b0
    4       R   SWR_AUDIO_CKOUT0X_DUMMY4                    1'b0
    5       R   SWR_AUDIO_OCP                               1'b0
    6       R   SWR_AUDIO_ZCD                               1'b0
    7       R   SWR_AUDIO_LX_FALL_DET                       1'b0
    9:8     R   SWR_AUDIO_S                                 2'b00
    10      R   SWR_AUDIO_CK_CTRL                           1'b0
    11      R   SWR_AUDIO_NI                                1'b0
    12      R   SWR_AUDIO_PI                                1'b0
    13      R   SWR_AUDIO_PGATEd                            1'b0
    14      R   SWR_AUDIO_NGATE_HV                          1'b0
    15      R   SWR_AUDIO_PGATE_HV                          1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_ONTIME_OVER: 1;
        uint16_t SWR_AUDIO_ONTIME_SET: 1;
        uint16_t SWR_AUDIO_CKOUT0X_DUMMY2: 1;
        uint16_t SWR_AUDIO_PFM_CTRL: 1;
        uint16_t SWR_AUDIO_CKOUT0X_DUMMY4: 1;
        uint16_t SWR_AUDIO_OCP: 1;
        uint16_t SWR_AUDIO_ZCD: 1;
        uint16_t SWR_AUDIO_LX_FALL_DET: 1;
        uint16_t SWR_AUDIO_S: 2;
        uint16_t SWR_AUDIO_CK_CTRL: 1;
        uint16_t SWR_AUDIO_NI: 1;
        uint16_t SWR_AUDIO_PI: 1;
        uint16_t SWR_AUDIO_PGATEd: 1;
        uint16_t SWR_AUDIO_NGATE_HV: 1;
        uint16_t SWR_AUDIO_PGATE_HV: 1;
    };
} AON_FAST_C_KOUT0X_SWR_AUDIO_TYPE;

/* 0x11DA
    6:0     R   SWR_AUDIO_ZCDQ                              7'b0000000
    7       R   SWR_AUDIO_CKOUT1X_DUMMY7                    1'b0
    8       R   SWR_AUDIO_CKOUT1X_DUMMY8                    1'b0
    9       R   SWR_AUDIO_UPDATE_CK                         1'b0
    10      R   SWR_AUDIO_EN_UPDATE                         1'b0
    11      R   SWR_AUDIO_UD                                1'b0
    12      R   SWR_AUDIO_SampleOver                        1'b0
    13      R   SWR_AUDIO_LX_DET                            1'b0
    14      R   SWR_AUDIO_CKOUT1X_DUMMY14                   1'b0
    15      R   SWR_AUDIO_ZCD_SET                           1'b0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_ZCDQ: 7;
        uint16_t SWR_AUDIO_CKOUT1X_DUMMY7: 1;
        uint16_t SWR_AUDIO_CKOUT1X_DUMMY8: 1;
        uint16_t SWR_AUDIO_UPDATE_CK: 1;
        uint16_t SWR_AUDIO_EN_UPDATE: 1;
        uint16_t SWR_AUDIO_UD: 1;
        uint16_t SWR_AUDIO_SampleOver: 1;
        uint16_t SWR_AUDIO_LX_DET: 1;
        uint16_t SWR_AUDIO_CKOUT1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_ZCD_SET: 1;
    };
} AON_FAST_C_KOUT1X_SWR_AUDIO_TYPE;

/* 0x11DC
    0       R   SWR_AUDIO_CKOUT2X_DUMMY0                    1'b0
    1       R   SWR_AUDIO_CKOUT2X_DUMMY1                    1'b0
    2       R   SWR_AUDIO_SOFTSTART_OVER                    1'b0
    3       R   SWR_AUDIO_VREFSS_OVER                       1'b0
    4       R   SWR_AUDIO_STOP                              1'b0
    5       R   SWR_AUDIO_VREFSS_START                      1'b0
    10:6    R   SWR_AUDIO_SSCODE                            5'b00000
    11      R   SWR_AUDIO_ENLVS_O                           1'b0
    12      R   SWR_AUDIO_LDO17_POR                         1'b0
    13      R   SWR_AUDIO_ENHV17_O                          1'b0
    14      R   SWR_AUDIO_OCP_OUT                           1'b0
    15      R   SWR_AUDIO_TIE_RDY                           1'b0
 */
typedef union _AON_FAST_C_KOUT2X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_CKOUT2X_DUMMY0: 1;
        uint16_t SWR_AUDIO_CKOUT2X_DUMMY1: 1;
        uint16_t SWR_AUDIO_SOFTSTART_OVER: 1;
        uint16_t SWR_AUDIO_VREFSS_OVER: 1;
        uint16_t SWR_AUDIO_STOP: 1;
        uint16_t SWR_AUDIO_VREFSS_START: 1;
        uint16_t SWR_AUDIO_SSCODE: 5;
        uint16_t SWR_AUDIO_ENLVS_O: 1;
        uint16_t SWR_AUDIO_LDO17_POR: 1;
        uint16_t SWR_AUDIO_ENHV17_O: 1;
        uint16_t SWR_AUDIO_OCP_OUT: 1;
        uint16_t SWR_AUDIO_TIE_RDY: 1;
    };
} AON_FAST_C_KOUT2X_SWR_AUDIO_TYPE;

/* 0x1400
    0       R/W BT_PLL_pow_pll                              1'b0
    3:1     R/W BT_PLL_cp_bia[2:0]                          3'b000
    5:4     R/W BT_PLL_cp_set[1:0]                          2'b11
    7:6     R/W BT_PLL_cs_set[1:0]                          2'b11
    10:8    R/W BT_PLL_rs_set[2:0]                          3'b011
    11      R/W BT_PLL_fref_edge                            1'b0
    12      R/W BT_PLL_ck_pn_sel                            1'b0
    14:13   R/W BT_PLL_VC_THL[1:0]                          2'b00
    15      R/W BT_PLL_c3_set                               1'b0
 */
typedef union _AON_FAST_REG_BT_ANAPAR_PLL0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_pow_pll: 1;
        uint16_t BT_PLL_cp_bia_2_0: 3;
        uint16_t BT_PLL_cp_set_1_0: 2;
        uint16_t BT_PLL_cs_set_1_0: 2;
        uint16_t BT_PLL_rs_set_2_0: 3;
        uint16_t BT_PLL_fref_edge: 1;
        uint16_t BT_PLL_ck_pn_sel: 1;
        uint16_t BT_PLL_VC_THL_1_0: 2;
        uint16_t BT_PLL_c3_set: 1;
    };
} AON_FAST_REG_BT_ANAPAR_PLL0_TYPE;

/* 0x1402
    5:0     R/W BT_PLL_div[5:0]                             6'b000100
    8:6     R/W BT_PLL_r3_set[2:0]                          3'b010
    9       R/W BT_PLL_AGPIO_EN                             1'b0
    10      R/W BT_PLL_AGPIO_gpo                            1'b0
    12:11   R/W BT_PLL_AGPIO_S[1:0]                         2'b00
    13      R/W BT_PLL_AGPIO_SEL                            1'b0
    14      R/W gate_dac_clock                              1'b1
    15      R/W gate_adc_clock                              1'b1
 */
typedef union _AON_FAST_REG_BT_ANAPAR_PLL1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_div_5_0: 6;
        uint16_t BT_PLL_r3_set_2_0: 3;
        uint16_t BT_PLL_AGPIO_EN: 1;
        uint16_t BT_PLL_AGPIO_gpo: 1;
        uint16_t BT_PLL_AGPIO_S_1_0: 2;
        uint16_t BT_PLL_AGPIO_SEL: 1;
        uint16_t gate_dac_clock: 1;
        uint16_t gate_adc_clock: 1;
    };
} AON_FAST_REG_BT_ANAPAR_PLL1_TYPE;

/* 0x1404
    0       R/W BT_PLL_cko1_en                              1'b1
    2:1     R/W div4_2_sel                                  2'b01
    3       R/W BT_PLL_cko2_en                              1'b1
    4       R/W BT_PLL_CK_BTADC_en                          1'b1
    5       R/W BT_PLL_CK_BTADC_sel                         1'b1
    6       R/W BT_PLL_CK_BTDAC_en                          1'b1
    7       R/W BT_PLL_CK_BTDAC_sel                         1'b1
    12:8    R/W BT_PLL_RG2X_DUMMY8                          5'b00000
    13      R/W BT_PLL_reg_BTADC_APR_en                     1'b1
    14      R/W BT_PLL_reg_BTDAC_APR_en                     1'b1
    15      R/W BT_PLL_power_out                            1'b1
 */
typedef union _AON_FAST_REG_BT_ANAPAR_PLL2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_cko1_en: 1;
        uint16_t div4_2_sel: 2;
        uint16_t BT_PLL_cko2_en: 1;
        uint16_t BT_PLL_CK_BTADC_en: 1;
        uint16_t BT_PLL_CK_BTADC_sel: 1;
        uint16_t BT_PLL_CK_BTDAC_en: 1;
        uint16_t BT_PLL_CK_BTDAC_sel: 1;
        uint16_t BT_PLL_RG2X_DUMMY8: 5;
        uint16_t BT_PLL_reg_BTADC_APR_en: 1;
        uint16_t BT_PLL_reg_BTDAC_APR_en: 1;
        uint16_t BT_PLL_power_out: 1;
    };
} AON_FAST_REG_BT_ANAPAR_PLL2_TYPE;

/* 0x1406
    0       R/W BT_PLL_LP_PLL_pow_pll                       1'b0
    1       R/W BT_PLL_LP_PLL_fref_edge                     1'b0
    3:2     R/W BT_PLL_LP_PLL_cp_set[1:0]                   2'b10
    5:4     R/W BT_PLL_LP_PLL_cs_set[1:0]                   2'b10
    8:6     R/W BT_PLL_LP_PLL_rs_set[2:0]                   3'b011
    9       R/W BT_PLL_LP_PLL_cko5_en                       1'b1
    10      R/W BT_PLL_LP_PLL_cko5_sel                      1'b1
    12:11   R/W BT_PLL_LP_PLL_VC_THL[1:0]                   2'b00
    13      R/W BT_PLL_LP_PLL_pow_cpop                      1'b0
    15:14   R/W BT_PLL_RG3X_DUMMY14                         2'b00
 */
typedef union _AON_FAST_REG_BT_ANAPAR_PLL3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_LP_PLL_pow_pll: 1;
        uint16_t BT_PLL_LP_PLL_fref_edge: 1;
        uint16_t BT_PLL_LP_PLL_cp_set_1_0: 2;
        uint16_t BT_PLL_LP_PLL_cs_set_1_0: 2;
        uint16_t BT_PLL_LP_PLL_rs_set_2_0: 3;
        uint16_t BT_PLL_LP_PLL_cko5_en: 1;
        uint16_t BT_PLL_LP_PLL_cko5_sel: 1;
        uint16_t BT_PLL_LP_PLL_VC_THL_1_0: 2;
        uint16_t BT_PLL_LP_PLL_pow_cpop: 1;
        uint16_t BT_PLL_RG3X_DUMMY14: 2;
    };
} AON_FAST_REG_BT_ANAPAR_PLL3_TYPE;

/* 0x1408
    0       R/W BT_PLL_LP_PLL_c3_set                        1'b0
    3:1     R/W BT_PLL_LP_PLL_r3_set[2:0]                   3'b010
    15:4    R/W BT_PLL_RG4X_DUMMY0                          12'b000000000000
 */
typedef union _AON_FAST_REG_BT_ANAPAR_PLL4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_LP_PLL_c3_set: 1;
        uint16_t BT_PLL_LP_PLL_r3_set_2_0: 3;
        uint16_t BT_PLL_RG4X_DUMMY0: 12;
    };
} AON_FAST_REG_BT_ANAPAR_PLL4_TYPE;

/* 0x140A
    15:0    R/W BT_PLL_RG5X_DUMMY0                          16'b0000000000000000
 */
typedef union _AON_FAST_REG_BT_ANAPAR_PLL5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_RG5X_DUMMY0;
    };
} AON_FAST_REG_BT_ANAPAR_PLL5_TYPE;

/* 0x140C
    0       R/W BT_PLL_LDO_SW_LDO2PORCUT                    1'b0
    1       R/W BT_PLL_LDO_ERC_V12A_BTPLL                   1'b0
    2       R/W BT_PLL_LDO_pow_LDO                          1'b0
    3       R/W BT_PLL_LDO_VPULSE                           1'b0
    6:4     R/W BT_PLL_LDO_SW[2:0]                          3'b010
    7       R/W BT_PLL_LDO_DOUBLE_OP_I                      1'b0
    15:8    R/W BT_PLL_LDO_RG0X_DUMMY8                      8'b00000000
 */
typedef union _AON_FAST_REG_BT_ANAPAR_LDO_PLL0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_PLL_LDO_SW_LDO2PORCUT: 1;
        uint16_t BT_PLL_LDO_ERC_V12A_BTPLL: 1;
        uint16_t BT_PLL_LDO_pow_LDO: 1;
        uint16_t BT_PLL_LDO_VPULSE: 1;
        uint16_t BT_PLL_LDO_SW_2_0: 3;
        uint16_t BT_PLL_LDO_DOUBLE_OP_I: 1;
        uint16_t BT_PLL_LDO_RG0X_DUMMY8: 8;
    };
} AON_FAST_REG_BT_ANAPAR_LDO_PLL0_TYPE;

/* 0x1470
    1:0     R/W XTAL32K_SEL_CUR_MAIN                        2'b01
    5:2     R/W XTAL32K_SEL_CUR_GM_INI                      4'b1001
    9:6     R/W XTAL32K_SEL_CUR_GM                          4'b0101
    11:10   R/W XTAL32K_SEL_CUR_REP                         2'b01
    15:12   R/W XTAL32K_SEL_GM                              4'b1111
 */
typedef union _AON_FAST_RG0X_32KXTAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL32K_SEL_CUR_MAIN: 2;
        uint16_t XTAL32K_SEL_CUR_GM_INI: 4;
        uint16_t XTAL32K_SEL_CUR_GM: 4;
        uint16_t XTAL32K_SEL_CUR_REP: 2;
        uint16_t XTAL32K_SEL_GM: 4;
    };
} AON_FAST_RG0X_32KXTAL_TYPE;

/* 0x1472
    0       R/W XTAL32K_EN_CAP_INITIAL                      1'b1
    1       R/W XTAL32K_EN_CAP_AWAKE                        1'b1
    7:2     R/W XTAL32K_TUNE_SC_XI_FREQ                     6'b100000
    13:8    R/W XTAL32K_TUNE_SC_XO_FREQ                     6'b100000
    15:14   R/W XTAL32K_SEL_TOK                             2'b11
 */
typedef union _AON_FAST_RG1X_32KXTAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL32K_EN_CAP_INITIAL: 1;
        uint16_t XTAL32K_EN_CAP_AWAKE: 1;
        uint16_t XTAL32K_TUNE_SC_XI_FREQ: 6;
        uint16_t XTAL32K_TUNE_SC_XO_FREQ: 6;
        uint16_t XTAL32K_SEL_TOK: 2;
    };
} AON_FAST_RG1X_32KXTAL_TYPE;

/* 0x1474
    0       R/W XTAL32K_EN_FBRES_B                          1'b0
    3:1     R/W XTAL32K_SEL_GM_REP                          3'b111
    4       R/W XTAL32K_TUNE_SC_XI_EXTRA_B                  1'b0
    5       R/W XTAL32K_TUNE_SC_XO_EXTRA_B                  1'b0
    6       R/W XTAL32K_EN_GPIO_MODE                        1'b0
    7       R/W XTAL32K_RG2X_DUMMY2                         1'b0
    15:8    R/W XTAL32K_RG2X_DUMMY1                         8'b00000000
 */
typedef union _AON_FAST_RG2X_32KXTAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL32K_EN_FBRES_B: 1;
        uint16_t XTAL32K_SEL_GM_REP: 3;
        uint16_t XTAL32K_TUNE_SC_XI_EXTRA_B: 1;
        uint16_t XTAL32K_TUNE_SC_XO_EXTRA_B: 1;
        uint16_t XTAL32K_EN_GPIO_MODE: 1;
        uint16_t XTAL32K_RG2X_DUMMY2: 1;
        uint16_t XTAL32K_RG2X_DUMMY1: 8;
    };
} AON_FAST_RG2X_32KXTAL_TYPE;

/* 0x1476
    5:0     R/W OSC32K_TUNE_RCAL_FREQ                       6'b100000
    12:6    R/W OSC32K_REG0X_DUMMY2                         7'b0000000
    13      R/W OSC32K_REG0X_DUMMY1                         1'b0
    14      R/W OSC32K_GATED_STUP_OK                        1'b0
    15      R/W OSC32K_SEL_LDO_VREF                         1'b0
 */
typedef union _AON_FAST_RG0X_32KOSC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t OSC32K_TUNE_RCAL_FREQ: 6;
        uint16_t OSC32K_REG0X_DUMMY2: 7;
        uint16_t OSC32K_REG0X_DUMMY1: 1;
        uint16_t OSC32K_GATED_STUP_OK: 1;
        uint16_t OSC32K_SEL_LDO_VREF: 1;
    };
} AON_FAST_RG0X_32KOSC_TYPE;

/* 0x1478
    0       R/W POW32K_32KOSC                               1'b1
    1       R/W POW32K_32KXTAL                              1'b0
    7:2     R/W POW32K_DUMMY3                               6'b100000
    14:8    R/W POW32K_DUMMY2                               7'b0000000
    15      R/W POW32K_DUMMY1                               1'b0
 */
typedef union _AON_FAST_RG0X_POW_32K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t POW32K_32KOSC: 1;
        uint16_t POW32K_32KXTAL: 1;
        uint16_t POW32K_DUMMY3: 6;
        uint16_t POW32K_DUMMY2: 7;
        uint16_t POW32K_DUMMY1: 1;
    };
} AON_FAST_RG0X_POW_32K_TYPE;

/* 0x1490
    2:0     R/W XTAL_MODE                                   3'b100
    3       R/W XTAL_DEBUG                                  1'b0
    6:4     R/W XTAL_MODE_DEBUG                             3'b100
    15:7    R/W XTAL_MODE_DUMMY                             9'b0
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL_mode_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_MODE: 3;
        uint16_t XTAL_DEBUG: 1;
        uint16_t XTAL_MODE_DEBUG: 3;
        uint16_t XTAL_MODE_DUMMY: 9;
    };
} AON_FAST_BT_ANAPAR_XTAL_mode_TYPE;

/* 0x1492
    0       R/W XTAL_EN_XTAL_AAC_GM                         1'b0
    1       R/W XTAL_EN_XTAL_AAC_PKDET                      1'b0
    2       R/W XTAL_EN_XTAL_LPS                            1'b0
    3       R/W XTAL_GATED_XTAL_OK0                         1'b0
    6:4     R/W XTAL_AAC_PK_SEL                             3'b101
    10:7    R/W XTAL_AAC_PK_LP_SEL                          4'b0101
    15:11   R/W XTAL_BUF_GMN                                5'b01000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_EN_XTAL_AAC_GM: 1;
        uint16_t XTAL_EN_XTAL_AAC_PKDET: 1;
        uint16_t XTAL_EN_XTAL_LPS: 1;
        uint16_t XTAL_GATED_XTAL_OK0: 1;
        uint16_t XTAL_AAC_PK_SEL: 3;
        uint16_t XTAL_AAC_PK_LP_SEL: 4;
        uint16_t XTAL_BUF_GMN: 5;
    };
} AON_FAST_BT_ANAPAR_XTAL0_TYPE;

/* 0x1494
    4:0     R/W XTAL_BUF_GMN_LP                             5'b01000
    9:5     R/W XTAL_BUF_GMP                                5'b01000
    14:10   R/W XTAL_BUF_GMP_LP                             5'b01000
    15      R/W XTAL_BK_BG                                  1'b0
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_BUF_GMN_LP: 5;
        uint16_t XTAL_BUF_GMP: 5;
        uint16_t XTAL_BUF_GMP_LP: 5;
        uint16_t XTAL_BK_BG: 1;
    };
} AON_FAST_BT_ANAPAR_XTAL1_TYPE;

/* 0x1496
    0       R/W XTAL_DELAY_AFE                              1'b0
    1       R/W XTAL_DELAY_USB                              1'b0
    2       R/W XTAL_DOUBLE_OP_I                            1'b0
    4:3     R/W XTAL_DRV_AFE                                2'b11
    6:5     R/W XTAL_DRV_USB                                2'b11
    8:7     R/W XTAL_DRV_RF                                 2'b11
    9       R/W XTAL_DRV_RF_LATCH                           1'b0
    10      R/W XTAL_GATED_AFEP                             1'b0
    11      R/W XTAL_GATED_AFEN                             1'b0
    12      R/W XTAL_GATED_USBP                             1'b0
    13      R/W XTAL_GATED_USBN                             1'b0
    14      R/W XTAL_GATED_RFP                              1'b0
    15      R/W XTAL_GATED_RFN                              1'b0
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_DELAY_AFE: 1;
        uint16_t XTAL_DELAY_USB: 1;
        uint16_t XTAL_DOUBLE_OP_I: 1;
        uint16_t XTAL_DRV_AFE: 2;
        uint16_t XTAL_DRV_USB: 2;
        uint16_t XTAL_DRV_RF: 2;
        uint16_t XTAL_DRV_RF_LATCH: 1;
        uint16_t XTAL_GATED_AFEP: 1;
        uint16_t XTAL_GATED_AFEN: 1;
        uint16_t XTAL_GATED_USBP: 1;
        uint16_t XTAL_GATED_USBN: 1;
        uint16_t XTAL_GATED_RFP: 1;
        uint16_t XTAL_GATED_RFN: 1;
    };
} AON_FAST_BT_ANAPAR_XTAL2_TYPE;

/* 0x1498
    5:0     R/W XTAL_GM                                     6'b111111
    11:6    R/W XTAL_GM_LP                                  6'b111111
    12      R/W XTAL_GM_SEP                                 1'b0
    13      R/W XTAL_IDOUBLE                                1'b0
    14      R/W XTAL_IS_FINE_MANU                           1'b0
    15      R/W XTAL_SW_LDO2PWRCUT                          1'b0
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_GM: 6;
        uint16_t XTAL_GM_LP: 6;
        uint16_t XTAL_GM_SEP: 1;
        uint16_t XTAL_IDOUBLE: 1;
        uint16_t XTAL_IS_FINE_MANU: 1;
        uint16_t XTAL_SW_LDO2PWRCUT: 1;
    };
} AON_FAST_BT_ANAPAR_XTAL3_TYPE;

/* 0x149A
    5:0     R/W XTAL_GM_OK0                                 6'b111111
    11:6    R/W XTAL_IS                                     6'b111111
    15:12   R/W XTAL_REG4X_DUMMY1                           4'b0000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_GM_OK0: 6;
        uint16_t XTAL_IS: 6;
        uint16_t XTAL_REG4X_DUMMY1: 4;
    };
} AON_FAST_BT_ANAPAR_XTAL4_TYPE;

/* 0x149C
    7:0     R/W XTAL_IS_FINE                                8'b11111111
    13:8    R/W XTAL_IS_LP                                  6'b000000
    15:14   R/W XTAL_PKDET_LP_TSEL                          2'b01
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_IS_FINE: 8;
        uint16_t XTAL_IS_LP: 6;
        uint16_t XTAL_PKDET_LP_TSEL: 2;
    };
} AON_FAST_BT_ANAPAR_XTAL5_TYPE;

/* 0x149E
    2:0     R/W XTAL_LPS_CKMODE                             3'b110
    5:3     R/W XTAL_LDO                                    3'b011
    8:6     R/W XTAL_LDO_OK                                 3'b011
    11:9    R/W XTAL_LPM_CKO_SEL                            3'b011
    14:12   R/W XTAL_SEL_TOK                                3'b101
    15      R/W XTAL_reg_fast_settling                      1'b1
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_LPS_CKMODE: 3;
        uint16_t XTAL_LDO: 3;
        uint16_t XTAL_LDO_OK: 3;
        uint16_t XTAL_LPM_CKO_SEL: 3;
        uint16_t XTAL_SEL_TOK: 3;
        uint16_t XTAL_reg_fast_settling: 1;
    };
} AON_FAST_BT_ANAPAR_XTAL6_TYPE;

/* 0x14A0
    7:0     R/W XTAL_SC_XI                                  8'b00111111
    15:8    R/W XTAL_SC_XI_LP                               8'b11111111
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SC_XI: 8;
        uint16_t XTAL_SC_XI_LP: 8;
    };
} AON_FAST_BT_ANAPAR_XTAL7_TYPE;

/* 0x14A2
    7:0     R/W XTAL_SC_XI_OK0                              8'b11110000
    15:8    R/W XTAL_SC_XO                                  8'b00111111
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SC_XI_OK0: 8;
        uint16_t XTAL_SC_XO: 8;
    };
} AON_FAST_BT_ANAPAR_XTAL8_TYPE;

/* 0x14A4
    7:0     R/W XTAL_SC_XO_LP                               8'b11111111
    15:8    R/W XTAL_SC_XO_OK0                              8'b11110000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SC_XO_LP: 8;
        uint16_t XTAL_SC_XO_OK0: 8;
    };
} AON_FAST_BT_ANAPAR_XTAL9_TYPE;

/* 0x14A6
    2:0     R/W XTAL_XORES_SEL                              3'b000
    4:3     R/W XTAL_reg_stp                                2'b00
    8:5     R/W XTAL_reg_dout_offset                        4'b0011
    9       R/W XTAL_GATED_LPMODE                           1'b0
    10      R/W XTAL_GATED_EN_PEAKDET_LP                    1'b0
    11      R/W XTAL_LDOPC_SEL                              1'b0
    12      R/W XTAL_BUF_LPS_SEL                            1'b0
    13      R/W XTAL_LPMODE_CLK_SEL                         1'b0
    14      R/W XTAL_reg_fast_always_on                     1'b0
    15      R/W XTAL_LPMODE_CLK_AON                         1'b0
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_XORES_SEL: 3;
        uint16_t XTAL_reg_stp: 2;
        uint16_t XTAL_reg_dout_offset: 4;
        uint16_t XTAL_GATED_LPMODE: 1;
        uint16_t XTAL_GATED_EN_PEAKDET_LP: 1;
        uint16_t XTAL_LDOPC_SEL: 1;
        uint16_t XTAL_BUF_LPS_SEL: 1;
        uint16_t XTAL_LPMODE_CLK_SEL: 1;
        uint16_t XTAL_reg_fast_always_on: 1;
        uint16_t XTAL_LPMODE_CLK_AON: 1;
    };
} AON_FAST_BT_ANAPAR_XTAL10_TYPE;

/* 0x14A8
    0       R/W XTAL_IOP_SEL                                1'b0
    1       R/W XTAL_EN_XTAL_PDCK_LP                        1'b0
    4:2     R/W XTAL_LDO_SW_LP                              3'b000
    9:5     R/W XTAL_PDC_LP                                 5'b10000
    10      R/W XTAL_PDC_MANUAL                             1'b0
    11      R/W XTAL_PKDET_CMP_SWAP                         1'b0
    12      R/W XTAL_PKDET_LOAD_SWAP                        1'b0
    15:13   R/W XTAL_SEL_TOK01                              3'b100
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_IOP_SEL: 1;
        uint16_t XTAL_EN_XTAL_PDCK_LP: 1;
        uint16_t XTAL_LDO_SW_LP: 3;
        uint16_t XTAL_PDC_LP: 5;
        uint16_t XTAL_PDC_MANUAL: 1;
        uint16_t XTAL_PKDET_CMP_SWAP: 1;
        uint16_t XTAL_PKDET_LOAD_SWAP: 1;
        uint16_t XTAL_SEL_TOK01: 3;
    };
} AON_FAST_BT_ANAPAR_XTAL11_TYPE;

/* 0x14AA
    3:0     R/W XTAL_LPSCLK_CNTRL                           4'b0000
    4       R/W XTAL_EN_XTAL_SEL_TOK01                      1'b0
    5       R/W XTAL_EN_XO_CLK_SW                           1'b0
    6       R/W XTAL_FASTSET_MANU                           1'b0
    8:7     R/W XTAL_OV_RATIO                               2'b01
    11:9    R/W XTAL_OV_UNIT                                3'b000
    12      R/W XTAL_LPS_CAP_CTRL                           1'b1
    14:13   R/W XTAL_LPS_CAP_CYC                            2'b00
    15      R/W XTAL_MD_LPOW                                1'b0
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_LPSCLK_CNTRL: 4;
        uint16_t XTAL_EN_XTAL_SEL_TOK01: 1;
        uint16_t XTAL_EN_XO_CLK_SW: 1;
        uint16_t XTAL_FASTSET_MANU: 1;
        uint16_t XTAL_OV_RATIO: 2;
        uint16_t XTAL_OV_UNIT: 3;
        uint16_t XTAL_LPS_CAP_CTRL: 1;
        uint16_t XTAL_LPS_CAP_CYC: 2;
        uint16_t XTAL_MD_LPOW: 1;
    };
} AON_FAST_BT_ANAPAR_XTAL12_TYPE;

/* 0x14AC
    5:0     R/W XTAL_WAIT_CYC                               6'b000010
    7:6     R/W XTAL_LPS_CAP_STEP                           2'b01
    8       R/W XTAL_BYPASS_CTRL                            1'b0
    11:9    R/W XTAL_RDY_SEL_TOK                            3'b100
    15:12   R/W XTAL_CTRL_OUT_SEL                           4'b0000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_WAIT_CYC: 6;
        uint16_t XTAL_LPS_CAP_STEP: 2;
        uint16_t XTAL_BYPASS_CTRL: 1;
        uint16_t XTAL_RDY_SEL_TOK: 3;
        uint16_t XTAL_CTRL_OUT_SEL: 4;
    };
} AON_FAST_BT_ANAPAR_XTAL13_TYPE;

/* 0x14AE
    15:0    R/W XTAL_CTRL1                                  16'b0000000000000010
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_CTRL1;
    };
} AON_FAST_BT_ANAPAR_XTAL14_TYPE;

/* 0x14B0
    15:0    R/W XTAL_CTRL2                                  16'b0000000000000000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_CTRL2;
    };
} AON_FAST_BT_ANAPAR_XTAL15_TYPE;

/* 0x14B2
    15:0    R/W XTAL_REG16X_DUMMY1                          16'b0000000000000000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL16_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG16X_DUMMY1;
    };
} AON_FAST_BT_ANAPAR_XTAL16_TYPE;

/* 0x14B4
    15:0    R/W XTAL_REG17X_DUMMY1                          16'b0000000000000000
 */
typedef union _AON_FAST_BT_ANAPAR_XTAL17_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG17X_DUMMY1;
    };
} AON_FAST_BT_ANAPAR_XTAL17_TYPE;

/* 0x14F0
    10:0    R/W reserved                                    11'b00000000000
    14:11   R/W OSC40M_OSC_FSET[3:0]                        4'b1000
    15      R/W OSC40M_POW_OSC                              1'b1
 */
typedef union _AON_FAST_BT_ANAPAR_OSC40M_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reserved: 11;
        uint16_t OSC40M_OSC_FSET_3_0: 4;
        uint16_t OSC40M_POW_OSC: 1;
    };
} AON_FAST_BT_ANAPAR_OSC40M_TYPE;

/* 0x1510
    0       R/W AUXADC_POW_ADC                              1'b1
    1       R/W AUXADC_POW_REF                              1'b1
    2       R/W AUXADC_SEL_CLK                              1'b0
    3       R/W AUXADC_EN_CLK_DELAY                         1'b0
    5:4     R/W AUXADC_SEL_VREF                             2'b01
    6       R/W AUXADC_EN_CLK_REVERSE                       1'b0
    8:7     R/W AUXADC_SEL_CMPDEC                           2'b00
    9       R/W AUXADC_EN_META                              1'b0
    10      R/W AUXADC_EN_LN                                1'b0
    11      R/W AUXADC_EN_LNA                               1'b0
    13:12   R/W AUXADC_VCM_SEL                              2'b11
    15:14   R/W AUXADC_RG0X_DUMMY                           2'b00
 */
typedef union _AON_FAST_REG0X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_POW_ADC: 1;
        uint16_t AUXADC_POW_REF: 1;
        uint16_t AUXADC_SEL_CLK: 1;
        uint16_t AUXADC_EN_CLK_DELAY: 1;
        uint16_t AUXADC_SEL_VREF: 2;
        uint16_t AUXADC_EN_CLK_REVERSE: 1;
        uint16_t AUXADC_SEL_CMPDEC: 2;
        uint16_t AUXADC_EN_META: 1;
        uint16_t AUXADC_EN_LN: 1;
        uint16_t AUXADC_EN_LNA: 1;
        uint16_t AUXADC_VCM_SEL: 2;
        uint16_t AUXADC_RG0X_DUMMY: 2;
    };
} AON_FAST_REG0X_AUXADC_TYPE;

/* 0x1512
    3:0     R/W AUXADC_EN_BYPASS_MODE                       4'b0000
    7:4     R/W AUXADC_RG1X_DUMMY                           4'b0000
    10:8    R/W AUXADC_SEL_LDO09_REF                        3'b010
    11      R/W AUXADC_EN_LDO_VPULSE                        1'b1
    12      R/W AUXADC_SEL_LDO_MODE                         1'b1
    13      R/W AUXADC_DOUBLE_OP                            1'b0
    14      R/W AUXADC_EN_ILIMIT                            1'b0
    15      R/W AUXADC_EN_TG                                1'b0
 */
typedef union _AON_FAST_REG1X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_EN_BYPASS_MODE: 4;
        uint16_t AUXADC_RG1X_DUMMY: 4;
        uint16_t AUXADC_SEL_LDO09_REF: 3;
        uint16_t AUXADC_EN_LDO_VPULSE: 1;
        uint16_t AUXADC_SEL_LDO_MODE: 1;
        uint16_t AUXADC_DOUBLE_OP: 1;
        uint16_t AUXADC_EN_ILIMIT: 1;
        uint16_t AUXADC_EN_TG: 1;
    };
} AON_FAST_REG1X_AUXADC_TYPE;

/* 0x1514
    0       R/W AUXADC_EN_LDO33                             1'b1
    2:1     R/W AUXADC_SEL_LDO33_REF                        2'b10
    3       R/W AUXADC_EN_LDO09                             1'b1
    4       R/W AUXADC_POW_SD                               1'b1
    5       R/W AUXADC_EN_SD_POSEDGE                        1'b0
    8:6     R/W AUXADC_SEL_SD_CH                            3'b000
    14:9    R/W AUXADC_MBIAS_SEL                            6'b000000
    15      R/W AUXADC_EN_DIODE                             1'b1
 */
typedef union _AON_FAST_REG2X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_EN_LDO33: 1;
        uint16_t AUXADC_SEL_LDO33_REF: 2;
        uint16_t AUXADC_EN_LDO09: 1;
        uint16_t AUXADC_POW_SD: 1;
        uint16_t AUXADC_EN_SD_POSEDGE: 1;
        uint16_t AUXADC_SEL_SD_CH: 3;
        uint16_t AUXADC_MBIAS_SEL: 6;
        uint16_t AUXADC_EN_DIODE: 1;
    };
} AON_FAST_REG2X_AUXADC_TYPE;

/* 0x1516
    3:0     R/W AUXADC_PAD_E                                4'b0000
    7:4     R/W AUXADC_RG3X_DUMMY2                          4'b0000
    11:8    R/W AUXADC_PAD_E2                               4'b0000
    15:12   R/W AUXADC_RG3X_DUMMY1                          4'b0000
 */
typedef union _AON_FAST_REG3X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_PAD_E: 4;
        uint16_t AUXADC_RG3X_DUMMY2: 4;
        uint16_t AUXADC_PAD_E2: 4;
        uint16_t AUXADC_RG3X_DUMMY1: 4;
    };
} AON_FAST_REG3X_AUXADC_TYPE;

/* 0x1518
    3:0     R/W AUXADC_PAD_PD                               4'b0000
    7:4     R/W AUXADC_RG4X_DUMMY2                          4'b0000
    11:8    R/W AUXADC_PAD_PU                               4'b0000
    15:12   R/W AUXADC_RG4X_DUMMY1                          4'b0000
 */
typedef union _AON_FAST_REG4X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_PAD_PD: 4;
        uint16_t AUXADC_RG4X_DUMMY2: 4;
        uint16_t AUXADC_PAD_PU: 4;
        uint16_t AUXADC_RG4X_DUMMY1: 4;
    };
} AON_FAST_REG4X_AUXADC_TYPE;

/* 0x151A
    3:0     R/W AUXADC_PAD_SHDN                             4'b1111
    7:4     R/W AUXADC_RG5X_DUMMY2                          4'b0000
    11:8    R/W AUXADC_PAD_SMT                              4'b1111
    15:12   R/W AUXADC_RG5X_DUMMY1                          4'b0000
 */
typedef union _AON_FAST_REG5X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_PAD_SHDN: 4;
        uint16_t AUXADC_RG5X_DUMMY2: 4;
        uint16_t AUXADC_PAD_SMT: 4;
        uint16_t AUXADC_RG5X_DUMMY1: 4;
    };
} AON_FAST_REG5X_AUXADC_TYPE;

/* 0x151C
    3:0     R/W AUXADC_PAD_I                                4'b0000
    7:4     R/W AUXADC_RG6X_DUMMY2                          4'b0000
    11:8    R/W AUXADC_PAD_PUPDC                            4'b0000
    15:12   R/W AUXADC_RG6X_H3L1                            4'b0000
 */
typedef union _AON_FAST_REG6X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_PAD_I: 4;
        uint16_t AUXADC_RG6X_DUMMY2: 4;
        uint16_t AUXADC_PAD_PUPDC: 4;
        uint16_t AUXADC_RG6X_H3L1: 4;
    };
} AON_FAST_REG6X_AUXADC_TYPE;

/* 0x1530
    2:0     R/W RG0X_CODEC_LDO_DUMMY0                       2'b00
    3       R/W CODEC_L2L_EN                                1'b0
    4       R/W CODEC_LDO_LV_POW                            1'b0
    8:5     R/W CODEC_LDO_TUNE                              4'b0
    9       R/W RG0X_CODEC_LDO_DUMMY9                       1'b0
    10      R/W RG0X_CODEC_LDO_DUMMY10                      1'b0
    11      R/W CODEC_LDO_POW                               1'b0
    12      R/W RG0X_CODEC_LDO_DUMMY12                      1'b0
    13      R/W RG0X_CODEC_LDO_DUMMY13                      1'b0
    14      R/W CODEC_LDO_COMP_INT                          1'b0
    15      R/W RG0X_CODEC_LDO_DUMMY15                      1'b0
 */
typedef union _AON_FAST_RG0X_CODEC_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RG0X_CODEC_LDO_DUMMY0: 3;
        uint16_t CODEC_L2L_EN: 1;
        uint16_t CODEC_LDO_LV_POW: 1;
        uint16_t CODEC_LDO_TUNE: 4;
        uint16_t RG0X_CODEC_LDO_DUMMY9: 1;
        uint16_t RG0X_CODEC_LDO_DUMMY10: 1;
        uint16_t CODEC_LDO_POW: 1;
        uint16_t RG0X_CODEC_LDO_DUMMY12: 1;
        uint16_t RG0X_CODEC_LDO_DUMMY13: 1;
        uint16_t CODEC_LDO_COMP_INT: 1;
        uint16_t RG0X_CODEC_LDO_DUMMY15: 1;
    };
} AON_FAST_RG0X_CODEC_LDO_TYPE;

/* 0x1540
    7:0     R/W BTADDA_LDO_REG0X_DUMMY0                     8'h0
    8       R/W BTADDA_LDO_SW_LDO2PWRCUT                    1'b0
    9       R/W BTADDA_LDO_POW_LDO_VREF                     1'b1
    10      R/W BTADDA_LDO_POW_LDO_OP                       1'b1
    11      R/W BTADDA_LDO_LDO_VPULSE                       1'b0
    14:12   R/W BTADDA_LDO_LDO_SW                           3'b010
    15      R/W BTADDA_LDO_DOUBLE_OP_I                      1'b0
 */
typedef union _AON_FAST_REG0X_BTADDA_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTADDA_LDO_REG0X_DUMMY0: 8;
        uint16_t BTADDA_LDO_SW_LDO2PWRCUT: 1;
        uint16_t BTADDA_LDO_POW_LDO_VREF: 1;
        uint16_t BTADDA_LDO_POW_LDO_OP: 1;
        uint16_t BTADDA_LDO_LDO_VPULSE: 1;
        uint16_t BTADDA_LDO_LDO_SW: 3;
        uint16_t BTADDA_LDO_DOUBLE_OP_I: 1;
    };
} AON_FAST_REG0X_BTADDA_LDO_TYPE;

/* 0x1550
    7:0     R/W REG0X_USB_DUMMY0                            8'h0
    8       R/W USB_PMUIB_SEL                               1'b1
    9       R/W UA33PC_EN                                   1'b0
    10      R/W USB2_ANA_EN                                 1'b0
    11      R/W USB_POW_LDO                                 1'b0
    12      R/W USB_POW_BG                                  1'b0
    13      R/W USB_POW_L                                   1'b0
    14      R/W ISO_LV                                      1'b1
    15      R/W BY_PASS_ON                                  1'b1
 */
typedef union _AON_FAST_REG0X_USB_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_USB_DUMMY0: 8;
        uint16_t USB_PMUIB_SEL: 1;
        uint16_t UA33PC_EN: 1;
        uint16_t USB2_ANA_EN: 1;
        uint16_t USB_POW_LDO: 1;
        uint16_t USB_POW_BG: 1;
        uint16_t USB_POW_L: 1;
        uint16_t ISO_LV: 1;
        uint16_t BY_PASS_ON: 1;
    };
} AON_FAST_REG0X_USB_TYPE;

/* 0x1560
    0       R/W REG0X_PAD_ADC_0_DUMMY0                      1'b0
    1       R/W REG0X_PAD_ADC_0_DUMMY1                      1'b0
    2       R/W PAD_ADC_WKUP_INT_EN[0]                      1'b0
    3       R/W REG0X_PAD_ADC_0_DUMMY3                      1'b1
    4       R/W PAD_ADC_S[0]                                1'b0
    5       R/W PAD_ADC_SMT[0]                              1'b0
    6       R/W REG0X_PAD_ADC_0_DUMMY6                      1'b0
    7       R/W PAD_ADC_E2[0]                               1'b0
    8       R/W PAD_ADC_SHDN[0]                             1'b1
    9       R/W AON_PAD_ADC_E[0]                            1'b0
    10      R/W PAD_ADC_WKPOL[0]                            1'b0
    11      R/W PAD_ADC_WKEN[0]                             1'b0
    12      R/W AON_PAD_ADC_O[0]                            1'b0
    13      R/W PAD_ADC_PUPDC[0]                            1'b0
    14      R/W PAD_ADC_PU[0]                               1'b0
    15      R/W PAD_ADC_PU_EN[0]                            1'b1
 */
typedef union _AON_FAST_REG0X_PAD_ADC_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_ADC_0_DUMMY0: 1;
        uint16_t REG0X_PAD_ADC_0_DUMMY1: 1;
        uint16_t PAD_ADC_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_ADC_0_DUMMY3: 1;
        uint16_t PAD_ADC_S_0: 1;
        uint16_t PAD_ADC_SMT_0: 1;
        uint16_t REG0X_PAD_ADC_0_DUMMY6: 1;
        uint16_t PAD_ADC_E2_0: 1;
        uint16_t PAD_ADC_SHDN_0: 1;
        uint16_t AON_PAD_ADC_E_0: 1;
        uint16_t PAD_ADC_WKPOL_0: 1;
        uint16_t PAD_ADC_WKEN_0: 1;
        uint16_t AON_PAD_ADC_O_0: 1;
        uint16_t PAD_ADC_PUPDC_0: 1;
        uint16_t PAD_ADC_PU_0: 1;
        uint16_t PAD_ADC_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_ADC_0_TYPE;

/* 0x1562
    0       R/W REG1X_PAD_ADC_1_DUMMY0                      1'b0
    1       R/W REG1X_PAD_ADC_1_DUMMY1                      1'b0
    2       R/W PAD_ADC_WKUP_INT_EN[1]                      1'b0
    3       R/W REG1X_PAD_ADC_1_DUMMY3                      1'b1
    4       R/W PAD_ADC_S[1]                                1'b0
    5       R/W PAD_ADC_SMT[1]                              1'b0
    6       R/W REG1X_PAD_ADC_1_DUMMY6                      1'b0
    7       R/W PAD_ADC_E2[1]                               1'b0
    8       R/W PAD_ADC_SHDN[1]                             1'b1
    9       R/W AON_PAD_ADC_E[1]                            1'b0
    10      R/W PAD_ADC_WKPOL[1]                            1'b0
    11      R/W PAD_ADC_WKEN[1]                             1'b0
    12      R/W AON_PAD_ADC_O[1]                            1'b0
    13      R/W PAD_ADC_PUPDC[1]                            1'b0
    14      R/W PAD_ADC_PU[1]                               1'b0
    15      R/W PAD_ADC_PU_EN[1]                            1'b1
 */
typedef union _AON_FAST_REG1X_PAD_ADC_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG1X_PAD_ADC_1_DUMMY0: 1;
        uint16_t REG1X_PAD_ADC_1_DUMMY1: 1;
        uint16_t PAD_ADC_WKUP_INT_EN_1: 1;
        uint16_t REG1X_PAD_ADC_1_DUMMY3: 1;
        uint16_t PAD_ADC_S_1: 1;
        uint16_t PAD_ADC_SMT_1: 1;
        uint16_t REG1X_PAD_ADC_1_DUMMY6: 1;
        uint16_t PAD_ADC_E2_1: 1;
        uint16_t PAD_ADC_SHDN_1: 1;
        uint16_t AON_PAD_ADC_E_1: 1;
        uint16_t PAD_ADC_WKPOL_1: 1;
        uint16_t PAD_ADC_WKEN_1: 1;
        uint16_t AON_PAD_ADC_O_1: 1;
        uint16_t PAD_ADC_PUPDC_1: 1;
        uint16_t PAD_ADC_PU_1: 1;
        uint16_t PAD_ADC_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_ADC_1_TYPE;

/* 0x1564
    0       R/W REG2X_PAD_ADC_2_DUMMY0                      1'b0
    1       R/W REG2X_PAD_ADC_2_DUMMY1                      1'b0
    2       R/W PAD_ADC_WKUP_INT_EN[2]                      1'b0
    3       R/W REG2X_PAD_ADC_2_DUMMY3                      1'b1
    4       R/W PAD_ADC_S[2]                                1'b0
    5       R/W PAD_ADC_SMT[2]                              1'b0
    6       R/W REG2X_PAD_ADC_2_DUMMY6                      1'b0
    7       R/W PAD_ADC_E2[2]                               1'b0
    8       R/W PAD_ADC_SHDN[2]                             1'b1
    9       R/W AON_PAD_ADC_E[2]                            1'b0
    10      R/W PAD_ADC_WKPOL[2]                            1'b0
    11      R/W PAD_ADC_WKEN[2]                             1'b0
    12      R/W AON_PAD_ADC_O[2]                            1'b0
    13      R/W PAD_ADC_PUPDC[2]                            1'b0
    14      R/W PAD_ADC_PU[2]                               1'b0
    15      R/W PAD_ADC_PU_EN[2]                            1'b1
 */
typedef union _AON_FAST_REG2X_PAD_ADC_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG2X_PAD_ADC_2_DUMMY0: 1;
        uint16_t REG2X_PAD_ADC_2_DUMMY1: 1;
        uint16_t PAD_ADC_WKUP_INT_EN_2: 1;
        uint16_t REG2X_PAD_ADC_2_DUMMY3: 1;
        uint16_t PAD_ADC_S_2: 1;
        uint16_t PAD_ADC_SMT_2: 1;
        uint16_t REG2X_PAD_ADC_2_DUMMY6: 1;
        uint16_t PAD_ADC_E2_2: 1;
        uint16_t PAD_ADC_SHDN_2: 1;
        uint16_t AON_PAD_ADC_E_2: 1;
        uint16_t PAD_ADC_WKPOL_2: 1;
        uint16_t PAD_ADC_WKEN_2: 1;
        uint16_t AON_PAD_ADC_O_2: 1;
        uint16_t PAD_ADC_PUPDC_2: 1;
        uint16_t PAD_ADC_PU_2: 1;
        uint16_t PAD_ADC_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_ADC_2_TYPE;

/* 0x1566
    0       R/W REG3X_PAD_ADC_3_DUMMY0                      1'b0
    1       R/W REG3X_PAD_ADC_3_DUMMY1                      1'b0
    2       R/W PAD_ADC_WKUP_INT_EN[3]                      1'b0
    3       R/W REG3X_PAD_ADC_3_DUMMY3                      1'b1
    4       R/W PAD_ADC_S[3]                                1'b0
    5       R/W PAD_ADC_SMT[3]                              1'b0
    6       R/W REG3X_PAD_ADC_3_DUMMY6                      1'b0
    7       R/W PAD_ADC_E2[3]                               1'b0
    8       R/W PAD_ADC_SHDN[3]                             1'b1
    9       R/W AON_PAD_ADC_E[3]                            1'b0
    10      R/W PAD_ADC_WKPOL[3]                            1'b0
    11      R/W PAD_ADC_WKEN[3]                             1'b0
    12      R/W AON_PAD_ADC_O[3]                            1'b0
    13      R/W PAD_ADC_PUPDC[3]                            1'b0
    14      R/W PAD_ADC_PU[3]                               1'b0
    15      R/W PAD_ADC_PU_EN[3]                            1'b1
 */
typedef union _AON_FAST_REG3X_PAD_ADC_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG3X_PAD_ADC_3_DUMMY0: 1;
        uint16_t REG3X_PAD_ADC_3_DUMMY1: 1;
        uint16_t PAD_ADC_WKUP_INT_EN_3: 1;
        uint16_t REG3X_PAD_ADC_3_DUMMY3: 1;
        uint16_t PAD_ADC_S_3: 1;
        uint16_t PAD_ADC_SMT_3: 1;
        uint16_t REG3X_PAD_ADC_3_DUMMY6: 1;
        uint16_t PAD_ADC_E2_3: 1;
        uint16_t PAD_ADC_SHDN_3: 1;
        uint16_t AON_PAD_ADC_E_3: 1;
        uint16_t PAD_ADC_WKPOL_3: 1;
        uint16_t PAD_ADC_WKEN_3: 1;
        uint16_t AON_PAD_ADC_O_3: 1;
        uint16_t PAD_ADC_PUPDC_3: 1;
        uint16_t PAD_ADC_PU_3: 1;
        uint16_t PAD_ADC_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_ADC_3_TYPE;

/* 0x1568
    0       R/W REG0X_PAD_P1_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_0_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P1_0_DUMMY3                       1'b1
    4       R/W PAD_P1_S[0]                                 1'b0
    5       R/W PAD_P1_SMT[0]                               1'b0
    6       R/W PAD_P1_E3[0]                                1'b0
    7       R/W PAD_P1_E2[0]                                1'b0
    8       R/W PAD_P1_SHDN[0]                              1'b1
    9       R/W AON_PAD_P1_E[0]                             1'b0
    10      R/W PAD_P1_WKPOL[0]                             1'b0
    11      R/W PAD_P1_WKEN[0]                              1'b0
    12      R/W AON_PAD_P1_O[0]                             1'b0
    13      R/W PAD_P1_PUPDC[0]                             1'b0
    14      R/W PAD_P1_PU[0]                                1'b0
    15      R/W PAD_P1_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P1_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_0_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P1_0_DUMMY3: 1;
        uint16_t PAD_P1_S_0: 1;
        uint16_t PAD_P1_SMT_0: 1;
        uint16_t PAD_P1_E3_0: 1;
        uint16_t PAD_P1_E2_0: 1;
        uint16_t PAD_P1_SHDN_0: 1;
        uint16_t AON_PAD_P1_E_0: 1;
        uint16_t PAD_P1_WKPOL_0: 1;
        uint16_t PAD_P1_WKEN_0: 1;
        uint16_t AON_PAD_P1_O_0: 1;
        uint16_t PAD_P1_PUPDC_0: 1;
        uint16_t PAD_P1_PU_0: 1;
        uint16_t PAD_P1_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P1_0_TYPE;

/* 0x156A
    0       R/W REG0X_PAD_P1_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_1_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P1_1_DUMMY3                       1'b1
    4       R/W PAD_P1_S[1]                                 1'b0
    5       R/W PAD_P1_SMT[1]                               1'b0
    6       R/W PAD_P1_E3[1]                                1'b0
    7       R/W PAD_P1_E2[1]                                1'b0
    8       R/W PAD_P1_SHDN[1]                              1'b1
    9       R/W AON_PAD_P1_E[1]                             1'b0
    10      R/W PAD_P1_WKPOL[1]                             1'b0
    11      R/W PAD_P1_WKEN[1]                              1'b0
    12      R/W AON_PAD_P1_O[1]                             1'b0
    13      R/W PAD_P1_PUPDC[1]                             1'b0
    14      R/W PAD_P1_PU[1]                                1'b0
    15      R/W PAD_P1_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_1_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P1_1_DUMMY3: 1;
        uint16_t PAD_P1_S_1: 1;
        uint16_t PAD_P1_SMT_1: 1;
        uint16_t PAD_P1_E3_1: 1;
        uint16_t PAD_P1_E2_1: 1;
        uint16_t PAD_P1_SHDN_1: 1;
        uint16_t AON_PAD_P1_E_1: 1;
        uint16_t PAD_P1_WKPOL_1: 1;
        uint16_t PAD_P1_WKEN_1: 1;
        uint16_t AON_PAD_P1_O_1: 1;
        uint16_t PAD_P1_PUPDC_1: 1;
        uint16_t PAD_P1_PU_1: 1;
        uint16_t PAD_P1_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P1_1_TYPE;

/* 0x156C
    0       R/W REG0X_PAD_P1_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_2_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P1_2_DUMMY3                       1'b1
    4       R/W PAD_P1_S[2]                                 1'b0
    5       R/W PAD_P1_SMT[2]                               1'b0
    6       R/W PAD_P1_E3[2]                                1'b0
    7       R/W PAD_P1_E2[2]                                1'b0
    8       R/W PAD_P1_SHDN[2]                              1'b1
    9       R/W AON_PAD_P1_E[2]                             1'b0
    10      R/W PAD_P1_WKPOL[2]                             1'b0
    11      R/W PAD_P1_WKEN[2]                              1'b0
    12      R/W AON_PAD_P1_O[2]                             1'b0
    13      R/W PAD_P1_PUPDC[2]                             1'b0
    14      R/W PAD_P1_PU[2]                                1'b0
    15      R/W PAD_P1_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_2_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P1_2_DUMMY3: 1;
        uint16_t PAD_P1_S_2: 1;
        uint16_t PAD_P1_SMT_2: 1;
        uint16_t PAD_P1_E3_2: 1;
        uint16_t PAD_P1_E2_2: 1;
        uint16_t PAD_P1_SHDN_2: 1;
        uint16_t AON_PAD_P1_E_2: 1;
        uint16_t PAD_P1_WKPOL_2: 1;
        uint16_t PAD_P1_WKEN_2: 1;
        uint16_t AON_PAD_P1_O_2: 1;
        uint16_t PAD_P1_PUPDC_2: 1;
        uint16_t PAD_P1_PU_2: 1;
        uint16_t PAD_P1_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P1_2_TYPE;

/* 0x156E
    0       R/W REG0X_PAD_P1_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_3_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P1_3_DUMMY3                       1'b1
    4       R/W PAD_P1_S[3]                                 1'b0
    5       R/W PAD_P1_SMT[3]                               1'b0
    6       R/W PAD_P1_E3[3]                                1'b0
    7       R/W PAD_P1_E2[3]                                1'b0
    8       R/W PAD_P1_SHDN[3]                              1'b1
    9       R/W AON_PAD_P1_E[3]                             1'b0
    10      R/W PAD_P1_WKPOL[3]                             1'b0
    11      R/W PAD_P1_WKEN[3]                              1'b0
    12      R/W AON_PAD_P1_O[3]                             1'b0
    13      R/W PAD_P1_PUPDC[3]                             1'b0
    14      R/W PAD_P1_PU[3]                                1'b0
    15      R/W PAD_P1_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_3_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P1_3_DUMMY3: 1;
        uint16_t PAD_P1_S_3: 1;
        uint16_t PAD_P1_SMT_3: 1;
        uint16_t PAD_P1_E3_3: 1;
        uint16_t PAD_P1_E2_3: 1;
        uint16_t PAD_P1_SHDN_3: 1;
        uint16_t AON_PAD_P1_E_3: 1;
        uint16_t PAD_P1_WKPOL_3: 1;
        uint16_t PAD_P1_WKEN_3: 1;
        uint16_t AON_PAD_P1_O_3: 1;
        uint16_t PAD_P1_PUPDC_3: 1;
        uint16_t PAD_P1_PU_3: 1;
        uint16_t PAD_P1_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P1_3_TYPE;

/* 0x1570
    0       R/W REG0X_PAD_P1_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_4_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P1_4_DUMMY3                       1'b1
    4       R/W PAD_P1_S[4]                                 1'b0
    5       R/W PAD_P1_SMT[4]                               1'b0
    6       R/W PAD_P1_E3[4]                                1'b0
    7       R/W PAD_P1_E2[4]                                1'b0
    8       R/W PAD_P1_SHDN[4]                              1'b1
    9       R/W AON_PAD_P1_E[4]                             1'b0
    10      R/W PAD_P1_WKPOL[4]                             1'b0
    11      R/W PAD_P1_WKEN[4]                              1'b0
    12      R/W AON_PAD_P1_O[4]                             1'b0
    13      R/W PAD_P1_PUPDC[4]                             1'b0
    14      R/W PAD_P1_PU[4]                                1'b0
    15      R/W PAD_P1_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_4_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P1_4_DUMMY3: 1;
        uint16_t PAD_P1_S_4: 1;
        uint16_t PAD_P1_SMT_4: 1;
        uint16_t PAD_P1_E3_4: 1;
        uint16_t PAD_P1_E2_4: 1;
        uint16_t PAD_P1_SHDN_4: 1;
        uint16_t AON_PAD_P1_E_4: 1;
        uint16_t PAD_P1_WKPOL_4: 1;
        uint16_t PAD_P1_WKEN_4: 1;
        uint16_t AON_PAD_P1_O_4: 1;
        uint16_t PAD_P1_PUPDC_4: 1;
        uint16_t PAD_P1_PU_4: 1;
        uint16_t PAD_P1_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P1_4_TYPE;

/* 0x1572
    0       R/W REG0X_PAD_P1_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_5_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P1_5_DUMMY3                       1'b1
    4       R/W PAD_P1_S[5]                                 1'b0
    5       R/W PAD_P1_SMT[5]                               1'b0
    6       R/W PAD_P1_E3[5]                                1'b0
    7       R/W PAD_P1_E2[5]                                1'b0
    8       R/W PAD_P1_SHDN[5]                              1'b1
    9       R/W AON_PAD_P1_E[5]                             1'b0
    10      R/W PAD_P1_WKPOL[5]                             1'b0
    11      R/W PAD_P1_WKEN[5]                              1'b0
    12      R/W AON_PAD_P1_O[5]                             1'b0
    13      R/W PAD_P1_PUPDC[5]                             1'b0
    14      R/W PAD_P1_PU[5]                                1'b0
    15      R/W PAD_P1_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_5_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P1_5_DUMMY3: 1;
        uint16_t PAD_P1_S_5: 1;
        uint16_t PAD_P1_SMT_5: 1;
        uint16_t PAD_P1_E3_5: 1;
        uint16_t PAD_P1_E2_5: 1;
        uint16_t PAD_P1_SHDN_5: 1;
        uint16_t AON_PAD_P1_E_5: 1;
        uint16_t PAD_P1_WKPOL_5: 1;
        uint16_t PAD_P1_WKEN_5: 1;
        uint16_t AON_PAD_P1_O_5: 1;
        uint16_t PAD_P1_PUPDC_5: 1;
        uint16_t PAD_P1_PU_5: 1;
        uint16_t PAD_P1_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P1_5_TYPE;

/* 0x1574
    0       R/W REG0X_PAD_P1_6_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_6_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[6]                       1'b0
    3       R/W REG0X_PAD_P1_6_DUMMY3                       1'b1
    4       R/W PAD_P1_S[6]                                 1'b0
    5       R/W PAD_P1_SMT[6]                               1'b0
    6       R/W PAD_P1_E3[6]                                1'b0
    7       R/W PAD_P1_E2[6]                                1'b0
    8       R/W PAD_P1_SHDN[6]                              1'b1
    9       R/W AON_PAD_P1_E[6]                             1'b0
    10      R/W PAD_P1_WKPOL[6]                             1'b0
    11      R/W PAD_P1_WKEN[6]                              1'b0
    12      R/W AON_PAD_P1_O[6]                             1'b0
    13      R/W PAD_P1_PUPDC[6]                             1'b0
    14      R/W PAD_P1_PU[6]                                1'b0
    15      R/W PAD_P1_PU_EN[6]                             1'b1
 */
typedef union _AON_FAST_REG6X_PAD_P1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_6_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_6_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_6: 1;
        uint16_t REG0X_PAD_P1_6_DUMMY3: 1;
        uint16_t PAD_P1_S_6: 1;
        uint16_t PAD_P1_SMT_6: 1;
        uint16_t PAD_P1_E3_6: 1;
        uint16_t PAD_P1_E2_6: 1;
        uint16_t PAD_P1_SHDN_6: 1;
        uint16_t AON_PAD_P1_E_6: 1;
        uint16_t PAD_P1_WKPOL_6: 1;
        uint16_t PAD_P1_WKEN_6: 1;
        uint16_t AON_PAD_P1_O_6: 1;
        uint16_t PAD_P1_PUPDC_6: 1;
        uint16_t PAD_P1_PU_6: 1;
        uint16_t PAD_P1_PU_EN_6: 1;
    };
} AON_FAST_REG6X_PAD_P1_6_TYPE;

/* 0x1576
    0       R/W REG0X_PAD_P1_7_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P1_7_DUMMY1                       1'b0
    2       R/W PAD_P1_WKUP_INT_EN[7]                       1'b0
    3       R/W REG0X_PAD_P1_7_DUMMY3                       1'b1
    4       R/W PAD_P1_S[7]                                 1'b0
    5       R/W PAD_P1_SMT[7]                               1'b0
    6       R/W PAD_P1_E3[7]                                1'b0
    7       R/W PAD_P1_E2[7]                                1'b0
    8       R/W PAD_P1_SHDN[7]                              1'b1
    9       R/W AON_PAD_P1_E[7]                             1'b0
    10      R/W PAD_P1_WKPOL[7]                             1'b0
    11      R/W PAD_P1_WKEN[7]                              1'b0
    12      R/W AON_PAD_P1_O[7]                             1'b0
    13      R/W PAD_P1_PUPDC[7]                             1'b0
    14      R/W PAD_P1_PU[7]                                1'b0
    15      R/W PAD_P1_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_REG7X_PAD_P1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P1_7_DUMMY0: 1;
        uint16_t REG0X_PAD_P1_7_DUMMY1: 1;
        uint16_t PAD_P1_WKUP_INT_EN_7: 1;
        uint16_t REG0X_PAD_P1_7_DUMMY3: 1;
        uint16_t PAD_P1_S_7: 1;
        uint16_t PAD_P1_SMT_7: 1;
        uint16_t PAD_P1_E3_7: 1;
        uint16_t PAD_P1_E2_7: 1;
        uint16_t PAD_P1_SHDN_7: 1;
        uint16_t AON_PAD_P1_E_7: 1;
        uint16_t PAD_P1_WKPOL_7: 1;
        uint16_t PAD_P1_WKEN_7: 1;
        uint16_t AON_PAD_P1_O_7: 1;
        uint16_t PAD_P1_PUPDC_7: 1;
        uint16_t PAD_P1_PU_7: 1;
        uint16_t PAD_P1_PU_EN_7: 1;
    };
} AON_FAST_REG7X_PAD_P1_7_TYPE;

/* 0x1578
    0       R/W REG0X_PAD_P2_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_0_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P2_0_DUMMY3                       1'b1
    4       R/W PAD_P2_S[0]                                 1'b0
    5       R/W PAD_P2_SMT[0]                               1'b0
    6       R/W REG0X_PAD_P2_0_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[0]                                1'b0
    8       R/W PAD_P2_SHDN[0]                              1'b1
    9       R/W AON_PAD_P2_E[0]                             1'b0
    10      R/W PAD_P2_WKPOL[0]                             1'b0
    11      R/W PAD_P2_WKEN[0]                              1'b0
    12      R/W AON_PAD_P2_O[0]                             1'b0
    13      R/W PAD_P2_PUPDC[0]                             1'b0
    14      R/W PAD_P2_PU[0]                                1'b1
    15      R/W PAD_P2_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P2_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_0_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P2_0_DUMMY3: 1;
        uint16_t PAD_P2_S_0: 1;
        uint16_t PAD_P2_SMT_0: 1;
        uint16_t REG0X_PAD_P2_0_DUMMY6: 1;
        uint16_t PAD_P2_E2_0: 1;
        uint16_t PAD_P2_SHDN_0: 1;
        uint16_t AON_PAD_P2_E_0: 1;
        uint16_t PAD_P2_WKPOL_0: 1;
        uint16_t PAD_P2_WKEN_0: 1;
        uint16_t AON_PAD_P2_O_0: 1;
        uint16_t PAD_P2_PUPDC_0: 1;
        uint16_t PAD_P2_PU_0: 1;
        uint16_t PAD_P2_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P2_0_TYPE;

/* 0x157A
    0       R/W REG0X_PAD_P2_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_1_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P2_1_DUMMY3                       1'b1
    4       R/W PAD_P2_S[1]                                 1'b0
    5       R/W PAD_P2_SMT[1]                               1'b0
    6       R/W REG0X_PAD_P2_1_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[1]                                1'b0
    8       R/W PAD_P2_SHDN[1]                              1'b1
    9       R/W AON_PAD_P2_E[1]                             1'b0
    10      R/W PAD_P2_WKPOL[1]                             1'b0
    11      R/W PAD_P2_WKEN[1]                              1'b0
    12      R/W AON_PAD_P2_O[1]                             1'b0
    13      R/W PAD_P2_PUPDC[1]                             1'b0
    14      R/W PAD_P2_PU[1]                                1'b0
    15      R/W PAD_P2_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P2_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_1_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P2_1_DUMMY3: 1;
        uint16_t PAD_P2_S_1: 1;
        uint16_t PAD_P2_SMT_1: 1;
        uint16_t REG0X_PAD_P2_1_DUMMY6: 1;
        uint16_t PAD_P2_E2_1: 1;
        uint16_t PAD_P2_SHDN_1: 1;
        uint16_t AON_PAD_P2_E_1: 1;
        uint16_t PAD_P2_WKPOL_1: 1;
        uint16_t PAD_P2_WKEN_1: 1;
        uint16_t AON_PAD_P2_O_1: 1;
        uint16_t PAD_P2_PUPDC_1: 1;
        uint16_t PAD_P2_PU_1: 1;
        uint16_t PAD_P2_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P2_1_TYPE;

/* 0x157C
    0       R/W REG0X_PAD_P2_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_2_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P2_2_DUMMY3                       1'b1
    4       R/W PAD_P2_S[2]                                 1'b0
    5       R/W PAD_P2_SMT[2]                               1'b0
    6       R/W REG0X_PAD_P2_2_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[2]                                1'b0
    8       R/W PAD_P2_SHDN[2]                              1'b1
    9       R/W AON_PAD_P2_E[2]                             1'b0
    10      R/W PAD_P2_WKPOL[2]                             1'b0
    11      R/W PAD_P2_WKEN[2]                              1'b0
    12      R/W AON_PAD_P2_O[2]                             1'b0
    13      R/W PAD_P2_PUPDC[2]                             1'b0
    14      R/W PAD_P2_PU[2]                                1'b0
    15      R/W PAD_P2_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P2_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_2_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P2_2_DUMMY3: 1;
        uint16_t PAD_P2_S_2: 1;
        uint16_t PAD_P2_SMT_2: 1;
        uint16_t REG0X_PAD_P2_2_DUMMY6: 1;
        uint16_t PAD_P2_E2_2: 1;
        uint16_t PAD_P2_SHDN_2: 1;
        uint16_t AON_PAD_P2_E_2: 1;
        uint16_t PAD_P2_WKPOL_2: 1;
        uint16_t PAD_P2_WKEN_2: 1;
        uint16_t AON_PAD_P2_O_2: 1;
        uint16_t PAD_P2_PUPDC_2: 1;
        uint16_t PAD_P2_PU_2: 1;
        uint16_t PAD_P2_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P2_2_TYPE;

/* 0x157E
    0       R/W REG0X_PAD_P2_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_3_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P2_3_DUMMY3                       1'b1
    4       R/W PAD_P2_S[3]                                 1'b0
    5       R/W PAD_P2_SMT[3]                               1'b0
    6       R/W REG0X_PAD_P2_3_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[3]                                1'b0
    8       R/W PAD_P2_SHDN[3]                              1'b1
    9       R/W AON_PAD_P2_E[3]                             1'b0
    10      R/W PAD_P2_WKPOL[3]                             1'b0
    11      R/W PAD_P2_WKEN[3]                              1'b0
    12      R/W AON_PAD_P2_O[3]                             1'b0
    13      R/W PAD_P2_PUPDC[3]                             1'b0
    14      R/W PAD_P2_PU[3]                                1'b0
    15      R/W PAD_P2_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P2_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_3_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P2_3_DUMMY3: 1;
        uint16_t PAD_P2_S_3: 1;
        uint16_t PAD_P2_SMT_3: 1;
        uint16_t REG0X_PAD_P2_3_DUMMY6: 1;
        uint16_t PAD_P2_E2_3: 1;
        uint16_t PAD_P2_SHDN_3: 1;
        uint16_t AON_PAD_P2_E_3: 1;
        uint16_t PAD_P2_WKPOL_3: 1;
        uint16_t PAD_P2_WKEN_3: 1;
        uint16_t AON_PAD_P2_O_3: 1;
        uint16_t PAD_P2_PUPDC_3: 1;
        uint16_t PAD_P2_PU_3: 1;
        uint16_t PAD_P2_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P2_3_TYPE;

/* 0x1580
    0       R/W REG0X_PAD_P2_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_4_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P2_4_DUMMY3                       1'b1
    4       R/W PAD_P2_S[4]                                 1'b0
    5       R/W PAD_P2_SMT[4]                               1'b0
    6       R/W REG0X_PAD_P2_4_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[4]                                1'b0
    8       R/W PAD_P2_SHDN[4]                              1'b1
    9       R/W AON_PAD_P2_E[4]                             1'b0
    10      R/W PAD_P2_WKPOL[4]                             1'b0
    11      R/W PAD_P2_WKEN[4]                              1'b0
    12      R/W AON_PAD_P2_O[4]                             1'b0
    13      R/W PAD_P2_PUPDC[4]                             1'b0
    14      R/W PAD_P2_PU[4]                                1'b0
    15      R/W PAD_P2_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P2_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_4_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P2_4_DUMMY3: 1;
        uint16_t PAD_P2_S_4: 1;
        uint16_t PAD_P2_SMT_4: 1;
        uint16_t REG0X_PAD_P2_4_DUMMY6: 1;
        uint16_t PAD_P2_E2_4: 1;
        uint16_t PAD_P2_SHDN_4: 1;
        uint16_t AON_PAD_P2_E_4: 1;
        uint16_t PAD_P2_WKPOL_4: 1;
        uint16_t PAD_P2_WKEN_4: 1;
        uint16_t AON_PAD_P2_O_4: 1;
        uint16_t PAD_P2_PUPDC_4: 1;
        uint16_t PAD_P2_PU_4: 1;
        uint16_t PAD_P2_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P2_4_TYPE;

/* 0x1582
    0       R/W REG0X_PAD_P2_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_5_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P2_5_DUMMY3                       1'b1
    4       R/W PAD_P2_S[5]                                 1'b0
    5       R/W PAD_P2_SMT[5]                               1'b0
    6       R/W REG0X_PAD_P2_5_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[5]                                1'b0
    8       R/W PAD_P2_SHDN[5]                              1'b1
    9       R/W AON_PAD_P2_E[5]                             1'b0
    10      R/W PAD_P2_WKPOL[5]                             1'b0
    11      R/W PAD_P2_WKEN[5]                              1'b0
    12      R/W AON_PAD_P2_O[5]                             1'b0
    13      R/W PAD_P2_PUPDC[5]                             1'b0
    14      R/W PAD_P2_PU[5]                                1'b0
    15      R/W PAD_P2_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P2_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_5_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P2_5_DUMMY3: 1;
        uint16_t PAD_P2_S_5: 1;
        uint16_t PAD_P2_SMT_5: 1;
        uint16_t REG0X_PAD_P2_5_DUMMY6: 1;
        uint16_t PAD_P2_E2_5: 1;
        uint16_t PAD_P2_SHDN_5: 1;
        uint16_t AON_PAD_P2_E_5: 1;
        uint16_t PAD_P2_WKPOL_5: 1;
        uint16_t PAD_P2_WKEN_5: 1;
        uint16_t AON_PAD_P2_O_5: 1;
        uint16_t PAD_P2_PUPDC_5: 1;
        uint16_t PAD_P2_PU_5: 1;
        uint16_t PAD_P2_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P2_5_TYPE;

/* 0x1584
    0       R/W REG0X_PAD_P2_6_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_6_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[6]                       1'b0
    3       R/W REG0X_PAD_P2_6_DUMMY3                       1'b1
    4       R/W PAD_P2_S[6]                                 1'b0
    5       R/W PAD_P2_SMT[6]                               1'b0
    6       R/W REG0X_PAD_P2_6_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[6]                                1'b0
    8       R/W PAD_P2_SHDN[6]                              1'b1
    9       R/W AON_PAD_P2_E[6]                             1'b0
    10      R/W PAD_P2_WKPOL[6]                             1'b0
    11      R/W PAD_P2_WKEN[6]                              1'b0
    12      R/W AON_PAD_P2_O[6]                             1'b0
    13      R/W PAD_P2_PUPDC[6]                             1'b0
    14      R/W PAD_P2_PU[6]                                1'b0
    15      R/W PAD_P2_PU_EN[6]                             1'b1
 */
typedef union _AON_FAST_REG6X_PAD_P2_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_6_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_6_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_6: 1;
        uint16_t REG0X_PAD_P2_6_DUMMY3: 1;
        uint16_t PAD_P2_S_6: 1;
        uint16_t PAD_P2_SMT_6: 1;
        uint16_t REG0X_PAD_P2_6_DUMMY6: 1;
        uint16_t PAD_P2_E2_6: 1;
        uint16_t PAD_P2_SHDN_6: 1;
        uint16_t AON_PAD_P2_E_6: 1;
        uint16_t PAD_P2_WKPOL_6: 1;
        uint16_t PAD_P2_WKEN_6: 1;
        uint16_t AON_PAD_P2_O_6: 1;
        uint16_t PAD_P2_PUPDC_6: 1;
        uint16_t PAD_P2_PU_6: 1;
        uint16_t PAD_P2_PU_EN_6: 1;
    };
} AON_FAST_REG6X_PAD_P2_6_TYPE;

/* 0x1586
    0       R/W REG0X_PAD_P2_7_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P2_7_DUMMY1                       1'b0
    2       R/W PAD_P2_WKUP_INT_EN[7]                       1'b0
    3       R/W REG0X_PAD_P2_7_DUMMY3                       1'b1
    4       R/W PAD_P2_S[7]                                 1'b0
    5       R/W PAD_P2_SMT[7]                               1'b0
    6       R/W REG0X_PAD_P2_7_DUMMY6                       1'b0
    7       R/W PAD_P2_E2[7]                                1'b0
    8       R/W PAD_P2_SHDN[7]                              1'b1
    9       R/W AON_PAD_P2_E[7]                             1'b0
    10      R/W PAD_P2_WKPOL[7]                             1'b0
    11      R/W PAD_P2_WKEN[7]                              1'b0
    12      R/W AON_PAD_P2_O[7]                             1'b0
    13      R/W PAD_P2_PUPDC[7]                             1'b0
    14      R/W PAD_P2_PU[7]                                1'b0
    15      R/W PAD_P2_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_REG7X_PAD_P2_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P2_7_DUMMY0: 1;
        uint16_t REG0X_PAD_P2_7_DUMMY1: 1;
        uint16_t PAD_P2_WKUP_INT_EN_7: 1;
        uint16_t REG0X_PAD_P2_7_DUMMY3: 1;
        uint16_t PAD_P2_S_7: 1;
        uint16_t PAD_P2_SMT_7: 1;
        uint16_t REG0X_PAD_P2_7_DUMMY6: 1;
        uint16_t PAD_P2_E2_7: 1;
        uint16_t PAD_P2_SHDN_7: 1;
        uint16_t AON_PAD_P2_E_7: 1;
        uint16_t PAD_P2_WKPOL_7: 1;
        uint16_t PAD_P2_WKEN_7: 1;
        uint16_t AON_PAD_P2_O_7: 1;
        uint16_t PAD_P2_PUPDC_7: 1;
        uint16_t PAD_P2_PU_7: 1;
        uint16_t PAD_P2_PU_EN_7: 1;
    };
} AON_FAST_REG7X_PAD_P2_7_TYPE;

/* 0x1588
    0       R/W REG0X_PAD_P3_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P3_0_DUMMY1                       1'b0
    2       R/W PAD_P3_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P3_0_DUMMY3                       1'b1
    4       R/W PAD_P3_S[0]                                 1'b0
    5       R/W PAD_P3_SMT[0]                               1'b0
    6       R/W PAD_P3_E3[0]                                1'b0
    7       R/W PAD_P3_E2[0]                                1'b0
    8       R/W PAD_P3_SHDN[0]                              1'b1
    9       R/W AON_PAD_P3_E[0]                             1'b0
    10      R/W PAD_P3_WKPOL[0]                             1'b0
    11      R/W PAD_P3_WKEN[0]                              1'b0
    12      R/W AON_PAD_P3_O[0]                             1'b0
    13      R/W PAD_P3_PUPDC[0]                             1'b0
    14      R/W PAD_P3_PU[0]                                1'b0
    15      R/W PAD_P3_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P3_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P3_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P3_0_DUMMY1: 1;
        uint16_t PAD_P3_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P3_0_DUMMY3: 1;
        uint16_t PAD_P3_S_0: 1;
        uint16_t PAD_P3_SMT_0: 1;
        uint16_t PAD_P3_E3_0: 1;
        uint16_t PAD_P3_E2_0: 1;
        uint16_t PAD_P3_SHDN_0: 1;
        uint16_t AON_PAD_P3_E_0: 1;
        uint16_t PAD_P3_WKPOL_0: 1;
        uint16_t PAD_P3_WKEN_0: 1;
        uint16_t AON_PAD_P3_O_0: 1;
        uint16_t PAD_P3_PUPDC_0: 1;
        uint16_t PAD_P3_PU_0: 1;
        uint16_t PAD_P3_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P3_0_TYPE;

/* 0x158A
    0       R/W REG0X_PAD_P3_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P3_1_DUMMY1                       1'b0
    2       R/W PAD_P3_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P3_1_DUMMY3                       1'b1
    4       R/W PAD_P3_S[1]                                 1'b0
    5       R/W PAD_P3_SMT[1]                               1'b0
    6       R/W PAD_P3_E3[1]                                1'b0
    7       R/W PAD_P3_E2[1]                                1'b0
    8       R/W PAD_P3_SHDN[1]                              1'b1
    9       R/W AON_PAD_P3_E[1]                             1'b0
    10      R/W PAD_P3_WKPOL[1]                             1'b0
    11      R/W PAD_P3_WKEN[1]                              1'b0
    12      R/W AON_PAD_P3_O[1]                             1'b0
    13      R/W PAD_P3_PUPDC[1]                             1'b0
    14      R/W PAD_P3_PU[1]                                1'b0
    15      R/W PAD_P3_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P3_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P3_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P3_1_DUMMY1: 1;
        uint16_t PAD_P3_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P3_1_DUMMY3: 1;
        uint16_t PAD_P3_S_1: 1;
        uint16_t PAD_P3_SMT_1: 1;
        uint16_t PAD_P3_E3_1: 1;
        uint16_t PAD_P3_E2_1: 1;
        uint16_t PAD_P3_SHDN_1: 1;
        uint16_t AON_PAD_P3_E_1: 1;
        uint16_t PAD_P3_WKPOL_1: 1;
        uint16_t PAD_P3_WKEN_1: 1;
        uint16_t AON_PAD_P3_O_1: 1;
        uint16_t PAD_P3_PUPDC_1: 1;
        uint16_t PAD_P3_PU_1: 1;
        uint16_t PAD_P3_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P3_1_TYPE;

/* 0x158C
    0       R/W REG0X_PAD_P3_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P3_2_DUMMY1                       1'b0
    2       R/W PAD_P3_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P3_2_DUMMY3                       1'b1
    4       R/W PAD_P3_S[2]                                 1'b0
    5       R/W PAD_P3_SMT[2]                               1'b0
    6       R/W PAD_P3_E3[2]                                1'b0
    7       R/W PAD_P3_E2[2]                                1'b0
    8       R/W PAD_P3_SHDN[2]                              1'b1
    9       R/W AON_PAD_P3_E[2]                             1'b0
    10      R/W PAD_P3_WKPOL[2]                             1'b0
    11      R/W PAD_P3_WKEN[2]                              1'b0
    12      R/W AON_PAD_P3_O[2]                             1'b0
    13      R/W PAD_P3_PUPDC[2]                             1'b0
    14      R/W PAD_P3_PU[2]                                1'b0
    15      R/W PAD_P3_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P3_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P3_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P3_2_DUMMY1: 1;
        uint16_t PAD_P3_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P3_2_DUMMY3: 1;
        uint16_t PAD_P3_S_2: 1;
        uint16_t PAD_P3_SMT_2: 1;
        uint16_t PAD_P3_E3_2: 1;
        uint16_t PAD_P3_E2_2: 1;
        uint16_t PAD_P3_SHDN_2: 1;
        uint16_t AON_PAD_P3_E_2: 1;
        uint16_t PAD_P3_WKPOL_2: 1;
        uint16_t PAD_P3_WKEN_2: 1;
        uint16_t AON_PAD_P3_O_2: 1;
        uint16_t PAD_P3_PUPDC_2: 1;
        uint16_t PAD_P3_PU_2: 1;
        uint16_t PAD_P3_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P3_2_TYPE;

/* 0x158E
    0       R/W REG0X_PAD_P3_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P3_3_DUMMY1                       1'b0
    2       R/W PAD_P3_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P3_3_DUMMY3                       1'b1
    4       R/W PAD_P3_S[3]                                 1'b0
    5       R/W PAD_P3_SMT[3]                               1'b0
    6       R/W PAD_P3_E3[3]                                1'b0
    7       R/W PAD_P3_E2[3]                                1'b0
    8       R/W PAD_P3_SHDN[3]                              1'b1
    9       R/W AON_PAD_P3_E[3]                             1'b0
    10      R/W PAD_P3_WKPOL[3]                             1'b0
    11      R/W PAD_P3_WKEN[3]                              1'b0
    12      R/W AON_PAD_P3_O[3]                             1'b0
    13      R/W PAD_P3_PUPDC[3]                             1'b0
    14      R/W PAD_P3_PU[3]                                1'b0
    15      R/W PAD_P3_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P3_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P3_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P3_3_DUMMY1: 1;
        uint16_t PAD_P3_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P3_3_DUMMY3: 1;
        uint16_t PAD_P3_S_3: 1;
        uint16_t PAD_P3_SMT_3: 1;
        uint16_t PAD_P3_E3_3: 1;
        uint16_t PAD_P3_E2_3: 1;
        uint16_t PAD_P3_SHDN_3: 1;
        uint16_t AON_PAD_P3_E_3: 1;
        uint16_t PAD_P3_WKPOL_3: 1;
        uint16_t PAD_P3_WKEN_3: 1;
        uint16_t AON_PAD_P3_O_3: 1;
        uint16_t PAD_P3_PUPDC_3: 1;
        uint16_t PAD_P3_PU_3: 1;
        uint16_t PAD_P3_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P3_3_TYPE;

/* 0x1590
    0       R/W REG0X_PAD_P3_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P3_4_DUMMY1                       1'b0
    2       R/W PAD_P3_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P3_4_DUMMY3                       1'b1
    4       R/W PAD_P3_S[4]                                 1'b0
    5       R/W PAD_P3_SMT[4]                               1'b0
    6       R/W PAD_P3_E3[4]                                1'b0
    7       R/W PAD_P3_E2[4]                                1'b0
    8       R/W PAD_P3_SHDN[4]                              1'b1
    9       R/W AON_PAD_P3_E[4]                             1'b0
    10      R/W PAD_P3_WKPOL[4]                             1'b0
    11      R/W PAD_P3_WKEN[4]                              1'b0
    12      R/W AON_PAD_P3_O[4]                             1'b0
    13      R/W PAD_P3_PUPDC[4]                             1'b0
    14      R/W PAD_P3_PU[4]                                1'b0
    15      R/W PAD_P3_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P3_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P3_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P3_4_DUMMY1: 1;
        uint16_t PAD_P3_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P3_4_DUMMY3: 1;
        uint16_t PAD_P3_S_4: 1;
        uint16_t PAD_P3_SMT_4: 1;
        uint16_t PAD_P3_E3_4: 1;
        uint16_t PAD_P3_E2_4: 1;
        uint16_t PAD_P3_SHDN_4: 1;
        uint16_t AON_PAD_P3_E_4: 1;
        uint16_t PAD_P3_WKPOL_4: 1;
        uint16_t PAD_P3_WKEN_4: 1;
        uint16_t AON_PAD_P3_O_4: 1;
        uint16_t PAD_P3_PUPDC_4: 1;
        uint16_t PAD_P3_PU_4: 1;
        uint16_t PAD_P3_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P3_4_TYPE;

/* 0x1592
    0       R/W REG0X_PAD_P3_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P3_5_DUMMY1                       1'b0
    2       R/W PAD_P3_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P3_5_DUMMY3                       1'b1
    4       R/W PAD_P3_S[5]                                 1'b0
    5       R/W PAD_P3_SMT[5]                               1'b0
    6       R/W PAD_P3_E3[5]                                1'b0
    7       R/W PAD_P3_E2[5]                                1'b0
    8       R/W PAD_P3_SHDN[5]                              1'b1
    9       R/W AON_PAD_P3_E[5]                             1'b0
    10      R/W PAD_P3_WKPOL[5]                             1'b0
    11      R/W PAD_P3_WKEN[5]                              1'b0
    12      R/W AON_PAD_P3_O[5]                             1'b0
    13      R/W PAD_P3_PUPDC[5]                             1'b0
    14      R/W PAD_P3_PU[5]                                1'b0
    15      R/W PAD_P3_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P3_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P3_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P3_5_DUMMY1: 1;
        uint16_t PAD_P3_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P3_5_DUMMY3: 1;
        uint16_t PAD_P3_S_5: 1;
        uint16_t PAD_P3_SMT_5: 1;
        uint16_t PAD_P3_E3_5: 1;
        uint16_t PAD_P3_E2_5: 1;
        uint16_t PAD_P3_SHDN_5: 1;
        uint16_t AON_PAD_P3_E_5: 1;
        uint16_t PAD_P3_WKPOL_5: 1;
        uint16_t PAD_P3_WKEN_5: 1;
        uint16_t AON_PAD_P3_O_5: 1;
        uint16_t PAD_P3_PUPDC_5: 1;
        uint16_t PAD_P3_PU_5: 1;
        uint16_t PAD_P3_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P3_5_TYPE;

/* 0x1594
    0       R/W REG0X_PAD_P4_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_0_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P4_0_DUMMY3                       1'b1
    4       R/W PAD_P4_S[0]                                 1'b0
    5       R/W PAD_P4_SMT[0]                               1'b0
    6       R/W REG0X_PAD_P4_0_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[0]                                1'b0
    8       R/W PAD_P4_SHDN[0]                              1'b1
    9       R/W AON_PAD_P4_E[0]                             1'b0
    10      R/W PAD_P4_WKPOL[0]                             1'b0
    11      R/W PAD_P4_WKEN[0]                              1'b0
    12      R/W AON_PAD_P4_O[0]                             1'b0
    13      R/W PAD_P4_PUPDC[0]                             1'b0
    14      R/W PAD_P4_PU[0]                                1'b0
    15      R/W PAD_P4_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P4_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_0_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P4_0_DUMMY3: 1;
        uint16_t PAD_P4_S_0: 1;
        uint16_t PAD_P4_SMT_0: 1;
        uint16_t REG0X_PAD_P4_0_DUMMY6: 1;
        uint16_t PAD_P4_E2_0: 1;
        uint16_t PAD_P4_SHDN_0: 1;
        uint16_t AON_PAD_P4_E_0: 1;
        uint16_t PAD_P4_WKPOL_0: 1;
        uint16_t PAD_P4_WKEN_0: 1;
        uint16_t AON_PAD_P4_O_0: 1;
        uint16_t PAD_P4_PUPDC_0: 1;
        uint16_t PAD_P4_PU_0: 1;
        uint16_t PAD_P4_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P4_0_TYPE;

/* 0x1596
    0       R/W REG0X_PAD_P4_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_1_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P4_1_DUMMY3                       1'b1
    4       R/W PAD_P4_S[1]                                 1'b0
    5       R/W PAD_P4_SMT[1]                               1'b0
    6       R/W REG0X_PAD_P4_1_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[1]                                1'b0
    8       R/W PAD_P4_SHDN[1]                              1'b1
    9       R/W AON_PAD_P4_E[1]                             1'b0
    10      R/W PAD_P4_WKPOL[1]                             1'b0
    11      R/W PAD_P4_WKEN[1]                              1'b0
    12      R/W AON_PAD_P4_O[1]                             1'b0
    13      R/W PAD_P4_PUPDC[1]                             1'b0
    14      R/W PAD_P4_PU[1]                                1'b0
    15      R/W PAD_P4_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P4_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_1_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P4_1_DUMMY3: 1;
        uint16_t PAD_P4_S_1: 1;
        uint16_t PAD_P4_SMT_1: 1;
        uint16_t REG0X_PAD_P4_1_DUMMY6: 1;
        uint16_t PAD_P4_E2_1: 1;
        uint16_t PAD_P4_SHDN_1: 1;
        uint16_t AON_PAD_P4_E_1: 1;
        uint16_t PAD_P4_WKPOL_1: 1;
        uint16_t PAD_P4_WKEN_1: 1;
        uint16_t AON_PAD_P4_O_1: 1;
        uint16_t PAD_P4_PUPDC_1: 1;
        uint16_t PAD_P4_PU_1: 1;
        uint16_t PAD_P4_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P4_1_TYPE;

/* 0x1598
    0       R/W REG0X_PAD_P4_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_2_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P4_2_DUMMY3                       1'b1
    4       R/W PAD_P4_S[2]                                 1'b0
    5       R/W PAD_P4_SMT[2]                               1'b0
    6       R/W REG0X_PAD_P4_2_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[2]                                1'b0
    8       R/W PAD_P4_SHDN[2]                              1'b1
    9       R/W AON_PAD_P4_E[2]                             1'b0
    10      R/W PAD_P4_WKPOL[2]                             1'b0
    11      R/W PAD_P4_WKEN[2]                              1'b0
    12      R/W AON_PAD_P4_O[2]                             1'b0
    13      R/W PAD_P4_PUPDC[2]                             1'b0
    14      R/W PAD_P4_PU[2]                                1'b0
    15      R/W PAD_P4_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P4_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_2_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P4_2_DUMMY3: 1;
        uint16_t PAD_P4_S_2: 1;
        uint16_t PAD_P4_SMT_2: 1;
        uint16_t REG0X_PAD_P4_2_DUMMY6: 1;
        uint16_t PAD_P4_E2_2: 1;
        uint16_t PAD_P4_SHDN_2: 1;
        uint16_t AON_PAD_P4_E_2: 1;
        uint16_t PAD_P4_WKPOL_2: 1;
        uint16_t PAD_P4_WKEN_2: 1;
        uint16_t AON_PAD_P4_O_2: 1;
        uint16_t PAD_P4_PUPDC_2: 1;
        uint16_t PAD_P4_PU_2: 1;
        uint16_t PAD_P4_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P4_2_TYPE;

/* 0x159A
    0       R/W REG0X_PAD_P4_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_3_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P4_3_DUMMY3                       1'b1
    4       R/W PAD_P4_S[3]                                 1'b0
    5       R/W PAD_P4_SMT[3]                               1'b0
    6       R/W REG0X_PAD_P4_3_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[3]                                1'b0
    8       R/W PAD_P4_SHDN[3]                              1'b1
    9       R/W AON_PAD_P4_E[3]                             1'b0
    10      R/W PAD_P4_WKPOL[3]                             1'b0
    11      R/W PAD_P4_WKEN[3]                              1'b0
    12      R/W AON_PAD_P4_O[3]                             1'b0
    13      R/W PAD_P4_PUPDC[3]                             1'b0
    14      R/W PAD_P4_PU[3]                                1'b0
    15      R/W PAD_P4_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P4_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_3_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P4_3_DUMMY3: 1;
        uint16_t PAD_P4_S_3: 1;
        uint16_t PAD_P4_SMT_3: 1;
        uint16_t REG0X_PAD_P4_3_DUMMY6: 1;
        uint16_t PAD_P4_E2_3: 1;
        uint16_t PAD_P4_SHDN_3: 1;
        uint16_t AON_PAD_P4_E_3: 1;
        uint16_t PAD_P4_WKPOL_3: 1;
        uint16_t PAD_P4_WKEN_3: 1;
        uint16_t AON_PAD_P4_O_3: 1;
        uint16_t PAD_P4_PUPDC_3: 1;
        uint16_t PAD_P4_PU_3: 1;
        uint16_t PAD_P4_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P4_3_TYPE;

/* 0x159C
    0       R/W REG0X_PAD_P4_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_4_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P4_4_DUMMY3                       1'b1
    4       R/W PAD_P4_S[4]                                 1'b0
    5       R/W PAD_P4_SMT[4]                               1'b0
    6       R/W REG0X_PAD_P4_4_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[4]                                1'b0
    8       R/W PAD_P4_SHDN[4]                              1'b1
    9       R/W AON_PAD_P4_E[4]                             1'b0
    10      R/W PAD_P4_WKPOL[4]                             1'b0
    11      R/W PAD_P4_WKEN[4]                              1'b0
    12      R/W AON_PAD_P4_O[4]                             1'b0
    13      R/W PAD_P4_PUPDC[4]                             1'b0
    14      R/W PAD_P4_PU[4]                                1'b0
    15      R/W PAD_P4_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P4_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_4_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P4_4_DUMMY3: 1;
        uint16_t PAD_P4_S_4: 1;
        uint16_t PAD_P4_SMT_4: 1;
        uint16_t REG0X_PAD_P4_4_DUMMY6: 1;
        uint16_t PAD_P4_E2_4: 1;
        uint16_t PAD_P4_SHDN_4: 1;
        uint16_t AON_PAD_P4_E_4: 1;
        uint16_t PAD_P4_WKPOL_4: 1;
        uint16_t PAD_P4_WKEN_4: 1;
        uint16_t AON_PAD_P4_O_4: 1;
        uint16_t PAD_P4_PUPDC_4: 1;
        uint16_t PAD_P4_PU_4: 1;
        uint16_t PAD_P4_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P4_4_TYPE;

/* 0x159E
    0       R/W REG0X_PAD_P4_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_5_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P4_5_DUMMY3                       1'b1
    4       R/W PAD_P4_S[5]                                 1'b0
    5       R/W PAD_P4_SMT[5]                               1'b0
    6       R/W REG0X_PAD_P4_5_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[5]                                1'b0
    8       R/W PAD_P4_SHDN[5]                              1'b1
    9       R/W AON_PAD_P4_E[5]                             1'b0
    10      R/W PAD_P4_WKPOL[5]                             1'b0
    11      R/W PAD_P4_WKEN[5]                              1'b0
    12      R/W AON_PAD_P4_O[5]                             1'b0
    13      R/W PAD_P4_PUPDC[5]                             1'b0
    14      R/W PAD_P4_PU[5]                                1'b0
    15      R/W PAD_P4_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P4_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_5_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P4_5_DUMMY3: 1;
        uint16_t PAD_P4_S_5: 1;
        uint16_t PAD_P4_SMT_5: 1;
        uint16_t REG0X_PAD_P4_5_DUMMY6: 1;
        uint16_t PAD_P4_E2_5: 1;
        uint16_t PAD_P4_SHDN_5: 1;
        uint16_t AON_PAD_P4_E_5: 1;
        uint16_t PAD_P4_WKPOL_5: 1;
        uint16_t PAD_P4_WKEN_5: 1;
        uint16_t AON_PAD_P4_O_5: 1;
        uint16_t PAD_P4_PUPDC_5: 1;
        uint16_t PAD_P4_PU_5: 1;
        uint16_t PAD_P4_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P4_5_TYPE;

/* 0x15A0
    0       R/W REG0X_PAD_P4_6_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_6_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[6]                       1'b0
    3       R/W REG0X_PAD_P4_6_DUMMY3                       1'b1
    4       R/W PAD_P4_S[6]                                 1'b0
    5       R/W PAD_P4_SMT[6]                               1'b0
    6       R/W REG0X_PAD_P4_6_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[6]                                1'b0
    8       R/W PAD_P4_SHDN[6]                              1'b1
    9       R/W AON_PAD_P4_E[6]                             1'b0
    10      R/W PAD_P4_WKPOL[6]                             1'b0
    11      R/W PAD_P4_WKEN[6]                              1'b0
    12      R/W AON_PAD_P4_O[6]                             1'b0
    13      R/W PAD_P4_PUPDC[6]                             1'b0
    14      R/W PAD_P4_PU[6]                                1'b0
    15      R/W PAD_P4_PU_EN[6]                             1'b1
 */
typedef union _AON_FAST_REG6X_PAD_P4_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_6_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_6_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_6: 1;
        uint16_t REG0X_PAD_P4_6_DUMMY3: 1;
        uint16_t PAD_P4_S_6: 1;
        uint16_t PAD_P4_SMT_6: 1;
        uint16_t REG0X_PAD_P4_6_DUMMY6: 1;
        uint16_t PAD_P4_E2_6: 1;
        uint16_t PAD_P4_SHDN_6: 1;
        uint16_t AON_PAD_P4_E_6: 1;
        uint16_t PAD_P4_WKPOL_6: 1;
        uint16_t PAD_P4_WKEN_6: 1;
        uint16_t AON_PAD_P4_O_6: 1;
        uint16_t PAD_P4_PUPDC_6: 1;
        uint16_t PAD_P4_PU_6: 1;
        uint16_t PAD_P4_PU_EN_6: 1;
    };
} AON_FAST_REG6X_PAD_P4_6_TYPE;

/* 0x15A2
    0       R/W REG0X_PAD_P4_7_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P4_7_DUMMY1                       1'b0
    2       R/W PAD_P4_WKUP_INT_EN[7]                       1'b0
    3       R/W REG0X_PAD_P4_7_DUMMY3                       1'b1
    4       R/W PAD_P4_S[7]                                 1'b0
    5       R/W PAD_P4_SMT[7]                               1'b0
    6       R/W REG0X_PAD_P4_7_DUMMY6                       1'b0
    7       R/W PAD_P4_E2[7]                                1'b0
    8       R/W PAD_P4_SHDN[7]                              1'b1
    9       R/W AON_PAD_P4_E[7]                             1'b0
    10      R/W PAD_P4_WKPOL[7]                             1'b0
    11      R/W PAD_P4_WKEN[7]                              1'b0
    12      R/W AON_PAD_P4_O[7]                             1'b0
    13      R/W PAD_P4_PUPDC[7]                             1'b0
    14      R/W PAD_P4_PU[7]                                1'b0
    15      R/W PAD_P4_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_REG7X_PAD_P4_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P4_7_DUMMY0: 1;
        uint16_t REG0X_PAD_P4_7_DUMMY1: 1;
        uint16_t PAD_P4_WKUP_INT_EN_7: 1;
        uint16_t REG0X_PAD_P4_7_DUMMY3: 1;
        uint16_t PAD_P4_S_7: 1;
        uint16_t PAD_P4_SMT_7: 1;
        uint16_t REG0X_PAD_P4_7_DUMMY6: 1;
        uint16_t PAD_P4_E2_7: 1;
        uint16_t PAD_P4_SHDN_7: 1;
        uint16_t AON_PAD_P4_E_7: 1;
        uint16_t PAD_P4_WKPOL_7: 1;
        uint16_t PAD_P4_WKEN_7: 1;
        uint16_t AON_PAD_P4_O_7: 1;
        uint16_t PAD_P4_PUPDC_7: 1;
        uint16_t PAD_P4_PU_7: 1;
        uint16_t PAD_P4_PU_EN_7: 1;
    };
} AON_FAST_REG7X_PAD_P4_7_TYPE;

/* 0x15A4
    0       R/W REG0X_PAD_P5_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_0_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P5_0_DUMMY3                       1'b1
    4       R/W PAD_P5_S[0]                                 1'b0
    5       R/W PAD_P5_SMT[0]                               1'b0
    6       R/W PAD_P5_E3[0]                                1'b0
    7       R/W PAD_P5_E2[0]                                1'b0
    8       R/W PAD_P5_SHDN[0]                              1'b1
    9       R/W AON_PAD_P5_E[0]                             1'b0
    10      R/W PAD_P5_WKPOL[0]                             1'b0
    11      R/W PAD_P5_WKEN[0]                              1'b0
    12      R/W AON_PAD_P5_O[0]                             1'b0
    13      R/W PAD_P5_PUPDC[0]                             1'b0
    14      R/W PAD_P5_PU[0]                                1'b0
    15      R/W PAD_P5_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P5_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_0_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P5_0_DUMMY3: 1;
        uint16_t PAD_P5_S_0: 1;
        uint16_t PAD_P5_SMT_0: 1;
        uint16_t PAD_P5_E3_0: 1;
        uint16_t PAD_P5_E2_0: 1;
        uint16_t PAD_P5_SHDN_0: 1;
        uint16_t AON_PAD_P5_E_0: 1;
        uint16_t PAD_P5_WKPOL_0: 1;
        uint16_t PAD_P5_WKEN_0: 1;
        uint16_t AON_PAD_P5_O_0: 1;
        uint16_t PAD_P5_PUPDC_0: 1;
        uint16_t PAD_P5_PU_0: 1;
        uint16_t PAD_P5_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P5_0_TYPE;

/* 0x15A6
    0       R/W REG0X_PAD_P5_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_1_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P5_1_DUMMY3                       1'b1
    4       R/W PAD_P5_S[1]                                 1'b0
    5       R/W PAD_P5_SMT[1]                               1'b0
    6       R/W PAD_P5_E3[1]                                1'b0
    7       R/W PAD_P5_E2[1]                                1'b0
    8       R/W PAD_P5_SHDN[1]                              1'b1
    9       R/W AON_PAD_P5_E[1]                             1'b0
    10      R/W PAD_P5_WKPOL[1]                             1'b0
    11      R/W PAD_P5_WKEN[1]                              1'b0
    12      R/W AON_PAD_P5_O[1]                             1'b0
    13      R/W PAD_P5_PUPDC[1]                             1'b0
    14      R/W PAD_P5_PU[1]                                1'b0
    15      R/W PAD_P5_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P5_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_1_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P5_1_DUMMY3: 1;
        uint16_t PAD_P5_S_1: 1;
        uint16_t PAD_P5_SMT_1: 1;
        uint16_t PAD_P5_E3_1: 1;
        uint16_t PAD_P5_E2_1: 1;
        uint16_t PAD_P5_SHDN_1: 1;
        uint16_t AON_PAD_P5_E_1: 1;
        uint16_t PAD_P5_WKPOL_1: 1;
        uint16_t PAD_P5_WKEN_1: 1;
        uint16_t AON_PAD_P5_O_1: 1;
        uint16_t PAD_P5_PUPDC_1: 1;
        uint16_t PAD_P5_PU_1: 1;
        uint16_t PAD_P5_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P5_1_TYPE;

/* 0x15A8
    0       R/W REG0X_PAD_P5_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_2_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P5_2_DUMMY3                       1'b1
    4       R/W PAD_P5_S[2]                                 1'b0
    5       R/W PAD_P5_SMT[2]                               1'b0
    6       R/W PAD_P5_E3[2]                                1'b0
    7       R/W PAD_P5_E2[2]                                1'b0
    8       R/W PAD_P5_SHDN[2]                              1'b1
    9       R/W AON_PAD_P5_E[2]                             1'b0
    10      R/W PAD_P5_WKPOL[2]                             1'b0
    11      R/W PAD_P5_WKEN[2]                              1'b0
    12      R/W AON_PAD_P5_O[2]                             1'b0
    13      R/W PAD_P5_PUPDC[2]                             1'b0
    14      R/W PAD_P5_PU[2]                                1'b0
    15      R/W PAD_P5_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P5_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_2_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P5_2_DUMMY3: 1;
        uint16_t PAD_P5_S_2: 1;
        uint16_t PAD_P5_SMT_2: 1;
        uint16_t PAD_P5_E3_2: 1;
        uint16_t PAD_P5_E2_2: 1;
        uint16_t PAD_P5_SHDN_2: 1;
        uint16_t AON_PAD_P5_E_2: 1;
        uint16_t PAD_P5_WKPOL_2: 1;
        uint16_t PAD_P5_WKEN_2: 1;
        uint16_t AON_PAD_P5_O_2: 1;
        uint16_t PAD_P5_PUPDC_2: 1;
        uint16_t PAD_P5_PU_2: 1;
        uint16_t PAD_P5_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P5_2_TYPE;

/* 0x15AA
    0       R/W REG0X_PAD_P5_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_3_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P5_3_DUMMY3                       1'b1
    4       R/W PAD_P5_S[3]                                 1'b0
    5       R/W PAD_P5_SMT[3]                               1'b0
    6       R/W PAD_P5_E3[3]                                1'b0
    7       R/W PAD_P5_E2[3]                                1'b0
    8       R/W PAD_P5_SHDN[3]                              1'b1
    9       R/W AON_PAD_P5_E[3]                             1'b0
    10      R/W PAD_P5_WKPOL[3]                             1'b0
    11      R/W PAD_P5_WKEN[3]                              1'b0
    12      R/W AON_PAD_P5_O[3]                             1'b0
    13      R/W PAD_P5_PUPDC[3]                             1'b0
    14      R/W PAD_P5_PU[3]                                1'b0
    15      R/W PAD_P5_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P5_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_3_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P5_3_DUMMY3: 1;
        uint16_t PAD_P5_S_3: 1;
        uint16_t PAD_P5_SMT_3: 1;
        uint16_t PAD_P5_E3_3: 1;
        uint16_t PAD_P5_E2_3: 1;
        uint16_t PAD_P5_SHDN_3: 1;
        uint16_t AON_PAD_P5_E_3: 1;
        uint16_t PAD_P5_WKPOL_3: 1;
        uint16_t PAD_P5_WKEN_3: 1;
        uint16_t AON_PAD_P5_O_3: 1;
        uint16_t PAD_P5_PUPDC_3: 1;
        uint16_t PAD_P5_PU_3: 1;
        uint16_t PAD_P5_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P5_3_TYPE;

/* 0x15AC
    0       R/W REG0X_PAD_P5_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_4_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P5_4_DUMMY3                       1'b1
    4       R/W PAD_P5_S[4]                                 1'b0
    5       R/W PAD_P5_SMT[4]                               1'b0
    6       R/W PAD_P5_E3[4]                                1'b0
    7       R/W PAD_P5_E2[4]                                1'b0
    8       R/W PAD_P5_SHDN[4]                              1'b1
    9       R/W AON_PAD_P5_E[4]                             1'b0
    10      R/W PAD_P5_WKPOL[4]                             1'b0
    11      R/W PAD_P5_WKEN[4]                              1'b0
    12      R/W AON_PAD_P5_O[4]                             1'b0
    13      R/W PAD_P5_PUPDC[4]                             1'b0
    14      R/W PAD_P5_PU[4]                                1'b0
    15      R/W PAD_P5_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P5_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_4_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P5_4_DUMMY3: 1;
        uint16_t PAD_P5_S_4: 1;
        uint16_t PAD_P5_SMT_4: 1;
        uint16_t PAD_P5_E3_4: 1;
        uint16_t PAD_P5_E2_4: 1;
        uint16_t PAD_P5_SHDN_4: 1;
        uint16_t AON_PAD_P5_E_4: 1;
        uint16_t PAD_P5_WKPOL_4: 1;
        uint16_t PAD_P5_WKEN_4: 1;
        uint16_t AON_PAD_P5_O_4: 1;
        uint16_t PAD_P5_PUPDC_4: 1;
        uint16_t PAD_P5_PU_4: 1;
        uint16_t PAD_P5_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P5_4_TYPE;

/* 0x15AE
    0       R/W REG0X_PAD_P5_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_5_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P5_5_DUMMY3                       1'b1
    4       R/W PAD_P5_S[5]                                 1'b0
    5       R/W PAD_P5_SMT[5]                               1'b0
    6       R/W PAD_P5_E3[5]                                1'b0
    7       R/W PAD_P5_E2[5]                                1'b0
    8       R/W PAD_P5_SHDN[5]                              1'b1
    9       R/W AON_PAD_P5_E[5]                             1'b0
    10      R/W PAD_P5_WKPOL[5]                             1'b0
    11      R/W PAD_P5_WKEN[5]                              1'b0
    12      R/W AON_PAD_P5_O[5]                             1'b0
    13      R/W PAD_P5_PUPDC[5]                             1'b0
    14      R/W PAD_P5_PU[5]                                1'b0
    15      R/W PAD_P5_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P5_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_5_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P5_5_DUMMY3: 1;
        uint16_t PAD_P5_S_5: 1;
        uint16_t PAD_P5_SMT_5: 1;
        uint16_t PAD_P5_E3_5: 1;
        uint16_t PAD_P5_E2_5: 1;
        uint16_t PAD_P5_SHDN_5: 1;
        uint16_t AON_PAD_P5_E_5: 1;
        uint16_t PAD_P5_WKPOL_5: 1;
        uint16_t PAD_P5_WKEN_5: 1;
        uint16_t AON_PAD_P5_O_5: 1;
        uint16_t PAD_P5_PUPDC_5: 1;
        uint16_t PAD_P5_PU_5: 1;
        uint16_t PAD_P5_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P5_5_TYPE;

/* 0x15B0
    0       R/W REG0X_PAD_P5_6_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_6_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[6]                       1'b0
    3       R/W REG0X_PAD_P5_6_DUMMY3                       1'b1
    4       R/W PAD_P5_S[6]                                 1'b0
    5       R/W PAD_P5_SMT[6]                               1'b0
    6       R/W PAD_P5_E3[6]                                1'b0
    7       R/W PAD_P5_E2[6]                                1'b0
    8       R/W PAD_P5_SHDN[6]                              1'b1
    9       R/W AON_PAD_P5_E[6]                             1'b0
    10      R/W PAD_P5_WKPOL[6]                             1'b0
    11      R/W PAD_P5_WKEN[6]                              1'b0
    12      R/W AON_PAD_P5_O[6]                             1'b0
    13      R/W PAD_P5_PUPDC[6]                             1'b0
    14      R/W PAD_P5_PU[6]                                1'b0
    15      R/W PAD_P5_PU_EN[6]                             1'b1
 */
typedef union _AON_FAST_REG6X_PAD_P5_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_6_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_6_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_6: 1;
        uint16_t REG0X_PAD_P5_6_DUMMY3: 1;
        uint16_t PAD_P5_S_6: 1;
        uint16_t PAD_P5_SMT_6: 1;
        uint16_t PAD_P5_E3_6: 1;
        uint16_t PAD_P5_E2_6: 1;
        uint16_t PAD_P5_SHDN_6: 1;
        uint16_t AON_PAD_P5_E_6: 1;
        uint16_t PAD_P5_WKPOL_6: 1;
        uint16_t PAD_P5_WKEN_6: 1;
        uint16_t AON_PAD_P5_O_6: 1;
        uint16_t PAD_P5_PUPDC_6: 1;
        uint16_t PAD_P5_PU_6: 1;
        uint16_t PAD_P5_PU_EN_6: 1;
    };
} AON_FAST_REG6X_PAD_P5_6_TYPE;

/* 0x15B2
    0       R/W REG0X_PAD_P5_7_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P5_7_DUMMY1                       1'b0
    2       R/W PAD_P5_WKUP_INT_EN[7]                       1'b0
    3       R/W REG0X_PAD_P5_7_DUMMY3                       1'b1
    4       R/W PAD_P5_S[7]                                 1'b0
    5       R/W PAD_P5_SMT[7]                               1'b0
    6       R/W PAD_P5_E3[7]                                1'b0
    7       R/W PAD_P5_E2[7]                                1'b0
    8       R/W PAD_P5_SHDN[7]                              1'b1
    9       R/W AON_PAD_P5_E[7]                             1'b0
    10      R/W PAD_P5_WKPOL[7]                             1'b0
    11      R/W PAD_P5_WKEN[7]                              1'b0
    12      R/W AON_PAD_P5_O[7]                             1'b0
    13      R/W PAD_P5_PUPDC[7]                             1'b0
    14      R/W PAD_P5_PU[7]                                1'b0
    15      R/W PAD_P5_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_REG7X_PAD_P5_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P5_7_DUMMY0: 1;
        uint16_t REG0X_PAD_P5_7_DUMMY1: 1;
        uint16_t PAD_P5_WKUP_INT_EN_7: 1;
        uint16_t REG0X_PAD_P5_7_DUMMY3: 1;
        uint16_t PAD_P5_S_7: 1;
        uint16_t PAD_P5_SMT_7: 1;
        uint16_t PAD_P5_E3_7: 1;
        uint16_t PAD_P5_E2_7: 1;
        uint16_t PAD_P5_SHDN_7: 1;
        uint16_t AON_PAD_P5_E_7: 1;
        uint16_t PAD_P5_WKPOL_7: 1;
        uint16_t PAD_P5_WKEN_7: 1;
        uint16_t AON_PAD_P5_O_7: 1;
        uint16_t PAD_P5_PUPDC_7: 1;
        uint16_t PAD_P5_PU_7: 1;
        uint16_t PAD_P5_PU_EN_7: 1;
    };
} AON_FAST_REG7X_PAD_P5_7_TYPE;

/* 0x15B4
    0       R/W REG0X_PAD_P6_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_0_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P6_0_DUMMY3                       1'b1
    4       R/W PAD_P6_S[0]                                 1'b0
    5       R/W PAD_P6_SMT[0]                               1'b0
    6       R/W PAD_P6_E3[0]                                1'b0
    7       R/W PAD_P6_E2[0]                                1'b0
    8       R/W PAD_P6_SHDN[0]                              1'b1
    9       R/W AON_PAD_P6_E[0]                             1'b0
    10      R/W PAD_P6_WKPOL[0]                             1'b0
    11      R/W PAD_P6_WKEN[0]                              1'b0
    12      R/W AON_PAD_P6_O[0]                             1'b0
    13      R/W PAD_P6_PUPDC[0]                             1'b0
    14      R/W PAD_P6_PU[0]                                1'b0
    15      R/W PAD_P6_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P6_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_0_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P6_0_DUMMY3: 1;
        uint16_t PAD_P6_S_0: 1;
        uint16_t PAD_P6_SMT_0: 1;
        uint16_t PAD_P6_E3_0: 1;
        uint16_t PAD_P6_E2_0: 1;
        uint16_t PAD_P6_SHDN_0: 1;
        uint16_t AON_PAD_P6_E_0: 1;
        uint16_t PAD_P6_WKPOL_0: 1;
        uint16_t PAD_P6_WKEN_0: 1;
        uint16_t AON_PAD_P6_O_0: 1;
        uint16_t PAD_P6_PUPDC_0: 1;
        uint16_t PAD_P6_PU_0: 1;
        uint16_t PAD_P6_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P6_0_TYPE;

/* 0x15B6
    0       R/W REG0X_PAD_P6_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_1_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P6_1_DUMMY3                       1'b1
    4       R/W PAD_P6_S[1]                                 1'b0
    5       R/W PAD_P6_SMT[1]                               1'b0
    6       R/W PAD_P6_E3[1]                                1'b0
    7       R/W PAD_P6_E2[1]                                1'b0
    8       R/W PAD_P6_SHDN[1]                              1'b1
    9       R/W AON_PAD_P6_E[1]                             1'b0
    10      R/W PAD_P6_WKPOL[1]                             1'b0
    11      R/W PAD_P6_WKEN[1]                              1'b0
    12      R/W AON_PAD_P6_O[1]                             1'b0
    13      R/W PAD_P6_PUPDC[1]                             1'b0
    14      R/W PAD_P6_PU[1]                                1'b0
    15      R/W PAD_P6_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P6_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_1_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P6_1_DUMMY3: 1;
        uint16_t PAD_P6_S_1: 1;
        uint16_t PAD_P6_SMT_1: 1;
        uint16_t PAD_P6_E3_1: 1;
        uint16_t PAD_P6_E2_1: 1;
        uint16_t PAD_P6_SHDN_1: 1;
        uint16_t AON_PAD_P6_E_1: 1;
        uint16_t PAD_P6_WKPOL_1: 1;
        uint16_t PAD_P6_WKEN_1: 1;
        uint16_t AON_PAD_P6_O_1: 1;
        uint16_t PAD_P6_PUPDC_1: 1;
        uint16_t PAD_P6_PU_1: 1;
        uint16_t PAD_P6_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P6_1_TYPE;

/* 0x15B8
    0       R/W REG0X_PAD_P6_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_2_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P6_2_DUMMY3                       1'b1
    4       R/W PAD_P6_S[2]                                 1'b0
    5       R/W PAD_P6_SMT[2]                               1'b0
    6       R/W PAD_P6_E3[2]                                1'b0
    7       R/W PAD_P6_E2[2]                                1'b0
    8       R/W PAD_P6_SHDN[2]                              1'b1
    9       R/W AON_PAD_P6_E[2]                             1'b0
    10      R/W PAD_P6_WKPOL[2]                             1'b0
    11      R/W PAD_P6_WKEN[2]                              1'b0
    12      R/W AON_PAD_P6_O[2]                             1'b0
    13      R/W PAD_P6_PUPDC[2]                             1'b0
    14      R/W PAD_P6_PU[2]                                1'b0
    15      R/W PAD_P6_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P6_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_2_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P6_2_DUMMY3: 1;
        uint16_t PAD_P6_S_2: 1;
        uint16_t PAD_P6_SMT_2: 1;
        uint16_t PAD_P6_E3_2: 1;
        uint16_t PAD_P6_E2_2: 1;
        uint16_t PAD_P6_SHDN_2: 1;
        uint16_t AON_PAD_P6_E_2: 1;
        uint16_t PAD_P6_WKPOL_2: 1;
        uint16_t PAD_P6_WKEN_2: 1;
        uint16_t AON_PAD_P6_O_2: 1;
        uint16_t PAD_P6_PUPDC_2: 1;
        uint16_t PAD_P6_PU_2: 1;
        uint16_t PAD_P6_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P6_2_TYPE;

/* 0x15BA
    0       R/W REG0X_PAD_P6_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_3_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P6_3_DUMMY3                       1'b1
    4       R/W PAD_P6_S[3]                                 1'b0
    5       R/W PAD_P6_SMT[3]                               1'b0
    6       R/W PAD_P6_E3[3]                                1'b0
    7       R/W PAD_P6_E2[3]                                1'b0
    8       R/W PAD_P6_SHDN[3]                              1'b1
    9       R/W AON_PAD_P6_E[3]                             1'b0
    10      R/W PAD_P6_WKPOL[3]                             1'b0
    11      R/W PAD_P6_WKEN[3]                              1'b0
    12      R/W AON_PAD_P6_O[3]                             1'b0
    13      R/W PAD_P6_PUPDC[3]                             1'b0
    14      R/W PAD_P6_PU[3]                                1'b0
    15      R/W PAD_P6_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P6_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_3_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P6_3_DUMMY3: 1;
        uint16_t PAD_P6_S_3: 1;
        uint16_t PAD_P6_SMT_3: 1;
        uint16_t PAD_P6_E3_3: 1;
        uint16_t PAD_P6_E2_3: 1;
        uint16_t PAD_P6_SHDN_3: 1;
        uint16_t AON_PAD_P6_E_3: 1;
        uint16_t PAD_P6_WKPOL_3: 1;
        uint16_t PAD_P6_WKEN_3: 1;
        uint16_t AON_PAD_P6_O_3: 1;
        uint16_t PAD_P6_PUPDC_3: 1;
        uint16_t PAD_P6_PU_3: 1;
        uint16_t PAD_P6_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P6_3_TYPE;

/* 0x15BC
    0       R/W REG0X_PAD_P6_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_4_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P6_4_DUMMY3                       1'b1
    4       R/W PAD_P6_S[4]                                 1'b0
    5       R/W PAD_P6_SMT[4]                               1'b0
    6       R/W PAD_P6_E3[4]                                1'b0
    7       R/W PAD_P6_E2[4]                                1'b0
    8       R/W PAD_P6_SHDN[4]                              1'b1
    9       R/W AON_PAD_P6_E[4]                             1'b0
    10      R/W PAD_P6_WKPOL[4]                             1'b0
    11      R/W PAD_P6_WKEN[4]                              1'b0
    12      R/W AON_PAD_P6_O[4]                             1'b0
    13      R/W PAD_P6_PUPDC[4]                             1'b0
    14      R/W PAD_P6_PU[4]                                1'b0
    15      R/W PAD_P6_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P6_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_4_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P6_4_DUMMY3: 1;
        uint16_t PAD_P6_S_4: 1;
        uint16_t PAD_P6_SMT_4: 1;
        uint16_t PAD_P6_E3_4: 1;
        uint16_t PAD_P6_E2_4: 1;
        uint16_t PAD_P6_SHDN_4: 1;
        uint16_t AON_PAD_P6_E_4: 1;
        uint16_t PAD_P6_WKPOL_4: 1;
        uint16_t PAD_P6_WKEN_4: 1;
        uint16_t AON_PAD_P6_O_4: 1;
        uint16_t PAD_P6_PUPDC_4: 1;
        uint16_t PAD_P6_PU_4: 1;
        uint16_t PAD_P6_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P6_4_TYPE;

/* 0x15BE
    0       R/W REG0X_PAD_P6_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_5_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P6_5_DUMMY3                       1'b1
    4       R/W PAD_P6_S[5]                                 1'b0
    5       R/W PAD_P6_SMT[5]                               1'b0
    6       R/W PAD_P6_E3[5]                                1'b0
    7       R/W PAD_P6_E2[5]                                1'b0
    8       R/W PAD_P6_SHDN[5]                              1'b1
    9       R/W AON_PAD_P6_E[5]                             1'b0
    10      R/W PAD_P6_WKPOL[5]                             1'b0
    11      R/W PAD_P6_WKEN[5]                              1'b0
    12      R/W AON_PAD_P6_O[5]                             1'b0
    13      R/W PAD_P6_PUPDC[5]                             1'b0
    14      R/W PAD_P6_PU[5]                                1'b0
    15      R/W PAD_P6_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P6_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_5_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P6_5_DUMMY3: 1;
        uint16_t PAD_P6_S_5: 1;
        uint16_t PAD_P6_SMT_5: 1;
        uint16_t PAD_P6_E3_5: 1;
        uint16_t PAD_P6_E2_5: 1;
        uint16_t PAD_P6_SHDN_5: 1;
        uint16_t AON_PAD_P6_E_5: 1;
        uint16_t PAD_P6_WKPOL_5: 1;
        uint16_t PAD_P6_WKEN_5: 1;
        uint16_t AON_PAD_P6_O_5: 1;
        uint16_t PAD_P6_PUPDC_5: 1;
        uint16_t PAD_P6_PU_5: 1;
        uint16_t PAD_P6_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P6_5_TYPE;

/* 0x15C0
    0       R/W REG0X_PAD_P6_6_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P6_6_DUMMY1                       1'b0
    2       R/W PAD_P6_WKUP_INT_EN[6]                       1'b0
    3       R/W REG0X_PAD_P6_6_DUMMY3                       1'b1
    4       R/W PAD_P6_S[6]                                 1'b0
    5       R/W PAD_P6_SMT[6]                               1'b0
    6       R/W PAD_P6_E3[6]                                1'b0
    7       R/W PAD_P6_E2[6]                                1'b0
    8       R/W PAD_P6_SHDN[6]                              1'b1
    9       R/W AON_PAD_P6_E[6]                             1'b0
    10      R/W PAD_P6_WKPOL[6]                             1'b0
    11      R/W PAD_P6_WKEN[6]                              1'b0
    12      R/W AON_PAD_P6_O[6]                             1'b0
    13      R/W PAD_P6_PUPDC[6]                             1'b0
    14      R/W PAD_P6_PU[6]                                1'b0
    15      R/W PAD_P6_PU_EN[6]                             1'b1
 */
typedef union _AON_FAST_REG6X_PAD_P6_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P6_6_DUMMY0: 1;
        uint16_t REG0X_PAD_P6_6_DUMMY1: 1;
        uint16_t PAD_P6_WKUP_INT_EN_6: 1;
        uint16_t REG0X_PAD_P6_6_DUMMY3: 1;
        uint16_t PAD_P6_S_6: 1;
        uint16_t PAD_P6_SMT_6: 1;
        uint16_t PAD_P6_E3_6: 1;
        uint16_t PAD_P6_E2_6: 1;
        uint16_t PAD_P6_SHDN_6: 1;
        uint16_t AON_PAD_P6_E_6: 1;
        uint16_t PAD_P6_WKPOL_6: 1;
        uint16_t PAD_P6_WKEN_6: 1;
        uint16_t AON_PAD_P6_O_6: 1;
        uint16_t PAD_P6_PUPDC_6: 1;
        uint16_t PAD_P6_PU_6: 1;
        uint16_t PAD_P6_PU_EN_6: 1;
    };
} AON_FAST_REG6X_PAD_P6_6_TYPE;

/* 0x15C2
    0       R/W REG0X_PAD_P7_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_0_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P7_0_DUMMY3                       1'b1
    4       R/W PAD_P7_S[0]                                 1'b0
    5       R/W PAD_P7_SMT[0]                               1'b0
    6       R/W PAD_P7_E3[0]                                1'b0
    7       R/W PAD_P7_E2[0]                                1'b0
    8       R/W PAD_P7_SHDN[0]                              1'b0
    9       R/W AON_PAD_P7_E[0]                             1'b0
    10      R/W PAD_P7_WKPOL[0]                             1'b0
    11      R/W PAD_P7_WKEN[0]                              1'b0
    12      R/W AON_PAD_P7_O[0]                             1'b0
    13      R/W PAD_P7_PUPDC[0]                             1'b0
    14      R/W PAD_P7_PU[0]                                1'b0
    15      R/W PAD_P7_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P7_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_0_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P7_0_DUMMY3: 1;
        uint16_t PAD_P7_S_0: 1;
        uint16_t PAD_P7_SMT_0: 1;
        uint16_t PAD_P7_E3_0: 1;
        uint16_t PAD_P7_E2_0: 1;
        uint16_t PAD_P7_SHDN_0: 1;
        uint16_t AON_PAD_P7_E_0: 1;
        uint16_t PAD_P7_WKPOL_0: 1;
        uint16_t PAD_P7_WKEN_0: 1;
        uint16_t AON_PAD_P7_O_0: 1;
        uint16_t PAD_P7_PUPDC_0: 1;
        uint16_t PAD_P7_PU_0: 1;
        uint16_t PAD_P7_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P7_0_TYPE;

/* 0x15C4
    0       R/W REG0X_PAD_P7_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_1_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P7_1_DUMMY3                       1'b1
    4       R/W PAD_P7_S[1]                                 1'b0
    5       R/W PAD_P7_SMT[1]                               1'b0
    6       R/W PAD_P7_E3[1]                                1'b0
    7       R/W PAD_P7_E2[1]                                1'b0
    8       R/W PAD_P7_SHDN[1]                              1'b0
    9       R/W AON_PAD_P7_E[1]                             1'b0
    10      R/W PAD_P7_WKPOL[1]                             1'b0
    11      R/W PAD_P7_WKEN[1]                              1'b0
    12      R/W AON_PAD_P7_O[1]                             1'b0
    13      R/W PAD_P7_PUPDC[1]                             1'b0
    14      R/W PAD_P7_PU[1]                                1'b0
    15      R/W PAD_P7_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P7_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_1_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P7_1_DUMMY3: 1;
        uint16_t PAD_P7_S_1: 1;
        uint16_t PAD_P7_SMT_1: 1;
        uint16_t PAD_P7_E3_1: 1;
        uint16_t PAD_P7_E2_1: 1;
        uint16_t PAD_P7_SHDN_1: 1;
        uint16_t AON_PAD_P7_E_1: 1;
        uint16_t PAD_P7_WKPOL_1: 1;
        uint16_t PAD_P7_WKEN_1: 1;
        uint16_t AON_PAD_P7_O_1: 1;
        uint16_t PAD_P7_PUPDC_1: 1;
        uint16_t PAD_P7_PU_1: 1;
        uint16_t PAD_P7_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P7_1_TYPE;

/* 0x15C6
    0       R/W REG0X_PAD_P7_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_2_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P7_2_DUMMY3                       1'b1
    4       R/W PAD_P7_S[2]                                 1'b0
    5       R/W PAD_P7_SMT[2]                               1'b0
    6       R/W PAD_P7_E3[2]                                1'b0
    7       R/W PAD_P7_E2[2]                                1'b0
    8       R/W PAD_P7_SHDN[2]                              1'b0
    9       R/W AON_PAD_P7_E[2]                             1'b0
    10      R/W PAD_P7_WKPOL[2]                             1'b0
    11      R/W PAD_P7_WKEN[2]                              1'b0
    12      R/W AON_PAD_P7_O[2]                             1'b0
    13      R/W PAD_P7_PUPDC[2]                             1'b0
    14      R/W PAD_P7_PU[2]                                1'b0
    15      R/W PAD_P7_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P7_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_2_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P7_2_DUMMY3: 1;
        uint16_t PAD_P7_S_2: 1;
        uint16_t PAD_P7_SMT_2: 1;
        uint16_t PAD_P7_E3_2: 1;
        uint16_t PAD_P7_E2_2: 1;
        uint16_t PAD_P7_SHDN_2: 1;
        uint16_t AON_PAD_P7_E_2: 1;
        uint16_t PAD_P7_WKPOL_2: 1;
        uint16_t PAD_P7_WKEN_2: 1;
        uint16_t AON_PAD_P7_O_2: 1;
        uint16_t PAD_P7_PUPDC_2: 1;
        uint16_t PAD_P7_PU_2: 1;
        uint16_t PAD_P7_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P7_2_TYPE;

/* 0x15C8
    0       R/W REG0X_PAD_P7_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_3_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P7_3_DUMMY3                       1'b1
    4       R/W PAD_P7_S[3]                                 1'b0
    5       R/W PAD_P7_SMT[3]                               1'b0
    6       R/W PAD_P7_E3[3]                                1'b0
    7       R/W PAD_P7_E2[3]                                1'b0
    8       R/W PAD_P7_SHDN[3]                              1'b0
    9       R/W AON_PAD_P7_E[3]                             1'b0
    10      R/W PAD_P7_WKPOL[3]                             1'b0
    11      R/W PAD_P7_WKEN[3]                              1'b0
    12      R/W AON_PAD_P7_O[3]                             1'b0
    13      R/W PAD_P7_PUPDC[3]                             1'b0
    14      R/W PAD_P7_PU[3]                                1'b0
    15      R/W PAD_P7_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P7_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_3_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P7_3_DUMMY3: 1;
        uint16_t PAD_P7_S_3: 1;
        uint16_t PAD_P7_SMT_3: 1;
        uint16_t PAD_P7_E3_3: 1;
        uint16_t PAD_P7_E2_3: 1;
        uint16_t PAD_P7_SHDN_3: 1;
        uint16_t AON_PAD_P7_E_3: 1;
        uint16_t PAD_P7_WKPOL_3: 1;
        uint16_t PAD_P7_WKEN_3: 1;
        uint16_t AON_PAD_P7_O_3: 1;
        uint16_t PAD_P7_PUPDC_3: 1;
        uint16_t PAD_P7_PU_3: 1;
        uint16_t PAD_P7_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P7_3_TYPE;

/* 0x15CA
    0       R/W REG0X_PAD_P7_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_4_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P7_4_DUMMY3                       1'b1
    4       R/W PAD_P7_S[4]                                 1'b0
    5       R/W PAD_P7_SMT[4]                               1'b0
    6       R/W PAD_P7_E3[4]                                1'b0
    7       R/W PAD_P7_E2[4]                                1'b0
    8       R/W PAD_P7_SHDN[4]                              1'b0
    9       R/W AON_PAD_P7_E[4]                             1'b0
    10      R/W PAD_P7_WKPOL[4]                             1'b0
    11      R/W PAD_P7_WKEN[4]                              1'b0
    12      R/W AON_PAD_P7_O[4]                             1'b0
    13      R/W PAD_P7_PUPDC[4]                             1'b0
    14      R/W PAD_P7_PU[4]                                1'b0
    15      R/W PAD_P7_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P7_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_4_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P7_4_DUMMY3: 1;
        uint16_t PAD_P7_S_4: 1;
        uint16_t PAD_P7_SMT_4: 1;
        uint16_t PAD_P7_E3_4: 1;
        uint16_t PAD_P7_E2_4: 1;
        uint16_t PAD_P7_SHDN_4: 1;
        uint16_t AON_PAD_P7_E_4: 1;
        uint16_t PAD_P7_WKPOL_4: 1;
        uint16_t PAD_P7_WKEN_4: 1;
        uint16_t AON_PAD_P7_O_4: 1;
        uint16_t PAD_P7_PUPDC_4: 1;
        uint16_t PAD_P7_PU_4: 1;
        uint16_t PAD_P7_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P7_4_TYPE;

/* 0x15CC
    0       R/W REG0X_PAD_P7_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_5_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P7_5_DUMMY3                       1'b1
    4       R/W PAD_P7_S[5]                                 1'b0
    5       R/W PAD_P7_SMT[5]                               1'b0
    6       R/W PAD_P7_E3[5]                                1'b0
    7       R/W PAD_P7_E2[5]                                1'b0
    8       R/W PAD_P7_SHDN[5]                              1'b0
    9       R/W AON_PAD_P7_E[5]                             1'b0
    10      R/W PAD_P7_WKPOL[5]                             1'b0
    11      R/W PAD_P7_WKEN[5]                              1'b0
    12      R/W AON_PAD_P7_O[5]                             1'b0
    13      R/W PAD_P7_PUPDC[5]                             1'b0
    14      R/W PAD_P7_PU[5]                                1'b0
    15      R/W PAD_P7_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P7_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_5_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P7_5_DUMMY3: 1;
        uint16_t PAD_P7_S_5: 1;
        uint16_t PAD_P7_SMT_5: 1;
        uint16_t PAD_P7_E3_5: 1;
        uint16_t PAD_P7_E2_5: 1;
        uint16_t PAD_P7_SHDN_5: 1;
        uint16_t AON_PAD_P7_E_5: 1;
        uint16_t PAD_P7_WKPOL_5: 1;
        uint16_t PAD_P7_WKEN_5: 1;
        uint16_t AON_PAD_P7_O_5: 1;
        uint16_t PAD_P7_PUPDC_5: 1;
        uint16_t PAD_P7_PU_5: 1;
        uint16_t PAD_P7_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P7_5_TYPE;

/* 0x15CE
    0       R/W REG0X_PAD_P7_6_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P7_6_DUMMY1                       1'b0
    2       R/W PAD_P7_WKUP_INT_EN[6]                       1'b0
    3       R/W REG0X_PAD_P7_6_DUMMY3                       1'b1
    4       R/W PAD_P7_S[6]                                 1'b0
    5       R/W PAD_P7_SMT[6]                               1'b0
    6       R/W PAD_P7_E3[6]                                1'b0
    7       R/W PAD_P7_E2[6]                                1'b0
    8       R/W PAD_P7_SHDN[6]                              1'b0
    9       R/W AON_PAD_P7_E[6]                             1'b0
    10      R/W PAD_P7_WKPOL[6]                             1'b0
    11      R/W PAD_P7_WKEN[6]                              1'b0
    12      R/W AON_PAD_P7_O[6]                             1'b0
    13      R/W PAD_P7_PUPDC[6]                             1'b0
    14      R/W PAD_P7_PU[6]                                1'b0
    15      R/W PAD_P7_PU_EN[6]                             1'b1
 */
typedef union _AON_FAST_REG6X_PAD_P7_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P7_6_DUMMY0: 1;
        uint16_t REG0X_PAD_P7_6_DUMMY1: 1;
        uint16_t PAD_P7_WKUP_INT_EN_6: 1;
        uint16_t REG0X_PAD_P7_6_DUMMY3: 1;
        uint16_t PAD_P7_S_6: 1;
        uint16_t PAD_P7_SMT_6: 1;
        uint16_t PAD_P7_E3_6: 1;
        uint16_t PAD_P7_E2_6: 1;
        uint16_t PAD_P7_SHDN_6: 1;
        uint16_t AON_PAD_P7_E_6: 1;
        uint16_t PAD_P7_WKPOL_6: 1;
        uint16_t PAD_P7_WKEN_6: 1;
        uint16_t AON_PAD_P7_O_6: 1;
        uint16_t PAD_P7_PUPDC_6: 1;
        uint16_t PAD_P7_PU_6: 1;
        uint16_t PAD_P7_PU_EN_6: 1;
    };
} AON_FAST_REG6X_PAD_P7_6_TYPE;

/* 0x15D0
    0       R/W REG0X_PAD_P8_0_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P8_0_DUMMY1                       1'b0
    2       R/W PAD_P8_WKUP_INT_EN[0]                       1'b0
    3       R/W REG0X_PAD_P8_0_DUMMY3                       1'b1
    4       R/W PAD_P8_S[0]                                 1'b0
    5       R/W PAD_P8_SMT[0]                               1'b0
    6       R/W REG0X_PAD_P8_0_DUMMY6                       1'b0
    7       R/W PAD_P8_E2[0]                                1'b0
    8       R/W PAD_P8_SHDN[0]                              1'b0
    9       R/W AON_PAD_P8_E[0]                             1'b0
    10      R/W PAD_P8_WKPOL[0]                             1'b0
    11      R/W PAD_P8_WKEN[0]                              1'b0
    12      R/W AON_PAD_P8_O[0]                             1'b0
    13      R/W PAD_P8_PUPDC[0]                             1'b0
    14      R/W PAD_P8_PU[0]                                1'b0
    15      R/W PAD_P8_PU_EN[0]                             1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P8_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P8_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P8_0_DUMMY1: 1;
        uint16_t PAD_P8_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P8_0_DUMMY3: 1;
        uint16_t PAD_P8_S_0: 1;
        uint16_t PAD_P8_SMT_0: 1;
        uint16_t REG0X_PAD_P8_0_DUMMY6: 1;
        uint16_t PAD_P8_E2_0: 1;
        uint16_t PAD_P8_SHDN_0: 1;
        uint16_t AON_PAD_P8_E_0: 1;
        uint16_t PAD_P8_WKPOL_0: 1;
        uint16_t PAD_P8_WKEN_0: 1;
        uint16_t AON_PAD_P8_O_0: 1;
        uint16_t PAD_P8_PUPDC_0: 1;
        uint16_t PAD_P8_PU_0: 1;
        uint16_t PAD_P8_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P8_0_TYPE;

/* 0x15D2
    0       R/W REG0X_PAD_P8_1_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P8_1_DUMMY1                       1'b0
    2       R/W PAD_P8_WKUP_INT_EN[1]                       1'b0
    3       R/W REG0X_PAD_P8_1_DUMMY3                       1'b1
    4       R/W PAD_P8_S[1]                                 1'b0
    5       R/W PAD_P8_SMT[1]                               1'b0
    6       R/W REG0X_PAD_P8_1_DUMMY6                       1'b0
    7       R/W PAD_P8_E2[1]                                1'b0
    8       R/W PAD_P8_SHDN[1]                              1'b0
    9       R/W AON_PAD_P8_E[1]                             1'b0
    10      R/W PAD_P8_WKPOL[1]                             1'b0
    11      R/W PAD_P8_WKEN[1]                              1'b0
    12      R/W AON_PAD_P8_O[1]                             1'b0
    13      R/W PAD_P8_PUPDC[1]                             1'b0
    14      R/W PAD_P8_PU[1]                                1'b0
    15      R/W PAD_P8_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_REG1X_PAD_P8_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P8_1_DUMMY0: 1;
        uint16_t REG0X_PAD_P8_1_DUMMY1: 1;
        uint16_t PAD_P8_WKUP_INT_EN_1: 1;
        uint16_t REG0X_PAD_P8_1_DUMMY3: 1;
        uint16_t PAD_P8_S_1: 1;
        uint16_t PAD_P8_SMT_1: 1;
        uint16_t REG0X_PAD_P8_1_DUMMY6: 1;
        uint16_t PAD_P8_E2_1: 1;
        uint16_t PAD_P8_SHDN_1: 1;
        uint16_t AON_PAD_P8_E_1: 1;
        uint16_t PAD_P8_WKPOL_1: 1;
        uint16_t PAD_P8_WKEN_1: 1;
        uint16_t AON_PAD_P8_O_1: 1;
        uint16_t PAD_P8_PUPDC_1: 1;
        uint16_t PAD_P8_PU_1: 1;
        uint16_t PAD_P8_PU_EN_1: 1;
    };
} AON_FAST_REG1X_PAD_P8_1_TYPE;

/* 0x15D4
    0       R/W REG0X_PAD_P8_2_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P8_2_DUMMY1                       1'b0
    2       R/W PAD_P8_WKUP_INT_EN[2]                       1'b0
    3       R/W REG0X_PAD_P8_2_DUMMY3                       1'b1
    4       R/W PAD_P8_S[2]                                 1'b0
    5       R/W PAD_P8_SMT[2]                               1'b0
    6       R/W REG0X_PAD_P8_2_DUMMY6                       1'b0
    7       R/W PAD_P8_E2[2]                                1'b0
    8       R/W PAD_P8_SHDN[2]                              1'b0
    9       R/W AON_PAD_P8_E[2]                             1'b0
    10      R/W PAD_P8_WKPOL[2]                             1'b0
    11      R/W PAD_P8_WKEN[2]                              1'b0
    12      R/W AON_PAD_P8_O[2]                             1'b0
    13      R/W PAD_P8_PUPDC[2]                             1'b0
    14      R/W PAD_P8_PU[2]                                1'b0
    15      R/W PAD_P8_PU_EN[2]                             1'b1
 */
typedef union _AON_FAST_REG2X_PAD_P8_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P8_2_DUMMY0: 1;
        uint16_t REG0X_PAD_P8_2_DUMMY1: 1;
        uint16_t PAD_P8_WKUP_INT_EN_2: 1;
        uint16_t REG0X_PAD_P8_2_DUMMY3: 1;
        uint16_t PAD_P8_S_2: 1;
        uint16_t PAD_P8_SMT_2: 1;
        uint16_t REG0X_PAD_P8_2_DUMMY6: 1;
        uint16_t PAD_P8_E2_2: 1;
        uint16_t PAD_P8_SHDN_2: 1;
        uint16_t AON_PAD_P8_E_2: 1;
        uint16_t PAD_P8_WKPOL_2: 1;
        uint16_t PAD_P8_WKEN_2: 1;
        uint16_t AON_PAD_P8_O_2: 1;
        uint16_t PAD_P8_PUPDC_2: 1;
        uint16_t PAD_P8_PU_2: 1;
        uint16_t PAD_P8_PU_EN_2: 1;
    };
} AON_FAST_REG2X_PAD_P8_2_TYPE;

/* 0x15D6
    0       R/W REG0X_PAD_P8_3_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P8_3_DUMMY1                       1'b0
    2       R/W PAD_P8_WKUP_INT_EN[3]                       1'b0
    3       R/W REG0X_PAD_P8_3_DUMMY3                       1'b1
    4       R/W PAD_P8_S[3]                                 1'b0
    5       R/W PAD_P8_SMT[3]                               1'b0
    6       R/W REG0X_PAD_P8_3_DUMMY6                       1'b0
    7       R/W PAD_P8_E2[3]                                1'b0
    8       R/W PAD_P8_SHDN[3]                              1'b0
    9       R/W AON_PAD_P8_E[3]                             1'b0
    10      R/W PAD_P8_WKPOL[3]                             1'b0
    11      R/W PAD_P8_WKEN[3]                              1'b0
    12      R/W AON_PAD_P8_O[3]                             1'b0
    13      R/W PAD_P8_PUPDC[3]                             1'b0
    14      R/W PAD_P8_PU[3]                                1'b0
    15      R/W PAD_P8_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_REG3X_PAD_P8_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P8_3_DUMMY0: 1;
        uint16_t REG0X_PAD_P8_3_DUMMY1: 1;
        uint16_t PAD_P8_WKUP_INT_EN_3: 1;
        uint16_t REG0X_PAD_P8_3_DUMMY3: 1;
        uint16_t PAD_P8_S_3: 1;
        uint16_t PAD_P8_SMT_3: 1;
        uint16_t REG0X_PAD_P8_3_DUMMY6: 1;
        uint16_t PAD_P8_E2_3: 1;
        uint16_t PAD_P8_SHDN_3: 1;
        uint16_t AON_PAD_P8_E_3: 1;
        uint16_t PAD_P8_WKPOL_3: 1;
        uint16_t PAD_P8_WKEN_3: 1;
        uint16_t AON_PAD_P8_O_3: 1;
        uint16_t PAD_P8_PUPDC_3: 1;
        uint16_t PAD_P8_PU_3: 1;
        uint16_t PAD_P8_PU_EN_3: 1;
    };
} AON_FAST_REG3X_PAD_P8_3_TYPE;

/* 0x15D8
    0       R/W REG0X_PAD_P8_4_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P8_4_DUMMY1                       1'b0
    2       R/W PAD_P8_WKUP_INT_EN[4]                       1'b0
    3       R/W REG0X_PAD_P8_4_DUMMY3                       1'b1
    4       R/W PAD_P8_S[4]                                 1'b0
    5       R/W PAD_P8_SMT[4]                               1'b0
    6       R/W REG0X_PAD_P8_4_DUMMY6                       1'b0
    7       R/W PAD_P8_E2[4]                                1'b0
    8       R/W PAD_P8_SHDN[4]                              1'b0
    9       R/W AON_PAD_P8_E[4]                             1'b0
    10      R/W PAD_P8_WKPOL[4]                             1'b0
    11      R/W PAD_P8_WKEN[4]                              1'b0
    12      R/W AON_PAD_P8_O[4]                             1'b0
    13      R/W PAD_P8_PUPDC[4]                             1'b0
    14      R/W PAD_P8_PU[4]                                1'b0
    15      R/W PAD_P8_PU_EN[4]                             1'b1
 */
typedef union _AON_FAST_REG4X_PAD_P8_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P8_4_DUMMY0: 1;
        uint16_t REG0X_PAD_P8_4_DUMMY1: 1;
        uint16_t PAD_P8_WKUP_INT_EN_4: 1;
        uint16_t REG0X_PAD_P8_4_DUMMY3: 1;
        uint16_t PAD_P8_S_4: 1;
        uint16_t PAD_P8_SMT_4: 1;
        uint16_t REG0X_PAD_P8_4_DUMMY6: 1;
        uint16_t PAD_P8_E2_4: 1;
        uint16_t PAD_P8_SHDN_4: 1;
        uint16_t AON_PAD_P8_E_4: 1;
        uint16_t PAD_P8_WKPOL_4: 1;
        uint16_t PAD_P8_WKEN_4: 1;
        uint16_t AON_PAD_P8_O_4: 1;
        uint16_t PAD_P8_PUPDC_4: 1;
        uint16_t PAD_P8_PU_4: 1;
        uint16_t PAD_P8_PU_EN_4: 1;
    };
} AON_FAST_REG4X_PAD_P8_4_TYPE;

/* 0x15DA
    0       R/W REG0X_PAD_P8_5_DUMMY0                       1'b0
    1       R/W REG0X_PAD_P8_5_DUMMY1                       1'b0
    2       R/W PAD_P8_WKUP_INT_EN[5]                       1'b0
    3       R/W REG0X_PAD_P8_5_DUMMY3                       1'b1
    4       R/W PAD_P8_S[5]                                 1'b0
    5       R/W PAD_P8_SMT[5]                               1'b0
    6       R/W REG0X_PAD_P8_5_DUMMY6                       1'b0
    7       R/W PAD_P8_E2[5]                                1'b0
    8       R/W PAD_P8_SHDN[5]                              1'b0
    9       R/W AON_PAD_P8_E[5]                             1'b0
    10      R/W PAD_P8_WKPOL[5]                             1'b0
    11      R/W PAD_P8_WKEN[5]                              1'b0
    12      R/W AON_PAD_P8_O[5]                             1'b0
    13      R/W PAD_P8_PUPDC[5]                             1'b0
    14      R/W PAD_P8_PU[5]                                1'b0
    15      R/W PAD_P8_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_REG5X_PAD_P8_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P8_5_DUMMY0: 1;
        uint16_t REG0X_PAD_P8_5_DUMMY1: 1;
        uint16_t PAD_P8_WKUP_INT_EN_5: 1;
        uint16_t REG0X_PAD_P8_5_DUMMY3: 1;
        uint16_t PAD_P8_S_5: 1;
        uint16_t PAD_P8_SMT_5: 1;
        uint16_t REG0X_PAD_P8_5_DUMMY6: 1;
        uint16_t PAD_P8_E2_5: 1;
        uint16_t PAD_P8_SHDN_5: 1;
        uint16_t AON_PAD_P8_E_5: 1;
        uint16_t PAD_P8_WKPOL_5: 1;
        uint16_t PAD_P8_WKEN_5: 1;
        uint16_t AON_PAD_P8_O_5: 1;
        uint16_t PAD_P8_PUPDC_5: 1;
        uint16_t PAD_P8_PU_5: 1;
        uint16_t PAD_P8_PU_EN_5: 1;
    };
} AON_FAST_REG5X_PAD_P8_5_TYPE;

/* 0x15DC
    0       R/W REG0X_PAD_SPIC0_CSN_0_DUMMY0                1'b0
    1       R/W REG0X_PAD_SPIC0_CSN_0_DUMMY1                1'b0
    2       R/W REG0X_PAD_SPIC0_CSN_DUMMY2                  1'b0
    3       R/W REG0X_PAD_SPIC0_CSN_DUMMY3                  1'b1
    4       R/W PAD_SPIC0_CSN_S                             1'b0
    5       R/W PAD_SPIC0_CSN_SMT                           1'b0
    6       R/W PAD_SPIC0_CSN_E3                            1'b0
    7       R/W PAD_SPIC0_CSN_E2                            1'b0
    8       R/W PAD_SPIC0_CSN_SHDN                          1'b1
    9       R/W AON_PAD_SPIC0_CSN_E                         1'b0
    10      R/W REG0X_PAD_SPIC0_CSN_DUMMY10                 1'b0
    11      R/W REG0X_PAD_SPIC0_CSN_DUMMY11                 1'b0
    12      R/W AON_PAD_SPIC0_CSN_O                         1'b0
    13      R/W PAD_SPIC0_CSN_PUPDC                         1'b0
    14      R/W PAD_SPIC0_CSN_PU                            1'b0
    15      R/W PAD_SPIC0_CSN_PU_EN                         1'b1
 */
typedef union _AON_FAST_REG0X_PAD_SPIC0_CSN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC0_CSN_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC0_CSN_0_DUMMY1: 1;
        uint16_t REG0X_PAD_SPIC0_CSN_DUMMY2: 1;
        uint16_t REG0X_PAD_SPIC0_CSN_DUMMY3: 1;
        uint16_t PAD_SPIC0_CSN_S: 1;
        uint16_t PAD_SPIC0_CSN_SMT: 1;
        uint16_t PAD_SPIC0_CSN_E3: 1;
        uint16_t PAD_SPIC0_CSN_E2: 1;
        uint16_t PAD_SPIC0_CSN_SHDN: 1;
        uint16_t AON_PAD_SPIC0_CSN_E: 1;
        uint16_t REG0X_PAD_SPIC0_CSN_DUMMY10: 1;
        uint16_t REG0X_PAD_SPIC0_CSN_DUMMY11: 1;
        uint16_t AON_PAD_SPIC0_CSN_O: 1;
        uint16_t PAD_SPIC0_CSN_PUPDC: 1;
        uint16_t PAD_SPIC0_CSN_PU: 1;
        uint16_t PAD_SPIC0_CSN_PU_EN: 1;
    };
} AON_FAST_REG0X_PAD_SPIC0_CSN_TYPE;

/* 0x15DE
    0       R/W REG0X_PAD_SPIC0_SCK_0_DUMMY0                1'b0
    1       R/W REG0X_PAD_SPIC0_SCK_0_DUMMY1                1'b0
    2       R/W REG1X_PAD_SPIC0_SCK_DUMMY2                  1'b0
    3       R/W REG1X_PAD_SPIC0_SCK_DUMMY3                  1'b1
    4       R/W PAD_SPIC0_SCK_S                             1'b0
    5       R/W PAD_SPIC0_SCK_SMT                           1'b0
    6       R/W PAD_SPIC0_SCK_E3                            1'b0
    7       R/W PAD_SPIC0_SCK_E2                            1'b0
    8       R/W PAD_SPIC0_SCK_SHDN                          1'b1
    9       R/W AON_PAD_SPIC0_SCK_E                         1'b0
    10      R/W REG1X_PAD_SPIC0_SCK_DUMMY10                 1'b0
    11      R/W REG1X_PAD_SPIC0_SCK_DUMMY11                 1'b0
    12      R/W AON_PAD_SPIC0_SCK_O                         1'b0
    13      R/W PAD_SPIC0_SCK_PUPDC                         1'b0
    14      R/W PAD_SPIC0_SCK_PU                            1'b0
    15      R/W PAD_SPIC0_SCK_PU_EN                         1'b1
 */
typedef union _AON_FAST_REG1X_PAD_SPIC0_SCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC0_SCK_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC0_SCK_0_DUMMY1: 1;
        uint16_t REG1X_PAD_SPIC0_SCK_DUMMY2: 1;
        uint16_t REG1X_PAD_SPIC0_SCK_DUMMY3: 1;
        uint16_t PAD_SPIC0_SCK_S: 1;
        uint16_t PAD_SPIC0_SCK_SMT: 1;
        uint16_t PAD_SPIC0_SCK_E3: 1;
        uint16_t PAD_SPIC0_SCK_E2: 1;
        uint16_t PAD_SPIC0_SCK_SHDN: 1;
        uint16_t AON_PAD_SPIC0_SCK_E: 1;
        uint16_t REG1X_PAD_SPIC0_SCK_DUMMY10: 1;
        uint16_t REG1X_PAD_SPIC0_SCK_DUMMY11: 1;
        uint16_t AON_PAD_SPIC0_SCK_O: 1;
        uint16_t PAD_SPIC0_SCK_PUPDC: 1;
        uint16_t PAD_SPIC0_SCK_PU: 1;
        uint16_t PAD_SPIC0_SCK_PU_EN: 1;
    };
} AON_FAST_REG1X_PAD_SPIC0_SCK_TYPE;

/* 0x15E0
    0       R/W REG0X_PAD_SPIC0_SIO0_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC0_SIO0_0_DUMMY1               1'b0
    2       R/W REG2X_PAD_SPIC0_SIO0_DUMMY2                 1'b0
    3       R/W REG2X_PAD_SPIC0_SIO0_DUMMY3                 1'b1
    4       R/W PAD_SPIC0_SIO0_S                            1'b0
    5       R/W PAD_SPIC0_SIO0_SMT                          1'b0
    6       R/W PAD_SPIC0_SIO0_E3                           1'b0
    7       R/W PAD_SPIC0_SIO0_E2                           1'b0
    8       R/W PAD_SPIC0_SIO0_SHDN                         1'b1
    9       R/W AON_PAD_SPIC0_SIO0_E                        1'b0
    10      R/W REG2X_PAD_SPIC0_SIO0_DUMMY10                1'b0
    11      R/W REG2X_PAD_SPIC0_SIO0_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC0_SIO0_O                        1'b0
    13      R/W PAD_SPIC0_SIO0_PUPDC                        1'b0
    14      R/W PAD_SPIC0_SIO0_PU                           1'b0
    15      R/W PAD_SPIC0_SIO0_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG2X_PAD_SPIC0_SIO0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC0_SIO0_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC0_SIO0_0_DUMMY1: 1;
        uint16_t REG2X_PAD_SPIC0_SIO0_DUMMY2: 1;
        uint16_t REG2X_PAD_SPIC0_SIO0_DUMMY3: 1;
        uint16_t PAD_SPIC0_SIO0_S: 1;
        uint16_t PAD_SPIC0_SIO0_SMT: 1;
        uint16_t PAD_SPIC0_SIO0_E3: 1;
        uint16_t PAD_SPIC0_SIO0_E2: 1;
        uint16_t PAD_SPIC0_SIO0_SHDN: 1;
        uint16_t AON_PAD_SPIC0_SIO0_E: 1;
        uint16_t REG2X_PAD_SPIC0_SIO0_DUMMY10: 1;
        uint16_t REG2X_PAD_SPIC0_SIO0_DUMMY11: 1;
        uint16_t AON_PAD_SPIC0_SIO0_O: 1;
        uint16_t PAD_SPIC0_SIO0_PUPDC: 1;
        uint16_t PAD_SPIC0_SIO0_PU: 1;
        uint16_t PAD_SPIC0_SIO0_PU_EN: 1;
    };
} AON_FAST_REG2X_PAD_SPIC0_SIO0_TYPE;

/* 0x15E2
    0       R/W REG0X_PAD_SPIC0_SIO1_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC0_SIO1_0_DUMMY1               1'b0
    2       R/W REG3X_PAD_SPIC0_SIO1_DUMMY2                 1'b0
    3       R/W REG3X_PAD_SPIC0_SIO1_DUMMY3                 1'b1
    4       R/W PAD_SPIC0_SIO1_S                            1'b0
    5       R/W PAD_SPIC0_SIO1_SMT                          1'b0
    6       R/W PAD_SPIC0_SIO1_E3                           1'b0
    7       R/W PAD_SPIC0_SIO1_E2                           1'b0
    8       R/W PAD_SPIC0_SIO1_SHDN                         1'b1
    9       R/W AON_PAD_SPIC0_SIO1_E                        1'b0
    10      R/W REG3X_PAD_SPIC0_SIO1_DUMMY10                1'b0
    11      R/W REG3X_PAD_SPIC0_SIO1_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC0_SIO1_O                        1'b0
    13      R/W PAD_SPIC0_SIO1_PUPDC                        1'b0
    14      R/W PAD_SPIC0_SIO1_PU                           1'b0
    15      R/W PAD_SPIC0_SIO1_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG3X_PAD_SPIC0_SIO1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC0_SIO1_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC0_SIO1_0_DUMMY1: 1;
        uint16_t REG3X_PAD_SPIC0_SIO1_DUMMY2: 1;
        uint16_t REG3X_PAD_SPIC0_SIO1_DUMMY3: 1;
        uint16_t PAD_SPIC0_SIO1_S: 1;
        uint16_t PAD_SPIC0_SIO1_SMT: 1;
        uint16_t PAD_SPIC0_SIO1_E3: 1;
        uint16_t PAD_SPIC0_SIO1_E2: 1;
        uint16_t PAD_SPIC0_SIO1_SHDN: 1;
        uint16_t AON_PAD_SPIC0_SIO1_E: 1;
        uint16_t REG3X_PAD_SPIC0_SIO1_DUMMY10: 1;
        uint16_t REG3X_PAD_SPIC0_SIO1_DUMMY11: 1;
        uint16_t AON_PAD_SPIC0_SIO1_O: 1;
        uint16_t PAD_SPIC0_SIO1_PUPDC: 1;
        uint16_t PAD_SPIC0_SIO1_PU: 1;
        uint16_t PAD_SPIC0_SIO1_PU_EN: 1;
    };
} AON_FAST_REG3X_PAD_SPIC0_SIO1_TYPE;

/* 0x15E4
    0       R/W REG0X_PAD_SPIC0_SIO2_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC0_SIO2_0_DUMMY1               1'b0
    2       R/W REG4X_PAD_SPIC0_SIO2_DUMMY2                 1'b0
    3       R/W REG4X_PAD_SPIC0_SIO2_DUMMY3                 1'b1
    4       R/W PAD_SPIC0_SIO2_S                            1'b0
    5       R/W PAD_SPIC0_SIO2_SMT                          1'b0
    6       R/W PAD_SPIC0_SIO2_E3                           1'b0
    7       R/W PAD_SPIC0_SIO2_E2                           1'b0
    8       R/W PAD_SPIC0_SIO2_SHDN                         1'b1
    9       R/W AON_PAD_SPIC0_SIO2_E                        1'b0
    10      R/W REG4X_PAD_SPIC0_SIO2_DUMMY10                1'b0
    11      R/W REG4X_PAD_SPIC0_SIO2_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC0_SIO2_O                        1'b0
    13      R/W PAD_SPIC0_SIO2_PUPDC                        1'b0
    14      R/W PAD_SPIC0_SIO2_PU                           1'b0
    15      R/W PAD_SPIC0_SIO2_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG4X_PAD_SPIC0_SIO2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC0_SIO2_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC0_SIO2_0_DUMMY1: 1;
        uint16_t REG4X_PAD_SPIC0_SIO2_DUMMY2: 1;
        uint16_t REG4X_PAD_SPIC0_SIO2_DUMMY3: 1;
        uint16_t PAD_SPIC0_SIO2_S: 1;
        uint16_t PAD_SPIC0_SIO2_SMT: 1;
        uint16_t PAD_SPIC0_SIO2_E3: 1;
        uint16_t PAD_SPIC0_SIO2_E2: 1;
        uint16_t PAD_SPIC0_SIO2_SHDN: 1;
        uint16_t AON_PAD_SPIC0_SIO2_E: 1;
        uint16_t REG4X_PAD_SPIC0_SIO2_DUMMY10: 1;
        uint16_t REG4X_PAD_SPIC0_SIO2_DUMMY11: 1;
        uint16_t AON_PAD_SPIC0_SIO2_O: 1;
        uint16_t PAD_SPIC0_SIO2_PUPDC: 1;
        uint16_t PAD_SPIC0_SIO2_PU: 1;
        uint16_t PAD_SPIC0_SIO2_PU_EN: 1;
    };
} AON_FAST_REG4X_PAD_SPIC0_SIO2_TYPE;

/* 0x15E6
    0       R/W REG0X_PAD_SPIC0_SIO3_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC0_SIO3_0_DUMMY1               1'b0
    2       R/W REG5X_PAD_SPIC0_SIO3_DUMMY2                 1'b0
    3       R/W REG5X_PAD_SPIC0_SIO3_DUMMY3                 1'b1
    4       R/W PAD_SPIC0_SIO3_S                            1'b0
    5       R/W PAD_SPIC0_SIO3_SMT                          1'b0
    6       R/W PAD_SPIC0_SIO3_E3                           1'b0
    7       R/W PAD_SPIC0_SIO3_E2                           1'b0
    8       R/W PAD_SPIC0_SIO3_SHDN                         1'b1
    9       R/W AON_PAD_SPIC0_SIO3_E                        1'b0
    10      R/W REG5X_PAD_SPIC0_SIO3_DUMMY10                1'b0
    11      R/W REG5X_PAD_SPIC0_SIO3_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC0_SIO3_O                        1'b0
    13      R/W PAD_SPIC0_SIO3_PUPDC                        1'b0
    14      R/W PAD_SPIC0_SIO3_PU                           1'b0
    15      R/W PAD_SPIC0_SIO3_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG5X_PAD_SPIC0_SIO3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC0_SIO3_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC0_SIO3_0_DUMMY1: 1;
        uint16_t REG5X_PAD_SPIC0_SIO3_DUMMY2: 1;
        uint16_t REG5X_PAD_SPIC0_SIO3_DUMMY3: 1;
        uint16_t PAD_SPIC0_SIO3_S: 1;
        uint16_t PAD_SPIC0_SIO3_SMT: 1;
        uint16_t PAD_SPIC0_SIO3_E3: 1;
        uint16_t PAD_SPIC0_SIO3_E2: 1;
        uint16_t PAD_SPIC0_SIO3_SHDN: 1;
        uint16_t AON_PAD_SPIC0_SIO3_E: 1;
        uint16_t REG5X_PAD_SPIC0_SIO3_DUMMY10: 1;
        uint16_t REG5X_PAD_SPIC0_SIO3_DUMMY11: 1;
        uint16_t AON_PAD_SPIC0_SIO3_O: 1;
        uint16_t PAD_SPIC0_SIO3_PUPDC: 1;
        uint16_t PAD_SPIC0_SIO3_PU: 1;
        uint16_t PAD_SPIC0_SIO3_PU_EN: 1;
    };
} AON_FAST_REG5X_PAD_SPIC0_SIO3_TYPE;

/* 0x15E8
    0       R/W REG0X_PAD_SPIC1_CSN_0_DUMMY0                1'b0
    1       R/W REG0X_PAD_SPIC1_CSN_0_DUMMY1                1'b0
    2       R/W REG0X_PAD_SPIC1_CSN_DUMMY2                  1'b0
    3       R/W REG0X_PAD_SPIC1_CSN_DUMMY3                  1'b1
    4       R/W PAD_SPIC1_CSN_S                             1'b0
    5       R/W PAD_SPIC1_CSN_SMT                           1'b0
    6       R/W PAD_SPIC1_CSN_E3                            1'b0
    7       R/W PAD_SPIC1_CSN_E2                            1'b0
    8       R/W PAD_SPIC1_CSN_SHDN                          1'b1
    9       R/W AON_PAD_SPIC1_CSN_E                         1'b0
    10      R/W REG0X_PAD_SPIC1_CSN_DUMMY10                 1'b0
    11      R/W REG0X_PAD_SPIC1_CSN_DUMMY11                 1'b0
    12      R/W AON_PAD_SPIC1_CSN_O                         1'b0
    13      R/W PAD_SPIC1_CSN_PUPDC                         1'b0
    14      R/W PAD_SPIC1_CSN_PU                            1'b0
    15      R/W PAD_SPIC1_CSN_PU_EN                         1'b1
 */
typedef union _AON_FAST_REG0X_PAD_SPIC1_CSN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC1_CSN_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC1_CSN_0_DUMMY1: 1;
        uint16_t REG0X_PAD_SPIC1_CSN_DUMMY2: 1;
        uint16_t REG0X_PAD_SPIC1_CSN_DUMMY3: 1;
        uint16_t PAD_SPIC1_CSN_S: 1;
        uint16_t PAD_SPIC1_CSN_SMT: 1;
        uint16_t PAD_SPIC1_CSN_E3: 1;
        uint16_t PAD_SPIC1_CSN_E2: 1;
        uint16_t PAD_SPIC1_CSN_SHDN: 1;
        uint16_t AON_PAD_SPIC1_CSN_E: 1;
        uint16_t REG0X_PAD_SPIC1_CSN_DUMMY10: 1;
        uint16_t REG0X_PAD_SPIC1_CSN_DUMMY11: 1;
        uint16_t AON_PAD_SPIC1_CSN_O: 1;
        uint16_t PAD_SPIC1_CSN_PUPDC: 1;
        uint16_t PAD_SPIC1_CSN_PU: 1;
        uint16_t PAD_SPIC1_CSN_PU_EN: 1;
    };
} AON_FAST_REG0X_PAD_SPIC1_CSN_TYPE;

/* 0x15EA
    0       R/W REG0X_PAD_SPIC1_SCK_0_DUMMY0                1'b0
    1       R/W REG0X_PAD_SPIC1_SCK_0_DUMMY1                1'b0
    2       R/W REG1X_PAD_SPIC1_SCK_DUMMY2                  1'b0
    3       R/W REG1X_PAD_SPIC1_SCK_DUMMY3                  1'b1
    4       R/W PAD_SPIC1_SCK_S                             1'b0
    5       R/W PAD_SPIC1_SCK_SMT                           1'b0
    6       R/W PAD_SPIC1_SCK_E3                            1'b0
    7       R/W PAD_SPIC1_SCK_E2                            1'b0
    8       R/W PAD_SPIC1_SCK_SHDN                          1'b1
    9       R/W AON_PAD_SPIC1_SCK_E                         1'b0
    10      R/W REG1X_PAD_SPIC1_SCK_DUMMY10                 1'b0
    11      R/W REG1X_PAD_SPIC1_SCK_DUMMY11                 1'b0
    12      R/W AON_PAD_SPIC1_SCK_O                         1'b0
    13      R/W PAD_SPIC1_SCK_PUPDC                         1'b0
    14      R/W PAD_SPIC1_SCK_PU                            1'b0
    15      R/W PAD_SPIC1_SCK_PU_EN                         1'b1
 */
typedef union _AON_FAST_REG1X_PAD_SPIC1_SCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC1_SCK_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC1_SCK_0_DUMMY1: 1;
        uint16_t REG1X_PAD_SPIC1_SCK_DUMMY2: 1;
        uint16_t REG1X_PAD_SPIC1_SCK_DUMMY3: 1;
        uint16_t PAD_SPIC1_SCK_S: 1;
        uint16_t PAD_SPIC1_SCK_SMT: 1;
        uint16_t PAD_SPIC1_SCK_E3: 1;
        uint16_t PAD_SPIC1_SCK_E2: 1;
        uint16_t PAD_SPIC1_SCK_SHDN: 1;
        uint16_t AON_PAD_SPIC1_SCK_E: 1;
        uint16_t REG1X_PAD_SPIC1_SCK_DUMMY10: 1;
        uint16_t REG1X_PAD_SPIC1_SCK_DUMMY11: 1;
        uint16_t AON_PAD_SPIC1_SCK_O: 1;
        uint16_t PAD_SPIC1_SCK_PUPDC: 1;
        uint16_t PAD_SPIC1_SCK_PU: 1;
        uint16_t PAD_SPIC1_SCK_PU_EN: 1;
    };
} AON_FAST_REG1X_PAD_SPIC1_SCK_TYPE;

/* 0x15EC
    0       R/W REG0X_PAD_SPIC1_SIO0_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC1_SIO0_0_DUMMY1               1'b0
    2       R/W REG2X_PAD_SPIC1_SIO0_DUMMY2                 1'b0
    3       R/W REG2X_PAD_SPIC1_SIO0_DUMMY3                 1'b1
    4       R/W PAD_SPIC1_SIO0_S                            1'b0
    5       R/W PAD_SPIC1_SIO0_SMT                          1'b0
    6       R/W PAD_SPIC1_SIO0_E3                           1'b0
    7       R/W PAD_SPIC1_SIO0_E2                           1'b0
    8       R/W PAD_SPIC1_SIO0_SHDN                         1'b1
    9       R/W AON_PAD_SPIC1_SIO0_E                        1'b0
    10      R/W REG2X_PAD_SPIC1_SIO0_DUMMY10                1'b0
    11      R/W REG2X_PAD_SPIC1_SIO0_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC1_SIO0_O                        1'b0
    13      R/W PAD_SPIC1_SIO0_PUPDC                        1'b0
    14      R/W PAD_SPIC1_SIO0_PU                           1'b0
    15      R/W PAD_SPIC1_SIO0_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG2X_PAD_SPIC1_SIO0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC1_SIO0_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC1_SIO0_0_DUMMY1: 1;
        uint16_t REG2X_PAD_SPIC1_SIO0_DUMMY2: 1;
        uint16_t REG2X_PAD_SPIC1_SIO0_DUMMY3: 1;
        uint16_t PAD_SPIC1_SIO0_S: 1;
        uint16_t PAD_SPIC1_SIO0_SMT: 1;
        uint16_t PAD_SPIC1_SIO0_E3: 1;
        uint16_t PAD_SPIC1_SIO0_E2: 1;
        uint16_t PAD_SPIC1_SIO0_SHDN: 1;
        uint16_t AON_PAD_SPIC1_SIO0_E: 1;
        uint16_t REG2X_PAD_SPIC1_SIO0_DUMMY10: 1;
        uint16_t REG2X_PAD_SPIC1_SIO0_DUMMY11: 1;
        uint16_t AON_PAD_SPIC1_SIO0_O: 1;
        uint16_t PAD_SPIC1_SIO0_PUPDC: 1;
        uint16_t PAD_SPIC1_SIO0_PU: 1;
        uint16_t PAD_SPIC1_SIO0_PU_EN: 1;
    };
} AON_FAST_REG2X_PAD_SPIC1_SIO0_TYPE;

/* 0x15EE
    0       R/W REG0X_PAD_SPIC1_SIO1_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC1_SIO1_0_DUMMY1               1'b0
    2       R/W REG3X_PAD_SPIC1_SIO1_DUMMY2                 1'b0
    3       R/W REG3X_PAD_SPIC1_SIO1_DUMMY3                 1'b1
    4       R/W PAD_SPIC1_SIO1_S                            1'b0
    5       R/W PAD_SPIC1_SIO1_SMT                          1'b0
    6       R/W PAD_SPIC1_SIO1_E3                           1'b0
    7       R/W PAD_SPIC1_SIO1_E2                           1'b0
    8       R/W PAD_SPIC1_SIO1_SHDN                         1'b1
    9       R/W AON_PAD_SPIC1_SIO1_E                        1'b0
    10      R/W REG3X_PAD_SPIC1_SIO1_DUMMY10                1'b0
    11      R/W REG3X_PAD_SPIC1_SIO1_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC1_SIO1_O                        1'b0
    13      R/W PAD_SPIC1_SIO1_PUPDC                        1'b0
    14      R/W PAD_SPIC1_SIO1_PU                           1'b0
    15      R/W PAD_SPIC1_SIO1_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG3X_PAD_SPIC1_SIO1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC1_SIO1_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC1_SIO1_0_DUMMY1: 1;
        uint16_t REG3X_PAD_SPIC1_SIO1_DUMMY2: 1;
        uint16_t REG3X_PAD_SPIC1_SIO1_DUMMY3: 1;
        uint16_t PAD_SPIC1_SIO1_S: 1;
        uint16_t PAD_SPIC1_SIO1_SMT: 1;
        uint16_t PAD_SPIC1_SIO1_E3: 1;
        uint16_t PAD_SPIC1_SIO1_E2: 1;
        uint16_t PAD_SPIC1_SIO1_SHDN: 1;
        uint16_t AON_PAD_SPIC1_SIO1_E: 1;
        uint16_t REG3X_PAD_SPIC1_SIO1_DUMMY10: 1;
        uint16_t REG3X_PAD_SPIC1_SIO1_DUMMY11: 1;
        uint16_t AON_PAD_SPIC1_SIO1_O: 1;
        uint16_t PAD_SPIC1_SIO1_PUPDC: 1;
        uint16_t PAD_SPIC1_SIO1_PU: 1;
        uint16_t PAD_SPIC1_SIO1_PU_EN: 1;
    };
} AON_FAST_REG3X_PAD_SPIC1_SIO1_TYPE;

/* 0x15F0
    0       R/W REG0X_PAD_SPIC1_SIO2_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC1_SIO2_0_DUMMY1               1'b0
    2       R/W REG4X_PAD_SPIC1_SIO2_DUMMY2                 1'b0
    3       R/W REG4X_PAD_SPIC1_SIO2_DUMMY3                 1'b1
    4       R/W PAD_SPIC1_SIO2_S                            1'b0
    5       R/W PAD_SPIC1_SIO2_SMT                          1'b0
    6       R/W PAD_SPIC1_SIO2_E3                           1'b0
    7       R/W PAD_SPIC1_SIO2_E2                           1'b0
    8       R/W PAD_SPIC1_SIO2_SHDN                         1'b1
    9       R/W AON_PAD_SPIC1_SIO2_E                        1'b0
    10      R/W REG4X_PAD_SPIC1_SIO2_DUMMY10                1'b0
    11      R/W REG4X_PAD_SPIC1_SIO2_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC1_SIO2_O                        1'b0
    13      R/W PAD_SPIC1_SIO2_PUPDC                        1'b0
    14      R/W PAD_SPIC1_SIO2_PU                           1'b0
    15      R/W PAD_SPIC1_SIO2_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG4X_PAD_SPIC1_SIO2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC1_SIO2_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC1_SIO2_0_DUMMY1: 1;
        uint16_t REG4X_PAD_SPIC1_SIO2_DUMMY2: 1;
        uint16_t REG4X_PAD_SPIC1_SIO2_DUMMY3: 1;
        uint16_t PAD_SPIC1_SIO2_S: 1;
        uint16_t PAD_SPIC1_SIO2_SMT: 1;
        uint16_t PAD_SPIC1_SIO2_E3: 1;
        uint16_t PAD_SPIC1_SIO2_E2: 1;
        uint16_t PAD_SPIC1_SIO2_SHDN: 1;
        uint16_t AON_PAD_SPIC1_SIO2_E: 1;
        uint16_t REG4X_PAD_SPIC1_SIO2_DUMMY10: 1;
        uint16_t REG4X_PAD_SPIC1_SIO2_DUMMY11: 1;
        uint16_t AON_PAD_SPIC1_SIO2_O: 1;
        uint16_t PAD_SPIC1_SIO2_PUPDC: 1;
        uint16_t PAD_SPIC1_SIO2_PU: 1;
        uint16_t PAD_SPIC1_SIO2_PU_EN: 1;
    };
} AON_FAST_REG4X_PAD_SPIC1_SIO2_TYPE;

/* 0x15F2
    0       R/W REG0X_PAD_SPIC1_SIO3_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC1_SIO3_0_DUMMY1               1'b0
    2       R/W REG5X_PAD_SPIC1_SIO3_DUMMY2                 1'b0
    3       R/W REG5X_PAD_SPIC1_SIO3_DUMMY3                 1'b1
    4       R/W PAD_SPIC1_SIO3_S                            1'b0
    5       R/W PAD_SPIC1_SIO3_SMT                          1'b0
    6       R/W PAD_SPIC1_SIO3_E3                           1'b0
    7       R/W PAD_SPIC1_SIO3_E2                           1'b0
    8       R/W PAD_SPIC1_SIO3_SHDN                         1'b1
    9       R/W AON_PAD_SPIC1_SIO3_E                        1'b0
    10      R/W REG5X_PAD_SPIC1_SIO3_DUMMY10                1'b0
    11      R/W REG5X_PAD_SPIC1_SIO3_DUMMY11                1'b0
    12      R/W AON_PAD_SPIC1_SIO3_O                        1'b0
    13      R/W PAD_SPIC1_SIO3_PUPDC                        1'b0
    14      R/W PAD_SPIC1_SIO3_PU                           1'b0
    15      R/W PAD_SPIC1_SIO3_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG5X_PAD_SPIC1_SIO3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC1_SIO3_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC1_SIO3_0_DUMMY1: 1;
        uint16_t REG5X_PAD_SPIC1_SIO3_DUMMY2: 1;
        uint16_t REG5X_PAD_SPIC1_SIO3_DUMMY3: 1;
        uint16_t PAD_SPIC1_SIO3_S: 1;
        uint16_t PAD_SPIC1_SIO3_SMT: 1;
        uint16_t PAD_SPIC1_SIO3_E3: 1;
        uint16_t PAD_SPIC1_SIO3_E2: 1;
        uint16_t PAD_SPIC1_SIO3_SHDN: 1;
        uint16_t AON_PAD_SPIC1_SIO3_E: 1;
        uint16_t REG5X_PAD_SPIC1_SIO3_DUMMY10: 1;
        uint16_t REG5X_PAD_SPIC1_SIO3_DUMMY11: 1;
        uint16_t AON_PAD_SPIC1_SIO3_O: 1;
        uint16_t PAD_SPIC1_SIO3_PUPDC: 1;
        uint16_t PAD_SPIC1_SIO3_PU: 1;
        uint16_t PAD_SPIC1_SIO3_PU_EN: 1;
    };
} AON_FAST_REG5X_PAD_SPIC1_SIO3_TYPE;

/* 0x15F4
    0       R/W REG0X_PAD_SPIC2_CSN_0_DUMMY0                1'b0
    1       R/W REG0X_PAD_SPIC2_CSN_0_DUMMY1                1'b0
    2       R/W PAD_SPIC2_CSN_WKUP_INT_EN                   1'b0
    3       R/W REG0X_PAD_SPIC2_CSN_DUMMY3                  1'b1
    4       R/W PAD_SPIC2_CSN_S                             1'b0
    5       R/W PAD_SPIC2_CSN_SMT                           1'b0
    6       R/W PAD_SPIC2_CSN_E3                            1'b0
    7       R/W PAD_SPIC2_CSN_E2                            1'b0
    8       R/W PAD_SPIC2_CSN_SHDN                          1'b1
    9       R/W AON_PAD_SPIC2_CSN_E                         1'b0
    10      R/W PAD_SPIC2_CSN_WKPOL                         1'b0
    11      R/W PAD_SPIC2_CSN_WKEN                          1'b0
    12      R/W AON_PAD_SPIC2_CSN_O                         1'b0
    13      R/W PAD_SPIC2_CSN_PUPDC                         1'b0
    14      R/W PAD_SPIC2_CSN_PU                            1'b0
    15      R/W PAD_SPIC2_CSN_PU_EN                         1'b1
 */
typedef union _AON_FAST_REG0X_PAD_SPIC2_CSN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC2_CSN_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC2_CSN_0_DUMMY1: 1;
        uint16_t PAD_SPIC2_CSN_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_SPIC2_CSN_DUMMY3: 1;
        uint16_t PAD_SPIC2_CSN_S: 1;
        uint16_t PAD_SPIC2_CSN_SMT: 1;
        uint16_t PAD_SPIC2_CSN_E3: 1;
        uint16_t PAD_SPIC2_CSN_E2: 1;
        uint16_t PAD_SPIC2_CSN_SHDN: 1;
        uint16_t AON_PAD_SPIC2_CSN_E: 1;
        uint16_t PAD_SPIC2_CSN_WKPOL: 1;
        uint16_t PAD_SPIC2_CSN_WKEN: 1;
        uint16_t AON_PAD_SPIC2_CSN_O: 1;
        uint16_t PAD_SPIC2_CSN_PUPDC: 1;
        uint16_t PAD_SPIC2_CSN_PU: 1;
        uint16_t PAD_SPIC2_CSN_PU_EN: 1;
    };
} AON_FAST_REG0X_PAD_SPIC2_CSN_TYPE;

/* 0x15F6
    0       R/W REG0X_PAD_SPIC2_SCK_0_DUMMY0                1'b0
    1       R/W REG0X_PAD_SPIC2_SCK_0_DUMMY1                1'b0
    2       R/W PAD_SPIC2_SCK_WKUP_INT_EN                   1'b0
    3       R/W REG1X_PAD_SPIC2_SCK_DUMMY3                  1'b1
    4       R/W PAD_SPIC2_SCK_S                             1'b0
    5       R/W PAD_SPIC2_SCK_SMT                           1'b0
    6       R/W PAD_SPIC2_SCK_E3                            1'b0
    7       R/W PAD_SPIC2_SCK_E2                            1'b0
    8       R/W PAD_SPIC2_SCK_SHDN                          1'b1
    9       R/W AON_PAD_SPIC2_SCK_E                         1'b0
    10      R/W PAD_SPIC2_SCK_WKPOL                         1'b0
    11      R/W PAD_SPIC2_SCK_WKEN                          1'b0
    12      R/W AON_PAD_SPIC2_SCK_O                         1'b0
    13      R/W PAD_SPIC2_SCK_PUPDC                         1'b0
    14      R/W PAD_SPIC2_SCK_PU                            1'b0
    15      R/W PAD_SPIC2_SCK_PU_EN                         1'b1
 */
typedef union _AON_FAST_REG1X_PAD_SPIC2_SCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC2_SCK_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC2_SCK_0_DUMMY1: 1;
        uint16_t PAD_SPIC2_SCK_WKUP_INT_EN: 1;
        uint16_t REG1X_PAD_SPIC2_SCK_DUMMY3: 1;
        uint16_t PAD_SPIC2_SCK_S: 1;
        uint16_t PAD_SPIC2_SCK_SMT: 1;
        uint16_t PAD_SPIC2_SCK_E3: 1;
        uint16_t PAD_SPIC2_SCK_E2: 1;
        uint16_t PAD_SPIC2_SCK_SHDN: 1;
        uint16_t AON_PAD_SPIC2_SCK_E: 1;
        uint16_t PAD_SPIC2_SCK_WKPOL: 1;
        uint16_t PAD_SPIC2_SCK_WKEN: 1;
        uint16_t AON_PAD_SPIC2_SCK_O: 1;
        uint16_t PAD_SPIC2_SCK_PUPDC: 1;
        uint16_t PAD_SPIC2_SCK_PU: 1;
        uint16_t PAD_SPIC2_SCK_PU_EN: 1;
    };
} AON_FAST_REG1X_PAD_SPIC2_SCK_TYPE;

/* 0x15F8
    0       R/W REG0X_PAD_SPIC2_SIO0_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC2_SIO0_0_DUMMY1               1'b0
    2       R/W PAD_SPIC2_SIO0_WKUP_INT_EN                  1'b0
    3       R/W REG2X_PAD_SPIC2_SIO0_DUMMY3                 1'b1
    4       R/W PAD_SPIC2_SIO0_S                            1'b0
    5       R/W PAD_SPIC2_SIO0_SMT                          1'b0
    6       R/W PAD_SPIC2_SIO0_E3                           1'b0
    7       R/W PAD_SPIC2_SIO0_E2                           1'b0
    8       R/W PAD_SPIC2_SIO0_SHDN                         1'b1
    9       R/W AON_PAD_SPIC2_SIO0_E                        1'b0
    10      R/W PAD_SPIC2_SIO0_WKPOL                        1'b0
    11      R/W PAD_SPIC2_SIO0_WKEN                         1'b0
    12      R/W AON_PAD_SPIC2_SIO0_O                        1'b0
    13      R/W PAD_SPIC2_SIO0_PUPDC                        1'b0
    14      R/W PAD_SPIC2_SIO0_PU                           1'b0
    15      R/W PAD_SPIC2_SIO0_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG2X_PAD_SPIC2_SIO0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC2_SIO0_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC2_SIO0_0_DUMMY1: 1;
        uint16_t PAD_SPIC2_SIO0_WKUP_INT_EN: 1;
        uint16_t REG2X_PAD_SPIC2_SIO0_DUMMY3: 1;
        uint16_t PAD_SPIC2_SIO0_S: 1;
        uint16_t PAD_SPIC2_SIO0_SMT: 1;
        uint16_t PAD_SPIC2_SIO0_E3: 1;
        uint16_t PAD_SPIC2_SIO0_E2: 1;
        uint16_t PAD_SPIC2_SIO0_SHDN: 1;
        uint16_t AON_PAD_SPIC2_SIO0_E: 1;
        uint16_t PAD_SPIC2_SIO0_WKPOL: 1;
        uint16_t PAD_SPIC2_SIO0_WKEN: 1;
        uint16_t AON_PAD_SPIC2_SIO0_O: 1;
        uint16_t PAD_SPIC2_SIO0_PUPDC: 1;
        uint16_t PAD_SPIC2_SIO0_PU: 1;
        uint16_t PAD_SPIC2_SIO0_PU_EN: 1;
    };
} AON_FAST_REG2X_PAD_SPIC2_SIO0_TYPE;

/* 0x15FA
    0       R/W REG0X_PAD_SPIC2_SIO1_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC2_SIO1_0_DUMMY1               1'b0
    2       R/W PAD_SPIC2_SIO1_WKUP_INT_EN                  1'b0
    3       R/W REG3X_PAD_SPIC2_SIO1_DUMMY3                 1'b1
    4       R/W PAD_SPIC2_SIO1_S                            1'b0
    5       R/W PAD_SPIC2_SIO1_SMT                          1'b0
    6       R/W PAD_SPIC2_SIO1_E3                           1'b0
    7       R/W PAD_SPIC2_SIO1_E2                           1'b0
    8       R/W PAD_SPIC2_SIO1_SHDN                         1'b1
    9       R/W AON_PAD_SPIC2_SIO1_E                        1'b0
    10      R/W PAD_SPIC2_SIO1_WKPOL                        1'b0
    11      R/W PAD_SPIC2_SIO1_WKEN                         1'b0
    12      R/W AON_PAD_SPIC2_SIO1_O                        1'b0
    13      R/W PAD_SPIC2_SIO1_PUPDC                        1'b0
    14      R/W PAD_SPIC2_SIO1_PU                           1'b0
    15      R/W PAD_SPIC2_SIO1_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG3X_PAD_SPIC2_SIO1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC2_SIO1_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC2_SIO1_0_DUMMY1: 1;
        uint16_t PAD_SPIC2_SIO1_WKUP_INT_EN: 1;
        uint16_t REG3X_PAD_SPIC2_SIO1_DUMMY3: 1;
        uint16_t PAD_SPIC2_SIO1_S: 1;
        uint16_t PAD_SPIC2_SIO1_SMT: 1;
        uint16_t PAD_SPIC2_SIO1_E3: 1;
        uint16_t PAD_SPIC2_SIO1_E2: 1;
        uint16_t PAD_SPIC2_SIO1_SHDN: 1;
        uint16_t AON_PAD_SPIC2_SIO1_E: 1;
        uint16_t PAD_SPIC2_SIO1_WKPOL: 1;
        uint16_t PAD_SPIC2_SIO1_WKEN: 1;
        uint16_t AON_PAD_SPIC2_SIO1_O: 1;
        uint16_t PAD_SPIC2_SIO1_PUPDC: 1;
        uint16_t PAD_SPIC2_SIO1_PU: 1;
        uint16_t PAD_SPIC2_SIO1_PU_EN: 1;
    };
} AON_FAST_REG3X_PAD_SPIC2_SIO1_TYPE;

/* 0x15FC
    0       R/W REG0X_PAD_SPIC2_SIO2_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC2_SIO2_0_DUMMY1               1'b0
    2       R/W PAD_SPIC2_SIO2_WKUP_INT_EN                  1'b0
    3       R/W REG4X_PAD_SPIC2_SIO2_DUMMY3                 1'b1
    4       R/W PAD_SPIC2_SIO2_S                            1'b0
    5       R/W PAD_SPIC2_SIO2_SMT                          1'b0
    6       R/W PAD_SPIC2_SIO2_E3                           1'b0
    7       R/W PAD_SPIC2_SIO2_E2                           1'b0
    8       R/W PAD_SPIC2_SIO2_SHDN                         1'b1
    9       R/W AON_PAD_SPIC2_SIO2_E                        1'b0
    10      R/W PAD_SPIC2_SIO2_WKPOL                        1'b0
    11      R/W PAD_SPIC2_SIO2_WKEN                         1'b0
    12      R/W AON_PAD_SPIC2_SIO2_O                        1'b0
    13      R/W PAD_SPIC2_SIO2_PUPDC                        1'b0
    14      R/W PAD_SPIC2_SIO2_PU                           1'b0
    15      R/W PAD_SPIC2_SIO2_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG4X_PAD_SPIC2_SIO2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC2_SIO2_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC2_SIO2_0_DUMMY1: 1;
        uint16_t PAD_SPIC2_SIO2_WKUP_INT_EN: 1;
        uint16_t REG4X_PAD_SPIC2_SIO2_DUMMY3: 1;
        uint16_t PAD_SPIC2_SIO2_S: 1;
        uint16_t PAD_SPIC2_SIO2_SMT: 1;
        uint16_t PAD_SPIC2_SIO2_E3: 1;
        uint16_t PAD_SPIC2_SIO2_E2: 1;
        uint16_t PAD_SPIC2_SIO2_SHDN: 1;
        uint16_t AON_PAD_SPIC2_SIO2_E: 1;
        uint16_t PAD_SPIC2_SIO2_WKPOL: 1;
        uint16_t PAD_SPIC2_SIO2_WKEN: 1;
        uint16_t AON_PAD_SPIC2_SIO2_O: 1;
        uint16_t PAD_SPIC2_SIO2_PUPDC: 1;
        uint16_t PAD_SPIC2_SIO2_PU: 1;
        uint16_t PAD_SPIC2_SIO2_PU_EN: 1;
    };
} AON_FAST_REG4X_PAD_SPIC2_SIO2_TYPE;

/* 0x15FE
    0       R/W REG0X_PAD_SPIC2_SIO3_0_DUMMY0               1'b0
    1       R/W REG0X_PAD_SPIC2_SIO3_0_DUMMY1               1'b0
    2       R/W PAD_SPIC2_SIO3_WKUP_INT_EN                  1'b0
    3       R/W REG5X_PAD_SPIC2_SIO3_DUMMY3                 1'b1
    4       R/W PAD_SPIC2_SIO3_S                            1'b0
    5       R/W PAD_SPIC2_SIO3_SMT                          1'b0
    6       R/W PAD_SPIC2_SIO3_E3                           1'b0
    7       R/W PAD_SPIC2_SIO3_E2                           1'b0
    8       R/W PAD_SPIC2_SIO3_SHDN                         1'b1
    9       R/W AON_PAD_SPIC2_SIO3_E                        1'b0
    10      R/W PAD_SPIC2_SIO3_WKPOL                        1'b0
    11      R/W PAD_SPIC2_SIO3_WKEN                         1'b0
    12      R/W AON_PAD_SPIC2_SIO3_O                        1'b0
    13      R/W PAD_SPIC2_SIO3_PUPDC                        1'b0
    14      R/W PAD_SPIC2_SIO3_PU                           1'b0
    15      R/W PAD_SPIC2_SIO3_PU_EN                        1'b1
 */
typedef union _AON_FAST_REG5X_PAD_SPIC2_SIO3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_SPIC2_SIO3_0_DUMMY0: 1;
        uint16_t REG0X_PAD_SPIC2_SIO3_0_DUMMY1: 1;
        uint16_t PAD_SPIC2_SIO3_WKUP_INT_EN: 1;
        uint16_t REG5X_PAD_SPIC2_SIO3_DUMMY3: 1;
        uint16_t PAD_SPIC2_SIO3_S: 1;
        uint16_t PAD_SPIC2_SIO3_SMT: 1;
        uint16_t PAD_SPIC2_SIO3_E3: 1;
        uint16_t PAD_SPIC2_SIO3_E2: 1;
        uint16_t PAD_SPIC2_SIO3_SHDN: 1;
        uint16_t AON_PAD_SPIC2_SIO3_E: 1;
        uint16_t PAD_SPIC2_SIO3_WKPOL: 1;
        uint16_t PAD_SPIC2_SIO3_WKEN: 1;
        uint16_t AON_PAD_SPIC2_SIO3_O: 1;
        uint16_t PAD_SPIC2_SIO3_PUPDC: 1;
        uint16_t PAD_SPIC2_SIO3_PU: 1;
        uint16_t PAD_SPIC2_SIO3_PU_EN: 1;
    };
} AON_FAST_REG5X_PAD_SPIC2_SIO3_TYPE;

/* 0x1600
    0       R/W REG0X_PAD_MIC1_P_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_MIC1_P_0_DUMMY1                   1'b0
    2       R/W PAD_MIC1_P_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_MIC1_P_0_DUMMY3                   1'b1
    4       R/W PAD_MIC1_P_S                                1'b1
    5       R/W PAD_MIC1_P_SMT                              1'b0
    6       R/W PAD_MIC1_P_E3                               1'b0
    7       R/W PAD_MIC1_P_E2                               1'b0
    8       R/W PAD_MIC1_P_SHDN                             1'b1
    9       R/W AON_PAD_MIC1_P_E                            1'b0
    10      R/W PAD_MIC1_P_WKPOL                            1'b0
    11      R/W PAD_MIC1_P_WKEN                             1'b0
    12      R/W AON_PAD_MIC1_P_O                            1'b0
    13      R/W PAD_MIC1_P_PUPDC                            1'b0
    14      R/W PAD_MIC1_P_PU                               1'b0
    15      R/W PAD_MIC1_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG0X_PAD_MIC1_P_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MIC1_P_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MIC1_P_0_DUMMY1: 1;
        uint16_t PAD_MIC1_P_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MIC1_P_0_DUMMY3: 1;
        uint16_t PAD_MIC1_P_S: 1;
        uint16_t PAD_MIC1_P_SMT: 1;
        uint16_t PAD_MIC1_P_E3: 1;
        uint16_t PAD_MIC1_P_E2: 1;
        uint16_t PAD_MIC1_P_SHDN: 1;
        uint16_t AON_PAD_MIC1_P_E: 1;
        uint16_t PAD_MIC1_P_WKPOL: 1;
        uint16_t PAD_MIC1_P_WKEN: 1;
        uint16_t AON_PAD_MIC1_P_O: 1;
        uint16_t PAD_MIC1_P_PUPDC: 1;
        uint16_t PAD_MIC1_P_PU: 1;
        uint16_t PAD_MIC1_P_PU_EN: 1;
    };
} AON_FAST_REG0X_PAD_MIC1_P_TYPE;

/* 0x1602
    0       R/W REG0X_PAD_MIC1_N_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_MIC1_N_0_DUMMY1                   1'b0
    2       R/W PAD_MIC1_N_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_MIC1_N_0_DUMMY3                   1'b1
    4       R/W PAD_MIC1_N_S                                1'b1
    5       R/W PAD_MIC1_N_SMT                              1'b0
    6       R/W PAD_MIC1_N_E3                               1'b0
    7       R/W PAD_MIC1_N_E2                               1'b0
    8       R/W PAD_MIC1_N_SHDN                             1'b1
    9       R/W AON_PAD_MIC1_N_E                            1'b0
    10      R/W PAD_MIC1_N_WKPOL                            1'b0
    11      R/W PAD_MIC1_N_WKEN                             1'b0
    12      R/W AON_PAD_MIC1_N_O                            1'b0
    13      R/W PAD_MIC1_N_PUPDC                            1'b0
    14      R/W PAD_MIC1_N_PU                               1'b0
    15      R/W PAD_MIC1_N_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG1X_PAD_MIC1_N_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MIC1_N_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MIC1_N_0_DUMMY1: 1;
        uint16_t PAD_MIC1_N_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MIC1_N_0_DUMMY3: 1;
        uint16_t PAD_MIC1_N_S: 1;
        uint16_t PAD_MIC1_N_SMT: 1;
        uint16_t PAD_MIC1_N_E3: 1;
        uint16_t PAD_MIC1_N_E2: 1;
        uint16_t PAD_MIC1_N_SHDN: 1;
        uint16_t AON_PAD_MIC1_N_E: 1;
        uint16_t PAD_MIC1_N_WKPOL: 1;
        uint16_t PAD_MIC1_N_WKEN: 1;
        uint16_t AON_PAD_MIC1_N_O: 1;
        uint16_t PAD_MIC1_N_PUPDC: 1;
        uint16_t PAD_MIC1_N_PU: 1;
        uint16_t PAD_MIC1_N_PU_EN: 1;
    };
} AON_FAST_REG1X_PAD_MIC1_N_TYPE;

/* 0x1604
    0       R/W REG0X_PAD_MIC2_P_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_MIC2_P_0_DUMMY1                   1'b0
    2       R/W PAD_MIC2_P_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_MIC2_P_0_DUMMY3                   1'b1
    4       R/W PAD_MIC2_P_S                                1'b1
    5       R/W PAD_MIC2_P_SMT                              1'b0
    6       R/W PAD_MIC2_P_E3                               1'b0
    7       R/W PAD_MIC2_P_E2                               1'b0
    8       R/W PAD_MIC2_P_SHDN                             1'b1
    9       R/W AON_PAD_MIC2_P_E                            1'b0
    10      R/W PAD_MIC2_P_WKPOL                            1'b0
    11      R/W PAD_MIC2_P_WKEN                             1'b0
    12      R/W AON_PAD_MIC2_P_O                            1'b0
    13      R/W PAD_MIC2_P_PUPDC                            1'b0
    14      R/W PAD_MIC2_P_PU                               1'b0
    15      R/W PAD_MIC2_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG2X_PAD_MIC2_P_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MIC2_P_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MIC2_P_0_DUMMY1: 1;
        uint16_t PAD_MIC2_P_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MIC2_P_0_DUMMY3: 1;
        uint16_t PAD_MIC2_P_S: 1;
        uint16_t PAD_MIC2_P_SMT: 1;
        uint16_t PAD_MIC2_P_E3: 1;
        uint16_t PAD_MIC2_P_E2: 1;
        uint16_t PAD_MIC2_P_SHDN: 1;
        uint16_t AON_PAD_MIC2_P_E: 1;
        uint16_t PAD_MIC2_P_WKPOL: 1;
        uint16_t PAD_MIC2_P_WKEN: 1;
        uint16_t AON_PAD_MIC2_P_O: 1;
        uint16_t PAD_MIC2_P_PUPDC: 1;
        uint16_t PAD_MIC2_P_PU: 1;
        uint16_t PAD_MIC2_P_PU_EN: 1;
    };
} AON_FAST_REG2X_PAD_MIC2_P_TYPE;

/* 0x1606
    0       R/W REG0X_PAD_MIC2_N_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_MIC2_N_0_DUMMY1                   1'b0
    2       R/W PAD_MIC2_N_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_MIC2_N_0_DUMMY3                   1'b1
    4       R/W PAD_MIC2_N_S                                1'b1
    5       R/W PAD_MIC2_N_SMT                              1'b0
    6       R/W PAD_MIC2_N_E3                               1'b0
    7       R/W PAD_MIC2_N_E2                               1'b0
    8       R/W PAD_MIC2_N_SHDN                             1'b1
    9       R/W AON_PAD_MIC2_N_E                            1'b0
    10      R/W PAD_MIC2_N_WKPOL                            1'b0
    11      R/W PAD_MIC2_N_WKEN                             1'b0
    12      R/W AON_PAD_MIC2_N_O                            1'b0
    13      R/W PAD_MIC2_N_PUPDC                            1'b0
    14      R/W PAD_MIC2_N_PU                               1'b0
    15      R/W PAD_MIC2_N_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG3X_PAD_MIC2_N_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MIC2_N_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MIC2_N_0_DUMMY1: 1;
        uint16_t PAD_MIC2_N_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MIC2_N_0_DUMMY3: 1;
        uint16_t PAD_MIC2_N_S: 1;
        uint16_t PAD_MIC2_N_SMT: 1;
        uint16_t PAD_MIC2_N_E3: 1;
        uint16_t PAD_MIC2_N_E2: 1;
        uint16_t PAD_MIC2_N_SHDN: 1;
        uint16_t AON_PAD_MIC2_N_E: 1;
        uint16_t PAD_MIC2_N_WKPOL: 1;
        uint16_t PAD_MIC2_N_WKEN: 1;
        uint16_t AON_PAD_MIC2_N_O: 1;
        uint16_t PAD_MIC2_N_PUPDC: 1;
        uint16_t PAD_MIC2_N_PU: 1;
        uint16_t PAD_MIC2_N_PU_EN: 1;
    };
} AON_FAST_REG3X_PAD_MIC2_N_TYPE;

/* 0x1608
    0       R/W REG0X_PAD_MIC3_P_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_MIC3_P_0_DUMMY1                   1'b0
    2       R/W PAD_MIC3_P_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_MIC3_P_0_DUMMY3                   1'b1
    4       R/W PAD_MIC3_P_S                                1'b0
    5       R/W PAD_MIC3_P_SMT                              1'b0
    6       R/W PAD_MIC3_P_E3                               1'b0
    7       R/W PAD_MIC3_P_E2                               1'b0
    8       R/W PAD_MIC3_P_SHDN                             1'b1
    9       R/W AON_PAD_MIC3_P_E                            1'b0
    10      R/W PAD_MIC3_P_WKPOL                            1'b0
    11      R/W PAD_MIC3_P_WKEN                             1'b0
    12      R/W AON_PAD_MIC3_P_O                            1'b0
    13      R/W PAD_MIC3_P_PUPDC                            1'b0
    14      R/W PAD_MIC3_P_PU                               1'b0
    15      R/W PAD_MIC3_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG4X_PAD_MIC3_P_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MIC3_P_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MIC3_P_0_DUMMY1: 1;
        uint16_t PAD_MIC3_P_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MIC3_P_0_DUMMY3: 1;
        uint16_t PAD_MIC3_P_S: 1;
        uint16_t PAD_MIC3_P_SMT: 1;
        uint16_t PAD_MIC3_P_E3: 1;
        uint16_t PAD_MIC3_P_E2: 1;
        uint16_t PAD_MIC3_P_SHDN: 1;
        uint16_t AON_PAD_MIC3_P_E: 1;
        uint16_t PAD_MIC3_P_WKPOL: 1;
        uint16_t PAD_MIC3_P_WKEN: 1;
        uint16_t AON_PAD_MIC3_P_O: 1;
        uint16_t PAD_MIC3_P_PUPDC: 1;
        uint16_t PAD_MIC3_P_PU: 1;
        uint16_t PAD_MIC3_P_PU_EN: 1;
    };
} AON_FAST_REG4X_PAD_MIC3_P_TYPE;

/* 0x160A
    0       R/W REG0X_PAD_MIC3_N_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_MIC3_N_0_DUMMY1                   1'b0
    2       R/W PAD_MIC3_N_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_MIC3_N_0_DUMMY3                   1'b1
    4       R/W PAD_MIC3_N_S                                1'b0
    5       R/W PAD_MIC3_N_SMT                              1'b0
    6       R/W PAD_MIC3_N_E3                               1'b0
    7       R/W PAD_MIC3_N_E2                               1'b0
    8       R/W PAD_MIC3_N_SHDN                             1'b1
    9       R/W AON_PAD_MIC3_N_E                            1'b0
    10      R/W PAD_MIC3_N_WKPOL                            1'b0
    11      R/W PAD_MIC3_N_WKEN                             1'b0
    12      R/W AON_PAD_MIC3_N_O                            1'b0
    13      R/W PAD_MIC3_N_PUPDC                            1'b0
    14      R/W PAD_MIC3_N_PU                               1'b0
    15      R/W PAD_MIC3_N_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG5X_PAD_MIC3_N_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MIC3_N_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MIC3_N_0_DUMMY1: 1;
        uint16_t PAD_MIC3_N_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MIC3_N_0_DUMMY3: 1;
        uint16_t PAD_MIC3_N_S: 1;
        uint16_t PAD_MIC3_N_SMT: 1;
        uint16_t PAD_MIC3_N_E3: 1;
        uint16_t PAD_MIC3_N_E2: 1;
        uint16_t PAD_MIC3_N_SHDN: 1;
        uint16_t AON_PAD_MIC3_N_E: 1;
        uint16_t PAD_MIC3_N_WKPOL: 1;
        uint16_t PAD_MIC3_N_WKEN: 1;
        uint16_t AON_PAD_MIC3_N_O: 1;
        uint16_t PAD_MIC3_N_PUPDC: 1;
        uint16_t PAD_MIC3_N_PU: 1;
        uint16_t PAD_MIC3_N_PU_EN: 1;
    };
} AON_FAST_REG5X_PAD_MIC3_N_TYPE;

/* 0x160C
    0       R/W REG0X_PAD_LOUT_P_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_LOUT_P_0_DUMMY1                   1'b0
    2       R/W PAD_LOUT_P_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_LOUT_P_0_DUMMY3                   1'b1
    4       R/W PAD_LOUT_P_S                                1'b0
    5       R/W PAD_LOUT_P_SMT                              1'b0
    6       R/W PAD_LOUT_P_E3                               1'b0
    7       R/W PAD_LOUT_P_E2                               1'b0
    8       R/W PAD_LOUT_P_SHDN                             1'b1
    9       R/W AON_PAD_LOUT_P_E                            1'b0
    10      R/W PAD_LOUT_P_WKPOL                            1'b0
    11      R/W PAD_LOUT_P_WKEN                             1'b0
    12      R/W AON_PAD_LOUT_P_O                            1'b0
    13      R/W PAD_LOUT_P_PUPDC                            1'b0
    14      R/W PAD_LOUT_P_PU                               1'b0
    15      R/W PAD_LOUT_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG6X_PAD_LOUT_P_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_LOUT_P_0_DUMMY0: 1;
        uint16_t REG0X_PAD_LOUT_P_0_DUMMY1: 1;
        uint16_t PAD_LOUT_P_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_LOUT_P_0_DUMMY3: 1;
        uint16_t PAD_LOUT_P_S: 1;
        uint16_t PAD_LOUT_P_SMT: 1;
        uint16_t PAD_LOUT_P_E3: 1;
        uint16_t PAD_LOUT_P_E2: 1;
        uint16_t PAD_LOUT_P_SHDN: 1;
        uint16_t AON_PAD_LOUT_P_E: 1;
        uint16_t PAD_LOUT_P_WKPOL: 1;
        uint16_t PAD_LOUT_P_WKEN: 1;
        uint16_t AON_PAD_LOUT_P_O: 1;
        uint16_t PAD_LOUT_P_PUPDC: 1;
        uint16_t PAD_LOUT_P_PU: 1;
        uint16_t PAD_LOUT_P_PU_EN: 1;
    };
} AON_FAST_REG6X_PAD_LOUT_P_TYPE;

/* 0x160E
    0       R/W REG0X_PAD_LOUT_N_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_LOUT_N_0_DUMMY1                   1'b0
    2       R/W PAD_LOUT_N_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_LOUT_N_0_DUMMY3                   1'b1
    4       R/W PAD_LOUT_N_S                                1'b0
    5       R/W PAD_LOUT_N_SMT                              1'b0
    6       R/W PAD_LOUT_N_E3                               1'b0
    7       R/W PAD_LOUT_N_E2                               1'b0
    8       R/W PAD_LOUT_N_SHDN                             1'b1
    9       R/W AON_PAD_LOUT_N_E                            1'b0
    10      R/W PAD_LOUT_N_WKPOL                            1'b0
    11      R/W PAD_LOUT_N_WKEN                             1'b0
    12      R/W AON_PAD_LOUT_N_O                            1'b0
    13      R/W PAD_LOUT_N_PUPDC                            1'b0
    14      R/W PAD_LOUT_N_PU                               1'b0
    15      R/W PAD_LOUT_N_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG7X_PAD_LOUT_N_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_LOUT_N_0_DUMMY0: 1;
        uint16_t REG0X_PAD_LOUT_N_0_DUMMY1: 1;
        uint16_t PAD_LOUT_N_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_LOUT_N_0_DUMMY3: 1;
        uint16_t PAD_LOUT_N_S: 1;
        uint16_t PAD_LOUT_N_SMT: 1;
        uint16_t PAD_LOUT_N_E3: 1;
        uint16_t PAD_LOUT_N_E2: 1;
        uint16_t PAD_LOUT_N_SHDN: 1;
        uint16_t AON_PAD_LOUT_N_E: 1;
        uint16_t PAD_LOUT_N_WKPOL: 1;
        uint16_t PAD_LOUT_N_WKEN: 1;
        uint16_t AON_PAD_LOUT_N_O: 1;
        uint16_t PAD_LOUT_N_PUPDC: 1;
        uint16_t PAD_LOUT_N_PU: 1;
        uint16_t PAD_LOUT_N_PU_EN: 1;
    };
} AON_FAST_REG7X_PAD_LOUT_N_TYPE;

/* 0x1610
    0       R/W REG0X_PAD_ROUT_P_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_ROUT_P_0_DUMMY1                   1'b0
    2       R/W PAD_ROUT_P_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_ROUT_P_0_DUMMY3                   1'b1
    4       R/W PAD_ROUT_P_S                                1'b0
    5       R/W PAD_ROUT_P_SMT                              1'b0
    6       R/W PAD_ROUT_P_E3                               1'b0
    7       R/W PAD_ROUT_P_E2                               1'b0
    8       R/W PAD_ROUT_P_SHDN                             1'b1
    9       R/W AON_PAD_ROUT_P_E                            1'b0
    10      R/W PAD_ROUT_P_WKPOL                            1'b0
    11      R/W PAD_ROUT_P_WKEN                             1'b0
    12      R/W AON_PAD_ROUT_P_O                            1'b0
    13      R/W PAD_ROUT_P_PUPDC                            1'b0
    14      R/W PAD_ROUT_P_PU                               1'b0
    15      R/W PAD_ROUT_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG8X_PAD_ROUT_P_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_ROUT_P_0_DUMMY0: 1;
        uint16_t REG0X_PAD_ROUT_P_0_DUMMY1: 1;
        uint16_t PAD_ROUT_P_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_ROUT_P_0_DUMMY3: 1;
        uint16_t PAD_ROUT_P_S: 1;
        uint16_t PAD_ROUT_P_SMT: 1;
        uint16_t PAD_ROUT_P_E3: 1;
        uint16_t PAD_ROUT_P_E2: 1;
        uint16_t PAD_ROUT_P_SHDN: 1;
        uint16_t AON_PAD_ROUT_P_E: 1;
        uint16_t PAD_ROUT_P_WKPOL: 1;
        uint16_t PAD_ROUT_P_WKEN: 1;
        uint16_t AON_PAD_ROUT_P_O: 1;
        uint16_t PAD_ROUT_P_PUPDC: 1;
        uint16_t PAD_ROUT_P_PU: 1;
        uint16_t PAD_ROUT_P_PU_EN: 1;
    };
} AON_FAST_REG8X_PAD_ROUT_P_TYPE;

/* 0x1612
    0       R/W REG0X_PAD_ROUT_N_0_DUMMY0                   1'b0
    1       R/W REG0X_PAD_ROUT_N_0_DUMMY1                   1'b0
    2       R/W PAD_ROUT_N_WKUP_INT_EN                      1'b0
    3       R/W REG0X_PAD_ROUT_N_0_DUMMY3                   1'b1
    4       R/W PAD_ROUT_N_S                                1'b0
    5       R/W PAD_ROUT_N_SMT                              1'b0
    6       R/W PAD_ROUT_N_E3                               1'b0
    7       R/W PAD_ROUT_N_E2                               1'b0
    8       R/W PAD_ROUT_N_SHDN                             1'b1
    9       R/W AON_PAD_ROUT_N_E                            1'b0
    10      R/W PAD_ROUT_N_WKPOL                            1'b0
    11      R/W PAD_ROUT_N_WKEN                             1'b0
    12      R/W AON_PAD_ROUT_N_O                            1'b0
    13      R/W PAD_ROUT_N_PUPDC                            1'b0
    14      R/W PAD_ROUT_N_PU                               1'b0
    15      R/W PAD_ROUT_N_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG9X_PAD_ROUT_N_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_ROUT_N_0_DUMMY0: 1;
        uint16_t REG0X_PAD_ROUT_N_0_DUMMY1: 1;
        uint16_t PAD_ROUT_N_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_ROUT_N_0_DUMMY3: 1;
        uint16_t PAD_ROUT_N_S: 1;
        uint16_t PAD_ROUT_N_SMT: 1;
        uint16_t PAD_ROUT_N_E3: 1;
        uint16_t PAD_ROUT_N_E2: 1;
        uint16_t PAD_ROUT_N_SHDN: 1;
        uint16_t AON_PAD_ROUT_N_E: 1;
        uint16_t PAD_ROUT_N_WKPOL: 1;
        uint16_t PAD_ROUT_N_WKEN: 1;
        uint16_t AON_PAD_ROUT_N_O: 1;
        uint16_t PAD_ROUT_N_PUPDC: 1;
        uint16_t PAD_ROUT_N_PU: 1;
        uint16_t PAD_ROUT_N_PU_EN: 1;
    };
} AON_FAST_REG9X_PAD_ROUT_N_TYPE;

/* 0x1614
    0       R/W REG0X_PAD_AUX_L_0_DUMMY0                    1'b0
    1       R/W REG0X_PAD_AUX_L_0_DUMMY1                    1'b0
    2       R/W PAD_AUX_L_WKUP_INT_EN                       1'b0
    3       R/W REG0X_PAD_AUX_L_0_DUMMY3                    1'b1
    4       R/W PAD_AUX_L_S                                 1'b0
    5       R/W PAD_AUX_L_SMT                               1'b0
    6       R/W PAD_AUX_L_E3                                1'b0
    7       R/W PAD_AUX_L_E2                                1'b0
    8       R/W PAD_AUX_L_SHDN                              1'b1
    9       R/W AON_PAD_AUX_L_E                             1'b0
    10      R/W PAD_AUX_L_WKPOL                             1'b0
    11      R/W PAD_AUX_L_WKEN                              1'b0
    12      R/W AON_PAD_AUX_L_O                             1'b0
    13      R/W PAD_AUX_L_PUPDC                             1'b0
    14      R/W PAD_AUX_L_PU                                1'b0
    15      R/W PAD_AUX_L_PU_EN                             1'b1
 */
typedef union _AON_FAST_REG10X_PAD_AUX_L_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_AUX_L_0_DUMMY0: 1;
        uint16_t REG0X_PAD_AUX_L_0_DUMMY1: 1;
        uint16_t PAD_AUX_L_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_AUX_L_0_DUMMY3: 1;
        uint16_t PAD_AUX_L_S: 1;
        uint16_t PAD_AUX_L_SMT: 1;
        uint16_t PAD_AUX_L_E3: 1;
        uint16_t PAD_AUX_L_E2: 1;
        uint16_t PAD_AUX_L_SHDN: 1;
        uint16_t AON_PAD_AUX_L_E: 1;
        uint16_t PAD_AUX_L_WKPOL: 1;
        uint16_t PAD_AUX_L_WKEN: 1;
        uint16_t AON_PAD_AUX_L_O: 1;
        uint16_t PAD_AUX_L_PUPDC: 1;
        uint16_t PAD_AUX_L_PU: 1;
        uint16_t PAD_AUX_L_PU_EN: 1;
    };
} AON_FAST_REG10X_PAD_AUX_L_TYPE;

/* 0x1616
    0       R/W REG0X_PAD_AUX_R_0_DUMMY0                    1'b0
    1       R/W REG0X_PAD_AUX_R_0_DUMMY1                    1'b0
    2       R/W PAD_AUX_R_WKUP_INT_EN                       1'b0
    3       R/W REG0X_PAD_AUX_R_0_DUMMY3                    1'b1
    4       R/W PAD_AUX_R_S                                 1'b0
    5       R/W PAD_AUX_R_SMT                               1'b0
    6       R/W PAD_AUX_R_E3                                1'b0
    7       R/W PAD_AUX_R_E2                                1'b0
    8       R/W PAD_AUX_R_SHDN                              1'b1
    9       R/W AON_PAD_AUX_R_E                             1'b0
    10      R/W PAD_AUX_R_WKPOL                             1'b0
    11      R/W PAD_AUX_R_WKEN                              1'b0
    12      R/W AON_PAD_AUX_R_O                             1'b0
    13      R/W PAD_AUX_R_PUPDC                             1'b0
    14      R/W PAD_AUX_R_PU                                1'b0
    15      R/W PAD_AUX_R_PU_EN                             1'b1
 */
typedef union _AON_FAST_REG11X_PAD_AUX_R_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_AUX_R_0_DUMMY0: 1;
        uint16_t REG0X_PAD_AUX_R_0_DUMMY1: 1;
        uint16_t PAD_AUX_R_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_AUX_R_0_DUMMY3: 1;
        uint16_t PAD_AUX_R_S: 1;
        uint16_t PAD_AUX_R_SMT: 1;
        uint16_t PAD_AUX_R_E3: 1;
        uint16_t PAD_AUX_R_E2: 1;
        uint16_t PAD_AUX_R_SHDN: 1;
        uint16_t AON_PAD_AUX_R_E: 1;
        uint16_t PAD_AUX_R_WKPOL: 1;
        uint16_t PAD_AUX_R_WKEN: 1;
        uint16_t AON_PAD_AUX_R_O: 1;
        uint16_t PAD_AUX_R_PUPDC: 1;
        uint16_t PAD_AUX_R_PU: 1;
        uint16_t PAD_AUX_R_PU_EN: 1;
    };
} AON_FAST_REG11X_PAD_AUX_R_TYPE;

/* 0x1618
    0       R/W REG0X_PAD_MICBIAS_0_DUMMY0                  1'b0
    1       R/W REG0X_PAD_MICBIAS_0_DUMMY1                  1'b0
    2       R/W PAD_MICBIAS_WKUP_INT_EN                     1'b0
    3       R/W REG0X_PAD_MICBIAS_0_DUMMY3                  1'b1
    4       R/W PAD_MICBIAS_S                               1'b0
    5       R/W PAD_MICBIAS_SMT                             1'b0
    6       R/W PAD_MICBIAS_E3                              1'b0
    7       R/W PAD_MICBIAS_E2                              1'b0
    8       R/W PAD_MICBIAS_SHDN                            1'b1
    9       R/W AON_PAD_MICBIAS_E                           1'b0
    10      R/W PAD_MICBIAS_WKPOL                           1'b0
    11      R/W PAD_MICBIAS_WKEN                            1'b0
    12      R/W AON_PAD_MICBIAS_O                           1'b0
    13      R/W PAD_MICBIAS_PUPDC                           1'b0
    14      R/W PAD_MICBIAS_PU                              1'b0
    15      R/W PAD_MICBIAS_PU_EN                           1'b1
 */
typedef union _AON_FAST_REG12X_PAD_MICBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_MICBIAS_0_DUMMY0: 1;
        uint16_t REG0X_PAD_MICBIAS_0_DUMMY1: 1;
        uint16_t PAD_MICBIAS_WKUP_INT_EN: 1;
        uint16_t REG0X_PAD_MICBIAS_0_DUMMY3: 1;
        uint16_t PAD_MICBIAS_S: 1;
        uint16_t PAD_MICBIAS_SMT: 1;
        uint16_t PAD_MICBIAS_E3: 1;
        uint16_t PAD_MICBIAS_E2: 1;
        uint16_t PAD_MICBIAS_SHDN: 1;
        uint16_t AON_PAD_MICBIAS_E: 1;
        uint16_t PAD_MICBIAS_WKPOL: 1;
        uint16_t PAD_MICBIAS_WKEN: 1;
        uint16_t AON_PAD_MICBIAS_O: 1;
        uint16_t PAD_MICBIAS_PUPDC: 1;
        uint16_t PAD_MICBIAS_PU: 1;
        uint16_t PAD_MICBIAS_PU_EN: 1;
    };
} AON_FAST_REG12X_PAD_MICBIAS_TYPE;

/* 0x161A
    0       R/W REG0X_PAD_P10_0_DUMMY0                      1'b0
    1       R/W REG0X_PAD_P10_0_DUMMY1                      1'b0
    2       R/W PAD_P10_WKUP_INT_EN[0]                      1'b0
    3       R/W REG0X_PAD_P10_0_DUMMY3                      1'b1
    4       R/W PAD_P10_S[0]                                1'b0
    5       R/W PAD_P10_SMT[0]                              1'b0
    6       R/W REG0X_PAD_P10_0_DUMMY6                      1'b0
    7       R/W PAD_P10_E2[0]                               1'b0
    8       R/W PAD_P10_SHDN[0]                             1'b0
    9       R/W AON_PAD_P10_E[0]                            1'b0
    10      R/W PAD_P10_WKPOL[0]                            1'b0
    11      R/W PAD_P10_WKEN[0]                             1'b0
    12      R/W AON_PAD_P10_O[0]                            1'b0
    13      R/W PAD_P10_PUPDC[0]                            1'b0
    14      R/W PAD_P10_PU[0]                               1'b0
    15      R/W PAD_P10_PU_EN[0]                            1'b1
 */
typedef union _AON_FAST_REG0X_PAD_P10_0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG0X_PAD_P10_0_DUMMY0: 1;
        uint16_t REG0X_PAD_P10_0_DUMMY1: 1;
        uint16_t PAD_P10_WKUP_INT_EN_0: 1;
        uint16_t REG0X_PAD_P10_0_DUMMY3: 1;
        uint16_t PAD_P10_S_0: 1;
        uint16_t PAD_P10_SMT_0: 1;
        uint16_t REG0X_PAD_P10_0_DUMMY6: 1;
        uint16_t PAD_P10_E2_0: 1;
        uint16_t PAD_P10_SHDN_0: 1;
        uint16_t AON_PAD_P10_E_0: 1;
        uint16_t PAD_P10_WKPOL_0: 1;
        uint16_t PAD_P10_WKEN_0: 1;
        uint16_t AON_PAD_P10_O_0: 1;
        uint16_t PAD_P10_PUPDC_0: 1;
        uint16_t PAD_P10_PU_0: 1;
        uint16_t PAD_P10_PU_EN_0: 1;
    };
} AON_FAST_REG0X_PAD_P10_0_TYPE;


#endif
