/**
 * @file    led.c
 * @brief   LED 控制模块实现
 * @details 基于 STM32F4 GPIO 控制板载 LED。
 *          硬件配置通过宏和查表方式实现，修改 LED 连接的引脚时只需修改硬件配置区的宏定义即可。
 *          支持低电平点亮或高电平点亮两种接法，通过 LED_ON / LED_OFF 宏统一管理。
 */

#include "led.h"   /* 包含本模块头文件，获取枚举定义和函数声明 */


/*==================== 硬件配置区 ====================*/
/* 说明：以下宏定义根据实际硬件电路配置，更换 LED 引脚时只需修改此区域 */

/* 时钟定义 —— 所有 LED 共用同一个 GPIO 端口时钟 */
#define LED_GPIO_CLK                RCC_AHB1Periph_GPIOF    /**< GPIOF 端口时钟 */

/* GPIO 端口定义 —— 每个 LED 对应的 GPIO 端口 */
#define LED1_GPIO_PORT              GPIOF                    /**< LED1 所在端口 */
#define LED1_GPIO_PIN               GPIO_Pin_6               /**< LED1 引脚：PF6 */

#define LED2_GPIO_PORT              GPIOF                    /**< LED2 所在端口 */
#define LED2_GPIO_PIN               GPIO_Pin_7               /**< LED2 引脚：PF7 */

#define LED3_GPIO_PORT              GPIOF                    /**< LED3 所在端口 */
#define LED3_GPIO_PIN               GPIO_Pin_8               /**< LED3 引脚：PF8 */

/*
 * LED 电平定义
 * 本硬件 LED 为低电平点亮，高电平熄灭：
 *   - LED_ON  = Bit_RESET（输出低电平，LED 点亮）
 *   - LED_OFF = Bit_SET  （输出高电平，LED 熄灭）
 *
 * 若硬件改为高电平点亮，只需将 LED_ON 改为 Bit_SET，LED_OFF 改为 Bit_RESET 即可
 */
#define LED_ON                Bit_RESET                     /**< LED 点亮时 GPIO 输出的电平（低电平有效） */
#define LED_OFF               Bit_SET                       /**< LED 熄灭时 GPIO 输出的电平（高电平） */


/*==================== 内部数据结构 ====================*/

/**
 * @brief LED 硬件映射结构体
 * @note  将每个 LED 的逻辑编号与实际 GPIO 端口、引脚关联
 */
typedef struct
{
    GPIO_TypeDef *port;     /**< 该 LED 对应的 GPIO 端口基地址 */
    uint16_t      pin;      /**< 该 LED 对应的 GPIO 引脚号 */
} led_hw_t;

/**
 * @brief LED 硬件映射表
 * @note  数组索引与 led_id_t 枚举值一一对应。
 *        新增 LED 时在此数组中追加对应条目即可。
 */
static const led_hw_t g_led_list[LED_COUNT] =
{
    {LED1_GPIO_PORT, LED1_GPIO_PIN},   /**< LED1 -> PF6 */
    {LED2_GPIO_PORT, LED2_GPIO_PIN},   /**< LED2 -> PF7 */
    {LED3_GPIO_PORT, LED3_GPIO_PIN}    /**< LED3 -> PF8 */
};


/*==================== 内部辅助函数 ====================*/

/**
 * @brief  校验 LED 编号是否有效
 * @param  led  LED 编号
 * @return int  1=有效，0=无效（编号超出范围）
 * @note   用于防止数组越界访问
 */
static int LED_IsValid(led_id_t led)
{
    return (led < LED_COUNT);          /* 编号必须小于 LED 总数，否则无效 */
}


/*==================== 对外接口实现 ====================*/

/**
 * @brief  初始化所有 LED 的 GPIO 引脚
 * @note   执行流程：
 *         1. 使能 GPIO 时钟
 *         2. 先将所有 LED 引脚设为安全电平（熄灭）
 *         3. 配置 GPIO 为推挽输出模式
 *         4. 再次确保 LED 全部熄灭
 */
