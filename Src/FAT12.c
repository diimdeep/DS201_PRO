/********************* (C) COPYRIGHT 2011 e-Design Co.,Ltd. ********************
 File Name : FAT12.c  
 Version   : DS203_SYS Ver 1.5x                                  Author : bure
*******************************************************************************/
#include <string.h>
#include "FAT12.h"
#include "BIOS.h"
//#include "ASM.h"
#include "Func.h"
#include "Ext_Flash.h"
#include "DFU_Func.h"

typedef struct
{
  u32 FAT1_BASE;          // FAT1����ʼ��ַ
  u32 FAT2_BASE;          // FAT2����ʼ��ַ
  u32 ROOT_BASE;          // ��Ŀ¼��ʼ��ַ
  u32 FILE_BASE;          // �ļ�����ʼ��ַ 
  u32 FAT_LEN;
  u32 SEC_LEN;            // �������� 
  u32 FAT_END;            // ���ӽ���
  u8  FAT1_SEC;           // FAT1������
  u8  FAT2_SEC;
}FAT_InitTypeDef;
//-------FLASH-----W25Q64BV--------------------------------------------------//
#define FILE_BASE_8M    0x7000     // �ļ�����ʼ��ַ  
#define ROOT_BASE_8M    0x3000     // ��Ŀ¼��ʼ��ַ
#define FAT_LEN_8M      0x1000 
#define FAT1_BASE_8M    0x1000   // FAT1����ʼ��ַ
#define FAT2_BASE_8M    0x2000     // FAT2����ʼ��ַ
#define SEC_LEN_8M      0x1000     // �������� 
#define FAT1_SEC_8M     0x1         // FAT1������
#define FAT2_SEC_8M     0x1         // FAT2������
#define FAT_END_8M      0x7FF        // ���ӽ���
//----------------------------------------------------------------------------//
#define FILE_BASE_2M    0x8000   // �ļ�����ʼ��ַ  
#define ROOT_BASE_2M    0x4000     // ��Ŀ¼��ʼ��ַ
#define FAT_LEN_2M      0x1800 
#define FAT1_BASE_2M    0x1000     // FAT1����ʼ��ַ
#define FAT2_BASE_2M    0x2800   // FAT2����ʼ��ַ
#define SEC_LEN_2M      0x200        // �������� 
#define FAT1_SEC_2M     0x0C         // FAT1������
#define FAT2_SEC_2M     0x0C         // FAT2������
#define FAT_END_2M      0xFFF        // ���ӽ���
//----------------------------------------------------------------------------//
#define OK           0            // �������
#define SEC_ERR      1            // ������д����
#define FAT_ERR      2            // FAT���д����
#define OVER         3            // �������
#define NEW          4            // ��Ŀ¼��

#define OW           0            // ��д(���ݴ�0��1��д)
#define RW           1            // ��д

