/**
  ******************************************************************************
  * @file    stm32f4xx_dma.h
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   This file contains all the functions prototypes for the DMA firmware 
  *          library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************  
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_DMA_H
#define __STM32F4xx_DMA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DMA
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  DMA Init structure definition
  */

typedef struct
{
  uint32_t DMA_Channel;            /*!< 指定用于特定流的通道。
                                        此参数可以是 @ref DMA_channel 的值 */
 
  uint32_t DMA_PeripheralBaseAddr; /*!< 指定DMAy流x的外设基地址。 */

  uint32_t DMA_Memory0BaseAddr;    /*!< 指定DMAy流x的内存0基地址。
                                        当双缓冲模式未启用时，此内存是默认使用的内存。 */

  uint32_t DMA_DIR;                /*!< 指定数据是从内存传输到外设，
                                        从内存到内存还是从外设到内存。
                                        此参数可以是 @ref DMA_data_transfer_direction 的值 */

  uint32_t DMA_BufferSize;         /*!< 指定特定流的数据单元缓冲区大小。
                                        数据单元等于DMA_PeripheralDataSize或DMA_MemoryDataSize成员
                                        中根据传输方向设置的配置。 */

  uint32_t DMA_PeripheralInc;      /*!< 指定外设地址寄存器是否应该递增。
                                        此参数可以是 @ref DMA_peripheral_incremented_mode 的值 */

  uint32_t DMA_MemoryInc;          /*!< 指定内存地址寄存器是否应该递增。
                                        此参数可以是 @ref DMA_memory_incremented_mode 的值 */

  uint32_t DMA_PeripheralDataSize; /*!< 指定外设数据宽度。
                                        此参数可以是 @ref DMA_peripheral_data_size 的值 */

  uint32_t DMA_MemoryDataSize;     /*!< 指定内存数据宽度。
                                        此参数可以是 @ref DMA_memory_data_size 的值 */

  uint32_t DMA_Mode;               /*!< 指定DMAy流x的操作模式。
                                        此参数可以是 @ref DMA_circular_normal_mode 的值
                                        @note 如果在选定流上配置了内存到内存的数据传输，
                                              则不能使用循环缓冲模式 */

  uint32_t DMA_Priority;           /*!< 指定DMAy流x的软件优先级。
                                        此参数可以是 @ref DMA_priority_level 的值 */

  uint32_t DMA_FIFOMode;          /*!< 指定将为特定流使用FIFO模式还是直接模式。
                                        此参数可以是 @ref DMA_fifo_direct_mode 的值
                                        @note 如果在选定流上配置了内存到内存的数据传输，
                                               则不能使用直接模式（禁用FIFO模式） */

  uint32_t DMA_FIFOThreshold;      /*!< 指定FIFO阈值级别。
                                        此参数可以是 @ref DMA_fifo_threshold_level 的值 */

  uint32_t DMA_MemoryBurst;        /*!< 指定内存传输的突发传输配置。
                                        它指定了在单个不可中断事务中要传输的数据量。此参数可以是 @ref DMA_memory_burst 的值
                                        @note 仅当启用了地址递增模式时，突发模式才可行。 */

  uint32_t DMA_PeripheralBurst;    /*!< 指定外设传输的突发传输配置。
                                        它指定了在单个不可中断事务中要传输的数据量。此参数可以是 @ref DMA_peripheral_burst 的值
                                        @note 仅当启用了地址递增模式时，突发模式才可行。 */  
}DMA_InitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup DMA_Exported_Constants
  * @{
  */

#define IS_DMA_ALL_PERIPH(PERIPH) (((PERIPH) == DMA1_Stream0) || \
                                   ((PERIPH) == DMA1_Stream1) || \
                                   ((PERIPH) == DMA1_Stream2) || \
                                   ((PERIPH) == DMA1_Stream3) || \
                                   ((PERIPH) == DMA1_Stream4) || \
                                   ((PERIPH) == DMA1_Stream5) || \
                                   ((PERIPH) == DMA1_Stream6) || \
                                   ((PERIPH) == DMA1_Stream7) || \
                                   ((PERIPH) == DMA2_Stream0) || \
                                   ((PERIPH) == DMA2_Stream1) || \
                                   ((PERIPH) == DMA2_Stream2) || \
                                   ((PERIPH) == DMA2_Stream3) || \
                                   ((PERIPH) == DMA2_Stream4) || \
                                   ((PERIPH) == DMA2_Stream5) || \
                                   ((PERIPH) == DMA2_Stream6) || \
                                   ((PERIPH) == DMA2_Stream7))

