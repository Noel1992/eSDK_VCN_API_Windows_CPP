/*lint -w0 */

/******************************************************************************

                 Copyright (C), 2001-2014, Huawei Tech. Co., Ltd.

 ******************************************************************************
  FileName      : wsecsmplinux.c
  Version       : 1.0.0
  Author        : l00171031
  Date          : 2014-6-13
  Description   : Linux操作系统函数适配

  History   :
        1.Date            : 2014-6-13
          Author          : l00171031
          Modification    : 创建文件

******************************************************************************/
#ifdef WSEC_WIN32

#include <windows.h>
#include <winbase.h>
#include "wsec_type.h"
#include "wsec_pri.h"

#endif /* WSEC_WIN32 */


#ifdef WSEC_LINUX

#include <pthread.h>
#include <unistd.h> 
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include "wsec_type.h"
#include "wsec_pri.h"

#endif /* WSEC_LINUX */

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifdef WSEC_WIN32
/* Create Recursive Lock */
WSEC_BOOL WSEC_DeftCreateLock( WSEC_HANDLE *phMutex )
{
    LPCRITICAL_SECTION lpCriticalSection;

    lpCriticalSection = (LPCRITICAL_SECTION)WSEC_MALLOC(sizeof(CRITICAL_SECTION));
    if (!lpCriticalSection)
    {
        return WSEC_FALSE;
    }

    InitializeCriticalSection(lpCriticalSection);

    *phMutex = (WSEC_HANDLE)lpCriticalSection;

    return WSEC_TRUE;
}

/* Destroy Lock */
WSEC_VOID WSEC_DeftDestroyLock( WSEC_HANDLE hMutex )
{
    LPCRITICAL_SECTION lpCriticalSection;

    lpCriticalSection = (LPCRITICAL_SECTION)hMutex;

    DeleteCriticalSection(lpCriticalSection);

    /*lint -e506*/
    WSEC_FREE(lpCriticalSection);  
    return;
}

/* Mutex Lock */
WSEC_VOID WSEC_DeftLock( WSEC_HANDLE hMutex )
{
    EnterCriticalSection((LPCRITICAL_SECTION)hMutex);
    return;
}

/* Mutex Unlock */
WSEC_VOID WSEC_DeftUnlock( WSEC_HANDLE hMutex )
{
    LeaveCriticalSection((LPCRITICAL_SECTION)hMutex); 	
    return;
}

#endif /* WSEC_WIN32 */

#ifdef WSEC_LINUX
/* Create Recursive Lock */
WSEC_BOOL WSEC_DeftCreateLock( WSEC_HANDLE *phMutex )
{
    pthread_mutex_t tepmutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t * mutex;
    WSEC_BOOL bRet = WSEC_FALSE;
    
    mutex = (pthread_mutex_t *)WSEC_MALLOC(sizeof(pthread_mutex_t));
    if (!mutex) {return WSEC_FALSE;}

    do
    {
        if (WSEC_MEMCPY(mutex,sizeof(pthread_mutex_t),&tepmutex,sizeof(pthread_mutex_t)) != EOK) {break;}
        *phMutex = (WSEC_HANDLE )mutex;
        bRet = (pthread_mutex_init((pthread_mutex_t *)*phMutex,NULL) == 0);
    }do_end;

    if (!bRet) 
    {
        WSEC_FREE(mutex);
        *phMutex = WSEC_NULL_PTR;
    }

    return bRet;
}

/* Destroy Lock */
WSEC_VOID WSEC_DeftDestroyLock( WSEC_HANDLE hMutex )
{
    WSEC_UNCARE(pthread_mutex_destroy((pthread_mutex_t *)hMutex));
    WSEC_FREE(hMutex);
}

/* Mutex Lock */
WSEC_VOID WSEC_DeftLock( WSEC_HANDLE hMutex )
{
    WSEC_UNCARE(pthread_mutex_lock((pthread_mutex_t *)hMutex));
    return;
}

/* Mutex Unlock */
WSEC_VOID WSEC_DeftUnlock( WSEC_HANDLE hMutex )
{
    WSEC_UNCARE(pthread_mutex_unlock((pthread_mutex_t *)hMutex));
    return;
}

#endif /* WSEC_LINUX */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/*lint -restore */
