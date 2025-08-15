#include "Led.h"   

/****************************************************************
 * �������ƣ�LED_Init
 * ��    �ܣ���ʼ��LED���ţ�������LEDĬ��״̬
 * ��    ������
 * �� �� ֵ����
 ****************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                    // ����GPIO��ʼ���ṹ�����
    
    /* 1. ʹ��ʱ�� */
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
    
    /* 2. ��������Ϊ�������ģʽ */
    GPIO_InitStructure.GPIO_Pin   = LED_GPIO_PIN;           // ѡ������
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;       // �������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // ����ٶ�50MHz
    
    /* 3. ���ÿ⺯�����GPIO��ʼ�� */
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
    
    /* 4. Ĭ��Ϩ��LED���������øߵ�ƽ���͵�ƽ�������ߵ�ƽϨ�� */
    GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
}
