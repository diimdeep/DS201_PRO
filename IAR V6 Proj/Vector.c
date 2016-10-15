/********************* (C) COPYRIGHT 2012 e-Design Co.,Ltd. ********************
 File Name   : Vector.c                                  Author : bure
 Description : STM32F10x vector table for EWARM4.x toolchain.
               This module performs:
               - Set the initial SP
               - Set the initial PC == __program_start,
               - Set the vector table entries with the exceptions ISR address
********************************************************************************
附加说明：
 1. 编译选项为 DFU_MODE 时生成底层程序入口向量表
 2. 编译选项为 APP_MODE 时生成中断程序入口向量表

*******************************************************************************/
#include "stm32f10x_lib.h"
#include "Interrupt.h"
#include "Version.h"
#include "Display.h"
#include "Ident.h"
//#include "Bios.h"

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;
void __iar_program_start( void );

#pragma language = extended
#pragma segment  = "CSTACK"
#define VER6
#ifdef VER6
  void __iar_program_start( void );
#else  
  void __program_start( void );
#endif  
#pragma location = "intvec"

//==================== STM32F10x DFU Vector Table entries ======================
const intvec_elem __vector_table[] =
{
  { .__ptr = __sfe( "CSTACK" ) }, // 0x00
#ifdef VER6  
  __iar_program_start,                // 0x04
#else  
  __program_start,                // 0x04
#endif  

#ifdef DFU_MODE
    V_LCD_Init,                   // 0x08 NMIException
    V_SetBlock,                   // 0x0C HardFaultException
    V_SetPosi,                    // 0x10 MemManageException
    V_SetPixel,                   // 0x14 BusFaultException
    V_ReadPixel,                  // 0x18 UsageFaultException
    V_Lic_Gen,                    // 0x1C = 0,
    V_Dev_SN,                     // 0x20 = 0,
    V_Ident,                      // 0x24 = 0,
    V_Chk_SYS,                    // 0x28 = 0,
    V_Lic_ok,                     // 0x2C SVCHandler
    V_GetChar8x14,                // 0x30 DebugMonitor
    V_Dev_Info,                   // 0x34 = 0,
    V_Direction,                  // 0x38 SVC,
    0, 0, 0, 0,                   // SysTick, WWDG, PVD, TAMPER,
    0, 0, 0, 0, 0, 0, 0, 0,       // RTC, FLASH, RCC, EXTI0~4,
    0, 0, 0, 0, 0, 0, 0, 0,       // DMA1_Ch1~7, ADC1~2,

    USB_HP_CAN_TX_IRQHandler,
    USB_LP_CAN_RX0_IRQHandler,
#else
    NMIException,
    HardFaultException,
    MemManageException,
    BusFaultException,
    UsageFaultException,
    0, 0, 0, 0,                   // Reserved
    SVCHandler,
    DebugMonitor,
    0,                            // Reserved
    PendSVC,
    SysTickHandler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMPER_IRQHandler,
    RTC_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_IRQHandler,
    DMA1_Channel3_IRQHandler,
    DMA1_Channel4_IRQHandler,
    DMA1_Channel5_IRQHandler,
    DMA1_Channel6_IRQHandler,
    DMA1_Channel7_IRQHandler,
    ADC1_2_IRQHandler,
    USB_HP_CAN_TX_IRQHandler,
    USB_LP_CAN_RX0_IRQHandler,
    CAN_RX1_IRQHandler,
    CAN_SCE_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_IRQHandler,
    TIM1_UP_IRQHandler,
    TIM1_TRG_COM_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    EXTI15_10_IRQHandler,
    RTCAlarm_IRQHandler,
    USBWakeUp_IRQHandler,
    TIM8_BRK_IRQHandler,
    TIM8_UP_IRQHandler,
    TIM8_TRG_COM_IRQHandler,
    TIM8_CC_IRQHandler,
    ADC3_IRQHandler,
    FSMC_IRQHandler,
    SDIO_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    TIM6_IRQHandler,
    TIM7_IRQHandler,
    DMA2_Channel1_IRQHandler,
    DMA2_Channel2_IRQHandler,
    DMA2_Channel3_IRQHandler,
    DMA2_Channel4_5_IRQHandler,
  #endif
};

/********************************* END OF FILE ********************************/
