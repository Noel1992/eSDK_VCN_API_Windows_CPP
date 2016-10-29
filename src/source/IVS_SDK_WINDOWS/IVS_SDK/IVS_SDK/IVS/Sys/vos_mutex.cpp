/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef WIN32
#include "../../stdafx.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "vos.h"



VOS_Mutex *VOS_CreateMutex( VOID )
{
	ULONG ulResult = VOS_OK ;

	VOS_Mutex *pstMutex = VOS_NULL ;

	pstMutex = (VOS_Mutex *) VOS_malloc(sizeof(VOS_Mutex));//lint !e838
	if ( NULL == pstMutex  )
	{
		return VOS_NULL ;
	}
#if VOS_APP_OS == VOS_OS_LINUX
	ulResult = (ULONG)pthread_mutex_init( &pstMutex->mutex, 0);
	if( VOS_OK != ulResult )
	{
		VOS_free( pstMutex );        
		return VOS_NULL ;    
	}
#elif VOS_APP_OS == VOS_OS_WIN32
	pstMutex->mutex = CreateMutex(NULL,0,NULL);
	if (NULL == pstMutex->mutex)
	{
		VOS_free( pstMutex );        
		return VOS_NULL ;    
	}
	(void)ulResult; //过PCLINT
#endif
	return pstMutex ;
}


ULONG VOS_DestroyMutex( VOS_Mutex *pstMutex )
{  
	ULONG ulResult = VOS_OK;
	if (NULL == pstMutex)
	{
		return VOS_ERR;
	}
#if VOS_APP_OS == VOS_OS_LINUX
	pthread_mutex_destroy( &pstMutex->mutex );
#elif VOS_APP_OS == VOS_OS_WIN32
	(void)CloseHandle(pstMutex->mutex);/*lint !e613*/ //无此问题
#endif
	VOS_free( pstMutex );

	return ulResult ;
}


ULONG VOS_MutexLock( VOS_Mutex *pstMutex )
{
	ULONG ulResult = VOS_OK;

	if(NULL == pstMutex)
	{
		return VOS_ERR;
	}

#if VOS_APP_OS == VOS_OS_LINUX 
	ulResult = (ULONG)pthread_mutex_lock(&pstMutex->mutex);
	if( VOS_OK != ulResult )
	{
		return ulResult ;
	}
#elif VOS_APP_OS == VOS_OS_WIN32
	ulResult = WaitForSingleObject(pstMutex->mutex,INFINITE);//lint !e838
	if(WAIT_OBJECT_0 != ulResult)//lint !e835
	{
		return VOS_ERR;
	}
#endif
	return VOS_OK ;
}/*lint !e818*///使用公共平台源代码，是否Const不做要求

ULONG VOS_MutexUnlock( VOS_Mutex *pstMutex )
{    
	ULONG ulResult = VOS_OK;

	//BEGIN V100R001C05 ADD 2010-02-08 ligengqiang l00124479 for 视频流中断后客户端消失
	if(NULL == pstMutex)
	{
		return VOS_ERR;
	}
	//END   V100R001C05 ADD 2010-02-08 ligengqiang l00124479 for 视频流中断后客户端消失

#if VOS_APP_OS == VOS_OS_LINUX
	ulResult = (ULONG)pthread_mutex_unlock(&pstMutex->mutex);
	if( VOS_OK != ulResult )
	{
		return ulResult ;
	}
#elif VOS_APP_OS == VOS_OS_WIN32
	if((NULL == pstMutex)/*lint !e774*/
		|| (TRUE != ReleaseMutex(pstMutex->mutex)))
	{
		ulResult = VOS_ERR;
		return ulResult;
	}
#endif
	return VOS_OK ;
}/*lint !e818*/ //无此问题

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


