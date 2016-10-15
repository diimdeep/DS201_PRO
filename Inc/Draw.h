/********************* (C) COPYRIGHT 2012 e-Design Co.,Ltd. ********************
 File Name : Draw.h
 Version   : For DS201 Ver 3.0                                   Author : bure
*******************************************************************************/
#ifndef __DRAW_H
#define __DRAW_H

#include "stm32f10x_lib.h"

//========== ������ʾ������ض��� ===========

#define LCD_X1     0
#define LCD_X2     319
#define LCD_Y1     0
#define LCD_Y2     239
#define X_SIZE     300
#define Y_SIZE     200
#define X_BASE     1
#define Y_BASE     1
#define MIN_X      1
#define MIN_Y      19
#define MAX_X      (X_SIZE + MIN_X)
#define MAX_Y      (Y_SIZE + MIN_Y)

//============= ��ɫ��ɫ�ʶ��� ===============

#define CYN        0xFFE0  // 0
#define PUR        0xF81F  // 1
#define YEL        0x07FF  // 2
#define GRN        0x07E0  // 3
#define CYN_       0x8400  // 4
#define PUR_       0x8010  // 5
#define YEL_       0x0410  // 6
#define GRN_       0x0410  // 7

#define ORN        0x051F  // 8
#define BLK        0x0000  // 9
#define WHT        0xFFFF  // 10
#define BLU        0xFC10  // 11
#define RED        0x001F  // 12
#define GRY        0x7BEF  // 13
#define LGN        0x27E4  // 14
#define DAR        0x39E7  // 15

//=========== ��ʾ���ڱ�־λ���� =============

#define SHOW       0       // All Show
#define D_HID      1       // End Dot & Line Hide
#define L_HID      2       // Only Line Hide
#define W_HID      4       // Wave Hide
#define A_HID      7       // End Dot & Line & Wave Hide
#define P_HID      1       // Pop Hide
#define LIST_POP   2       //
#define DAILOG_POP 4       //
#define DEMO_POP   8       //

//=========== ���ڲ������б�־λ�� ===========

#define P1F        0
#define P2F        1
#define P3F        2
#define P4F        3
#define VTF        4
#define V1F        5
#define V2F        6
#define T0F        7
#define T1F        8
#define T2F        9

//=========== ���ڲ������б���λ�� ============
#define P1x2       10
#define P2x2       11
#define P3x2       12
#define P4x2       13
#define VTx2       14
#define V1x2       15
#define V2x2       16
#define T0x1       17
#define T1x1       18
#define T2x1       19

//=========== ���ڲ���������ɫλ�� ============
#define PC1        20
#define PC2        21
#define PC3        22
#define PC4        23
#define VTC        24
#define V1C        25
#define V2C        26
#define T0C        27
#define T1C        28
#define T2C        29
#define PC1_       30
#define PC2_       31
#define PC3_       32
#define PC4_       33

//========= ���ڲ������е���������λ�� =========

#define BXx1       34           // Pop x0 Color
#define WXx1       35           // Pop Width
#define BYx2       36           // Pop y0 *2
#define HYx2       37           // Pop Hight *2
#define POP_F      38           // Pop Flag
#define POP_B      39           // Pop Board Color
#define POP_T      40           // Pop Text Color

//=============== ��ʾ��ʽ =====================

#define PRN        0            // ������ʾ
#define INV        1            // ������ʾ
#define SYMB       2            // ��ʾͼ��
#define VOID       4            // ������ʾ

//============== ��ʾ�������� ==================

#define POP_SIZE        1600
#define TRACK1_SIZE     300
#define TRACK2_SIZE     300
#define TRACK23_SIZE    300


extern uc8 DEMO[];
extern u8  WaveBuf[], Data[], PopType, Cal_flag;
extern s16 ParamTab[];
extern u16 Nx, Ny, Background, Foreground;


void PopColor(u16 Board_Color, u16 Text_Color);
void SetColor(u16 Board_Color, u16 Text_Color);
void NotePixel(u8 Color);
void Buil_Pop(void);
void NoteChar(u8 Mode, u8 Code);
void DispChar(u8 Mode, u8 Code);
void NoteSTR(u16 Nx0, u16 Ny0, u8 Mode, u8 *Str);
void DispStr(u16 x0,  u16 y0,  u8 Mode, u8 *Str);
void List_Pop(void);
void Close_Pop(void);
void Demo_Pop(void);
void Dialog_Pop(u8* Str);
void ToolTip(u8* Str);
void Calibrate_Pop(u8* Str);
void CalSave_Pop(u8* Str);
#endif

/********************************* END OF FILE ********************************/
