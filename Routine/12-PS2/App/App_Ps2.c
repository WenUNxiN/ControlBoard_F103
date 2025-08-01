#include "App_Ps2.h"
#include "Uart.h"
#include "Global.h"

#include <string.h>

/*==============================================
 * 1. 红灯模式：16 个按键对应的 “按下即发送” 指令
 *    
 *=============================================*/
/* 1. 红灯指令表 —— 按下即发送的字符串 */
const char *pre_cmd_set_red[PSX_BUTTON_NUM] = {
    "RED_L2\r\n",      // L2
    "RED_R2\r\n",      // R2
    "RED_L1\r\n",      // L1
    "RED_R1\r\n",      // R1
    "RED_RU\r\n",      // RU
    "RED_RR\r\n",      // RR
    "RED_RD\r\n",      // RD
    "RED_RL\r\n",      // RL
    "RED_SE\r\n",      // SE
    "RED_AL\r\n",      // AL
    "RED_AR\r\n",      // AR
    "RED_ST\r\n",      // ST
    "RED_LU\r\n",      // LU
    "RED_LR\r\n",      // LR
    "RED_LD\r\n",      // LD
    "RED_LL\r\n"       // LL
};

/* 2. 绿灯指令表 —— 同上，可换成小车指令 */
const char *pre_cmd_set_grn[PSX_BUTTON_NUM] = {
    "GRN_L2\r\n",      // L2
    "GRN_R2\r\n",      // R2
    "GRN_L1\r\n",      // L1
    "GRN_R1\r\n",      // R1
    "GRN_RU\r\n",      // RU
    "GRN_RR\r\n",      // RR
    "GRN_RD\r\n",      // RD
    "GRN_RL\r\n",      // RL
    "GRN_SE\r\n",      // SE
    "GRN_AL\r\n",      // AL
    "GRN_AR\r\n",      // AR
    "GRN_ST\r\n",      // ST
    "GRN_LU\r\n",      // LU
    "GRN_LR\r\n",      // LR
    "GRN_LD\r\n",      // LD
    "GRN_LL\r\n"       // LL
};

/*==============================================
 * 3. 初始化：仅调底层 PS2 引脚/时序初始化
 *=============================================*/
void AppPs2Init(void)
{
    Ps2_Init();          // 配置 GPIO + 默认电平
}

/*==============================================
 * 4. 主循环任务：每 50 ms 读一次手柄
 *=============================================*/
void AppPs2Run(void)
{
    static unsigned char psx_button_bak[2] = {0};   // 上一次按键值
    static u32 systick_ms_bak = 0;                  // 时间基准

    /* 50 ms 周期控制 */
    if (Millis() - systick_ms_bak < 50)
        return;
    systick_ms_bak = Millis();

    /* 通过 SPI读取 9 字节手柄数据到 psx_buf */
    Ps2_WriteRead();

    /* 若按键字节无变化则直接返回（简单消抖） */
    if ((psx_button_bak[0] == psx_buf[3]) && (psx_button_bak[1] == psx_buf[4]))
        return;

    /* 有变化 → 解析并下发对应指令 */
    ParsePsx_Buf(psx_buf + 3, psx_buf[1]);   // buf+3 指向 2 字节按键数据；buf[1] 为手柄模式
    psx_button_bak[0] = psx_buf[3];
    psx_button_bak[1] = psx_buf[4];
    
}

/*==============================================
 * 5. 按键解析：检测“按下/松开”边缘并发送串口指令
 *    buf[0] buf[1] 为 16 位按键状态
 *    mode  为红灯/绿灯模式
 *=============================================*/
void ParsePsx_Buf(unsigned char *buf, unsigned char mode)
{
    u8 i;
    /* bak 记录上一次 16 位按键状态，初值 0xFFFF 表示全未按下 */
    static u16 bak = 0xffff, temp, temp2;

    /* 合成 16 位当前状态 */
    temp = (buf[0] << 8) + buf[1];

    /* 只有当按键状态变化时才处理 */
    if (bak != temp)
    {
        temp2 = temp;          // 备份最新状态
        temp &= bak;           // 得到“按下”边缘：1→0 的位

        /* 逐位扫描 0~15 号按键 */
        for (i = 0; i < 16; i++)
        {
            /* 该位为 0 表示当前处于“按下”或“保持按下” */
            if (!((1 << i) & temp))
            {
                /* 如果上一次该位为 1 → 本次为“按下”动作 */
                if ((1 << i) & bak)        // 边缘：上一次为1，本次为0 → 按下
                {
                    const char *cmd = NULL;

                    if (mode == PS2_LED_RED)
                        cmd = pre_cmd_set_red[i];
                    else if (mode == PS2_LED_GRN)
                        cmd = pre_cmd_set_grn[i];
                    else
                        continue;

                    /* 直接发送字符串，末尾已带 \r\n */
                    printf("DOWN %s", cmd);

                    bak = 0xffff;          // 允许再次触发
                }
                else
                {
                    /* ---------- 按键松开 ---------- */
                    const char *cmd = NULL;

                    /* 同样复制整条指令 */
                    if (mode == PS2_LED_RED)
                        cmd = pre_cmd_set_red[i];
                    else if (mode == PS2_LED_GRN)
                        cmd = pre_cmd_set_grn[i];
                    else
                        continue;
                    
                   /* 直接发送字符串，末尾已带 \r\n */
                    printf("UP %s", cmd);
                }
            }
        }
        /* 更新上一次状态 */
        bak = temp2;
    }
    return;
}
