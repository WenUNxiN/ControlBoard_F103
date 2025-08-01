#include "ServoPID.h"
#include "Timer.h"
#include "PwmServo.h"

float Position_KP = 120, Position_KI = 0.1, Position_KD = 500; // PID²ÎÊý

/***********************************************

 ***********************************************/
void PidInit(void)
{
    u8 f;
    for (f = 0; f < DJ_NUM; f++)
    {
        pid_doing[f].aim = 1500; //+eeprom_info.dj_bias_pwm[f];
        pid_doing[f].cur = 1500; //+eeprom_info.dj_bias_pwm[f];
        pid_doing[f].inc = 0;
    }
    return;
}

int Position_PID(int position, int target)
{
    static float Bias, Pwm, Integral_bias, Last_Bias;
    Bias = target - position;
    Integral_bias += Bias;
    Pwm = Position_KP * Bias / 100 + Position_KI * Integral_bias / 100 + Position_KD * (Bias - Last_Bias) / 100;
    Last_Bias = Bias;
    return Pwm;
}

/***********************************************

 ***********************************************/
void Set_PwmServo(int index, int pwm, int time)
{
    duoji_doing[index].aim = pwm;
    duoji_doing[index].time = time;
    duoji_doing[index].inc = (float)((duoji_doing[index].aim - duoji_doing[index].cur) / (duoji_doing[index].time / 20.000));
}
