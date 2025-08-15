/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    ʵ�ֵĹ��ܣ���������ಢ��������ʾ��OLED��
	
    OLED���ţ�
    SCL-PC14
    SDA-PC15
    
    ���������ţ�
    ��Echo��PA4
    ��Trig��PA5
    
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/

#include "main.h"

int main(void)
{
    Rcc_Init();     // ʱ�ӳ�ʼ��
    SWJGpio_Init(); // ��SWDģʽ - ����JTAG���ܣ��ͷ�PA15, PB3, PB4
    SysTickInit();  // ϵͳʱ���ʼ��
    US_Init();
    OLED_Init();    // ��ʼ��OLED
    OLED_Clear();   // OLED���

    OLED_Print(0, 0, "����˼��");
    OLED_Print(0, 2, "num:");
    
    float num = 0.0f;
    while (1)
    {
        num = US_DisRead();             //��ȡ����
        OLED_ShowNum(30, 3, num, 4, 15); // ��ʾ
        Delay_ms(50);
    }
}
