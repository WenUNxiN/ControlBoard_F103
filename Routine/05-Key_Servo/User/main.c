/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
	
    ʵ�ֵĹ��ܣ�����KEY���ת��500��s����������ص�500
                PWM ���� 500 ��s �C 2500 ��s ����ӳ�� 270����
                ? 500 ��s �� 0�㣨��С�Ƕȣ�
                ? 1500 ��s �� 135�㣨����λ��
                ? 2500 ��s �� 270�㣨���Ƕȣ�
    
    PWM������ţ�
    SERVO0-PB3
    SERVO1-PB4
    SERVO2-PB5
    SERVO3-PB6
    SERVO4-PB7
    SERVO5-PB8
    SERVO6-PB9

    KEY���ţ�
    KEY-PB2
    
    ��Ƶ��72M
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h"
#include "SysTick.h"
#include "rcc.h"
#include "sys.h"
#include "Led.h"
#include "Key.h"
#include "PwmServo.h" 
#include "Timer.h"

int main(void) {	
    
    Rcc_Init();         //ʱ�ӳ�ʼ��
    SWJGpio_Init();     //��SWDģʽ - ����JTAG���ܣ��ͷ�PA15, PB3, PB4
    SysTickInit(); 		//ϵͳʱ���ʼ��
    LED_Init();         //LED��ʼ��
    Key_Init();         //������ʼ��
    PwmServoInit();     //PWM�����ʼ��
    // ���� APB1=72 MHz��Ԥ��Ƶ 72-1���Զ���װ�� 20000-1 �� 50 Hz
    TIM2_Init(20000, 72 - 1); /* ��ʼ����ʱ��2������pwm���ƶ�� */
    
    int num = 0;
    
	while(1) {
        
        if(Key_Scan() == KEY_PRESS)   // ��⡰���¡��ɿ���һ����������
        {
            num += 500;                     //ÿ������500us
            if(num > 2500) num = 500;       //��λ
            PwmServo_DoingSet(0, num, 1000);//���ö��0λ��
            PwmServo_DoingSet(1, num, 1000);//���ö��1λ��
            PwmServo_DoingSet(2, num, 1000);//���ö��2λ��
            PwmServo_DoingSet(3, num, 1000);//���ö��3λ��
            PwmServo_DoingSet(4, num, 1000);//���ö��4λ��
            PwmServo_DoingSet(5, num, 1000);//���ö��5λ��
            PwmServo_DoingSet(6, num, 1000);//���ö��6λ��
        }
	}
}