#define IS_DMA_ALL_CONTROLLER(CONTROLLER) (((CONTROLLER) == DMA1) || \
                                           ((CONTROLLER) == DMA2))

/** @defgroup DMA_channel 
  * @{
  */ 
#define DMA_Channel_0                     ((uint32_t)0x00000000)
#define DMA_Channel_1                     ((uint32_t)0x02000000)
#define DMA_Channel_2                     ((uint32_t)0x04000000)
#define DMA_Channel_3                     ((uint32_t)0x06000000)
#define DMA_Channel_4                     ((uint32_t)0x08000000)
#define DMA_Channel_5                     ((uint32_t)0x0A000000)
#define DMA_Channel_6                     ((uint32_t)0x0C000000)
#define DMA_Channel_7                     ((uint32_t)0x0E000000)

#define IS_DMA_CHANNEL(CHANNEL) (((CHANNEL) == DMA_Channel_0) || \
                                 ((CHANNEL) == DMA_Channel_1) || \
                                 ((CHANNEL) == DMA_Channel_2) || \
                                 ((CHANNEL) == DMA_Channel_3) || \
                                 ((CHANNEL) == DMA_Channel_4) || \
                                 ((CHANNEL) == DMA_Channel_5) || \
                                 ((CHANNEL) == DMA_Channel_6) || \
                                 ((CHANNEL) == DMA_Channel_7))
/**
  * @}
  */ 


/** @defgroup DMA_data_transfer_direction 
  * @{
  */ 
#define DMA_DIR_PeripheralToMemory        ((uint32_t)0x00000000)
#define DMA_DIR_MemoryToPeripheral        ((uint32_t)0x00000040) 
#define DMA_DIR_MemoryToMemory            ((uint32_t)0x00000080)

#define IS_DMA_DIRECTION(DIRECTION) (((DIRECTION) == DMA_DIR_PeripheralToMemory ) || \
                                     ((DIRECTION) == DMA_DIR_MemoryToPeripheral)  || \
                                     ((DIRECTION) == DMA_DIR_MemoryToMemory)) 
/**
  * @}
  */ 


/** @defgroup DMA_data_buffer_size 
  * @{
  */ 
#define IS_DMA_BUFFER_SIZE(SIZE) (((SIZE) >= 0x1) && ((SIZE) < 0x10000))
/**
  * @}
  */ 


/** @defgroup DMA_peripheral_incremented_mode 
  * @{
  */ 
#define DMA_PeripheralInc_Enable          ((uint32_t)0x00000200)
#define DMA_PeripheralInc_Disable         ((uint32_t)0x00000000)

#define IS_DMA_PERIPHERAL_INC_STATE(STATE) (((STATE) == DMA_PeripheralInc_Enable) || \
                                            ((STATE) == DMA_PeripheralInc_Disable))
/**
  * @}
  */ 


/** @defgroup DMA_memory_incremented_mode 
  * @{
  */ 
#define DMA_MemoryInc_Enable              ((uint32_t)0x00000400)
#define DMA_MemoryInc_Disable             ((uint32_t)0x00000000)

#define IS_DMA_MEMORY_INC_STATE(STATE) (((STATE) == DMA_MemoryInc_Enable) || \
                                        ((STATE) == DMA_MemoryInc_Disable))
/**
  * @}
  */ 


/** @defgroup DMA_peripheral_data_size 
  * @{
  */ 
