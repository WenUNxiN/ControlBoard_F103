#include "SysTick.h"

static u32 systick_ms = 0; /* ��¼ʱ�� */

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
  ��������:	void My_SysTickInit(void)
  ���ܽ���:	��ʼ��systick
  ��������:	��
  ����ֵ:		��
 ***********************************************/
void SysTickInit(void) // 1����@72MHz
{
  SysTick_Config(SystemCoreClock / 1000);
}

/***********************************************
  ��������:	SysTick_Handler()
  ���ܽ���:	SysTick�ж�
  ��������:	��
  ����ֵ:		��
 ***********************************************/
void SysTick_Handler(void)
{
  systick_ms++;
}

/* ��ȡ�δ�ʱ����ֵ */
/***********************************************
  ��������:	My_Millis()
  ���ܽ���:
  ��������:	��
  ����ֵ:		��
 ***********************************************/
u32 Millis(void)
{
  return systick_ms;
}
