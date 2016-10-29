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

// #include "StdAfx.h"
#include "CriticalSectionEx.h"

CCRITICAL_SECTIONEX::CCRITICAL_SECTIONEX()
{
	m_pMutex = VOS_CreateMutex();
	// ::InitializeCriticalSectionAndSpinCount(&m_CriticalSectionEx, 4000 );
}
CCRITICAL_SECTIONEX::~CCRITICAL_SECTIONEX()
{
	VOS_DestroyMutex(m_pMutex);
	// ::DeleteCriticalSection(&m_CriticalSectionEx);
}
void CCRITICAL_SECTIONEX::EnterCriticalSectionEx()
{
	(void)VOS_MutexLock(m_pMutex);
	// ::EnterCriticalSection(&m_CriticalSectionEx);
}
void CCRITICAL_SECTIONEX::LeaveCriticalSectionEx()
{
	(void)VOS_MutexUnlock(m_pMutex);
	// ::LeaveCriticalSection(&m_CriticalSectionEx);
}