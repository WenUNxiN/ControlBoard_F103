#ifndef _ULTRASONIC__H_          // ��ֹ�ظ�����
#define _ULTRASONIC__H_

#include "main.h"                // ������ͷ�ļ����� HAL��stdint �ȣ�

/*---------------- ���ź궨�� ----------------*/
/* �������ţ�Trig��PA5 ��� */
#define Trig_Pin                GPIO_Pin_5        // �����ź������
#define Trig_GPIO_Port          GPIOA             // �����˿�
#define Trig_GPIO_CLK           RCC_APB2Periph_GPIOA   // GPIOA ʱ��

/* �������ţ�Echo��PA4 ���� */
#define Echo_Pin                GPIO_Pin_4        // �����ź������
#define Echo_GPIO_Port          GPIOA
#define Echo_GPIO_CLK           RCC_APB2Periph_GPIOA

/*---------------- λ�������� ----------------*/
// ���� Trig ���ŵ�ƽ��TRIG_SET(1) �ߵ�ƽ��TRIG_SET(0) �͵�ƽ
#define TRIG_SET(x)     GPIO_WriteBit(Trig_GPIO_Port, Trig_Pin, (BitAction)x)

// ��ȡ Echo ���ŵ�ƽ������ 0 �� 1
#define ECHO_GET()      GPIO_ReadInputDataBit(Echo_GPIO_Port, Echo_Pin)

/*---------------- ����ӿ����� ----------------*/
void US_Init(void);          // ��ʼ�� Trig��Echo ���ż�ʱ��
float US_DisRead(void);      // ��ȡһ�ξ��룬��λ cm����Χ 2 cm ~ 400 cm��

#endif

