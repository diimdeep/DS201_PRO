/********************* (C) COPYRIGHT 2011 e-Design Co.,Ltd. ********************
 File Name : Bios.c
 Version   : For DS201 Ver 4.0                                Author : bure,xie
*******************************************************************************/
#include "string.h"
#include "HDW_Config.h"
#include "BIOS.h"
#include "Func.h"
#include "Menu.h"
#include "Draw.h"
#include "Process.h"
#include "DFU_Func.h"

vu8  Key_Repeat_Cnt = 0, Key_Wait_Cnt = 0, Key_Status_Last = 0, Key_Buffer = 0;
vu16 Cursor_Cnt = 0, Sec_Cnt, mS_Cnt = 0, Tim_Cnt, Delay_Cnt = 0, PopupCnt = 10;
vu8  Twink = 0, Blink, TmpKeyP = 0, TmpKeyM = 0;
u32  Vb_Sum=0, Vbattrey;

/*******************************************************************************
 Set_Range: 电压量程转换模拟开关状态设置
*******************************************************************************/
void Set_Range(u8 Range)
{
  switch (Range) {
    case 0://10mV ×1/Div
    case 1://20mV ×1/Div
    case 2://50mV ×1/Div
    case 3://0.1V ×1/Div
      RANGE_A_HIGH(); RANGE_B_HIGH(); RANGE_C_HIGH(); RANGE_D_HIGH(); break;
    case 4://0.2V ×1/Div实
    case 5://0.5V ×1/Div
    case 6://1V ×1/Div
      RANGE_A_HIGH(); RANGE_B_HIGH(); RANGE_C_HIGH(); RANGE_D_LOW(); break;
    case 7://2V ×1/Div
    case 8://5V ×1/Div
    case 9://10V ×1/Div
      RANGE_A_LOW(); RANGE_B_HIGH(); RANGE_C_HIGH(); RANGE_D_LOW(); break;
    case 10://0.1V/Div （10mV ×10/Div）
    case 11://0.2V/Div （20mV ×10/Div）
    case 12://0.5V/Div （50mV ×10/Div）
    case 13://  1V/Div （0.1V ×10/Div）
      RANGE_A_HIGH(); RANGE_B_LOW(); RANGE_C_HIGH(); RANGE_D_LOW(); break;
    case 14:// 2V/Div （0.2V ×10/Div）
    case 15:// 5V/Div （0.5V ×10/Div）
    case 16://10V/Div （  1V ×10/Div）
      RANGE_A_LOW(); RANGE_B_LOW(); RANGE_C_HIGH(); RANGE_D_LOW(); break;
    case 17:// 20V/Div （ 2V ×10/Div）
    case 18:// 50V/Div （ 5V ×10/Div）
    case 19://100V/Div （10V ×10/Div）
      RANGE_A_HIGH(); RANGE_B_HIGH(); RANGE_C_LOW(); RANGE_D_LOW(); break;
    default : //Connecte To GND
      RANGE_A_LOW(); RANGE_B_LOW(); RANGE_C_LOW(); RANGE_D_LOW(); break;
  }
}
/*******************************************************************************
 Set_Duty: 输出方波占空比设置
*******************************************************************************/
void Set_Duty(void)
{
  TIM4_CCR1 = (FARR[List[FREQ_OUT].Val]+1)*(10-List[FREQ_DUT].Val)/10;
}
/*******************************************************************************
 Set_Fout: 输出方波频率设置
*******************************************************************************/
void Set_Fout(u8 Range)
{
  TIM4_PSC  =  FPSC[Range];
  TIM4_ARR  =  FARR[Range];
  TIM4_CCR1 = (FARR[Range]+1)/2;
}
/*******************************************************************************
 Set_Base: 水平扫描时基转换设置
*******************************************************************************/
void Set_Base(u8 Base)
{
  TIM1_PSC = PSC[Base];
  TIM1_ARR = ARR[Base];
  TIM1_CCR1= (ARR[Base]+1)/2;
}
/*******************************************************************************
 Set_Y_Pos: 零电压基线调整设置
*******************************************************************************/
void Set_Y_Pos(u16 Range, u16 Yp)
{
  TIM2_CCR4 = 1024 + (Pg[Range]*512*(100 - Yp))/(Km[Range]*200);

}
/*******************************************************************************
 ADC_Start: 重新开始ADC扫描采样
*******************************************************************************/
void ADC_Start(void)
{
  memset(Sampl, 0, DEPTH[List[SMPL_DPTH].Val]);
  Sampl[0]   = List[TIM_BASE].Val;         // 保存采样时基档位值
  DMA1_CCR1   = 0x00000000;
  DMA1_CMAR1  = (u32)&Sampl[1];             // 重新设定DMA通道1
  DMA1_CNDTR1 = DEPTH[List[SMPL_DPTH].Val]; // 0x00001000;
  DMA1_CCR1  |= 0x00000001;                 // 重新开始扫描采样
}
/*******************************************************************************

*******************************************************************************/
u16 ADC_Cnt(u16 Value)
{
  return Value - DMA1_CNDTR1;   // ADC采样计数值
}
/*******************************************************************************
 USB_ON: 检测外接USB状态
*******************************************************************************/
u8 USB_On(void)
{
  if(GPIOB_IDR & 0x0400) return 1; // USB On
  else                   return 0; // USB Off
}

