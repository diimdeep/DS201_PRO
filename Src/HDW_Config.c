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
  GPIOA_CRL = 0x88888080;  /* GPIOA Bit0-7 ״̬����
                |||||||+----- Nib0  Ain   ģ������
                ||||||+------ Nib1  NC    ��������
                |||||+------- Nib2  Vbat  ģ������
                ||||+-------- Nib3  NC    ��������
                |||+--------- Nib4  KP    ��������
                ||+---------- Nib5  KL    ��������
                |+----------- Nib6  KU    ��������
                +------------ Nib7  KR    ��������*/
  GPIOA_CRH = 0x8884488B;  /* GPIOA Bit8-15 ״̬����
                |||||||+----- Nib8  MCO   �������
                ||||||+------ Nib9  NC    ��������
                |||||+------- Nib10 NC    ��������
                ||||+-------- Nib11 D-    ��������
                |||+--------- Nib12 D+    ��������
                ||+---------- Nib13 TMS   ��������
                |+----------- Nib14 TCK   ��������
                +------------ Nib15 TDI   ��������*/
  GPIOA_ODR = 0x0FFFF;

  GPIOB_CRL = 0x88888411;  /* GPIOB Bit0-7 ״̬����
                |||||||+----- Nib0  A     �������
                ||||||+------ Nib1  D     �������
                |||||+------- Nib2  BT1   ��������
                ||||+-------- Nib3  TDO   ��������
                |||+--------- Nib4  TRST  ��������
                ||+---------- Nib5  NC    ��������
                |+----------- Nib6  NC    ��������
                +------------ Nib7  NC    ��������*/
  GPIOB_CRH = 0xBBB1B488;  /* GPIOB Bit8-15 ״̬����
                |||||||+----- Nib8  NC    ��������
                ||||||+------ Nib9  NC    ��������
                |||||+------- Nib10 Vusb  ��������
                ||||+-------- Nib11 V0    �������
                |||+--------- Nib12 SDCS  �������
                ||+---------- Nib13 SCK   �������
                |+----------- Nib14 MISO  �������
                +------------ Nib15 MOSI  �������*/
  GPIOB_ODR = 0x0FFFF;

  GPIOC_CRL = 0x84118881;  /* GPIOB Bit0-7 ״̬����
                |||||||+----- Nib0  nCLS  �������
                ||||||+------ Nib1  NC    ��������
                |||||+------- Nib2  NC    ��������
                ||||+-------- Nib3  NC    ��������
                |||+--------- Nib4  C     �������
                ||+---------- Nib5  B     �������
                |+----------- Nib6  NC    ��������
                +------------ Nib7  NC    ��������*/
  GPIOC_CRH = 0x88888884;  /* GPIOB Bit8-15 ״̬����
                |||||||+----- Nib8  NC    ��������
                ||||||+------ Nib9  NC    ��������
                |||||+------- Nib10 NC    ��������
                ||||+-------- Nib11 NC    ��������
                |||+--------- Nib12 NC    ��������
                ||+---------- Nib13 NC    ��������
                |+----------- Nib14 NC    ��������
                +------------ Nib15 NC    ��������*/
  GPIOC_ODR = 0x0FFFF;

  GPIOD_CRL = 0x38338838;  /* GPIOB Bit0-7 ״̬����
                |||||||+----- Nib0  NC    ��������
                ||||||+------ Nib1  RS    �������
                |||||+------- Nib2  NC    ��������
                ||||+-------- Nib3  NC    ��������
                |||+--------- Nib4  nRD   �������
                ||+---------- Nib5  nWR   �������
                |+----------- Nib6  NC    ��������
                +------------ Nib7  nCS   �������*/
  GPIOD_CRH = 0x888B8884;  /* GPIOB Bit8-15 ״̬����
                |||||||+----- Nib8  NC    ��������
                ||||||+------ Nib9  KD    ��������
                |||||+------- Nib10 SDDT  ��������
                ||||+-------- Nib11 KM    ��������
                |||+--------- Nib12 Fout  �������
                ||+---------- Nib13 NC    ��������
                |+----------- Nib14 NC    ��������
                +------------ Nib15 NC    ��������*/
  GPIOD_ODR = 0x0FF7F;

  GPIOE_CRL = 0x33333333;   //Bit0-7  ���������LCD_DB0-7 ��
  GPIOE_CRH = 0x33333333;   //Bit8-15 ���������LCD_DB8-15��

  AFIO_MAPR = 0x00001200;  /* AFIO��ӳ��Ĵ��� Bit0-31 ״̬����
                ||||||++----- ����Ϊ��λֵ
                |||||+------- Bit9   TIM2_CH3_CH4 ��ӳ���� PB10,PB11
                ||||+-------- Bit12  TIM4_CH1     ��ӳ���� PD12
                ++++--------- ����Ϊ��λֵ */
}

