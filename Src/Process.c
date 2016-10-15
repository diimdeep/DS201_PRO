/******************** (C) COPYRIGHT 2012 e-Design Co.,Ltd. *********************
 File Name : Process.c
 Version   : DS201 Ver 3.0                                       Author : bure
*******************************************************************************/

#include "HDW_Config.h"
#include "Process.h"
#include "string.h"
#include "Menu.h"
#include "BIOS.h"
#include "Draw.h"
#include "Func.h"
#include "Display.h"

u16 Sampl[0x1800+2];              //采样缓冲区 Sampl[0]保存采样时基档位值
s16 Vmax, Vmin, Vmid, Vavg, Start;
u16 Vrms, Vpp,  LowT, HighT, Edge;

/*******************************************************************************
 WaveProcess: 计算处理采样缓冲区数据
//5.04  Vp定义为s16，Depth的值-1，Max的值-1,Sum，Ssq定义为s_64
*******************************************************************************/

void WaveProcess(void)
{
  s_64  Sum = 0, Ssq = 0;
  u16* pBuf = &Sampl[2];
  s16  k, m, n, Vi, Ve, Vo, Vtl, Vth, Vp, p = 0, Max = -4095, Min = 4095;
  u16  i, j, s, Div, Depth, Xp, StartU = 0, StartD = 0;
  u16  Dt = 0, Tsl = 0, Tsh = 0, FallCnt = 0, RiseCnt = 0;
  u8   Yp, Base, Range, Sm, State = 2, Tm, Ext = List[EXT_REFN].Val;
  u8   ret=0;

/* ----------------- 数据预处理,生成近程指针提高数据调用效率 -----------------*/

  Vtl   = List[THRESHOLD].Val + List[V0_POSI].Val - List[SENSITIVE].Val;
  Vth   = List[THRESHOLD].Val + List[V0_POSI].Val + List[SENSITIVE].Val;
  Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;
  Base  = List[TIM_BASE].Val;
  Sm    = List[SYNCMODE].Val;
  Tm    = List[TR_MODE].Val;
  //Depth = DEPTH[List[SMPL_DPTH].Val];
  Depth = DEPTH[List[SMPL_DPTH].Val] - 1;
  Xp    = List[VIEWPOSI].Val;
  Yp    = List[V0_POSI].Val;
  s     = Ks[Base];
  n     = Kn[Range];
  m     = Km[Range];
  Div   = Yd[Range];
//  Vp    = 2048 -((Yp  - n)*Div)/m; // Vi = n +(m*(2048 - Vs))/Div;
//  Vtl   = 2048 -((Vtl - n)*Div)/m;
//  Vth   = 2048 -((Vth - n)*Div)/m;
  Vp    = ((Yp  - 100)*Div)/m; // Vi = 100 +(m*(2048 - Vs))/Div;
  Vtl   = ((Vtl - 100)*Div)/m;
  Vth   = ((Vth - 100)*Div)/m;

/* ---------------------------- 采样数据分析 ---------------------------------*/

  for(i=0; i<Depth; i++){
    if(i>=ADC_Cnt(Depth))break;             // 跳空一周期等待A/D转换完成
    Vi = 2048 - pBuf[i];                       // 读取反相后的采样数据
    if(Max < Vi) Max = Vi;                     // 统计最大值与最小值
    if(Min > Vi) Min = Vi;
    Sum += Vi;                                 // 统计累加和
    Ssq += (Vi - Vp)*(Vi - Vp);                // 统计平方和
    if(State != 2) Dt++;                       // 放弃首个跳变沿前的时间累计
    if(Vi > Vth){                              // Vi 高于阈值上限
      if(State == 0){
        Tsl += Dt; Dt = 0; RiseCnt++;          // 低位时间累计，上升沿累计
        if(StartU < PRE_SMPL) StartU = i;      // 记录预采样后第一个上升沿位置
      }
      State = 1;                               // 当前状态设为高位
    } else if(Vi < Vtl){                       // Vi 低于阈值下限
      if(State == 1){
        Tsh += Dt; Dt = 0; FallCnt++;          // 高位时间累计，下降沿累计
        if(StartD < PRE_SMPL) StartD = i;      // 记录预采样后第一个下降沿位置
      }
      State = 0;                               // 当前状态设为低位
    }
  }
  
/* -------------------------- 生成波形显示数据 -------------------------------*/

  Start = ((1024 - s)*PRE_SMPL + 512)/1024 + Xp;    // 插值后视窗起始位置
  if(Sm != NONE){
    if(Tm == 0){
      if(StartD > PRE_SMPL){ Start += StartD -(PRE_SMPL+2);ret=1; }// 下降沿触发时
      else if(Sm != AUTO)   Start  = -1;
    } else {
      if(StartU > PRE_SMPL){ Start += StartU -(PRE_SMPL+2);ret=1; } // 上升沿触发时
      else if(Sm != AUTO)   Start  = -1;
    }
  }
  k = Start;                                     // 触发同步后视窗起始位置
  if(k < 0){
    if(Sm!=NORM) 
    memset(WaveBuf, 255, 300);           // NORM, SINGL 无触发时消隐
  } 
  else for(j=0; j<300; ){
    Vi = 2048 - pBuf[k++];
    if(Vi == 2048) WaveBuf[j++]= 255;            // 无信号则消隐
    else {
      Vi = n + (m*Vi)/Div;                       // 计算当前波形点的基值
      while(p > 0){
        Vo = Ve +((Vi - Ve)*(1024 - p))/1024;    // 计算当前波形点的插值
        if(Vo > 202) Vo = 202;
        if(Vo <   1) Vo = 1;                     // 超界限幅
        WaveBuf[j++]= Vo;
        if(Ext > 1) Build_Ext(j, Ext, Yp);
        if(j >= 300)  break;
        p -= s;
      }
      p += 1024;
      Ve = Vi;                                   // 暂存当前波形点的值
    }
  }
/* ------------------------ 计算采样数据统计结果 -----------------------------*/


    if(i == Depth) {                                     // 采样全部完成
    Vmax  = n +(m * Max)/Div - Yp - 1;
    Vmin  = n +(m * Min)/Div - Yp;
    Vavg  = n +(m*(Sum/Depth))/Div - Yp - 1;
    Vrms  = (m*Sqrt32(Ssq/Depth))/Div;
    Vmid  = (Vmax + Vmin)/2;
    Vpp   = Vmax - Vmin;
    HighT = Tsh/FallCnt;//
    LowT  = Tsl/RiseCnt;//
    Edge  =FallCnt+ RiseCnt;
    if((Sm == SINGL) && (ret==1)){Status = STOP; DispStatus();} // 进入暂停状态
    if((Status & STOP)!=STOP) ADC_Start();                 // 重新开始采样
  }
}
/*******************************************************************************
 Build_Ext: 扩展通道波形显示
*******************************************************************************/
void  Build_Ext(u16 x, u8 Ext, u8 Yp)
{
  u8* Buf = WaveBuf + TRACK1_SIZE;
  u8  Vo;

  switch(Ext){
  case 2: // Inp+D
    Vo = Data[x]- Data[1]+ WaveBuf[x]; break;
  case 3: // D-Inp
    Vo = Data[x]- WaveBuf[x]+ Yp; break;
  case 4: // Inp-D
    Vo = Data[1]- Data[x]+ WaveBuf[x]; break;
  case 5: // -Inp
    Vo = 2*Yp - WaveBuf[x]; break;
  }
  if(Vo > 202) Vo = 202;
  if(Vo <   1) Vo = 1;                     // 超界限幅
  Buf[x]= Vo;
}
/*******************************************************************************
 Calibrate: 垂直偏移校准
*******************************************************************************/
void Calibrate(void)
{
  u16 Range, i, j, Tmp[6];

  SetColor(BLK, WHT);
  DispStr(8*8, 80, 0, "Waiting for Calibration!");
  SetColor(BLK, GRN);
  Tmp[0] = List[SYNCMODE].Val;
  Tmp[1] = List[SMPL_DPTH].Val;
  Tmp[2] = List[V0_POSI].Val;
  Tmp[3] = List[Y_RANGES].Val;
  Tmp[4] = List[PROBEATT].Val;
  Tmp[5] = List[TIM_BASE].Val;
  //Set_Range(20);                            // 设置Y量程档位
  Set_Range(0);                            // 设置Y量程档位
  Set_Base(6);                              // 0.1mS/Div
  List[SYNCMODE].Val = NONE;                // Mode: NONE=1
  List[SMPL_DPTH].Val = MAX[SMPL_DPTH];     // Depth: 1=2048,2=4096,3=6144
  ADC_Start();  
  List[V0_POSI].Val = 10;
  Set_Y_Pos(0, 10);
  Delayms(500);
  for(Range=0; Range<20; Range++){
    DispStr(10*8 + Range*8, 60, 0, ".");
    if(Range < 10) {
      List[Y_RANGES].Val = Range;
      List[PROBEATT].Val = 0;
    } else {
      List[Y_RANGES].Val = Range - 10;
      List[PROBEATT].Val = 1;
    }
    i = 0;
    j = 0;
    while(i++ < 2){
      Set_Y_Pos(Range, 10);
//      while(ADC_Cnt(8192) < 8192){};
      WaveProcess();
//      while(ADC_Cnt(8192) < 8192){};
      WaveProcess();
      Kn[Range] -= Vavg;
      
      Set_Y_Pos(Range, List[V0_POSI].Val);              // 设置垂直偏移
      Delayms(50);
      Set_Range(Range);                                 // 设置Y量程档位
      Delayms(50);
      
      if(Vavg != 0) i = 0;
      if(j++ > 20) break;
    }
  }
  
  Set_Y_Pos(0, 175);
  List[V0_POSI].Val = 175;
  Delayms(500);
  for(Range=0; Range<20; Range++){
    DispStr(10*8 + Range*8, 40, 0, ".");
    if(Range < 10) {
      List[Y_RANGES].Val = Range;
      List[PROBEATT].Val = 0;
    } else {
      List[Y_RANGES].Val = Range - 10;
      List[PROBEATT].Val = 1;
    }
    i = 0;
    j = 0;
    while(i++ < 2){
      Set_Y_Pos(Range, 175);
//      while(ADC_Cnt(8192) < 8192){};
      WaveProcess();
//      while(ADC_Cnt(8192) < 8192){};
      WaveProcess();
      Pg[Range] -= (Vavg*Yd[Range])/1024;
      
      Set_Y_Pos(Range, List[V0_POSI].Val);              // 设置垂直偏移
      Delayms(50);
      Set_Range(Range);                                 // 设置Y量程档位
      Delayms(50);
      
      if(Vavg != 0) i = 0;
      if(j++ > 20) break;
    }
  }
  List[SYNCMODE].Val = Tmp[0];
  List[SMPL_DPTH].Val = Tmp[1];
  List[V0_POSI].Val = Tmp[2];
  List[Y_RANGES].Val = Tmp[3];
  List[PROBEATT].Val = Tmp[4];
  List[TIM_BASE].Val =  Tmp[5];
}
/*******************************************************************************
 Cal_Auto: 自动垂直偏移校准（补差值）
*******************************************************************************/
void Auto_Calibrate(void)
{
  u16 i, j, Range,Tmp[6];
  
  Tmp[0] = List[SYNCMODE].Val;
  Tmp[1] = List[SMPL_DPTH].Val;
  Tmp[2] = List[V0_POSI].Val;
  Tmp[3] = List[Y_RANGES].Val;
  Tmp[4] = List[PROBEATT].Val;
  Tmp[5] = List[TIM_BASE].Val;
  for(i=0;i<2;i++){
    List[Y_RANGES].Val = 0;
    List[PROBEATT].Val = i? 1:0;
    List[PROBEATT].Flg |= UPD;
    List[Y_RANGES].Flg |= UPD;
    List[CAL_AUTO].Flg |= UPD;
    Update_Titel();
    Update_Blink();
    Update_Detail();
    WaveProcess();
    __DrawWindow(WaveBuf, ParamTab); 
    Delayms(300);
    for(j=0;j<10;j++){
      Range = List[Y_RANGES].Val + 10*i;
      List[PROBEATT].Flg |= UPD;
      List[Y_RANGES].Flg |= UPD;
      List[CAL_AUTO].Flg |= UPD;
      Update_Titel();
      Update_Blink();
      Update_Detail();
      WaveProcess();
      __DrawWindow(WaveBuf, ParamTab); 
      Delayms(300);
      
      WaveProcess();
      //if(Kn[Range]<MAX[CAL_ZERO] && Kn[Range] >MIN[CAL_ZERO])
      Kn[Range]-=Vavg;
      Delayms(300);
      
      List[Y_RANGES].Flg |= UPD;
      List[PROBEATT].Flg |= UPD;
      List[CAL_AUTO].Flg |= UPD;
      Update_Titel(); 
      Update_Blink();
      Update_Detail();
      WaveProcess();
      __DrawWindow(WaveBuf, ParamTab);    
      Delayms(300);
      
      if(List[Y_RANGES].Val <MAX[Y_RANGES])
        List[Y_RANGES].Val++;
      List[Y_RANGES].Flg |= UPD;
      List[Y_RANGES].Flg |= UPD;
      List[CAL_AUTO].Flg |= UPD;
      Update_Titel(); 
      Update_Blink(); 
      Update_Detail();
      WaveProcess();
      __DrawWindow(WaveBuf, ParamTab);  
      Delayms(300); 
      PopupCnt =5;
    }
    
    List[SYNCMODE].Val = Tmp[0];
    List[SMPL_DPTH].Val = Tmp[1];
    List[V0_POSI].Val = Tmp[2];
    List[Y_RANGES].Val = Tmp[3];
    List[PROBEATT].Val = Tmp[4];
    List[TIM_BASE].Val =  Tmp[5];
    
    Current = YN;
    List[Y_RANGES].Flg |= UPD;
    List[PROBEATT].Flg |= UPD;
    List[CAL_AUTO].Flg |= UPD;
    Update_Detail();
    Update_Titel(); 
    Update_Blink();
  }
}
/*******************************************************************************
 Auto_Fit:
*******************************************************************************/

