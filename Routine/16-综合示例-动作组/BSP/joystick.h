#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "Application.h"

typedef enum
{
    POS_CENTER = 0,   // 中
    POS_UP,           // 上
    POS_DOWN,         // 下
    POS_LEFT,         // 左
    POS_RIGHT         // 右
} JoystickPos_t;

#define CHANNEL_VRX_ADC     ADC_Channel_0   // PA0
#define CHANNEL_VRY_ADC     ADC_Channel_6   // PA6

#define PORT_VRXY           GPIOA
#define GPIO_VRX            GPIO_Pin_0
#define GPIO_VRY            GPIO_Pin_6
#define RCC_VRXY            RCC_APB2Periph_GPIOA
#define RCC_VRXY_ADC        RCC_APB2Periph_ADC1
#define PORT_VRXY_ADC       ADC1            

// 摇杆LED
#define JOYSTICK_LED_PIN        GPIO_Pin_11
#define JOYSTICK_LED_PORT       GPIOA             
#define JOYSTICK_LED_CLK        RCC_APB2Periph_GPIOA 
        
#define JOYSTICK_LED_GET_LEVEL()    GPIO_ReadOutputDataBit(JOYSTICK_LED_PORT, JOYSTICK_LED_PIN)   
#define JOYSTICK_LED_TOGGLE()		GPIO_WriteBit(JOYSTICK_LED_PORT, JOYSTICK_LED_PIN, (BitAction)(1 - JOYSTICK_LED_GET_LEVEL()))
#define JOYSTICK_LED_OFF()		    GPIO_SetBits(JOYSTICK_LED_PORT, JOYSTICK_LED_PIN)
#define JOYSTICK_LED_ON()			GPIO_ResetBits(JOYSTICK_LED_PORT, JOYSTICK_LED_PIN)

// 摇杆按键
#define JOYSTICK_KEY_CLK      RCC_APB2Periph_GPIOC
#define JOYSTICK_KEY_PORT     GPIOC
#define JOYSTICK_KEY_PIN      GPIO_Pin_13

#define SAMPLES             30                 // 均值滤波采样次数

void ADC_Joystick_Init(void);
unsigned int Get_Joystick_Percentage_value(char dir);
JoystickPos_t JoystickStatus(void);

#endif

