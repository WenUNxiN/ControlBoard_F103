/**
 * @file   Function.h
 * @brief  功能任务层头文件
 * @note   声明与外部模块交互的接口及全局标志
 */

#ifndef _FUNCTION__H_
#define _FUNCTION__H_

#include "Application.h"    // 包含系统基本类型与宏

/*----------------- 外部变量声明 -----------------*/
extern u8 Us_ok;            ///< 超声波任务触发标志（1=启动一次流程）
extern u8 clampMode;        ///< 夹爪状态：0=无动作 1=夹取 2=松开

/*----------------- 宏定义 -----------------*/
#define SERVO_STEP 5        ///< 摇杆模式下，每步舵机增减 PWM 脉冲数（单位：μs）

/*----------------- 函数接口 -----------------*/
/**
 * @brief  颜色识别分拣任务
 * @param  interval_ms 两次识别之间的最小间隔（毫秒）
 */
void ColorTask(uint32_t interval_ms);

/**
 * @brief  声音/触摸按键任务
 * @note   内部 20 ms 周期调用，无需额外参数
 */
void SoundTouchTask(void);

/**
 * @brief  摇杆控制任务
 * @note   内部 50 ms 周期调用，实现实时坐标控制
 */
void JoystickTask(void);

/**
 * @brief  超声波测距+自动抓取任务
 * @param  interval_ms 两次测距之间的最小间隔（毫秒）
 */
void UsTask(uint32_t interval_ms);

#endif  /* _FUNCTION__H_ */
