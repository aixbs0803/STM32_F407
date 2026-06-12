/**
 * @file    key.h
 * @brief   按键扫描模块头文件
 * @details 提供按键初始化、扫描和读取接口。
 *          采用状态机消抖方案：连续多次采样相同时才确认按键状态变化，
 *          有效滤除机械触点抖动，消抖阈值由 KEY_Scan 中的计数值控制。
 *          支持 2 个独立按键，按键读取后自动清零（一次性消费语义）。
 */

#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 GPIO 和 RCC 相关类型定义 */
#include <stdint.h>      /* 标准整型头文件，提供 uint8_t / uint16_t 等类型 */

/*==================== 硬件配置宏 ====================*/

#define KEY1_PORT      GPIOA               /**< 按键1 所在 GPIO 端口：PORTA */
#define KEY1_PIN       GPIO_Pin_0          /**< 按键1 引脚：PA0 */
#define KEY1_CLK       RCC_AHB1Periph_GPIOA  /**< 按键1 端口时钟 */

#define KEY2_PORT      GPIOC               /**< 按键2 所在 GPIO 端口：PORTC */
#define KEY2_PIN       GPIO_Pin_13         /**< 按键2 引脚：PC13 */
#define KEY2_CLK       RCC_AHB1Periph_GPIOC  /**< 按键2 端口时钟 */

/*==================== 对外接口 ====================*/

void    KEY_Init(void);     /**< 初始化按键 GPIO 为浮空输入模式 */
void    KEY_Scan(void);     /**< 按键扫描与消抖处理，应在定时中断中周期性调用（每 1ms） */
uint8_t key_get(void);     /**< 获取最近一次按下的按键编号，读取后自动清零，返回 0 表示无按键 */

#endif /* __KEY_H__ */
