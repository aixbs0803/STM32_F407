#include "data_logger_core.h"
#include <stdio.h>
#include <string.h>

/*
 * 函数功能：
 *   挂载 SPI Flash 上的 FatFs 文件系统。
 */
FRESULT DataLogger_Mount(FATFS *fs)
{
    FRESULT result;     /* f_mount 返回值。 */

    /* 步骤1：挂载逻辑盘 0:/，第三个参数为 1 表示立即挂载。 */
    result = f_mount(fs, "0:/", 1);

    /* 步骤2：保存最近一次 FatFs 结果，供 status 命令和错误 LED 使用。 */
    DataLogger_SetFsResult(result);

    /* 步骤3：更新存储可用状态。 */
    g_data_logger_status.storage_ready = (result == FR_OK) ? 1U : 0U;

    return result;
}

/*
 * 函数功能：
 *   创建新的 CSV 日志文件，并写入 CSV 表头。
 *
 * 文件命名规则：
 *   优先创建 0:/LOG001.CSV 到 0:/LOG999.CSV；
 *   如果 001..999 都存在，则覆盖 0:/LOG999.CSV。
 */
FRESULT DataLogger_OpenLog(FIL *file)
{
    FRESULT result;             /* FatFs API 返回值。 */
    UINT written = 0;           /* f_write 实际写入字节数。 */
    UINT header_len;            /* CSV 表头长度。 */
    uint16_t index;             /* 日志文件序号。 */
    char filename[20];          /* 当前尝试创建的文件名。 */
    const char *header = "seq,tick_ms,adc0_raw,adc1_raw,adc0_mv,adc1_mv\r\n";

    /*
     * 步骤1：从 LOG001.CSV 开始尝试创建新文件。
     * 使用 FA_CREATE_NEW 可以避免覆盖已有实验数据。
     */
    for (index = 1U; index <= 999U; index++)
    {
        sprintf(filename, "0:/LOG%03u.CSV", (unsigned int)index);
        result = f_open(file, filename, FA_CREATE_NEW | FA_WRITE);
        if (result == FR_OK)
        {
            /* 步骤2：创建成功后记录当前文件名，并标记文件已打开。 */
            strcpy(g_data_logger_status.current_file, filename);
            g_data_logger_status.file_open = 1U;

            /* 步骤3：写入 CSV 表头，方便用 Excel 或串口直接查看字段含义。 */
            header_len = (UINT)strlen(header);
            result = f_write(file, header, header_len, &written);
            if ((result == FR_OK) && (written != header_len))
            {
                result = FR_DISK_ERR;
            }

            /* 步骤4：表头写入成功后立即同步，保证文件头落到 Flash。 */
            if (result == FR_OK)
            {
                result = f_sync(file);
            }

            /* 步骤5：如果写表头或同步失败，关闭文件并清除打开标志。 */
            if (result != FR_OK)
            {
                f_close(file);
                g_data_logger_status.file_open = 0U;
            }

            DataLogger_SetFsResult(result);
            return result;
        }

        /*
         * 步骤6：如果只是文件已存在，就继续尝试下一个序号；
         * 如果是其他错误，直接返回。
         */
        if (result != FR_EXIST)
        {
            DataLogger_SetFsResult(result);
            return result;
        }
    }

    /*
     * 步骤7：如果 001..999 都存在，退化为覆盖 LOG999.CSV。
     * 这样系统仍然可以继续演示，不会因为文件名耗尽彻底无法启动。
     */
    strcpy(filename, "0:/LOG999.CSV");
    result = f_open(file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (result == FR_OK)
    {
        g_data_logger_status.file_open = 1U;
        strcpy(g_data_logger_status.current_file, filename);

        header_len = (UINT)strlen(header);
        result = f_write(file, header, header_len, &written);
        if ((result == FR_OK) && (written != header_len))
        {
            result = FR_DISK_ERR;
        }

        if (result == FR_OK)
        {
            result = f_sync(file);
        }

        if (result != FR_OK)
        {
            f_close(file);
            g_data_logger_status.file_open = 0U;
        }
    }

    DataLogger_SetFsResult(result);
    return result;
}

/*
 * 函数功能：
 *   同步并关闭当前日志文件。
 */
void DataLogger_CloseLog(FIL *file)
{
    FRESULT result;     /* f_sync/f_close 返回值。 */

    /* 步骤1：如果没有打开文件，直接返回。 */
    if (g_data_logger_status.file_open == 0U)
    {
        return;
    }

    /* 步骤2：先同步文件内容，尽量减少停止时的数据丢失。 */
    result = f_sync(file);
    DataLogger_SetFsResult(result);

    /* 步骤3：关闭文件并更新状态。 */
    result = f_close(file);
    DataLogger_SetFsResult(result);
    g_data_logger_status.file_open = 0U;
}

/*
 * 函数功能：
 *   停止或重启采集前，把采样队列里残留的样本写入文件。
 */
void DataLogger_DrainSamplesToFile(FIL *file)
{
    data_logger_sample_t sample;  /* 从采样队列取出的残留样本。 */
    FRESULT result;               /* 写文件返回值。 */

    /* 步骤1：持续非阻塞读取队列，直到队列为空。 */
    while (xQueueReceive(g_data_logger_sample_queue, &sample, 0) == pdPASS)
    {
        /* 步骤2：如果文件已经关闭，样本无法写入，只能丢弃。 */
        if (g_data_logger_status.file_open == 0U)
        {
            continue;
        }

        /* 步骤3：把残留样本写入 CSV，并更新统计计数。 */
        result = DataLogger_WriteSample(file, &sample);
        if (result == FR_OK)
        {
            g_data_logger_status.sample_count++;
        }
        else
        {
            DataLogger_SetFsResult(result);
            g_data_logger_status.write_error_count++;
        }
    }
}

/*
 * 函数功能：
 *   读取当前日志文件的一部分内容，并通过 USART1 输出。
 */
void DataLogger_HandleRead(FIL *active_file)
{
    FRESULT result;             /* FatFs API 返回值。 */
    UINT read_len = 0;          /* 本次 f_read 实际读出的字节数。 */
    UINT to_read;               /* 本次计划读取的字节数。 */
    uint32_t total = 0;         /* 已经通过串口输出的总字节数。 */
    UINT i;                     /* 串口逐字节发送用的循环变量。 */

    /* 步骤1：没有创建过日志文件时，直接提示。 */
    if (g_data_logger_status.current_file[0] == '\0')
    {
        printf("no log file\r\n");
        return;
    }

    /*
     * 步骤2：如果当前文件正在写入，先同步一次。
     * 这样 read 命令看到的数据尽量接近最新写入内容。
     */
    if (g_data_logger_status.file_open != 0U)
    {
        result = f_sync(active_file);
        DataLogger_SetFsResult(result);
        if (result != FR_OK)
        {
            printf("sync before read failed, ret=%d\r\n", (int)result);
            return;
        }
    }

    /* 步骤3：以只读方式重新打开当前日志文件。 */
    result = f_open(&g_data_logger_read_file,
                    g_data_logger_status.current_file,
                    FA_READ | FA_OPEN_EXISTING);
    if (result != FR_OK)
    {
        DataLogger_SetFsResult(result);
        printf("read open failed, ret=%d\r\n", (int)result);
        return;
    }

    /* 步骤4：分块读取并通过 USART1 输出，最多输出 DATA_LOGGER_READ_MAX_BYTES。 */
    printf("----- %s -----\r\n", g_data_logger_status.current_file);
    do
    {
        to_read = sizeof(g_data_logger_read_buf);
        if ((total + to_read) > DATA_LOGGER_READ_MAX_BYTES)
        {
            to_read = (UINT)(DATA_LOGGER_READ_MAX_BYTES - total);
        }

        result = f_read(&g_data_logger_read_file,
                        g_data_logger_read_buf,
                        to_read,
                        &read_len);
        if (result != FR_OK)
        {
            DataLogger_SetFsResult(result);
            break;
        }

        for (i = 0; i < read_len; i++)
        {
            USART1_SendByte(USART1, (char)g_data_logger_read_buf[i]);
        }
        total += read_len;
    } while ((read_len > 0U) && (total < DATA_LOGGER_READ_MAX_BYTES));

    /* 步骤5：如果达到输出上限，提示日志内容被截断。 */
    if (total >= DATA_LOGGER_READ_MAX_BYTES)
    {
        printf("\r\n----- truncated at %lu bytes -----\r\n",
               (unsigned long)DATA_LOGGER_READ_MAX_BYTES);
    }
    printf("\r\n----- end -----\r\n");

    /* 步骤6：关闭只读文件对象。 */
    result = f_close(&g_data_logger_read_file);
    DataLogger_SetFsResult(result);
}

/*
 * 函数功能：
 *   停止采集并重新格式化 SPI Flash 上的 FatFs 文件系统。
 */
void DataLogger_HandleFormat(FATFS *fs, FIL *file)
{
    FRESULT result;     /* FatFs API 返回值。 */
    MKFS_PARM opt;      /* f_mkfs 格式化参数。 */

    /* 步骤1：格式化前必须停止采集，避免一边写文件一边擦写文件系统。 */
    g_data_logger_status.running = 0U;
    g_data_logger_status.paused = 0U;

    /* 步骤2：尽量写完队列残留样本，然后关闭当前日志文件。 */
    DataLogger_DrainSamplesToFile(file);
    DataLogger_CloseLog(file);

    /* 步骤3：卸载当前文件系统。 */
    f_mount(NULL, "0:/", 0);

    /* 步骤4：配置 FatFs 格式化参数，0 表示使用 FatFs 推荐默认值。 */
    opt.fmt = FM_FAT;
    opt.n_fat = 0;
    opt.align = 0;
    opt.n_root = 0;
    opt.au_size = 0;

    /* 步骤5：执行格式化。 */
    printf("format start\r\n");
    result = f_mkfs("0:/", &opt, g_data_logger_mkfs_work, sizeof(g_data_logger_mkfs_work));
    DataLogger_SetFsResult(result);
    if (result != FR_OK)
    {
        g_data_logger_status.storage_ready = 0U;
        printf("format failed, ret=%d\r\n", (int)result);
        return;
    }

    /* 步骤6：格式化成功后重新挂载文件系统，并清除当前文件名。 */
    result = DataLogger_Mount(fs);
    if (result == FR_OK)
    {
        g_data_logger_status.current_file[0] = '\0';
        g_data_logger_status.error = 0U;
        printf("format ok\r\n");
    }
    else
    {
        printf("format ok, remount failed ret=%d\r\n", (int)result);
    }
}

/*
 * 函数功能：
 *   把一条采样数据格式化为 CSV 行，并写入当前日志文件。
 */
FRESULT DataLogger_WriteSample(FIL *file, const data_logger_sample_t *sample)
{
    char line[96];          /* 一行 CSV 文本缓冲区。 */
    int len;                /* sprintf 生成的字符串长度。 */
    UINT written = 0;       /* f_write 实际写入的字节数。 */
    FRESULT result;         /* FatFs API 返回值。 */

    /* 步骤1：把样本结构体格式化成 CSV 文本行。 */
    len = sprintf(line, "%lu,%lu,%u,%u,%u,%u\r\n",
                  (unsigned long)sample->seq,
                  (unsigned long)sample->tick_ms,
                  (unsigned int)sample->raw0,
                  (unsigned int)sample->raw1,
                  (unsigned int)sample->mv0,
                  (unsigned int)sample->mv1);

    /* 步骤2：如果格式化失败，返回参数错误。 */
    if (len <= 0)
    {
        return FR_INVALID_PARAMETER;
    }

    /* 步骤3：写入文件，并检查实际写入长度是否完整。 */
    result = f_write(file, line, (UINT)len, &written);
    if ((result == FR_OK) && (written != (UINT)len))
    {
        result = FR_DISK_ERR;
    }

    /* 步骤4：保存 FatFs 结果，供 status 和错误 LED 使用。 */
    DataLogger_SetFsResult(result);
    return result;
}

/*
 * 函数功能：
 *   统一记录最近一次 FatFs 返回值，并在出错时置位 error 标志。
 */
void DataLogger_SetFsResult(FRESULT result)
{
    /* 步骤1：保存最近一次文件系统操作结果。 */
    g_data_logger_status.last_fs_result = result;

    /* 步骤2：任何非 FR_OK 都认为是错误，用 LED3 和 status 提示。 */
    if (result != FR_OK)
    {
        g_data_logger_status.error = 1U;
    }
}
