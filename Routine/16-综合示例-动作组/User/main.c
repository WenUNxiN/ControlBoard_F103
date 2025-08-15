/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
	
	ʵ�ֵĹ��ܣ�
	1��7·PWM�������
	2����λ����PC/С���򣩿��ƶ��
	3�����ѻ��洢����
	
	PWM������ţ�
		SERVO0-PB3
		SERVO1-PB4
		SERVO2-PB5
		SERVO3-PB6
		
		SERVO4-PB7
		SERVO5-PB8
		SERVO6-PB9
		
	���������ţ�
		BEEP-PA3
        
	LED���ţ�
		LED-PA15
        
    PS2�ֱ����ţ�	
	  PS1-DAT-PA1
	  PS2-CMD-PA7
	  PS6-ATT-PB0
	  PS7-CLK-PB1
      
	�������ţ�
	  KEY-PB2
	
	�������ţ�
      PA9-TX
      PA10-RX
	
	��Ƶ��72M
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "Application.h"

#include "bmp.h"

void loop_oled(void)
{
    static u32  last_tick   = 0;
    static u8   last_mode   = 0xFF;      // 0xFF ��֤��һ�α�ˢ��

    /* ���ˢ��һ�Σ���ѡ�������ɵ�С�� */
    if (Millis() - last_tick < 800)
        return;
    last_tick = Millis();

    /* ģʽδ�ı� �� �����κ��� */
    if (SetMode == last_mode)
        return;

    /* ģʽ�ѱ仯������ + ������ʾ */
    last_mode = SetMode;
    OLED_Clear();

    switch (SetMode)
    {
        case 0:  OLED_Print(32, 3, "����˼��");     break;
        case 1:  OLED_Print(52, 3, "��ɫ");         break;
        case 2:  OLED_Print(43, 3, "������");       break;
        case 3:  OLED_Print(32, 3, "����/����");    break;
        case 4:  OLED_Print(52, 3, "ҡ��");         break;
        default: break;
    }
}

int main(void)
{
    setup_app();
//    OLED_DrawBMP(0, 0, 128, 8, BMP);  // ��λͼ
    
    while (1)
    {
        loop_oled();
        loop_app();
    }
}
