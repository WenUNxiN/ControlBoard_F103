#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>	//标准库文件
#include <string.h> //标准库文件
#include <math.h>	//标准库文件
#include <stdint.h> //标准库文件
#include <stdarg.h> //标准库文件

#include "stm32f10x_conf.h"
#include "stm32f10x.h"

/*
	初始化函数声明
*/
extern void setup_app(void);
/*
	主循环函数声明
*/
extern void loop_app(void);

#endif
