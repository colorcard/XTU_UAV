#ifndef MY_INTERRUPT_H
#define MY_INTERRUPT_H

#include "main.h"
#include "OLED.h"
#include "NRF24L01.h"
#include "MPU6050.h"
#include "MadgwickAHRS.h"
#include "Global.h"

extern DroneState drone_state; // 假设类型为 DroneState_t，根据实际类型修改


void PackNRF24L01Packet(float pitch, float roll, float yaw, uint8_t mode, uint8_t throttle, uint8_t *packet);
void UnpackNRF24L01Packet(const uint8_t *packet, float *pitch, float *roll, float *yaw, uint8_t *mode, uint8_t *throttle);

#endif /* MY_INTERRUPT_H */
