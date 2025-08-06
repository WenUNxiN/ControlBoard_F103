#ifndef __KEY__H_
#define	__KEY__H_
#include "sys.h"

/* ��״̬�� */
typedef enum {
    KEY_IDLE = 0,       // ��ʼ/���ɿ�
    KEY_DEBOUNCE,       // ������
    KEY_PRESSED,        // ��ȷ�ϰ���
    KEY_WAIT_RELEASE    // �ȴ��ɿ�
} KeyState_t;

// ���ذ���
#define KEY_GPIO_CLK        RCC_APB2Periph_GPIOB
#define KEY_GPIO_PORT       GPIOB			   
#define KEY_GPIO_PIN	    GPIO_Pin_2

// ����ģ��
#define TOUCH_KEY_CLK      RCC_APB2Periph_GPIOA
#define TOUCH_KEY_PORT     GPIOA
#define TOUCH_KEY_PIN      GPIO_Pin_12
#define SOUND_KEY_CLK      RCC_APB2Periph_GPIOA
#define SOUND_KEY_PORT     GPIOA
#define SOUND_KEY_PIN      GPIO_Pin_8

#define KEY_PRESS	    1
#define KEY_RELEASE	    0

// ���ذ�����ʼ��
void Key_Init(void);
u8 Key_Scan(void);

// ����������ʼ��
void Sound_Touch_Key_Init(void);
u8 Touch_Key_Scan(void);

#endif /* __KEY_H */

