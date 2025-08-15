#ifndef _UART__H_
#define _UART__H_
#include "main.h"

#define UART_BUF_SIZE 1024 // 定义最大接收字节数 128

extern char Uart_ReceiveBuf[UART_BUF_SIZE]; // 接收缓冲,最大UART_BUF_SIZE个字节.末字节为换行符
extern u16 Uart_GetOK;                      // 接收完成标记
extern u8 Uart_Mode;                        /* 指令的模式 */

#define interrupt_open() \
    {                    \
        __enable_irq();  \
    }

#define readOnly(x) \
    x->CR1 |= 4;    \
    x->CR1 &= 0xFFFFFFF7; // 串口x配置为只读，CR1->RE=1, CR1->TE=0
#define sendOnly(x) \
    x->CR1 |= 8;    \
    x->CR1 &= 0xFFFFFFFB; // 串口x配置为只写，CR1->RE=0, CR1->TE=1

void Uart1_Init(u32 baud);

#endif
