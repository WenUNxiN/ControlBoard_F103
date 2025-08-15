/********************************************
1.解析协议

********************************************/
#include "Protocol.h"
#include <stdio.h>
#include <string.h>

#include "Application.h"

extern const DH_Params dh_params[5];

int pwm_value[7];
u8 AI_mode = 255; // 运行的传感器的模式
char cmd_return[1024];
eeprom_info_t eeprom_info;

JointAngles solutions;

/***********************************************
    函数名称:void Rewrite_Eeprom(void)
    功能介绍:把eeprom_info写入到W25Q64_INFO_ADDR_SAVE_STR位置
    函数参数:	无
    返回值:无
 ***********************************************/
void Rewrite_Eeprom(void)
{
    w25x_erase_sector(W25Q64_INFO_ADDR_SAVE_STR / 4096);
    w25x_writeS((u8 *)(&eeprom_info), W25Q64_INFO_ADDR_SAVE_STR, sizeof(eeprom_info_t));
}

/***********************************************
    函数名称:void Parse_Action(char *Uart_ReceiveBuf)
    功能介绍:处理 #000P1500T1000! 类似的字符串
    函数参数:	*Uart_ReceiveBuf：要处理的字符串
    返回值:无
 ***********************************************/
void Parse_Action(char *Uart_ReceiveBuf)
{
    u16 index, time, i = 0;
    int bias, len;
    float pwm;
    // UartAll_Printf(Uart_ReceiveBuf);

    // 调整偏差指令
    if (Uart_ReceiveBuf[0] == '#' && Uart_ReceiveBuf[4] == 'P' && Uart_ReceiveBuf[5] == 'S' && Uart_ReceiveBuf[6] == 'C' && Uart_ReceiveBuf[7] == 'K' && Uart_ReceiveBuf[12] == '!') // 带入偏差调节
    {
        // PWM舵机对应处理
        // 下标
        index = (Uart_ReceiveBuf[1] - '0') * 100 + (Uart_ReceiveBuf[2] - '0') * 10 + (Uart_ReceiveBuf[3] - '0');
        bias = (Uart_ReceiveBuf[9] - '0') * 100 + (Uart_ReceiveBuf[10] - '0') * 10 + (Uart_ReceiveBuf[11] - '0');
        if ((bias >= -100) && (bias <= 100) && (index < DJ_NUM))
        {

            if (Uart_ReceiveBuf[8] == '+')
            {
                duoji_doing[index].cur = duoji_doing[index].cur - eeprom_info.dj_bias_pwm[index] + bias;
                eeprom_info.dj_bias_pwm[index] = bias;
            }
            else if (Uart_ReceiveBuf[8] == '-')
            {
                duoji_doing[index].cur = duoji_doing[index].cur - eeprom_info.dj_bias_pwm[index] - bias;
                eeprom_info.dj_bias_pwm[index] = -bias;
            }
            duoji_doing[index].aim = duoji_doing[index].cur;
            duoji_doing[index].inc = 0.001;
            // Rewrite_Eeprom();
        }

        // 总线舵机对应处理
        SetPrintfUart(2);
        printf("%s", Uart_ReceiveBuf); // 总线发送
    }
    // 停止指令
    else if (Uart_ReceiveBuf[0] == '#' && Uart_ReceiveBuf[4] == 'P' && Uart_ReceiveBuf[5] == 'D' && Uart_ReceiveBuf[6] == 'S' && Uart_ReceiveBuf[7] == 'T' && Uart_ReceiveBuf[8] == '!')
    {
        // PWM舵机对应处理
        index = (Uart_ReceiveBuf[1] - '0') * 100 + (Uart_ReceiveBuf[2] - '0') * 10 + (Uart_ReceiveBuf[3] - '0');
        PwmServoStop(index);
        return;
    }

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

            //  手爪限位
            if (index == 5)
            {
                if (pwm >= 2000)
                    pwm = 2000;
                if (pwm <= 1200)
                    pwm = 1200;
            }
            if (index == 3)
            {
                if (pwm >= 1700)
                    pwm = 1700;
            }
            if (index == 2)
            {
                if (pwm >= 2100)
                    pwm = 2100;
                if (pwm <= 900)
                    pwm = 900;
            }
            if (index == 1)
            {
                if (pwm >= 2100)
                    pwm = 2100;
                if (pwm <= 900)
                    pwm = 900;
            }

            // PWM舵机对应处理
            pwm += eeprom_info.dj_bias_pwm[index];
            PwmServo_DoingSet(index, pwm, time);

//            SetPrintfUart(1);
//            printf("\r\nindex:%d,pwm:%g,time:%d\r\n", index, pwm, time); // 返回解析的数据

            SetPrintfUart(2);
            printf("#%03dP%04dT%04d!", (u16)index, (u16)pwm, (u16)time); // 总线发送
        }
        else
        {
            i++;
        }
    }
}

/***********************************************
    函数名称:void Save_Action(char *str)
    功能介绍:动作组保存函数
                只有用<>包含的字符串才能在此函数中进行解析
    函数参数:	*str：保存动作组字符串
    返回值:无
 ***********************************************/
