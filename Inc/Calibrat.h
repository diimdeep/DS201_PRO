/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Calibrat.h  
 Version   : DS203_APP Ver 2.3x                                  Author : bure
*******************************************************************************/
#ifndef __CALIBRAT_H
#define __CALIBRAT_H

#include "stm32f10x_lib.h"

#define _100uS      12   // T_BASE = 100uS/Div
#define _2uS        17   // T_BASE = 2uS/Div

void Balance(void);
void Calibrat(u8 Channel);
 


#endif  

/*******************************  END OF FILE  ********************************/
