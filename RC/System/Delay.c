#include "Delay.h"

/**
  * @brief  初始化DWT(Data Watchpoint and Trace)单元
  * @param  无
  * @retval 0-成功; 1-失败(DWT不可用)
  */
uint8_t DWT_Init(void)
{
    // 启用DWT跟踪
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    // 重置周期计数器
    DWT->CYCCNT = 0;
    
    // 启用周期计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    // 检查DWT是否可用
    if(DWT->CYCCNT)
    {
        return 0;  // 初始化成功
    }
    else
    {
        return 1;  // DWT不可用
    }
}

/**
  * @brief  纳秒级延时(近似)
  * @param  ns 延时的纳秒数
  * @retval 无
  * @note   在80MHz下，1个时钟周期=12.5ns，所以这是近似延时
  */
void Delay_ns(uint32_t ns)
{
    uint32_t start = DWT->CYCCNT;
    // 80MHz → 0.08 cycles/ns (需要浮点运算)
    uint32_t cycles = (ns * 8) / 100;  // 整数近似计算
    if(cycles < 1) cycles = 1;         // 至少延时1个周期
    while ((DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  微秒级延时
  * @param  us 延时的微秒数
  * @retval 无
  */
void Delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * 80;  // 80MHz → 80 cycles/μs
    // 处理计数器溢出情况
    while ((DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  毫秒级延时
  * @param  ms 延时的毫秒数
  * @retval 无
  */
void Delay_ms(uint32_t ms)
{
    uint32_t start = DWT->CYCCNT;
    // 80MHz → 80,000 cycles/ms (80 cycles/μs * 1000)
    uint32_t cycles = ms * 80000;  
    while ((DWT->CYCCNT - start) < cycles);
}

/**
  * @brief  秒级延时
  * @param  s 延时的秒数
  * @retval 无
  */
void Delay_s(uint32_t s)
{
    while(s--)
    {
        Delay_ms(1000);  // 调用毫秒延时1000次
    }
}

