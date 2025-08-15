#ifndef __MAIN_H__
#define __MAIN_H__

#include "main.h"
#include <stdio.h>  
#include "SysTick.h"
#include "rcc.h"
#include "sys.h"
#include "OLED.h"
#include "Ultrasonic.h"
#include "PwmServo.h" 
#include "Timer.h"
#include "Global.h"
#include "Uart.h"
#include "Led.h"
#include "Beep.h"
#include "Key.h"
#include "Protocol.h"
#include "SPI_Flash.h"
#include "App_Ps2.h"
#include "Action.h"
#include "Ps2.h"
#include "ServoPID.h"
#include "Kinematics.h"
#include "Soft_I2C.h"

/*
	初始化函数声明
*/

void setup_app(void);
//void setup_systick(void);	//初始化滴答时钟，1S增加一次systick_ms的值
void Uart_Init(void);
void Parameter_Init(void);   //从W25Q64中读取参数
void ServoState_Init(void);  //舵机上电初始位置
void setup_run_action(void); //执行开机动作组
extern void Buzzer_times(u32 time, u32 count);

/*
	主循环函数声明
*/
extern void Led_Blink(u32 time);	        //循环执行工作指示灯
void loop_key(void);

void Loop_Uart(void);
#endif

