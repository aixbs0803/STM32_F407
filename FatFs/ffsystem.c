/*------------------------------------------------------------------------*/
/* FatFs 用户提供的操作系统相关函数示例代码                               */
/*------------------------------------------------------------------------*/

#include "ff.h"


#if FF_USE_LFN == 3	/* 使用动态内存分配 */

/*------------------------------------------------------------------------*/
/* 分配/释放内存块                                                        */
/*------------------------------------------------------------------------*/

#include <stdlib.h>		/* 使用 POSIX API */


void* ff_memalloc (	/* 返回指向已分配内存块的指针 (内存不足时返回 null) */
	UINT msize		/* 要分配的字节数 */
)
{
	return malloc((size_t)msize);	/* 分配新的内存块 */
}


void ff_memfree (
	void* mblock	/* 指向要释放的内存块的指针 (为 null 时无效) */
)
{
	free(mblock);	/* 释放内存块 */
}

#endif




#if FF_FS_REENTRANT	/* 互斥锁 */
/*------------------------------------------------------------------------*/
/* 互斥量定义                                                             */
/*------------------------------------------------------------------------*/

#define OS_TYPE	0	/* 0:Win32, 1:uITRON4.0, 2:uC/OS-II, 3:FreeRTOS, 4:CMSIS-RTOS */


#if   OS_TYPE == 0	/* Win32 */
#include <windows.h>
static HANDLE Mutex[FF_VOLUMES + 1];	/* 互斥量句柄表 */

#elif OS_TYPE == 1	/* uITRON */
#include "itron.h"
#include "kernel.h"
static mtxid Mutex[FF_VOLUMES + 1];		/* 互斥量 ID 表 */

#elif OS_TYPE == 2	/* uc/OS-II */
#include "includes.h"
static OS_EVENT *Mutex[FF_VOLUMES + 1];	/* 互斥量指针表 */

#elif OS_TYPE == 3	/* FreeRTOS */
#include "FreeRTOS.h"
#include "semphr.h"
static SemaphoreHandle_t Mutex[FF_VOLUMES + 1];	/* 互斥量句柄表 */

#elif OS_TYPE == 4	/* CMSIS-RTOS */
#include "cmsis_os.h"
static osMutexId Mutex[FF_VOLUMES + 1];	/* 互斥量 ID 表 */

#endif



/*------------------------------------------------------------------------*/
/* 创建互斥量                                                              */
/*------------------------------------------------------------------------*/
/* 此函数在 f_mount 函数中调用，用于为卷创建新的互斥量或信号量。
/  当返回 0 时，f_mount 函数以 FR_INT_ERR 失败。
*/

int ff_mutex_create (	/* 返回 1:函数成功 或 0:无法创建互斥量 */
	int vol				/* 互斥量 ID: 卷互斥量 (0 到 FF_VOLUMES - 1) 或系统互斥量 (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	Mutex[vol] = CreateMutex(NULL, FALSE, NULL);
	return (int)(Mutex[vol] != INVALID_HANDLE_VALUE);

#elif OS_TYPE == 1	/* uITRON */
	T_CMTX cmtx = {TA_TPRI,1};

	Mutex[vol] = acre_mtx(&cmtx);
	return (int)(Mutex[vol] > 0);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	Mutex[vol] = OSMutexCreate(0, &err);
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	Mutex[vol] = xSemaphoreCreateMutex();
	return (int)(Mutex[vol] != NULL);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDef(cmsis_os_mutex);

	Mutex[vol] = osMutexCreate(osMutex(cmsis_os_mutex));
	return (int)(Mutex[vol] != NULL);

#endif
}


/*------------------------------------------------------------------------*/
/* 删除互斥量                                                              */
/*------------------------------------------------------------------------*/
/* 此函数在 f_mount 函数中调用，用于删除通过 ff_mutex_create 函数
/  创建的卷互斥量或信号量。
*/

void ff_mutex_delete (	/* 返回 1:函数成功 或 0:因错误无法删除 */
	int vol				/* 互斥量 ID: 卷互斥量 (0 到 FF_VOLUMES - 1) 或系统互斥量 (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	CloseHandle(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	del_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexDel(Mutex[vol], OS_DEL_ALWAYS, &err);

#elif OS_TYPE == 3	/* FreeRTOS */
	vSemaphoreDelete(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexDelete(Mutex[vol]);

#endif
}


/*------------------------------------------------------------------------*/
/* 请求访问卷的授权                                                        */
/*------------------------------------------------------------------------*/
/* 此函数在进入文件函数时调用，用于锁定卷。
/  当返回 0 时，文件函数以 FR_TIMEOUT 失败。
*/

int ff_mutex_take (	/* 返回 1:成功 或 0:超时 */
	int vol			/* 互斥量 ID: 卷互斥量 (0 到 FF_VOLUMES - 1) 或系统互斥量 (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	return (int)(WaitForSingleObject(Mutex[vol], FF_FS_TIMEOUT) == WAIT_OBJECT_0);

#elif OS_TYPE == 1	/* uITRON */
	return (int)(tloc_mtx(Mutex[vol], FF_FS_TIMEOUT) == E_OK);

#elif OS_TYPE == 2	/* uC/OS-II */
	OS_ERR err;

	OSMutexPend(Mutex[vol], FF_FS_TIMEOUT, &err));
	return (int)(err == OS_NO_ERR);

#elif OS_TYPE == 3	/* FreeRTOS */
	return (int)(xSemaphoreTake(Mutex[vol], FF_FS_TIMEOUT) == pdTRUE);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	return (int)(osMutexWait(Mutex[vol], FF_FS_TIMEOUT) == osOK);

#endif
}



/*------------------------------------------------------------------------*/
/* 释放访问卷的授权                                                        */
/*------------------------------------------------------------------------*/
/* 此函数在离开文件函数时调用，用于解锁卷。
*/

void ff_mutex_give (
	int vol			/* 互斥量 ID: 卷互斥量 (0 到 FF_VOLUMES - 1) 或系统互斥量 (FF_VOLUMES) */
)
{
#if OS_TYPE == 0	/* Win32 */
	ReleaseMutex(Mutex[vol]);

#elif OS_TYPE == 1	/* uITRON */
	unl_mtx(Mutex[vol]);

#elif OS_TYPE == 2	/* uC/OS-II */
	OSMutexPost(Mutex[vol]);

#elif OS_TYPE == 3	/* FreeRTOS */
	xSemaphoreGive(Mutex[vol]);

#elif OS_TYPE == 4	/* CMSIS-RTOS */
	osMutexRelease(Mutex[vol]);

#endif
}

#endif	/* FF_FS_REENTRANT */

