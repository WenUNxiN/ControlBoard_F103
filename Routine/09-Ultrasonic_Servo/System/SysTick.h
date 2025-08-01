#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "sys.h"
void Delay_ns(u16 t);
void Delay_us(u16 x_us);
void Delay_ms(u16 x_ms);

u32 Millis(void); /* 获取滴答时钟数值 */
void SysTickInit(void);
void TIM1_Init(u16 arr, u16 psc);

#endif /* __SYSTICK_H */
