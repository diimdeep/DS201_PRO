/******************** (C) COPYRIGHT 2011 e-Design Co., Ltd. ********************
 File Name : Interrupt.c
 Version   : STM32_DFU Ver 3.1                                   Author : bure
*******************************************************************************/
#include "Interrupt.h"
#include "Version.h"
#include "USB_istr.h"
#include "USB_int.h"
#include "Bios.h"

void NMIException(void)
{}

void HardFaultException(void)
{
  while (1) {}
}

void MemManageException(void)
{
  while (1) {}
}

void BusFaultException(void)
{
  while (1) {}
}
void UsageFaultException(void)
{
  while (1) {}
}

void DebugMonitor(void)
{}

void SVCHandler(void)
{}

void PendSVC(void)
{}

void SysTickHandler(void)
{}

void WWDG_IRQHandler(void)
{}

void PVD_IRQHandler(void)
{}

void TAMPER_IRQHandler(void)
{}

void RTC_IRQHandler(void)
{}

void FLASH_IRQHandler(void)
{}

void RCC_IRQHandler(void)
{}

void EXTI0_IRQHandler(void)
{}

void EXTI1_IRQHandler(void)
{}

void EXTI2_IRQHandler(void)
{}

void EXTI3_IRQHandler(void)
{}

void EXTI4_IRQHandler(void)
{}

void DMA1_Channel1_IRQHandler(void)
{}

void DMA1_Channel2_IRQHandler(void)
{}

void DMA1_Channel3_IRQHandler(void)
{}

void DMA1_Channel4_IRQHandler(void)
{}

void DMA1_Channel5_IRQHandler(void)
{}

void DMA1_Channel6_IRQHandler(void)
{}

void DMA1_Channel7_IRQHandler(void)
{}

void ADC1_2_IRQHandler(void)
{}

void USB_HP_CAN_TX_IRQHandler(void)
{
  CTR_HP();
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
  USB_Istr();
}

void CAN_RX1_IRQHandler(void)
{}

void CAN_SCE_IRQHandler(void)
{}

void EXTI9_5_IRQHandler(void)
{}

void TIM1_BRK_IRQHandler(void)
{}

void TIM1_UP_IRQHandler(void)
{

}

void TIM1_TRG_COM_IRQHandler(void)
{}

void TIM1_CC_IRQHandler(void)
{}

void TIM2_IRQHandler(void)
{}

void TIM3_IRQHandler(void)
{
  Tim3_ISP();
}

void TIM4_IRQHandler(void)
{}

void I2C1_EV_IRQHandler(void)
{}

void I2C1_ER_IRQHandler(void)
{}

void I2C2_EV_IRQHandler(void)
{}

void I2C2_ER_IRQHandler(void)
{}

void SPI1_IRQHandler(void)
{}

void SPI2_IRQHandler(void)
{}

void USART1_IRQHandler(void)
{}

void USART2_IRQHandler(void)
{}

void USART3_IRQHandler(void)
{}

void EXTI15_10_IRQHandler(void)
{}

void RTCAlarm_IRQHandler(void)
{}

void USBWakeUp_IRQHandler(void)
{}

void TIM8_BRK_IRQHandler(void)
{}

void TIM8_UP_IRQHandler(void)
{}

void TIM8_TRG_COM_IRQHandler(void)
{}

void TIM8_CC_IRQHandler(void)
{}

void ADC3_IRQHandler(void)
{}

void FSMC_IRQHandler(void)
{}

void SDIO_IRQHandler(void)
{}

void TIM5_IRQHandler(void)
{
  #ifndef DFU_MODE

  #endif
}

void SPI3_IRQHandler(void)
{}

void UART4_IRQHandler(void)
{}

void UART5_IRQHandler(void)
{}

void TIM6_IRQHandler(void)
{

}

void TIM7_IRQHandler(void)
{}

void DMA2_Channel1_IRQHandler(void)
{
}

void DMA2_Channel2_IRQHandler(void)
{}

void DMA2_Channel3_IRQHandler(void)
{}

void DMA2_Channel4_5_IRQHandler(void)
{}


/********************************* END OF FILE ********************************/
