/**
 * @file   OLED.c
 * @brief  SSD1306 0.96" OLED ����ʵ�֣���� I2C��
 * @note   128��64 �ֱ��ʣ��Դ水 8 ҳ��128 �ֽ���֯
 */

#include "OLED.h"
#include "OLEDFont.h"   // ��ģ��
#include "Soft_I2C.h"   // ��� I2C ��д

/*==================== �Դ�ṹ ====================*/
// OLED_GRAM[page][column] ��Ӧ 8��128 ����
// page 0 ��Ӧ y=0~7��page 1 ��Ӧ y=8~15������ page 7 ��Ӧ y=56~63

/*==================== �ײ��ֽ�д�� ====================*/
/**
 * @brief  ͨ����� I2C �� OLED д����/����
 * @param  dat ��д����
 * @param  cmd 0 д���1 д����
 */
void OLED_WRByte(u8 dat, u8 cmd)
{
    if (cmd)
        SoftI2C_write(0x78 >> 1, 0x40, &dat, 1);  // 0x40 ��ʾ�����ֽ�Ϊ�Դ�����
    else
        SoftI2C_write(0x78 >> 1, 0x00, &dat, 1);  // 0x00 ��ʾ�����ֽ�Ϊ����
}

/*==================== �Դ���� ====================*/
void OLED_Fill(u8 fill_Data)
{
    for (u8 page = 0; page < 8; page++)
    {
        OLED_WRByte(0xB0 + page, OLED_CMD); // ����ҳ��ַ
        OLED_WRByte(0x00, OLED_CMD);        // �е�ַ�� 4 λ
        OLED_WRByte(0x10, OLED_CMD);        // �е�ַ�� 4 λ
        for (u8 col = 0; col < 128; col++)
            OLED_WRByte(fill_Data, OLED_DATA);
    }
}

/*==================== �������� ====================*/
void OLED_SetPos(u8 x, u8 y)
{
    OLED_WRByte(0xB0 + y, OLED_CMD);           // ҳ��ַ
    OLED_WRByte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); // �е�ַ�� 4 λ
    OLED_WRByte(x & 0x0F, OLED_CMD);           // �е�ַ�� 4 λ
}

/*==================== ��ʾ���� ====================*/
void OLED_Display_On(void)
{
    OLED_WRByte(0x8D, OLED_CMD); OLED_WRByte(0x14, OLED_CMD); // ������ɱ�
    OLED_WRByte(0xAF, OLED_CMD); // ����ʾ
}
void OLED_Display_Off(void)
{
    OLED_WRByte(0x8D, OLED_CMD); OLED_WRByte(0x10, OLED_CMD); // �رյ�ɱ�
    OLED_WRByte(0xAE, OLED_CMD); // ����ʾ
}

/*==================== ���� ====================*/
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WRByte(0xB0 + i, OLED_CMD); // ҳ��ַ
        OLED_WRByte(0x00, OLED_CMD);     // �е�ַ�� 4 λ
        OLED_WRByte(0x10, OLED_CMD);     // �е�ַ�� 4 λ
        for (n = 0; n < 128; n++)
            OLED_WRByte(0x00, OLED_DATA);
    }
}

/*==================== ���ַ���ʾ ====================*/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
    chr -= ' ';                        // ��ģ��ӿո�ʼ
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

/*==================== ������ ====================*/
u32 OLED_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--) result *= m;
    return result;
}

/*==================== ������ʾ ====================*/
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

/*==================== �ַ�����ʾ ====================*/
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

/*==================== printf ��������֧�����ģ� ====================*/
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
        else if ((u8)s[k] > 127)           // ����
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
                    k++;                 // ������һ�ֽ�
                    break;
                }
            }
        }
    }
}

/*==================== λͼ��ʾ ====================*/
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

/*==================== ��ʼ������ ====================*/
void OLED_Init(void)
{
    Delay_ms(100);           // �ȴ���Դ�ȶ�
    OLED_WRByte(0xAE, OLED_CMD); // ����ʾ
    OLED_WRByte(0x00, OLED_CMD); // �е�ַ��
    OLED_WRByte(0x10, OLED_CMD); // �е�ַ��
    OLED_WRByte(0x40, OLED_CMD); // ��ʼ��
    OLED_WRByte(0xB0, OLED_CMD); // ҳ��ַ
    OLED_WRByte(0x81, OLED_CMD); OLED_WRByte(0xFF, OLED_CMD); // �Աȶ�
    OLED_WRByte(0xA1, OLED_CMD); // ����ӳ��
    OLED_WRByte(0xA6, OLED_CMD); // ����/��ɫ
    OLED_WRByte(0xA8, OLED_CMD); OLED_WRByte(0x3F, OLED_CMD); // ������ 1/64
    OLED_WRByte(0xC8, OLED_CMD); // COM ɨ�跽��
    OLED_WRByte(0xD3, OLED_CMD); OLED_WRByte(0x00, OLED_CMD); // ��ʾƫ��
    OLED_WRByte(0xD5, OLED_CMD); OLED_WRByte(0x80, OLED_CMD); // ʱ�ӷ�Ƶ
    OLED_WRByte(0xD8, OLED_CMD); OLED_WRByte(0x05, OLED_CMD); // ������ɫ
    OLED_WRByte(0xD9, OLED_CMD); OLED_WRByte(0xF1, OLED_CMD); // Ԥ�������
    OLED_WRByte(0xDA, OLED_CMD); OLED_WRByte(0x12, OLED_CMD); // COM ��������
    OLED_WRByte(0xDB, OLED_CMD); OLED_WRByte(0x30, OLED_CMD); // VCOMH
    OLED_WRByte(0x8D, OLED_CMD); OLED_WRByte(0x14, OLED_CMD); // ��ɱ�ʹ��
    OLED_WRByte(0xAF, OLED_CMD); // ����ʾ
}
