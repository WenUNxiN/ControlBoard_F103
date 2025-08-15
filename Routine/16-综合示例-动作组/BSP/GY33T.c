#include "GY33T.h"

//**************************************
//��GY33Tд��һ���ֽ�����
//**************************************
uint8_t GY33T_Write_Byte(uint8_t Slave_Address,uint8_t REG_Address,uint8_t data)
{
    return SoftI2C_write(Slave_Address, REG_Address, &data, 1);
}
//**************************************
//��ȡlength���ֽ�����
//**************************************
uint8_t GY33T_Read(uint8_t Slave_Address,uint8_t REG_Address,uint8_t *REG_data,uint8_t length)
{
    return SoftI2C_read(Slave_Address >> 1, REG_Address, REG_data, length);;
}
