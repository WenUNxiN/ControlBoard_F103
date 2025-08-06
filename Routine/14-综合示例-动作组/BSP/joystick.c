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
 * 函 数 名 称：ADC_Joystick_Init
 * 功       能：初始化 PA0/PA6 为模拟输入，ADC1 基本配置
 * ★MOD★   : 去掉 DMA，改为单次软件触发
 ******************************************************************/
void ADC_Joystick_Init(void)
{
    Joystick_LED_Init();
    Joystick_Key_Init();
    /* 1. 时钟 ---------------------------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_VRXY,      ENABLE);   // GPIOA
    RCC_APB2PeriphClockCmd(RCC_VRXY_ADC,  ENABLE);   // ADC1

    /* 2. GPIO 配置 ----------------------------------------------*/
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_VRX | GPIO_VRY;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;    // 模拟输入
    GPIO_Init(PORT_VRXY, &GPIO_InitStructure);

    /* 3. ADC 基本配置 -------------------------------------------*/
    ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode       = DISABLE;  // 单通道
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;  // ★MOD★ 单次
    ADC_InitStruct.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel       = 1;        // 单通道
    ADC_Init(PORT_VRXY_ADC, &ADC_InitStruct);

    /* 4. ADC 时钟 12 MHz ----------------------------------------*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /* 5. 使能 ADC 并校准 ----------------------------------------*/
    ADC_Cmd(PORT_VRXY_ADC, ENABLE);
    ADC_ResetCalibration(PORT_VRXY_ADC);
    while (ADC_GetResetCalibrationStatus(PORT_VRXY_ADC));
    ADC_StartCalibration(PORT_VRXY_ADC);
    while (ADC_GetCalibrationStatus(PORT_VRXY_ADC));
}

/******************************************************************
 * 函 数 名 称：Get_Adc_Joystick_Value
 * 功       能：读取指定通道（PA0 或 PA6）的 ADC 平均值
 * ★MOD★   : 去掉 DMA，软件触发 + 均值
 ******************************************************************/
unsigned int Get_Adc_Joystick_Value(uint8_t CHx)
{
    uint32_t sum = 0;

    /* 配置本次要转换的通道 */
    ADC_RegularChannelConfig(PORT_VRXY_ADC, CHx, 1, ADC_SampleTime_55Cycles5);

    /* 连续采样 SAMPLES 次，取平均 */
    for (uint8_t i = 0; i < SAMPLES; i++)
    {
        ADC_SoftwareStartConvCmd(PORT_VRXY_ADC, ENABLE);          // 启动转换
        while (ADC_GetFlagStatus(PORT_VRXY_ADC, ADC_FLAG_EOC) == RESET); // 等待完成
        sum += ADC_GetConversionValue(PORT_VRXY_ADC);             // 读结果
    }

    return (sum + SAMPLES / 2) / SAMPLES;  // 四舍五入
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
    uint8_t xVal = Get_Joystick_Percentage_value(0);  // X 轴
    uint8_t yVal = Get_Joystick_Percentage_value(1);  // Y 轴

    /* 死区/阈值定义（可调） */
    const uint8_t TH_MIN = 15;   // 0~15 视为左/下
    const uint8_t TH_MAX = 85;   // 85~100 视为右/上
    /* 中间 15~85 视为中心带 */

    /* 判断 Y 轴：左 / 中 / 右 */
    if (yVal >= TH_MAX)          return POS_LEFT;
    else if (yVal <= TH_MIN)     return POS_RIGHT;

    /* 判断 X 轴：上 / 中 / 下 */
    if (xVal <= TH_MIN)          return POS_DOWN;
    else if (xVal >= TH_MAX)     return POS_UP;

    /* 其他情况默认为中 */
    return POS_CENTER;
}
