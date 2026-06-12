#include "data_logger_core.h"
#include <stdio.h>
#include <string.h>

/*
 * 函数功能：
 *   采集任务。按 sample_period_ms 周期读取 ADC DMA 缓冲区，把样本放入采样队列。
 *
 * 关键点：
 *   ADC 真正采样由 TIM2 触发，DMA 自动搬运到 adc_value[]；
 *   本任务只是周期性取最新值、换算电压并送给存储任务。
 */
void DataLogger_AcquireTask(void *pvParameters)
{
    TickType_t last_wake = xTaskGetTickCount();  /* 保存上一次唤醒 tick，用于固定周期延时。 */
    data_logger_sample_t sample;                 /* 当前准备发送到队列的一条样本。 */
    uint32_t packed0;                            /* adc_value[0] 的快照，避免处理时被 DMA 更新。 */
    uint32_t packed1;                            /* adc_value[1] 的快照，避免处理时被 DMA 更新。 */
    uint16_t period_ms;                          /* 本轮采样任务使用的周期，单位 ms。 */

    (void)pvParameters;

    for (;;)
    {
        /* 步骤1：读取当前采样周期；如果被错误设置成 0，则退回默认 10ms。 */
        period_ms = g_data_logger_status.sample_period_ms;
        if (period_ms == 0U)
        {
            period_ms = DATA_LOGGER_DEFAULT_PERIOD_MS;
        }

        /* 步骤2：使用 vTaskDelayUntil 实现稳定周期，降低普通 delay 的累计漂移。 */
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(period_ms));

        /* 步骤3：只有处于 running 且未 paused 时才产生样本。 */
        if ((g_data_logger_status.running == 0U) || (g_data_logger_status.paused != 0U))
        {
            continue;
        }

        /* 步骤4：读取 DMA 缓冲区快照。ADC 配置为 12 位，所以只取低 12 位。 */
        packed0 = adc_value[0];
        packed1 = adc_value[1];

        /* 步骤5：填充样本序号、时间戳、原始 ADC 值。 */
        sample.seq = ++g_data_logger_sample_seq;
        sample.tick_ms = g_delay_tick;
        sample.raw0 = (uint16_t)(packed0 & 0x0FFFU);
        sample.raw1 = (uint16_t)(packed1 & 0x0FFFU);

        /* 步骤6：把 ADC 原始值换算成 mV，方便 CSV 日志直接查看。 */
        sample.mv0 = (uint16_t)(((uint32_t)sample.raw0 * DATA_LOGGER_ADC_VREF_MV) /
                                DATA_LOGGER_ADC_FULL_SCALE);
        sample.mv1 = (uint16_t)(((uint32_t)sample.raw1 * DATA_LOGGER_ADC_VREF_MV) /
                                DATA_LOGGER_ADC_FULL_SCALE);

        /*
         * 步骤7：把样本送入队列。
         * 这里不阻塞等待，队列满就丢弃并记录 dropped_count，避免采集任务被存储卡住。
         */
        if (xQueueSend(g_data_logger_sample_queue, &sample, 0) != pdPASS)
        {
            g_data_logger_status.dropped_count++;
            g_data_logger_status.error = 1U;
        }
    }
}

/*
 * 函数功能：
 *   存储任务。统一处理 FatFs 挂载、CSV 创建、样本写入、日志读取和格式化。
 *
 * 设计原因：
 *   FatFs 文件对象不适合被多个任务同时操作，所以所有文件操作集中到这个任务。
 */
