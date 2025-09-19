/**
  ******************************************************************************
  * @file    Uart.c
  * @author  xxx
  * @brief   多串口驱动 + 简易协议解析
  *          - 串口1：USB-CDC/调试口，DMA 循环接收 + 空闲中断
  *          - 串口2：RS-485 半双工，普通中断接收
  *          - 串口3：用户串口，DMA 循环接收 + 空闲中断
  *          - 支持四种简易协议帧
  *              1) $XXX!            系统命令
  *              2) #SSSPPPPTTTT!   单舵机控制
  *              3) { ... }         多舵机批量控制
  *              4) <G...>          动作组保存/调用
  ******************************************************************************
  */

#include "Uart.h"
#include <stdio.h>
#include <string.h>

/*------------- 全局变量 -------------*/
u8  SW_Uart = 1;                        /* printf 重定向目标：1-UART1 2-UART2 3-UART3 */
char Uart_ReceiveBuf[UART_BUF_SIZE];    /* 统一解析缓冲区，用于存放完整的一帧数据 */
uint8_t Uart1_DMA_RX_BUF[UART1_DMA_RX_BUF_SIZE];   // 串口1 DMA 接收缓冲区
uint8_t Uart3_DMA_RX_BUF[UART3_DMA_RX_BUF_SIZE];   /* 串口3 DMA 接收缓冲区 */
u16 Uart_ReceiveCount = 0;              /* 串口3 DMA 已接收字节数（当前帧长度） */
u16 Uart_GetOK = 0;                     /* 接收完成标志：1表示已收到完整帧，等待主循环处理 */
u8  Uart_Mode = 0;                      /* 解析状态机：0空闲；1~4对应四种协议帧 */

/*==============================================================================
 * 名称：Uart1_Init
 * 功能：初始化串口1（PA9/PA10），用于调试/下载/USB-CDC
 * 参数：baud——波特率
 *============================================================================*/
void Uart1_Init(u32 baud)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

    /* 1. 开启相关外设时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    USART_DeInit(USART1); // 复位串口1寄存器到缺省值

    /* 2. 配置GPIO：TX=PA9（复用推挽输出），RX=PA10（浮空输入） */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 配置USART1参数：波特率、8位数据、1位停止位、无校验、无硬件流控、收发使能 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    /* 4. 配置DMA1通道5用于USART1_RX，循环模式，内存递增，外设不递增 */
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)Uart1_DMA_RX_BUF;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = UART1_DMA_RX_BUF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel5, ENABLE);           // 启动DMA
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // 使能USART1的DMA接收请求

    /* 5. 使能USART1空闲中断（用于DMA接收完成检测） */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    /* 6. 配置NVIC，USART1中断优先级1,0 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 7. 使能USART1 */
    USART_Cmd(USART1, ENABLE);
}

/*==============================================================================
 * 名称：Uart2_Init
 * 功能：初始化串口2（PA2/PA3），RS-485 半双工
 * 参数：baud——波特率
 * 说明：半双工模式，收发共用一根线
 *============================================================================*/
void Uart2_Init(u32 baud)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* 2. 配置GPIO：TX=PA2（复用推挽输出），RX与TX共用，无需单独配置 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 配置USART2参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure);

    /* 4. 使能USART2半双工模式 */
    USART_HalfDuplexCmd(USART2, ENABLE);

    /* 5. 配置NVIC，USART2中断优先级2,0 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 6. 使能接收中断，关闭发送中断 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_TXE,  DISABLE);

    USART_Cmd(USART2, ENABLE);
}

/*==============================================================================
 * 名称：Uart3_Init
 * 功能：初始化串口3（PB10/PB11），DMA 循环接收 + 空闲中断
 * 参数：baud——波特率
 *============================================================================*/
