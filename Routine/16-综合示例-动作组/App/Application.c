/********************************
 * 启动文件
 * 负责上电初始化、外设配置、
 * 默认动作组执行及主循环调度
 ********************************/
#include "Application.h"

u8  SetMode;   // 当前运行模式（0~14）
u8  UsMode;    // 超声波模式子状态
extern u8 clampMode; // 夹爪状态（1/2）

/*----------------------------
 * 上电一次性初始化
 *----------------------------*/
void setup_app(void)
{
    Rcc_Init();                 // 使能各外设时钟
    SWJGpio_Init();             // 关闭 JTAG，释放 PA15/PB3/PB4
    SysTickInit();              // 初始化 1 ms 系统嘀嗒（Delay_ms 依赖）
    Key_Init();                 // 普通按键初始化
    Sound_Touch_Key_Init();     // 声音/触摸按键初始化
    Buzzer_Init();              // 蜂鸣器
    LED_Init();                 // 状态指示灯
    ADC_Joystick_Init();        // 摇杆 ADC
    Uart_Init();                // 串口1/2/3
    US_Init();                  // HC-SR04 超声波
    SoftI2C_init();             // 软件 I2C
    OLED_Init();                // SSD1306 OLED
    OLED_Clear();               // 清屏
    PwmServoInit();             // 舵机 PWM/IO
    PidInit();                  // PID 参数
    TIM1_Init(20000, 72 - 1);   // 20 ms 周期，用于 PID 控制
    TIM2_Init(20000, 72 - 1);   // 备用定时器
    SpiFlash_Init();            // W25Q64
    Ps2_Init();                 // PS2 手柄
    GY33T_Write_Byte(0xA4, 0x02, 0); // GY33 颜色传感器刷新周期 100 ms
    Parameter_Init();           // 读取 W25Q64 保存的参数
    ServoState_Init();          // 舵机初始位置
    uart1_send_str("$APP_START!");
    Buzzer_times(200, 3);       // 启动提示音
    setup_run_action();         // 执行开机动作组

    SetPrintfUart(2);           // printf 重定向到串口2
    printf("#RunStop!");        // 通知 OpenMV 进入待机
}

/*----------------------------
 * 主循环
 *----------------------------*/
void loop_app(void)
{
    Led_Blink(800);     // 每 800 ms 翻转一次 LED
    loop_key();         // 普通按键扫描
    loop_Joystick_key();// 摇杆按键扫描
    Loop_Uart();        // 串口数据解析
    Loop_Action();      // 动作组运行
    LoopMode();         // 基础模式（颜色/摇杆/超声/声音）
    LoopVisionMode();   // 视觉模式（颜色/标签/人脸/数字识别）
}

/*----------------------------
 * 从 Flash 中加载参数
 *----------------------------*/
void Parameter_Init(void)
{
    w25x_read((u8 *)&eeprom_info, W25Q64_INFO_ADDR_SAVE_STR, sizeof(eeprom_info));

    /* 舵机中位值未校验 -> 写默认值 1500 */
    if (eeprom_info.servo_init_pos[DJ_NUM] != FLAG_VERIFY)
    {
        for (u8 i = 0; i < DJ_NUM; i++)
            eeprom_info.servo_init_pos[i] = 1500;
    }
    sprintf((char *)cmd_return,
            "init_pos:%d,%d,%d,%d,%d,%d,%d,%d\r\n",
            eeprom_info.servo_init_pos[0], eeprom_info.servo_init_pos[1],
            eeprom_info.servo_init_pos[2], eeprom_info.servo_init_pos[3],
            eeprom_info.servo_init_pos[4], eeprom_info.servo_init_pos[5],
            eeprom_info.servo_init_pos[6], eeprom_info.servo_init_pos[7]);
    uart1_send_str(cmd_return);

    /* 舵机偏差值未校验 -> 写 0 */
    if (eeprom_info.dj_bias_pwm[DJ_NUM] != FLAG_VERIFY)
    {
        for (u8 i = 0; i < DJ_NUM; i++)
            eeprom_info.dj_bias_pwm[i] = 0;
        eeprom_info.dj_bias_pwm[DJ_NUM] = FLAG_VERIFY;
    }
}

