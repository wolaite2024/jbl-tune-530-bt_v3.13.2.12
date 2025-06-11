;/*****************************************************************************/
;/*     startup_rtl8763.s: Startup file for Realtek RTL8763 device series     */
;/*****************************************************************************/
#include "mem_config.h"

__initial_sp    EQU     APP_GLOBAL_ADDR

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    VECTOR, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     System_Handler            ;[0]  System On interrupt
                DCD     WDT_Handler               ;[1]  Watch dog global insterrupt
                DCD     BTMAC_Handler             ;[2]  See Below Table ( an Extension of interrupt )
                DCD     USB_IP_Handler  ;[3]  Master Peripheral IP interrupt (usb or sdio)
                DCD     TIM2_Handler              ;[4]  Timer2 global interrupt
                DCD     Platform_Handler          ;[5]  Platform interrupt (platfrom error interrupt)
                DCD     I2S0_TX_Handler            ;[6]  I2S0 interrupt
                DCD     I2S0_RX_Handler            ;[7]  I2S1 interrupt
                DCD     UART2_Handler             ;[8]  Data_Uart1 interrupt (used for DSP)
                DCD     GPIOA0_Handler            ;[9]  GPIOA0 interrupt
                DCD     GPIOA1_Handler            ;[10] GPIOA1 interrupt
                DCD     UART1_Handler             ;[11] Log_Uart interrupt (Log0)
                DCD     UART0_Handler             ;[12] Data_Uart interrupt
                DCD     RTC_Handler               ;[13] Realtime counter interrupt
                DCD     SPI0_Handler              ;[14] SPI0 interrupt
                DCD     SPI1_Handler              ;[15] SPI1 interrupt
                DCD     I2C0_Handler              ;[16] I2C0 interrupt
                DCD     I2C1_Handler              ;[17] I2C1 interrupt
                DCD     ADC_Handler               ;[18] ADC global interrupt
                DCD     Peripheral_Handler        ;[19] See Below Table ( an Extension of interrupt )
                DCD     GDMA0_Channel0_Handler    ;[20] RTK-DMA0 channel 0 global interrupt
                DCD     GDMA0_Channel1_Handler    ;[21] RTK-DMA0 channel 1 global interrupt
                DCD     GDMA0_Channel2_Handler    ;[22] RTK-DMA0 channel 2 global interrupt
                DCD     GDMA0_Channel3_Handler    ;[23] RTK-DMA0 channel 3 global interrupt
                DCD     GDMA0_Channel4_Handler    ;[24] RTK-DMA0 channel 4 global interrupt
                DCD     GDMA0_Channel5_Handler    ;[25] RTK-DMA0 channel 5 global interrupt
                DCD     KeyScan_Handler           ;[26] keyscan global interrupt
                DCD     QDEC_Handler              ;[27] qdecode global interrupt
                DCD     IR_Handler                ;[28] IR module global interrupt
                DCD     DSP_Handler               ;[29] DSP interrupt
                DCD     GDMA0_Channel6_Handler    ;[30] RTK-DMA0 channel 6 global interrupt
                DCD     GDMA0_Channel7_Handler    ;[31] RTK-DMA0 channel 7 global interrupt
                DCD     GDMA0_Channel8_Handler    ;[32] RTK-DMA0 channel 8 global interrupt
                DCD     GDMA0_Channel9_Handler    ;[33] RTK-DMA0 channel 9 global interrupt
                DCD     GDMA0_Channel10_Handler   ;[34] RTK-DMA0 channel 6 global interrupt
                DCD     GDMA0_Channel11_Handler   ;[35] RTK-DMA0 channel 7 global interrupt
                DCD     GDMA0_Channel12_Handler   ;[36] RTK-DMA0 channel 8 global interrupt
                DCD     GDMA0_Channel13_Handler   ;[37] RTK-DMA0 channel 9 global interrupt
                DCD     GDMA0_Channel14_Handler   ;[38] RTK-DMA0 channel 6 global interrupt
                DCD     GDMA0_Channel15_Handler   ;[39] RTK-DMA0 channel 7 global interrupt
                DCD     TIM3_Handler              ;[40] Timer3 global interrupt
                DCD     TIM4_Handler              ;[41] Timer4 global interrupt
                DCD     TIM5_Handler              ;[42] Timer5 global interrupt
                DCD     TIM6_Handler              ;[43] Timer6 global interrupt
                DCD     TIM7_Handler              ;[44] Timer7 global interrupt
                DCD     TIM8to11_Handler              ;[45] TRNG interrupt
                DCD     GPIO_A_9_Handler          ;[46] GPIO_A[9] interrupt
                DCD     GPIO_A_16_Handler         ;[47] GPIO_A[16] interrupt
                DCD     GPIO_A_2to7_Handler       ;[48] GPIO_A[2:7] interrupt
                DCD     GPIO_A_8_10to15_Handler   ;[49] GPIO_A[8,10:15] interrupt
                DCD     GPIO_A_17to23_Handler     ;[50] GPIO_A[17:23] interrupt
                DCD     GPIO_A_24to31_Handler     ;[51] GPIO_A[24:31] interrupt
                DCD     GPIO_B_0to7_Handler       ;[52] GPIO_B[0:7] interrupt
                DCD     GPIO_B_8to15_Handler      ;[53] GPIO_B[8:15] interrupt
                DCD     GPIO_B_16to23_Handler     ;[54] GPIO_B[16:23] interrupt
                DCD     GPIO_B_24to31_Handler     ;[55] GPIO_B[24:31] interrupt
                DCD     UART3_Handler             ;[56] Uart3 interrupt(used for DSP)
                DCD     SPI2_Handler              ;[57] SPI2 interrupt
                DCD     I2C2_Handler              ;[58] I2C2 interrupt
                DCD     DSP_Event_1_Handler       ;[59] DSP event interrupt 1
                DCD     DSP_Event_2_Handler       ;[60] DSP event interrupt 2
                DCD     SHA256_Handler            ;[61] SHA256 interrupt
                DCD     USB_ISOC_Handler          ;[62] USB ISO interrupt
                DCD     BT_CLK_compare_Handler    ;[63] BT clock compare interrupt
                DCD     HW_RSA_Handler            ;[64] HW RSA interrupt
                DCD     PSRAM_Handler             ;[65] PSRAM interrupt
                DCD     Compare_clk_4_Handler     ;[66] Compare Clock interrupt 4
                DCD     Compare_clk_5_Handler     ;[67] Compare Clock interrupt 5
                DCD     dspdma_int_all_host_Handler    ;[68] RTK-DMA0 channel 0 global interrupt
                DCD     NNA_Handler    ;[69] RTK-DMA0 channel 1 global interrupt
                DCD     Compare_clk_9_Handler     ;[70] RTK-DMA0 channel 2 global interrupt
                DCD     Compare_clk_10_Handler    ;[71] RTK-DMA0 channel 3 global interrupt
                DCD     GPIO_C_0to7_Handler       ;[72] RTK-DMA0 channel 4 global interrupt
                DCD     GPIO_C_8to15_Handler      ;[73] RTK-DMA0 channel 5 global interrupt
                DCD     0                         ;[74] RTK-DMA0 channel 6 global interrupt
                DCD     0                         ;[75] RTK-DMA0 channel 7 global interrupt
                DCD     TIM8_Handler              ;[76] Timer8 global interrupt
                DCD     TIM9_Handler              ;[77] Timer9 global interrupt
                DCD     TIM10_Handler             ;[78] Timer10 global interrupt
                DCD     TIM11_Handler             ;[79] Timer11 global interrupt
                DCD     TIM12_Handler             ;[80] Timer12 global interrupt
                DCD     TIM13_Handler             ;[81] Timer13 global interrupt
                DCD     TIM14_Handler             ;[82] Timer14 global interrupt
                DCD     TIM15_Handler             ;[83] Timer15 global interrupt
                DCD     SPIC0_Handler             ;[84] SPIC0 interrupt
                DCD     SPIC1_Handler             ;[85] SPIC1 interrupt
                DCD     SPIC2_Handler             ;[86] SPIC2 interrupt
                DCD     SPIC3_Handler             ;[87] SPIC3 interrupt
                DCD     SDIO_HOST_Handler         ;[88] SDIO host interrupt
                DCD     SPIC4_Handler
                DCD     ERR_CORR_CODE_Handler
                DCD     SLAVE_PORT_MONITOR_Handler
                ;       Name,                     IRQ_NUM,  StatusBit
                DCD     RESERVED0_Handler         ;19,      0,
                DCD     RESERVED1_Handler         ;19,      1,
                DCD     RESERVED2_Handler         ;19,      2,
                DCD     RESERVED3_Handler         ;19,      3,
                DCD     RESERVED4_Handler         ;19,      4,
                DCD     RESERVED5_Handler         ;19,      5,
                DCD     RESERVED6_Handler         ;19,      6,
                DCD     SPDIF_Rx_Handler          ;19,      7,
                DCD     SPDIF_Tx_Handler          ;19,      8,
                DCD     MBIAS_MFG_DET_L_Handler   ;19,      9,
                DCD     SPI2W_Handler             ;19,      10,
                DCD     LPCOMP_Handler            ;19,      11,
                DCD     MBIAS_VBAT_DET_Handler    ;19,      12,
                DCD     MBIAS_ADP_DET_Handler     ;19,      13,
                DCD     HW_ASRC1_Handler          ;19,      14,
                DCD     HW_ASRC2_Handler          ;19,      15,
                DCD     VADBUF_Handler            ;19,      16,
                DCD     PTA_Mailbox_Handler       ;19,      17,
                DCD     DSP2MCU_Handler           ;19,      18,
                DCD     SPORT2_TX_Handler         ;19,      19,
                DCD     SPORT2_RX_Handler         ;19,      20,
                DCD     SPORT3_TX_Handler         ;19,      21,
                DCD     SPORT3_RX_Handler         ;19,      22,
                DCD     VAD_Handler               ;19,      23,
                DCD     ANC_Handler               ;19,      24,
                DCD     SPORT0_TX_Handler         ;19,      25,
                DCD     SPORT0_RX_Handler         ;19,      26,
                DCD     SPORT1_TX_Handler         ;19,      27,
                DCD     SPORT1_RX_Handler         ;19,      28,
                DCD     USB_UTMI_SUSPEND_N_Handler;19,      29,
                DCD     USB_DLPS_Resume_Handler   ;19,      30,
                DCD     RESERVED7_Handler        ;19,      31,
                DCD     ADP_IN_DET_Handler
                DCD     ADP_OUT_DET_Handler
                DCD     GPIOA2_Handler
                DCD     GPIOA3_Handler
                DCD     GPIOA4_Handler
                DCD     GPIOA5_Handler
                DCD     GPIOA6_Handler
                DCD     GPIOA7_Handler
                DCD     GPIOA8_Handler
                DCD     GPIOA9_Handler
                DCD     GPIOA10_Handler
                DCD     GPIOA11_Handler
                DCD     GPIOA12_Handler
                DCD     GPIOA13_Handler
                DCD     GPIOA14_Handler
                DCD     GPIOA15_Handler
                DCD     GPIOA16_Handler
                DCD     GPIOA17_Handler
                DCD     GPIOA18_Handler
                DCD     GPIOA19_Handler
                DCD     GPIOA20_Handler
                DCD     GPIOA21_Handler
                DCD     GPIOA22_Handler
                DCD     GPIOA23_Handler
                DCD     GPIOA24_Handler
                DCD     GPIOA25_Handler
                DCD     GPIOA26_Handler
                DCD     GPIOA27_Handler
                DCD     GPIOA28_Handler
                DCD     GPIOA29_Handler
                DCD     GPIOA30_Handler
                DCD     GPIOA31_Handler
                DCD     GPIOB0_Handler
                DCD     GPIOB1_Handler
                DCD     GPIOB2_Handler
                DCD     GPIOB3_Handler
                DCD     GPIOB4_Handler
                DCD     GPIOB5_Handler
                DCD     GPIOB6_Handler
                DCD     GPIOB7_Handler
                DCD     GPIOB8_Handler
                DCD     GPIOB9_Handler
                DCD     GPIOB10_Handler
                DCD     GPIOB11_Handler
                DCD     GPIOB12_Handler
                DCD     GPIOB13_Handler
                DCD     GPIOB14_Handler
                DCD     GPIOB15_Handler
                DCD     GPIOB16_Handler
                DCD     GPIOB17_Handler
                DCD     GPIOB18_Handler
                DCD     GPIOB19_Handler
                DCD     GPIOB20_Handler
                DCD     GPIOB21_Handler
                DCD     GPIOB22_Handler
                DCD     GPIOB23_Handler
                DCD     GPIOB24_Handler
                DCD     GPIOB25_Handler
                DCD     GPIOB26_Handler
                DCD     GPIOB27_Handler
                DCD     GPIOB28_Handler
                DCD     GPIOB29_Handler
                DCD     GPIOB30_Handler
                DCD     GPIOB31_Handler
                DCD     GPIOC0_Handler
                DCD     GPIOC1_Handler
                DCD     GPIOC2_Handler
                DCD     GPIOC3_Handler
                DCD     GPIOC4_Handler
                DCD     GPIOC5_Handler
                DCD     GPIOC6_Handler
                DCD     GPIOC7_Handler
                DCD     GPIOC8_Handler
                DCD     GPIOC9_Handler
                DCD     GPIOC10_Handler
                DCD     GPIOC11_Handler
                DCD     GPIOC12_Handler
                DCD     GPIOC13_Handler
                DCD     GPIOC14_Handler
                DCD     GPIOC15_Handler
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    RESET, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT Reset_Handler               [WEAK]
                IMPORT system_init
                IMPORT __main

                IF     :DEF:__USE_C_LIBRARY_INIT; // use c library init

                LDR    R0, =system_init
                BLX    R0
                LDR    R0, =__main
                BX     R0

                ELSE

                ; write our init flow: copy rw, encrypted and clear zi
                ; data on rw, data on zi, buffer on rw, buffer on zi

                ; copy data on rw
                IMPORT |Load$$RAM_GLOBAL$$RW$$Base|
                IMPORT |Image$$RAM_GLOBAL$$RW$$Base|
                IMPORT |Image$$RAM_GLOBAL$$RW$$Length|
                LDR R0, =|Load$$RAM_GLOBAL$$RW$$Base|
                LDR R1, =|Image$$RAM_GLOBAL$$RW$$Base|
                LDR R2, =|Image$$RAM_GLOBAL$$RW$$Length|
                CMP R2, #0
                BEQ end_of_copy_data_on_rw
