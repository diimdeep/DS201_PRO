/********************* (C) COPYRIGHT 2011 e-Design Co.,Ltd. ********************
 File Name : BIOS.h
 Version   : For DS202 Ver 1.0                                   Author : bure
*******************************************************************************/
#ifndef __BIOS_H
#define __BIOS_H

#include "stm32f10x_lib.h"
#define TYPE_DS201
#define LCD_DATA_OUT        GPIOE_ODR
#define LCD_nWR_ACT()       GPIOD_BSRR = GPIOD_BRR  = GPIO_Pin_5

//-------------------------------------------------------------
#ifdef TYPE_DS201
  #define LCD_DATA_INP        GPIOE_IDR
  #define LCD_DATA_BUS_INP()  GPIOE_CRL = GPIOC_CRH = 0x44444444
  #define LCD_DATA_BUS_OUT()  GPIOE_CRL = GPIOC_CRH = 0x33333333
  #define LCD_nRST_LOW()      GPIOC_BRR  = GPIO_Pin_0
  #define LCD_nRST_HIGH()     GPIOC_BSRR = GPIO_Pin_0
  #define LCD_RS_LOW()        GPIOD_BRR  = GPIO_Pin_1
  #define LCD_RS_HIGH()       GPIOD_BSRR = GPIO_Pin_1
  #define LCD_nWR_LOW()       GPIOD_BRR  = GPIO_Pin_5
  #define LCD_nWR_HIGH()      GPIOD_BSRR = GPIO_Pin_5
  #define LCD_nRD_LOW()       GPIOD_BRR  = GPIO_Pin_4
  #define LCD_nRD_HIGH()      GPIOD_BSRR = GPIO_Pin_4
  #define LCD_nRD_ACT()       GPIOD_BSRR = GPIOB_BRR  = GPIO_Pin_4
#endif
//============================ 按键输入相关定义 ==============================//

#define KEYCODE_VOID    0
#define KEYCODE_PLAY    1
#define KEYCODE_MANU    2
#define KEYCODE_UP      3
#define KEYCODE_DOWN    4
#define KEYCODE_LEFT    5
#define KEYCODE_RIGHT   6

#define KEYCODE_B       7
#define KEYCODE_ACT     8
#define KEYCODE_MEM     9
//#define KEYCODE_MEM     10

extern vu8  Key_Buffer, Twink, Blink, Key_Status_Last;
extern vu16 Delay_Cnt, Tim_Cnt, PopupCnt;

u16  Bat_Vol(void);
void Set_Range(u8 Range);
void Set_Base(u8 Base);
void ADC_Start(void);
u16  ADC_Cnt(u16 Value);
void Set_Y_Pos(u16 i, u16 Y0);
u8   USB_On(void);
char SD_Card_ON(void);
char KeyScan(void);
void Tim3_ISP(void);
void Set_Fout(u8 Range);
void Set_Duty(void);

//void LCD_WR_REG(u16 Reg, u16 Data);
void __Set_Posi(u16 x0, u16 y0);
void __Set_Pixel(u16 Color);
void __ClrScrn(u16 Color);
//void LCD_Init(void);
void __Display_Logo(u16 x0, u16 y0);
u8*  __DeviceInfo(u8 Device);
#endif

/*******************************  END OF FILE  ********************************/
