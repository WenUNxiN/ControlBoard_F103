/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
	
    实现的功能：按下KEY，翻转LED状态
	
    LED引脚：
    LED-PA15
    
    KEY引脚：
    KEY-PB2
    
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "SysTick.h"
#include "sys.h"
#include "Led.h"
#include "Key.h"

int main(void) {	
    
    SWJGpio_Init(); //仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
    LED_Init();     //LED初始化
    Key_Init();
    
	while(1) {
        
        /* 每 20 ms 扫描一次按键 */
        if(Key_Scan() == KEY_PRESS)   // 检测“按下→松开”一次完整动作
        {
            LED_TOGGLE();             // LED 状态翻转
        }

        Delay_ms(20);                 // 简单消抖延时
	}
}