copy_data_on_rw
                LDR R3, [R0]
                ADDS R0, R0, #4
                STR R3, [R1]
                ADDS R1, R1, #4
                SUBS R2, R2, #4
                BNE copy_data_on_rw
end_of_copy_data_on_rw

                ; clear data on zi
                IMPORT |Image$$RAM_GLOBAL$$ZI$$Base|
                IMPORT |Image$$RAM_GLOBAL$$ZI$$Length|
                LDR R0, =|Image$$RAM_GLOBAL$$ZI$$Base|
                LDR R1, =|Image$$RAM_GLOBAL$$ZI$$Length|
                CMP R1, #0
                BEQ end_of_clear_data_on_zi
clear_data_on_zi
                STR R2, [R0]
                ADDS R0, R0, #4
                SUBS R1, R1, #4
                BNE clear_data_on_zi
end_of_clear_data_on_zi

                ; copy buffer on rw
                IMPORT |Load$$RAM_TEXT$$RW$$Base|
                IMPORT |Image$$RAM_TEXT$$RW$$Base|
                IMPORT |Image$$RAM_TEXT$$RW$$Length|
                LDR R0, =|Load$$RAM_TEXT$$RW$$Base|
                LDR R1, =|Image$$RAM_TEXT$$RW$$Base|
                LDR R2, =|Image$$RAM_TEXT$$RW$$Length|
                CMP R2, #0
                BEQ end_of_copy_buffer_on_rw
