/**
 * @file    adc.c
 * @brief   ADC 数据采集模块实现
 * @details 实现 STM32F4 双 ADC 同步规则采样，包含 GPIO、DMA 和 ADC 三重配置。
 *
 *          采集架构：
 *          - 触发源：TIM2 TRGO 更新事件（可配置频率，默认 100Hz）
 *          - ADC 模式：双 ADC 同步规则同时模式（ADC_DualMode_RegSimult）
 *          - DMA 模式：DMA2_Stream0，循环模式，双缓冲（2 个 32 位字）
 *          - 采样通道：ADC1 采集 CH8+CH6，ADC2 采集 CH9+CH9
 *
 *          通道映射：
 *          - PB0 (ADC1_IN8)  -> adc_value[0]
 *          - PA6 (ADC1_IN6)  -> adc_value[0]（与 CH8 平均）
 *          - PB1 (ADC2_IN9)  -> adc_value[1]（双通道采样，取平均）
 */

#include "adc.h"   /* 包含本模块头文件，获取全局变量声明和函数声明 */
#include "tim.h"   /* 包含定时器模块头文件，用于调用 TIM2_ADC_Trigger_Init */


/*==================== 全局变量 ====================*/

/**
 * @brief ADC 双通道采集结果缓冲区
 * @note  由 DMA2_Stream0 循环填充：
 *        - adc_value[0]：ADC1 的 DR 寄存器低 16 位（CH8 和 CH6 的平均值）
 *        - adc_value[1]：ADC2 的 DR 寄存器高 16 位（CH9 双采样的平均值）
 *        数据由 DMA 自动更新，无需 CPU 干预。
 */
uint32_t adc_value[2] = {0};


/**
 * @brief  初始化 ADC 模拟输入 GPIO
 * @note   将 PB0、PA6、PB1 配置为模拟输入模式（AIN）。
 *         模拟输入模式下 GPIO 输出驱动器和内部上下拉均关闭，
 *         引脚直接连接到 ADC 模块。
 *
 *         引脚分配：
 *         - PB0：ADC1 通道 8
 *         - PA6：ADC1 通道 6
 *         - PB1：ADC2 通道 9（被配置两次，对应两个规则通道序列号）
 */
static void ADC_Logger_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;   /* GPIO 初始化结构体 */

    /* 使能 GPIOB 和 GPIOA 端口时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE);

    /* 配置通用模拟输入参数 */
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AIN;      /* 模拟输入模式，断开数字电路 */
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;   /* 无上拉/下拉（模拟模式需高阻态） */
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;     /* 速度参数（模拟模式无实际影响） */
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;       /* 输出类型（模拟模式无实际影响） */

    /* 配置 PB0（ADC1_IN8）和 PB1（ADC2_IN9）为模拟输入 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;   /* 选中 PB0 和 PB1 */
    GPIO_Init(GPIOB, &GPIO_InitStruct);                    /* 应用配置到 GPIOB */

    /* 配置 PA6（ADC1_IN6）为模拟输入 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;                 /* 选中 PA6 */
    GPIO_Init(GPIOA, &GPIO_InitStruct);                    /* 应用配置到 GPIOA */
}


/**
 * @brief  初始化 DMA2 流0 用于 ADC 数据循环传输
 * @note   配置 DMA2_Stream0 将 ADC 双路规则数据寄存器（ADC_CDR）的值循环传输到 adc_value[2] 数组。
 *
 *         DMA 源地址：ADC1_BASE + 0x300 + 0x08 = ADC 通用数据寄存器（ADC_CDR）
 *         该寄存器为 32 位，高 16 位为 ADC2 数据，低 16 位为 ADC1 数据。
 *
 *         DMA 配置要点：
 *         - 通道 0：ADC1 专用 DMA 通道
 *         - 循环模式：持续传输，无需 CPU 重启 DMA
 *         - 字传输：外设和存储器数据宽度均为 Word（32 位）
 *         - 高优先级：确保采集数据不丢失
 *
 *         初始化前先停止并复位 DMA 流，确保配置从已知状态开始。
 */
