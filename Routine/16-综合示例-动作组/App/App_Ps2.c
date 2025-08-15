#include "App_Ps2.h"
#include "Uart.h"
#include "Global.h"
#include "Protocol.h"

#include <string.h>

// 颜色反了
const char *pre_cmd_set_red[PSX_BUTTON_NUM] = {
	// 手柄按键功能字符串 红灯模式下使用
	"<PS2_RED01:#005P0600T2000!^#005PDST!>",																			  // L2
	"<PS2_RED02:#005P2400T2000!^#005PDST!>",																			  // R2
	"<PS2_RED03:#004P0600T2000!^#004PDST!>",																			  // L1
	"<PS2_RED04:#004P2400T2000!^#004PDST!>",																			  // R1
																														  //	"<PS2_RED05:#002P2400T2000!^#002PDST!>", // RU
	"<PS2_RED05:{#000P1500T0200!#001P1600T0200!#002P1550T0200!#003P1500T0200!#004P1500T0200!#005P1550T0200!}^#002PDST!>", // RU    //控制6个舵机   多个舵机以上要加上大括号{}
	"<PS2_RED06:#003P2400T2000!^#003PDST!>",																			  // RR
	"<PS2_RED07:#002P0600T2000!^#002PDST!>",																			  // RD
	"<PS2_RED08:#003P0600T2000!^#003PDST!>",																			  // RL

	"<PS2_RED09:#006P2400T2000!^#006PDST!>", // SE
	"<PS2_RED10:#006P0600T2000!^#006PDST!>", // AL

	"<PS2_RED11:#007P0600T2000!^#007PDST!>", // AR
	"<PS2_RED12:#007P2400T2000!^#007PDST!>", // ST

	//	"<PS2_RED13:#001P0600T2000!^#001PDST!>", // LU
	"<PS2_RED13:{#000P1350T0200!#001P1400T0200!#002P1380T0200!#003P1330T0200!#004P1300T0200!#005P1350T0200!}^#001PDST!>", // LU	//控制6个舵机    多个舵机以上要加上大括号{}
	"<PS2_RED14:#000P0600T2000!^#000PDST!>",																			  // LR
	"<PS2_RED15:#001P2400T2000!^#001PDST!>",																			  // LD
	"<PS2_RED16:#000P2400T2000!^#000PDST!>",																			  // LL
};

const char *pre_cmd_set_grn[PSX_BUTTON_NUM] = {
	// 绿灯模式下按键的配置
	"<PS2_RED01:#005P0600T2000!^#005PDST!>", // L2
	"<PS2_RED02:#005P2400T2000!^#005PDST!>", // R2
	"<PS2_RED03:#004P0600T2000!^#004PDST!>", // L1
	"<PS2_RED04:#004P2400T2000!^#004PDST!>", // R1
	"<PS2_RED05:#002P2400T2000!^#002PDST!>", // RU
	"<PS2_RED06:#003P2400T2000!^#003PDST!>", // RR
	"<PS2_RED07:#002P0600T2000!^#002PDST!>", // RD
	"<PS2_RED08:#003P0600T2000!^#003PDST!>", // RL

	"<PS2_RED09:#006P2400T2000!^#006PDST!>", // SE
	"<PS2_RED10:#006P0600T2000!^#006PDST!>", // AL

	"<PS2_RED11:#007P0600T2000!^#007PDST!>", // AR
	"<PS2_RED12:#007P2400T2000!^#007PDST!>", // ST

	"<PS2_RED13:#001P0600T2000!^#001PDST!>", // LU
	"<PS2_RED14:#000P0600T2000!^#000PDST!>", // LR
	"<PS2_RED15:#001P2400T2000!^#001PDST!>", // LD
	"<PS2_RED16:#000P2400T2000!^#000PDST!>", // LL

	//	"<PS2_RED01:$DCR:0,500,500,0!^$DCR:0,0,0,0!>",			   // L2  左上500     //PS2在绿灯模式下控制小车
	//	"<PS2_RED02:$DCR:500,0,0,500!^$DCR:0,0,0,0!>",			   // R2	右上500
	//	"<PS2_RED03:$DCR:0,1000,1000,0!^$DCR:0,0,0,0!>",		   // L1	左上1000
	//	"<PS2_RED04:$DCR:1000,0,0,1000!^$DCR:0,0,0,0!>",		   // R1	右上1000
	//	"<PS2_RED05:$DCR:1000,1000,1000,1000!^$DCR:0,0,0,0!>",	   // RU	前进1000
	//	"<PS2_RED06:$DCR:1000,-1000,-1000,1000!^$DCR:0,0,0,0!>",   // RR	右平移1000
	//	"<PS2_RED07:$DCR:-1000,-1000,-1000,-1000!^$DCR:0,0,0,0!>", // RD	后退1000
	//	"<PS2_RED08:$DCR:-1000,1000,1000,-1000!^$DCR:0,0,0,0!>",   // RL	左平移1000
	//	"<PS2_RED09:$DJR!>",									   // SE
	//	"<PS2_RED10:>",											   // AL
	//	"<PS2_RED11:>",											   // AR
	//	"<PS2_RED12:$DJR!>",									   // ST
	//	"<PS2_RED13:$DCR:500,500,500,500!^$DCR:0,0,0,0!>",		   // LU	前进500
	//	"<PS2_RED14:$DCR:500,-500,500,-500!^$DCR:0,0,0,0!>",	   // LR	右转500
	//	"<PS2_RED15:$DCR:-500,-500,-500,-500!^$DCR:0,0,0,0!>",	   // LD	后退500
	//	"<PS2_RED16:$DCR:-500,500,-500,500!^$DCR:0,0,0,0!>",	   // LL	左转500
};

