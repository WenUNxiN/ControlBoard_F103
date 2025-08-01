// ========================= ͷ�ļ� =========================
#include "PwmServo.h"   // ���ļ���Ӧ��ͷ�ļ��������ꡢ���Ͷ����
#include "Global.h"     // ȫ�ֱ���/�궨��
#include <math.h>
// ========================= ȫ�ֱ��� =========================
servo_t duoji_doing[DJ_NUM];   // ���� 0~7 �Ŷ���ġ������С�������Ŀ��ֵ����ǰֵ��������ִ��ʱ�䡢ƫ��ȣ�

// ========================= ����ʵ�� =========================

/*----------------------------------------------------------
 * �������ƣ�PwmServoInit
 * ��    �ܣ���� GPIO ��ʼ����������� 50 MHz��
 * ��    ������
 * �� �� ֵ����
 *----------------------------------------------------------*/
void PwmServoInit(void)
{
    u8 i;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. ʹ�����ö˿ڵ� RCC ʱ��
    RCC_APB2PeriphClockCmd(SERVO0_GPIO_CLK | SERVO1_GPIO_CLK | SERVO2_GPIO_CLK |
                           SERVO3_GPIO_CLK | SERVO4_GPIO_CLK | SERVO5_GPIO_CLK |
                           SERVO6_GPIO_CLK, ENABLE);

    // 2. ͳһ�ṹ�������50 MHz �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;

    // 3. ���γ�ʼ�� 7 · GPIO������ͷ�ļ����壩
    GPIO_InitStructure.GPIO_Pin = SERVO0_PIN;  GPIO_Init(SERVO0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO1_PIN;  GPIO_Init(SERVO1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO2_PIN;  GPIO_Init(SERVO2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO3_PIN;  GPIO_Init(SERVO3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO4_PIN;  GPIO_Init(SERVO4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO5_PIN;  GPIO_Init(SERVO5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SERVO6_PIN;  GPIO_Init(SERVO6_GPIO_PORT, &GPIO_InitStructure);

    // 4. ��ʼ��ȫ�ֽṹ�����飺Ĭ��Ŀ�� 1500us����ǰ 1500us������ 20��ʱ�� 5000ms
    for (i = 0; i < DJ_NUM; i++)
    {
        duoji_doing[i].aim  = 1500;
        duoji_doing[i].cur  = 1500;
        duoji_doing[i].inc  = 20;
        duoji_doing[i].time = 5000;
    }
}

/*----------------------------------------------------------
 * �������ƣ�PwmServo_PinSet
 * ��    �ܣ�ֱ������ĳ��������ŵ�ƽ��������� PWM��
 * ��    ����index 0~7   level 1=�� 0=��
 * �� �� ֵ����
 *----------------------------------------------------------*/
void PwmServo_PinSet(u8 index, BitAction level)
{
    switch (index)
    {
        case 0: SERVO0_PIN_SET(level); break;
        case 1: SERVO1_PIN_SET(level); break;
        case 2: SERVO2_PIN_SET(level); break;
        case 3: SERVO3_PIN_SET(level); break;
        case 4: SERVO4_PIN_SET(level); break;
        case 5: SERVO5_PIN_SET(level); break;
        case 6: SERVO6_PIN_SET(level); break;
//        case 7: SERVO7_PIN_SET(level); break;
        default: break;
    }
}

/*----------------------------------------------------------
 * �������ƣ�PwmServo_DoingSet
 * ��    �ܣ���ָ������·���ƽ���˶���ָ��
 * ��    ����index 0~6
 *            aim   Ŀ������ 500~2500 us��0 ��ʾֹͣ��
 *            time  ��ɶ�������ʱ�� 0~10000 ms
 * �� �� ֵ����
 * ע    �⣺�ڲ������ÿ 20 ms ��Ҫ����/���ٵ����� inc
 *----------------------------------------------------------*/
void PwmServo_DoingSet(u8 index, int aim, int time)
{
    if (index >= DJ_NUM) return;          // Խ�籣��

    /* 1. aim==0 ��Ϊֹͣ */
    if (aim == 0)
    {
        duoji_doing[index].inc = 0;       // ���ٵ���
        duoji_doing[index].aim = duoji_doing[index].cur;
        return;
    }

    /* 2. �޷� */
    if (aim > 2490) aim = 2490;
    else if (aim < 510) aim = 510;

    if (time > 10000) time = 9999;

    /* 3. ����ǰֵ�ѵ���Ŀ��ֵ��Ϊ��ֹ�� 0����Ϊ����΢Сƫ�ƣ�0.0077�� */
    if (duoji_doing[index].cur == aim)
    {
        aim += 0.0077f;
    }

    /* 4. ʱ��̫�̣�<20ms���� ������λ */
    if (time < 20)
    {
        duoji_doing[index].aim = aim;
        duoji_doing[index].cur = aim;
        duoji_doing[index].inc = 0;
        // printf("#%03dP%04dT%04d!", (u16)index, (u16)aim, 0);
    }
    /* 5. ����ƽ���˶������� 20 ms һ������ inc */
    else
    {
        duoji_doing[index].aim  = aim;
        duoji_doing[index].time = time;
        duoji_doing[index].inc  = (aim - duoji_doing[index].cur) / (time / 20.0f);
        // printf("#%03dP%04dT%04d!", (u16)index, (u16)aim, (u16)time);
    }
}

/*----------------------------------------------------------
 * �������ƣ�PwmServo_IncOffset
 * ��    �ܣ��� 20 ms ������������ã��ö������һ����
 * ��    ����index 0~7
 * �� �� ֵ����
 *----------------------------------------------------------*/
void PwmServo_IncOffset(u8 index)
{
    int aim_temp;

    if (duoji_doing[index].inc == 0) return; // �����ƶ�

    aim_temp = duoji_doing[index].aim;

    /* 1. �ٴ��޷������ౣ���� */
    if (aim_temp > 2490) aim_temp = 2490;
    else if (aim_temp < 500) aim_temp = 500;

    /* 2. �ж��Ƿ񵽴�Ŀ�꣬��������ֹͣ */
    if (abs_float(aim_temp - duoji_doing[index].cur) <= abs_float(duoji_doing[index].inc * 2))
    {
        duoji_doing[index].cur = aim_temp;
        duoji_doing[index].inc = 0;
    }
    /* 3. δ����ۼ����� */
    else
    {
        duoji_doing[index].cur += duoji_doing[index].inc;
    }
}

/*----------------------------------------------------------
 * �������ƣ�PwmServo_BiasSet
 * ��    �ܣ�������ĳ������á�ƫ�����������У׼�����λ��
 * ��    ����index 0~7   bias ƫ��ֵ
 * �� �� ֵ����
 *----------------------------------------------------------*/
void PwmServo_BiasSet(uint8_t index, int bias)
{
    if (index >= DJ_NUM) return;
    duoji_doing[index].bias = bias;
    duoji_doing[index].inc  = 0.001f; // ��һ����С��������ֹ�� 0
}

/*----------------------------------------------------------
 * �������ƣ�PwmServoStop
 * ��    �ܣ�ָֹͣ�������ȫ�����
 * ��    ����index 0~7 ָ�������255 ��ʾȫ��
 * �� �� ֵ����
 *----------------------------------------------------------*/
void PwmServoStop(u8 index)
{
    if (index < DJ_NUM) // ��·ֹͣ
    {
        duoji_doing[index].inc = 0;
        duoji_doing[index].aim = duoji_doing[index].cur;
    }
    else if (index == 255) // ȫ��ֹͣ
    {
        for (index = 0; index < DJ_NUM; index++)
        {
            duoji_doing[index].inc = 0;
            duoji_doing[index].aim = duoji_doing[index].cur;
        }
    }
}

float fix_raio = 0.99;
/**********************************************************
 * @brief ���ö���Ƕ�
 * @param angle: float -> [0, 180]
 **********************************************************/
void PwmServo_SetAngle(uint8_t index, float angle,float Servo_angle){ 
    angle = fmax(0, fmin(angle, Servo_angle));

    angle *= fix_raio;

    float duty = 500 + (angle/Servo_angle)*2000;
    
    PwmServo_DoingSet(index, duty, 1000);
}
