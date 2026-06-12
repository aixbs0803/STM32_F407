/**
 * @file    interrupt_init.h
 * @brief   NVIC 中断初始化模块头文件
 * @details 提供外设中断的 NVIC 优先级配置接口。
 *          当前仅包含 USART1 接收中断的初始化。
 *          如需添加新的中断初始化，在此文件中声明对应的初始化函数。
 */

#ifndef __INTERRUPT_INIT_H__
#define __INTERRUPT_INIT_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 NVIC_InitTypeDef 等类型定义 */

/**
 * @brief  初始化 USART1 的 NVIC 中断配置
 * @note   抢占优先级 1，子优先级 3。
 *         在 usart1_init() 中调用，必须在使能 USART1 中断之前配置。
 */
void USART1_Interrupt_Init(void);

#endif /* __INTERRUPT_INIT_H__ */
