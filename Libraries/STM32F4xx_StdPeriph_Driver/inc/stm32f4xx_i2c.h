/**
  ******************************************************************************
  * @file    stm32f4xx_i2c.h
  * @author  MCD 应用团队
  * @version V1.8.0
  * @date    2016年11月04日
  * @brief   本文件包含 I2C 固件库的所有函数原型。
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; 2016 STMicroelectronics 版权所有</center></h2>
  *
  * 本文件遵循 MCD-ST Liberty 软件许可协议 V2（以下简称“许可证”）授权；
  * 除非遵守许可证，否则不得使用本文件。
  * 你可以通过以下地址获取许可证副本：
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * 除非适用法律要求或书面同意，按许可证分发的软件均按“原样”提供，
  * 不附带任何形式的明示或暗示担保。
  * 有关许可证下的具体权限和限制，请参阅许可证。
  *
  ******************************************************************************  
  */ 

/* 防止递归包含的定义 --------------------------------------------------------*/
#ifndef __STM32F4xx_I2C_H
#define __STM32F4xx_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

/* 包含文件 ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup STM32F4xx_StdPeriph_Driver STM32F4xx 标准外设驱动
  * @{
  */

/** @addtogroup I2C I2C
  * @{
  */

/* 导出的类型 ---------------------------------------------------------------*/

/** 
  * @brief  I2C 初始化结构体定义  
  */

typedef struct
{
  uint32_t I2C_ClockSpeed;          /*!< 指定时钟频率。
                                         该参数必须设置为低于 400kHz 的值 */

  uint16_t I2C_Mode;                /*!< 指定 I2C 模式。
                                         该参数可取 @ref I2C_mode 中的值 */

  uint16_t I2C_DutyCycle;           /*!< 指定 I2C 快速模式占空比。
                                         该参数可取 @ref I2C_duty_cycle_in_fast_mode 中的值 */

  uint16_t I2C_OwnAddress1;         /*!< 指定设备的第一个自身地址。
                                         该参数可以是 7 位或 10 位地址。 */

  uint16_t I2C_Ack;                 /*!< 使能或失能应答。
                                         该参数可取 @ref I2C_acknowledgement 中的值 */

  uint16_t I2C_AcknowledgedAddress; /*!< 指定应答 7 位还是 10 位地址。
                                         该参数可取 @ref I2C_acknowledged_address 中的值 */
}I2C_InitTypeDef;

/* 导出的常量 ---------------------------------------------------------------*/


/** @defgroup I2C_Exported_Constants I2C 导出的常量
  * @{
  */
#define IS_I2C_ALL_PERIPH(PERIPH) (((PERIPH) == I2C1) || \
                                   ((PERIPH) == I2C2) || \
                                   ((PERIPH) == I2C3))
/** @defgroup I2C_Digital_Filter I2C 数字滤波器
  * @{
  */

#define IS_I2C_DIGITAL_FILTER(FILTER)   ((FILTER) <= 0x0000000F)
/**
  * @}
  */


/** @defgroup I2C_mode I2C 模式 
  * @{
  */

#define I2C_Mode_I2C                    ((uint16_t)0x0000)
#define I2C_Mode_SMBusDevice            ((uint16_t)0x0002)  
#define I2C_Mode_SMBusHost              ((uint16_t)0x000A)
#define IS_I2C_MODE(MODE) (((MODE) == I2C_Mode_I2C) || \
                           ((MODE) == I2C_Mode_SMBusDevice) || \
                           ((MODE) == I2C_Mode_SMBusHost))
/**
  * @}
  */

/** @defgroup I2C_duty_cycle_in_fast_mode I2C 快速模式占空比 
  * @{
  */

#define I2C_DutyCycle_16_9              ((uint16_t)0x4000) /*!< I2C 快速模式 Tlow/Thigh = 16/9 */
#define I2C_DutyCycle_2                 ((uint16_t)0xBFFF) /*!< I2C 快速模式 Tlow/Thigh = 2 */
#define IS_I2C_DUTY_CYCLE(CYCLE) (((CYCLE) == I2C_DutyCycle_16_9) || \
                                  ((CYCLE) == I2C_DutyCycle_2))
