#ifndef _SPI_FLASH__H_
#define _SPI_FLASH__H_
#include "stm32f10x_spi.h"
#include "SysTick.h"

/* SPI 片选引脚选择 */
#define SPI_FLASH_CS(x) GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)x)

/*******W25Q系列芯片ID*******/
#define W25Q80 0XEF13
#define W25Q16 0XEF14
#define W25Q32 0XEF15
#define W25Q64 0XEF16

/*******W25Q64芯片变量宏定义*******/
#define W25Q64_SECTOR_SIZE 4096 // 4K
#define W25Q64_SECTOR_NUM 2048  // 8*1024/4 = 2048

/*******W25Q64芯片地址存储表*******/
#define FLASH_ASC16_ADDRESS 0
#define FLASH_HZK16_ADDRESS 0x1000

#define FLASH_SYSTEM_CONFIG_ADDRESS 0x43000

#define FLASH_BITMAP1_SIZE_ADDRESS 0x50000
#define FLASH_BITMAP2_SIZE_ADDRESS FLASH_BITMAP1_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAP3_SIZE_ADDRESS FLASH_BITMAP2_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAP4_SIZE_ADDRESS FLASH_BITMAP3_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAP5_SIZE_ADDRESS FLASH_BITMAP4_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAP6_SIZE_ADDRESS FLASH_BITMAP5_SIZE_ADDRESS + 0x28000

#define FLASH_BITMAPMAIN_SIZE_ADDRESS FLASH_BITMAP6_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAPDS1302_SIZE_ADDRESS FLASH_BITMAPMAIN_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAPDS18B20_SIZE_ADDRESS FLASH_BITMAPDS1302_SIZE_ADDRESS + 0x28000
#define FLASH_BITMAPBLUETOOTH_SIZE_ADDRESS FLASH_BITMAPDS18B20_SIZE_ADDRESS + 0x28000

/*******FLASH相关指令表*******/
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02 /* 写页命令 */
#define W25X_SectorErase 0x20 /* 扇区擦除指令 */
#define W25X_BlockErase 0xD8
#define W25X_ChipErase 0xC7        /* 芯片擦除命令 */
#define W25X_PowerDown 0xB9        /* 掉电命令 */
#define W25X_ReleasePowerDown 0xAB /* 唤醒指令 */
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90 /* 读取ID命令 */
#define W25X_JedecDeviceID 0x9F

void SpiFlash_On(u8 x);               /* LED引脚和SPI引脚重合，切换SPI和LED引脚状态 */
void SpiFlash_Init(void);             /* 初始化SPI FLASH的IO口 */
void Spi_SetSpeed(u16 SpeedSet); /* SPI 速度设置函数 */
u8 Spi_WriteRead(u8 TxData);          /* SPI读写数据 */

u16 SpiFlash_Read_Id(void);                        /* 读取芯片ID W25X16的ID:0XEF14 */
u8 SpiFlash_Read_SR(void);                         /* 读SPI_FLASH的状态寄存器 */
void SpiFlash_Write_SR(u8 byte);                   /* 写SPI_FLASH的状态寄存器 */
void SpiFlash_Write_Enable(void);                  /* SPI_FLASH写使能，WEL置位 */
void SpiFlash_Write_Disable(void);                 /* SPI_FLASH写禁止，将WEL清零 */
char SpiFlash_Read_Char(u32 readAddr);             /* SPI读取1个字节的数据 */
void SpiFlash_Write_Char(char tmp, u32 WriteAddr); /* SPI写入1个字节的数据 */

void SpiFlash_Read(u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead);             /* 在指定地址开始读取指定长度的数据 */
void SpiFlash_Write(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);          /* 在指定地址开始写入指定长度的数据 */
void SpiFlash_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);     /* SPI在一页内写入少于256个字节的数据 */
void SpiFlash_Write_Sector(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);   /* SPI在一扇区内写入少于4096个字节的数据 */
void SpiFlash_Write_No_Check(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite); /* 无检验写SPI_FLASH */
void SpiFlash_Erase_Sector(u32 Dst_Addr);                                     /* 擦除一个扇区 最少150毫秒 */
void SpiFlash_Erase_Chip(void);                                               /* 擦除整个芯片 */
void SpiFlash_Wait_Busy(void);                                                /* 等待空闲 */
void SpiFlash_Power_Down(void);                                               /* 进入掉电模式 */
void SpiFlash_Wake_Up(void);                                                  /* 唤醒 */
void SpiFlash_Write_S(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);


#define w25x_init() SpiFlash_Init()
#define w25x_readId() spi_flash_read_id()
#define w25x_read(buf, addr, len) SpiFlash_Read(buf, addr, len)
#define w25x_write(buf, addr, len) SpiFlash_Write_No_Check(buf, addr, len)
#define w25x_writeS(buf, addr, len) SpiFlash_Write_S(buf, addr, len)
#define w25x_erase_sector(addr) SpiFlash_Erase_Sector(addr)
#define w25x_wait_busy() SpiFlash_Wait_Busy()
#endif
