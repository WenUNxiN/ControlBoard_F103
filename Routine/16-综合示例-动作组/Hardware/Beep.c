#include "Beep.h"
#include "SysTick.h"

/*
 * ��������Buzzer_Init
 * ��  ������ʼ�����������õ��� GPIO
 * ��  ������
 * ��  �أ���
 */
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;          // ���� GPIO ��ʼ���ṹ��

    /* 1. �򿪷������������ڵ� GPIO ʱ�� */
    RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);

    /* 2. ���÷��������� */
    GPIO_InitStructure.GPIO_Pin   = BUZZER_GPIO_PIN;      // ѡ�����������
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;     // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // IO �ڷ�ת�ٶ� 50 MHz
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);     // ���ÿ⺯���������

    /* 3. Ĭ�Ϲرշ�����*/
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
}

/*
 * ��������Buzzer_times
 * ��  �����÷������Թ̶�ռ�ձ������ɴ�
 * ��  ����
 *     time  ��ÿ�Ρ��֡���ડ�������ʱ������λ ms
 *     count ������ٴΣ������һ�Σ�
 * ��  �أ���
 */
void Buzzer_times(u32 time, u32 count)
{
    for (int i = 0; i < count; i++)
    {
        BUZZER_ON();          
        Delay_ms(time);   
        BUZZER_OFF();         
        Delay_ms(time);       
    }
}
