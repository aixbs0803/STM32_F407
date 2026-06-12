/**
 * @file    usart.c
 * @brief   USART1 串口通信模块实现
 * @details 实现 USART1 全双工串口通信，包含以下功能：
 *          - 硬件初始化：GPIO 复用 + USART 外设配置（115200-8-N-1）
 *          - printf 重定向：通过 fputc 将标准输出重定向到 USART1
 *          - 中断接收：RXNE 中断驱动，字符逐字节存入行缓冲区
 *          - 命令队列：环形缓冲区存储完整命令行（最多 4 条，每条 64 字节）
 *          - 溢出统计：记录因队列满而丢弃的命令数量
 *          - 半主机禁用：通过 #pragma 和 _sys_exit 禁用 ARM 半主机模式
 *
 *          硬件连接：
 *          - PA9  -> USART1_TX（复用推挽输出）
 *          - PA10 -> USART1_RX（复用推挽输出，内部上拉）
 */

#include "usart.h"   /* 包含本模块头文件，获取宏定义和函数声明 */


/*==================== 半主机模式禁用 ====================*/
/* 以下配置禁用 ARM 半主机模式，使得 printf 等标准 I/O 函数通过
 * 自定义的 fputc 输出到 USART1，而非半主机调试通道 */

#pragma import(__use_no_semihosting)             /* 告诉链接器不使用半主机库函数 */

/**
 * @brief  标准库 FILE 结构体最小定义
 * @note   半主机禁用时需要提供一个 FILE 结构体定义来满足链接器需求
 */
struct __FILE
{
    int handle;   /* 文件句柄（未使用，仅为满足结构体定义） */
};

FILE __stdout;    /* 标准输出流对象，被 fputc 的 FILE* 参数引用 */
FILE __stdin;     /* 标准输入流对象 */


/*==================== USART1 接收缓冲区（模块级静态变量） ====================*/

/**
 * @brief 当前接收行缓冲区
 * @note  字符逐字节填充，收到 \r 或 \n 时终结并移入命令队列
 */
static char s_usart1_rx_line[USART1_CMD_MAX_LEN];

/**
 * @brief 当前行缓冲区写入位置（已写入的字符数）
 * @note  volatile 修饰：由中断服务函数写入，主循环读取
 */
static volatile uint16_t s_usart1_rx_pos = 0;

/**
 * @brief 命令环形队列存储区
 * @note  大小为 QUEUE_LEN × CMD_MAX_LEN，用于暂存已完成的命令行
 */
static char s_usart1_cmd_queue[USART1_CMD_QUEUE_LEN][USART1_CMD_MAX_LEN];

/**
 * @brief 命令队列写入索引（生产者：中断服务函数）
 * @note  volatile 修饰：中断与主循环共享
 */
static volatile uint8_t s_usart1_cmd_wr = 0;

/**
 * @brief 命令队列读取索引（消费者：主循环调用 USART1_ReadLine）
 * @note  volatile 修饰：中断与主循环共享
 */
static volatile uint8_t s_usart1_cmd_rd = 0;

/**
 * @brief 命令队列中待读取的命令数量
 * @note  volatile 修饰：生产消费计数器，非零时表示有新命令
 */
static volatile uint8_t s_usart1_cmd_count = 0;

/**
 * @brief 接收溢出计数器
 * @note  当命令队列满时仍有新命令到达，此计数递增。
 *        可用于统计采样周期内丢失的串口数据帧数。
 */
static volatile uint32_t s_usart1_rx_overflow = 0;




/*==================== 半主机支持函数 ====================*/

/**
 * @brief  程序退出处理（半主机模式禁用时必须实现）
 * @param  x  退出状态码（未使用）
 * @note   禁用半主机时链接器要求实现此函数，本实现为空，避免编译警告。
 */
void _sys_exit(int x)
{
    (void)x;   /* 抑制"未使用参数"警告 */
}

