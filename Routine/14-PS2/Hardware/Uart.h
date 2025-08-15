#ifndef _UART__H_
#define _UART__H_

#include "main.h"

#define USART1_RECV_CALLBACK   1            //串口1回调处理

/* =========================================================
 * 公共接口
 * =======================================================*/
void Uart1_Init(u32 baud);//USART1 初始化
void Uart1_send_data(u8 ucch);//发送单个字节
void Uart1_send_String(u8 *ucstr);//发送字符串

 /**
  * 接收回调声明（仅在 USART1_RECV_CALLBACK = 1 时生效）
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


#endif /* _UART__H_ */

