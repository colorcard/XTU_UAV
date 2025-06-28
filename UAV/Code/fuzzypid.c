#include "fuzzypid.h"
#include <stdio.h>


#define N_RULES 7
#define ABS(x) ((x) > 0 ? (x) : -(x))
//归一化因子
#define FUZZY_ANGLE_NORM_FACTOR (1.0f / 20.0f)

//模糊PID结构体
FuzzyPID pidRateX_New, pidRateY_New, pidRateZ_New;
FuzzyPID pidRoll_New, pidPitch_New, pidYaw_New;
FuzzyPID *pPidObject_New[] = { &pidRateX_New, &pidRateY_New, &pidRateZ_New, &pidRoll_New, &pidPitch_New, &pidYaw_New };


//#define LIMIT(val, min, max) (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val))

typedef struct {
    float a, b, c;
} TriMF;

static const TriMF mf_params[N_RULES] = {
    { -1.0, -1.0, -0.6 }, // NL
    { -1.0, -0.6, -0.2 }, // NM
    { -0.6, -0.2,  0.0 }, // NS
    { -0.2,  0.0,  0.2 }, // ZO
    {  0.0,  0.2,  0.6 }, // PS
    {  0.2,  0.6,  1.0 }, // PM
    {  0.6,  1.0,  1.0 }  // PL
};

const float rule_table_dKp[N_RULES][N_RULES] = {
    { -0.01, -0.01, -0.008, -0.006, -0.004, -0.002,  0.00 },
    { -0.01, -0.008, -0.006, -0.004, -0.002,  0.00,  0.002 },
    { -0.008, -0.006, -0.004, -0.002,  0.00,  0.002,  0.004 },
    { -0.006, -0.004, -0.002,  0.00,  0.002,  0.004,  0.006 },
    { -0.004, -0.002,  0.00,  0.002,  0.004,  0.006,  0.008 },
    { -0.002,  0.00,  0.002,  0.004,  0.006,  0.008,  0.01 },
    {  0.00,  0.002,  0.004,  0.006,  0.008,  0.01,  0.01 }
};

const float rule_table_dKd[N_RULES][N_RULES] = {
    { -0.004, -0.004, -0.003, -0.002, -0.0015, -0.001,  0.00 },
    { -0.004, -0.003, -0.002, -0.0015, -0.001,  0.00,  0.001 },
    { -0.003, -0.002, -0.0015, -0.001,  0.00,  0.001,  0.0015 },
    { -0.002, -0.0015, -0.001,  0.00,  0.001,  0.0015,  0.002 },
    { -0.0015, -0.001,  0.00,  0.001,  0.0015,  0.002,  0.003 },
    { -0.001,  0.00,  0.001,  0.0015,  0.002,  0.003,  0.004 },
    {  0.00,  0.001,  0.0015,  0.002,  0.003,  0.004,  0.004 }
};






static float triangle_mf(float x, float a, float b, float c) {
    if (x <= a || x >= c) return 0.0f;
    else if (x == b) return 1.0f;
    else if (x < b) return (x - a) / (b - a);
    else return (c - x) / (c - b);
}
static void fuzzify(float x, float *membership) {
    int i;
    for (i = 0; i < N_RULES; ++i) {
        membership[i] = triangle_mf(x, mf_params[i].a, mf_params[i].b, mf_params[i].c);
    }
}
static float fuzzy_inference(float e, float ec, const float rule_table[N_RULES][N_RULES]) {
    float me[N_RULES], mec[N_RULES];
    float numerator = 0, denominator = 0;
    int i, j;
    fuzzify(e, me);
    fuzzify(ec, mec);
    for (i = 0; i < N_RULES; ++i) {
        for (j = 0; j < N_RULES; ++j) {
            float weight = me[i] * mec[j];
            numerator   += weight * rule_table[i][j];
            denominator += weight;
        }
    }
    if (denominator == 0) return 0;
    return numerator / denominator;
}

void fuzzy_pid_init(FuzzyPID* fuzzy_pid,
                    float Kp, float Ti, float Kd,
                    float error_filter,
                    float output_max,
                    float output_min,
                    float integral_max,
                    float integral_min,
                    int use_fuzzy) {
    fuzzy_pid->Kp = Kp;
    fuzzy_pid->Ti = Ti;
    fuzzy_pid->Ki = Kp * Ti;
    fuzzy_pid->Kd = Kd;
    fuzzy_pid->integral = 0.0f;
    fuzzy_pid->prev_error = 0.0f;
    fuzzy_pid->last_error = 0.0f;
    fuzzy_pid->before_last_error = 0.0f;
    fuzzy_pid->error_filter = error_filter;
    fuzzy_pid->delta_error = 0.0f;
    fuzzy_pid->output_max = output_max;
    fuzzy_pid->output_min = output_min;
    fuzzy_pid->output = 0.0f;
    fuzzy_pid->integral_max = integral_max;
    fuzzy_pid->integral_min = integral_min;
    fuzzy_pid->use_fuzzy = use_fuzzy;
}

