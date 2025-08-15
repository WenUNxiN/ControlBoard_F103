#ifndef _UART__H_
#define _UART__H_

#include "main.h"

// 串口使能
#define USART1_ENABLE   1
#define USART2_ENABLE   0
#define USART3_ENABLE   0

#define USART1_RECV_CALLBACK   1            //串口1回调处理
#define USART2_RECV_CALLBACK   0            //串口2回调处理
#define USART3_RECV_CALLBACK   0            //串口3回调处理

// printf选择 x == 串口x
#define USART_PRINTF   1

/* =========================================================
 * 公共接口
 * =======================================================*/
#if USART1_ENABLE
void Uart1_Init(u32 baud);//USART1 初始化
void Uart1_send_data(u8 ucch);//发送单个字节
void Uart1_send_String(u8 *ucstr);//发送字符串
#endif

#if USART2_ENABLE
void Uart2_Init(u32 baud);//USART2 初始化
void Uart2_send_data(u8 ucch);//发送单个字节
void Uart2_send_String(u8 *ucstr);//发送字符串
#endif

#if USART3_ENABLE
void Uart3_Init(u32 baud);//USART3 初始化
void Uart3_send_data(u8 ucch);//发送单个字节
void Uart3_send_String(u8 *ucstr);//发送字符串
#endif
 /**
  * 接收回调声明（仅在 USART1_RECV_CALLBACK = 1 时生效）
  *                   USART2_RECV_CALLBACK = 1
  *                   USART3_RECV_CALLBACK = 1
  *
  * @brief  用户层接收完成回调（弱符号）
  * @param  data 指向接收到的完整数据帧
  * @param  len  数据帧长度
  * @retval 无
  * @note   需在应用层实现该函数；中断完成后自动调用
  */
#if USART1_RECV_CALLBACK
extern void Usart1_on_recv(u8* data, u32 len);
#endif

#if USART2_RECV_CALLBACK
extern void Usart2_on_recv(u8* data, u32 len);
#endif

#if USART3_RECV_CALLBACK
extern void Usart3_on_recv(u8* data, u32 len);
#endif

#endif /* _UART__H_ */

