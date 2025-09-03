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

void loop_oled(void)
{
    static u32 last_tick = 0;
    static u8 last_mode = 0xFF; // 0xFF 保证第一次必刷新

    /* 多久刷新一次（可选，想更快可调小） */
    if (Millis() - last_tick < 800)
        return;
    last_tick = Millis();

    /* 模式未改变 → 不做任何事 */
    if (SetMode == last_mode)
        return;

    /* 模式已变化：清屏 + 重新显示 */
    last_mode = SetMode;
    OLED_Clear();

    switch (SetMode)
    {
    case 0: OLED_Print(32, 3, "艾米思悠");break;
    case 1: OLED_Print(10, 3, "传感器颜色分拣");break;
    case 2: OLED_Print(53, 3, "摇杆");break;
    case 3: OLED_Print(42, 3, "超声波");break;
    case 4: OLED_Print(30, 3, "声音/触摸");break;
    case 5: OLED_Print(20, 3, "视觉颜色分拣");break;
    case 6: OLED_Print(32, 3, "颜色码垛");break;
    case 7: OLED_Print(32, 3, "颜色追踪");break;
    case 8: OLED_Print(32, 3, "人脸追踪");break;
    case 9: OLED_Print(22, 3, "二维码分拣");break;
    case 10:OLED_Print(25, 3, "二维码码垛");break;
    case 11:OLED_Print(25, 3, "二维码追踪");break;
    case 12:OLED_Print(32, 3, "数字分拣");break;
    case 13:OLED_Print(32, 3, "数字追踪");break;
    case 14:OLED_Print(32, 3, "垃圾分拣");break;
    default:
        break;
    }
}

int main(void)
{
    setup_app();
    //    OLED_DrawBMP(0, 0, 128, 8, BMP);  // 画位图
    
    while (1)
    {
        loop_oled();
        loop_app();
    }
}
