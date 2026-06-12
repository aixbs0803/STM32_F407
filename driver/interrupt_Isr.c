/**
 * @file    interrupt_Isr.c
 * @brief   中断服务函数（ISR）实现
 * @details 包含所有外设中断的服务函数实现。
 *          当前实现的中断：
 *          - USART1_IRQHandler：USART1 接收中断，每收到一个字符调用 USART1_RxIrqHandler
 *          - TIM6_DAC_IRQHandler：TIM6 更新中断（1ms），驱动按键扫描和系统滴答
 *
 *          中断优先级（NVIC_PriorityGroup_4 分组下）：
 *          - TIM6：抢占 0，子 0（最高优先级）
 *          - USART1：抢占 1，子 0
 */

#include "main.h"   /* 主程序头文件，声明 g_delay_tick 全局变量和 USART1_RxIrqHandler */


/**
 * @brief  USART1 全局中断服务函数
 * @note   处理两种中断源：
 *         - RXNE（接收非空）：读取接收到的字符，交给 USART1_RxIrqHandler 处理
 *         - TC（发送完成）：清除 TC 标志位，表示发送完成
 *
 *         中断触发条件：
 *         - RXNE：USART 数据寄存器收到新数据时置位
 *         - TC：发送移位寄存器为空且发送缓冲区为空时置位
 */
void USART1_IRQHandler(void)
{
    /* 处理接收中断（RXNE）：有数据到达 */
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)       /* 检查 RXNE 中断标志是否置位 */
    {
        char ch = (char)USART_ReceiveData(USART1);               /* 读取 USART1 数据寄存器获取接收字符 */
        USART1_RxIrqHandler(ch);                                 /* 将字符传递给接收处理函数（行组装） */
    }

    /* 处理发送完成中断（TC）：发送操作结束 */
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)         /* 检查 TC 中断标志是否置位 */
    {
        USART_ClearFlag(USART1, USART_FLAG_TC);                  /* 清除发送完成标志位 */
    }
}


/**
 * @brief  TIM6 和 DAC 全局中断服务函数
 * @note   本项目中仅使用 TIM6 更新中断（DAC 未使用，但共享中断向量）。
 *
 *         TIM6 配置为 1ms 周期定时器，每次溢出产生更新中断。
 *         中断中执行两项操作：
 *         1. KEY_Scan()：驱动按键扫描消抖（每 1ms 采样一次）
 *         2. g_delay_tick++：系统滴答递增（驱动非阻塞延时模块 Delay_ms/Delay_done_flag）
 *
 *         中断频率：1kHz（周期 1ms），确保按键消抖和延时的精度。
 */
void TIM6_DAC_IRQHandler(void)
{
    /* 确认是 TIM6 更新中断（而非 DAC 中断） */
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)      /* 检查 TIM6 更新中断标志是否置位 */
    {
        KEY_Scan();                                          /* 调用按键扫描函数（1ms 周期采样消抖） */
        g_delay_tick++;                                      /* 系统滴答计数器递增（用于非阻塞延时） */
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);          /* 清除 TIM6 更新中断标志位 */
    }
}