void Save_Action(char *str)
{
    int32_t action_index = -1;

    if (str[1] == '$' && str[2] == '!') // 删除开机动作
    {
        eeprom_info.pre_cmd[PRE_CMD_SIZE] = 0;
        Rewrite_Eeprom();
        Uart1_Print("@CLEAR PRE_CMD OK!");
        return;
    }
    else if (str[1] == '$') // 设置开机动作
    {
        memset(eeprom_info.pre_cmd, 0, sizeof(eeprom_info.pre_cmd));
        strcpy(eeprom_info.pre_cmd, str + 1);        // 对字符串进行复制
        eeprom_info.pre_cmd[strlen(str) - 2] = '\0'; // 赋值字符0
        eeprom_info.pre_cmd[PRE_CMD_SIZE] = FLAG_VERIFY;
        Rewrite_Eeprom();
        Uart1_Print("@SET PRE_CMD OK!"); //
        return;
    }

    // 获取动作的组号如果不正确，或是第6个字符不是#则认为字符串错误
    action_index = (str[2] - '0') * 1000 + (str[3] - '0') * 100 + (str[4] - '0') * 10 + (str[5] - '0');
    // Uart1_Print("@GET ACTION!");
    // <G0000#000P1500T1000!>
    if ((action_index < 0) || str[6] != '#')
    {
        Uart1_Print("E");
        return;
    }

    if ((action_index * ACTION_SIZE % 4096) == 0)
    {
        w25x_erase_sector(action_index * ACTION_SIZE / 4096);
    }
    // 把尖括号替换成大括号直接存储到存储芯片里面去，则在执行动作组的时候直接拿出来解析就可以了
    Replace_Char(str, '<', '{');
    Replace_Char(str, '>', '}');

    w25x_write((u8 *)str, action_index * ACTION_SIZE, strlen(str) + 1);

    // 反馈一个A告诉上位机我已经接收到了
    Uart1_Print("A");
    return;
}

// 解析DGT动作执行指令
void Parse_Group_Cmd(char *cmd)
{
    int pos = 0;
    if (pos = Str_Contain_Str(cmd, "$DGT:"), pos)
    {
        if (sscanf((char *)cmd, "$DGT:%d-%d,%d!", &group_num_start, &group_num_end, &group_num_times))
        {
            group_do_ok = 1;
            if (group_num_start != group_num_end)
            {
                do_start_index = group_num_start;
                do_time = group_num_times;
                group_do_ok = 0;
            }
            else
            {
                DoGroup_Once(group_num_start);
            }
        }
    }
}

// 命令模式 $XXX!
/*
        所有舵机停止命令：    $DST!
        第x个舵机停止命令：   $DST:x!
        单片机重启命令：$RST!
        检查动作组x到y组命令：$CGP:x-y!
        执行第x个动作：       $DGS:x!
        执行第x到y组动作z次： $DGT:x-y,z!
        小车左x、右轮y速度:   $DCR:x,y!
        所有舵机复位命令：    $DJR!
        获取应答信号：        $GETA!
        获取智能信号：        $SMODE1!
        $KMS:%d,%d,%d,%d!  参数：x, y, z, time
        设置角度：$Angle:%d,%f,%f!   舵机号 角度 舵机最大角度
*/
// 命令解析函数
/***********************************************
    函数名称:void Parse_Cmd(char *cmd)
    功能介绍:命令解析函数
    函数参数:	*cmd：要处理的命令
    返回值:无
 ***********************************************/
