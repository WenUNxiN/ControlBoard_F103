#include "Timer.h"
#include "PwmServo.h"
#include "ServoPID.h"

/***********************************************
��������:	TIM1_Init(u16 arr, u16 psc)
���ܽ���:	��ʼ��TIM1������PID���ƶ��
��������:	arr �������Զ���װֵ
					psc Ԥ��Ƶ��
����ֵ:		��
***********************************************/
void TIM1_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // ʱ�� TIM1 ʹ��
	// ��ʱ�� TIM1 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr;						// �����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// ����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ���벶���Ƶ������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM ���ϼ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				// �ڳ�ʼ�� TIM2
	TIM_ARRPreloadConfig(TIM1, DISABLE);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // ����������ж�

	// �ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;			  // TIM1 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ� 0 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // �����ȼ� 2 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // �ܳ�ʼ�� NVIC �Ĵ���
	TIM_Cmd(TIM1, ENABLE);									  // ��ʹ�� TIM1
}

/*************************************************
 * �������ƣ�TIM2_Init
 * ���ܽ��ܣ���ʼ�� TIM2���������ɶ������ PWM ����
 * ����˵����
 *   arr - �Զ���װ��ֵ��������ʱ���ڣ�
 *   psc - Ԥ��Ƶϵ������������Ƶ�ʣ�
 * ����ֵ����
 *************************************************/
void TIM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. ʹ�� TIM2 ʱ�ӣ�APB1 ���ߣ�
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 2. ���� TIM2 ʱ����Ԫ
    TIM_TimeBaseStructure.TIM_Period        = arr;                // �Զ���װ��ֵ��ARR��
    TIM_TimeBaseStructure.TIM_Prescaler     = psc;                // Ԥ��Ƶ����PSC��
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // ʱ�ӷ�Ƶ������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; // ���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 3. ���� ARR Ԥװ�أ�������Ч��
    TIM_ARRPreloadConfig(TIM2, DISABLE);

    // 4. ʹ�� TIM2 �����жϣ�UEV��
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 5. ���� NVIC �ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;   // �ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;          // ��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;          // ��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;     // ʹ���ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);

    // 6. ���� TIM2 ������
    TIM_Cmd(TIM2, ENABLE);
}

/* ��ʱ��1�жϺ�����������PID���Ʋ��� */
void TIM1_IRQHandler(void)
{
	static u8 duoji_index2 = 0;
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) // ��� TIM1 �����жϷ������
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ��� TIM1 �����жϱ�־
		while(duoji_index2<8)
		{	
			pid_doing[duoji_index2].cur=Position_PID(pid_doing[duoji_index2].cur,pid_doing[duoji_index2].aim);
//			duoji_doing[duoji_index2].cur=pid_doing[duoji_index2].cur; //��pid����ֵ����
			duoji_index2++;
		}
	}
}


/*************************************************
 * �������ƣ�TIM2_IRQHandler
 * ���ܽ��ܣ�TIM2 �жϷ��������������� 8 ·��� PWM ����
 * ʵ��ԭ��
 *   - ÿ�����ռ�� 2.5ms��2500 ��������
 *   - 8 ������� 20ms��20000 ������һ������
 *   - ʹ�� flag �л��ߵ͵�ƽ
 *   - ÿ���ж�ֻ����һ�������һ�β���
 *************************************************/
void TIM2_IRQHandler(void)
{
    static u8 flag = 0;         // 0���ߵ�ƽ�׶Σ�1���͵�ƽ�׶�
    static u8 duoji_index1 = 0; // ��ǰ����Ķ����ţ�0~7��
    int temp;

    // 1. �ж��Ƿ��������ж�
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        // 2. ������ 8 ���������������
        if (duoji_index1 == 8)
        {
            duoji_index1 = 0;
        }

        // 3. flag = 0���ߵ�ƽ�׶Σ������ȣ�
        if (flag == 0)
        {
            // ������һ���ж�ʱ��Ϊ��ǰ�������
            TIM2->ARR = (unsigned int)(duoji_doing[duoji_index1].cur);

            // ���øö������Ϊ�ߵ�ƽ
            PwmServo_PinSet(duoji_index1, Bit_SET);

            // ���¶����ǰֵ��ƽ���ƶ���
            PwmServo_IncOffset(duoji_index1);
        }
        // 4. flag = 1���͵�ƽ�׶Σ�ʣ��ʱ�䣩
        else
        {
            // ����ʣ��ʱ�䣺2.5ms - ����
            temp = 2500 - (unsigned int)(duoji_doing[duoji_index1].cur);

            // ������һ���ж�ʱ��Ϊʣ��ʱ��
            TIM2->ARR = temp;

            // ���øö������Ϊ�͵�ƽ
            PwmServo_PinSet(duoji_index1, Bit_RESET);

            // �л�����һ�����
            duoji_index1++;
        }

        // 5. �л� flag ״̬����/�͵�ƽ���棩
        flag = !flag;

        // 6. ����жϱ�־λ
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
