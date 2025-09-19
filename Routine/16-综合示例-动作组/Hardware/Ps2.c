#include "Ps2.h"

/* 配置序列：进入配置→打开模拟→退出配置 */
const u8 cfg[4][5] = {
    {0x01, 0x43, 0x00, 0x01, 0x00},  // 进入配置模式
    {0x01, 0x44, 0x00, 0x01, 0x03},  // 设置为模拟模式 Ox03 锁存设置，即不可通过按键“MODE”设置模式。0xEE 不锁存软件设置，可通过按键“MODE”设置模式。
    {0x01, 0x4F, 0x00, 0xFF, 0xFF},  // 启用所有轴 后面两字节全部置 1（0xFF），相当于一次性把这 4 个轴的模拟采样功能全部打开
    {0x01, 0x43, 0x00, 0x00, 0x00}   // 退出配置模式
};

/* ---------------- 热插拔支持 ---------------- */
static u8 ps2_hp_state   = 0;   // 0:OFF  1:PLUG  2:READY
static u8 ps2_hp_loss    = 0;   // 连续丢失计数
static u8 ps2_hp_stable  = 2;   // 静止帧计数

/* 全局 9 字节缓冲区 */
u8 psx_buf[9] = {0};

#define PS2_BIT_DLY  8       

/* 单字节 SPI-like 收发，LSB 先行 */
static u8 Ps2TransferByte(u8 tx)
{
    u8 rx = 0;
    for (u8 i = 0; i < 8; i++)
    {
        PS2_CMD(tx & 0x01 ? Bit_SET : Bit_RESET);   // 输出位
        tx >>= 1;

        PS2_CLK(1);  Delay_us(PS2_BIT_DLY);
        PS2_CLK(0);  Delay_us(PS2_BIT_DLY);     // 下降沿输出
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

    /* 配置序列：进入配置→打开模拟→退出配置 */
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

/* 短探针：只发 0x01 0x42，返回 1=在线 */
u8 Ps2Probe(void)
{
    u8 rx;
    PS2_CS(0);
    Ps2TransferByte(0x01);
    rx = Ps2TransferByte(0x42);
    PS2_CS(1);
    return (rx == PS2_MODE_GRN || rx == PS2_MODE_RED) ? 1 : 0;
}

/* 重新握手 */
void Ps2ReConfig(void)
{
    /* 配置序列：进入配置→打开模拟→退出配置 */
    for(u8 i = 0; i < 4; i++)
    {
        PS2_CS(0);
        for(u8 j = 0; j < 5; j++) Ps2TransferByte(cfg[i][j]);
        PS2_CS(1);
        Delay_ms(8);
    }
}

/* 供外部 10 ms 调用，返回值：1=数据有效  0=离线 */
u8 Ps2HotplugTask(void)
{
    switch(ps2_hp_state)
    {
    case 0:                     /* OFF */
        if(Ps2Probe())
        {
            ps2_hp_loss = 0;
            ps2_hp_state = 1;   /* 进入 PLUG */
        }
//        printf("OFF\n");
        return 0;

    case 1:                     /* PLUG */
        if(!Ps2Probe() && ++ps2_hp_loss > 3)
        {
            ps2_hp_state = 0;
            return 0;
        }
        Ps2ReConfig();          /* 握手 */
        if(Ps2Probe())
        {
            ps2_hp_state = 2;
            ps2_hp_loss  = 0;
            ps2_hp_stable = 6;  /* 要求连续 6 帧静止 */
        }
//         printf("PLUG\n");
        return 0;

    case 2:                     /* READY */
        if(!Ps2Probe() && ++ps2_hp_loss > 3)
        {
            ps2_hp_state = 0;
            memset(psx_buf,0,9);
            ps2_hp_stable = 6;
            return 0;
        }
//        printf("READY\n");
        
        Delay_ms(1);
        Ps2_WriteRead();        /* 读 9 字节 */

        /* 前 6 帧必须全部通过死区才认为“人没动” */
        if(ps2_hp_stable)
        {
            u8 all_zero = 1;
            for(u8 i = 0; i < 4; i++)
            {
                s8 v = (s8)(psx_buf[5+i] - 128);
                if(v > 8 || v < -8) { all_zero = 0; break; }
            }
            if(all_zero) ps2_hp_stable--;
            else         ps2_hp_stable = 6;   /* 重新计数 */
            return 0;                           /* 仍丢弃 */
        }
        return 1;                               /* 数据已有效 */
    }
    return 0;
}

