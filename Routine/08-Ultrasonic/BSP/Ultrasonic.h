#ifndef _ULTRASONIC__H_          // 防止重复包含
#define _ULTRASONIC__H_

#include "main.h"                // 工程主头文件（含 HAL、stdint 等）

/*---------------- 引脚宏定义 ----------------*/
/* 触发引脚（Trig）PA5 输出 */
#define Trig_Pin                GPIO_Pin_5        // 触发信号输出脚
#define Trig_GPIO_Port          GPIOA             // 所属端口
#define Trig_GPIO_CLK           RCC_APB2Periph_GPIOA   // GPIOA 时钟

/* 回响引脚（Echo）PA4 输入 */
#define Echo_Pin                GPIO_Pin_4        // 回响信号输入脚
#define Echo_GPIO_Port          GPIOA
#define Echo_GPIO_CLK           RCC_APB2Periph_GPIOA

/*---------------- 位带操作宏 ----------------*/
// 设置 Trig 引脚电平：TRIG_SET(1) 高电平；TRIG_SET(0) 低电平
#define TRIG_SET(x)     GPIO_WriteBit(Trig_GPIO_Port, Trig_Pin, (BitAction)x)

// 读取 Echo 引脚电平：返回 0 或 1
#define ECHO_GET()      GPIO_ReadInputDataBit(Echo_GPIO_Port, Echo_Pin)

/*---------------- 对外接口声明 ----------------*/
void US_Init(void);          // 初始化 Trig、Echo 引脚及时钟
float US_DisRead(void);      // 读取一次距离，单位 cm（范围 2 cm ~ 400 cm）

#endif