/**
  * @}
  */ 

/** @defgroup I2C_acknowledgement I2C 应答
  * @{
  */

#define I2C_Ack_Enable                  ((uint16_t)0x0400)
#define I2C_Ack_Disable                 ((uint16_t)0x0000)
#define IS_I2C_ACK_STATE(STATE) (((STATE) == I2C_Ack_Enable) || \
                                 ((STATE) == I2C_Ack_Disable))
/**
  * @}
  */

/** @defgroup I2C_transfer_direction I2C 传输方向 
  * @{
  */

#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
#define IS_I2C_DIRECTION(DIRECTION) (((DIRECTION) == I2C_Direction_Transmitter) || \
                                     ((DIRECTION) == I2C_Direction_Receiver))
/**
  * @}
  */

/** @defgroup I2C_acknowledged_address I2C 应答地址 
  * @{
  */

#define I2C_AcknowledgedAddress_7bit    ((uint16_t)0x4000)
#define I2C_AcknowledgedAddress_10bit   ((uint16_t)0xC000)
#define IS_I2C_ACKNOWLEDGE_ADDRESS(ADDRESS) (((ADDRESS) == I2C_AcknowledgedAddress_7bit) || \
                                             ((ADDRESS) == I2C_AcknowledgedAddress_10bit))
/**
  * @}
  */ 

/** @defgroup I2C_registers I2C 寄存器 
  * @{
  */

#define I2C_Register_CR1                ((uint8_t)0x00)
#define I2C_Register_CR2                ((uint8_t)0x04)
#define I2C_Register_OAR1               ((uint8_t)0x08)
#define I2C_Register_OAR2               ((uint8_t)0x0C)
#define I2C_Register_DR                 ((uint8_t)0x10)
#define I2C_Register_SR1                ((uint8_t)0x14)
#define I2C_Register_SR2                ((uint8_t)0x18)
#define I2C_Register_CCR                ((uint8_t)0x1C)
#define I2C_Register_TRISE              ((uint8_t)0x20)
#define IS_I2C_REGISTER(REGISTER) (((REGISTER) == I2C_Register_CR1) || \
                                   ((REGISTER) == I2C_Register_CR2) || \
                                   ((REGISTER) == I2C_Register_OAR1) || \
                                   ((REGISTER) == I2C_Register_OAR2) || \
                                   ((REGISTER) == I2C_Register_DR) || \
                                   ((REGISTER) == I2C_Register_SR1) || \
                                   ((REGISTER) == I2C_Register_SR2) || \
                                   ((REGISTER) == I2C_Register_CCR) || \
                                   ((REGISTER) == I2C_Register_TRISE))
/**
  * @}
  */

/** @defgroup I2C_NACK_position I2C NACK 位置 
  * @{
  */

#define I2C_NACKPosition_Next           ((uint16_t)0x0800)
#define I2C_NACKPosition_Current        ((uint16_t)0xF7FF)
#define IS_I2C_NACK_POSITION(POSITION)  (((POSITION) == I2C_NACKPosition_Next) || \
                                         ((POSITION) == I2C_NACKPosition_Current))
/**
  * @}
  */ 

/** @defgroup I2C_SMBus_alert_pin_level I2C SMBus 警报引脚电平 
  * @{
  */

#define I2C_SMBusAlert_Low              ((uint16_t)0x2000)
#define I2C_SMBusAlert_High             ((uint16_t)0xDFFF)
#define IS_I2C_SMBUS_ALERT(ALERT) (((ALERT) == I2C_SMBusAlert_Low) || \
                                   ((ALERT) == I2C_SMBusAlert_High))
/**
  * @}
  */

/** @defgroup I2C_PEC_position I2C PEC 位置 
  * @{
  */

#define I2C_PECPosition_Next            ((uint16_t)0x0800)
#define I2C_PECPosition_Current         ((uint16_t)0xF7FF)
#define IS_I2C_PEC_POSITION(POSITION) (((POSITION) == I2C_PECPosition_Next) || \
                                       ((POSITION) == I2C_PECPosition_Current))
