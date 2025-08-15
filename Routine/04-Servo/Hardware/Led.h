#ifndef _LED__H_
#define _LED__H_

#include "sys.h"

/* 定义LED引脚、端口及时钟宏 */
#define LED_GPIO_PIN     GPIO_Pin_15           
#define LED_GPIO_PORT    GPIOA                 
#define LED_GPIO_CLK     RCC_APB2Periph_GPIOA  


/* 读取LED当前输出电平（1=高电平/灭，0=低电平/亮） */
#define LED_GET_LEVEL()  GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_GPIO_PIN)

/* LED状态翻转：如果当前为亮则变为灭，反之亦然 */
#define LED_TOGGLE()     GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, (BitAction)(1 - LED_GET_LEVEL()))

/* LED熄灭：将引脚置高电平（低电平点亮，高电平熄灭） */
#define LED_OFF()        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN)

/* LED点亮：将引脚置低电平 */
#define LED_ON()         GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN)


/* 函数声明：初始化LED引脚 */
void LED_Init(void);

#endif /* LED__H_ */
