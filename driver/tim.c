/**
 * @file    tim.c
 * @brief   定时器配置模块实现
 * @details TIM6：基础定时器，产生 1ms 周期中断，为系统提供时间基准。
 *          TIM2：通用定时器，产生可配置频率的 TRGO 输出，触发 ADC 双路同步采样。
 *
 *          时钟树说明（STM32F407 @168MHz）：
 *          - APB1 时钟 = 42MHz（TIMx 在 APB1 下时钟为 84MHz）
 *          - APB2 时钟 = 84MHz
 *          - TIM6 挂在 APB1：84MHz
 *          - TIM2 挂在 APB1：84MHz
 */

#include "tim.h"   /* 包含本模块头文件，获取函数声明 */


/**
 * @brief  初始化 TIM6：1ms 周期定时器
 * @note   系统滴答定时器，中断频率 1kHz（周期 1ms）。
 *         中断服务函数 TIM6_DAC_IRQHandler 中：
 *         - g_delay_tick++：驱动非阻塞延时模块
 *         - KEY_Scan()：驱动按键扫描模块
 *
 *         计算过程：
 *         - TIM6 时钟 = 84MHz
 *         - 预分频 8400 → 84MHz / 8400 = 10kHz（0.1ms 分辨率）
 *         - 周期 10 → 10kHz / 10 = 1kHz（1ms 中断间隔）
 *         - 即：84MHz / 8400 / 10 = 1kHz
 *
 *         NVIC 配置：
 *         - 优先级分组 4（4 位全部用于抢占优先级，符合 FreeRTOS 使用习惯）
 *         - 抢占优先级 0，子优先级 0
 */
void TIM6_Init(void)
{
    NVIC_InitTypeDef         NVIC_InitStruct;          /* NVIC 中断控制器初始化结构体 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;   /* 定时器时基初始化结构体 */

    /* ========== 步骤1：NVIC 中断优先级配置 ========== */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);                      /* FreeRTOS 项目统一使用 4 位抢占优先级 */
    NVIC_InitStruct.NVIC_IRQChannel                   = TIM6_DAC_IRQn;   /* 选择 TIM6_DAC 中断通道 */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;               /* 抢占优先级 0（最高） */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;               /* PriorityGroup_4 下无子优先级 */
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;          /* 使能该中断通道 */
    NVIC_Init(&NVIC_InitStruct);                                         /* 将配置写入 NVIC 寄存器 */

    /* ========== 步骤2：TIM6 时基配置 ========== */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);                /* 使能 TIM6 外设时钟（APB1 @84MHz） */

    TIM_TimeBaseInitStruct.TIM_Prescaler         = 8400U - 1U;          /* 预分频值：84MHz / 8400 = 10kHz */
    TIM_TimeBaseInitStruct.TIM_Period            = 10U - 1U;            /* 自动重载值：10kHz / 10 = 1kHz（1ms） */
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;        /* 时钟分频：不分频 */
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;  /* 向上计数模式 */
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0U;                  /* 重复计数器：不使用 */
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);                    /* 将时基配置写入 TIM6 寄存器 */

    /* ========== 步骤3：使能中断和定时器 ========== */
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                           /* 使能 TIM6 更新中断 */
    TIM_Cmd(TIM6, ENABLE);                                               /* 启动 TIM6 计数器 */
}


/**
 * @brief  初始化 TIM2 为 ADC 采样触发定时器
 * @param  sample_hz  期望的 ADC 采样频率（Hz），合法范围 1~10000
 * @note   TIM2 通过 TRGO 输出更新事件，触发 ADC1/ADC2 的双 ADC 同步规则采样。
 *         内部自动限幅：sample_hz = 0 时取默认值 100Hz，> 10000 时钳位到 10000Hz。
 *
 *         频率计算：
 *         - TIM2 时钟：84MHz（APB1 定时器时钟）
 *         - 预分频：84-1 → 84MHz / 84 = 1MHz（1us 分辨率）
 *         - 周期：period = 1000000 / sample_hz
 *         - 采样频率 = 1MHz / period
 *
 *         示例：
 *         sample_hz=100  → period=10000 → Fs=1MHz/10000=100Hz
 *         sample_hz=1000 → period=1000  → Fs=1MHz/1000=1000Hz
 */
void TIM2_ADC_Trigger_Init(uint32_t sample_hz)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;   /* 定时器时基结构体 */
    uint32_t period;                                   /* 计算得到的自动重载值 */

    /* 参数合法性检查：采样频率不能为 0 */
    if (sample_hz == 0U)
    {
        sample_hz = 100U;                              /* 默认采样频率 100Hz */
    }

    /* 参数合法性检查：采样频率上限限制，防止过高的采样率 */
    if (sample_hz > 10000U)
    {
        sample_hz = 10000U;                            /* 钳位到最大 10kHz */
    }

    /* ========== 步骤1：使能 TIM2 时钟并复位 ========== */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   /* 使能 TIM2 外设时钟 */
    TIM_Cmd(TIM2, DISABLE);                                 /* 先停止定时器 */
    TIM_DeInit(TIM2);                                       /* 复位 TIM2 所有寄存器到默认值 */

    /* ========== 步骤2：根据采样频率计算自动重载值 ========== */
    period = 1000000U / sample_hz;                           /* 计算周期值：1MHz / sample_hz */
    if (period == 0U)                                        /* 防止除零或舍入导致的 0 */
    {
        period = 1U;                                         /* 最小周期值为 1，对应 1MHz 采样（理论上限） */
    }

    /* ========== 步骤3：TIM2 时基配置 ========== */
    TIM_TimeBaseInitStruct.TIM_Prescaler         = 84U - 1U;               /* 预分频：84MHz / 84 = 1MHz */
    TIM_TimeBaseInitStruct.TIM_CounterMode       = TIM_CounterMode_Up;     /* 向上计数模式 */
    TIM_TimeBaseInitStruct.TIM_Period            = period - 1U;            /* 自动重载值 */
    TIM_TimeBaseInitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;           /* 时钟不分频 */
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0U;                     /* 重复计数器不使用 */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);                       /* 将时基配置写入 TIM2 */

    /* ========== 步骤4：配置 TRGO 输出并启动 ========== */
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);   /* 选择 TRGO 信号源为更新事件（溢出时输出） */
    TIM_Cmd(TIM2, ENABLE);                                   /* 启动 TIM2 */
}