/**
  * @}
  */ 

/** @defgroup I2C_interrupts_definition I2C 中断定义 
  * @{
  */

#define I2C_IT_BUF                      ((uint16_t)0x0400)
#define I2C_IT_EVT                      ((uint16_t)0x0200)
#define I2C_IT_ERR                      ((uint16_t)0x0100)
#define IS_I2C_CONFIG_IT(IT) ((((IT) & (uint16_t)0xF8FF) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */ 

/** @defgroup I2C_interrupts_definition I2C 中断定义 
  * @{
  */

#define I2C_IT_SMBALERT                 ((uint32_t)0x01008000)
#define I2C_IT_TIMEOUT                  ((uint32_t)0x01004000)
#define I2C_IT_PECERR                   ((uint32_t)0x01001000)
#define I2C_IT_OVR                      ((uint32_t)0x01000800)
#define I2C_IT_AF                       ((uint32_t)0x01000400)
#define I2C_IT_ARLO                     ((uint32_t)0x01000200)
#define I2C_IT_BERR                     ((uint32_t)0x01000100)
#define I2C_IT_TXE                      ((uint32_t)0x06000080)
#define I2C_IT_RXNE                     ((uint32_t)0x06000040)
#define I2C_IT_STOPF                    ((uint32_t)0x02000010)
#define I2C_IT_ADD10                    ((uint32_t)0x02000008)
#define I2C_IT_BTF                      ((uint32_t)0x02000004)
#define I2C_IT_ADDR                     ((uint32_t)0x02000002)
#define I2C_IT_SB                       ((uint32_t)0x02000001)

#define IS_I2C_CLEAR_IT(IT) ((((IT) & (uint16_t)0x20FF) == 0x00) && ((IT) != (uint16_t)0x00))

#define IS_I2C_GET_IT(IT) (((IT) == I2C_IT_SMBALERT) || ((IT) == I2C_IT_TIMEOUT) || \
                           ((IT) == I2C_IT_PECERR) || ((IT) == I2C_IT_OVR) || \
                           ((IT) == I2C_IT_AF) || ((IT) == I2C_IT_ARLO) || \
                           ((IT) == I2C_IT_BERR) || ((IT) == I2C_IT_TXE) || \
                           ((IT) == I2C_IT_RXNE) || ((IT) == I2C_IT_STOPF) || \
                           ((IT) == I2C_IT_ADD10) || ((IT) == I2C_IT_BTF) || \
                           ((IT) == I2C_IT_ADDR) || ((IT) == I2C_IT_SB))
/**
  * @}
  */

/** @defgroup I2C_flags_definition I2C 标志定义 
  * @{
  */

/** 
  * @brief  SR2 寄存器标志  
  */

#define I2C_FLAG_DUALF                  ((uint32_t)0x00800000)
#define I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)
#define I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)
#define I2C_FLAG_GENCALL                ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                    ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                    ((uint32_t)0x00010000)

/** 
  * @brief  SR1 寄存器标志  
  */

#define I2C_FLAG_SMBALERT               ((uint32_t)0x10008000)
#define I2C_FLAG_TIMEOUT                ((uint32_t)0x10004000)
#define I2C_FLAG_PECERR                 ((uint32_t)0x10001000)
#define I2C_FLAG_OVR                    ((uint32_t)0x10000800)
#define I2C_FLAG_AF                     ((uint32_t)0x10000400)
#define I2C_FLAG_ARLO                   ((uint32_t)0x10000200)
#define I2C_FLAG_BERR                   ((uint32_t)0x10000100)
#define I2C_FLAG_TXE                    ((uint32_t)0x10000080)
#define I2C_FLAG_RXNE                   ((uint32_t)0x10000040)
#define I2C_FLAG_STOPF                  ((uint32_t)0x10000010)
#define I2C_FLAG_ADD10                  ((uint32_t)0x10000008)
#define I2C_FLAG_BTF                    ((uint32_t)0x10000004)
#define I2C_FLAG_ADDR                   ((uint32_t)0x10000002)
#define I2C_FLAG_SB                     ((uint32_t)0x10000001)

