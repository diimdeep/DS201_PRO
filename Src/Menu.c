/******************** (C) COPYRIGHT 2011 e-Design Co.,Ltd. *********************
 File Name : Menu.c
 Version   : For DS202 Ver 1.0                                   Author : bure
*******************************************************************************/
#include "Process.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Menu.h"
#include "Draw.h"
#include "Func.h"
#include "BIOS.h"


typedef uint64_t     u64;
//extern s16 HighT,LowT;

// 长按"P"键显示操作帮助提示信息
// 按"P"键再按"M"键保存当前参数
// 在弹出注释框时，长按"M"键执行当前选项(显示/消隐/执行)
// 短按"P"键切换"运行"/"暂停"状态
// 短按"M"键切换项目选择菜单"弹出"/"消隐"状态


//----------+------+------+------+------+------+------+------+------+------+------+
// Y量程x1  |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
// Y量程x10 |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17  |  18  |  19  |
//----------+------+------+------+------+------+------+------+------+------+------+
uc8  R[][6]={"10mV","20mV","50mV","0.1V","0.2V","0.5V","1.0V","2.0V","5.0V","10V",
             "10mV","20mV","50mV","0.1V","0.2V","0.5V","1.0V","2.0V","5.0V","10V" };
//----------+------+------+------+------+------+------+------+------+------+------+
u16  Kn[]  ={   85 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 ,
/*Vol_Diff*/    85 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 };
uc16 Knc[] ={   85 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 ,
                85 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 ,   93 ,   97 ,   99 };
//----------+------+------+------+------+------+------+------+------+------+------+
u16  Km[]  ={  662 ,  684 ,  558 ,  570 ,  528 ,  422 ,  429 ,  538 ,  436 ,  433 ,
/*Vol_Gain*/   662 ,  684 ,  558 ,  570 ,  525 ,  422 ,  418 ,  485 ,  397 ,  386 };
uc16 Kmc[] ={  662 ,  684 ,  558 ,  570 ,  528 ,  422 ,  429 ,  538 ,  436 ,  433 ,
               662 ,  684 ,  558 ,  570 ,  525 ,  422 ,  418 ,  485 ,  397 ,  386 };
//----------+------+------+------+------+------+------+------+------+------+------+
//uc32 Sv[]  ={ _4E2 , _8E2 , _2E3 , _4E3 , _8E3 , _2E4 , _4E4 , _8E4 , _2E5 , _4E5 ,
///*Scale_Vol*/ _4E3 , _8E3 , _2E4 , _4E4 , _8E4 , _2E5 , _4E5 , _8E5 , _2E6 , _4E6 };
//----------+------+------+------+------+------+------+------+------+------+------+
u16   Pg[] ={  197 ,  395 ,  790 , 1590 ,  395 ,  790 , 1590 ,  395 ,  790 , 1590 ,
/*Posi_Gain*/  197 ,  395 ,  790 , 1590 ,  395 ,  790 , 1590 ,  395 ,  790 , 1590 };
//----------+------+------+------+------+------+------+------+------+------+------+
u16   Yd[] ={ 1024 , 2048 , 4096 , 8192 , 2048 , 4096 , 8192 , 2048 , 4096 , 8192 ,
/*Y_Div*/     1024 , 2048 , 4096 , 8192 , 2048 , 4096 , 8192 , 2048 , 4096 , 8192 };
//----------+------+------+------+------+------+------+------+------+------+------+

//----------+------+------+------+------+------+------+------+------+------+------+
// 扫描时基 |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
// 档位号:  |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17  |  18  |  19  |
//----------+------+------+------+------+------+------+------+------+------+------+
uc8  B[][6]={"1uS " ,"2uS " ,"5uS " ,"10uS","20uS","50uS",".1mS",".2mS",".5mS","1mS " ,
             "2mS " ,"5mS " ,"10mS","20mS","50mS","0.1S","0.2S","0.5S"," 1S "," 2S ",};
//----------+------+------+------+------+------+------+------+------+------+------+
//uc32 St[]  ={ _4E1 , _8E1 , _2E2 , _4E2 , _8E2 , _2E3 , _4E3 , _8E3 , _2E4 , _4E4 ,
///*Scale_Tim*/ _8E4 , _2E5 , _4E5 , _8E5 , _2E6 , _4E6 , _8E6 , _2E7 , _4E7 , _8E7 };
//----------+------+------+------+------+------+------+------+------+------+------+
uc16 PSC[] ={ 12-1 , 12-1 , 12-1 , 12-1 , 12-1 , 16-1 , 16-1 , 16-1 , 16-1 , 16-1 ,
              16-1 , 32-1 ,  64-1, 64-1 , 128-1, 256-1, 256-1, 256-1, 512-1, 512-1};
//----------+------+------+------+------+------+------+------+------+------+------+
uc16 ARR[] ={   6  ,   6  ,   6  ,   6  ,   6  ,  9-1 , 18-1 , 36-1 , 90-1 , 180-1,
              360-1, 450-1, 450-1, 900-1,1125-1,1125-1,2250-1,5625-1, 11249, 28124};
//----------+------+------+------+------+------+------+------+------+------+------+
uc16 Ks[]  ={  41  ,  82  ,  205 ,  410 ,  819 , 1024 , 1024 , 1024 , 1024 , 1024 ,
/*插值系数*/  1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 , 1024 };
//----------+------+------+------+------+------+------+------+------+------+------+

//输出频率计算公式：Q=72MHZ/FPSC/FARR)
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
// 输出频率 |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |
// 档位编号:|   8   |   9   |  10   |  11   |  12   |  13   |  14   |  15   |
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
uc8  Q[][6]={ "10Hz", "20Hz", "50Hz","100Hz","200Hz","500Hz","1KHz ","2KHz",
              "5KHz","10KHz","20KHz","50KHz",".1MHz",".2MHz",".5MHz","1MHz"};
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
uc16 FPSC[]={  128-1,  128-1,   64-1,   64-1,   16-1,   16-1,    4-1,    4-1,
                 1-1,    1-1,    1-1,    1-1,    1-1,    1-1,    1-1,    1-1};
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
uc16 FARR[]={56250-1,28125-1,22500-1,11250-1,22500-1, 9000-1, 18000-1,9000-1,
             14400-1, 7200-1, 3600-1, 1440-1,  720-1,  360-1,  144-1,   72-1};
//----------+-------+-------+-------+-------+-------+-------+-------+-------+