void DataLogger_StorageTask(void *pvParameters)
{
    data_logger_storage_cmd_t cmd;        /* 从命令队列取出的存储控制命令。 */
    data_logger_sample_t sample;          /* 从采样队列取出的待写入样本。 */
    FRESULT result;                       /* FatFs API 返回值。 */

    (void)pvParameters;

    /* 步骤1：清空 FatFs 对象，确保文件系统对象和文件对象处于初始状态。 */
    memset(&g_data_logger_fs, 0, sizeof(g_data_logger_fs));
    memset(&g_data_logger_log_file, 0, sizeof(g_data_logger_log_file));
    memset(&g_data_logger_read_file, 0, sizeof(g_data_logger_read_file));

    /* 步骤2：上电后先尝试挂载 SPI Flash 文件系统。 */
    result = DataLogger_Mount(&g_data_logger_fs);
    if (result == FR_NO_FILESYSTEM)
    {
        printf("storage has no filesystem, run format\r\n");
    }

    for (;;)
    {
        /*
         * 步骤3：先处理控制命令。
         * 使用 while 是为了一次循环内清空积压命令，让 start/stop/read/format 响应更及时。
         */
        while (xQueueReceive(g_data_logger_storage_cmd_queue, &cmd, 0) == pdPASS)
        {
            switch (cmd.type)
            {
            case DATA_LOGGER_STORAGE_CMD_START:
                /*
                 * 如果之前已经打开文件，先停采、写完队列残留样本、关闭旧文件，
                 * 再创建新日志文件。
                 */
                if (g_data_logger_status.file_open != 0U)
                {
                    g_data_logger_status.running = 0U;
                    DataLogger_DrainSamplesToFile(&g_data_logger_log_file);
                    DataLogger_CloseLog(&g_data_logger_log_file);
                }

                /* 如果文件系统还没挂载成功，启动前再尝试挂载一次。 */
                if (g_data_logger_status.storage_ready == 0U)
                {
                    result = DataLogger_Mount(&g_data_logger_fs);
                    if (result != FR_OK)
                    {
                        printf("start failed, mount ret=%d\r\n", (int)result);
                        break;
                    }
                }

                /*
                 * 启动一次新的记录前清空队列和计数器。
                 * 这样 status 输出的数据只对应本次采集。
                 */
                xQueueReset(g_data_logger_sample_queue);
                g_data_logger_sample_seq = 0;
                g_data_logger_status.sample_count = 0U;
                g_data_logger_status.dropped_count = 0U;
                g_data_logger_status.write_error_count = 0U;
                g_data_logger_status.sync_count = 0U;
                g_data_logger_status.error = 0U;

                /* 创建 CSV 文件并写入表头，成功后才允许采集任务开始产出样本。 */
                result = DataLogger_OpenLog(&g_data_logger_log_file);
                if (result == FR_OK)
                {
                    g_data_logger_status.running = 1U;
                    g_data_logger_status.paused = 0U;
                    printf("logger started: %s\r\n", g_data_logger_status.current_file);
                }
                else
                {
                    DataLogger_SetFsResult(result);
                    printf("open log failed, ret=%d\r\n", (int)result);
                }
                break;

            case DATA_LOGGER_STORAGE_CMD_STOP:
                /* 停止采集后，把队列中尚未写入的样本写完，再同步并关闭文件。 */
                g_data_logger_status.running = 0U;
                DataLogger_DrainSamplesToFile(&g_data_logger_log_file);
                DataLogger_CloseLog(&g_data_logger_log_file);
                printf("logger stopped\r\n");
                break;

            case DATA_LOGGER_STORAGE_CMD_READ:
                /* read 命令读取当前日志文件，并通过 USART1 输出一段内容。 */
                DataLogger_HandleRead(&g_data_logger_log_file);
                break;

            case DATA_LOGGER_STORAGE_CMD_FORMAT:
                /* format/erase 命令重新格式化 SPI Flash 上的 FatFs 文件系统。 */
                DataLogger_HandleFormat(&g_data_logger_fs, &g_data_logger_log_file);
                break;

            default:
                break;
            }
        }

        /*
         * 步骤4：没有立即要处理的命令时，等待采样队列。
         * 最多阻塞 100ms，保证即使没有样本也能周期性回到命令处理逻辑。
         */
        if (xQueueReceive(g_data_logger_sample_queue, &sample, pdMS_TO_TICKS(100)) == pdPASS)
        {
            /* 步骤5：只有文件打开且处于运行状态时才写 CSV。 */
            if ((g_data_logger_status.file_open != 0U) && (g_data_logger_status.running != 0U))
            {
                result = DataLogger_WriteSample(&g_data_logger_log_file, &sample);
                if (result == FR_OK)
                {
                    g_data_logger_status.sample_count++;

                    /*
                     * 步骤6：每写入固定数量样本同步一次文件。
                     * 这样比每条都同步更快，比一直不同步更安全。
                     */
                    if ((g_data_logger_status.sample_count % DATA_LOGGER_SYNC_EVERY_SAMPLES) == 0U)
                    {
                        result = f_sync(&g_data_logger_log_file);
                        DataLogger_SetFsResult(result);
                        if (result == FR_OK)
                        {
                            g_data_logger_status.sync_count++;
                        }
                    }
                }
                else
                {
                    DataLogger_SetFsResult(result);
                    g_data_logger_status.write_error_count++;
                }
            }
        }
    }
}

