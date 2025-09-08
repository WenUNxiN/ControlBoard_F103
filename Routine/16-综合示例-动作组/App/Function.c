/*
 * Function.c
 * 功能模块实现
 *   1. 颜色识别分拣任务
 *   2. 声音 / 触摸按键任务
 *   3. 摇杆控制任务
 *   4. 超声波测距 + 抓取任务
 */

#include "Function.h"

u8 Us_ok;          // 超声波任务触发标志
u8 clampMode = 0;  // 夹爪状态：0-无动作 1-夹取 2-松开

/*------------------------------------------
 * 颜色 → 指令映射表
 *------------------------------------------*/
typedef struct
{
    uint8_t color;   // GY33 返回的颜色值
    char    cmd[32]; // 对应动作组指令
} ColorMap_t;

static const ColorMap_t color_map[] = {
    {0x01, "$DGT:1-10,1!"},   // 红色
    {0x20, "$DGT:11-20,1!"},  // 绿色
    {0x40, "$DGT:21-30,1!"},  // 蓝色
    /* 可按需继续扩展 */
};

/*------------------------------------------
 * ColorTask
 * 传感器颜色分拣主循环
 * interval_ms : 两次识别最小间隔
 *------------------------------------------*/
void ColorTask(uint32_t interval_ms)
{
    /* 简易状态机：IDLE-空闲  LOCKED-动作执行中 */
    static enum { IDLE, LOCKED } state = IDLE;
    static uint32_t last_tick = 0;

    /* 节拍控制：不到时间直接返回 */
    if (Millis() - last_tick < interval_ms)
        return;
    last_tick = Millis();

    /* 若动作尚未完成，直接退出 */
    if (state == LOCKED)
    {
        if (!group_do_ok) return;   // 动作仍在跑
        state = IDLE;               // 动作跑完，解锁
    }

    uint8_t color;
    /* 从 GY33 颜色寄存器 0x15 读 1 Byte */
    if (!GY33T_Read(0xA4, 0x15, &color, 1))          // 读取成功返回 0
    {
        /* 遍历映射表，匹配颜色 */
        for (size_t i = 0; i < sizeof(color_map) / sizeof(color_map[0]); ++i)
        {
            if (color == color_map[i].color)
            {
                Buzzer_times(200, 3);               // 蜂鸣器提示
                Parse_Group_Cmd((char *)color_map[i].cmd); // 下发动作组
                state = LOCKED;                     // 锁定，等待动作完成
                break;
            }
        }
    }
}

/*------------------------------------------
 * SoundTouchTask
 * 声音/触摸按键扫描 + 执行动作
 * 每 20 ms 扫描一次
 *------------------------------------------*/
void SoundTouchTask(void)
{
    static enum { IDLE, LOCKED } action_state = IDLE;
    static uint32_t touch_tick_bak = 0;
    static KeyState_t key_state = KEY_IDLE;

    /* 20 ms 周期扫描 */
    if (Millis() - touch_tick_bak < 20) return;
    touch_tick_bak = Millis();

    /* 动作未结束直接返回 */
    if (action_state == LOCKED)
    {
        if (!group_do_ok) return;
        action_state = IDLE;
    }

    /* 读取两个按键状态：触摸 & 声音 */
    uint8_t touch_level = GPIO_ReadInputDataBit(TOUCH_KEY_PORT, TOUCH_KEY_PIN);
    uint8_t sound_level = GPIO_ReadInputDataBit(SOUND_KEY_PORT, SOUND_KEY_PIN);

    /* 简易状态机消抖 */
    switch (key_state)
    {
    case KEY_IDLE:
        if (touch_level == KEY_PRESS || sound_level == KEY_PRESS)
            key_state = KEY_DEBOUNCE;
        break;

    case KEY_DEBOUNCE:
        if (touch_level == KEY_PRESS || sound_level == KEY_PRESS)
            key_state = KEY_PRESSED;
        else
            key_state = KEY_IDLE;
        break;

    case KEY_PRESSED:
        Buzzer_times(200, 3);
        Parse_Group_Cmd("$DGT:31-40,1!");  // 固定动作组
        action_state = LOCKED;             // 锁定
        key_state    = KEY_WAIT_RELEASE;
        break;

    case KEY_WAIT_RELEASE:
        if (touch_level != KEY_PRESS && sound_level != KEY_PRESS)
            key_state = KEY_IDLE;
        break;
    }
}

