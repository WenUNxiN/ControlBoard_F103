#include "Beep.h"
#include "SysTick.h"

/*
 * 函数名：Buzzer_Init
 * 描  述：初始化蜂鸣器所用到的 GPIO
 * 参  数：无
 * 返  回：无
 */
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;          // 定义 GPIO 初始化结构体

    /* 1. 打开蜂鸣器引脚所在的 GPIO 时钟 */
    RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);

    /* 2. 配置蜂鸣器引脚 */
    GPIO_InitStructure.GPIO_Pin   = BUZZER_GPIO_PIN;      // 选择蜂鸣器引脚
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // IO 口翻转速度 50 MHz
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);     // 调用库函数完成配置

    /* 3. 默认关闭蜂鸣器*/
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

/*
 * 函数名：Buzzer_times
 * 描  述：让蜂鸣器以固定占空比响若干次
 * 参  数：
 *     time  ：每次“嘀”或“嗒”持续的时长，单位 ms
 *     count ：响多少次（嘀嗒算一次）
 * 返  回：无
 */
void Buzzer_times(u32 time, u32 count)
{
    for (int i = 0; i < count; i++)
    {
        BUZZER_ON();          
        Delay_ms(time);   
        BUZZER_OFF();         
        Delay_ms(time);       
    }
}
