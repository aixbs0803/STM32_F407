/**
 * @file    interrupt_Isr.h
 * @brief   中断服务函数（ISR）声明头文件
 * @details 包含各中断服务函数共同依赖的头文件。
 *          ISR 的实现位于 interrupt_Isr.c 中。
 *          依赖：
 *          - stm32f4xx.h：STM32F4 标准外设库
 *          - led.h：LED 控制
 *          - delay.h：非阻塞延时（g_delay_tick 声明在 main.h 中）
 *          - key.h：按键扫描
 *
 *          已实现的 ISR：
 *          - USART1_IRQHandler：USART1 接收中断
 *          - TIM6_DAC_IRQHandler：TIM6 更新中断（1ms 系统滴答）
 */

#ifndef __INTERRUPT_ISR_H__
#define __INTERRUPT_ISR_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 USART/TIM 等外设寄存器操作 */
#include "led.h"         /* LED 控制模块，ISR 中可能用于状态指示 */
#include "delay.h"       /* 延时模块，声明 g_delay_tick（在 main.h 中定义） */
#include "key.h"         /* 按键扫描模块，TIM6 ISR 中周期性调用 KEY_Scan */

#endif /* __INTERRUPT_ISR_H__ */
