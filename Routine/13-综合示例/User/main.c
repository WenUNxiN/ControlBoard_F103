/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
	
	实现的功能：
	1、7路PWM舵机控制
	2、上位机（PC/小程序）控制舵机
	3、可脱机存储控制
	
	PWM舵机引脚：
		SERVO0-PB3
		SERVO1-PB4
		SERVO2-PB5
		SERVO3-PB6
		
		SERVO4-PB7
		SERVO5-PB8
		SERVO6-PB9
		
	蜂鸣器引脚：
		BEEP-PA3
        
	LED引脚：
		LED-PA15
        
    PS2手柄引脚：	
	  PS1-DAT-PA1
	  PS2-CMD-PA7
	  PS6-ATT-PB0
	  PS7-CLK-PB1
      
	按键引脚：
	  KEY-PB2
	
	串口引脚：
      PA9-TX
      PA10-RX
	
	主频：72M
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "Application.h"

#include "bmp.h"

int main(void)
{
    setup_app();

//    OLED_Print(32, 3, "艾米思悠");
    
    OLED_DrawBMP(0, 0, 128, 8, BMP);  // 画位图
    
    while (1)
    {
        loop_app();
    }
}
