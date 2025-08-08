/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    �հ�ģ��
    
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/
#include "main.h" //��׼���ļ�

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
    
	Rcc_Init();				  // ʹ�ܸ�����ʱ��
	SWJGpio_Init();			  // �ر� JTAG���ͷ� PA15/PB3/PB4
	SysTickInit();			  // ��ʼ�� 1 ms ϵͳ��ણ�Delay_ms ������
    Uart1_Init(115200);//USART1 ��ʼ��
	// ��ʼ������3 DMA ����
    Uart3_DMA_Init(115200);

    while (1)
    {
        if (Uart_GetOK)
        {
            printf("��������: %s\r\n", Uart_ReceiveBuf);
            Uart_GetOK = 0;
        }
    }
}

