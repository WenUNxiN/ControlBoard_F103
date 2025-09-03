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
    static u32 last_tick = 0;
    static u8 last_mode = 0xFF; // 0xFF ��֤��һ�α�ˢ��

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
    case 0: OLED_Print(32, 3, "����˼��");break;
    case 1: OLED_Print(10, 3, "��������ɫ�ּ�");break;
    case 2: OLED_Print(53, 3, "ҡ��");break;
    case 3: OLED_Print(42, 3, "������");break;
    case 4: OLED_Print(30, 3, "����/����");break;
    case 5: OLED_Print(20, 3, "�Ӿ���ɫ�ּ�");break;
    case 6: OLED_Print(32, 3, "��ɫ���");break;
    case 7: OLED_Print(32, 3, "��ɫ׷��");break;
    case 8: OLED_Print(32, 3, "����׷��");break;
    case 9: OLED_Print(22, 3, "��ά��ּ�");break;
    case 10:OLED_Print(25, 3, "��ά�����");break;
    case 11:OLED_Print(25, 3, "��ά��׷��");break;
    case 12:OLED_Print(32, 3, "���ַּ�");break;
    case 13:OLED_Print(32, 3, "����׷��");break;
    case 14:OLED_Print(32, 3, "�����ּ�");break;
    default:
        break;
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
