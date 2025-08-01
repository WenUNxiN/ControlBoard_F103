#ifndef _PROTOCOL__H_           // ��ֹ�ظ�����
#define _PROTOCOL__H_

/* ---------- 1. ͷ�ļ� ---------- */
#include <stdio.h>              // printf ��
#include <string.h>             // strlen��strstr��strcpy ��

#include "sys.h"                
#include "PwmServo.h"           
#include "Global.h"             
#include "Uart.h"               

/* ---------- 3. ����ӿ� ---------- */
void Parse_Action(char *Uart_ReceiveBuf);   // �����������֡��#ID P PWM T TIME!��
void Parse_Cmd(char *cmd);                  // �����ı����$XXX!��

#endif /* _PROTOCOL__H_ */