void Uart3_Init(u32 baud)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

    /* 1. 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 2. 配置GPIO：TX=PB10，RX=PB11 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 3. 配置USART3参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure);

    /* 4. 配置DMA1通道3用于USART3_RX，循环模式 */
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)Uart3_DMA_RX_BUF;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = UART3_DMA_RX_BUF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel3, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

    /* 5. 使能USART3空闲中断 */
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    /* 6. 配置NVIC，USART3中断优先级1,0 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART3, ENABLE);
}

/*==============================================================================
 * 名称：fputc
 * 功能：重定向 printf 到指定串口（需勾选 Use MicroLIB）
 * 参数：ch - 待发送字符，f - 文件指针（未使用）
 * 返回：发送的字符
 *============================================================================*/
int fputc(int ch, FILE *f)
{
    switch (SW_Uart)
    {
        case 1:
            while (!(USART1->SR & USART_SR_TXE)); // 等待发送缓冲区空
            USART1->DR = (uint8_t)ch;
            break;
        case 2:
            while (!(USART2->SR & USART_SR_TXE));
            USART2->DR = (uint8_t)ch;
            break;
        case 3:
            while (!(USART3->SR & USART_SR_TXE));
            USART3->DR = (uint8_t)ch;
            break;
        default:
            return 0;
    }
    return ch;
}

/*==============================================================================
 * 名称：SetPrintfUart
 * 功能：动态切换 printf 输出串口
 * 参数：sw_uart - 1-UART1 2-UART2 3-UART3
 *============================================================================*/
void SetPrintfUart(u8 sw_uart)
{
    SW_Uart = sw_uart;
}

/*==============================================================================
 * 名称：UartAll_Printf
 * 功能：向 UART1/2/3 三个串口同时发送同一字符串（调试用）
 * 参数：str - 要发送的字符串
 *============================================================================*/
void UartAll_Printf(char *str)
{
    SetPrintfUart(1); printf("%s", str);
    SetPrintfUart(2); printf("%s", str);
    SetPrintfUart(3); printf("%s", str);
}

/*==============================================================================
 * 名称：uart1_send_str
 * 功能：阻塞方式直接向串口1发送字符串（不经过 printf）
 * 参数：str - 要发送的字符串
 *============================================================================*/
void uart1_send_str(char *str)
{
    while (*str)
    {
        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // 等待发送完成
    }
}

/**
 * @brief   通过 USART2 发送任意长度字节流
 * @param   data  待发送缓冲区首地址
 * @param   len   待发送字节数
 * @note    1. 先关闭接收中断，防止发送期间被 RX 打断
 *          2. 逐字节等待 TXE 置位后写入
 *          3. 发送完毕重新打开接收中断，恢复正常接收
 */
void uart2_send_bytes(uint8_t *data, int len)
{
    /*  Step1: 关闭接收中断，避免发送过程中被 RX 打断  */
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

    /*  Step2: 逐字节阻塞发送  */
    for (int i = 0; i < len; i++)
    {
        USART_SendData(USART2, data[i]);            // 写
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            ;                                       // 等待发送寄存器空
    }

    /*  Step3: 重新打开接收中断，恢复串口接收  */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}
/*==============================================================================
 * 名称：uart2_send_str
 * 功能：阻塞方式向串口2发送字符串（关闭接收中断，防止冲突）
 * 参数：s - 要发送的字符串
 *============================================================================*/
void uart2_send_str(char *s)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // 发送时关闭接收中断
	while (*s)
	{
		USART_SendData(USART2, *s++);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;
	}
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 发送完成后重新打开接收中断
}

/*==============================================================================
 * 名称：USART1_IRQHandler
 * 功能：串口1空闲中断，配合DMA接收不定长数据，按字节解析四种协议帧
 *============================================================================*/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        /* 1. 清空闲中断标志（必须读SR和DR） */
        volatile uint32_t tmp;
        tmp = USART1->SR;
        tmp = USART1->DR;
        (void)tmp;

        /* 2. 暂停DMA，计算已接收字节数 */
        DMA_Cmd(DMA1_Channel5, DISABLE);
        uint16_t recv_len = UART1_DMA_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);

        /* 3. 逐字节解析接收到的数据 */
        if (recv_len > 0 && recv_len <= UART_BUF_SIZE)
        {
            for (uint16_t i = 0; i < recv_len; i++)
            {
                Uart_DataParse(Uart1_DMA_RX_BUF[i]);
            }
        }

        /* 4. 重新装载DMA缓冲区大小并启动DMA */
        DMA_SetCurrDataCounter(DMA1_Channel5, UART1_DMA_RX_BUF_SIZE);
        DMA_Cmd(DMA1_Channel5, ENABLE);
    }
}

