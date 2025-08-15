#include "Ps2.h"

/* 全局缓冲区，存放一次完整通信后手柄返回的 9 字节数据 */
u8 psx_buf[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*----------------------------------------------*
 * 名称  : Ps2_Init
 * 功能  : 初始化 PS2 手柄所需的 4 根信号线
 * 引脚  : DAT 下拉输入，CMD/CS/CLK 推挽输出
 * 说明  : 时钟全部开 50 MHz，默认拉高
 *----------------------------------------------*/
void Ps2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 打开 4 个 GPIO 端口的时钟 */
    RCC_APB2PeriphClockCmd(PS2_DAT_GPIO_CLK | PS2_CMD_GPIO_CLK |
                           PS2_CS_GPIO_CLK | PS2_CLK_GPIO_CLK, ENABLE);

    /* DAT 线：下拉输入，用来接收手柄数据 */
    GPIO_InitStructure.GPIO_Pin  = PS2_DAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;      // Input Pull-Down
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(PS2_DAT_GPIO_PORT, &GPIO_InitStructure);

    /* CMD 线：推挽输出，主机向手柄发命令 */
    GPIO_InitStructure.GPIO_Pin  = PS2_CMD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // Push-Pull
    GPIO_Init(PS2_CMD_GPIO_PORT, &GPIO_InitStructure);

    /* CS 线：推挽输出，低电平有效片选 */
    GPIO_InitStructure.GPIO_Pin  = PS2_CS_PIN;
    GPIO_Init(PS2_CS_GPIO_PORT, &GPIO_InitStructure);

    /* CLK 线：推挽输出，时钟信号 */
    GPIO_InitStructure.GPIO_Pin  = PS2_CLK_PIN;
    GPIO_Init(PS2_CLK_GPIO_PORT, &GPIO_InitStructure);

    /* 默认电平：CS=1(未选中)，CLK=1(空闲高)，CMD=1 */
    PS2_CS(1);
    PS2_CLK(1);
    PS2_CMD(1);
}

/*------------------------------------------------*
 * 名称  : Ps2_Transfer
 * 功能  : 按 SPI-like 协议收发 1 字节
 * 参数  : dat  —— 要发送的 8 位数据
 * 返回  : 读到的 8 位数据
 * 说明  : LSB 先行，CLK 空闲高，下降沿输出，上升沿采样
 *------------------------------------------------*/
u8 Ps2_Transfer(unsigned char dat)
{
    unsigned char rd_data, wt_data, i;
    wt_data = dat;
    rd_data = 0;

    /* 逐位处理 8 次 */
    for (i = 0; i < 8; i++)
    {
        PS2_CMD((wt_data & (0x01 << i))); // 把当前 bit 放到 CMD 线
        PS2_CLK(1);  
        Delay_us(6);       // 时钟拉高，维持 6 us
        PS2_CLK(0);  
        Delay_us(6);       // 时钟拉低，维持 6 us
        PS2_CLK(1);                     // 时钟再拉高，产生上升沿

        /* 在上升沿采样 DAT 线，若为 1 则把对应 bit 置 1 */
        if (PS2_DAT())
            rd_data |= 0x01 << i;
    }
    return rd_data;  // 返回读到的 8 位
}

/*------------------------------------------------*
 * 名称  : Ps2_WriteRead
 * 功能  : 完成一次完整的 PS2 手柄数据读取
 * 说明  : 先拉低 CS，随后连续收发 9 字节，结果存 psx_buf
 *------------------------------------------------*/
void Ps2_WriteRead(void)
{
    PS2_CS(0);                        // 拉低片选，开始通信

    /* 按顺序收发 9 字节，并存入全局缓冲区 */
    psx_buf[0] = Ps2_Transfer(START_CMD);   // 0x01 起始命令
    psx_buf[1] = Ps2_Transfer(ASK_DAT_CMD); // 0x42 请求数据命令
    psx_buf[2] = Ps2_Transfer(psx_buf[0]);  // 第 3 字节
    psx_buf[3] = Ps2_Transfer(psx_buf[0]);  // 第 4 字节
    psx_buf[4] = Ps2_Transfer(psx_buf[0]);  // 第 5 字节
    psx_buf[5] = Ps2_Transfer(psx_buf[0]);  // 第 6 字节
    psx_buf[6] = Ps2_Transfer(psx_buf[0]);  // 第 7 字节
    psx_buf[7] = Ps2_Transfer(psx_buf[0]);  // 第 8 字节
    psx_buf[8] = Ps2_Transfer(psx_buf[0]);  // 第 9 字节

    PS2_CS(1);                        // 拉高片选，结束通信
}
