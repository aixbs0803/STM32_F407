#ifndef __TASK_USER_H__
#define __TASK_USER_H__

/*
 * 对外只暴露任务层入口。
 * main.c 完成外设初始化后调用 LoggerApp_Start()，之后进入 FreeRTOS 调度。
 */
void LoggerApp_Start(void);

#endif
