#ifndef _OLED__H_ // ��ֹ�ظ�����
#define _OLED__H_

#include "main.h"

/*-------------------- ��ʾ��س��� --------------------*/
#define OLED_MODE 0     // 0��4 �� SPI ģʽ��������ʵ�ʱ��ļ��� I?C��
#define SIZE 8          // ����߶� 8 ���أ�8��16 ����
#define XLevelL 0x00    // �е�ַ�� 4 λ���루SSD1306 �ڲ��ã�
#define XLevelH 0x10    // �е�ַ�� 4 λ����
#define Max_Column 128  // ��Ļˮƽ���� 128
#define Max_Row 64      // ��Ļ��ֱ���� 64
#define Brightness 0xFF // �ԱȶȼĴ���ֵ��Խ��Խ����

#define X_WIDTH 128 // ��Ļ�����Ӧ�ò������꣩
#define Y_WIDTH 64  // ��Ļ��

/*-------------------- I?C д����/���ݱ�־ --------------------*/
#define OLED_CMD 0  // д����
#define OLED_DATA 1 // д����

/* OLED �߲� API����Ӳ���ӿ��޹أ�ֱ�ӵ��ü��ɣ� */
void OLED_WRByte(u8 dat, u8 cmd);                         // ���ݾɽӿڣ�cmd=0 ���1 ����
void OLED_Fill(u8 fill_Data);                             // ȫ�����
void OLED_SetPos(u8 x, u8 y);                             // ���ù�꣨�� x��ҳ y��
void OLED_Display_On(void);                               // ����ʾ
void OLED_Display_Off(void);                              // �ر���ʾ�����ߣ�
void OLED_Clear(void);                                    // ����
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size);     // ��ʾ�����ַ�
u32 OLED_Pow(u8 m, u8 n);                                 // m �� n �η����ڲ��ã�
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2); // ��ʾ����
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size);  // ��ʾ�ַ���
void OLED_Print(u8 x, u8 y, char *s);                     // printf ������
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]);  // ��λͼ
void OLED_Init(void);                                     // OLED ��ʼ������

#endif /* _OLED__H_ */

