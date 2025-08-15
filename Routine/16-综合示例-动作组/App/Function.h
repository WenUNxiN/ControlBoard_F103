#ifndef _FUNCTION__H_
#define _FUNCTION__H_

#include "Application.h"

extern u8 Us_ok;

// 摇杆一次动作多少
#define SERVO_STEP 5

// 颜色任务
void ColorTask(uint32_t interval_ms);
void SoundTouchTask(void);
void JoystickTask(void);
void UsTask(uint32_t interval_ms);

#endif