copy_buffer_on_rw
                LDR R3, [R0]
                ADDS R0, R0, #4
                STR R3, [R1]
                ADDS R1, R1, #4
                SUBS R2, R2, #4
                BNE copy_buffer_on_rw
end_of_copy_buffer_on_rw

                ; copy buffer on ro (for ram function)
                IMPORT |Load$$RAM_TEXT$$RO$$Base|
                IMPORT |Image$$RAM_TEXT$$RO$$Base|
                IMPORT |Image$$RAM_TEXT$$RO$$Length|
                LDR R0, =|Load$$RAM_TEXT$$RO$$Base|
                LDR R1, =|Image$$RAM_TEXT$$RO$$Base|
                LDR R2, =|Image$$RAM_TEXT$$RO$$Length|
                CMP R2, #0
                BEQ end_of_copy_buffer_on_ro
copy_buffer_on_ro
                LDR R3, [R0]
                ADDS R0, R0, #4
                STR R3, [R1]
                ADDS R1, R1, #4
                SUBS R2, R2, #4
                BNE copy_buffer_on_ro
end_of_copy_buffer_on_ro

                ; clear buffer on zi
                IMPORT |Image$$RAM_TEXT$$ZI$$Base|
                IMPORT |Image$$RAM_TEXT$$ZI$$Length|
                LDR R0, =|Image$$RAM_TEXT$$ZI$$Base|
                LDR R1, =|Image$$RAM_TEXT$$ZI$$Length|
                CMP R1, #0
                BEQ end_of_clear_buffer_on_zi
