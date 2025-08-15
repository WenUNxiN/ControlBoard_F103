#include "Key.h"

/***********************************************

 ***********************************************/
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = KEY_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

/***********************************************

 ***********************************************/
u8 Key_Scan(void)
{
	static u8 Key_Flag = KEY_RELEASE;	  //
	static u8 Key_LastFlag = KEY_RELEASE; //
	Key_LastFlag = Key_Flag;
	Key_Flag = GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN);

	if (Key_LastFlag == KEY_PRESS && Key_Flag == KEY_RELEASE)
	{
		return KEY_PRESS;
	}
	return Key_Flag;
}



void Sound_Touch_Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TOUCH_KEY_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TOUCH_KEY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(TOUCH_KEY_PORT, &GPIO_InitStructure);
    
	RCC_APB2PeriphClockCmd(SOUND_KEY_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SOUND_KEY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(SOUND_KEY_PORT, &GPIO_InitStructure);
}

/***********************************************

 ***********************************************/
u8 Touch_Key_Scan(void)
{
	static u8 Key_Flag = KEY_RELEASE;	  //
	static u8 Key_LastFlag = KEY_RELEASE; //
	Key_LastFlag = Key_Flag;
	Key_Flag = GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN);

	if (Key_LastFlag == KEY_PRESS && Key_Flag == KEY_RELEASE)
	{
		return KEY_PRESS;
	}
	return Key_Flag;
}

