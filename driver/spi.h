/**
 * @file    spi.h
 * @brief   SPI Flash (W25Qxx) 驱动程序头文件
 * @details 基于 STM32F4 SPI1 外设驱动外部 SPI NOR Flash 存储器（如 W25Q64/W25Q128）。
 *          提供 Flash 初始化、字节收发、扇区擦除、页读取/写入、忙等待和 ID 读取功能。
 *          SPI 模式：全双工主机模式，CPOL=1/CPHA=1（模式3），MSB 先发，2分频。
 *          Flash 片选引脚：PG6，通过宏 SPI1_FLASH_Chose_ENABLE/DISABLE 控制。
 */

#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx.h"   /* STM32F4 标准外设库，提供 SPI/GPIO/RCC 等类型定义 */
#include "usart.h"       /* 串口模块，用于错误日志打印（printf） */
#include "led.h"         /* LED 模块，用于状态指示 */

/*==================== 宏定义 ====================*/

#define DummyByte  0x00   /**< SPI 通信中的哑字节（dummy byte）：
                               主机发送 0x00 以产生 SCK 时钟，用于从机回传数据时使用 */

/**
 * @brief Flash 片选（CS）控制宏
 * @note  PG6 低电平选中 Flash，高电平取消选中。
 *        每次 SPI 指令帧前后必须分别拉低/拉高片选信号。
 */
#define SPI1_FLASH_Chose_ENABLE     GPIO_ResetBits(GPIOG, GPIO_Pin_6)   /**< 拉低 PG6，使能 Flash 片选 */
#define SPI1_FLASH_Chose_DISABLE    GPIO_SetBits(GPIOG, GPIO_Pin_6)     /**< 拉高 PG6，禁用 Flash 片选 */

/*==================== 对外接口 ====================*/

void     SPI_Flash_Init(void);                                              /**< 初始化 SPI1 和 Flash GPIO */
uint8_t  SPI_Flash_SendandReceive_Byte(uint8_t data);                       /**< SPI 收发一个字节，返回接收数据 */
void     SPI_FLASH_Busy_State(void);                                        /**< 轮询等待 Flash 内部操作完成（BUSY 位清零） */
void     SPI_Flash_ClearFlash(uint32_t add);                                /**< 擦除指定扇区（扇区号 * 4096 字节） */
void     SPI_Flash_ReadDat(uint8_t *data, uint32_t add, uint32_t len);     /**< 从指定地址读取指定长度的数据 */
void     SPI_Flash_WriteDat(uint8_t *data, uint32_t add, uint32_t len);    /**< 向指定地址写入指定长度数据（自动跨页） */
uint32_t SPI_FLASH_DriverID(void);                                          /**< 读取 Flash 芯片 JEDEC ID（制造商+器件ID） */

#endif /* __SPI_H__ */
