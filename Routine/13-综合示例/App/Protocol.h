#ifndef _PROTOCOL__H_
#define _PROTOCOL__H_

#include "sys.h"
#include "Global.h"

extern char cmd_return[1024];
extern eeprom_info_t eeprom_info;
#define FLAG_VERIFY 0x25  

void Parse_Action(char *Uart_ReceiveBuf);
void Save_Action(char *str);
void Parse_Group_Cmd(char *cmd);
void Parse_Cmd(char *cmd);
#endif
