#ifndef _ACTION__H_
#define _ACTION__H_

#include <string.h> 

#include "sys.h"
#include "SysTick.h"
#include "Uart.h"
#include "PwmServo.h"
#include "Protocol.h"
#include "SPI_Flash.h"

#define ACTION_SIZE 256	  // 一个动作的存储大小

extern u8 group_do_ok;
extern int do_start_index;	// 动作组执行 起始序号
extern int do_time;			// 动作组执行 执行次数
extern int group_num_start; // 动作组执行 起始序号
extern int group_num_end;	// 动作组执行 终止序号
extern int group_num_times; // 动作组执行 起始变量

int GetMaxTime(char *str);
void DoGroup_Once(int group_num);
void Loop_Action(void);

#endif
