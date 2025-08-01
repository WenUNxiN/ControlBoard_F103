#include "Ultrasonic.h"
/***********************************************
	��������:	void USTimer_Init(void)
	���ܽ���:	��ʱ��4��ʼ��,����1us���������㳬��������
	��������:	��
	����ֵ:	��
 ***********************************************/
void USTimer_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ�� TIM4 ʹ��

	TIM_TimeBaseInitStructure.TIM_Period = 30000;/* �趨�������Զ���װֵ */
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;/* Ԥ��Ƶ�� */
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;/* ����ʱ�ӷָ�:TDTS = Tck_tim */
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;/* TIM���ϼ���ģʽ */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
}

/***********************************************
	��������:	void US_Init(void)
	���ܽ��ܣ���������������ʼ��
	������������
	����ֵ��	��
 ***********************************************/
void US_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(Trig_GPIO_CLK | Echo_GPIO_CLK, ENABLE);

	// ��ʼ��������IO�� Trig   Echo 
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
	��������:	void US_Init(void)
	���ܽ��ܣ���ȡ����,�����ͣ���Ӱ���������ٶ�
	������������
	����ֵ��	���ؾ��� ��λcm  ����=�ߵ�ƽʱ��*���٣�340M/S�� /2
 ***********************************************/
float US_DisRead(void)
{
	uint16_t time_us;//��ʱ���ļ���ʱ��
	uint32_t start_time;//��ʼ��ʱ���¼
	uint32_t timeout = 27;//���ó�ʱʱ��Ϊ27����

	//����
	TRIG_SET(1);
	Delay_us(10);
	TRIG_SET(0);

	TIM_Cmd(TIM4, ENABLE);//ʹ��TIMx����

	//��¼��ʼʱ��
	start_time = Millis();
	// �ȴ�ECHO�źŸߵ�ƽ
	while(ECHO_GET() == 0)
	{
	  if ((Millis() - start_time) > timeout)
	  {
			TIM_Cmd(TIM4, DISABLE);//����TIM4����
			return -1;//��ʱ����
	  }
	}

	TIM_SetCounter(TIM4, 0);//�������

	start_time = Millis();
	while (ECHO_GET())
	{
	  if ((Millis() - start_time) > timeout)
	  {
			TIM_Cmd(TIM4, DISABLE); // ����TIM4����
			return -1; // ��ʱ����
	  }
	}

	time_us = TIM_GetCounter(TIM4);
	TIM_Cmd(TIM4, DISABLE); // ʹ��TIM4����
	// 340m/s = 0.017cm/us
	if(time_us < (int)(450*1000/0.017))
	{
	  return (float)time_us * 0.017;
	}
	return -1;
}



