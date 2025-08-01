/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    ʵ�ֵĹ��ܣ�ͨ�����������ƶ��������Խ������Ƕ�ԽС����֮Խ��
                ͬʱOLED��ʾ���롣
    
    PWM������ţ�
    SERVO0-PB3
    SERVO1-PB4
    SERVO2-PB5
    SERVO3-PB6
    SERVO4-PB7
    SERVO5-PB8
    SERVO6-PB9
    
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
    /* ---------- 1. ϵͳ�ײ��ʼ�� ---------- */
    Rcc_Init();     // ʹ�ܸ�����ʱ��
    SWJGpio_Init(); // �ر� JTAG���ͷ� PA15/PB3/PB4
    SysTickInit();  // ��ʼ�� 1 ms ϵͳ��ણ�Delay_ms ������

    /* ---------- 2. ����Ӳ����ʼ�� ---------- */
    US_Init();      // ������ HC-SR04 ����/ʱ����ʼ��
    OLED_Init();    // SSD1306 OLED ��� I2C ��ʼ��
    OLED_Clear();   // ��������ֹ��Ӱ
    PwmServoInit(); // ��� GPIO/PWM ���ų�ʼ��
    /*
     * TIM2��72 MHz / (PSC+1) = 1 MHz
     * ARR=20000 �� ���� 20 ms (50 Hz)�����϶����׼
     */
    TIM2_Init(20000, 72 - 1);

    /* ---------- 3. UI ��ʾ�̶����� ---------- */
    OLED_Print(0, 0, "����˼��"); 
    OLED_Print(0, 2, "num:");     

    /* ---------- 4. ��������������λ ---------- */
    float dist_cm = 0.0f; // ��ǰ���ֵ
    float pulse = 0.0f;   // ӳ����������(?s)

    /* �������ת����λ 1500 ?s������ 1000 ms ���ϵ��ȶ�ʱ�� */
    PwmServo_DoingSet(0, 1500, 1000);

    /* ---------- 5. ��ѭ����������ʾ��ӳ������ ---------- */
    while (1)
    {
        dist_cm = US_DisRead();              // ��ȡ��ǰ���� (cm)
        OLED_ShowNum(30, 3, dist_cm, 4, 15); // ��ʾ����

        pulse = map(dist_cm, 0, 20, 500, 2500); // 0~20 cm �� 500~2500 ?s
        pulse = CLAMP(pulse, 500, 2500);        // �޷�����Խ��

        PwmServo_DoingSet(0, pulse, 0); // ���� PWM ռ�ձ�
        PwmServo_DoingSet(1, pulse, 0); // ���� PWM ռ�ձ�
        PwmServo_DoingSet(2, pulse, 0); // ���� PWM ռ�ձ�
        PwmServo_DoingSet(3, pulse, 0); // ���� PWM ռ�ձ�
        PwmServo_DoingSet(4, pulse, 0); // ���� PWM ռ�ձ�
        PwmServo_DoingSet(5, pulse, 0); // ���� PWM ռ�ձ�
        PwmServo_DoingSet(6, pulse, 0); // ���� PWM ռ�ձ�
        Delay_ms(50);                   // 50 ms ˢ��
    }
}
