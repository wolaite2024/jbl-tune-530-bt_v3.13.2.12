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
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     Default_Handler
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     Default_Handler
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     System_Handler
                DCD     WDT_Handler
                DCD     BTMAC_Handler
                DCD     DSP_Handler
                DCD     RXI300_Handler
                DCD     SPI0_Handler
                DCD     I2C0_Handler
                DCD     ADC_Handler
                DCD     SPORT0_TX_Handler
                DCD     SPORT0_RX_Handler
                DCD     TIM2_Handler
                DCD     TIM3_Handler
                DCD     TIM4_Handler
                DCD     RTC_Handler
                DCD     UART0_Handler
                DCD     UART1_Handler
                DCD     UART2_Handler
                DCD     Peri_Handler
                DCD     GPIO_A0_Handler
                DCD     GPIO_A1_Handler
                DCD     GPIO_A_2_7_Handler
                DCD     GPIO_A_8_15_Handler
                DCD     GPIO_A_16_23_Handler
                DCD     GPIO_A_24_31_Handler
                DCD     SPORT1_RX_Handler

                DCD     SPORT1_TX_Handler
                DCD     ADP_DET_Handler
                DCD     VBAT_DET_Handler
                DCD     GDMA0_Channel0_Handler
                DCD     GDMA0_Channel1_Handler
                DCD     GDMA0_Channel2_Handler
                DCD     GDMA0_Channel3_Handler
                DCD     GDMA0_Channel4_Handler
                DCD     GDMA0_Channel5_Handler
                DCD     GDMA0_Channel6_Handler
                DCD     GDMA0_Channel7_Handler
                DCD     GDMA0_Channel8_Handler
                DCD     GPIO_B_0_7_Handler
                DCD     GPIO_B_8_15_Handler
                DCD     GPIO_B_16_23_Handler
                DCD     GPIO_B_24_31_Handler
                DCD     SPI1_Handler
                DCD     SPI2_Handler
                DCD     I2C1_Handler
                DCD     I2C2_Handler
                DCD     KeyScan_Handler
                DCD     QDEC_Handler
                DCD     USB_Handler
                DCD     USB_ISOC_Handler
                DCD     SPIC0_Handler
                DCD     SPIC1_Handler
                DCD     SPIC2_Handler
                DCD     SPIC3_Handler
                DCD     TIM5_Handler
                DCD     TIM6_Handler
                DCD     TIM7_Handler
                DCD     ASRC0_Handler
                DCD     ASRC1_Handler
                DCD     I8080_Handler
                DCD     ISO7816_Handler
                DCD     SDIO0_Handler
                DCD     SPORT2_RX_Handler
                DCD     ANC_Handler
                DCD     TOUCH_Handler

                DCD     MFB_DET_L_Handler
                DCD     PTA_Mailbox_Handler
                DCD     Utmi_Suspend_N_Handler
                DCD     IR_Handler
                DCD     TRNG_Handler
                DCD     PSRAMC_Handler
                DCD     LPCOMP_Handler
                DCD     TIM5_Handler
                DCD     TIM6_Handler
                DCD     TIM7_Handler
                DCD     Default_Handler
                DCD     LPCOMP_Handler
                DCD     VBAT_DET_Handler
                DCD     ADP_DET_Handler

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

                EXPORT NMI_Handler        [WEAK]
                EXPORT HardFault_Handler        [WEAK]
                EXPORT MemManage_Handler        [WEAK]
                EXPORT BusFault_Handler        [WEAK]
                EXPORT UsageFault_Handler        [WEAK]
                EXPORT Default_Handler        [WEAK]
                EXPORT Default_Handler        [WEAK]
                EXPORT Default_Handler        [WEAK]
                EXPORT Default_Handler        [WEAK]
                EXPORT SVC_Handler        [WEAK]
                EXPORT DebugMon_Handler        [WEAK]
                EXPORT Default_Handler        [WEAK]
                EXPORT PendSV_Handler        [WEAK]
                EXPORT SysTick_Handler        [WEAK]

                ;ExternalInterrupts
                EXPORT System_Handler        [WEAK]
                EXPORT WDT_Handler        [WEAK]
                EXPORT BTMAC_Handler        [WEAK]
                EXPORT DSP_Handler        [WEAK]
                EXPORT RXI300_Handler        [WEAK]
                EXPORT SPI0_Handler        [WEAK]
                EXPORT I2C0_Handler        [WEAK]
                EXPORT ADC_Handler        [WEAK]
                EXPORT SPORT0_TX_Handler        [WEAK]
                EXPORT SPORT0_RX_Handler        [WEAK]
                EXPORT TIM2_Handler        [WEAK]
                EXPORT TIM3_Handler        [WEAK]
                EXPORT TIM4_Handler        [WEAK]
                EXPORT RTC_Handler        [WEAK]
                EXPORT UART0_Handler        [WEAK]
                EXPORT UART1_Handler        [WEAK]
                EXPORT UART2_Handler        [WEAK]
                EXPORT Peri_Handler        [WEAK]
                EXPORT GPIO_A0_Handler        [WEAK]
                EXPORT GPIO_A1_Handler        [WEAK]
                EXPORT GPIO_A_2_7_Handler        [WEAK]
                EXPORT GPIO_A_8_15_Handler        [WEAK]
                EXPORT GPIO_A_16_23_Handler        [WEAK]
                EXPORT GPIO_A_24_31_Handler        [WEAK]
                EXPORT SPORT1_RX_Handler        [WEAK]

                EXPORT SPORT1_TX_Handler        [WEAK]
                EXPORT ADP_DET_Handler        [WEAK]
                EXPORT VBAT_DET_Handler        [WEAK]
                EXPORT GDMA0_Channel0_Handler        [WEAK]
                EXPORT GDMA0_Channel1_Handler        [WEAK]
                EXPORT GDMA0_Channel2_Handler        [WEAK]
                EXPORT GDMA0_Channel3_Handler        [WEAK]
                EXPORT GDMA0_Channel4_Handler        [WEAK]
                EXPORT GDMA0_Channel5_Handler        [WEAK]
                EXPORT GDMA0_Channel6_Handler        [WEAK]
                EXPORT GDMA0_Channel7_Handler        [WEAK]
                EXPORT GDMA0_Channel8_Handler        [WEAK]
                EXPORT GPIO_B_0_7_Handler        [WEAK]
                EXPORT GPIO_B_8_15_Handler        [WEAK]
                EXPORT GPIO_B_16_23_Handler        [WEAK]
                EXPORT GPIO_B_24_31_Handler        [WEAK]
                EXPORT SPI1_Handler        [WEAK]
                EXPORT SPI2_Handler        [WEAK]
                EXPORT I2C1_Handler        [WEAK]
                EXPORT I2C2_Handler        [WEAK]
                EXPORT KeyScan_Handler        [WEAK]
                EXPORT QDEC_Handler        [WEAK]
                EXPORT USB_Handler        [WEAK]
                EXPORT USB_ISOC_Handler        [WEAK]
                EXPORT SPIC0_Handler        [WEAK]
                EXPORT SPIC1_Handler        [WEAK]
                EXPORT SPIC2_Handler        [WEAK]
                EXPORT SPIC3_Handler        [WEAK]
                EXPORT TIM5_Handler        [WEAK]
                EXPORT TIM6_Handler        [WEAK]
                EXPORT TIM7_Handler        [WEAK]
                EXPORT ASRC0_Handler        [WEAK]
                EXPORT ASRC1_Handler        [WEAK]
                EXPORT I8080_Handler        [WEAK]
                EXPORT ISO7816_Handler        [WEAK]
                EXPORT SDIO0_Handler        [WEAK]
                EXPORT SPORT2_RX_Handler        [WEAK]
                EXPORT ANC_Handler        [WEAK]
                EXPORT TOUCH_Handler        [WEAK]

                EXPORT MFB_DET_L_Handler        [WEAK]
                EXPORT PTA_Mailbox_Handler        [WEAK]
                EXPORT Utmi_Suspend_N_Handler        [WEAK]
                EXPORT IR_Handler        [WEAK]
                EXPORT TRNG_Handler        [WEAK]
                EXPORT PSRAMC_Handler        [WEAK]
                EXPORT LPCOMP_Handler        [WEAK]
                EXPORT TIM5_Handler        [WEAK]
                EXPORT TIM6_Handler        [WEAK]
                EXPORT TIM7_Handler        [WEAK]
                EXPORT Default_Handler        [WEAK]
                EXPORT LPCOMP_Handler        [WEAK]
                EXPORT VBAT_DET_Handler        [WEAK]
                EXPORT ADP_DET_Handler        [WEAK]

                EXPORT ADP_IN_DET_Handler        [WEAK]
                EXPORT ADP_OUT_DET_Handler        [WEAK]
                EXPORT GPIOA2_Handler        [WEAK]
                EXPORT GPIOA3_Handler        [WEAK]
                EXPORT GPIOA4_Handler        [WEAK]
                EXPORT GPIOA5_Handler        [WEAK]
                EXPORT GPIOA6_Handler        [WEAK]
                EXPORT GPIOA7_Handler        [WEAK]
                EXPORT GPIOA8_Handler        [WEAK]
                EXPORT GPIOA9_Handler        [WEAK]
                EXPORT GPIOA10_Handler        [WEAK]
                EXPORT GPIOA11_Handler        [WEAK]
                EXPORT GPIOA12_Handler        [WEAK]
                EXPORT GPIOA13_Handler        [WEAK]
                EXPORT GPIOA14_Handler        [WEAK]
                EXPORT GPIOA15_Handler        [WEAK]
                EXPORT GPIOA16_Handler        [WEAK]
                EXPORT GPIOA17_Handler        [WEAK]
                EXPORT GPIOA18_Handler        [WEAK]
                EXPORT GPIOA19_Handler        [WEAK]
                EXPORT GPIOA20_Handler        [WEAK]
                EXPORT GPIOA21_Handler        [WEAK]
                EXPORT GPIOA22_Handler        [WEAK]
                EXPORT GPIOA23_Handler        [WEAK]
                EXPORT GPIOA24_Handler        [WEAK]
                EXPORT GPIOA25_Handler        [WEAK]
                EXPORT GPIOA26_Handler        [WEAK]
                EXPORT GPIOA27_Handler        [WEAK]
                EXPORT GPIOA28_Handler        [WEAK]
                EXPORT GPIOA29_Handler        [WEAK]
                EXPORT GPIOA30_Handler        [WEAK]
                EXPORT GPIOA31_Handler        [WEAK]
                EXPORT GPIOB0_Handler        [WEAK]
                EXPORT GPIOB1_Handler        [WEAK]
                EXPORT GPIOB2_Handler        [WEAK]
                EXPORT GPIOB3_Handler        [WEAK]
                EXPORT GPIOB4_Handler        [WEAK]
                EXPORT GPIOB5_Handler        [WEAK]
                EXPORT GPIOB6_Handler        [WEAK]
                EXPORT GPIOB7_Handler        [WEAK]
                EXPORT GPIOB8_Handler        [WEAK]
                EXPORT GPIOB9_Handler        [WEAK]
                EXPORT GPIOB10_Handler        [WEAK]
                EXPORT GPIOB11_Handler        [WEAK]
                EXPORT GPIOB12_Handler        [WEAK]
                EXPORT GPIOB13_Handler        [WEAK]
                EXPORT GPIOB14_Handler        [WEAK]
                EXPORT GPIOB15_Handler        [WEAK]
                EXPORT GPIOB16_Handler        [WEAK]
                EXPORT GPIOB17_Handler        [WEAK]
                EXPORT GPIOB18_Handler        [WEAK]
                EXPORT GPIOB19_Handler        [WEAK]
                EXPORT GPIOB20_Handler        [WEAK]
                EXPORT GPIOB21_Handler        [WEAK]
                EXPORT GPIOB22_Handler        [WEAK]
                EXPORT GPIOB23_Handler        [WEAK]
                EXPORT GPIOB24_Handler        [WEAK]
                EXPORT GPIOB25_Handler        [WEAK]
                EXPORT GPIOB26_Handler        [WEAK]
                EXPORT GPIOB27_Handler        [WEAK]
                EXPORT GPIOB28_Handler        [WEAK]
                EXPORT GPIOB29_Handler        [WEAK]
                EXPORT GPIOB30_Handler        [WEAK]
                EXPORT GPIOB31_Handler        [WEAK]

