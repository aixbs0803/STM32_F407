/**
 * @file    interrupt_init.c
 * @brief   NVIC 中断初始化模块实现
 * @details 为各外设配置 NVIC 中断优先级和使能状态。
 *          当前仅包含 USART1 中断的 NVIC 配置。
 *
 *          FreeRTOS 项目统一使用 NVIC_PriorityGroup_4：
 *          4 位全部用于抢占优先级，不使用子优先级。
 *
 *          中断优先级分配：
 *          - TIM6：抢占 0，子 0（最高，系统辅助节拍）
 *          - USART1：抢占 1，子 0（串口接收，低于系统辅助节拍）
 */

#include "interrupt_init.h"   /* 包含本模块头文件，获取函数声明 */


/**
 * @brief  初始化 USART1 NVIC 中断
 * @note   抢占优先级 1，子优先级 0。
 *         使能 USART1 全局中断通道。
 *         该函数在 usart1_init() 中被调用。
 */
void USART1_Interrupt_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;   /* NVIC 初始化结构体变量 */

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);                   /* FreeRTOS 项目统一使用 4 位抢占优先级 */
    NVIC_InitStruct.NVIC_IRQChannel                   = USART1_IRQn;   /* 选择 USART1 全局中断通道 */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;             /* 抢占优先级：1（低于 TIM6 的 0） */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;             /* PriorityGroup_4 下无子优先级 */
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;        /* 使能该中断通道 */
    NVIC_Init(&NVIC_InitStruct);                                       /* 将配置写入 NVIC 寄存器 */
}