void LED_Init(void)
{
    
    /* 步骤1：使能 GPIOF 端口时钟，为后续 GPIO 操作提供时钟源 */
    RCC_AHB1PeriphClockCmd(LED_GPIO_CLK, ENABLE);

    /* 步骤2：上电初始将所有 LED 引脚置为高电平（熄灭），避免上电瞬间闪烁 */
    GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);   /* LED1 熄灭：PF6 输出高 */
    GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);   /* LED2 熄灭：PF7 输出高 */
    GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);   /* LED3 熄灭：PF8 输出高 */

    /* 步骤3：逐一配置每个 LED 引脚为推挽输出模式 */
    GPIO_InitTypeDef GPIO_InitStruct;  /* GPIO 初始化结构体变量 */
    GPIO_InitStruct.GPIO_Pin   = LED1_GPIO_PIN;    /* 选中 LED1 引脚 PF6 */
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;    /* 设为通用输出模式 */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  /* 输出速率 2MHz（LED 控制无需高速） */
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    /* 推挽输出类型 */
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL; /* 无上拉/下拉 */
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);   /* 将配置写入 LED1 所在的 GPIOF */

    GPIO_InitStruct.GPIO_Pin   = LED2_GPIO_PIN;    /* 选中 LED2 引脚 PF7，其余配置不变 */
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);   /* 将配置写入 LED2 所在的 GPIOF */

    GPIO_InitStruct.GPIO_Pin   = LED3_GPIO_PIN;    /* 选中 LED3 引脚 PF8，其余配置不变 */
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);   /* 将配置写入 LED3 所在的 GPIOF */

    /* 步骤4：通过统一接口再次确保所有 LED 默认熄灭 */
    LED_Off(LED1);                                 /* 熄灭 LED1 */
    LED_Off(LED2);                                 /* 熄灭 LED2 */
    LED_Off(LED3);                                 /* 熄灭 LED3 */
}

/**
 * @brief  点亮指定 LED
 * @param  led  LED 编号（led_id_t 枚举值）
 * @note   根据 LED_ON 宏的定义输出对应电平。
 *         低电平点亮模式下输出 Bit_RESET，高电平点亮模式下输出 Bit_SET。
 *         先校验编号有效性，无效编号不做任何操作。
 */
void LED_On(led_id_t led)
{
    if (LED_IsValid(led) == 0)            /* 校验 LED 编号是否有效 */
    {
        return;                           /* 无效编号，直接返回 */
    }

    if (LED_ON == Bit_SET)                /* 若 LED 为高电平点亮 */
    {
        GPIO_SetBits(g_led_list[led].port, g_led_list[led].pin);   /* 输出高电平点亮 LED */
    }
    else                                  /* 若 LED 为低电平点亮（本硬件采用此方式） */
    {
        GPIO_ResetBits(g_led_list[led].port, g_led_list[led].pin); /* 输出低电平点亮 LED */
    }
}

/**
 * @brief  熄灭指定 LED
 * @param  led  LED 编号（led_id_t 枚举值）
 * @note   逻辑与 LED_On 相反，根据 LED_OFF 宏的定义输出对应电平。
 *         先校验编号有效性。
 */
void LED_Off(led_id_t led)
{
    if (LED_IsValid(led) == 0)            /* 校验 LED 编号是否有效 */
    {
        return;                           /* 无效编号，直接返回 */
    }

    if (LED_OFF == Bit_SET)               /* 若 LED 熄灭时需要高电平 */
    {
        GPIO_SetBits(g_led_list[led].port, g_led_list[led].pin);   /* 输出高电平熄灭 LED */
    }
    else                                  /* 若 LED 熄灭时需要低电平 */
    {
        GPIO_ResetBits(g_led_list[led].port, g_led_list[led].pin); /* 输出低电平熄灭 LED */
    }
}

/**
 * @brief  翻转指定 LED 的亮灭状态
 * @param  led  LED 编号（led_id_t 枚举值）
 * @note   通过对 GPIO ODR 寄存器对应位进行异或操作实现翻转。
 *         先校验编号有效性。
 */
void LED_Toggle(led_id_t led)
{
    if (!LED_IsValid(led))               /* 校验 LED 编号是否有效 */
    {
        return;                          /* 无效编号，直接返回 */
    }

    /**
     * 通过 ODR 寄存器异或操作翻转引脚电平：
     *   - 若当前输出高（LED 灭），异或后变低（LED 亮）
     *   - 若当前输出低（LED 亮），异或后变高（LED 灭）
     */
    g_led_list[led].port->ODR ^= g_led_list[led].pin;   /* 异或翻转对应引脚电平 */
}

/**
 * @brief  将指定 LED 设置为目标状态
 * @param  led    LED 编号（led_id_t 枚举值）
 * @param  state  目标状态：LED_STATE_ON 或 LED_STATE_OFF
 * @note   对 LED_On 和 LED_Off 的封装，便于以参数化的方式控制 LED。
 */
void LED_Set(led_id_t led, led_state_t state)
{
    if (!LED_IsValid(led))               /* 校验 LED 编号是否有效 */
    {
        return;                          /* 无效编号，直接返回 */
    }

    if (state == LED_STATE_ON)           /* 目标状态为点亮 */
    {
        LED_On(led);                     /* 调用点亮函数 */
    }
    else                                 /* 目标状态为熄灭 */
    {
        LED_Off(led);                    /* 调用熄灭函数 */
    }
}
