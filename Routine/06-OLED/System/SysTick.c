#include "SysTick.h"

static u32 systick_ms = 0; /* 记录时间 */

void Delay_ns(u16 t)
{
  while (t--)
    ;
  return;
}

void Delay_us(u16 x_us)
{
  volatile unsigned int num;
  volatile unsigned int t;
  for (num = 0; num < x_us; num++)
  {
    t = 11;
    while (t != 0)
    {
      t--;
    }
  }
}

void Delay_ms(u16 x_ms)
{
  volatile unsigned int num;
  for (num = 0; num < x_ms; num++)
  {
    Delay_us(1000);
  }
}

/***********************************************
  函数名称:	void My_SysTickInit(void)
  功能介绍:	初始化systick
  函数参数:	无
  返回值:		无
 ***********************************************/
void SysTickInit(void) // 1毫秒@72MHz
{
  SysTick_Config(SystemCoreClock / 1000);
}

/***********************************************
  函数名称:	SysTick_Handler()
  功能介绍:	SysTick中断
  函数参数:	无
  返回值:		无
 ***********************************************/
void SysTick_Handler(void)
{
  systick_ms++;
}

/* 获取滴答时钟数值 */
/***********************************************
  函数名称:	My_Millis()
  功能介绍:
  函数参数:	无
  返回值:		无
 ***********************************************/
u32 Millis(void)
{
  return systick_ms;
}