#define DMA_PeripheralDataSize_Byte       ((uint32_t)0x00000000) 
#define DMA_PeripheralDataSize_HalfWord   ((uint32_t)0x00000800) 
#define DMA_PeripheralDataSize_Word       ((uint32_t)0x00001000)

#define IS_DMA_PERIPHERAL_DATA_SIZE(SIZE) (((SIZE) == DMA_PeripheralDataSize_Byte)  || \
                                           ((SIZE) == DMA_PeripheralDataSize_HalfWord) || \
                                           ((SIZE) == DMA_PeripheralDataSize_Word))
/**
  * @}
  */ 


/** @defgroup DMA_memory_data_size 
  * @{
  */ 
#define DMA_MemoryDataSize_Byte           ((uint32_t)0x00000000) 
#define DMA_MemoryDataSize_HalfWord       ((uint32_t)0x00002000) 
#define DMA_MemoryDataSize_Word           ((uint32_t)0x00004000)

#define IS_DMA_MEMORY_DATA_SIZE(SIZE) (((SIZE) == DMA_MemoryDataSize_Byte)  || \
                                       ((SIZE) == DMA_MemoryDataSize_HalfWord) || \
                                       ((SIZE) == DMA_MemoryDataSize_Word ))
/**
  * @}
  */ 


/** @defgroup DMA_circular_normal_mode 
  * @{
  */ 
#define DMA_Mode_Normal                   ((uint32_t)0x00000000)    //正常模式
#define DMA_Mode_Circular                 ((uint32_t)0x00000100)    //循环模式

#define IS_DMA_MODE(MODE) (((MODE) == DMA_Mode_Normal ) || \
                           ((MODE) == DMA_Mode_Circular)) 
/**
  * @}
  */ 


/** @defgroup DMA_priority_level 
  * @{
  */ 
#define DMA_Priority_Low                  ((uint32_t)0x00000000)
#define DMA_Priority_Medium               ((uint32_t)0x00010000) 
#define DMA_Priority_High                 ((uint32_t)0x00020000)
#define DMA_Priority_VeryHigh             ((uint32_t)0x00030000)

#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_Priority_Low )   || \
                                   ((PRIORITY) == DMA_Priority_Medium) || \
                                   ((PRIORITY) == DMA_Priority_High)   || \
                                   ((PRIORITY) == DMA_Priority_VeryHigh)) 
/**
  * @}
  */ 


/** @defgroup DMA_fifo_direct_mode 
  * @{
  */ 
#define DMA_FIFOMode_Disable              ((uint32_t)0x00000000) 
#define DMA_FIFOMode_Enable               ((uint32_t)0x00000004)

#define IS_DMA_FIFO_MODE_STATE(STATE) (((STATE) == DMA_FIFOMode_Disable ) || \
                                       ((STATE) == DMA_FIFOMode_Enable)) 
/**
  * @}
  */ 


/** @defgroup DMA_fifo_threshold_level 
  * @{
  */ 
#define DMA_FIFOThreshold_1QuarterFull    ((uint32_t)0x00000000)
#define DMA_FIFOThreshold_HalfFull        ((uint32_t)0x00000001) 
#define DMA_FIFOThreshold_3QuartersFull   ((uint32_t)0x00000002)
#define DMA_FIFOThreshold_Full            ((uint32_t)0x00000003)

#define IS_DMA_FIFO_THRESHOLD(THRESHOLD) (((THRESHOLD) == DMA_FIFOThreshold_1QuarterFull ) || \
                                          ((THRESHOLD) == DMA_FIFOThreshold_HalfFull)      || \
                                          ((THRESHOLD) == DMA_FIFOThreshold_3QuartersFull) || \
                                          ((THRESHOLD) == DMA_FIFOThreshold_Full)) 
/**
  * @}
  */ 


/** @defgroup DMA_memory_burst 
  * @{
  */ 
#define DMA_MemoryBurst_Single            ((uint32_t)0x00000000)
#define DMA_MemoryBurst_INC4              ((uint32_t)0x00800000)  
#define DMA_MemoryBurst_INC8              ((uint32_t)0x01000000)
#define DMA_MemoryBurst_INC16             ((uint32_t)0x01800000)

