#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "interrupt_init.h"
#include "usart.h"
#include "spi.h"
#include "diskio.h"
#include "ff.h"
#include "string.h"
#include "stdio.h"
#include "adc.h"
#include "tim.h"
#include "interrupt_Isr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_user.h"

extern uint32_t g_delay_tick;

#endif