/*******************************************************************************
 SD_Card_ON: 检测SD_Card状态
*******************************************************************************/
char SD_Card_ON(void)
{
  if(GPIOD_IDR & 0x0400 ) return 0;
  else                    return 1; //SD_Card_ON
}
/*******************************************************************************
 Bat_Vol: 检测电池电压
*******************************************************************************/
u16 Bat_Vol(void)
{
  u16 Tmp = Vb_Sum/16;
  if((Vbattrey >(Tmp+10))||(Vbattrey <(Tmp-10))) Vbattrey = Tmp; // 去抖动
  return Vbattrey;
}
/*******************************************************************************
* 函数名：KeyScan    说明：读取按键  返回：按键扫描码，KeyBuff=按键码
*******************************************************************************/
void Tim3_ISP(void)
{
  unsigned char KeyCode;
  TIM3_SR = 0;                             //清中断标志
  Tim_Cnt++;
  if(Delay_Cnt>0) Delay_Cnt--;
  if (mS_Cnt > 0){
    mS_Cnt--;
    if ((mS_Cnt%20)== 0){                 //每20mS读1次键盘
      if(Key_Wait_Cnt)    Key_Wait_Cnt--;
      if(Key_Repeat_Cnt)  Key_Repeat_Cnt--;
      KeyCode = KeyScan();
      if(KeyCode !=0) Key_Buffer = KeyCode;

      if(Cursor_Cnt >0) Cursor_Cnt--;
      else {
        Cursor_Cnt = 12;                  // 12*20mS=240mS
        Twink = !Twink;
        Blink =  BLINK;
      }
    }
    if ((mS_Cnt%100)== 0){
      Vb_Sum += ((1330*(*(vu16*)(ADC1_DR_ADDR+2)))>> 10)- Vb_Sum/16;
      List[BATTERY].Flg |= UPD;// 刷新电池电量显示
    }
  } else {
    mS_Cnt = 1000;
    if(PopupCnt > 0) {
      if(Cal_flag == 0)PopupCnt--;
      else ;
    }
    else             Close_Pop();        // 关闭对话框
  }
}
char KeyScan(void)
{
  char Status = 0, KeyCode = 0;

  if((~GPIOA_IDR)& 0x0010) Status |= 0x01; //KEY_PLAY_ON(KEY_A)
  if((~GPIOD_IDR)& 0x0800) Status |= 0x02; //KEY_MENU_ON(KEY_OK)
  if((~GPIOA_IDR)& 0x0040) Status |= 0x04; //KEY_UP_ON
  if((~GPIOD_IDR)& 0x0200) Status |= 0x08; //KEY_DOWN_ON
  if((~GPIOA_IDR)& 0x0020) Status |= 0x10; //KEY_LEFT_ON
  if((~GPIOA_IDR)& 0x0080) Status |= 0x20; //KEY_RIGHT_ON
  if((~GPIOA_IDR)& 0x0008) Status |= 0x40; //KEY_B_ON

  if(Status &(~Key_Status_Last)){                     // 有按键动作
    Key_Wait_Cnt   = 50;                              // 重置持续按键 1.0S 计数
    Key_Repeat_Cnt = 3;                               // 设定 60mS 自动重复周期
    if(Status & 0x01)  TmpKeyP = KEYCODE_PLAY;        // KeyCode(Play/A)
    if(Status & 0x02)  TmpKeyM = KEYCODE_MANU;        // KeyCode(Manu/OK)
    if(Status & 0x04)  KeyCode = KEYCODE_UP;          // KeyCode(Up)
    if(Status & 0x08)  KeyCode = KEYCODE_DOWN;        // KeyCode(Down)
    if(Status & 0x10)  KeyCode = KEYCODE_LEFT;        // KeyCode(Left)
    if(Status & 0x20)  KeyCode = KEYCODE_RIGHT;       // KeyCode(Right)
  } else {
    if(Status & Key_Status_Last){
      if((Key_Wait_Cnt < 25)&&(Key_Repeat_Cnt == 0)){ // 持续按键 0.5S 以上
        if(Status & 0x04)  KeyCode = KEYCODE_UP;      // KeyCode(Up)
        if(Status & 0x08)  KeyCode = KEYCODE_DOWN;    // KeyCode(Down)
        if(Status & 0x10)  KeyCode = KEYCODE_LEFT;    // KeyCode(Left)
        if(Status & 0x20)  KeyCode = KEYCODE_RIGHT;   // KeyCode(Right)
        if(Status & 0x3C)  Key_Repeat_Cnt = 3;        // 设定 60mS 自动重复周期
      }
      if((Key_Wait_Cnt == 0)&&(Key_Repeat_Cnt == 0)){ // 持续按键 1.0S 以上
        if(Status & 0x01) {KeyCode = KEYCODE_ACT; TmpKeyP = 0;}
        if(Status & 0x02) {KeyCode = KEYCODE_MEM; TmpKeyM = 0;}
        if(Status & 0x03)  Key_Repeat_Cnt = 50;       //设定 1.0S 自动重复周期
      }
    } else {
      if(TmpKeyP) {KeyCode = TmpKeyP; TmpKeyP = 0;}
      if(TmpKeyM) {KeyCode = TmpKeyM; TmpKeyM = 0;}
      Key_Wait_Cnt=50;                                //重置持续按键 1.0S 计数
    }
  }
  Key_Status_Last = Status;
  return KeyCode;
}



/********************************* END OF FILE ********************************/

