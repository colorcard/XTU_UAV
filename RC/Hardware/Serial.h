#ifndef __SERIAL_H
#define __SERIAL_H
#include <string.h>
#include <stdbool.h>
#include "main.h"  // 包含主头文件，通常包含设备相关定义和HAL库

//#define UART4_RX_BUF_SIZE  256      // 接收缓冲区大小
//#define UART4_TX_BUF_SIZE  256      // 发送缓冲区大小

//extern UART_HandleTypeDef huart4;   // HAL库UART句柄

//extern volatile uint8_t uart4_rx_buf[UART4_RX_BUF_SIZE];  // 接收缓冲区
//extern volatile uint16_t uart4_rx_len;                    // 接收数据长度
//extern volatile bool uart4_rx_ready;                      // 接收完成标志

//// 函数声明
//void UART4_SendByte(uint8_t data);
//void UART4_SendString(const char *str);
//void UART4_SendData(uint8_t *data, uint16_t len);
//void UART4_StartReceive(void);

#endif // __SERIAL_H
