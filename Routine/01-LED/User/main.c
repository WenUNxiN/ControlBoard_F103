/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    ʵ�ֵĹ��ܣ�LED��˸
	
    LED���ţ�
    LED-PA15
    
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "SysTick.h"
#include "sys.h"
#include "Led.h"

int main(void) {	
    
    SWJGpio_Init(); //��SWDģʽ - ����JTAG���ܣ��ͷ�PA15, PB3, PB4
    LED_Init();     //LED��ʼ��
    
	while(1) {
        // ��˸LED
        LED_TOGGLE();  
        Delay_ms(500);  
	}
}



