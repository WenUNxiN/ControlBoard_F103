/***************************************************************
 *  @����  �� IMSY
 *  @����  �� 2025-05-20
 *  @��˾  �� С�������˿Ƽ����޹�˾
 *  @�ļ�  �� ArmControl
 *
 *  ���ܸ���
 *  1. 7 · PWM �������
 *  2. ��λ����PC / С�������߿���
 *  3. �ѻ����У�������̻��� W25Q64��
 *
 *  ���ŷ���
 *  ���� ��� 0~6��PB3 / PB4 / PB5 / PB6 / PB7 / PB8 / PB9
 *  ���� ������ ��PA3
 *  ���� LED    ��PA15
 *  ���� PS2 �ֱ���PA1(DAT)  PA7(CMD)  PB0(ATT)  PB1(CLK)
 *  ���� ���ܼ�  ��PB2
 *  ���� ����   ��PA9(TX)  PA10(RX)
 *
 *  ��Ƶ��72 MHz
 *  �ͺţ�STM32F103C8T6
 ***************************************************************/
#include "main.h"
#include "Application.h"
#include "bmp.h"

/*----------------------------
 * OLED ģʽ��ʾ
 * ���� SetMode �ı�ʱˢ��һ��
 *----------------------------*/
void loop_oled(void)
{
    static u32 last_tick = 0;
    static u8  last_mode = 0xFF;      // ��ֵ��Ϊ 0xFF����֤��һ�α�ˢ��

    /* ˢ������ 800 ms���ɵ��� */
    if (Millis() - last_tick < 800)
        return;
    last_tick = Millis();

    /* ģʽδ�ı� �� ֱ�ӷ��أ��������� */
    if (SetMode == last_mode)
        return;

    last_mode = SetMode;

    /* �������ӡ������ */
    OLED_Clear();
    switch (SetMode)
    {
        case 0:  OLED_Print(32, 3, "����˼��");       break;  // ������ʾ
        case 1:  OLED_Print(10, 3, "��������ɫ�ּ�"); break;
        case 2:  OLED_Print(53, 3, "ҡ��");           break;
        case 3:  OLED_Print(42, 3, "������");         break;
        case 4:  OLED_Print(30, 3, "����/����");      break;
        case 5:  OLED_Print(20, 3, "�Ӿ���ɫ�ּ�");   break;
        case 6:  OLED_Print(32, 3, "��ɫ���");       break;
        case 7:  OLED_Print(32, 3, "��ɫ׷��");       break;
        case 8:  OLED_Print(32, 3, "����׷��");       break;
        case 9:  OLED_Print(22, 3, "��ά��ּ�");     break;
        case 10: OLED_Print(25, 3, "��ά�����");     break;
        case 11: OLED_Print(25, 3, "��ά��׷��");     break;
        case 12: OLED_Print(32, 3, "���ַּ�");       break;
        case 13: OLED_Print(32, 3, "����׷��");       break;
        case 14: OLED_Print(32, 3, "�����ּ�");       break;
        case 15: OLED_Print(53, 3, "�ֱ�");           break;
        default: break;
    }
}

/*----------------------------
 * ������
 *----------------------------*/
int main(void)
{
    setup_app();                // ϵͳ��ʼ������ Application.c��

    // OLED_DrawBMP(0, 0, 128, 8, BMP);  // �����������������

    while (1)
    {
        loop_oled();            // OLED ģʽˢ��
        loop_app();             // Ӧ����ѭ��
    }
}
