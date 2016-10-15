/******************** (C) COPYRIGHT 2011 e-Design Co.,Ltd. *********************
 File Name : Menu.h
 Version   : For DS202 Ver 1.0                                   Author : bure
*******************************************************************************/
#include "stm32f10x_lib.h"

//======= 电压及时间倍乘系数 ========
#define _4E1       40
#define _8E1       80
#define _2E2       200
#define _4E2       400
#define _8E2       800
#define _2E3       2000
#define _4E3       4000
#define _8E3       8000
#define _2E4       20000
#define _4E4       40000
#define _8E4       80000
//#define _2E5       200000
#define _4E5       400000
#define _8E5       800000
#define _2E6       2000000
#define _4E6       4000000
#define _8E6       8000000
#define _2E7       20000000
#define _4E7       40000000
#define _8E7       80000000



typedef struct { uc8*  PTR;          // 量纲字符串指针
                 s16  Val;           // 项目栏变量值
                 u8   Flg;} item;    // 项目栏相关标记

typedef struct { uc8*  PTR;          // 量纲字符串指针
                 sc16  Val;           // 项目栏变量值
                 uc8   Flg;} item_c;    // 项目栏相关标记

//======== 标题编号 =========
#define YN         0
#define XN         1
#define TR         2
#define ME         3
#define EX         4
#define FN         5
#define SN         6
#define CA         7

//======== 项目编号 ========

typedef enum _ITEM_NUMBER
{
  Y_RANGES, PROBEATT, FITRANGE,   V0_POSI,   V1_POSI,   V2_POSI,  YN_TYPE,
  TIM_BASE, FIT_BASE, VIEWPOSI, SMPL_DPTH,   T1_POSI,   T2_POSI,  XN_TYPE,
  SYNCMODE,  TR_MODE, FITTRIGG, THRESHOLD, SENSITIVE,   TR_TYPE,
  FREQUENC,     DUTY,     VRMS,      VAVG,      VP_P,      VMAX,     VMIN,
  EXT_REFN,  EXT_POSI,
  SAVE_BMP, SAVE_DAT, SAVE_BUF,  SAVE_CSV,  LOAD_DAT,  LOAD_BUF,
  FREQ_OUT, FREQ_DUT,
  CAL_ZERO, CAL_GAIN, RESTORE,CAL_AUTO,
  BATTERY,
} ITEM_NUMBER;

//========== 其他 ==========
#define T_X        303     // 标题栏的起始X坐标
#define N_X          4     // 项目栏在注释块内的起始x坐标

#define RUN        0x00
#define STOP       0x01    // 采样暂停
#define LOAD       0X10

#define BLINK      1
#define TIPS       1       // 自动提示状态
#define UPD        0x20    // 更新标记

extern uc16 TCOLOR[];
extern uc8 FnNote[][14];
extern uc8 V[][6];
extern uc8 R[][6];
extern uc8 N[][6];
extern u8  MenuNow[];
extern uc8  MenuNow_C[];
extern uc8 M_ITEMS[], MU_BEGN[];

extern item List[];
extern item_c List_c[];
extern uc8  ESTR[][8];
extern sc16 MAX[],MIN[];
extern uc16 S_X[];

extern u8    Current, Status, NumStr[];
extern u16   Presampling;
extern u16   Km[], Kn[], Yd[], Pg[];
extern uc16  FPSC[], FARR[], PSC[], ARR[], Ks[], DEPTH[],Knc[],Kmc[];
extern uc8   G[];

void Update_Blink(void);
void Show_List(void);
void Update_Detail(void);
void Update_Titel(void);
void DispStatus(void);
void ClrMeter(void);
void DispMeter(u8 Item, u8 St, u8 Mode, s32 Val);
void ShowTips(u8 Item, u8* ptr);
void ShowView(u16 Posi, u8 Dpth);
void PrintClk(u16 x0, u16 y0, u8 Phase);
void DispFileInfo(u8 Info);
/******************************** END OF FILE *********************************/
