/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
  功能描述：按下PS2手柄串口会输出对应的数据。
	
  PS2手柄引脚：	
	  PS1-DAT-PA1
	  PS2-CMD-PA7
	  PS6-ATT-PB0
	  PS7-CLK-PB1
      
	按键引脚：
	  KEY-PB2
	
	串口： PA9-TX, PA10-RX
	
	主频：72M
	单片机型号：STM32F103C8T6
	
***************************************************************/

#include "main.h"
#include "SysTick.h"
#include "rcc.h"
#include "sys.h"
#include "Uart.h"
#include "App_Ps2.h"

void Usart1_on_recv(u8* data, u32 len){

}

int main(void)
{
    Rcc_Init();				  // 使能各外设时钟
	SWJGpio_Init();			  // 关闭 JTAG，释放 PA15/PB3/PB4
	SysTickInit();			  // 初始化 1 ms 系统嘀嗒（Delay_ms 依赖）
    Uart1_Init(115200);       // 串口1初始化
    AppPs2Init();             // PS2手柄初始化
    while (1)
    {
        AppPs2Run();          // 循环读手柄
    }
}
