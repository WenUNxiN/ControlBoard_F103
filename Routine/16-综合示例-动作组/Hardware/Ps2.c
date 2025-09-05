#include "Ps2.h"

/* 全局 9 字节缓冲区 */
u8 psx_buf[9] = {0};

/* 内部静态函数：单字节 SPI-like 收发，LSB 先行 */
static u8 Ps2TransferByte(u8 tx)
{
    u8 rx = 0;
    for (u8 i = 0; i < 8; i++)
    {
        PS2_CMD(tx & 0x01 ? Bit_SET : Bit_RESET);   // 输出位
        tx >>= 1;

        PS2_CLK(1);  Delay_us(6);
        PS2_CLK(0);  Delay_us(6);     // 下降沿输出
        PS2_CLK(1);                   // 上升沿采样

        if (PS2_DAT()) rx |= (1 << i);
    }
    return rx;
}

/*----------------------------------------------------------
 * 名称  : Ps2_Init
 * 功能  : 1. 初始化 GPIO
 *         2. 发送配置序列，打开模拟量输出
 *         3. 回到轮询模式
 *----------------------------------------------------------*/
void Ps2_Init(void)
{
    GPIO_InitTypeDef gpio;

    /* 1. 开时钟 */
    RCC_APB2PeriphClockCmd(PS2_CLK_GPIO_CLK | PS2_CS_GPIO_CLK |
                           PS2_CMD_GPIO_CLK | PS2_DAT_GPIO_CLK, ENABLE);

    /* 2. 配置 4 根线 */
    gpio.GPIO_Pin  = PS2_DAT_PIN;
    gpio.GPIO_Mode = GPIO_Mode_IPD;        // 下拉输入
    gpio.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(PS2_DAT_GPIO_PORT, &gpio);

    gpio.GPIO_Mode = GPIO_Mode_Out_PP;     // 其余推挽输出
    gpio.GPIO_Pin  = PS2_CMD_PIN;
    GPIO_Init(PS2_CMD_GPIO_PORT, &gpio);
    gpio.GPIO_Pin  = PS2_CS_PIN;
    GPIO_Init(PS2_CS_GPIO_PORT, &gpio);
    gpio.GPIO_Pin  = PS2_CLK_PIN;
    GPIO_Init(PS2_CLK_GPIO_PORT, &gpio);

    /* 默认电平 */
    PS2_CS(1); PS2_CLK(1); PS2_CMD(1);
    Delay_ms(50);      // 等待手柄上电

    /* 3. 配置序列：进入配置→打开模拟→退出配置 */
    const u8 cfg[4][5] = {
        {0x01, 0x43, 0x00, 0x01, 0x00},  // enter config
        {0x01, 0x44, 0x00, 0x01, 0x03},  // set mode (analog)
        {0x01, 0x4F, 0x00, 0xFF, 0xFF},  // enable all axes
        {0x01, 0x43, 0x00, 0x00, 0x00}   // exit config
    };
    for (u8 i = 0; i < 4; i++)
    {
        PS2_CS(0);
        for (u8 j = 0; j < 5; j++) Ps2TransferByte(cfg[i][j]);
        PS2_CS(1);
        Delay_ms(10);
    }

    /* 4. 最后发一次 0x42 让手柄回到 9 字节轮询 */
    PS2_CS(0);
    Ps2TransferByte(0x01);
    Ps2TransferByte(0x42);
    for (u8 i = 0; i < 7; i++) Ps2TransferByte(0x00);
    PS2_CS(1);
}

/*----------------------------------------------------------
 * 名称  : Ps2_WriteRead
 * 功能  : 完成一次 9 字节轮询，结果存入 psx_buf
 *----------------------------------------------------------*/
void Ps2_WriteRead(void)
{
    PS2_CS(0);
    psx_buf[0] = Ps2TransferByte(0x01);   // start
    psx_buf[1] = Ps2TransferByte(0x42);   // request data
    for (u8 i = 2; i < 9; i++) psx_buf[i] = Ps2TransferByte(0x00);
    PS2_CS(1);
}
