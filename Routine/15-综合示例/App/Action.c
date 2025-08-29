#include "Action.h"

u8 group_do_ok = 1;

int do_start_index;  // 动作组执行 起始序号
int do_time;         // 动作组执行 执行次数
int group_num_start; // 动作组执行 起始序号
int group_num_end;   // 动作组执行 终止序号
int group_num_times; // 动作组执行 起始变量

u32 action_time = 0;

/***********************************************
    函数名称:GetMaxTime(char *str)
    功能介绍:获取最大时间
    函数参数:*str：命令字符串
    返回值:时间
 ***********************************************/
int GetMaxTime(char *str)
{
    int i = 0, max_time = 0, tmp_time = 0;
    while (str[i])
    {
        if (str[i] == 'T')
        {
            tmp_time = (str[i + 1] - '0') * 1000 + (str[i + 2] - '0') * 100 + (str[i + 3] - '0') * 10 + (str[i + 4] - '0');
            if (tmp_time > max_time)
                max_time = tmp_time;
            i = i + 4;
            continue;
        }
        i++;
    }
    return max_time;
}

/***********************************************
    函数名称:void DoGroup_Once(int group_num)
    功能介绍:	执行一次指定动作组
    函数参数:	group_num 动作组序号
    返回值:无
 ***********************************************/
void DoGroup_Once(int group_num)
{
    // 将Uart_ReceiveBuf清零
    memset(Uart_ReceiveBuf, 0, sizeof(Uart_ReceiveBuf));
    // 从存储芯片中读取第group_num个动作组
    w25x_read((u8 *)Uart_ReceiveBuf, group_num * ACTION_SIZE, ACTION_SIZE);
    // 获取最大的组时间
    action_time = GetMaxTime(Uart_ReceiveBuf);
    // 把读取出来的动作组传递到Parse_Action执行
    Parse_Action(Uart_ReceiveBuf);
}

/***********************************************
    函数名称:	void Loop_Action(void)
    功能介绍:	动作组批量执行
    函数参数无
    返回值:无
 ***********************************************/
void Loop_Action(void)
{
    static u32 systick_ms_bak = 0;
    if (group_do_ok == 0)
    {
        if (Millis() - systick_ms_bak > action_time)
        {
            systick_ms_bak = Millis();
            if (group_num_times != 0 && do_time == 0)
            {
                group_do_ok = 1;
                uart1_send_str("@GroupDone!");
                return;
            }
            // 调用do_start_index个动作
            DoGroup_Once(do_start_index);

            if (group_num_start < group_num_end)
            {
                if (do_start_index == group_num_end)
                {
                    do_start_index = group_num_start;
                    if (group_num_times != 0)
                    {
                        do_time--;
                    }
                    return;
                }
                do_start_index++;
            }
            else
            {
                if (do_start_index == group_num_end)
                {
                    do_start_index = group_num_start;
                    if (group_num_times != 0)
                    {
                        do_time--;
                    }
                    return;
                }
                do_start_index--;
            }
        }
    }
    else
    {
        action_time = 10;
    }
}