static void ADC_Logger_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStruct;                       /* DMA 初始化结构体 */

    /* ========== 步骤1：使能 DMA2 时钟 ========== */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  /* 使能 DMA2 外设时钟 */

    /* ========== 步骤2：停止并复位 DMA 流 ========== */
    DMA_Cmd(DMA2_Stream0, DISABLE);                        /* 先关闭 DMA2_Stream0 */
    while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE)      /* 等待 DMA 流完全停止 */
    {
    }
    DMA_DeInit(DMA2_Stream0);                              /* 复位 DMA2_Stream0 所有寄存器 */

    /* ========== 步骤3：配置 DMA 传输参数 ========== */
    DMA_InitStruct.DMA_Channel            = DMA_Channel_0;                                    /* DMA 通道 0（ADC1 专用） */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(ADC1_BASE + 0x300U + 0x08U);           /* 外设地址：ADC_CDR（0x40012308） */
    DMA_InitStruct.DMA_Memory0BaseAddr    = (uint32_t)adc_value;                              /* 存储器地址：adc_value 数组首地址 */
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralToMemory;                       /* 传输方向：外设 → 存储器 */
    DMA_InitStruct.DMA_BufferSize         = 2;                                                /* 缓冲区大小：2 个数据单元（每个 32 位） */
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;                        /* 外设地址不递增（始终读 ADC_CDR） */
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;                             /* 存储器地址递增（adc_value[0] → [1]） */
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;                      /* 外设数据宽度：32 位 */
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;                          /* 存储器数据宽度：32 位 */
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Circular;                                /* 循环模式：传输完自动从头开始 */
    DMA_InitStruct.DMA_Priority           = DMA_Priority_High;                                /* DMA 优先级：高 */
    DMA_InitStruct.DMA_FIFOMode           = DMA_FIFOMode_Disable;                             /* FIFO 模式：禁用（直接模式） */
    DMA_InitStruct.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull;                   /* FIFO 阈值：1/4 满（FIFO 禁用时无影响） */
    DMA_InitStruct.DMA_MemoryBurst        = DMA_MemoryBurst_INC4;                             /* 存储器突发传输：4 次增量突发 */
    DMA_InitStruct.DMA_PeripheralBurst    = DMA_PeripheralBurst_INC4;                         /* 外设突发传输：4 次增量突发 */

    DMA_Init(DMA2_Stream0, &DMA_InitStruct);               /* 将配置写入 DMA2_Stream0 寄存器 */

    /* ========== 步骤4：清除标志并使能 DMA ========== */
    DMA_ClearFlag(DMA2_Stream0,                            /* 清除所有 DMA 中断标志，避免上电误触发 */
                  DMA_FLAG_TCIF0  |                        /* 传输完成标志 */
                  DMA_FLAG_HTIF0  |                        /* 半传输完成标志 */
                  DMA_FLAG_TEIF0  |                        /* 传输错误标志 */
                  DMA_FLAG_DMEIF0 |                        /* 直接模式错误标志 */
                  DMA_FLAG_FEIF0);                         /* FIFO 错误标志 */

    DMA_Cmd(DMA2_Stream0, ENABLE);                         /* 使能 DMA2_Stream0，等待 ADC 触发 */
}


/**
 * @brief  初始化 ADC1 和 ADC2 为双 ADC 同步规则采样模式
 * @note   ADC1 作为主 ADC，ADC2 作为从 ADC，两者由同一个 TRGO 信号同步触发。
 *
 *         双 ADC 同步规则同时模式（ADC_DualMode_RegSimult）：
 *         每次触发同时启动两个 ADC 的规则通道序列，转换结果合并到
 *         一个 32 位寄存器 ADC_CDR 中（高 16 位=ADC2，低 16 位=ADC1），
 *         由 DMA 一次性传输。
 *
 *         通道配置：
 *         - ADC1 规则序列：CH8（SQ1）→ CH6（SQ2），共 2 个通道，自动平均
 *         - ADC2 规则序列：CH9（SQ1）→ CH9（SQ2），共 2 个通道，自动平均
 *
 *         触发配置：
 *         - 触发源：TIM2 TRGO（定时器更新事件）
 *         - 触发边沿：上升沿
 *
 *         ADC 时钟：84MHz / 4 = 21MHz（APB2 预分频 4）
 */
