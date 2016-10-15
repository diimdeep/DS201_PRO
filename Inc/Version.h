/********************* (C) COPYRIGHT 2012 e-Design Co.,Ltd. ********************
 Brief : Device version description                              Author : bure
*******************************************************************************/
#include "stm32f10x_lib.h"
#include "STM32F10x.h"

#ifndef __VERSION_H
#define __VERSION_H

#define TYPE_DS201 // TYPE_DS202 // TYPE_DS203
#define APP_MODE   // DFU_MODE   //

typedef  void (*pFunc)(void);

/*============================================================================*/
#ifdef TYPE_DS201

  #include "DS201V3_0.h"

  #define PRODUCT_INFO      "DS201"
  #define SCH_VER           "V3.0"
  #define MCU_TYPE          "STM32F103VB"
  #define ADC_TYPE          "MCU's ADC"
  #define LIB_VER           "Ver3.00" // "Ver3.01"
  #define LCD_ST_7781       // LCD_ILI_9341

  #ifdef LCD_ST_7781
    #define LCD_TYPE        "ST7781" // LCD Controller = ST7781
    #define VERSION_STR     'D','F','U',' ','V','3','_','1','1','_','A',8
    #define DFU_VER         "V3.21A"
  #endif

  #ifdef LCD_ILI_9341
    #define LCD_TYPE        "ILI9341" // LCD Controller = ILI9341
    #define VERSION_STR     'D','F','U',' ','V','3','_','1','1','_','D',8
    #define DFU_VER         "V3.xxD"
  #endif

#endif
//============================================================================//
#ifdef TYPE_DS202

  #include "DS202V1_3.h"

  #ifdef DFU_MODE
    #define LCD_ILI_9341 // LCD_ST_7781 //

    #ifdef LCD_ST_7781
      #define LCD_TYPE        "ST7781" // LCD Controller = ST7781
      #define VERSION_STR     'D','F','U',' ','V','3','_','3','0','_','E',8
      #define DFU_VER         "V3.30E"
    #endif

    #ifdef LCD_ILI_9341
      #define LCD_TYPE        "ILI 9341" // LCD Controller = ILI 9341
      #define VERSION_STR     'D','F','U',' ','V','3','_','3','0','_','F',8
      #define DFU_VER         "V3.30E"
    #endif

  #endif
#endif



#endif /* VERSION_H */
/*********************************  END OF FILE  ******************************/
