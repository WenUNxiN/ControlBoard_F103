/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    实现的功能：通过串口解析指令，并让舵机运动
    
    PWM舵机引脚：
    SERVO0-PB3
    SERVO1-PB4
    SERVO2-PB5
    SERVO3-PB6
    SERVO4-PB7
    SERVO5-PB8
    SERVO6-PB9
    
    串口1引脚：
    PA9-TX
    PA10-RX
    
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"

int main(void)
{
    /* ---------- 1. 系统底层初始化 ---------- */
    Rcc_Init();         // 使能所有外设时钟
    SWJGpio_Init();     // 关闭 JTAG，仅保留 SWD，释放 PA15/PB3/PB4
    SysTickInit();      // 配置 1 ms SysTick（Delay_ms 用）
    Uart1_Init(115200); // 初始化 USART1（PA9-TX，PA10-RX）

    /* ---------- 2. 舵机 PWM 初始化 ---------- */
    PwmServoInit();           // 配置舵机 IO 为复用推挽
    TIM2_Init(20000, 72 - 1); // TIM2：72 MHz/72 = 1 MHz → ARR=20000 → 50 Hz (20 ms)

    /* ---------- 3. 主循环：串口命令解析 ---------- */
    while (1)
    {
        if (Uart_GetOK) // 收到完整帧
        {
            switch (Uart_Mode) // 根据帧头决定解析方式
            {
            case 1: // 文本命令 $XXX!
                Parse_Cmd(Uart_ReceiveBuf);
                break;

            case 2: // 单舵机帧 #ID P PWM T TIME!
                Parse_Action(Uart_ReceiveBuf);
                break;

            case 3: // 多舵机帧 {...}
                Parse_Action(Uart_ReceiveBuf);
                break;
            }

            Uart_Mode = 0;  // 复位帧类型
            Uart_GetOK = 0; // 允许接收下一帧
        }
    }
}
