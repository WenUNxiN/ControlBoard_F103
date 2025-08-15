#ifndef _SOFT_I2C_H_
#define _SOFT_I2C_H_

#include "Application.h"

/*-------------------- 软件 I2C 引脚定义 --------------------*/
/* SCL 引脚：PC14 */
#define SCL_PORT_RCC    RCC_APB2Periph_GPIOC // 打开 GPIOC 时钟
#define SCL_PORT        GPIOC                    // SCL 所在端口
#define SCL_PIN         GPIO_Pin_14               // SCL 引脚

/* SDA 引脚：PC15 */
#define SDA_PORT_RCC    RCC_APB2Periph_GPIOC
#define SDA_PORT        GPIOC
#define SDA_PIN         GPIO_Pin_15

/* 延时 */
#define DELAY_COUNT  0

/* 位带宏，方便直接拉低/拉高 GPIO，实现软件 I2C 时序 */
#define SCL(BIT) 		GPIO_WriteBit(SCL_PORT, SCL_PIN, BIT?Bit_SET:Bit_RESET)
#define SDA(BIT) 		GPIO_WriteBit(SDA_PORT, SDA_PIN, BIT?Bit_SET:Bit_RESET)
#define SDA_STATE() 	       GPIO_ReadInputDataBit(SDA_PORT, SDA_PIN)

// 上拉输入模式
#define SDA_IN()         {      GPIO_InitTypeDef  GPIO_InitStructure;\
                                GPIO_InitStructure.GPIO_Pin = SDA_PIN;\
                                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;\
                                GPIO_Init(SDA_PORT, &GPIO_InitStructure);\
                         }
// 开漏输出
#define SDA_OUT()         {     GPIO_InitTypeDef  GPIO_InitStructure;\
                                GPIO_InitStructure.GPIO_Pin = SDA_PIN;\
                                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;\
                                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                GPIO_Init(SDA_PORT, &GPIO_InitStructure);\
                         }

void SoftI2C_init(void);
uint8_t SoftI2C_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);
uint8_t SoftI2C_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);

uint8_t SoftI2C_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len);

#endif
