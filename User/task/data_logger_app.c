#include "data_logger_core.h"
#include <stdio.h>
#include <string.h>

/*
 * 数据采集记录仪任务层的全局对象定义。
 * 这些变量在 data_logger_core.h 中声明，在本文件中分配实际存储空间。
 */
QueueHandle_t g_data_logger_sample_queue = NULL;
QueueHandle_t g_data_logger_storage_cmd_queue = NULL;
data_logger_status_t g_data_logger_status;
uint32_t g_data_logger_sample_seq = 0;
FATFS g_data_logger_fs;
FIL g_data_logger_log_file;
FIL g_data_logger_read_file;
BYTE g_data_logger_mkfs_work[FF_MAX_SS];
uint8_t g_data_logger_read_buf[DATA_LOGGER_READ_CHUNK_SIZE];

/*
 * 函数功能：
 *   创建数据采集记录仪所需的队列和 FreeRTOS 任务，并启动调度器。
 *
 * 运行关系：
 *   main() 完成外设初始化后调用本函数；
 *   本函数成功后不会返回，因为 vTaskStartScheduler() 会接管 CPU 调度。
 */
void LoggerApp_Start(void)
{
    BaseType_t create_result = pdPASS;    /* 记录任务创建是否全部成功。 */

    /* 步骤1：清空运行状态，避免上电后状态变量保留随机值。 */
    memset(&g_data_logger_status, 0, sizeof(g_data_logger_status));

    /* 步骤2：设置默认采样周期。10ms 对应 100Hz。 */
    g_data_logger_status.sample_period_ms = DATA_LOGGER_DEFAULT_PERIOD_MS;

    /*
     * 步骤3：创建两个队列。
     * 采样队列负责传递 ADC 样本；
     * 命令队列负责把 start/stop/read/format 等动作交给存储任务执行。
     */
    g_data_logger_sample_queue =
        xQueueCreate(DATA_LOGGER_SAMPLE_QUEUE_LEN, sizeof(data_logger_sample_t));
    g_data_logger_storage_cmd_queue =
        xQueueCreate(DATA_LOGGER_CMD_QUEUE_LEN, sizeof(data_logger_storage_cmd_t));

    /* 步骤4：如果队列创建失败，点亮错误灯并返回，避免后续任务使用空句柄。 */
    if ((g_data_logger_sample_queue == NULL) || (g_data_logger_storage_cmd_queue == NULL))
    {
        LED_On(LED3);
        printf("logger queue create failed\r\n");
        return;
    }

    /*
     * 步骤5：创建任务。
     * 采集和存储任务优先级较高，保证采样和写文件及时；
     * 串口和按键属于交互任务，优先级略低；
     * LED 只显示状态，优先级最低。
     */
    if (xTaskCreate(DataLogger_AcquireTask, "acquire", 256, NULL, 3, NULL) != pdPASS)
    {
        create_result = pdFAIL;
    }
    if (xTaskCreate(DataLogger_StorageTask, "storage", 512, NULL, 3, NULL) != pdPASS)
    {
        create_result = pdFAIL;
    }
    if (xTaskCreate(DataLogger_CliTask, "cli", 384, NULL, 2, NULL) != pdPASS)
    {
        create_result = pdFAIL;
    }
    if (xTaskCreate(DataLogger_KeyTask, "key", 128, NULL, 2, NULL) != pdPASS)
    {
        create_result = pdFAIL;
    }
    if (xTaskCreate(DataLogger_LedTask, "led", 128, NULL, 1, NULL) != pdPASS)
    {
        create_result = pdFAIL;
    }

    /* 步骤6：只要有一个任务创建失败，就进入错误提示，不启动调度器。 */
    if (create_result != pdPASS)
    {
        LED_On(LED3);
        printf("logger task create failed\r\n");
        return;
    }

    /* 步骤7：启动 FreeRTOS 调度器，从这里开始由任务抢占式运行。 */
    vTaskStartScheduler();

    /*
     * 步骤8：正常情况下不会运行到这里。
     * 如果运行到这里，通常是 FreeRTOS heap 不足导致调度器启动失败。
     */
    LED_On(LED3);
    for (;;)
    {
    }
}
