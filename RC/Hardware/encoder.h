#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"
#define ENCODER_MAX 1000 // 最大编码器值
#define ENCODER_MIN 0   // 最小编码器值
// uint16_t Encoder_Value; // 编码器值

void Encoder_Init(void);
int16_t Get_Encoder_Count(void);

#endif
