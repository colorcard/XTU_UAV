#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "main.h"
//声明变量的头文件
extern float Yaw; // 偏航角
extern float Pitch; // 俯仰角   
extern float Roll; // 横滚角
extern float Yaw_copy; // 用于记录Yaw角度的副本
extern float Pitch_copy; // 用于记录Pitch角度的副本
extern float Roll_copy; // 用于记录Roll角度的副本
extern int16_t Encoder_Value; // 编码器值
extern uint8_t page; // 页面状态量，0表示菜单显示，1表示进入功能页面

/********数据包发送相关定义（start）*************/
// 限幅宏
#define LIMIT(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

// 数据范围与精度
#define ANGLE_MIN   -179.0f
#define ANGLE_MAX    179.0f
#define ANGLE_SCALE   10.0f   // 精度0.1

#define MODE_MIN      0
#define MODE_MAX      9
#define THROTTLE_MIN  0
#define THROTTLE_MAX  100

/********数据包发送相关定义（end）*************/
/********无人机状态结构体（start）*****************/
typedef struct {
    // 飞行模式 (0-9)
    uint8_t flight_mode;
    
    // 油门大小 (0-100%)
    uint8_t throttle;
    
    // 姿态角 (单位: 度)
    float pitch;    // 俯仰角 (-179~179)
    float yaw;      // 偏航角 (-179~179) 
    float roll;     // 横滚角 (-179~179)

    // 可选扩展字段
    uint32_t timestamp;  // 时间戳(ms)
    float battery_voltage; // 电池电压(V)
    uint8_t signal_strength; // 信号强度(0-100%)
} DroneState;

extern DroneState drone_state;
extern DroneState Remote_Control; // 用于接收遥控器数据的结构体
/********无人机状态结构体（end）*******************/

/****功能参数外部调用接口(start)*********/
extern uint8_t page;
/****功能参数外部调用接口(end)***********/

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;
extern uint16_t temp;
extern uint8_t Send_Flag; // 用于记录发送标志位


#endif /* __GLOBAL_H */
