/**
 * @file    led.h
 * @brief   LED 控制模块头文件
 * @details 提供板载 LED 的点亮、熄灭、翻转和状态设置接口。
 *          通过枚举定义 LED 编号和状态，便于上层代码以语义化的方式控制 LED。
 *          支持 LED_COUNT 个 LED，LED 编号从 0 开始。
 */

#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 GPIO 相关类型定义 */

/**
 * @brief LED 编号枚举
 * @note  新增 LED 时需在此枚举中添加对应编号，并在 led.c 的硬件配置表中添加端口/引脚映射
 */
typedef enum
{
    LED1 = 0,            /**< 第1个 LED，编号 0 */
    LED2,                /**< 第2个 LED，编号 1 */
    LED3,                /**< 第3个 LED，编号 2 */
    LED_COUNT            /**< LED 总数，用于数组大小定义和有效性校验（值=3） */
} led_id_t;

/**
 * @brief LED 状态枚举
 */
typedef enum
{
    LED_STATE_OFF = 0,   /**< LED 熄灭状态 */
    LED_STATE_ON         /**< LED 点亮状态 */
} led_state_t;

/* ======================== 对外接口 ======================== */

void LED_Init(void);                                          /**< 初始化所有 LED 的 GPIO，默认全部熄灭 */
void LED_On(led_id_t led);                                    /**< 点亮指定编号的 LED */
void LED_Off(led_id_t led);                                   /**< 熄灭指定编号的 LED */
void LED_Toggle(led_id_t led);                                /**< 翻转指定编号 LED 的亮灭状态 */
void LED_Set(led_id_t led, led_state_t state);               /**< 将指定编号 LED 设置为指定状态（亮/灭） */

#endif /* __LED_H__ */
