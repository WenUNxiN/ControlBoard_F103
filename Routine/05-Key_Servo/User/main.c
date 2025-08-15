/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
	
    实现的功能：按下KEY舵机转动500μs，到达最大后回到500
                PWM 脉宽 500 μs – 2500 μs 线性映射 270°舵机
                ? 500 μs → 0°（最小角度）
                ? 1500 μs → 135°（中立位）
                ? 2500 μs → 270°（最大角度）
    
    PWM舵机引脚：
    SERVO0-PB3
    SERVO1-PB4
    SERVO2-PB5
    SERVO3-PB6
    SERVO4-PB7
    SERVO5-PB8
    SERVO6-PB9

    KEY引脚：
    KEY-PB2
    
    主频：72M
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "SysTick.h"
#include "rcc.h"
#include "sys.h"
#include "Led.h"
#include "Key.h"
#include "PwmServo.h" 
#include "Timer.h"

int main(void) {	
    
    Rcc_Init();         //时钟初始化
    SWJGpio_Init();     //仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
    SysTickInit(); 		//系统时间初始化
    LED_Init();         //LED初始化
    Key_Init();         //按键初始化
    PwmServoInit();     //PWM舵机初始化
    // 假设 APB1=72 MHz，预分频 72-1，自动重装载 20000-1 → 50 Hz
    TIM2_Init(20000, 72 - 1); /* 初始化定时器2，用于pwm控制舵机 */
    
    int num = 0;
    
	while(1) {
        
        if(Key_Scan() == KEY_PRESS)   // 检测“按下→松开”一次完整动作
        {
            num += 500;                     //每次增加500us
            if(num > 2500) num = 500;       //限位
            PwmServo_DoingSet(0, num, 1000);//设置舵机0位置
            PwmServo_DoingSet(1, num, 1000);//设置舵机1位置
            PwmServo_DoingSet(2, num, 1000);//设置舵机2位置
            PwmServo_DoingSet(3, num, 1000);//设置舵机3位置
            PwmServo_DoingSet(4, num, 1000);//设置舵机4位置
            PwmServo_DoingSet(5, num, 1000);//设置舵机5位置
            PwmServo_DoingSet(6, num, 1000);//设置舵机6位置
        }
	}
}