void Auto_Fit(void)
{
  s16 Tmp;

  if(List[FITRANGE].Val){
    Tmp = Vmax - Vmin;
    if((Tmp < 40)&&(List[Y_RANGES].Val > 1)){
      List[Y_RANGES].Val--;
      Set_Range(List[Y_RANGES].Val + 10*List[PROBEATT].Val);
      WaveProcess();
      List[Y_RANGES].Flg |= UPD;
    } else if((Tmp > 110)&&(List[Y_RANGES].Val < 9)){
      List[Y_RANGES].Val++;
      Set_Range(List[Y_RANGES].Val + 10*List[PROBEATT].Val);
      WaveProcess();
      List[Y_RANGES].Flg |= UPD;
    }
  }
  if(List[FITTRIGG].Val){
    Tmp = (Vmax-Vmin)/4;
    if((List[THRESHOLD].Val > (Vmid + Tmp))||
      (List[THRESHOLD].Val < (Vmid + Tmp))){
      List[THRESHOLD].Val = Vmid;
      WaveProcess();
      List[THRESHOLD].Flg |= UPD;
    }
  }
  if((List[FIT_BASE].Val)&&(Start >= 0)){
    Tmp = DEPTH[List[SMPL_DPTH].Val]/X_SIZE;
    if((Edge < 3*Tmp)&&(List[TIM_BASE].Val < 9)){
      List[TIM_BASE].Val++;
      Set_Base(List[TIM_BASE].Val);
      WaveProcess();
      List[TIM_BASE].Flg |= UPD;
    }
    if((Edge > 8*Tmp)&&(List[TIM_BASE].Val > 0)){
      List[TIM_BASE].Val--;
      Set_Base(List[TIM_BASE].Val);
      WaveProcess();
      List[TIM_BASE].Flg |= UPD;
    }
  }
}
/******************************** END OF FILE *********************************/
