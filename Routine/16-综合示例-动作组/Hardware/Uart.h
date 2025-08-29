#ifndef _UART__H_
#define _UART__H_

#include "main.h"

/*===================== 宏定义 =====================*/
/* 串口1 DMA 接收缓冲区长度（单位：字节） */
#define UART1_DMA_RX_BUF_SIZE   1024
/* 串口3 DMA 接收缓冲区长度（单位：字节） */
#define UART3_DMA_RX_BUF_SIZE   1024
/* 普通解析缓冲区长度（单位：字节），用于存放完整的一帧数据 */
#define UART_BUF_SIZE           1024

/*===================== 外部变量声明 =====================*/

/* 统一解析缓冲区，存放完整的一帧数据，供主循环解析 */
extern char      Uart_ReceiveBuf[UART_BUF_SIZE];

/* 接收完成标志：置 1 表示已收到完整一帧数据，等待主循环处理 */
extern uint16_t  Uart_GetOK;

/* 指令解析状态机：0 表示空闲；1~4 分别对应四种协议帧类型 */
extern uint8_t   Uart_Mode;

/* DMA 接收计数：仅串口3使用，表示当前已接收到的字节数 */
extern uint16_t  Uart_ReceiveCount;

/*===================== 宏函数 =====================*/

/* 开启全局中断（等同于 CMSIS 的 __enable_irq()） */
#define interrupt_open()    { __enable_irq(); }

/* 串口1接收中断使能宏：开启 USART1 接收寄存器非空中断 */
#define uart1_open()        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE)

/* 串口1接收中断禁用宏：关闭 USART1 接收寄存器非空中断 */
#define uart1_close()       USART_ITConfig(USART1, USART_IT_RXNE, DISABLE)

/* 串口2接收中断使能宏：开启 USART2 接收寄存器非空中断 */
#define uart2_open()        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE)

/* 串口2接收中断禁用宏：关闭 USART2 接收寄存器非空中断 */
#define uart2_close()       USART_ITConfig(USART2, USART_IT_RXNE, DISABLE)

/* 串口3接收中断使能宏：开启 USART3 接收寄存器非空中断
   （注意：串口3使用DMA+空闲中断，通常不需要RXNE中断）*/
#define uart3_open()        USART_ITConfig(USART3, USART_IT_RXNE, ENABLE)

/* 串口3接收中断禁用宏：关闭 USART3 接收寄存器非空中断 */
#define uart3_close()       USART_ITConfig(USART3, USART_IT_RXNE, DISABLE)

/* 将指定串口配置为“只读”（仅接收）：
   置位 CR1.RE（第2位），清零 CR1.TE（第3位） */
#define readOnly(x)         do{ (x)->CR1 |=  (1 << 2); (x)->CR1 &= ~(1 << 3); }while(0)

/* 将指定串口配置为“只写”（仅发送）：
   置位 CR1.TE（第3位），清零 CR1.RE（第2位） */
#define sendOnly(x)         do{ (x)->CR1 |=  (1 << 3); (x)->CR1 &= ~(1 << 2); }while(0)

/*===================== 函数声明 =====================*/

/* 串口1初始化函数：用于USB-CDC/调试口，波特率可设 */
void Uart1_Init(u32 baud);

/* 串口2初始化函数：用于RS-485半双工通信，波特率可设 */
void Uart2_Init(u32 baud);

/* 串口3初始化函数：用于用户串口，使用DMA+空闲中断接收，波特率可设 */
void Uart3_Init(u32 baud);

/* printf重定向选择函数：设置printf输出到指定串口
   参数sw_uart：1-UART1 2-UART2 3-UART3 */
void SetPrintfUart(u8 sw_uart);

/* 调试函数：同时向UART1/2/3发送同一字符串（调试用） */
void UartAll_Printf(char *str);

/* 阻塞方式向UART1发送字符串（不经过printf） */
void uart1_send_str(char *str);

/* 阻塞方式向UART2发送字符串（发送时关闭接收中断防止冲突） */
void uart2_send_str(char *s);

/* 协议解析函数：主循环调用，用于解析DMA/中断接收到的完整帧数据
   参数rx_data：接收到的单个字节 */
void Uart_DataParse(char rx_data);

#endif /* _UART__H_ */
