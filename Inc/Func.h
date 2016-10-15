/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Function.h
 Version   : DS203_APP Ver 2.3x                                  Author : bure
*******************************************************************************/
#ifndef __Function_H__
#define __Function_H__

#include "stm32f10x_lib.h"


#define SIGN         0
#define UNSIGN       1
#define STD          2

extern u32 TestCnt;

u32  Power(u8 x, u8 y);
u_64  Exp(u8 x);
u8*  SeekStr(u8* ptr, u8 Idx);

void Delayms(u32 mS);
void Int2Str(u8 *p, s32 n, uc8 *pUnit, u8 e, u8 Mode);

void Value2Str(u8 *p, s_64 n, uc8 *pUnit, s8 e, u8 Mode);
u8   Str2Byte(u8 x,u8 y);
void u16ToDec4Str(u8 *p, u16 n);
void u16ToDec5Str(u8 *p, u16 n);
void s16ToDec5(u8 *p, s16 n);
void u8ToDec3(u8 *p, u8 n);
void u8ToDec2(u8 *p, u8 n);
void Char2Hex(u8 *p, u8 n);
void Shor2Hex(u8 *p, u16 n);
void Word2Hex(u8 *p, u32 n);
u16  Sqrt32(u32 n);
void s8ToPercen(u8 *p, s8 n);

#endif
/********************************* END OF FILE ********************************/