/***********************************************
	函数名称:	void AppPs2Init(void)
	功能介绍:	PS2设备控制初始化
	函数参数:	无
	返回值:		无
 ***********************************************/
void AppPs2Init(void)
{
	Ps2_Init(); /* PS2引脚初始化 */
}

/***********************************************
   函数名称:	void AppPs2Run(void)
   功能介绍:	循环执行工作
   函数参数:	无
   返回值:		无
 ***********************************************/
void AppPs2Run(void)
{
	static unsigned char psx_button_bak[2] = {0};
	static u32 systick_ms_bak = 0;

	// 每50ms处理1次
	if (Millis() - systick_ms_bak < 50)
		return;
	systick_ms_bak = Millis();

	Ps2_WriteRead(); /* 读取ps2数据 */

	// 对比两次获取的按键值是否相同 ，相同就不处理，不相同则处理
	if ((psx_button_bak[0] == psx_buf[3]) && (psx_button_bak[1] == psx_buf[4]))
	{
	}
	else
	{
		// 处理buf3和buf4两个字节，这两个字节存储这手柄16个按键的状态
		ParsePsx_Buf(psx_buf + 3, psx_buf[1]);
		psx_button_bak[0] = psx_buf[3];
		psx_button_bak[1] = psx_buf[4];
	}
}

/***********************************************
	函数名称:	ParsePsx_Buf(unsigned char *buf, unsigned char mode)
	功能介绍:	处理手柄按键字符
	函数参数:	*buf:发送字符串
				mode:模式 主要是红灯和绿灯模式
	返回值:		无
 ***********************************************/
void ParsePsx_Buf(unsigned char *buf, unsigned char mode)
{
	u8 i, pos = 0;
	static u16 bak = 0xffff, temp, temp2;
	temp = (buf[0] << 8) + buf[1];
	if (bak != temp)
	{
		temp2 = temp;
		temp &= bak;
		for (i = 0; i < 16; i++)
		{ // 16个按键一次轮询
			if ((1 << i) & temp)
			{
			}
			else
			{
				if ((1 << i) & bak)
				{ // press 表示按键按下了
					memset(Uart_ReceiveBuf, 0, sizeof(Uart_ReceiveBuf));
					if (mode == PS2_LED_RED)
					{
						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_red[i], strlen(pre_cmd_set_red[i]));
					}
					else if (mode == PS2_LED_GRN)
					{
						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
					}
					else
						continue;
					pos = Str_Contain_Str(Uart_ReceiveBuf, "^");
					if (pos)
						Uart_ReceiveBuf[pos - 1] = '\0';
					if (Str_Contain_Str(Uart_ReceiveBuf, "$"))
					{
						uart1_close();
						Uart_GetOK = 0;
						strcpy((char *)cmd_return, (char *)Uart_ReceiveBuf + 11);
						strcpy((char *)Uart_ReceiveBuf, (char *)cmd_return);
						Uart_GetOK = 1;
						uart1_open();
						Uart_Mode = 1;
					}
					else if (Str_Contain_Str(Uart_ReceiveBuf, "#"))
					{
						uart1_close();
						Uart_GetOK = 0;
						strcpy((char *)cmd_return, (char *)Uart_ReceiveBuf + 11);
						strcpy((char *)Uart_ReceiveBuf, (char *)cmd_return);
						Uart_GetOK = 1;
						uart1_open();
						Uart_Mode = 2;
					}
					bak = 0xffff;
				}
				else
				{ // release 表示按键松开了

					memset(Uart_ReceiveBuf, 0, sizeof(Uart_ReceiveBuf));
					if (mode == PS2_LED_RED)
					{
						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_red[i], strlen(pre_cmd_set_red[i]));
					}
					else if (mode == PS2_LED_GRN)
					{
						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
					}
					else
						continue;
					pos = Str_Contain_Str(Uart_ReceiveBuf, "^");
					if (pos)
					{
						if (Str_Contain_Str(Uart_ReceiveBuf + pos, "$"))
						{
							strcpy((char *)cmd_return, (char *)Uart_ReceiveBuf + pos);
							cmd_return[strlen((char *)cmd_return) - 1] = '\0';
							strcpy((char *)Uart_ReceiveBuf, (char *)cmd_return);
							Parse_Cmd(Uart_ReceiveBuf);
						}
						else if (Str_Contain_Str(Uart_ReceiveBuf + pos, "#"))
						{
							strcpy((char *)cmd_return, (char *)Uart_ReceiveBuf + pos);
							cmd_return[strlen((char *)cmd_return) - 1] = '\0';
							strcpy((char *)Uart_ReceiveBuf, (char *)cmd_return);
							Parse_Action(Uart_ReceiveBuf);
						}
					}
				}
			}
		}
		bak = temp2;
	}
	return;
}
