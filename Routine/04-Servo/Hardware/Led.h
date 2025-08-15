#ifndef _LED__H_
#define _LED__H_

#include "sys.h"

/* ����LED���š��˿ڼ�ʱ�Ӻ� */
#define LED_GPIO_PIN     GPIO_Pin_15           
#define LED_GPIO_PORT    GPIOA                 
#define LED_GPIO_CLK     RCC_APB2Periph_GPIOA  


/* ��ȡLED��ǰ�����ƽ��1=�ߵ�ƽ/��0=�͵�ƽ/���� */
#define LED_GET_LEVEL()  GPIO_ReadOutputDataBit(LED_GPIO_PORT, LED_GPIO_PIN)

/* LED״̬��ת�������ǰΪ�����Ϊ�𣬷�֮��Ȼ */
#define LED_TOGGLE()     GPIO_WriteBit(LED_GPIO_PORT, LED_GPIO_PIN, (BitAction)(1 - LED_GET_LEVEL()))

/* LEDϨ�𣺽������øߵ�ƽ���͵�ƽ�������ߵ�ƽϨ�� */
#define LED_OFF()        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN)

/* LED�������������õ͵�ƽ */
#define LED_ON()         GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN)


/* ������������ʼ��LED���� */
void LED_Init(void);

#endif /* LED__H_ */
