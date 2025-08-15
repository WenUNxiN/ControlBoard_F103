#ifndef _PROTOCOL__H_           // 防止重复包含
#define _PROTOCOL__H_

/* ---------- 1. 头文件 ---------- */
#include <stdio.h>              // printf 等
#include <string.h>             // strlen、strstr、strcpy 等

#include "sys.h"                
#include "PwmServo.h"           
#include "Global.h"             
#include "Uart.h"               

/* ---------- 3. 对外接口 ---------- */
void Parse_Action(char *Uart_ReceiveBuf);   // 解析舵机控制帧（#ID P PWM T TIME!）
void Parse_Cmd(char *cmd);                  // 解析文本命令（$XXX!）

#endif /* _PROTOCOL__H_ */
