/******************** (C) COPYRIGHT 2013 e-Design Co.,Ltd. *********************
  File Name : Main.c                                         Author : bure,xie
********************************************************************************

*******************************************************************************/
#include "string.h"
#include "Version.h"
#include "usb_lib.h"
#include "HDW_Config.h"
#include "DFU_Func.h"
#include "Func.h"
#include "Draw.h"
#include "BIOS.h"
#include "Menu.h"
#include "Process.h"
#include "FAT12.h"
#include "File.h"
#include "Display.h"


extern s16 TIM_BASE_Val;

void main(void)
{
  u16 Range, i; 
  u8 SAVE_FLAG = 0,rc=0;
  u8 Pop_Num = 0;
//  u8* str;
 
  Set_System();
  NVIC_Configuration();
  GPIO_Config();
  DMA_Configuration();
  ADC_Configuration();
//---add----------------------------------------------------------------------//  
 rc=Init_Fat_Value();  
  SPI_Config(); 
  USB_Init();  
  Tim_Config();

/*----------显示开机提示信息页面----------*/
  if(*__DevInfoStr(OEM) == '2') SetColor(WHT, BLK);
  else                          SetColor(BLK, WHT);
  DispStr(8, 90, PRN, "         Oscilloscope Ver5.09      ");
  DispStr(8, 70, PRN, "        System Initializing...       ");
  if(rc==1)
  {  SetColor(RED, BLK);
    DispStr(8, 50, PRN, "        Hardware  Versions Error    ");
      while(1){;;};
  }
  Read_Parameter();
  memcpy(Data, DEMO, 300);
  PopType |= DEMO_POP;

  if(Key_Status_Last & 0x02) {
    __ClrScrn(BLK);
    Calibrate(); // 开机按M键执行垂直偏移校正程序
  }
  else  Delayms(1000);

  __ClrScrn(GRY);
  Update_Titel();
   if(__Lic_ok()) PopType &= ~DEMO_POP;  // 检查 Licence 正确则关闭DEMO窗

  while (1){
   if((PopType & DEMO_POP)&&(!(PopType &(DAILOG_POP+LIST_POP)))) Demo_Pop();
    Update_Detail();
    Update_Blink();
//    DispStatus();            // 显示运行状态
    WaveProcess();
    __DrawWindow(WaveBuf, ParamTab);
    
   if(TIM_BASE_Val!=0)
    {
      List[TIM_BASE].Val=TIM_BASE_Val;
      List[TIM_BASE].Flg |= UPD;
      List[TIM_BASE].Flg |= UPD;
      TIM_BASE_Val=Current;
      Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;
      Update_Titel();
      Current=TIM_BASE_Val;
      TIM_BASE_Val=0;
    }
    if(Key_Buffer != 0){
      PopupCnt = 10;                  // 无按键时菜单窗维持显示 10秒
      Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;
      if((Current != ME)&&(Current != FN)) List[MenuNow[Current]].Flg |= UPD;
     switch (Key_Buffer){
     case KEYCODE_MEM:
       if(PopType & DAILOG_POP) Close_Pop();
       else {
         Dialog_Pop("Save The File?");
         Pop_Num = 2;
         if(MenuNow[FN]>=LOAD_DAT)MenuNow[FN]=SAVE_BMP;
         SetColor(GRY, TCOLOR[FN]);
         u8ToDec3(NumStr, List[MenuNow[FN]].Val);
         DispStr(S_X[MenuNow[FN]],    2, PRN, "Save ");
         DispStr(S_X[MenuNow[FN]]+32, 2, PRN, NumStr);
         DispStr(S_X[MenuNow[FN]]+56, 2, PRN, (u8*)(List[MenuNow[FN]].PTR +(MenuNow[FN] - SAVE_BMP)*6));
         SAVE_FLAG=KEYCODE_MEM;
        }
        break;
     case KEYCODE_ACT:
       SAVE_FLAG=0;
       if(PopType & DAILOG_POP) Close_Pop();
       else {
         Dialog_Pop("Save ParamTab?");
         Pop_Num = 1;}
       break;
     case KEYCODE_PLAY:
       if((Current == CA)&&(Cal_flag==0)){
         if(PopType & DAILOG_POP){
           if(Pop_Num == 1){
             Save_Parameter();
             Close_Pop();
           }
           else{
             Close_Pop();
             List_Pop();
           }
         } else if((PopType & LIST_POP) && (MenuNow[CA]==RESTORE )) {
           Restore_CalPar();
           DispStr(S_X[MenuNow[FN]], 2, PRN, "Restore Set");
         }
         else if((PopType & LIST_POP) && (MenuNow[CA]==CAL_AUTO )) { //0513
           Close_Pop();
           Calibrate_Pop("Waiting for Calibration");
           Auto_Calibrate();
           Close_Pop();
           CalSave_Pop("Save ParamTab?");
           Cal_flag = 1;
         }
         break;
       }
//-------------------
       if ((Current == FN)||(PopType & DAILOG_POP)){
         i = 0;
         if((PopType & DAILOG_POP ) &&(SAVE_FLAG!=KEYCODE_MEM)){
            Save_Parameter();
            Cal_flag = 0;
         }
         else{
           if(SAVE_FLAG==KEYCODE_MEM)Close_Pop();
           if(MenuNow[FN] == SAVE_BMP) i = Save_Bmp(List[SAVE_BMP].Val);
           if(MenuNow[FN] == SAVE_DAT) i = Save_Dat(List[SAVE_DAT].Val);
           if(MenuNow[FN] == SAVE_BUF) i = Save_Buf(List[SAVE_BUF].Val);
           if(MenuNow[FN] == SAVE_CSV) i = Save_Csv(List[SAVE_CSV].Val);
           if(MenuNow[FN] == LOAD_DAT) i = Load_Dat(List[LOAD_DAT].Val);
           if(MenuNow[FN] == LOAD_BUF) i = Load_Buf(List[LOAD_BUF].Val);
           SAVE_FLAG = 0;
         }
         DispFileInfo(i);
         if(PopType & DAILOG_POP)  Close_Pop();
        } else {
          if(Status & STOP){
            if(Status & LOAD )Status &= ~LOAD;
            Status &= ~STOP;     // 进入运行状态
            if(List[SYNCMODE].Val==SINGL) ADC_Start();
            if(*(u16*)F_Sector==0xaa55)Restore_Scene();
          } else { 
            Status |=  STOP;     // 进入暂停状态
          #ifdef SAVEBMP
            Save_Bmp(List[SAVE_BMP].Val);
            List[SAVE_BMP].Val++;
          #endif
          }
        }
        DispStatus();            // 显示运行状态

        break;
      case KEYCODE_MANU:
        if(!(PopType & LIST_POP)){
          if(Current == CA){
            if(!(PopType & DAILOG_POP)){
              Dialog_Pop(" Calibration ?");
              Pop_Num = 3;}
            else  {
              Close_Pop();
              Cal_flag = 0;}
          }
          else  {
            Cal_flag = 0;
            List_Pop(); // 菜单窗关闭时, 打开菜单窗
          }
          //else  Close_Pop(); // 菜单窗关闭时, 打开菜单窗
        }
        else {
          Close_Pop();                // 菜单窗已打开时, 关闭菜单窗
          // ------------ 恢复提示信息显示区左端原有显示信息 ------------
          if(Current == XN)      List[T1_POSI   ].Flg |= UPD;
          else if(Current == YN) List[V1_POSI   ].Flg |= UPD;
          else                   List[MenuNow[SN]].Flg |= UPD;
          // ------------ 恢复提示信息显示区右端原有显示信息 ------------
          if(MenuNow[Current] == SMPL_DPTH) List[SMPL_DPTH].Flg  |= UPD; // 采样深度更新显示
          else if(Current == FN)           List[MenuNow[FN]].Flg |= UPD; // 指定的文件读写项更新显示
          else                             List[MenuNow[ME]].Flg |= UPD; // 指定的波形测量项更新显示
        }
        break;
      case KEYCODE_UP:
        if (PopType & LIST_POP){
          if(MenuNow[Current] > MU_BEGN[Current]) MenuNow[Current]--;
          else                  MenuNow[Current] += M_ITEMS[Current]-1;
          if(((List[FITRANGE].Val)&&(MenuNow[Current] == Y_RANGES))||
          ((List[FIT_BASE].Val)&&(MenuNow[Current] == TIM_BASE))){
            MenuNow[Current] += M_ITEMS[Current]-1;
          }
          if((List[FITTRIGG].Val)&&(MenuNow[Current] == THRESHOLD)){
            MenuNow[Current]--;
          }
        } else {
          if (Current > 0) Current--;
          else             Current = CA;
        }
        break;
     case KEYCODE_DOWN:
        if(PopType & LIST_POP){
          MenuNow[Current]++;
          if(MenuNow[Current] >=(MU_BEGN[Current]+M_ITEMS[Current])){
            MenuNow[Current] = MU_BEGN[Current];
          }
          if(((List[FITRANGE].Val)&&(MenuNow[Current] == Y_RANGES))||
          ((List[FITTRIGG].Val)&&(MenuNow[Current] == THRESHOLD))||
          ((List[FIT_BASE].Val)&&(MenuNow[Current] == TIM_BASE))){
            MenuNow[Current]++;
          }
        } else {
          if (Current < 7) Current++;
          else             Current = YN;
        }
        break;
      case KEYCODE_LEFT:
        if((Current==CA)&& !(PopType & LIST_POP))break;
        List[MenuNow[Current]].Flg |= UPD;  // 刷新变更前原显示项目内容
        if(List[MenuNow[Current]].Val <=MIN[MenuNow[Current]]){
          if((MenuNow[Current] == FITRANGE)||(MenuNow[Current] == YN_TYPE )||
            ( MenuNow[Current] == FIT_BASE)||(MenuNow[Current] == XN_TYPE )||
            ( MenuNow[Current] == FITTRIGG)||(MenuNow[Current] == TR_MODE )||
            ( MenuNow[Current] == SYNCMODE)||(MenuNow[Current] == EXT_REFN)||
            (MenuNow[Current] == PROBEATT))
            List[MenuNow[Current]].Val =MAX[MenuNow[Current]] ;
        } else {
          if((MenuNow[Current] == VIEWPOSI)&&((List[MenuNow[Current]].Val) >=150 ))
            List[MenuNow[Current]].Val-=25;
          else{
          //============5.04修复触发线超出界面死机========
            if(MenuNow[Current] == THRESHOLD){  
              if((List[V0_POSI].Val + List[THRESHOLD].Val) > 2)  //不超出下限
                List[MenuNow[Current]].Val--;
            }
            else if(MenuNow[Current] == V0_POSI){
              if((List[V0_POSI].Val + List[THRESHOLD].Val) <2){
                List[V0_POSI].Val--;
                List[THRESHOLD].Val++;
              }else List[MenuNow[Current]].Val--;
            }else if((MenuNow[Current] ==V1_POSI) && (List[V1_POSI].Val <= List[V2_POSI].Val))
            List[MenuNow[Current]].Val=List[V2_POSI].Val;
            else List[MenuNow[Current]].Val--;
          }
          //==============================================
            if(MenuNow[Current] == SMPL_DPTH){
              List[VIEWPOSI].Val=1 ;
           }
            if((MenuNow[Current] ==T2_POSI) && (List[T2_POSI].Val <= List[T1_POSI].Val))
              List[MenuNow[Current]].Val=List[T1_POSI].Val;
        }
        if(MenuNow[Current] == CAL_ZERO){
          if(Kn[Range] >MIN[CAL_ZERO] ) Kn[Range]--;
        }
        if(MenuNow[Current] == CAL_GAIN){
          if(Km[Range] >MIN[CAL_GAIN]) Km[Range]--;
          Set_Y_Pos(Range, List[V0_POSI].Val);  // 重新设置垂直偏移位置
        }
        if(MenuNow[Current]==CAL_AUTO){ //0513
           List[Y_RANGES].Flg |= UPD;
           if(List[Y_RANGES].Val >MIN[Y_RANGES])
           List[Y_RANGES].Val--;
        }
        if(Current == TR) Status &= ~STOP; // 改变同步模式后恢复运行状态
        break;
      case KEYCODE_RIGHT:
        if((Current==CA)&& !(PopType & LIST_POP))break;
        List[MenuNow[Current]].Flg |= UPD;  // 刷新变更前原显示项目内容
        if(List[MenuNow[Current]].Val >=MAX[MenuNow[Current]] ){
          if((MenuNow[Current] == FITRANGE)||(MenuNow[Current] == YN_TYPE )||
            ( MenuNow[Current] == FIT_BASE)||(MenuNow[Current] == XN_TYPE )||
            ( MenuNow[Current] == FITTRIGG)||(MenuNow[Current] == TR_MODE )||
            ( MenuNow[Current] == SYNCMODE)||(MenuNow[Current] == EXT_REFN)||
            (MenuNow[Current] == PROBEATT))
            List[MenuNow[Current]].Val =MIN[MenuNow[Current]] ;
        } else {
            if(MenuNow[Current] == VIEWPOSI)
            {
              if(List[MenuNow[Current]].Val <(DEPTH[List[SMPL_DPTH].Val]-(301-List[SMPL_DPTH].Val)))
              {
                if(List[MenuNow[Current]].Val >150 )
                  List[MenuNow[Current]].Val += 25;
                else
                  List[MenuNow[Current]].Val++;
              }
            }
            else{
              //============4.27修复触发线超出界面死机========
              if(MenuNow[Current] == THRESHOLD){
                if((List[V0_POSI].Val + List[THRESHOLD].Val) < 198)
                 List[MenuNow[Current]].Val++; 
              }
              else if(MenuNow[Current] == V0_POSI){
                if((List[V0_POSI].Val + List[THRESHOLD].Val) > 198){
                  List[V0_POSI].Val++;
                  List[THRESHOLD].Val--;
                }else List[MenuNow[Current]].Val++;
              }
              else if((MenuNow[Current] ==V2_POSI) && (List[V2_POSI].Val >= List[V1_POSI].Val))
                List[MenuNow[Current]].Val=List[V1_POSI].Val;
              else List[MenuNow[Current]].Val++;
            }
            //==============================================  
            if((MenuNow[Current] ==T1_POSI) && (List[T1_POSI].Val >= List[T2_POSI].Val))
               List[MenuNow[Current]].Val=List[T2_POSI].Val;
        }
        if(MenuNow[Current] == CAL_ZERO){
          if(Kn[Range] <MAX[CAL_ZERO] ) Kn[Range]++;
        }
        if(MenuNow[Current] == CAL_GAIN){
          if(Km[Range] < MAX[CAL_GAIN]) Km[Range]++;
          Set_Y_Pos(Range, List[V0_POSI].Val);  // 重新设置垂直偏移位置
        }
        if(MenuNow[Current]==CAL_AUTO){ //0513
           List[Y_RANGES].Flg |= UPD; 
           if(List[Y_RANGES].Val <MAX[Y_RANGES])
           List[Y_RANGES].Val++;
        }
        if(Current == TR) {Status &= ~STOP; // 改变同步模式后恢复运行状态
        if(List[SYNCMODE].Val==SINGL)ADC_Start(); } 
        break;
     } //switch end
     List[MenuNow[Current]].Flg |= UPD;
      if(PopType & LIST_POP) Show_List();
      Update_Titel();
      Key_Buffer = 0;
    } //if key end
  } //while end
}
/********************************* END OF FILE ********************************/
