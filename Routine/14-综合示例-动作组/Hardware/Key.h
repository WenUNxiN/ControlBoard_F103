#ifndef __KEY__H_
#define	__KEY__H_
#include "sys.h"

/* 简单状态机 */
typedef enum {
    KEY_IDLE = 0,       // 初始/已松开
    KEY_DEBOUNCE,       // 消抖中
    KEY_PRESSED,        // 已确认按下
    KEY_WAIT_RELEASE    // 等待松开
} KeyState_t;

// 板载按键
#define KEY_GPIO_CLK        RCC_APB2Periph_GPIOB
#define KEY_GPIO_PORT       GPIOB			   
#define KEY_GPIO_PIN	    GPIO_Pin_2

// 触摸模块
#define TOUCH_KEY_CLK      RCC_APB2Periph_GPIOA
#define TOUCH_KEY_PORT     GPIOA
#define TOUCH_KEY_PIN      GPIO_Pin_12
#define SOUND_KEY_CLK      RCC_APB2Periph_GPIOA
#define SOUND_KEY_PORT     GPIOA
#define SOUND_KEY_PIN      GPIO_Pin_8

#define KEY_PRESS	    1
#define KEY_RELEASE	    0

// 板载按键初始化
void Key_Init(void);
u8 Key_Scan(void);

// 触摸按键初始化
void Sound_Touch_Key_Init(void);
u8 Touch_Key_Scan(void);

#endif /* __KEY_H */