/*******************************************************************************
* ������ : Timer_configuration    ˵�� : ϵͳ��ʱ����ʼ������
*******************************************************************************/
void Tim_Config(void)
{
//---------------TIM1���ڶ�ʱ����ADC����-------------------
  TIM1_PSC = 72-1;
  TIM1_ARR = 1000-1;
  TIM1_CCR1= 500;
  TIM1_CR1 = 0x0094;/*0000 0000 1001 0100
                      |||| |||| |||| |||+---CEN=0����ֹ������
                      |||| |||| |||| ||+----UDIS=0������UEV����
                      |||| |||| |||| |+-----URS=1��ֻ�м�������������жϻ�DMA����
                      |||| |||| |||| +------OPM=0���ڷ��������¼�ʱ����������ֹͣ
                      |||| |||| |||+--------DIR=1�����������¼���
                      |||| |||| |++---------CMS=00��ѡ����ض���ģʽ
                      |||| |||| +-----------ARPE=1��TIM1_ARR�Ĵ�����װ�뻺����
                      |||| ||++-------------CKD=00,CK_INT��Ƶ��Ϊ1
                      ++++-++---------------����*/
  TIM1_RCR = 0x0000;/*0000 0000 0000 0001
                      |||| |||| ++++ ++++---�ڱ��ض���ģʽ�£�PWM���ڵ���Ŀ+1
                      ++++-++++-------------����*/
  TIM1_CCER =0x0001;/*0000 0000 0000 0001
                      |||| |||| |||| |||+---CC1E=1��OC1�ź��������Ӧ���������
                      |||| |||| |||| ||+----CC1P=0��OC1�ߵ�ƽ��Ч
                      |||| |||| |||| |+-----CC1NE=0��OC1N��ֹ���
                      |||| |||| |||| +------CC1NP=0��OC1N�ߵ�ƽ��Ч
                      ++++-++++-++++--------CC2��CC3��CC4����Ϊ��λֵ*/
  TIM1_CCMR1=0x0078;/*0000 0000 0111 1100
                      |||| |||| |||| ||++---CC1S=00��CC1ͨ��������Ϊ���
                      |||| |||| |||| |+-----OC1FE=1��������Ƚ�1 ����ʹ�ܣ�
                      |||| |||| |||| +------OC1PE=1������Ƚ�1Ԥװ��ʹ��
                      |||| |||| |+++--------0C1M=111��PWMģʽ2
                      |||| |||| +-----------OC1CE=0��OC1REF ����ETRF�����Ӱ��
                      ++++-++++-------------CC2ͨ������Ϊ��λֵ*/
  TIM1_BDTR =0x8000;/*1000 0000 0000 0000
                      |+++-++++-++++-++++---����λ���ֲ���
                      +---------------------MOE=0�������ʹ��*/
  TIM1_DIER =0x4200;/*0100 0011 0000 0000  DMA���ж�ʹ�ܼĴ���
                       |     ||        +----CC1IE=0����������/�Ƚ�1�ж�
                       |     |+-------------UDE=1��������µ�DMA����
                       |     +--------------CC1DE=1��������/�Ƚ�1��DMA����
                       +--------------------TDE=1��������DMA����*/
  TIM1_CR1 |=0x0001;//CEN=1��ʹ��TIMER1������

//-----------------TIM2_CH4����PWM���Yͨ��ƫ�Ƶ�ѹֵ-------------------
  TIM2_PSC = 0;
  TIM2_ARR = 4096;              //72MHz/3840=17.58KHz
  TIM2_CCR4= 1024;
  TIM2_CR1 = 0x0084;/*0000 0000 1000 0100
                      |||| |||| |||| |||+---CEN=0����ֹ������
                      |||| |||| |||| ||+----UDIS=0������UEV����
                      |||| |||| |||| |+-----URS=1��ֻ�м�������������жϻ�DMA����
                      |||| |||| |||| +------OPM=0���ڷ��������¼�ʱ����������ֹͣ
                      |||| |||| |||+--------DIR=0�����������ϼ���
                      |||| |||| |++---------CMS=00��ѡ����ض���ģʽ
                      |||| |||| +-----------ARPE=1��TIM2_ARR�Ĵ�����װ�뻺����
                      |||| ||++-------------CKD=00,CK_INT��Ƶ��Ϊ1
                      ++++-++---------------����*/
  TIM2_CCER =0x3000;/*0011 0000 0000 0000
                      |||| ++++-++++-++++---CC1��CC2��CC3����Ϊ��λֵ
                      |||+------------------CC4E=1��OC4�ź��������Ӧ���������
                      ||+-------------------CC4P=1��OC4�͵�ƽ��Ч
                      |+--------------------CC4NE=0��OC4N��ֹ���
                      +---------------------CC4NP=0��OC4N�ߵ�ƽ��Ч*/
  TIM2_CCMR2=0x7C00;/*0111 1100 0000 0000
                      |||| |||| ++++-++++---CC3ͨ������Ϊ��λֵ
                      |||| ||++-------------CC4S=00��CC4ͨ��������Ϊ���
                      |||| |+---------------OC4FE=1��������Ƚ�4 ����ʹ�ܣ�
                      |||| +----------------OC4PE=1������Ƚ�4Ԥװ��ʹ��
                      |+++------------------0C4M=111��PWMģʽ2
                      +---------------------OC4CE=0��OC4REF ����ETRF�����Ӱ��*/
  TIM2_CR1 |=0x0001;//CEN=1��ʹ��TIMER2������

//-----------------TIM3����ϵͳ��ʱ-------------------
  TIM3_PSC  =9;  //9
  TIM3_ARR  =7199;  //1mS ��������=��PSC+1������ARR+1��/��72MHz/2����uS��
  TIM3_CCR1 =3600; //�����ȣ�ռ�ձȣ�50%
  TIM3_CR1 = 0x0084;/*0000 0000 1000 0100
                      |||| |||| |||| |||+---CEN=0����ֹ������
                      |||| |||| |||| ||+----UDIS=0������UEV����
                      |||| |||| |||| |+-----URS=1��ֻ�м�������������жϻ�DMA����
                      |||| |||| |||| +------OPM=0���ڷ��������¼�ʱ����������ֹͣ
                      |||| |||| |||+--------DIR=0�����������ϼ���
                      |||| |||| |++---------CMS=00��ѡ����ض���ģʽ
                      |||| |||| +-----------ARPE=1��TIM3_ARR�Ĵ�����װ�뻺����
                      |||| ||++-------------CKD=00,CK_INT��Ƶ��Ϊ1
                      ++++-++---------------����*/
  TIM3_DIER =0x0002;/*0000 0000 0000 0010  DMA���ж�ʹ�ܼĴ���
                             |         +----CC1IE=1��������/�Ƚ�1�ж�
                             +--------------CC1DE=1��������/�Ƚ�1��DMA����*/
  TIM3_CR1 |=0x0001;//CEN=1��ʹ��TIMER3������

//------------------TIM4_CH1�������ѡ��Ƶ�ʵı�׼����----------------------
  TIM4_PSC = 7;
  TIM4_ARR  =  FARR[List[FREQ_OUT].Val];
  TIM4_CCR1 = (FARR[List[FREQ_OUT].Val]+1)/2;
  TIM4_CR1 = 0x0084;/*0000 0000 1000 0100
                      |||| |||| |||| |||+---CEN=0����ֹ������
                      |||| |||| |||| ||+----UDIS=0������UEV����
                      |||| |||| |||| |+-----URS=1��ֻ�м�������������жϻ�DMA����
                      |||| |||| |||| +------OPM=0���ڷ��������¼�ʱ����������ֹͣ
                      |||| |||| |||+--------DIR=0�����������ϼ���
                      |||| |||| |++---------CMS=00��ѡ����ض���ģʽ
                      |||| |||| +-----------ARPE=1��TIM3_ARR�Ĵ�����װ�뻺����
                      |||| ||++-------------CKD=00,CK_INT��Ƶ��Ϊ1
                      ++++-++---------------����*/
  TIM4_CCER =0x0001;/*0000 0000 0000 0001
                      |||| |||| |||| |||+---CC1E=1��OC1�ź��������Ӧ���������
                      |||| |||| |||| ||+----CC1P=0��OC1�ߵ�ƽ��Ч
                      |||| |||| |||| |+-----CC1NE=0��OC1N��ֹ���
                      |||| |||| |||| +------CC1NP=0��OC1N�ߵ�ƽ��Ч
                      ++++-++++-++++--------CC2��CC3��CC4����Ϊ��λֵ*/
  TIM4_CCMR1=0x0078;/*0000 0000 0111 1100
                      |||| |||| |||| ||++---CC1S=00��CC1ͨ��������Ϊ���
                      |||| |||| |||| |+-----OC1FE=1��������Ƚ�1 ����ʹ�ܣ�
                      |||| |||| |||| +------OC1PE=1������Ƚ�1Ԥװ��ʹ��
                      |||| |||| |+++--------0C1M=111��PWMģʽ2
                      |||| |||| +-----------OC1CE=0��OC1REF ����ETRF�����Ӱ��
                      ++++-++++-------------CC2ͨ������Ϊ��λֵ*/
  TIM4_CR1 |=0x0001;//CEN=1��ʹ��TIMER4������
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
  DMA_ISR = 0x00000000;/*DMA�ж�״̬�Ĵ�������ͨ������IFCR��Ӧλ���
              |||||||+---GIF1 =0��ͨ��1û����TE��HT��TC�¼�ȫ���жϱ�־
              |||||||+---TCIF1=0��ͨ��1û������������жϱ�־
              |||||||+---HTIF1=0��ͨ��1û�����봫���жϱ�־
              |||||||+---TEIF1=0��ͨ��1û������������жϱ�־
              ||||||+----ͨ��2�жϱ�־��
              ||||++-----ͨ��3�жϱ�־��
              ||||++-----ͨ��4�жϱ�־��
              |||+-------ͨ��5�жϱ�־��
              ||+--------ͨ��6�жϱ�־��
              ||+--------ͨ��7�жϱ�־��
              +----------����*/
  DMA_IFCR= 0x00000000;/*DMA�жϱ�־����Ĵ���������������û����
              |||||||+---CGIF1 =1�����ͨ��1��TE��HT��TC�¼�ȫ���жϱ�־
              |||||||+---CTCIF1=1�����ͨ��1�Ĵ�������жϱ�־
              |||||||+---CHTIF1=1�����ͨ��1�İ봫���жϱ�־
              |||||||+---CTEIF1=1�����ͨ��1�Ĵ�������жϱ�־
              ||||||+----���ͨ��2�жϱ�־��
              ||||++-----���ͨ��3�жϱ�־��
              ||||++-----���ͨ��4�жϱ�־��
              |||+-------���ͨ��5�жϱ�־��
              ||+--------���ͨ��6�жϱ�־��
              ||+--------���ͨ��7�жϱ�־��
              +----------����*/
  DMA1_CCR1=  0x00003580;/*0011 0101 1000 0000
               ||||       |||| |||| |||| |||+--EN = 0��ͨ���رգ�1��ͨ������
               ||||       |||| |||| |||| ||+---TCIE=0��������������ж�
               ||||       |||| |||| |||| |+----HTIE=0����ֹ�봫���ж�
               ||||       |||| |||| |||| +-----TEIE=0����ֹ��������ж�
               ||||       |||| |||| |||+-------DIR =0�����ݴ��䷽�򣨴��������
               ||||       |||| |||| ||+--------CIRC=0����ִ��ѭ������
               ||||       |||| |||| |+---------PINC=0����ִ�������ַ����ģʽ
               ||||       |||| |||| +----------MINC=1��ִ�д洢����ַ����ģʽ
               ||||       |||| ||++------------PSIZE=01��16λ�������ݿ��
               ||||       |||| ++--------------MSIZE=01��16λ�洢�����ݿ��
               ||||       ||++-----------------PL=11�����ͨ�����ȼ�
               ||||       |+-------------------MEM2MEM=0���Ǵ洢�����洢��ģʽ
               ++++-------+--------------------����*/

  DMA1_CNDTR1 = 0x00001000;          // DMAͨ��1���������Ĵ���(1024*2 Bytes)
  DMA1_CPAR1  = ADC1_DR_ADDR;        // DMAͨ��1���������ݼĴ�������ַ
  DMA1_CMAR1  = (u32)&Sampl[1];      // DMAͨ��1��Դ��Ŀ��Ĵ洢����ַ
  DMA1_CCR1  |= 0x00000001;          // EN = 1��DMAͨ��1����
}
/*******************************************************************************
* ADC_configuration: ADC initializtion
*******************************************************************************/
void ADC_Configuration(void)
{
  ADC2_CR1 =0x00000000;
  ADC1_CR1 =0x00000000;/*
              ||||||++---AWDCH[4:0]��ģ�⿴�Ź�ͨ��ѡ��λ
              ||||||+----=000��������EOC��AWD��JEOC�ж�
              |||||+-----Nib8_SCAN=0����ʹ��ɨ��ģʽ
              ||||++-----���ָ�λֵ
              |||+-------DUALMOD=0������ģʽ
              ||+--------����ͨ����ע��ͨ������ģ�⿴�Ź�
              ++---------����*/
  ADC2_CR2 =0x00100000;
  ADC1_CR2 =0x00100100;/*
              |||||||+---ADON=0���ر�ADCת��/У׼��=1������ADC������ת��
              |||||||+---CONT=0��1������ת��ģʽ
              |||||||+---CAL=0��=1��A/DУ��ʼ׼��У׼���λ����Ӳ�����
              |||||||+---RSTCAL=0��=1����ʼ��У׼�Ĵ�����У׼���λ����Ӳ�����
              ||||||+----����
              |||||+-----ALIGN=0 & DMA=1�������Ҷ����ʹ��DMAģʽ
              ||||+------
              |||+-------=0000������ͨ��ʹ�ö�ʱ��1��CC1�¼�����ת����=1110��SWSTART
              ||+--------=1��ʹ���ⲿ�����ź�����ת��
              ++---------����*/
  ADC2_SQR1=0x00000000;
  ADC1_SQR1=0x00000000;/*ADC�������мĴ��� 1
              ||++++++---SQ13-16=00000��ת�������е�13-16��ת��ͨ�����(5Bits*4)
              ++---------����*/
  ADC2_SQR3 =0x00000002;
  ADC1_SQR3 =0x00000000;/*ADC�������мĴ��� 3
               ||||||++---SQ1=00000��ת�������еĵ�1��ת��ͨ���ı��(0)
               +++++++----SQ2-6=00000��ת�������е�2-6��ת��ͨ�����(5Bits*5)
               +----------����*/
  ADC2_SMPR2=0x00000000;
  ADC1_SMPR2=0x00000000;/*ADC����ʱ��Ĵ��� 1
               |||||||+---SMP01=001(3Bits)��ʹͨ��00�Ĳ���ʱ�� 1.5T
               ||||+------SMP04=001(3Bits)��ʹͨ��04�Ĳ���ʱ�� 7.5T
               |+++++++---SMP11-17=000(3Bits*7)��ͨ��11-17�Ĳ���ʱ�� 1.5T
               ++---------����*/
  ADC1_CR2 |=0x00000001;
  ADC2_CR2 |=0x00000001;       //ADON=1������ADC1��ADC2
  ADC1_CR2 |=0x00000008;
  while(ADC1_CR2 & 0x00000008);//��ʼ��ADC1У׼�Ĵ���
  ADC2_CR2 |=0x00000008;
  while(ADC2_CR2 & 0x00000008);//��ʼ��ADC2У׼�Ĵ���
  ADC1_CR2 |=0x00000004;
  while(ADC1_CR2 & 0x00000004);//ADC1У׼
  ADC2_CR2 |=0x00000004;
  while(ADC2_CR2 & 0x00000004);//ADC2У׼
}


/******************************** END OF FILE *********************************/
