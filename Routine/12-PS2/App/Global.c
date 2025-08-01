#include "Global.h"

/***********************************************
    函数名称:u16 My_Str_Contain_Str(char *str,char *str2)
    功能介绍:	判断子串是否包含
    函数参数:	*str：字符串一
                *str2：字符串2
    返回值:无
 ***********************************************/
u16 Str_Contain_Str(char *str, char *str2)
{
    char *str_temp, *str_temp2;
    str_temp = str;
    str_temp2 = str2;
    while (*str_temp)
    {
        if (*str_temp == *str_temp2)
        {
            while (*str_temp2)
            {
                if (*str_temp++ != *str_temp2++)
                {
                    str_temp = str_temp - (str_temp2 - str2) + 1;
                    str_temp2 = str2;
                    break;
                }
            }
            if (!*str_temp2)
            {
                return (str_temp - str);
            }
        }
        else
        {
            str_temp++;
        }
    }
    return 0;
}

void Selection_Sort(int *a, int len)
{
    int i, j, mi, t;
    for (i = 0; i < len - 1; i++)
    {
        mi = i;
        for (j = i + 1; j < len; j++)
        {
            if (a[mi] > a[j])
            {
                mi = j;
            }
        }

        if (mi != i)
        {
            t = a[mi];
            a[mi] = a[i];
            a[i] = t;
        }
    }
}

/***********************************************
    函数名称:int abs_int(int int1)
    功能介绍:	int型 取绝对值函数
    函数参数:int1：取绝对值的数
    返回值:无
 ***********************************************/
int abs_int(int int1)
{
    if (int1 > 0)
        return int1;
    return (-int1);
}

/***********************************************
    函数名称: abs_float(float)
    功能介绍:	float 取绝对值函数
    函数参数:value：取绝对值的数
    返回值:无
 ***********************************************/
float abs_float(float value)
{
    if (value > 0)
    {
        return value;
    }
    return (-value);
}

/***********************************************
    函数名称:int My_Abs_Int(int int1)
    功能介绍:	/字符串中的字符替代函数 把str字符串中所有的ch1换成ch2
    函数参数:*str：字符串
                ch1：字符1
                ch2：字符2
    返回值:无
 ***********************************************/
void Replace_Char(char *str, char ch1, char ch2)
{
    while (*str)
    {
        if (*str == ch1)
        {
            *str = ch2;
        }
        str++;
    }
    return;
}

/***********************************************
    函数名称:void Int_Exchange(int *int1, int *int2)
    功能介绍:两个int变量交换
    函数参数:*int1：变量1
                *int2：变量2
    返回值:无
 ***********************************************/
void Int_Exchange(int *int1, int *int2)
{
    int int_temp;
    int_temp = *int1;
    *int1 = *int2;
    *int2 = int_temp;
}

/**
 * @brief  通用线性映射函数（map）
 * @param  x        当前输入值
 * @param  in_min   输入范围最小值
 * @param  in_max   输入范围最大值
 * @param  out_min  输出范围最小值
 * @param  out_max  输出范围最大值
 * @return          线性映射后的输出值
 *
 * 公式：out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
 * 典型用法：把 0~20 cm 映射到 500~2500 µs
 *     pulse = map(distance_cm, 0, 20, 500, 2500);
 */
float map(float x, float in_min, float in_max,
          float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
