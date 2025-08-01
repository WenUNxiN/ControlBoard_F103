#ifndef _OLED__H_ // 防止重复包含
#define _OLED__H_

#include "main.h" // 包含工程公共头文件（stdint、GPIO 定义等）

/*-------------------- 显示相关常量 --------------------*/
#define OLED_MODE 0     // 0：4 线 SPI 模式（保留，实际本文件用 I?C）
#define SIZE 8          // 字体高度 8 像素（8×16 点阵）
#define XLevelL 0x00    // 列地址低 4 位掩码（SSD1306 内部用）
#define XLevelH 0x10    // 列地址高 4 位掩码
#define Max_Column 128  // 屏幕水平像素 128
#define Max_Row 64      // 屏幕垂直像素 64
#define Brightness 0xFF // 对比度寄存器值（越大越亮）

#define X_WIDTH 128 // 屏幕宽（宏给应用层算坐标）
#define Y_WIDTH 64  // 屏幕高

/*-------------------- 软件 I?C 引脚定义 --------------------*/
/* SCL 引脚：PC14 */
#define OLED_SCLPin_CLK RCC_APB2Periph_GPIOC // 打开 GPIOC 时钟
#define OLED_SCLPin_PORT GPIOC               // SCL 所在端口
#define OLED_SCLPin_PIN GPIO_Pin_14          // SCL 引脚

/* SDA 引脚：PC15 */
#define OLED_SDAPin_CLK RCC_APB2Periph_GPIOC
#define OLED_SDAPin_PORT GPIOC
#define OLED_SDAPin_PIN GPIO_Pin_15

/* 位带宏，方便直接拉低/拉高 GPIO，实现软件 I?C 时序 */
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOC, GPIO_Pin_14) // SCL = 0
#define OLED_SCLK_Set() GPIO_SetBits(GPIOC, GPIO_Pin_14)   // SCL = 1

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOC, GPIO_Pin_15) // SDA = 0
#define OLED_SDIN_Set() GPIO_SetBits(GPIOC, GPIO_Pin_15)   // SDA = 1

/*-------------------- I?C 写命令/数据标志 --------------------*/
#define OLED_CMD 0  // 写命令
#define OLED_DATA 1 // 写数据

/*-------------------- 函数原型（软件 I?C 版） --------------------*/
/* 软件 I?C 底层时序 */
void I2C_Start(void);            // 产生 I?C 起始条件
void I2C_Stop(void);             // 产生 I?C 停止条件
void I2C_Wait_Ack(void);         // 等待从机 ACK
void I2C_WriteByte(u8 IIC_Byte); // 发送 1 Byte 数据（MSB 先出）

/* 封装：写命令/写数据（内部自动加 OLED 的 I?C 从机地址 0x78）*/
void I2C_WriteCmd(u8 IIC_Command); // 写 SSD1306 命令
void I2C_WriteData(u8 IIC_Data);   // 写 SSD1306 显存数据

/* OLED 高层 API（与硬件接口无关，直接调用即可） */
void OLED_WRByte(u8 dat, u8 cmd);                         // 兼容旧接口：cmd=0 命令，1 数据
void OLED_Fill(u8 fill_Data);                             // 全屏填充
void OLED_SetPos(u8 x, u8 y);                             // 设置光标（列 x，页 y）
void OLED_Display_On(void);                               // 打开显示
void OLED_Display_Off(void);                              // 关闭显示（休眠）
void OLED_Clear(void);                                    // 清屏
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size);     // 显示单个字符
u32 OLED_Pow(u8 m, u8 n);                                 // m 的 n 次方（内部用）
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2); // 显示数字
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size);  // 显示字符串
void OLED_Print(u8 x, u8 y, char *s);                     // printf 风格输出
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]);  // 画位图
void OLED_Init(void);                                     // OLED 初始化序列

#endif /* _OLED__H_ */
