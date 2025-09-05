/**
 * @file   OLED.h
 * @brief  SSD1306 0.96" OLED 驱动头文件（128×64，软件 I2C）
 * @note   所有硬件相关宏已注释，仅保留高层 API 与常量
 */

#ifndef _OLED__H_
#define _OLED__H_

#include "Application.h"    // 统一数据类型

/*==================== 显示规格常量 ====================*/
#define OLED_MODE     0     ///< 0：保留 4 线 SPI 模式位（实际使用软件 I2C）
#define SIZE          8     ///< 字体高度 8 像素（8×16 点阵）
#define XLevelL       0x00  ///< SSD1306 列地址低 4 位
#define XLevelH       0x10  ///< SSD1306 列地址高 4 位
#define Max_Column    128   ///< 水平像素
#define Max_Row       64    ///< 垂直像素
#define Brightness    0xFF  ///< 对比度：0x00~0xFF

#define X_WIDTH       128   ///< 应用层坐标宽
#define Y_WIDTH       64    ///< 应用层坐标高

/*==================== I2C 写命令/数据标志 ====================*/
#define OLED_CMD   0    ///< 写寄存器命令
#define OLED_DATA  1    ///< 写显存数据

/*==================== 对外 API ====================*/
void OLED_WRByte   (u8 dat, u8 cmd);                         ///< 写单字节
void OLED_Fill     (u8 fill_Data);                           ///< 全屏填充
void OLED_SetPos   (u8 x, u8 y);                             ///< 设置光标
void OLED_Display_On (void);                                 ///< 开显示
void OLED_Display_Off(void);                                 ///< 关显示
void OLED_Clear    (void);                                   ///< 清屏
void OLED_ShowChar (u8 x, u8 y, u8 chr, u8 Char_Size);       ///< 单字符
u32  OLED_Pow      (u8 m, u8 n);                             ///< 幂运算
void OLED_ShowNum  (u8 x, u8 y, u32 num, u8 len, u8 size2);  ///< 数字
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size);     ///< 字符串
void OLED_Print    (u8 x, u8 y, char *s);                    ///< printf 风格
void OLED_DrawBMP  (u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]);    ///< 位图
void OLED_Init     (void);                                   ///< 上电初始化

#endif /* _OLED__H_ */