#define IS_I2C_CLEAR_FLAG(FLAG) ((((FLAG) & (uint16_t)0x20FF) == 0x00) && ((FLAG) != (uint16_t)0x00))

#define IS_I2C_GET_FLAG(FLAG) (((FLAG) == I2C_FLAG_DUALF) || ((FLAG) == I2C_FLAG_SMBHOST) || \
                               ((FLAG) == I2C_FLAG_SMBDEFAULT) || ((FLAG) == I2C_FLAG_GENCALL) || \
                               ((FLAG) == I2C_FLAG_TRA) || ((FLAG) == I2C_FLAG_BUSY) || \
                               ((FLAG) == I2C_FLAG_MSL) || ((FLAG) == I2C_FLAG_SMBALERT) || \
                               ((FLAG) == I2C_FLAG_TIMEOUT) || ((FLAG) == I2C_FLAG_PECERR) || \
                               ((FLAG) == I2C_FLAG_OVR) || ((FLAG) == I2C_FLAG_AF) || \
                               ((FLAG) == I2C_FLAG_ARLO) || ((FLAG) == I2C_FLAG_BERR) || \
                               ((FLAG) == I2C_FLAG_TXE) || ((FLAG) == I2C_FLAG_RXNE) || \
                               ((FLAG) == I2C_FLAG_STOPF) || ((FLAG) == I2C_FLAG_ADD10) || \
                               ((FLAG) == I2C_FLAG_BTF) || ((FLAG) == I2C_FLAG_ADDR) || \
                               ((FLAG) == I2C_FLAG_SB))
/**
  * @}
  */

/** @defgroup I2C_Events I2C 事件 
  * @{
  */

/**
 ===============================================================================
               I2C 主模式事件（事件按通信顺序分组）
 ===============================================================================
 */

/** 
  * @brief  通信开始
  * 
  * 发送起始条件（I2C_GenerateSTART() 函数）后，主设备必须等待该事件。
  * 这表示起始条件已正确释放到 I2C 总线上（总线空闲，没有其他设备正在通信）。
  * 
  */
/* --EV5 */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY、MSL 和 SB 标志 */

/** 
  * @brief  地址应答
  * 
  * 检查 EV5（起始条件已正确释放到总线）后，主设备发送将要通信的从设备地址
  * （I2C_Send7bitAddress() 函数，同时也决定通信方向：主发送器或主接收器）。
  * 然后主设备必须等待从设备应答其地址。如果总线上发送了应答，
  * 将设置以下事件之一：
  * 
  *  1) 主接收器（7 位寻址）情况下：设置 I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 事件。
  *  
  *  2) 主发送器（7 位寻址）情况下：设置 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED 事件。
  *  
  *  3) 10 位寻址模式情况下，主设备（刚生成起始条件并检查 EV5 后）必须发送
  *  10 位寻址模式头（I2C_SendData() 函数）。然后主设备应等待 EV9，
  *  这表示 10 位寻址头已正确发送到总线上。随后主设备应使用
  *  I2C_Send7bitAddress() 函数发送 10 位地址的第二部分（LSB）。
  *  然后主设备应等待 EV6 事件。 
  *     
  */

/* --EV6 */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY、MSL、ADDR、TXE 和 TRA 标志 */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY、MSL 和 ADDR 标志 */
/* --EV9 */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY、MSL 和 ADD10 标志 */

/** 
  * @brief 通信事件
  * 
  * 如果通信已建立（已生成起始条件且从设备地址已应答），
  * 主设备必须针对通信流程检查以下事件之一：
  *  
  * 1) 主接收器模式：主设备必须等待 EV7 事件，然后读取从从设备接收的数据
  *    （I2C_ReceiveData() 函数）。
  * 
  * 2) 主发送器模式：主设备必须发送数据（I2C_SendData() 函数），
  *    然后等待 EV8 或 EV8_2 事件。
  *    这两个事件相似： 
  *     - EV8 表示数据已写入数据寄存器并正在移出。
  *     - EV8_2 表示数据已实际移出并输出到总线上。
  *     大多数情况下，应用程序使用 EV8 已足够。
  *     使用 EV8_2 会使通信变慢，但可保证更可靠的检测。
  *     EV8_2 也比 EV8 更适合检测最后一次数据传输
  *     （生成停止条件之前）。
  *     
  *  @note 如果用户软件不能保证在当前字节传输结束前处理 EV7 事件，
  *        则用户可以同时检查 EV7 和 BTF 标志
  *        （即 (I2C_EVENT_MASTER_BYTE_RECEIVED | I2C_FLAG_BTF)）。
  *        在这种情况下通信可能会变慢。
  * 
  */

