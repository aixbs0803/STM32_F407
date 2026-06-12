#include "data_logger_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void DataLogger_LowerString(char *line);

/*
 * 函数功能：
 *   解析串口输入的一行命令，并执行对应动作。
 *
 * 支持命令：
 *   start、stop、pause、resume、status、read、format、erase、rate N、help。
 */
void DataLogger_ProcessCommand(char *line)
{
    char *arg;                /* 指向命令参数，例如 rate 100 中的 100。 */
    uint32_t hz;              /* rate 命令输入的采样频率。 */
    uint16_t period;          /* 根据 hz 换算出的任务周期，单位 ms。 */

    /* 步骤1：命令统一转成小写，让 START/start/Start 都能识别。 */
    DataLogger_LowerString(line);

    /*
     * 步骤2：把第一个空格拆成字符串结束符。
     * line 保留命令名，arg 指向参数起始位置。
     */
    arg = strchr(line, ' ');
    if (arg != NULL)
    {
        *arg++ = '\0';
        while (*arg == ' ')
        {
            arg++;
        }
    }

    /* 步骤3：空命令直接忽略。 */
    if (line[0] == '\0')
    {
        return;
    }

    /* 步骤4：按命令名分发。文件相关动作都通过存储命令队列执行。 */
    if (strcmp(line, "start") == 0)
    {
        DataLogger_SendStorageCmd(DATA_LOGGER_STORAGE_CMD_START);
    }
    else if (strcmp(line, "stop") == 0)
    {
        g_data_logger_status.running = 0U;
        DataLogger_SendStorageCmd(DATA_LOGGER_STORAGE_CMD_STOP);
    }
    else if (strcmp(line, "pause") == 0)
    {
        g_data_logger_status.paused = 1U;
        printf("paused\r\n");
    }
    else if (strcmp(line, "resume") == 0)
    {
        g_data_logger_status.paused = 0U;
        printf("resumed\r\n");
    }
    else if (strcmp(line, "status") == 0)
    {
        DataLogger_PrintStatus();
    }
    else if (strcmp(line, "read") == 0)
    {
        DataLogger_SendStorageCmd(DATA_LOGGER_STORAGE_CMD_READ);
    }
    else if ((strcmp(line, "format") == 0) || (strcmp(line, "erase") == 0))
    {
        g_data_logger_status.running = 0U;
        DataLogger_SendStorageCmd(DATA_LOGGER_STORAGE_CMD_FORMAT);
    }
    else if (strcmp(line, "rate") == 0)
    {
        /*
         * 步骤5：rate 命令用于同时修改 TIM2 触发频率和采集任务周期。
         * 范围限制在 1..1000Hz，避免任务周期过小造成队列和存储压力过大。
         */
        if ((arg == NULL) || (arg[0] == '\0'))
        {
            printf("usage: rate <1..1000>\r\n");
            return;
        }

        hz = strtoul(arg, NULL, 10);
        if ((hz < 1U) || (hz > 1000U))
        {
            printf("rate out of range\r\n");
            return;
        }

        period = (uint16_t)(1000U / hz);
        if (period == 0U)
        {
            period = 1U;
        }

        g_data_logger_status.sample_period_ms = period;
        TIM2_ADC_Trigger_Init(hz);
        printf("sample rate=%luHz period=%ums\r\n",
               (unsigned long)hz,
               (unsigned int)period);
    }
    else if (strcmp(line, "help") == 0)
    {
        DataLogger_PrintHelp();
    }
    else
    {
        printf("unknown command: %s\r\n", line);
        DataLogger_PrintHelp();
    }
}

/*
 * 函数功能：
 *   输出串口命令帮助信息。
 */
void DataLogger_PrintHelp(void)
{
    printf("commands:\r\n");
    printf("  start   - open CSV and start sampling\r\n");
    printf("  stop    - stop sampling and close CSV\r\n");
    printf("  pause   - pause sampling\r\n");
    printf("  resume  - resume sampling\r\n");
    printf("  status  - print runtime counters\r\n");
    printf("  read    - print current log, max 2048 bytes\r\n");
    printf("  format  - make FatFs filesystem on SPI Flash\r\n");
    printf("  erase   - alias of format\r\n");
    printf("  rate N  - set sampling rate in Hz, 1..1000\r\n");
    printf("  help    - print this help\r\n");
}

/*
 * 函数功能：
 *   输出当前运行状态和统计计数，便于演示和调试。
 */
void DataLogger_PrintStatus(void)
{
    /* 步骤1：输出运行态、暂停态、存储挂载态、文件打开态和错误态。 */
    printf("run=%u pause=%u storage=%u file=%u error=%u period=%ums\r\n",
           (unsigned int)g_data_logger_status.running,
           (unsigned int)g_data_logger_status.paused,
           (unsigned int)g_data_logger_status.storage_ready,
           (unsigned int)g_data_logger_status.file_open,
           (unsigned int)g_data_logger_status.error,
           (unsigned int)g_data_logger_status.sample_period_ms);

    /* 步骤2：输出样本写入数量、丢样数量、写错误数量、同步次数和串口溢出次数。 */
    printf("samples=%lu dropped=%lu write_err=%lu sync=%lu rx_overflow=%lu fs=%d\r\n",
           (unsigned long)g_data_logger_status.sample_count,
           (unsigned long)g_data_logger_status.dropped_count,
           (unsigned long)g_data_logger_status.write_error_count,
           (unsigned long)g_data_logger_status.sync_count,
           (unsigned long)USART1_GetRxOverflowCount(),
           (int)g_data_logger_status.last_fs_result);

    /* 步骤3：输出当前文件名；如果还没有创建日志文件，则输出 -。 */
    printf("file=%s\r\n",
           (g_data_logger_status.current_file[0] != '\0') ?
           g_data_logger_status.current_file : "-");
}

/*
 * 函数功能：
 *   向存储任务发送命令。
 *
 * 设计原因：
 *   串口任务和按键任务只负责发命令，不直接操作 FatFs 文件，
 *   这样可以降低并发访问文件系统的风险。
 */
void DataLogger_SendStorageCmd(data_logger_storage_cmd_type_t type)
{
    data_logger_storage_cmd_t cmd;         /* 准备发送到存储命令队列的命令包。 */

    /* 步骤1：填充命令类型。 */
    cmd.type = type;

    /* 步骤2：尝试发送命令；如果队列满，记录错误并提示。 */
    if ((g_data_logger_storage_cmd_queue == NULL) ||
        (xQueueSend(g_data_logger_storage_cmd_queue, &cmd, pdMS_TO_TICKS(50)) != pdPASS))
    {
        g_data_logger_status.error = 1U;
        printf("storage command queue full\r\n");
    }
}

/*
 * 函数功能：
 *   把字符串中的大写字母转换为小写字母。
 *
 * 注意：
 *   这里只处理 ASCII 命令字符，不处理中文或其他编码字符。
 */
static void DataLogger_LowerString(char *line)
{
    /* 步骤1：逐字节扫描字符串，直到遇到 '\0'。 */
    while (*line != '\0')
    {
        /* 步骤2：如果当前字符是 A..Z，就换算成 a..z。 */
        if ((*line >= 'A') && (*line <= 'Z'))
        {
            *line = (char)(*line + ('a' - 'A'));
        }

        /* 步骤3：移动到下一个字符。 */
        line++;
    }
}
