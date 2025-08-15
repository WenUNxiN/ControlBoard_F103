/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    ʵ�ֵĹ��ܣ�ͨ�����ڽ���ָ����ö���˶�
    
    PWM������ţ�
    SERVO0-PB3
    SERVO1-PB4
    SERVO2-PB5
    SERVO3-PB6
    SERVO4-PB7
    SERVO5-PB8
    SERVO6-PB9
    
    ����1���ţ�
    PA9-TX
    PA10-RX
    
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h"

int main(void)
{
    /* ---------- 1. ϵͳ�ײ��ʼ�� ---------- */
    Rcc_Init();         // ʹ����������ʱ��
    SWJGpio_Init();     // �ر� JTAG�������� SWD���ͷ� PA15/PB3/PB4
    SysTickInit();      // ���� 1 ms SysTick��Delay_ms �ã�
    Uart1_Init(115200); // ��ʼ�� USART1��PA9-TX��PA10-RX��

    /* ---------- 2. ��� PWM ��ʼ�� ---------- */
    PwmServoInit();           // ���ö�� IO Ϊ��������
    TIM2_Init(20000, 72 - 1); // TIM2��72 MHz/72 = 1 MHz �� ARR=20000 �� 50 Hz (20 ms)

    /* ---------- 3. ��ѭ��������������� ---------- */
    while (1)
    {
        if (Uart_GetOK) // �յ�����֡
        {
            switch (Uart_Mode) // ����֡ͷ����������ʽ
            {
            case 1: // �ı����� $XXX!
                Parse_Cmd(Uart_ReceiveBuf);
                break;

            case 2: // �����֡ #ID P PWM T TIME!
                Parse_Action(Uart_ReceiveBuf);
                break;

            case 3: // ����֡ {...}
                Parse_Action(Uart_ReceiveBuf);
                break;
            }

            Uart_Mode = 0;  // ��λ֡����
            Uart_GetOK = 0; // ���������һ֡
        }
    }
}
