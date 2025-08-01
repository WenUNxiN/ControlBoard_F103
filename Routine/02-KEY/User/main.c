/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
	
    ʵ�ֵĹ��ܣ�����KEY����תLED״̬
	
    LED���ţ�
    LED-PA15
    
    KEY���ţ�
    KEY-PB2
    
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "SysTick.h"
#include "sys.h"
#include "Led.h"
#include "Key.h"

int main(void) {	
    
    SWJGpio_Init(); //��SWDģʽ - ����JTAG���ܣ��ͷ�PA15, PB3, PB4
    LED_Init();     //LED��ʼ��
    Key_Init();
    
	while(1) {
        
        /* ÿ 20 ms ɨ��һ�ΰ��� */
        if(Key_Scan() == KEY_PRESS)   // ��⡰���¡��ɿ���һ����������
        {
            LED_TOGGLE();             // LED ״̬��ת
        }

        Delay_ms(20);                 // ��������ʱ
	}
}



