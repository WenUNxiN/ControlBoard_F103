/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    ʵ�ֵĹ��ܣ�OLED��ʾ
	
    OLED���ţ�
    SCL ���ţ�PC14 
    SDA ���ţ�PC15
    
    ��Ƶ72Mhz
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "OLED.h"

#include "bmp.h"

int main(void)
{
    Rcc_Init();     // ʱ�ӳ�ʼ��
    SWJGpio_Init(); // ��SWDģʽ - ����JTAG���ܣ��ͷ�PA15, PB3, PB4
    SysTickInit();  // ϵͳʱ���ʼ��
    OLED_Init();    // ��ʼ��OLED
    OLED_Clear();   // OLED���

    OLED_Print(32, 3, "����˼��");
    
//    OLED_DrawBMP(0, 0, 128, 8, BMP);  // ��λͼ

    while (1)
    {
        
    }
}
