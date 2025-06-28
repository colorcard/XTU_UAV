#ifndef FUZZYPID_H
#define FUZZYPID_H

#include "Global.h"
#include "main.h"
typedef struct {
    float Kp, Ki, Kd;
    float Ti;                 // 积分时间常数
    int use_fuzzy;            // 是否开启模糊校准（1=开，0=关）
    float integral;
    float prev_error, last_error, before_last_error;
    float error_filter;
    float delta_error;
    float output_max, output_min;
    float output;
    float integral_max, integral_min;
} FuzzyPID;

extern FuzzyPID pidRateX_New, pidRateY_New, pidRateZ_New;
extern FuzzyPID pidRoll_New, pidPitch_New, pidYaw_New;
extern FuzzyPID *pPidObject_New[]; // 模糊PID对象数组


void fuzzy_pid_init(FuzzyPID* fuzzy_pid,
                    float Kp, float Ti, float Kd,   // 注意Ki由Kp/Ti自动获得
                    float error_filter,
                    float output_max,
                    float output_min,
                    float integral_max,
                    float integral_min,
                    int use_fuzzy);

void fuzzy_pid_set_mode(FuzzyPID* fuzzy_pid, int use_fuzzy);
void fuzzy_pid_set_Ti(FuzzyPID* fuzzy_pid, float Ti);

float fuzzy_pid_control(FuzzyPID* fuzzy_pid, float target, float current_value);

void fuzzyPidReset(FuzzyPID **pid, const uint8_t len);
void init_all_pid_new(void);


#endif
