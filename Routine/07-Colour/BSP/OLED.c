#include "OLED.h"
#include "OLEDFont.h"

// OLED���Դ�
// ��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

/***********************************************
	��������:	void OLED_WRByte(u8 dat,u8 cmd)
	���ܽ���:	OLEDд���ݻ���ָ��
	��������:dat������
				cmd��д���ݻ���дָ��
							OLED_CMD
							OLED_DATA
	����ֵ:	��
***********************************************/
void OLED_WRByte(u8 dat, u8 cmd)
{
    if (cmd)
	{
		SoftI2C_write(0x78 >> 1, 0x40, &dat, 1);
	}
	else
	{
		SoftI2C_write(0x78 >> 1, 0x00, &dat, 1);
	}
}
/***********************************************
	��������:void OLED_Fill(u8 fill_Data)
	���ܽ���:�������
	��������:fill_Data������
	����ֵ:	��
***********************************************/
void OLED_Fill(u8 fill_Data)
{
	unsigned char m, n;
	for (m = 0; m < 8; m++)
	{
		OLED_WRByte(0xb0 + m, 0); // page0-page1
		OLED_WRByte(0x00, 0);	  // low column start address
		OLED_WRByte(0x10, 0);	  // high column start address
		for (n = 0; n < 128; n++)
		{
			OLED_WRByte(fill_Data, 1);
		}
	}
}

/***********************************************
	��������:void OLED_SetPos(u8 x, u8 y)
	���ܽ���:��������
	��������:x:x����
				y:y����
	����ֵ:	��
***********************************************/
void OLED_SetPos(u8 x, u8 y)
{
	OLED_WRByte(0xb0 + y, OLED_CMD);
	OLED_WRByte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WRByte((x & 0x0f), OLED_CMD);
}

/***********************************************
	��������:void OLED_Display_On(void)
	���ܽ���:����OLED��ʾ
	��������:��
	����ֵ:	��
***********************************************/
void OLED_Display_On(void)
{
	OLED_WRByte(0X8D, OLED_CMD); // SET DCDC����
	OLED_WRByte(0X14, OLED_CMD); // DCDC ON
	OLED_WRByte(0XAF, OLED_CMD); // DISPLAY ON
}

/***********************************************
	��������:void OLED_Display_Off(void)
	���ܽ���:�ر�OLED��ʾ
	��������:��
	����ֵ:	��
***********************************************/
void OLED_Display_Off(void)
{
	OLED_WRByte(0X8D, OLED_CMD); // SET DCDC����
	OLED_WRByte(0X10, OLED_CMD); // DCDC OFF
	OLED_WRByte(0XAE, OLED_CMD); // DISPLAY OFF
}

/***********************************************
	��������:void OLED_Clear(void)
	���ܽ���:����
	��������:��
	����ֵ:	��
***********************************************/
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WRByte(0xb0 + i, OLED_CMD); // ����ҳ��ַ��0~7��
		OLED_WRByte(0x00, OLED_CMD);	 // ������ʾλ�á��е͵�ַ
		OLED_WRByte(0x10, OLED_CMD);	 // ������ʾλ�á��иߵ�ַ
		for (n = 0; n < 128; n++)
			OLED_WRByte(0, OLED_DATA);
	} // ������ʾ
}

/***********************************************
	��������:void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
	���ܽ���:��ָ��λ����ʾһ���ַ�,���������ַ�
	��������:x:0~127
				y:0~7
				mode:0,������ʾ;1,������ʾ
				Char_Size:ѡ������ 16/12
	����ֵ:	��
***********************************************/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' '; // �õ�ƫ�ƺ��ֵ
	if (x > Max_Column - 1)
	{
		x = 0;
		y = y + 2;
	}
	if (Char_Size == 16)
	{
		OLED_SetPos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WRByte(F8X16[c * 16 + i], OLED_DATA);
		OLED_SetPos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WRByte(F8X16[c * 16 + i + 8], OLED_DATA);
	}
	else
	{
		OLED_SetPos(x, y);
		for (i = 0; i < 6; i++)
			OLED_WRByte(F6x8[c][i], OLED_DATA);
	}
}

/***********************************************
	��������:u32 OLED_Pow(u8 m,u8 n)
	���ܽ���:m^n����
	��������:m,n
	����ֵ:	��
***********************************************/
u32 OLED_Pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

/***********************************************
	��������:void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
	���ܽ���:��ʾ����
	��������:x:0~127
				y:0~63
				num:��ֵ(0~4294967295)
				len :���ֵ�λ��
				size:�����С
	����ֵ:	��
***********************************************/
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
	u8 t, temp;
	u8 enshow = 0;
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
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
	}
}

