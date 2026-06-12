/**
 * @file    delay.h
 * @brief   非阻塞延时模块头文件
 * @details 基于系统滴答计数的非阻塞延时机制，通过 Delay_struct 结构体管理延时状态，
 *          避免使用阻塞式延时导致主循环被挂起。
 *          依赖 main.h 中声明的全局变量 g_delay_tick（由定时器中断驱动递增）。
 */

#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 uint32_t 等类型定义 */
#include "main.h"        /* 主程序头文件，声明全局变量 g_delay_tick */

/**
 * @brief 非阻塞延时控制结构体
 * @note  使用时先调用 Delay_ms 设置延时参数，然后循环调用 Delay_done_flag 检测是否到期。
 *        同一个结构体不可被多个延时任务并发使用。
 */
typedef struct
{
    uint32_t start_time;     /**< 延时起始时刻，记录 Delay_ms 调用时的 g_delay_tick 值 */
    uint32_t delay_time;     /**< 延时时长，单位为毫秒（1ms 对应 1 个 g_delay_tick 计数） */
    uint8_t  time_done_flag; /**< 延时完成标志：0=未完成，1=已完成（防止重复判定） */
} Delay_struct;

/**
 * @brief  启动一次非阻塞延时
 * @param  delay  延时结构体指针，用于记录本次延时的起始时间和目标时长
 * @param  ms     延时时长，单位毫秒
 * @note   本函数仅设置参数，不阻塞程序执行，需配合 Delay_done_flag 轮询使用
 */
void Delay_ms(Delay_struct *delay, uint32_t ms);

/**
 * @brief  检测非阻塞延时是否完成
 * @param  delay  延时结构体指针
 * @return uint8_t  0=延时未完成，1=延时已完成（或参数无效）
 * @note   该函数应放在主循环或定时中断中周期性调用
 */
uint8_t Delay_done_flag(Delay_struct *delay);

#endif /* __DELAY_H__ */
