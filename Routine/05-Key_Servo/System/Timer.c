#include "Timer.h"
#include "PwmServo.h"

/*************************************************
 * 函数名称：TIM2_Init
 * 功能介绍：初始化 TIM2，用于生成舵机控制 PWM 波形
 * 参数说明：
 *   arr - 自动重装载值（决定定时周期）
 *   psc - 预分频系数（决定计数频率）
 * 返回值：无
 *************************************************/
void TIM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 使能 TIM2 时钟（APB1 总线）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 2. 配置 TIM2 时基单元
    TIM_TimeBaseStructure.TIM_Period        = arr;                // 自动重装载值（ARR）
    TIM_TimeBaseStructure.TIM_Prescaler     = psc;                // 预分频器（PSC）
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // 时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 3. 禁用 ARR 预装载（立即生效）
    TIM_ARRPreloadConfig(TIM2, DISABLE);

    // 4. 使能 TIM2 更新中断（UEV）
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 5. 配置 NVIC 中断优先级
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;   // 中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;          // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;     // 使能中断通道
    NVIC_Init(&NVIC_InitStructure);

    // 6. 启动 TIM2 计数器
    TIM_Cmd(TIM2, ENABLE);
}


/*************************************************
 * 函数名称：TIM2_IRQHandler
 * 功能介绍：TIM2 中断服务函数，用于生成 8 路舵机 PWM 波形
 * 实现原理：
 *   - 每个舵机占用 2.5ms（2500 个计数）
 *   - 8 个舵机共 20ms（20000 计数）一个周期
 *   - 使用 flag 切换高低电平
 *   - 每次中断只处理一个舵机的一段波形
 *************************************************/
void TIM2_IRQHandler(void)
{
    static u8 flag = 0;         // 0：高电平阶段；1：低电平阶段
    static u8 duoji_index1 = 0; // 当前处理的舵机编号（0~7）
    int temp;

    // 1. 判断是否发生更新中断
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // 2. 处理完 8 个舵机后，重置索引
        if (duoji_index1 == 8)
        {
            duoji_index1 = 0;
        }

        // 3. flag = 0：高电平阶段（脉冲宽度）
        if (flag == 0)
        {
            // 设置下一次中断时间为当前舵机脉宽
            TIM2->ARR = (unsigned int)(duoji_doing[duoji_index1].cur);

            // 设置该舵机引脚为高电平
            PwmServo_PinSet(duoji_index1, Bit_SET);

            // 更新舵机当前值（平滑移动）
            PwmServo_IncOffset(duoji_index1);
        }
        // 4. flag = 1：低电平阶段（剩余时间）
        else
        {
            // 计算剩余时间：2.5ms - 脉宽
            temp = 2500 - (unsigned int)(duoji_doing[duoji_index1].cur);

            // 设置下一次中断时间为剩余时间
            TIM2->ARR = temp;

            // 设置该舵机引脚为低电平
            PwmServo_PinSet(duoji_index1, Bit_RESET);

            // 切换到下一个舵机
            duoji_index1++;
        }

        // 5. 切换 flag 状态（高/低电平交替）
        flag = !flag;

        // 6. 清除中断标志位
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
