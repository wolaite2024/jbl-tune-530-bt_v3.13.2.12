/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_hw_aes.h
* @brief
* @details
* @author    eason li
* @date      2016-01-04
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef __RTL876X_HW_AES_H
#define __RTL876X_HW_AES_H

#ifdef __cpluspuls
extern "C" {
#endif
    /* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "rtl876x.h"
// *INDENT-OFF*
/**
  * @brief referenc to hw aes register table
  */
typedef struct
{
    union
    {
        __IO uint32_t CTL;
        struct
        {
            __IO uint32_t enc_en:         1; /* aes encryption enable */
            __IO uint32_t dec_en:         1; /* aes decryption enable */
            __IO uint32_t ase256_en:      1; /* aes 256-bit mode enable */
            __IO uint32_t aes_mode_sel:   3; /* aes mode select, see HW_AES_MODE below */
            __IO uint32_t access_mode:    1; /* 0: CPU mode, 1: DMA mode */
            __IO uint32_t scram_en:       1; /* scramble function, 0:disable, 1:enable */
            __IO uint32_t use_hidden_key: 1; /* secure related */
            __IO uint32_t copy_hidden_key: 1; /* secure related */
            __IO uint32_t hidden_256:     1; /* secure related */
            __IO uint32_t poly_en:        1; /* XOR polynomial, 0:disable, 1:enable */
            __IO uint32_t rsvd0:          2;
            __IO uint32_t ctr_mode:       2;
            __IO uint32_t rsvd1:          15;
            __IO uint32_t dout_rdy:       1; /* aes data output ready signal used in CPU mode.
                                          when data output is ready, dout_rdy = 1.
                                          if dout_rdy == 1, "CPU reads enc_dout or dec_dout
                                          for 4 times (128 bits)" will clear dout_rdy to 0.
                                        */
        } CTL_BITS;
    };
    __IO uint32_t data_in;
    __I  uint32_t enc_dout;
    __I  uint32_t dec_dout;
    __IO uint32_t IRK[8];
    __IO uint32_t iv[4];
} HW_AES_TypeDef;

#define HWAES_CTL       0x0
#define HWAES_DATAIN    0x4
#define HWAES_ENC_DOUT  0x8
#define HWAES_DEC_DOUT  0xC
#define HWAES_IRK       0x10
#define HWAES_IV        0x30
#define HWAES_SEC_REG   0x40
#define HWAES_SEC_STS   0x74
#define HWAES_AUTHEN    0x78

typedef enum
{
    AES_NONE,
    AES_CBC,
    AES_ECB,
    AES_CFB,
    AES_OFB,
    AES_CTR
} HW_AES_WORK_MODE;

typedef enum
{
    HW_AES_CPU_MODE,
    HW_AES_DMA_MODE
} HW_AES_ACCESS_MODE;

typedef enum
{
    CTR_IV_64BITS_MSB,
    CTR_IV_96BITS_MSB,
    CTR_IV_64BITS_LSB,
    CTR_IV_96BITS_LSB,
} HW_AES_CTR_MODE;

#define HW_AES_RX_DMA_IO_NUM 14
#define HW_AES_TX_DMA_IO_NUM 13

#define HW_AES_SET_ENC_EN(isEnable)         (HWAES->CTL_BITS.enc_en = isEnable)
#define HW_AES_SET_DEC_EN(isEnable)         (HWAES->CTL_BITS.dec_en = isEnable)
#define HW_AES_SET_256_EN(isEnable)         (HWAES->CTL_BITS.ase256_en = isEnable)
#define HW_AES_GET_256_EN                   (HWAES->CTL_BITS.ase256_en)
#define HW_AES_SET_AES_MODE(mode)           (HWAES->CTL_BITS.aes_mode_sel = (mode & 0x7))
#define HW_AES_SET_WORK_MODE(mode)          (HWAES->CTL_BITS.access_mode = (mode & BIT0))
#define HW_AES_IS_DATA_OUT_READY            (HWAES->CTL_BITS.dout_rdy)
#define HW_AES_SET_CTR_MODE(mode)           (HWAES->CTL_BITS.ctr_mode = mode)
#define HW_AES_GET_CTR_MODE                 (HWAES->CTL_BITS.ctr_mode)
#define HW_AES_SET_INPUT_DATA(data)         (HWAES->data_in = (uint32_t)data)
#define HW_AES_READ_ENC_OUTPUT(Out)         (Out = HWAES->enc_dout)
#define HW_AES_READ_DEC_OUTPUT(Out)         (Out = HWAES->dec_dout)
#define HW_AES_SET_IRK(pIRK, cnt)           for (uint8_t i = 0;i < cnt;i ++) {HWAES->IRK[(cnt - 1) - i] = pIRK[i];}
#define HW_AES_GET_IRK(pIRK, cnt)           for (uint8_t i = 0;i < cnt;i ++) {pIRK[i] = HWAES->IRK[(cnt - 1) - i];}
#define HW_AES_SET_IV(pIV)                  for (uint8_t i = 0;i < 4;i ++) {HWAES->iv[3 - i] = pIV[i];}
#define HW_AES_GET_IV(pIV)                  for (uint8_t i = 0;i < 4;i ++) {pIV[i] = HWAES->iv[3 - i];}
#define HW_AES_SET_SCRAMBLE_EN(isEnable)    (HWAES->CTL_BITS.scram_en = isEnable)
#define HW_AES_USE_HIDDEN_KEY(isEnable)     (HWAES->CTL_BITS.use_hidden_key = isEnable)
#define HW_AES_COPY_HIDDEN_KEY(isEnable)    (HWAES->CTL_BITS.copy_hidden_key = isEnable)
#define HW_AES_SET_HIDDEN_256(isEnable)     (HWAES->CTL_BITS.hidden_256 = isEnable)

#define HWAES_DMA_RX_CH_NUM 6
#define HWAES_DMA_TX_CH_NUM 7
#define MAX_DMA_BUF_SZ 0xFF0

#if 1
#define AES_INFO(...) DIRECT_LOG(__VA_ARGS__);
#else
#define AES_INFO(...)
#endif

typedef void (*AES_DMA_CB)(void *);

static __forceinline void hw_aes_clear(void)
{
    HWAES->CTL = 0;
}
static __forceinline void hw_aes_set_clk(bool is_enable)
{
    /* turn on hw aes clock */
    SYSBLKCTRL->u_238.BITS_238.BIT_SOC_ACTCK_AES_EN = is_enable;
    SYSBLKCTRL->u_238.BITS_238.BIT_SOC_SLPCK_AES_EN = is_enable;

    /* enable hw aes */
    SYSBLKCTRL->u_218.BITS_218.BIT_PERI_AES_EN = is_enable;
}

extern bool (*hw_aes_init)(const uint32_t *aesKey, uint32_t *iv, HW_AES_WORK_MODE work_mode, bool isAes256);
extern bool (*hw_aes_cpu_mode)(uint32_t *in, uint32_t *out, uint32_t word_len, bool isEncrypt, bool isMac);
extern bool (*hw_aes_dma_operate)(uint32_t *in, uint32_t *out, uint32_t word_len, bool isEncrypt, uint8_t dma_rx_ch_num, uint8_t dma_tx_ch_num, bool isMac, AES_DMA_CB cb, void *parameter);
extern bool (*hw_aes_cmac128_updata)(uint32_t *p_in, uint32_t *p_out, uint32_t word_len, const uint32_t *p_aes_key, int mode);

void hw_aes_set_ctr_mode(HW_AES_CTR_MODE mode);
bool hw_aes_cpu_operate(uint32_t *in, uint32_t *out, uint32_t word_len, bool isEncrypt);

void hw_aes_set_dma_rx_done(bool isDone);
bool hw_aes_is_dma_rx_done(void);
void hw_aes_set_dma_tx_done(bool isDone);
bool hw_aes_is_dma_tx_done(void);
bool hw_aes_dma_done(void);
void hw_aes_set_dma_move_src(uint32_t src);
void hw_aes_set_dma_move_dst(uint32_t dst);
void hw_aes_set_dma_carry_size(uint32_t size);
bool hw_aes_cmac(uint8_t *key, uint8_t *input, uint32_t length, uint8_t *mac, int mode);
void xor_128(const unsigned char *a, const unsigned char *b, unsigned char *out);
void leftshift_onebit(unsigned char *input, unsigned char *output);
void padding(unsigned char *lastb, unsigned char *pad, int length);
// *INDENT-ON*
#ifdef __cplusplus
}
#endif
#endif /*__RTL8762X_GDMA_H*/
