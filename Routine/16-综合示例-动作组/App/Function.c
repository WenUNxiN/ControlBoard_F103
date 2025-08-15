#include "Function.h"

/* 颜色 → 指令映射表
   color  : 读到的寄存器值
   cmd    : 直接丢给 Parse_Group_Cmd 的完整字符串 */
typedef struct
{
    uint8_t color;
    char cmd[32];
} ColorMap_t;

/*
    0x01 "红色"                
    0x02 "黄色"                
    0x04 "粉色"                
    0x08 "白色"                
    0x10 "黑色"                
    0x20 "绿色"                
    0x40 "深蓝色"              
    0x80 "蓝色"
*/

static const ColorMap_t color_map[] = {
    {0x01, "$DGT:1-10,1!"},    // 红色
    {0x20, "$DGT:11-20,1!"},   // 黄色
    {0x40, "$DGT:21-30,1!"},   // 蓝色
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
                Buzzer_times(200, 3);   // 蜂鸣器提示
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
        Parse_Group_Cmd("$DGT:31-39,1!");
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
    static int kms_x = 150; 
    static int kms_y = 0;


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
        printf("Up\n"); // 向上
        kms_x += SERVO_STEP;  
        if(kms_x >= 250) {kms_x = 250;}
        break;

    case POS_DOWN: // 向下
        printf("Down\n");
        kms_x -= SERVO_STEP;  
        if(kms_x <= -250) {kms_x = -250;}
        break;

    case POS_LEFT: // 向左
        printf("Left\n");
        kms_y += SERVO_STEP;  
        if(kms_y >= 250) {kms_y = 250;}
        break;

    case POS_RIGHT: // 向右
        printf("Right\n");
        kms_y -= SERVO_STEP;  
        if(kms_y <= -250) {kms_y = -250;}
        break;
    }
    sprintf((char *)cmd_return, "$KMS:%03d,%03d,100,1000!\r\n", (int)kms_x, (int)kms_y);
    Parse_Cmd(cmd_return);
}