/**
 * @brief  printf 底层字符输出函数重定向
 * @param  ch  要输出的字符
 * @param  f   文件指针（未使用）
 * @return int  输出的字符（ASCII 码）
 * @note   标准库 printf 最终会调用此函数输出每个字符。
 *         本实现将字符通过 USART1_SendByte 发送到串口。
 */
int fputc(int ch, FILE *f)
{
    (void)f;                                     /* 抑制"未使用参数"警告 */
    USART1_SendByte(USART1, (char)ch);           /* 将字符通过 USART1 发送 */
    return ch;                                   /* 返回发送的字符 */
}


/*==================== 对外接口实现 ====================*/

/**
 * @brief  初始化 USART1 外设
 * @note   配置流程：
 *         1. 使能 GPIOA 和 USART1 时钟
 *         2. 配置 PA9(TX) 和 PA10(RX) 为复用推挽输出
 *         3. 映射引脚复用功能到 USART1
 *         4. 配置 USART1：115200-8-N-1，收发双工
 *         5. 配置 NVIC 中断并使能 RXNE 接收中断
 */
void usart1_init(void)
{
    GPIO_InitTypeDef  gpio_init_struct;    /* GPIO 初始化结构体 */
    USART_InitTypeDef usart_init_struct;   /* USART 初始化结构体 */

    /* ========== 步骤1：使能 GPIO 和 USART1 时钟 ========== */
    RCC_AHB1PeriphClockCmd(USART1_TX_CLK | USART1_RX_CLK, ENABLE);   /* 使能 GPIOA 时钟（TX 和 RX 共用） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);            /* 使能 USART1 外设时钟 */

    /* ========== 步骤2：配置 TX/RX GPIO 为复用功能 ========== */
    gpio_init_struct.GPIO_Pin   = USART1_TX_PIN | USART1_RX_PIN;   /* 同时选中 PA9 和 PA10 */
    gpio_init_struct.GPIO_Mode  = GPIO_Mode_AF;                     /* 复用功能模式，由 USART1 控制 */
    gpio_init_struct.GPIO_OType = GPIO_OType_PP;                    /* 推挽输出 */
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;                 /* 高速 50MHz */
    gpio_init_struct.GPIO_PuPd  = GPIO_PuPd_UP;                    /* 内部上拉，空闲时保持高电平 */
    GPIO_Init(USART1_TX_PORT, &gpio_init_struct);                    /* 将配置应用到 GPIOA */

    /* ========== 步骤3：引脚复用功能映射 ========== */
    GPIO_PinAFConfig(USART1_TX_PORT, USART1_TX_GPIO_SOURCE, GPIO_AF_USART1);   /* PA9 映射到 USART1_TX */
    GPIO_PinAFConfig(USART1_RX_PORT, USART1_RX_GPIO_SOURCE, GPIO_AF_USART1);   /* PA10 映射到 USART1_RX */

    /* ========== 步骤4：配置 USART1 通信参数 ========== */
    usart_init_struct.USART_BaudRate            = 115200;                    /* 波特率：115200 bps */
    usart_init_struct.USART_WordLength          = USART_WordLength_8b;       /* 数据位：8 位 */
    usart_init_struct.USART_StopBits            = USART_StopBits_1;          /* 停止位：1 位 */
    usart_init_struct.USART_Parity              = USART_Parity_No;           /* 校验位：无 */
    usart_init_struct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   /* 模式：收发双工 */
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  /* 硬件流控：无 */
    USART_Init(USART1, &usart_init_struct);                                 /* 将配置写入 USART1 */

    /* ========== 步骤5：配置中断并使能 USART1 ========== */
    USART1_Interrupt_Init();                            /* 调用 interrupt_init 模块配置 NVIC 中断优先级 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);      /* 使能接收缓冲区非空中断（RXNE） */
    USART_Cmd(USART1, ENABLE);                          /* 使能 USART1 外设 */
}


