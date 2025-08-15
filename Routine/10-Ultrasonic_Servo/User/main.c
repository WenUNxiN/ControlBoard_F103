/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    实现的功能：通过超声波控制舵机，距离越近舵机角度越小，反之越大。
                同时OLED显示距离。
    
    PWM舵机引脚：
    SERVO0-PB3
    SERVO1-PB4
    SERVO2-PB5
    SERVO3-PB6
    SERVO4-PB7
    SERVO5-PB8
    SERVO6-PB9
    
    OLED引脚：
    SCL-PC14
    SDA-PC15
	
    超声波引脚：
    （Echo）PA4
    （Trig）PA5
    
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"

int main(void)
{
    /* ---------- 1. 系统底层初始化 ---------- */
    Rcc_Init();     // 使能各外设时钟
    SWJGpio_Init(); // 关闭 JTAG，释放 PA15/PB3/PB4
    SysTickInit();  // 初始化 1 ms 系统嘀嗒（Delay_ms 依赖）

    /* ---------- 2. 外设硬件初始化 ---------- */
    US_Init();      // 超声波 HC-SR04 引脚/时基初始化
    OLED_Init();    // SSD1306 OLED 软件 I2C 初始化
    OLED_Clear();   // 清屏，防止残影
    PwmServoInit(); // 舵机 GPIO/PWM 引脚初始化
    /*
     * TIM2：72 MHz / (PSC+1) = 1 MHz
     * ARR=20000 → 周期 20 ms (50 Hz)，符合舵机标准
     */
    TIM2_Init(20000, 72 - 1);

    /* ---------- 3. UI 显示固定文字 ---------- */
    OLED_Print(0, 0, "艾米思悠"); 
    OLED_Print(0, 2, "num:");     

    /* ---------- 4. 变量定义与舵机零位 ---------- */
    float dist_cm = 0.0f; // 当前测距值
    float pulse = 0.0f;   // 映射后的脉冲宽度(?s)

    /* 将舵机先转到中位 1500 ?s，持续 1000 ms 给上电稳定时间 */
    PwmServo_DoingSet(0, 1500, 1000);

    /* ---------- 5. 主循环：测距→显示→映射→输出 ---------- */
    while (1)
    {
        dist_cm = US_DisRead();              // 读取当前距离 (cm)
        OLED_ShowNum(30, 3, dist_cm, 4, 15); // 显示距离

        pulse = map(dist_cm, 0, 20, 500, 2500); // 0~20 cm → 500~2500 ?s
        pulse = CLAMP(pulse, 500, 2500);        // 限幅，防越界

        PwmServo_DoingSet(0, pulse, 0); // 更新 PWM 占空比
        PwmServo_DoingSet(1, pulse, 0); // 更新 PWM 占空比
        PwmServo_DoingSet(2, pulse, 0); // 更新 PWM 占空比
        PwmServo_DoingSet(3, pulse, 0); // 更新 PWM 占空比
        PwmServo_DoingSet(4, pulse, 0); // 更新 PWM 占空比
        PwmServo_DoingSet(5, pulse, 0); // 更新 PWM 占空比
        PwmServo_DoingSet(6, pulse, 0); // 更新 PWM 占空比
        Delay_ms(50);                   // 50 ms 刷新
    }
}
