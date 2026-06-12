#include "spi.h"   /* 包含本模块头文件，获取宏定义和函数声明 */


/**
 * @brief  初始化 SPI1 和 Flash 相关 GPIO
 * @note   执行流程：
 *         1. 使能 GPIOG、GPIOB、SPI1 时钟
 *         2. 配置 SCK/MISO/MOSI 为复用推挽输出
 *         3. 配置 CS(PG6) 为通用推挽输出，默认高电平（不选中）
 *         4. 映射引脚复用功能到 SPI1
 *         5. 配置 SPI1 外设参数并使能
 */
void SPI_Flash_Init(void)
{
    /* ======================== 步骤1：使能时钟 ======================= */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOB, ENABLE);   /* 使能 GPIOG 和 GPIOB 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);                           /* 使能 SPI1 外设时钟 */

    /* ======================== 步骤2：配置 GPIO ======================= */
    GPIO_InitTypeDef GPIO_InitStructure;                           /* GPIO 初始化结构体 */

    /* 配置 SPI 通信引脚（SCK/MISO/MOSI）为复用功能模式 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                 /* 复用功能模式，引脚由 SPI1 外设控制 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             /* 高速 50MHz，SPI 通信需要较高速度 */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                /* 推挽输出 */
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                /* 上拉，确保空闲时电平稳定 */

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;                    /* PG6：CS 片选引脚（先按复用模式初始化） */
    GPIO_Init(GPIOG, &GPIO_InitStructure);                        /* 应用配置到 GPIOG */

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;                    /* PB3：SCK 时钟信号 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        /* 应用配置到 GPIOB */

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;                    /* PB4：MISO 主机输入/从机输出 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        /* 应用配置到 GPIOB */

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;                    /* PB5：MOSI 主机输出/从机输入 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        /* 应用配置到 GPIOB */

    /* CS 引脚（PG6）重新配置为通用输出模式，由软件手动控制片选时序 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;                /* 通用输出模式 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;             /* 输出速率 25MHz */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;                   /* PG6 */
    GPIO_Init(GPIOG, &GPIO_InitStructure);                        /* 覆盖 PG6 配置为普通输出 */

    /* ======================== 步骤3：引脚复用功能映射 ======================= */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);       /* 将 PB3 映射为 SPI1 复用功能（SCK） */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);       /* 将 PB4 映射为 SPI1 复用功能（MISO） */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);       /* 将 PB5 映射为 SPI1 复用功能（MOSI） */

    /* ======================== 步骤4：配置 SPI1 外设参数 ======================= */
    SPI_InitTypeDef SPI_InitStructure;                             /* SPI 初始化结构体 */

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  /* 双线全双工模式 */
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  /* 主机模式 */
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  /* 8 位数据帧 */
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;                    /* 空闲时 SCK 为高电平 */
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                   /* 第二个时钟边沿采样（模式3） */
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     /* 软件 NSS，不由硬件自动控制 */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;         /* 波特率 2 分频：84MHz/2=42MHz */
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 /* 高位（MSB）先发送 */
    SPI_InitStructure.SPI_CRCPolynomial     = 0;                                /* CRC 多项式（不使用 CRC，填 0） */

    SPI_Init(SPI1, &SPI_InitStructure);                            /* 将配置写入 SPI1 寄存器 */
    SPI_Cmd(SPI1, ENABLE);                                         /* 使能 SPI1 外设 */
    SPI1_FLASH_Chose_DISABLE;                                      /* 初始化完成后确保片选为高（不选中 Flash） */
}


/**
 * @brief  通过 SPI1 发送一个字节并接收返回的字节
 * @param  data  要发送的字节数据
 * @return uint8_t  从 SPI 接收到的字节
 * @note   SPI 全双工特性：每发送一个字节同时也会收到一个字节。
 *         对于 Flash 命令，发送指令或地址后收到的字节可能为无效数据（dummy）。
 *         内置超时机制，避免 SPI 外设异常时死循环。
 */
uint8_t SPI_Flash_SendandReceive_Byte(uint8_t data)
{
    uint8_t  RxData  = 0;                  /* 接收数据缓存 */
    uint16_t Timeout = 0xffff;             /* 超时计数器，防止硬件异常时死循环 */

    /* 等待发送缓冲区为空（TXE=1 表示可以写入新数据） */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
        if (Timeout-- == 0)                /* 超时计数器递减，减到 0 则超时退出 */
        {
            printf("error: SPI_I2S_FLAG_TXE == RESET \r\n");   /* 输出超时错误日志 */
            return 0x00;                   /* 返回 0 表示通信失败 */
        }
    }

    SPI_I2S_SendData(SPI1, data);          /* 将待发送数据写入 SPI 数据寄存器，启动传输 */

    Timeout = 0xffff;                      /* 重置超时计数器 */
    /* 等待接收缓冲区非空（RXNE=1 表示有接收数据可读） */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if (Timeout-- == 0)                /* 超时计数器递减，减到 0 则超时退出 */
        {
            printf("error: SPI_I2S_FLAG_RXNE == RESET \r\n");   /* 输出超时错误日志 */
            return 0x00;                   /* 返回 0 表示通信失败 */
        }
    }
    RxData = SPI_I2S_ReceiveData(SPI1);    /* 从 SPI 数据寄存器读取接收到的字节 */
    return RxData;                         /* 返回接收到的数据 */
}


