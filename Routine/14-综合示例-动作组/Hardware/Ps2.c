#include "Ps2.h"

/* 数据存储数组 */
u8 psx_buf[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* PS2手柄初始化 */
/***********************************************
   函数名称:void My_Ps2Init(void)
   功能介绍:	PS2手柄初始化
   函数参数:	无
   返回值:	无
 ***********************************************/
void Ps2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(PS2_DAT_GPIO_CLK | PS2_CMD_GPIO_CLK | PS2_CS_GPIO_CLK | PS2_CLK_GPIO_CLK, ENABLE); // 使能端口时钟

    GPIO_InitStructure.GPIO_Pin = PS2_DAT_PIN;         // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;      // 下拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
    GPIO_Init(PS2_DAT_GPIO_PORT, &GPIO_InitStructure); // 根据设定参数初始化GPIO

    GPIO_InitStructure.GPIO_Pin = PS2_CMD_PIN;         // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 50M
    GPIO_Init(PS2_CMD_GPIO_PORT, &GPIO_InitStructure); // 根据设定参数初始化GPIO

    GPIO_InitStructure.GPIO_Pin = PS2_CS_PIN;         // 端口配置
    GPIO_Init(PS2_CS_GPIO_PORT, &GPIO_InitStructure); // 根据设定参数初始化GPIO

    GPIO_InitStructure.GPIO_Pin = PS2_CLK_PIN;         // 端口配置
    GPIO_Init(PS2_CLK_GPIO_PORT, &GPIO_InitStructure); // 根据设定参数初始化GPIO

    PS2_CS(1);
    PS2_CLK(1);
    PS2_CMD(1);
}

/***********************************************
    功能介绍：读写1个字节
    函数参数：无
    返回值：无
 ***********************************************/
u8 Ps2_Transfer(unsigned char dat)
{
    unsigned char rd_data, wt_data, i;
    wt_data = dat;
    rd_data = 0;
    for (i = 0; i < 8; i++)
    {
        PS2_CMD((wt_data & (0x01 << i)));
        PS2_CLK(1);
        Delay_us(6);
        PS2_CLK(0);
        Delay_us(6);
        PS2_CLK(1);
        if (PS2_DAT())
        {
            rd_data |= 0x01 << i;
        }
    }
    return rd_data;
}

/***********************************************
    功能介绍：读取手柄数据
    函数参数：无
    返回值：无
 ***********************************************/
void Ps2_WriteRead(void)
{
    PS2_CS(0);
    psx_buf[0] = Ps2_Transfer(START_CMD);
    psx_buf[1] = Ps2_Transfer(ASK_DAT_CMD);
    psx_buf[2] = Ps2_Transfer(psx_buf[0]);
    psx_buf[3] = Ps2_Transfer(psx_buf[0]);
    psx_buf[4] = Ps2_Transfer(psx_buf[0]);
    psx_buf[5] = Ps2_Transfer(psx_buf[0]);
    psx_buf[6] = Ps2_Transfer(psx_buf[0]);
    psx_buf[7] = Ps2_Transfer(psx_buf[0]);
    psx_buf[8] = Ps2_Transfer(psx_buf[0]);
    PS2_CS(1);
}
