#include "My_Interrupt.h"
extern float Yaw_copy;
extern float Pitch_copy;
extern float Roll_copy;
uint32_t my_Tick = 0; 
DroneState drone_state; // 假设类型为 DroneState_t，根据实际类型修改

DroneState Remote_Control = {
    .flight_mode = 0,
    .throttle = 0,
    .pitch = 0,
    .yaw = 0,
    .roll = 0,
    .timestamp = 0,
    .battery_voltage = 0.0f,
    .signal_strength = 0
};

/**
 * @brief 打包NRF24L01数据包
 * @param pitch: 俯仰角度，单位为度
 * @param roll: 翻滚角度，单位为度
 * @param yaw: 偏航角度，单位为度
 * @param mode: 模式标志位
 * @param throttle: 油门值
 * @param packet: 指向存储打包数据的数组的指针
 */
void PackNRF24L01Packet(float pitch, float roll, float yaw, uint8_t mode, uint8_t throttle, uint8_t *packet)
{
    // 限幅
    pitch = LIMIT(pitch, ANGLE_MIN, ANGLE_MAX);
    roll  = LIMIT(roll,  ANGLE_MIN, ANGLE_MAX);
    yaw   = LIMIT(yaw,   ANGLE_MIN, ANGLE_MAX);
    mode  = (mode > MODE_MAX) ? MODE_MAX : mode;
    throttle = (throttle > THROTTLE_MAX) ? THROTTLE_MAX : throttle;

    // 编码：将-179.0~179.0映射到int16_t
    int16_t pitch_i = (int16_t)(pitch * ANGLE_SCALE + 0.5f);
    int16_t roll_i  = (int16_t)(roll  * ANGLE_SCALE + 0.5f);
    int16_t yaw_i   = (int16_t)(yaw   * ANGLE_SCALE + 0.5f);

    packet[0] = pitch_i & 0xFF;
    packet[1] = (pitch_i >> 8) & 0xFF;

    packet[2] = roll_i & 0xFF;
    packet[3] = (roll_i >> 8) & 0xFF;

    packet[4] = yaw_i & 0xFF;
    packet[5] = (yaw_i >> 8) & 0xFF;

    packet[6] = mode;
    packet[7] = throttle;
}

/**
 * @brief 解包NRF24L01数据包
 * @param packet: 指向接收到的数据包的指针
 * @param pitch: 指向存储pitch值的变量的指针
 * @param roll: 指向存储roll值的变量的指针
 * @param yaw: 指向存储yaw值的变量的指针
 * @param mode: 指向存储mode值的变量的指针
 * @param throttle: 指向存储throttle值的变量的指针
 */
void UnpackNRF24L01Packet(const uint8_t *packet, float *pitch, float *roll, float *yaw, uint8_t *mode, uint8_t *throttle)
{
    int16_t pitch_i = packet[0] | (packet[1] << 8);
    int16_t roll_i  = packet[2] | (packet[3] << 8);
    int16_t yaw_i   = packet[4] | (packet[5] << 8);

    *pitch = pitch_i / ANGLE_SCALE;
    *roll  = roll_i  / ANGLE_SCALE;
    *yaw   = yaw_i   / ANGLE_SCALE;

    *pitch    = LIMIT(*pitch, ANGLE_MIN, ANGLE_MAX);
    *roll     = LIMIT(*roll,  ANGLE_MIN, ANGLE_MAX);
    *yaw      = LIMIT(*yaw,   ANGLE_MIN, ANGLE_MAX);

    *mode = (packet[6] > MODE_MAX) ? MODE_MAX : packet[6];
    *throttle = (packet[7] > THROTTLE_MAX) ? THROTTLE_MAX : packet[7];
}



/**
 * @brief 定时器2中断回调函数
 * @param htim: 定时器句柄
 * 
 * 该函数在定时器2的溢出事件发生时被调用。
 * 在此函数中，我们扫描按键并根据按键动作更新菜单。
 * 同时在OLED上显示当前菜单的ID。
 * 将通讯数据打包并在这个函数中发送。
 * 数据10ms一发送，若收到数据即进行解码
 * 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) // 判断是哪个定时器
    {
        //轮询接受数据
        if(NRF24L01_Receive()==1){
            //接收到数据，解包
            UnpackNRF24L01Packet(NRF24L01_RxPacket, &Remote_Control.pitch, &Remote_Control.roll, &Remote_Control.yaw, &Remote_Control.flight_mode, &Remote_Control.throttle);
            OLED_ShowFloatNum(4*16, 16*0, Remote_Control.pitch, 3,2, OLED_8X16);
            OLED_ShowFloatNum(4*16, 16*1, Remote_Control.roll, 3,2, OLED_8X16);
            OLED_ShowFloatNum(4*16, 16*2, Remote_Control.yaw, 3,2, OLED_8X16);

            //从机发送姿态数据
            drone_state.pitch = Pitch_copy; // 更新俯仰角
            drone_state.roll = Roll_copy;   // 更新横滚角
            drone_state.yaw = Yaw_copy;     // 更新偏航角
            // 打包并发送数据
            PackNRF24L01Packet(drone_state.pitch, drone_state.roll, drone_state.yaw, drone_state.flight_mode, drone_state.throttle, NRF24L01_TxPacket);
            Send_Flag = NRF24L01_Send(); // 发送数据包
            OLED_ShowNum(7*16, 16*3+8, Send_Flag,2, OLED_6X8);

        }

        //PID控制器,周期为3ms
        my_Tick++;
        if (my_Tick == 3&&drone_state.flight_mode == 1) // 仅在飞行模式1时执行PID控制
        {
            my_Tick = 0; // 重置计时器
            // 执行PID控制逻辑
            



            
            // 或者其他相关的控制逻辑
        }
        {
            /* code */
        }
        

    }
}










