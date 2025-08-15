#include "Beep.h"
// #include "SysTick.h"

/***********************************************

 ***********************************************/
void Buzzer_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}