/**
 * @brief  向 Flash 发送写使能指令（0x06）
 * @note   在执行页编程（0x02）、扇区擦除（0x20）等修改操作前必须先发送写使能指令。
 *         发送后在 CS 拉高时 Flash 内部将 WEL 位（写使能锁存）置 1。
 */
void SPI_Flash_Write_Enable(void)
{
    SPI1_FLASH_Chose_ENABLE;               /* 拉低 CS 片选，选中 Flash */
    SPI_Flash_SendandReceive_Byte(0x06);   /* 发送写使能指令 0x06 */
    SPI1_FLASH_Chose_DISABLE;              /* 拉高 CS 片选，Flash 锁存 WEL 位 */
}


/**
 * @brief  擦除 Flash 指定扇区（4KB）
 * @param  add  扇区号（0 ~ N-1，每个扇区 4096 字节）
 * @note   扇区擦除操作会将对应 4KB 区域所有数据恢复为 0xFF。
 *         执行流程：写使能 -> 发送 0x20 指令 -> 发送 24 位地址 -> 等待忙完成。
 *         调用示例：SPI_Flash_ClearFlash(0) 擦除第 0 扇区（地址 0x000000 ~ 0x000FFF）。
 */
void SPI_Flash_ClearFlash(uint32_t add)
{
    add *= 4096;                           /* 扇区号乘以 4096，转换为扇区首字节地址（24 位） */

    SPI_Flash_Write_Enable();              /* 先发送写使能指令（0x06） */

    SPI1_FLASH_Chose_ENABLE;               /* 拉低 CS，开始指令帧 */
    SPI_Flash_SendandReceive_Byte(0x20);   /* 发送扇区擦除指令 0x20 */
    SPI_Flash_SendandReceive_Byte((uint8_t)(add >> 16));   /* 发送地址 bit[23:16] */
    SPI_Flash_SendandReceive_Byte((uint8_t)(add >> 8));    /* 发送地址 bit[15:8] */
    SPI_Flash_SendandReceive_Byte((uint8_t)(add & 0xff));  /* 发送地址 bit[7:0] */
    SPI1_FLASH_Chose_DISABLE;              /* 拉高 CS，Flash 开始执行擦除操作 */

    SPI_FLASH_Busy_State();                /* 轮询等待擦除操作完成（BUSY 位清零） */
}


/**
 * @brief  轮询等待 Flash 内部操作完成
 * @note   Flash 在执行擦除/编程等耗时操作时会将状态寄存器的 BUSY 位（bit0）置 1。
 *         本函数通过循环发送 0x05（读状态寄存器）指令，读取并检查 BUSY 位，
 *         直到 BUSY 位清零表示操作完成。
 *         内置超时保护，避免 Flash 异常时死循环。
 */
void SPI_FLASH_Busy_State(void)
{
    uint8_t  state;                        /* 状态寄存器值 */
    uint16_t timeout = 0xffff;            /* 超时计数器 */

    SPI1_FLASH_Chose_ENABLE;               /* 拉低 CS，选中 Flash */
    SPI_Flash_SendandReceive_Byte(0x05);   /* 发送读状态寄存器指令 0x05 */

    do
    {
        state = SPI_Flash_SendandReceive_Byte(DummyByte);   /* 发送哑字节，读取状态寄存器值 */
        if (timeout-- == 0)                                  /* 超时检查 */
        {
            printf("error: SPI_FLASH_Busy_State() \r\n");   /* 输出超时错误日志 */
        }
    } while ((state & 1) == 1);            /* 检查 BUSY 位（bit0）：1=忙，继续等待；0=空闲，退出 */

    SPI1_FLASH_Chose_DISABLE;              /* 拉高 CS，结束状态查询 */
}


/**
 * @brief  从 Flash 指定地址读取数据
 * @param  data  数据接收缓冲区指针
 * @param  add   Flash 内部起始地址（字节地址，24 位）
 * @param  len   要读取的字节数
 * @note   发送 0x03（读数据）指令和 24 位地址后，Flash 会从该地址开始连续输出数据。
 *         每发送一个哑字节（0x00）产生 8 个 SCK 时钟，Flash 通过 MISO 返回一个数据字节。
 */
