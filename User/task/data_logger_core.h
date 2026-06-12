#ifndef __DATA_LOGGER_CORE_H__
#define __DATA_LOGGER_CORE_H__

#include "../main.h"
#include "queue.h"
#include <stdint.h>

/*
 * 本头文件只给 User/task 目录下的数据采集记录仪任务层使用。
 * 这里集中放“跨模块共享”的常量、数据结构、全局状态和函数声明。
 *
 * 编译方式说明：
 * task_user.c 会通过 #include "./task/xxx.c" 把这些模块合并成一个编译单元，
 * 所以 Keil 工程里仍然只需要保留 task_user.c，不需要单独添加这些 .c 文件。
 */

/* 采样数据队列深度：采集任务写入，存储任务读出。 */
#define DATA_LOGGER_SAMPLE_QUEUE_LEN        64U

/* 存储命令队列深度：串口任务/按键任务写入，存储任务读出。 */
#define DATA_LOGGER_CMD_QUEUE_LEN           8U

/* 默认采样周期：10ms 对应 100Hz。 */
#define DATA_LOGGER_DEFAULT_PERIOD_MS       10U

/* CSV 文件同步间隔：每写入 32 条样本执行一次 f_sync，降低掉电丢数据风险。 */
#define DATA_LOGGER_SYNC_EVERY_SAMPLES      32U

/* 串口读取日志时每次从 FatFs 文件读取的字节数。 */
#define DATA_LOGGER_READ_CHUNK_SIZE         64U

/* 串口 read 命令最多输出的日志字节数，避免一次性刷屏太久。 */
#define DATA_LOGGER_READ_MAX_BYTES          2048U

/* STM32F407 ADC 配置为 12 位，最大数字量为 4095。 */
#define DATA_LOGGER_ADC_FULL_SCALE          4095U

/* ADC 参考电压，单位 mV；用于把 raw 值换算为毫伏。 */
#define DATA_LOGGER_ADC_VREF_MV             3300U

/*
 * 单条采样数据。
 * AcquireTask 从 adc_value[] 中取出 ADC DMA 搬运后的原始值，
 * 再填充成这个结构体，交给 StorageTask 写入 CSV。
 */
typedef struct
{
    uint32_t seq;       /* 样本序号，从 1 开始递增，用于观察是否丢样。 */
    uint32_t tick_ms;   /* 采样时刻，来自 TIM6 维护的 g_delay_tick，单位 ms。 */
    uint16_t raw0;      /* ADC 通道 0 原始值，范围 0..4095。 */
    uint16_t raw1;      /* ADC 通道 1 原始值，范围 0..4095。 */
    uint16_t mv0;       /* ADC 通道 0 换算后的电压，单位 mV。 */
    uint16_t mv1;       /* ADC 通道 1 换算后的电压，单位 mV。 */
} data_logger_sample_t;

/*
 * 存储任务命令类型。
 * 串口任务和按键任务不直接操作 FatFs 文件，而是发命令给 StorageTask，
 * 这样可以避免多个任务同时访问同一个文件对象。
 */
typedef enum
{
    DATA_LOGGER_STORAGE_CMD_START = 0,   /* 创建/打开 CSV 文件并开始记录。 */
    DATA_LOGGER_STORAGE_CMD_STOP,        /* 停止记录、写完队列残留数据并关闭文件。 */
    DATA_LOGGER_STORAGE_CMD_READ,        /* 通过串口输出当前日志文件内容。 */
    DATA_LOGGER_STORAGE_CMD_FORMAT       /* 格式化 SPI Flash 上的 FatFs 文件系统。 */
} data_logger_storage_cmd_type_t;

/* 存储任务命令包，目前只有命令类型，后续可以扩展参数。 */
typedef struct
{
    data_logger_storage_cmd_type_t type;  /* 具体执行哪一种存储动作。 */
} data_logger_storage_cmd_t;

/*
 * 运行状态结构体。
 * LED 任务、串口 status 命令、采集任务、存储任务都会读取或更新它。
 */
typedef struct
{
    volatile uint8_t running;             /* 1=正在采集记录，0=停止。 */
    volatile uint8_t paused;              /* 1=暂停采集，0=允许采集。 */
    volatile uint8_t storage_ready;       /* 1=FatFs 挂载成功，0=存储不可用。 */
    volatile uint8_t file_open;           /* 1=日志文件已打开，0=没有打开文件。 */
    volatile uint8_t error;               /* 1=出现过错误，用于 LED3 闪烁提示。 */
    volatile uint16_t sample_period_ms;   /* 采集任务周期，单位 ms。 */
    volatile uint32_t sample_count;       /* 已成功写入 CSV 的样本数量。 */
    volatile uint32_t dropped_count;      /* 采样队列满导致丢弃的样本数量。 */
    volatile uint32_t write_error_count;  /* FatFs 写入失败次数。 */
    volatile uint32_t sync_count;         /* f_sync 成功次数。 */
    volatile FRESULT last_fs_result;      /* 最近一次 FatFs API 返回值。 */
    char current_file[20];                /* 当前日志文件名，例如 0:/LOG001.CSV。 */
} data_logger_status_t;

/* 采样队列句柄：AcquireTask 写入，StorageTask 读取。 */
extern QueueHandle_t g_data_logger_sample_queue;

/* 存储命令队列句柄：CliTask/KeyTask 写入，StorageTask 读取。 */
extern QueueHandle_t g_data_logger_storage_cmd_queue;

/* 全局运行状态：集中保存运行、暂停、错误、计数器和当前文件名。 */
extern data_logger_status_t g_data_logger_status;

/* 采样序号计数器：每产生一条样本加 1。 */
extern uint32_t g_data_logger_sample_seq;

/* FatFs 文件系统对象：挂载 SPI Flash 逻辑盘时使用。 */
extern FATFS g_data_logger_fs;

/* 当前写入中的日志文件对象。 */
extern FIL g_data_logger_log_file;

/* read 命令临时打开日志文件时使用的文件对象。 */
extern FIL g_data_logger_read_file;

/* f_mkfs 工作缓冲区，大小按 FatFs 最大扇区大小分配。 */
extern BYTE g_data_logger_mkfs_work[FF_MAX_SS];

/* read 命令的串口输出缓冲区。 */
extern uint8_t g_data_logger_read_buf[DATA_LOGGER_READ_CHUNK_SIZE];

/* 应用入口：创建队列、创建任务并启动 FreeRTOS 调度器。 */
void LoggerApp_Start(void);

/* FreeRTOS 任务入口函数。 */
void DataLogger_AcquireTask(void *pvParameters);
void DataLogger_StorageTask(void *pvParameters);
void DataLogger_CliTask(void *pvParameters);
void DataLogger_KeyTask(void *pvParameters);
void DataLogger_LedTask(void *pvParameters);

/* 串口命令模块接口。 */
void DataLogger_PrintHelp(void);
void DataLogger_PrintStatus(void);
void DataLogger_ProcessCommand(char *line);
void DataLogger_SendStorageCmd(data_logger_storage_cmd_type_t type);

/* FatFs 存储模块接口。 */
FRESULT DataLogger_Mount(FATFS *fs);
FRESULT DataLogger_OpenLog(FIL *file);
void DataLogger_CloseLog(FIL *file);
void DataLogger_DrainSamplesToFile(FIL *file);
void DataLogger_HandleRead(FIL *active_file);
void DataLogger_HandleFormat(FATFS *fs, FIL *file);
FRESULT DataLogger_WriteSample(FIL *file, const data_logger_sample_t *sample);
void DataLogger_SetFsResult(FRESULT result);

#endif
