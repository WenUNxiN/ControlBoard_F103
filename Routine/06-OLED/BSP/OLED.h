#ifndef _OLED__H_ // ��ֹ�ظ�����
#define _OLED__H_

#include "main.h" // �������̹���ͷ�ļ���stdint��GPIO ����ȣ�

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

/*-------------------- ��� I?C ���Ŷ��� --------------------*/
/* SCL ���ţ�PC14 */
#define OLED_SCLPin_CLK RCC_APB2Periph_GPIOC // �� GPIOC ʱ��
#define OLED_SCLPin_PORT GPIOC               // SCL ���ڶ˿�
#define OLED_SCLPin_PIN GPIO_Pin_14          // SCL ����

/* SDA ���ţ�PC15 */
#define OLED_SDAPin_CLK RCC_APB2Periph_GPIOC
#define OLED_SDAPin_PORT GPIOC
#define OLED_SDAPin_PIN GPIO_Pin_15

/* λ���꣬����ֱ������/���� GPIO��ʵ����� I?C ʱ�� */
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOC, GPIO_Pin_14) // SCL = 0
#define OLED_SCLK_Set() GPIO_SetBits(GPIOC, GPIO_Pin_14)   // SCL = 1

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOC, GPIO_Pin_15) // SDA = 0
#define OLED_SDIN_Set() GPIO_SetBits(GPIOC, GPIO_Pin_15)   // SDA = 1

/*-------------------- I?C д����/���ݱ�־ --------------------*/
#define OLED_CMD 0  // д����
#define OLED_DATA 1 // д����

/*-------------------- ����ԭ�ͣ���� I?C �棩 --------------------*/
/* ��� I?C �ײ�ʱ�� */
void I2C_Start(void);            // ���� I?C ��ʼ����
void I2C_Stop(void);             // ���� I?C ֹͣ����
void I2C_Wait_Ack(void);         // �ȴ��ӻ� ACK
void I2C_WriteByte(u8 IIC_Byte); // ���� 1 Byte ���ݣ�MSB �ȳ���

/* ��װ��д����/д���ݣ��ڲ��Զ��� OLED �� I?C �ӻ���ַ 0x78��*/
void I2C_WriteCmd(u8 IIC_Command); // д SSD1306 ����
void I2C_WriteData(u8 IIC_Data);   // д SSD1306 �Դ�����

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
