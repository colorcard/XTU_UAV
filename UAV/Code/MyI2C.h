#ifndef __MYI2C_H
#define __MYI2C_H

#include <stdint.h>
#include "main.h"  // 包含主头文件，通常包含设备相关定义和HAL库
//#include "Delay.h"

#define MyI2C_SCL_Pin GPIO_PIN_6      // SCL引脚
#define MyI2C_SDA_Pin GPIO_PIN_7      // SDA引脚
#define MyI2C_SCL_GPIO_Port GPIOB     // SCL引脚所在GPIO端口
#define MyI2C_SDA_GPIO_Port GPIOB     // SDA引脚所在GPIO端口

void MyI2C_GPIO_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

#endif