/**************************V5.02**********************************/
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//  比例尺定义 | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13|14|15|16|17|18|19|
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//uc8  B_SCALE[]={ 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4};
  uc8  B_SCALE[]={ 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4, 8, 2, 4,8};
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//uc8  T_SCALE[]={ 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6};
  uc8  T_SCALE[]={ 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7};
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
uc8  V_SCALE[]={ 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6};
//-------------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
/***************************************************************/

uc8  P[][6] = {"%"};
uc8  S[][6] = {"/Sec",};
uc8  A[][6] = {"_"   , "@"};
uc8  E[][6] = {"L"   , "H"};
uc8  N[][6] = {"1"   , "10"};
uc8  L[][6] = {"Save", "LOad"};
uc8  T[][6] = {"uS"  , "uS"   , "mS"   , "S " };
uc8  V[][6] = {"uV"  , "mV"   , "V "   , "kV"};
uc8  F[][6] = {"Hz"  , "Hz"   , "Kc"   , "Mc"};
uc8  D[][6] = {"256 ", "512 " , "1K  " , "2K  " , "4K  " , "6K  "};
uc8  M[][6] = {"STOP", "NONE" , "AUTO" , "NORM" , "SINGL"};
uc8  I[][6] = {"Data", "-Data", "Inp+D", "D-Inp", "Inp-D", "-Inp", " Off "};
uc8  C[][6] = {".BMP", ".DAT" , ".BUF" , ".CSV" , ".DAT" , ".BUF"};
uc8  Z[][6] = {"Frqn", "Duty" , "Vrms" , "Vavg" , "Vp_p" , "Vmax" , "Vmin"};

//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
// 标题定义   |垂直 |水平 |触发 |测量 |扩展 |数据 |系统 |校正 |                |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 TSTR[][4]={"Yn" ,"Xn" ,"Tr" ,"Me" ,"Ex" ,"Fn" ,"Sn" ,"Ca" };//标题字符串   |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc16 TCOLOR[]={ CYN , YEL , GRN , WHT , PUR , BLU , ORN , WHT };//标题颜色值   |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 TITEL_Y[]={ 200 , 175 , 150 , 125 , 100 ,  75 ,  50 ,  25 };//标题 Y坐标   |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 BLOCK_X[]={ 214 , 214 , 214 , 230 , 214 , 214 , 214 , 214 };//菜单窗X坐标  |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 BLOCK_Y[]={  45 ,  37 ,  67 ,  30 ,  62 ,  19 ,  11 ,   3 };//菜单窗Y坐标  |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 BLOCK_W[]={  80 ,  80 ,  80 ,  64 ,  80 ,  80 ,  80 ,  80 };//菜单窗宽度   |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 M_ITEMS[]={   7 ,   7 ,   6 ,   7 ,   2 ,   6 ,   2 ,   4 };//菜单项目行数 |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
uc8 MU_BEGN[]={   0 ,   7 ,  14 ,  20 ,  27 ,  29 ,  35 ,  37 };//标题起始编号 |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
u8  MenuNow[]={   0 ,   7 ,  17 ,  20 ,  27 ,  29 ,  35 ,  38 };//当前项目编号 |
uc8 MenuNow_C[]={   0 ,   7 ,  17 ,  20 ,  27 ,  29 ,  35 ,  38 };//当前项目编号 |
//------------+-----+-----+-----+-----+-----+-----+-----+-----+----------------+
//----项目栏英文字符串----------------------------------------------------------------------------------//
uc8 ESTR[][8]={	"Y Ranges",  "ProbeAtt",  "FitRange",  "Y Offset",  "CursorV1",  "CursorV2",  "Hide_Vn_",
								"TimeBase",  "Fit Base",  "ViewPosi",  "SmplDpth",  "CursorT1",  "CursorT2",  "Hide_Tn_",
								"SyncMode",  "TrigMode",  "Auto Fit",  "Threshol",  "Sensitiv",  "Hide-Tri",
								" Freq "  ,  " Duty "  ,  " Vrms "  ,  " Vavg "  ,  " Vp-p "  ,  " Vmax "  ,  " Vmin "  ,
								"Ext Refn",  "Ext Posi",                    
								"Save Bmp",  "Save Dat",  "Save Buf",  "Save Csv",  "Load Dat",  "Load Buf",
								"Out Ferq",  "Out Duty",      
								"Cal Zero",  "CalRange", "Restore ", "Cal Auto",                     
								"Battery " };  
//----项目栏结果信息x坐标--------------------------------------------------------------------------------//							                             
uc16 S_X[]={	56  , 90 ,44  ,120 , 0  , 0  , 0  ,   
							132 ,120 ,100 ,232 , 0  , 0  , 0  ,		        
				  		0   ,228 ,218 ,  0 ,238 , 0  ,	              
				  		232 ,232 ,232 ,232 ,232 ,232 ,232 ,	          
				  		172 , 0  ,                                    
				  		232 ,232 ,232 ,232 ,232 ,232 ,                
				  		0   ,0   ,                                    
				  		80  ,80  ,80, 0,                                   
				  		295}; 
//----项目栏变量值上限--------------------------------------------------------------------------------// 
sc16 MAX[]=	{	9   ,1  ,1    ,198, 198, 198, 1 , 
							19  ,1  ,5844 ,5  , 298, 298, 1 ,
							4   ,1  ,1    ,98 , 98 , 1,
							100 ,100,100  ,100, 100, 100,100, 
							6   ,199, 
							255 ,255,255  ,255,255,255,
							15  ,9  ,
							199 ,2048,99, 9,
							4};
