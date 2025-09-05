/***************************************************************
 *  @作者  ： IMSY
 *  @日期  ： 2025-05-20
 *  @公司  ： 小艾机器人科技有限公司
 *  @文件  ： ArmControl
 *
 *  功能概述
 *  1. 7 路 PWM 舵机驱动
 *  2. 上位机（PC / 小程序）在线控制
 *  3. 脱机运行（动作组固化到 W25Q64）
 *
 *  引脚分配
 *  ├─ 舵机 0~6：PB3 / PB4 / PB5 / PB6 / PB7 / PB8 / PB9
 *  ├─ 蜂鸣器 ：PA3
 *  ├─ LED    ：PA15
 *  ├─ PS2 手柄：PA1(DAT)  PA7(CMD)  PB0(ATT)  PB1(CLK)
 *  ├─ 功能键  ：PB2
 *  └─ 串口   ：PA9(TX)  PA10(RX)
 *
 *  主频：72 MHz
 *  型号：STM32F103C8T6
 ***************************************************************/
#include "main.h"
#include "Application.h"
#include "bmp.h"

/*----------------------------
 * OLED 模式显示
 * 仅在 SetMode 改变时刷新一次
 *----------------------------*/
void loop_oled(void)
{
    static u32 last_tick = 0;
    static u8  last_mode = 0xFF;      // 初值设为 0xFF，保证第一次必刷新

    /* 刷新周期 800 ms（可调） */
    if (Millis() - last_tick < 800)
        return;
    last_tick = Millis();

    /* 模式未改变 → 直接返回，减少闪屏 */
    if (SetMode == last_mode)
        return;

    last_mode = SetMode;

    /* 清屏后打印新名称 */
    OLED_Clear();
    switch (SetMode)
    {
        case 0:  OLED_Print(32, 3, "艾米思悠");       break;  // 开机显示
        case 1:  OLED_Print(10, 3, "传感器颜色分拣"); break;
        case 2:  OLED_Print(53, 3, "摇杆");           break;
        case 3:  OLED_Print(42, 3, "超声波");         break;
        case 4:  OLED_Print(30, 3, "声音/触摸");      break;
        case 5:  OLED_Print(20, 3, "视觉颜色分拣");   break;
        case 6:  OLED_Print(32, 3, "颜色码垛");       break;
        case 7:  OLED_Print(32, 3, "颜色追踪");       break;
        case 8:  OLED_Print(32, 3, "人脸追踪");       break;
        case 9:  OLED_Print(22, 3, "二维码分拣");     break;
        case 10: OLED_Print(25, 3, "二维码码垛");     break;
        case 11: OLED_Print(25, 3, "二维码追踪");     break;
        case 12: OLED_Print(32, 3, "数字分拣");       break;
        case 13: OLED_Print(32, 3, "数字追踪");       break;
        case 14: OLED_Print(32, 3, "垃圾分拣");       break;
        case 15: OLED_Print(53, 3, "手柄");           break;
        default: break;
    }
}

/*----------------------------
 * 主函数
 *----------------------------*/
int main(void)
{
    setup_app();                // 系统初始化（见 Application.c）

    // OLED_DrawBMP(0, 0, 128, 8, BMP);  // 如需启动画面可启用

    while (1)
    {
        loop_oled();            // OLED 模式刷新
        loop_app();             // 应用主循环
    }
}
