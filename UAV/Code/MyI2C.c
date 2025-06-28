#include "MyI2C.h"                  // Device header
#include "Delay.h"

/**
  * @brief  写入SCL引脚电平
  * @param  BitValue: 要写入的电平值 (0或1)
  * @retval 无
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
    HAL_GPIO_WritePin(MyI2C_SCL_GPIO_Port, MyI2C_SCL_Pin, (GPIO_PinState)BitValue);
    Delay_us(10);
}

/**
  * @brief  写入SDA引脚电平
  * @param  BitValue: 要写入的电平值 (0或1)
  * @retval 无
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
    HAL_GPIO_WritePin(MyI2C_SDA_GPIO_Port, MyI2C_SDA_Pin, (GPIO_PinState)BitValue);
    Delay_us(10);
}

/**
  * @brief  读取SDA引脚电平
  * @param  无
  * @retval SDA引脚的电平状态 (0或1)
  */
uint8_t MyI2C_R_SDA(void)
{
    return (uint8_t)HAL_GPIO_ReadPin(MyI2C_SDA_GPIO_Port, MyI2C_SDA_Pin);
}

/**
  * @brief  I2C GPIO初始化
  * @param  无
  * @retval 无
  */
void MyI2C_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* SCL引脚配置 */
    GPIO_InitStruct.Pin = MyI2C_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MyI2C_SCL_GPIO_Port, &GPIO_InitStruct);
    
    /* SDA引脚配置 */
    GPIO_InitStruct.Pin = MyI2C_SDA_Pin;
    HAL_GPIO_Init(MyI2C_SDA_GPIO_Port, &GPIO_InitStruct);
    
    /* 初始状态: SCL和SDA都拉高 */
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
}

void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
