/******************** (C) COPYRIGHT 2012 e-Design Co.,Ltd. *********************
 File Name : HDW_Config.c
 Version   : DS201 Ver 3.0                                       Author : bure
*******************************************************************************/
#include "string.h"
#include "HDW_Config.h"
#include "Process.h"
#include "Menu.h"
#include "DFU_Func.h"
#include "USB_bot.h"
#include "USB_desc.h"
//#include "msd.h"
#include "Ext_Flash.h"
/*******************************************************************************
* Set_System: System clock initializtion
*******************************************************************************/
void Set_System(void)
{
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  if(RCC_WaitForHSEStartUp() == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);    //Flash 2 wait state
    RCC_HCLKConfig(RCC_SYSCLK_Div1);      //HCLK = SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);       //PCLK2 = HCLK
    RCC_PCLK1Config(RCC_HCLK_Div2);       //PCLK1 = HCLK/2
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);     //ADCCLK = PCLK2/4 = 18MHz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //PLLCLK = 72 MHz
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource()!=0x08){}
  }
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  RCC_APB2PeriphClockCmd(
    RCC_APB2Periph_ADC1  | RCC_APB2Periph_ADC2  | RCC_APB2Periph_GPIOA |
    RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
    RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO  | RCC_APB2Periph_TIM1, ENABLE);

  RCC_APB1PeriphClockCmd(
    RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 |
    RCC_APB1Periph_SPI2, ENABLE);

  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);     //USBCLK = 48MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);  //Enable USB clock

}
/*******************************************************************************
* NVIC_Configuration:  interrupt seting  initializtion
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN_TX_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // enabling TIM1 interrupt
  NVIC_InitStructure.NVIC_IRQChannel=TIM1_CC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // enabling TIM3 interrupt
  NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // enabling DMA interrupt
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* GPIO_Config: GPIO port initializtion
*******************************************************************************/
void GPIO_Config(void)
{
  GPIOA_CRL = 0x88888080;  /* GPIOA Bit0-7 状态设置
                |||||||+----- Nib0  Ain   模拟输入
                ||||||+------ Nib1  NC    上拉输入
                |||||+------- Nib2  Vbat  模拟输入
                ||||+-------- Nib3  NC    上拉输入
                |||+--------- Nib4  KP    上拉输入
                ||+---------- Nib5  KL    上拉输入
                |+----------- Nib6  KU    上拉输入
                +------------ Nib7  KR    上拉输入*/
  GPIOA_CRH = 0x8884488B;  /* GPIOA Bit8-15 状态设置
                |||||||+----- Nib8  MCO   复用输出
                ||||||+------ Nib9  NC    上拉输入
                |||||+------- Nib10 NC    上拉输入
                ||||+-------- Nib11 D-    高阻输入
                |||+--------- Nib12 D+    高阻输入
                ||+---------- Nib13 TMS   上拉输入
                |+----------- Nib14 TCK   上拉输入
                +------------ Nib15 TDI   上拉输入*/
  GPIOA_ODR = 0x0FFFF;

  GPIOB_CRL = 0x88888411;  /* GPIOB Bit0-7 状态设置
                |||||||+----- Nib0  A     低速输出
                ||||||+------ Nib1  D     低速输出
                |||||+------- Nib2  BT1   高阻输入
                ||||+-------- Nib3  TDO   上拉输入
                |||+--------- Nib4  TRST  上拉输入
                ||+---------- Nib5  NC    上拉输入
                |+----------- Nib6  NC    上拉输入
                +------------ Nib7  NC    上拉输入*/
  GPIOB_CRH = 0xBBB1B488;  /* GPIOB Bit8-15 状态设置
                |||||||+----- Nib8  NC    上拉输入
                ||||||+------ Nib9  NC    上拉输入
                |||||+------- Nib10 Vusb  高阻输入
                ||||+-------- Nib11 V0    复用输出
                |||+--------- Nib12 SDCS  低速输出
                ||+---------- Nib13 SCK   复用输出
                |+----------- Nib14 MISO  复用输出
                +------------ Nib15 MOSI  复用输出*/
  GPIOB_ODR = 0x0FFFF;

  GPIOC_CRL = 0x84118881;  /* GPIOB Bit0-7 状态设置
                |||||||+----- Nib0  nCLS  低速输出
                ||||||+------ Nib1  NC    上拉输入
                |||||+------- Nib2  NC    上拉输入
                ||||+-------- Nib3  NC    上拉输入
                |||+--------- Nib4  C     低速输出
                ||+---------- Nib5  B     低速输出
                |+----------- Nib6  NC    高阻输入
                +------------ Nib7  NC    上拉输入*/
  GPIOC_CRH = 0x88888884;  /* GPIOB Bit8-15 状态设置
                |||||||+----- Nib8  NC    高阻输入
                ||||||+------ Nib9  NC    上拉输入
                |||||+------- Nib10 NC    上拉输入
                ||||+-------- Nib11 NC    上拉输入
                |||+--------- Nib12 NC    上拉输入
                ||+---------- Nib13 NC    上拉输入
                |+----------- Nib14 NC    上拉输入
                +------------ Nib15 NC    上拉输入*/
  GPIOC_ODR = 0x0FFFF;

  GPIOD_CRL = 0x38338838;  /* GPIOB Bit0-7 状态设置
                |||||||+----- Nib0  NC    上拉输入
                ||||||+------ Nib1  RS    高速输出
                |||||+------- Nib2  NC    上拉输入
                ||||+-------- Nib3  NC    上拉输入
                |||+--------- Nib4  nRD   高速输出
                ||+---------- Nib5  nWR   高速输出
                |+----------- Nib6  NC    上拉输入
                +------------ Nib7  nCS   高速输出*/
  GPIOD_CRH = 0x888B8884;  /* GPIOB Bit8-15 状态设置
                |||||||+----- Nib8  NC    上拉输入
                ||||||+------ Nib9  KD    上拉输入
                |||||+------- Nib10 SDDT  上拉输入
                ||||+-------- Nib11 KM    上拉输入
                |||+--------- Nib12 Fout  复用输出
                ||+---------- Nib13 NC    上拉输入
                |+----------- Nib14 NC    上拉输入
                +------------ Nib15 NC    上拉输入*/
  GPIOD_ODR = 0x0FF7F;

  GPIOE_CRL = 0x33333333;   //Bit0-7  高速输出（LCD_DB0-7 ）
  GPIOE_CRH = 0x33333333;   //Bit8-15 高速输出（LCD_DB8-15）

  AFIO_MAPR = 0x00001200;  /* AFIO重映射寄存器 Bit0-31 状态设置
                ||||||++----- 配置为复位值
                |||||+------- Bit9   TIM2_CH3_CH4 重映射至 PB10,PB11
                ||||+-------- Bit12  TIM4_CH1     重映射至 PD12
                ++++--------- 配置为复位值 */
}

