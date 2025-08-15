#include "Led.h"   

/****************************************************************
 * 函数名称：LED_Init
 * 功    能：初始化LED引脚，并设置LED默认状态
 * 参    数：无
 * 返 回 值：无
 ****************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                    // 定义GPIO初始化结构体变量
    
    /* 1. 使能时钟 */
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    
    /* 2. 配置引脚为推挽输出模式 */
    GPIO_InitStructure.GPIO_Pin   = LED_GPIO_PIN;           // 选择引脚
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 输出速度50MHz
    
    /* 3. 调用库函数完成GPIO初始化 */
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    /* 4. 默认熄灭LED：将引脚置高电平（低电平点亮，高电平熄灭） */
    GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}