#define IS_DMA_MEMORY_BURST(BURST) (((BURST) == DMA_MemoryBurst_Single) || \
                                    ((BURST) == DMA_MemoryBurst_INC4)  || \
                                    ((BURST) == DMA_MemoryBurst_INC8)  || \
                                    ((BURST) == DMA_MemoryBurst_INC16))
/**
  * @}
  */ 


/** @defgroup DMA_peripheral_burst 
  * @{
  */ 
#define DMA_PeripheralBurst_Single        ((uint32_t)0x00000000)
#define DMA_PeripheralBurst_INC4          ((uint32_t)0x00200000)  
#define DMA_PeripheralBurst_INC8          ((uint32_t)0x00400000)
#define DMA_PeripheralBurst_INC16         ((uint32_t)0x00600000)

#define IS_DMA_PERIPHERAL_BURST(BURST) (((BURST) == DMA_PeripheralBurst_Single) || \
                                        ((BURST) == DMA_PeripheralBurst_INC4)  || \
                                        ((BURST) == DMA_PeripheralBurst_INC8)  || \
                                        ((BURST) == DMA_PeripheralBurst_INC16))
/**
  * @}
  */ 


/** @defgroup DMA_fifo_status_level 
  * @{
  */
#define DMA_FIFOStatus_Less1QuarterFull   ((uint32_t)0x00000000 << 3)
#define DMA_FIFOStatus_1QuarterFull       ((uint32_t)0x00000001 << 3)
#define DMA_FIFOStatus_HalfFull           ((uint32_t)0x00000002 << 3) 
#define DMA_FIFOStatus_3QuartersFull      ((uint32_t)0x00000003 << 3)
#define DMA_FIFOStatus_Empty              ((uint32_t)0x00000004 << 3)
#define DMA_FIFOStatus_Full               ((uint32_t)0x00000005 << 3)

#define IS_DMA_FIFO_STATUS(STATUS) (((STATUS) == DMA_FIFOStatus_Less1QuarterFull ) || \
                                    ((STATUS) == DMA_FIFOStatus_HalfFull)          || \
                                    ((STATUS) == DMA_FIFOStatus_1QuarterFull)      || \
                                    ((STATUS) == DMA_FIFOStatus_3QuartersFull)     || \
                                    ((STATUS) == DMA_FIFOStatus_Full)              || \
                                    ((STATUS) == DMA_FIFOStatus_Empty)) 
/**
  * @}
  */ 

/** @defgroup DMA_flags_definition 
  * @{
  */
