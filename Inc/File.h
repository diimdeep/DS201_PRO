/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : file.h  
 Version   : DS203_APP Ver 2.5x                                  Author : bure
*******************************************************************************/
#ifndef __FILE_H
#define __FILE_H

#include "stm32f10x_lib.h"

#define OK              0
#define VER_ERR         1
#define NO_FILE         2
#define FILE_RW_ERR     3
#define DISK_RW_ERR     4

extern vu8 DFS_FUNC;
extern unsigned char  F_Sector[];

u8 Save_Bmp(s16 FileNum);
u8 Color_Num(u16 Color);
void Read_Parameter(void);
char Save_Parameter(void);
u8 Save_Dat(s16 FileNum);
u8 SD_Save_Dat(s16 FileNo);
u8 Load_Dat(s16 FileNum);
u8 Save_Buf(s16 FileNum);
u8 Load_Buf(s16 FileNum);
u8 Save_Csv(s16 FileNum);
void Make_Filename(s16 FileNum,u8* FileName);
void reset_parameter(void);
void Restore_CalPar(void);
void Restore_Scene(void);

int DFS_PROC(u8 func); 
void SVC_IRST(void);
int SD_OpenFile(u8* pFileName);
int FileProc(u8 index,u8 filenum);
#endif
/********************************* END OF FILE ********************************/
