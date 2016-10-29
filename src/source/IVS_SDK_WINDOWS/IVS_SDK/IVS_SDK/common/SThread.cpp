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


// #include "stdafx.h"
#include "SThread.h"
//#include "NSS_Log_Manager.h"
//lint -e40 -e63 -e10 -e1401 -e1055 -e746 -e1013 -e1762
CSThread::CSThread ()
	:m_pVosThread(VOS_NULL)
{
}

CSThread::~CSThread ()
{
	if(m_pVosThread != VOS_NULL)
	{
		VOS_free(m_pVosThread);
	}

	m_pVosThread = VOS_NULL;
}

// 执行事件
void CSThread::Process ()
{
	return;
}

// 创建线程
IVS_INT32 CSThread::CreateThread()
{
	if (IsCreated())
	{
		// 该线程已存
		return IVS_SUCCEED;
	}

	if (VOS_OK != VOS_CreateThread((VOS_THREAD_FUNC)invoke, (void *)this, 
		&m_pVosThread, VOS_DEFAULT_STACK_SIZE))
	{
		return IVS_FAIL;
	};

	// 创建成功
	return IVS_SUCCEED;

}

void CSThread::CloseThread()
{
	//IVS_LOG(LOG_INFO, "Enter %s", __FUNCTION__);

	if (NULL == m_pVosThread)
	{
		//IVS_LOG(LOG_INFO, "Leave %s", __FUNCTION__);
		return;
	}

	if(VOS_pthread_self() != m_pVosThread->pthead)
	{
		//IVS_LOG(LOG_INFO, "wait %s", __FUNCTION__);
		VOS_ThreadJoin(m_pVosThread);
	}
	
	//释放内存
	VOS_free(m_pVosThread);
	m_pVosThread = VOS_NULL;

	//IVS_LOG(LOG_INFO, "Leave %s", __FUNCTION__);
}

//lint +e40 +e63 +e10 +e1401 +e1055 +e746 +e1013 +e1762
