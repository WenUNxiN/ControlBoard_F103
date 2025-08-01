#ifndef _PS2__H_
#define _PS2__H_
#include "sys.h"
#include "SysTick.h"

/* 定义PS2引脚，修改编号就可以修改PS2引脚 */
#define PS2_DAT_PIN 		GPIO_Pin_1
#define PS2_DAT_GPIO_PORT 	GPIOA               /* GPIO端口 */
#define PS2_DAT_GPIO_CLK 	RCC_APB2Periph_GPIOA /* GPIO端口时钟 */

#define PS2_CMD_PIN 		GPIO_Pin_7
#define PS2_CMD_GPIO_PORT 	GPIOA               /* GPIO端口 */
#define PS2_CMD_GPIO_CLK 	RCC_APB2Periph_GPIOA /* GPIO端口时钟 */

#define PS2_CS_PIN 			GPIO_Pin_0
#define PS2_CS_GPIO_PORT 	GPIOB               /* GPIO端口 */
#define PS2_CS_GPIO_CLK 	RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

#define PS2_CLK_PIN 		GPIO_Pin_1
#define PS2_CLK_GPIO_PORT 	GPIOB               /* GPIO端口 */
#define PS2_CLK_GPIO_CLK 	RCC_APB2Periph_GPIOB /* GPIO端口时钟 */

/*******PS2相关指令表*******/
#define START_CMD 0x01   /* 开始命令 */
#define ASK_DAT_CMD 0x42 /* 请求数据 */

/*******PS2模式数据表*******/
#define PS2_MODE_GRN 0x41 /* 模拟绿灯 */
#define PS2_MODE_RED 0x73 /* 模拟红灯 */

/* 控制PS2的宏 */
#define PS2_DAT() 	GPIO_ReadInputDataBit(PS2_DAT_GPIO_PORT, PS2_DAT_PIN)        // 读取状态
#define PS2_CMD(x) 	GPIO_WriteBit(PS2_CMD_GPIO_PORT, PS2_CMD_PIN, (BitAction)x) 
#define PS2_CS(x)	GPIO_WriteBit(PS2_CS_GPIO_PORT, PS2_CS_PIN, (BitAction)x)   
#define PS2_CLK(x)  GPIO_WriteBit(PS2_CLK_GPIO_PORT, PS2_CLK_PIN, (BitAction)x) 


extern u8 psx_buf[9];

/*******PS2相关函数声明*******/
void Ps2_Init(void);       /* PS2手柄初始化 */
void Ps2_WriteRead(void); /* 读取手柄数据 */
#endif