static void ADC_Logger_ADC_Init(void)
{
    ADC_CommonInitTypeDef ADC_CommonInitStruct;   /* ADC 通用初始化结构体（多 ADC 模式配置） */
    ADC_InitTypeDef       ADC_InitStruct;         /* ADC 独立初始化结构体（单 ADC 参数配置） */

    /* ========== 步骤1：使能 ADC 时钟 ========== */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);   /* 使能 ADC1 和 ADC2 外设时钟 */

    /* ========== 步骤2：配置 ADC 通用参数（多 ADC 模式） ========== */
    ADC_CommonInitStruct.ADC_Mode              = ADC_DualMode_RegSimult;           /* 双 ADC 同步规则同时模式 */
    ADC_CommonInitStruct.ADC_Prescaler         = ADC_Prescaler_Div4;               /* ADC 时钟预分频：84MHz/4=21MHz（ADC 推荐<36MHz） */
    ADC_CommonInitStruct.ADC_DMAAccessMode     = ADC_DMAAccessMode_2;              /* DMA 访问模式 2：每 2 个半字数据产生一次 DMA 请求 */
    ADC_CommonInitStruct.ADC_TwoSamplingDelay  = ADC_TwoSamplingDelay_5Cycles;    /* 双 ADC 采样间隔：5 个 ADCCLK 周期 */
    ADC_CommonInit(&ADC_CommonInitStruct);                                       /* 将通用配置写入 ADC_CCR 寄存器 */

    /* ========== 步骤3：配置 ADC1 和 ADC2 独立参数 ========== */
    ADC_InitStruct.ADC_Resolution          = ADC_Resolution_12b;                  /* ADC 分辨率：12 位（0~4095） */
    ADC_InitStruct.ADC_ScanConvMode        = ENABLE;                              /* 扫描模式：使能（多通道自动扫描） */
    ADC_InitStruct.ADC_ContinuousConvMode  = DISABLE;                             /* 连续转换模式：禁用（由定时器触发单次转换） */
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;   /* 外部触发边沿：上升沿 */
    ADC_InitStruct.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_T2_TRGO;        /* 外部触发源：TIM2 TRGO 输出 */
    ADC_InitStruct.ADC_DataAlign           = ADC_DataAlign_Right;                 /* 数据对齐：右对齐 */
    ADC_InitStruct.ADC_NbrOfConversion     = 2;                                   /* 规则通道数量：2 个通道 */

    ADC_Init(ADC1, &ADC_InitStruct);         /* 将独立参数配置写入 ADC1 */
    ADC_Init(ADC2, &ADC_InitStruct);         /* 将独立参数配置写入 ADC2（与 ADC1 相同） */

    /* ========== 步骤4：配置规则通道序列 ========== */
    /* ADC1 规则序列：SQ1=CH8(PB0), SQ2=CH6(PA6)，采样时间 15 个 ADCCLK 周期 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_15Cycles);   /* ADC1 规则序列第 1 个：通道 8 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_15Cycles);   /* ADC1 规则序列第 2 个：通道 6 */

    /* ADC2 规则序列：SQ1=CH9(PB1), SQ2=CH9(PB1)，通道 9 采样两次取平均 */
    ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 1, ADC_SampleTime_15Cycles);   /* ADC2 规则序列第 1 个：通道 9 */
    ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 2, ADC_SampleTime_15Cycles);   /* ADC2 规则序列第 2 个：通道 9（同通道双采样） */

    /* ========== 步骤5：使能 DMA 和多模式传输 ========== */
    ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);  /* 使能多模式 DMA 请求：最后一次传输后产生 DMA 请求 */
    ADC_DMACmd(ADC1, ENABLE);                              /* 使能 ADC1 的 DMA 请求（主 ADC 控制 DMA） */

    /* ========== 步骤6：使能 ADC ========== */
    ADC_Cmd(ADC1, ENABLE);                                 /* 使能 ADC1（主 ADC 必须先启动） */
    ADC_Cmd(ADC2, ENABLE);                                 /* 使能 ADC2（从 ADC 后启动） */
}


/**
 * @brief  初始化 ADC 数据采集系统的顶层函数
 * @note   按顺序调用各子模块初始化函数，确保配置顺序正确：
 *         1. GPIO 模拟输入初始化（配置引脚为模拟模式）
 *         2. DMA 循环传输初始化（配置数据搬运通道）
 *         3. TIM2 触发定时器初始化（100Hz 采样频率）
 *         4. ADC 外设初始化（配置双 ADC 同步采样模式）
 *
 *         初始化顺序重要：DMA 必须在 ADC 之前配置，TIM2 必须在 ADC 触发配置前启动。
 */
void ADC_Logger_Init(void)
{
    ADC_Logger_GPIO_Init();              /* 步骤1：初始化模拟输入 GPIO 引脚 */
    ADC_Logger_DMA_Init();               /* 步骤2：初始化 DMA2 循环传输通道 */
    TIM2_ADC_Trigger_Init(100U);         /* 步骤3：初始化 TIM2 触发定时器（默认 100Hz 采样率） */
    ADC_Logger_ADC_Init();               /* 步骤4：初始化 ADC1/ADC2 双 ADC 同步采样模式 */
}
