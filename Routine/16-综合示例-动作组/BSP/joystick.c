#include "joystick.h"

void Joystick_LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(JOYSTICK_LED_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JOYSTICK_LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(JOYSTICK_LED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(JOYSTICK_LED_PORT, JOYSTICK_LED_PIN);
}


void Joystick_Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(JOYSTICK_KEY_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JOYSTICK_KEY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(JOYSTICK_KEY_PORT, &GPIO_InitStructure);
}

/******************************************************************
 * �� �� �� �ƣ�ADC_Joystick_Init
 * ��       �ܣ���ʼ�� PA0/PA6 Ϊģ�����룬ADC1 ��������
 * ��MOD��   : ȥ�� DMA����Ϊ�����������
 ******************************************************************/
void ADC_Joystick_Init(void)
{
    Joystick_LED_Init();
    Joystick_Key_Init();
    /* 1. ʱ�� ---------------------------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_VRXY,      ENABLE);   // GPIOA
    RCC_APB2PeriphClockCmd(RCC_VRXY_ADC,  ENABLE);   // ADC1

    /* 2. GPIO ���� ----------------------------------------------*/
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_VRX | GPIO_VRY;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;    // ģ������
    GPIO_Init(PORT_VRXY, &GPIO_InitStructure);

    /* 3. ADC �������� -------------------------------------------*/
    ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode       = DISABLE;  // ��ͨ��
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;  // ��MOD�� ����
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel       = 1;        // ��ͨ��
    ADC_Init(PORT_VRXY_ADC, &ADC_InitStruct);

    /* 4. ADC ʱ�� 12 MHz ----------------------------------------*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /* 5. ʹ�� ADC ��У׼ ----------------------------------------*/
    ADC_Cmd(PORT_VRXY_ADC, ENABLE);
    ADC_ResetCalibration(PORT_VRXY_ADC);
    while (ADC_GetResetCalibrationStatus(PORT_VRXY_ADC));
    ADC_StartCalibration(PORT_VRXY_ADC);
    while (ADC_GetCalibrationStatus(PORT_VRXY_ADC));
}

/******************************************************************
 * �� �� �� �ƣ�Get_Adc_Joystick_Value
 * ��       �ܣ���ȡָ��ͨ����PA0 �� PA6���� ADC ƽ��ֵ
 * ��MOD��   : ȥ�� DMA��������� + ��ֵ
 ******************************************************************/
unsigned int Get_Adc_Joystick_Value(uint8_t CHx)
{
    uint32_t sum = 0;

    /* ���ñ���Ҫת����ͨ�� */
    ADC_RegularChannelConfig(PORT_VRXY_ADC, CHx, 1, ADC_SampleTime_55Cycles5);

    /* �������� SAMPLES �Σ�ȡƽ�� */
    for (uint8_t i = 0; i < SAMPLES; i++)
    {
        ADC_SoftwareStartConvCmd(PORT_VRXY_ADC, ENABLE);          // ����ת��
        while (ADC_GetFlagStatus(PORT_VRXY_ADC, ADC_FLAG_EOC) == RESET); // �ȴ����
        sum += ADC_GetConversionValue(PORT_VRXY_ADC);             // �����
    }

    return (sum + SAMPLES / 2) / SAMPLES;  // ��������
}

unsigned int Get_Joystick_Percentage_value(char dir)
{
      int adc_new = 0;
      int Percentage_value = 0;

      if( dir == 0)
      {
              adc_new = Get_Adc_Joystick_Value(CHANNEL_VRX_ADC);
      }
      else
      {
              adc_new = Get_Adc_Joystick_Value(CHANNEL_VRY_ADC);
      }

      Percentage_value = ((float)adc_new/4095.0f) * 100.f;
      return Percentage_value;
}

JoystickPos_t JoystickStatus(void)
{
    uint8_t xVal = Get_Joystick_Percentage_value(0);  // X ��
    uint8_t yVal = Get_Joystick_Percentage_value(1);  // Y ��

    /* ����/��ֵ���壨�ɵ��� */
    const uint8_t TH_MIN = 15;   // 0~15 ��Ϊ��/��
    const uint8_t TH_MAX = 85;   // 85~100 ��Ϊ��/��
    /* �м� 15~85 ��Ϊ���Ĵ� */

    /* �ж� Y �᣺�� / �� / �� */
    if (yVal >= TH_MAX)          return POS_LEFT;
    else if (yVal <= TH_MIN)     return POS_RIGHT;

    /* �ж� X �᣺�� / �� / �� */
    if (xVal <= TH_MIN)          return POS_DOWN;
    else if (xVal >= TH_MAX)     return POS_UP;

    /* �������Ĭ��Ϊ�� */
    return POS_CENTER;
}
