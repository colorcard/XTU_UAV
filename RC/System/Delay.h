#ifndef __DELAY_H
#define __DELAY_H

#include "main.h"  // 包含主头文件，通常包含设备相关定义和HAL库
//#include "core_cm4.h"  // 包含CMSIS核心头文件，提供对DWT寄存器的访问
// DWT延时函数

uint8_t DWT_Init(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_ns(uint32_t ns);
#endif