void Parse_Cmd(char *cmd)
{
    int pos = 0, index = 0, int1 = 0, int2 = 0,int3 = 0,int4 = 0;
    float angle = 0, Servo_angle = 0;

    if (pos = Str_Contain_Str(cmd, "$DRS!"), pos)
    {
        SetPrintfUart(1);
        printf("hello word!");
        PwmServoStop(255);
    }
    else if (pos = Str_Contain_Str(cmd, "$DST!"), pos)
    {
        group_do_ok = 1;
        SetPrintfUart(1);
        printf("Parse_Cmd OK");
        UartAll_Printf("$DST! OK");
        PwmServoStop(255);
        AI_mode = 255;
    }
    else if (pos = Str_Contain_Str(cmd, "$DST:"), pos)
    {
        if (sscanf((char *)cmd, "$DST:%d!", &index))
        {
            PwmServoStop(index);
            sprintf((char *)cmd_return, "#%03dPDST!\r\n", (int)index);
            UartAll_Printf(cmd_return);
            memset(cmd_return, 0, sizeof(cmd_return));
        }
    }
    else if (pos = Str_Contain_Str(cmd, "$RST!"), pos)
    {
        Soft_Reset();
    }
    else if (pos = Str_Contain_Str(cmd, "$DGS:"), pos)
    {
        if (sscanf((char *)cmd, "$DGS:%d!", &int1))
        {
            group_do_ok = 1;
            DoGroup_Once(int1);
        }
    }
    else if (pos = Str_Contain_Str(cmd, "$DGT:"), pos)
    {
        if (sscanf((char *)cmd, "$DGT:%d-%d,%d!", &group_num_start, &group_num_end, &group_num_times))
        {
            SetPrintfUart(1);
            printf("DGT   DGT\r\n");
            group_do_ok = 1;
            if (group_num_start != group_num_end)
            {
                do_start_index = group_num_start;
                do_time = group_num_times;
                group_do_ok = 0;
            }
            else
            {
                DoGroup_Once(group_num_start);
            }
        }
    }
    else if (pos = Str_Contain_Str(cmd, "$DJR!"), pos)
    {
        UartAll_Printf("#255P1500T2000!");
        PwmServo_DoingSet(255, 1500, 2000);
        AI_mode = 255;
    }
    else if (pos = Str_Contain_Str(cmd, "$SMODE"), pos)
    {
        if (sscanf((char *)Uart_ReceiveBuf, "$SMODE%d!", &int1))
        {
            AI_mode = int1;
            if (int1 == 0)
                UartAll_Printf("#010P1512T0000!");
            if (int1 == 2)
                UartAll_Printf("#010P1511T0000!");
        }
    }
    else if (pos = Str_Contain_Str(cmd, "$GETA!"), pos)
    {
        SetPrintfUart(1);
        printf("AAA");
    }
    else if (pos = Str_Contain_Str(cmd, "$SMODE"), pos)
    {
        if (sscanf((char *)Uart_ReceiveBuf, "$SMODE%d!", &int1))
        {
            if (int1 == 0)
                UartAll_Printf("#010P1512T0000!");
            if (int1 == 2)
                UartAll_Printf("#010P1511T0000!");
        }
    }
    else if (pos = Str_Contain_Str(cmd, "$SMART_STOP!"), pos)
    {

        Delay_ms(10);
        Parse_Action("#255PDST!");
        Delay_ms(10);
        SetPrintfUart(1);
        printf("#006P1500T0000!#007P1500T0000!");
        SetPrintfUart(2);
        printf("#006P1500T0000!#007P1500T0000!");
        Delay_ms(10);
        printf("@OK!");
        Delay_ms(10);
    }
    else if (pos = Str_Contain_Str(cmd, "$KMS:"), pos)
    {
        if (sscanf((char *)cmd, "$KMS:%d,%d,%d,%d!", &int1, &int2, &int3, &int4))
        {
            Uart1_Print("Try to find best pos:\r\n");
            Pose target_pose = {
                        .x = (float)int1, .y = (float)int2, .z = (float)int3, 
                        .roll = 0.0f, .pitch = (float)-90, .yaw = (float)0};
            int kms_value = inverseKinematics(&target_pose, &solutions);
            if (kms_value == 0)
            {
                printf("IK Solution found:\n");
                for (int i = 0; i < 6; i++)
                {
                    SetPrintfUart(1);
                    printf("Joint %d: %.2f degrees\n", i, solutions.theta[i]);
                }
                angle_to_PWM();
                for (int i = 0; i < 6; i++)
                {
                    sprintf((char *)cmd_return, "#%03dP%04dT%04d!\n", i, pwm_value[i], int4);
                    Uart1_Print(cmd_return);
                    Parse_Action(cmd_return);
                }
            }
            else
            {
                printf("Target pose is unreachable:%d.\n", kms_value);
            }
        }
        // 智能控制相关指令，
    }
    else if (pos = Str_Contain_Str(cmd, "$Angle:"), pos)
    {
        if (sscanf((char *)cmd, "$Angle:%d,%f,%f!", &index, &angle, &Servo_angle))
        {
            SetPrintfUart(1);
            printf("Angle   Angle\r\n");
            PwmServo_SetAngle(index, angle, Servo_angle);
        }
    }
    else if (pos = Str_Contain_Str(cmd, "$BEEP!"), pos)
    {
        SetPrintfUart(1);
        printf("buzzer\r\n");
        BUZZER_ON();
        Delay_ms(30);
        BUZZER_OFF();
        Delay_ms(30);
        BUZZER_ON();
        Delay_ms(30);
        BUZZER_OFF();
        Delay_ms(30);
        BUZZER_ON();
        Delay_ms(30);
        BUZZER_OFF();
    }
}

void angle_to_PWM()
{
    pwm_value[0] = 1500 + solutions.theta[0] * 2000 / 270;
    pwm_value[1] = 1500 + solutions.theta[1] * 2000 / 270;
    pwm_value[2] = 1500 - solutions.theta[2] * 2000 / 270;
    pwm_value[4] = 1500 + solutions.theta[3] * 2000 / 270;
    pwm_value[3] = 1500 + solutions.theta[4] * 2000 / 270;
    pwm_value[5] = 1500 + solutions.theta[5] * 2000 / 270;
    // sprintf((char *)cmd_return, "#001P%4dT%4d!\r\n", pwm_value[0]);
    // UartAll_Printf(cmd_return);
}
