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

#ifndef _CTHREADHEADER_
#define _CTHREADHEADER_
#include "vos.h"
#include "hwsdk.h"
#include "ivs_error.h"

//lint -e601 -e10 -e40 -e1013 -e63
class CSThread 
{
public:
	CSThread ();
	virtual ~CSThread ();

	// 创建线程
	IVS_INT32 CreateThread();

	// 关闭线程
	void CloseThread();

	// 是否存在线程
	int	IsCreated()const { return (m_pVosThread != VOS_NULL);}

protected:

	// 执行事件
	virtual void Process ();	

	static void *invoke(void *argc)
	{
		CSThread *pThis = (CSThread *)argc;
		pThis->Process();
		VOS_pthread_exit(NULL);
		pThis->m_pVosThread = VOS_NULL;
		return NULL;
	};

public:
	
	 VOS_Thread *m_pVosThread;
	
};
 //lint +e601 +e10 +e40 +e1013 +e63

#endif // _CTHREADHEADER_
