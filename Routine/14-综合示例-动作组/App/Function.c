#include "Function.h"

/* 颜色 → 指令映射表
   color  : 读到的寄存器值
   cmd    : 直接丢给 Parse_Group_Cmd 的完整字符串 */
typedef struct
{
    uint8_t color;
    char cmd[32];
} ColorMap_t;

static const ColorMap_t color_map[] = {
    {0x01, "$DGT:1-8,1!"},    // 红色
    {0x02, "$DGT:9-16,1!"},   // 黄色
    {0x20, "$DGT:17-24,1!"},  // 绿色
};

/* 颜色识别主循环
   interval_ms : 两次识别的最小间隔
   返回后再次调用即可，无需额外任务创建 */
void ColorTask(uint32_t interval_ms)
{
    static enum { IDLE, LOCKED } state = IDLE;
    static uint32_t last_tick = 0;

    /* 节拍控制 */
    if (Millis() - last_tick < interval_ms)
        return;
    last_tick = Millis();

    /* 动作运行中：等待完成标志 */
    if (state == LOCKED)
    {
        if (!group_do_ok)
            return; // 还在跑
        state = IDLE;
    }

    uint8_t num;
    // 从颜色寄存器读取数据
    if (!GY33T_Read(0xa4, 0x15, &num, 1))
    {
        for (size_t i = 0; i < sizeof(color_map) / sizeof(color_map[0]); ++i)
        {
            if (num == color_map[i].color)
            {
                Parse_Group_Cmd((char *)color_map[i].cmd);
                state = LOCKED; // 立即锁定，直到动作完成
                break;
            }
        }
    }
}

/* 声音触摸任务
   每 20 ms 扫描一次，检测触摸和声音按键 */
void SoundTouchTask(void)
{
    static enum { IDLE, LOCKED } action_state = IDLE;
    static u32 touch_tick_bak = 0;
    static KeyState_t state = KEY_IDLE;

    /* 每 20 ms 扫描一次 */
    if (Millis() - touch_tick_bak < 20) return;
    touch_tick_bak = Millis();

    /* 动作运行中：等待完成标志 */
    if (action_state == LOCKED)
    {
        if (!group_do_ok)
            return; // 还在跑
        action_state = IDLE;
    }

    uint8_t pin_level = GPIO_ReadInputDataBit(TOUCH_KEY_PORT, TOUCH_KEY_PIN);
    uint8_t pin_level1 = GPIO_ReadInputDataBit(SOUND_KEY_PORT, SOUND_KEY_PIN);

    switch (state)
    {
    case KEY_IDLE:
        if (pin_level == KEY_PRESS || pin_level1 == KEY_PRESS) 
        {
            state = KEY_DEBOUNCE;
        }
        break;

    case KEY_DEBOUNCE:
        if (pin_level == KEY_PRESS || pin_level1 == KEY_PRESS) 
        {
            state = KEY_PRESSED;
        } 
        else 
        {
            state = KEY_IDLE; // 抖动，放弃
        }
        break;

    case KEY_PRESSED:
        Buzzer_times(200, 3);
        Parse_Group_Cmd("$DGT:25-31,1!");
        action_state = LOCKED; // 立即锁定，直到动作完成
        state = KEY_WAIT_RELEASE;
        break;

    case KEY_WAIT_RELEASE:
        if (pin_level != KEY_PRESS && pin_level1 != KEY_PRESS) 
        {
            state = KEY_IDLE; // 回到初始态，准备下一次
        }
        break;
    }
}


/* 摇杆任务
   每 50 ms 扫描一次，根据摇杆方向调整舵机角度 */
void JoystickTask(void)
{
    static u32 joystick_tick_bak = 0;
    static u16 xpwm = 1500; // 初始 PWM 值
    static u16 ypwm = 1500;
    static u16 ypwm1 = 1500;

    /* 每 50 ms 扫描一次 */
    if (Millis() - joystick_tick_bak < 50) return; 
    joystick_tick_bak = Millis();

    JoystickPos_t pos = JoystickStatus();
    switch (pos)
    {
    case POS_CENTER:
        printf("Center\n");
        break;

    case POS_UP:
        printf("Up\n");
        if (ypwm < 2100) 
        {
            ypwm += SERVO_STEP;
        }
        PwmServo_DoingSet(1, ypwm, 1000);
        if (ypwm1 > 500) 
        {
            ypwm1 -= SERVO_STEP + 10;
        }
        PwmServo_DoingSet(3, ypwm1, 1000);
        break;

    case POS_DOWN:
        printf("Down\n");
        if (ypwm > 900) 
        {
            ypwm -= SERVO_STEP;
        }
        PwmServo_DoingSet(1, ypwm, 1000);
        if (ypwm1 < 1700) 
        {
            ypwm1 += SERVO_STEP + 10;
        }
        PwmServo_DoingSet(3, ypwm1, 1000);
        break;

    case POS_LEFT:
        printf("Left\n");
        if (xpwm < 2500) 
        {
            xpwm += SERVO_STEP;
        }
        PwmServo_DoingSet(0, xpwm, 1000);
        break;

    case POS_RIGHT:
        printf("Right\n");
        if (xpwm > 500) 
        {
            xpwm -= SERVO_STEP;
        }
        PwmServo_DoingSet(0, xpwm, 1000);
        break;
    }
}

