#include "Kinematics.h"

#define pi 3.1415926

kinematics_t kinematics;

/*
	设置四个关节的长度
	单位1mm
*/

void setup_kinematics(float L0, float L1, float L2, float L3, kinematics_t *kinematics)
{
	// 放大10倍
	kinematics->L0 = L0 * 10;
	kinematics->L1 = L1 * 10;
	kinematics->L2 = L2 * 10;
	kinematics->L3 = L3 * 10;
}

/*
	x,y 为映射到平面的坐标
	z为距离地面的距离
	Alpha 为爪子和平面的夹角 -25~-65范围比较好
*/

int Kinematics_Analysis(float x, float y, float z, float Alpha, kinematics_t *kinematics)
{
	float theta3, theta4, theta5, theta6;
	float l0, l1, l2, l3;
	float aaa, bbb, ccc, zf_flag;

	// 放大10倍
	x = x * 10;
	y = y * 10;
	z = z * 10;

	l0 = kinematics->L0;
	l1 = kinematics->L1;
	l2 = kinematics->L2;
	l3 = kinematics->L3;

	if (x == 0)
	{
		theta6 = 0.0;
	}
	else
	{
		theta6 = atan(x / y) * 270.0 / pi;
	}

	y = sqrt(x * x + y * y);
	y = y - l3 * cos(Alpha * pi / 180.0);
	z = z - l0 - l3 * sin(Alpha * pi / 180.0);
	if (z < -l0)
	{
		return 1;
	}
	if (sqrt(y * y + z * z) > (l1 + l2))
	{
		return 2;
	}

	ccc = acos(y / sqrt(y * y + z * z));
	bbb = (y * y + z * z + l1 * l1 - l2 * l2) / (2 * l1 * sqrt(y * y + z * z));
	if (bbb > 1 || bbb < -1)
	{
		return 5;
	}
	if (z < 0)
	{
		zf_flag = -1;
	}
	else
	{
		zf_flag = 1;
	}
	theta5 = ccc * zf_flag + acos(bbb);
	theta5 = theta5 * 180.0 / pi;
	if (theta5 > 180.0 || theta5 < 0.0)
	{
		return 6;
	}

	aaa = -(y * y + z * z - l1 * l1 - l2 * l2) / (2 * l1 * l2);
	if (aaa > 1 || aaa < -1)
	{
		return 3;
	}
	theta4 = acos(aaa);
	theta4 = 180.0 - theta4 * 180.0 / pi;
	if (theta4 > 135.0 || theta4 < -135.0)
	{
		return 4;
	}

	theta3 = Alpha - theta5 + theta4;
	if (theta3 > 90.0 || theta3 < -90.0)
	{
		return 7;
	}

	kinematics->servo_angle[0] = theta6;
	kinematics->servo_angle[1] = theta5 - 90;
	kinematics->servo_angle[2] = theta4;
	kinematics->servo_angle[3] = theta3;

	kinematics->servo_pwm[0] = (int)(1500 - 2000.0 * kinematics->servo_angle[0] / 270.0);
	kinematics->servo_pwm[1] = (int)(1500 - 2000.0 * kinematics->servo_angle[1] / 270.0);
	kinematics->servo_pwm[2] = (int)(1500 + 2000.0 * kinematics->servo_angle[2] / 270.0);
	kinematics->servo_pwm[3] = (int)(1500 - 2000.0 * kinematics->servo_angle[3] / 270.0);

	return 0;
}

/***********************************************
	函数名称:int kinematics_Move(float x, float y, float z, int time)
	功能介绍:逆运动解算
	函数参数:x,y：平面坐标
				z:高度
			time：执行时间
	返回值:成功状态
 ***********************************************/
int kinematics_Move(float x, float y, float z, int time)
{
	int i, min = 0, flag = 0;

	if (y < 0)
		return 0;

	// 寻找最佳角度
	flag = 0;
	for (i = 0; i >= -135; i--)
	{
		if (0 == Kinematics_Analysis(x, y, z, i, &kinematics))
		{
			if (i < min)
				min = i;
			flag = 1;
		}
	}

	// 用3号舵机与水平最大的夹角作为最佳值
	if (flag)
	{
		Kinematics_Analysis(x, y, z, min, &kinematics);
		kinematics.servo_pwm[3] = 3000 - kinematics.servo_pwm[3];
		sprintf((char *)cmd_return, "{#000P%04dT%04d!#001P%04dT%04d!#002P%04dT%04d!#003P%04dT%04d!}",
				kinematics.servo_pwm[0], time,
				kinematics.servo_pwm[1], time,
				kinematics.servo_pwm[2] - 680, time,
				kinematics.servo_pwm[3] + 580, time);
		//                                       kinematics.servo_pwm[2] - 700,time,
		//                                       kinematics.servo_pwm[3] + 600,time);

		SetPrintfUart(1);
		printf("{#000P%04dT%04d!#001P%04dT%04d!#002P%04dT%04d!#003P%04dT%04d!}\n",
			   kinematics.servo_pwm[0], time,
			   kinematics.servo_pwm[1], time,
			   kinematics.servo_pwm[2] - 700, time,
			   kinematics.servo_pwm[3] + 600, time);
		Parse_Action(cmd_return);
		return 1;
	}

	return 0;
}