//----项目栏变量值下限--------------------------------------------------------------------------------// 
sc16 MIN[]=	{ 0,0,0,2,2,2,0,   								                
							0, 0, 1, 0, 2,2,0,
							1,0,0,-98,1,0, 
							0,0,0,0,0,0,0,
							0,1,
							0,0,0,0,0,0,
							0,1,
							-199,0,0, 0,
							0};
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
// 项目定义  |    ESTR     |    CSTR        | S_X    |  PTR  | Val | MAX   | MIN   | Flg |
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
item List[]={{/* "Y Ranges",*//*"量程选择"*/ /* 56 ,*/(u8*)R ,   6 , /*  9 ,   0 ,*/ UPD },
             {/* "ProbeAtt",*//*"探头模式"*/ /* 90 ,*/(u8*)N ,   0 , /*  1 ,   0 ,*/ UPD },
             {/* "FitRange",*//*"自动量程"*/ /* 44 ,*/(u8*)A ,   0 , /*  1 ,   0 ,*/ UPD },
             {/* "Y Offset",*//*"垂直偏移"*/ /*120 ,*/     0 ,  50 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV1",*//*"高位游标"*/ /*  0 ,*/(u8*)V , 150 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV2",*//*"低位游标"*/ /*  0 ,*/(u8*)V ,  25 , /*198 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"游标型式"*/ /*  0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "TimeBase",*//*"时基选择"*/ /*132 ,*/(u8*)B ,   7 ,/*  19 ,   0 ,*/ UPD },//
             {/* "Fit Base",*//*"自动时基"*/ /*120 ,*/(u8*)A ,   0 ,/*   1 ,   0 ,*/ UPD },
             {/* "ViewPosi",*//*"水平位移"*/ /*100 ,*/     0 ,   1 ,/*3795 ,   1 ,*/ UPD },
             {/* "SmplDpth",*//*"采样深度"*/ /*232 ,*/(u8*)D ,   2 ,/*   3 ,   0 ,*/   0 },
             {/* "CursorT1",*//*"左端游标"*/ /*  0 ,*/(u8*)T ,  48 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorT2",*//*"右端游标"*/ /*  0 ,*/(u8*)T , 210 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"游标型式"*/ /*  0 ,*/     0 ,   0 ,/*   1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"SyncMode"*//*"同步模式"*//*   0 ,*/(u8*)M ,   2 , /*  4 ,   1 ,*/ UPD },
             { /*"TrigMode"*//*"触发极性"*//* 228 ,*/(u8*)E ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Auto Fit"*//*"自动阈值"*//* 218 ,*/(u8*)A ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Threshol"*//*"触发阈值"*//*   0 ,*/     0 ,  40 , /* 98 ,  -98,*/ UPD },      //198
             { /*"Sensitiv"*//*"触发回差"*//* 238 ,*/     0 ,   5 , /* 98 ,   1 ,*/ UPD },
             { /*"CursorTp"*//*"游标型式"*//*   0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*  " Freq ",*//*  "频率值"*//* 232 ,*/(u8*)F ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Duty ",*//*  "占空比"*//* 232 ,*/(u8*)P ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vrms ",*//*  "有效值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vavg ",*//*  "平均值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vp-p ",*//*  "峰峰值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmax ",*//*  "最大值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmin ",*//*  "最小值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Ext Refn",*//*"参考模式"*/ /*172 ,*/(uc8*)I,   0 , /*  5 ,   0 , */UPD },
             { /*"Ext Posi",*//*"垂直偏移"*/ /*  0 ,*/     0 , 125 , /*199 ,   1 , */UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Save Bmp",*//*"保存图像"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Dat",*//*"保存波形"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Buf",*//*"保存数据"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Csv",*//*"保存资料"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Dat",*//*"读取波形"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Buf",*//*"读取数据"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Out Ferq",*//*"输出频率"*/ /*  0 ,*/(u8*)Q ,   6 , /* 15 ,   0 , */UPD },
             {/* "Out Duty",*//*"输出占比"*/ /*  0 ,*/(u8*)P ,   5 , /*  9 ,   1 , */  0 },
//             { "AutoTips",/*"自动提示"*/   0 ,     0 ,   1 ,   1 ,   0 ,   0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Cal Zero",*//*"零点校正"*/ /* 80 , */    0 ,   0 ,/* 199 ,-199 , */  UPD },
             {/* "CalRange",*//*"增益校正"*/ /* 80 , */    0 ,1024 ,/*2048 ,   0 , */  0 },
             {/* "CalRange",*//*"恢复校准值"*/ /* 80 , */    0 ,99 ,/*2048 ,   0 , */  0 },   //恢复出厂校准值
             {/* "Cal Auto",*//*"零点校正"*/ /* 0 , */     0 ,   0 ,/* 9 ,  0 , */  UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Battery ",*//*"电池状态"*//* 295 ,  */   0 ,   1 , /*  4 ,   0 , */UPD }};
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
// 项目定义  |    ESTR     |    CSTR        | S_X    |  PTR  | Val | MAX   | MIN   | Flg |
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
item_c List_c[]={
             {/* "Y Ranges",*//*"量程选择"*/ /* 56 ,*/(u8*)R ,   6 , /*  9 ,   0 ,*/ UPD },
             {/* "ProbeAtt",*//*"探头模式"*/ /* 90 ,*/(u8*)N ,   0 , /*  1 ,   0 ,*/ UPD },
             {/* "FitRange",*//*"自动量程"*/ /* 44 ,*/(u8*)A ,   0 , /*  1 ,   0 ,*/ UPD },
             {/* "Y Offset",*//*"垂直偏移"*/ /*120 ,*/     0 ,  50 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV1",*//*"高位游标"*/ /*  0 ,*/(u8*)V , 150 , /*198 ,   2 ,*/ UPD },
             {/* "CursorV2",*//*"低位游标"*/ /*  0 ,*/(u8*)V ,  25 , /*198 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"游标型式"*/ /*  0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "TimeBase",*//*"时基选择"*/ /*132 ,*/(u8*)B ,   7 ,/*  19 ,   0 ,*/ UPD },//
             {/* "Fit Base",*//*"自动时基"*/ /*120 ,*/(u8*)A ,   0 ,/*   1 ,   0 ,*/ UPD },
             {/* "ViewPosi",*//*"水平位移"*/ /*100 ,*/     0 ,   1 ,/*3795 ,   1 ,*/ UPD },
             {/* "SmplDpth",*//*"采样深度"*/ /*232 ,*/(u8*)D ,   2 ,/*   3 ,   0 ,*/   0 },
             {/* "CursorT1",*//*"左端游标"*/ /*  0 ,*/(u8*)T ,  48 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorT2",*//*"右端游标"*/ /*  0 ,*/(u8*)T , 210 ,/* 298 ,   2 ,*/ UPD },
             {/* "CursorTp",*//*"游标型式"*/ /*  0 ,*/     0 ,   0 ,/*   1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"SyncMode"*//*"同步模式"*//*   0 ,*/(u8*)M ,   2 , /*  4 ,   1 ,*/ UPD },
             { /*"TrigMode"*//*"触发极性"*//* 228 ,*/(u8*)E ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Auto Fit"*//*"自动阈值"*//* 218 ,*/(u8*)A ,   0 , /*  1 ,   0 ,*/ UPD },
             { /*"Threshol"*//*"触发阈值"*//*   0 ,*/     0 ,  40 , /* 98 ,  -98,*/ UPD },      //198
             { /*"Sensitiv"*//*"触发回差"*//* 238 ,*/     0 ,   5 , /* 98 ,   1 ,*/ UPD },
             { /*"CursorTp"*//*"游标型式"*//*   0 ,*/     0 ,   0 , /*  1 ,   0 ,*/ UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*  " Freq ",*//*  "频率值"*//* 232 ,*/(u8*)F ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Duty ",*//*  "占空比"*//* 232 ,*/(u8*)P ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vrms ",*//*  "有效值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vavg ",*//*  "平均值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vp-p ",*//*  "峰峰值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmax ",*//*  "最大值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
             { /*  " Vmin ",*//*  "最小值"*//* 232 ,*/(u8*)V ,   0 ,/* 100 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Ext Refn",*//*"参考模式"*/ /*172 ,*/(uc8*)I,   0 , /*  5 ,   0 , */UPD },
             { /*"Ext Posi",*//*"垂直偏移"*/ /*  0 ,*/     0 , 125 , /*199 ,   1 , */UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             { /*"Save Bmp",*//*"保存图像"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Dat",*//*"保存波形"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Buf",*//*"保存数据"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Save Csv",*//*"保存资料"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Dat",*//*"读取波形"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
             { /*"Load Buf",*//*"读取数据"*//* 232 ,*/(u8*)C ,   0 , /*255 ,   0 , */  0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Out Ferq",*//*"输出频率"*/ /*  0 ,*/(u8*)Q ,   6 , /* 15 ,   0 , */UPD },
             {/* "Out Duty",*//*"输出占比"*/ /*  0 ,*/(u8*)P ,   5 , /*  9 ,   1 , */  0 },
//             { "AutoTips",/*"自动提示"*/   0 ,     0 ,   1 ,   1 ,   0 ,   0 },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Cal Zero",*//*"零点校正"*/ /* 80 , */    0 ,   0 ,/* 199 ,-199 , */  UPD },
             {/* "CalRange",*//*"增益校正"*/ /* 80 , */    0 ,1024 ,/*2048 ,   0 , */  0 },
             {/* "CalRange",*//*"恢复校准值"*/ /* 80 , */    0 ,99 ,/*2048 ,   0 , */  0 },   //恢复出厂校准值
             {/* "Cal Auto",*//*"零点校正"*/ /* 0 , */     0 ,   0 ,/* 9 ,  0 , */  UPD },
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+
             {/* "Battery ",*//*"电池状态"*//* 295 ,  */   0 ,   1 , /*  4 ,   0 , */UPD }};
//-----------+-------------+----------------+--------+-------+-----+-------+-------+-----+

//----------------+------------------------+-----------------+
// 项目提示字符串 |           ESTR         |       CSTR      |
//----------------+------------------------+-----------------+
uc8 YnTips[][24]={"to Set the Input Range ",/*"键设置输入量程"*/
                  "Select Input Attenuator",/*"键设置探头模式"*/
                  "to Set the Auto Range  ",/*"键设置自动模式"*/
                  "to Set the Input Rosi  ",/*"键设置垂直偏移"*/
                  "to Set the V1 Cursor   ",/*"键设置高位游标"*/
                  "to Set the V2 Cursor   ",/*"键设置低位游标"*/
                  "to Set the Cursor Type ",/*"键选择游标型式"*/};
//----------------+------------------------+-----------------+
uc8 XnTips[][24]={"to Set the Time Base   ",/*"键设置扫描时基"*/
                  "to Set the Auto Base   ",/*"键设置自动模式"*/
//                "IJJJJJJJJJJJJJJJJJJJJJK",/*"键设置视窗位置"*/
                  "to Set the View Posi   ",/*"键设置视窗位置"*/
                  "Select Sampling Depth  ",/*"键选择采样深度"*/
                  "to Set the T1 Cursor   ",/*"键设置左端游标"*/
                  "to Set the T2 Cursor   ",/*"键设置右端游标"*/
                  "to Set the Cursor Type ",/*"键选择游标型式"*/};
//----------------+------------------------+-----------------+
uc8 TrTips[][24]={"to Set the Sync Mode   ",/*"键设置同步方式"*/
                  "to Set the Trigg Mode  ",/*"键设置触发方式"*/
                  "to Set the Auto Level  ",/*"键设置自动阈值"*/
                  "to Set the Trigg Level ",/*"键设置触发阈值"*/
                  "to Set the Sensitivity ",/*"键设触发灵敏度"*/
                  "to Set the Cursor Type ",/*"键选择游标型式"*/};
//----------------+------------------------+-----------------+
uc8 RnTips[][24]={"to Set the Ref. Kind"   ,/*"键设置参考类型"*/
                  "to Set the Ref. Offset ",/*"键设置参考偏移"*/};
//----------------+------------------------+-----------------+
uc8 FnTips[][24]={" Select to Save ",       /*"键选择存文件号"*/
                  " Select to Load ",       /*"键选择读文件号"*/};
//----------------+------------------------+-----------------+
uc8 SnTips[][24]={"to Set the Output Frqn ",/*"键选择输出频率"*/
                  "to Set the Output Fdut ",/*"键选择输出占比"*/
//                  {"to Set the Buzzer Vol  ","键设置声音音量"},
//                  {"to Set the Lcd Bright  ","键设置背光亮度"},
//                  {"to Set the Idle Timer  ","键设置休眠定时"},
                  "to Set the Auto Tips   ",/*"键选择自动提示"*/};
//                  {"to Set the Popup Type  ","键选择界面风格"},
//                  {"to Select the Language ","键选择菜单语言"}
//----------------+------------------------+-----------------+
uc8 SnNote[][10]= {"No Tips ",              /*"关闭提示"       */
                   "Tips En ",              /*"自动提示"       */};
//                  {"No Trans",              "不透明窗"       },
//                  {"Translu ",              "半透明窗"       },
//                  {"English ",              "英文菜单"       },
//                  {"Chinese ",              "中文菜单"       }
//----------------+------------------------+-----------------+
uc8 FnNote[][14]= {"File R/W Ok!",          /*"文件操作完成"   */
                   "File Ver Err",          /*"文件版本错误"   */
                   "Not Found!  ",          /*"文件名不存在"   */
                   "File R/W Err",          /*"文件操作错误"   */
                   "Disk Busy!  ",          /*"磁盘读写错误"   */};
//----------------+------------------------+-----------------+


//uc8  MeterStr[7][6]={"Frqn","Duty","Vrms","Vavg","Vp_p","Vmax","Vmin"};
uc16 DEPTH[]={256 , 512 , 1024 , 2048 , 4096 , 6144};//8192

//              0-us                       9--ms                     18--1s
uc32 Ts[] = {1,2,5,10,20,50,100,200,500,1,2,5,10,20,50,100,200,500,1,2};

u8   Current = 0, Status = 0, NumStr[11];
u16  Presampling = X_SIZE/2;//IdleCnt = 300, 
extern u16 Edge;

//------------------------------------------------------------------------------------
/*******************************************************************************
 Update_Item: 根据Update[x]和Erase[x]的标志，刷新显示各项目栏内容
*******************************************************************************/
void Update_Titel(void)
{
  u16 i;

  for(i=0; i<8; i++){
    SetColor(GRY, TCOLOR[i]);
    DispStr(T_X, TITEL_Y[i], (Current == i)? INV : PRN, (u8*)TSTR[i]);
  }
}
/*******************************************************************************
 Show_Note: 设置菜单窗位置、尺寸、颜色及显示内容
*******************************************************************************/
void Show_List(void)
{
  u16 i, Ny0, Nx0, CurrentMax = MU_BEGN[Current]+ M_ITEMS[Current];
  u8  Flag = 0;;
  u8  str[9];
  ParamTab[BXx1]= BLOCK_X[Current];
  ParamTab[BYx2]= BLOCK_Y[Current]*2;
  ParamTab[WXx1]= BLOCK_W[Current];
  ParamTab[HYx2]=(M_ITEMS[Current]+1)*32;
  PopColor(DAR, TCOLOR[Current]);
  Buil_Pop();
  for(i=MU_BEGN[Current]; i<CurrentMax; i++){
    if(i == MenuNow[Current]) Flag = INV;
    else                      Flag = 0;
    if(((List[FITRANGE].Val)&&(i == Y_RANGES))||
    ((List[FIT_BASE].Val)&&(i == TIM_BASE))||
    ((List[FITTRIGG].Val)&&(i == THRESHOLD))) Flag = VOID;
    Ny0 = ((CurrentMax - i)*16 - 8);
    Nx0 = N_X + 3;
    memset(str,0,9);
    memcpy(str,&ESTR[i],8);
    NoteSTR(Nx0, Ny0, Flag, str);
  }
  List_Pop();
}
/*******************************************************************************
 Update_Blink: 显示闪烁处理
*******************************************************************************/
void Update_Blink(void)
{
  u8* ptr;
  //s32 Tmp ; V5.02
  s_64 Tmp;
  u16 i, Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;

  i = MenuNow[Current];
  ptr = (u8*)(List[i].PTR +(List[i].Val)*6);
  if(Blink == BLINK){
    Blink = 0;
    if((i != SMPL_DPTH) && (Current != FN)) List[MenuNow[ME]].Flg |= UPD;// 指定的波形测量项更新显示
    switch(i){
    case SAVE_BMP:
    case SAVE_DAT:
    case SAVE_BUF:
    case SAVE_CSV:
    case LOAD_DAT:
    case LOAD_BUF:
      SetColor(GRY, TCOLOR[FN]);
      u8ToDec3(NumStr, List[i].Val);
      DispStr(S_X[i]+32, 2, Twink, NumStr);
      break;
    case Y_RANGES:
      SetColor(GRY, TCOLOR[YN]);
      DispStr(S_X[i], 224, Twink,      ptr);
      break;
    case PROBEATT:
      SetColor(GRY, TCOLOR[YN]);
      DispStr(S_X[i],   224, Twink+SYMB, "X");
      DispStr(S_X[i]+8, 224, Twink,      ptr);
      break;
    case FITRANGE:
      SetColor(GRY, TCOLOR[YN]);
      DispStr(S_X[i], 224, Twink+SYMB, ptr);  
      break;
    case V0_POSI:
      if(Twink) ParamTab[P1F] |=  D_HID;
      else      ParamTab[P1F] &= ~D_HID;
      break;
    case V1_POSI:
    case V2_POSI:
      Tmp = (i == V1_POSI)? V1F : V2F;
      if(Twink) ParamTab[Tmp] |=  D_HID;
      else      ParamTab[Tmp] &= ~D_HID;
      Tmp  =(ParamTab[V1x2]- ParamTab[V2x2])/2;         // V2 - V1 数值
      Tmp *= B_SCALE[Range]* Exp(V_SCALE[Range]);
      Value2Str(NumStr, Tmp,(u8*)V, 3, UNSIGN);
      SetColor(GRY, TCOLOR[YN]);
      DispStr(0*8, 2, Twink+SYMB, "D");
      DispStr(1*8, 2, Twink,      ":");
      DispStr(2*8, 2, Twink,      NumStr);
      break;
    case YN_TYPE:
      if(Twink) ParamTab[V1F] |=  D_HID;
      else      ParamTab[V1F] &= ~D_HID;
      if(Twink) ParamTab[V2F] |=  D_HID;
      else      ParamTab[V2F] &= ~D_HID;
      break;
    case TIM_BASE:
      SetColor(GRY, TCOLOR[XN]);
      DispStr(S_X[i], 224, Twink, ptr);
      break;
    case FIT_BASE:
      SetColor(GRY, TCOLOR[XN]);
      DispStr(S_X[i], 224, Twink+SYMB, ptr);
      break;
    case VIEWPOSI:
      if(Twink) ParamTab[T0F] |=  D_HID;
      else      ParamTab[T0F] &= ~D_HID;
      break;
    case SMPL_DPTH:
      SetColor(GRY, TCOLOR[XN]);
      DispStr(304, 2, Twink,(u8*)&D[List[SMPL_DPTH].Val]);
      break;
    case T1_POSI:
    case T2_POSI:
      Tmp = (i == T1_POSI)? T1F : T2F;
      if(Twink) ParamTab[Tmp] |=  D_HID;
      else      ParamTab[Tmp] &= ~D_HID;
      Tmp  = ParamTab[T2x1] - ParamTab[T1x1];          // T2 - T1 数值
      Tmp *= B_SCALE[List[TIM_BASE].Val]* Exp(T_SCALE[List[TIM_BASE].Val]);
      if(Tmp>=0)
        Value2Str(NumStr, Tmp, (u8*)T, 3, UNSIGN);
      else
        Value2Str(NumStr, Tmp, (u8*)T, 3, SIGN);
      SetColor(GRY, TCOLOR[XN]);
      DispStr(0*8, 2, Twink+SYMB, "D");
      DispStr(1*8, 2, Twink+PRN,  ":");
      DispStr(2*8, 2, Twink+PRN,  NumStr);
      break;
    case XN_TYPE:
      if(Twink) ParamTab[T1F] |=  D_HID;
      else      ParamTab[T1F] &= ~D_HID;
      if(Twink) ParamTab[T2F] |=  D_HID;
      else      ParamTab[T2F] &= ~D_HID;
      break;
    case SYNCMODE:
      if(!(Status &  STOP)){
      SetColor(GRY, TCOLOR[TR]);
      DispStr(S_X[i], 224, Twink, ptr);
      }
      break;
    case TR_MODE:
    case FITTRIGG:
      SetColor(GRY, TCOLOR[TR]);
      DispStr(S_X[i], 224, Twink+SYMB, ptr);
      break;
    case THRESHOLD:
      if(Twink) ParamTab[VTF] |=  D_HID;
      else      ParamTab[VTF] &= ~D_HID;
      break;
    case SENSITIVE:
      Tmp  = List[i].Val;                        // 触发回差值
      Tmp *= B_SCALE[Range]* Exp(V_SCALE[Range]);
      Value2Str(NumStr, Tmp, (u8*)V, 2, UNSIGN);
      SetColor(GRY, TCOLOR[TR]);
      DispStr(S_X[i],   224, Twink+SYMB, "^");
      DispStr(S_X[i]+8, 224, Twink     ,  NumStr);
      break;
    case TR_TYPE:
      if(Twink) ParamTab[VTF] |=  D_HID;
      else      ParamTab[VTF] &= ~D_HID;
      break;
    case EXT_REFN:
      SetColor(GRY, TCOLOR[EX]);
      DispStr(S_X[i], 224, Twink, ptr);
      break;
    case EXT_POSI:
      if(Twink) ParamTab[P2F] |=  D_HID;
      else      ParamTab[P2F] &= ~D_HID;
      break;
    case FREQ_OUT:
      SetColor(GRY, TCOLOR[SN]);
      DispStr(S_X[i]+24, 2, Twink, (u8*)Q[List[i].Val]);
      DispStr(S_X[i],    2, Twink, "FO:");       
      break;
    case FREQ_DUT:
      SetColor(GRY, TCOLOR[SN]);
      Value2Str(NumStr, List[i].Val*10, List[i].PTR, 2, UNSIGN);
      DispStr(S_X[i]+40, 2, Twink, NumStr);
      DispStr(S_X[i],    2, Twink, "Duty:");        
      break;
    case CAL_ZERO:
      if(Twink) ParamTab[P1F] |=  D_HID;
      else      ParamTab[P1F] &= ~D_HID;
      SetColor(GRY, TCOLOR[CA]);
      Value2Str(NumStr, Vavg, "", 2, SIGN);
      DispStr(16, 2, PRN,        "      ");
      DispStr( 0, 2, Twink+PRN,  "PO");
      DispStr(16, 2, Twink+PRN,  NumStr);
      break;
    case CAL_GAIN:
      if(Twink) ParamTab[P1F] |=  W_HID;
      else      ParamTab[P1F] &= ~W_HID;
      SetColor(GRY, TCOLOR[CA]);
      Tmp = Vavg*B_SCALE[Range]* Exp(V_SCALE[Range]);
      Value2Str(NumStr, Tmp, (u8*)V, 2, SIGN);
      DispStr(16, 2, PRN,        "      ");
      DispStr( 0, 2, Twink+PRN,  "Vg");
      DispStr(16, 2, Twink+PRN,  NumStr);
      break;
    case CAL_AUTO:
      if(Twink) ParamTab[P1F] |=  D_HID;
      else      ParamTab[P1F] &= ~D_HID;
      SetColor(GRY, TCOLOR[CA]);
      Value2Str(NumStr, Vavg, "", 2, SIGN);
      DispStr(16, 2, PRN,        "      ");
      DispStr( 0, 2, Twink+PRN,  "PO");
      DispStr(16, 2, Twink+PRN,  NumStr);
      SetColor(GRY, TCOLOR[YN]);
      ptr = (u8*)(List[0].PTR +(List[0].Val)*6);
      DispStr(S_X[0], 224, Twink,      ptr);
      break;
    }
    if(i != CAL_GAIN) ParamTab[P1F] &= ~W_HID;
    if(i != V0_POSI) ParamTab[P1F]   = L_HID ;//0513
  }
}
/*******************************************************************************
 Update_Detail: 各个项目的显示刷新处理
*******************************************************************************/
void Update_Detail(void)
{
  u16 i, j, k = HighT+LowT;
//s32 Tmp = 0; V5.02
  s_64 Tmp = 0;
  u8  *ptr, Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;
  u64 FTmp;

  for(i=Y_RANGES; i<=BATTERY; i++){
    if(List[i].Flg & UPD){
      List[i].Flg &= ~UPD;
      ptr = (u8*)(List[i].PTR +(List[i].Val)*6);
      switch (i){
      case CAL_ZERO:   // 刷新显示零点校正
        ParamTab[P1F] &= ~D_HID;
        SetColor(GRY, TCOLOR[CA]);
        Value2Str(NumStr, Vavg, "", 2, SIGN);
        DispStr(16, 2, PRN,  "      ");
        DispStr( 0, 2, PRN,  "PO");
        DispStr(16, 2, PRN,  NumStr);
        break;
      case CAL_GAIN:   // 刷新显示增益校正
        ParamTab[P1F] &= ~W_HID;
        SetColor(GRY, TCOLOR[CA]);
        Tmp = Vavg*B_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp, (u8*)V, 2, SIGN);
        DispStr(16, 2, PRN,  "      ");
        DispStr( 0, 2, PRN,  "Vg");
        DispStr(16, 2, PRN,  NumStr);
        break;
      case CAL_AUTO:   // 刷新显示零点校正
        ParamTab[P1F] &= ~D_HID;
        SetColor(GRY, TCOLOR[CA]);
        Value2Str(NumStr, Vavg, "", 2, SIGN);
        DispStr(16, 2, PRN,  "      ");
        DispStr( 0, 2, PRN,  "PO");
        DispStr(16, 2, PRN,  NumStr);
        break;

      case Y_RANGES:   // 刷新显示 Y 量程档位
      case PROBEATT:   // 刷新显示探头衰减比率
      case FITRANGE:   // 刷新显示自动量程状态
      case V0_POSI:    // 刷新显示垂直偏移位置
      case V1_POSI:    // 刷新显示高位游标位置
      case V2_POSI:    // 刷新显示低位游标位置
      case YN_TYPE:    // 刷新显示游标型式
        SetColor(GRY, TCOLOR[YN]);
        if(i == FITRANGE)DispStr(S_X[i], 224, SYMB, ptr);
        if(i == Y_RANGES){
          DispStr(S_X[i]+24, 224, PRN,  " ");                   
          DispStr(S_X[i],    224, PRN,  ptr);                   
        }
        if(i == PROBEATT){
          DispStr(S_X[i],    224, SYMB, "X");              
          DispStr(S_X[i]+16, 224, PRN,  " ");     
          DispStr(S_X[i]+8,  224, PRN,  ptr);
        }
        ParamTab[P1x2] = 2*((List[V0_POSI].Val+1));
        ParamTab[VTx2] = ParamTab[P1x2]+ 2*(List[THRESHOLD].Val+1);
        ParamTab[V1x2] = 2*(List[V1_POSI].Val+1);
        ParamTab[V2x2] = 2*(List[V2_POSI].Val+1);
        ParamTab[V1F]  = (List[YN_TYPE].Val)? L_HID : SHOW;
        ParamTab[V2F]  = (List[YN_TYPE].Val)? L_HID : SHOW;

        Tmp  =(ParamTab[V1x2]- ParamTab[V2x2])/2;         // 刷新显示V2-V1数值
        Tmp *= B_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp,(u8*)V, 3, UNSIGN);
        DispStr(0*8, 2, SYMB, "D");
        DispStr(1*8, 2, PRN,  ":");
        DispStr(6*8, 2, PRN,  "  ");
        DispStr(2*8, 2, PRN,  NumStr);

        SetColor(GRY, TCOLOR[TR]);
        Tmp  = List[SENSITIVE].Val;                       // 刷新显示触发回差值
        Tmp *= B_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp,(u8*)V, 2, UNSIGN);
        DispStr(S_X[SENSITIVE]+8, 224, PRN,  "      ");     
        DispStr(S_X[SENSITIVE]+8, 224, PRN,  NumStr);     
        DispStr(S_X[SENSITIVE],   224, SYMB, "^");        
        Set_Y_Pos(Range, List[V0_POSI].Val);              // 设置垂直偏移
        Set_Range(Range);                                 // 设置Y量程档位
        break;

       case BATTERY://显示电池状态
        SetColor(GRY, WHT);
        if(USB_On()) DispStr(295, 224, SYMB, "[\\]");     // 显示 USB 供电标志
        else {
          Tmp = Bat_Vol();                                // 电池电压(mV)
          if(Tmp > 3900)     DispStr(295, 224, SYMB, "EFG");
          else if(Tmp > 3500)DispStr(295, 224, SYMB, "AFG");
          else if(Tmp > 3200)DispStr(295, 224, SYMB, "ABG");      //3600
          else               DispStr(295, 224, SYMB, "ABC");
        }
        break;

      case SAVE_BMP: //显示与选择要保存的文件编号
      case SAVE_DAT:
      case SAVE_BUF:
      case SAVE_CSV:
      case LOAD_DAT: //显示与选择要读取的文件编号
      case LOAD_BUF:
        SetColor(GRY, TCOLOR[FN]);
        u8ToDec3(NumStr, List[i].Val);
        DispStr(S_X[i],    2, PRN, (u8*)((i > SAVE_CSV)? "Load " : "Save "));//0513
        DispStr(S_X[i]+32, 2, PRN, NumStr);
        DispStr(S_X[i]+56, 2, PRN, (u8*)(List[i].PTR +(i - SAVE_BMP)*6));
        break;

      case FREQ_OUT: // 显示输出脉冲频率
        SetColor(GRY, TCOLOR[SN]);
        Set_Fout(List[i].Val);
        DispStr(S_X[i]+24, 2, PRN, "     ");              
        DispStr(S_X[i]+24, 2, PRN, (u8*)Q[List[i].Val]);
        DispStr(S_X[i],    2, PRN, "FO:");
        break;
       
      case FREQ_DUT: // 显示输出脉冲占空比
        SetColor(GRY, TCOLOR[SN]);
        Set_Duty();
        u8ToDec2(NumStr, List[i].Val*10);
        DispStr(S_X[i]+40, 2, PRN, NumStr);               
        DispStr(S_X[i]+56, 2, PRN, (u8*)List[i].PTR);
        DispStr(S_X[i],    2, PRN, "Duty:");       
        break;

      case EXT_REFN: // 显示参考波形类型
      case EXT_POSI: // 显示参考波形偏移
        SetColor(GRY, TCOLOR[EX]);
        if(i == EXT_REFN){
          DispStr(S_X[i]+32, 224, PRN, " ");      
          DispStr(S_X[i],    224, PRN, ptr);
        }
        if(List[EXT_REFN].Val == 6) ParamTab[P2F] |=  W_HID;  // 消隐参考波形
        else                        ParamTab[P2F] &= ~W_HID;  // 显示参考波形
        if(List[EXT_REFN].Val < 4) ParamTab[P2x2]=(List[EXT_POSI].Val + 1)*2;
        else                       ParamTab[P2x2]=(List[ V0_POSI].Val + 1)*2;
        if(List[EXT_REFN].Val < 2){
          for(j=2; j<300-2; j++){
            if(List[EXT_REFN].Val)
              Tmp = List[EXT_POSI].Val + Data[1]- Data[j]; // -Data
            else
              Tmp = List[EXT_POSI].Val - Data[1]+ Data[j]; // Data
            if(Tmp > 202) Tmp = 202; // 上限幅
            if(Tmp <   1) Tmp = 1;   // 下限幅
            WaveBuf[TRACK1_SIZE+j] = Tmp;
          }
        }
        break;

      case SYNCMODE:  // 刷新显示同步模式
      case TR_MODE:   // 刷新显示触发方式
      case FITTRIGG:  // 刷新显示自动触发阈值
      case THRESHOLD: // 刷新显示触发阈值
      case SENSITIVE: // 刷新显示触发灵敏度
      case TR_TYPE:   // 刷新显示游标型式
        SetColor(GRY, TCOLOR[TR]);
        DispStatus();
        if(i == TR_MODE )DispStr(S_X[i], 224, SYMB, ptr);
        if(i == FITTRIGG)DispStr(S_X[i], 224, SYMB, ptr);
        ParamTab[VTx2]= ParamTab[P1x2]+2*List[THRESHOLD].Val;
        Tmp  = List[SENSITIVE].Val;
        Tmp *= B_SCALE[Range]* Exp(V_SCALE[Range]);
        Value2Str(NumStr, Tmp, (u8*)V, 2, UNSIGN);
        DispStr(S_X[SENSITIVE]+8*5, 224, PRN,  " ");        
        DispStr(S_X[SENSITIVE],   224, SYMB, "^");
        DispStr(S_X[SENSITIVE]+8, 224, PRN,  NumStr);
        ParamTab[VTF]=(List[TR_TYPE].Val)? L_HID : SHOW;
        break;

      case TIM_BASE:  // 刷新显示时基档位
      case FIT_BASE:  // 刷新显示自动时基状态
      case VIEWPOSI:  // 刷新显示水平视窗位置
      case SMPL_DPTH: // 刷新显示采样深度
      case T1_POSI:   // 刷新显示左端游标位置
      case T2_POSI:   // 刷新显示右端游标位置
      case XN_TYPE:   // 刷新显示游标型式
        SetColor(GRY, TCOLOR[XN]);
//        DispStr(S_X[TIM_BASE]+24, 224, PRN,  " ");                       
        if(i == TIM_BASE)DispStr(S_X[i],    224, PRN,  ptr);      
        if(i == FIT_BASE)DispStr(S_X[i],    224, SYMB, ptr);      
        if(i == SMPL_DPTH){
          DispStr(232, 2,  PRN, "SmplDpth:");
          DispStr(304, 2,  PRN, (u8*)&D[List[SMPL_DPTH].Val]);
        }
        ShowView(List[VIEWPOSI].Val, List[SMPL_DPTH].Val);
        ParamTab[T0x1] = 2 + Presampling - List[VIEWPOSI].Val;
        ParamTab[T0F] &= ~D_HID;
        ParamTab[T1F] &= ~D_HID;
        ParamTab[T1F] &= ~D_HID;
        ParamTab[T1x1] = List[T1_POSI].Val+1;
        ParamTab[T2x1] = List[T2_POSI].Val+1;
        ParamTab[T1F]  = (List[XN_TYPE].Val)? L_HID : SHOW;
        ParamTab[T2F]  = (List[XN_TYPE].Val)? L_HID : SHOW;
        Tmp  = ParamTab[T2x1] - ParamTab[T1x1];
        Tmp *= B_SCALE[List[TIM_BASE].Val]* Exp(T_SCALE[List[TIM_BASE].Val]);
        Value2Str(NumStr, Tmp, (u8*)T, 3, UNSIGN);
        DispStr(0*8, 2, SYMB, "D");
        DispStr(1*8, 2, PRN,  ":");
        DispStr(7*8, 2, PRN,  " ");
        DispStr(2*8, 2, PRN,  NumStr);
        Set_Base(List[TIM_BASE].Val);          // 硬件设置扫描时基档位
      break;
      case FREQUENC://显示频率
        FTmp=Edge*25*1000/2;
        if(List[TIM_BASE].Val<9)FTmp= FTmp*1000000;
        else if(List[TIM_BASE].Val<18)FTmp= FTmp*1000;
        FTmp=FTmp/Ts[List[TIM_BASE].Val]/DEPTH[List[SMPL_DPTH].Val];
        Tmp=FTmp;
        if(FTmp>110*1000000)
          DispMeter(i, 0, UNSIGN,Tmp);
        else
          DispMeter(i, k, UNSIGN,Tmp);
      break;        
      case DUTY:    //显示占空比
         DispMeter(i, k, UNSIGN,(HighT*100)/k);
        break;
      case VRMS://显示电压有效值
        DispMeter(i, 1, UNSIGN, Vrms*B_SCALE[Range]*Exp(V_SCALE[Range]));
        break;
      case VP_P://显示电压峰峰值
        DispMeter(i, 1, UNSIGN,  Vpp*B_SCALE[Range]*Exp(V_SCALE[Range]));
        break;
      case VAVG://显示电压平均值
        DispMeter(i, 1, SIGN,  Vavg*B_SCALE[Range]*Exp(V_SCALE[Range]));
        break;
      case VMAX://显示电压最大值
        DispMeter(i, 1, SIGN,  Vmax*B_SCALE[Range]*Exp(V_SCALE[Range]));
        break;
      case VMIN://显示电压最小值
        DispMeter(i, 1, SIGN,  Vmin*B_SCALE[Range]*Exp(V_SCALE[Range]));
        break;
      }
    }
  }
}
/*******************************************************************************

*******************************************************************************/
void DispMeter(u8 Item, u8 St, u8 Mode, s32 Val)
{
  ClrMeter();
  SetColor(GRY, TCOLOR[ME]);
  DispStr(S_X[Item], 2, PRN, (u8*)&Z[Item-FREQUENC]);// 显示测量项目名称  
  Value2Str(NumStr, Val, List[Item].PTR, 3, Mode);
  if(St) DispStr(S_X[Item]+32, 2, PRN, NumStr);   // 显示测量数值+量纲  
  else   DispStr(S_X[Item]+32, 2, PRN, ": ---- ");   
}
/*******************************************************************************

*******************************************************************************/
void DispStatus(void)
{
  u8* p;
  SetColor(GRY, (Status & STOP) ? RED : TCOLOR[TR]);
  p =(Status & STOP)?(u8*)M :(u8*)(List[SYNCMODE].PTR +(List[SYNCMODE].Val)*6);
  DispStr(S_X[SYNCMODE], 224, PRN, "     ");   
  DispStr(S_X[SYNCMODE], 224, PRN, p);         
}
/*******************************************************************************

*******************************************************************************/
void ClrMeter(void)
{
  DispStr(232, 2, PRN, "           ");
}
/*******************************************************************************

*******************************************************************************/
void ShowView(u16 Posi, u8 Dpth)
{
  u16 i, x0, Width, Depth = DEPTH[List[SMPL_DPTH].Val];

  DispStr( 68, 2, SYMB, "IJJJJJJJJJJJJJJJJJJK"); // 宽度共156点
  Width = (156*300)/Depth;
  x0 = 71 + (156*List[VIEWPOSI].Val)/Depth;
  __Set_Posi(x0,         4); for (i=0; i<11; i++) __Set_Pixel(WHT);
  __Set_Posi(Width+x0-1, 4); for (i=0; i<11; i++) __Set_Pixel(WHT);
  for (i=x0+1; i<(Width+x0-1); i++){
    __Set_Posi(i,  3); __Set_Pixel(WHT);
    __Set_Posi(i, 15); __Set_Pixel(WHT);
  }
}

/*******************************************************************************

*******************************************************************************/
void DispFileInfo(u8 Info)
{ 
  DispStr(232, 2, PRN, "             ");
  if(Info!=0){
    DispStr(232, 2, PRN, ((u8*)&FnNote+14*Info));
    Delayms(2000);
    DispStr(232, 2, PRN, "             ");
  }
}
/******************************** END OF FILE *********************************/