/**
 * @brief  阻塞发送单个字节
 * @param  USARTx  USART 外设指针（通常为 USART1）
 * @param  byte    要发送的字节
 * @note   轮询等待 TXE（发送数据寄存器空）标志置位后才写入数据。
 *         阻塞方式，发送期间占用 CPU。
 */
void USART1_SendByte(USART_TypeDef *USARTx, char byte)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)   /* 等待 TXE=1（发送缓冲区为空） */
    {
    }

    USART_SendData(USARTx, (uint8_t)byte);   /* 将字节写入 USART 数据寄存器，启动发送 */
}


/**
 * @brief  发送字符串（以 '\0' 结尾）
 * @param  USARTx  USART 外设指针
 * @param  str     要发送的字符串指针
 * @note   NULL 指针安全检查，遍历字符串每字符调用 USART1_SendByte 发送。
 */
void USART1_SendString(USART_TypeDef *USARTx, const char *str)
{
    if (str == NULL)                     /* 空指针检查 */
    {
        return;                          /* 空字符串，直接返回 */
    }

    while (*str != '\0')                 /* 遍历直到字符串结束符 '\0' */
    {
        USART1_SendByte(USARTx, *str++); /* 发送当前字符后指针后移 */
    }
}


/**
 * @brief  USART1 接收中断处理函数
 * @param  ch  从 USART1 数据寄存器读取的字符
 * @note   由 USART1_IRQHandler 调用，处理逐字节流组装为命令行。
 *
 *         处理规则：
 *         - '\r' 或 '\n'：行结束符，将当前缓冲区内容作为一条命令存入队列
 *         - '\b' 或 0x7F（DEL）：退格删除，缓冲区指针前移
 *         - 0x20~0x7E：可打印 ASCII 字符，存入缓冲区
 *         - 其他控制字符（<0x20 或 >0x7E）：丢弃
 *
 *         缓冲区管理：
 *         - 行缓冲区最大长度 CMD_MAX_LEN-1（留 1 字节给 '\0'）
 *         - 超过最大长度时丢弃整行并增加溢出计数
 */
void USART1_RxIrqHandler(char ch)
{
    uint16_t i;   /* 循环索引变量 */

    /* 情况1：行结束符 \r 或 \n → 完成当前命令 */
    if ((ch == '\r') || (ch == '\n'))
    {
        if (s_usart1_rx_pos == 0)            /* 空行（连续回车），忽略 */
        {
            return;                          /* 无数据，直接返回 */
        }

        s_usart1_rx_line[s_usart1_rx_pos] = '\0';   /* 在缓冲区末尾添加字符串结束符 */

        if (s_usart1_cmd_count < USART1_CMD_QUEUE_LEN)   /* 命令队列未满 */
        {
            /* 将行缓冲区内容复制到命令队列的写入位置 */
            for (i = 0; i <= s_usart1_rx_pos; i++)
            {
                s_usart1_cmd_queue[s_usart1_cmd_wr][i] = s_usart1_rx_line[i];   /* 逐字符复制 */
            }
            s_usart1_cmd_wr++;                                         /* 写索引前移 */
            if (s_usart1_cmd_wr >= USART1_CMD_QUEUE_LEN)              /* 写索引回绕（环形缓冲区） */
            {
                s_usart1_cmd_wr = 0;                                   /* 回绕到队列头部 */
            }
            s_usart1_cmd_count++;                                      /* 命令计数递增 */
        }
        else                                                           /* 命令队列已满 */
        {
            s_usart1_rx_overflow++;                                    /* 溢出计数递增，丢弃此命令 */
        }

        s_usart1_rx_pos = 0;                                           /* 重置行缓冲区位置，准备接收下一行 */
        return;                                                        /* 处理完毕 */
    }

    /* 情况2：退格符 \b 或 DEL（0x7F） → 删除前一个字符 */
    if ((ch == '\b') || (ch == 0x7f))
    {
        if (s_usart1_rx_pos > 0)          /* 缓冲区非空时才能退格 */
        {
            s_usart1_rx_pos--;            /* 缓冲区指针前移一位，覆盖前一个字符 */
        }
        return;                           /* 处理完毕 */
    }

    /* 情况3：非打印字符（控制字符） → 丢弃 */
    if ((ch < 0x20) || (ch > 0x7e))
    {
        return;                           /* 不可打印字符，直接丢弃 */
    }

    /* 情况4：普通可打印字符 → 存入缓冲区 */
    if (s_usart1_rx_pos < (USART1_CMD_MAX_LEN - 1U))    /* 缓冲区未满（留 1 字节给 '\0'） */
    {
        s_usart1_rx_line[s_usart1_rx_pos++] = ch;        /* 存入缓冲区，位置指针后移 */
    }
    else                                                 /* 缓冲区已满 */
    {
        s_usart1_rx_overflow++;                          /* 溢出计数递增 */
        s_usart1_rx_pos = 0;                             /* 丢弃当前行，重置缓冲区 */
    }
}


