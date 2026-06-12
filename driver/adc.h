/**
 * @file    adc.h
 * @brief   ADC 数据采集模块头文件
 * @details 提供双 ADC（ADC1 + ADC2）同步采集功能的初始化接口。
 *          ADC1 采集通道 8（PB0）和通道 6（PA6），ADC2 采集通道 9（PB1 × 2）。
 *          采用 TIM2 TRGO 触发 + DMA 循环传输 + 双 ADC 同步规则模式，
 *          实现由定时器驱动的连续等间隔采样。
 *          采集结果存储在全局数组 adc_value[2] 中，由 DMA 自动更新。
 */

#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 ADC/DMA/GPIO 类型定义 */

/**
 * @brief ADC 采集结果数组
 * @note  adc_value[0]：ADC1 双通道采样的平均值（来自 ADC_DR 32 位寄存器的低 16 位）
 *         adc_value[1]：ADC2 双通道采样的平均值（来自 ADC_DR 32 位寄存器的高 16 位）
 *         由 DMA2_Stream0 循环传输自动更新
 */
extern uint32_t adc_value[2];

/**
 * @brief  初始化 ADC 数据采集系统
 * @note   依次调用子模块初始化函数完成完整的 ADC 采集链配置：
 *         1. ADC_Logger_GPIO_Init()：配置模拟输入引脚
 *         2. ADC_Logger_DMA_Init()：配置 DMA2 循环传输
 *         3. TIM2_ADC_Trigger_Init(100U)：配置 100Hz 触发定时器
 *         4. ADC_Logger_ADC_Init()：配置双 ADC 同步规则采样模式
 */
void ADC_Logger_Init(void);

#endif /* __ADC_H__ */
