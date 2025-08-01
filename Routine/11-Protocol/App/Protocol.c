/********************************************
1.解析协议
********************************************/
#include "Protocol.h"

/***********************************************
    函数名称:void Parse_Action(char *Uart_ReceiveBuf)
    功能介绍:处理 #000P1500T1000! 类似的字符串
    函数参数:	*Uart_ReceiveBuf：要处理的字符串
    返回值:无
 ***********************************************/
void Parse_Action(char *Uart_ReceiveBuf)
{
    u16 index, time, i = 0;
    int len;
    float pwm;

    len = strlen((char *)Uart_ReceiveBuf); // 获取串口接收数据的长度
    // 指定舵机位置指令                                       
    while (Uart_ReceiveBuf[i] && (len >= i))
    {
        if (Uart_ReceiveBuf[i] == '#') // 帧头
        {
            index = 0;
            i++;
            while (Uart_ReceiveBuf[i] && Uart_ReceiveBuf[i] != 'P') // 解析PWM数值
            {
                index = index * 10 + Uart_ReceiveBuf[i] - '0';
                i++;
            }
        }
        else if (Uart_ReceiveBuf[i] == 'P')
        {
            pwm = 0;
            i++;
            while (Uart_ReceiveBuf[i] && Uart_ReceiveBuf[i] != 'T') // 解析时间值
            {
                pwm = pwm * 10 + Uart_ReceiveBuf[i] - '0';
                i++;
            }
        }
        else if (Uart_ReceiveBuf[i] == 'T')
        {
            time = 0;
            i++;
            while (Uart_ReceiveBuf[i] && Uart_ReceiveBuf[i] != '!') // 帧尾
            {
                time = time * 10 + Uart_ReceiveBuf[i] - '0';
                i++;
            }

            // PWM舵机对应处理
            PwmServo_DoingSet(index, pwm, time);
        }
        else
        {
            i++;
        }
    }
}

/*************************************************
 * 名称：Parse_Cmd
 * 功能：解析并响应上位机发送的纯文本命令
 * 支持的命令：
 *   $POSDEV!
 *   $SSM!
 * 参数：cmd —— 以 '\0' 结尾的完整命令字符串
 * 返回：无
 *************************************************/
void Parse_Cmd(char *cmd)
{
    /* Str_Contain_Str 返回非 0 表示找到子串 */
    if (Str_Contain_Str(cmd, "$POSDEV!"))
    {
        printf("%s\n", cmd); // 原样回显
    }
    else if (Str_Contain_Str(cmd, "$SSM!"))
    {
        printf("%s\n", cmd); // 原样回显
    }
}
