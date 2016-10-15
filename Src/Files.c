/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
Project Name: DS201V4																							Version 4.00	
* File Name: Files.c       Hardware: DS0201V3.00                  Author: xie
*******************************************************************************/
#include <stdlib.h>
#include "string.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_flash.h"
#include "Draw.h"
#include "Func.h"
#include "File.h"
#include "FAT12.h"
#include "Menu.h"
#include "Process.h"
#include "Display.h"
#include "DFU_Func.h"
#include "Ext_Flash.h"

#define Page_Address    0x0801FC00


//vu8 DFS_FUNC;
u8* NamePtr;
u32*  ViPtr;
s16 TIM_BASE_Val;
extern u8 USB_FLAG;

uc16 CLK_TAB[44] =
  {0x000,0x000,0x000,0x000,0x070,0x3FF,0x070,0x000,0x000,0x000,0x000,
   0x000,0x002,0x004,0x008,0x070,0x070,0x070,0x080,0x100,0x200,0x000,
   0x020,0x020,0x020,0x020,0x070,0x070,0x070,0x020,0x020,0x020,0x020,
   0x000,0x200,0x100,0x080,0x070,0x070,0x070,0x008,0x004,0x002,0x000};

uc16 BMP_Color[16] = {WHT,  CYN, CYN_,  YEL,  YEL_, PUR, PUR_, GRN,
                      GRN_, GRY, ORN, BLU, RED, BLK, LGN,  DAR};

