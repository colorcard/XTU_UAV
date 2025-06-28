#include "encoder.h"

extern TIM_HandleTypeDef htim3;

//由于作者使用的CubMX无选择编码器模式这一选项，故直接自行撰写初始化代码
void Encoder_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 1. 使能TIM3和GPIO端口时钟 */
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* 2. 配置GPIO: 假设TIM3_CH1->PA6, TIM3_CH2->PA7（具体引脚请查数据手册） */
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 3. 配置TIM3为编码器模式 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 0; 
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0xFFFF; 
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0xF;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter = 0xF;

    if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
    {
        // 初始化失败处理
        Error_Handler();
    }

    /* 4. 启动编码器接口 */
    if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK)
    {
        // 启动失败处理
        //Error_Handler();
        //printf("Failed to start encoder interface.\n");
        return; // 这里可以选择不处理错误，直接返回
    }
}

//编码器计数器限幅
void Clamp_Encoder_Count(void) {
    int16_t value = __HAL_TIM_GET_COUNTER(&htim3);
    if (value > ENCODER_MAX) {
        __HAL_TIM_SET_COUNTER(&htim3, ENCODER_MAX);
    } else if (value < ENCODER_MIN) {
        __HAL_TIM_SET_COUNTER(&htim3, ENCODER_MIN);
    }
}

/* 用户可通过如下函数读取编码器值 */
int16_t Get_Encoder_Count(void)
{
    Clamp_Encoder_Count();

    // 返回当前编码器计数值
    return __HAL_TIM_GET_COUNTER(&htim3);
}

//依据编码器计数来设置油门大小油门大小





// /* 错误处理函数，用户可自定义 */
// void Error_Handler(void)
// {
//     // 用户自定义错误处理
//     while(1) {
//         // 可点灯、打印等
//     }
// }


