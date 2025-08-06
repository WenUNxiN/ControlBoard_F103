#include "Uart.h"
#include <stdio.h> //标准库文件

u8 SW_Uart = 1;
char Uart_ReceiveBuf[UART_BUF_SIZE];
u16 Uart_GetOK;
u8 Uart_Mode;

/***********************************************
	函数名称:	void My_Uart1_Init(u32 baud)
	功能介绍:	串口1初始化，连接usb的串口
	函数参数:	无
	返回值:	无
 ***********************************************/
/* 初始化串口1 */
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
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							/* 字长为8位数据格式 */
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

/***********************************************
	函数名称:	uart2_init()
	功能介绍:	初始化串口2，总线串口
	函数参数:	baud 波特率
	返回值:		无
 ***********************************************/
void Uart2_Init(u32 baud)
{

	//	 /* 全双工代码 */
	//	GPIO_InitTypeDef GPIO_InitStructure;
	//	USART_InitTypeDef USART_InitStructure;
	//	NVIC_InitTypeDef NVIC_InitStructure;

	//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//	USART_InitStructure.USART_BaudRate = baud;
	//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//	USART_InitStructure.USART_Parity = USART_Parity_No;
	//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//	USART_Init(USART2, &USART_InitStructure);

	//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//	NVIC_Init(&NVIC_InitStructure);

	//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//	USART_ITConfig(USART2, USART_IT_TXE, DISABLE); /* 禁止串口发送中断 */

	//	USART_Cmd(USART2, ENABLE);

	/* 半双工代码 */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_HalfDuplexCmd(USART2, ENABLE); // 注意这个，启动半双工模式

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE); /* 禁止串口发送中断 */

	USART_Cmd(USART2, ENABLE);
}

/***********************************************
	函数名称:	uart3_init()
	功能介绍:	初始化串口3,用户串口
	函数参数:	baud 波特率
	返回值:		无
 ***********************************************/
void Uart3_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE); /* 禁止串口发送中断 */

	USART_Cmd(USART3, ENABLE);
}

/* 重定义fputc函数,写这个函数可以使用printf,记得开启Use MicroLIB */
int fputc(int ch, FILE *f)
{
	if (SW_Uart == 1)
	{ // 串口1
		while ((USART1->SR & 0X40) == 0)
			; // 循环发送,直到发送完毕
		USART1->DR = (u8)ch;
		return ch;
	}
	else
	{
		if (SW_Uart == 2)
		{ // 串口2
			while ((USART2->SR & 0X40) == 0)
				; // 循环发送,直到发送完毕
			USART2->DR = (u8)ch;
			return ch;
		}
		else
		{
			if (SW_Uart == 3)
			{ // 串口3
				while ((USART3->SR & 0X40) == 0)
					; // 循环发送,直到发送完毕
				USART3->DR = (u8)ch;
				return ch;
			}
			else
			{
				return 0;
			}
		}
	}
}
/***********************************************
	函数名称:	void My_SetPrintfUart(u8 sw_uart)
	功能介绍:	选择重定义fputc函数的串口选择
	函数参数:	sw_uart 1~3
	返回值:	无
 ***********************************************/
void SetPrintfUart(u8 sw_uart)
{
	SW_Uart = sw_uart;
}

/***********************************************
	函数名称:void UartAll_Printf(char *str)
	功能介绍:UartAll_Printf全部串口发送
	函数参数:	*str：字符串（可通过sprintf合成发送）
	返回值:		无
 ***********************************************/
void UartAll_Printf(char *str)
{
	SetPrintfUart(1);
	printf("%s", str);
	SetPrintfUart(2);
	printf("%s", str);
	SetPrintfUart(3);
	printf("%s", str);
}

void Uart1_Print(char *str)
{
	// SetPrintfUart(1);
	// printf("%s",str);
	while (*str)
	{
		USART_SendData(USART1, *str++);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			;
	}
	return;
}

/***********************************************
	函数名称:	USART1_IRQHandler()
	功能介绍:	串口1中断服务程序
	函数参数:	无
	返回值:无
 ***********************************************/
