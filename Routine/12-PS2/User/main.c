/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
  ��������������PS2�ֱ����ڻ������Ӧ�����ݡ�
	
  PS2�ֱ����ţ�	
	  PS1-DAT-PA1
	  PS2-CMD-PA7
	  PS6-ATT-PB0
	  PS7-CLK-PB1
      
	�������ţ�
	  KEY-PB2
	
	���ڣ� PA9-TX, PA10-RX
	
	��Ƶ��72M
	��Ƭ���ͺţ�STM32F103C8T6
	
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
    Rcc_Init();				  // ʹ�ܸ�����ʱ��
	SWJGpio_Init();			  // �ر� JTAG���ͷ� PA15/PB3/PB4
	SysTickInit();			  // ��ʼ�� 1 ms ϵͳ��ણ�Delay_ms ������
    Uart1_Init(115200);       // ����1��ʼ��
    AppPs2Init();             // PS2�ֱ���ʼ��
    while (1)
    {
        AppPs2Run();          // ѭ�����ֱ�
    }
}