#define DMA_FLAG_FEIF0                    ((uint32_t)0x10800001)
#define DMA_FLAG_DMEIF0                   ((uint32_t)0x10800004)
#define DMA_FLAG_TEIF0                    ((uint32_t)0x10000008)
#define DMA_FLAG_HTIF0                    ((uint32_t)0x10000010)
#define DMA_FLAG_TCIF0                    ((uint32_t)0x10000020)
#define DMA_FLAG_FEIF1                    ((uint32_t)0x10000040)
#define DMA_FLAG_DMEIF1                   ((uint32_t)0x10000100)
#define DMA_FLAG_TEIF1                    ((uint32_t)0x10000200)
#define DMA_FLAG_HTIF1                    ((uint32_t)0x10000400)
#define DMA_FLAG_TCIF1                    ((uint32_t)0x10000800)
#define DMA_FLAG_FEIF2                    ((uint32_t)0x10010000)
#define DMA_FLAG_DMEIF2                   ((uint32_t)0x10040000)
#define DMA_FLAG_TEIF2                    ((uint32_t)0x10080000)
#define DMA_FLAG_HTIF2                    ((uint32_t)0x10100000)
#define DMA_FLAG_TCIF2                    ((uint32_t)0x10200000)
#define DMA_FLAG_FEIF3                    ((uint32_t)0x10400000)
#define DMA_FLAG_DMEIF3                   ((uint32_t)0x11000000)
#define DMA_FLAG_TEIF3                    ((uint32_t)0x12000000)
#define DMA_FLAG_HTIF3                    ((uint32_t)0x14000000)
#define DMA_FLAG_TCIF3                    ((uint32_t)0x18000000)
#define DMA_FLAG_FEIF4                    ((uint32_t)0x20000001)
#define DMA_FLAG_DMEIF4                   ((uint32_t)0x20000004)
#define DMA_FLAG_TEIF4                    ((uint32_t)0x20000008)
#define DMA_FLAG_HTIF4                    ((uint32_t)0x20000010)
#define DMA_FLAG_TCIF4                    ((uint32_t)0x20000020)
#define DMA_FLAG_FEIF5                    ((uint32_t)0x20000040)
#define DMA_FLAG_DMEIF5                   ((uint32_t)0x20000100)
#define DMA_FLAG_TEIF5                    ((uint32_t)0x20000200)
#define DMA_FLAG_HTIF5                    ((uint32_t)0x20000400)
#define DMA_FLAG_TCIF5                    ((uint32_t)0x20000800)
#define DMA_FLAG_FEIF6                    ((uint32_t)0x20010000)
#define DMA_FLAG_DMEIF6                   ((uint32_t)0x20040000)
#define DMA_FLAG_TEIF6                    ((uint32_t)0x20080000)
#define DMA_FLAG_HTIF6                    ((uint32_t)0x20100000)
#define DMA_FLAG_TCIF6                    ((uint32_t)0x20200000)
#define DMA_FLAG_FEIF7                    ((uint32_t)0x20400000)
#define DMA_FLAG_DMEIF7                   ((uint32_t)0x21000000)
#define DMA_FLAG_TEIF7                    ((uint32_t)0x22000000)
#define DMA_FLAG_HTIF7                    ((uint32_t)0x24000000)
#define DMA_FLAG_TCIF7                    ((uint32_t)0x28000000)

#define IS_DMA_CLEAR_FLAG(FLAG) ((((FLAG) & 0x30000000) != 0x30000000) && (((FLAG) & 0x30000000) != 0) && \
                                 (((FLAG) & 0xC002F082) == 0x00) && ((FLAG) != 0x00))

#define IS_DMA_GET_FLAG(FLAG) (((FLAG) == DMA_FLAG_TCIF0)  || ((FLAG) == DMA_FLAG_HTIF0)  || \
                               ((FLAG) == DMA_FLAG_TEIF0)  || ((FLAG) == DMA_FLAG_DMEIF0) || \
                               ((FLAG) == DMA_FLAG_FEIF0)  || ((FLAG) == DMA_FLAG_TCIF1)  || \
                               ((FLAG) == DMA_FLAG_HTIF1)  || ((FLAG) == DMA_FLAG_TEIF1)  || \
                               ((FLAG) == DMA_FLAG_DMEIF1) || ((FLAG) == DMA_FLAG_FEIF1)  || \
                               ((FLAG) == DMA_FLAG_TCIF2)  || ((FLAG) == DMA_FLAG_HTIF2)  || \
                               ((FLAG) == DMA_FLAG_TEIF2)  || ((FLAG) == DMA_FLAG_DMEIF2) || \
                               ((FLAG) == DMA_FLAG_FEIF2)  || ((FLAG) == DMA_FLAG_TCIF3)  || \
                               ((FLAG) == DMA_FLAG_HTIF3)  || ((FLAG) == DMA_FLAG_TEIF3)  || \
                               ((FLAG) == DMA_FLAG_DMEIF3) || ((FLAG) == DMA_FLAG_FEIF3)  || \
                               ((FLAG) == DMA_FLAG_TCIF4)  || ((FLAG) == DMA_FLAG_HTIF4)  || \
                               ((FLAG) == DMA_FLAG_TEIF4)  || ((FLAG) == DMA_FLAG_DMEIF4) || \
                               ((FLAG) == DMA_FLAG_FEIF4)  || ((FLAG) == DMA_FLAG_TCIF5)  || \
                               ((FLAG) == DMA_FLAG_HTIF5)  || ((FLAG) == DMA_FLAG_TEIF5)  || \
                               ((FLAG) == DMA_FLAG_DMEIF5) || ((FLAG) == DMA_FLAG_FEIF5)  || \
                               ((FLAG) == DMA_FLAG_TCIF6)  || ((FLAG) == DMA_FLAG_HTIF6)  || \
                               ((FLAG) == DMA_FLAG_TEIF6)  || ((FLAG) == DMA_FLAG_DMEIF6) || \
                               ((FLAG) == DMA_FLAG_FEIF6)  || ((FLAG) == DMA_FLAG_TCIF7)  || \
                               ((FLAG) == DMA_FLAG_HTIF7)  || ((FLAG) == DMA_FLAG_TEIF7)  || \
                               ((FLAG) == DMA_FLAG_DMEIF7) || ((FLAG) == DMA_FLAG_FEIF7))
