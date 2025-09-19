/*********************************************************
 * 文件：Uart.c
 * 说明：USART1 串口驱动
 *********************************************************/

#include "Uart.h"
#include <stdio.h>      // 为 printf 提供 FILE 结构体

/* ------------------ 全局变量 ------------------ */
char Uart_ReceiveBuf[UART_BUF_SIZE];    // 环形或一次性接收缓冲区
u16  Uart_GetOK = 0;                    // 接收完成标志，1 表示收到完整帧
u8   Uart_Mode  = 0;                    // 0=等待帧头 1=命令 2=单舵机 3=多舵机

/* ********************************************************
 * 名称：Uart1_Init
 * 功能：初始化 USART1（PA9-TX, PA10-RX）
 * 参数：baud —— 目标波特率，如 115200
 * 返回：无
 * ********************************************************/
void Uart1_Init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 使能端口时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	USART_DeInit(USART1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		/* PA.9 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 复用推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; /* 浮空输入 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud;										/* 串口波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						/* 字长为8位数据格式 */
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							/* 停止位1位 */
	USART_InitStructure.USART_Parity = USART_Parity_No;								/* 无奇偶校验位 */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					/* 收发模式 */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; /* 无硬件数据流控制 */
	USART_Init(USART1, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; /* 抢占优先级 */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  /* 子优先级 */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  /* IRQ通道使能 */
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /* 开启串口接受中断 */
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE); /* 禁止串口发送中断 */

	USART_Cmd(USART1, ENABLE); /* 使能串口1  */
}

/* ********************************************************
 * 名称：fputc
 * 功能：重定向 printf 到 USART1（需要开启 MicroLIB）
 * ********************************************************/
int fputc(int ch, FILE *f)
{
    /* 等待发送寄存器空 */
    while ((USART1->SR & USART_SR_TXE) == 0);
    USART1->DR = (u8)ch;
    return ch;
}

/* ***********************************************************
 * 名称：USART1_IRQHandler
 * 功能：USART1 中断服务函数
 *       支持三种帧格式：
 *         1) 命令模式      $XXX!               → Uart_Mode = 1
 *         2) 单舵机模式    #000P1500T1000!     → Uart_Mode = 2
 *         3) 多舵机模式    {#...!#...!}        → Uart_Mode = 3
 *       收到帧尾 '!' 或 '}' 后把 Uart_GetOK 置 1
 * ***********************************************************/
void USART1_IRQHandler(void)
{
    u8  sbuf_bak;
    static u16 buf_index = 0;   // 当前写入位置

    /* 仅处理接收中断 */
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        sbuf_bak = USART_ReceiveData(USART1);   // 读 DR 清中断标志

        /* 如果已经接收完成，则丢弃新数据 */
        if (Uart_GetOK)
            return;

        /* ---------- 帧头识别 ---------- */
        if (Uart_Mode == 0)
        {
            buf_index = 0;
            if (sbuf_bak == '$')        // 命令帧
                Uart_Mode = 1;
            else if (sbuf_bak == '#')   // 单舵机帧
                Uart_Mode = 2;
            else if (sbuf_bak == '{')   // 多舵机帧
                Uart_Mode = 3;
        }

        /* ---------- 保存字节 ---------- */
        Uart_ReceiveBuf[buf_index++] = sbuf_bak;

        /* ---------- 帧尾判断 ---------- */
        if ((Uart_Mode == 1 || Uart_Mode == 2) && sbuf_bak == '!')
        {
            Uart_ReceiveBuf[buf_index] = '\0';
            Uart_GetOK = 1;
        }
        else if (Uart_Mode == 3 && sbuf_bak == '}')
        {
            Uart_ReceiveBuf[buf_index] = '\0';
            Uart_GetOK = 1;
        }

        /* ---------- 缓冲区溢出保护 ---------- */
        if (buf_index >= UART_BUF_SIZE)
            buf_index = 0;

        /* 清中断标志（读 DR 已自动清零，可省略） */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
