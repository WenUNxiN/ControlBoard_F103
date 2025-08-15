#include "sys.h"
#include "core_cm3.h"
#include "stm32f10x_iwdg.h"

/***********************************************
	仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
 ***********************************************/
void SWJGpio_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

/***********************************************
	函数名称:	Soft_Reset()
	功能介绍:	单片机软件复位
	函数参数:	无
	返回值:		无
 ***********************************************/
void Soft_Reset(void)
{
	// 关闭所有中断
	__set_FAULTMASK(1);
	// 复位
	NVIC_SystemReset();
}

/**
 * 初始化独立看门狗
 * prer:分频数:0~7(只有低 3 位有效!)
 * 分频因子=4*2^prer.但最大值只能是 256!
 * rlr:重装载寄存器值:低 11 位有效.
 * 时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
 */
void IWDG_Init(void)
{
	/*写入0x5555,用于允许狗狗寄存器写入功能*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	/* 喂狗时间 3s/6.4MS=468 .注意不能大于0xfff*/
	IWDG_SetReload(468);
	/* 喂狗 */
	IWDG_ReloadCounter();
	/* 使能狗狗 */
	IWDG_Enable();
}
