#include "Soft_I2C.h"

/******************************************************************
 * 函 数 名 称：Start
 * 函 数 说 明：IIC起始信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void Start(void)
{
    SDA_OUT();

    SCL(0);
    SDA(1);
    SCL(1);

    Delay_us(DELAY_COUNT);

    SDA(0);
    Delay_us(DELAY_COUNT);
    SCL(0);
    Delay_us(DELAY_COUNT);
}

/******************************************************************
 * 函 数 名 称：Stop
 * 函 数 说 明：IIC停止信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void Stop(void)
{
    SDA_OUT();

    SCL(0);
    SDA(0);

    SCL(1);
    Delay_us(DELAY_COUNT);
    SDA(1);
    Delay_us(DELAY_COUNT);
}

/******************************************************************
 * 函 数 名 称：IIC_Send_Ack
 * 函 数 说 明：主机发送应答
 * 函 数 形 参：0应答ACK  1非应答NACK
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void Send_Ack(uint8_t ack)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    Delay_us(DELAY_COUNT);
    if (!ack)
        SDA(0);
    else
        SDA(1);
    SCL(1);
    Delay_us(DELAY_COUNT);
    SCL(0);
    SDA(1);
}

/******************************************************************
 * 函 数 名 称：IIC_Wait_Ack
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：1=无应答   0=有应答
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
uint8_t Wait_Ack(void)
{
    char ack = 0;
    unsigned char ack_flag = 10;
    SDA_IN();
    SDA(1);
    Delay_us(DELAY_COUNT);
    SCL(1);
    Delay_us(DELAY_COUNT);
    while ((SDA_STATE() == 1) && (ack_flag))
    {
        ack_flag--;
        Delay_us(DELAY_COUNT);
    }

    if (ack_flag <= 0)
    {
        Stop();
        return 1;
    }
    else
    {
        SCL(0);
        SDA_OUT();
    }
    return ack;
}
/******************************************************************
 * 函 数 名 称：IIC_Write
 * 函 数 说 明：IIC写一个字节
 * 函 数 形 参：dat写入的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void Send(uint8_t data)
{
    uint8_t i;
    SDA_OUT();

    for(i = 0; i < 8; i++) {
        if(data & 0x80) {
            SDA(1);
        } else {
            SDA(0);
        }
        SCL(1);
        Delay_us(DELAY_COUNT);
        SCL(0);
        Delay_us(DELAY_COUNT);
        data <<= 1;
    }
}

/******************************************************************
 * 函 数 名 称：IIC_Read
 * 函 数 说 明：IIC读1个字节
 * 函 数 形 参：无
 * 函 数 返 回：读出的1个字节数据
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
uint8_t Read(void)
{
    unsigned char i, receive = 0;
    SDA_IN(); // SDA设置为输入
    for (i = 0; i < 8; i++)
    {
        SCL(0);
        Delay_us(DELAY_COUNT);
        SCL(1);
        Delay_us(DELAY_COUNT);
        receive <<= 1;
        if (SDA_STATE())
        {
            receive |= 1;
        }
        Delay_us(DELAY_COUNT);
    }

    return receive;
}

void SoftI2C_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // SCL
    RCC_APB2PeriphClockCmd(SCL_PORT_RCC, ENABLE); // 使能端口时钟
    GPIO_InitStructure.GPIO_Pin = SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
    GPIO_Init(SCL_PORT, &GPIO_InitStructure);

    // SDA
    RCC_APB2PeriphClockCmd(SDA_PORT_RCC, ENABLE); // 使能端口时钟
    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  // 开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 速度50MHz
    GPIO_Init(SDA_PORT, &GPIO_InitStructure);

    GPIO_SetBits(SCL_PORT, SCL_PIN);
    GPIO_SetBits(SDA_PORT, SDA_PIN);
    
}
uint8_t SoftI2C_write(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t len)
{
    Start();

    Send(addr << 1); // 发送设备写地址
    if (Wait_Ack())
        return 1; // 等待响应

    Send(reg); // 发送寄存器地址
    if (Wait_Ack())
        return 2; // 等待响应

    while (len--)
    {
        Send(*data++);
        if (Wait_Ack())
            return 3; // 等待响应
    }

    Stop();
    return 0;
}

uint8_t SoftI2C_write2(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t offset, uint32_t len)
{
    Start();

    Send(addr << 1); // 发送设备写地址
    if (Wait_Ack())
        return 1; // 等待响应

    Send(reg); // 发送寄存器地址
    if (Wait_Ack())
        return 2; // 等待响应

    do
    {
        Send(*data);
        data += offset;
        if (Wait_Ack())
            return 3;
    } while (--len);

    Stop();
    return 0;
}

uint8_t SoftI2C_read(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t len)
{
    Start();

    Send(addr << 1); // 发送设备写地址
    if (Wait_Ack())
        return 1; // 等待响应

    Send(reg); // 发送寄存器地址
    if (Wait_Ack())
        return 2; // 等待响应

    Start();
    Send((addr << 1) | 0x01); // 发送设备读地址
    if (Wait_Ack())
        return 3; // 等待响应

    do
    {
        *data = Read();
        data++;
        if (len != 1)
            Send_Ack(0); // 发送 ACK
    } while (--len);
    Send_Ack(1); // 发送 NACK
    Stop();

    return 0;

}

