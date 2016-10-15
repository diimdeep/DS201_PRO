/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Function.c
 Version   : DS203 APP Ver 2.5x                          Author : bure & Kewei
*******************************************************************************/
#include "Version.h"
#include "Func.h"
#include "Bios.h"

u32 TestCnt = 0;

/*******************************************************************************
  计算 x 的 y 次方
*******************************************************************************/
u32 Power(u8 x, u8 y)
{
  u32 m = x;

  if(y == 0) return 1;
  while (--y) m *= x;
  return m;
}
/*******************************************************************************
  计算 10 的 x 次方
*******************************************************************************/
//u32 Exp(u8 x)  V5.02
u_64 Exp(u8 x)
{
  //u32 m = 1;
  u_64 m = 1;

  while(x--) m *= 10;
  return m;
}
/*******************************************************************************
  在数据区中查找第 Idx 个字符串的起始地址
*******************************************************************************/
u8* SeekStr(u8* ptr, u8 Idx)
{
  while(Idx--) while(*ptr++);
  return ptr;
}
/*******************************************************************************
 Value2Str: 32位数转e位有效数字符串 + 量纲字符串（结构为Unit[][6]）+  模式
*******************************************************************************/
//void Value2Str(u8 *p, s32 n, uc8 *pUnit, s8 e, u8 Mode)  V5.02
void Value2Str(u8 *p, s_64 n, uc8 *pUnit, s8 e, u8 Mode)
{
  s16 i = 0;
  //s32 m = n, c = 5;
  s_64 m = n, c = 5;

  if(Mode == SIGN){
    if(n == 0) *p++ = ' ';
    if(n >  0) *p++ = '+';
    if(n <  0){*p++ = '-'; n = -n;}
  }
  while(m >= 10){m /= 10; i++;} // 计算 n 的有效位数 i
  if((i%3 == 2)&&(e == 2)) e++;
  m = n; i = 0;
  while(m >= 10){
    m /= 10;
    if(++i > e) c *= 10;        // n 的有效位数 i 大于e则计算四舍五入值
  }
  if(i >= e) n += c;            // n 加上四舍五入值
  m = n; i = 0;
  while(m >= 10){m /= 10; i++;} // 重新计算 n 的有效位数 i

  m = i/3;                      // 计算量纲单位取值偏移量
  while(e--){
    *p++ = '0'+ n/Exp(i);
    if(e &&(i%3 == 0)) *p++ = '.';
    n = (i < 0)? 0 : n%Exp(i);
    i--;
  }
  pUnit += 6*m;                 //
  do {*p++ = *pUnit;}
  while(*pUnit++);              // 加上量纲字符字符串
}

/*******************************************************************************
 Two ASCII character Change to 1 Byte HEX data
*******************************************************************************/
u8 Str2Byte(u8 x,u8 y) // 双ASCII字符转1字节二进制数
{
  uc8 Hexcode[17]="0123456789ABCDEF";
  u8 i, Temp=0;

  if(x>='a' && x<='z')  x-=32;     // 小写改大写
  if(y>='a' && y<='z')  y-=32;     // 小写改大写
  for(i=0;i<16;i++){
    if(Hexcode[i]==x)  Temp+=i*16; // 将字符转为高4位十六进制数值
  }
  for(i=0;i<16;i++){
    if(Hexcode[i]==y)  Temp+=i;    // 将字符转为低4位十六进制数值
  }
  return Temp;
}