/**
 * @brief  从命令队列中读取一行命令
 * @param  line     输出缓冲区指针，用于存放读取的命令行
 * @param  max_len  输出缓冲区的最大长度（含 '\0'）
 * @return uint8_t  1=成功读取到命令，0=队列为空或参数无效
 * @note   消费者函数，由主循环调用。
 *         操作队列时临禁中断以保证原子性。
 *         最多读取 max_len-1 个字符，最后强制添加 '\0'。
 */
uint8_t USART1_ReadLine(char *line, uint16_t max_len)
{
    uint16_t i = 0;        /* 字符复制索引 */
    uint8_t  rd;           /* 当前读取索引的临时副本 */

    /* 参数有效性检查 */
    if ((line == NULL) || (max_len == 0U))
    {
        return 0;          /* 空指针或零长度，返回失败 */
    }

    /* 临界区：临时关中断，保证队列操作的原子性 */
    __disable_irq();                               /* 关闭全局中断 */

    if (s_usart1_cmd_count == 0)                   /* 命令队列为空 */
    {
        __enable_irq();                            /* 恢复中断 */
        line[0] = '\0';                            /* 设置空字符串 */
        return 0;                                  /* 返回无命令 */
    }

    rd = s_usart1_cmd_rd;                          /* 获取当前读索引的副本 */
    while ((i + 1U) < max_len)                     /* 最多复制 max_len-1 个字符（留 '\0' 空间） */
    {
        line[i] = s_usart1_cmd_queue[rd][i];       /* 从队列中复制一个字符 */
        if (line[i] == '\0')                        /* 遇到字符串结束符 */
        {
            break;                                 /* 复制完毕，退出循环 */
        }
        i++;                                       /* 索引递增 */
    }
    line[max_len - 1U] = '\0';                     /* 强制在缓冲区末尾添加结束符，防止溢出 */

    /* 更新读取指针和计数 */
    s_usart1_cmd_rd++;                              /* 读索引前移 */
    if (s_usart1_cmd_rd >= USART1_CMD_QUEUE_LEN)   /* 读索引回绕 */
    {
        s_usart1_cmd_rd = 0;                       /* 回绕到队列头部 */
    }
    s_usart1_cmd_count--;                           /* 命令计数递减 */

    __enable_irq();                                 /* 恢复全局中断 */
    return 1;                                       /* 成功读取一条命令 */
}


/**
 * @brief  获取接收溢出计数
 * @return uint32_t  累积的溢出次数（因命令队列满而丢弃的命令数）
 * @note   可用于上层统计采样周期内的数据丢失情况。
 *         典型用途：在日志导出时报告丢失的帧数。
 */
uint32_t USART1_GetRxOverflowCount(void)
{
    return s_usart1_rx_overflow;   /* 返回溢出计数器的当前值 */
}
