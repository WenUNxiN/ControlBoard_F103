#ifndef _PWMSERVO__H_
#define _PWMSERVO__H_
#include "sys.h"

// 引脚定义
#define SERVO0_PIN GPIO_Pin_3
#define SERVO0_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO0_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define SERVO1_PIN GPIO_Pin_4
#define SERVO1_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO1_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define SERVO2_PIN GPIO_Pin_5
#define SERVO2_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO2_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define SERVO3_PIN GPIO_Pin_6
#define SERVO3_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO3_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define SERVO4_PIN GPIO_Pin_7
#define SERVO4_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO4_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define SERVO5_PIN GPIO_Pin_8
#define SERVO5_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO5_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define SERVO6_PIN GPIO_Pin_9
#define SERVO6_GPIO_PORT GPIOB               /* GPIO端口 */
#define SERVO6_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

/* 控制舵机引脚输出的宏 */
#define SERVO0_PIN_SET(level) GPIO_WriteBit(SERVO0_GPIO_PORT, SERVO0_PIN, level)
#define SERVO1_PIN_SET(level) GPIO_WriteBit(SERVO1_GPIO_PORT, SERVO1_PIN, level)
#define SERVO2_PIN_SET(level) GPIO_WriteBit(SERVO2_GPIO_PORT, SERVO2_PIN, level)
#define SERVO3_PIN_SET(level) GPIO_WriteBit(SERVO3_GPIO_PORT, SERVO3_PIN, level)
#define SERVO4_PIN_SET(level) GPIO_WriteBit(SERVO4_GPIO_PORT, SERVO4_PIN, level)
#define SERVO5_PIN_SET(level) GPIO_WriteBit(SERVO5_GPIO_PORT, SERVO5_PIN, level)
#define SERVO6_PIN_SET(level) GPIO_WriteBit(SERVO6_GPIO_PORT, SERVO6_PIN, level)

#define DJ_NUM 8 /* 舵机数量 */

typedef struct
{
    uint16_t aim;  // 执行目标
    uint16_t time; // 执行时间
    float cur;     // 当前值
    float inc;     // 增量
    int bias;      // 偏差
} servo_t;

extern servo_t duoji_doing[DJ_NUM];

void PwmServoInit(void);                             /* 舵机引脚初始化 */
void PwmServo_PinSet(u8 index, BitAction level);     /* 设置舵机引脚电平 */
void PwmServo_DoingSet(u8 index, int aim, int time); /* 设置舵机参数 */
void PwmServo_IncOffset(u8 index);                   // 舵机增量
void PwmServo_BiasSet(uint8_t index, int bias);      // 设置舵机偏差参数
void PwmServoStop(u8 index);                         // 舵机停止
void PwmServo_SetAngle(uint8_t index, float angle,float Servo_angle);  // 设置舵机角度
#endif
