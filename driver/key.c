/**
 * @file    key.c
 * @brief   按键扫描模块实现
 * @details 采用计数式软件消抖方案：
 *          1. 每次调用 KEY_Scan 时读取 GPIO 引脚电平
 *          2. 若当前电平与上次记录的电平相同，计数器递增
 *          3. 若计数器达到消抖阈值（10 次），确认按键状态稳定，更新状态
 *          4. 若当前电平与上次不同，重置计数器并更新参考值
 *          5. 上层通过 key_get 获取按键事件，读取后自动清零（消费语义）
 */

#include "key.h"   /* 包含本模块头文件，获取宏定义和函数声明 */


/*==================== 全局变量（按键状态管理） ====================*/

uint16_t key_last[2]  = {0};      /**< 上一次按键电平状态：
                                        bit0=按键1上一次电平，bit1=按键2上一次电平
                                        用于检测电平变化 */
uint16_t key_state[2] = {0};      /**< 稳定后的按键状态：
                                        经过消抖确认后的当前按键电平状态 */
uint16_t key_cnt[2]   = {0};      /**< 按键消抖计数器：
                                        连续相同电平的采样次数，达到阈值(10)后确认稳定 */
uint8_t  key_pos      = 0;        /**< 按键事件记录：
                                        0=无按键，1=检测到按键1，2=检测到按键2
                                        被 key_get 读取后自动清零 */


/**
 * @brief  初始化按键 GPIO
 * @note   将两个按键引脚配置为浮空输入模式（无上下拉），
 *         依赖外部硬件电路提供默认电平。
 *         使能相应端口时钟。
 */
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;   /* GPIO 初始化结构体变量 */

    /* 使能按键1和按键2所在端口（GPIOA 和 GPIOC）的时钟 */
    RCC_AHB1PeriphClockCmd(KEY1_CLK | KEY2_CLK, ENABLE);

    /* 配置通用 GPIO 参数：输入模式、2MHz 速度（输入模式速率无实际影响） */
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;      /* 输入模式 */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;   /* GPIO 速度（输入模式下无实际意义） */
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;     /* 输出类型（输入模式下无意义，保持默认） */
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;  /* 无内部上拉/下拉，依赖外部电路 */

    /* 初始化按键1：PA0 */
    GPIO_InitStruct.GPIO_Pin = KEY1_PIN;            /* 选中按键1引脚 */
    GPIO_Init(KEY1_PORT, &GPIO_InitStruct);         /* 应用配置到 GPIOA */

    /* 初始化按键2：PC13 */
    GPIO_InitStruct.GPIO_Pin = KEY2_PIN;            /* 选中按键2引脚 */
    GPIO_Init(KEY2_PORT, &GPIO_InitStruct);         /* 应用配置到 GPIOC */
}

/**
 * @brief  按键扫描与消抖处理
 * @note   应在定时中断（如 TIM6 每 1ms）中调用，确保稳定的采样周期。
 *         消抖原理：
 *         - 连续 10 次读到相同电平时，认为按键状态稳定，更新 key_state
 *         - 电平发生变化时，重置计数器重新消抖
 *         - 仅当按键从稳定高电平变为稳定低电平时记录按键事件（按下检测）
 *
 *         状态变量说明：
 *         - key_last[n]：上一次读取的原始电平
 *         - key_cnt[n]：连续相同电平计数
 *         - key_state[n]：消抖后的稳定状态
 *         - key_pos：按键事件（仅在按下时写入）
 */
void KEY_Scan(void)
{
    uint16_t val_one;                                            /* 临时变量，存储当前读取的按键电平 */

    /* ========== 按键1 扫描处理 ========== */
    val_one = GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN);        /* 读取按键1（PA0）当前引脚电平 */
    if (key_last[0] == val_one)                                  /* 当前电平与上次记录的电平相同 */
    {
        key_cnt[0]++;                                            /* 消抖计数器递增，说明电平持续稳定 */
        if (key_cnt[0] >= 10)                                    /* 连续10次采样相同，判定为稳定状态 */
        {
            key_state[0] = val_one;                              /* 更新按键1的稳定状态值 */
            if (val_one == Bit_SET) key_pos = 1;                 /* 若稳定状态为高电平（按下），记录按键1事件 */
        }
    }
    else                                                         /* 当前电平与上次不同（电平变化或抖动） */
    {
        key_cnt[0] = 0;                                          /* 重置消抖计数器，重新开始消抖 */
        key_last[0] = val_one;                                   /* 更新上一次电平记录为当前值 */
    }

    /* ========== 按键2 扫描处理 ========== */
    val_one = GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN);        /* 读取按键2（PC13）当前引脚电平 */
    if (val_one == key_last[1])                                  /* 当前电平与上次记录的电平相同 */
    {
        key_cnt[1]++;                                            /* 消抖计数器递增 */
        if (key_cnt[1] >= 10)                                    /* 连续10次采样相同，判定为稳定状态 */
        {
            key_state[1] = val_one;                              /* 更新按键2的稳定状态值 */
            if (val_one) key_pos = 2;                            /* 若稳定状态为非零（高电平/按下），记录按键2事件 */
        }
    }
    else                                                         /* 当前电平与上次不同 */
    {
        key_cnt[1] = 0;                                          /* 重置消抖计数器 */
        key_last[1] = val_one;                                   /* 更新上一次电平记录 */
    }
}

/**
 * @brief  获取并消费按键事件
 * @return uint8_t  0=无按键事件，1=按键1按下，2=按键2按下
 * @note   读取后自动清零 key_pos，实现一次性消费语义。
 *         调用此函数后 key_pos 被重置为 0，避免重复读取同一个按键事件。
 */
uint8_t key_get(void)
{
    uint8_t t = key_pos;    /* 保存当前按键事件值 */
    key_pos = 0;            /* 清除按键事件标志，实现一次性消费 */
    return t;               /* 返回按键编号 */
}