/* 主接收器模式 -------------------------------------*/ 
/* --EV7 */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY、MSL 和 RXNE 标志 */

/* 主发送器模式 -------------------------------------*/
/* --EV8 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA、BUSY、MSL、TXE 标志 */
/* --EV8_2 */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA、BUSY、MSL、TXE 和 BTF 标志 */


/**
 ===============================================================================
               I2C 从模式事件（事件按通信顺序分组）
 ===============================================================================
 */


/** 
  * @brief  通信开始事件
  * 
  * 通信开始时等待以下事件之一。这表示 I2C 外设在总线上检测到
  * 起始条件（由主设备生成），随后检测到外设地址。
  * 外设在总线上生成 ACK 条件（如果已通过 I2C_AcknowledgeConfig()
  * 函数使能应答功能），并设置下列事件：
  *  
  * 1) 正常情况（从设备只管理一个地址）下，当主设备发送的地址
  *   与外设自身地址（由 I2C_OwnAddress1 字段配置）匹配时，
  *   设置 I2C_EVENT_SLAVE_XXX_ADDRESS_MATCHED 事件
  *   （其中 XXX 可以是 TRANSMITTER 或 RECEIVER）。
  *    
  * 2) 如果主设备发送的地址与外设第二地址匹配（由 I2C_OwnAddress2Config()
  *   函数配置，并由 I2C_DualAddressCmd() 函数使能），
  *   则设置 I2C_EVENT_SLAVE_XXX_SECONDADDRESS_MATCHED 事件
  *   （其中 XXX 可以是 TRANSMITTER 或 RECEIVER）。
  *   
  * 3) 如果主设备发送的地址为通用呼叫（地址 0x00），并且外设已使能通用呼叫
  *   （使用 I2C_GeneralCallCmd() 函数），则设置以下事件：
  *   I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED。   
  * 
  */

/* --EV1  （以下所有事件均为 EV1 的变体） */   
/* 1) 从设备管理单一地址的情况 */
#define  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED          ((uint32_t)0x00020002) /* BUSY 和 ADDR 标志 */
#define  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED       ((uint32_t)0x00060082) /* TRA、BUSY、TXE 和 ADDR 标志 */

/* 2) 从设备管理双地址的情况 */
#define  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    ((uint32_t)0x00820000)  /* DUALF 和 BUSY 标志 */
#define  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)  /* DUALF、TRA、BUSY 和 TXE 标志 */

/* 3) 从设备使能通用呼叫的情况 */
#define  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED        ((uint32_t)0x00120000)  /* GENCALL 和 BUSY 标志 */

/** 
  * @brief  通信事件
  * 
  * 当 EV1 已检查且满足以下情况时，等待这些事件之一： 
  * 
  * - 从接收器模式：
  *     - EV2：应用程序期望接收一个数据字节时。 
  *     - EV4：应用程序期望通信结束时：主设备发送停止条件，
  *       数据传输停止。
  *    
  * - 从发送器模式：
  *    - EV3：从设备已发送一个字节，并且应用程序期望字节传输结束时。
  *      I2C_EVENT_SLAVE_BYTE_TRANSMITTED 和 I2C_EVENT_SLAVE_BYTE_TRANSMITTING
  *      这两个事件相似。当用户软件不能保证在当前字节传输结束前处理 EV3 时，
  *      可选择使用第二个事件。
  *    - EV3_2：主设备发送 NACK，以通知从设备数据传输应结束
  *      （发送停止条件之前）时。在这种情况下，从设备必须停止发送数据字节，
  *      并等待总线上的停止条件。
  *      
  *  @note 如果用户软件不能保证在当前字节传输结束前处理 EV2 事件，
  *        则用户可以同时检查 EV2 和 BTF 标志
  *        （即 (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_BTF)）。
  *        在这种情况下通信可能会变慢。
  *
  */

