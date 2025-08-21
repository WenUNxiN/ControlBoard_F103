/**
  ******************************************************************************
  * @file    Uart.c
  * @author  xxx
  * @brief   多串口驱动 + 简易协议解析
  *          - 串口1：USB-CDC/调试口，普通中断接收
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
char Uart_ReceiveBuf[UART_BUF_SIZE];    /* 统一解析缓冲区 */
uint8_t Uart1_DMA_RX_BUF[UART1_DMA_RX_BUF_SIZE];   // DMA 双缓冲
uint8_t Uart3_DMA_RX_BUF[UART3_DMA_RX_BUF_SIZE];  /* DMA 双缓冲 */
u16 Uart_ReceiveCount = 0;              /* DMA 已接收字节数 */
u16 Uart_GetOK = 0;                     /* 接收完成标志 */
u8  Uart_Mode = 0;                      /* 解析状态机 0空闲 1~4对应四种协议 */

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

    /* 1. 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    USART_DeInit(USART1);

    /* 2. GPIO  TX=PA9  RX=PA10 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. USART 参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    /* 4. DMA1 Channel5 → USART1_RX  循环模式 */
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

    DMA_Cmd(DMA1_Channel5, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    /* 5. 空闲中断 */
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    /* 6. NVIC */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 7. 使能串口 */
    USART_Cmd(USART1, ENABLE);
}

/*==============================================================================
 * 名称：Uart2_Init
 * 功能：初始化串口2（PA2/PA3），RS-485 半双工
 * 参数：baud——波特率
 * 说明：半双工
 *============================================================================*/
void Uart2_Init(u32 baud)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* 2. GPIO 仅 TX(PA2) 需要配置为复用推挽，RX 与 TX 共线 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. USART 参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure);

    /* 4. 半双工模式 */
    USART_HalfDuplexCmd(USART2, ENABLE);

    /* 5. NVIC */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 6. 中断配置 */
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

    /* 1. 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 2. GPIO TX=PB10 RX=PB11 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 3. USART 参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure);

    /* 4. DMA1 Channel3 → USART3_RX  循环模式 */
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

    /* 5. 空闲中断 */
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    /* 6. NVIC */
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
 *============================================================================*/
int fputc(int ch, FILE *f)
{
    switch (SW_Uart)
    {
        case 1:
            while (!(USART1->SR & USART_SR_TXE));
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
 *============================================================================*/
void SetPrintfUart(u8 sw_uart)
{
    SW_Uart = sw_uart;
}

/*==============================================================================
 * 名称：UartAll_Printf
 * 功能：向 1/2/3 三个串口同时发送字符串
 *============================================================================*/
void UartAll_Printf(char *str)
{
    SetPrintfUart(1); printf("%s", str);
    SetPrintfUart(2); printf("%s", str);
    SetPrintfUart(3); printf("%s", str);
}

/*==============================================================================
 * 名称：Uart1_Print
 * 功能：阻塞方式直接向串口1发送字符串（不经过 printf）
 *============================================================================*/
void Uart1_Print(char *str)
{
    while (*str)
    {
        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}

/*==============================================================================
 * 名称：USART1_IRQHandler
 * 功能：串口1接收中断，按字节解析四种协议帧
 *============================================================================*/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        /* 1. 清 IDLE 标志 */
        volatile uint32_t tmp;
        tmp = USART1->SR;
        tmp = USART1->DR;
        (void)tmp;

        /* 2. 关 DMA 计算已收字节数 */
        DMA_Cmd(DMA1_Channel5, DISABLE);
        uint16_t recv_len = UART1_DMA_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);

        /* 3. 逐字节解析并回显 */
        if (recv_len > 0 && recv_len <= UART_BUF_SIZE)
        {
            for (uint16_t i = 0; i < recv_len; i++)
            {
                Uart_DataParse(Uart1_DMA_RX_BUF[i]);
            }
        }

        /* 4. 重新装载 DMA */
        DMA_SetCurrDataCounter(DMA1_Channel5, UART1_DMA_RX_BUF_SIZE);
        DMA_Cmd(DMA1_Channel5, ENABLE);
    }
}

/*==============================================================================
 * 名称：USART2_IRQHandler
 * 功能：串口2接收中断，逻辑与串口1完全一致
 *============================================================================*/
void USART2_IRQHandler(void)
{
    if (USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET)
    {
        char rx_data = USART_ReceiveData(USART2);
        Uart_DataParse(rx_data);          // 统一解析入口
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/*==============================================================================
 * 名称：USART3_IRQHandler
 * 功能：串口3空闲中断，配合 DMA 接收不定长数据
 *============================================================================*/
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        /* 1. 清 IDLE 标志 */
        volatile uint32_t tmp;
        tmp = USART3->SR;
        tmp = USART3->DR;
        (void)tmp;

        /* 2. 关闭 DMA，计算已接收字节数 */
        DMA_Cmd(DMA1_Channel3, DISABLE);
        Uart_ReceiveCount = UART3_DMA_RX_BUF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);

        /* 3. 逐字节解析 DMA 接收到的数据 */
        if (Uart_ReceiveCount > 0 && Uart_ReceiveCount <= UART_BUF_SIZE)
        {
            for (int i = 0; i < Uart_ReceiveCount; i++) {
                Uart_DataParse(Uart3_DMA_RX_BUF[i]);
//                USART_SendData(USART1, Uart3_DMA_RX_BUF[i]);// 回显
            }
        }

        /* 4. 重新装载 DMA */
        DMA_SetCurrDataCounter(DMA1_Channel3, UART3_DMA_RX_BUF_SIZE);
        DMA_Cmd(DMA1_Channel3, ENABLE);
    }
}

/**
 * 名称：Uart_DataParse
 * 功能：统一的协议解析函数，按字节处理数据
 * 参数：rx_data - 接收到的单个字节
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
            case '#': Uart_Mode = 2; break;  // 单舵机
            case '{': Uart_Mode = 3; break;  // 多舵机
            default:  return;                // 非法字符，继续等待
        }
        buf_index = 0;
    }
    
    // 存储当前字节
    Uart_ReceiveBuf[buf_index++] = rx_data;

    // 根据帧尾判断接收是否完成
    if ((Uart_Mode == 4 && rx_data == '>') ||
        (Uart_Mode == 1 && rx_data == '!') ||
        (Uart_Mode == 2 && rx_data == '!') ||
        (Uart_Mode == 3 && rx_data == '}'))
    {
        Uart_ReceiveBuf[buf_index] = '\0';  // 添加字符串结束符
        Uart_GetOK = 1;                     // 通知主循环处理
    }
    SetPrintfUart(2);
    printf("%s", Uart_ReceiveBuf); // 总线发送
}
