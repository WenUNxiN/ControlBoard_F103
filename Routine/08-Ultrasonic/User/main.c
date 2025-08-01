/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    实现的功能：超声波测距并将数据显示在OLED上
	
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
    Rcc_Init();     // 时钟初始化
    SWJGpio_Init(); // 仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
    SysTickInit();  // 系统时间初始化
    US_Init();
    OLED_Init();    // 初始化OLED
    OLED_Clear();   // OLED清空

    OLED_Print(0, 0, "艾米思悠");
    OLED_Print(0, 2, "num:");
    
    float num = 0.0f;
    while (1)
    {
        num = US_DisRead();             //读取距离
        OLED_ShowNum(30, 3, num, 4, 15); // 显示
        Delay_ms(50);
    }
}