void USART1_IRQHandler(void) /* 最后数据发送\r\n结束 */
{
	u8 sbuf_bak;
	static u16 buf_index = 0;

	if (USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)
	{
		sbuf_bak = USART_ReceiveData(USART1);
		// USART_SendData(USART1, sbuf_bak);
		if (Uart_GetOK)
			return;
		if (sbuf_bak == '<')
		{
			Uart_Mode = 4;
			buf_index = 0;
		}
		else if (Uart_Mode == 0)
		{
			if (sbuf_bak == '$')
			{
				// 命令模式 $XXX!
				Uart_Mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				// 单舵机模式	#000P1500T1000! 类似这种命令
				Uart_Mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				// 多舵机模式	{#000P1500T1000!#001P1500T1000!} 多个单舵机命令用大括号括起来
				Uart_Mode = 3;
			}
			else if (sbuf_bak == '<')
			{
				// 保存动作组模式	<G0000#000P1500T1000!#001P1500T1000!B000!> 用尖括号括起来 带有组序号
				Uart_Mode = 4;
			}
			buf_index = 0;
		}

		Uart_ReceiveBuf[buf_index++] = sbuf_bak;

		if ((Uart_Mode == 4) && (sbuf_bak == '>'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 1) && (sbuf_bak == '!'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 2) && (sbuf_bak == '!'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 3) && (sbuf_bak == '}'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}

		if (buf_index >= UART_BUF_SIZE)
			buf_index = 0;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

/***********************************************
	函数名称:	void USART2_IRQHandler(void)
	功能介绍:	串口2中断函数
	函数参数:	无
	返回值:无
 ***********************************************/
void USART2_IRQHandler(void)
{
	u8 sbuf_bak;
	static u16 buf_index = 0;

	if (USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET)
	{
		sbuf_bak = USART_ReceiveData(USART2);
		if (Uart_GetOK)
			return;
		if (sbuf_bak == '<')
		{
			Uart_Mode = 4;
			buf_index = 0;
		}
		else if (Uart_Mode == 0)
		{
			if (sbuf_bak == '$')
			{
				// 命令模式 $XXX!
				Uart_Mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				// 单舵机模式	#000P1500T1000! 类似这种命令
				Uart_Mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				// 多舵机模式	{#000P1500T1000!#001P1500T1000!} 多个单舵机命令用大括号括起来
				Uart_Mode = 3;
			}
			else if (sbuf_bak == '<')
			{
				// 保存动作组模式	<G0000#000P1500T1000!#001P1500T1000!B000!> 用尖括号括起来 带有组序号
				Uart_Mode = 4;
			}
			buf_index = 0;
		}

		Uart_ReceiveBuf[buf_index++] = sbuf_bak;

		if ((Uart_Mode == 4) && (sbuf_bak == '>'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 1) && (sbuf_bak == '!'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 2) && (sbuf_bak == '!'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 3) && (sbuf_bak == '}'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}

		if (buf_index >= UART_BUF_SIZE)
			buf_index = 0;
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

/***********************************************
	函数名称:	void USART3_IRQHandler(void)
	功能介绍:	串口3中断函数
	函数参数:	无
	返回值:无
 ***********************************************/
void USART3_IRQHandler(void)
{
	u8 sbuf_bak;
	static u16 buf_index = 0;
	if (USART_GetFlagStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		sbuf_bak = USART_ReceiveData(USART3);
		USART_SendData(USART1, sbuf_bak);
		if (Uart_GetOK)
			return;
		if (sbuf_bak == '<')
		{
			Uart_Mode = 4;
			buf_index = 0;
		}
		else if (Uart_Mode == 0)
		{
			if (sbuf_bak == '$')
			{
				Uart_Mode = 1;
			}
			else if (sbuf_bak == '#')
			{
				Uart_Mode = 2;
			}
			else if (sbuf_bak == '{')
			{
				Uart_Mode = 3;
			}
			else if (sbuf_bak == '<')
			{
				Uart_Mode = 4;
			}
			buf_index = 0;
		}

		Uart_ReceiveBuf[buf_index++] = sbuf_bak;

		if ((Uart_Mode == 4) && (sbuf_bak == '>'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 1) && (sbuf_bak == '!'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 2) && (sbuf_bak == '!'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}
		else if ((Uart_Mode == 3) && (sbuf_bak == '}'))
		{
			Uart_ReceiveBuf[buf_index] = '\0';
			Uart_GetOK = 1;
		}

		if (buf_index >= UART_BUF_SIZE)
			buf_index = 0;
	}
}

/***********************************************
	函数名称:	void My_Uart_DataParse(char rx_data,uint8_t uart_num)
	功能介绍:	解析串口接收的数据
	函数参数:	uart_data 串口接收的数据
				uart_num 串口号
	返回值:无
 ***********************************************/
void Uart_DataParse(char rx_data, uint8_t uart_num)
{
	static u16 buf_index = 0;

	if (Uart_GetOK)
		return;

	if (Uart_Mode == 0) // 空闲
	{
		// 判断第一个字符
		if (rx_data == '$')
		{
			Uart_Mode = 1;
		}
		else if (rx_data == '#')
		{
			// 单舵机模式	#000P1500T1000! 类似这种命令
			Uart_Mode = 2;
		}
		else if (rx_data == '{')
		{
			// 多舵机模式	{#000P1500T1000!#001P1500T1000!} 多个单舵机命令用大括号括起来
			Uart_Mode = 3;
		}
		else if (rx_data == '<')
		{
			// 保存动作组模式	<G0000#000P1500T1000!#001P1500T1000!B000!> 用尖括号括起来 带有组序号
			Uart_Mode = 4;
		}
		buf_index = 0; // 清空缓存区
	}
	// 进行缓存
	Uart_ReceiveBuf[buf_index++] = rx_data;
	// 命令模式 帧头 $ 帧尾 !
	if ((Uart_Mode == 1) && (rx_data == '!'))
	{
		Uart_ReceiveBuf[buf_index] = '\0'; // 填充结束位
		Uart_GetOK = 1;					   // 接受完成标志位
	}
	// 单舵机模式 帧头 # 帧尾 !
	else if ((Uart_Mode == 2) && (rx_data == '!'))
	{
		Uart_ReceiveBuf[buf_index] = '\0';
		Uart_GetOK = 1;
	}
	// 多舵机模式 帧头 { 帧尾 }
	else if ((Uart_Mode == 3) && (rx_data == '}'))
	{
		Uart_ReceiveBuf[buf_index] = '\0';
		Uart_GetOK = 1;
	}
	// 保存动作组模式 帧头 < 帧尾 >
	else if ((Uart_Mode == 4) && (rx_data == '>'))
	{
		Uart_ReceiveBuf[buf_index] = '\0';
		Uart_GetOK = 1;
	}
	// 超出大小则清空
	if (buf_index >= UART_BUF_SIZE)
		buf_index = 0;
}
