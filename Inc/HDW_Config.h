/******************** (C) COPYRIGHT 2012 e-Design Co.,Ltd. *********************
 File Name : HDW_Config.h
 Version   : DS201 Ver 3.0                                       Author : bure
*******************************************************************************/
#ifndef __HDW_CONFIG_H
#define __HDW_CONFIG_H

#include "stm32f10x_lib.h"
#include "STM32F10x.h"
//============================ Flash 空间分配 ================================//
#define LOGO_BASE      ((u32)(0x0801C800)) // Size = 12KB


//============================ SD卡控制相关定义 ==============================//

#define MSD_CS_LOW()   GPIOB_BRR  = GPIO_Pin_12  //Select MSD Card
#define MSD_CS_HIGH()  GPIOB_BSRR = GPIO_Pin_12  //Deselect MSD Card

//========================== 模拟通道控制相关定义 ============================//

#define RANGE_A_LOW()  GPIOB_BRR  = GPIO_Pin_0
#define RANGE_A_HIGH() GPIOB_BSRR = GPIO_Pin_0
#define RANGE_B_LOW()  GPIOC_BRR  = GPIO_Pin_5
#define RANGE_B_HIGH() GPIOC_BSRR = GPIO_Pin_5
#define RANGE_C_LOW()  GPIOC_BRR  = GPIO_Pin_4
#define RANGE_C_HIGH() GPIOC_BSRR = GPIO_Pin_4
#define RANGE_D_LOW()  GPIOB_BRR  = GPIO_Pin_1
#define RANGE_D_HIGH() GPIOB_BSRR = GPIO_Pin_1

//============================ 按键输入相关定义 ==============================//

#define KEY_UP         GPIO_Pin_6  //GPIOA
#define KEY_DOWN       GPIO_Pin_9  //GPIOD
#define KEY_LEFT       GPIO_Pin_5  //GPIOA
#define KEY_RIGHT      GPIO_Pin_7  //GPIOA
#define KEY_PLAY       GPIO_Pin_4  //GPIOA
#define KEY_MENU       GPIO_Pin_11 //GPIOD

//============================= LCD 控制相关定义 =============================//

#define LDC_DATA_OUT   GPIOE_ODR
#define LDC_DATA_INP   GPIOE_IDR
#define LCD_RS_LOW()   GPIOD_BRR  = GPIO_Pin_1
#define LCD_RS_HIGH()  GPIOD_BSRR = GPIO_Pin_1
#define LCD_nWR_LOW()  GPIOD_BRR  = GPIO_Pin_5
#define LCD_nWR_HIGH() GPIOD_BSRR = GPIO_Pin_5
#define LCD_nWR_ACT()  GPIOD_BSRR = GPIOD_BRR  = GPIO_Pin_5
#define LCD_nRD_LOW()  GPIOD_BRR  = GPIO_Pin_4
#define LCD_nRD_HIGH() GPIOD_BSRR = GPIO_Pin_4
#define LCD_nRD_ACT()  GPIOD_BSRR = GPIOB_BRR  = GPIO_Pin_4


#define AFIO_MAPR      (*((vu32 *)(AFIO_BASE+0x04)))

#define SCS_BASE       ((u32)0xE000E000)
#define SysTick_BASE   (SCS_BASE + 0x0010)
//============================ 硬件初始化函数声明 ============================//

void Set_System(void);
void NVIC_Configuration(void);
void GPIO_Config(void);
void SPI_Config(void);

void DMA_Configuration(void);
void ADC_Configuration(void);
void Tim_Config(void);

#endif
/****************************** END OF FILE ***********************************/
