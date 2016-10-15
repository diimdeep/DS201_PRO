/********************* (C) COPYRIGHT 2012 e-Design Co.,Ltd. ********************
 Brief   : 底层硬件配置                                          Author : bure
*******************************************************************************/
#include "STM32F10x.h"

#define REC_BASE            0x08003C00



#define USB_PWR()          (GPIOC_IDR & GPIO_Pin_11)
#define CHG_ST()           (GPIOE_IDR & GPIO_Pin_1)

#define LCD_ROW             320
#define LOGO_X              4*8
#define X_SIZE              300
#define Y_SIZE              200


#define SPIx                SPI2

#define EECS_LOW()          GPIOB_BRR  = GPIO_Pin_12 /* ECS = 0 */
#define EECS_HIGH()         GPIOB_BSRR = GPIO_Pin_12



#define RCC_CFG()           RCC_CFGR    = 0x001D8402;/*RCC peripheral clock config
                                            |||||||+--Bits3~0 = xx10 PLL used as sys clock
                                            ||||||+---Bits7~4 = 0000 AHB clock = SYSCLK
                                            |||||+----Bits10~8  = 100 PCLK1=HCLK divided by 2
                                            ||||++----Bits13~11 = 000 PCLK2=HCLK
                                            ||||+-----Bits15~14 = 10 ADC prescaler PCLK2 divided by 6
                                            |||+------Bit16 =1 HSE oscillator clock selected as PLL input clock
                                            |||+------Bit17 =0 HSE clock not divided
                                            ||++------Bits21~18 =0111 PLL input clock x 9
                                            ||+-------Bit22=0 USB prescaler is PLL clock divided by 1.5
                                            |+--------Bits26~24 =000 MCO as No clock
                                            |+--------Bits26~24 =100 MCO as SYSCLK
                                            |+--------Bits26~24 =111 MCO as SYSCLK/2
                                            ++--------Bits31~27 Reserved*/
#define RCC_AHB_CFG()       RCC_AHBENR  = 0x00000117;/*AHB peripheral clock config
                                            |||||||+--Bit3~0 =x111 SRAM, DMA2~1 clock En
                                            ||||||+---Bit7~3 =x0x1 CRC disabled, FLITF(Sleep mode) En
                                            ||||||+---Bit6 =0  clock
                                            |||||+----Bit11~8 = x0x1 FSMC clock enabled
                                            |||||+----Bit10=0 SDIO clock disabled
                                            ++++++----Bits31:11=0 Reserved*/
#define RCC_APB2_CFG()      RCC_APB2ENR = 0x0000AE7D;/* APB2 peripheral config
                                            |||||||+--Bit3~0   = 11x1 port B~A, Alternate En
                                            ||||||+---Bit7~4   = 0111 port F disabled, port E~C En
                                            |||||+----Bit11~8  = 1110 ADC1~2 TIM1 En, port G disabled
                                            ||||+-----Bit15~12 = 1010 ADC3 TIM8 En & USART1 SPI1 disabled
                                            ++++------Bit31:17 = Reserved*/
#define RCC_APB1_CFG()      RCC_APB1ENR = 0x20E0403F;/* APB1 peripheral clock config
                                            |||||||+--Bit3~0   = 1111 Timer5~2 clock enabled
                                            ||||||+---Bit7~4   = xx11 Tim7 & Tim6Ensabled
                                            |||||+----Bit11~8  = 0xxx Window watchdog clock disabled
                                            ||||+-----Bit15~14 = 01xx SPI3 disabled & SPI2 enabled
                                            |||+------Bit19~16 = 000x USART4~2 clock disabled
                                            ||+-------Bit23~20 = 1110 USB & I2C1~2 En USART5 disabled
                                            |+--------Bit27~24 = 0x0x Backup, CAN disabled
                                            +---------Bit31~28 = xx10 DAC En &  Power interfacedisabled*/
#define TIMER_IRQ_CH        TIM6_IRQChannel
#define TIMER_INT_EN()      TIM6_CR1  |= 0x0001;     // TIM6 En
#define TIMER_IF_CLR()      TIM6_SR    = 0;          // Clear TIM6 interrupt flag

#define GPIOA_OUTPUT()      GPIOA_ODR  = 0x0000;
#define GPIOA_L_DEF()       GPIOA_CRL  = 0xBB04B003; /* PA  PB  DCo Afo Pfo Bin Bin AH  */
#define GPIOA_H_DEF()       GPIOA_CRH  = 0x388BB888; /* Pon SWC SWD D+  D-  RX  TX  xxx */

#define GPIOB_OUTPUT()      GPIOB_ODR  = 0xF0F0;
#define GPIOB_L_DEF()       GPIOB_CRL  = 0xBB88B433; /* SDA SCL xxx xxx BL  BT1 BH  BM  */
#define GPIOB_H_DEF()       GPIOB_CRH  = 0xBBB333B8; /* SI  S0  SCK ECR BG  BC  BL  xxx */

