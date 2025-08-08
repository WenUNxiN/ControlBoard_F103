/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    空白模板
    
	单片机型号：STM32F103C8T6
	
***************************************************************/
#include "main.h" //标准库文件

#include "SysTick.h"
#include "rcc.h"
#include "sys.h"
#include "uart3_dma.h"
#include "Uart.h"

#include <stdio.h>
#include <string.h>

void Usart1_on_recv(u8* data, u32 len){
    
}

int main(void) {
    
	Rcc_Init();				  // 使能各外设时钟
	SWJGpio_Init();			  // 关闭 JTAG，释放 PA15/PB3/PB4
	SysTickInit();			  // 初始化 1 ms 系统嘀嗒（Delay_ms 依赖）
    Uart1_Init(115200);//USART1 初始化
	// 初始化串口3 DMA 接收
    Uart3_DMA_Init(115200);

    while (1)
    {
        if (Uart_GetOK)
        {
            printf("串口数据: %s\r\n", Uart_ReceiveBuf);
            Uart_GetOK = 0;
        }
    }
}