/*==============================================================================
 * 名称：USART2_IRQHandler
 * 功能：串口2接收中断，每收到一个字节立即解析
 *============================================================================*/
void USART2_IRQHandler(void)
{
	// 判断接收寄存器非空中断标志
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		// 读取接收到的字节
		uint8_t rx = USART_ReceiveData(USART2);
		
		// 数据解析
		Uart_DataParse(rx);
		
		// 手动清除中断标志
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);  
	}
}

/*==============================================================================
 * 名称：USART3_IRQHandler
 * 功能：串口3空闲中断，配合DMA接收不定长数据
 *============================================================================*/
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        /* 1. 清空闲中断标志（必须读SR和DR） */
        volatile uint32_t tmp;
        tmp = USART3->SR;
        tmp = USART3->DR;
        (void)tmp;

        /* 2. 暂停DMA，计算已接收字节数 */
        DMA_Cmd(DMA1_Channel3, DISABLE);
        Uart_ReceiveCount = UART3_DMA_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);

        /* 3. 逐字节解析DMA接收到的数据 */
        if (Uart_ReceiveCount > 0 && Uart_ReceiveCount <= UART_BUF_SIZE)
        {
            for (int i = 0; i < Uart_ReceiveCount; i++) {
                Uart_DataParse(Uart3_DMA_RX_BUF[i]);
            }
        }

        /* 4. 重新装载DMA缓冲区大小并启动DMA */
        DMA_SetCurrDataCounter(DMA1_Channel3, UART3_DMA_RX_BUF_SIZE);
        DMA_Cmd(DMA1_Channel3, ENABLE);
    }
}

/**
 * 名称：Uart_DataParse
 * 功能：统一的协议解析函数，按字节处理数据
 * 参数：rx_data - 接收到的单个字节
 * 说明：根据帧头（$ # { <）判断帧类型，遇到帧尾（! } >）后标记接收完成
 */
void Uart_DataParse(char rx_data)
{
    static uint16_t buf_index = 0;

    // 如果已经接收到完整数据包，在主循环处理完之前，不处理新数据
    if (Uart_GetOK)
        return;
    
    // 缓冲区溢出保护
    if (buf_index >= UART_BUF_SIZE) {
        buf_index = 0;
        Uart_Mode = 0;
    }

    // 空闲状态下根据首字符判断帧类型
    if (Uart_Mode == 0)
    {
        switch (rx_data)
        {
            case '<': Uart_Mode = 4; break;  // 动作组保存/调用
            case '$': Uart_Mode = 1; break;  // 系统命令
            case '#': Uart_Mode = 2; break;  // 单舵机控制
            case '{': Uart_Mode = 3; break;  // 多舵机批量控制
            default:  return;                // 非法字符，继续等待
        }
        buf_index = 0;
    }
    
    // 存储当前字节到解析缓冲区
    Uart_ReceiveBuf[buf_index++] = rx_data;

    // 根据帧尾判断接收是否完成
    if ((Uart_Mode == 4 && rx_data == '>') || // 动作组帧尾
        (Uart_Mode == 1 && rx_data == '!') || // 系统命令帧尾
        (Uart_Mode == 2 && rx_data == '!') || // 单舵机帧尾
        (Uart_Mode == 3 && rx_data == '}'))   // 多舵机帧尾
    {
        Uart_ReceiveBuf[buf_index] = '\0';  // 添加字符串结束符，便于后续处理
        Uart_GetOK = 1;                     // 标记接收完成，通知主循环处理
    }
}
