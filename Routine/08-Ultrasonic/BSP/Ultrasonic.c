#include "Ultrasonic.h"
/***********************************************
	函数名称:	void USTimer_Init(void)
	功能介绍:	定时器4初始化,计数1us，用来计算超声波距离
	函数参数:	无
	返回值:	无
 ***********************************************/
void USTimer_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟 TIM4 使能

	TIM_TimeBaseInitStructure.TIM_Period = 30000;/* 设定计数器自动重装值 */
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;/* 预分频器 */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;/* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;/* TIM向上计数模式 */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
}

/***********************************************
	函数名称:	void US_Init(void)
	功能介绍：超声波传感器初始化
	函数参数：无
	返回值：	无
 ***********************************************/
void US_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(Trig_GPIO_CLK | Echo_GPIO_CLK, ENABLE);

	// 初始化超声波IO口 Trig   Echo 
	GPIO_InitStructure.GPIO_Pin = Trig_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Trig_GPIO_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = Echo_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(Echo_GPIO_Port, &GPIO_InitStructure);
	
	USTimer_Init();
}

/***********************************************
	函数名称:	void US_Init(void)
	功能介绍：获取距离,阻塞型，会影响主程序速度
	函数参数：无
	返回值：	返回距离 单位cm  距离=高电平时间*声速（340M/S） /2
 ***********************************************/
float US_DisRead(void)
{
	uint16_t time_us;//定时器的计数时间
	uint32_t start_time;//开始的时间记录
	uint32_t timeout = 27;//设置超时时间为27毫秒

	//触发
	TRIG_SET(1);
	Delay_us(10);
	TRIG_SET(0);

	TIM_Cmd(TIM4, ENABLE);//使能TIMx外设

	//记录开始时间
	start_time = Millis();
	// 等待ECHO信号高电平
	while(ECHO_GET() == 0)
	{
	  if ((Millis() - start_time) > timeout)
	  {
			TIM_Cmd(TIM4, DISABLE);//禁用TIM4外设
			return -1;//超时返回
	  }
	}

	TIM_SetCounter(TIM4, 0);//清除计数

	start_time = Millis();
	while (ECHO_GET())
	{
	  if ((Millis() - start_time) > timeout)
	  {
			TIM_Cmd(TIM4, DISABLE); // 禁用TIM4外设
			return -1; // 超时返回
	  }
	}

	time_us = TIM_GetCounter(TIM4);
	TIM_Cmd(TIM4, DISABLE); // 使能TIM4外设
	// 340m/s = 0.017cm/us
	if(time_us < (int)(450*1000/0.017))
	{
	  return (float)time_us * 0.017;
	}
	return -1;
}