clear_buffer_on_zi
                STR R2, [R0]
                ADDS R0, R0, #4
                SUBS R1, R1, #4
                BNE clear_buffer_on_zi
				NOP
end_of_clear_buffer_on_zi


                IMPORT system_init
                IMPORT main
                LDR    R0, =system_init
                BLX    R0
                LDR    R0, =main
                BX     R0

                ENDIF ; end of macro __USE_C_LIBRARY_INIT

                ENDP ; end of Reset_Handler


                AREA    |.text|, CODE, READONLY
Default_Handler PROC
                EXPORT Default_Handler           [WEAK]
                EXPORT NMI_Handler               [WEAK]
                EXPORT HardFault_Handler         [WEAK]
                EXPORT MemManage_Handler         [WEAK]
                EXPORT BusFault_Handler          [WEAK]
                EXPORT UsageFault_Handler        [WEAK]
                EXPORT SVC_Handler               [WEAK]
                EXPORT DebugMon_Handler          [WEAK]
                EXPORT PendSV_Handler            [WEAK]
                EXPORT SysTick_Handler           [WEAK]
                EXPORT System_Handler            [WEAK]
                EXPORT WDT_Handler                      [WEAK]
                EXPORT BTMAC_Handler             [WEAK]
                EXPORT USB_IP_Handler                   [WEAK]
                EXPORT TIM2_Handler              [WEAK]
                EXPORT Platform_Handler                 [WEAK]
                EXPORT I2S0_TX_Handler              [WEAK]
                EXPORT I2S0_RX_Handler              [WEAK]
                EXPORT UART2_Handler                    [WEAK]
                EXPORT GPIOA0_Handler                   [WEAK]
                EXPORT GPIOA1_Handler                   [WEAK]
                EXPORT UART1_Handler                    [WEAK]
                EXPORT UART0_Handler                    [WEAK]
                EXPORT RTC_Handler               [WEAK]
                EXPORT SPI0_Handler              [WEAK]
                EXPORT SPI1_Handler              [WEAK]
                EXPORT I2C0_Handler              [WEAK]
                EXPORT I2C1_Handler              [WEAK]
                EXPORT ADC_Handler               [WEAK]
                EXPORT Peripheral_Handler        [WEAK]
                EXPORT GDMA0_Channel0_Handler    [WEAK]
                EXPORT GDMA0_Channel1_Handler    [WEAK]
                EXPORT GDMA0_Channel2_Handler    [WEAK]
                EXPORT GDMA0_Channel3_Handler    [WEAK]
                EXPORT GDMA0_Channel4_Handler    [WEAK]
                EXPORT GDMA0_Channel5_Handler    [WEAK]
                EXPORT KeyScan_Handler           [WEAK]
                EXPORT QDEC_Handler              [WEAK]
                EXPORT IR_Handler                [WEAK]
                EXPORT DSP_Handler               [WEAK]
                EXPORT GDMA0_Channel6_Handler    [WEAK]
                EXPORT GDMA0_Channel7_Handler    [WEAK]
                EXPORT GDMA0_Channel8_Handler           [WEAK]
                EXPORT GDMA0_Channel9_Handler           [WEAK]
                EXPORT GDMA0_Channel10_Handler          [WEAK]
                EXPORT GDMA0_Channel11_Handler          [WEAK]
                EXPORT GDMA0_Channel12_Handler          [WEAK]
                EXPORT GDMA0_Channel13_Handler          [WEAK]
                EXPORT GDMA0_Channel14_Handler          [WEAK]
                EXPORT GDMA0_Channel15_Handler          [WEAK]
                EXPORT TIM3_Handler              [WEAK]
                EXPORT TIM4_Handler              [WEAK]
                EXPORT TIM5_Handler              [WEAK]
                EXPORT TIM6_Handler              [WEAK]
                EXPORT TIM7_Handler              [WEAK]
                EXPORT TIM8to11_Handler                 [WEAK]
                EXPORT GPIO_A_9_Handler                 [WEAK]
                EXPORT GPIO_A_16_Handler                [WEAK]
                EXPORT GPIO_A_2to7_Handler              [WEAK]
                EXPORT GPIO_A_8_10to15_Handler          [WEAK]
                EXPORT GPIO_A_17to23_Handler            [WEAK]
                EXPORT GPIO_A_24to31_Handler            [WEAK]
                EXPORT GPIO_B_0to7_Handler              [WEAK]
                EXPORT GPIO_B_8to15_Handler             [WEAK]
                EXPORT GPIO_B_16to23_Handler            [WEAK]
                EXPORT GPIO_B_24to31_Handler            [WEAK]
                EXPORT UART3_Handler                    [WEAK]
                EXPORT SPI2_Handler                     [WEAK]
                EXPORT I2C2_Handler                     [WEAK]
                EXPORT DSP_Event_1_Handler              [WEAK]
                EXPORT DSP_Event_2_Handler              [WEAK]
                EXPORT SHA256_Handler                   [WEAK]
                EXPORT USB_ISOC_Handler                 [WEAK]
                EXPORT BT_CLK_compare_Handler           [WEAK]
                EXPORT HW_RSA_Handler                   [WEAK]
                EXPORT PSRAM_Handler                    [WEAK]
                EXPORT Compare_clk_4_Handler            [WEAK]
                EXPORT Compare_clk_5_Handler            [WEAK]
                EXPORT dspdma_int_all_host_Handler           [WEAK]
                EXPORT NNA_Handler           [WEAK]
                EXPORT Compare_clk_9_Handler           [WEAK]
                EXPORT Compare_clk_10_Handler           [WEAK]
                EXPORT GPIO_C_0to7_Handler           [WEAK]
                EXPORT GPIO_C_8to15_Handler           [WEAK]
                EXPORT TIM8_Handler                     [WEAK]
                EXPORT TIM9_Handler                     [WEAK]
                EXPORT TIM10_Handler                    [WEAK]
                EXPORT TIM11_Handler                    [WEAK]
                EXPORT TIM12_Handler                    [WEAK]
                EXPORT TIM13_Handler                    [WEAK]
                EXPORT TIM14_Handler                    [WEAK]
                EXPORT TIM15_Handler                    [WEAK]
                EXPORT SPIC0_Handler                    [WEAK]
                EXPORT SPIC1_Handler                    [WEAK]
                EXPORT SPIC2_Handler                    [WEAK]
                EXPORT SPIC3_Handler                    [WEAK]
                EXPORT SDIO_HOST_Handler                [WEAK]
                EXPORT SPIC4_Handler                    [WEAK]
                EXPORT ERR_CORR_CODE_Handler            [WEAK]
                EXPORT SLAVE_PORT_MONITOR_Handler                [WEAK]
                EXPORT RESERVED0_Handler                [WEAK]
                EXPORT RESERVED1_Handler                [WEAK]
                EXPORT RESERVED2_Handler                [WEAK]
                EXPORT RESERVED3_Handler                [WEAK]
                EXPORT RESERVED4_Handler                [WEAK]
                EXPORT RESERVED5_Handler                [WEAK]
                EXPORT RESERVED6_Handler                [WEAK]
                EXPORT SPDIF_Rx_Handler                 [WEAK]
                EXPORT SPDIF_Tx_Handler                 [WEAK]
                EXPORT MBIAS_MFG_DET_L_Handler          [WEAK]
                EXPORT SPI2W_Handler             [WEAK]
                EXPORT LPCOMP_Handler            [WEAK]
                EXPORT MBIAS_VBAT_DET_Handler           [WEAK]
                EXPORT MBIAS_ADP_DET_Handler            [WEAK]
                EXPORT HW_ASRC1_Handler                 [WEAK]
                EXPORT HW_ASRC2_Handler                 [WEAK]
                EXPORT VADBUF_Handler                   [WEAK]
                EXPORT PTA_Mailbox_Handler       [WEAK]
                EXPORT DSP2MCU_Handler                  [WEAK]
                EXPORT SPORT2_TX_Handler                [WEAK]
                EXPORT SPORT2_RX_Handler                [WEAK]
                EXPORT SPORT3_TX_Handler                [WEAK]
                EXPORT SPORT3_RX_Handler                [WEAK]
                EXPORT VAD_Handler                      [WEAK]
                EXPORT ANC_Handler                      [WEAK]
                EXPORT SPORT0_TX_Handler         [WEAK]
                EXPORT SPORT0_RX_Handler         [WEAK]
                EXPORT SPORT1_TX_Handler         [WEAK]
                EXPORT SPORT1_RX_Handler         [WEAK]
                EXPORT USB_UTMI_SUSPEND_N_Handler       [WEAK]
                EXPORT USB_DLPS_Resume_Handler          [WEAK]
                EXPORT TIM8to11_Handler                 [WEAK]
                EXPORT ADP_IN_DET_Handler        [WEAK]
                EXPORT ADP_OUT_DET_Handler       [WEAK]
                EXPORT GPIOA2_Handler                   [WEAK]
                EXPORT GPIOA3_Handler                   [WEAK]
                EXPORT GPIOA4_Handler                   [WEAK]
                EXPORT GPIOA5_Handler                   [WEAK]
                EXPORT GPIOA6_Handler                   [WEAK]
                EXPORT GPIOA7_Handler                   [WEAK]
                EXPORT GPIOA8_Handler                   [WEAK]
                EXPORT GPIOA9_Handler                   [WEAK]
                EXPORT GPIOA10_Handler                  [WEAK]
                EXPORT GPIOA11_Handler                  [WEAK]
                EXPORT GPIOA12_Handler                  [WEAK]
                EXPORT GPIOA13_Handler                  [WEAK]
                EXPORT GPIOA14_Handler                  [WEAK]
                EXPORT GPIOA15_Handler                  [WEAK]
                EXPORT GPIOA16_Handler                  [WEAK]
                EXPORT GPIOA17_Handler                  [WEAK]
                EXPORT GPIOA18_Handler                  [WEAK]
                EXPORT GPIOA19_Handler                  [WEAK]
                EXPORT GPIOA20_Handler                  [WEAK]
                EXPORT GPIOA21_Handler                  [WEAK]
                EXPORT GPIOA22_Handler                  [WEAK]
                EXPORT GPIOA23_Handler                  [WEAK]
                EXPORT GPIOA24_Handler                  [WEAK]
                EXPORT GPIOA25_Handler                  [WEAK]
                EXPORT GPIOA26_Handler                  [WEAK]
                EXPORT GPIOA27_Handler                  [WEAK]
                EXPORT GPIOA28_Handler                  [WEAK]
                EXPORT GPIOA29_Handler                  [WEAK]
                EXPORT GPIOA30_Handler                  [WEAK]
                EXPORT GPIOA31_Handler                  [WEAK]
                EXPORT GPIOB0_Handler                   [WEAK]
                EXPORT GPIOB1_Handler                   [WEAK]
                EXPORT GPIOB2_Handler                   [WEAK]
                EXPORT GPIOB3_Handler                   [WEAK]
                EXPORT GPIOB4_Handler                   [WEAK]
                EXPORT GPIOB5_Handler                   [WEAK]
                EXPORT GPIOB6_Handler                   [WEAK]
                EXPORT GPIOB7_Handler                   [WEAK]
                EXPORT GPIOB8_Handler                   [WEAK]
                EXPORT GPIOB9_Handler                   [WEAK]
                EXPORT GPIOB10_Handler                  [WEAK]
                EXPORT GPIOB11_Handler                  [WEAK]
                EXPORT GPIOB12_Handler                  [WEAK]
                EXPORT GPIOB13_Handler                  [WEAK]
                EXPORT GPIOB14_Handler                  [WEAK]
                EXPORT GPIOB15_Handler                  [WEAK]
                EXPORT GPIOB16_Handler                  [WEAK]
                EXPORT GPIOB17_Handler                  [WEAK]
                EXPORT GPIOB18_Handler                  [WEAK]
                EXPORT GPIOB19_Handler                  [WEAK]
                EXPORT GPIOB20_Handler                  [WEAK]
                EXPORT GPIOB21_Handler                  [WEAK]
                EXPORT GPIOB22_Handler                  [WEAK]
                EXPORT GPIOB23_Handler                  [WEAK]
                EXPORT GPIOB24_Handler                  [WEAK]
                EXPORT GPIOB25_Handler                  [WEAK]
                EXPORT GPIOB26_Handler                  [WEAK]
                EXPORT GPIOB27_Handler                  [WEAK]
                EXPORT GPIOB28_Handler                  [WEAK]
                EXPORT GPIOB29_Handler                  [WEAK]
                EXPORT GPIOB30_Handler                  [WEAK]
                EXPORT GPIOB31_Handler                  [WEAK]
                EXPORT GPIOC0_Handler                   [WEAK]
                EXPORT GPIOC1_Handler                   [WEAK]
                EXPORT GPIOC2_Handler                   [WEAK]
                EXPORT GPIOC3_Handler                   [WEAK]
                EXPORT GPIOC4_Handler                   [WEAK]
                EXPORT GPIOC5_Handler                   [WEAK]
                EXPORT GPIOC6_Handler                   [WEAK]
                EXPORT GPIOC7_Handler                   [WEAK]
                EXPORT GPIOC8_Handler                   [WEAK]
                EXPORT GPIOC9_Handler                   [WEAK]
                EXPORT GPIOC10_Handler                  [WEAK]
                EXPORT GPIOC11_Handler                  [WEAK]
                EXPORT GPIOC12_Handler                  [WEAK]
                EXPORT GPIOC13_Handler                  [WEAK]
                EXPORT GPIOC14_Handler                  [WEAK]
                EXPORT GPIOC15_Handler                  [WEAK]