/* 从接收器模式 -------------------------------*/ 
/* --EV2 */
#define  I2C_EVENT_SLAVE_BYTE_RECEIVED                     ((uint32_t)0x00020040)  /* BUSY 和 RXNE 标志 */
/* --EV4  */
#define  I2C_EVENT_SLAVE_STOP_DETECTED                     ((uint32_t)0x00000010)  /* STOPF 标志 */

/* 从发送器模式 -------------------------------*/
/* --EV3 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTED                  ((uint32_t)0x00060084)  /* TRA、BUSY、TXE 和 BTF 标志 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTING                 ((uint32_t)0x00060080)  /* TRA、BUSY 和 TXE 标志 */
/* --EV3_2 */
#define  I2C_EVENT_SLAVE_ACK_FAILURE                       ((uint32_t)0x00000400)  /* AF 标志 */

/*
 ===============================================================================
                          事件说明结束
 ===============================================================================
 */

#define IS_I2C_EVENT(EVENT) (((EVENT) == I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED) || \
                             ((EVENT) == I2C_EVENT_SLAVE_BYTE_RECEIVED) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF)) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL)) || \
                             ((EVENT) == I2C_EVENT_SLAVE_BYTE_TRANSMITTED) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF)) || \
                             ((EVENT) == (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL)) || \
                             ((EVENT) == I2C_EVENT_SLAVE_STOP_DETECTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_MODE_SELECT) || \
                             ((EVENT) == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_BYTE_RECEIVED) || \
                             ((EVENT) == I2C_EVENT_MASTER_BYTE_TRANSMITTED) || \
                             ((EVENT) == I2C_EVENT_MASTER_BYTE_TRANSMITTING) || \
                             ((EVENT) == I2C_EVENT_MASTER_MODE_ADDRESS10) || \
                             ((EVENT) == I2C_EVENT_SLAVE_ACK_FAILURE))
/**
  * @}
  */

/** @defgroup I2C_own_address1 I2C 自身地址 1 
  * @{
  */

#define IS_I2C_OWN_ADDRESS1(ADDRESS1) ((ADDRESS1) <= 0x3FF)
/**
  * @}
  */

/** @defgroup I2C_clock_speed I2C 时钟速度 
  * @{
  */

#define IS_I2C_CLOCK_SPEED(SPEED) (((SPEED) >= 0x1) && ((SPEED) <= 400000))
/**
  * @}
  */

/**
  * @}
  */

/* 导出的宏 -----------------------------------------------------------------*/
/* 导出的函数 ---------------------------------------------------------------*/ 

/*  用于将 I2C 配置恢复为默认复位状态的函数 *********************************/
void I2C_DeInit(I2C_TypeDef* I2Cx);

/* 初始化和配置函数 **********************************************************/
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_DigitalFilterConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DigitalFilter);
void I2C_AnalogFilterCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address);
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DutyCycle);
void I2C_NACKPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_NACKPosition);
void I2C_SMBusAlertConfig(I2C_TypeDef* I2Cx, uint16_t I2C_SMBusAlert);
void I2C_ARPCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 数据传输函数 **************************************************************/ 
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);

/* PEC 管理函数 **************************************************************/ 
void I2C_TransmitPEC(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_PECPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_PECPosition);
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState);
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx);

/* DMA 传输管理函数 **********************************************************/
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_DMALastTransferCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* 中断、事件和标志管理函数 **************************************************/
uint16_t I2C_ReadRegister(I2C_TypeDef* I2Cx, uint8_t I2C_Register);
void I2C_ITConfig(I2C_TypeDef* I2Cx, uint16_t I2C_IT, FunctionalState NewState);