void SPI_Flash_ReadDat(uint8_t *data, uint32_t add, uint32_t len)
{
    SPI1_FLASH_Chose_ENABLE;               /* 拉低 CS，选中 Flash */

    SPI_Flash_SendandReceive_Byte(0x03);                         /* 发送读数据指令 0x03 */
    SPI_Flash_SendandReceive_Byte((uint8_t)(add >> 16));         /* 发送地址 bit[23:16] */
    SPI_Flash_SendandReceive_Byte((uint8_t)(add >> 8));          /* 发送地址 bit[15:8] */
    SPI_Flash_SendandReceive_Byte((uint8_t)(add & 0xff));        /* 发送地址 bit[7:0] */

    while (len--)                                                 /* 循环读取 len 个字节 */
    {
        *data++ = SPI_Flash_SendandReceive_Byte(DummyByte);      /* 发送哑字节，接收 Flash 返回的数据并存入缓冲区 */
    }

    SPI1_FLASH_Chose_DISABLE;              /* 拉高 CS，结束读取 */
}


/**
 * @brief  向 Flash 指定地址写入数据（页编程，自动跨页处理）
 * @param  data  待写入数据缓冲区指针
 * @param  add   Flash 内部起始地址（字节地址，24 位）
 * @param  len   要写入的字节数
 * @note   实现自动跨页写入：
 *         1. Flash 页大小为 256 字节，一次页编程不能跨页边界
 *         2. 计算当前地址所在页的剩余空间（page_remain）
 *         3. 取 min(len, page_remain) 作为本次写入长度
 *         4. 发送写使能 -> 0x02 页编程指令 -> 24 位地址 -> 数据 -> 等待完成
 *         5. 更新地址和剩余长度，重复直到 len=0
 */
void SPI_Flash_WriteDat(uint8_t *data, uint32_t add, uint32_t len)
{
    uint32_t write_len;                    /* 本次写入的字节数 */
    uint32_t page_remain;                  /* 当前页内剩余可写字节数 */
    uint32_t i;                            /* 循环计数器 */

    while (len)                            /* 循环直到所有数据写入完毕 */
    {
        page_remain = 256 - (add % 256);   /* 计算当前页剩余空间：256 - (地址对256取模) */

        /**
         * 取待写长度和页剩余空间的较小值作为本次写入长度：
         *   - 若 len <= page_remain：一次写完，write_len = len
         *   - 若 len >  page_remain：先写满当前页，write_len = page_remain
         */
        write_len = (len < page_remain) ? len : page_remain;

        SPI_Flash_Write_Enable();          /* 发送写使能指令 0x06（每次页编程前必须重新使能） */

        SPI1_FLASH_Chose_ENABLE;           /* 拉低 CS，选中 Flash */
        SPI_Flash_SendandReceive_Byte(0x02);                        /* 发送页编程指令 0x02 */
        SPI_Flash_SendandReceive_Byte((uint8_t)(add >> 16));        /* 发送地址 bit[23:16] */
        SPI_Flash_SendandReceive_Byte((uint8_t)(add >> 8));         /* 发送地址 bit[15:8] */
        SPI_Flash_SendandReceive_Byte((uint8_t)(add & 0xff));       /* 发送地址 bit[7:0] */

        for (i = 0; i < write_len; i++)                              /* 连续发送 write_len 个字节数据 */
        {
            SPI_Flash_SendandReceive_Byte(*data++);                  /* 发送一个数据字节，指针递增 */
        }

        SPI1_FLASH_Chose_DISABLE;          /* 拉高 CS，Flash 开始执行页编程 */
        SPI_FLASH_Busy_State();            /* 等待当前页编程完成 */

        add += write_len;                  /* 地址前移已写入的字节数 */
        len -= write_len;                  /* 剩余长度减去已写入的字节数 */
    }
}


/**
 * @brief  读取 Flash 芯片的 JEDEC 制造商和设备 ID
 * @return uint32_t  24 位 ID 值（高 8 位=制造商 ID，中 8 位=存储器类型，低 8 位=容量）
 * @note   发送 0x9F（读 JEDEC ID）指令后连续读取 3 个字节，拼接为 32 位 ID。
 *         例如 W25Q64 返回 ID=0xEF4017（EF=Winbond, 40=SPI Flash, 17=64Mbit）。
 */
uint32_t SPI_FLASH_DriverID(void)
{
    uint8_t  ID1, ID2, ID3;               /* 分别存储 3 个 ID 字节 */
    uint32_t ID = 0;                       /* 最终拼接的 32 位 ID 值 */

    SPI1_FLASH_Chose_ENABLE;               /* 拉低 CS，选中 Flash */

    SPI_Flash_SendandReceive_Byte(0x9f);   /* 发送读 JEDEC ID 指令 0x9F */

    ID1 = SPI_Flash_SendandReceive_Byte(DummyByte);   /* 读取第 1 个 ID 字节：制造商 ID */
    ID2 = SPI_Flash_SendandReceive_Byte(DummyByte);   /* 读取第 2 个 ID 字节：存储器类型 */
    ID3 = SPI_Flash_SendandReceive_Byte(DummyByte);   /* 读取第 3 个 ID 字节：容量标识 */

    SPI1_FLASH_Chose_DISABLE;              /* 拉高 CS，结束指令 */

    ID = (ID1 << 16) | (ID2 << 8) | (ID3);              /* 拼接为 32 位 ID：0x00 | ID1 | ID2 | ID3 */
    return ID;                              /* 返回完整的芯片 ID */
}
