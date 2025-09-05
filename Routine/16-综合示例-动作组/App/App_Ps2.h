#ifndef _APP_PS2_H_
#define _APP_PS2_H_
#include "Ps2.h"

#define PS2_LED_RED 0x41  // 
#define PS2_LED_GRN 0x73  // 
#define PSX_BUTTON_NUM 16 // 
#define PS2_MAX_LEN 64   //

extern char cmd_return[1024];


void AppPs2Init(void);
void AppPs2Run(void);

void ParsePsx_Buf(unsigned char *buf, unsigned char mode);
#endif
