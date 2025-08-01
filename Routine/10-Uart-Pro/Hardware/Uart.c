/**
  ******************************************************************************
  * @file    Uart.c
  * @author  IMSY
  * @version V1.0
  * @date    2025-05-20
  * @brief   USART1 驱动：初始化、发送、接收中断、printf 重定向
  ******************************************************************************
  */

#include "Uart.h"
#include <stdio.h>          /* 标准库，用于 printf 重定向 */


#if USART1_ENABLE
/*-------------------- 接收缓冲区定义 --------------------*/
#define RX_BUFFER_LEN1   1024                /* 接收环形缓冲区大小 */
static uint8_t  rx_buffer1[RX_BUFFER_LEN1];   /* 缓冲区实体 */
static uint32_t rx_cnt1 = 0;                 /* 当前已接收字节计数 */

/*-------------------- 串口1初始化 --------------------*/
/**
  * @brief  USART 初始化
  * @param  baud 目标波特率，例如 115200
  * @retval 无
  * @note   PA9-TX, PA10-RX；开启接收中断+空闲中断
  */
void Uart1_Init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. 使能 GPIOA 与 USART1 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    /* 2. 复位 USART1 寄存器到默认值 */
    USART_DeInit(USART1);

    /* 3. 配置 PA9-TX 为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 4. 配置 PA10-RX 为浮空输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 5. 串口工作参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    /* 6. 中断优先级配置 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 7. 使能接收寄存器非空中断 & 空闲中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    /* 8. 使能 USART1 */
    USART_Cmd(USART1, ENABLE);
}

/*-------------------- 发送接口 --------------------*/
/**
  * @brief  发送单个字节
  * @param  ucch 待发送字符
  * @retval 无
  */
void Uart1_send_data(u8 ucch)
{
    USART_SendData(USART1, ucch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); /* 等待发送完成 */
}

/**
  * @brief  发送字符串
  * @param  ucstr 指向以 '\0' 结尾的字符串
  * @retval 无
  */
void Uart1_send_String(uint8_t *ucstr)
{
    while (ucstr && *ucstr)        /* 地址非空且字符非 '\0' */
    {
        Uart1_send_data(*ucstr++);
    }
}
#endif

#if USART2_ENABLE
/*-------------------- 接收缓冲区定义 --------------------*/
#define RX_BUFFER_LEN2   1024                /* 接收环形缓冲区大小 */
static uint8_t  rx_buffer2[RX_BUFFER_LEN2];   /* 缓冲区实体 */
static uint32_t rx_cnt2 = 0;                 /* 当前已接收字节计数 */

/*-------------------- 串口2初始化 --------------------*/
/**
  * @brief  USART2 初始化
  * @param  baud 目标波特率，例如 115200
  * @retval 无
  * @note   PA2-TX, PA3-RX；开启接收中断+空闲中断
  */
void Uart2_Init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. 使能 GPIOA 与 USART2 时钟（USART2 在 APB1） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* 2. 复位 USART2 寄存器到默认值 */
    USART_DeInit(USART2);

    /* 3. 配置 PA2-TX 为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 4. 配置 PA3-RX 为浮空输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 5. 串口工作参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure);

    /* 6. 中断优先级配置 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;  /* 改为 USART2 中断号 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 7. 使能接收寄存器非空中断 & 空闲中断 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

    /* 8. 使能 USART2 */
    USART_Cmd(USART2, ENABLE);
}

/*-------------------- 发送接口 --------------------*/
/**
  * @brief  发送单个字节（通过 USART2）
  * @param  ucch 待发送字符
  * @retval 无
  */
void Uart2_send_data(u8 ucch)
{
    USART_SendData(USART2, ucch);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); /* 等待发送完成 */
}

/**
  * @brief  发送字符串（通过 USART2）
  * @param  ucstr 指向以 '\0' 结尾的字符串
  * @retval 无
  */
void Uart2_send_String(uint8_t *ucstr)
{
    while (ucstr && *ucstr)        /* 地址非空且字符非 '\0' */
    {
        Uart2_send_data(*ucstr++);
    }
}
#endif

#if USART3_ENABLE
/*-------------------- 接收缓冲区定义 --------------------*/
#define RX_BUFFER_LEN3   1024                /* 接收环形缓冲区大小 */
static uint8_t  rx_buffer3[RX_BUFFER_LEN3];   /* 缓冲区实体 */
static uint32_t rx_cnt3 = 0;                 /* 当前已接收字节计数 */

/*-------------------- 串口3初始化 --------------------*/
/**
  * @brief  USART3 初始化
  * @param  baud 目标波特率，例如 115200
  * @retval 无
  * @note   PB10-TX, PB11-RX；开启接收中断+空闲中断
  */