/**
  * @}
  */ 


/** @defgroup DMA_interrupt_enable_definitions 
  * @{
  */ 
#define DMA_IT_TC                         ((uint32_t)0x00000010)
#define DMA_IT_HT                         ((uint32_t)0x00000008)
#define DMA_IT_TE                         ((uint32_t)0x00000004)
#define DMA_IT_DME                        ((uint32_t)0x00000002)
#define DMA_IT_FE                         ((uint32_t)0x00000080)

#define IS_DMA_CONFIG_IT(IT) ((((IT) & 0xFFFFFF61) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */ 


/** @defgroup DMA_interrupts_definitions 
  * @{
  */ 
#define DMA_IT_FEIF0                      ((uint32_t)0x90000001)// FIFO出错
#define DMA_IT_DMEIF0                     ((uint32_t)0x10001004)// 直接模式出错
#define DMA_IT_TEIF0                      ((uint32_t)0x10002008)// DMA传输出错
#define DMA_IT_HTIF0                      ((uint32_t)0x10004010)// DMA传了一半
#define DMA_IT_TCIF0                      ((uint32_t)0x10008020)// DMA传输完成
#define DMA_IT_FEIF1                      ((uint32_t)0x90000040)
#define DMA_IT_DMEIF1                     ((uint32_t)0x10001100)
#define DMA_IT_TEIF1                      ((uint32_t)0x10002200)
#define DMA_IT_HTIF1                      ((uint32_t)0x10004400)
#define DMA_IT_TCIF1                      ((uint32_t)0x10008800)
#define DMA_IT_FEIF2                      ((uint32_t)0x90010000)
#define DMA_IT_DMEIF2                     ((uint32_t)0x10041000)
#define DMA_IT_TEIF2                      ((uint32_t)0x10082000)
#define DMA_IT_HTIF2                      ((uint32_t)0x10104000)
#define DMA_IT_TCIF2                      ((uint32_t)0x10208000)
#define DMA_IT_FEIF3                      ((uint32_t)0x90400000)
#define DMA_IT_DMEIF3                     ((uint32_t)0x11001000)
#define DMA_IT_TEIF3                      ((uint32_t)0x12002000)
#define DMA_IT_HTIF3                      ((uint32_t)0x14004000)
#define DMA_IT_TCIF3                      ((uint32_t)0x18008000)
#define DMA_IT_FEIF4                      ((uint32_t)0xA0000001)
#define DMA_IT_DMEIF4                     ((uint32_t)0x20001004)
#define DMA_IT_TEIF4                      ((uint32_t)0x20002008)
#define DMA_IT_HTIF4                      ((uint32_t)0x20004010)
#define DMA_IT_TCIF4                      ((uint32_t)0x20008020)
#define DMA_IT_FEIF5                      ((uint32_t)0xA0000040)
#define DMA_IT_DMEIF5                     ((uint32_t)0x20001100)
#define DMA_IT_TEIF5                      ((uint32_t)0x20002200)
#define DMA_IT_HTIF5                      ((uint32_t)0x20004400)
#define DMA_IT_TCIF5                      ((uint32_t)0x20008800)
#define DMA_IT_FEIF6                      ((uint32_t)0xA0010000)
#define DMA_IT_DMEIF6                     ((uint32_t)0x20041000)
#define DMA_IT_TEIF6                      ((uint32_t)0x20082000)
#define DMA_IT_HTIF6                      ((uint32_t)0x20104000)
#define DMA_IT_TCIF6                      ((uint32_t)0x20208000)
#define DMA_IT_FEIF7                      ((uint32_t)0xA0400000)
#define DMA_IT_DMEIF7                     ((uint32_t)0x21001000)
#define DMA_IT_TEIF7                      ((uint32_t)0x22002000)
#define DMA_IT_HTIF7                      ((uint32_t)0x24004000)
#define DMA_IT_TCIF7                      ((uint32_t)0x28008000)