NMI_Handler
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler
System_Handler
WDT_Handler
BTMAC_Handler
USB_IP_Handler
TIM2_Handler
Platform_Handler
I2S0_TX_Handler
I2S0_RX_Handler
UART2_Handler
GPIOA0_Handler
GPIOA1_Handler
UART1_Handler
UART0_Handler
RTC_Handler
SPI0_Handler
SPI1_Handler
I2C0_Handler
I2C1_Handler
ADC_Handler
Peripheral_Handler
GDMA0_Channel0_Handler
GDMA0_Channel1_Handler
GDMA0_Channel2_Handler
GDMA0_Channel3_Handler
GDMA0_Channel4_Handler
GDMA0_Channel5_Handler
KeyScan_Handler
QDEC_Handler
IR_Handler
DSP_Handler
GDMA0_Channel6_Handler
GDMA0_Channel7_Handler
GDMA0_Channel8_Handler
GDMA0_Channel9_Handler
GDMA0_Channel10_Handler
GDMA0_Channel11_Handler
GDMA0_Channel12_Handler
GDMA0_Channel13_Handler
GDMA0_Channel14_Handler
GDMA0_Channel15_Handler
TIM3_Handler
TIM4_Handler
TIM5_Handler
TIM6_Handler
TIM7_Handler
TIM8to11_Handler
GPIO_A_9_Handler
GPIO_A_16_Handler
GPIO_A_2to7_Handler
GPIO_A_8_10to15_Handler
GPIO_A_17to23_Handler
GPIO_A_24to31_Handler
GPIO_B_0to7_Handler
GPIO_B_8to15_Handler
GPIO_B_16to23_Handler
GPIO_B_24to31_Handler
UART3_Handler
SPI2_Handler
I2C2_Handler
DSP_Event_1_Handler
DSP_Event_2_Handler
SHA256_Handler
USB_ISOC_Handler
BT_CLK_compare_Handler
HW_RSA_Handler
PSRAM_Handler
Compare_clk_4_Handler
Compare_clk_5_Handler
dspdma_int_all_host_Handler
NNA_Handler
Compare_clk_9_Handler
Compare_clk_10_Handler
GPIO_C_0to7_Handler
GPIO_C_8to15_Handler
TIM8_Handler
TIM9_Handler
TIM10_Handler
TIM11_Handler
TIM12_Handler
TIM13_Handler
TIM14_Handler
TIM15_Handler
SPIC0_Handler
SPIC1_Handler
SPIC2_Handler
SPIC3_Handler
SDIO_HOST_Handler
SPIC4_Handler
ERR_CORR_CODE_Handler
SLAVE_PORT_MONITOR_Handler
RESERVED0_Handler
RESERVED1_Handler
RESERVED2_Handler
RESERVED3_Handler
RESERVED4_Handler
RESERVED5_Handler
RESERVED6_Handler
SPDIF_Rx_Handler
SPDIF_Tx_Handler
MBIAS_MFG_DET_L_Handler
SPI2W_Handler
LPCOMP_Handler
MBIAS_VBAT_DET_Handler
MBIAS_ADP_DET_Handler
HW_ASRC1_Handler
HW_ASRC2_Handler
VADBUF_Handler
PTA_Mailbox_Handler
DSP2MCU_Handler
SPORT2_TX_Handler
SPORT2_RX_Handler
SPORT3_TX_Handler
SPORT3_RX_Handler
VAD_Handler
ANC_Handler
SPORT0_TX_Handler
SPORT0_RX_Handler
SPORT1_TX_Handler
SPORT1_RX_Handler
USB_UTMI_SUSPEND_N_Handler
USB_DLPS_Resume_Handler
RESERVED7_Handler
ADP_IN_DET_Handler
ADP_OUT_DET_Handler
GPIOA2_Handler
GPIOA3_Handler
GPIOA4_Handler
GPIOA5_Handler
GPIOA6_Handler
GPIOA7_Handler
GPIOA8_Handler
GPIOA9_Handler
GPIOA10_Handler
GPIOA11_Handler
GPIOA12_Handler
GPIOA13_Handler
GPIOA14_Handler
GPIOA15_Handler
GPIOA16_Handler
GPIOA17_Handler
GPIOA18_Handler
GPIOA19_Handler
GPIOA20_Handler
GPIOA21_Handler
GPIOA22_Handler
GPIOA23_Handler
GPIOA24_Handler
GPIOA25_Handler
GPIOA26_Handler
GPIOA27_Handler
GPIOA28_Handler
GPIOA29_Handler
GPIOA30_Handler
GPIOA31_Handler
GPIOB0_Handler
GPIOB1_Handler
GPIOB2_Handler
GPIOB3_Handler
GPIOB4_Handler
GPIOB5_Handler
GPIOB6_Handler
GPIOB7_Handler
GPIOB8_Handler
GPIOB9_Handler
GPIOB10_Handler
GPIOB11_Handler
GPIOB12_Handler
GPIOB13_Handler
GPIOB14_Handler
GPIOB15_Handler
GPIOB16_Handler
GPIOB17_Handler
GPIOB18_Handler
GPIOB19_Handler
GPIOB20_Handler
GPIOB21_Handler
GPIOB22_Handler
GPIOB23_Handler
GPIOB24_Handler
GPIOB25_Handler
GPIOB26_Handler
GPIOB27_Handler
GPIOB28_Handler
GPIOB29_Handler
GPIOB30_Handler
GPIOB31_Handler
GPIOC0_Handler
GPIOC1_Handler
GPIOC2_Handler
GPIOC3_Handler
GPIOC4_Handler
GPIOC5_Handler
GPIOC6_Handler
GPIOC7_Handler
GPIOC8_Handler
GPIOC9_Handler
GPIOC10_Handler
GPIOC11_Handler
GPIOC12_Handler
GPIOC13_Handler
GPIOC14_Handler
GPIOC15_Handler
                IMPORT log_direct
                LDR    R0, =0x20000000
                LDR    R1, =DEFAULT_HANDLER_TXT
                MRS    R2, IPSR
                LDR    R3, =log_direct
                BLX    R3
                B      .

                ENDP


; User Initial Stack
                EXPORT  __user_setup_stackheap
__user_setup_stackheap PROC
                BX      LR
                ENDP

DEFAULT_HANDLER_TXT
                DCB "Error! Please implement your ISR Handler for IRQ %d!\n", 0 ; Null terminated string
                ALIGN

                END
