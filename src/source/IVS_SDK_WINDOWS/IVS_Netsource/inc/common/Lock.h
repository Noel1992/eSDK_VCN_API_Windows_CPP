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

#ifndef __IVS_PLAYER_LOCK_H__
#define __IVS_PLAYER_LOCK_H__


#ifdef _MSC_VER   // windows use 
# include <Windows.h>
typedef CRITICAL_SECTION     LOCK_T;
#  define LOCK_INIT(lock)      InitializeCriticalSection(&(lock))
#  define LOCK(lock)           EnterCriticalSection(&(lock))
#  define UNLOCK(lock);        LeaveCriticalSection(&(lock))
#  define LOCK_DESTROY(lock)   DeleteCriticalSection(&(lock))

#else  //linux

#include <pthread.h>
typedef pthread_mutex_t      LOCK_T;
#  define LOCK_INIT(lock)      pthread_mutex_init(&(lock), NULL)
#  define LOCK(lock)           pthread_mutex_lock(&(lock))
#  define UNLOCK(lock);        pthread_mutex_unlock(&(lock))
#  define LOCK_DESTROY(lock)   pthread_mutex_destroy(&(lock))
#endif // _MSC_VER


class CIVSMutex
{
public:
	CIVSMutex()  { LOCK_INIT(m_lock); }
	~CIVSMutex() { LOCK_DESTROY(m_lock);}
    void Lock()   { LOCK(m_lock); }
	void UnLock() { UNLOCK(m_lock); }
private:
	LOCK_T  m_lock;
};

class CAutoLock
{
public:
    CAutoLock(CIVSMutex& t) { m_pMutex = &t; m_pMutex->Lock(); }
	~CAutoLock() { m_pMutex->UnLock(); }
private:
    CIVSMutex *m_pMutex;
};





#endif // __IVS_PLAYER_LOCK_H__