#define IS_DMA_CLEAR_IT(IT) ((((IT) & 0x30000000) != 0x30000000) && \
                             (((IT) & 0x30000000) != 0) && ((IT) != 0x00) && \
                             (((IT) & 0x40820082) == 0x00))

#define IS_DMA_GET_IT(IT) (((IT) == DMA_IT_TCIF0) || ((IT) == DMA_IT_HTIF0)  || \
                           ((IT) == DMA_IT_TEIF0) || ((IT) == DMA_IT_DMEIF0) || \
                           ((IT) == DMA_IT_FEIF0) || ((IT) == DMA_IT_TCIF1)  || \
                           ((IT) == DMA_IT_HTIF1) || ((IT) == DMA_IT_TEIF1)  || \
                           ((IT) == DMA_IT_DMEIF1)|| ((IT) == DMA_IT_FEIF1)  || \
                           ((IT) == DMA_IT_TCIF2) || ((IT) == DMA_IT_HTIF2)  || \
                           ((IT) == DMA_IT_TEIF2) || ((IT) == DMA_IT_DMEIF2) || \
                           ((IT) == DMA_IT_FEIF2) || ((IT) == DMA_IT_TCIF3)  || \
                           ((IT) == DMA_IT_HTIF3) || ((IT) == DMA_IT_TEIF3)  || \
                           ((IT) == DMA_IT_DMEIF3)|| ((IT) == DMA_IT_FEIF3)  || \
                           ((IT) == DMA_IT_TCIF4) || ((IT) == DMA_IT_HTIF4)  || \
                           ((IT) == DMA_IT_TEIF4) || ((IT) == DMA_IT_DMEIF4) || \
                           ((IT) == DMA_IT_FEIF4) || ((IT) == DMA_IT_TCIF5)  || \
                           ((IT) == DMA_IT_HTIF5) || ((IT) == DMA_IT_TEIF5)  || \
                           ((IT) == DMA_IT_DMEIF5)|| ((IT) == DMA_IT_FEIF5)  || \
                           ((IT) == DMA_IT_TCIF6) || ((IT) == DMA_IT_HTIF6)  || \
                           ((IT) == DMA_IT_TEIF6) || ((IT) == DMA_IT_DMEIF6) || \
                           ((IT) == DMA_IT_FEIF6) || ((IT) == DMA_IT_TCIF7)  || \
                           ((IT) == DMA_IT_HTIF7) || ((IT) == DMA_IT_TEIF7)  || \
                           ((IT) == DMA_IT_DMEIF7)|| ((IT) == DMA_IT_FEIF7))
/**
  * @}
  */ 


/** @defgroup DMA_peripheral_increment_offset 
  * @{
  */ 
#define DMA_PINCOS_Psize                  ((uint32_t)0x00000000)
#define DMA_PINCOS_WordAligned            ((uint32_t)0x00008000)

#define IS_DMA_PINCOS_SIZE(SIZE) (((SIZE) == DMA_PINCOS_Psize) || \
                                  ((SIZE) == DMA_PINCOS_WordAligned))
/**
  * @}
  */ 


/** @defgroup DMA_flow_controller_definitions 
  * @{
  */ 