/*------------------------------------------
 * JoystickTask
 * 摇杆控制任务（50 ms 周期）
 *------------------------------------------*/
void JoystickTask(void)
{
    static uint32_t joystick_tick_bak = 0;
    static int kms_x = 150;   // 机械臂 X 坐标
    static int kms_y = 0;     // 机械臂 Y 坐标
    static uint8_t last_clamp = 0xFF; // 上一次夹爪状态

    /* 50 ms 周期 */
    if (Millis() - joystick_tick_bak < 50) return;
    joystick_tick_bak = Millis();

    /* 读取摇杆方向 */
    JoystickPos_t pos = JoystickStatus();
    switch (pos)
    {
    case POS_CENTER: break;   // 无动作
    case POS_UP:
        kms_x += SERVO_STEP;
        if (kms_x > 250) kms_x = 250;
        break;
    case POS_DOWN:
        kms_x -= SERVO_STEP;
        if (kms_x < -250) kms_x = -250;
        break;
    case POS_LEFT:
        kms_y += SERVO_STEP;
        if (kms_y > 250) kms_y = 250;
        break;
    case POS_RIGHT:
        kms_y -= SERVO_STEP;
        if (kms_y < -250) kms_y = -250;
        break;
    }

    /* 实时下发坐标 */
    sprintf((char *)cmd_return, "$KMS:%03d,%03d,100,1000!\r\n", (int)kms_x, (int)kms_y);
    Parse_Cmd(cmd_return);

    /* 夹爪状态变化才执行夹取/松开流程 */
    if (clampMode == last_clamp) return;
    last_clamp = clampMode;

    if (clampMode == 1)        // 夹取动作
    {
        /* 下降 */
        sprintf((char *)cmd_return, "$KMS:%03d,%03d,5,1000!\r\n", (int)kms_x, (int)kms_y);
        Parse_Cmd(cmd_return);
        Delay_ms(1000);

        /* 夹爪闭合 */
        PwmServo_DoingSet(5, 1750, 1000);
        Delay_ms(1000);

        /* 上升 */
        sprintf((char *)cmd_return, "$KMS:%03d,%03d,100,1000!\r\n", (int)kms_x, (int)kms_y);
        Parse_Cmd(cmd_return);
    }
    else if (clampMode == 2)   // 松开动作
    {
        /* 下降 */
        sprintf((char *)cmd_return, "$KMS:%03d,%03d,5,1000!\r\n", (int)kms_x, (int)kms_y);
        Parse_Cmd(cmd_return);
        Delay_ms(1000);

        /* 夹爪张开 */
        PwmServo_DoingSet(5, 1300, 1000);
        Delay_ms(1000);

        /* 上升 */
        sprintf((char *)cmd_return, "$KMS:%03d,%03d,100,1000!\r\n", (int)kms_x, (int)kms_y);
        Parse_Cmd(cmd_return);
    }
}

/*------------------------------------------
 * UsTask
 * 超声波测距 + 自动抓取
 * interval_ms : 两次测距最小间隔
 *------------------------------------------*/
void UsTask(uint32_t interval_ms)
{
    static uint32_t last_tick = 0;

    /* 节拍控制 */
    if (Millis() - last_tick < interval_ms) return;
    last_tick = Millis();

    /* 由外部置位 Us_ok 才启动一次流程 */
    if (!Us_ok) return;

    int distance = US_DisRead() * 10 + 120;   // mm → 脉冲
    if (distance > 250) return;               // 超范围放弃

    /* 调试打印 */
    SetPrintfUart(1);
    printf("\nUs:%d\r\n", distance);

    /* 张开夹爪（保险） */
    PwmServo_DoingSet(5, 1300, 1000);

    /* 移动至物体上方 */
    sprintf((char *)cmd_return, "$KMS:%03d,0,100,1000!\r\n", distance);
    printf("\nUsNum:%s\r\n", cmd_return);
    Parse_Cmd(cmd_return);
    Delay_ms(1000);

    /* 根据距离决定下降高度 */
    int downZ = (distance <= 200) ? 5 : 10;
    sprintf((char *)cmd_return, "$KMS:%03d,0,%03d,1000!\r\n", distance, downZ);
    Parse_Cmd(cmd_return);
    Delay_ms(1000);

    /* 夹取 */
    PwmServo_DoingSet(5, 1750, 1000);
    Delay_ms(500);

    /* 搬运到指定位置 */
    Parse_Group_Cmd("$DGT:41-47,1!");

    /* 本次流程结束 */
    Us_ok = 0;
}
