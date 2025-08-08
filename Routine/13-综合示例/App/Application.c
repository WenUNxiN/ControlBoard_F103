/********************************
启动文件
********************************/
#include "Application.h"

/*----------------- 系统初始化 -----------------*/
void setup_app(void)
{
	Rcc_Init();				          // 使能各外设时钟
	SWJGpio_Init();			          // 关闭 JTAG，释放 PA15/PB3/PB4
	SysTickInit();			          // 初始化 1 ms 系统嘀嗒（Delay_ms 依赖）
	//Key_Init();    		          // 按键初始化（已注释）
	Buzzer_Init();			          // 蜂鸣器初始化
	LED_Init();				          // LED 初始化
	Uart_Init();			          // 串口 1/2/3 初始化（波特率 115200）
	US_Init();				          // 超声波 HC-SR04 引脚/时基初始化
	SoftI2C_init();                   // 软件 I²C（用于 OLED）
	OLED_Init();			          // SSD1306 OLED 初始化
	OLED_Clear();			          // 清屏，防止残影
	PwmServoInit();			          // 舵机 GPIO/PWM 引脚初始化
	PidInit();				          // PID 参数初始化
	TIM1_Init(20000, 72 - 1);         // 20 ms 周期定时器，用于 PID 控制舵机
	TIM2_Init(20000, 72 - 1);         // 同上，可扩展功能
	SpiFlash_Init();                  // W25Q64 SPI Flash 初始化
	Ps2_Init();                       // PS2 手柄（如有）初始化
	Parameter_Init();                 // 从 Flash 读取保存的参数
	ServoState_Init();                // 设定舵机上电初始位置
	Uart1_Print("$APP_START!");       // 串口打印启动标志
	Buzzer_times(200, 3);             // 蜂鸣器响 3 次，提示启动完成
	setup_run_action();               // 执行开机预设动作组
}

/*----------------- 主循环 -----------------*/
void loop_app(void)
{
	Led_Blink(800);                   // LED 每 800 ms 翻转一次
	// loop_key();                    // 按键扫描（已注释）
	Loop_Uart();                      // 串口数据解析
	AppPs2Run();                      // PS2 手柄任务
	Loop_Action();                    // 动作组执行
}

/*----------------- 参数初始化 -----------------*/
void Parameter_Init(void)
{
	/* 从 Flash 读取 EEPROM 信息结构体 */
	w25x_read((u8 *)(&eeprom_info), W25Q64_INFO_ADDR_SAVE_STR, sizeof(eeprom_info));

	/* 若校验失败，则恢复默认舵机中位 1500 µs */
	if (eeprom_info.servo_init_pos[DJ_NUM] != FLAG_VERIFY)
	{
		for (u8 i = 0; i < DJ_NUM; i++)
			eeprom_info.servo_init_pos[i] = 1500;
	}
	/* 打印当前舵机中位值 */
	sprintf((char *)cmd_return, "init_pos:%d,%d,%d,%d,%d,%d,%d,%d\r\n",
	        eeprom_info.servo_init_pos[0], eeprom_info.servo_init_pos[1],
	        eeprom_info.servo_init_pos[2], eeprom_info.servo_init_pos[3],
	        eeprom_info.servo_init_pos[4], eeprom_info.servo_init_pos[5],
	        eeprom_info.servo_init_pos[6], eeprom_info.servo_init_pos[7]);
	Uart1_Print(cmd_return);

	/* 若校验失败，则恢复默认 PWM 零偏 */
	if (eeprom_info.dj_bias_pwm[DJ_NUM] != FLAG_VERIFY)
	{
		for (u8 i = 0; i < DJ_NUM; i++)
			eeprom_info.dj_bias_pwm[i] = 0;
		eeprom_info.dj_bias_pwm[DJ_NUM] = FLAG_VERIFY; // 写回校验标志
	}
}

/*----------------- 开机动作组 -----------------*/
void setup_run_action(void)
{
	/* 若 Flash 中保存了合法的预设指令，则立即执行 */
	if (eeprom_info.pre_cmd[PRE_CMD_SIZE] == FLAG_VERIFY)
	{
		strcpy((char *)Uart_ReceiveBuf, (char *)eeprom_info.pre_cmd);
		if (eeprom_info.pre_cmd[0] == '$')
			Parse_Group_Cmd(eeprom_info.pre_cmd);   // 解析并执行
	}
}

/*----------------- 舵机上电初始位置 -----------------*/
void ServoState_Init(void)
{
	for (u8 i = 0; i < DJ_NUM; i++)
	{
		/* 目标位置 = 中位 + 零偏 */
		duoji_doing[i].aim  = eeprom_info.servo_init_pos[i] + eeprom_info.dj_bias_pwm[i];
		duoji_doing[i].cur  = eeprom_info.servo_init_pos[i] + eeprom_info.dj_bias_pwm[i];
		duoji_doing[i].inc  = 0;        // 步进值
		duoji_doing[i].time = 5000;     // 5 s 平滑到位
	}
}

/*----------------- 蜂鸣器提示音 -----------------*/
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

/*----------------- LED 闪烁 -----------------*/
void Led_Blink(u32 time)
{
	static u32 systick_ms_bak_led = 0;
	if (Millis() - systick_ms_bak_led < time)
		return;
	systick_ms_bak_led = Millis();
	LED_TOGGLE();                     // LED 状态翻转
}

/*----------------- 按键扫描 -----------------*/
void loop_key(void)
{
	static u32 sys_time_bak = 0;
	if (Millis() - sys_time_bak < 50)
		return;
	sys_time_bak = Millis();
	if (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == KEY_PRESS)
	{
		Delay_ms(20);                 // 去抖
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

/*----------------- 统一串口初始化 -----------------*/
void Uart_Init(void)
{
	Uart1_Init(115200);   // USB-CDC/调试
	Uart2_Init(115200);   // RS-485
	Uart3_Init(115200);   // 用户串口
}

/*----------------- 串口数据解析调度 -----------------*/
void Loop_Uart(void)
{
    // 检查是否有完整数据包需要处理
    if (Uart_GetOK)
    {
        switch (Uart_Mode)
        {
            case 1: Parse_Group_Cmd(Uart_ReceiveBuf);
                    Parse_Cmd(Uart_ReceiveBuf);
                    break;
            case 2: Parse_Action(Uart_ReceiveBuf);
                    break;
            case 3: Parse_Action(Uart_ReceiveBuf);
                    break;
            case 4: Save_Action(Uart_ReceiveBuf);
                    break;
        }
        
        // 复位状态机
        Uart_Mode = 0;
        Uart_GetOK = 0;
    }
}