/*
 * 函数功能：
 *   串口命令任务。接收 USART1 的一行命令，并交给命令解析函数执行。
 */
void DataLogger_CliTask(void *pvParameters)
{
    char line[USART1_CMD_MAX_LEN];        /* USART1_ReadLine 读出的完整命令行。 */

    (void)pvParameters;

    /* 步骤1：上电后输出项目名称和命令帮助，方便串口调试。 */
    printf("\r\nSTM32F407 FreeRTOS data logger\r\n");
    DataLogger_PrintHelp();
    printf("cmd> ");

    for (;;)
    {
        /* 步骤2：读取一行命令；读到回车后才返回非 0。 */
        if (USART1_ReadLine(line, sizeof(line)) != 0U)
        {
            /* 步骤3：换行后解析命令，并重新打印提示符。 */
            printf("\r\n");
            DataLogger_ProcessCommand(line);
            printf("cmd> ");
        }
        else
        {
            /* 步骤4：没有完整命令时短暂让出 CPU。 */
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

/*
 * 函数功能：
 *   按键任务。KEY1 控制暂停/恢复，KEY2 控制开始/停止。
 */
void DataLogger_KeyTask(void *pvParameters)
{
    uint8_t key;                           /* key_get() 返回的按键编号。 */

    (void)pvParameters;

    for (;;)
    {
        /* 步骤1：扫描按键，key_get 内部负责消抖和键值判断。 */
        key = key_get();

        if (key == 1U)
        {
            /*
             * 步骤2：KEY1 只在运行时生效。
             * 每按一次就在暂停和恢复之间切换。
             */
            if (g_data_logger_status.running != 0U)
            {
                g_data_logger_status.paused =
                    (g_data_logger_status.paused == 0U) ? 1U : 0U;
                printf("key pause=%u\r\n", (unsigned int)g_data_logger_status.paused);
            }
        }
        else if (key == 2U)
        {
            /*
             * 步骤3：KEY2 相当于串口 start/stop。
             * 文件操作仍然通过命令队列交给存储任务执行。
             */
            if (g_data_logger_status.running != 0U)
            {
                DataLogger_SendStorageCmd(DATA_LOGGER_STORAGE_CMD_STOP);
            }
            else
            {
                DataLogger_SendStorageCmd(DATA_LOGGER_STORAGE_CMD_START);
            }
        }

        /* 步骤4：20ms 扫描周期，兼顾响应速度和 CPU 占用。 */
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/*
 * 函数功能：
 *   LED 状态任务。
 *
 * 显示规则：
 *   LED1：系统心跳，每 500ms 翻转；
 *   LED2：正在采集常亮，暂停时闪烁，停止时熄灭；
 *   LED3：错误状态闪烁，无错误时熄灭。
 */
void DataLogger_LedTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        /* 步骤1：LED1 固定翻转，表示 FreeRTOS 调度仍在运行。 */
        LED_Toggle(LED1);

        /* 步骤2：LED2 表示采集状态。 */
        if ((g_data_logger_status.running != 0U) && (g_data_logger_status.paused == 0U))
        {
            LED_On(LED2);
        }
        else if ((g_data_logger_status.running != 0U) && (g_data_logger_status.paused != 0U))
        {
            LED_Toggle(LED2);
        }
        else
        {
            LED_Off(LED2);
        }

        /* 步骤3：LED3 表示错误状态。 */
        if (g_data_logger_status.error != 0U)
        {
            LED_Toggle(LED3);
        }
        else
        {
            LED_Off(LED3);
        }

        /* 步骤4：500ms 刷新一次 LED 状态。 */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