//#define DISK          7
uc8 DiskDevInfo_8M[]={"8MB Internal"};
uc8 McuType[]={"STM32F103VC"};
extern u8 Clash;
FAT_InitTypeDef FAT_V;
int Init_Fat_Value(void)
{

 u8* ptr;


  ptr=__DevInfoStr(MCU);
  if(memcmp(McuType,ptr,11))return 1;
    ptr=__DevInfoStr(DISK);
    if(memcmp((u8*)ptr,DiskDevInfo_8M,3)==0)flash_mode=FLASH_8M;
    else flash_mode=FLASH_2M;

  if(flash_mode==FLASH_8M){
    FAT_V.FAT1_BASE=FAT1_BASE_8M; 
    FAT_V.FAT2_BASE=FAT2_BASE_8M; 
    FAT_V.ROOT_BASE=ROOT_BASE_8M;  
    FAT_V.FILE_BASE=FILE_BASE_8M;      
    FAT_V.FAT1_SEC=FAT1_SEC_8M;
    FAT_V.FAT2_SEC=FAT1_SEC_8M;
    FAT_V.SEC_LEN = SEC_LEN_8M;    
    FAT_V.FAT_LEN = FAT_LEN_8M; 
    FAT_V.FAT_END = FAT_END_8M;
  }
  else{
    FAT_V.FAT1_BASE=FAT1_BASE_2M; 
    FAT_V.FAT2_BASE=FAT2_BASE_2M; 
    FAT_V.ROOT_BASE=ROOT_BASE_2M;  
    FAT_V.FILE_BASE=FILE_BASE_2M;      
    FAT_V.FAT1_SEC=FAT1_SEC_2M;
    FAT_V.FAT2_SEC=FAT1_SEC_2M;
    FAT_V.SEC_LEN = SEC_LEN_2M;    
    FAT_V.FAT_LEN = FAT_LEN_2M; 
    FAT_V.FAT_END = FAT_END_2M;
  }
  return 0;
}
/*******************************************************************************
 ������ҳ��(256 Bytes)  ����USB��д��ͻ���ض�
*******************************************************************************/
u8 ReadDiskData(u8* pBuffer, u32 ReadAddr, u16 Lenght)
{
  u8 n = 0;
  
  while(1){
    Clash = 0;
    ExtFlash_PageRD(pBuffer, ReadAddr, Lenght);
    if(n++ > 6) return SEC_ERR;     // ��ʱ������
    if(Clash == 0) return OK;       // �޳�ͻ�����򷵻�
  }
}
/*******************************************************************************
 д����ҳ��(256 Bytes)  ����USB��д��ͻ����д
*******************************************************************************/
u8 ProgDiskPage(u8* pBuffer, u32 ProgAddr)
{                         
  u8   n = 0; 
  
  while(1){
    Clash = 0;
    ExtFlash_PageWR(pBuffer, ProgAddr);
    if(n++ > 6) return SEC_ERR;     // ��ʱ������
    if(Clash == 0) return OK;       // �޳�ͻ�����򷵻�
  }
} 
/*******************************************************************************
 ������һ�����Ӵغź󷵻أ���ǰ�غű�����ָ��+1��λ��
*******************************************************************************/
u8 NextCluster(u16* pCluster)
{
  u16 FatNum;
  u32 Addr ;
  
  Addr=FAT_V.FAT1_BASE +(*pCluster + *pCluster/2);
  
  *(pCluster+1)= *pCluster;                                   // ����ǰһ���غ�
  *pCluster = 0;
  if((*(pCluster+1) >=FAT_V.FAT_END)||(*(pCluster+1)< 2)) return SEC_ERR;
  if(ReadDiskData((u8*)&FatNum, Addr, 2)!= OK) return SEC_ERR;
  *pCluster= (*(pCluster+1) & 1)?(FatNum >>4):(FatNum & 0xFFF); // ָ����һ���غ�
  return OK; 
}
/*******************************************************************************
 ���ļ�����(512 Bytes), ����ʱָ��ָ����һ���غţ���ǰ�غű�����ָ��+1��λ��
*******************************************************************************/
u8 ReadFileSec(u8* pBuffer, u16* pCluster)
{
  u32 ReadAddr =FAT_V.FILE_BASE + FAT_V.SEC_LEN*(*pCluster-2);

  if(ReadDiskData(pBuffer, ReadAddr, FAT_V.SEC_LEN)!=OK) return SEC_ERR;  //(SEC_LEN=4096/512)
  if(NextCluster(pCluster)!=0) return FAT_ERR;                 // ȡ��һ���غ�
  return OK;
} 
/*******************************************************************************
 д�ļ�����(512/4096 Bytes)����д��ǰFAT�����ز��ҵ�����һ���غ�
*******************************************************************************/
u8 ProgFileSec(u8* pBuffer, u16* pCluster)
{
  u16 Tmp;
  u32 ProgAddr = FAT_V.FILE_BASE + FAT_V.SEC_LEN*(*pCluster-2);
  if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // д���� 4096 Byte �� дǰ������
  if(flash_mode==FLASH_2M){
    pBuffer += 256;  ProgAddr += 256;
    if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // д�������
  }
  
  if(NextCluster(pCluster)!=0) return FAT_ERR;                 // ȡ��һ���غ�
  Tmp = *(pCluster+1);
  if(*pCluster == 0){
    *pCluster = Tmp;
    if(SeekBlank (pBuffer, pCluster )!= OK) return OVER;
    if(SetCluster(pBuffer, pCluster )!= OK) return SEC_ERR;
  }
  return OK;
}
/*******************************************************************************
 ���ҿ��дغţ�����ʱָ��ָ����һ�����дغţ���ǰ�غű�����ָ��+1��λ��
*******************************************************************************/
u8 SeekBlank(u8* pBuffer, u16* pCluster)
{
  u16  Tmp;
  u8   Buffer[2];
  u8   Tmp_Flag = 1;

  *(pCluster+1)= *pCluster;                                    // ���浱ǰ�غ�
  

  //-----------------------
  for(*pCluster=0; (*pCluster)<4095; (*pCluster)++){
    if(ReadDiskData(Buffer, FAT_V.FAT1_BASE +(*pCluster)+(*pCluster)/2, 2)!= 0) return SEC_ERR;
    Tmp = ((*pCluster)& 1)?((*(u16*)Buffer)>>4):((*(u16*)Buffer)& 0xFFF);
    
    if((Tmp == 0)&&(Tmp_Flag == 0)&&(((*pCluster))!= *(pCluster+1))) {
      Tmp_Flag = 1;
      return OK;
    }
    if((Tmp == 0)&&(Tmp_Flag == 1))  {
      *(pCluster+2) = *pCluster;
      Tmp_Flag = 0;
    } 
  }
  //-----------------------
  return OK;
}         
/*******************************************************************************
 ����һ�غ�д��FAT��ǰ������λ������ʱָ��ָ����һ�غţ�ָ��+1Ϊ��ǰ�غ�
*******************************************************************************/
u8 SetCluster(u8* pBuffer, u16* pCluster)
{
  u16  Offset, i, k;
  u32  SecAddr;

  i = *(pCluster+1);                    // ��ȡԭ��ǰ�غ�
  k = *pCluster;                        // ��ȡ��һ�غ�
  Offset = i+ i/2;
  SecAddr = FAT_V.FAT1_BASE +(Offset & 0xF000 );
  Offset &= 0x0FFF;
  if(ReadDiskData(pBuffer, SecAddr, FAT_V.SEC_LEN)!= 0) return SEC_ERR; //???????????? old=258
  if(i & 1){
    pBuffer[Offset  ]=(pBuffer[Offset]& 0x0F)+((k <<4)& 0xF0);
    pBuffer[Offset+1]= k >>4;
  } else {
    pBuffer[Offset  ]= k & 0xFF;
    pBuffer[Offset+1]=(pBuffer[Offset+1]& 0xF0)+((k>>8)& 0x0F);
  }
  
  if(ProgDiskPage(pBuffer, SecAddr)!= 0) return SEC_ERR;
  if(flash_mode==FLASH_2M)
    if(ProgDiskPage(pBuffer+256, SecAddr+256)!= 0) return SEC_ERR;
  return OK;
}
/*******************************************************************************
 ��ģʽ���ļ��������ļ���һ���غż�Ŀ¼���ַ�� 0�غż���һ���հ�Ŀ¼���ַ
*******************************************************************************/
u8 OpenFileRd(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr)
{
  u16 i, n;

  *pCluster = 0;
  for(*pDirAddr=FAT_V.ROOT_BASE; *pDirAddr<FAT_V.FILE_BASE; ){
    if(ReadDiskData(pBuffer, *pDirAddr,FAT_V.SEC_LEN)!= OK) return SEC_ERR;
    for(n=0; n<FAT_V.SEC_LEN; n+=32){   //?????????????????????256
      for(i=0; i<11; i++){
        if(pBuffer[n + i]!= 0){
          if(pBuffer[n + i]!= pFileName[i]) break;
          if(i == 10){                             // �ҵ��ļ���
            *pCluster = *(u16*)(pBuffer + n + 0x1A); // �ļ���һ���غ�
            return OK;         
          }
        } else return NEW;               // ������һ���հ�Ŀ¼��󷵻�
      }
      *pDirAddr += 32;
    }
  }
  return OVER;
}
/*******************************************************************************
 дģʽ���ļ��������ļ���һ���غż�Ŀ¼���ַ
*******************************************************************************/
u8 OpenFileWr(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr)
{
  u32 i, n,offset;
  
  i = OpenFileRd(pBuffer, pFileName, pCluster, pDirAddr);
  if(i != NEW) return i;
  else{                                                    // ��ǰ��Ϊ�հ�Ŀ¼��
    if(SeekBlank(pBuffer, pCluster)!= OK) return OVER;     // ��FAT��������
    if(flash_mode==FLASH_8M)
      n =*pDirAddr & 0xFFF;
    else
      n =*pDirAddr & 0xFF;                              // nΪ��ǰҳĿ¼�� OLD=0XFF
    offset=*pDirAddr-n;
//    if(__ReadDiskData(pBuffer,(*pDirAddr)-n, 4096)!= OK) return SEC_ERR;    //256
    if(ReadDiskData(pBuffer,offset, FAT_V.SEC_LEN)!= OK) return SEC_ERR; 
    for(i=0; i<11; i++) pBuffer[n+i]= pFileName[i];      // ������Ŀ¼��offset +
    *(u16*)(pBuffer + n + 0x1A)= *pCluster;

//    if(__ProgDiskPage(pBuffer,(*pDirAddr)-n)!= OK) return SEC_ERR;
    if(ProgDiskPage(pBuffer,offset)!= OK) return SEC_ERR;
    return OK;
  }
}                
/*******************************************************************************
 �ر��ļ�����������д��FAT�����ļ�����д��Ŀ¼�����FAT1��FAT2
*******************************************************************************/
u8 CloseFile(u8* pBuffer, u32 Lenght, u16* pCluster, u32* pDirAddr)
{
  u32 n;
  if(flash_mode==FLASH_8M)n=0xFFF;
  else  n=0xFF;
  *pCluster = 0xFFF;
  SetCluster(pBuffer, pCluster); 

  if(ReadDiskData(pBuffer, (*pDirAddr &(~n)), FAT_V.SEC_LEN)!= OK) return SEC_ERR;
  *(u8* )(pBuffer +(*pDirAddr & n)+ 0x0B)= 0x20;
  *(u32*)(pBuffer +(*pDirAddr & n)+ 0x1C)= Lenght;
  if(ProgDiskPage(pBuffer, (*pDirAddr &(~n)))!= OK) return SEC_ERR;
  if(flash_mode==FLASH_8M){
    if(ReadDiskData(pBuffer, FAT_V.FAT1_BASE, FAT_V.SEC_LEN)!= OK) return SEC_ERR;
    if(ProgDiskPage(pBuffer, FAT_V.FAT2_BASE     )!= OK) return SEC_ERR; 
  }
  else{
    for(n=0; n<FAT_V.FAT1_SEC; n++){
      if(ReadDiskData(pBuffer, FAT_V.FAT1_BASE+n*256, 256)!= OK) return SEC_ERR;
      if(ProgDiskPage(pBuffer, FAT_V.FAT2_BASE+n*256     )!= OK) return SEC_ERR;
    } 
  }
  return OK;
}
