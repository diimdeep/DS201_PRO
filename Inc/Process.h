/******************** (C) COPYRIGHT 2012 e-Design Co.,Ltd. *********************
 File Name : Process.h
 Version   : DS201 Ver 3.0                                       Author : bure
*******************************************************************************/
#ifndef __PROEESS_H
#define __PROEESS_H

#include "stm32f10x_lib.h"

#define NONE              1
#define AUTO              2
#define NORM              3
#define SINGL             4
//5.03
#define SCAN              5


#define RISE              0
#define FALL              1
#define PRE_SMPL          150

extern u16 Sampl[];
extern s16 Vmax, Vmin, Vmid, Vavg, Start;
extern u16 Vram, Vrms, Vpp,  LowT, HighT;

void WaveProcess(void);
void Build_Ext(u16 x, u8 Ext, u8 Yp);
void Calibrate(void);
void Auto_Fit(void);
void Auto_Calibrate(void);

#endif /*__PROEESS_H*/
/******************************** END OF FILE *********************************/
