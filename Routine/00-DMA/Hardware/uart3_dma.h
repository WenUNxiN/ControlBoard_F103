#ifndef __UART3_DMA_H
#define __UART3_DMA_H

#include "stm32f10x.h"

#define UART3_DMA_RX_BUF_SIZE 128
#define UART_BUF_SIZE 128

extern uint8_t Uart_ReceiveBuf[UART_BUF_SIZE];
extern uint8_t Uart_GetOK;
extern uint16_t Uart3_DataLen;

void Uart3_DMA_Init(uint32_t baud);

#endif
