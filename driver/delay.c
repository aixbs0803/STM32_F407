/**
 * @file    delay.c
 * @brief   非阻塞延时模块实现
 * @details 基于全局滴答计数器 g_delay_tick 实现非阻塞延时。
 *          g_delay_tick 由 TIM6 中断服务程序每 1ms 递增一次（参见 interrupt_Isr.c）。
 *          将延时起始时刻与当前时刻做差值比较，当差值 >= 目标延时时长时判定完成。
 */

#include "delay.h"   /* 包含本模块头文件，获取结构体定义和函数声明 */

/**
 * @brief  启动一次非阻塞延时
 * @param  delay  延时结构体指针，用于存储本次延时的状态信息
 * @param  ms     目标延时时长，单位毫秒（与 g_delay_tick 步进周期一致）
 * @note   仅初始化延时参数，函数立即返回，不阻塞程序运行。
 *         调用后应周期性调用 Delay_done_flag 检查延时是否完成。
 */
void Delay_ms(Delay_struct *delay, uint32_t ms)
{
    delay->start_time = g_delay_tick;    /* 记录当前系统滴答值作为延时起始时刻 */
    delay->delay_time = ms;              /* 设置目标延时时长（毫秒） */
    delay->time_done_flag = 0;           /* 清除完成标志，表示延时尚未完成 */
}

/**
 * @brief  检测非阻塞延时是否完成
 * @param  delay  延时结构体指针，包含起始时间、目标时长和完成标志
 * @return uint8_t  1=延时已完成（或参数无效），0=延时进行中
 * @note   通过计算当前滴答值与起始值的差值来判断是否达到目标时长。
 *         一旦完成，设置 time_done_flag 防止重复判定为完成。
 */
uint8_t Delay_done_flag(Delay_struct *delay)
{
    uint32_t now_time = g_delay_tick;    /* 获取当前系统滴答值，用于计算时间差 */

    /* 参数有效性检查：空指针或已标记完成则直接返回完成状态 */
    if ((delay == NULL) || (delay->time_done_flag == 1U))
    {
        return 1U;                       /* 返回 1 表示"已完成"或"无需等待" */
    }

    /* 计算时间差：当前时刻减去起始时刻，判断是否达到或超过目标时长 */
    if ((now_time - delay->start_time) >= delay->delay_time)
    {
        delay->time_done_flag = 1U;      /* 标记延时完成，防止下次调用再次判定 */
        return 1U;                       /* 返回 1 表示延时已完成 */
    }

    return 0U;                           /* 延时尚未完成，返回 0 */
}