/*******************************************************************************
* 函数名 : Timer_configuration    说明 : 系统定时器初始化配置
*******************************************************************************/
void Tim_Config(void)
{
//---------------TIM1用于定时触发ADC采样-------------------
  TIM1_PSC = 72-1;
  TIM1_ARR = 1000-1;
  TIM1_CCR1= 500;
  TIM1_CR1 = 0x0094;/*0000 0000 1001 0100
                      |||| |||| |||| |||+---CEN=0，禁止计数器
                      |||| |||| |||| ||+----UDIS=0，允许UEV更新
                      |||| |||| |||| |+-----URS=1：只有计数器溢出产生中断或DMA请求
                      |||| |||| |||| +------OPM=0：在发生更新事件时，计数器不停止
                      |||| |||| |||+--------DIR=1，计数器向下计数
                      |||| |||| |++---------CMS=00，选择边沿对齐模式
                      |||| |||| +-----------ARPE=1，TIM1_ARR寄存器被装入缓冲器
                      |||| ||++-------------CKD=00,CK_INT分频比为1
                      ++++-++---------------保留*/
  TIM1_RCR = 0x0000;/*0000 0000 0000 0001
                      |||| |||| ++++ ++++---在边沿对齐模式下，PWM周期的数目+1
                      ++++-++++-------------保留*/
  TIM1_CCER =0x0001;/*0000 0000 0000 0001
                      |||| |||| |||| |||+---CC1E=1，OC1信号输出到对应的输出引脚
                      |||| |||| |||| ||+----CC1P=0，OC1高电平有效
                      |||| |||| |||| |+-----CC1NE=0，OC1N禁止输出
                      |||| |||| |||| +------CC1NP=0，OC1N高电平有效
                      ++++-++++-++++--------CC2、CC3、CC4配置为复位值*/
  TIM1_CCMR1=0x0078;/*0000 0000 0111 1100
                      |||| |||| |||| ||++---CC1S=00，CC1通道被配置为输出
                      |||| |||| |||| |+-----OC1FE=1，允输出比较1 快速使能？
                      |||| |||| |||| +------OC1PE=1，输出比较1预装载使能
                      |||| |||| |+++--------0C1M=111，PWM模式2
                      |||| |||| +-----------OC1CE=0，OC1REF 不受ETRF输入的影响
                      ++++-++++-------------CC2通道配置为复位值*/
  TIM1_BDTR =0x8000;/*1000 0000 0000 0000
                      |+++-++++-++++-++++---其它位保持不变
                      +---------------------MOE=0，主输出使能*/
  TIM1_DIER =0x4200;/*0100 0011 0000 0000  DMA和中断使能寄存器
                       |     ||        +----CC1IE=0：不允许捕获/比较1中断
                       |     |+-------------UDE=1：允许更新的DMA请求
                       |     +--------------CC1DE=1：允许捕获/比较1的DMA请求
                       +--------------------TDE=1：允许触发DMA请求*/
  TIM1_CR1 |=0x0001;//CEN=1，使能TIMER1计数器

//-----------------TIM2_CH4用于PWM输出Y通道偏移电压值-------------------
  TIM2_PSC = 0;
  TIM2_ARR = 4096;              //72MHz/3840=17.58KHz
  TIM2_CCR4= 1024;
  TIM2_CR1 = 0x0084;/*0000 0000 1000 0100
                      |||| |||| |||| |||+---CEN=0，禁止计数器
                      |||| |||| |||| ||+----UDIS=0，允许UEV更新
                      |||| |||| |||| |+-----URS=1：只有计数器溢出产生中断或DMA请求
                      |||| |||| |||| +------OPM=0：在发生更新事件时，计数器不停止
                      |||| |||| |||+--------DIR=0，计数器向上计数
                      |||| |||| |++---------CMS=00，选择边沿对齐模式
                      |||| |||| +-----------ARPE=1，TIM2_ARR寄存器被装入缓冲器
                      |||| ||++-------------CKD=00,CK_INT分频比为1
                      ++++-++---------------保留*/
  TIM2_CCER =0x3000;/*0011 0000 0000 0000
                      |||| ++++-++++-++++---CC1、CC2、CC3配置为复位值
                      |||+------------------CC4E=1，OC4信号输出到对应的输出引脚
                      ||+-------------------CC4P=1，OC4低电平有效
                      |+--------------------CC4NE=0，OC4N禁止输出
                      +---------------------CC4NP=0，OC4N高电平有效*/
  TIM2_CCMR2=0x7C00;/*0111 1100 0000 0000
                      |||| |||| ++++-++++---CC3通道配置为复位值
                      |||| ||++-------------CC4S=00，CC4通道被配置为输出
                      |||| |+---------------OC4FE=1，允输出比较4 快速使能？
                      |||| +----------------OC4PE=1，输出比较4预装载使能
                      |+++------------------0C4M=111，PWM模式2
                      +---------------------OC4CE=0，OC4REF 不受ETRF输入的影响*/
  TIM2_CR1 |=0x0001;//CEN=1，使能TIMER2计数器

//-----------------TIM3用于系统定时-------------------
  TIM3_PSC  =9;  //9
  TIM3_ARR  =7199;  //1mS 脉冲周期=（PSC+1）×（ARR+1）/（72MHz/2）（uS）
  TIM3_CCR1 =3600; //脉冲宽度（占空比）50%
  TIM3_CR1 = 0x0084;/*0000 0000 1000 0100
                      |||| |||| |||| |||+---CEN=0，禁止计数器
                      |||| |||| |||| ||+----UDIS=0，允许UEV更新
                      |||| |||| |||| |+-----URS=1：只有计数器溢出产生中断或DMA请求
                      |||| |||| |||| +------OPM=0：在发生更新事件时，计数器不停止
                      |||| |||| |||+--------DIR=0，计数器向上计数
                      |||| |||| |++---------CMS=00，选择边沿对齐模式
                      |||| |||| +-----------ARPE=1，TIM3_ARR寄存器被装入缓冲器
                      |||| ||++-------------CKD=00,CK_INT分频比为1
                      ++++-++---------------保留*/
  TIM3_DIER =0x0002;/*0000 0000 0000 0010  DMA和中断使能寄存器
                             |         +----CC1IE=1：允许捕获/比较1中断
                             +--------------CC1DE=1：允许捕获/比较1的DMA请求*/
  TIM3_CR1 |=0x0001;//CEN=1，使能TIMER3计数器

//------------------TIM4_CH1用于输出选定频率的标准方波----------------------
  TIM4_PSC = 7;
  TIM4_ARR  =  FARR[List[FREQ_OUT].Val];
  TIM4_CCR1 = (FARR[List[FREQ_OUT].Val]+1)/2;
  TIM4_CR1 = 0x0084;/*0000 0000 1000 0100
                      |||| |||| |||| |||+---CEN=0，禁止计数器
                      |||| |||| |||| ||+----UDIS=0，允许UEV更新
                      |||| |||| |||| |+-----URS=1：只有计数器溢出产生中断或DMA请求
                      |||| |||| |||| +------OPM=0：在发生更新事件时，计数器不停止
                      |||| |||| |||+--------DIR=0，计数器向上计数
                      |||| |||| |++---------CMS=00，选择边沿对齐模式
                      |||| |||| +-----------ARPE=1，TIM3_ARR寄存器被装入缓冲器
                      |||| ||++-------------CKD=00,CK_INT分频比为1
                      ++++-++---------------保留*/
  TIM4_CCER =0x0001;/*0000 0000 0000 0001
                      |||| |||| |||| |||+---CC1E=1，OC1信号输出到对应的输出引脚
                      |||| |||| |||| ||+----CC1P=0，OC1高电平有效
                      |||| |||| |||| |+-----CC1NE=0，OC1N禁止输出
                      |||| |||| |||| +------CC1NP=0，OC1N高电平有效
                      ++++-++++-++++--------CC2、CC3、CC4配置为复位值*/
  TIM4_CCMR1=0x0078;/*0000 0000 0111 1100
                      |||| |||| |||| ||++---CC1S=00，CC1通道被配置为输出
                      |||| |||| |||| |+-----OC1FE=1，允输出比较1 快速使能？
                      |||| |||| |||| +------OC1PE=1，输出比较1预装载使能
                      |||| |||| |+++--------0C1M=111，PWM模式2
                      |||| |||| +-----------OC1CE=0，OC1REF 不受ETRF输入的影响
                      ++++-++++-------------CC2通道配置为复位值*/
  TIM4_CR1 |=0x0001;//CEN=1，使能TIMER4计数器
}
/*******************************************************************************
* SPI_Config: SPI  port initializtion
*******************************************************************************/
void SPI_Config(void)
{
  SPI_InitTypeDef   SPI_InitStructure;

  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);  // SPI2 Config
  SPI_Cmd(SPI2, ENABLE);               // SPI2 enable

}
/*******************************************************************************
* DMA_configuration: DMA channel initializtion
*******************************************************************************/
void DMA_Configuration(void)
{
  DMA_ISR = 0x00000000;/*DMA中断状态寄存器，可通过设置IFCR对应位清除
              |||||||+---GIF1 =0：通道1没产生TE、HT或TC事件全局中断标志
              |||||||+---TCIF1=0：通道1没产生传输完成中断标志
              |||||||+---HTIF1=0：通道1没产生半传输中断标志
              |||||||+---TEIF1=0：通道1没产生传输错误中断标志
              ||||||+----通道2中断标志组
              ||||++-----通道3中断标志组
              ||||++-----通道4中断标志组
              |||+-------通道5中断标志组
              ||+--------通道6中断标志组
              ||+--------通道7中断标志组
              +----------保留*/
  DMA_IFCR= 0x00000000;/*DMA中断标志清除寄存器，可由软件设置或清除
              |||||||+---CGIF1 =1：清除通道1的TE、HT或TC事件全局中断标志
              |||||||+---CTCIF1=1：清除通道1的传输完成中断标志
              |||||||+---CHTIF1=1：清除通道1的半传输中断标志
              |||||||+---CTEIF1=1：清除通道1的传输错误中断标志
              ||||||+----清除通道2中断标志组
              ||||++-----清除通道3中断标志组
              ||||++-----清除通道4中断标志组
              |||+-------清除通道5中断标志组
              ||+--------清除通道6中断标志组
              ||+--------清除通道7中断标志组
              +----------保留*/
  DMA1_CCR1=  0x00003580;/*0011 0101 1000 0000
               ||||       |||| |||| |||| |||+--EN = 0：通道关闭，1：通道开启
               ||||       |||| |||| |||| ||+---TCIE=0：不允许传输完成中断
               ||||       |||| |||| |||| |+----HTIE=0：禁止半传输中断
               ||||       |||| |||| |||| +-----TEIE=0：禁止传输错误中断
               ||||       |||| |||| |||+-------DIR =0：数据传输方向（从外设读）
               ||||       |||| |||| ||+--------CIRC=0：不执行循环操作
               ||||       |||| |||| |+---------PINC=0：不执行外设地址增量模式
               ||||       |||| |||| +----------MINC=1：执行存储器地址增量模式
               ||||       |||| ||++------------PSIZE=01：16位外设数据宽度
               ||||       |||| ++--------------MSIZE=01：16位存储器数据宽度
               ||||       ||++-----------------PL=11：最高通道优先级
               ||||       |+-------------------MEM2MEM=0：非存储器到存储器模式
               ++++-------+--------------------保留*/

  DMA1_CNDTR1 = 0x00001000;          // DMA通道1传输数量寄存器(1024*2 Bytes)
  DMA1_CPAR1  = ADC1_DR_ADDR;        // DMA通道1的外设数据寄存器基地址
  DMA1_CMAR1  = (u32)&Sampl[1];      // DMA通道1的源或目标的存储器地址
  DMA1_CCR1  |= 0x00000001;          // EN = 1：DMA通道1开启
}
/*******************************************************************************
* ADC_configuration: ADC initializtion
*******************************************************************************/
void ADC_Configuration(void)
{
  ADC2_CR1 =0x00000000;
  ADC1_CR1 =0x00000000;/*
              ||||||++---AWDCH[4:0]：模拟看门狗通道选择位
              ||||||+----=000：不允许EOC、AWD、JEOC中断
              |||||+-----Nib8_SCAN=0：不使用扫描模式
              ||||++-----保持复位值
              |||+-------DUALMOD=0：独立模式
              ||+--------规则通道和注入通道禁用模拟看门狗
              ++---------保留*/
  ADC2_CR2 =0x00100000;
  ADC1_CR2 =0x00100100;/*
              |||||||+---ADON=0：关闭ADC转换/校准，=1：开启ADC并启动转换
              |||||||+---CONT=0：1：连续转换模式
              |||||||+---CAL=0，=1：A/D校开始准，校准后该位将被硬件清除
              |||||||+---RSTCAL=0，=1：初始化校准寄存器，校准后该位将被硬件清除
              ||||||+----保留
              |||||+-----ALIGN=0 & DMA=1：数据右对齐和使用DMA模式
              ||||+------
              |||+-------=0000：规则通道使用定时器1的CC1事件启动转换，=1110：SWSTART
              ||+--------=1：使用外部触发信号启动转换
              ++---------保留*/
  ADC2_SQR1=0x00000000;
  ADC1_SQR1=0x00000000;/*ADC规则序列寄存器 1
              ||++++++---SQ13-16=00000：转换序列中第13-16个转换通道编号(5Bits*4)
              ++---------保留*/
  ADC2_SQR3 =0x00000002;
  ADC1_SQR3 =0x00000000;/*ADC规则序列寄存器 3
               ||||||++---SQ1=00000：转换序列中的第1个转换通道的编号(0)
               +++++++----SQ2-6=00000：转换序列中第2-6个转换通道编号(5Bits*5)
               +----------保留*/
  ADC2_SMPR2=0x00000000;
  ADC1_SMPR2=0x00000000;/*ADC采样时间寄存器 1
               |||||||+---SMP01=001(3Bits)：使通道00的采样时间 1.5T
               ||||+------SMP04=001(3Bits)：使通道04的采样时间 7.5T
               |+++++++---SMP11-17=000(3Bits*7)：通道11-17的采样时间 1.5T
               ++---------保留*/
  ADC1_CR2 |=0x00000001;
  ADC2_CR2 |=0x00000001;       //ADON=1：开启ADC1、ADC2
  ADC1_CR2 |=0x00000008;
  while(ADC1_CR2 & 0x00000008);//初始化ADC1校准寄存器
  ADC2_CR2 |=0x00000008;
  while(ADC2_CR2 & 0x00000008);//初始化ADC2校准寄存器
  ADC1_CR2 |=0x00000004;
  while(ADC1_CR2 & 0x00000004);//ADC1校准
  ADC2_CR2 |=0x00000004;
  while(ADC2_CR2 & 0x00000004);//ADC2校准
}


/******************************** END OF FILE *********************************/
