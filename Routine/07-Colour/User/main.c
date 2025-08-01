/***************************************************************
	*	@����	��	IMSY
	*	@����	��	2025��05��20��
	*	@����	��	С�������˿Ƽ����޹�˾
	*	@����	��	ArmControl
    
    ʵ�ֵĹ��ܣ�OLED��ʾ����ɫʶ��ʶ����ɫ���򴮿ڷ������ݡ�ͬʱOLED��ʾλͼ
	
    ���ţ�
    SCL ���ţ�PC14 
    SDA ���ţ�PC15
    
    ��Ƶ72M
	��Ƭ���ͺţ�STM32F103C8T6
	
***************************************************************/

#include "main.h" //��׼���ļ�
#include "bmp.h"

void Usart1_on_recv(u8 *data, u32 len)
{

}

int main(void)
{
    u8 num = 0x00;
    /*  �������� ������Ĭ��100ms
        4��10ms 3��24ms 2��100ms 1��154ms 0��700ms 
    */
    u8 td = 0;  
    u16 delay_t;

    Rcc_Init();     // ʱ�ӳ�ʼ��
    SWJGpio_Init(); // ��SWDģʽ - ����JTAG���ܣ��ͷ�PA15, PB3, PB4
    SysTickInit();  // ϵͳʱ���ʼ��
    Uart1_Init(115200);

    SoftI2C_init();
    OLED_Init();
    OLED_Clear(); // OLED���

    OLED_DrawBMP(0, 0, 128, 8, BMP);  // ��λͼ

    Delay_ms(300); // �ȴ�ģ���ʼ�����
    GY33T_Write_Byte(0xA4, 0x02, td); // ���ø���Ƶ��
    switch (td)
    {
    case 0:
        delay_t = 1000;
        break;
    case 1:
        delay_t = 200;
        break;
    case 2:
        delay_t = 150;
        break;
    case 3:
        delay_t = 50;
        break;
    case 4:
        delay_t = 20;
        break;
    }

    Delay_ms(delay_t);

    while (1)
    {
        //����ɫ�Ĵ�����ȡ����
        if (!GY33T_Read(0xa4, 0x15, &num, 1))
        {
            printf("colour:0x%x\r\n ", num);
            if (num == 0x01)
            {
                printf("��ɫ��\r\n");
            }
            else if (num == 0x02)
            {
                printf("��ɫ��\r\n");
            }
            else if (num == 0x04)
            {
                printf("��ɫ��\r\n");
            }
            else if (num == 0x08)
            {
                printf("��ɫ��\r\n");
            }
            else if (num == 0x10)
            {
                printf("��ɫ��\r\n");
            }
            else if (num == 0x20)
            {
                printf("��ɫ��\r\n");
            }
            else if (num == 0x40)
            {
                printf("����ɫ��\r\n");
            }
            else if (num == 0x80)
            {
                printf("��ɫ��\r\n");
            }
        }
        Delay_ms(delay_t); // �ȴ�����
    }
}