#define GPIOC_OUTPUT()      GPIOC_ODR  = 0xF000;
#define GPIOC_L_DEF()       GPIOC_CRL  = 0x88330030; /* xxx xxx AG  AC  Ain Ain AM  Vbt */
#define GPIOC_H_DEF()       GPIOC_CRH  = 0x88884444; /* xxx xxx xxx xxx Vin KEY Din Cin */

#define GPIOD_OUTPUT()      GPIOD_ODR  = 0xEFFF;
#define GPIOD_L_DEF()       GPIOD_CRL  = 0xB3BB38BB; /* NE1 CLR NWE NOE RS  xxx D03 D02 */
#define GPIOD_H_DEF()       GPIOD_CRH  = 0xBB333BBB; /* D01 D00 ZCE ECS APE D15 D14 D13 */

#define GPIOE_OUTPUT()      GPIOE_ODR  = 0xFFFE;
#define GPIOE_L_DEF()       GPIOE_CRL  = 0xB8888888; /* D04 xxx xxx xxx xxx xxx CHG SLP */
#define GPIOE_H_DEF()       GPIOE_CRH  = 0xBBBBBBBB; /* D05 D06 D07 D08 D09 D10 D11 D12 */

#define GPIO_AF_MAP()       AFIO_MAPR  = 0x02000000  /* Bit26~24 = 010 Only SW-DP enable*/

#define SAMPLING_PSC(n)     TIM1_PSC   = n
#define SAMPLING_ARR(n)     TIM1_ARR   = n
#define SAMPLING_CCR(n)     TIM1_CCR1  = n
#define SAMPLING_CLK_CFG()  TIM1_PSC   = 36-1;   \
                            TIM1_ARR   = 8-1; \
                            TIM1_CCR1  = 4; \
                            TIM1_DIER  = 0x0100; /* 允许更新的DMA请求 */ \
                            TIM1_CR1   = 0x0095  /* Reload, DMAEn */

#define PLUSE_OUTPUT_CFG()  TIM5_PSC   = 4-1;    \
                            TIM5_ARR   = 18000-1;\
                            TIM5_CCR4  = 18000/2;\
                            TIM5_CCER  = 0x1000; /* OC4(Active high)*/ \
                            TIM5_CCMR2 = 0x7800; /* CH4(PWM 2, Preload)*/ \
                            TIM5_CR1   = 0x0081  /* Reload En, TIM5 En */

#define OFFSET_A(Val)       TIM3_CCR2  = Val
#define OFFSET_B(Val)       TIM3_CCR1  = Val
#define INP_OFFSET_CFG()    TIM3_PSC   = 0;      \
                            TIM3_ARR   = 4096;   \
                            TIM3_CCER  = 0x0033; /* OC1~2  active low */ \
                            TIM3_CCMR1 = 0x7878; /* CH1~2 PWM 2, Preload */ \
                            OFFSET_A(1024);      \
                            OFFSET_B(1024);      \
                            TIM3_CR1   = 0x0081  /* Reload, Up, TIM4 En */


#define SYSTEM_INT_CFG()    TIM6_PSC   = 72-1;   \
                            TIM6_ARR   = 1000-1; /* 1mS */ \
                            TIM6_DIER  = 0x0001; /* Interrupt En */ \
                            TIM6_CR1   = 0x0085  /* Reload, TIM6 En */


#define V_BTY()             ADC3_DR
#define ADC_CFG()           ADC3_CR1   = 0x00000000; \
                            ADC2_CR1   = 0x00000000; \
                            ADC1_CR1   = 0x00060000; /*ADC 在配置改变前要关闭双模式
                                              |  +- = xx1x EOC interrupt enabled
                                              +---- = 0110 Regular simultaneous mode
                                              +---- = 0111 Fast interleaved mode*/ \
                            ADC3_CR2   = 0x005E0002; \
                            ADC2_CR2   = 0x00100000; \
                            ADC1_CR2   = 0x00100100; /*ADC control register 2
                                             || | +-- = xx1x Continuous mode enabled
                                             || +---- = 0001 DMA mode enabled
                                             |+------ = 1110 SWSTART
                                             |+------ = 0000 Tim1 cc1 trigger
                                             +------- = xxx1 Conversion on external event enabled
                                             +------- = x1xx conversion of regular channels*/ \
                            ADC3_SQR3  = 0x0000000A; /* 1st conversion in regular sequence = CH10 */ \
