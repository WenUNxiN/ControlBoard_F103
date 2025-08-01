/********************************
启动文件
********************************/
#include "Application.h"

void setup_app(void)
{
	Rcc_Init();				  // 使能各外设时钟
	SWJGpio_Init();			  // 关闭 JTAG，释放 PA15/PB3/PB4
	SysTickInit();			  // 初始化 1 ms 系统嘀嗒（Delay_ms 依赖）
	//Key_Init();    		  //按键初始化                                                                                                           //Key_Init();    		//按键初始化
	Buzzer_Init();			  // 蜂鸣器初始化
	LED_Init();				  // led初始化
	Uart_Init();			  // 串口初始化
	US_Init();				  // 超声波 HC-SR04 引脚/时基初始化
	SoftI2C_init();
    OLED_Init();			  // SSD1306 OLED 软件 I2C 初始化
	OLED_Clear();			  // 清屏，防止残影
	PwmServoInit();			  // 舵机 GPIO/PWM 引脚初始化
	PidInit();				  // PID初始化
	TIM1_Init(20000, 72 - 1); // 初始化定时器1，用于PID控制舵机
	TIM2_Init(20000, 72 - 1);
    SpiFlash_Init();
	Ps2_Init();
	Parameter_Init(); // 读取W25Q64内存储的数据
	ServoState_Init();
	Uart1_Print("$APP_START!");
	Buzzer_times(200, 3);
	setup_run_action();

	setup_kinematics(150, 120, 110, 125, &kinematics);
	//setup_kinematics(135, 130, 130, 150, &kinematics);
}

void loop_app(void)
{
	Led_Blink(800);
	// loop_key();
	Loop_Uart();
	AppPs2Run();
	Loop_Action();
}

void Parameter_Init(void)
{
	w25x_read((u8 *)(&eeprom_info), W25Q64_INFO_ADDR_SAVE_STR, sizeof(eeprom_info));
	if (eeprom_info.servo_init_pos[DJ_NUM] != FLAG_VERIFY)
	{
		for (u8 i = 0; i < DJ_NUM; i++)
		{
			eeprom_info.servo_init_pos[i] = 1500;
		}
	}
	sprintf((char *)cmd_return, "init_pos:%d,%d,%d,%d,%d,%d,%d,%d\r\n", eeprom_info.servo_init_pos[0], eeprom_info.servo_init_pos[1], eeprom_info.servo_init_pos[2], eeprom_info.servo_init_pos[3], eeprom_info.servo_init_pos[4], eeprom_info.servo_init_pos[5], eeprom_info.servo_init_pos[6], eeprom_info.servo_init_pos[7]);
	Uart1_Print(cmd_return);

	if (eeprom_info.dj_bias_pwm[DJ_NUM] != FLAG_VERIFY)
	{
		for (u8 i = 0; i < DJ_NUM; i++)
		{
			eeprom_info.dj_bias_pwm[i] = 0;
		}
		eeprom_info.dj_bias_pwm[DJ_NUM] = FLAG_VERIFY;
	}
}

// 执行开机动作组
void setup_run_action(void)
{
	if (eeprom_info.pre_cmd[PRE_CMD_SIZE] == FLAG_VERIFY)
	{
		strcpy((char *)Uart_ReceiveBuf, (char *)eeprom_info.pre_cmd);
		if (eeprom_info.pre_cmd[0] == '$')
		{
			Parse_Group_Cmd(eeprom_info.pre_cmd);
		}
	}
}

// 舵机上电初始位置
void ServoState_Init(void)
{
	for (u8 i = 0; i < DJ_NUM; i++)
	{
		duoji_doing[i].aim = eeprom_info.servo_init_pos[i] + eeprom_info.dj_bias_pwm[i];
		duoji_doing[i].cur = eeprom_info.servo_init_pos[i] + eeprom_info.dj_bias_pwm[i];
		duoji_doing[i].inc = 0;
	}
}

/***********************************************
函数名称：Buzzer_times
功能介绍：蜂鸣器响time时长，响count次数
函数参数：time，间隔时长，count，次数
返回值：无
 ***********************************************/
void Buzzer_times(u32 time, u32 count)
{

	for (int i = 0; i < count; i++)
	{
		BUZZER_ON();
		Delay_ms(time);
		BUZZER_OFF();
		Delay_ms(time);
	}
}

void Led_Blink(u32 time)
{
	static u32 systick_ms_bak_led = 0;
	if (Millis() - systick_ms_bak_led < time)
		return;
	systick_ms_bak_led = Millis();
	LED_TOGGLE();
}

void loop_key(void)
{
	static u32 sys_time_bak = 0;
	if (Millis() - sys_time_bak < 50)
		return;
	sys_time_bak = Millis();
	if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == KEY_PRESS)
	{
		Delay_ms(20);
		if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == KEY_PRESS)
		{
			BUZZER_ON();
			LED_ON();
			Uart1_Print("$KEY_PRESS!");
		}
	}
	else
	{
		BUZZER_OFF();
	}
}

/***********************************************

 ***********************************************/
void Uart_Init(void)
{
	Uart1_Init(115200);
	Uart2_Init(115200);
	Uart3_Init(115200);
}

/***********************************************

  ***********************************************/
void Loop_Uart(void)
{
	if (Uart_GetOK)
	{
		if (Uart_Mode == 1)
		{
			//
			Parse_Group_Cmd(Uart_ReceiveBuf);
			Parse_Cmd(Uart_ReceiveBuf);
		}
		else if (Uart_Mode == 2)
		{
			//
			Parse_Action(Uart_ReceiveBuf);
		}
		else if (Uart_Mode == 3)
		{
			Parse_Action(Uart_ReceiveBuf);
		}
		else if (Uart_Mode == 4)
		{
			Save_Action(Uart_ReceiveBuf);
		}
		Uart_Mode = 0;
		Uart_GetOK = 0;
	}
}
