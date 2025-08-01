/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    实现的功能：LED闪烁
	
    LED引脚：
    LED-PA15
    
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "SysTick.h"
#include "sys.h"
#include "Led.h"

int main(void) {	
    
    SWJGpio_Init(); //仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
    LED_Init();     //LED初始化
    
	while(1) {
        // 闪烁LED
        LED_TOGGLE();  
        Delay_ms(500);  
	}
}