/***********************************************
	��������:void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
	���ܽ���:��ʾһ���ַ��Ŵ�
	��������:x:0~127
				y:0~63
				*chr:�ַ���
				len :���ֵ�λ��
				Char_Size:ѡ������ 16/12
	����ֵ:	��
***********************************************/
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if (x > 120)
		{
			x = 0;
			y += 2;
		}
		j++;
	}
}

/***********************************************
	��������:void OLED_Print(u8 x, u8 y, char *s)
	���ܽ���:��ʾһ���ַ��Ŵ�
	��������:x:0~127
				y:0-7
				*chr:�ַ���
				*s:Ҫ��ʾ���ַ�����Ӣ�ľ���
	����ֵ:	��
***********************************************/
void OLED_Print(u8 x, u8 y, char *s)
{
	unsigned char i, k, t, length;
	unsigned short Index = 0;
	length = strlen(s); // ȡ�ַ����ܳ�
	for (k = 0; k < length; k++)
	{
		if (*(s + k) <= 127)
		{ // С��128��ASCII����
			OLED_ShowChar(x, y, *(s + k), 16);
			x += 8; // x��������8
		}
		else if (*(s + k) > 127)
		{											  // ����127��Ϊ���֣�ǰ��������ɺ�������
			Index = (*(s + k) << 8) | (*(s + k + 1)); // ȡ���ֵ�����
			for (i = 0; i < sizeof(CN16_Msk) / 34; i++)
			{ // ������
				if (Index == CN16_Msk[i].Index)
				{
					// ��ѯ�������
					OLED_SetPos(x, y);
					for (t = 0; t < 16; t++)
						OLED_WRByte(CN16_Msk[i].Msk[t], OLED_DATA); // д����ģ

					OLED_SetPos(x, y + 1);
					for (t = 16; t < 32; t++)
						OLED_WRByte(CN16_Msk[i].Msk[t], OLED_DATA);
					x += 16;
					k += 1; // ����ռ2B,����һ��
				}
			}
		}
	}
}

/***********************************************
	��������:void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
	���ܽ���:��ʾͼƬ
	��������:x:0~127--0��ʼ--1��ֹ
				y:0-7 --0��ʼ--1��ֹ
				BMP[]:ͼƬ������
	����ֵ:	��
***********************************************/
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[])
{
	u32 j = 0;
	u8 x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_SetPos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WRByte(BMP[j++], OLED_DATA);
		}
	}
}

//

/***********************************************
	��������:void OLED_Init(void)
	���ܽ���:��ʼ��SSD1306
	��������:��
	����ֵ:	��
***********************************************/
void OLED_Init(void)
{
	Delay_ms(100);
	OLED_WRByte(0xAE, OLED_CMD); //--display off
	OLED_WRByte(0x00, OLED_CMD); //---set low column address
	OLED_WRByte(0x10, OLED_CMD); //---set high column address
	OLED_WRByte(0x40, OLED_CMD); //--set start line address
	OLED_WRByte(0xB0, OLED_CMD); //--set page address
	OLED_WRByte(0x81, OLED_CMD); // contract control
	OLED_WRByte(0xFF, OLED_CMD); //--128
	OLED_WRByte(0xA1, OLED_CMD); // set segment remap
	OLED_WRByte(0xA6, OLED_CMD); //--normal / reverse
	OLED_WRByte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
	OLED_WRByte(0x3F, OLED_CMD); //--1/32 duty
	OLED_WRByte(0xC8, OLED_CMD); // Com scan direction
	OLED_WRByte(0xD3, OLED_CMD); //-set display offset
	OLED_WRByte(0x00, OLED_CMD); //

	OLED_WRByte(0xD5, OLED_CMD); // set osc division
	OLED_WRByte(0x80, OLED_CMD); //

	OLED_WRByte(0xD8, OLED_CMD); // set area color mode off
	OLED_WRByte(0x05, OLED_CMD); //

	OLED_WRByte(0xD9, OLED_CMD); // Set Pre-Charge Period
	OLED_WRByte(0xF1, OLED_CMD); //

	OLED_WRByte(0xDA, OLED_CMD); // set com pin configuartion
	OLED_WRByte(0x12, OLED_CMD); //

	OLED_WRByte(0xDB, OLED_CMD); // set Vcomh
	OLED_WRByte(0x30, OLED_CMD); //

	OLED_WRByte(0x8D, OLED_CMD); // set charge pump enable
	OLED_WRByte(0x14, OLED_CMD); //

	OLED_WRByte(0xAF, OLED_CMD); //--turn on oled panel
}