/*----------------------------
 * 上电后自动执行的动作组
 *----------------------------*/
void setup_run_action(void)
{
    if (eeprom_info.pre_cmd[PRE_CMD_SIZE] == FLAG_VERIFY)
    {
        strcpy((char *)Uart_ReceiveBuf, (char *)eeprom_info.pre_cmd);
        if (eeprom_info.pre_cmd[0] == '$')
            Parse_Group_Cmd(eeprom_info.pre_cmd);
    }
}

/*----------------------------
 * 舵机上电初始位置
 *----------------------------*/
void ServoState_Init(void)
{
    for (u8 i = 0; i < DJ_NUM; i++)
    {
        duoji_doing[i].aim  = eeprom_info.servo_init_pos[i] + eeprom_info.dj_bias_pwm[i];
        duoji_doing[i].cur  = duoji_doing[i].aim;
        duoji_doing[i].inc  = 0;
        duoji_doing[i].time = 5000; // 5 s 内完成到位
    }
}

/*----------------------------
 * LED 闪烁
 *----------------------------*/
void Led_Blink(u32 time)
{
    static u32 systick_ms_bak_led = 0;
    if (Millis() - systick_ms_bak_led < time) return;
    systick_ms_bak_led = Millis();
    LED_TOGGLE();
}

/*----------------------------
 * 普通按键扫描（含消抖）
 * 每按一次 SetMode +1 (0~14)
 *----------------------------*/
void loop_key(void)
{
    static u32 tick_bak = 0;
    static KeyState_t state = KEY_IDLE;

    if (Millis() - tick_bak < 20) return;
    tick_bak = Millis();

    uint8_t pin_level = GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN);

    switch (state)
    {
    case KEY_IDLE:
        if (pin_level == KEY_PRESS) state = KEY_DEBOUNCE;
        break;
    case KEY_DEBOUNCE:
        if (pin_level == KEY_PRESS) state = KEY_PRESSED;
        else                        state = KEY_IDLE;
        break;
    case KEY_PRESSED:
        uart1_send_str("$KEY_PRESS!");
        SetMode = (SetMode + 1) % 16;
        printf("%d\n", SetMode);
        state = KEY_WAIT_RELEASE;
        break;
    case KEY_WAIT_RELEASE:
        if (pin_level != KEY_PRESS)
        {
            Buzzer_times(50, 1);
            state = KEY_IDLE;
        }
        break;
    }
}

/*----------------------------
 * 摇杆按键扫描
 * 仅在 SetMode==2（摇杆）或 3（超声波）时生效
 *----------------------------*/
void loop_Joystick_key(void)
{
    static u32  Joystick_tick_bak = 0;
    static KeyState_t state1 = KEY_IDLE;

    if (SetMode != 2 && SetMode != 3) return;

    if (Millis() - Joystick_tick_bak < 20) return;
    Joystick_tick_bak = Millis();

    JOYSTICK_LED_ON(); // 摇杆指示灯

    uint8_t pin_level = GPIO_ReadInputDataBit(JOYSTICK_KEY_PORT, JOYSTICK_KEY_PIN);

    clampMode = 0;                 // 每次循环强制清零（调试）
    SetPrintfUart(1);
    printf("loop entry clamp=%d\r\n", clampMode);

    switch (state1)
    {
    case KEY_IDLE:
        if (pin_level == 0) state1 = KEY_DEBOUNCE;
        break;
    case KEY_DEBOUNCE:
        if (pin_level == 0) state1 = KEY_PRESSED;
        else                state1 = KEY_IDLE;
        break;
    case KEY_PRESSED:
        BUZZER_ON();
        uart1_send_str("$ROCKER_PRESS!");

        if (SetMode == 2)       // 摇杆模式：夹爪 1↔2 切换
        {
            static uint8_t toggle = 0;
            clampMode = toggle ? 2 : 1;
            toggle ^= 1;
        }
        if (SetMode == 3)       // 超声波模式：启动一次测距
        {
            UsMode = 1;
            Us_ok  = 1;
        }
        state1 = KEY_WAIT_RELEASE;
        break;
    case KEY_WAIT_RELEASE:
        if (pin_level != 0)
        {
            BUZZER_OFF();
            state1 = KEY_IDLE;
        }
        break;
    }
}