/*******************************************************************************
 u16ToDec4Str: 无符号16位二进制数转4位十进制字符串，有效数字前填空格
*******************************************************************************/
void u16ToDec4Str(u8 *p, u16 n)
{
  if(n/10000){
    *p++ = 'O';
    *p++ = 'v';
    *p++ = 'e';
    *p++ = 'r';
    *p   = 0;
    return;
  }
  *p++ = '0'+n/1000;
  n %= 1000;
  *p++ = '0'+n/100;
  n %= 100;
  *p++ = '0'+n/10;
  n %= 10;
  *p++ = '0'+n;
  *p = 0;
  if(p[-4] == '0'){
    p[-4] = ' ';
    if(p[-3] == '0'){
      p[-3] = ' ';
      if(p[-2] == '0') p[-2] = ' ';
    }
  }
}
/*******************************************************************************
 u16ToDec5Str: 无符号16位二进制数转5位十进制字符串
*******************************************************************************/
void u16ToDec5Str(u8 *p, u16 n)
{
  *p++ = '0'+n/10000;
  n %= 10000;
  *p++ = '0'+n/1000;
  n %= 1000;
  *p++ = '0'+n/100;
  n %= 100;
  *p++ = '0'+n/10;
  n %= 10;
  *p++ = '0'+n;
  *p = 0;
}
void s16ToDec5(u8 *p, s16 n)
{
  if(n >= 0) *p++ = '+';
  else      {*p++ = '-'; n = -n;}
  u16ToDec5Str(p, n);
}
/*******************************************************************************
 u8ToDec3: Change Byte to 3 decimal number string
*******************************************************************************/
void u8ToDec3(u8 *p, u8 n)
{
    *p++ = '0'+n/100;
    n %= 100;
    *p++ = '0'+n/10;
    n %= 10;
    *p++ = '0'+n;
    *p = 0;
}
/*******************************************************************************
 s8ToPercen: Change sign char to +(-)x.xx string
*******************************************************************************/
void s8ToPercen(u8 *p, s8 n)
{
    if(n >= 0)  *p++ = '+';
    else {
      *p++ = '-';
      n = -n;
    }
    *p++ = '0'+n/100;
    n %= 100;
    *p++ = '.';
    *p++ = '0'+n/10;
    n %= 10;
    *p++ = '0'+n;
    *p = 0;
}
/*******************************************************************************
 u8ToDec2: Change Byte to 2 decimal number string
*******************************************************************************/
void u8ToDec2(u8 *p, u8 n)
{
//    *p++ = '0'+n/100;
    n %= 100;
    *p++ = '0'+n/10;
    n %= 10;
    *p++ = '0'+n;
    *p = 0;
}
/*******************************************************************************
 Char2Hex: Change Byte to 2 hex number string
*******************************************************************************/
void Char2Hex(u8 *p, u8 n)
{
    if(n/16>9) *p++ = 'A'+(n/16-10);
    else       *p++ = '0'+n/16;
    n %= 16;
    if(n>9)    *p++ = 'A'+(n-10);
    else       *p++ = '0'+n;
    *p = 0;
}
/*******************************************************************************
 Shor2Hex: Change 2Bytes to 4 hex number string
*******************************************************************************/
void Shor2Hex(u8 *p, u16 n)
{
    if(n/0x1000 >9) *p++ = 'A'+(n/0x1000-10);
    else            *p++ = '0'+ n/0x1000;
    n %= 0x1000;
    if(n/0x100 >9)  *p++ = 'A'+(n/0x100-10);
    else            *p++ = '0'+ n/0x100;
    n %= 0x100;
    if(n/0x10 >9)   *p++ = 'A'+(n/0x10-10);
    else            *p++ = '0'+ n/0x10;
    n %= 0x10;
    if(n >9)        *p++ = 'A'+(n-10);
    else            *p++ = '0'+n;
    *p = 0;
}
/*******************************************************************************
 Word2Hex: Change 4 Bytes to 8 hex number string
*******************************************************************************/
void Word2Hex(u8 *p, u32 n)
{
  s8 i, k;

  for(i=28; i>=0; i-=4){
    k = ((n >> i)& 0xF);
    if(k > 9) *p++ = 'A'+ k-10;
    else      *p++ = '0'+ k;
  }
  *p = 0;

}
/*******************************************************************************
 Int_sqrt: unsigned int square root
*******************************************************************************/
u16 Sqrt32(u32 n)
{ u32 k;
  if ( n == 0 ) return 0;
  k = 2*Sqrt32(n/4)+1;
  if ( k*k > n ) return k-1;
  else return k;
}

/*******************************************************************************
 delayms: 毫秒（mS）延时程序。 输入: 延时等待的毫秒数值（在72MHz主频情况下）
*******************************************************************************/
void Delayms(u32 mS)
{
  Delay_Cnt = mS;
  while (Delay_Cnt > 0){}
}
/********************************* END OF FILE ********************************/