NMI_Handler
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
SVC_Handler
DebugMon_Handler
PendSV_Handler
SysTick_Handler

; ExternalInterrupts
System_Handler
WDT_Handler
BTMAC_Handler
DSP_Handler
RXI300_Handler
SPI0_Handler
I2C0_Handler
ADC_Handler
SPORT0_TX_Handler
SPORT0_RX_Handler
TIM2_Handler
TIM3_Handler
TIM4_Handler
RTC_Handler
UART0_Handler
UART1_Handler
UART2_Handler
Peri_Handler
GPIO_A0_Handler
GPIO_A1_Handler
GPIO_A_2_7_Handler
GPIO_A_8_15_Handler
GPIO_A_16_23_Handler
GPIO_A_24_31_Handler
SPORT1_RX_Handler

SPORT1_TX_Handler
ADP_DET_Handler
VBAT_DET_Handler
GDMA0_Channel0_Handler
GDMA0_Channel1_Handler
GDMA0_Channel2_Handler
GDMA0_Channel3_Handler
GDMA0_Channel4_Handler
GDMA0_Channel5_Handler
GDMA0_Channel6_Handler
GDMA0_Channel7_Handler
GDMA0_Channel8_Handler
GPIO_B_0_7_Handler
GPIO_B_8_15_Handler
GPIO_B_16_23_Handler
GPIO_B_24_31_Handler
SPI1_Handler
SPI2_Handler
I2C1_Handler
I2C2_Handler
KeyScan_Handler
QDEC_Handler
USB_Handler
USB_ISOC_Handler
SPIC0_Handler
SPIC1_Handler
SPIC2_Handler
SPIC3_Handler
TIM5_Handler
TIM6_Handler
TIM7_Handler
ASRC0_Handler
ASRC1_Handler
I8080_Handler
ISO7816_Handler
SDIO0_Handler
SPORT2_RX_Handler
ANC_Handler
TOUCH_Handler

MFB_DET_L_Handler
PTA_Mailbox_Handler
Utmi_Suspend_N_Handler
IR_Handler
TRNG_Handler
PSRAMC_Handler
LPCOMP_Handler
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
