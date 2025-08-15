#ifndef _GLOBAL__H_
#define _GLOBAL__H_

#include "sys.h"

#define W25Q64_INFO_ADDR_SAVE_STR			(((8<<10)-2)<<10)  //(8*1024-1)*1024		//eeprom_info结构体存储的位置

#define DJ_NUM 8
#define PRE_CMD_SIZE 128

/**
 * @brief  将数值限制在指定区间 [lo, hi] 内
 * @param  x   待限制值
 * @param  lo  下限
 * @param  hi  上限
 * @return     若 x < lo 返回 lo；若 x > hi 返回 hi；否则返回 x
 *
 * 用法示例：
 *     float pulse = CLAMP(map(dist_cm, 0, 20, 500, 2500), 500, 2500);
 */
#define CLAMP(x, lo, hi)  ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

extern u8 needSaveFlag;			// 偏差保存标志
extern u32 bias_systick_ms_bak; // 偏差保存标志时间
extern u32 action_time;


typedef struct
{
	u32 dj_record_num;
	char pre_cmd[PRE_CMD_SIZE + 1];  //开机动作指令
	int dj_bias_pwm[DJ_NUM + 1];    //PWM舵机偏差
	int servo_init_pos[DJ_NUM + 1];
} eeprom_info_t;

u16 Str_Contain_Str(char *str,char *str2);//判断是不是包含在其中
int abs_int(int int1);//int类型取绝对值
float abs_float(float value);//float类型取绝对值
float map(float x, float in_min, float in_max,float out_min, float out_max);

void Selection_Sort(int *a, int len);//
void Replace_Char(char *str, char ch1, char ch2);
void Int_Exchange(int *int1, int *int2);


#endif
