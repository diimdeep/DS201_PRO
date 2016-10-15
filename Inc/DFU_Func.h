/********************* (C) COPYRIGHT 2013 e-Design Co.,Ltd. ********************
 DFU_Func.h   For DFU Ver3.26                                   Author : bure
*******************************************************************************/
#ifndef __DFU_FUNC_H
#define __DFU_FUNC_H

#include "stm32f10x_lib.h"

#define RGB(_R,_G,_B) (((_R & 0x38)>> 1)|((_G & 0x3C)<< 5)|((_B & 0x38)<< 10))

#define GRD_FLAG       0x0040
#define LN1_FLAG       0x0020
#define LN2_FLAG       0x0800
#define WAV_FLAG       0x0080
#define CH2_FLAG       0x0002
#define REF_FLAG       0x1000
#define C_GROUP       (WAV_FLAG | CH2_FLAG | REF_FLAG)
#define F_SELEC       (C_GROUP  | GRD_FLAG | LN1_FLAG | LN2_FLAG)

#define LN1_COLOR     ((RGB(32,63,32) & (~F_SELEC)) | LN1_FLAG)
#define GRD_COLOR     ((RGB(16,16,16) & (~F_SELEC)) | GRD_FLAG)
#define LN2_COLOR     ((RGB(63,63,63) & (~F_SELEC)) | LN2_FLAG)
#define WAV_COLOR     ((RGB( 0,63,63) & (~F_SELEC)) | WAV_FLAG)
#define CH2_COLOR     ((RGB(63,63, 0) & (~F_SELEC)) | CH2_FLAG)
#define REF_COLOR     ((RGB(63, 0,63) & (~F_SELEC)) | REF_FLAG)
#define BACKGROUND    ((RGB( 0, 0, 0)))
#define CH1_COLOR      WAV_COLOR
#define CH3_COLOR      REF_COLOR

typedef enum {SCH, MCU, LCD, ADC, FPGA, DFU, OEM, DISK, NON} DEV_NUM;


void __Delay_mS(u32 mS);
void __Dev_SN(void);
void __Chk_SYS(u32 Licence);
u8   __Lic_ok(void);

void __LCD_Init(void);
void __Add_Color(u16 x, u16 y, u16 Color);

u16  __Get_Pixel(u16 x, u16 y);
void __Set_Pixel(u16 Color);
void __Disp_Logo(u16 x, u16 y);
void __Disp_Str(u16 x, u16 y, u16 Color, u8 *s);

void __SetBlock(u16 x1, u16 y1, u16 x2, u16 y2);
void __ClrScrn(u16 Color);
void __Set_Posi(u16 x, u16 y);
u16  __Font_8x14(u8 Ascii, u16 Row);

void __FLASH_Prog(u32 Address, u16 Data);
void __FLASH_Erase(u32 Address);
u8*  __DevInfoStr(u8 Device);
void __EraseColor(u16 x, u16 y, u16 Color);

void __Set_Pixels(u16 Color, u32 n);
void __SetPixels(u16 *pBuf, u32 n);

void  __SVC_Entry(void);
#endif /* DFU_FUNC_H */
/*********************************  END OF FILE  ******************************/