/*                            INDEPENDENCE();*/\
                            ADC3_SMPR1 = 0x00000006; /* Adc CH10 Sample time is 71.5T */ \
                            ADC2_SMPR2 = 0x00000000; /* Adc CH12 Sample time is 1.5T */ \
                            ADC2_SMPR1 = 0x00000000; /* Adc CH2  Sample time is 1.5T */ \
                            ADC1_SMPR2 = 0x00000000; /* Adc CH11 Sample time is 1.5T */ \
                            ADC1_SMPR1 = 0x00000000; /* Adc CH1  Sample time is 1.5T */ \
                            ADC3_CR2  |= 0x00000001; /* ADON=1：开启ADC3 */ \
                            ADC2_CR2  |= 0x00000001; /* ADON=1：开启ADC2 */ \
                            ADC1_CR2  |= 0x00000001; /* ADON=1：开启ADC1 */ \
                            ADC3_CR2  |= 0x00000008; \
                            while(ADC3_CR2 & 0x00000008); /* 初始化ADC3校准寄存器 */ \
                            ADC2_CR2 |= 0x00000008;  \
                            while(ADC2_CR2 & 0x00000008); /* 初始化ADC2校准寄存器 */ \
                            ADC1_CR2 |= 0x00000008;  \
                            while(ADC1_CR2 & 0x00000008); /* 初始化ADC1校准寄存器 */ \
                            ADC3_CR2 |= 0x00000004; \
                            while(ADC2_CR2 & 0x00000004); /* ADC3校准 */ \
                            ADC2_CR2 |= 0x00000004; \
                            while(ADC2_CR2 & 0x00000004); /* ADC2校准 */ \
                            ADC1_CR2 |= 0x00000004; \
                            while(ADC1_CR2 & 0x00000004); /* ADC1校准 */ \
                            ADC3_CR2 |= ADC_CR2_SWSTART   /* ADC3 开始连续转换 */

#define ANALOG_OUT(Val)     DAC_DHR12R1 = Val
#define OUT_OFFSET(Val)     DAC_DHR12R2 = Val

#define ANALOG_OUT_CFG()    ANALOG_OUT(2048);\
                            OUT_OFFSET(0);   \
                            DAC_CR = 0x00010001  /* DAC Ch1 &Ch2 Enable */
                                   //     |  |+-- Bit0   =xxx1 Ch1 enable
                                   //     |  +--- Bit7~6 =1x Triangle wave En
                                   //     +------ Bit16  =xxx1 Ch2 enable

#define SET_ANALOG_OUT()    GPIOA_CRL  |= 0x00004000; /* Afo = Act, Pfo = Hiz */ \
                            GPIOA_CRL  &= 0xFFF04FFF; \
                            TIM7_CR1    = 0x0085      /* Reload, TIM7 En */
#define SET_PLUSE_OUT()     GPIOA_CRL  |= 0x0004B000; /* Afo = Hiz, Pfo = Act */ \
                            GPIOA_CRL  &= 0xFFF4BFFF; \
                            TIM7_CR1    = 0x0084      /* Reload, TIM7 disabled */
#define PLUSE_PSC(Val)      TIM5_PSC   = Val
#define PLUSE_ARR(Val)      TIM5_ARR   = Val
#define PLUSE_CCR(Val)      TIM5_CCR4  = Val

#define ANALOG_PSC(Val)     TIM7_PSC   = Val
#define ANALOG_ARR(Val)     TIM7_ARR   = Val

#define OUTPUT_CMD          0x25B1
#define ANALOG_CMD          0x3A81
                          //  |||+-- Bit0      =xxx0/1 DMA_CHx disabled/enable
                          //  |||+-- Bit1      =xx0/1x DMA complete interrupt disabled/enable
                          //  ||+--- Bit4      =xxx1 Read from memory
                          //  ||+--- Bit5      =xx1x Circular mode enabled
                          //  ||+--- Bit7      =1xxx Memory increment mode enabled
                          //  |+---- Bits9~8   =xx01 Peripheral size=16 bits
                          //  |+---- Bits9~8   =xx10 Peripheral size=32 bits
                          //  |+---- Bits11~10 =01xx Memory size=16 bits
                          //  |+---- Bits11~10 =10xx Memory size=32 bits
                          //  +----- Bits13~12 =xx00 Priority level Low
                          //  +----- Bits13~12 =xx01 Priority level Medium
                          //  +----- Bits13~12 =xx11 Priority level very high
                          //  +----- Bits14    =x1xx M to M mode enabled

#define OUTPUT_DMA_CFG(n)   DMA2_CCR4   = 0;  \
                            DMA2_CPAR4  = DAC_DHR12R1_ADDR; /* CH4 peripheral address  */ \
                            DMA2_CMAR4  =(u32)OutBuff;      /* CH4 Memory address      */ \
                            DMA2_CNDTR4 = n;                /* CH4 counter(360*2 Bytes)*/ \
                            DMA2_CCR4   = OUTPUT_CMD

#define INPUT_DMA_RST(n)    DMA1_CCR1   = 0;\
                            DMA1_CPAR1  = ADC1_DR_ADDR;     /* CH_A_B peripheral address  */ \
                            DMA1_CMAR1  =(u32)SmplBuff;     /* CH_A_B Memory address      */ \
                            DMA1_CNDTR1 = n                 /* CH_A_B counter(4K*4 Bytes) */
#define INPUT_DMA_EN()      DMA1_CCR1   = ANALOG_CMD

#define SMPL_CNT(Val)      (Val - DMA1_CNDTR1)

#define FILL_DMA            DMA1_Channel2_BASE
#define SEND_DMA            DMA2_Channel2_BASE


/******************************** END OF FILE *********************************/