void fuzzy_pid_set_mode(FuzzyPID* fuzzy_pid, int use_fuzzy) {
    fuzzy_pid->use_fuzzy = use_fuzzy;
}
void fuzzy_pid_set_Ti(FuzzyPID* fuzzy_pid, float Ti) {
    fuzzy_pid->Ti = LIMIT(Ti, 0.01f, 100.0f); // 防止Ti为0
    fuzzy_pid->Ki = fuzzy_pid->Kp / fuzzy_pid->Ti;
}

float fuzzy_pid_control(FuzzyPID* fuzzy_pid, float target, float current_value) {
    float new_error, e_norm, ec_norm, dkp = 0, dkd = 0;

    new_error = target - current_value;

    fuzzy_pid->before_last_error = fuzzy_pid->last_error;
    fuzzy_pid->last_error = fuzzy_pid->prev_error;
    fuzzy_pid->prev_error = new_error;
    fuzzy_pid->delta_error = fuzzy_pid->prev_error - fuzzy_pid->last_error;

    // 死区处理
    if (ABS(fuzzy_pid->prev_error) <= fuzzy_pid->error_filter) {
        fuzzy_pid->prev_error = 0.0f;
    }

    // 归一化因子建议见前文，可根据实际最大误差调整
    // 例：最大可控偏角为10度归一化为1.0
    e_norm  = LIMIT(fuzzy_pid->prev_error * FUZZY_ANGLE_NORM_FACTOR, -1.0f, 1.0f);
    ec_norm = LIMIT(fuzzy_pid->delta_error * FUZZY_ANGLE_NORM_FACTOR, -1.0f, 1.0f);

    if (fuzzy_pid->use_fuzzy) {
        dkp = fuzzy_inference(e_norm, ec_norm, rule_table_dKp);
        dkd = fuzzy_inference(e_norm, ec_norm, rule_table_dKd);

        fuzzy_pid->Kp += dkp;
        fuzzy_pid->Kd += dkd;

        fuzzy_pid->Kp = LIMIT(fuzzy_pid->Kp, 5.0f, 10.0f);
        fuzzy_pid->Kd = LIMIT(fuzzy_pid->Kd, 0.0f, 0.9f);
        fuzzy_pid->Ki = fuzzy_pid->Kp / fuzzy_pid->Ti;
    }

    fuzzy_pid->integral += fuzzy_pid->prev_error;
    if (fuzzy_pid->integral > fuzzy_pid->integral_max)
        fuzzy_pid->integral = fuzzy_pid->integral_max;
    if (fuzzy_pid->integral < fuzzy_pid->integral_min)
        fuzzy_pid->integral = fuzzy_pid->integral_min;

    fuzzy_pid->output = fuzzy_pid->Kp * fuzzy_pid->prev_error
        + fuzzy_pid->Ki * fuzzy_pid->integral
        + fuzzy_pid->Kd * (fuzzy_pid->prev_error + fuzzy_pid->before_last_error - 2 * fuzzy_pid->last_error);

    fuzzy_pid->output = LIMIT(fuzzy_pid->output, fuzzy_pid->output_min, fuzzy_pid->output_max);

    return fuzzy_pid->output;
}

/// 重置模糊PID控制器
void fuzzyPidReset(FuzzyPID **pid, const uint8_t len) {
    uint8_t i;
    for(i = 0; i < len; i++) {
        pid[i]->integral = 0.0f;
        pid[i]->prev_error = 0.0f;
        pid[i]->last_error = 0.0f;
        pid[i]->before_last_error = 0.0f;
        pid[i]->delta_error = 0.0f;
        pid[i]->output = 0.0f;
    }
}


//模糊PID控制器初始化函数(3sm控制周期)
void init_all_pid_new(void) {
    const float dt = 0.003f; 
   //速度环
    fuzzy_pid_init(&pidRateX_New, 2.0f, dt, 0.08f,  // Kp, Ti, Kd
                   1.0f,                           // error_filter
                   200.0f, -200.0f,                // output_max, output_min
                   100.0f, -100.0f,                // integral_max, integral_min
                   0);                             // use_fuzzy

    fuzzy_pid_init(&pidRateY_New, 2.0f, dt, 0.08f,
                   1.0f,
                   200.0f, -200.0f,
                   100.0f, -100.0f,
                   0);

    fuzzy_pid_init(&pidRateZ_New, 6.0f, dt, 0.5f,
                   1.0f,
                   200.0f, -200.0f,
                   60.0f, -60.0f,
                   0);

    // 位置环
    fuzzy_pid_init(&pidRoll_New, 7.0f, dt, 0.08f,
                   1.0f,
                   200.0f, -200.0f,
                   100.0f, -100.0f,
                   1);

    fuzzy_pid_init(&pidPitch_New, 7.0f, dt, 0.08f,
                   1.0f,
                   200.0f, -200.0f,
                   100.0f, -100.0f,
                   1);

    fuzzy_pid_init(&pidYaw_New, 4.0f, dt, 0.08f,
                   1.0f,
                   200.0f, -200.0f,
                   100.0f, -100.0f,
                   1);
}
