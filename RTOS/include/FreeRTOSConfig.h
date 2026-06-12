/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. 或其关联公司。保留所有权利。
 *
 * 特此免费授予任何获得本软件及相关文档文件（"Software"）副本的人
 * 不受限制地处理 Software 的权限，包括但不限于使用、复制、修改、
 * 合并、发布、分发、再许可和/或销售 Software 副本的权利，并允许
 * 获得 Software 的人这样做，但须符合以下条件：
 *
 * 上述版权声明和本许可声明应包含在 Software 的所有副本或主要部分中。
 *
 * Software 按“原样”提供，不提供任何形式的明示或暗示担保，包括但不限于
 * 适销性、特定用途适用性和非侵权担保。在任何情况下，作者或版权持有人
 * 均不对因 Software 或 Software 的使用或其他交易而引起、源于或相关的
 * 任何索赔、损害或其他责任负责，无论该责任是基于合同、侵权或其他行为。
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * 应用程序相关定义。
 *
 * 这些定义应根据你的具体硬件和应用需求进行调整。
 *
 * 这些参数在 FreeRTOS.org 网站上的 FreeRTOS API 文档的
 * 'CONFIGURATION' 章节中有说明。
 *
 * 参见 http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* 确保 stdint 只被编译器使用，而不是被汇编器使用。 */
#ifdef __CC_ARM
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION			1		/* 任务抢占调度开关：1 使能抢占式调度，0 使用协作式调度 */
#define configUSE_IDLE_HOOK				0		/* 空闲任务钩子函数开关：1 使能 vApplicationIdleHook()，0 关闭 */
#define configUSE_TICK_HOOK				0		/* 系统节拍钩子函数开关：1 使能 vApplicationTickHook()，0 关闭 */
#define configCPU_CLOCK_HZ				( SystemCoreClock )
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES			( 5 )  	/* 最大任务优先级 */
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 130 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 75 * 1024 ) )
#define configMAX_TASK_NAME_LEN			( 10 )
#define configUSE_TRACE_FACILITY		1		/* 跟踪调试功能开关：1 使能任务状态查询和 trace 相关功能，0 关闭 */
#define configUSE_16_BIT_TICKS			0		/* TickType_t 位宽开关：1 使用 16 位 tick，0 使用 32 位 tick */
#define configIDLE_SHOULD_YIELD			1		/* 空闲任务让出 CPU 开关：1 空闲任务会让同优先级任务运行，0 不主动让出 */
#define configUSE_MUTEXES				1		/* 互斥量开关：1 使能 mutex，0 关闭 mutex API */
#define configQUEUE_REGISTRY_SIZE		8
#define configCHECK_FOR_STACK_OVERFLOW	0		/* 栈溢出检测开关：0 关闭，1/2 使能不同等级的栈溢出检查 */
#define configUSE_RECURSIVE_MUTEXES		1		/* 递归互斥量开关：1 使能 recursive mutex，0 关闭 */
#define configUSE_MALLOC_FAILED_HOOK	0		/* 内存申请失败钩子开关：1 使能 vApplicationMallocFailedHook()，0 关闭 */
#define configUSE_APPLICATION_TASK_TAG	0		/* 应用任务标签开关：1 使能任务 tag，0 关闭 */
#define configUSE_COUNTING_SEMAPHORES	1		/* 计数信号量开关：1 使能 counting semaphore，0 关闭 */
#define configGENERATE_RUN_TIME_STATS	0		/* 运行时间统计开关：1 使能任务运行时间统计，0 关闭 */

/* Co-routine 定义。 */
#define configUSE_CO_ROUTINES 		0			/* 协程功能开关：1 使能 co-routine，0 关闭 */
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* 软件定时器定义。 */
#define configUSE_TIMERS				1		/* 软件定时器开关：1 使能 software timer，0 关闭 */
#define configTIMER_TASK_PRIORITY		( 2 )
#define configTIMER_QUEUE_LENGTH		10
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )

/* 将以下定义设置为 1 表示包含对应 API function，设置为 0 表示排除对应 API function。 */
#define INCLUDE_vTaskPrioritySet		1		/* vTaskPrioritySet() API 开关：1 编译进内核，0 排除 */
#define INCLUDE_uxTaskPriorityGet		1		/* uxTaskPriorityGet() API 开关：1 编译进内核，0 排除 */
#define INCLUDE_vTaskDelete				1		/* vTaskDelete() API 开关：1 编译进内核，0 排除 */
#define INCLUDE_vTaskCleanUpResources	1		/* vTaskCleanUpResources() API 开关：1 编译进内核，0 排除 */
#define INCLUDE_vTaskSuspend			1		/* vTaskSuspend()/vTaskResume() API 开关：1 编译进内核，0 排除 */
#define INCLUDE_vTaskDelayUntil			1		/* vTaskDelayUntil() API 开关：1 编译进内核，0 排除 */
#define INCLUDE_vTaskDelay				1		/* vTaskDelay() API 开关：1 编译进内核，0 排除 */

/* Cortex-M 专用定义。 */
#ifdef __NVIC_PRIO_BITS
	/* 使用 CMSIS 时会指定 __NVIC_PRIO_BITS。 */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 个优先级等级 */
#endif

/* 调用“设置优先级”function 时可使用的最低中断优先级。 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* 调用中断安全 FreeRTOS API functions 的任何 interrupt service routine 可使用的
最高中断优先级。不要在优先级高于此值的任何中断中调用中断安全
FreeRTOS API functions！（优先级越高，数值越小。 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* kernel port layer 自身使用的中断优先级。这些定义通用于所有 Cortex-M ports，
不依赖任何特定的 library functions。 */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY 不能设置为 0 !!!!
参见 http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html。 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	
/* 普通 assert() 语义，不依赖 assert.h header file。 */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
/* 将 FreeRTOS port interrupt handlers 映射到其 CMSIS 标准名称的定义。 */
#define vPortSVCHandler SVC_Handler					// SVC 中断处理函数
#define xPortPendSVHandler PendSV_Handler		// 周期性服务中断处理函数
#define xPortSysTickHandler SysTick_Handler    // 系统节拍中断处理函数
#define configSUPPORT_STATIC_ALLOCATION    1 // 开启静态内存分配
#define configKERNEL_PROVIDED_STATIC_MEMORY    1 // 开启内核提供的静态内存分配
#define configUSE_STATS_FORMATTING_FUNCTIONS    1 // 开启任务列表功能

#endif /* FREERTOS_CONFIG_H */
