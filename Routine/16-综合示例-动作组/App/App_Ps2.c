#include "App_Ps2.h"
#include "Uart.h"
#include "Global.h"
#include "Protocol.h"

#include <string.h>

#define DEAD_ZONE   8
#define MOVE_TH     3

#define PSX_BUTTON_NUM  16      /* 按键个数 */

static s32 pos_x = 150, pos_y = 0, pos_z = 120;   /* 全局坐标，单位 mm */
#define POS_MIN     -200
#define POS_MAX     200
#define SCALE       1          /* 灵敏度，越大越快 */

// 绿灯模式下按键的配置
const char *pre_cmd_set_grn[PSX_BUTTON_NUM] = {
	// 手柄按键功能字符串 红灯模式下使用
	"<PS2_GRN01:#005P0600T2000!^#005PDST!>",																			  // L2
	"<PS2_GRN02:#005P2400T2000!^#005PDST!>",																			  // R2
	"<PS2_GRN03:#004P0600T2000!^#004PDST!>",																			  // L1
	"<PS2_GRN04:#004P2400T2000!^#004PDST!>",																			  // R1
																														  //	"<PS2_RED05:#002P2400T2000!^#002PDST!>", // RU
	"<PS2_RED05:#002P2400T2000!^#002PDST!>", // RU
	"<PS2_GRN06:#003P2400T2000!^#003PDST!>",																			  // RR
	"<PS2_GRN07:#002P0600T2000!^#002PDST!>",																			  // RD
	"<PS2_GRN08:#003P0600T2000!^#003PDST!>",																			  // RL

    "<PS2_GRN09:$DJR!>",					// SE
    "<PS2_GRN10:>",							// AL
    "<PS2_GRN11:>",							// AR
    "<PS2_GRN12:{#000P1500T2000!#001P1500T2000!#002P1500T2000!#003P1500T2000!#004P1500T2000!#005P1500T2000!}^#001PDST!>",	// ST
    
	"<PS2_RED13:#001P0600T2000!^#001PDST!>", // LU
	"<PS2_GRN14:#000P0600T2000!^#000PDST!>",																			  // LR
	"<PS2_GRN15:#001P2400T2000!^#001PDST!>",																			  // LD
	"<PS2_GRN16:#000P2400T2000!^#000PDST!>",																			  // LL
};

//const char *pre_cmd_set_grn[PSX_BUTTON_NUM] = {
//	
//	"<PS2_RED01:#005P0600T2000!^#005PDST!>", // L2
//	"<PS2_RED02:#005P2400T2000!^#005PDST!>", // R2
//	"<PS2_RED03:#004P0600T2000!^#004PDST!>", // L1
//	"<PS2_RED04:#004P2400T2000!^#004PDST!>", // R1
//	"<PS2_RED05:#002P2400T2000!^#002PDST!>", // RU
//	"<PS2_RED06:#003P2400T2000!^#003PDST!>", // RR
//	"<PS2_RED07:#002P0600T2000!^#002PDST!>", // RD
//	"<PS2_RED08:#003P0600T2000!^#003PDST!>", // RL

//	"<PS2_RED09:#006P2400T2000!^#006PDST!>", // SE
//	"<PS2_RED10:#006P0600T2000!^#006PDST!>", // AL

//	"<PS2_RED11:#007P0600T2000!^#007PDST!>", // AR
//	"<PS2_RED12:#007P2400T2000!^#007PDST!>", // ST

//	"<PS2_RED13:#001P0600T2000!^#001PDST!>", // LU
//	"<PS2_RED14:#000P0600T2000!^#000PDST!>", // LR
//	"<PS2_RED15:#001P2400T2000!^#001PDST!>", // LD
//	"<PS2_RED16:#000P2400T2000!^#000PDST!>", // LL

//};

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
    static u32 systick_ms_bak = 0;
    static unsigned char psx_button_bak[2] = {0};

    /* 1. 50 ms 节拍 */
    if (Millis() - systick_ms_bak < 50) return;
    systick_ms_bak = Millis();
    
    SetPrintfUart(1);

    /* 1. 热插拔状态机 → 数据有效才继续 */
    if(!Ps2HotplugTask()) return;        // 离线或静止丢弃期间直接退出

    /* 3. 原有按键处理（不变） */
    if ((psx_button_bak[0] != psx_buf[3]) || (psx_button_bak[1] != psx_buf[4]))
    {
        ParsePsx_Buf(psx_buf + 3, psx_buf[1]);
        psx_button_bak[0] = psx_buf[3];
        psx_button_bak[1] = psx_buf[4];
    }

    
    /* 4. ===== 增量保持 + 变化才刷新 ===== */
    s8 joy_now[4];
    for (u8 i = 0; i < 4; ++i) joy_now[i] = (s8)(psx_buf[5 + i] - 128);

    /* 死区 */
    for (u8 i = 0; i < 4; ++i)
        if (abs_int(joy_now[i]) <= DEAD_ZONE) joy_now[i] = 0;

    /* 速度→增量 */
    s16 dx = -joy_now[3] * SCALE / 40;   /* LX 左正 */
    s16 dy = -joy_now[2] * SCALE / 40;   /* LY 前正 */
    s16 dz =  joy_now[1] * SCALE / 40;   /* RY 前负 */

    /* 累加 */
    pos_x += dx;
    pos_y += dy;
    pos_z += dz;

    /* 限幅 */
    if (pos_x < POS_MIN) pos_x = POS_MIN; else if (pos_x > POS_MAX) pos_x = POS_MAX;
    if (pos_y < POS_MIN) pos_y = POS_MIN; else if (pos_y > POS_MAX) pos_y = POS_MAX;
    if (pos_z < 0) pos_z = 0; else if (pos_z > POS_MAX) pos_z = POS_MAX;

    /* 有变化才发命令 */
    if (dx | dy | dz)
    {
        char buf[64];
        sprintf((char*)buf, "$KMS:%d,%d,%d,1000!", pos_x, pos_y, pos_z);
        /* 可选调试信息 */
        printf("%s\r\n",buf);
        Parse_Cmd(buf);

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
					if (mode == PS2_LED_GRN)
					{
						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
					}
					else if (mode == PS2_LED_RED)
					{
//						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
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
					if (mode == PS2_LED_GRN)
					{
						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
					}
					else if (mode == PS2_LED_RED)
					{
//						memcpy((char *)Uart_ReceiveBuf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
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
