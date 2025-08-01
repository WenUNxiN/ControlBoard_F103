/***************************************************************
	*	@笔者	：	IMSY
	*	@日期	：	2025年05月20日
	*	@所属	：	小艾机器人科技有限公司
	*	@名称	：	ArmControl
    
    实现的功能：OLED显示与颜色识别，识别到颜色后向串口发送数据。同时OLED显示位图
	
    引脚：
    SCL 引脚：PC14 
    SDA 引脚：PC15
    
    主频72M
	单片机型号：STM32F103C8T6
	
***************************************************************/

#include "main.h" //标准库文件
#include "bmp.h"

void Usart1_on_recv(u8 *data, u32 len)
{

}

int main(void)
{
    u8 num = 0x00;
    /*  更新速率 不设置默认100ms
        4：10ms 3：24ms 2：100ms 1：154ms 0：700ms 
    */
    u8 td = 0;  
    u16 delay_t;

    Rcc_Init();     // 时钟初始化
    SWJGpio_Init(); // 仅SWD模式 - 禁用JTAG功能，释放PA15, PB3, PB4
    SysTickInit();  // 系统时间初始化
    Uart1_Init(115200);

    SoftI2C_init();
    OLED_Init();
    OLED_Clear(); // OLED清空

    OLED_DrawBMP(0, 0, 128, 8, BMP);  // 画位图

    Delay_ms(300); // 等待模块初始化完成
    GY33T_Write_Byte(0xA4, 0x02, td); // 设置更新频率
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
        //从颜色寄存器读取数据
        if (!GY33T_Read(0xa4, 0x15, &num, 1))
        {
            printf("colour:0x%x\r\n ", num);
            if (num == 0x01)
            {
                printf("红色！\r\n");
            }
            else if (num == 0x02)
            {
                printf("黄色！\r\n");
            }
            else if (num == 0x04)
            {
                printf("粉色！\r\n");
            }
            else if (num == 0x08)
            {
                printf("白色！\r\n");
            }
            else if (num == 0x10)
            {
                printf("黑色！\r\n");
            }
            else if (num == 0x20)
            {
                printf("绿色！\r\n");
            }
            else if (num == 0x40)
            {
                printf("深蓝色！\r\n");
            }
            else if (num == 0x80)
            {
                printf("蓝色！\r\n");
            }
        }
        Delay_ms(delay_t); // 等待更新
    }
}