uc8  BmpHead[54]   = {0X42, 0X4D, 0X76, 0X96, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X76, 0X00, 0X00, 0X00, 0X28, 0X00,
                      0X00, 0X00, 0X40, 0X01, 0X00, 0X00, 0XF0, 0X00,
                      0X00, 0X00, 0X01, 0X00, 0X04, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X82, 0X0B, 0X00, 0X00, 0X12, 0X0b,
                      0X00, 0X00, 0X12, 0X0b, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

unsigned int DiskStart, Hidden_Sec, Root_Addr, ClusterNum, File_Addr;
unsigned short RsvdSecCnt, SectorSize, FAT_Size, FileLen;
unsigned short SecPerClus, DirFliePtr, DirSecNum, FAT1_Addr, FAT_Ptr;
unsigned char  F_Buff[4096],F_Sector[512];//
u32* FilePtr;
/*******************************************************************************
Color_Num: 求出当前颜色的对应调色板编号
*******************************************************************************/
u8 Color_Num(u16 Color)
{
  if(Color == WHT)                    return 0;
  else if((Color & CYN  )== CYN  )    return 1;
    else if((Color & CYN_ )== CYN_ )  return 2;
  else if((Color & YEL   )== YEL   )  return 3;
  else if((Color & YEL_  )== YEL_  )  return 4;
  else if((Color & PUR )== PUR )      return 5;
  else if((Color & PUR_)== PUR_)      return 6;
  else if((Color & GRN   )== GRN   )  return 7;
  else if((Color & GRN_  )== GRN_  )  return 8;
  else if((Color & GRY  )== GRY  )    return 9;
  else if((Color & ORN)== ORN)        return 10;
  else if((Color & BLU  )== BLU  )    return 11;
  else if((Color & RED   )== RED   )  return 12;
  else if((Color & BLK   )== BLK   )  return 13;
  else if((Color & LGN   )== LGN   )  return 14;
  else                                return 15;
}
/*******************************************************************************
 打开指定扩展名的文件            输入：文件扩展名             返回值：0x00=成功
*******************************************************************************/
void Make_Filename(s16 FileNo,unsigned char* FileName)
{
	u8 Num[4];
	u8ToDec3(Num, FileNo);
	FileName[4] = Num[0];
  FileName[5] = Num[1];
  FileName[6] = Num[2];
}

/*******************************************************************************
Save_Dat: 保存当前屏幕显示图像原始数据    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Dat(s16 FileNo)
{
  u8 pFileName[12]="FILE    DAT";
  u16 Rvalue=DISK_RW_ERR ;
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16* ptr;
  Make_Filename(FileNo, pFileName);
  memset(F_Buff,0,4096);
  if(OpenFileWr(F_Buff, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  F_Buff[0]=1;              //Ver0001
  F_Buff[1]=0;
  memcpy(F_Buff+2, WaveBuf, 300);
  ptr=(u16*)F_Buff+(2+300)/2;
  *ptr++=List[Y_RANGES].Val;
  *ptr++=List[PROBEATT].Val;  
  if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
  if(CloseFile(F_Buff, 512, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(List[SAVE_DAT].Val<99)List[SAVE_DAT].Val++;
  return OK;
}
/*******************************************************************************
Load_Dat: 加载保存过的屏幕图像原始数据    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Load_Dat(s16 FileNo)
{
  u8 pFileName[12]="FILE    DAT";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16  Rvalue=0;
  int i;
  u16* ptr;
  Make_Filename(FileNo, pFileName);
  if((Rvalue=OpenFileRd(F_Buff, pFileName, pCluster, pDirAddr))!= OK) return Rvalue;
  if(ReadFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR;
  memcpy(Data, F_Buff+2, 300);
  ptr=(u16*)F_Buff+(2+300)/2;
  List[Y_RANGES].Val=*ptr++;
  List[PROBEATT].Val=*ptr++;
  for(i=0;i<2;i++)List[i].Flg |= UPD;  
  List[EXT_REFN].Flg |= UPD;  
  Update_Detail();
  if(List[LOAD_DAT].Val<99)List[LOAD_DAT].Val++;
  return OK;
}

/*******************************************************************************
Save_Bmp: 保存当前屏幕显示图像为BMP格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Bmp(s16 FileNo)
{
	u8  pFileName[12]="IMG_    BMP";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 Rvalue=DISK_RW_ERR;
  s16 x, y, i=54, j,ColorH,ColorL ;
  u16 k=0, l=0;
  u32 length;
  
  if(flash_mode==FLASH_8M)length=4096;
  else length=512;
  
  if(Current == FN){
    Close_Pop();
    __DrawWindow(WaveBuf, ParamTab);
  }
  Make_Filename(FileNo, pFileName);
  if(OpenFileWr(F_Buff, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  memcpy(F_Buff, BmpHead, 54);
  i = 0x0036;                                     // 调色板存放开始地址
  for(j=0; j<16; ++j){
    F_Buff[j*4 +i+0]=(BMP_Color[j] & 0xF800)>>8;  // Bule
    F_Buff[j*4 +i+1]=(BMP_Color[j] & 0x07E0)>>3;  // Green&
    F_Buff[j*4 +i+2]=(BMP_Color[j] & 0x001F)<<3;  // Red
    F_Buff[j*4 +i+3]= 0;                          // Alpha
  }
  i = 0x0076;                                     // 图像数据开始存放地址
  for(y=0; y<240; y++){
    for(x=0; x<320 ; x+=2){
      ColorH =__Get_Pixel(x, y);
      ColorL =__Get_Pixel(x+1, y);
      F_Buff[i] =(Color_Num(ColorH)<<4)+ Color_Num(ColorL);
      i++;
      if(i>=length){
        i=0;
        if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
        if(l==0)PrintClk(168,2,(k++ >>1)& 3);         // 进度指示
        l++;
        if(l>=2)l=0;
      }
    }
  }
  if(i!=0){
      if(ProgFileSec(F_Buff, pCluster)!= OK){List_Pop();return FILE_RW_ERR;} // 写入数据
    }
  if(CloseFile(F_Buff, 76*512, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(Current == FN)List_Pop();
  return OK;
}
/*******************************************************************************
Save_Buf: 保存采集数据缓存区为BUF格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Buf(s16 FileNo)
{
  u8 pFileName[12]="DATA_   BUF";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 i=0, Rvalue=DISK_RW_ERR;
  u16 *p;
  u8  l=0;
  u16 n,k;

  Make_Filename(FileNo, pFileName);
  memset(F_Buff,0,4096);
  if(OpenFileWr(F_Buff, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  if(flash_mode==FLASH_8M){
    n=2; k=4096;
  }else{
    n=16;k=512;
  }
  
  
  for(i=0; i<n; i++){
//    memcpy(F_Buff,&(Sampl[i*512/2+2]),512);
    memcpy(F_Buff,&(Sampl[i*k/2+2]),k);
    if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
    PrintClk(264,2,(l++ >>1) & 3);                    // 进度指示
  }
  memset(F_Buff,0,k);
  p =(u16*)&F_Buff;
  *p++=Sampl[0];
  *p++=Sampl[1];
  *p++=List[Y_RANGES].Val;
  *p++=List[PROBEATT].Val;
  *p++=List[SMPL_DPTH].Val;
  for(i=0;i<20;i++)*p++=Kn[i];
  for(i=0;i<20;i++)*p++=Km[i];
  for(i=0;i<20;i++)*p++=Yd[i];
  if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
  if(CloseFile(F_Buff, 16*512+k, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  return OK;
}
/*******************************************************************************
Restore_Scene: 加载采集数据缓冲区时破坏了现场，恢复正常时，恢复现场
*******************************************************************************/
void Restore_Scene(void)
{  int i;
  u16 *ptr;

  ptr=(u16*)F_Sector;
  *ptr++=0x0;
  Sampl[0]=*ptr++;
  Sampl[1]=*ptr++;
  List[Y_RANGES].Val=*ptr++;
  List[PROBEATT].Val=*ptr++;
  List[SMPL_DPTH].Val=*ptr++;
  for(i=0;i<20;i++)Kn[i]=*ptr++;
  for(i=0;i<20;i++)Km[i]=*ptr++;
  for(i=0;i<20;i++)Yd[i]=*ptr++;
}
/*******************************************************************************
Load_Buf: 加载保存过的采集数据缓冲区    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Load_Buf(s16 FileNo)
{
	u8 pFileName[12]="DATA    BUF";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16 *ptr;
  u32 i,Rvalue=1;
  u32 n,k;

  Make_Filename(FileNo, pFileName);
  if( (Rvalue=OpenFileRd(F_Buff, pFileName, pCluster, pDirAddr))!= OK) return Rvalue;
  if(flash_mode==FLASH_8M){
    n=2;k=4096;
  }else{
    n=16;k=512;
  }
  
	for(i=0;i<n;i++){
    if(ReadFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR;
    memcpy(&(Sampl[i*k/2+2]),F_Buff,k);
	}
  if(ReadFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR;
  ptr=(u16*)F_Sector;                               //保存现场
  *ptr++=0xaa55;
  *ptr++=Sampl[0];
  *ptr++=Sampl[1];
  *ptr++=List[Y_RANGES].Val;
  *ptr++=List[PROBEATT].Val;
  *ptr++=List[SMPL_DPTH].Val;
  for(i=0;i<20;i++)*ptr++=Kn[i];
  for(i=0;i<20;i++)*ptr++=Km[i];
  for(i=0;i<20;i++)*ptr++=Yd[i];   			
  ptr =(u16*)F_Buff;                            //读原来的参数
  Sampl[0]=*ptr++;
  Sampl[1]=*ptr++;
  List[Y_RANGES].Val=*ptr++;
  List[PROBEATT].Val=*ptr++;
  List[SMPL_DPTH].Val=*ptr++;
  for(i=0;i<20;i++)Kn[i]=*ptr++;
  for(i=0;i<20;i++)Km[i]=*ptr++;
  for(i=0;i<20;i++)Yd[i]=*ptr++;
  Status |=  STOP;
  DispStatus();
  return 0;
}
//------------------------------------------------------------------
void make_Vertical(u8* buf,u8* len)
{
  u8 i=0;
  u8* ptr;

  ptr=(u8*)&R[List[Y_RANGES].Val];
  while(*ptr != 0)buf[i++] = *ptr++;
  buf[i++]   = 0x2a;
  ptr=(u8*)&N[List[PROBEATT].Val];
  while(*ptr != 0)buf[i++] = *ptr++;
  buf[i]   = 0x2c;
  buf[i+1] = 0;
  *len = i+1;
}
/*******************************************************************************
Save_Csv: 保存采集数据缓存区为CSV格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Csv(s16 FileNo)
{
  u8 pFileName[12]="DATA    CSV";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 i,k=0,l=0,Rvalue=DISK_RW_ERR,length;
  u8 Num[4],track[4];
	s16 temp,Data;
  u8 count,Range;
  
  if(flash_mode==FLASH_8M)
    length=4096;
  else
    length=512;
  Range = List[Y_RANGES].Val + 10*List[PROBEATT].Val;
  Make_Filename(FileNo, pFileName);
  if(OpenFileWr(F_Buff, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  memcpy(F_Buff,"TRACK   ",8);
  make_Vertical(&F_Buff[8],&count);
  k=8+count;
  memcpy(&F_Buff[k],"\r\n",2);
  k+=2;
  for(i=2; i<4096; i++){
    Data = Kn[Range] + (Km[Range]*(2048-Sampl[i]))/Yd[Range];    // 计算当前波形点的基值
    temp=Range+(Data & 0xff);
    if(temp > 0){
      if(temp > 200)  track[0] = 199;
      else            track[0] = temp;
    } else            track[0] = 0;
    u8ToDec3(Num,track[0]);
    for(count=0; count<3; count++){
      if(Num[count] == 0) break;
      F_Buff[k++] = Num[count];
      if(k >= length){
        if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
        PrintClk(264,2,(l++ >>1) & 3);                    // 进度指示
        k = 0;
      }
    }
    F_Buff[k++] = 0x2c;
    if(k >= length){
      if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
      PrintClk(264,2,(l++ >>1)& 3);                    // 进度指示
      k = 0;
    }
    F_Buff[k++] = 0x0d;
    if(k >= length){
      if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
      PrintClk(264,2,(l++ >>1)& 3);                    // 进度指示
      k = 0;
    }
    F_Buff[k++] = 0x0a;
    if(k >= length){
      if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
      PrintClk(264,2,(l++ >>1)& 3);                    // 进度指示
      k = 0;
    }
  }
  if(k != 0){
    F_Buff[k++]=0x0d;
    F_Buff[k++]=0x0a;
    memset(&F_Buff[k],0,(512-k));
    k=0;
    if(ProgFileSec(F_Buff, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
    PrintClk(264,2,(l++ >>1)& 3);                     // 进度指示
  }
  if(CloseFile(F_Buff, l*length, pCluster, pDirAddr)!= OK) return DISK_RW_ERR;
  return OK;
}

/*******************************************************************************
 Save_Parameter: 保存当前的工作参数                          Return: 0= Success
*******************************************************************************/
char Save_Parameter(void)
{
  unsigned short i, j;
  u16* ptr;

  FLASH_Unlock();
  j=FLASH_ErasePage(Page_Address);
  if(j== FLASH_COMPLETE){
    ptr=(u16*)&F_Buff;
    *ptr++=0x0300;                              //Ver:3.00
    *ptr++=Current;
    *ptr++=Status;
    for(i=0;i<40;i++) *ptr++=List[i].Val;
    for(i=0;i<8;i++)  *ptr++=MenuNow[i];
    for(i=0;i<20;i++) *ptr++=Kn[i];
    for(i=0;i<20;i++) *ptr++=Km[i];               //88+3
    *ptr++=0xaa55;
    ptr=(u16*)&F_Buff;
    for(i=0; i<256; i+=2){                      //128 SHORT
      j= FLASH_ProgramHalfWord(Page_Address+i, *ptr++);
      if(j!= FLASH_COMPLETE) break;
    }
  }
  FLASH_Lock();
  return j;
}
/*******************************************************************************
Read_Parameter: 从FLASH读出参数域
*******************************************************************************/
void Read_Parameter(void)
{
  u8 i;
  u16* ptr;
  ptr=(u16*)(Page_Address+91*2);
  TIM_BASE_Val=List[TIM_BASE].Val;
  List[TIM_BASE].Val=0;
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Page_Address;
  if(*ptr++!=0x0300) return ;
  Current=*ptr++;
  List[MenuNow[Current]].Flg |= UPD;
  ptr++;
  for(i=0;i<40;i++) List[i].Val=*ptr++;
  for(i=0;i<8;i++)  MenuNow[i]=*ptr++;
  for(i=0;i<20;i++)Kn[i]=*ptr++;
  for(i=0;i<20;i++)Km[i]=*ptr++;
  TIM_BASE_Val=List[TIM_BASE].Val;
  List[TIM_BASE].Val=0;

  return ;
}

/*******************************************************************************
 Print_Clk: 进度指示
*******************************************************************************/
void PrintClk(u16 x0, u16 y0, u8 Phase)
{
  u16 i, j, b;
  x0=272;
  DispStr(x0-8, 2, PRN, "    ");
  for(i=0; i<11; ++i){
    b = CLK_TAB[Phase *11 + i];
    for(j=0; j<11; ++j){
      __Set_Posi((x0+j), (y0+i));
      if((b >> j)& 0x001)
        __Set_Pixel(GRN);
      else
        __Set_Pixel(Background);
    }
  }
}

void Restore_CalPar(void)
{
  u8 i = 0;
  
  TIM_BASE_Val=List[TIM_BASE].Val;
  List[TIM_BASE].Val=0;
  Current=0;
  List[MenuNow[Current]].Flg |= UPD;
  for(i=0;i<40;i++) List[i].Val= List_c[i].Val;
  for(i=0;i<8;i++)  MenuNow[i]= MenuNow_C[i];
  TIM_BASE_Val=List[TIM_BASE].Val;
  List[TIM_BASE].Val=0;
  for(i=Y_RANGES; i<=BATTERY; i++){
  List[i].Flg |= UPD;
  }
  Update_Titel();
  Close_Pop();
  //Save_Parameter();
}
/******************************** END OF FILE *********************************/

