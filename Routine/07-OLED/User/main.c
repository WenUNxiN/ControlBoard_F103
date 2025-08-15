/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    实现的功能：OLED显示
	
    OLED引脚：
    SCL 引脚：PC14 
    SDA 引脚：PC15
    
    主频72Mhz
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "OLED.h"

#include "bmp.h"

int main(void)
{
    Rcc_Init();     // 时钟初始化
    SWJGpio_Init(); // 仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
    SysTickInit();  // 系统时间初始化
    OLED_Init();    // 初始化OLED
    OLED_Clear();   // OLED清空

    OLED_Print(32, 3, "艾米思悠");
    
//    OLED_DrawBMP(0, 0, 128, 8, BMP);  // 画位图

    while (1)
    {
        
    }
}
