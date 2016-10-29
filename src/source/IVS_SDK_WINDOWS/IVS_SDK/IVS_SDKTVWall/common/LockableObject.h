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

/********************************************************************
    filename	: 	LockableObject.h
    author		:	wx58056
    created		:	2011/10/28
    description	:	定义可锁对象的相关类
    copyright	:	Copyright (C) 2011-2015
    history		:	2011/10/28 初始版本
 *********************************************************************/

#ifndef LOCKABLE_OBJECT_H
#define LOCKABLE_OBJECT_H

///////////////////////////////////////////////////////////////////////////////
/// @namespace CUMW
/// @brief 客户端中间件专有命名空间
///
/// CUMW之锁相关定义
///////////////////////////////////////////////////////////////////////////////
namespace CUMW
{
    ///////////////////////////////////////////////////////////////////////////
    /// @brief 可锁抽象基类
    ///
    /// 提供可锁类的通用接口
    ///////////////////////////////////////////////////////////////////////////
    class CLockableObject
    {
public:
        CLockableObject(){};
        virtual ~CLockableObject(){};

        virtual void Lock(){};

        virtual void Unlock(){};
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 锁类
    ///
    /// 自动解锁的锁?
    ///////////////////////////////////////////////////////////////////////////
    class CLock
    {
public:
        CLock(CLockableObject& obj) : m_objLockabelObject(obj)
        {
            m_objLockabelObject.Lock();
        };

        ~CLock()
        {
            m_objLockabelObject.Unlock();
        };

private:
        CLockableObject & m_objLockabelObject;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 临界区域类
    ///
    /// 封装临界区域的可锁类
    ///////////////////////////////////////////////////////////////////////////
    class CCriticalSection : public CLockableObject
    {
public:
        CCriticalSection()
        {
            InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 4000 );
        };

        virtual ~CCriticalSection()
        {
            DeleteCriticalSection(&m_CriticalSection);
        };

        virtual void Lock()
        {
            EnterCriticalSection(&m_CriticalSection);
        };

        virtual void Unlock()
        {
            LeaveCriticalSection(&m_CriticalSection);
        };

private:
        CRITICAL_SECTION m_CriticalSection;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// @brief 互斥体类
    ///
    /// 封装互斥体的可锁类
    ///////////////////////////////////////////////////////////////////////////
    class CMutex : public CLockableObject
    {
public:
        CMutex()
        {
            m_hMutex = CreateMutex(NULL, FALSE, NULL);
        };

        virtual ~CMutex()
        {
            if (NULL != m_hMutex)
            {
                CloseHandle(m_hMutex);
            }
        };

        virtual void Lock()
        {
            if (NULL != m_hMutex)
            {
                WaitForSingleObject(m_hMutex, INFINITE);
            }
        };

        virtual void Unlock()
        {
            if (NULL != m_hMutex)
            {
                (void)ReleaseMutex(m_hMutex);
            }
        };

private:
        HANDLE m_hMutex;
    };
}

#endif //LOCKABLE_OBJECT_H
