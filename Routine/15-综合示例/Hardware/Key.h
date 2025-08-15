#ifndef 	__KEY__H_
#define	__KEY__H_
#include "sys.h"

#define    KEY_GPIO_CLK    RCC_APB2Periph_GPIOB
#define    KEY_GPIO_PORT    GPIOB			   
#define    KEY_GPIO_PIN		 GPIO_Pin_2

#define KEY_PRESS	  1
#define KEY_RELEASE	0

void Key_Init(void);
u8 Key_Scan(void);

#endif /* __KEY_H */

