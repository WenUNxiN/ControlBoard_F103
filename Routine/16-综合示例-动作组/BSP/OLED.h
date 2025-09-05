/**
 * @file   OLED.h
 * @brief  SSD1306 0.96" OLED ����ͷ�ļ���128��64����� I2C��
 * @note   ����Ӳ����غ���ע�ͣ��������߲� API �볣��
 */

#ifndef _OLED__H_
#define _OLED__H_

#include "Application.h"    // ͳһ��������

/*==================== ��ʾ����� ====================*/
#define OLED_MODE     0     ///< 0������ 4 �� SPI ģʽλ��ʵ��ʹ����� I2C��
#define SIZE          8     ///< ����߶� 8 ���أ�8��16 ����
#define XLevelL       0x00  ///< SSD1306 �е�ַ�� 4 λ
#define XLevelH       0x10  ///< SSD1306 �е�ַ�� 4 λ
#define Max_Column    128   ///< ˮƽ����
#define Max_Row       64    ///< ��ֱ����
#define Brightness    0xFF  ///< �Աȶȣ�0x00~0xFF

#define X_WIDTH       128   ///< Ӧ�ò������
#define Y_WIDTH       64    ///< Ӧ�ò������

/*==================== I2C д����/���ݱ�־ ====================*/
#define OLED_CMD   0    ///< д�Ĵ�������
#define OLED_DATA  1    ///< д�Դ�����

/*==================== ���� API ====================*/
void OLED_WRByte   (u8 dat, u8 cmd);                         ///< д���ֽ�
void OLED_Fill     (u8 fill_Data);                           ///< ȫ�����
void OLED_SetPos   (u8 x, u8 y);                             ///< ���ù��
void OLED_Display_On (void);                                 ///< ����ʾ
void OLED_Display_Off(void);                                 ///< ����ʾ
void OLED_Clear    (void);                                   ///< ����
void OLED_ShowChar (u8 x, u8 y, u8 chr, u8 Char_Size);       ///< ���ַ�
u32  OLED_Pow      (u8 m, u8 n);                             ///< ������
void OLED_ShowNum  (u8 x, u8 y, u32 num, u8 len, u8 size2);  ///< ����
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size);     ///< �ַ���
void OLED_Print    (u8 x, u8 y, char *s);                    ///< printf ���
void OLED_DrawBMP  (u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]);    ///< λͼ
void OLED_Init     (void);                                   ///< �ϵ��ʼ��

#endif /* _OLED__H_ */
