#ifndef _PWMSERVO__H_
#define _PWMSERVO__H_
#include "sys.h"

// ���Ŷ���
#define SERVO0_PIN GPIO_Pin_3
#define SERVO0_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO0_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

#define SERVO1_PIN GPIO_Pin_4
#define SERVO1_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO1_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

#define SERVO2_PIN GPIO_Pin_5
#define SERVO2_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO2_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

#define SERVO3_PIN GPIO_Pin_6
#define SERVO3_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO3_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

#define SERVO4_PIN GPIO_Pin_7
#define SERVO4_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO4_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

#define SERVO5_PIN GPIO_Pin_8
#define SERVO5_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO5_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

#define SERVO6_PIN GPIO_Pin_9
#define SERVO6_GPIO_PORT GPIOB               /* GPIO�˿� */
#define SERVO6_GPIO_CLK RCC_APB2Periph_GPIOB /* GPIO�˿�ʱ�� */

/* ���ƶ����������ĺ� */
#define SERVO0_PIN_SET(level) GPIO_WriteBit(SERVO0_GPIO_PORT, SERVO0_PIN, level)
#define SERVO1_PIN_SET(level) GPIO_WriteBit(SERVO1_GPIO_PORT, SERVO1_PIN, level)
#define SERVO2_PIN_SET(level) GPIO_WriteBit(SERVO2_GPIO_PORT, SERVO2_PIN, level)
#define SERVO3_PIN_SET(level) GPIO_WriteBit(SERVO3_GPIO_PORT, SERVO3_PIN, level)
#define SERVO4_PIN_SET(level) GPIO_WriteBit(SERVO4_GPIO_PORT, SERVO4_PIN, level)
#define SERVO5_PIN_SET(level) GPIO_WriteBit(SERVO5_GPIO_PORT, SERVO5_PIN, level)
#define SERVO6_PIN_SET(level) GPIO_WriteBit(SERVO6_GPIO_PORT, SERVO6_PIN, level)

#define DJ_NUM 8 /* ������� */

typedef struct
{
    uint16_t aim;  // ִ��Ŀ��
    uint16_t time; // ִ��ʱ��
    float cur;     // ��ǰֵ
    float inc;     // ����
    int bias;      // ƫ��
} servo_t;

extern servo_t duoji_doing[DJ_NUM];

void PwmServoInit(void);                             /* ������ų�ʼ�� */
void PwmServo_PinSet(u8 index, BitAction level);     /* ���ö�����ŵ�ƽ */
void PwmServo_DoingSet(u8 index, int aim, int time); /* ���ö������ */
void PwmServo_IncOffset(u8 index);                   // �������
void PwmServo_BiasSet(uint8_t index, int bias);      // ���ö��ƫ�����
void PwmServoStop(u8 index);                         // ���ֹͣ
void PwmServo_SetAngle(uint8_t index, float angle,float Servo_angle);  // ���ö���Ƕ�
#endif