#define DMA_FlowCtrl_Memory               ((uint32_t)0x00000000)
#define DMA_FlowCtrl_Peripheral           ((uint32_t)0x00000020)

#define IS_DMA_FLOW_CTRL(CTRL) (((CTRL) == DMA_FlowCtrl_Memory) || \
                                ((CTRL) == DMA_FlowCtrl_Peripheral))
/**
  * @}
  */ 


/** @defgroup DMA_memory_targets_definitions 
  * @{
  */ 
#define DMA_Memory_0                      ((uint32_t)0x00000000)
#define DMA_Memory_1                      ((uint32_t)0x00080000)

#define IS_DMA_CURRENT_MEM(MEM) (((MEM) == DMA_Memory_0) || ((MEM) == DMA_Memory_1))
/**
  * @}
  */ 

/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/ 

/*  Function used to set the DMA configuration to the default reset state *****/ 
void DMA_DeInit(DMA_Stream_TypeDef* DMAy_Streamx);//DMA流恢复默认复位状态函数。参数1：流

/* Initialization and Configuration functions *********************************/
void DMA_Init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct);//DMA结构体配置函数。参数1：流。参数2：结构体
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);//DMA结构体默认值初始化函数。参数1：结构体
void DMA_Cmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);//DMA流使能或失能函数。参数1：流。参数2：使能或失能状态

/* Optional Configuration functions *******************************************/
void DMA_PeriphIncOffsetSizeConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_Pincos);//DMA外设地址递增偏移大小配置函数。参数1：流。参数2：外设递增偏移大小
void DMA_FlowControllerConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FlowCtrl);//DMA流控制器配置函数。参数1：流。参数2：流控制器

/* Data Counter functions *****************************************************/
void DMA_SetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx, uint16_t Counter);//DMA当前传输数据计数器设置函数。参数1：流。参数2：数据单元数量
uint16_t DMA_GetCurrDataCounter(DMA_Stream_TypeDef* DMAy_Streamx);//DMA当前剩余数据计数器获取函数。参数1：流。返回值：剩余数据单元数量

/* Double Buffer mode functions ***********************************************/
void DMA_DoubleBufferModeConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t Memory1BaseAddr,
                                uint32_t DMA_CurrentMemory);//DMA双缓冲模式配置函数。参数1：流。参数2：内存1基地址。参数3：当前内存目标
void DMA_DoubleBufferModeCmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);//DMA双缓冲模式使能或失能函数。参数1：流。参数2：使能或失能状态
void DMA_MemoryTargetConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t MemoryBaseAddr,
                            uint32_t DMA_MemoryTarget);//DMA双缓冲内存目标地址配置函数。参数1：流。参数2：内存基地址。参数3：内存目标
uint32_t DMA_GetCurrentMemoryTarget(DMA_Stream_TypeDef* DMAy_Streamx);//DMA当前使用内存目标获取函数。参数1：流。返回值：当前内存目标0或1

/* Interrupts and flags management functions **********************************/
FunctionalState DMA_GetCmdStatus(DMA_Stream_TypeDef* DMAy_Streamx);//DMA流使能状态获取函数。参数1：流。返回值：ENABLE或DISABLE
uint32_t DMA_GetFIFOStatus(DMA_Stream_TypeDef* DMAy_Streamx);//DMA FIFO填充状态获取函数。参数1：流。返回值：FIFO填充状态
FlagStatus DMA_GetFlagStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG);//DMA标志位状态获取函数。参数1：流。参数2：标志位。返回值：SET或RESET
void DMA_ClearFlag(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG);//DMA标志位清除函数。参数1：流。参数2：要清除的标志位
void DMA_ITConfig(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT, FunctionalState NewState);//DMA中断配置函数。参数1：流。参数2：中断源。参数3：使能或失能状态
ITStatus DMA_GetITStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);//DMA中断状态获取函数。参数1：流。参数2：中断源。返回值：SET或RESET
void DMA_ClearITPendingBit(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_IT);//DMA中断挂起位清除函数。参数1：流。参数2：要清除的中断挂起位

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_DMA_H */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
