#ifndef _LED__H_
#define	_LED__H_
#include "sys.h"

#define LED_GPIO_PIN GPIO_Pin_15
#define LED_GPIO_PORT GPIOA             
#define LED_GPIO_CLK RCC_APB2Periph_GPIOA 


#define LED_GET_LEVEL() GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_GPIO_PIN)   
#define LED_TOGGLE()		   GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, (BitAction)(1 - LED_GET_LEVEL()))
#define LED_OFF()		   GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PIN)
#define LED_ON()			   GPIO_ResetBits(LED_GPIO_PORT,LED_GPIO_PIN)


void LED_Init(void);

#endif /* _My_LED__H_ */