/* 
 ===============================================================================
                          I2C 状态监控函数
 ===============================================================================
  本 I2C 驱动根据应用需求和约束，提供三种不同的 I2C 状态监控方式：
         
   
     1. 基本状态监控（使用 I2C_CheckEvent() 函数）
     -----------------------------------------------------------
        该方式将状态寄存器（SR1 和 SR2）的内容与给定事件进行比较
        （该事件可以由一个或多个标志组合而成）。
        如果当前状态包含给定标志，则返回 SUCCESS；
        如果当前状态缺少一个或多个标志，则返回 ERROR。

          - 使用场景
             - 该函数适用于大多数应用以及启动阶段操作，
               因为这些事件已在产品参考手册（RM0090）中完整描述。
             - 也适用于需要定义自有事件的用户。

          - 限制
             - 如果发生错误（即除被监控标志外还设置了错误标志），
               即使通信被挂起或真实状态已损坏，I2C_CheckEvent() 函数
               也可能返回 SUCCESS。
               在这种情况下，建议使用错误中断监控错误事件，
               并在中断 IRQ 处理函数中进行处理。
         
     注意 
         对于错误管理，建议使用以下函数：
           - I2C_ITConfig() 用于配置并使能错误中断（I2C_IT_ERR）。
           - I2Cx_ER_IRQHandler() 在发生错误中断时被调用，
             其中 x 为外设实例（I2C1、I2C2 ...）。
           - 在 I2Cx_ER_IRQHandler() 函数中调用 I2C_GetFlagStatus()
             或 I2C_GetITStatus()，用于确定发生了哪种错误。
           - 使用 I2C_ClearFlag() 或 I2C_ClearITPendingBit()，以及/或者
             I2C_SoftwareResetCmd()，以及/或者 I2C_GenerateStop()，
             用于清除错误标志和错误源，并恢复到正确的通信状态。
             
 
     2. 高级状态监控（使用 I2C_GetLastEvent() 函数）
     -------------------------------------------------------------------- 
        使用 I2C_GetLastEvent() 函数，该函数以单个字（uint32_t）
        返回两个状态寄存器的映像（状态寄存器 2 的值左移 16 位后
        与状态寄存器 1 拼接）。

          - 使用场景
             - 该函数适用于上述相同应用，但可克服前面提到的
               I2C_GetFlagStatus() 函数限制。
             - 返回值可与本文件中已定义的事件或用户定义的自定义值进行比较。
               当需要同时监控多个标志时，该函数较为适用。
             - 与 I2C_CheckEvent() 函数不同，本函数允许用户选择何时接受事件
               （所有事件标志均已设置且没有其他标志被设置，或者像
               I2C_CheckEvent() 函数一样只要求所需标志已设置）。

          - 限制
             - 用户可能需要定义自己的事件。
             - 如果用户决定只检查常规通信标志（并忽略错误标志），
               则关于错误管理的相同说明同样适用于本函数。
      
 
     3. 基于标志的状态监控（使用 I2C_GetFlagStatus() 函数）
     -----------------------------------------------------------------------
     
      使用 I2C_GetFlagStatus() 函数，该函数只返回单个标志的状态
      （例如 I2C_FLAG_RXNE ...）。 

          - 使用场景
             - 该函数可用于特定应用或调试阶段。
             - 当只需要检查一个标志时较为适用（大多数 I2C 事件
               需要通过多个标志进行监控）。
          - 限制： 
             - 调用该函数时会访问状态寄存器。
               访问状态寄存器时，一些标志会被清除。
               因此，检查一个标志的状态可能会清除其他标志。
             - 为了监控单个事件，可能需要调用该函数两次或更多次。           
 */

/*
 ===============================================================================
                          1. 基本状态监控
 ===============================================================================
 */
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
/*
 ===============================================================================
                          2. 高级状态监控
 ===============================================================================
 */
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx);
/*
 ===============================================================================
                          3. 基于标志的状态监控
 ===============================================================================
 */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);


void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);

#ifdef __cplusplus
}
#endif

#endif /*__STM32F4xx_I2C_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) STMicroelectronics 版权所有 *****文件结束****/
