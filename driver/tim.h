/**
 * @file    tim.h
 * @brief   定时器配置模块头文件
 * @details 提供两个定时器的初始化接口：
 *          - TIM6：1ms 基础定时器，用于系统滴答（驱动 Delay 模块和按键扫描）
 *          - TIM2：ADC 触发定时器，用于产生可配置频率的 TRGO 信号触发 ADC 采样
 */

#ifndef __TIM_H__
#define __TIM_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 TIM 和 NVIC 相关类型定义 */

/**
 * @brief  初始化 TIM6 为 1ms 周期定时器
 * @note   配置完成后 TIM6 每 1ms 产生一次更新中断，在 TIM6_DAC_IRQHandler 中
 *         驱动 g_delay_tick 递增并调用 KEY_Scan。
 *         预分频：8400-1，周期：10-1 → 84MHz/8400/10 = 1kHz（1ms周期）
 */
void TIM6_Init(void);

/**
 * @brief  初始化 TIM2 为 ADC 触发定时器
 * @param  sample_hz  ADC 采样频率（Hz），范围 1~10000
 * @note   TIM2 产生 TRGO 更新事件触发 ADC1/ADC2 双 ADC 同步采样。
 *         预分频固定 84-1，周期根据 sample_hz 计算：
 *         period = 1000000 / sample_hz（TIM2 时钟 1MHz 即 1us 分辨率）。
 *         采样频率 = 1MHz / (period + 1)。
 */
void TIM2_ADC_Trigger_Init(uint32_t sample_hz);

#endif /* __TIM_H__ */
