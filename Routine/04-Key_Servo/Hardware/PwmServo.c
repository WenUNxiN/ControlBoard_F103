// ========================= 头文件 =========================
#include "PwmServo.h"   // 本文件对应的头文件，包含宏、类型定义等
#include "Global.h"     // 全局变量/宏定义
#include <math.h>
// ========================= 全局变量 =========================
servo_t duoji_doing[DJ_NUM];   // 保存 0~7 号舵机的“运行中”参数（目标值、当前值、增量、执行时间、偏差等）

// ========================= 函数实现 =========================

/*----------------------------------------------------------
 * 函数名称：PwmServoInit
 * 功    能：舵机 GPIO 初始化（推挽输出 50 MHz）
 * 参    数：无
 * 返 回 值：无
 *----------------------------------------------------------*/
void PwmServoInit(void)
{
    u8 i;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 使能所用端口的 RCC 时钟
    RCC_APB2PeriphClockCmd(SERVO0_GPIO_CLK | SERVO1_GPIO_CLK | SERVO2_GPIO_CLK |
                           SERVO3_GPIO_CLK | SERVO4_GPIO_CLK | SERVO5_GPIO_CLK |
                           SERVO6_GPIO_CLK, ENABLE);

    // 2. 统一结构体参数：50 MHz 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;

    // 3. 依次初始化 7 路 GPIO（宏在头文件定义）
    GPIO_InitStructure.GPIO_Pin = SERVO0_PIN;  GPIO_Init(SERVO0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO1_PIN;  GPIO_Init(SERVO1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO2_PIN;  GPIO_Init(SERVO2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO3_PIN;  GPIO_Init(SERVO3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO4_PIN;  GPIO_Init(SERVO4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO5_PIN;  GPIO_Init(SERVO5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO6_PIN;  GPIO_Init(SERVO6_GPIO_PORT, &GPIO_InitStructure);

    // 4. 初始化全局结构体数组：默认目标 1500us，当前 1500us，增量 20，时间 5000ms
    for (i = 0; i < DJ_NUM; i++)
    {
        duoji_doing[i].aim  = 1500;
        duoji_doing[i].cur  = 1500;
        duoji_doing[i].inc  = 20;
        duoji_doing[i].time = 5000;
    }
}

/*----------------------------------------------------------
 * 函数名称：PwmServo_PinSet
 * 功    能：直接设置某个舵机引脚电平（用于软件 PWM）
 * 参    数：index 0~7   level 1=高 0=低
 * 返 回 值：无
 *----------------------------------------------------------*/
void PwmServo_PinSet(u8 index, BitAction level)
{
    switch (index)
    {
        case 0: SERVO0_PIN_SET(level); break;
        case 1: SERVO1_PIN_SET(level); break;
        case 2: SERVO2_PIN_SET(level); break;
        case 3: SERVO3_PIN_SET(level); break;
        case 4: SERVO4_PIN_SET(level); break;
        case 5: SERVO5_PIN_SET(level); break;
        case 6: SERVO6_PIN_SET(level); break;
//        case 7: SERVO7_PIN_SET(level); break;
        default: break;
    }
}

/*----------------------------------------------------------
 * 函数名称：PwmServo_DoingSet
 * 功    能：给指定舵机下发“平滑运动”指令
 * 参    数：index 0~6
 *            aim   目标脉宽 500~2500 us（0 表示停止）
 *            time  完成动作所需时间 0~10000 ms
 * 返 回 值：无
 * 注    意：内部会计算每 20 ms 需要增加/减少的增量 inc
 *----------------------------------------------------------*/
void PwmServo_DoingSet(u8 index, int aim, int time)
{
    if (index >= DJ_NUM) return;          // 越界保护

    /* 1. aim==0 视为停止 */
    if (aim == 0)
    {
        duoji_doing[index].inc = 0;       // 不再递增
        duoji_doing[index].aim = duoji_doing[index].cur;
        return;
    }

    /* 2. 限幅 */
    if (aim > 2490) aim = 2490;
    else if (aim < 510) aim = 510;

    if (time > 10000) time = 9999;

    /* 3. 若当前值已等于目标值，为防止除 0，人为增加微小偏移（0.0077） */
    if (duoji_doing[index].cur == aim)
    {
        aim += 0.0077f;
    }

    /* 4. 时间太短（<20ms）→ 立即到位 */
    if (time < 20)
    {
        duoji_doing[index].aim = aim;
        duoji_doing[index].cur = aim;
        duoji_doing[index].inc = 0;
        // printf("#%03dP%04dT%04d!", (u16)index, (u16)aim, 0);
    }
    /* 5. 正常平滑运动：计算 20 ms 一次增量 inc */
    else
    {
        duoji_doing[index].aim  = aim;
        duoji_doing[index].time = time;
        duoji_doing[index].inc  = (aim - duoji_doing[index].cur) / (time / 20.0f);
        // printf("#%03dP%04dT%04d!", (u16)index, (u16)aim, (u16)time);
    }
}

/*----------------------------------------------------------
 * 函数名称：PwmServo_IncOffset
 * 功    能：在 20 ms 周期任务里调用，让舵机“走一步”
 * 参    数：index 0~7
 * 返 回 值：无
 *----------------------------------------------------------*/
void PwmServo_IncOffset(u8 index)
{
    int aim_temp;

    if (duoji_doing[index].inc == 0) return; // 无需移动

    aim_temp = duoji_doing[index].aim;

    /* 1. 再次限幅（冗余保护） */
    if (aim_temp > 2490) aim_temp = 2490;
    else if (aim_temp < 500) aim_temp = 500;

    /* 2. 判断是否到达目标，若到达则停止 */
    if (abs_float(aim_temp - duoji_doing[index].cur) <= abs_float(duoji_doing[index].inc * 2))
    {
        duoji_doing[index].cur = aim_temp;
        duoji_doing[index].inc = 0;
    }
    /* 3. 未到达，累加增量 */
    else
    {
        duoji_doing[index].cur += duoji_doing[index].inc;
    }
}

/*----------------------------------------------------------
 * 函数名称：PwmServo_BiasSet
 * 功    能：单独给某舵机设置“偏差”参数（用于校准舵机中位）
 * 参    数：index 0~7   bias 偏差值
 * 返 回 值：无
 *----------------------------------------------------------*/
void PwmServo_BiasSet(uint8_t index, int bias)
{
    if (index >= DJ_NUM) return;
    duoji_doing[index].bias = bias;
    duoji_doing[index].inc  = 0.001f; // 给一个极小增量，防止除 0
}

/*----------------------------------------------------------
 * 函数名称：PwmServoStop
 * 功    能：停止指定舵机或全部舵机
 * 参    数：index 0~7 指定舵机；255 表示全部
 * 返 回 值：无
 *----------------------------------------------------------*/
void PwmServoStop(u8 index)
{
    if (index < DJ_NUM) // 单路停止
    {
        duoji_doing[index].inc = 0;
        duoji_doing[index].aim = duoji_doing[index].cur;
    }
    else if (index == 255) // 全部停止
    {
        for (index = 0; index < DJ_NUM; index++)
        {
            duoji_doing[index].inc = 0;
            duoji_doing[index].aim = duoji_doing[index].cur;
        }
    }
}

float fix_raio = 0.99;
/**********************************************************
 * @brief 设置舵机角度
 * @param angle: float -> [0, 180]
 **********************************************************/
void PwmServo_SetAngle(uint8_t index, float angle,float Servo_angle){ 
    angle = fmax(0, fmin(angle, Servo_angle));

    angle *= fix_raio;

    float duty = 500 + (angle/Servo_angle)*2000;
    
    PwmServo_DoingSet(index, duty, 1000);
}
