#ifndef _UART__H_
#define _UART__H_

#include "main.h"

/*===================== 宏定义 =====================*/

/* 串口3 DMA 接收缓冲区长度，必须与源文件保持一致 */
#define UART3_DMA_RX_BUF_SIZE   1024

/* 普通解析缓冲区长度（单位：字节） */
#define UART_BUF_SIZE           1024

/*===================== 外部变量声明 =====================*/

/* 统一解析缓冲区，存放完整的一帧数据 */
extern char      Uart_ReceiveBuf[UART_BUF_SIZE];

/* 接收完成标志：置 1 表示收到完整一帧，等待主循环处理 */
extern uint16_t  Uart_GetOK;

/* 指令解析状态机：0 空闲；1~4 对应四种协议帧 */
extern uint8_t   Uart_Mode;

/* DMA 接收计数：仅在串口 3 使用，表示当前已接收字节数 */
extern uint16_t  Uart_ReceiveCount;

/*===================== 宏函数 =====================*/

/* 开启全局中断（等同于 CMSIS 的 __enable_irq()） */
#define interrupt_open()    { __enable_irq(); }

/* 串口 1 接收中断的开关宏 */
#define uart1_open()        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE)
#define uart1_close()       USART_ITConfig(USART1, USART_IT_RXNE, DISABLE)

/* 串口 2 接收中断的开关宏 */
#define uart2_open()        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE)
#define uart2_close()       USART_ITConfig(USART2, USART_IT_RXNE, DISABLE)

/* 串口 3 接收中断的开关宏（注意：串口 3 使用 DMA+空闲中断，通常不需要 RXNE 中断） */
#define uart3_open()        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE)
#define uart3_close()       USART_ITConfig(USART3, USART_IT_RXNE, DISABLE)

/* 将指定串口配置为“只读”（仅接收）：
   置位 CR1.RE（第 2 位），清零 CR1.TE（第 3 位） */
#define readOnly(x)         do{ (x)->CR1 |=  (1 << 2); (x)->CR1 &= ~(1 << 3); }while(0)

/* 将指定串口配置为“只写”（仅发送）：
   置位 CR1.TE（第 3 位），清零 CR1.RE（第 2 位） */
#define sendOnly(x)         do{ (x)->CR1 |=  (1 << 3); (x)->CR1 &= ~(1 << 2); }while(0)

/*===================== 函数声明 =====================*/

/* 串口初始化函数 */
void Uart1_Init(u32 baud);      /* 串口1：USB-CDC/调试口 */
void Uart2_Init(u32 baud);      /* 串口2：RS-485 半双工口 */
void Uart3_Init(u32 baud);      /* 串口3：用户串口（DMA+空闲中断） */

/* printf 重定向选择：1-UART1 2-UART2 3-UART3 */
void SetPrintfUart(u8 sw_uart);

/* 同时向 UART1/2/3 发送同一字符串（调试用） */
void UartAll_Printf(char *str);

/* 阻塞方式向 UART1 发送字符串（不依赖 printf） */
void Uart1_Print(char *str);

/* 主循环调用：解析 DMA/中断接收到的完整帧 */
void Uart_DataParse(char rx_data);

#endif /* _UART__H_ */
