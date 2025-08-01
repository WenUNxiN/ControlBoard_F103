#include "Key.h"

/****************************************************************
 * �������ƣ�Key_Init
 * ��    �ܣ���ʼ��������Ӧ��GPIO����
 * ��    ������
 * �� �� ֵ����
 * ��ע��Ϣ��
 *   1. ʹ�ܰ������ڶ˿ڵ�ʱ��
 *   2. ���ð�������Ϊ���������롱ģʽ��GPIO_Mode_IPD��
 *      Ĭ�ϱ��������͵�ƽ������δ����ʱΪ0
 ****************************************************************/
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                /* ����GPIO��ʼ���ṹ����� */

    /* ʹ�ܰ����˿ڵ�ʱ�� */
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLK, ENABLE);

    /* ���ð������Ų��� */
    GPIO_InitStructure.GPIO_Pin  = KEY_GPIO_PIN;        /* ѡ�񰴼����� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;       /* ��������ģʽ���ް���ʱ��ƽΪ�� */

    /* ���ÿ⺯�����GPIO��ʼ�� */
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStructure);
}

/****************************************************************
 * �������ƣ�Key_Scan
 * ��    �ܣ�������ʽ����ɨ�裨����������
 * ��    ������
 * �� �� ֵ��u8
 *            KEY_PRESS   ���� ��⵽һ�Ρ����¡��ͷš�����������
 *            KEY_RELEASE ���� ��ǰ����δ���»��������״̬
 * ��ע��Ϣ��
 *   1. ʹ��������̬������¼��һ�κͱ��εĵ�ƽ״̬
 *   2. ������һ��Ϊ����(KEY_PRESS)�ұ���Ϊ�ͷ�(KEY_RELEASE)ʱ��
 *      ��Ϊ���һ����Ч�������������� KEY_PRESS
 *   3. ������������� KEY_RELEASE��ʵ�֡����ּ�⡱
 ****************************************************************/
u8 Key_Scan(void)
{
    static u8 Key_Flag     = KEY_RELEASE;   /* ��ǰ��ƽ״̬ */
    static u8 Key_LastFlag = KEY_RELEASE;   /* ��һ�ε�ƽ״̬ */

    /* ������һ��״̬������ȡ��ǰ���ŵ�ƽ */
    Key_LastFlag = Key_Flag;
    Key_Flag = GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN);

    /* ���ڡ���һ�ΰ��¡������ͷš���˲�䷵�� KEY_PRESS */
    if (Key_LastFlag == KEY_PRESS && Key_Flag == KEY_RELEASE)
    {
        return KEY_PRESS;
    }

    /* �����������Ϊδ������Ч�������� */
    return KEY_RELEASE;
}
	