void Uart3_Init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. 使能 GPIOB 与 USART3 时钟（USART3 在 APB1） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* 2. 复位 USART3 寄存器到默认值 */
    USART_DeInit(USART3);

    /* 3. 配置 PB10-TX 为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 4. 配置 PB11-RX 为浮空输入 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 5. 串口工作参数 */
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure);

    /* 6. 中断优先级配置 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 7. 使能接收寄存器非空中断 & 空闲中断 */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    /* 8. 使能 USART3 */
    USART_Cmd(USART3, ENABLE);
}

/*-------------------- 发送接口 --------------------*/
/**
  * @brief  发送单个字节（通过 USART3）
  * @param  ucch 待发送字符
  * @retval 无
  */
void Uart3_send_data(u8 ucch)
{
    USART_SendData(USART3, ucch);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); /* 等待发送完成 */
}

/**
  * @brief  发送字符串（通过 USART3）
  * @param  ucstr 指向以 '\0' 结尾的字符串
  * @retval 无
  */
void Uart3_send_String(uint8_t *ucstr)
{
    while (ucstr && *ucstr)        /* 地址非空且字符非 '\0' */
    {
        Uart3_send_data(*ucstr++);
    }
}
#endif

/*-------------------- printf 重定向 --------------------*/
/**
  * @brief  重定向 fputc，使 printf 输出到 USART1
  * @note   需在 Keil → Options → Target 中勾选 "Use MicroLIB"
  */
int fputc(int ch, FILE *f)
{
    #if USART_PRINTF == 1
    while (!(USART1->SR & USART_SR_TXE)); /* 等待发送寄存器空 */
    USART1->DR = (uint8_t)ch;
    return ch;
    #endif
    #if USART_PRINTF == 2
    while (!(USART2->SR & USART_SR_TXE)); /* 等待发送寄存器空 */
    USART2->DR = (uint8_t)ch;
    return ch;
    #endif
    #if USART_PRINTF == 3
    while (!(USART3->SR & USART_SR_TXE)); /* 等待发送寄存器空 */
    USART3->DR = (uint8_t)ch;
    return ch;
    #endif
    
}

/*--------------------  USART 中断服务函数 --------------------*/
/**
  * @brief  USART 全局中断服务程序
  * @note   处理接收寄存器非空中断(RXNE) 与 空闲中断(IDLE)
  *         空闲中断用于帧结束判定
  */
#if USART1_ENABLE
void USART1_IRQHandler(void)
{
    /*---- 接收数据寄存器非空中断 ----*/
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); /* 清中断标志 */

        uint8_t data = USART_ReceiveData(USART1);       /* 读数据同时清 RXNE */
        if (rx_cnt1 < RX_BUFFER_LEN1 - 1)                 /* 防溢出 */
            rx_buffer1[rx_cnt1++] = data;
    }

    /*---- 空闲中断（一帧数据结束） ----*/
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART1);

        rx_buffer1[rx_cnt1] = '\0';   /* 字符串结束符 */

#if USART1_RECV_CALLBACK
        /* 用户回调处理完整帧 */
        Usart1_on_recv(rx_buffer1, rx_cnt1);
#endif
        rx_cnt1 = 0;                 /* 清理缓冲区 */
    }
}
#endif

#if USART2_ENABLE
void USART2_IRQHandler(void)
{
    /*---- 接收数据寄存器非空中断 ----*/
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); /* 清中断标志 */

        uint8_t data = USART_ReceiveData(USART2);       /* 读数据同时清 RXNE */
        if (rx_cnt2 < RX_BUFFER_LEN2 - 1)                 /* 防溢出 */
            rx_buffer2[rx_cnt++] = data;
    }

    /*---- 空闲中断（一帧数据结束） ----*/
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART2);

        rx_buffer2[rx_cnt2] = '\0';   /* 字符串结束符 */

#if USART2_RECV_CALLBACK
        /* 用户回调处理完整帧 */
        Usart2_on_recv(rx_buffer2, rx_cnt);
#endif
        rx_cnt = 0;                 /* 清理缓冲区 */
    }
}
#endif

#if USART3_ENABLE
void USART3_IRQHandler(void)
{
    /*---- 接收数据寄存器非空中断 ----*/
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); /* 清中断标志 */

        uint8_t data = USART_ReceiveData(USART3);       /* 读数据同时清 RXNE */
        if (rx_cnt3 < RX_BUFFER_LEN3 - 1)                 /* 防溢出 */
            rx_buffer3[rx_cnt3++] = data;
    }

    /*---- 空闲中断（一帧数据结束） ----*/
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART3);

        rx_buffer3[rx_cnt3] = '\0';   /* 字符串结束符 */

#if USART3_RECV_CALLBACK
        /* 用户回调处理完整帧 */
        Usart3_on_recv(rx_buffer3, rx_cnt3);
#endif
        rx_cnt3 = 0;                 /* 清理缓冲区 */
    }
} 
#endif
