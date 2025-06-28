#include "Serial.h"
/* 在 main.c 或 uart.c 中添加全局变量 */
//volatile uint8_t uart4_rx_buf[UART4_RX_BUF_SIZE];
//volatile uint16_t uart4_rx_len = 0;
//volatile bool uart4_rx_ready = false;

///**
//  * @brief  启动UART4接收（启用空闲中断）
//  * @param  无
//  * @retval 无
//  */
//void UART4_StartReceive(void) {
//    uart4_rx_len = 0;
//    uart4_rx_ready = false;
//    HAL_UART_Receive_IT(&huart4, (uint8_t *)uart4_rx_buf, 1);  // 启动单字节接收
//    __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);               // 启用空闲中断
//}

///**
//  * @brief  UART4接收中断回调函数
//  * @param  huart: UART句柄
//  * @retval 无
//  */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == UART1) {
//        if (uart4_rx_len < UART4_RX_BUF_SIZE - 1) {
//            uart4_rx_len++;  // 更新接收长度
//        }
//        HAL_UART_Receive_IT(&huart4, (uint8_t *)&uart4_rx_buf[uart4_rx_len], 1);  // 继续接收
//    }
//}

///**
//  * @brief  UART4空闲中断回调函数（检测数据包结束）
//  * @param  huart: UART句柄
//  * @retval 无
//  */
//void HAL_UART_IdleCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == UART1 && uart4_rx_len > 0) {
//        uart4_rx_buf[uart4_rx_len] = '\0';  // 添加字符串结束符
//        uart4_rx_ready = true;              // 设置接收完成标志
//        UART4_StartReceive();               // 重新启动接收
//    }
//}

///**
//  * @brief  UART4发送单个字节
//  * @param  data: 要发送的字节
//  * @retval 无
//  */
//void UART4_SendByte(uint8_t data) {
//    HAL_UART_Transmit(&huart4, &data, 1, HAL_MAX_DELAY);
//}

///**
//  * @brief  UART4发送字符串（自动计算长度）
//  * @param  str: 要发送的字符串
//  * @retval 无
//  */
//void UART4_SendString(const char *str) {
//    HAL_UART_Transmit(&huart4, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
//}

///**
//  * @brief  UART4发送原始数据（数组/数据包）
//  * @param  data: 数据指针
//  * @param  len: 数据长度
//  * @retval 无
//  */
//void UART4_SendData(uint8_t *data, uint16_t len) {
//    HAL_UART_Transmit(&huart4, data, len, HAL_MAX_DELAY);
//}
