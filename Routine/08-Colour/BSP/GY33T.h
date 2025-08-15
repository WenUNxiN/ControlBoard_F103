#ifndef _GY33T_H
#define _GY33T_H

#include "main.h"

uint8_t GY33T_Write_Byte(uint8_t Slave_Address,uint8_t REG_Address,uint8_t data);
uint8_t GY33T_Read(uint8_t Slave_Address,uint8_t REG_Address,uint8_t *REG_data,uint8_t length);

#endif