/*----------------------------
 * 基础工作模式任务调度
 *----------------------------*/
void LoopMode(void)
{
    static u32 systick_ms_bak_mode = 0;
    if (Millis() - systick_ms_bak_mode < 10) return;
    systick_ms_bak_mode = Millis();

    switch (SetMode)
    {
    case 1: ColorTask(1000);     uart2_send_str("0x01"); break; // 颜色
    case 2: JoystickTask();      uart2_send_str("0x02"); break; // 摇杆
    case 3: if (UsMode == 1) UsTask(1000);uart2_send_str("0x03"); break; // 超声波
    case 4: SoundTouchTask();    uart2_send_str("0x04"); break; // 声音/触摸
    case 15: AppPs2Run();    uart2_send_str("0x15"); break;     // PS2 手柄处理
    default: break;
    }
}

/*----------------------------
 * 视觉模式一次性指令下发
 * 模式变化时才向 OpenMV 发送新命令
 *----------------------------*/
void LoopVisionMode(void)
{
    static u32 systick_ms_bak_mode = 0;
    static u8  last_mode = 0xFF;

    if (Millis() - systick_ms_bak_mode < 10) return;
    systick_ms_bak_mode = Millis();

    if (SetMode == last_mode) return;
    last_mode = SetMode;

    switch (SetMode)
    {
    case 5:  uart2_send_str("0x05"); uart2_send_str("#ColorSort!");        break;
    case 6:  uart2_send_str("0x06"); uart2_send_str("#ColorStack!");       break;
    case 7:  uart2_send_str("0x07"); uart2_send_str("#PTZColorTrace!");    break;
    case 8:  uart2_send_str("0x08"); uart2_send_str("#FaceTrack!");        break;
    case 9:  uart2_send_str("0x09"); uart2_send_str("#ApriltagSort!");     break;
    case 10: uart2_send_str("0x10"); uart2_send_str("#ApriltagStack!");    break;
    case 11: uart2_send_str("0x11"); uart2_send_str("#ApriltagTrack!");    break;
    case 12: uart2_send_str("0x12"); uart2_send_str("#ApriltagNumSort!");  break;
    case 13: uart2_send_str("0x13"); uart2_send_str("#NumTrack!");         break;
    case 14: uart2_send_str("0x14"); uart2_send_str("#GarbageSorting!");   break;
    default: break;
    }
}

/*----------------------------
 * 串口初始化
 *----------------------------*/
void Uart_Init(void)
{
    Uart1_Init(115200);
    Uart2_Init(115200);
    Uart3_Init(115200);
}

/*----------------------------
 * 串口数据解析调度
 *----------------------------*/
void Loop_Uart(void)
{
    if (Uart_GetOK)
    {
        switch (Uart_Mode)
        {
        case 1: Parse_Group_Cmd(Uart_ReceiveBuf);
                Parse_Cmd(Uart_ReceiveBuf);
                break;
        case 2:
        case 3: Parse_Action(Uart_ReceiveBuf);
                break;
        case 4: Save_Action(Uart_ReceiveBuf);
                break;
        }
        Uart_Mode = 0;
        Uart_GetOK = 0;
    }
}
