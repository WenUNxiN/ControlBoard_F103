#ifndef _TIMER__H_
#define _TIMER__H_

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

void TIM1_Init(u16 arr, u16 psc);
void TIM2_Init(u16 arr, u16 psc);

#endif
