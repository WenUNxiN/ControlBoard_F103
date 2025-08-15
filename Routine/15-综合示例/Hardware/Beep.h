#ifndef __BEEP__H_
#define	__BEEP__H_
#include "sys.h"

#define BUZZER_GPIO_PIN     GPIO_Pin_3
#define BUZZER_GPIO_PORT    GPIOA     
#define BUZZER_GPIO_CLK     RCC_APB2Periph_GPIOA 

#define BUZZER_GET_LEVEL()  GPIO_ReadOutputDataBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN)   
#define BUZZER_TOGGLE()		GPIO_WriteBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, (BitAction)(1 - My_BUZZER_GET_LEVEL())) 
#define BUZZER_ON()		    GPIO_SetBits(BUZZER_GPIO_PORT,BUZZER_GPIO_PIN)
#define BUZZER_OFF()		GPIO_ResetBits(BUZZER_GPIO_PORT,BUZZER_GPIO_PIN)

void Buzzer_Init(void);
void Buzzer_times(u32 time, u32 count);

#endif

