#ifndef _SERVOPID__H_
#define _SERVOPID__H_

void PidInit(void);
int Position_PID (int position,int target);
void Set_PwmServo(int index, int pwm, int time);
#endif
