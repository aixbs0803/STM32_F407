#include "task_user.h"

/*
 * task_user.c 是 Keil 工程中的任务层编译入口。
 *
 * 按你的要求，拆分后的源码放在 User/task 目录下，但 Keil 工程里不用逐个添加这些文件。
 * 这里直接用相对路径 #include "./task/xxx.c" 把多个实现文件合并进同一个编译单元。
 *
 * 注意：
 *   User/task 目录下的 .c 文件已经被本文件包含；
 *   不要再把它们单独添加到 Keil 工程，否则会出现函数和全局变量重复定义。
 */

#include "./task/data_logger_app.c"
#include "./task/data_logger_tasks.c"
#include "./task/data_logger_cli.c"
#include "./task/data_logger_storage.c"
