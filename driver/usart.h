#ifndef __USART_H__
#define __USART_H__

#include "stm32f4xx.h"       /* STM32F4 标准外设库 */
#include "interrupt_init.h"  /* USART1 NVIC 中断初始化 */
#include <stdint.h>          /* 标准整型定义 */
#include <stddef.h>          /* 标准定义（NULL 等） */
#include <stdio.h>           /* 标准 I/O（printf 等） */

/*==================== 硬件配置宏 ====================*/

#define USART1_TX_PORT        GPIOA               /**< TX 引脚所在端口 */
#define USART1_TX_PIN         GPIO_Pin_9          /**< TX 引脚：PA9 */
#define USART1_TX_CLK         RCC_AHB1Periph_GPIOA   /**< TX 端口时钟 */
#define USART1_TX_GPIO_SOURCE GPIO_PinSource9     /**< TX 引脚复用源编号 */

#define USART1_RX_PORT        GPIOA               /**< RX 引脚所在端口 */
#define USART1_RX_PIN         GPIO_Pin_10         /**< RX 引脚：PA10 */
#define USART1_RX_CLK         RCC_AHB1Periph_GPIOA   /**< RX 端口时钟 */
#define USART1_RX_GPIO_SOURCE GPIO_PinSource10    /**< RX 引脚复用源编号 */


/*==================== 缓冲区配置宏 ====================*/

#define USART1_CMD_MAX_LEN      64U   /**< 单条命令最大长度（含 '\0' 结束符） */
#define USART1_CMD_QUEUE_LEN     4U   /**< 命令队列容量（环形缓冲区） */

/*==================== 对外接口 ====================*/

void     usart1_init(void);                                              /**< 初始化 USART1 外设和 GPIO */
void     USART1_SendByte(USART_TypeDef *USARTx, char byte);              /**< 发送单个字节（阻塞等待 TXE 就绪） */
void     USART1_SendString(USART_TypeDef *USARTx, const char *str);      /**< 发送字符串（遇 '\0' 结束） */
void     USART1_RxIrqHandler(char ch);                                   /**< 接收中断处理：字符入缓冲，回车/换行完成命令 */
uint8_t  USART1_ReadLine(char *line, uint16_t max_len);                  /**< 从命令队列读取一行命令 */
uint32_t USART1_GetRxOverflowCount(void);                                /**< 获取接收溢出计数（用于采样丢失统计） */

#endif /* __USART_H__ */
