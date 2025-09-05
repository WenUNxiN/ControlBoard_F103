/**
 * @file   OLED.c
 * @brief  SSD1306 0.96" OLED 驱动实现（软件 I2C）
 * @note   128×64 分辨率；显存按 8 页×128 字节组织
 */

#include "OLED.h"
#include "OLEDFont.h"   // 字模表
#include "Soft_I2C.h"   // 软件 I2C 读写

/*==================== 显存结构 ====================*/
// OLED_GRAM[page][column] 对应 8×128 点阵
// page 0 对应 y=0~7，page 1 对应 y=8~15，…… page 7 对应 y=56~63

/*==================== 底层字节写入 ====================*/
/**
 * @brief  通过软件 I2C 向 OLED 写命令/数据
 * @param  dat 待写内容
 * @param  cmd 0 写命令；1 写数据
 */
void OLED_WRByte(u8 dat, u8 cmd)
{
    if (cmd)
        SoftI2C_write(0x78 >> 1, 0x40, &dat, 1);  // 0x40 表示后续字节为显存数据
    else
        SoftI2C_write(0x78 >> 1, 0x00, &dat, 1);  // 0x00 表示后续字节为命令
}

/*==================== 显存填充 ====================*/
void OLED_Fill(u8 fill_Data)
{
    for (u8 page = 0; page < 8; page++)
    {
        OLED_WRByte(0xB0 + page, OLED_CMD); // 设置页地址
        OLED_WRByte(0x00, OLED_CMD);        // 列地址低 4 位
        OLED_WRByte(0x10, OLED_CMD);        // 列地址高 4 位
        for (u8 col = 0; col < 128; col++)
            OLED_WRByte(fill_Data, OLED_DATA);
    }
}

/*==================== 坐标设置 ====================*/
void OLED_SetPos(u8 x, u8 y)
{
    OLED_WRByte(0xB0 + y, OLED_CMD);           // 页地址
    OLED_WRByte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); // 列地址高 4 位
    OLED_WRByte(x & 0x0F, OLED_CMD);           // 列地址低 4 位
}

/*==================== 显示开关 ====================*/
void OLED_Display_On(void)
{
    OLED_WRByte(0x8D, OLED_CMD); OLED_WRByte(0x14, OLED_CMD); // 开启电荷泵
    OLED_WRByte(0xAF, OLED_CMD); // 开显示
}
void OLED_Display_Off(void)
{
    OLED_WRByte(0x8D, OLED_CMD); OLED_WRByte(0x10, OLED_CMD); // 关闭电荷泵
    OLED_WRByte(0xAE, OLED_CMD); // 关显示
}

/*==================== 清屏 ====================*/
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WRByte(0xB0 + i, OLED_CMD); // 页地址
        OLED_WRByte(0x00, OLED_CMD);     // 列地址低 4 位
        OLED_WRByte(0x10, OLED_CMD);     // 列地址高 4 位
        for (n = 0; n < 128; n++)
            OLED_WRByte(0x00, OLED_DATA);
    }
}

/*==================== 单字符显示 ====================*/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
    chr -= ' ';                        // 字模表从空格开始
    if (x > Max_Column - 1) { x = 0; y += 2; }

    if (Char_Size == 16)
    {
        OLED_SetPos(x, y);
        for (u8 i = 0; i < 8; i++)
            OLED_WRByte(F8X16[chr * 16 + i], OLED_DATA);
        OLED_SetPos(x, y + 1);
        for (u8 i = 0; i < 8; i++)
            OLED_WRByte(F8X16[chr * 16 + 8 + i], OLED_DATA);
    }
    else
    {
        OLED_SetPos(x, y);
        for (u8 i = 0; i < 6; i++)
            OLED_WRByte(F6x8[chr][i], OLED_DATA);
    }
}

/*==================== 幂运算 ====================*/
u32 OLED_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--) result *= m;
    return result;
}

/*==================== 数字显示 ====================*/
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
    u8 t, temp, enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
                continue;
            }
            else enshow = 1;
        }
        OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
    }
}

/*==================== 字符串显示 ====================*/
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
    u8 j = 0;
    while (chr[j] != '\0')
    {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8;
        if (x > 120) { x = 0; y += 2; }
        j++;
    }
}

/*==================== printf 风格输出（支持中文） ====================*/
void OLED_Print(u8 x, u8 y, char *s)
{
    u32 length = strlen(s), Index;
    for (u32 k = 0; k < length; k++)
    {
        if ((u8)s[k] <= 127)               // ASCII
        {
            OLED_ShowChar(x, y, s[k], 16);
            x += 8;
        }
        else if ((u8)s[k] > 127)           // 汉字
        {
            Index = ((u8)s[k] << 8) | (u8)s[k + 1];
            for (u32 i = 0; i < sizeof(CN16_Msk) / 34; i++)
            {
                if (Index == CN16_Msk[i].Index)
                {
                    OLED_SetPos(x, y);
                    for (u8 t = 0; t < 16; t++)
                        OLED_WRByte(CN16_Msk[i].Msk[t], OLED_DATA);
                    OLED_SetPos(x, y + 1);
                    for (u8 t = 16; t < 32; t++)
                        OLED_WRByte(CN16_Msk[i].Msk[t], OLED_DATA);
                    x += 16;
                    k++;                 // 跳过下一字节
                    break;
                }
            }
        }
    }
}

/*==================== 位图显示 ====================*/
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[])
{
    u32 j = 0;
    for (u8 y = y0; y < y1; y++)
    {
        OLED_SetPos(x0, y);
        for (u8 x = x0; x < x1; x++)
            OLED_WRByte(BMP[j++], OLED_DATA);
    }
}

/*==================== 初始化序列 ====================*/
void OLED_Init(void)
{
    Delay_ms(100);           // 等待电源稳定
    OLED_WRByte(0xAE, OLED_CMD); // 关显示
    OLED_WRByte(0x00, OLED_CMD); // 列地址低
    OLED_WRByte(0x10, OLED_CMD); // 列地址高
    OLED_WRByte(0x40, OLED_CMD); // 起始行
    OLED_WRByte(0xB0, OLED_CMD); // 页地址
    OLED_WRByte(0x81, OLED_CMD); OLED_WRByte(0xFF, OLED_CMD); // 对比度
    OLED_WRByte(0xA1, OLED_CMD); // 段重映射
    OLED_WRByte(0xA6, OLED_CMD); // 正常/反色
    OLED_WRByte(0xA8, OLED_CMD); OLED_WRByte(0x3F, OLED_CMD); // 复用率 1/64
    OLED_WRByte(0xC8, OLED_CMD); // COM 扫描方向
    OLED_WRByte(0xD3, OLED_CMD); OLED_WRByte(0x00, OLED_CMD); // 显示偏移
    OLED_WRByte(0xD5, OLED_CMD); OLED_WRByte(0x80, OLED_CMD); // 时钟分频
    OLED_WRByte(0xD8, OLED_CMD); OLED_WRByte(0x05, OLED_CMD); // 区域颜色
    OLED_WRByte(0xD9, OLED_CMD); OLED_WRByte(0xF1, OLED_CMD); // 预充电周期
    OLED_WRByte(0xDA, OLED_CMD); OLED_WRByte(0x12, OLED_CMD); // COM 引脚配置
    OLED_WRByte(0xDB, OLED_CMD); OLED_WRByte(0x30, OLED_CMD); // VCOMH
    OLED_WRByte(0x8D, OLED_CMD); OLED_WRByte(0x14, OLED_CMD); // 电荷泵使能
    OLED_WRByte(0xAF, OLED_CMD); // 开显示
}
