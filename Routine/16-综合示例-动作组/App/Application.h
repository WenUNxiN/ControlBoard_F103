#ifndef __APPLICATION_H__
#define __APPLICATION_H__

/* ********************************************************
 * 文件名：Application.h
 * 功  能：项目总头文件，集中包含所有外设驱动及全局声明
 * 说  明：
 *   1. 宏保护：__APPLICATION_H__，防止重复包含
 *   2. 引入标准头及所有外设驱动
 *   3. 声明系统初始化、主循环及常用工具函数
 * ********************************************************/

/* ---------- C 库头文件 ---------- */
#include <stdio.h>          // 标准输入/输出

/* ---------- 驱动层 ---------- */
#include "SysTick.h"        // 1 ms 系统滴答
#include "rcc.h"            // 时钟初始化
#include "sys.h"            // 系统相关（软复位、时钟配置等）
#include "OLED.h"           // 0.96" I²C OLED 显示屏
#include "Ultrasonic.h"     // HC-SR04 超声波
#include "PwmServo.h"       // 7 路 PWM 舵机驱动
#include "Timer.h"          // 通用定时器
#include "Global.h"         // 全局变量/宏定义
#include "Uart.h"           // 串口 1/2/3 驱动
#include "Led.h"            // 状态指示灯
#include "Beep.h"           // 蜂鸣器
#include "Key.h"            // 普通按键
#include "Protocol.h"       // 通信协议解析
#include "SPI_Flash.h"      // W25Q64 外部 Flash
#include "App_Ps2.h"        // PS2 手柄应用层
#include "Action.h"         // 动作组管理
#include "Ps2.h"            // PS2 手柄底层
#include "ServoPID.h"       // 舵机 PID 闭环
#include "Kinematics.h"     // 正/逆运动学
#include "Soft_I2C.h"       // 软件 I²C 时序
#include "GY33T.h"          // GY33 颜色传感器
#include "Function.h"       // 颜色/摇杆/超声/声音任务
#include "joystick.h"       // 摇杆 ADC 与按键

/* ========================================================
 * 系统级初始化（一次性）
 * ======================================================== */
void setup_app(void);           // 系统上电初始化入口
void Uart_Init(void);           // 串口 1/2/3 统一初始化
void Parameter_Init(void);      // 从 W25Q64 读取参数
void ServoState_Init(void);     // 舵机上电初始位置
void setup_run_action(void);    // 执行开机动作组

/* ========================================================
 * 工具函数（跨文件使用）
 * ======================================================== */
extern void Buzzer_times(u32 time, u32 count); // 蜂鸣器提示
extern u8   SetMode;                           // 当前运行模式

/* ========================================================
 * 主循环任务
 * ======================================================== */
extern void Led_Blink(u32 time);   // 800 ms 翻转 LED
extern void loop_key(void);        // 普通按键扫描
extern void loop_Joystick_key(void); // 摇杆按键扫描
extern void LoopMode(void);        // 基础工作模式
extern void LoopVisionMode(void);  // 视觉工作模式
extern void Loop_Uart(void);       // 串口协议解析

#endif  /* __APPLICATION_H__ */

